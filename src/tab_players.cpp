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
#include "tab_players.h"
#include "popup.h"
#include "command.h"


#define CMD_SHOW_ALLYLESS "PlayersShowAllyless"
#define CMD_PLAYERS_CHECK_ALL "PlayerCheckAll"
#define CMD_PLAYERS_UNCHECK_ALL "PlayerUnheckAll"
#define CMD_PLAYERS_SET_COLOR "PlayerSetColor"
#define CMD_PLAYERS_RANDOM_COLOR "PlayerRandomColor"
#define CMD_PLAYERS_ALLY_COLOR "PlayerAllyColor"


#define COL_COLOR 0
#define COL_PLAYER 1
#define COL_POINTS 2
#define COL_ALLY 3
#define COL_TOWNS 4
#define COL_ATT 5
#define COL_DEFF 6
#define COL_TOT 7
#define COL_TOWNPOINTS 8
#define COL_POINTS_ACT 9
#define COL_ATT_ACT 10
#define COL_DEFF_ACT 11
#define COL_ACT 12

TabPlayers::TabPlayers ():TableTool ("Players", tr("Players"))
{
	QFrame * f= new QFrame ;
	ui.setupUi (f);

	new Setting (this, PLAYER_TAB_CURRENT_ALLY, -1);
	setRootIndexSetting(PLAYER_TAB_CURRENT_ALLY);

	addCommand (ui._allyless, CMD_SHOW_ALLYLESS, tr ("Show players without alliance"), tr ("Show players without alliance tt"),	tr ("Show players without alliance wt"),QIcon (":/icons/ally_no.ico"));
	addCommand (this , CMD_PLAYERS_CHECK_ALL, tr ("Check all"), tr("Check all tt"), tr("Check all wt"),QIcon ());
	addCommand (this , CMD_PLAYERS_UNCHECK_ALL, tr ("Uncheck all"), tr("Uncheck all tt"), tr("Uncheck all wt"),QIcon ());
	addCommand (this , CMD_PLAYERS_SET_COLOR, tr ("Set color of all"), tr("Set color fo all tt"), tr("Set color fo all wt"),QIcon ());
	addCommand (this , CMD_PLAYERS_RANDOM_COLOR, tr ("Set random colors"), tr("Set random colors tt"), tr("Set random colors wt"),QIcon ());
	addCommand (this , CMD_PLAYERS_ALLY_COLOR, tr ("Reset to ally color"), tr("Reset to ally color tt"), tr("Reset to ally color wt"),QIcon ());

	setCentralWidget (f);
	//                 int tid, const QIcon &ic,             const QString &nam,   const QString &tp,     const QString &wt,    hidabl   movable, sortable
	addColDef (ColDef (COL_COLOR , QIcon (":/icons/color.ico"),  tr("Player color"),tr("Player color tt"),tr("Player color wt"), false, true,  true)); 
	addColDef (ColDef (COL_PLAYER , QIcon (":/icons/player.ico"),        tr("Player name"), tr("Player name tt"), tr("Player name wt"), false, true,  true)); 
	addColDef (ColDef (COL_POINTS , QIcon (":/icons/player_points.ico"), tr("Player points"), tr("Player points tt"), tr("Player points wt"),  true, true,  true)); 
	addColDef (ColDef (COL_ALLY , QIcon (":/icons/ally.ico"), tr("Ally"), tr("Player ally tt"), tr("Player ally wt"),  true, true,  true)); 
	addColDef (ColDef (COL_TOWNS , QIcon (":/icons/town.ico"), tr("Number of towns"), tr("Number of towns tt"), tr("Number of towns wt"),  true, true,  true)); 
	addColDef (ColDef (COL_ATT , QIcon (":/icons/sword.ico"), tr("Off bashies"), tr("Off bashies tt"), tr("Off bashies wt"),  true, true,  true)); 
	addColDef (ColDef (COL_DEFF , QIcon (":/icons/shield.ico"), tr("Deff bashies"), tr("Deff bashies tt"), tr("Deff bashies wt"),  true, true,  true)); 
	addColDef (ColDef (COL_TOT , QIcon (":/icons/swords.ico"), tr("Total bashies"), tr("Total bashies tt"), tr("Total bashies wt"),  true, true,  true)); 
	addColDef (ColDef (COL_TOWNPOINTS , QIcon (":/icons/town_points.ico"), tr("Avg points per town"), tr("Avg points per town tt"), tr("Avg points per town wt"),  true, true,  true)); 
	addColDef (ColDef (COL_POINTS_ACT , QIcon (":/icons/points_activity.ico"), tr("Last points increase"), tr("Last points increase tt"), tr("Last points increase wt"),  true, true,  true)); 
	addColDef (ColDef (COL_ATT_ACT , QIcon (":/icons/sword_activity.ico"), tr("Last off activity"), tr("Last off activity tt"), tr("Last off activity wt"),  true, true,  true)); 
	addColDef (ColDef (COL_DEFF_ACT , QIcon (":/icons/shield_activity.ico"), tr("Last deff activity"), tr("Last deff activity tt"), tr("Last deff activity wt"),  true, true,  true)); 
	addColDef (ColDef (COL_ACT , QIcon (":/icons/activity.ico"), tr("Last activity"), tr("Last activity tt"), tr("Last activity wt"),  true, true,  true)); 
}

void TabPlayers::updateCell ( int row, int col, const ColDef & d)
{
	if(!idx.size()) return;

	WorldData & w = Cmd::worldData();
	
	Player & pl = w.players()[idx[row]];

	int ai = pl.ally;
	QString an = tr ("No alliance");
	if (ai >= 0) an = w.allys()[ai].name;

	QTableWidgetItem  * it = 0;
	if (d.type_id == COL_COLOR) { // color
		it = new ColorItem (pl.color,idx[row]);
	} else if (d.type_id == COL_PLAYER) { // name
		it = new TextItem (pl.name,idx[row],true,pl.show);
	} else if (d.type_id == COL_POINTS) { // points
		it = new NumItem (pl.points, idx[row]);
	} else if (d.type_id == COL_ALLY) { // ally
		it = new TextItem (an, idx[row],false,false);
	} else if (d.type_id == COL_TOWNS) { // towns
		it = new NumItem (pl.towns.size(), idx[row]);
	} else if (d.type_id == COL_ATT) { // off
		it = new NumItem (pl.off, idx[row]);
	} else if (d.type_id == COL_DEFF) { // deff
		it = new NumItem (pl.deff, idx[row]);
	} else if (d.type_id == COL_TOT) { // total
		it = new NumItem (pl.off+pl.deff, idx[row]);
	} else if (d.type_id == COL_TOWNPOINTS) { //  avg points per town
		int np = pl.towns.size();
		if(!np) np=1;
		it = new NumItem (pl.points/np, idx[row]);
	} else if (d.type_id == COL_POINTS_ACT) { // last points increase
		it = new ActTimeItem (w.stats().lastIncrease (Stats::PlayerPoints, pl.id), idx[row]);
	} else if (d.type_id == COL_ATT_ACT) { //  last off increase
		it = new ActTimeItem (w.stats().lastIncrease (Stats::PlayerOff, pl.id), idx[row]);
	} else if (d.type_id == COL_DEFF_ACT) { //  last deff increase
		it = new ActTimeItem (w.stats().lastIncrease (Stats::PlayerDeff, pl.id), idx[row]);
	} else if (d.type_id == COL_ACT) { //  last activity
		QDateTime t1=w.stats().lastIncrease (Stats::PlayerPoints, pl.id);
		QDateTime t2=w.stats().lastIncrease (Stats::PlayerOff, pl.id);
		it = new ActTimeItem ((t1>t2)?t1:t2, idx[row]);
	}
	it->setToolTip(d.tip);
	it->setWhatsThis(d.what);
	ui._tab->setItem (row, col, it);
}

void TabPlayers::updateCellData   (int row, int col, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	QTableWidgetItem  * it = ui._tab->item(row,col);
	if(!it) return;
	if (d.type_id == COL_COLOR) { // color
		dynamic_cast<ColorItem*>(it)->setColor (w.players()[itemIndex(row)].color);
	} else if (d.type_id == COL_PLAYER) { // player name
		it->setCheckState (w.players()[itemIndex(row)].show?Qt::Checked:Qt::Unchecked);
	}
}

QString TabPlayers::itemBBCode       (int index, const ColDef &d)
{
	QString r = itemText(index,d);
	if(d.type_id == COL_PLAYER) return QString("[player]%1[/player]").arg(r);
	if(d.type_id == COL_ALLY) return  QString("[ally]%1[/ally]").arg(r);
	return r;
}

QString  TabPlayers::itemText         (int index, const ColDef &d)
{
	WorldData & w = Cmd::worldData();
	const Player& pl = w.players()[index];
	QString aln = tr("no ally");
	if (pl.ally>= 0) aln = w.allys()[pl.ally].name;
	QDateTime t1=w.stats().lastIncrease (Stats::PlayerPoints, pl.id);
	QDateTime t2=w.stats().lastIncrease (Stats::PlayerOff, pl.id);
	QDateTime t3=w.stats().lastIncrease (Stats::PlayerDeff, pl.id);
	int ppt = 0; // ;
	if(pl.towns.size()) ppt = pl.points/ pl.towns.size();
	
	Qt::DateFormat df = Qt::DefaultLocaleShortDate;

	if (d.type_id == COL_COLOR) { // color
		return pl.color.name();
	} else if (d.type_id == COL_PLAYER) { // name
		return pl.name;
	} else if (d.type_id == COL_POINTS) { // points
		return QString::number (pl.points);
	} else if (d.type_id == COL_ALLY) { // ally
		return aln;
	} else if (d.type_id == COL_TOWNS) { // towns
		return  QString::number (pl.towns.size());
	} else if (d.type_id == COL_ATT) { // off
		return  QString::number (pl.off);
	} else if (d.type_id == COL_DEFF) { // deff
		return QString::number (pl.deff);
	} else if (d.type_id == COL_TOT) { // total
		return QString::number (pl.off+pl.deff);
	} else if (d.type_id == COL_TOWNPOINTS) { //  avg points per town
		return QString::number (ppt);
	} else if (d.type_id == COL_POINTS_ACT) { // last points increase
		return  t1.toString(df);
	} else if (d.type_id == COL_ATT_ACT) { //  last off increase
		return  t2.toString(df);
	} else if (d.type_id == COL_DEFF_ACT) { //  last deff increase
		return  t3.toString(df);
	} else if (d.type_id == COL_ACT) { //  last activity
		if (t1>t2) 
			return t1.toString(df);
		else
			return t2.toString(df);
	}
	return QString();
}



void TabPlayers::fillItemContextMenu (QMenu*m, int)
{
	fillContextMenu (m);
}

void TabPlayers::fillContextMenu (QMenu*m)
{
	Cmd::get(CMD_SHOW_ALLYLESS)->addToMenu (m);
	m->addSeparator ();
	Cmd::get(CMD_PLAYERS_CHECK_ALL)->addToMenu (m);
	Cmd::get(CMD_PLAYERS_UNCHECK_ALL)->addToMenu (m);
	Cmd::get(CMD_PLAYERS_SET_COLOR)->addToMenu (m);
	Cmd::get(CMD_PLAYERS_RANDOM_COLOR)->addToMenu (m);
	Cmd::get(CMD_PLAYERS_ALLY_COLOR)->addToMenu (m);
	m->addSeparator ();
	TableTool::fillHeaderMenu (m,true);
}

void TabPlayers::showContextMenu     (const QPoint& pos)
{
	QMenu m;
	fillContextMenu (&m);
	m.exec (mapToGlobal (pos));
}

void TabPlayers::showItemContextMenu (const QPoint& pos, int idx, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	Player & pl = w.players()[idx];
	if (d.type_id == COL_COLOR) { // color	
		QString tx = QString ("<b>%1&nbsp;<img src=':/icons/player.png' width=16 height=16>&nbsp;%2").
			arg(tr("Select color of")).arg(pl.name);
		QColor ocol = pl.color;
		QColor ncol = ColorPopup::selectColor (mapToGlobal (pos), ocol, tx.arg(pl.name));
		pl.color = ncol;
		if (ocol == ncol) return;
		itemDataChanged ();
		emit itemDataChangedSignal (this, idx);	
		emit updateMapSignal ();
		return;
	}
	showContextMenu (pos);
}

void TabPlayers::handleCmd (const QString& key)
{

	if (key == CMD_SHOW_ALLYLESS) {
		showPlayersWithoutAlly ();
	} else if (key == CMD_PLAYERS_CHECK_ALL) {
		WorldData w = Cmd::worldData();
		for (int i=0;i< idx.size();++i) {
			w.players()[idx[i]].show = true;
		}
		itemDataChanged();
		emit updateMapSignal();
	} else if (key == CMD_PLAYERS_UNCHECK_ALL) {
		WorldData w = Cmd::worldData();
		for (int i=0;i< idx.size();++i) {
			w.players()[idx[i]].show = false;
		}
		itemDataChanged();
		emit updateMapSignal();
	} else if (key == CMD_PLAYERS_SET_COLOR) {
		QColor nc = ColorPopup::selectColor (QCursor::pos(), QColor(0,0,0,0), tr("Select a new color for all players"));
		if (nc == QColor(0,0,0,0)) return;
		WorldData w = Cmd::worldData();
		for (int i=0;i< idx.size();++i) {
			w.players()[idx[i]].color = nc;
		}
		itemDataChanged();
		emit updateMapSignal();

	} else if (key == CMD_PLAYERS_RANDOM_COLOR) {
		WorldData w = Cmd::worldData();
		for (int i=0;i< idx.size();++i) {
			w.players()[idx[i]].color = randomColor();
		}
		itemDataChanged();
		emit updateMapSignal();
	} else if (key == CMD_PLAYERS_ALLY_COLOR) {

		int a = rootIndex().toInt();
		if(a<0) return;
		WorldData w = Cmd::worldData();
		QColor ac = w.allys()[a].color;
		for (int i=0;i< idx.size();++i) {
			w.players()[idx[i]].color = ac;
		}
		itemDataChanged();
		emit updateMapSignal();
	} else
		TableTool::handleCmd (key);

}
void TabPlayers::updateCommands () 
{
	TableTool::updateCommands ();
}
void TabPlayers::updateContent ()
{
	TableTool::updateContent ();
}


TabPlayers::~TabPlayers ()
{
}

TableSetting * TabPlayers::getTableSettings () 
{
	WorldData & w = Cmd::worldData();
	return &w.playersTableSetting();
};

void TabPlayers::setItemCheckState (int idx, bool b)
{
	WorldData & w = Cmd::worldData();
	if (w.players()[idx].show == b) 
		return;
	w.players()[idx].show = b;
	emit updateMapSignal ();
}

int TabPlayers::rowCount ()
{
	return idx.size();
}


QTableWidget * TabPlayers::table() 
{
	return ui._tab;
}

Qt::SortOrder TabPlayers::defaultSortOrder () 
{
	return Qt::DescendingOrder;
}

int TabPlayers::defaultSortColumn () 
{
	return 2;
}

void TabPlayers::currentTableChanged ()
{
	int a = rootIndex().toInt();

	WorldData & w = Cmd::worldData();
	
	if (a < 0 || a >= w.allys().size()) {
		ui._info->setText(tr("<b>Players without alliance</b>"));
		a = -1;
	} else {
		QString an = w.allys()[a].name;
		ui._info->setText(QString ("<b>") +
			tr("Members of") +
			"&nbsp;<img src=':/icons/ally.png' width=16 height=16>&nbsp;" 
			+ an + "</b>");
	}

	idx.clear();
	for (int i=0; i < w.players().size(); ++i) {
		if ( w.players()[i].ally == a) idx.push_back (i);
	}
}


int  TabPlayers::itemId     (int index)
{
	return idx[index];
}

