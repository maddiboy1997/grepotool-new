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
#include <iostream>


void TriMesh::doFlipEdge (Edge* e) 
{
	Tri * t0 = e->tri[0];
	Tri * t1 = e->tri[1];

	int i0 = t0->oppositeVertexIndex(e);
	int i1 = t1->oppositeVertexIndex(e);

	Vertex * v0 = t0->vertex[i0];
	Vertex * v2 = t1->vertex[i1];

	Vertex * v1 = t0->vertex[(i0+1)%3];
	Vertex * v3 = t0->vertex[(i0+2)%3];

	Edge *e0 = t0->edge[(i0+2)%3]; 
	Edge *e1 =  t1->edge[(i1+1)%3];
	Edge *e2 = t1-> edge[(i1+2)%3];
	Edge *e3 = t0-> edge[(i0+1)%3];

	// v0      v1    v0       v1
	// +--e0----+     +---e0---+
	// |       /|     |\       |
	// |  t0  / |     | \  t1  |          Remove t0&t1 from edges
	// |     /  |     |  \     |          reset edge  
	// e3   /   e1    e3  \    e1         recreate triangles
	// |   e    |     |    e   |
	// |  /     |     |     \  |
	// | /  t1  |     | t0   \ |          recursively check outer edges
	// |/       |     |       \|  
	// +---e2---+     +---e2---+  
	// v3       v2    v3      v2

	t0->removeFromEdges();
	t1->removeFromEdges();

	e->vertex[0] = v0;
	e->vertex[1] = v2;

	*t0 = Tri (v0, v2, v3, e2, e3, e);

	*t1 = Tri (v2, v0, v1, e0, e1, e);

	t0->registerEdges ();
	t1->registerEdges ();

}

void TriMesh::flipEdge (Edge* e) 
{
	if (e->isOuterEdge()) return;
	Tri * t0 = e->tri[0];
	Tri * t1 = e->tri[1];
	int i0 = t0->oppositeVertexIndex(e);
	int i1 = t1->oppositeVertexIndex(e);

	Vertex * v0 = t0->vertex[i0];
	Vertex * v2 = t1->vertex[i1];
	Vertex * v1 = t0->vertex[(i0+1)%3];
	Vertex * v3 = t0->vertex[(i0+2)%3];

	bool inv1 = t1->pointInOuterCircle (v0->point);
	bool inv2 = t0->pointInOuterCircle (v2->point);
	if(!inv1 && ! inv2) return;

	QPointF dmy;
	LineSeg liold (v1->point, v3->point);
	LineSeg linew (v0->point, v2->point);
	if(!liold.intersects(linew,dmy)) 
		return;

	doFlipEdge(e);
	if (t0->edge[0] != e) flipEdge (t0->edge[0]);
	if (t0->edge[1] != e) flipEdge (t0->edge[1]);
	if (t0->edge[2] != e) flipEdge (t0->edge[2]);
	if (t1->edge[0] != e) flipEdge (t1->edge[0]);
	if (t1->edge[1] != e) flipEdge (t1->edge[1]);
	if (t1->edge[2] != e) flipEdge (t1->edge[2]);
}


QVector <QPolygonF > TriMesh::delaunayPolygons () 
{
	QVector <QPolygonF > pols;
	for(int i=0;i< edges.size(); ++i) {
		edges[i]->vertex[0]->e = edges[i];
		edges[i]->vertex[1]->e = edges[i];
	}

	
	for (int i=4;i< verts.size(); ++i) {
		QPolygonF pol;
		Vertex * v = verts[i];
		Edge * startedge = verts[i]->e;
		Tri * curt = startedge->tri[0];
		Edge * runedge = curt->nextEdge (startedge, v);
		LineSeg l1 = runedge->lineSeg();
		LineSeg l0 = startedge->lineSeg();
		pol.push_back(l0.coIntersection(l1));
		while (runedge != startedge) {
			l0=l1;
			curt = (runedge->tri[0] == curt) ? runedge->tri[1]:runedge->tri[0];
			runedge = curt->nextEdge (runedge, v);
			l1 = runedge->lineSeg();
			pol.push_back(l0.coIntersection(l1));
		}
		pols.push_back(pol);
	}
	return pols;
}


void TriMesh::splitTriangle (Tri* t, Vertex * v)
{
	//t->check();

	Edge * te0 = t->edge[0];
	Edge * te1 = t->edge[1];
	Edge * te2 = t->edge[2];
	Vertex * tp0 = t->vertex[t->oppositeVertexIndex(te0)];
	Vertex * tp1 = t->vertex[t->oppositeVertexIndex(te1)];
	Vertex * tp2 = t->vertex[t->oppositeVertexIndex(te2)];

	//                  tp0              
	//                  +             Create new vertex   v  
	//                 /|\            Create 3 new edges (e0,e1,e2) 
	//                / | \           
	//               /  e0 \          Remove Triangle t from outer edges 
	//              /t1 | t2\         
	//            te1   v   te2       Create 3 new Triangles  
	//            /   /   \   \     
	//           / e2      e1  \     
	//          / /    t0     \ \     Flip all edges if necessary
	//         //               \\   
	//        +--------te0--------+
	//       tp2                   tp1 



	Edge ne0 (tp0, v);
	Edge ne1 (tp1, v);
	Edge ne2 (tp2, v);

	Edge * e0 = addEdge (ne0);
	Edge * e1 = addEdge (ne1);
	Edge * e2 = addEdge (ne2);

	t->removeFromEdges ();

	Tri t0 (tp2,   v, tp1,  e1, te0 , e2);
	Tri t1 (tp0,   v, tp2,  e2, te1,  e0);
	Tri t2 (tp0, tp1,   v,  e1,  e0, te2);

	Tri * tt0 = addTriangle (t1);
	Tri * tt1 = addTriangle (t2);
	*t = t0;
	tt0->registerEdges ();
	tt1->registerEdges ();
	t->registerEdges ();

	flipEdge (te0);
	flipEdge (te1);
	flipEdge (te2);
	return;
}




void TriMesh::splitEdge (Edge * e, Vertex * v)
{
	// e->check();

	Tri * t0 = e->tri[0];
	Tri * t1 = e->tri[1];


	Vertex * tp0 = t0->vertex[t0->oppositeVertexIndex(e)];// t0->vertex[i0];
	Vertex * tp1 = t1->vertex[t1->oppositeVertexIndex(e)]; // i1];
	int i0 = t0->vertexIndex(tp0);
	int i1 = t1->vertexIndex(tp1);

	Vertex * p1 = t0->vertex[(i0+1)%3];
	Vertex * p0 = t0->vertex[(i0+2)%3];

	Edge *e0 = t0->edge[(i0+2)%3];
	Edge *e1 = t1->edge[(i1+1)%3];
	Edge *e2 = t1->edge[(i1+2)%3];
	Edge *e3 = t0->edge[(i0+1)%3];

	// tp0      p1    tp0      p1
	// +--e0----+     +---e0---+     Remove t0&t1 from edges          
	// |       /|     |\    1 /|     
	// |  t0  / |     | n0   / |     create 3 new edges and replace e
	// |     /  |     |  \  n1 |
	// e3   /   e1    e3  \/ 2 e1    create 2 new triangles and replace t0&t1
	// |   e    |     | 0 /\   |
	// |  /     |     |  /  n2 |
	// | /  t1  |     | n3   \ |
	// |/       |     |/   3  \|  
	// +---e2---+     +---e2---+  
	// p0      tp1    p0      tp1

	t0->removeFromEdges ();
	t1->removeFromEdges ();
	Edge * n0 = addEdge (Edge (tp0,v));
	Edge * n1 = addEdge (Edge (p1,v));
	Edge * n2 = addEdge (Edge (tp1,v));
	Edge * n3 = e;
	*n3 = Edge (p0,v);

	Tri * t2 = addTriangle (Tri (tp0,  v,  p0,   n3,e3,n0));
	Tri * t3 = addTriangle (Tri (tp0, p1,  v,    n1,n0,e0));
	*t0 =        Tri (p1, tp1,  v,    n2,n1,e1); 
	*t1 =        Tri (tp1, p0,  v,    n3,n2,e2); 

	t1->registerEdges ();
	t0->registerEdges ();
	t2->registerEdges ();
	t3->registerEdges ();

	flipEdge (e0);
	flipEdge (e1);
	flipEdge (e2);
	flipEdge (e3);
}

Vertex* TriMesh::addPoint(const QPointF & p) 
{
	Vertex * v = new Vertex(p);
	verts.push_back (v);
	double d2 = 0;

	Edge* cce = closestEdge2 (p, d2); 
	if (cce) {
		if(d2 < 3) 
			splitEdge (cce, v);
		else {
			if(cce->tri[0]->containsPoint(p)) {
				splitTriangle (cce->tri[0],v);
			} else {
				splitTriangle (cce->tri[1],v);
			}

		}
		return v;
	}

	int dd2 = 0;
	int ce = closestEdge (p, dd2);
	if (dd2<3) 
		splitEdge (edges[ce], v);
	else {
		if( edges[ce]->tri[0]->containsPoint(p)) {
			splitTriangle (edges[ce]->tri[0],v);
		} else {
			splitTriangle (edges[ce]->tri[1],v);
		}

	}
	return v;
};


Tri* TriMesh::addTriangle(const Tri & p)
{
	Tri * t = new Tri (p);
	tri.push_back(t);
	return t;
}

Edge* TriMesh::addEdge (const Edge& e) {
	Edge * ne = new Edge (e);
	edges.push_back (ne);
	return ne;
}


TriMesh::~TriMesh () 
{
	for(int i=0;i< tri.size(); ++i) delete tri[i];
	for(int i=0;i< verts.size(); ++i) delete verts[i];
	for(int i=0;i< edges.size(); ++i) delete edges[i];
}

TriMesh::TriMesh ()
{
	// start with a huge rectangle

	// 0     1
	// +--0--+
	// |    /| 
	// |   / | 
	// 3  4  1 
	// | /   | 
	// |/    | 
	// +--2--+ 
	// 3     2
	verts.push_back(new Vertex(QPointF(0,0)));
	verts.push_back(new Vertex(QPointF(128000,0)));
	verts.push_back(new Vertex(QPointF(128000,128000)));
	verts.push_back(new Vertex(QPointF(0,128000)));

	edges.push_back(new Edge(verts[0],verts[1]));
	edges.push_back(new Edge(verts[1],verts[2]));
	edges.push_back(new Edge(verts[2],verts[3]));
	edges.push_back(new Edge(verts[3],verts[0]));

	// the first inner edge
	edges.push_back(new Edge(verts[3],verts[1]));

	// the first 2 triangles
	Tri * t0 = new Tri (verts[0], verts[1], verts[3], edges[4], edges[3], edges[0]);
	Tri * t1 = new Tri (verts[1], verts[2], verts[3], edges[2], edges[4], edges[1]);
	t0->registerEdges();
	t1->registerEdges();
	tri.push_back (t0);
	tri.push_back (t1);
};

int TriMesh::closestEdge (const QPointF & p, int&d2)
{
	int f = -1;
	d2 = 0;
	double dd2 = 0.0;
	QPointF pp;
	for (int i=4;i< edges.size(); ++i) {
		if (edges[i]->lineSeg().perpend(p,pp)) {
			double dx = p.x()-pp.x();
			double dy = p.y()-pp.y();
			double dd = (dx*dx)+(dy*dy);
			if ((f < 0) || (dd < dd2)) {
				dd2  = dd;
				f   = i;
			}
		}
	}
	d2 = sqrt(dd2);
	return f;
}

Edge* TriMesh::closestEdge2 (const QPointF & p, double &d2)
{
	Edge * f = 0;
	d2 = 0.0;
	
	QPointF pp;

	Tri* t= tri[tri.size()-1];
	Tri * nt = t->find(p);
	Tri * ot = 0;
	while (nt != t && nt != ot) {
		ot=t;
		t=nt;
		nt = t->find (p);
	};
	for (int i=0;i< 3; ++i) {			
		if (t->edge[i]->perpend(p,pp)) {
			double dx = p.x()-pp.x();
			double dy = p.y()-pp.y();
			double dd = (dx*dx)+(dy*dy);
			if ((f == 0) || (dd < d2)) {
				d2  = dd;
				f   = t->edge[i];
			}
		}
	}
	if(f) return f;

	for (int t=0;t< tri.size(); ++t) {
		if(p.x()<tri[t]->xmin) continue;
		if(p.x()>tri[t]->xmax) continue;
		if(p.y()<tri[t]->ymin) continue;
		if(p.y()>tri[t]->ymax) continue;
		for (int i=0;i< 3; ++i) {			
			if (tri[t]->edge[i]->perpend(p,pp)) {
				double dx = p.x()-pp.x();
				double dy = p.y()-pp.y();
				double dd = (dx*dx)+(dy*dy);
				if ((f == 0) || (dd < d2)) {
					d2  = dd;
					f   = tri[t]->edge[i];
				}
			}
		}
	}
	// d2 = sqrt(dd2);

	return f;
}



