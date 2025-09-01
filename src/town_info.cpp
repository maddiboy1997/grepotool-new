/* town_info.cpp - Town database entry widget

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

#include "town_info.h"
#include "database.h"
#include "command.h"

TownInfo::TownInfo ():ToolWidget ("TownInfo", tr("Town data")), town(-1)
{
	QFrame * f = new QFrame (this);
	ui.setupUi(f);
	setCentralWidget (f);
	connect (ui._goto,SIGNAL(clicked()), this,SLOT(gotoClicked()));

	bg = new QButtonGroup  (this);
	gods.push_back (ui._any);
	gods.push_back (ui._zeus);
	gods.push_back (ui._poseidon);
	gods.push_back (ui._hera);
	gods.push_back (ui._athene);
	gods.push_back (ui._hades);
	gods.push_back (ui._artemis);
	ui._any->setChecked (true);
	for (int i=0;i< gods.size(); ++i) {
		bg->addButton (gods[i],i);
	}
	bg->setExclusive(true);
	connect (bg, SIGNAL (buttonClicked(int)), this, SLOT(godChanged (int)));


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

	special.push_back (ui._meteorology);
	special.push_back (ui._cartography);
	special.push_back (ui._movement_boost);
	special.push_back (ui._lighthouse);
	for(int i=0;i< special.size(); ++i) {
		connect (special[i],SIGNAL(clicked()),this,SLOT(updateTown()));
	}

	for(int i=1;i< units.size(); ++i) {
		NumEdit * ne = new NumEdit (units[i],0,WorldData::unitIconFile ((UnitIndex) i)); 
		edits.push_back(ne);
	}

	godChanged(0);
	selectTown(-1);
}

void TownInfo::worldChanged ()
{
	selectTown(-1);
}

void TownInfo::fillContextMenu (QMenu*)
{
}

void TownInfo::initWidget ()
{
}


void TownInfo::godChanged (int g)
{
	GodIndex god = (GodIndex) g;
	if (god == AnyGod) {
		ui._minotaur->setEnabled(false);
		ui._manti->setEnabled(false);
		ui._zyklop->setEnabled(false);
		ui._hydra->setEnabled(false);
		ui._harpy->setEnabled(false);
		ui._medusa->setEnabled(false);
		ui._centaur->setEnabled(false);
		ui._pega->setEnabled(false);
		ui._cerberus->setEnabled(false);     // 
		ui._fury->setEnabled(false);
		ui._griffin->setEnabled(false);
		ui._boar->setEnabled(false);
	} else if (god == Zeus) { // zeus
		ui._minotaur->setEnabled(true);
		ui._manti->setEnabled(true);
		ui._zyklop->setEnabled(false);
		ui._hydra->setEnabled(false);
		ui._harpy->setEnabled(false);
		ui._medusa->setEnabled(false);
		ui._centaur->setEnabled(false);
		ui._pega->setEnabled(false);
		ui._cerberus->setEnabled(false);     // 
		ui._fury->setEnabled(false);
		ui._griffin->setEnabled(false);
		ui._boar->setEnabled(false);
	} else if (god == Poseidon) { // 
		ui._minotaur->setEnabled(false);
		ui._manti->setEnabled(false);
		ui._zyklop->setEnabled(true);
		ui._hydra->setEnabled(true);
		ui._harpy->setEnabled(false);
		ui._medusa->setEnabled(false);
		ui._centaur->setEnabled(false);
		ui._pega->setEnabled(false);
		ui._cerberus->setEnabled(false);     // 
		ui._fury->setEnabled(false);
		ui._griffin->setEnabled(false);
		ui._boar->setEnabled(false);
	} else if (god == Athene) { // 
		ui._minotaur->setEnabled(false);
		ui._manti->setEnabled(false);
		ui._zyklop->setEnabled(false);
		ui._hydra->setEnabled(false);
		ui._harpy->setEnabled(false);
		ui._medusa->setEnabled(false);
		ui._centaur->setEnabled(true);
		ui._pega->setEnabled(true);
		ui._cerberus->setEnabled(false);     // 
		ui._fury->setEnabled(false);
		ui._griffin->setEnabled(false);
		ui._boar->setEnabled(false);
	} else if (god == Hera) { // 
		ui._minotaur->setEnabled(false);
		ui._manti->setEnabled(false);
		ui._zyklop->setEnabled(false);
		ui._hydra->setEnabled(false);
		ui._harpy->setEnabled(true);
		ui._medusa->setEnabled(true);
		ui._centaur->setEnabled(false);
		ui._pega->setEnabled(false);
		ui._cerberus->setEnabled(false);     // 
		ui._fury->setEnabled(false);
		ui._griffin->setEnabled(false);
		ui._boar->setEnabled(false);
	} else if (god == Hades) { // 
		ui._minotaur->setEnabled(false);
		ui._manti->setEnabled(false);
		ui._zyklop->setEnabled(false);
		ui._hydra->setEnabled(false);
		ui._harpy->setEnabled(false);
		ui._medusa->setEnabled(false);
		ui._centaur->setEnabled(false);
		ui._pega->setEnabled(false);
		ui._cerberus->setEnabled(true);     // 
		ui._fury->setEnabled(true);
		ui._griffin->setEnabled(false);
		ui._boar->setEnabled(false);
	} else if (god == Artemis) { //
		ui._minotaur->setEnabled(false);
		ui._manti->setEnabled(false);
		ui._zyklop->setEnabled(false);
		ui._hydra->setEnabled(false);
		ui._harpy->setEnabled(false);
		ui._medusa->setEnabled(false);
		ui._centaur->setEnabled(false);
		ui._pega->setEnabled(false);
		ui._cerberus->setEnabled(false);     // 
		ui._fury->setEnabled(false);
		ui._griffin->setEnabled(true);
		ui._boar->setEnabled(true);
	}
	updateTown ();
}

TownInfo::~TownInfo ()
{
}

void TownInfo::selectTown (int idx)
{

	// if (town == idx) return;
	static bool sem = false;
	if (sem) return;
	sem = true;
	town = idx;
	ui._town->setText (Cmd::worldData().TownLabelText (town,false));


	if(town<0) {
		for (int i=0;i< special.size(); ++i) special[i]->setEnabled(false);
		for (int i=1;i< units.size(); ++i) units[i]->setEnabled(false);
		for (int i=1;i < gods.size(); ++i) gods[i]->setEnabled(false);
		ui._any->setChecked(true);
		sem=false;		
		return;
	} else {
		for (int i=0;i < special.size(); ++i) special[i]->setEnabled(true);
		for (int i=1;i < units.size(); ++i) units[i]->setEnabled(true);
		Town& to = Cmd::worldData().towns()[idx];
		int pi = to.player;
		GodIndex g = to.god;
		if (pi<0) {
			ui._any->setChecked(true);
			for (int i=1;i < gods.size(); ++i) gods[i]->setEnabled(false);
			for (int i=15;i<=26; ++i) units[i]->setEnabled(false);
		} else {
			for (int i=0;i < gods.size(); ++i) gods[i]->setEnabled(true);
			gods[(int)g]->setChecked (true);
			for (int i=1;i< UnitMax; ++i) {
				edits[i-1]->setValuePtr(&(to.units[i])); 
			}
			ui._movement_boost->setChecked (to.movement_boost);
			ui._lighthouse->setChecked (to.buildings[Lighthouse]>0);
			ui._cartography->setChecked (to.research[ResearchCartography]);
			ui._meteorology->setChecked (to.research[ResearchMeteorology]);
		}
		
	}
	sem=false;
}

void TownInfo::updateTown ()
{
	if (town == -1) return;

	Town& to = Cmd::worldData().towns()[town];

	to.movement_boost = ui._movement_boost->isChecked ();
	to.buildings[Lighthouse]= ui._lighthouse->isChecked ()? 1: 0;
	to.research[ResearchCartography] = ui._cartography->isChecked ();
	to.research[ResearchMeteorology] = ui._meteorology->isChecked ();

	emit itemDataChangedSignal (this, town);

}
