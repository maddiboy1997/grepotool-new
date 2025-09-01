#ifndef ___INFMAP_H
#define ___INFMAP_H
/* infmap.h  - political maps

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

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QImage>

class OldWD;


// The entire map has a pixel resolution of 1000 x 1000 fields
// each with a size of 128 x 128 pixels
// the following defines the reach of the influence of a city
#define CITY_REACH 20

// each field is subdivided into 16 squares of 32x32 pixel each
#define GRID_DIV 4


#define INF_MAP_WIDTH (1000*GRID_DIV)
#define TOWN_INF_WIDTH (2 * GRID_DIV * CITY_REACH + 1)
#define INF_MAP_SIZE INF_MAP_WIDTH*INF_MAP_WIDTH
#define TOWN_INF_SIZE TOWN_INF_WIDTH*TOWN_INF_WIDTH


class InfMapRenderer : public QThread {
	Q_OBJECT;
public:
	InfMapRenderer (QObject * p);
	~InfMapRenderer ();
	void render (const WorldData& d);

signals:
	void renderedImage(const QImage &);
	void progressSignal(int);

protected:
	void run();

private:
	QMutex m;
	QWaitCondition _condition;
	bool _restart;
	bool _abort;    
	QColor _oc ;
	int    _opa;

	QVector<QColor>  _cols;
	QVector<bool>    _show;
	QVector<QVector <QPoint> > _towns;	
};



#endif