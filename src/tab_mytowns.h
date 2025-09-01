#ifndef ___MYTOWN_BROWSER_H
#define ___MYTOWN_BROWSER_H
/* tab_mytowns.h  - MyTowns lists

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
#include "ui_lists.h"
#include "tabletool.h"

class TabMyTowns : public TableTool {
	Q_OBJECT;

public:

	TabMyTowns ();
	~TabMyTowns ();
	MyTownList & currentList ();
	
	bool extendedSelection () const {return true;};

	TableSetting * getTableSettings ();
	QTableWidget * table();
	Qt::SortOrder defaultSortOrder ();
	int defaultSortColumn ();
	int  itemId     (int index);
	int rowCount ();

	void updateCell    (int row, int col, const ColDef & d);
	void updateCellData   (int, int , const ColDef & );
	bool editItemData     (int, const ColDef & );

	void copyToNewList ();
	void delSel ();
	void addTown   (int to, const UnitIndex &u = Attackship);
	QString       itemBBCode       (int index, const ColDef &);
	QString       itemText         (int index, const ColDef &);

	void fillItemContextMenu (QMenu*m, int a);
	void fillContextMenu (QMenu*m);
	void showContextMenu     (const QPoint& pos);
	void showItemContextMenu (const QPoint& pos, int logicalIndex, const ColDef & d);
	void handleCmd (const QString& key);
	void updateCommands () ;
	void updateContent ();

	void currentTableChanged ();


public slots:
	void editListName(bool);
	void comboChanged (int);
	void setTargetTown (int);
	void editDateTime  ();
	void townInfoChanged (int );
	void tabSelChg ();
signals:
	void currentSelectionChangedSignal (int, UnitIndex);

protected:
	Ui::Lists ui;
};
#endif