#ifndef ___TABLE_MODEL_H
#define ___TABLE_MODEL_H
/* table_model.h  - Table model view architecture

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

#include <QTableView>
#include <QAbstractTableModel>
#include <QAbstractItemDelegate>
#include <QFontMetrics>
#include <QModelIndex>
#include <QSize>
#include <QActionGroup>

class QMenu;

class TableModel : public QAbstractTableModel {
	Q_OBJECT
public:
	virtual ~TableModel ();

	int          columnCount(const QModelIndex &parent = QModelIndex()) const;
	int          rowCount ( const QModelIndex & idx = QModelIndex()) const;
	virtual void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );
	QVariant     data ( const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant     headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	void         resetIndex        (const QVector <int> & nidx, const QVariant& tabroot);
	void         setItemCheckState (const QModelIndex & index, Qt::CheckState state);
	bool         customEditOfItems (const QModelIndexList & idx);
	void         fillContextMenu   (QMenu* m, const QModelIndexList & idx);

	void            storeSelection (const QModelIndexList & idx);
	QModelIndexList restoreSelection ()  const;

	QSize        iconSize    () const;
	void         setIconSize (const QSize & s);

	int          rowOfIndex  (int idx) const;
	int          indexOfRow  (int row) const;
	QModelIndexList rowsOfItems (const QVector<int>&idx)  const;

	virtual int               numColumns        () const = 0;
	virtual int               defaultSortColumn () const = 0;
	virtual Qt::SortOrder     defaultSortOrder  () const = 0;
	virtual int               maxIndex          () const = 0;

	virtual bool              canHideColumn     (int) const;
	virtual bool              canMoveColumn     (int) const;
	virtual bool              canSortByColumn   (int) const;
	virtual QIcon             headerIcon        (int) const = 0;
	virtual QString           headerText        (int) const = 0;
	virtual QString           headerTooltip     (int) const = 0;
	virtual Qt::AlignmentFlag itemAlignment     (int col) const;

	virtual bool              lessThan        (int a, int b, int col) const;
	virtual QVariant          itemDisplayData (int idx, int col) const = 0;
	virtual QVariant          itemText        (int idx, int col) const = 0;
	virtual QVariant          itemDecoration  (int idx, int col) const = 0;
	virtual QVariant          itemBBCode      (int idx, int col) const = 0;
	virtual QVariant          itemTooltip     (int idx, int col) const = 0;
	virtual bool              itemCheckable   (int col) const;
	virtual bool              itemChecked     (int idx, int col) const;
	virtual void              setItemChecked  (int idx, int col, bool);

	virtual QByteArray        headerState     () const = 0;
	virtual void              saveHeaderState (const QByteArray& b) = 0;
	virtual bool              itemCustomEdit  (const QVector<int>&idx, int col);
	virtual void              fillContextMenu (QMenu* m,const QVector<int>&idx);
	
public slots:
	void         triggerRedraw     () {emit layoutChanged ();};

protected:
	TableModel(QObject *parent = 0);

protected:
	QVariant      _tab_no;
	QVector <int> _idx;
	QVector <bool>_sel;
	QSize         _icon_size;
};

class ItemDelegate : public QAbstractItemDelegate {
	Q_OBJECT
public:
	ItemDelegate(QObject *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index ) const;
	bool editorEvent ( QEvent * event, QAbstractItemModel * model, 
		const QStyleOptionViewItem & option, const QModelIndex & index );
};

class TableView : public ToolWidget {
	Q_OBJECT;
public:
	virtual ~TableView ();
	void     initTable ();

	QVector <int> selectedItems () ;

	virtual QTableView *   table () = 0;
	virtual TableModel *   model () = 0;

	virtual void fillContextMenu (QMenu *) {}; 

	virtual void delSel () {};
	virtual bool canDelSel () const {return false;};

public slots:
	void contextMenuRequested(const QPoint&);
	void headerClicked(int);
	void sectionMoved(int,int,int);
	void sectionResized();
	void colVisibilityChanged(QAction* act);
	void updateTable ();
	void copyBBCode ();
	void copyTable ();
	void selectIndex (const QVector <int> &);
	void selectIndex (int);

	void saveHeaderState();
	void restoreHeaderState();

	void itemActivated ( const QModelIndex & index );
	void itemClicked ( const QModelIndex & index );
	void itemDoubleClicked ( const QModelIndex & index );
	void itemSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );

protected:
	TableView (const QString& id, const QString& title);
	bool event (QEvent *ev);
	void sortModel (int column=-1);
	QActionGroup *vis_ag;
};

template <class T> class TableT : public TableModel {
public :	
	virtual ~TableT () {};

protected:
	TableT (QObject *parent = 0) : TableModel (parent) {};
};



#endif