#ifndef __MAPVIEW_H
#define __MAPVIEW_H
/* mapview.h  - Map viewer

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


#include "global_data.h"
#include "world_data.h"
#include "toolwidget.h"

#include <QFrame>
#include <QByteArray>


class MeshThread;
class MapThread;


class MapView : public QFrame {

	Q_OBJECT;
public:
	MapView (QWidget * par);
	~MapView ();

	QPointF toContent (QPoint p) const {
		double xp = p.x() - width()/2;
		xp /= zoom;
		xp += offset.x();		
		double yp = p.y() - height()/2;
		yp /= zoom;
		yp += offset.y();		
		return QPointF (xp,yp);
	};

	QPoint toScreen (QPointF p) const {
		double xp = p.x() - offset.x();
		xp*=zoom;
		xp += width()/2;
		double yp = p.y() - offset.y();
		yp*=zoom;
		yp += height()/2;
		return QPoint (xp,yp);
	};

	int townAt (const QPoint &p) const; 
	void worldChanged ();

	QPoint currentOffset () const {return offset;};
	int currentZoomLevel () const {return zoom_level;};
	
signals:
	void zoomChangedSignal (double);
	void mousePosChangedSignal (QPoint);
	void townDblClickSignal (int);
	void contextMenuSignal (const QPoint&);

protected:
	void wheelEvent (QWheelEvent* e); 
	void mousePressEvent (QMouseEvent* e) ;
	void mouseReleaseEvent (QMouseEvent* e) ;
	void mouseMoveEvent (QMouseEvent* e) ;
	bool event ( QEvent * ev );
	void paintEvent (QPaintEvent* e); 
	void mouseDoubleClickEvent ( QMouseEvent *e) ;


public slots:
	void updateInfMap ();
	void updateMap ();
	void showToolTip (const QPoint &p); 
	void zoomTo (const QPoint &p, int zoom_level);
	void scrollTo (const QPoint &p);

	void setZoomLevel (int);

	void mapRendered (const QImage& img, const QRectF& r );

private:


	void renderBuffer (QPainter* p);
	void renderGrid (QPainter* p, QPointF minp, QPointF maxp);
	void renderAllyLabels (QPainter * p, QPointF minp, QPointF maxp);
	QPoint bestAllyLabelPos (int a, const QRectF& r, int & cnt);

	QPoint lastmouse;
	
	QPoint offset;
	double zoom;
	int zoom_level;

	QMutex mutex;
	QPixmap ocean; 
	
	QImage buff;
	QRectF buff_rect;

	MeshThread * mesher;
	MapThread * mapper;

};


class MapWindow : public ToolWidget {
	Q_OBJECT;
public:
	MapWindow (): ToolWidget("MapView", tr("Map viewer")) {
		_view = new MapView (this);
		setCentralWidget (_view);
		_view->setContextMenuPolicy (Qt::CustomContextMenu);
		connect (_view, SIGNAL (customContextMenuRequested(const QPoint&)) , this, SLOT(contextMenuRequest (const QPoint&)));
		connect (_view, SIGNAL(townDblClickSignal (int)),this,SLOT(itemDoubleClick (int)));

	};
	~MapWindow (){};
	MapView *_view;

	void prepareWorldChange () {};
	void worldChanged () {_view->worldChanged ();};
	void initWidget (){};
	void createSettingsSubMenu (QMenu*, bool sm);

public slots:
	void contextMenuRequest (const QPoint& p);
	void fillItemContextMenu (QMenu*m, int);
	void fillContextMenu (QMenu*);
	void itemDoubleClick (int i) {
		emit itemDoubleClickSignal (this ,i );
	};

};




#endif