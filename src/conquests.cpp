/* conquests.cpp -  world data: Conquests

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

class Conquest {
public:
	Conquest();
	Conquest(const Conquest & o){*this = o;};;
	Conquest& operator = (const Conquest & o);
	int town_id;
	int time;
	int new_player_id;
	int old_player_id;
    int new_ally_id;
	int old_ally_id;
	int town_points;
};


bool WorldDataLoader::processConquests () {

	QString head = tr("Loading statistics of %1").arg(data.world());
	QString fn = data.statsFile();	
	bool update_needed = filesDownloaded() || !QFileInfo (fn).exists();
	if (QFileInfo (fn).exists()) {
		bool err=false;
		emit progressSignal(-1,head);
		App::processEvents();
		QString msg;
		QByteArray ba = readZipfile (fn, err, msg);
		if ( !err ) {
			QDataStream ds(&ba,QIODevice::ReadOnly);	
			ds.setVersion(QDataStream::Qt_4_7);
			ds >> data.stats();
		} else update_needed = true;
	}

	if (!update_needed) return true;

	fn = data.conquerFile();	
	head = tr("Processing conquests of %1").arg(data.world());
	if (QFileInfo (fn).exists()) {
		bool err=false;
		emit progressSignal(-1,head);
		App::processEvents();
		QByteArray ba = readGzipfile (fn, err);
		
		if(!err) {
			QTextStream  ds(&ba,QIODevice::ReadOnly);		
			
			int tot=0;
			int operc=-1;
			while (!ds.readLine().isEmpty()) ++tot;
			ds.resetStatus();
			ds.seek(0);

			int i=0;
			QString li = ds.readLine();
			while ( ds.status() == QTextStream::Ok ) {
				if (abortRequested()) {
					setError ("", tr("User abort"),tr("Loading was cancelled by user"));
					return false;
				};
				QStringList k = li.split(",");
				int perc = 100 * (i+1)/(tot+1) ;
				if (perc>operc) {
					operc=perc;
					emit progressSignal (perc, head);
					App::instance->processEvents();
				}
				if (k.size()!= 7) {
					setError(fn,
					tr("Failed to read file"), 
					tr("Corrupted file"));
					return false;
				}
				// "town.id", "time", "new.player.id", "old.player.id",
                // "new.ally.id", "old.ally.id", "town.points"
				int town_id = k[0].toInt();
				int timestamp = k[1].toInt();
				int new_player_id = (k[2].isEmpty())? -1 : k[2].toInt();
				int old_player_id = (k[3].isEmpty())? -1 : k[3].toInt();
				int old_ally_id = (k[4].isEmpty())? -1 : k[4].toInt();
				int new_ally_id = (k[5].isEmpty())? -1 : k[5].toInt(); 
				int points = k[6].toInt();
				data.stats().handleConquest(timestamp, town_id, points, old_player_id, old_ally_id, new_player_id, new_ally_id);
				++i;
				li = ds.readLine();
				if (li.isEmpty()) break;
			}
		}
	}

	int nt= data.towns().size();
	int ng =0;
	int np = data.players().size();
	int npa = 0;
	int na = data.allys().size();

	head = tr("Processing towns of %1").arg(data.world());
	emit progressSignal(-1,head);
	App::processEvents();
	for (int i=0;i< data.towns().size(); ++i) {
		const Town & to = data.towns()[i];
		int pid = to.player;
		int aid = to.ally;
		if (pid>=0) pid = data.players()[pid].id;
		if (aid>=0) aid = data.allys()[aid].id;
		if (pid<0) ++ng;
		data.stats().handleTown (to.id, to.name, to.points, pid , aid);
	}

	head = tr("Processing allys of %1").arg(data.world());
	emit progressSignal(-1,head);
	App::processEvents();
	for (int i=0;i< data.allys().size(); ++i) {
		const Ally & al = data.allys()[i];
		data.stats().handleAlly   (al.id, al.name, al.points, al.players.size(), al.towns.size(), al.off, al.deff);
	}

	head = tr("Processing players of %1").arg(data.world());
	emit progressSignal(-1,head);
	App::processEvents();

	for (int i=0;i< data.players().size(); ++i) {
		const Player & pl = data.players()[i];
		int aid = pl.ally;
		if (aid>=0) aid = data.allys()[aid].id;
		if (aid<0) ++npa;
		data.stats().handlePlayer (pl.id, pl.name, pl.points, aid, pl.towns.size(), pl.off, pl.deff);
	}

	data.stats().dataBase(Stats::WorldNumPlayers,0).addValue(np); 
	data.stats().dataBase(Stats::WorldNumAllyless,0).addValue(npa); 
	data.stats().dataBase(Stats::WorldNumAllys,0).addValue(na); 
	data.stats().dataBase(Stats::WorldNumTowns,0).addValue(nt); 
	data.stats().dataBase(Stats::WorldNumGhosts,0).addValue(ng); 


	head = tr("Storing statistics of %1").arg(data.world());
	emit progressSignal(-1,head);
	App::processEvents();

	QByteArray hs;
	QDataStream os(&hs,QIODevice::WriteOnly);
	os.setVersion(QDataStream::Qt_4_7);
	os << data.stats();		
	writeZipfile (data.statsFile (),hs);
	return true;
}; 
