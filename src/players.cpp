/* players.cpp -  world data: Players

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

#include "world_data.h"
#include "defines.h"
#include "app.h"
#include "json.h"

Player::Player()
{
	show=false;
	color=randomColor();
	premium=0;
}

Player& Player::operator = (const Player& o)
{
	id=o.id;
	name=o.name;
	ally=o.ally;
	points=o.points;
	off=o.off;
	deff=o.deff;
	color=o.color;
	show=o.show;
	towns=o.towns;
	premium=o.premium;
	return *this;
}

bool WorldDataLoader::processPlayers() {
	QString head = tr("Processing players of %1").arg(data.world());
	bool err=false;
	emit progressSignal(-1,head);
	App::processEvents();
	QString msg;
	QString s (readGzipfile(data.playerFile(),err));
	if ( err ) {
		setError(data.playerFile(),tr("Failed to read player descriptions"), tr("GUnzip failed"));
		return false;
	}
	QStringList l=s.split("\n",QString::SkipEmptyParts);
	if(!l.size()) {
		setError(data.playerFile(),
			tr("Failed to read player descriptions"), 
			tr("File is empty"));
		return false;
	}
	data.players().resize(l.size());
	int tot = l.size();
	int operc=-1;
	for(int i=0;i< l.size(); ++i) {
		int perc = 100 * (i+1)/tot ;
		if (perc>operc) {
			operc=perc;
			emit progressSignal (perc, head);
			App::instance->processEvents();
		}

		if(abortRequested()) {
			setError ("", tr("User abort"),tr("Loading was cancelled by user"));
			return false;
		};

		QStringList k = l[i].split(",");
		if(k.size()!= 6) {
		setError(data.playerFile(),
			tr("Failed to read player descriptions"), 
			tr("Corrupted file"));
			return false;
		}
		int ally = -1;
		if(!k[2].isEmpty()) ally = data.allyHash()[k[2].toInt()];
		if(ally >= 0) data.allys()[ally].players.push_back(i);

		int id = k[0].toInt();
		data.playerHash()[id] = i;

		data.players()[i].id = id;
		data.players()[i].name = replaceStrangeChars(k[1]);
		data.players()[i].ally = ally;
		data.players()[i].points = k[3].toInt();
		data.players()[i].off = 0;
		data.players()[i].deff = 0;
		data.players()[i].color = randomColor();
		data.players()[i].show = false;
		data.players()[i].towns.clear();
		data.players()[i].premium = 0;	
	}
	return true;
}; 
