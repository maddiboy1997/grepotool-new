/* infmap.cpp - political map layer

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

#include "infmap.h"
#include "setting.h"
#include "tri_mesh.h"

#define INF_MAP_WIDTH (1000*GRID_DIV)
#define TOWN_INF_WIDTH (2 * GRID_DIV * CITY_REACH + 1)
#define INF_MAP_SIZE INF_MAP_WIDTH*INF_MAP_WIDTH
#define TOWN_INF_SIZE TOWN_INF_WIDTH*TOWN_INF_WIDTH


static int __ally [INF_MAP_SIZE];
static int __base [INF_MAP_SIZE];
static int __count[INF_MAP_SIZE];
static int __tinf[TOWN_INF_SIZE];

InfMapRenderer::InfMapRenderer (QObject * p): QThread (p),_restart(false),_abort(false){};


InfMapRenderer::~InfMapRenderer()
 {
     m.lock();
     _abort = true;
     _condition.wakeOne();
     m.unlock();
     wait();
 }


void InfMapRenderer::render (const WorldData &d)
{
	QMutexLocker ml(&m);
	_oc = Setting::value(MAPVIEW_OCEAN_COLOR).value<QColor>();
	_opa = 200; // set.map_opacity;

	int na = d.allys().size();
	_cols.resize(na);
	_show.resize(na);
	_towns.resize(na);
	for (int i=0;i<na;++i) {
		_cols[i] = d.allys()[i].color;
		_show[i] = d.allys()[i].show;
		_towns[i] = d.townPositionsOfAlly(i);
	}
	if (!isRunning()) {
		start (LowPriority);
	} else {
		_restart = true;
		_condition.wakeOne();
	}
};

void InfMapRenderer::run()
{

	int reach = GRID_DIV * CITY_REACH;
	int r2 = reach*reach;

	int cell_si = 128 / GRID_DIV;

	memset (__tinf, 0, sizeof(int) * TOWN_INF_SIZE);

	double pi = 4.0*atan(1.0);

	for (int x = 0; x <= reach; ++x) {
		int x2 = x*x;
		for (int y = 0; y <= reach; ++y)  {
			int d2 = x2 + y*y;
			int idx = reach + 1 + x +  TOWN_INF_WIDTH * (y + reach + 1);	
			if (d2 <= r2) {
				if (d2==0) {
					__tinf[idx]=10000;
				} else {
					double d = sqrt((double)d2);
					d = cos (d* pi/(double) reach) * 10000.;
					// d = reach-d;
					__tinf[idx]=d;
					idx = reach+1 - x +  TOWN_INF_WIDTH * (y+reach+1);	
					__tinf[idx]=d;
					idx = reach+1 - x +  TOWN_INF_WIDTH * (reach+1-y);	
					__tinf[idx]=d;
					idx = reach+1 + x +  TOWN_INF_WIDTH * (reach+1-y);	
					__tinf[idx]=d;
				}
			}
		}
	}




	forever {
		if (_abort) return;

		m.lock();
		QVector<QColor> cols (_cols);
		QVector<bool> show ( _show);
		// Settings     set (_set);

		QColor oc (_oc);
		int   opa = (_opa);

		QVector<QVector <QPoint> > towns (_towns);
		m.unlock ();

		int cnt = 0;
		int tot = show.size();
		int numa = tot;
		int perc = 0;

		// render inf_map

		memset (__base, 0, sizeof(int)*INF_MAP_SIZE);


		for (int a = 0; a < numa; ++a) {

			int npr = (cnt*100) / tot;
			if (npr > perc) {
				perc = npr;
				emit progressSignal (npr);
			}
			++cnt;

			if (_abort) return;
			if (_restart) break;

			if (!show[a])
				continue;

			// place the towns on the count map
			memset (__count, 0, sizeof(int) * INF_MAP_SIZE);
			int nt = towns[a].size();
			for (int t = 0; t < nt; ++t) {
				if (_abort) return;
				if (_restart) break;
	
				QPoint p = towns[a][t];
				int px = p.x() / cell_si - reach -1;
				int py = p.y() / cell_si - reach -1;
				for (int x = 0; x < TOWN_INF_WIDTH ;++x) {
					if (_abort) return;
					if (_restart) break;
					int mx = px+x;
					if(mx<0 || mx >= INF_MAP_WIDTH) continue;
					for (int y = 0; y < TOWN_INF_WIDTH ;++y) {
						int my = py+y;
						if(my < 0 || my >= INF_MAP_WIDTH) continue;
						int mindex = mx + my * INF_MAP_WIDTH;
						int pindex = x + y * TOWN_INF_WIDTH;
						if (__count[mindex] < __tinf[pindex]) 
							__count[mindex] = __tinf[pindex];
					}
				}
			}

			// merge count into base map
			for (int i=0;i<INF_MAP_SIZE;++i) {
				if (__count[i]>__base[i]){
					__base[i]=__count[i];
					__ally[i]=a;
				}
			}
		}
		emit progressSignal (99);
		if (_abort) return;
		// if (_restart) break;

		QImage r (INF_MAP_WIDTH,INF_MAP_WIDTH,QImage::Format_RGB32);

		for (int x=0; x<INF_MAP_WIDTH; ++x) {
			if (_abort) return;
			if (_restart) break;

			for (int y=0; y<INF_MAP_WIDTH; ++y) {
				QColor c = oc;
				int idx = x + y * INF_MAP_WIDTH;
				int w = __base[idx];
				if ( w ) {
					c = _cols[__ally[idx]];
					int alp = w * opa / 10000;
					int oa = 255 - alp;
					int red = (c.red()*alp + oc.red()*oa) / (255);				
					int green = (c.green()*alp+oc.green()*oa) / (255);					
					int blue  = (c.blue()*alp +oc.blue()*oa) / (255);				
					c = QColor (red,green,blue); 
				}
				r.setPixel(x,y,c.rgb());
			}
		}

		emit progressSignal (100);
		// finally go to sleep
		m.lock();
		if (!_restart) 
			emit renderedImage (r);

		if (!_restart) // wait for next wakeup
			_condition.wait(&m);
		if (_abort) {
			m.unlock(); 
			return;
		}

		_restart = false;
		m.unlock(); 
	}
}
















