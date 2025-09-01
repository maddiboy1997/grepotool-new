#ifndef ___TOWNS_BROWSER_H
#define ___TOWNS_BROWSER_H
/* tab_towns.h  - Town list widget

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
#include "ui_towns.h"
#include "tabletool.h"

class TabTowns : public TableTool {
	Q_OBJECT;

public:

	TabTowns ();
	~TabTowns ();
	bool extendedSelection () const {return true;};

	TableSetting * getTableSettings ();
	QTableWidget * table();
	Qt::SortOrder defaultSortOrder ();
	int defaultSortColumn ();
	int  itemId     (int index);
	int rowCount ();
	void updateCell    (int row, int col, const ColDef & d);
	void currentTableChanged ();
	void prepareTableChange ();
	QString       itemBBCode       (int index, const ColDef &);
	QString       itemText         (int index, const ColDef &);

	int player () const {return rootIndex().value<QPoint>().x();};
	int ally () const {return rootIndex().value<QPoint>().y();};

	void fillItemContextMenu (QMenu*m, int a);
	void fillContextMenu (QMenu*m);

	void showContextMenu     (const QPoint& pos);
	void showItemContextMenu (const QPoint& pos, int logicalIndex, const ColDef & d);
	void handleCmd (const QString& key);
	void updateCommands () ;
	void updateContent ();
	void updateCellData   (int row, int col, const ColDef & d);

public slots:
	void setAlly (int);
	void setPlayer (int);
	void setGhosts () {setPlayer (-1);};

protected:
	Ui::Towns ui;
	QVector <int> idx;
};
#endif