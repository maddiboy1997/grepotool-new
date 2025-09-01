/* tab_targets.cpp - target browser

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
#include "tab_targets.h"
#include "popup.h"

#include "special_selector.h"
#include "unit_selector.h"


#define CMD_TARGETS_GOTO_TARGET    "TargetsGotoTarget"
#define CMD_TARGETS_SEL_ALL        "TargetsSelAll"
#define CMD_TARGETS_DEL_SEL        "TargetsDelSel"
#define CMD_TARGETS_ADD_TO_PLANS   "TargetsAddPlans"
#define CMD_TARGETS_NEW_LIST       "TargetsNewList"
#define CMD_TARGETS_EDIT_LIST      "TargetsEditList"
#define CMD_TARGETS_DEL_LIST       "TargetsDelList"
#define CMD_TARGETS_COPY_TO_LIST   "TargetCopyToList"
#define CMD_TARGETS_SET_ARRIVAL    "TargetSetArrival"
#define CMD_TARGETS_SET_DEPARTURE  "TargetSetDeparture"
#define CMD_TARGETS_FIND_TARGET    "TargetFindTarget"

#define COL_TOWN 0
#define COL_OWNER 1
#define COL_ALLY 2
//#define COL_UNIT 3
//#define COL_SPECIAL 4
#define COL_TRAVEL 5
#define COL_DEPART 6
#define COL_ARRIVE 7
#define COL_ISLAND 8

TabTargets::TabTargets ():TableTool ("Targets", tr("Targets"))
{
	QFrame * f= new QFrame ;
	ui.setupUi (f);
	setCentralWidget (f);

	connect(ui._current_tab, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
	// connect (ui._current_tab, SIGNAL(editTextChanged (const QString &)), this, SLOT(setListName(const QString&)));
	connect (ui._tab, SIGNAL(itemSelectionChanged()), this,SLOT(tabSelChg()));

	new Setting (this, TARGET_CURRENT_LIST, 0);
	setRootIndexSetting(TARGET_CURRENT_LIST);

	addCommand (ui._del_sel , CMD_TARGETS_DEL_SEL,  tr ("Delete selected"), tr("Delete selected entries"), tr("Delete selected wt"),QIcon (":/icons/delete.png"));
	addCommand (this , CMD_TARGETS_SEL_ALL,  tr ("Select all"), tr("Select the entire list"), tr("Select all wt"),QIcon (":/icons/select_all.ico"));
	addCommand (ui._new_list , CMD_TARGETS_NEW_LIST,  tr ("New list"), tr("Create new and empty list"), tr("New list wt"),QIcon (":/icons/new.png"));
	addCommand (ui._del_list , CMD_TARGETS_DEL_LIST,  tr ("Delete list"), tr("Delete the current list"), tr("Delete list wt"),QIcon (":/icons/table_delete.png"));
	addCommand (ui._edit , CMD_TARGETS_EDIT_LIST,  tr ("Edit list name"), tr("Edit the name of the current list"), tr("Edit list name wt"),QIcon (":/icons/edit.png"));
	addCommand (this , CMD_TARGETS_ADD_TO_PLANS,  tr ("Add to plans"), tr("Add selected items to plans"), tr("Add to plans wt"),QIcon (":/icons/plan_add.ico"));
	addCommand (ui._dep , CMD_TARGETS_SET_DEPARTURE,  tr ("Set departure time"), tr("Set fixed departure time"), tr("Set departure time wt"),QIcon (":/icons/time_departure.ico"));
	addCommand (ui._arr , CMD_TARGETS_SET_ARRIVAL,  tr ("Set arrival time"), tr("Set fixed arrival time"), tr("Set arrival time wt"),QIcon (":/icons/time_arrival.ico"));
	// addCommand (ui._find , CMD_TARGETS_FIND_TARGET,  tr ("Find target"), tr("Select a target town"), tr("Find target wt"),QIcon (":/icons/town_question.ico"));

	// ui._goto->setIcon("")
	ui._goto->hide();
	ui._find->hide();
	// addCommand (ui._goto, CMD_TARGETS_GOTO_TARGET,  tr ("Go to target"), tr("Scroll to target town"), tr("Go to target wt"),QIcon (":/icons/town_goto.ico"));

	//                 id,  icon ,                         name,   tip,     what,  hidable,  movable,  sortable
	addColDef (ColDef (COL_TOWN , QIcon (":/icons/town.ico"), tr("Town"), tr("Town tt"), tr("Town wt"), false, true,  true)); 
	addColDef (ColDef (COL_OWNER , QIcon (":/icons/player.ico"), tr("Owner"), tr("Owner tt"), tr("Player name wt"), true , true,  true)); 
	addColDef (ColDef (COL_ALLY , QIcon (":/icons/ally.ico"), tr("Ally"), tr("Town ally tt"), tr("Town ally wt"),  true, true,  true)); 
	//addColDef (ColDef (COL_UNIT , QIcon (":/icons/unit.ico"), tr("Slowest unit"),tr("Slowest unit tt"),tr("Slowest unit wt"), false, true,  true)); 
	//addColDef (ColDef (COL_SPECIAL , QIcon (":/icons/town_question.ico"), tr("Town settings"),tr("Town settings tt"),tr("Town settings wt"), false, true,  true)); 
	addColDef (ColDef (COL_TRAVEL , QIcon (":/icons/time_travel.ico"), tr("Travel time"),tr("Travel time tt"),tr("Travel time wt"), false, true,  true)); 
	addColDef (ColDef (COL_DEPART , QIcon (":/icons/time_departure.ico"), tr("Start time"),tr("Start time tt"),tr("Start time wt"), false, true,  true)); 
	addColDef (ColDef (COL_ARRIVE , QIcon (":/icons/time_arrival.ico"), tr("Arrival time"),tr("Arrivxal time tt"),tr("Arrival time wt"), false, true,  true)); 
}

void TabTargets::fillItemContextMenu (QMenu*m, int )
{
	fillContextMenu (m);
}

void TabTargets::fillContextMenu (QMenu*m)
{
	TableTool::fillHeaderMenu(m);
}
	

void TabTargets::showContextMenu     (const QPoint& pos)
{
	if (ui._current_tab->underMouse()) {
		editListName(true);
		return;
	}
	QMenu m;

	Cmd::get(CMD_TARGETS_SET_DEPARTURE)->addToMenu (&m);
	Cmd::get(CMD_TARGETS_SET_ARRIVAL)->addToMenu (&m);

	m.addSeparator();
	Cmd::get(CMD_TARGETS_SEL_ALL)->addToMenu (&m);
	Cmd::get(CMD_TARGETS_DEL_SEL)->addToMenu (&m);
	m.addSeparator();
	Cmd::get(CMD_TARGETS_NEW_LIST)->addToMenu (&m);
	Cmd::get(CMD_TARGETS_EDIT_LIST)->addToMenu (&m);
	Cmd::get(CMD_TARGETS_DEL_LIST)->addToMenu (&m);
	m.addSeparator();
	Cmd::get(CMD_TARGETS_ADD_TO_PLANS)->addToMenu (&m);
	m.addSeparator ();
	//QMenu * sm = m.addMenu(tr("Tab headers"));
	TableTool::fillHeaderMenu (&m,true);
	m.exec (mapToGlobal (pos));
}

bool TabTargets::editItemData     (int , const ColDef & )
{
	return false;
}

void TabTargets::tabSelChg ()
{
	QTableWidgetItem * it = ui._tab->currentItem ();
	if(!it) return;
	int idx = itemIndex(it->row());
	// UnitIndex u = currentList().l[idx].slowest_unit;
	WorldData & w = Cmd::worldData();
	int ti = w.townIndex(currentList().l[idx].town_index);
	emit currentSelectionChangedSignal (ti);
}




void TabTargets::showItemContextMenu (const QPoint& pos, int , const ColDef & )
{
	showContextMenu (pos);
}
void TabTargets::handleCmd (const QString& key)
{
	TargetList & tl = currentList();
	WorldData & w = Cmd::worldData();

	if (key == CMD_TARGETS_SEL_ALL) {
		ui._tab->selectAll();
	} else if (key == CMD_TARGETS_DEL_SEL) {

		delSel ();

	} else if (key == CMD_TARGETS_ADD_TO_PLANS) {


	} else if (key == CMD_TARGETS_NEW_LIST) {
		int ci = w.targets().size();
		w.targets().push_back (TargetList ("New List"));
		setRootIndex(ci);

	} else if (key == CMD_TARGETS_EDIT_LIST) {

		editListName (false);


	} else if (key == CMD_TARGETS_DEL_LIST) {
		if(w.targets().size() <=1) return;

		if (!DialogPopup::show(QCursor::pos(), tr("<b>Do you really want to<br>delete the current list?</b>"),tr("Confirmation"),true,true)) 
			return;

		int ci = rootIndex().toInt();
		w.targets().erase(w.targets().begin()+ci);
		setRootIndex(ci-1);
	} else if (key == CMD_TARGETS_SET_ARRIVAL) {

		QDateTime t = QDateTime::fromTime_t(tl.time);
		TimePopup::selectTime (QCursor::pos(), t, tr("Select a fixed arrival time"));
		tl.time = t.toTime_t ();
		tl.arrival = true;
		updateTable ();

	} else if (key == CMD_TARGETS_SET_DEPARTURE) {
		QDateTime t = QDateTime::fromTime_t(tl.time);
		TimePopup::selectTime (QCursor::pos(), t, tr("Select a fixed arrival time"));
		tl.arrival = false;
		tl.time = t.toTime_t ();
		updateTable ();


	} else {
		TableTool::handleCmd  (key);
	}
}
void TabTargets::updateCommands () 
{
	TableTool::updateCommands ();
}
void TabTargets::updateContent ()
{
	WorldData & w = Cmd::worldData();
	TargetList & tl = currentList();

	ui._current_tab->blockSignals(true);
	ui._current_tab->clear();
	for(int i=0;i<w.targets().size();++i) 
		ui._current_tab->addItem(w.targets()[i].name);
	ui._current_tab->setCurrentIndex(rootIndex().toInt());
	ui._current_tab->blockSignals(false);

	QDateTime ti = QDateTime::fromTime_t (tl.time);
	int tgt  = tl.town;
	QString tx = tr("");
	if (tgt >= 0 && tgt < w.towns().size()) {
		QString tn = w.towns()[tgt].name;
		QString uf = WorldData::unitIconFile(tl.unit);
		QString un = WorldData::unitName(tl.unit);
		// icon runtimes from town
		tx= QString ("<img src='%1' width=16 height=16><b>%2&nbsp;%3&nbsp;%4</b>").arg(uf).arg(un).arg(tr("travel times from")).arg(tn);
	}
	ui._info->setText(tx);
	TableTool::updateContent ();
}

void TabTargets::comboChanged (int v)
{
	setRootIndex(v);
}

void TabTargets:: editListName(bool on)
{
	TargetList & tl = currentList();
	QPoint po = mapToGlobal (ui._current_tab->pos());
	QString ti;
	if (!on) {
		po = QCursor::pos();
		ti = tr("Edit list name");
	}
	QString nln = TextPopup::selectText (po, tl.name,ti);		
	if (!nln.length()) return;		
	tl.name = nln;	
	ui._current_tab ->setItemText(rootIndex().toInt (), nln);
}



void TabTargets::updateCell (int row, int col, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	
	int ct  = rootIndex().toInt ();
	if (ct > w.targets().size()) return;
	const TargetList & tl = w.targets() [ct];
	const Target &t= tl.l[row];
	int ti = w.townIndex(t.town_index);

	QString tn;
	QString pn = tr("unknown player");
	QString an = tr("no alliance");;
	if (ti >= 0) {
		const Town & to = w.towns ()[ti];
		tn = to.name;
		if (to.ally>=0) an = w.allys()[to.ally].name;
		if (to.player>=0) pn = w.players()[to.player].name;
	}

	QString spc = w.townSpecialText (ti);

	QLabel *spcl = new QLabel; //  (spc);		
	QPixmap sp = w.townSpecialPixmap (ti);
	if (sp.width()) spcl->setPixmap(sp);
	else spcl->setText("---");

	int tt = w.calculateRuntime (tl.town, ti, tl.unit);

	QDateTime st = QDateTime::fromTime_t(tl.time);
	if (tl.arrival) {
		st =  QDateTime::fromTime_t(tl.time - tt);
	} 
	QDateTime ar = st.addSecs(tt);

	QTableWidgetItem  * it = 0;
	if (d.type_id == COL_TOWN) { // town
		it = new TextItem (tn , row, false, false);
	} else if (d.type_id == COL_OWNER) { // player
		it = new TextItem (pn, row, false, false);
	} else if (d.type_id == COL_ALLY) { // ally
		it = new TextItem (an, row, false, false);
	} else if (d.type_id == COL_TRAVEL) { // travel
		it = new TimeItem (tt,row);
	} else if (d.type_id == COL_DEPART) { // start
		it = new DateTimeItem (st,row);
	} else if (d.type_id == COL_ARRIVE) { // arrival
		it = new DateTimeItem (ar,row);
	} 
	
	if (!it) {
		return;
	}

	it->setToolTip(d.tip);
	it->setWhatsThis(d.what);
	ui._tab->setItem (row, col, it);
}

void TabTargets::updateCellData   (int row, int col, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	const TargetList & tl = currentList();
	const Target &t= tl.l[itemIndex(row)];
	QTableWidgetItem  * it = ui._tab->item(row,col);

	int ti = w.townIndex(t.town_index);

	QString tn;
	QString pn = tr("unknown player");
	QString an = tr("no alliance");;
	if (ti >= 0) {
		const Town & to = w.towns ()[ti];
		tn = to.name;
		if (to.ally>=0) an = w.allys()[to.ally].name;
		if (to.player>=0) pn = w.players()[to.player].name;
	}

	QString spc = w.townSpecialText (ti);

	int tt = w.calculateRuntime (tl.town, ti, tl.unit);

	QDateTime st = QDateTime::fromTime_t(tl.time);
	if (tl.arrival) {
		st =  QDateTime::fromTime_t(tl.time - tt);
	} 
	QDateTime ar = st.addSecs(tt);

	if (d.type_id == COL_TRAVEL) { // travel
		dynamic_cast<TimeItem *>(it)->setTime(tt);
	} else if (d.type_id == COL_DEPART) { // start
		dynamic_cast<DateTimeItem*>(it)->setDateTime (st);
	} else if (d.type_id == COL_ARRIVE) { // arrival
		dynamic_cast<DateTimeItem*>(it)->setDateTime (ar);
	} 
}

QString  TabTargets::itemBBCode (int index, const ColDef &d)
{
	WorldData & w = Cmd::worldData();
	const TargetList & tl = currentList();
	const Target &t= tl.l[index];
	int ti = w.townIndex(t.town_index);
	const Town & to = w.towns()[ti];

	QString r = itemText(index,d);
	if (d.type_id == COL_TOWN)
		return QString("[town]%1[/town]").arg(t.town_index);
	if (d.type_id == COL_OWNER)
		if(to.player>=0) 
			return QString("[player]%1[/player]").arg(r);
	if (d.type_id == COL_ALLY)
		if(to.ally>=0) 
			return QString("[ally]%1[/ally]").arg(r);
	if (d.type_id == COL_ISLAND)
		return QString("[island]%1[/island]").arg(QString::number (w.islands()[to.island].id));

	return r;
}

QString TabTargets::itemText         (int index, const ColDef &d) 
{

	WorldData & w = Cmd::worldData();
	const TargetList & tl = currentList();
	const Target &t= tl.l[index];

	int ti = w.townIndex(t.town_index);
	const Town & to = w.towns ()[ti];

	QString tn;
	QString pn = tr("unknown player");
	QString an = tr("no alliance");;
	tn = to.name;
	if (to.ally>=0) an = w.allys()[to.ally].name;
	if (to.player>=0) pn = w.players()[to.player].name;

	QString spc;// = w.townSpecialText (ti);
	if(to.research[ResearchMeteorology]) spc+= tr("Meteorology")+ " ";
	if(to.research[ResearchCartography]) spc+= tr("Cartography")+ " ";
	if(to.movement_boost) spc+= tr("Movement boost")+ " ";
	if(to.buildings[Lighthouse]) spc+=tr("Lighthouse")+ " ";


	int tt = w.calculateRuntime (tl.town, ti, tl.unit);

	int l= tt;
	int secs = l%60;
	l/=60;
	int mins = l%60;
	l/=60;
	int hours = l%60;
	QString ttx =QString("%1:%2:%3").arg(hours).arg(mins,2,10,QChar('0')).arg(secs,2,10,QChar('0'));

	QDateTime st = QDateTime::fromTime_t(tl.time);
	if (tl.arrival) {
		st =  QDateTime::fromTime_t(tl.time - tt);
	} 
	QDateTime ar = st.addSecs(tt);

	Qt::DateFormat df = Qt::DefaultLocaleShortDate;

	if (d.type_id == COL_TOWN) { // town
		return tn ;
	} else if (d.type_id == COL_OWNER) { // player
		return pn;
	} else if (d.type_id == COL_ALLY) { // ally
		return an;
	} else if (d.type_id == COL_TRAVEL) { // travel
		return ttx;
	} else if (d.type_id == COL_DEPART) { // start
		return st.toString(df);
	} else if (d.type_id == COL_ARRIVE) { // arrival
		return ar.toString(df);
	} 
	return QString();
}



TargetList & TabTargets::currentList ()
{
	WorldData & w = Cmd::worldData();	
	int ct  = rootIndex().toInt ();
	if (ct >= w.targets().size()) {
		ct = w.targets().size();
		w.targets().push_back (TargetList(tr("New list")));
	}
	return w.targets()[ct];
}


void TabTargets::townInfoChanged (int )
{

	itemDataChanged ();
}

void TabTargets::setTargetTown (int t, UnitIndex u)
{
	TargetList & l = currentList();

	l.town = t;
	l.unit= u;

	updateContent();

	itemDataChanged ();
}

void TabTargets::editDateTime  ()
{
}

TabTargets::~TabTargets ()
{
}

TableSetting * TabTargets::getTableSettings () 
{
	return & currentList().set;
};
	
int TabTargets::rowCount ()
{
	return currentList().l.size();	
}

void TabTargets::addTown (int to)
{
	if(to<0) return;
	WorldData & w = Cmd::worldData();
	TargetList & l = currentList();
	int ti = w.towns()[to].id;
	int selr = l.l.size();
	l.l.push_back (Target (ti));
	ui._tab->clearSelection();
	updateTable ();
	selectIndex (selr);
}

void TabTargets::copyToNewList ()
{
}
void TabTargets::delSel ()
{
	TargetList & tl = currentList();
	QList <int> sel = selection();
	QVector<Target> nl;
	for (int i=0;i< tl.l.size();++i) {
		if(!sel.contains(i)) nl.push_back(tl.l[i]);
	}
	tl.l = nl;
	ui._tab->clearSelection();
	updateTable ();
}

void TabTargets::currentTableChanged ()
{
	int a = rootIndex().toInt();
	int ct = ui._current_tab->currentIndex();
	updateContent ();
	if (a != ct) updateTable ();
}


QTableWidget * TabTargets::table() 
{
	return ui._tab;
}

Qt::SortOrder TabTargets::defaultSortOrder () 
{
	return Qt::DescendingOrder;
}

int TabTargets::defaultSortColumn () 
{
	return 2;
}

int  TabTargets::itemId     ( int index)
{
	return index ;
}


