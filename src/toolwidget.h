#ifndef ___TOOLWIDGET_H
#define ___TOOLWIDGET_H
/* toolwidget.h  - tool windows

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
#include "window.h"
#include "world_data.h"

class QMenu;

class ToolWidget : public Window {
	Q_OBJECT;
public:
	ToolWidget (const QString& id, const QString& ti);
	virtual ~ToolWidget ();

	QString title () const {return _title;};

	virtual void fillItemContextMenu (QMenu*m, int) {fillContextMenu (m);};
	virtual void fillContextMenu (QMenu*) {};

	virtual void createSettingsSubMenu (QMenu*, bool ) {};
	virtual void updateCommands () ;
	virtual void updateContent ();
	virtual void handleCmd (const QString& key);

signals:
	void contextMenuSignal     (QWidget* w, const QPoint &globalPos);
	void itemContextMenuSignal (QWidget* w, const QPoint &globalPos, int item_index);
	void itemClickSignal       (QWidget* w, int item_index);
	void itemDoubleClickSignal (QWidget* w, int item_index);
	void itemDataChangedSignal (QWidget* w, int item_index);
	void selectionChangedSignal (QWidget* w);

	void itemClickSignal       (int item_index);
	void itemDoubleClickSignal (int item_index);
	void itemDataChangedSignal (int item_index);
	void selectionChangedSignal ();

	// 
	void townDataChangedSignal (int town_index); // specials & unitinfo
	void allyDataChangedSignal (int ally_index); // color & selection
	void playerDataChangedSignal (int player_index); 
	void myTownChangedSignal (int list_no);
	void targetChangedSignal (int list_no);
	void globalSelectTownSignal (int list_no);
	void planChangedSignal (int list_no);
	void updateInfMapSignal ();
	void updateMapSignal ();
	void gotoTownSignal (int);

public slots: 


protected:
	QString   _title;
};



#endif