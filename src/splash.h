#ifndef ___SPLASH_H
#define ___SPLASH_H
/* splash.h  - SplashScreen

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
#include <QSplashScreen>
#include <QPushButton>
#include <QLabel>

#include "app.h"

class Splash : public QSplashScreen {
	Q_OBJECT;
public:

	enum State {
		SplashOk,
		SplashAbort,
		SplashBlocking
	};

	Splash ();

signals:
	void acceptedSignal ();

public slots:

	void loadGlobalProgress (int perc, const QString & s);

	void loadWorldProgress (int perc, const QString & s);
	
	void showError (const QString & tit,const QString & desc,const QString & det);

private:
	QMutex        m;
	State         st;
	QWidget *     dlg;
	QPushButton * bt;
	QLabel      * glob; 
	QLabel      * world; 
	QLabel       * base;
};




#endif