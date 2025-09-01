/* tab_towns.cpp - alliance browser

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
#include "tab_towns.h"
#include "popup.h"
#include "mainwin.h"

#define CMD_TOWNS_SHOW_GHOSTS  "TownsShowGhosts"
#define CMD_TOWNS_GOTO_TOWN    "TownsGotoTown"
#define CMD_TOWNS_GOTO_ISLAND   "TownsGotoIsland"
#define CMD_TOWNS_FIND_TOWN    "TownsFindTown"
#define CMD_TOWNS_FIND_PLAYER  "TownsFindPlayer"
#define CMD_TOWNS_SEL_ALL      "TownsSelAll"
#define CMD_TOWNS_ADD_TO_MYTOWNS "TownsAddMytowns"
#define CMD_TOWNS_ADD_TO_TARGETS "TownsAddTargets"


#define COL_TOWN 0
#define COL_PLAYER 1
#define COL_POINTS 2
#define COL_ALLY  3
#define COL_ISLAND 4

#define COL_OCEAN 5
#define COL_DIST_CENTER 6
#define COL_TYPE 7
#define COL_SPECIAL 8
#define COL_UNITS 9


TabTowns::TabTowns ():TableTool ("Towns", tr("Towns"))
{
	QFrame * f= new QFrame ;
	ui.setupUi (f);
	setCentralWidget (f);

	new Setting (this, TOWN_TAB_CURRENT_INDEX, QPoint (-1,-1));
	setRootIndexSetting (TOWN_TAB_CURRENT_INDEX);

	addCommand (ui._ghosts, CMD_TOWNS_SHOW_GHOSTS,    tr ("Show ghost towns"), tr("Show ghost towns"), tr("Show ghost towns wt"),QIcon (":/icons/ghost.ico"));
	addCommand (this ,      CMD_TOWNS_GOTO_TOWN,      tr ("Go to town"), tr("Scroll the map to this town"), tr("Go to town wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_GOTO_ISLAND,    tr ("Go to island"), tr("Scroll the map to the island"), tr("Go to island wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_FIND_TOWN,      tr ("Find town"), tr("Find a certain town"), tr("Find town wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_FIND_PLAYER,    tr ("Find player"), tr("Find a certain player"), tr("Find player wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_SEL_ALL,        tr ("Select all"), tr("Select the entire list"), tr("Select all wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_ADD_TO_MYTOWNS, tr ("Add to My Towns"), tr("Add the current selection to the current My Towns list"), tr("Add to My Towns wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_ADD_TO_TARGETS, tr ("Add to Targets"), tr("Add the current selection to the current Targets list tt"), tr("Add to Targets wt"),QIcon ());

	addColDef (ColDef (COL_TOWN , QIcon (":/icons/town.ico"),   tr("Town"), tr("Town tt"), tr("Town wt"), false, true,  true)); 
	addColDef (ColDef (COL_PLAYER , QIcon (":/icons/player.ico"), tr("Owner"), tr("Owner tt"), tr("Player name wt"), false, true,  true)); 
	addColDef (ColDef (COL_POINTS , QIcon (":/icons/points.ico"), tr("Points"), tr("Points tt"), tr("Points wt"),  true, true,  true)); 
	addColDef (ColDef (COL_ALLY , QIcon (":/icons/ally.ico"), tr("Ally"), tr("Player ally tt"), tr("Player ally wt"),  true, true,  true)); 
	addColDef (ColDef (COL_ISLAND , QIcon (":/icons/island.ico"), tr("Island"), tr("Island id"), tr("Island wt"),  true, true,  true)); 
	
    addColDef (ColDef (COL_OCEAN , QIcon (":/icons/coord_goto.ico"), tr("Ocean"), tr("Ocean number id"), tr("Ocean wt"),  true, true,  true)); 
    //addColDef (ColDef (COL_DIST_CENTER , QIcon (":/icons/coord_goto2.ico"), tr("Distance"),tr("Distance to map center tt"),tr("Distance to map center wt"), true, true,  true)); 
   // addColDef (ColDef (COL_TYPE , QIcon (":/icons/town_question.ico"), tr("Tactical type"),tr("Tactical type tt"),tr("Tactical type wt"), true, true,  true)); 
    //addColDef (ColDef (COL_SPECIAL, QIcon (":/icons/town_question.ico"), tr("Town settings"),tr("Town settings tt"),tr("Town settings wt"), true, true,  true)); 
    //addColDef (ColDef (COL_UNITS , QIcon (":/icons/helmet.ico"), tr("Units"), tr("Units in this town"), tr("Units wt"),  true, true,  true)); 


}

void TabTowns::handleCmd (const QString& key)
{
	WorldData & w = Cmd::worldData();

	if (key == CMD_TOWNS_SHOW_GHOSTS) {
		setGhosts();
	} else if (key == CMD_TOWNS_GOTO_TOWN) {
	} else if (key == CMD_TOWNS_GOTO_ISLAND) {
	} else if (key == CMD_TOWNS_FIND_TOWN) {

			

	} else if (key == CMD_TOWNS_FIND_PLAYER) {
	} else if (key == CMD_TOWNS_SEL_ALL) {
		ui._tab->selectAll();
	} else if (key == CMD_TOWNS_ADD_TO_MYTOWNS) {
		
		int cl = Setting::value(MYTOWN_CURRENT_LIST).toInt();
		MyTownList & tl = w.mytowns()[cl];
		QList <int> sel = selection ();
		for(int i=0;i<sel.size(); ++i) {
			tl.l.push_back(MyTown(w.towns()[sel[i]].id,Attackship));
		}
		emit myTownChangedSignal (cl);

	} else if (key == CMD_TOWNS_ADD_TO_TARGETS) {
		int cl = Setting::value(TARGET_CURRENT_LIST).toInt();
		TargetList & tl = w.targets()[cl];
		QList <int> sel = selection ();
		for(int i=0;i<sel.size(); ++i) {
			tl.l.push_back(Target(w.towns()[sel[i]].id));
		}
		emit targetChangedSignal (cl);

	} else {
		TableTool::handleCmd(key);
	}
}

void TabTowns::updateCommands ()
{
	TableTool::updateCommands ();
}

void TabTowns::updateContent ()
{
	TableTool::updateContent ();
}


QString TabTowns::itemBBCode       (int index, const ColDef &d)
{
	WorldData & w = Cmd::worldData();
	QString r= itemText(index,d);
	const Town & to = w.towns()[index];
	if (d.type_id == COL_TOWN)
		return QString("[town]%1[/town]").arg(to.id);
	if (d.type_id == COL_PLAYER)
		if(to.player>=0) 
			return QString("[player]%1[/player]").arg(r);
	if (d.type_id == COL_ALLY)
		if(to.ally>=0) 
			return QString("[ally]%1[/ally]").arg(r);
	if (d.type_id == COL_ISLAND)
		return QString("[island]%1[/island]").arg(QString::number (w.islands()[to.island].id));
	return r;
}

QString  TabTowns::itemText         (int index, const ColDef &d)
{
	WorldData & w = Cmd::worldData();
	const Town & to = w.towns()[index];
	QString pn = tr("no owner");
	QString an = tr("no ally");
	int pli = to.player;
	int ali = -1;

	if (pli>= 0) {
		ali = w.players() [pli].ally;
		pn =  w.players() [pli].name;
	}
	if (ali>=0) {
		an = w.allys()[ali].name;
	}

	if (d.type_id == COL_TOWN) { // town
		return to.name;	
	} else if (d.type_id == COL_PLAYER) { // player
		return pn;
	} else if (d.type_id == COL_POINTS) { // points
		return QString::number (to.points);
	} else if (d.type_id == COL_ALLY) { // ally
		return an;
	} else if (d.type_id == COL_ISLAND) { // island
		return QString::number (w.islands()[to.island].id);
	} else if (d.type_id == COL_OCEAN) {
		return QString::number (w.oceanNumberOfIsland(to.island));	
	} else if (d.type_id == COL_DIST_CENTER) {
		QPoint pc = MainWin::currentMapOffset() - to.pos();
		double dx = pc.x();
		double dy = pc.x();
		double d = sqrt(dx*dx+dy*dy);
		return QString::number (d/128);
	} else if (d.type_id == COL_TYPE) {
		return "-";
	} else if (d.type_id == COL_SPECIAL) {
		return w.townSpecialText (index);
	} else if (d.type_id == COL_UNITS) {
		return w.townUnitsText (index,true);
	}

	return QString();
}	

/*	} else if (d.type_id == COL_DIST_CENTER) {
		double nd = d /= 12.8;
		nd = int (nd);
		nd/=10;
		QString iltx = tr("%1 map units").arg (nd);
		it = new NumTextItem (d,iltx,idx[row]);
	} else if (d.type_id == COL_TYPE) {

		it = new TextItem ("---", idx[row]);
	} else if (d.type_id == COL_SPECIAL) {
		QString spc = w.townSpecialText (idx[row]);
		QLabel *spcl = new QLabel; //  (spc);		
		QPixmap sp = w.townSpecialPixmap (idx[row]);
		if (sp.width()) spcl->setPixmap(sp);
		else spcl->setText("---");
		ui._tab->setCellWidget (row,col,spcl);		

	} else if (d.type_id == COL_UNITS) {
		QLabel *spcl = new QLabel; //  (spc);		
		QPixmap sp = w.townUnitsPixmap (idx[row]);
		if (sp.width()) spcl->setPixmap(sp);
		else spcl->setText("---");
		ui._tab->setCellWidget (row,col,spcl);		
*/


void TabTowns::fillItemContextMenu (QMenu*m, int )
{
	fillContextMenu (m);
}
void TabTowns::fillContextMenu (QMenu*m)
{
	Cmd::get(CMD_TOWNS_SHOW_GHOSTS)->addToMenu (m);
	TableTool::fillHeaderMenu (m);
}



void TabTowns::showContextMenu     (const QPoint& pos)
{
	QMenu m;
	Cmd::get(CMD_TOWNS_SHOW_GHOSTS)->addToMenu (&m);
	Cmd::get(CMD_TOWNS_ADD_TO_MYTOWNS)->addToMenu (&m);
	Cmd::get(CMD_TOWNS_ADD_TO_TARGETS)->addToMenu (&m);
	m.addSeparator ();
	TableTool::fillHeaderMenu (&m,true);
	m.exec (mapToGlobal (pos));
}

void TabTowns::showItemContextMenu (const QPoint& pos, int idx, const ColDef & d)
{
	showContextMenu (pos);
}

TabTowns::~TabTowns ()
{
}

TableSetting * TabTowns::getTableSettings () 
{
	return & Cmd::worldData().townsTableSetting();
};
	
int TabTowns::rowCount ()
{
	return idx.size();
}

void TabTowns::currentTableChanged ()
{
	WorldData & w = Cmd::worldData();
	int a = ally ();
	int p = player ();
	if(a>=w.allys().size()) a=-1;
	if(p>=w.players().size()) p=-1;
	if (a >= 0) {
		idx = w.allys()[a].towns;
		QString an = w.allys()[a].name;
		ui._info->setText(QString ("<b>") +
			tr("Towns of") +
			"&nbsp;<img src=':/icons/ally.png' width=16 height=16>&nbsp;" 
			+ an + "</b>");
	} else if ( p>= 0) {
		idx = w.players()[p].towns;
		QString an = w.players()[p].name;
		ui._info->setText(QString ("<b>") +
			tr("Towns of") +
			"&nbsp;<img src=':/icons/player.png' width=16 height=16>&nbsp;" 
			+ an + "</b>");
	} else {
		idx.clear ();
		for(int i=0;i < w.towns().size();++i)
			if(w.towns()[i].player <0 ) 
				idx.push_back (i);
		ui._info->setText(QString ("<b><img src=':/icons/ghost.png' width=16 height=16>&nbsp;%1</b>").arg (tr("Ghost towns")));
	}

}

void TabTowns::prepareTableChange ()
{
}

void TabTowns::setAlly (int a)
{
	setRootIndex (QPoint (-1,a ));

}

void TabTowns::setPlayer (int p)
{
	setRootIndex (QPoint (p, -1));
}

QTableWidget * TabTowns::table() 
{
	return ui._tab;
}

Qt::SortOrder TabTowns::defaultSortOrder () 
{
	return Qt::DescendingOrder;
}

int TabTowns::defaultSortColumn () 
{
	return 2;
}

int  TabTowns::itemId     (int index)
{
	WorldData & w = Cmd::worldData();
	
	return w.allys()[index].id;
}

void TabTowns::updateCell (int row, int col, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	
	QTableWidgetItem  * it= 0;
	int ti = idx[row];
	const Town & to = w.towns() [ti];
	int pli = to.player;
	int ali = -1;
	QString pn = tr("no owner");
	QString an = tr("no ally");
	if (pli>= 0) {
		ali = w.players() [pli].ally;
		pn =  w.players() [pli].name;
	}
	if (ali>=0) {
		an = w.allys()[ali].name;
	}

	TableItem * tabi = dynamic_cast<TableItem *> ( ui._tab->item(row,col));
	if (tabi) tabi->index = idx[row];

	if (d.type_id == COL_TOWN) { // town
		if (tabi) {
			tabi->setText (to.name);
		} else {
			it = new TextItem (to.name, idx[row], false, false);
		}
	} else if (d.type_id == COL_PLAYER) { // player
		if (tabi)
			tabi->setText (pn);
		else
			it = new TextItem (pn, idx[row],false,false);
	} else if (d.type_id == COL_POINTS) { // points
		if (tabi) {
			dynamic_cast<NumItem*> (tabi)->n = to.points;
			tabi->setText (QString::number(to.points));
		} else
			it = new NumItem (to.points, idx[row]);
	} else if (d.type_id == COL_ALLY) { // ally
		if (tabi)
			tabi->setText (an);
		else
			it = new TextItem (an, idx[row],false,false);
	} else if (d.type_id == COL_ISLAND) { // island
		int ilid = w.islands()[to.island].id;
		if (tabi) {
			tabi->setText (QString::number (ilid));
			dynamic_cast<NumItem*> (tabi)->n = ilid;
		}else
			it = new NumItem (ilid,idx[row]);

	} else if (d.type_id == COL_OCEAN) {
		QPoint po = w.islandCoordinates (to.island);
		int onu = w.oceanNumberOfIsland(to.island);
		QString iltx = QString("M %1 (%2,%3)").arg (onu).arg (po.x()/128).arg(po.y()/128);
		if (tabi) {
			tabi->setText (iltx);
			dynamic_cast<NumTextItem*> (tabi)->n = onu;
		}else
			it = new NumTextItem (onu,iltx,idx[row]);
	} else if (d.type_id == COL_DIST_CENTER) {
		QPoint pc = MainWin::currentMapOffset() - to.pos();
		double dx = pc.x();
		double dy = pc.x();
		double d = sqrt(dx*dx+dy*dy);
		double nd = d /= 12.8;
		nd = int (nd);
		nd/=10;
		QString iltx = tr("%1 map units").arg (nd);
		if (tabi) {
			tabi->setText (iltx);
			dynamic_cast<NumTextItem*> (tabi)->n = d;
		}else
			it = new NumTextItem (d,iltx,idx[row]);
	} else if (d.type_id == COL_TYPE) {

		it = new TextItem ("---", idx[row],false,false);
	} else if (d.type_id == COL_SPECIAL) {
		QString spc = w.townSpecialText (idx[row]);
		QLabel *spcl = dynamic_cast<QLabel*> (ui._tab->cellWidget (row,col));
		if(! spcl) {
			spcl = new QLabel; //  (spc);		
			ui._tab->setCellWidget (row,col,spcl);		
		}
		QPixmap sp = w.townSpecialPixmap (idx[row]);
		spcl->setPixmap(sp);

		if (!sp.width()) spcl->setText("---");
		else spcl->setText("");
		
	} else if (d.type_id == COL_UNITS) {
		QLabel *spcl = dynamic_cast<QLabel*> (ui._tab->cellWidget (row,col));
		if(!spcl) {
			spcl = new QLabel;
			ui._tab->setCellWidget (row,col,spcl);
		}
		QPixmap sp = w.townUnitsPixmap (idx[row]);
		spcl->setPixmap(sp);
		if (!sp.width()) spcl->setText("---");
		else spcl->setText("");
		spcl->setToolTip(w.townUnitsText (idx[row]));
		
	}
	if(!it) return;
	it->setToolTip(d.tip);
	it->setWhatsThis(d.what);
	ui._tab->setItem (row, col, it);
}

void TabTowns::updateCellData   (int row, int col, const ColDef & d)
{
	
	WorldData & w = Cmd::worldData();
	int ti = itemIndex(row);
	const Town & to = w.towns() [ti];

	if (d.type_id == COL_DIST_CENTER) {
		QPoint pc = MainWin::currentMapOffset() - to.pos();
		double dx = pc.x();
		double dy = pc.x();
		double d = sqrt(dx*dx+dy*dy);
		double nd = d /= 12.8;
		nd = int (nd);
		nd/=10;
		QString iltx = tr("%1 map units").arg (nd);
		dynamic_cast<NumTextItem*> (ui._tab->item(row,col))->n = d;
		dynamic_cast<NumTextItem*> (ui._tab->item(row,col))->setText(iltx);
	} else if (d.type_id == COL_UNITS) {
		QLabel *spcl = dynamic_cast<QLabel*> (ui._tab->cellWidget (row,col));
		if(!spcl) {
			spcl = new QLabel;
			ui._tab->setCellWidget (row,col,spcl);
		}
		QPixmap sp = w.townUnitsPixmap (ti);
		spcl->setPixmap(sp);
		if (!sp.width()) spcl->setText("---");
		else spcl->setText("");
		spcl->setToolTip(w.townUnitsText (ti));
		
	} else if (d.type_id == COL_SPECIAL) {
		QString spc = w.townSpecialText (idx[row]);
				QLabel *spcl = dynamic_cast<QLabel*> (ui._tab->cellWidget (row,col));
		if(!spcl) {
			spcl = new QLabel;
			ui._tab->setCellWidget (row,col,spcl);
		}

		QPixmap sp = w.townSpecialPixmap (ti);
		spcl->setPixmap(sp);
		if (!sp.width()) spcl->setText("---");
		else spcl->setText("");
		spcl->setToolTip(spc);
		
	}
	
}