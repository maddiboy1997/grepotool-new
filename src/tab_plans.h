#ifndef ___PLAN_BROWSER_H
#define ___PLAN_BROWSER_H
/* tab_plans.h  - Plan list widget

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

class TabPlans : public TableTool {
	Q_OBJECT;

public:

	TabPlans ();
	~TabPlans ();
	TableSetting * getTableSettings ();
	QTableWidget * table();
	Qt::SortOrder defaultSortOrder ();
	int defaultSortColumn ();
	int  itemId     (int index);
	int rowCount ();
	void updateCell    (int row, int col, const ColDef & d);

protected:
	Ui::Lists ui;
};
#endif