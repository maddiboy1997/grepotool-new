/* ally_browser.cpp - alliance browser

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
#include "tab_allys.h"
#include "popup.h"

#define CMD_ALLYS_CHECK_ALL "AllysCheckAll"
#define CMD_ALLYS_UNCHECK_ALL "AllysUnheckAll"
#define CMD_ALLYS_SET_COLOR "AllysSetColor"
#define CMD_ALLYS_RANDOM_COLOR "AllysRandomColor"

#define COL_COLOR 0
#define COL_ALLY 1
#define COL_POINTS 2
#define COL_PLAYERS 3 
#define COL_TOWNS 4
#define COL_ATT 5 
#define COL_DEFF 6
#define COL_TOT 7

TabAllys::TabAllys ():TableTool ("Alliances", tr("Alliances"))
{
	QFrame * f= new QFrame ;
	ui.setupUi (f);
	setCentralWidget (f);
	//                 int tid, const QIcon &ic,             const QString &nam,   const QString &tp,     const QString &wt,    hidabl   movable, sortable
	addColDef (ColDef (COL_COLOR , QIcon (":/icons/ally_color.ico"), tr("Alliance color"),tr("Alliance color tt"),tr("Alliance color wt"), false, true,  true)); 
	addColDef (ColDef (COL_ALLY , QIcon (":/icons/ally.ico"),       tr("Alliance name"), tr("Alliance name tt"), tr("Alliance name wt"), false, true,  true)); 
	addColDef (ColDef (COL_POINTS , QIcon (":/icons/ally_points.ico"), tr("Alliance points"), tr("Alliance points tt"), tr("Alliance points wt"),  true, true,  true)); 
	addColDef (ColDef (COL_PLAYERS , QIcon (":/icons/player.ico"), tr("Number of players"), tr("Number of players tt"), tr("Number of players wt"),  true, true,  true)); 
	addColDef (ColDef (COL_TOWNS , QIcon (":/icons/town.ico"), tr("Number of towns"), tr("Number of towns tt"), tr("Number of towns wt"),  true, true,  true)); 
	addColDef (ColDef (COL_ATT , QIcon (":/icons/sword.ico"), tr("Off bashies"), tr("Off bashies tt"), tr("Off bashies wt"),  true, true,  true)); 
	addColDef (ColDef (COL_DEFF , QIcon (":/icons/shield.ico"), tr("Deff bashies"), tr("Deff bashies tt"), tr("Deff bashies wt"),  true, true,  true)); 
	addColDef (ColDef (COL_TOT , QIcon (":/icons/swords.ico"), tr("Total bashies"), tr("Total bashies tt"), tr("Total bashies wt"),  true, true,  true)); 


	addCommand (this , CMD_ALLYS_CHECK_ALL, tr ("Check all"), tr("Check all tt"), tr("Check all wt"),QIcon ());
	addCommand (this , CMD_ALLYS_UNCHECK_ALL, tr ("Uncheck all"), tr("Uncheck all tt"), tr("Uncheck all wt"),QIcon ());
	addCommand (this , CMD_ALLYS_SET_COLOR, tr ("Set color of all"), tr("Set color fo all tt"), tr("Set color fo all wt"),QIcon ());
	addCommand (this , CMD_ALLYS_RANDOM_COLOR, tr ("Set random colors"), tr("Set random colors tt"), tr("Set random colors wt"),QIcon ());

}



TabAllys::~TabAllys ()
{
}

void TabAllys::updateCell (int row, int col, const ColDef & d)
{
	QTableWidgetItem  * it = 0;
	WorldData & w = Cmd::worldData();
	if (d.type_id == COL_COLOR) { // color
		it = new ColorItem (w.allys()[row].color,row);
	} else if (d.type_id == COL_ALLY) { // name
		it = new TextItem (w.allys()[row].name,row,true,w.allys()[row].show);
	} else if (d.type_id == COL_POINTS) { // points
		it = new NumItem (w.allys()[row].points, row);
	} else if (d.type_id == COL_PLAYERS) { // player
		it = new NumItem (w.allys()[row].players.size(), row);
	} else if (d.type_id == COL_TOWNS) { // towns
		it = new NumItem (w.allys()[row].towns.size(), row);
	} else if (d.type_id == COL_ATT) { // off
		it = new NumItem (w.allys()[row].off, row);
	} else if (d.type_id == COL_DEFF) { // deff
		it = new NumItem (w.allys()[row].deff, row);
	} else if (d.type_id == COL_TOT) { // total
		it = new NumItem (w.allys()[row].off+w.allys()[row].deff, row);
	}
	it->setToolTip(d.tip);
	it->setWhatsThis(d.what);
	ui._tab->setItem (row, col, it);
}

QString  TabAllys::itemText         (int index, const ColDef &d)
{
	WorldData & w = Cmd::worldData();
	if (d.type_id == COL_COLOR) { // color
		return w.allys()[index].color.name();
	} else if (d.type_id == COL_ALLY) { // name
		return w.allys()[index].name;
	} else if (d.type_id == COL_POINTS) { // points
		return QString::number(w.allys()[index].points);
	} else if (d.type_id == COL_PLAYERS) { // player
		return QString::number(w.allys()[index].players.size());
	} else if (d.type_id == COL_TOWNS) { // towns
		return QString::number(w.allys()[index].towns.size());
	} else if (d.type_id == COL_ATT) { // off
		return QString::number(w.allys()[index].off);
	} else if (d.type_id == COL_DEFF) { // deff
		return QString::number(w.allys()[index].deff);
	} else if (d.type_id == COL_TOT) { // total
		return QString::number(w.allys()[index].off+w.allys()[index].deff);
	}
	return QString();
}

QString TabAllys::itemBBCode       (int index, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	if (d.type_id == COL_ALLY)
		return QString ("[ally]%1[/ally]").arg(w.allys()[index].name);
	return itemText(index,d);
}


void TabAllys::fillItemContextMenu (QMenu*m, int )
{
	fillContextMenu (m);
}


void TabAllys::fillContextMenu (QMenu*m)
{
	Cmd::get(CMD_ALLYS_CHECK_ALL)->addToMenu(m);
	Cmd::get(CMD_ALLYS_UNCHECK_ALL)->addToMenu(m);
	Cmd::get(CMD_ALLYS_SET_COLOR)->addToMenu(m);
	Cmd::get(CMD_ALLYS_RANDOM_COLOR)->addToMenu(m);
	TableTool::fillHeaderMenu (m,true);
}


void TabAllys::handleCmd (const QString& key)
{
	if (key ==  CMD_ALLYS_SET_COLOR) {
		QColor nc = ColorPopup::selectColor (QCursor::pos(), QColor(0,0,0,0), tr("Select a new color for all allys"));
		if (nc == QColor(0,0,0,0)) return;
		WorldData w = Cmd::worldData();
		for (int i=0;i< w.allys().size();++i) {
			w.allys()[i].color = nc;
		}
		itemDataChanged();
		emit updateInfMapSignal();
	} else if(key ==  CMD_ALLYS_RANDOM_COLOR) {
		WorldData w = Cmd::worldData();
		for (int i=0;i< w.allys().size();++i) {
			w.allys()[i].color = randomColor();
		}
		itemDataChanged();
		emit updateInfMapSignal();
	} else if(key ==  CMD_ALLYS_CHECK_ALL) {
		WorldData w = Cmd::worldData();
		for (int i=0;i< w.allys().size();++i) {
			w.allys()[i].show= true;
		}
		itemDataChanged();
		emit updateInfMapSignal();

	} else if(key ==  CMD_ALLYS_UNCHECK_ALL) {
		WorldData w = Cmd::worldData();
		for (int i=0;i< w.allys().size();++i) {
			w.allys()[i].show= false;
		}
		itemDataChanged();
		emit updateInfMapSignal();

	} else
		TableTool::handleCmd(key);
}

void TabAllys::updateCommands () 
{
	TableTool::updateCommands ();
}

void TabAllys::updateContent ()
{
	TableTool::updateContent ();
}


TableSetting * TabAllys::getTableSettings () 
{
	return & Cmd::worldData().allysTableSetting();
};
	
int TabAllys::rowCount ()
{
	return Cmd::worldData().allys().size();
}


QTableWidget * TabAllys::table() 
{
	return ui._tab;
}

Qt::SortOrder TabAllys::defaultSortOrder () 
{
	return Qt::DescendingOrder;
}

int TabAllys::defaultSortColumn () 
{
	return 2;
}

int  TabAllys::itemId     (int index)
{
	return Cmd::worldData().allys()[index].id;
}

void TabAllys::updateCellData   (int row, int col, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	QTableWidgetItem  * it = ui._tab->item(row,col);
	if(!it) return;
	if (d.type_id == COL_COLOR) { // color
		dynamic_cast<ColorItem*>(it)->setColor (w.allys()[itemIndex(row)].color);
	} else if (d.type_id == COL_ALLY) { // name
		it->setCheckState (w.allys()[itemIndex(row)].show?Qt::Checked:Qt::Unchecked);
	}
}

void TabAllys::setItemCheckState (int idx, bool b)
{
	WorldData & w = Cmd::worldData();
	if (w.allys()[idx].show == b) 
		return;
	w.allys()[idx].show = b;
	emit updateInfMapSignal ();
}

void TabAllys::showContextMenu     (const QPoint& pos)
{
	QMenu m;
	m.addSeparator ();
	fillContextMenu(&m);
	// TableTool::fillHeaderMenu (&m,true);
	m.exec (mapToGlobal (pos));
}

void TabAllys::showItemContextMenu (const QPoint& pos, int idx, const ColDef & d)
{
	WorldData & w = Cmd::worldData();

	Ally & al = w.allys()[idx];

	if (d.type_id == COL_COLOR) { // color
	
		QString tx = QString ("<b>%1&nbsp;<img src=':/icons/ally.png' width=16 height=16>&nbsp;%2").
			arg(tr("Select color of")).arg(al.name);

		QColor ocol = al.color;
		QColor ncol = ColorPopup::selectColor (mapToGlobal (pos), ocol, tx.arg(al.name));
		al.color = ncol;
		if (ocol == ncol) return;
		itemDataChanged ();
		emit itemDataChangedSignal (this, idx);	
		emit itemDataChangedSignal (idx);	
		return;
	}
	showContextMenu (pos);
}
