#ifndef __VORONOI_H
#define __VORONOI_H
/* tri_mesh.h  - delaunay - triangulation

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
#include <QtGui>

class Edge;
class Tri;

inline bool fuzzyZero (const double &d) {return ((d < 1e-10) && (d > -1e-10));};

class LineSeg {
public:
	LineSeg (const QPointF& pp0, const QPointF& pp1):p0(pp0),p1(pp1){};
	LineSeg ( const LineSeg& o):p0(o.p0),p1(o.p1) {};
	LineSeg& operator = (const LineSeg& o) {
		p0 = o.p0;
		p1 = o.p1;
		return *this;
	};
	inline QPointF center () const {return (p1+p0)/2.0;};
	inline QPointF dir () const {return p1-p0;};	
	inline double length2() const {
		QPointF l=dir();
		return (double)l.x()*(double)l.x() +(double)l.y()*(double)l.y() ;
	};
	LineSeg centerOrtho () const;
	bool intersects (const LineSeg & o, QPointF & r) const;

	bool perpend (const QPointF &p, QPointF & r) const;

	QPointF coIntersection (const LineSeg&o) const;

	QPointF p0;
	QPointF p1;
};


class Vertex {
public:
	Vertex(const QPointF &p) : point(p) {};
	Vertex(){};
	Vertex(const Vertex& o):point(o.point){};
	QPointF  point;
	Edge * e;
};

class Edge {
public:
	Edge (const Edge& o );
	Edge& operator = (const Edge& o );
	Edge ();

	inline Tri* oppositeTri(const Tri* o) {return (o==tri[0]) ? tri[1]:tri[0];};
	Edge (Vertex *p0, Vertex* p1);
	inline bool isEdge (const Vertex *p0, const Vertex* p1) const {
		return (vertex [0] == p0 && vertex [1] == p1) ||(vertex [0] == p1 && vertex [1] == p0);
	};

	inline QPointF center () const {return QPointF ( 
		((double) vertex[0]->point.x()+(double) vertex[1]->point.x()) /2.0,
		((double) vertex[0]->point.y()+(double) vertex[1]->point.y()) /2.0);
	};

	bool perpend (const QPointF &p, QPointF & r) const;
	bool intersects (const LineSeg & o, QPointF & r) const;


	Vertex * commonVertex (const Edge & o);
	bool containsVertex   (Vertex* v) const;
	bool containsTriangle (Tri* v) const;

	void addTri (Tri * t);
	void takeTri(Tri * t);

	inline bool isInnerEdge () const {return tri[1] != 0;};
	inline bool isOuterEdge () const {return tri[1] == 0;};
	inline LineSeg lineSeg () const {return LineSeg(vertex[0]->point, vertex[1]->point);};
	inline QPointF& point (int i) {return vertex[i]->point;};

	Vertex* vertex [2];
	Tri *   tri [2];
	double len2;
};

class Tri {
public:

	Tri (){};
	Tri (Vertex* p0, Vertex* p1, Vertex* p2, Edge* e0, Edge* e1, Edge* e2);

	Tri (const Tri& o){
		edge[0] = o.edge[0];
		edge[1] = o.edge[1];
		edge[2] = o.edge[2];
		vertex[0] = o.vertex[0];
		vertex[1] = o.vertex[1];
		vertex[2] = o.vertex[2];
		cog = o.cog;
		center=o.center;
		_rad2 = o._rad2;
	};


	Tri& operator = (const Tri& o){
		edge[0] = o.edge[0];
		edge[1] = o.edge[1];
		edge[2] = o.edge[2];
		vertex[0] = o.vertex[0];
		vertex[1] = o.vertex[1];
		vertex[2] = o.vertex[2];
		cog = o.cog;
		center=o.center;
		_rad2 = o._rad2;
		return *this;
	};

	Edge * nextEdge (Edge * se, Vertex * v) {
		if (se != edge[0] && edge[0]->containsVertex(v)) return edge [0];
		if (se != edge[1] && edge[1]->containsVertex(v)) return edge [1];
		return edge [2];
	};

	Tri* find (const QPointF&p);


	void  registerEdges ();
	void  removeFromEdges ();
	bool  containsPoint (const QPointF & p) const;
	bool  pointInOuterCircle (const QPointF & p) const;
	double   dist2 (const QPointF& p) const;
	int   containsEdge (const Edge *e) const;
	int   containsVertex (const Vertex *e) const;

	int vertexIndex (const Vertex *e) const ;

	Edge* getEdge (const Vertex *a,const Vertex *b) {
		if(edge[0]->isEdge(a,b) ) return edge[0];
		if(edge[0]->isEdge(a,b) ) return edge[1];
		return edge[2];
	};
	int   oppositeVertexIndex (const Edge *e) const;

	inline LineSeg lineSeg (int i) const {return edge[i]->lineSeg ();};

	Edge*   edge [3];
	Vertex* vertex [3];
	QPointF  cog;    // center of gravity
	QPointF  center; // center of circle containing all three points
	double  _rad2;   // squared radius of circle containing all three points
	double xmin;
	double xmax;
	double ymin;
	double ymax;
};

class TriMesh {
public:
	~TriMesh ();
	TriMesh ();
    Vertex *addPoint(const QPointF & p);
	QVector <QPolygonF > delaunayPolygons ();

private:
	void splitEdge (Edge * e, Vertex * v);
	void flipEdge (Edge* e) ;
	void splitTriangle (Tri* t, Vertex * v);
	void doFlipEdge (Edge* e) ;


    Tri*    addTriangle  (const Tri & p);
    Edge*   addEdge      (const Edge& e);
	int     closestEdge  (const QPointF & p, int&d2);
	Edge*   closestEdge2 (const QPointF & p, double &d2);

	QVector <Vertex*> verts;
	QVector <Edge*>   edges;
	QVector <Tri*>    tri;
};


class MeshThread : public QThread {
	Q_OBJECT;
public:
	MeshThread (QObject * p);
	~MeshThread ();
	void createMesh ();

signals:
	void meshCreated (const QVector<QPolygonF> &pol, const QVector<QPoint>& pts, const QVector<QColor>& col);
	void meshColorsChanged (const QVector<QColor>& col);
	void progress (int);

protected:
	void run();

private:
	QMutex _m;
	QWaitCondition _condition;
	bool _restart;
	bool _abort;    
	bool _update_all;
	QVector <QColor>  _cols;
	QVector <QPoint>  _pos;
};



#endif