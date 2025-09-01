/* splash.cpp  - SplashScreen

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
#include <QtGui>

#include "splash.h"
#include "defines.h"

#define SPLASH_TXT_X 66
#define SPLASH_TXT_Y 166

Splash::Splash () : QSplashScreen(QPixmap (":/icons/splash.png")),dlg(0){
	glob = new QLabel (this); 
	glob->resize(300,20);
	world = new QLabel (this); 
	world->resize(300,20);

	base = new QLabel (this);
	base->setText( "&copy;&nbsp;P.&nbsp;Vorpahl&nbsp;<img src=':/icons/player.png' width=18 height=18>&nbsp;<b>.Pete.");
	base->move(375, 60);
/*
	base->setText( QString("<p>Grepotool %1.%2 %3 <br>&copy; 2013 by P. Vorpahl (<img src=':/icons/player.png' width=18 height=18>&nbsp;<b>.Pete.)</b><br>%4</p>").
		arg(appVersionHi()).
		arg(appVersionLo()).
		arg(appVersionLevel()).
		arg(tr("Grepotool is a free utility for the<br>Browsergane Grepolis which is the <br>intellectual property of InnoGames GmbH.")));
*/
	glob->move(SPLASH_TXT_X, SPLASH_TXT_Y);
	world->move(SPLASH_TXT_X, SPLASH_TXT_Y+16);

	// QFont f = dir->font();
	// f.setPointSize (11);
	//base->setFont (f);
	// f.setBold (true);
	//f.setPointSize ();

	//dir->setFont(f);
	//glob->setFont(f);
	//world->setFont(f);
};


void Splash::loadGlobalProgress (int perc, const QString & s) {
	if (perc < 0) {
		glob->setText(s);
	} else {
		glob->setText(QString("%1 (%2%)").arg(s).arg(perc));
	}
};

void Splash::loadWorldProgress (int perc, const QString & s) {
	if (perc < 0) {
		world->setText(s);
	} else {
		world->setText(QString("%1 (%2%)").arg(s).arg(perc));
	}
};

void Splash::showError (const QString & tit,const QString & desc,const QString & det) {

	glob->setText ( QString ("<b>%1</b><br>%3<br>%4").arg(tit).arg(desc).arg(det) );
	world->hide ();
	bt = new QPushButton (tr("Exit"), this);
	int bx = (width() - bt->width()) / 2;
	int by = ((height() - bt->height())) -6;
	bt->move (QPoint(bx,by));
	bt->show();
	connect (bt, SIGNAL(clicked()), this, SIGNAL (acceptedSignal()));
};


