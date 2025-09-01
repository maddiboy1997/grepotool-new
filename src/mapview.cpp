/* mapview.cpp - map viewer

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

#include "mainwin.h"
#include "mapview.h"
#include "explorer.h"
#include "app.h"
#include "command.h"
#include "setting.h"
#include "command_ids.h"
#include "infmap.h"
#include "tri_mesh.h"
#include "map.h"


#include <QToolTip>

#define TOWN_BASE_SIZE 15


void MapWindow::contextMenuRequest (const QPoint& p)
{
	int to = _view->townAt (p);
	if(to < 0) {
		emit contextMenuSignal (this, _view->mapToGlobal (p));
	} else {
		emit itemContextMenuSignal (this, _view->mapToGlobal (p), to);
	}
}

void MapWindow::fillItemContextMenu (QMenu*m, int) {
	fillContextMenu (m);
}

void MapWindow::fillContextMenu (QMenu* m)
{
	createSettingsSubMenu (m,false);
}



void renderText (QPainter* p, const QRect & r, int flags, const QString & text, const QColor& bg, const QColor& fg)
{
	QRect r0 (r.topLeft()-QPoint(1,1),r.bottomRight()-QPoint(1,1));
	QRect r1 (r.topLeft()+QPoint(1,1),r.bottomRight()+QPoint(1,1));
	p->setPen(bg);
	p->drawText(r0,flags,text);
	p->drawText(r1,flags,text);
	p->setPen(fg);
	p->drawText(r,flags,text);
}

void renderText (QPainter* p, const QPoint& pos, const QString & text, const QColor& bg, const QColor& fg)
{
	QPoint p0 = pos-QPoint(1,1);
	QPoint p1 = pos+QPoint(1,1);
	p->setPen(bg);
	p->drawText(p0,text);
	p->drawText(p1,text);
	p->setPen(fg);
	p->drawText(pos,text);
}






MapView::MapView (QWidget * par) : QFrame (par), ocean(":/icons/ocean.png")
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	new Setting( this, MAPVIEW_GLOBAL_OFFSET, QPoint (500*128,500*128));
	CmdGroup * gz = new CmdGroup (this, MAPVIEW_GLOBAL_ZOOM, 0,
		tr("Map zoom"),	tr("Map zoom tt"), tr("Map zoom wt"),
		QIcon (":/icons/world_zoom.ico"));		
	CmdGroup * gtz = new CmdGroup (this, MAPVIEW_GOTO_ZOOM, 5,
		tr("Goto zoom"),tr("Goto zoom tt"), tr("Goto zoom wt"),
		QIcon (":/icons/world_zoom.ico"));		
	for (int i=0; i < NZOOM; ++i) 	{
		if(zoom_factors[i] < 6.0) gtz->addAction (tr("1:%1").arg(zoom_factors[i]));
		gz->addAction (tr("1:%1").arg(zoom_factors[i]));
	}
	CmdGroup * iz = new CmdGroup (this, MAPVIEW_INFMAP_ZOOM, 1,
		tr("Show political map"),	tr("Show political map tt"), tr("Show political map wt"),
		QIcon (":/icons/world_zoom.ico"));		
	CmdGroup * lz = new CmdGroup (this, MAPVIEW_INFMAP_LABEL_ZOOM, 1,
		tr("Show ally labels"),	tr("Show ally labels tt"), tr("Show ally labels wt"),
		QIcon (":/icons/world_zoom.ico"));		
	CmdGroup * rz = new CmdGroup (this, MAPVIEW_ROCK_ZOOM, 8,
		tr("Show rocks"),	tr("Show rocks tt"), tr("Show rocks wt"),
		QIcon (":/icons/world_zoom.ico"));		
	rz->addAction (tr("Never"));
	iz->addAction (tr("Never"));
	lz->addAction (tr("Never"));
	for (int i=0; i < NZOOM; ++i) {	
		lz->addAction (tr("<= 1:%1").arg(zoom_factors[i]));
		iz->addAction (tr("<= 1:%1").arg(zoom_factors[i]));
		rz->addAction (tr(">= 1:%1").arg(zoom_factors[i]));
	}
	new CmdCheck (this, MAPVIEW_MOUSE_WHEEL_INVERT, false, 
		tr("Invert mouse wheel"),tr("Invert mouse wheel tt"),tr("Invert mouse wheel wt"),
		QIcon (":/icons/zoom_mouse.ico"));

	new CmdCheck (this, MAPVIEW_SHOW_OCEAN_LABELS, true, 
		tr("Show ocean labels"),tr("Show ocean labels tt"), tr("Show ocean labels wt"));

	new CmdCheck (this, MAPVIEW_SHOW_GRID, true, 
		tr("Show grid"),tr("Show grid tt"), tr("Show grid wt"));

	new CmdCheck (this, MAPVIEW_SHOW_GHOSTS, true, 
		tr("Show ghosts"),tr("Show ghosts tt"), tr("Show ghosts wt"));

	CmdGroup * ilm = new CmdGroup (this, MAPVIEW_INFMAP_LABEL_MODE, 1,
		tr("Label positioning"),	tr("Label positioning tt"), tr("Label positioning wt"),
		QIcon (":/icons/world_zoom.ico"));		
	ilm->addAction (tr("Center of gravity of visible towns"));
	ilm->addAction (tr("Experimental"));

	CmdGroup * ilop = new CmdGroup (this, MAPVIEW_INFMAP_OPACITY,3,
		tr("Map opacity"),	tr("Map opacity tt"), tr("Map opacity wt"),
		QIcon (":/icons/map_opacity.ico"));		
	ilop->addAction("100%","","",QIcon());
	ilop->addAction("90%","","",QIcon());
	ilop->addAction("80%","","",QIcon());
	ilop->addAction("70%","","",QIcon());
	ilop->addAction("60%","","",QIcon());
	ilop->addAction("50%","","",QIcon());
	ilop->addAction("40%","","",QIcon());
	ilop->addAction("30%","","",QIcon());
	ilop->addAction("20%","","",QIcon());
	ilop->addAction("10%","","",QIcon());

	CmdGroup * icr = new CmdGroup (this, MAPVIEW_INFMAP_CITY_REACH, 9,
		tr("Map city reach"),	tr("Map city reach tt"), tr("Map city reach wt"),
		QIcon (":/icons/map_opacity.ico"));		
	for (int i=1;i<=40;++i)
		icr->addAction (tr("%1 map units").arg(i),"","",QIcon());

	CmdColor *cc;
	cc = new CmdColor (this, MAPVIEW_OCEAN_COLOR, QColor(0,85,127),tr("Ocean color"),tr("Ocean color tt"),tr("Ocean color wt")); connect(cc,SIGNAL(valueChangedSignal()),this ,SLOT(updateInfMap()));
	cc = new CmdColor (this, MAPVIEW_GRID_COLOR, Qt::white, tr("Grid color"),tr(" color tt"),tr(" color wt")); connect(cc,SIGNAL(valueChangedSignal()),this ,SLOT(updateMap()));
	cc = new CmdColor (this, MAPVIEW_OCEAN_LABEL_COLOR, Qt::white, tr("Ocean label color"),tr("Ocean label color tt"),tr("Ocean label color wt")); connect(cc,SIGNAL(valueChangedSignal()),this ,SLOT(updateMap()));
	cc = new CmdColor (this, MAPVIEW_GHOST_COLOR, Qt::gray, tr("Ghost town color"),tr("Ghost town color tt"),tr("Ghost town color wt")); connect(cc,SIGNAL(valueChangedSignal()),this ,SLOT(updateMap()));

	cc = new CmdColor (this, MAPVIEW_UNSELECTED_COLOR, Qt::white, tr("Default color of unselected players"),tr("Default color of unselected players tt"),tr("Default color of unselected players wt")); connect(cc,SIGNAL(valueChangedSignal()),this ,SLOT(updateMap()));

	CmdGroup * up = new CmdGroup (this, MAPVIEW_UNSELECTED_MODE, 0,
		tr("Display of unselected players"),tr("Display of unselected players tt"), tr("Display of unselected players wt"),
		QIcon ());		
	up->addAction (tr("Use default color"), tr("Display unselected towns either in their own color or in the color of the alliance"), tr("Use default color wt"));
	up->addAction (tr("Use same color"), tr("Display unselected towns all in the same color tt"), tr("Use same color wt"));
	up->addAction (tr("Dont show at all"), tr("Dont show at all tt"), tr("Dont show at all wt"));
	Setting::connect (MAPVIEW_UNSELECTED_MODE,this,SLOT(updateMap()));
	
	CmdGroup * db = new CmdGroup (this, MAPVIEW_TOWN_DBLCLICK, 0,
		tr("Doubleclick on town in map"),tr("Doubleclick on town in map tt"), tr("Doubleclick on town in map wt"),
		QIcon (":/icons/coord_goto2.ico"));		
	db->addAction (tr("Do nothing"), tr("Do nothing"), tr("Do nothing wt"));
	db->addAction (tr("Add to My towns"), tr("Add the town to the current to my towns list"), tr("Add to my towns wt"));
	db->addAction (tr("Add to Targets"), tr("Add the town to the current target list"), tr("Add to targets wt"));
	
	Setting::connect (MAPVIEW_SHOW_OCEAN_LABELS,this,SLOT(updateMap()));
	Setting::connect (MAPVIEW_SHOW_GRID,this,SLOT(updateMap()));
	Setting::connect (MAPVIEW_SHOW_GHOSTS,this,SLOT(updateMap()));
	Setting::connect (MAPVIEW_INFMAP_OPACITY,this,SLOT(updateMap()));
	Setting::connect (MAPVIEW_INFMAP_CITY_REACH,this,SLOT(updateMap()));

	//Setting::connect (MAPVIEW_GLOBAL_OFFSET,this,SLOT(updateMap()));
	//Setting::connect (MAPVIEW_GLOBAL_ZOOM,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_ROCK_ZOOM,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_INFMAP_ZOOM,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_INFMAP_LABEL_ZOOM,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_INFMAP_LABEL_MODE,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_GRID_COLOR,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_GHOST_COLOR,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_OCEAN_COLOR,this,SLOT(updateInfMap()));	
	Setting::connect (MAPVIEW_OCEAN_LABEL_COLOR,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_UNSELECTED_COLOR,this,SLOT(updateMap()));	
	Setting::connect (MAPVIEW_UNSELECTED_MODE,this,SLOT(updateMap()));	

	connect(gz, SIGNAL(selectionChangedSignal(int)), this, SLOT(setZoomLevel(int)));

	offset = Setting::value (MAPVIEW_GLOBAL_OFFSET).value<QPoint>();
	zoom_level = Setting::value (MAPVIEW_GLOBAL_ZOOM).toInt();
	zoom = 1.0/zoom_factors[zoom_level];

	setMouseTracking (true);
	

	mapper = new MapThread (this);
	mesher = new MeshThread (this);
	connect (mesher,SIGNAL ( meshCreated (const QVector<QPolygonF> &, const QVector<QPoint>&, const QVector<QColor>&)),
		mapper,SLOT(meshCreated (const QVector<QPolygonF> &, const QVector<QPoint>&, const QVector<QColor>&)));

	connect (mesher,SIGNAL ( meshColorsChanged (const QVector<QColor>&)),
		mapper,SLOT(meshColorsChanged (const QVector<QColor>&)));

	connect (mapper,SIGNAL(imageRendered (const QImage&, const QRectF&)), this, SLOT(mapRendered (const QImage&,const QRectF&)));

	connect (mesher, SIGNAL(progress(int)) ,MainWin::instance , SLOT(meshProgress(int)));;
	setContextMenuPolicy ( Qt::CustomContextMenu );
	
}

void MapWindow::createSettingsSubMenu (QMenu*m, bool sm)
{
	QMenu *mapsett = m;
	if(sm) mapsett = m->addMenu ("Map viewer settings");
	// QMenu * mapsett = sett->addMenu (tr("Map"));

	Cmd::get(MAPVIEW_OCEAN_COLOR)->addToMenu(mapsett);

	mapsett->addSeparator();
	Cmd::get(MAPVIEW_SHOW_GRID)->addToMenu(mapsett);
	Cmd::get(MAPVIEW_SHOW_OCEAN_LABELS)->addToMenu(mapsett);
	Cmd::get(MAPVIEW_GRID_COLOR)->addToMenu(mapsett);
	Cmd::get(MAPVIEW_OCEAN_LABEL_COLOR)->addToMenu(mapsett);
	
	mapsett->addSeparator();
	Cmd::get(MAPVIEW_SHOW_GHOSTS)->addToMenu(mapsett);
	Cmd::get(MAPVIEW_GHOST_COLOR)->addToMenu(mapsett);

	mapsett->addSeparator();
	
	Cmd::get(MAPVIEW_ROCK_ZOOM)->addToMenu(mapsett);
	Cmd::get(MAPVIEW_GOTO_ZOOM)->addToMenu(mapsett);
	mapsett->addSeparator();

	QMenu * pma = mapsett->addMenu (tr("Political map"));
	Cmd::get(MAPVIEW_INFMAP_ZOOM)->addToMenu(pma);
	pma->addSeparator();
	Cmd::get(MAPVIEW_INFMAP_LABEL_ZOOM)->addToMenu(pma);
	Cmd::get(MAPVIEW_INFMAP_LABEL_MODE)->addToMenu(pma);
	pma->addSeparator();
	Cmd::get(MAPVIEW_INFMAP_CITY_REACH)->addToMenu(pma);
	Cmd::get(MAPVIEW_INFMAP_OPACITY)->addToMenu(pma);
	mapsett->addSeparator();

	QMenu * muns = mapsett->addMenu (tr("Unselected players"));
	Cmd::get(MAPVIEW_UNSELECTED_MODE)->addToMenu(muns);
	muns->addSeparator();
	Cmd::get(MAPVIEW_UNSELECTED_COLOR)->addToMenu(muns);

	QMenu * interactset = mapsett->addMenu (tr("Mouse interactions"));
	Cmd::get(MAPVIEW_MOUSE_WHEEL_INVERT)->addToMenu(interactset);
	Cmd::get(MAPVIEW_TOWN_DBLCLICK)->addToMenu(interactset);

}




MapView::~MapView ()
{
};


void MapView::updateInfMap ()
{
	mapper->aquireImage (zoom_level, offset, size());
	mesher->createMesh ();
	update();
}

void MapView::updateMap ()
{
	mapper->aquireImage (zoom_level, offset, size());
	repaint ();
}

void MapView::mapRendered (const QImage& img, const QRectF & r)
{
	buff = img;
	buff_rect = r;
	update ();
}


void MapView::worldChanged ()
{
	mapper->clearMesh();
	updateInfMap ();
	zoomTo (Setting::value(MAPVIEW_GLOBAL_OFFSET).value<QPoint>(),Setting::value(MAPVIEW_GLOBAL_ZOOM).toInt());
}

void MapView::mousePressEvent (QMouseEvent* e) 
{
	lastmouse = e->pos();
}


void MapView::zoomTo (const QPoint &p, int zoom_l)
{
	zoom = 1.0 / zoom_factors[zoom_l];
	zoom_level = zoom_l;
	offset = p;

	updateMap();
}


void MapView::setZoomLevel (int zl)
{
	zoom = 1.0 / zoom_factors[zl];
	zoom_level = zl;
	// Setting::setValue (MAPVIEW_GLOBAL_ZOOM, zl);
	updateMap();
}


void MapView::scrollTo (const QPoint &p)
{
	offset = p;

	// Setting::setValue (MAPVIEW_GLOBAL_OFFSET, p);

	updateMap();
	emit zoomChangedSignal (zoom);
}


void MapView::wheelEvent (QWheelEvent* e) 
{
	int dzl = e->delta() / 120.0;

	if (Setting::value(MAPVIEW_MOUSE_WHEEL_INVERT).toBool())
		dzl = zoom_level - dzl;
	else 
		dzl += zoom_level ;

	if(dzl<0) dzl = 0;
	if(dzl>= NZOOM) dzl=NZOOM-1;

	double fk = zoom_factors[dzl];
	zoom_level = dzl;

	QPointF c = toContent (e->pos());
	zoom = 1.0 / fk;

	QPointF nc = toContent (e->pos())-c;

	offset -= QPoint (nc.x(),nc.y());
	//Setting::setValue (MAPVIEW_GLOBAL_ZOOM, zoom_level);
	//Setting::setValue (MAPVIEW_GLOBAL_OFFSET, offset);
	updateMap();
}
void MapView::mouseReleaseEvent (QMouseEvent* ) 
{
}

void MapView::mouseMoveEvent (QMouseEvent* e) 
{
	if (e->buttons () & Qt::LeftButton) {
		QPoint pp = e->pos();
		QPoint de = lastmouse-pp;
		lastmouse = pp;
		double dx = de.x();
		double dy = de.y();
		dx /= zoom;
		dy /= zoom;
		offset += QPoint(dx,dy);
		// Setting::setValue (MAPVIEW_GLOBAL_OFFSET, offset);		
		updateMap();
	} else {
		emit mousePosChangedSignal (e->pos());
	}
	int to = townAt(e->pos()) ;
	if (to>=0) QToolTip::showText (e->globalPos(), Cmd::worldData().TownLabelText(to,true));
	else QToolTip::hideText();
}

void MapView::mouseDoubleClickEvent ( QMouseEvent *e) 
{
	if (e->buttons () & Qt::LeftButton) {

		int t = townAt (e->pos());

		if (t<0)	return;

		emit townDblClickSignal (t);
	}
}


void MapView::renderGrid (QPainter* p, QPointF minp, QPointF maxp)
{
	QColor gridcol = Setting::value(MAPVIEW_GRID_COLOR).value<QColor>();
	QPen pen (gridcol);
	p->setPen (pen);

	int xp [11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int yp [11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

	for (int x = 0; x <= 10; ++x) {
		double v = x;
		v *= 12800.;
		double miy = minp.y();
		double may = maxp.y();
		if(miy < 0) miy = 0;
		if(may > 128000.) may = 128000.;
		QPoint p0 = toScreen (QPointF(v,miy));
		QPoint p1 = toScreen (QPointF(v,may));
		xp[x]= p1.x();
		if (v < minp.x() || v > maxp.x()) continue;
		p->drawLine (p0,p1);
	}
	for (int y = 0; y <= 10; ++y) {
		double v = y;
		v *= 12800.;
		double mix = minp.x();
		double maxx = maxp.x();
		if(mix < 0) mix = 0;
		if(maxx > 128000.) maxx = 128000.;
		QPoint p0 = toScreen (QPointF(mix,v));
		QPoint p1 = toScreen (QPointF(maxx,v));
		yp[y]= p1.y();
		p->drawLine (p0,p1);
		if (v < minp.y() || v > maxp.y()) continue;
	}
	if (Setting::value(MAPVIEW_SHOW_OCEAN_LABELS).toBool()) {

		int last_left  =-1;
		for (int x = 0; x < 11; ++x) if (xp[x] < 0) {
			xp[x]=0;
			last_left = x;
		}

		int last_top  =-1;
		for (int y = 0; y < 11; ++y) if (yp[y] < 0){
			yp[y]=0;
			last_top = y;
		}


		QVector <int> lx;
		if (last_left >= 0) lx.push_back(last_left);
		for (int x = 0; x < 10; ++x) {
			if (xp[x] < 0) continue;
			if (xp[x] >= width()) break;
			if (lx.size() && (xp[x] - xp[lx[0]]) < 35) lx[0] = x;
			else lx.push_back(x);
		}

		QVector <int> ly;
		if (last_top >= 0) ly.push_back(last_top);
		for (int y = 0; y < 10; ++y) {
			if (yp[y] < 0) continue;
			if (yp[y] >=height ()) break;
			if (ly.size() && (yp[y] - yp[ly[0]]) < 20) ly[0] = y;
			else ly.push_back(y);
		}

		QFont fnt = p->font();
		QFont ofnt = fnt;

		fnt.setPointSize (10);
		fnt.setBold(true);
		p->setFont(fnt);

		QColor oc = Qt::black;
		QColor lac = Setting::value(MAPVIEW_OCEAN_LABEL_COLOR).value<QColor>();
		p->setBrush(lac); 
		if (color2gray(lac) < 30 ) oc = Qt::white;
		p->setPen(oc);

		for (int x=0; x < lx.size();++x) {
			int xpos = xp[lx[x]];
			for (int y=0; y < ly.size();++y) {
				int ypos = yp[ly[y]];
				int m = lx[x] * 10 + ly[y];
				::renderText(p, QPoint(xpos+5,ypos+15),QString("M%1").arg(m),oc, lac);
			}
		}
		p->setFont(ofnt);
	}

}



#define MIN_VIEWPORT 128.0
#define MIN_TOWNS 5

QPoint MapView::bestAllyLabelPos (int a, const QRectF& r, int &count)
{
	int labmode = Setting::value(MAPVIEW_INFMAP_LABEL_MODE).toInt();

	count = 0;
	int w = r.width();
	int h = r.height();
	int w2 = w/2;
	int h2 = h/2;
	int w4 = w/4;
	int h4 = h/4;
	int w34 = w2+w4;
	int h34 = h2+h4;

	int counter [9];
	int x_counter [9];
	int y_counter [9];
	int lti[9];

	memset(counter,0,sizeof(int)*9);
	memset(x_counter,0,sizeof(int)*9);
	memset(y_counter,0,sizeof(int)*9);
	memset(lti,0,sizeof(int)*9);

	// find global center of gravity
	int cx = 0;
	int cy = 0;
	int lt = 0;
	WorldData & world = Cmd::worldData();
	const Ally & al = world.allys()[a];
	for (int j=0;j <al.towns.size(); ++j) {
		QPointF po = world.towns()[al.towns[j]].pos();
		if (r.contains (po) ) {

			lt = j;

			int xp = po.x();
			int yp = po.y();
			int xpo = xp;
			int ypo = yp;
			cx += xp;
			cy += yp;
			++count;

			xp -= r.left ();
			yp -= r.top ();

			bool bx0 = xp <w2;
			bool bx1 = xp >= w4 && xp <= w34;
			bool bx2 = xp >= w2;
			bool by0 = yp < h2;
			bool by1 = yp >= h4 && yp <= h34;
			bool by2 = yp >= h2;

			if (bx0) {
				if (by0) {
					++counter [0];
					x_counter [0] += xpo;
					y_counter [0] += ypo;	
					lti[1] = j;
				}
				if (by1) {
					++counter [1];
					x_counter [1] += xpo;
					y_counter [1] += ypo;	
					lti[1] = j;
				}
				if (by2) {
					++counter [2];
					x_counter [2] += xpo;
					y_counter [2] += ypo;	
					lti[2] = j;
				}
			}
			if (bx1) {
				if (by0) {
					++counter [3];
					x_counter [3] += xpo;
					y_counter [3] += ypo;	
					lti[3] = j;
				}
				if (by1) {
					++counter [4];
					x_counter [4] += xpo;
					y_counter [4] += ypo;	
					lti[4] = j;
				}
				if (by2) {
					++counter [5];
					x_counter [5] += xpo;
					y_counter [5] += ypo;	
					lti[5] = j;
				}
			}
			if (bx2) {
				if (by0) {
					++counter [6];
					x_counter [6] += xpo;
					y_counter [6] += ypo;	
					lti[6] = j;
				}
				if (by1) {
					++counter [7];
					x_counter [7] += xpo;
					y_counter [7] += ypo;	
					lti[7] = j;
				}
				if (by2) {
					++counter [8];
					x_counter [8] += xpo;
					y_counter [8] += ypo;	
					lti[8] = j;
				}
			}
		}
	}

	QPoint ret;
	if (!count) return ret;

	if (count < MIN_TOWNS) return world.towns()[al.towns[lt]].pos();

	bool dorec = ((double) w * zoom) > MIN_VIEWPORT && ((double) h * zoom) > MIN_VIEWPORT; 

	if (!dorec || labmode == 0) return QPoint (cx / count, cy / count);
	
	int bc  = 0;	
	int cc  = counter[0];
	for (int i=1;i<9; ++i) {
		if (counter[i] > cc) {
			cc = counter[i];
			bc = i;
		}
	}

	if (cc < MIN_TOWNS) return world.towns()[al.towns[lti[bc]]].pos();

	// 3*x+y
	int sy = bc%3;
	int sx = bc/3;

	QRectF nr(r.left()+sx*w4,r.top()+sy*h4,w2,h2);
	return bestAllyLabelPos (a, nr, count);
}

static bool canMoveRight (int v)  {return (v&4)==0;}
static bool canMoveUp (int v)  {return (v&2)==0;}
static bool canMoveDown (int v)  {return (v&8)==0;}
static bool canMoveLeft (int v)  {return (v&1)==0;}

static bool adjustLabelPos (QVector <QRect > &re, QVector <int> &lim, int /*wi*/, int /*he*/, int idx)
{
	int spac = 2;
	int ol = -1;
	QPoint dl(spac,spac);
	QRect r (re[idx].topLeft()-dl, re[idx].bottomRight()+dl);
	QRect r2;
	for (int i=0;i<re.size(); ++i) {
		if (i == idx) continue;
		r2 = QRect (re[i].topLeft()-dl, re[i].bottomRight()+dl);
		if (r.intersects(r2)) {
			ol = i;	
			break;
		}
	}

	if (ol < 0) return false;

	// find smallest movement of both labels
	int dx = r2.center().x() - r.center().x();
	int dy = r2.center().y() - r.center().y();
	int l_shift_r = 0;
	int r_shift_r = 0;
	int u_shift_r = 0;
	int d_shift_r = 0;
	int vm = 0;
	int hm = 0;
	if (dx < 0) { // r lies right of r2
		r_shift_r = r2.right() - r.left();			
		if (r_shift_r <0) r_shift_r=0;
		hm = r_shift_r;
	} else { // r lies left of r2 
		l_shift_r = r.right()-r2.left();			
		if (l_shift_r <0) l_shift_r=0;
		hm = l_shift_r;
	}
	if (dy < 0) { // r lies below r2
		d_shift_r = r2.bottom() - r.top();			
		if (d_shift_r <0) d_shift_r=0;
		vm = d_shift_r;
		if (hm && vm < hm) {
			l_shift_r = 0;
			r_shift_r = 0;
		}
	} else { // r lies above r2 
		u_shift_r = r.top()-r2.bottom();			
		if (u_shift_r <0) u_shift_r=0;
		vm = u_shift_r;
		if (hm && vm < hm) {
			l_shift_r = 0;
			r_shift_r = 0;
		}
	}
	int dx1;
	int dx2;
	if (l_shift_r) {		
		if (canMoveLeft (lim[idx])) {
			if (canMoveRight (lim[ol])) {
				dx1 = -l_shift_r / 2;
				dx2 = l_shift_r / 2;
			} else {
				dx1 = l_shift_r / 2;
			}
		} else {
			if (canMoveRight (lim[ol])) {
				dx2 = l_shift_r;
			} else {
				return false;
			}
		}
	}


	return false;
}

void MapView::renderAllyLabels (QPainter * p, QPointF minp, QPointF maxp)
{
 	TriMesh m;

	int mar = 6;

	QFont of = p->font();
	QFont f = of;
	f.setPointSize (12);
	f.setBold(true);
	p->setFont(f);

	QRectF vr ((minp),(maxp));

	QVector <QRect> rects;
	QVector <int > move_lim;
	QVector <QString> labs;
	QVector <QColor> cols;

	WorldData & world = Cmd::worldData();


	for (int i=0; i < world.allys().size(); ++i) {
		Ally & al = world.allys()[i];
		if (!al.show) continue;
		if (!al.towns.size()) continue;

		int count = 0;
		QPoint sp = toScreen (bestAllyLabelPos (i, vr, count));

		if(! count ) continue;

		QRect r = QFontMetrics (f).boundingRect(al.name);


		int x0 = sp.x() - r.width()/2;
		int y0 = sp.y() - r.height()/2;
		int ml = 0;
		if (x0<mar) {x0=mar; ml =1;};
		if (y0<mar) {y0=mar; ml |= 2;};
		if ((x0+r.width())> (width()-mar))   {x0 = width()-r.width()-mar; ml |= 4;};
		if ((y0+r.height()) > (height()-mar)) {y0 = height()-r.height()-mar; ml |= 8;}

		rects.push_back(QRect (x0,y0,r.width(),r.height()));
		cols.push_back (al.color);
		move_lim.push_back(ml);
		labs.push_back(al.name);
	}

	// TODO: check overlapping


	for(int i=0; i < labs.size() ; ++i) {
		QColor pc = Qt::black;
		if (color2gray (cols[i]) < 30) pc = Qt::white;
		::renderText(p, rects[i], Qt::AlignCenter, labs[i], pc, cols[i]);
	}
	p->setFont (of);

}

int MapView::townAt (const QPoint &p) const
{
	int r = TOWN_BASE_SIZE + 5;
	long long int r2 = r*r;
	QPointF pp = toContent (p);
	WorldData & world = Cmd::worldData();
	for (int i=0;i<world.towns().size(); ++i) {
		QPoint tp = world.towns()[i].pos();
		long long int dx = pp.x()-tp.x();				
		long long int dy = pp.y()-tp.y();				
		if (r2 >= (dx*dx+dy*dy)) 
			return i;
	}	
	return -1;
}

void MapView::showToolTip (const QPoint &p)
{
	int t = townAt (p);
	if (t<0) return;
}

bool MapView::event ( QEvent * ev )
{
	if (ev->type() == QEvent::ToolTip) {
		/*
		QHelpEvent * h = dynamic_cast<QHelpEvent *> (ev);
		int to = townAt (h->pos ());
		if (to >= 0) {
		QToolTip::showText (h->globalPos(),world.TownLabelText(to,true));

		} else {
		QToolTip::hideText();
		ev->ignore();
		}
		return true;
		*/
	} else if (ev->type() == QEvent::Resize) {
		updateMap ();
	}
	return QFrame::event (ev);
}





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


void MapView::renderBuffer (QPainter* p) {

	QMutexLocker ml(&mutex);

	if (buff.isNull()) return;
	QPointF tl = toScreen (buff_rect.topLeft());
	QPointF br = toScreen (buff_rect.bottomRight());
	p->drawImage (QRectF(tl,br), buff,buff.rect());

};


void MapView::paintEvent (QPaintEvent* e) 
{
	Setting::setValue (MAPVIEW_GLOBAL_ZOOM, zoom_level);
	Setting::setValue (MAPVIEW_GLOBAL_OFFSET, offset);

	QColor oceancol = Setting::value(MAPVIEW_OCEAN_COLOR).value<QColor>();

	QPointF minp = toContent(e->rect().topLeft());
	QPointF maxp = toContent(e->rect().bottomRight());
	QRectF contr (minp,maxp);
	
	int w = 512.*zoom;
	int minx = (int) minp.x()%128;
	int miny = (int) minp.y()%128;
	QBrush bb(ocean.copy(minx,miny,512,512).scaled(w,w));
	QPainter p(this);
	p.setPen (Qt::NoPen);
	p.setBrush (bb);
	p.drawRect(0,0,width(),height());
	
	int rml = Setting::value(MAPVIEW_ROCK_ZOOM).toInt()-1;
	int iml = Setting::value(MAPVIEW_INFMAP_ZOOM).toInt()-1;

	bool show_rocks = (rml >= 0 && zoom_level <= rml);

	QColor ghostcol = Setting::value(MAPVIEW_GHOST_COLOR).value<QColor>();
	bool showghosts = Setting::value(MAPVIEW_SHOW_GHOSTS).toBool();
	
	QColor unselcol = Setting::value(MAPVIEW_UNSELECTED_COLOR).value<QColor>();
	
	bool show_inf_map = (iml >= 0 && zoom_level >= iml);
	int unselmode = Setting::value(MAPVIEW_UNSELECTED_MODE).toInt();

	int  minrad = 6;

	if(show_inf_map)
	
		renderBuffer (&p);

	WorldData world = Cmd::worldData();
	int numi = world.islands().size();

	for (int i = 0; i < numi; ++i) {

		const Island& il = world.islands()[i];
		if (!show_rocks && il.type >16 && il.type <37) { 
			continue;
		}

		QImage img = il.image;

		QRectF imr (il.x,il.y,img.width(),height());
		if (!contr.intersects(imr)) continue;

		p.translate (toScreen (QPoint(il.x,il.y)));
		p.scale(zoom,zoom);
		p.drawImage (0, 0, il.image);
		p.resetTransform();

		p.setPen(Qt::black);
		if(zoom == 1.0){
			QSize is= App::instance->globalData().island(il.type).size();
			QPoint ip = world.islandCoordinates(i);
			ip+= QPoint(is.width()/2, is.height()/2);
			QString info = QString("(id: %1 index: %2)\n Type: %3").arg(il.id).arg(i).arg(il.type);
			p.drawText(toScreen(ip), info);
		}

		for (int j =0; j < il.towns.size(); ++j){

			bool tactical = false;

			int town = il.towns[j];
			const Town& to = world.towns()[town];
			QPoint po = toScreen (to.pos());
			int pi = to.player;			

			QColor c = ghostcol;
			bool show = showghosts;

			if (pi >= 0) {

				const Player & pla = world.players()[pi];

				show = pla.show;
				c = pla.color;
				tactical = show;

				int ai = pla.ally;

				if (ai>=0 && !show) {
					show = world.allys()[ai].show;
					c = world.allys()[ai].color;
				} 

				if (!show) { // Neither ally nor player are selected

					if (unselmode == 0) {        // color of ally

						show = true;

					} else if (unselmode == 1) { // all same color

						show = true;
						c = unselcol;
					}
				}
			}
	
			if (!show) continue;


			p.setBrush (c);
			double rad = 27 + to.points / 1000;
			rad *= zoom;
			QColor penc = Qt::black;
			if(color2gray(c) < 50) penc = Qt::white;
			QPen pen(penc);
			double lw = zoom * 5;
			if(lw<1) lw=1;
			pen.setWidthF(lw);
			p.setPen(pen);
			if (tactical ) { // && zoom_level < 18) {
				if (rad < minrad) rad = minrad;
				WorldData::renderTownTypeSymbol (&p,to.type,po,rad);
			} else {
				p.setBrush (c);
				rad *= 0.7978846;
				if (rad < 1) rad = 1;
				p.setPen(Qt::NoPen);
				p.drawEllipse (po,(int)rad,(int)rad);					
			}
			p.setPen(Qt::black);
			if(zoom == 1.0){
				p.drawText(po+QPoint(20,-20), QString::number(world.towns()[town].no));
			}

			

		}
	}

	if (Setting::value (MAPVIEW_SHOW_GRID).toBool())
		renderGrid (&p , minp, maxp);

	
	int aml = Setting::value(MAPVIEW_INFMAP_LABEL_ZOOM).toInt()-1;
	if (aml >= 0 && zoom_level >= aml) {
		renderAllyLabels (&p,minp,maxp);
	}

}
