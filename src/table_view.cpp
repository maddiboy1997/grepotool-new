/* tabke_view.cpp  - Table view handler

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
#include "table_model.h"
#include "setting.h"
#include <QApplication>
#include <QHeaderView>
#include <QMenu>


TableView::TableView (const QString& id, const QString& title) : ToolWidget (id,title)
{
}

TableView::~TableView ()
{
}

void TableView::initTable () 
{
	table ()->setModel( model () );

	ItemDelegate *del = new ItemDelegate ( this );
	
	table()->setItemDelegate ( del );
	
	table()->setSortingEnabled(false);
	
	table()->setCornerButtonEnabled(false);
	table()->horizontalHeader()->setSectionsMovable(true); 
	table()->verticalHeader()->setMinimumSectionSize (20);
	table()->verticalHeader()->setDefaultSectionSize (20);
	table()->verticalHeader()->setSectionResizeMode (QHeaderView::Fixed);

	setContextMenuPolicy(Qt::CustomContextMenu);


	table()->horizontalHeader ()->restoreState (model()->headerState());


	connect (this, SIGNAL (customContextMenuRequested ( const QPoint &)), this, SLOT (contextMenuRequested(const QPoint&)));

	connect (table()->horizontalHeader(), SIGNAL (sectionClicked ( int )), this, SLOT(headerClicked(int)));
	connect (table()->horizontalHeader(), SIGNAL (sectionMoved(int,int,int)), this, SLOT(sectionMoved(int,int,int)));
	connect (table()->horizontalHeader(), SIGNAL (sectionResized(int,int,int)), this, SLOT(sectionResized()));

	connect(table()->selectionModel(),SIGNAL (selectionChanged ( const QItemSelection & , const QItemSelection & )),
		this, SLOT (itemSelectionChanged ( const QItemSelection &, const QItemSelection & )));
	connect (table(), SIGNAL (clicked ( const QModelIndex & )) , this, SLOT (itemClicked ( const QModelIndex & )));
	connect (table(), SIGNAL (doubleClicked ( const QModelIndex &)) , this, SLOT (itemDoubleClicked ( const QModelIndex & )));
	connect (table(), SIGNAL (activated( const QModelIndex &)) , this, SLOT (itemActivated ( const QModelIndex & )));

	vis_ag = new QActionGroup (this);

	vis_ag->setExclusive (false);

	int ncol = model()->numColumns();

	for (int i =0;i<  ncol; ++i ) {
		if (!model()->canHideColumn(i)) continue;
		QAction * va = new QAction (model()->headerText(i),this);
		va->setCheckable (true);
		va->setData (i);
		vis_ag->addAction (va);
	}
	connect (vis_ag, SIGNAL(triggered(QAction*)), this, SLOT(colVisibilityChanged(QAction*)));


	Setting::connect (CURRENT_WORLD, this,SLOT(restoreHeaderState()));
	Setting::connect2 (CURRENT_WORLD, this,SLOT(saveHeaderState()));

}

void TableView::itemActivated ( const QModelIndex & index )
{
	emit itemClickSignal (model()->indexOfRow(index.row()));	
	emit itemClickSignal (this, model()->indexOfRow(index.row()));	
}

void TableView::itemClicked ( const QModelIndex & index )
{
	emit itemClickSignal (model()->indexOfRow(index.row()));	
	emit itemClickSignal (this, model()->indexOfRow(index.row()));	
}

void TableView::itemDoubleClicked ( const QModelIndex & index )
{
	emit itemDoubleClickSignal (model()->indexOfRow(index.row()));	
	emit itemDoubleClickSignal (this, model()->indexOfRow(index.row()));	
}

void TableView::itemSelectionChanged ( const QItemSelection &, const QItemSelection & )
{
	emit selectionChangedSignal (this);
	emit selectionChangedSignal ();
}


QVector <int> TableView::selectedItems ()  
{
	QVector <int> r;
	QModelIndexList l = table()->selectionModel ()->selectedRows();
	for(int i=0;i<l.size(); ++i) {
		r.push_back (model()->indexOfRow(l[i].row()));
	}
	return r;
}



bool TableView::event (QEvent *ev)
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

void TableView::colVisibilityChanged(QAction* act)
{
	int idx = act->data().toInt();
	bool cs = act->isChecked ();
	table()->horizontalHeader()->setSectionHidden (idx, !cs);
	model()->saveHeaderState (table()->horizontalHeader ()->saveState ());
}

void TableView::contextMenuRequested(const QPoint& pos)
{
	int col =  0;
	QPoint p = table()->viewport()->mapFrom (this,pos);
	QModelIndex idx = table()->indexAt (p);
	if(idx.isValid()) col = idx.column();
	QModelIndexList l = table()->selectionModel ()->selectedRows(col);
	if (model()->customEditOfItems (l)) {
		return;
	}
	QMenu m ;
	fillContextMenu(&m);
	model ()->fillContextMenu(&m, l);
	m.addSeparator ();
	QMenu * mm = m.addMenu (tr("Tab visibility"));
	for (int i=0;i < vis_ag->actions().size(); ++i) {
		QAction * act = vis_ag->actions()[i];
		int col = act->data().toInt();
		bool b = table()->horizontalHeader()->isSectionHidden(col);
		act->setChecked (!b);
		mm->addAction (act);
	}
	m.addSeparator();
	m.addAction (tr("Copy table (plain text)"), this, SLOT(copyTable()));
	m.addAction (tr("Copy table (BB-Codes)"), this, SLOT(copyBBCode()));
	m.exec (mapToGlobal (pos));
}

void TableView::headerClicked(int logicalIndex)
{
	sortModel (logicalIndex);
}

void TableView::sortModel (int column) 
{

	if (column < 0) 
		column = table()->horizontalHeader ()->sortIndicatorSection ();
	if (!model()->canSortByColumn (column)) return;

	table()->selectionModel ()->blockSignals (true);
	model()->storeSelection (table()->selectionModel ()->selectedRows());
	table()->selectionModel ()->clear ();
	table()->horizontalHeader()->setSortIndicatorShown (true);
	table()->sortByColumn (column, table()->horizontalHeader ()->sortIndicatorOrder());
	model()->saveHeaderState (table()->horizontalHeader ()->saveState ());
	QModelIndexList l = model()->restoreSelection();
	for (int i=0;i< l.size(); ++i) {
		table()->selectionModel ()->select (l[i], QItemSelectionModel::Select |QItemSelectionModel::Rows);
	}
	table()->selectionModel ()->blockSignals (false);
	table()->horizontalHeader ()->restoreState (model()->headerState());
	table()->horizontalHeader()->setSortIndicatorShown (true);
}

void TableView::selectIndex (const QVector <int> &idx)
{
	QModelIndexList l = model ()->rowsOfItems (idx);
	for (int i=0;i< l.size(); ++i) {
		table()->selectionModel ()->select (l[i], QItemSelectionModel::Select |QItemSelectionModel::Rows);
		table()->scrollTo(l[i]);
	}
		
}

void TableView::selectIndex (int idx)
{
	QVector <int> ix;
	ix.push_back(idx) ;
	selectIndex (ix);
}



void TableView::sectionMoved(int,int,int)
{
	bool move = true;
	for(int i=0;i< model()->numColumns(); ++i) {
		if(!model()->canMoveColumn (i) && i != table()->horizontalHeader()->visualIndex(i)) {
			move = false;
			break;
		}
	}
	if (move) {
		model ()->saveHeaderState (table()->horizontalHeader ()->saveState ());
	} else {
		table()->horizontalHeader ()->restoreState (model()->headerState());
	}
}

void TableView::saveHeaderState()
{
	model ()->saveHeaderState (table()->horizontalHeader ()->saveState ());
}

void TableView::restoreHeaderState()
{
	table()->horizontalHeader ()->restoreState (model()->headerState());
}

void TableView::sectionResized()
{
	model ()->saveHeaderState (table()->horizontalHeader ()->saveState ());
}

void TableView::updateTable ()
{
	table()->setSortingEnabled(false);
	table()->horizontalHeader ()->restoreState (model()->headerState());
	sortModel ();
	table()->horizontalHeader()->setSortIndicatorShown (true);
}

void TableView::copyBBCode ()
{
	QStringList r; 
	int nc = model()->numColumns();
	int nr = model()->rowCount();
	QString head ("#");
	for (int i = 0;i < nc;++i) {
		head += "\t";
		head += model()->headerText(i);
	}		
	r.push_back(head);
	for (int i=0;i< nr; ++i) {
		QString li =QString::number (i);
		for(int j=0;j < nc; ++j) {
			li+="\t";
			li += model()->itemBBCode (model()->indexOfRow(i), j).toString();  
		}
		r.push_back(li);
	}	
	QApplication::clipboard()->setText (r.join("\n"));
}

void TableView::copyTable ()
{
	QStringList r; 
	int nc = model()->numColumns();
	int nr = model()->rowCount();
	QString head ("#");
	for (int i = 0;i < nc;++i) {
		head += "\t";
		head += model()->headerText(i);
	}		
	r.push_back(head);
	for (int i=0;i< nr; ++i) {
		QString li =QString::number (i);
		for(int j=0;j < nc; ++j) {
			li+="\t";
			li += model()->itemText (model()->indexOfRow(i), j).toString();  
		}
		r.push_back(li);
	}	
	QApplication::clipboard()->setText (r.join("\n"));
}
