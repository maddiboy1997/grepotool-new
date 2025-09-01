/* unit_selector.cpp - Unit selector

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

#include "world_selector.h"
#include "app.h"
#include "command_ids.h"

#define MAX_WORLDS_PER_COL 10

static const char * _country_short [22] = 
		{"br","cz","de","dk","en","es","fr","gr","hu","it","jp",
		"kr","nl","no","pl","pt","ro","ru","se","sk","th","tr"};

static const char * _country_long [22] = 
		{"Brazil","Czech Republic","Germany","Denmark","International","Spain","France",
		"Greece","Hungary","Italy","Japan","Korea","Netherlands",
		"Norway","Poland","Portugal","Romania","Russia","Sweden","Slovakia","Thailand",
		"Turkey"};



WorldSelector::WorldSelector (QWidget * parent):Popup(parent)
{
}


QWidget * WorldSelector::createWidget ()
{

	QWidget *ret = new QWidget(this);

	ui.setupUi(ret);
	/*
	QStringList ctr = App::getCountries ();
	connect (ui._cancel,SIGNAL(clicked()), this, SLOT(cancel()));
	osrv = App::getSetting("",CURRENT_SERVER_LANG,"de").toString();
	index = App::getSetting("",CURRENT_WORLD,"41").toString();

	srv = osrv;

	QStringList cl = App::getCountries ();

	cg = new QButtonGroup(this);
	wg = new QButtonGroup(this);
	int r=0; int c= 0;
	int sel = 0;
	for (int i = 0; i< 22; ++i) {
		QToolButton * tb = new QToolButton (this); 
		cg->addButton(tb,i);
		QString ico = QString(":/icons/flag_%1.png").arg ( _country_short[i] );
		tb->setToolTip(QString("%1").arg ( _country_long[i]));
		c = i%3;
		r = i/3;
		ui._countries->addWidget(tb,r,c);
		if (srv == _country_short[i]) sel = i;
		tb->setCheckable (true);
		tb->setEnabled(cl.contains(_country_short[i]));
	}
	cg->button (sel)->setChecked(true);
	connect (cg, SIGNAL (buttonClicked(int)), this, SLOT(countryClicked(int))); 
	setupWorlds ();
	*/
	return  ret;
}


void WorldSelector::setupWorlds ()
{
	/*
	while (wg->buttons().size()) delete wg->buttons()[0];	
	QStringList wl = App::getWorlds (srv);

	int sel = 0;
	for (int i=0;i< wl.size(); ++i) {
		QString tx = App::getWorldName (index, wl[i]);
		QPushButton * b = new QPushButton(tx,this);
		b->setCheckable(true);
		b->setToolTip(QString("(%1%2)").arg ( i));
		wg->addButton (b,i);
		int r = i % MAX_WORLDS_PER_COL;
		int c = i / MAX_WORLDS_PER_COL;
		ui._worlds->addWidget (b,r,c);
		if (srv== osrv && index == wl[i]) 
			sel = i;
	}
	wg->button (sel)->setChecked(true);
	connect (wg, SIGNAL (buttonClicked(int)), this, SLOT(worldClicked(int))); 
	*/
}

QString WorldSelector::selectWorld (const QPoint & pos)
{
	WorldSelector w(0);
	QEventLoop l;
	w.popup (pos, &l);
	return w.country()+ w.server();
}

void WorldSelector::worldClicked (int i)
{
	/*
	index = QString::number(i+1);	
	QString oc = App::getSetting("",CURRENT_SERVER_LANG,"de").toString();
	QString ow =  App::getSetting("",CURRENT_WORLD,"41").toString();
	if (index != ow || srv != oc) 
		App::instance->aquireNewWorld(srv+index,App::instance->getWorldName(srv,index));
	*/
	exitLoop();
}

void WorldSelector::countryClicked (int i)
{
	srv = _country_short[i];
	setupWorlds();
}

void WorldSelector::cancel()
{
	exitLoop ();
}



