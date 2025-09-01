/* toolwidget.cpp - tool widgets

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

#include "tabletool.h"
#include "explorer.h"
#include "setting.h"
#include "app.h"

#include <QHeaderView>
#include <QMenu>


TableTool::TableTool (const QString& id, const QString& ti):ToolWidget (id,ti){}
TableTool::~TableTool () {}


int TableTool::itemIndex  (int row) 
{
	for(int i=0;i<table()->rowCount();++i) {
		TableItem * ti = dynamic_cast <TableItem*> ( table()->item(row,0));
		if (ti)
			return ti->index;
	}
	return -1;
}

void TableTool::initWidget ()
{
	table()->setSelectionBehavior (QAbstractItemView::SelectRows);

	if (extendedSelection ()) 
		table()->setSelectionMode (QAbstractItemView::ExtendedSelection);
	else
		table()->setSelectionMode (QAbstractItemView::SingleSelection);

	int ncol = colDefs.size();
	
	table()->setColumnCount(ncol);
	table()->setCornerButtonEnabled(false);
	table()->horizontalHeader()->setSectionsMovable(true); 
	table()->verticalHeader()->setSectionResizeMode (QHeaderView::Fixed);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect (this, SIGNAL (customContextMenuRequested ( const QPoint &)), this, SLOT (contextMenuRequested(const QPoint&)));

	connect (table()->horizontalHeader(), SIGNAL (sectionClicked ( int )), this, SLOT(headerClicked(int)));
	connect (table()->horizontalHeader(), SIGNAL (sectionMoved(int,int,int)), this, SLOT(sectionMoved(int,int,int)));
	connect (table()->horizontalHeader(), SIGNAL (sectionResized(int,int,int)), this, SLOT(sectionResized()));

	
	connect (table(), SIGNAL (cellDoubleClicked ( int , int )),	this, SLOT(cellDoubleClicked ( int , int )));
	connect (table(), SIGNAL (cellClicked ( int , int )),	this, SLOT(cellClicked ( int , int )));
	connect (table(), SIGNAL (cellActivated ( int , int )),	this, SLOT(cellClicked ( int , int )));
	connect (table(), SIGNAL (itemSelectionChanged ()),	this, SLOT(selectionChanged ()));
	connect (table(), SIGNAL (cellChanged (int,int)),	this, SLOT(cellChanged (int,int)));

	vis_ag = new QActionGroup (this);

	vis_ag->setExclusive (false);

	for (int i =0;i<  ncol; ++i ) {
		if(!colDefs[i].hidable) continue;
		QAction * va = new QAction (colDefs[i].name,this);
		va->setCheckable (true);
		va->setData (i);
		vis_ag->addAction (va);
	}
	connect (vis_ag, SIGNAL(triggered(QAction*)), this, SLOT(colVisibilityChanged(QAction*)));
}

void TableTool::updateCommands ()
{
	ToolWidget::updateCommands();
};

void TableTool::updateContent () 
{
	ToolWidget::updateContent();
};

void TableTool::handleCmd (const QString& key) 
{
	ToolWidget::handleCmd (key);
};



bool TableTool::event (QEvent *ev)
{
   if (ev->type() == QEvent::KeyPress) {

			QKeyEvent *kev = static_cast<QKeyEvent*>(ev);
			if (kev->key() == Qt::Key_Delete) {
				delSel();
				ev->accept();
				return true;
			}
	} else if (ev->type() == QEvent::MouseButtonRelease) {
			// QMouseEvent * mev =  static_cast<QMouseEvent*> (ev);

	}
	return ToolWidget::event(ev);
}


void TableTool::worldChanged () 
{
	if(!_is_init) {
		initWidget();
		_is_init= true;
	}
	currentTableChanged ();
	updateTable();
}

void TableTool::sectionResized (  )
{
	TableSetting * set = getTableSettings ();
	set->state = table()->horizontalHeader ()->saveState ();
}

void TableTool::worldAboutToChange ()
{
}


void TableTool::colVisibilityChanged(QAction* act)
{
	int idx = act->data().toInt();
	bool cs = act->isChecked ();
	table()->horizontalHeader()->setSectionHidden (idx, !cs);
	TableSetting * set = getTableSettings ();
	set->state = table()->horizontalHeader ()->saveState ();
}

void TableTool::rootIndexChanged ()
{
	root_index = Setting::value(root_index_name);
	currentTableChanged ();
	updateTable();
}

void TableTool::setRootIndexSetting (const QString & s)
{
	root_index_name = s;
	Setting::connect (s, this, SLOT(rootIndexChanged ()));
	root_index = Setting::value(s).toInt();
}

void TableTool::setRootIndex (const QVariant& v)
{
	Setting::setValue(root_index_name, v);	
}


void TableTool::contextMenuRequested (const QPoint& pos)
{		
	if (table()->horizontalHeader()->underMouse()) {

		// Tab visibility menu
		QMenu m ;

		fillHeaderMenu (&m);

		m.exec(mapToGlobal(pos));

		return;

	} else if (table()->viewport()->underMouse())  {
		
		// see if we have an Item 
		QPoint tp (table()->viewport()->mapFrom(this,pos));
		int row = table()->rowAt(tp.y());
		int col = table()->columnAt(tp.x());
		if (row>= 0 && col>= 0) {

			showItemContextMenu (pos , itemIndex (row), colDefs[col]);

			return;
		}
	} 
	
	showContextMenu (pos);		
}

void TableTool::sectionMoved ( int , int , int )
{
	bool move = true;
	for(int i=0;i< colDefs.size(); ++i) {
		if(!colDefs[i].movable && i != table()->horizontalHeader()->visualIndex(i)) {
			move = false;
			break;
		}
	}
	TableSetting * set = getTableSettings ();
	if (move) {
		set->state = table()->horizontalHeader ()->saveState ();
	} else {
		table()->horizontalHeader ()->restoreState (set->state);
	}
}

void TableTool::headerClicked(int logicalIndex)
{
	if(!_is_init) return;
	TableSetting * set = getTableSettings ();
	if (!colDefs[logicalIndex].sortable) return;

	table()->sortByColumn (logicalIndex,table()->horizontalHeader ()->sortIndicatorOrder());
	set->state = table()->horizontalHeader ()->saveState ();
}

void TableTool::sortTable  ()
{
	table()->sortByColumn (table()->horizontalHeader ()->sortIndicatorSection (), table()->horizontalHeader ()->sortIndicatorOrder());
}

QList <int> TableTool::selection () 
{
	QList <int> r;
	QList<QTableWidgetItem *> si = table()->selectedItems ();
	for (int i=0;i< si.size(); ++i) {		
		int ii = itemIndex (si[i]->row());
		if(ii >= 0 && !r.contains (ii))
			r.push_back(ii);
	}
	return r;
}

void TableTool::saveSelection ()
{
	TableSetting * set = getTableSettings ();

	QList<QTableWidgetItem *> si = table()->selectedItems ();
	set->selection.clear();
	for (int i=0;i< si.size(); ++i) {		
		int ii = itemIndex (si[i]->row());
		if(ii >= 0 && !set->selection.contains (ii))
			set->selection.push_back(ii);
	}
}

void TableTool::restoreSelection  () 
{
	TableSetting * set = getTableSettings ();
	table()->clearSelection();
	for (int i=0;i< table()->rowCount(); ++i) {
		int ii = itemIndex (i);
		if (set->selection.contains (ii))
			table()->selectRow (i);
	}
}

void TableTool::updateTable ()
{
	int ncol = colDefs.size();
	TableSetting * set = getTableSettings ();

	// initialize the header
	table()->setSortingEnabled(false);
	table()->setColumnCount (colDefs.size());
	table()->horizontalHeader()->setSortIndicatorShown (true);

	table()->clearSelection();

	for (int i=0; i< ncol; ++i) {
		QTableWidgetItem  * it = new QTableWidgetItem (colDefs[i].ico, colDefs[i].name);
		it->setToolTip(colDefs[i].tip);
		it->setWhatsThis(colDefs[i].what);
		table()->setHorizontalHeaderItem (i, it);		
	}

	// fill in the contents
	table()->setRowCount (rowCount());
	for (int r = 0; r<rowCount(); ++r) {
		for (int c=0; c < colDefs.size(); ++c) {
			updateCell (r, c, colDefs[c]);
		}
	}

	table()->horizontalHeader ()->restoreState (set->state);
	sortTable ();
}

void TableTool::itemDataChanged ()
{
	for (int r = 0; r<rowCount(); ++r) {
		for (int c=0; c < colDefs.size(); ++c) {
			updateCellData (r, c, colDefs[c]);
		}
	}
	sortTable ();
}



void TableTool::selectIndex (const QVector <int> &idx)  
{
	table()->clearSelection();
	for (int r = 0; r<rowCount(); ++r) {
		int ri = itemIndex(r);
		if (idx.contains(ri)) {
			table()->selectRow (r);
//			table()->ensureVisible (r);
		}
	}
}

void TableTool::selectIndex (int idx)  
{
	QVector<int > i ;
	i.push_back (idx);
	selectIndex (i);
}

void TableTool::addColDef (const ColDef & cd) 
{
	colDefs.push_back (cd);
}

QStringList TableTool::itemHeaders       ()
{
	QStringList r;
	for(int i=0;i< colDefs.size(); ++i) r.push_back(colDefs[i].name);
	return r;
}


/*! This only happens on checkable columns
*/
void TableTool::cellChanged (int row, int col)
{
	QTableWidgetItem * it = table()->item(row,col);
	if(!it) return;
	if(!(it->flags() & Qt::ItemIsUserCheckable)) return;
	setItemCheckState (itemIndex(row), it->checkState() == Qt::Checked);
	emit itemDataChangedSignal (this, itemIndex(row));	
}


void TableTool::cellClicked (int row, int  )
{
	emit itemClickSignal (this, itemIndex(row));	
}

void TableTool::cellDoubleClicked (int row, int)
{
	//if (extendedSelection()) table()->setSelectionMode(QAbstractItemView::MultiSelection);
	//table()->selectRow (row);
	//if (extendedSelection()) table()->setSelectionMode(QAbstractItemView::ExtendedSelection);
	emit itemDoubleClickSignal (this, itemIndex(row));	
}

void TableTool::selectionChanged ()
{
	saveSelection();
	emit selectionChangedSignal (this);
}

void TableTool::fillHeaderMenu (QMenu* mm, bool add_sm)
{
	QMenu * m= mm;
	if (add_sm) m = mm->addMenu (tr("Tab visibility"));
	for (int i=0;i < vis_ag->actions().size(); ++i) {
		QAction * act = vis_ag->actions()[i];
		int col = act->data().toInt();
		bool b = table()->horizontalHeader()->isSectionHidden(col);
		act->setChecked (!b);
		m->addAction (act);
	}
	mm->addSeparator();
	mm->addAction (tr("Copy table (plain text)"), this, SLOT(copyTable()));
	mm->addAction (tr("Copy table (BB-Codes)"), this, SLOT(copyBBCode()));
}
	
void TableTool::copyBBCode ()
{
	QStringList r; 
	r.push_back (QString ("#\t") + itemHeaders().join("\t")); 
	for (int i=0;i< rowCount();++i) {
		QString li =QString::number (i) + "\t";
		for(int j=0;j<colDefs.size(); ++j) {
			if(j>0) li+="\t";
			int c = j ; // table()->horizontalHeader()->visualIndex(j);
			li += itemBBCode (itemIndex(i), colDefs[c]);  
		}
		r.push_back(li);
	}	
	App::clipboard()->setText (r.join("\n"));
}


void TableTool::copyTable ()
{
	QStringList r; 
	r.push_back (QString ("#\t") + itemHeaders().join("\t")); 
	for (int i=0;i< rowCount();++i) {
		QString li =QString::number (i) + "\t";
		for(int j=0;j<colDefs.size(); ++j) {
			int c = j; // table()->horizontalHeader()->visualIndex(j);
			if(j>0) li+="\t";
			li += itemText (itemIndex(i), colDefs[c]);  
		}
		r.push_back(li);
	}	
	App::clipboard()->setText (r.join("\n"));
}
