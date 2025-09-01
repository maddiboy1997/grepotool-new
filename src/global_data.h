#ifndef ___GLOBAL_STATIC_DATA_H
#define ___GLOBAL_STATIC_DATA_H
/* global_static.h  - data handling

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

#include <QObject>
#include <QImage>
#include <QPoint>
#include "indexes.h"
#include "data.h"
#include "dataloader.h"

class ServerInfo {
public:
	QString server;
	QString name;	
};

class GlobalDataPrivate : public DataPriv {
public:
	GlobalDataPrivate (){};

	QImage           islands[61];	
	QPoint           islandCenter  (int island) const;
};

class GlobalData {
public:
	GlobalData () : _d(new GlobalDataPrivate) {_d->incRef();};
	GlobalData (const GlobalData & o):_d(const_cast <GlobalDataPrivate*> (o._d)){_d->incRef();};
	GlobalData & operator = (const GlobalData & o) {
		if(_d == o._d) return *this;
		if (_d->decRef()) delete _d;
		_d = const_cast <GlobalDataPrivate*> (o._d);
		_d->incRef();
		return *this;
	};
	QImage& island(int i) {return _d->islands[i];};
	QPoint  getIslandCenterOffset (int is) const {return _d->islandCenter(is);};
	~GlobalData () {if (_d->decRef()) delete _d;};
	void init ();
private:
	GlobalDataPrivate *_d;
};

#endif