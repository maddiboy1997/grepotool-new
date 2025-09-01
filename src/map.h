#ifndef ___MAP_H
#define ___MAP_H
/* map.h  - world map rendering

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

#include <QThread>
#include <QVector>
#include <QMutex>
#include <QWaitCondition>
#include <QPolygonF>
#include <QColor>

class MapThread  : public QThread {
	Q_OBJECT;
public:
	MapThread (QObject * p);
	~MapThread ();

	void aquireImage (int zoom_level, const QPoint &offset, const QSize &viewport);

	void run();

public slots:
	void meshCreated (const QVector<QPolygonF> &pol, const QVector<QPoint>& pts, const QVector<QColor>& col);
	void meshColorsChanged (const QVector<QColor>& col);
	void clearMesh();

signals:
	void imageRendered (const QImage& img, const QRectF& r);

protected:
	QMutex _m;
	QWaitCondition _condition;
	bool _restart;
	bool _abort;    

	QVector <QPolygonF> _pol;
	QVector <QColor>    _pol_col;
	QVector <QPoint>    _pol_pos;
	int                 _pol_reach;
	int                 _pol_opa;

	int                 _zoom_level;
	QPoint              _offset;
	QSize               _viewport;
};











#endif