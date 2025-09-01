/* app.cpp  - application class

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
#include "app.h"
#include "splash.h"
#include "setting.h"
#include "command_ids.h"
#include "command.h"
#include "world_data.h"

#include "dataloader.h"
#include "mainwin.h"
#include "world_data.h"
#include "QSplashScreen"
#include "ui_dlg_about.h"
#include "server_info.h"





App * App::instance(0);

App::App (int argc, char ** argv):QApplication (argc,argv) , trans(0),worldload(0)
{
	instance = this;

	new ServerList (this);

	new Setting (this, CURRENT_LANGUAGE, QString ("de"));

    new Setting (this, GLOBAL_DATADIR, applicationDirPath()+"/data");
	new CmdCheck (this, STORE_SETTINGS_PER_WORLD, false, tr("Store window layout per world"), tr("Store window layout per world tt"), tr("Store window layout per world wt"));

	setApplicationName (appTitle());
	setOrganizationName (appTitle());

	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	QString locale = QLocale::system().name().left(2);
	locale = Setting::value (CURRENT_LANGUAGE).toString();
	if (locale.isEmpty()) 
		locale = QLocale::system().name().left(2);
	setLanguage (locale);

	global.init();
}

App::~App () 
{
	if(worldload && worldload->isRunning()) 
		worldload->abort();
}



void App::cancelLoading ()
{
	if(worldload && worldload->isRunning()) 
		worldload->abort();
}


void App::setLanguage (const QString & s)
{
	if(!s.isEmpty() && s == language) return;
	QString l=s;
	QTranslator * ntrans = new QTranslator;
	QString fn = "Grepotool_"+ l;
	if (ntrans->load (fn)) {
		if (trans) {
			this->removeTranslator (trans);
		}		
		installTranslator (ntrans);
		if (trans) {
			delete trans;
			trans = ntrans;
		}
		language = l;
	} else {
		delete ntrans;
	}
	Setting::setValue (CURRENT_LANGUAGE, language);
}


QString App::dataDir()
{
	QString dir = applicationDirPath()+"/data"; // Setting::value(DATADIR).toString();
	QDir dad (dir);
	if (dad.exists()) return dir;
	if (!dad.mkdir (dir)) {

		// obtain (platform specific) application's data/settings directory
		/*
		QSettings ini(QSettings::IniFormat, QSettings::UserScope,
			QCoreApplication::organizationName(),
			QCoreApplication::applicationName());
		dir = QFileInfo(ini.fileName()).absolutePath()+"/data";
		Setting::setValue (DATADIR, dir);
		QDir dadi (dir);
		if (!dadi.exists()) dadi.mkdir(dir);
		*/
	}
	return dir;		
}


void App::exitProgram () 
{
	QMutexLocker ml(&mutex);
	splash_is_blocking = false;
}

QString App::worldDataDir(const QString &server)
{
	return dataDir()+"/"+server;
}



int App::run ()
{
	qRegisterMetaType<QImage>("QImage");
	qRegisterMetaType<QPoint>("QPoint");
	qRegisterMetaType<QColor>("QColor");
	qRegisterMetaType<QPointF>("QPointF");
	qRegisterMetaType<QPolygonF>("QPolygonF");
	qRegisterMetaType< QVector<QPoint> >("QVector<QPoint>");
	qRegisterMetaType< QVector<QColor> >("QVector<QColor>");
	qRegisterMetaType< QVector<QPointF> >("QVector<QPointF>");
	qRegisterMetaType< QVector<QPolygonF> >("QVector<QPolygonF>");
	qRegisterMetaType<QPixmap>("QPixmap");

	//setQuitOnLastWindowClosed(false);
	splash = new Splash ;
	connect (splash, SIGNAL (acceptedSignal()),
		this, SLOT(exitProgram()));
	splash->show();

	processEvents ();

	QDir dad (dataDir());
	if (!dad.exists()) {
		splash_is_blocking = true;
		splash->showError (tr("Failed to create data directory"),
			tr("Please reinstall Grepotool into a writable directory."),"");
		mutex.lock();
		while (splash_is_blocking) {
			mutex.unlock();
			processEvents();
			mutex.lock();
		}
		mutex.unlock();
		return 0;
	}
	
	processEvents();
	
	GrepoServer * srv = ServerList::instance->find(Setting::value (CURRENT_WORLD).toString());

	worldload = new WorldDataLoader(0, global, srv->serverName(), srv->worldName());

	QDir wdir (worldDataDir(srv->serverName()));
	if (!wdir.exists()) wdir.mkdir (worldDataDir(srv->serverName()));

	world_load_complete = false;   
	world_load_error = false;   

	connect (worldload,SIGNAL(progressSignal(int,const QString&)), 
		splash, SLOT(loadWorldProgress(int,const QString&)),
		Qt::QueuedConnection);
	connect (worldload,SIGNAL(completedSignal(bool)), 
		this, SLOT(initialWorldLoadComplete(bool)),
		Qt::QueuedConnection);
	worldload->loadSync(false);
	processEvents();
	mutex.lock();
	while (!world_load_complete) {
		mutex.unlock();
		processEvents ();
		mutex.lock();
	}
	mutex.unlock();
	if (world_load_error) {
		splash_is_blocking = true;
		splash->showError (worldload->errorTitle(),worldload->errorDescription(),worldload->errorDetails());
		mutex.lock();
		while (splash_is_blocking) {
			mutex.unlock();
			processEvents();
			mutex.lock();
		}
		mutex.unlock();
		delete worldload;
		worldload = 0;
		return 0;
	}

	MainWin mw;
	splash->finish(&mw);	
	
	mw.createWidgets ();
	mw.createCommands ();
	mw.createMenus ();
	mw.createConnections ();

	processEvents();

	Cmd::setWorld(worldload->data);
	delete worldload;
	worldload = 0;
	processEvents();

	ServerList::instance->updateServerMenu();

	mw.show ();

	return exec ();
}




void App::aquireNewWorld ( QString s, QString w, bool force_download)
{
	if (worldload) return;

	QDir dadg (worldDataDir(s));
	if (!dadg.exists()) dadg.mkdir (worldDataDir(s));
	worldload = new WorldDataLoader (0, global,s,w);
	
	world_load_complete = false;   
	world_load_error = false;   

	connect (worldload,SIGNAL(progressSignal(int,const QString&)), 
		this, SLOT(worldLoadProgress(int,const QString&)),
		Qt::QueuedConnection);

	connect (worldload,SIGNAL(completedSignal(bool)), 
		this, SLOT(worldLoadComplete(bool)),
		Qt::QueuedConnection);

	worldload->loadAsync (force_download);
	processEvents();
	mutex.lock();
	while (!world_load_complete) {
		mutex.unlock();
		processEvents ();
		mutex.lock();
	}
	delete worldload;
	worldload = 0;
	mutex.unlock();
}


void App::worldLoadProgress (int perc,  const QString& msg)
{
	emit worldLoadProgressSignal(perc, msg);	
}

void App::worldLoadComplete (bool err)
{
	mutex.lock();
	world_load_error = err;			
	world_load_complete = true;
	mutex.unlock();
	if (err) {
		emit worldLoadErrorSignal (worldload->errorTitle(),worldload->errorDescription(),worldload->errorDetails());
	} else {
		Cmd::setWorld(worldload->data);
	}
}

void App::initialWorldLoadComplete (bool err)
{
	mutex.lock();
	world_load_error = err;			
	world_load_complete = true;
	mutex.unlock();
}


