/* allys.cpp -  world data: alliances

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


Ally::Ally ()
{
	show=false;
	color=randomColor();
}

Ally& Ally::operator = (const Ally& o)
{
	id = o.id;
	name= o.name;
	points=o.points;
	off=o.off;
	deff=o.deff;
	wonders=o.wonders;
	color = o.color;
	show = o.show;
	players = o.players;
	towns=o.towns;
	return *this;
}

bool WorldDataLoader::processAllys() {
	QString head = tr("Processing alliances of %1").arg(data.world());
	bool err=false;
	emit progressSignal(-1,head);
	App::processEvents();
	QString msg;
	QString s (readGzipfile(data.allyFile(),err));
	if ( err ) {
		setError(data.allyFile(),tr("Failed to read alliance descriptions"), tr("GUnzip failed"));
		return false;
	}
	QStringList l=s.split("\n",QString::SkipEmptyParts);
	if(!l.size()) {
		setError(data.allyFile(),
			tr("Failed to read alliance descriptions"), 
			tr("File is empty"));
		return false;
	}
	data.allys().resize(l.size());
	int tot = l.size();
	int operc=-1;
	for(int i=0;i< l.size(); ++i) {
		int perc = 100 * (i+1)/(tot+1) ;
		if (perc>operc) {
			operc=perc;
			emit progressSignal (perc, head);
			App::instance->processEvents();
		}
		QStringList k = l[i].split(",");
		if(k.size()!= 6) {
		setError(data.islandFile(),
			tr("Failed to read alliance descriptions"), 
			tr("Corrupted file"));
			return false;
		}
		int id = k[0].toInt();
		data.allyHash()[id] = i;
		data.allys()[i].id = id;
		data.allys()[i].name = replaceStrangeChars(k[1]);
		data.allys()[i].points = k[2].toInt();
		data.allys()[i].off = 0;
		data.allys()[i].deff = 0;
		data.allys()[i].wonders = 0;
		data.allys()[i].color = randomColor();
		data.allys()[i].show = false;
		data.allys()[i].players.clear();
		data.allys()[i].towns.clear();
	}
	return true;
}; 
