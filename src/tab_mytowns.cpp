/* tab_mytowns.cpp -  browser

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

#include "tab_mytowns.h"
#include "explorer.h"
#include "popup.h"
#include "special_selector.h"
#include "unit_selector.h"

#define CMD_MYTOWNS_GOTO_TARGET    "MyTownsGotoTarget"
#define CMD_MYTOWNS_SEL_ALL        "MyTownsSelAll"
#define CMD_MYTOWNS_DEL_SEL        "MyTownsDelSel"
#define CMD_MYTOWNS_ADD_TO_PLANS   "MyTownsAddPlans"
#define CMD_MYTOWNS_NEW_LIST       "MyTownsNewList"
#define CMD_MYTOWNS_EDIT_LIST      "MyTownsEditList"
#define CMD_MYTOWNS_DEL_LIST       "MyTownsDelList"
#define CMD_MYTOWNS_COPY_TO_LIST   "MyTownCopyToList"
#define CMD_MYTOWNS_SET_ARRIVAL    "MyTownSetArrival"
#define CMD_MYTOWNS_SET_DEPARTURE  "MyTownSetDeparture"
#define CMD_MYTOWNS_FIND_TARGET    "MyTownFindTarget"

#define COL_TOWN 0
#define COL_OWNER 1
#define COL_ALLY 2
#define COL_UNIT 3
#define COL_SPECIAL 4
#define COL_TRAVEL 5
#define COL_DEPART 6
#define COL_ARRIVE 7
#define COL_ISLAND 8

TabMyTowns::TabMyTowns ():TableTool ("MyTowns", tr("My Towns"))
{
	QFrame * f= new QFrame ;
	ui.setupUi (f);
	setCentralWidget (f);

	connect(ui._current_tab, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
	// connect (ui._current_tab, SIGNAL(editTextChanged (const QString &)), this, SLOT(setListName(const QString&)));

	connect (ui._tab, SIGNAL(itemSelectionChanged()), this,SLOT(tabSelChg()));
	new Setting (this, MYTOWN_CURRENT_LIST, 0);
	setRootIndexSetting(MYTOWN_CURRENT_LIST);

	addCommand (ui._del_sel , CMD_MYTOWNS_DEL_SEL,  tr ("Delete selected"), tr("Delete selected entries"), tr("Delete selected wt"),QIcon (":/icons/delete.png"));
	addCommand (this , CMD_MYTOWNS_SEL_ALL,  tr ("Select all"), tr("Select the entire list"), tr("Select all wt"),QIcon (":/icons/select_all.ico"));
	addCommand (ui._new_list , CMD_MYTOWNS_NEW_LIST,  tr ("New list"), tr("Create new and empty list"), tr("New list wt"),QIcon (":/icons/new.png"));
	addCommand (ui._del_list , CMD_MYTOWNS_DEL_LIST,  tr ("Delete list"), tr("Delete the current list"), tr("Delete list wt"),QIcon (":/icons/table_delete.png"));
	addCommand (ui._edit , CMD_MYTOWNS_EDIT_LIST,  tr ("Edit list name"), tr("Edit the name of the current list"), tr("Edit list name wt"),QIcon (":/icons/edit.png"));
	addCommand (this , CMD_MYTOWNS_ADD_TO_PLANS,  tr ("Add to plans"), tr("Add selected items to plans"), tr("Add to plans wt"),QIcon (":/icons/plan_add.ico"));
	addCommand (ui._dep , CMD_MYTOWNS_SET_DEPARTURE,  tr ("Set departure time"), tr("Set fixed departure time"), tr("Set departure time wt"),QIcon (":/icons/time_departure.ico"));
	addCommand (ui._arr , CMD_MYTOWNS_SET_ARRIVAL,  tr ("Set arrival time"), tr("Set fixed arrival time"), tr("Set arrival time wt"),QIcon (":/icons/time_arrival.ico"));
	addCommand (ui._find , CMD_MYTOWNS_FIND_TARGET,  tr ("Find target"), tr("Select a target town"), tr("Find target wt"),QIcon (":/icons/town_question.ico"));
	addCommand (ui._goto, CMD_MYTOWNS_GOTO_TARGET,  tr ("Go to target"), tr("Scroll to target town"), tr("Go to target wt"),QIcon (":/icons/town_goto.ico"));

	//                 id,  icon ,                         name,   tip,     what,  hidable,  movable,  sortable
	addColDef (ColDef (COL_TOWN , QIcon (":/icons/town.ico"), tr("Town"), tr("Town tt"), tr("Town wt"), false, true,  true)); 
	addColDef (ColDef (COL_OWNER , QIcon (":/icons/player.ico"), tr("Owner"), tr("Owner tt"), tr("Player name wt"), true , true,  true)); 
	addColDef (ColDef (COL_ALLY , QIcon (":/icons/ally.ico"), tr("Ally"), tr("Town ally tt"), tr("Town ally wt"),  true, true,  true)); 
	addColDef (ColDef (COL_UNIT , QIcon (":/icons/unit.ico"), tr("Slowest unit"),tr("Slowest unit tt"),tr("Slowest unit wt"), false, true,  true)); 
	addColDef (ColDef (COL_SPECIAL , QIcon (":/icons/town_question.ico"), tr("Town settings"),tr("Town settings tt"),tr("Town settings wt"), false, true,  true)); 
	addColDef (ColDef (COL_TRAVEL , QIcon (":/icons/time_travel.ico"), tr("Travel time"),tr("Travel time tt"),tr("Travel time wt"), false, true,  true)); 
	addColDef (ColDef (COL_DEPART , QIcon (":/icons/time_departure.ico"), tr("Start time"),tr("Start time tt"),tr("Start time wt"), false, true,  true)); 
	addColDef (ColDef (COL_ARRIVE , QIcon (":/icons/time_arrival.ico"), tr("Arrival time"),tr("Arrivxal time tt"),tr("Arrival time wt"), false, true,  true)); 
}

void TabMyTowns::fillItemContextMenu (QMenu*m, int )
{
	fillContextMenu (m);
}

void TabMyTowns::fillContextMenu (QMenu*m)
{
	TableTool::fillHeaderMenu(m);
}

void TabMyTowns::tabSelChg ()
{
	QTableWidgetItem * it = ui._tab->currentItem ();
	if(!it) return;
	int idx = itemIndex(it->row());
	UnitIndex u = currentList().l[idx].slowest_unit;
	WorldData & w = Cmd::worldData();
	int ti = w.townIndex(currentList().l[idx].town_index);

	emit currentSelectionChangedSignal (ti,u);
}

void TabMyTowns::showContextMenu     (const QPoint& pos)
{
	if (ui._current_tab->underMouse()) {
		editListName(true);
		return;
	}
	QMenu m;
	Cmd::get(CMD_MYTOWNS_FIND_TARGET)->addToMenu (&m);
	Cmd::get(CMD_MYTOWNS_GOTO_TARGET)->addToMenu (&m);
	m.addSeparator();

	Cmd::get(CMD_MYTOWNS_SET_DEPARTURE)->addToMenu (&m);
	Cmd::get(CMD_MYTOWNS_SET_ARRIVAL)->addToMenu (&m);

	m.addSeparator();
	Cmd::get(CMD_MYTOWNS_SEL_ALL)->addToMenu (&m);
	Cmd::get(CMD_MYTOWNS_DEL_SEL)->addToMenu (&m);
	m.addSeparator();
	Cmd::get(CMD_MYTOWNS_NEW_LIST)->addToMenu (&m);
	Cmd::get(CMD_MYTOWNS_EDIT_LIST)->addToMenu (&m);
	Cmd::get(CMD_MYTOWNS_DEL_LIST)->addToMenu (&m);
	m.addSeparator();
	Cmd::get(CMD_MYTOWNS_ADD_TO_PLANS)->addToMenu (&m);
	m.addSeparator ();
	//QMenu * sm = m.addMenu(tr("Tab headers"));
	TableTool::fillHeaderMenu (&m,true);
	m.exec (mapToGlobal (pos));
}

bool TabMyTowns::editItemData     (int row, const ColDef & d)
{
	MyTownList & tl = currentList();
	MyTown &t= tl.l[row];

	WorldData & w = Cmd::worldData();
	int ti = w.townIndex(t.town_index);
	Town & to = w.towns() [ti];

	if (d.type_id == COL_UNIT) { // unit
		t.slowest_unit = UnitSelector::selectUnit (QCursor::pos(),t.slowest_unit);
		
	} else if (d.type_id == COL_SPECIAL) { // town specials
		int ts = 0;

		if (to.research[ResearchMeteorology]) ts += 1;
		if (to.research[ResearchCartography]) ts += 2;
		if (to.movement_boost) ts += 4;
		if (to.buildings[Lighthouse] > 0) ts += 8;

		int ns = SpecialSelector::selectSpecials (QCursor::pos(), ts);

		to.research[ResearchMeteorology] = (ns & 1) ;
		to.research[ResearchCartography] =  ((ns & 2) != 0);
		to.movement_boost =  ((ns & 4 ) != 0);
		to.buildings[Lighthouse] = ((ns & 8 )!= 0)? 1:0 ;

	} else {
		return false;
	}
	itemDataChanged ();
	return true;
}





void TabMyTowns::showItemContextMenu (const QPoint& pos, int logicalIndex, const ColDef & d)
{
	MyTownList & tl = currentList();
	MyTown &t= tl.l[logicalIndex];

	WorldData & w = Cmd::worldData();
	int ti = w.townIndex(t.town_index);
	Town & to = w.towns() [ti];

	QList <int> sel = selection();

	if (sel.size() && d.type_id == COL_UNIT) { // unit
		UnitIndex nu =  UnitSelector::selectUnit (QCursor::pos(),t.slowest_unit);
		
		for(int i=0;i<sel.size(); ++i) {
			tl.l[sel[i]].slowest_unit = nu;
		}
		itemDataChanged ();

		tabSelChg();


	} else if (sel.size() == 1 && d.type_id == COL_SPECIAL) { // town specials
		int ts = 0;

		if (to.research[ResearchMeteorology]) ts += 1;
		if (to.research[ResearchCartography]) ts += 2;
		if (to.movement_boost) ts += 4;
		if (to.buildings[Lighthouse] > 0) ts += 8;

		
		int ns = SpecialSelector::selectSpecials (QCursor::pos(), ts);

		to.research[ResearchMeteorology] = ((ns & 1) != 0);
		to.research[ResearchCartography] =  ((ns & 2) != 0);
		to.movement_boost =  ((ns & 4 ) != 0);
		to.buildings[Lighthouse] = ((ns & 8 )!= 0)? 1:0 ;

		itemDataChanged ();
		tabSelChg();

		emit townDataChangedSignal (ti);
		emit itemDataChangedSignal (this, ti);	

	} else {
		showContextMenu (pos);
	}
}
void TabMyTowns::handleCmd (const QString& key)
{
	MyTownList & tl = currentList();
	WorldData & w = Cmd::worldData();

	if (key == CMD_MYTOWNS_SEL_ALL) {
		ui._tab->selectAll();
	} else if (key == CMD_MYTOWNS_DEL_SEL) {

		delSel ();

	} else if (key == CMD_MYTOWNS_ADD_TO_PLANS) {


	} else if (key == CMD_MYTOWNS_NEW_LIST) {
		int ci = w.mytowns().size();
		w.mytowns().push_back (MyTownList ("New List"));
		setRootIndex(ci);

	} else if (key == CMD_MYTOWNS_EDIT_LIST) {

		editListName (false);


	} else if (key == CMD_MYTOWNS_DEL_LIST) {
		if(w.mytowns().size() <=1) return;

		if (!DialogPopup::show(QCursor::pos(), tr("<b>Do you really want to<br>delete the current list?</b>"),tr("Confirmation"),true,true)) 
			return;

		int ci = rootIndex().toInt();
		w.mytowns().erase(w.mytowns().begin()+ci);
		setRootIndex(ci-1);
	} else if (key == CMD_MYTOWNS_SET_ARRIVAL) {

		QDateTime t = QDateTime::fromTime_t(tl.time);
		TimePopup::selectTime (QCursor::pos(), t, tr("Select a fixed arrival time"));
		tl.time = t.toTime_t ();
		tl.arrival = true;
		updateTable ();

	} else if (key == CMD_MYTOWNS_SET_DEPARTURE) {
		QDateTime t = QDateTime::fromTime_t(tl.time);
		TimePopup::selectTime (QCursor::pos(), t, tr("Select a fixed arrival time"));
		tl.arrival = false;
		tl.time = t.toTime_t ();
		updateTable ();

	} else if (key == CMD_MYTOWNS_GOTO_TARGET) {
		if(tl.target >=0)
			emit gotoTownSignal (tl.target);

	} else if (key == CMD_MYTOWNS_FIND_TARGET) {

		QStringList l;
		for(int i=0;i< w.towns().size(); ++i) {
			QString tn = w.towns()[i].name;
			QString pn = tr ("Ghost town");
			int pi = w.towns()[i].player;
			if (pi >= 0) {
				pn = w.players()[pi].name;
			}
			l.push_back(QString ("%1 (%2)").arg(tn).arg(pn));
		}

		int nt = SelectorPopup::select(QCursor::pos(), l, tr("Select target Town"));

		if (nt >= 0) setTargetTown (nt);

	} else {
		TableTool::handleCmd  (key);
	}
}
void TabMyTowns::updateCommands () 
{
	TableTool::updateCommands ();
}
void TabMyTowns::updateContent ()
{
	WorldData & w = Cmd::worldData();
	MyTownList & tl = currentList();

	ui._current_tab->blockSignals(true);
	ui._current_tab->clear();
	for(int i=0;i<w.mytowns().size();++i) 
		ui._current_tab->addItem(w.mytowns()[i].name);
	ui._current_tab->setCurrentIndex(rootIndex().toInt());
	ui._current_tab->blockSignals(false);

	QDateTime ti = QDateTime::fromTime_t (tl.time);
	int tgt  = tl.target;
	QString tx = tr("No target");
	if (tgt >= 0) {
		QString tn = w.towns()[tgt].name;
		tx= QString ("<b>%1:&nbsp;%2</b>").arg(tr("Target")).arg(tn);
	}
	ui._info->setText(tx);
	TableTool::updateContent ();
}

void TabMyTowns::comboChanged (int v)
{
	setRootIndex(v);
}

void TabMyTowns:: editListName(bool on)
{
	MyTownList & tl = currentList();
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



void TabMyTowns::updateCell (int row, int col, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	
	int ct  = rootIndex().toInt ();
	if (ct > w.mytowns().size()) return;
	const MyTownList & tl = w.mytowns() [ct];
	const MyTown &t= tl.l[row];
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


	int tt = w.calculateRuntime (ti, tl.target, t.slowest_unit);

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
	} else if (d.type_id == COL_UNIT) { // unit
		it = new UnitItem (t.slowest_unit, row);
	} else if (d.type_id == COL_SPECIAL) { // town specials
		QString spc = w.townSpecialText (ti);
		QLabel *spcl = new QLabel; //  (spc);		
		QPixmap sp = w.townSpecialPixmap (ti);
		if (sp.width()) spcl->setPixmap(sp);
		else spcl->setText("---");

		ui._tab->setCellWidget (row,col,spcl);		
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

void TabMyTowns::updateCellData   (int row, int col, const ColDef & d)
{
	WorldData & w = Cmd::worldData();
	const MyTownList & tl = currentList();
	const MyTown &t= tl.l[itemIndex(row)];
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

	int tt = w.calculateRuntime (ti, tl.target, t.slowest_unit);

	QDateTime st = QDateTime::fromTime_t(tl.time);
	if (tl.arrival) {
		st =  QDateTime::fromTime_t(tl.time - tt);
	} 
	QDateTime ar = st.addSecs(tt);

	if (d.type_id == COL_UNIT) { // unit
		dynamic_cast<UnitItem*>(it)->setUnit(t.slowest_unit);
	} else if (d.type_id == COL_SPECIAL) { // town specials
		QLabel *spcl = new QLabel; //  (spc);		
		QPixmap sp = w.townSpecialPixmap (ti);
		if (sp.width()) spcl->setPixmap(sp);
		else spcl->setText("---");
		ui._tab->setCellWidget (row,col,spcl);		
	} else if (d.type_id == COL_TRAVEL) { // travel
		dynamic_cast<TimeItem *>(it)->setTime(tt);
	} else if (d.type_id == COL_DEPART) { // start
		dynamic_cast<DateTimeItem*>(it)->setDateTime (st);
	} else if (d.type_id == COL_ARRIVE) { // arrival
		dynamic_cast<DateTimeItem*>(it)->setDateTime (ar);
	} 
}

QString  TabMyTowns::itemBBCode (int index, const ColDef &d)
{
	WorldData & w = Cmd::worldData();
	const MyTownList & tl = currentList();
	const MyTown &t= tl.l[index];
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

QString TabMyTowns::itemText         (int index, const ColDef &d) 
{

	WorldData & w = Cmd::worldData();
	const MyTownList & tl = currentList();

	if(index<0 || index >= tl.l.size()) return "";

	const MyTown &t= tl.l[index];

	int ti = w.townIndex(t.town_index);
	if (ti<0) return "";

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


	int tt = w.calculateRuntime (ti, tl.target, t.slowest_unit);

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
	} else if (d.type_id == COL_UNIT) { // unit
		return w.unitName (t.slowest_unit);
	} else if (d.type_id == COL_SPECIAL) { // town specials
		return spc; 
	} else if (d.type_id == COL_TRAVEL) { // travel
		return ttx;
	} else if (d.type_id == COL_DEPART) { // start
		return st.toString(df);
	} else if (d.type_id == COL_ARRIVE) { // arrival
		return ar.toString(df);
	} 
	return QString();
}



MyTownList & TabMyTowns::currentList ()
{
	WorldData & w = Cmd::worldData();	
	int ct  = rootIndex().toInt ();
	if (ct >= w.mytowns().size()) {
		ct = w.mytowns().size();
		w.mytowns().push_back (MyTownList(tr("New list")));
	}
	return w.mytowns()[ct];
}


void TabMyTowns::townInfoChanged (int )
{

	itemDataChanged ();
}

void TabMyTowns::setTargetTown (int t)
{
	MyTownList & l = currentList();

	l.target = t;

	updateContent();

	itemDataChanged ();
}

void TabMyTowns::editDateTime  ()
{
}

TabMyTowns::~TabMyTowns ()
{
}

TableSetting * TabMyTowns::getTableSettings () 
{
	return & currentList().set;
};
	
int TabMyTowns::rowCount ()
{
	return currentList().l.size();	
}

void TabMyTowns::addTown (int to, const UnitIndex &u)
{
	if(to<0) return;
	WorldData & w = Cmd::worldData();
	MyTownList & l = currentList();
	int ti = w.towns()[to].id;
	int selr = l.l.size();
	l.l.push_back (MyTown (ti,u));
	ui._tab->clearSelection();
	updateTable ();
	selectIndex (selr);
}

void TabMyTowns::copyToNewList ()
{
}
void TabMyTowns::delSel ()
{
	MyTownList & tl = currentList();
	QList <int> sel = selection();
	QVector<MyTown> nl;
	for (int i=0;i< tl.l.size();++i) {
		if(!sel.contains(i)) nl.push_back(tl.l[i]);
	}
	tl.l = nl;
	ui._tab->clearSelection();
	updateTable ();
}

void TabMyTowns::currentTableChanged ()
{
	int a = rootIndex().toInt();
	int ct = ui._current_tab->currentIndex();
	updateContent ();
	if (a != ct) updateTable ();
}


QTableWidget * TabMyTowns::table() 
{
	return ui._tab;
}

Qt::SortOrder TabMyTowns::defaultSortOrder () 
{
	return Qt::DescendingOrder;
}

int TabMyTowns::defaultSortColumn () 
{
	return 2;
}

int  TabMyTowns::itemId     ( int index)
{
	return index ;
}

