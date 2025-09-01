/* global_static.cpp -  static global data

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
#include "app.h"
#include "defines.h"
#include "global_data.h"

#include <QStringList>
#include <QFile>
#include <QByteArray>




/*! remove the ocean Part from an island image
*/
static QImage processIslandImage (QImage img) {

	QRgb tc = img.pixel(0,0);

	QVector <QRgb> cols = img.colorTable ();

	int ti = -1;
	for (int i=0;i< cols.size();++i) {
		QRgb v = img.color(i);
		if (v == tc) {
			ti = i;
			break;
		}
	}
	// now remove all entries from the color table, where 
	// blue dominates
	if (ti<0) return img;
	for (int i=0;i< cols.size();++i) {
		if (i == ti) continue;
		int r = qRed (img.color(i));
		int g = qGreen(img.color(i));
		int b = qBlue (img.color(i));
		if ( b > r && b > g) img.setColor(i,tc);		
	}
	return img;
}

void GlobalData::init () 
{
	int imgno = 1;

	QString dir = ":/icons";

	QString fn ("/island%1.png");
	for(int i= 1; i <= 10; ++i ) {
		_d->islands[imgno] = processIslandImage (QImage(dir + fn.arg(i)));
		++imgno;		
	} 
	fn = "/uninhabited%1.png";
	for(int i= 1; i <= 6; ++i ) {
		_d->islands[imgno] = processIslandImage (QImage(dir + fn.arg(i)));
		++imgno;		
	} 
	fn = "/rock%1.png";
	for(int i= 1; i <= 20; ++i ) {
		_d->islands[imgno] = processIslandImage (QImage(dir + fn.arg(i)));
		++imgno;		
	} 

	fn = "/island%1.png";
	for(int i= 11; i <= 20; ++i ) {
		_d->islands[imgno] = processIslandImage (QImage(dir + fn.arg(i)));
		++imgno;		
	} 

	fn = "/uninhabited%1.png";
	for(int i= 7; i <= 20; ++i ) {
		_d->islands[imgno] = processIslandImage (QImage(dir + fn.arg(i)));
		++imgno;		
	} 
}


QPoint GlobalDataPrivate::islandCenter (int island) const 
{
	if (island<1 || island>60)  return QPoint(0,0);
	QSize s = islands[island].size();
	QPoint co = getIslandCenterOffset(island);
	return QPoint (s.width()/2, s.height()/2) + co;
}




