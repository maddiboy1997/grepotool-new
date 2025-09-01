#ifndef ___TABLETOOLWIDGET_H
#define ___TABLETOOLWIDGET_H
/* tabletool.h  - tool windows with a table

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
#include "toolwidget.h"

#include <QTableWidget>
#include <QActionGroup>

class ColDef {
public:
	ColDef () {};
	ColDef (int tid, const QIcon &ic, const QString &nam, const QString &tp, const QString &wt, bool hid,bool mov, bool sort) 
		: type_id (tid), ico (ic), name (nam), tip (tp), what(wt), hidable (hid), movable (mov), sortable (sort) {};
	ColDef (const ColDef & o) {*this = o;};
	ColDef& operator = (const ColDef & o) {
		type_id= o.type_id;
		ico=o.ico;
		name=o.name;
		tip=o.tip;
		what = o.what;
		movable=o.movable;
		hidable=o.hidable;
		sortable = o.sortable;
		return *this;
	};
	int           type_id;
	QIcon         ico;
	QString       name;
	QString       tip;
	QString       what;
	bool          hidable;
	bool          movable;
	bool          sortable;
};

class TableTool : public ToolWidget  {
	Q_OBJECT;
public:
	virtual ~TableTool ();

	virtual void sortTable  (); // overwrite for custom sorting or to provide fixed sorting
	virtual bool extendedSelection () const {return false;};


	virtual int            defaultSortColumn () = 0;
	virtual Qt::SortOrder  defaultSortOrder () = 0;
	virtual QTableWidget * table() = 0;

	virtual int            rowCount         () = 0;
	virtual TableSetting * getTableSettings () = 0;
	virtual int            itemId           (int index) = 0;
	virtual void           updateCell       (int index, int col, const ColDef & d) = 0;
	virtual QString        itemBBCode       (int index, const ColDef &) = 0;
	virtual QString        itemText         (int index, const ColDef &) = 0;

	virtual void           currentTableChanged () {};
	virtual void           updateCellData   (int, int , const ColDef & ) {};
	virtual void           setItemCheckState (int, bool) {};

	virtual void           showContextMenu     (const QPoint&) {};
	virtual void           showItemContextMenu (const QPoint&, int /*logicalIndex*/, const ColDef &) {};

	virtual bool           isEditable() const {return false;};
	virtual QStringList    tableNames () {return QStringList();};

	virtual void delSel(){};

	// be sure to call the TableTool:: funcs inside overrides!
	void initWidget ();
	void worldChanged ();
	void worldAboutToChange ();

	bool event ( QEvent * ev ) ;

	virtual void updateCommands () ;
	virtual void updateContent ();
	virtual void handleCmd (const QString& key);


public slots:
	void saveSelection ();
	void restoreSelection  ();
	void updateTable ();
	void cellChanged (int, int);       // item check state changed
	void cellClicked (int, int);       // 
	void cellDoubleClicked (int, int); 
	void selectionChanged ();
	void contextMenuRequested (const QPoint&);

	void sectionMoved ( int , int , int ); 
	void sectionResized (  ); 


	void headerClicked(int);
	void colVisibilityChanged(QAction*);

	void rootIndexChanged ();
	void setRootIndex (const QVariant &);

	void selectIndex (const QVector <int> &);
	void selectIndex (int);

	void itemDataChanged ();

	void copyBBCode ();
	void copyTable ();

protected:
	QStringList    itemHeaders       ();

	QList <int> selection ();

	void setRootIndexSetting (const QString & s);
	void fillHeaderMenu (QMenu*m , bool add_sm=false);
	TableTool (const QString& id, const QString& ti);
	int  itemIndex  (int row);
	QVariant rootIndex  () const {return root_index;};
	void addColDef (const ColDef & cd);

	QVector <ColDef> colDefs;
	QActionGroup *vis_ag;

	QString  root_index_name;
	QVariant root_index;
};





#endif