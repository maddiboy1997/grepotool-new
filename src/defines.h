#ifndef __DEFINES_H
#define __DEFINES_H
/* defines.h  - global definitions

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
#include <QString>
#include <QStringList>
#include <QVector>
#include <QColor>
#include <QImage>
#include <QFile>
#include <QRect>
#include <QFont>
#include <QDateTime>

class QPainter;

/* Global utility functions
*/
QString replaceStrangeChars (const QString &s);
QPoint getTownOffset (int island_type, int town_no);
int getTownOrientation (int island, int town_no);
int getTownLabelDirection (int island, int town_no);
QPoint getIslandCenterOffset (int island);

bool storeString (QFile & f, const QString &s);
bool restoreString (QFile & f, QString &s);
QImage processIsland (QImage);
QColor randomColor ();
QByteArray readZipfile (const QString& filename, bool& err, QString& msg);
bool writeZipfile  (const QString& filename, const QByteArray &d);
QByteArray readGzipfile (const QString& filename, bool& err);
inline int color2gray(const QColor&c) {return (25*c.red()+60*c.green()+15*c.blue()) / 100;};

void renderText (QPainter* p, const QRect & rectangle, int flags, const QString & text, const QColor& bg, const QColor& fg);
void renderText (QPainter* p, const QPoint& pos, const QString & text, const QColor& bg, const QColor& fg);


QString appTitle ();
QString appVersionLevel ();
int     appVersionLo ();
int     appVersionHi ();
QString appVersionInfo ();
QString appVersionNumberText2 ();
QString appCfgKey ();

#define NZOOM 35
static const double zoom_factors [NZOOM] = {
	1, 1.125, 1.25 , 1.5 , 1.75, 2   , 2.25, 2.5, 3.0, 3.5, 4,   5,
	6, 7    , 8    , 9 ,   10,   12.5,   15,  17.5,  20, 25,  30, 
	35,40   ,50,    60,    70 ,  80,     90, 100, 125, 150, 175, 200};


#endif
