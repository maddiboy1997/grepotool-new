/* tab_plans.cpp - plan browser

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
#include "explorer.h"
#include "tab_plans.h"
#include "popup.h"

TabPlans::TabPlans ():TableTool ("Plans", tr("Plans"))
{
	QFrame * f= new QFrame ;
	ui.setupUi (f);
	setCentralWidget (f);
	new Setting(this, PLAN_CURRENT_LIST,0);
}

TabPlans::~TabPlans ()
{
}

TableSetting * TabPlans::getTableSettings () 
{
	WorldData & w = Cmd::worldData();
	
	return & w.allysTableSetting();
};
	
int TabPlans::rowCount ()
{

	WorldData & w = Cmd::worldData();
	
	return w.allys().size();
}


QTableWidget * TabPlans::table() 
{
	return ui._tab;
}

Qt::SortOrder TabPlans::defaultSortOrder () 
{
	return Qt::DescendingOrder;
}

int TabPlans::defaultSortColumn () 
{
	return 2;
}

int  TabPlans::itemId     (int index)
{

	WorldData & w = Cmd::worldData();
	return w.allys()[index].id;
}

void TabPlans::updateCell (int row, int col, const ColDef & d)
{
	QTableWidgetItem  * it = 0;
	if (d.type_id == 0) { // color
	} else if (d.type_id == 1) { // name
	} else if (d.type_id == 2) { // name
	}
	it->setToolTip(d.tip);
	it->setWhatsThis(d.what);
	ui._tab->setItem (row, col, it);
}
