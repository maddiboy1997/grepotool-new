/* tri_mesh.cpp - Triangulation

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

#include "tri_mesh.h"

/*! \brief 2D - line intersection 

2D - line intersection 		
\param x0 Y coordinate of point 1 of first line segment
\param y0 Y coordinate of point 1 of first line segment
\param x1 Y coordinate of point 2 of first line segment
\param y1 Y coordinate of point 2 of first line segment
\param x2 Y coordinate of point 1 of second line segment
\param y2 Y coordinate of point 1 of second line segment
\param x3 Y coordinate of point 2 of second line segment
\param y3 Y coordinate of point 2 of second line segment
\param n parameter n of first line (0..1 if intersection in the segment) 
\param m parameter m of second line (0..1 if intersection in the segment)
\return true, if the two lines intersect 
*/
inline bool lineIntersect2D (const double &x0, const double &y0,
							 const double &x1, const double &y1,
							 const double &x2, const double &y2,
							 const double &x3, const double &y3,
							 double &n, double &m)
{      
	// 0 = x0 + n (x1-x0) - x2 - m (x3-x2) 
	// 0 = y0 + n (y1-y0) - y2 - m (y3-y2)

	//      x0-x2       x1-x0     y0-y2       y1-y0
	// m = ------- + n ------- = ------- + n -------      
	//      x3-x2       x3-x2     y3-y2       y3-y2

	//  x0-x2     y0-y2       y1-y0       x1-x0 
	// ------- - ------- = n ------- - n -------  
	//  x3-x2     y3-y2       y3-y2       x3-x2  

	// (y3-y2)(x0-x2)-(x3-x2)(y0-y2)    (y1-y0)(x3-x2)-(x1-x0)(y3-y2)
	// ---------------------------- = n ----------------------------
	//        (x3-x2)(y3-y2)                   (x3-x2)(y3-y2)

	//     (y3-y2)(x0-x2)-(x3-x2)(y0-y2)    
	// n = -----------------------------
	//     (y1-y0)(x3-x2)-(x1-x0)(y3-y2)       

	double nn = (y1-y0)*(x3-x2)-(x1-x0)*(y3-y2);    

	if (fuzzyZero(nn)) return false;

	double z = (x0-x2)*(y3-y2)-(y0-y2)*(x3-x2);

	n =  z / nn;
	if (! fuzzyZero (x3-x2) ) 
	{
		m = ((x0-x2) + n * (x1-x0)) / (x3-x2) ;
		return true;
	}
	else if (! fuzzyZero (y3-y2) ) 
	{
		m = ((y0-y2) + n * (y1-y0)) / (y3-y2);       
		return true ;
	}
	return false;
} 

static bool lineIntersection (const QPointF & p0, const QPointF & p1, // first line
							  const QPointF & p2,const QPointF & p3, // second line
							  QPointF & r)
{
	double n = 0.0; 
	double m = 0.0;
	if (!lineIntersect2D (p0.x(),p0.y(),p1.x(),p1.y(), p2.x(),p2.y(),p3.x(),p3.y(), n,m) ) return false;
	QPointF di = p1 - p0;
	r = p0 + (di * n);
	return true;
}

QPointF LineSeg::coIntersection (const LineSeg&o) const 
{
	double n = 0.0; 
	double m = 0.0;
	LineSeg co = centerOrtho ();
	LineSeg oco = o.centerOrtho ();

	if (!lineIntersect2D (co.p0.x(),co.p0.y(),co.p1.x(),co.p1.y(), oco.p0.x(),oco.p0.y(),oco.p1.x(),oco.p1.y(), n,m) ) {
		return (length2() > o.length2())? center():o.center();
	}

	return co.p0 + co.dir()*n;
}


LineSeg LineSeg::centerOrtho () const 
{ 
	QPointF nd = dir();
	QPointF c = center();
	QPointF od (nd.y(), -nd.x());
	return LineSeg (c, c+od);
};

bool LineSeg::intersects (const LineSeg & o, QPointF & r) const
{
	// return lineIntersection (p0, p1, o.p0, o.p1, r);		
	double n = 0.0; 
	double m = 0.0;
	if (!lineIntersect2D (p0.x(),p0.y(),p1.x(),p1.y(), o.p0.x(),o.p0.y(),o.p1.x(),o.p1.y(), n,m) ) return false;
	if (n<0.0) return false;
	if (m<0.0) return false;
	if (n>1.0) return false;
	if (m>1.0) return false;
	QPointF di = p1 - p0;
	r = p0 + (di * n);
	return true;
};


bool LineSeg::perpend (const QPointF &p, QPointF & r) const
{
	// line l1 = p0,p1
	//      l2 = p, pp
	QPointF dira = dir();

	double nn=0;
	double mm=0;
	lineIntersect2D (p0.x(),p0.y(),p1.x(),p1.y(), p.x(),p.y(),p.x()+dira.y(),p.y()-dira.x(), nn, mm);
	if(nn <= 0.0) return false;
	if(nn >= 1.0) return false;
	r = QPointF (nn*(double)dira.x()+p0.x(),nn*(double)dira.y()+p0.y());
	return true;
}

bool Edge::perpend (const QPointF &p, QPointF & r) const
{
	double p0x = vertex[0]->point.x();
	double p0y = vertex[0]->point.y();
	double p1x = vertex[1]->point.x();
	double p1y = vertex[1]->point.y();
	double dirax = p1x-p0x;
	double diray = p1y-p0y;
	double nn=0;
	double mm=0;
	lineIntersect2D (p0x,p0y,p1x,p1y, p.x(), p.y(), p.x()+diray, p.y()-dirax, nn, mm);
	if(nn <= 0.0) return false;
	if(nn >= 1.0) return false;
	r = QPointF (nn*dirax+p0x,nn*diray+p0y);
	return true;
}

bool Edge::intersects (const LineSeg & o, QPointF & r) const
{
	double p0x = vertex[0]->point.x();
	double p0y = vertex[0]->point.y();
	double p1x = vertex[1]->point.x();
	double p1y = vertex[1]->point.y();
	double n = 0.0; 
	double m = 0.0;
	if (!lineIntersect2D (p0x,p0y,p1x,p1y, o.p0.x(),o.p0.y(),o.p1.x(),o.p1.y(), n,m) ) return false;
	if (n<0.0) return false;
	if (m<0.0) return false;
	if (n>1.0) return false;
	if (m>1.0) return false;
	double dirax = p1x-p0x;
	double diray = p1y-p0y;
	r = QPointF (n*dirax+p0x,n*diray+p0y);
	return true;
};
