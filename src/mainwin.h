#ifndef ___MAINWIN_H
#define ___MAINWIN_H
/* mainwin.h  - main window

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

#include "defines.h"
#include "window.h"
#include <QAction>
#include <QSpinBox>
#include <QLabel>
#include <QClipboard>
#include <QProgressBar>

class ToolWidget;

class PlayerView;
class TownView;
class AllyView;
class TabMyTowns;
class TabTargets;
class TabPlans;

class TownInfo;

class MapView;
class MapWindow;
class WorldData;
class Selector;

class ActIndex : public QObject {
	Q_OBJECT;
public:
	ActIndex ( QAction* a, int idx, QObject *par, const char * slot):QObject (a),index(idx){
		connect (a, SIGNAL(triggered()), this, SLOT(actTriggered()));
		connect (this, SIGNAL(triggered(int)), par, slot);
	}; 

public slots:
	void actTriggered() {emit triggered(index);};

signals:
	void triggered (int);

private:
	int index;
};


class MainWin : public Window {
	Q_OBJECT;
public:
	MainWin () ;
	~MainWin () ;

	void worldChanged ();

signals:
	void townInfoChangedSignal (int); // after a report was recaived successfully

public slots:
	void restoreMainState ();
	void storeMainState ();

	void about ();

	void createWidgets ();
	void createCommands ();
	void createMenus ();
	void createConnections ();

	void loadProgress (int, const QString &);
	void loaderError (const QString &file,const QString &desc,const QString &msg);

	void clipboardChanged (QClipboard::Mode mode);

	void contextMenu     (QWidget* w, const QPoint &globalPos);
	void itemContextMenu (QWidget* w, const QPoint &globalPos, int item_index);
	void itemClick       (QWidget* w, int item_index);
	void itemDoubleClick (QWidget* w, int item_index);
	void itemDataChanged (QWidget* w, int item_index);
	void selectionChanged (QWidget* w);

	void offsetChanged ();
	void gotoCoords ();
	void selectTown (int);
	void selectPlayer (int);
	void zoomToTown (int);
	void scrollToTown (int);
	void addTownToMyTowns (int);
	void addTownToTargets (int);
	void addTargetToPlans (int);
	void addMyTownToPlans (int);

	void handleCmd (const QString& key);

	void meshProgress (int);
protected:

	void closeEvent(QCloseEvent*);

	void	moveEvent ( QMoveEvent * ev );

	void	resizeEvent ( QResizeEvent * ev );

	void registerToolWidget (ToolWidget * w, Qt::DockWidgetArea dw);
	void registerToolBar (QToolBar * b, const QString& s);

public:
	static MainWin * instance;
	static QPoint currentMapOffset ();
	static int currentMapZoomLevel ();

	MapWindow  * mapview;

	AllyView   * allys;
	TownView   * townview;
	PlayerView * players;

	TabMyTowns * mytowns;
	TabTargets * targets;
	TabPlans   * plans;

	TownInfo * town_info;

	QProgressBar *prog;
	QVector <QDockWidget *> dw_list;
	QVector <ToolWidget *> tw_list;
	QVector <QToolBar *> tb_list;

	Selector * townsel;
	Selector * playersel;

	QSpinBox *xpos;
	QSpinBox *ypos;
	QLabel*  meshlab;
	
};


#endif
