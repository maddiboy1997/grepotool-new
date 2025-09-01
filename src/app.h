#ifndef __APP_H
#define __APP_H
/* app.h  - application class

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

#include "global_data.h"

#include <QMutex>
#include <QApplication>

class Splash;
class WorldDataLoader;
class QTranslator;

class App : public QApplication {
	Q_OBJECT;
public:
	App (int argc, char ** argv);
	~App ();

	int run ();

	static QString     dataDir();
	static QString     worldDataDir(const QString &server);

	static App *       instance;
	static GlobalData  globalData()  {return instance->global;};

signals:

	void worldChangedSignal ();
	void worldLoadProgressSignal (int, const QString&);
	void worldLoadErrorSignal (const QString &,const QString &, const QString &);


public slots:
	void initialWorldLoadComplete (bool);

	void worldLoadProgress (int perc,  const QString& msg);
	void worldLoadComplete (bool err);

	void setLanguage (const QString & s);

	void aquireNewWorld ( QString s, QString w, bool force_download=false );
	void cancelLoading ();

	void exitProgram () ; 

private:
		

	QMutex mutex;

	Splash           * splash;

	bool              world_load_complete;   
	bool              world_load_error;   
	WorldDataLoader * worldload;

	bool splash_is_blocking;
	GlobalData        global;

	QTranslator *    trans;
	QString          language;  
};

#endif