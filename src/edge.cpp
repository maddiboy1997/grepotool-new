/* edge.cpp - Triangulation

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

Edge::Edge (const Edge& o ) {
	vertex[0] = o.vertex[0];
	vertex[1] = o.vertex[1];
	tri[0] = o.tri[0]; 
	tri[1] = o.tri[1];
	len2 = o.len2;
}

Edge& Edge::operator = (const Edge& o ) {
	vertex[0] = o.vertex[0];
	vertex[1] = o.vertex[1];
	tri[0] = o.tri[0]; 
	tri[1] = o.tri[1];
	len2 = o.len2;
	return *this;
};

Edge::Edge () {
	tri[0] = 0; 
	tri[1] = 0;
	vertex[0] = 0; 
	vertex[1] = 0;
};

Edge::Edge (Vertex *p0, Vertex* p1)	{
	vertex[0]=p0;
	vertex[1]=p1;
	tri[0] = 0;
	tri[1] = 0;
	double dx = p0->point.x() - p1->point.x();
	double dy = p0->point.y() - p1->point.y();
	len2 = dx*dx + dy*dy;
};

Vertex * Edge::commonVertex (const Edge & o) {
	if (vertex[0] == o.vertex[0]) return vertex [0];
	if (vertex[0] == o.vertex[1]) return vertex [0];
	if (vertex[1] == o.vertex[1]) return vertex [1];
	if (vertex[1] == o.vertex[0]) return vertex [1];
	return 0;
};

bool Edge::containsVertex(Vertex* v) const {
	return (vertex[0] == v) || (vertex[1] == v);
};

bool Edge::containsTriangle (Tri* v) const {
	if (tri[0] == v) return true;
	if (tri[1] == v) return true;
	return false;
};

void Edge::addTri (Tri * t) {
	if (!tri[0]) tri[0] = t;
	else tri[1] = t;
}

void Edge::takeTri(Tri * t) {
	if (tri[0] == t) {
		tri[0] = tri[1];
		tri[1] = 0;
	} else 	tri[1] = 0;
};


