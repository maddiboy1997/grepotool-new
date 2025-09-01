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


bool Tri::containsPoint (const QPointF & p) const
{
	LineSeg l(cog,p);
	QPointF ppp;
	if (l.intersects(LineSeg(vertex[0]->point,vertex[1]->point), ppp)) return false;
	if (l.intersects(LineSeg(vertex[2]->point,vertex[1]->point), ppp)) return false;
	if (l.intersects(LineSeg(vertex[2]->point,vertex[0]->point), ppp)) return false;
	return true;
}


Tri* Tri::find (const QPointF&p)
{
	LineSeg l(cog,p);
	QPointF ppp;
	if (edge[0]->intersects(l, ppp)) return edge[0]->oppositeTri(this);
	if (edge[1]->intersects(l, ppp)) return edge[1]->oppositeTri(this);
	if (edge[2]->intersects(l, ppp)) return edge[2]->oppositeTri(this);
	return this;
}



//    p1   e0  p2    Clockwise ordered:
//      +----+       Point 0 is opposite to e0 
//      |   /        Point 1 is opposite to e1 
//      |  /         Point 2 is opposite to e2 
//   e2 | / e1  
//      |/
//      + p0 
// 
Tri::Tri (Vertex* p0, Vertex* p1, Vertex* p2, Edge* e0, Edge* e1, Edge* e2) 
{
	vertex[0]=p0;
	vertex[1]=p1;
	vertex[2]=p2;

	edge[0] = e0;
	edge[1] = e1;
	edge[2] = e2;

	xmin = (p0->point.x() < p1->point.x())? p0->point.x() : p1->point.x();
	ymin = (p0->point.y() < p1->point.y())? p0->point.y() : p1->point.y();
	xmax = (p0->point.x() > p1->point.x())? p0->point.x() : p1->point.x();
	ymax = (p0->point.y() > p1->point.y())? p0->point.y() : p1->point.y();

	xmin = (xmin < p2->point.x()) ? xmin : p2->point.x();
	ymin = (ymin < p2->point.y()) ? ymin : p2->point.y();
	xmax = (xmax > p2->point.x()) ? xmax : p2->point.x();
	ymax = (ymax > p2->point.y()) ? ymax : p2->point.y();

	double x = ((double)p0->point.x()+ (double)p1->point.x()+ (double)p2->point.x()) / 3.0;
	double y = ((double)p0->point.y()+ (double)p1->point.y()+ (double)p2->point.y()) / 3.0;

	cog = QPointF(x,y);
	center = lineSeg(0).coIntersection(lineSeg(1));
	double dx = center.x()-p0->point.x();
	double dy = center.y()-p0->point.y();
	_rad2 = dx*dx+dy*dy;
};

void Tri::registerEdges ()
{
	edge[0]->addTri(this);
	edge[1]->addTri(this);
	edge[2]->addTri(this);
}

void Tri::removeFromEdges ()
{
	edge[0]->takeTri (this);
	edge[1]->takeTri (this);
	edge[2]->takeTri (this);
};

bool Tri::pointInOuterCircle (const QPointF & p) const
{
	double dx = p.x()- center.x();
	double dy = p.y()- center.y();
	return _rad2 > (dx*dx+dy*dy);
};
double Tri::dist2 (const QPointF& p) const {
	double dx = p.x()-center.x();
	double dy = p.y()-center.y();
	return (dx*dx+dy*dy);
};

int Tri::containsEdge (const Edge *e) const 
{
	int r = 0;
	if (e == edge [0]) ++r;
	if (e == edge [1]) ++r;
	if (e == edge [2]) ++r;
	return r;
};

int Tri::containsVertex (const Vertex *e) const 
{
	int r = 0;
	if (e == vertex [0]) ++r;
	if (e == vertex [1]) ++r;
	if (e == vertex [2]) ++r;
	return r;
};

int Tri::vertexIndex (const Vertex *e) const 
{
	if (e == vertex [0]) return 0;
	if (e == vertex [1]) return 1;
	if (e == vertex [2]) return 2;
	return -1;
};

int Tri::oppositeVertexIndex (const Edge *e) const
{
	if(e->isEdge(vertex[0],vertex[1])) return 2;
	if(e->isEdge(vertex[1],vertex[2])) return 0;
	if(e->isEdge(vertex[0],vertex[2])) return 1;
	return -1;
	if (e == edge [0]) return 0;
	if (e == edge [1]) return 1;
	if (e == edge [2]) return 2;
};
