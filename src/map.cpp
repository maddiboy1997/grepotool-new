/* map.cpp - Map rendering

Copyright (c) 2013 P. Vorpahl

This file is part of Grepotool, a free utility for the browsergame 
Grepolis which is the intellectual property of InnoGames GmbH.

Grepotool is free Software: you can redistribute it and/or modify 
it under the terms of the GNU Lesser General Public License as 
published by the Free Software Foundation, either version 3 of the 
License, or any later version. Alternatively, you may use it under 
the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or any later 
version.

Grepotool is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with 'Grepotool; see the files 'GPL.txt' and 'LGPL.txt' in 
the base directory of your installation. If not, see 
'http://www.gnu.org/licenses/'.
*/

#include "map.h"
#include "app.h"
#include "defines.h"
#include "command.h"
#include "command_ids.h"
#include "world_data.h"

#include <QPainter>

static QPointF toContent (const QPoint & p, int zl, const QPoint &offset, const QSize &vp)
{
	double zoom = 1.0 / zoom_factors[zl];
	double xp = p.x() - vp.width()/2;
	xp /= zoom;
	xp += offset.x();		
	double yp = p.y() - vp.height()/2;
	yp /= zoom;
	yp += offset.y();		
	return QPointF (xp,yp);
};

static QPoint toScreen (QPointF p, int zl, const QPoint &offset, const QSize &vp) 
{
	double zoom = 1.0 / zoom_factors[zl];
	double xp = p.x() - offset.x();
	xp*=zoom;
	xp += vp.width()/2;
	double yp = p.y() - offset.y();
	yp*=zoom;
	yp += vp.height()/2;
	return QPoint (xp,yp);
};


MapThread::MapThread (QObject * p): QThread (p),_restart(false),_abort(false)
{
}

MapThread::~MapThread ()
{
	_m.lock();
    _abort = true;
    _condition.wakeOne();
    _m.unlock();
    wait();
}

void MapThread::aquireImage (int zoom_level, const QPoint &offset, const QSize &viewport)
{
	QMutexLocker ml(&_m);

	WorldData d = Cmd::worldData();
	_zoom_level = zoom_level;
	_offset = offset;
	_viewport = QSize(viewport.width()*3,viewport.height()*3);
	_pol_reach = 128 * 10;
	_pol_opa = 200;

	int r = Setting::value (MAPVIEW_INFMAP_CITY_REACH).toInt()-1;
	int o = Setting::value (MAPVIEW_INFMAP_OPACITY).toInt();
	int ops [10] = {100,90,80,70,60,50,40,30,20,10}; 
	if(r>0)
		_pol_reach = 128 * r;
	if(o>=0 && o<10) _pol_opa = 255 * ops[o] / 100;
	

	if (!isRunning()) {
		start (LowPriority);
	} else {
		_restart = true;
		_condition.wakeOne();
	}
}

void MapThread::meshCreated (const QVector<QPolygonF> &pol, const QVector<QPoint>& pos, const QVector<QColor>& col)
{
	QMutexLocker ml(&_m);

	_pol = pol;
	_pol_col = col;
	_pol_pos = pos;

	if (!isRunning()) {
		start ();// LowPriority);
	} else {
		_restart = true;
		_condition.wakeOne();
	}
}

void MapThread::meshColorsChanged (const QVector<QColor>& col)
{
	QMutexLocker ml(&_m);

	_pol_col = col;

	if (!isRunning()) {
		start ();// LowPriority);
	} else {
		_restart = true;
		_condition.wakeOne();
	}
}

void MapThread::clearMesh()
{
	QMutexLocker ml(&_m);
	
	_pol.clear();
	_pol_col.clear();
	_pol_pos.clear();

	if (!isRunning()) {
		start ();// LowPriority);
	} else {
		_restart = true;
		_condition.wakeOne();
	}
}


void MapThread::run()
{
	QVector <QImage> wi (61); 
	for(int i=0;i< 61;++i) {
		wi[i] = App::globalData().island(i);
	}
	forever {
		if (_abort) return;

		// copy data
		_m.lock();
		QVector <QPolygonF> pol = _pol;
		QVector <QColor>    pol_col = _pol_col;
		QVector <QPoint>    pol_pos = _pol_pos;
		int                 pol_reach = _pol_reach;
		int                 pol_opa = _pol_opa;

		int                 zoom_level = _zoom_level;
		QPoint              offset = _offset;
		QSize               viewport = _viewport;
		_m.unlock ();



		QPointF minp = toContent(QPoint(0,0),zoom_level,offset,viewport);
		QPointF maxp = toContent(QPoint(viewport.width(),viewport.height()),zoom_level,offset,viewport);
		QRectF cr (minp,maxp);
		double zoom = 1.0 / zoom_factors[zoom_level];

		QImage img (viewport, QImage::Format_ARGB32_Premultiplied);

		img.fill(0);

		QPainter p(&img);
	
		/*
		p.setBrush(QColor (0,0,0,0));
		p.drawRect(QRect(0,0,viewport.width(),viewport.height()));
		*/
		p.setPen (Qt::NoPen);
		QPolygonF np;
		for (int i=0;i< pol.size(); ++i) {
			if(_abort) return;
			if(_restart) break;
				if (!cr.intersects (pol[i].boundingRect())) continue;
			np.resize (pol[i].size());
			for (int j=0; j<pol[i].size(); ++j ) {
				np[j] = toScreen (pol[i][j],zoom_level,offset,viewport);
			}
				QPointF cp = toScreen (pol_pos[i],zoom_level,offset,viewport);
			double pr = pol_reach*zoom;
			QColor c = pol_col[i];
			c.setAlpha(pol_opa);

			QRadialGradient g(cp, pr, cp);
			g.setColorAt(0.0, c);
			g.setColorAt(1.0, QColor(0,0,0,0));
			p.setBrush (QBrush(g));
			p.drawPolygon(np);
		}

		_m.lock();
		if (!_restart) 
			emit imageRendered (img, QRectF(minp,maxp));

		if (!_restart)
			// wait for next wakeup
			_condition.wait(&_m);
		if (_abort) {
			_m.unlock(); 
			return;
		}
		_restart = false;
		_m.unlock(); 
	}

}




