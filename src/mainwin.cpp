/* mainwin.cpp - main window

Copyright (c) 2013 P. Vorpahl

This file is part of Grepotool, a free utility for the browsergame 
Grepolis which is the intellectual property of InnoGames GmbH.

Grepotool is free Software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the Free 
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

#include "mainwin.h"
#include "mapview.h"
#include "ally_model.h"
#include "town_model.h"
#include "player_model.h"
//#include "tab_players.h"
#include "tab_mytowns.h"
#include "tab_targets.h"
#include "tab_plans.h"
#include "town_info.h"
#include "app.h"
#include "json.h"
#include "world_data.h"
#include "ui_dlg_about.h"
#include "command.h"
#include "command_ids.h"
#include "server_info.h"
#include "selector.h"

#include <QToolBar>
#include <QMenuBar>
#include <QScrollBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QDockWidget>

MainWin * MainWin::instance = 0;


MainWin::MainWin () : Window ("MainWindow")
{
	instance = this;
	setMinimumSize(800,600);

	Setting::connect (CURRENT_WORLD, this,SLOT(restoreMainState()));
	Setting::connect2 (CURRENT_WORLD, this,SLOT(storeMainState()));

	connect (App::instance, SIGNAL(worldLoadErrorSignal (const QString &,const QString &, const QString &)),
		this, SLOT (loaderError (const QString &,const QString &,const QString &)));

	connect (App::instance, SIGNAL(worldLoadProgressSignal (int, const QString&)),
		this, SLOT (loadProgress (int, const QString&)));

	connect(App::clipboard(), SIGNAL(changed (QClipboard::Mode)), this, SLOT(clipboardChanged (QClipboard::Mode)));

	// Just a hack to initialize the connection.
	App::clipboard()->setText("");

	prog = new QProgressBar;
	prog->setRange  (0,100);

	meshlab = new QLabel;
	QStatusBar *sp = new QStatusBar(this);
	sp->addPermanentWidget (meshlab);
	sp->addPermanentWidget (prog);

	townsel = new Selector(this,true);
	playersel = new Selector(this,false);

	connect (playersel,SIGNAL(itemSelectedSignal(int)), this, SLOT(selectPlayer(int)));
	connect (townsel,SIGNAL(itemSelectedSignal(int)), this, SLOT(selectTown(int)));
	connect (townsel,SIGNAL(gotoClickedSignal (int)), this, SLOT(zoomToTown(int)));

	setStatusBar (sp);
	QList<int>sl;
	sl.push_back(200);
	sl.push_back(400);

 }

void MainWin::meshProgress (int v)
{
	if (v == 1000) {
		meshlab->setText("ok");
	} else {
		int dec = v%10;
		int nv =  v/10;
		meshlab->setText(tr("Meshing %1.%2%").arg(nv).arg(dec));
	}
}


void MainWin::handleCmd (const QString& /*key*/)
{
}

QPoint MainWin::currentMapOffset () {return instance->mapview->_view->currentOffset();};
int MainWin::currentMapZoomLevel () {return instance->mapview->_view->currentZoomLevel();};

void MainWin::clipboardChanged (QClipboard::Mode mode)
{
	if (mode != QClipboard::Clipboard) return ;
	int r = Cmd::worldData().handleReport (App::clipboard()->text());
	if (r<0) return;
	emit townInfoChangedSignal (r);
	App::clipboard()->clear(mode);
	townview->updateTable ();
	town_info->selectTown (r);
}

void MainWin::restoreMainState ()
{
	townsel->updateList ();
	playersel->updateList ();

	for (int i=0;i<tw_list.size();++i) 
		tw_list[i]->worldChanged();
	QSettings s (appTitle(), appCfgKey());	
	QString glob ("Global/MainWindow/") ;
	QString srv = Cmd::worldData().server();
	QString wrld = srv+"/MainWindow/";

	QString basekey = glob;
	if(!srv.isEmpty() && Setting::value(STORE_SETTINGS_PER_WORLD).toBool()) {
		basekey = wrld;
	}
	bool mx = s.value(basekey+"WindowMaximized").toBool();
	if (mx) {
		setWindowState(windowState() | Qt::WindowMaximized);
	} else {
		restoreGeometry(s.value(basekey+"WindowGeometry").toByteArray());
	}
	restoreState(s.value(basekey+"WindowState").toByteArray());
	
	prog->hide();
	statusBar()->showMessage (tr("World load ok"));
}

void MainWin::storeMainState ()
{
	QString srv = Cmd::worldData().server();
	if (srv.isEmpty()) return;
	for (int i=0;i<tw_list.size();++i) 
		tw_list[i]->worldAboutToChange();

	QSettings s (appTitle(), appCfgKey());	
	QString glob ("Global/MainWindow/") ;
	QString wrld = srv+"/MainWindow/";
	s.setValue(glob+"WindowGeometry", saveGeometry());
	s.setValue(glob+"WindowState", saveState());
	s.setValue(glob+"WindowMaximized", isMaximized());
	s.setValue(wrld+"WindowGeometry", saveGeometry());
	s.setValue(wrld+"WindowState", saveState());
	s.setValue(wrld+"WindowMaximized", isMaximized());
}



void MainWin::createWidgets ()
{
	mapview = new MapWindow;
	setCentralWidget (mapview);
	registerToolWidget (mapview, Qt::NoDockWidgetArea);
	allys = new AllyView;
	registerToolWidget (allys, Qt::LeftDockWidgetArea);

	players = new PlayerView;
	registerToolWidget (players, Qt::LeftDockWidgetArea);

	mytowns = new TabMyTowns;
	registerToolWidget (mytowns, Qt::BottomDockWidgetArea);
	targets = new TabTargets;
	registerToolWidget (targets, Qt::BottomDockWidgetArea);

	townview = new TownView;
	registerToolWidget (townview, Qt::RightDockWidgetArea);

	town_info = new TownInfo;
	registerToolWidget (town_info, Qt::BottomDockWidgetArea);
}


void MainWin::selectTown (int i)
{
	if(i<0) return;
	WorldData&w= Cmd::worldData();

	int al = w.towns()[i].ally;
	int pl = w.towns()[i].player;
	if (al>=0) {
		allys->selectIndex(al);
		players->setAlly (al);
		players->selectIndex(pl);
	} else if (pl>=0) {
		players->setAlly (al);
		players->selectIndex(pl);
	}
	townview->setPlayer (pl);
	townview->selectIndex (i);
	town_info->selectTown(i);
	mytowns->setTargetTown(i);
}

void MainWin::selectPlayer (int i)
{
	if(i<0) return;
	WorldData&w= Cmd::worldData();
	const Player& pl = w.players()[i];
	int al = pl.ally;
	if (al>=0) {
		allys->selectIndex(al);
	}
	players->setAlly (al);
	players->selectIndex(i);
	townview->setPlayer (i);
}



void MainWin::zoomToTown (int i)
{
	if(i<0) return;
	WorldData & w= Cmd::worldData();
	QPoint p = w.towns()[i].pos();
	int dzl = Setting::value (MAPVIEW_GOTO_ZOOM).toInt();
	mapview->_view->zoomTo(p,dzl);
}

void MainWin::scrollToTown (int i)
{
	if(i<0) return;
	WorldData & w= Cmd::worldData();
	QPoint p = w.towns()[i].pos();
	mapview->_view->scrollTo(p);
}

void MainWin::addTownToMyTowns (int i)
{
	if(i<0) return;
	mytowns->addTown(i);

}

void MainWin::addTownToTargets (int i)
{
	if(i<0) return;
	targets->addTown(i);
}

void MainWin::addMyTownToPlans (int i)
{
	if(i<0) return;
}
void MainWin::addTargetToPlans (int i)
{
	if(i<0) return;
}



void MainWin::registerToolBar (QToolBar * b, const QString& s)
{
	b->setObjectName (s);
	tb_list.push_back (b);
}


void MainWin::registerToolWidget (ToolWidget * w, Qt::DockWidgetArea dw)
{
	connect (w, SIGNAL(contextMenuSignal (QWidget*, const QPoint &)),this,SLOT(contextMenu(QWidget*, const QPoint &)));
	connect (w, SIGNAL(itemContextMenuSignal (QWidget*, const QPoint &, int)),this,SLOT(itemContextMenu (QWidget*, const QPoint &, int)));
	connect (w, SIGNAL(itemClickSignal (QWidget*, int)),this,SLOT(itemClick (QWidget*, int)));
	connect (w, SIGNAL(itemDoubleClickSignal (QWidget*, int)),this,SLOT(itemDoubleClick (QWidget*, int)));
	connect (w, SIGNAL(itemDataChangedSignal (QWidget*, int)),this,SLOT(itemDataChanged (QWidget*, int)));
	connect (w, SIGNAL(selectionChangedSignal (QWidget*)),this,SLOT(selectionChanged (QWidget*)));
	connect (w, SIGNAL(globalSelectTownSignal(int)), this, SLOT(selectTown(int)));
	connect (w, SIGNAL(gotoTownSignal(int)), this, SLOT(zoomToTown(int)));
	tw_list.push_back (w);
	if (dw == Qt::NoDockWidgetArea) return;
	QDockWidget * d = new QDockWidget (w->title(), this);
	d->setObjectName(w->title());
	addDockWidget(dw, d);
	d->setWidget (w);
	dw_list.push_back (d);
	d->setObjectName (w->globalWidgetKey());
}


void MainWin::createCommands ()
{
}

void MainWin::createMenus ()
{
	QMenuBar * mb = menuBar();

	QMenu * f = mb->addMenu (tr("File"));
	f->addAction(tr("Exit"), this, SLOT(close()));
	
	QToolBar * stb = new QToolBar(tr("Server"));
	ServerList::instance->addToToolBar (stb);
	registerToolBar (stb, "Server");

	QAction * srv = mb->addMenu (ServerList::instance->serverMenu());
	srv->setText(tr("Server"));

	QMenu * vm = mb->addMenu (tr("View"));


	QMenu * sett = mb->addMenu (tr("Settings"));
	for(int i=0;i< tw_list.size();++i) {
		tw_list[i]->createSettingsSubMenu (sett,true);
	}

	QMenu * mapp = sett->addMenu (tr("Application"));
	Cmd::get(STORE_SETTINGS_PER_WORLD)->addToMenu(mapp);

	QMenu * h = mb->addMenu (tr("Help"));
	h->addAction(tr("About"), this, SLOT(about()));
	

	QToolBar * mtb = new QToolBar (tr("Mapview toolbar"));	

	Cmd::get(MAPVIEW_GLOBAL_ZOOM)->addToToolBar(mtb);
	mtb->addSeparator();

	xpos = new QSpinBox;
	ypos = new QSpinBox;
	xpos->setRange(0,1000);
	ypos->setRange(0,1000);
	mtb->addWidget(new QLabel("<b>X:</b>"));
	mtb->addWidget (xpos);
	mtb->addWidget(new QLabel("<b>Y:</b>"));
	mtb->addWidget (ypos);
	mtb->addAction (QIcon(":/icons/coord_goto.ico"), tr("Go to coords"), this, SLOT(gotoCoords()));
	Setting::connect(MAPVIEW_GLOBAL_OFFSET, this, SLOT(offsetChanged()));

	registerToolBar (mtb, "NavToolbar");


	QToolBar * srtb = new QToolBar (tr("Search toolbar"));	
	townsel->addToToolbar (srtb);
	mtb->addSeparator();
	playersel->addToToolbar (srtb);
	registerToolBar (srtb, "SearchToolbar");

	// finally populate view menu and add registered toolbars
	for (int i=0; i < dw_list.size(); ++i) vm->addAction (dw_list[i]->toggleViewAction());
	vm->addSeparator();
	for (int i=0; i < tb_list.size(); ++i) {
		addToolBar(tb_list[i]);
		vm->addAction (tb_list[i]->toggleViewAction());
	}

}

void MainWin::offsetChanged ()
{	
	QPoint p = Setting::value(MAPVIEW_GLOBAL_OFFSET).value<QPoint>();
	int x = p.x() / 128;
	int y = p.y() / 128;
	if(x<0) x = 0;
	if(y<0) y = 0;
	if(x>999) x = 999;
	if(y>999) y = 999;
	xpos->setValue(x);
	ypos->setValue(y);
	// towns->itemDataChanged();  
}


void MainWin::gotoCoords ()
{
	QPoint p(xpos->value()*128,ypos->value()*128);
	mapview->_view->scrollTo (p);
}


void MainWin::createConnections ()
{

	connect (mytowns, SIGNAL(currentSelectionChangedSignal(int,UnitIndex)),
		targets,SLOT(setTargetTown (int, UnitIndex)));

	connect (targets, SIGNAL(currentSelectionChangedSignal(int)),
		mytowns,SLOT(setTargetTown (int)));

	for (int i=0;i < tw_list.size();++i) {
		connect (tw_list[i],SIGNAL (updateInfMapSignal()), mapview->_view, SLOT(updateInfMap())); 
		connect (tw_list[i],SIGNAL (updateMapSignal()), mapview->_view, SLOT(updateMap())); 

		connect (tw_list[i],SIGNAL (townDataChangedSignal(int)), town_info, SLOT(selectTown(int))); 
		connect (tw_list[i],SIGNAL (townDataChangedSignal(int)), townview, SLOT(updateTable()));  
		connect (tw_list[i],SIGNAL (myTownChangedSignal(int)), mytowns, SLOT(updateTable())); 
		connect (tw_list[i],SIGNAL (targetChangedSignal(int)), targets, SLOT(updateTable())); 
		//connect (tw_list[i],SIGNAL (planChangedSignal(int)), plans, SLOT(updateMap())); 
	}

}





void MainWin::contextMenu     (QWidget* w, const QPoint &globalPos)
{
	ToolWidget* t = dynamic_cast<ToolWidget*>(w);
	QMenu m;
	t->fillContextMenu(&m);
	if (w == mapview) {
	} else if (w == allys) {
	} else if (w == players) {
	} else if (w == townview) {
	} else if (w == mytowns) {
	} else if (w == targets) {
	} else if (w == plans) {
	} else {
	}
	m.exec (globalPos);
}

void MainWin::itemContextMenu (QWidget* w, const QPoint &globalPos, int item_index)
{
	//void selectTown (int);
	//void zoomToTown (int);
	//void scrollToTown (int);
	//void addTownToMyTowns (int);
	//void addTownToTargets (int);

	int idx = item_index;
	ToolWidget* t = dynamic_cast<ToolWidget*>(w);
	QMenu m;
	if (w == mapview) {
		new ActIndex (m.addAction (tr("Select town")),idx,this, SLOT(selectTown (int)));
		new ActIndex (m.addAction (tr("Add to My Towns")),idx,this, SLOT(addTownToMyTowns (int)));
		new ActIndex (m.addAction (tr("Add to Targets")),idx,this, SLOT(addTownToTargets (int)));
	} else if (w == allys) {
	} else if (w == players) {
	} else if (w == mytowns) {
		new ActIndex (m.addAction (tr("Scroll to town")),idx,this, SLOT(scrollToTown (int)));
		new ActIndex (m.addAction (tr("Zoom to town")),idx,this, SLOT(zoomToTown (int)));
		//new ActIndex (m.addAction (tr("Add to Plans")),idx,this, SLOT(addMyTownToPlans (int)));
	} else if (w == targets) {
		new ActIndex (m.addAction (tr("Scroll to town")),idx,this, SLOT(scrollToTown (int)));
		new ActIndex (m.addAction (tr("Zoom to town")),idx,this, SLOT(zoomToTown (int)));
		//new ActIndex (m.addAction (tr("Add to Plans")),idx,this, SLOT(addTargetToPlans (int)));
	} else if (w == plans) {
	} else {
	}
	m.addSeparator();
	t->fillContextMenu(&m);
	m.exec (globalPos);
}

void MainWin::itemClick       (QWidget* w, int item_index)
{
	if (w == mapview) {
	} else if (w == allys) {
		players->setAlly (item_index);
		townview->setAlly (item_index);
	} else if (w == players) {
		townview->setPlayer (item_index);
	} else if (w == townview) {

		town_info->selectTown (item_index);
		mytowns->setTargetTown (item_index);


	} else if (w == mytowns) {

		int ti =  mytowns->currentList().l[item_index].town_index;
		town_info->selectTown (Cmd::worldData().townIndex(ti));
		
	} else if (w == targets) {

		int ti =  targets->currentList().l[item_index].town_index;
		town_info->selectTown (Cmd::worldData().townIndex(ti));
		

	} else if (w == plans) {

	} else {
	}
}

void MainWin::itemDoubleClick (QWidget* w, int item_index)
{
	if (w == mapview) {

		selectTown(item_index);
		town_info->selectTown (item_index);
		mytowns->setTargetTown (item_index);

	} else if (w == allys) {
	} else if (w == players) {
	} else if (w == mytowns) {
		
		int ti =  mytowns->currentList().l[item_index].town_index;
		town_info->selectTown (Cmd::worldData().townIndex(ti));
		zoomToTown(Cmd::worldData().townIndex(ti));

	} else if (w == targets) {

		int ti =  targets->currentList().l[item_index].town_index;
		town_info->selectTown (Cmd::worldData().townIndex(ti));
		zoomToTown(Cmd::worldData().townIndex(ti));

	} else if (w == plans) {
	} else {
	}
}

void MainWin::itemDataChanged (QWidget* w, int item_index)
{
	if (w == mapview) {
	} else if (w == town_info) {
		mytowns->itemDataChanged();
		townview->updateTable();


	} else if (w == allys) {
	} else if (w == players) {
	} else if (w == mytowns) {
			
		town_info->selectTown (item_index);
		townview->updateTable();

	} else if (w == targets) {
	} else if (w == plans) {
	} else {
	}
}

void MainWin::selectionChanged (QWidget* w)
{
	if (w == mapview) {
	} else if (w == allys) {
	} else if (w == players) {
	} else if (w == mytowns) {
	} else if (w == targets) {
	} else if (w == plans) {
	} else {
	}
}

void MainWin::worldChanged ()
{
	if (prog->isVisible()) {
		prog->hide();
	}
	statusBar()->showMessage (tr("World load ok"));
	// restoreWindowState ();
}

void MainWin::loadProgress (int p, const QString &m)
{
	if(!prog->isVisible()) {
		prog->show();
	}
	prog->setValue (p);
	statusBar()->showMessage (m);	
}

void MainWin::moveEvent ( QMoveEvent * ev )
{
	//if(_is_init) storeMainState();
	Window::moveEvent ( ev );
}
void MainWin::resizeEvent ( QResizeEvent * ev )
{
	// if(_is_init) storeMainState();
	Window::resizeEvent ( ev );
}


void MainWin::loaderError (const QString &file,const QString &desc, const QString &msg)
{
	prog->hide();
	QMessageBox msgBox;
	msgBox.setText(msg);
	msgBox.setInformativeText (desc);
	msgBox.setDetailedText (file);
	msgBox.exec();
}


void MainWin::closeEvent(QCloseEvent* ev)
{
	App::instance->cancelLoading ();

	// storeMainState ();
	
	WorldData d;
	Setting::setWorldData (d);
	Window::closeEvent(ev);
}


MainWin::~MainWin () 
{
}

void MainWin::about ()
{
	Ui::DlgAbout ui;
	QDialog dlg;
	ui.setupUi (&dlg);
	dlg.setWindowTitle (QString("Grepotool %1\n").arg (appVersionInfo()));
	ui._text_2->verticalScrollBar()->setSingleStep(8);	
	dlg.exec ();
}



