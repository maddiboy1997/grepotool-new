/* mesh_thread.cpp - Triangulation

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
#include "command.h"
#include "world_data.h"

MeshThread::MeshThread (QObject * p): QThread (p),_restart(false),_abort(false),_update_all(true)
{
}

MeshThread::~MeshThread ()
{
	_m.lock();
    _abort = true;
    _condition.wakeOne();
    _m.unlock();
    wait();
}

void MeshThread::createMesh ()
{
	QMutexLocker ml(&_m);
	
	WorldData d = Cmd::worldData();
	int na = d.allys().size();
	// _update_all = false;
	int idx= 0;
	int os = _cols.size();
	for (int i=0;i<na;++i) {
		const Ally& al = d.allys()[i];
		if (!al.show) continue;
		for (int j=0;j<al.towns.size();++j) {
			if (idx>=os) {
				_cols.push_back(al.color);
				_pos.push_back(d.towns()[al.towns[j]].pos());
				_update_all = true;
			} else {
				QPoint po = d.towns()[al.towns[j]].pos();
				if(po != _pos[idx]) _update_all = true;
				_pos [idx] = po;
				_cols[idx] = al.color;
			}
			++idx;
		}
	}
	if (idx!= os) {
		 _update_all = true;
		 _pos.resize(idx);
		 _cols.resize(idx);
	}
	if (!isRunning()) {
		start (LowPriority);
	} else {
		_restart = true;
		_condition.wakeOne();
	}
};


void MeshThread::run()
{
	forever {
		if (_abort) return;

		// copy data
		_m.lock();
		QVector<QColor> cols (_cols);
		QVector<QPoint> pos ( _pos);
		bool update_all = _update_all;
		_m.unlock ();

		if(update_all) {
		// create mesh
			TriMesh m;

			int prog = 0;
			emit progress(0);
			int ps = pos.size();
			for (int i=0; i < ps; ++i) {
				if (_abort) return;
				if (_restart) break;
				int nprog = i*1000 / ps;
				if(nprog != prog) {
					prog=nprog;
					emit progress (prog);
				}
				m.addPoint(pos[i]);
			}
			QVector <QPolygonF > pols = m.delaunayPolygons();

			_m.lock();
			
			emit progress(1000);
			if (!_restart) {
				_update_all = false;
				emit meshCreated (pols, pos, cols);
			}
		} else {
			_m.lock();
			emit progress(1000);
			if (!_restart) 
				emit meshColorsChanged (cols);
		}
		if (!_restart)
			// wait for next wakeup
			_condition.wait(&_m);
		if (_abort) {
			_m.unlock(); 
			return;
		}
		_restart = false;
		_m.unlock(); 
	}
}


