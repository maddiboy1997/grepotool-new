/* towns.cpp -  world data: Towns

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

#include "world_data.h"
#include "defines.h"
#include "app.h"
#include "json.h"

Town::Town()
{
	type= TownTypeUndefined;
	god=AnyGod;
	movement_boost = false;
	memset(buildings,0,BuildingMax*sizeof(int));
	memset(units,0,UnitMax*sizeof(int));
	memset(research,0,ResearchMax*sizeof(bool));
}

Town& Town::operator = (const Town& o)
{
	type= o.type;
	id= o.id;
	name=o.name;
	points=o.points;
	player=o.player;
	ally=o.ally;
	island=o.island;
	movement_boost = o.movement_boost;
	x=o.x;
	y=o.y;
	no=o.no;// on island;
	god=o.god;
	memcpy(buildings,o.buildings,BuildingMax*sizeof(int));
	memcpy(units,o.units,UnitMax*sizeof(int));
	memcpy(research,o.research,ResearchMax*sizeof(bool));
	return *this;
}

bool WorldDataLoader::processTowns() {
	QString head = tr("Processing towns of %1").arg(data.world());
	bool err=false;
	emit progressSignal(-1,head);
	App::processEvents();
	QString msg;
	// QString s (readGzipfile(data.townFile(),err));
	QByteArray ba = readGzipfile(data.townFile(),err);
	if ( err ) {
		setError(data.playerFile(),tr("Failed to read town descriptions"), tr("GUnzip failed"));
		return false;
	}
	QTextStream ds(&ba, QIODevice::ReadOnly);
	if ( ds.status() != QTextStream::Ok ) {
		setError(data.playerFile(),tr("Failed to read town descriptions"), tr("Data is possibly corrupted"));
		return false;
	}
	
	int tot=0;
	int operc=-1;
	while (!ds.readLine().isEmpty()) ++tot;
	ds.resetStatus();
	ds.seek(0);

	data.towns().resize(tot);
	int i=0;
	QString li = ds.readLine();
	while ( ds.status() == QTextStream::Ok ) {
		QStringList k = li.split(",");
		int perc = 100 * (i+1)/(tot+1) ;
		if (perc>operc) {
			operc=perc;
			emit progressSignal (perc, head);
			App::instance->processEvents();
		}

		if(abortRequested()) {
			setError ("", tr("User abort"),tr("Loading was cancelled by user"));
			return false;
		};
		if(k.size()!= 7) {
		setError(data.townFile(),
			tr("Failed to read town descriptions"), 
			tr("Corrupted file"));
			return false;
		}
		int id = k[0].toInt();
		data.townHash()[id] = i;

		data.towns()[i].id = id;
		int pl = -1;
		if(!k[1].isEmpty()) pl = data.playerHash()[k[1].toInt()];
		data.towns()[i].player = pl;
		int al = -1;
		if (pl>=0) {
			data.players()[pl].towns.push_back(i);
			al = data.players()[pl].ally;
			if (al >= 0) data.allys()[al].towns.push_back(i);
		}
		data.towns()[i].name = replaceStrangeChars(k[2]);

		data.towns()[i].ally = al;
		int x = k[3].toInt();
		int y = k[4].toInt();
		int hash_id = x*1000 + y;
		
		y = (x&1)? y*128+64 : y*128;
		x = x * 128;
		int isl = data.islandPosHash()[hash_id];
		int no = k[5].toInt();
		QPoint off = getTownOffset (data.islands()[isl].type, no);
		data.towns()[i].x = x+off.x();
		data.towns()[i].y = y+off.y();
		data.islands()[isl].towns.push_back(i);
		data.towns()[i].island = isl;
		data.towns()[i].no = no;
		data.towns()[i].points = k[6].toInt();
		
		++i;
		li = ds.readLine();
		if (li.isEmpty()) break;
	}
	return true;
}; 


TownTypeIndex Town::guessType ()
{

	TownTypeIndex ret = TownTypeUndefined;

	int fs = units[Attackship];
	int lo = units[Hoplite] + units[Slinger] + units[Rider] + units[Chariot];
	int mant = units[Manti];
	int harp = units[Harpy];
	int kol = units[Kolo];
	if (fs) {
		ret = TownTypeAtt;
		if (fs>100) ret = TownTypeAttShip;
		if ( kol ) ret = TownTypeAttKolo;
	} else if (lo) {
		ret = TownTypeAtt;		
		if ( kol ) ret = TownTypeAttKolo;
	} else if (mant > 10) {
		ret = TownTypeManti;		
	} else if (harp > 10) {
		ret = TownTypeHarpy;		
	}
	return ret;
}

QPixmap WorldData::townTypeSymbol(TownTypeIndex typ) 
{
	QImage img (40,40, QImage::Format_ARGB32_Premultiplied);
	img.fill(0);
	QPainter p(&img);
	QPen pen(Qt::black);
	pen.setWidth (5);
	p.setBrush(Qt::white);
	renderTownTypeSymbol (&p,typ,QPoint(20,20),20);
	return QPixmap::fromImage (img);
}


#define SIN45   0.7071068
#define SIN60   0.8660254
#define CIRCFAC 0.7978846

void WorldData::renderTownTypeSymbol(QPainter* paint, TownTypeIndex type, const QPoint& pos, const double &r)
{	
	double cx = pos.x();
	double cy = pos.y();
	if (type == TownTypeAtt) {       // rect  
		QPointF p0 (cx - r*SIN45,   cy-r*SIN45); 
		QPointF p1 (cx + r*SIN45,   cy-r*SIN45); 
		QPointF p2 (cx + r*SIN45,   cy+r*SIN45); 
		QPointF p3 (cx - r*SIN45,   cy+r*SIN45); 
		QPolygonF po;
		po << p0;
		po << p1;
		po << p2;
		po << p3;
		po << p0;
		paint->drawPolygon (po);
	} else if (type == TownTypeDeff) {      // Triangle
		QPointF p0 (cx,             cy-r); 
		QPointF p1 (cx - r * SIN60, cy + r * 0.5); 
		QPointF p2 (cx + r * SIN60, cy + r * 0.5); 
		QPolygonF po;
		po << p0;
		po << p1;
		po << p2;
		po << p0;
		paint->drawPolygon (po);
	} else if (type == TownTypeAttShip) {   // raute
		QPointF p0 (cx,             cy - r ); 
		QPointF p1 (cx + r,         cy); 
		QPointF p2 (cx ,            cy + r ); 
		QPointF p3 (cx -r ,         cy); 
		QPolygonF po;
		po << p0;
		po << p1;
		po << p2;
		po << p3;
		po << p0;
		paint->drawPolygon (po);
	} else if (type == TownTypeAttKolo) {   // rect+hat
		QPointF pp0 (cx - r*SIN45,   cy-r*SIN45); 
		QPointF p0  (cx,             cy - r ); 
		QPointF pp1 (cx + r*SIN45,   cy-r*SIN45); 
		QPointF pp2 (cx + r*SIN45,   cy+r*SIN45); 
		QPointF pp3 (cx - r*SIN45,   cy+r*SIN45); 
		QPolygonF ppo;
		ppo << pp0;
		ppo << p0;
		ppo << pp1;
		ppo << pp2;
		ppo << pp3;
		ppo << pp0;
		paint->drawPolygon (ppo);

	} else if (type == TownTypeBireme) {    // Tri down
		QPointF p0 (cx,             cy+r); 
		QPointF p1 (cx - r * SIN60, cy - r * 0.5); 
		QPointF p2 (cx + r * SIN60, cy - r * 0.5); 
		QPolygonF po;
		po << p0;
		po << p1;
		po << p2;
		po << p0;
		paint->drawPolygon (po);
	} else if (type == TownTypeManti) {     // star
		renderTownTypeSymbol(paint, TownTypeDeff, pos, r);
		renderTownTypeSymbol(paint, TownTypeBireme, pos, r);
	} else if (type == TownTypePega) {      // Rect with slash
		renderTownTypeSymbol(paint,TownTypeAtt, pos, r);
		QPointF p0 (cx - r*SIN45,   cy + r*SIN45); 
		QPointF p1 (cx + r*SIN45,   cy - r*SIN45); 
		paint->drawLine (p0,p1);		
	} else if (type == TownTypeHarpy) {    //  rect with backslash
		renderTownTypeSymbol(paint,TownTypeAtt, pos, r);
		QPointF p0 (cx + r*SIN45,   cy + r*SIN45); 
		QPointF p1 (cx - r*SIN45,   cy - r*SIN45); 
		paint->drawLine (p0,p1);		
	} else { //  if(typ == TownTypeUndefined) { // circle
		paint->drawEllipse (QPointF(cx,cy),r*CIRCFAC, r*CIRCFAC);
	}

}

