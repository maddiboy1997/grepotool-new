#ifndef __SELECTOR_H
#define __SELECTOR_H
/* selector.h - town and player selector

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

#include <QLineEdit>
#include <QListWidget>
#include <QAction>
#include <QToolBar>


class Selector : public QObject {
	Q_OBJECT;
public:
	Selector (QObject* p, bool show_towns);
	~Selector ();

	int selection () const;
	void addToToolbar (QToolBar* b);

	bool eventFilter(QObject *obj, QEvent *ev);


signals:
	void itemSelectedSignal (int);
	void gotoClickedSignal (int);

public slots:
	void updateList ();
	void itemSelected ( int c);
	void textEdited (const QString &);
	void itemActivated ( QListWidgetItem * item ); 

	void popupList ();
	void gotoClicked();


private:
	int          sel; 
	QIcon        ico;
	QLineEdit*   edit;
	QListWidget  *list;
	QAction *    goto_act;
	bool         towns; 
};

#endif
