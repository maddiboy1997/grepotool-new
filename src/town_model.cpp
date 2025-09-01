/* towns_model.cpp  - Town data model

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
#include "command.h"
#include "command_ids.h"
#include "mainwin.h"

#include "town_model.h"

#define CMD_TOWNS_SHOW_GHOSTS  "TownShowGhosts"
#define CMD_TOWNS_SEL_ALL      "TownSelAll"
#define CMD_TOWNS_ADD_TO_MYTOWNS "TownAddMytowns"
#define CMD_TOWNS_ADD_TO_TARGETS "TownAddTargets"

#define CMD_TOWNS_GUESS_TYPE   "TownGuessType"

TownView::TownView () : TableView ("NewTowns", tr("Towns")) 
{
	new Setting (this, TOWN_TAB_CURRENT_INDEX, QPoint (-1,-1));

	QFrame * f= new QFrame ;
	ui.setupUi (f);

	_model = new TownModel (this);

	initTable();
	setCentralWidget (f);
	Setting::connect (TOWN_TAB_CURRENT_INDEX,this,SLOT(settingChange()));
	Setting::connect (MAPVIEW_GLOBAL_OFFSET, this, SLOT(mapChange()));

	addCommand (ui._ghosts, CMD_TOWNS_SHOW_GHOSTS,    tr ("Show ghost towns"), tr("Show ghost towns"), tr("Show ghost towns wt"),QIcon (":/icons/ghost.ico"));
	addCommand (this ,      CMD_TOWNS_SEL_ALL,        tr ("Select all"), tr("Select the entire list"), tr("Select all wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_ADD_TO_MYTOWNS, tr ("Add to My Towns"), tr("Add the current selection to the current My Towns list"), tr("Add to My Towns wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_ADD_TO_TARGETS, tr ("Add to Targets"), tr("Add the current selection to the current Targets list tt"), tr("Add to Targets wt"),QIcon ());
	addCommand (this ,      CMD_TOWNS_GUESS_TYPE, tr ("Guess type"), tr("Guess the tactical type from units in the town."), tr("Guess type wt"),QIcon ());

	_type_ag = new QActionGroup (this);
	for (int i=0;i< TownTypeMax; ++i) {
		QAction * tta = new QAction (Cmd::worldData().townTypeText((TownTypeIndex) i, false), this);
		tta->setCheckable (true);
		tta->setData (i);
		_type_ag ->addAction (tta);
	}
	connect (_type_ag, SIGNAL(triggered(QAction*)), this, SLOT(townTypeSelected(QAction*)));

	connect (this, SIGNAL (itemDataChangedSignal (int)), this, SIGNAL (townDataChangedSignal (int)));
	connect (this, SIGNAL (itemDataChangedSignal (int)), this, SIGNAL (updateMapSignal()));
	connect (this, SIGNAL (itemDoubleClickSignal (int)), this, SIGNAL (gotoTownSignal(int)));
}

void TownView::townTypeSelected (QAction* act)
{
	WorldData & w = Cmd::worldData();

	TownTypeIndex st = (TownTypeIndex) act->data().toInt();
	QVector <int> s = selectedItems ();
	for (int i=0;i<s.size(); ++i) {
		w.towns()[s[i]].type = st;
		emit townDataChangedSignal (s[i]); 
	}

	model()->triggerRedraw ();
}


void TownView::setAlly (int a) {
	_model->setCurrentAlly (a);
	sortModel();
	WorldData & w = Cmd::worldData();

	ui._info->setText(QString ("<b>%1 ").arg (w.allys()[a].towns.size() ) +
		tr("towns of") +
		"&nbsp;<img src=':/icons/ally.png' width=16 height=16>&nbsp;" 
		+ w.allys()[a].name + "</b>");
};

void TownView::setPlayer (int p)  {
	_model->setCurrentPlayer(p);
	sortModel();
	WorldData & w = Cmd::worldData();

	if (p <0){
		int n = 0;
		for(int i=0;i < w.towns().size();++i)
			if(w.towns()[i].player <0 ) ++n; 
		ui._info->setText(QString ("<b><img src=':/icons/ghost.png' width=16 height=16>&nbsp;%1&nbsp;%2</b>").arg(n).arg (tr("Ghost towns")));
	} else {
		ui._info->setText(QString ("<b>%1 ").arg(w.players()[p].towns.size()) +
			tr("towns of") +
			"&nbsp;<img src=':/icons/player.png' width=16 height=16>&nbsp;" 
			+ w.players()[p].name + "</b>");
	}

};

void TownView::fillContextMenu (QMenu *m)
{	
	Cmd::get(CMD_TOWNS_SHOW_GHOSTS)->addToMenu (m);
	Cmd::get(CMD_TOWNS_SEL_ALL)->addToMenu (m);
	Cmd::get(CMD_TOWNS_ADD_TO_MYTOWNS)->addToMenu (m);
	Cmd::get(CMD_TOWNS_ADD_TO_TARGETS)->addToMenu (m);
	m->addSeparator();
	QMenu *mm = m->addMenu (tr("Tactical town type"));
	Cmd::get(CMD_TOWNS_GUESS_TYPE)->addToMenu (mm);		
	mm->addSeparator();
	for (int i=0;i < _type_ag->actions().size(); ++i) {
		mm->addAction (_type_ag->actions()[i]);
	}
}


void TownView::mapChange ()
{
	model()->triggerRedraw();
}


void TownView::settingChange ()
{
	QPoint p = Setting::value(TOWN_TAB_CURRENT_INDEX).value<QPoint>();
	if(p.x()>=0) setPlayer (p.x());
	else if (p.y()>=0) setAlly (p.y());
	else setPlayer (-1);
}

void TownView::handleCmd (const QString& key)
{
	WorldData & w = Cmd::worldData();

	if (key == CMD_TOWNS_SHOW_GHOSTS) {
		setGhosts();
	} else if (key == CMD_TOWNS_GUESS_TYPE) {

		QVector <int> s = selectedItems ();
		for (int i=0;i<s.size(); ++i) {
			TownTypeIndex st = w.towns()[s[i]].type;
			if(st == TownTypeUndefined) {
				w.towns()[s[i]].type = w.towns()[s[i]].guessType();
			}
		}
		model()->triggerRedraw ();

	} else if (key == CMD_TOWNS_SEL_ALL) {
		ui._tab->selectAll();

	} else if (key == CMD_TOWNS_ADD_TO_MYTOWNS) {
		
		int cl = Setting::value(MYTOWN_CURRENT_LIST).toInt();
		MyTownList & tl = w.mytowns()[cl];
		QVector <int> sel = selectedItems ();
		for(int i=0;i<sel.size(); ++i) {
			tl.l.push_back(MyTown(w.towns()[sel[i]].id,Attackship));
		}
		emit myTownChangedSignal (cl);

	} else if (key == CMD_TOWNS_ADD_TO_TARGETS) {
		int cl = Setting::value(TARGET_CURRENT_LIST).toInt();
		TargetList & tl = w.targets()[cl];
		QVector <int> sel = selectedItems ();
		for(int i=0;i<sel.size(); ++i) {
			tl.l.push_back(Target(w.towns()[sel[i]].id));
		}
		emit targetChangedSignal (cl);

	} else {
		ToolWidget::handleCmd(key);
	}

}



TownModel::TownModel(QObject *parent): TableModel(parent)
{
}

int TownModel::numColumns () const
{
	return 10;
}

int TownModel::maxIndex () const
{
	return Cmd::worldData().towns().size();
}


QByteArray TownModel::headerState () const
{
	return Cmd::worldData().townsTableSetting().state;
}

void TownModel::saveHeaderState (const QByteArray& b)
{
	Cmd::worldData().townsTableSetting().state = b;
}

bool TownModel::itemCheckable   (int ) const
{
	return false;
}

bool TownModel::itemChecked     (int , int ) const
{
	return false;
	
}

void TownModel::setItemChecked  (int , int , bool )
{
	
}



QVariant TownModel::itemDisplayData (int idx, int section) const
{
	if (idx < 0 || idx > Cmd::worldData().towns().size()) 
		return QVariant();

	const Town & to = Cmd::worldData().towns()[idx];
	if (section == 0)   
		return to.name;
	else if( section == 1 )
		return (to.player <0)?tr("no owner"): Cmd::worldData().players()[to.player].name;
	else if( section == 2 ) 
		return (to.ally <0)?tr("no ally"): Cmd::worldData().allys()[to.ally].name;
	else if( section == 3 ) 
		return QString::number(to.points);
	else if( section == 4 ) 
		return QString::number(Cmd::worldData().islands()[to.island].id);
	else if( section == 5 ) 
		return tr("M%1").arg(Cmd::worldData().oceanNumberOfIsland(to.island));
	else if( section == 6 )  {
		QPoint pc = MainWin::currentMapOffset() - to.pos();
		double dx = pc.x();
		double dy = pc.y();
		double d = sqrt(dx*dx+dy*dy)/12.8;
		int dd = d;
		return QString::number(dd/10)+"."+QString::number(dd%10);

	} else if( section == 7 ) {
		return Cmd::worldData().townSpecialPixmap(idx);
	} else if( section == 8 ){
		return Cmd::worldData().townUnitsPixmap(idx);
	} else if( section == 9 ){
		return WorldData::townTypeText(to.type,false);
	}
	return QVariant ();
}

QVariant TownModel::itemDecoration  (int idx, int col) const
{
	if( col == 9 ) {
		const Town & to = Cmd::worldData().towns()[idx];	
		return WorldData::townTypeSymbol(to.type).scaled(iconSize());
	}
	return QVariant();
};


QVariant TownModel::itemTooltip (int idx, int section) const
{
	if( section == 7 ) {
		return Cmd::worldData().townSpecialText(idx);
	} else if( section == 8 ){
		return Cmd::worldData().townUnitsText(idx);
	}
	return QVariant ();
}

QString TownModel::headerText      (int section) const
{
	if (section == 0)        return tr("Town");
	else if( section == 1 )  return tr("Owner");
	else if( section == 2 )  return tr("Ally");
	else if( section == 3 )  return tr("Points");
	else if( section == 4 )  return tr("Island");
	else if( section == 5 )  return tr("Ocean");
	else if( section == 6 )  return tr("Distance");
	else if( section == 7 )  return tr("Specials");
	else if( section == 8 )  return tr("Units");
	else if( section == 9 )  return tr("Type");
	return "";
}
QIcon TownModel::headerIcon      (int section) const
{
	if (section == 0)        return QIcon(":/icons/town.ico") ; // tr("Town");
	else if( section == 1 )  return QIcon(":/icons/player.ico") ; //tr("Owner");
	else if( section == 2 )  return QIcon(":/icons/ally.ico") ; //tr("Ally");
	else if( section == 3 )  return QIcon(":/icons/points.ico") ; //tr("Points");
	else if( section == 4 )  return QIcon(":/icons/island.ico") ; //tr("Island");
	else if( section == 5 )  return QIcon(":/icons/world.ico") ; //tr("Ocean");
	else if( section == 6 )  return QIcon(":/icons/coords_goto2.ico") ; //tr("Distance");
	else if( section == 7 )  return QIcon(":/icons/town_question.ico") ; //tr("Specials");
	else if( section == 8 )  return QIcon(":/icons/helmet.ico") ; //tr("Units");
	else if( section == 9 )  return QIcon(":/icons/symbol.ico") ; //tr("Type");
	return QIcon ();
}

QString TownModel::headerTooltip   (int section) const
{
	if (section == 0)        return tr("Town name");
	else if( section == 1 )  return tr("Owner");
	else if( section == 2 )  return tr("Ally");
	else if( section == 3 )  return tr("Points of town");
	else if( section == 4 )  return tr("Island id");
	else if( section == 5 )  return tr("Ocean number");
	else if( section == 6 )  return tr("Distance to current center of map");
	else if( section == 7 )  return tr("Town settings");
	else if( section == 8 )  return tr("Units in the town");
	else if( section == 9 )  return tr("Tactical type of the town");
	return QString();
}

QVariant TownModel::itemText        (int idx, int section) const
{
	const Town & to = Cmd::worldData().towns()[idx];
	if (section == 0)   
		return to.name;
	else if( section == 1 )
		return (to.player <0)?tr("no owner"): Cmd::worldData().players()[to.player].name;
	else if( section == 2 ) 
		return (to.ally <0)?tr("no ally"): Cmd::worldData().allys()[to.ally].name;
	else if( section == 3 ) 
		return QString::number(to.points);
	else if( section == 4 ) 
		return QString::number(Cmd::worldData().islands()[to.island].id);
	else if( section == 5 ) 
		return QString::number(Cmd::worldData().oceanNumberOfIsland(to.island));
	else if( section == 6 )  {
		QPoint pc = MainWin::currentMapOffset() - to.pos();
		double dx = pc.x();
		double dy = pc.y();
		double d = sqrt(dx*dx+dy*dy)/12.8;
		int dd = d;
		return QString::number(dd/10)+"."+QString::number(dd%10);

	} else if( section == 7 ) {
		return Cmd::worldData().townSpecialText(idx,true);
	} else if( section == 8 ){
		return Cmd::worldData().townUnitsText(idx,true);
	} else if( section == 9 ){
		return WorldData::townTypeText(to.type,false);
	}
	return QString();
}

QVariant TownModel::itemBBCode  (int idx, int section) const
{
	const Town & to = Cmd::worldData().towns()[idx];
	if (section == 0) {
		return QString("[town]%1[/town]").arg(to.id);
	} else if( section == 1 ) {
		if (to.player <0)
			return tr("no owner");
		return QString("[player]%1[/player]").arg(	Cmd::worldData().players()[to.player].name ) ;
	} else if( section == 2 ) {
		if (to.ally <0)
			return tr("no ally");
		return QString("[ally]%1[/ally]").arg(Cmd::worldData().allys()[to.ally].name) ;
	} else if( section == 4 ) {
		return QString("[island]%1[/island]").arg(Cmd::worldData().islands()[to.island].id);
	}
	return itemText (idx,section);
}

bool TownModel::lessThan  (int ai, int bi, int col) const
{
	WorldData &w = Cmd::worldData();

	const Town& a = w.towns()[ai];
	const Town& b = w.towns()[bi];

	int cm = 0;

	if (col == 0) { // town

		cm = a.name.compare(b.name, Qt::CaseInsensitive);

	} else if (col == 1 ) { // player

		QString pa = (a.player<0)?tr("no owner"):w.players()[a.player].name;
		QString pb = (b.player<0)?tr("no owner"):w.players()[b.player].name;

		cm = pa.compare(pb, Qt::CaseInsensitive);

	} else if (col == 2 ) { // ally

		QString pa = (a.ally<0)?tr("no ally"):w.allys()[a.ally].name;
		QString pb = (b.ally<0)?tr("no ally"):w.allys()[b.ally].name;

		cm = pa.compare(pb, Qt::CaseInsensitive);

	} else if (col == 3 ) { // points

		cm = a.points-b.points;

	} else if (col == 4 ) { // island

		cm = a.island- b.island;

	} else if (col == 5 ) { // ocean

		cm = w.oceanNumberOfIsland(a.island)-w.oceanNumberOfIsland(b.island);

	} else if (col == 6 ) { // distance

		QPointF pc = MainWin::currentMapOffset();
		QPointF da = a.pos()-pc;
		QPointF db = b.pos()-pc;

		cm = (da.x()*da.x()+da.y()*da.y()) - (db.x()*db.x()+db.y()*db.y());

	} else if (col == 7 ) { // town_sett

		int aa = 0;
		if(a.research[ResearchMeteorology]) aa += 1;
		if(a.research[ResearchCartography]) aa += 2;
		if(a.movement_boost)                aa += 4;
		if(a.buildings[Lighthouse])         aa += 8;

		int bb = 0;
		if(b.research[ResearchMeteorology]) bb += 1;
		if(b.research[ResearchCartography]) bb += 2;
		if(b.movement_boost)                bb += 4;
		if(b.buildings[Lighthouse])         bb += 8;

		cm = aa-bb;

	} else if (col == 8 ) { // units

		int aa= 0;
		int bb= 0;
		for(int k=0;k< UnitMax; ++k) {
			aa += a.units[k];
			bb += b.units[k];
		}

		cm = aa-bb;

	} else if (col == 9 ) { // tactical type

		cm = a.type - b.type;
	}
	return cm < 0;
}


void TownModel::setCurrentPlayer (int p)
{
	QVector <int> idx;
	if (p < 0) {
		// ghost towns
		int nt = Cmd::worldData().towns().size();
		for(int i=0; i < nt; ++i)  {
			if(Cmd::worldData().towns()[i].player < 0)
				idx.push_back (i);
		}
	} else {
		idx = Cmd::worldData().players()[p].towns;
	}
	resetIndex (idx,0);
}


void TownModel::setCurrentAlly (int a)
{	
	resetIndex (Cmd::worldData().allys()[a].towns, 0);
}


void TownModel::sort ( int col, Qt::SortOrder order )
{
	
	emit layoutAboutToBeChanged ();

	const WorldData *w = &Cmd::worldData();

	if (_sel.size() != _idx.size()) 

		_sel.resize(_idx.size());

	for ( int i = 0 ; i <  ( _idx.size() - 1 ); ++i ) {

		const Town * a = &(w->towns().at(_idx[i]));

		for ( int j = i+1; j < _idx.size(); ++j ) {

			const Town * b = &(w->towns().at(_idx[j]));

			int cm = 0;

			if (col == 0) { // town

				cm = a->name.compare(b->name, Qt::CaseInsensitive);

			} else if (col == 1 ) { // player

				if (a->player<0) {
					if (b->player >= 0) 
						cm = 1;
				} else if (b->player<0) {
					cm = -1;
				} else {
					cm = w->players().at(a->player).name.compare(w->players().at(b->player).name, Qt::CaseInsensitive);
				}

			} else if (col == 2 ) { // ally

				QString pa = (a->ally<0)?"":w->allys()[a->ally].name;
				QString pb = (b->ally<0)?"":w->allys()[b->ally].name;

				cm = pa.compare(pb, Qt::CaseInsensitive);

			} else if (col == 3 ) { // points

				cm = a->points-b->points;

			} else if (col == 4 ) { // island

				cm = a->island- b->island;

			} else if (col == 5 ) { // ocean

				cm = w->oceanNumberOfIsland(a->island)-w->oceanNumberOfIsland(b->island);

			} else if (col == 6 ) { // distance

				QPointF pc = MainWin::currentMapOffset();
				QPointF da = a->pos()-pc;
				QPointF db = b->pos()-pc;

				cm = (da.x()*da.x()+da.y()*da.y()) - (db.x()*db.x()+db.y()*db.y());

			} else if (col == 7 ) { // town_sett

				int aa = 0;
				if(a->research[ResearchMeteorology]) aa += 1;
				if(a->research[ResearchCartography]) aa += 2;
				if(a->movement_boost)                aa += 4;
				if(a->buildings[Lighthouse])         aa += 8;

				int bb = 0;
				if(b->research[ResearchMeteorology]) bb += 1;
				if(b->research[ResearchCartography]) bb += 2;
				if(b->movement_boost)                bb += 4;
				if(b->buildings[Lighthouse])         bb += 8;

				cm = aa-bb;

			} else if (col == 8 ) { // units

				int aa= 0;
				int bb= 0;
				for(int k=0;k< UnitMax; ++k) {
					aa += a->units[k];
					bb += b->units[k];
				}

				cm = aa-bb;

			} else if (col == 9 ) { // tactical type


				cm = a->type - b->type;
			}


			if (order != Qt::AscendingOrder) {
				cm = -cm;				
			}

			if (cm < 0) {

				int dmy = _idx[i];
				_idx[i] = _idx[j];
				_idx[j] = dmy;

				bool bdmy  = _sel[i];
				_sel[i] = _sel[j];
				_sel[j] = bdmy;
				
				a = b;
			}
		}
	}
	emit layoutChanged ();
}














