#ifndef ___TAB_CFG_H
#define ___TAB_CFG_H
/* tab_cfg.h  - Config-Management for tables

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
#include "app.h"
#include "indexes.h"



class TableSettings {
public:
	TableSettings (){}
	QVector <bool>    _visible;
	QVector <ColType> _entries;
	Qt::SortOrder     _sort_order;
	ColType           _sort_col;
};

class TabMan : public QObject {
	Q_OBJECT;
public:
	TabMan (QTableWidget * t, const QString & sid);
	virtual ~TabMan ();

	bool columnMovable (int pos);

	
public slots:

	void worldChanged () {
		store ();
		w = App::instance->worldData ();
		restore ();
	};
	void store ();
	void restore ();
	void resetDefaults ();
	bool moveColumnLeft (int it); 
	bool moveColumnRight (int it); 

	void updateTable ();

protected:
	QVector <ColInfo> defaultColumns () const;
	QTableWidgetItem * createItem (ColType ty, int idx) const;
	int itemCount () const; 

private:
	WorldData w;
	QVector <QAction*> _show_actions;

	QString           _sub_id; // name of list in MyTowns, Plans and Targets	
	QVector <ColInfo> _cols;
	int               _sort_col;
	Qt::SortOrder     _sort_order;
	TabOwner          _owner;

};



#endif