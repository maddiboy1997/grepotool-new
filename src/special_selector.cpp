/* special_selector.cpp - Town special selector

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

#include "special_selector.h"
#include "app.h"


SpecialSelector::SpecialSelector (QWidget * parent, int s)  : Popup (parent), spc (s)
{
}

QWidget * SpecialSelector::createWidget ()
{
	QWidget *ret = new QWidget (this);
	ui.setupUi (ret);
	bg = new QButtonGroup (this);
	bg->setExclusive (false);
	ui._meteorology->setChecked(spc & 1);
	ui._cartography->setChecked(spc & 2);
	ui._movement_boost->setChecked(spc & 4);
	ui._lighthouse->setChecked(spc & 8);
	bg->addButton (ui._meteorology,0);
	bg->addButton (ui._cartography,1);
	bg->addButton (ui._movement_boost,2);
	bg->addButton (ui._lighthouse,3);
	connect (bg, SIGNAL (buttonClicked(int)), this, SLOT(specialClicked(int)));
	return ret;
}

void SpecialSelector::specialClicked (int )
{
	spc = 0;
	if (ui._meteorology->isChecked()) spc += 1;
	if (ui._cartography->isChecked()) spc += 2;
	if (ui._movement_boost->isChecked()) spc += 4;
	if (ui._lighthouse->isChecked()) spc += 8;
	accept ();
}

int SpecialSelector::selectSpecials (const QPoint & pos, int spc)
{
	SpecialSelector w(0, spc);
	QEventLoop l;
	w.popup (pos, &l);
	return w.value();
}









