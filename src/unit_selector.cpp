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

#include "unit_selector.h"
#include "app.h"




UnitSelector::UnitSelector (QWidget * parent, UnitIndex u) : Popup (parent), unit (u) {
};


QWidget * UnitSelector::createWidget ()
{
	QWidget *ret = new QWidget(this);
	ui.setupUi(ret);
	
	units.push_back (0 ); // Militia  - just a dummy      
	units.push_back (ui._sword);
	units.push_back (ui._slinger);
	units.push_back (ui._archer);
	units.push_back (ui._hoplite);
	units.push_back (ui._rider);
	units.push_back (ui._chariot);
	units.push_back (ui._catapult);
	units.push_back (ui._slow);
	units.push_back (ui._biri);
	units.push_back (ui._fire);
	units.push_back (ui._brander);
	units.push_back (ui._fast);
	units.push_back (ui._trireme);
	units.push_back (ui._kolo);
	units.push_back (ui._minotaur);
	units.push_back (ui._manti);
	units.push_back (ui._zyklop);
	units.push_back (ui._hydra);
	units.push_back (ui._harpy);
	units.push_back (ui._medusa);
	units.push_back (ui._centaur);
	units.push_back (ui._pega);
	units.push_back (ui._cerberus);     // 
	units.push_back (ui._fury);
	units.push_back (ui._griffin);
	units.push_back (ui._boar);
	units.push_back (ui._godsent);

	bg = new QButtonGroup (this);
	for (int i=1;i<units.size(); ++i) {
		if ((int)unit == i) units[i]->setChecked(true);
		bg->addButton (units[i],i);
	}
	connect (bg, SIGNAL (buttonClicked(int)), this, SLOT(unitClicked(int)));
	return ret;
}


void UnitSelector::unitClicked (int u) 
{
	unit = (UnitIndex) u;		
	close ();
}


UnitIndex UnitSelector::selectUnit (const QPoint & pos, UnitIndex u) 
{
	UnitSelector w(0, u);
	QEventLoop l;
	w.popup (pos, &l);
	return w.value();
}
