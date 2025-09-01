#ifndef ___TOWNS_MODEL_H
#define ___TOWNS_MODEL_H
/* towns_model.h  - Town data model

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
#include "ui_towns2.h"

class TownModel : public TableModel
{
	Q_OBJECT

public:
	TownModel(QObject *parent = 0);

	void setCurrentPlayer (int);
	void setCurrentAlly (int);

	int           numColumns      () const;
	int           maxIndex        () const;

	bool          lessThan        (int a, int b, int col) const;
	QIcon         headerIcon      (int) const;
	QString       headerText      (int) const;
	QString       headerTooltip   (int) const;
	QVariant      itemDisplayData (int idx, int col) const;
	QVariant      itemText        (int idx, int col) const;
	QVariant      itemBBCode      (int idx, int col) const;
	QVariant      itemTooltip     (int idx, int col) const;
	void          renderItem      (int idx, int col, QPainter *painter, const QStyleOptionViewItem &option) const;
	QSize         itemSizeHint    (int idx, int col, const QStyleOptionViewItem &option) const;
	bool          itemCheckable   (int col) const;
	QVariant      itemDecoration  (int idx, int col) const;
	bool          itemChecked     (int idx, int col) const;
	void          setItemChecked  (int idx, int col, bool);

	int           defaultSortColumn () const {return 1;};
	Qt::SortOrder defaultSortOrder  ()  const {return Qt::AscendingOrder;};

	QByteArray    headerState       () const;
	void          saveHeaderState   (const QByteArray& b);

	void          sort ( int column, Qt::SortOrder order );
};

class TownView : public TableView {

	Q_OBJECT;

public:
	TownView ();
	QTableView *   table () {return ui._tab;}
	TableModel *   model () {return _model;};
	void fillContextMenu (QMenu *); 
	void handleCmd (const QString& key);

public slots:
	void setAlly (int i) ;
	void setPlayer (int i);
	void setGhosts () {setPlayer (-1);};
	void townTypeSelected (QAction* act);
	void settingChange ();
	void mapChange ();

private:
	TownModel *_model;
	QActionGroup *_type_ag;
	Ui::Towns2 ui;
};




















#endif