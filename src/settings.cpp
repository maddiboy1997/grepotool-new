/* settings.cpp - world settings

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
#include "command.h"


void WorldData::storeSettings ()
{
	if (server().isEmpty()) return;

	int version = 3;

	QByteArray b2;
	QDataStream ds2(&b2,QIODevice::WriteOnly);
	int na = allys().size();
	ds2 << version;
	ds2 << na;
	for (int i=0;i<na; ++i) {
		ds2<<allys()[i].id;
		ds2<<allys()[i].color;
		ds2<<allys()[i].show;
	}
	ds2 << allysTableSetting();
	writeZipfile (allySettingsFile(), b2);

	QByteArray b3;
	QDataStream ds3(&b3,QIODevice::WriteOnly);
	int np = players().size();
	ds3 << version;
	ds3 << np;
	for (int i=0;i<np; ++i) {
		ds3<<players()[i].id;
		ds3<<players()[i].color;
		ds3<<players()[i].show;
		ds3<<players()[i].premium;
	}
	ds3 << playersTableSetting();
	writeZipfile (playerSettingsFile(), b3);

	// only info on towns that have some data 
	int nt = towns().size();
	QVector <int> tt;
	for (int i=0;i<nt; ++i) {
		bool db = false;
		for (int k=0;!db && k< BuildingMax;++k) 
			if (towns()[i].buildings[k]) db=true;
		for (int k=0;!db && k< ResearchMax;++k) 
			if (towns()[i].research[k]) db=true;
		for (int k=0;!db && k< UnitMax;++k) 
			if (towns()[i].units[k]) db=true;
		if (towns()[i].god != AnyGod) db=true;
		if (towns()[i].movement_boost) db=true;
		if (towns()[i].type) db=true;
		if (db) tt.push_back(i);
	}

	QByteArray b4;
	QDataStream ds4(&b4,QIODevice::WriteOnly);
	ds4 << version;
	ds4 << tt.size();
	for (int i=0;i<tt.size(); ++i) {
		ds4<<towns()[tt[i]].id;
		int pl = towns()[tt[i]].player;
		if (pl>=0) pl = players()[pl].id;
		ds4<<pl;
		for (int k=0;k< BuildingMax;++k) 
			ds4<<towns()[tt[i]].buildings[k];
		for (int k=0;k< ResearchMax;++k) 
			ds4<<towns()[tt[i]].research[k];
		for (int k=0;k< UnitMax;++k) 
			ds4<<towns()[tt[i]].units[k];
		int g = (int) towns()[tt[i]].god;
		ds4 << g;
		ds4 << towns()[tt[i]].movement_boost;
		g = (int) towns()[tt[i]].type;
		ds4 << g;
	}	
	ds4 << townsTableSetting();
	writeZipfile (townSettingsFile(), b4);

	QByteArray b5;
	QDataStream ds5(&b5,QIODevice::WriteOnly);
	na = mytowns().size();
	ds5 << na;
	for (int i=0;i<na; ++i) {
		ds5<<mytowns()[i];
	}
	writeZipfile (myTownsFile(), b5);

	QByteArray b6;
	QDataStream ds6(&b6,QIODevice::WriteOnly);
	na = targets().size();
	ds6 << na;
	for (int i=0;i<na; ++i) {
		ds6<<targets()[i];
	}
	writeZipfile (targetsFile(), b6);

	QByteArray b7;
	QDataStream ds7(&b7,QIODevice::WriteOnly);
	na = plans().size();
	ds7 << na;
	for (int i=0;i<na; ++i) {
		ds7<<plans()[i];
	}
	writeZipfile (plansFile(), b7);


}

bool WorldDataLoader::processSettings () {

	QString head = tr("Processing settings of %1").arg(data.world());
	bool err=false;
	emit progressSignal(-1,head);
	App::processEvents();
	QString msg;

	if (QFileInfo (data.myTownsFile()).exists()) {
		int na = 0;
		QByteArray ba2 = readZipfile(data.myTownsFile(),err,msg);
		if ( !err ) {
			QDataStream ds2(&ba2,QIODevice::ReadOnly);
			ds2 >> na;
			for (int i=0;i<na; ++i) {
				MyTownList al;
				ds2>>al;
				data.mytowns().push_back(al);
			}	
		}
	}

	if (QFileInfo (data.targetsFile()).exists()) {
		int na = 0;
		QByteArray ba2 = readZipfile(data.targetsFile(),err,msg);
		if ( !err ) {
			QDataStream ds2(&ba2,QIODevice::ReadOnly);
			ds2 >> na;
			for (int i=0;i<na; ++i) {
				TargetList al;
				ds2>>al;
				data.targets().push_back(al);
			}	
		}
	}

	if (QFileInfo (data.plansFile()).exists()) {
		int na = 0;
		QByteArray ba2 = readZipfile(data.plansFile(),err,msg);
		if ( !err ) {
			QDataStream ds2(&ba2,QIODevice::ReadOnly);
			ds2 >> na;
			for (int i=0;i<na; ++i) {
				PlanList al;
				ds2>>al;
				data.plans().push_back(al);
			}	
		}
	}


	if (QFileInfo (data.allySettingsFile()).exists()) {
		int version = 0;
		int na = 0;// allys().size();
		QByteArray ba2 = readZipfile(data.allySettingsFile(),err,msg);
		if ( err ) {
			setError(data.allySettingsFile(),tr("Failed to read settings"), msg);
			return false;
		}
		QDataStream ds2(&ba2,QIODevice::ReadOnly);

		ds2 >> version;
		ds2 >> na;
		for (int i=0;i<na; ++i) {
			int al;
			ds2>>al;
			if(data.allyHash().contains(al)) {
				al = data.allyHash()[al];
				ds2>>data.allys()[al].color;
				ds2>>data.allys()[al].show;
			} else {
				QColor cc;
				ds2>>cc;
				bool bd;
				ds2>>bd;
				ds2>>al;
			}
		}
		if(version > 1) 
			ds2>>data.allysTableSetting ();
	}

	if (QFileInfo (data.playerSettingsFile()).exists()) {
		int version = 0;
		int np = 0;//players().size();
		QByteArray ba3 = readZipfile(data.playerSettingsFile(),err,msg);
		if ( err ) {
			setError(data.playerSettingsFile(),tr("Failed to read settings"), msg);
			return false;
		}
		QDataStream ds3(&ba3,QIODevice::ReadOnly);
		ds3 >> version;
		ds3 >> np;
		for (int i=0;i < np; ++i) {
			int al;
			ds3 >> al; // player id
			if(data.playerHash().contains(al)) {
				al = data.playerHash()[al];
				ds3>>data.players()[al].color;
				ds3>>data.players()[al].show;
				ds3>>data.players()[al].premium;

			} else {
				QColor cc;
				ds3>>cc;
				bool bd;
				ds3>>bd;
				ds3>>al;
			}
		}
		if(version > 1) 
			ds3>>data.playersTableSetting ();
	}

	if (QFileInfo (data.townSettingsFile()).exists()) {
		int version = 0;
		QByteArray ba4 = readZipfile(data.townSettingsFile(),err,msg);
		if ( err ) {
			setError(data.townSettingsFile(),tr("Failed to read settings"), msg);
			return false;
		}
		QDataStream ds4(&ba4,QIODevice::ReadOnly);
		ds4 >> version;
		int nt = 0; // towns().size();
		ds4 >> nt;
		for (int i=0;i<nt; ++i) {
			int al;
			int pl;
			ds4 >> al;
			ds4 >> pl;

			// see if player changed
			bool pl_chg = false;
			if (!data.townHash().contains(al)) {
				pl_chg = true;
			} else {
				int to = data.townHash()[al];
				int pl_id = data.towns()[to].player;
				if (pl_id>=0) pl_id = data.players()[pl_id].id;
				if (pl_id != pl) pl_chg = true;
			}

			if(!pl_chg) {
				al = data.townHash()[al];
				for (int k=0;k< BuildingMax;++k) 
					ds4>>data.towns()[al].buildings[k];
				for (int k=0;k< ResearchMax;++k) 
					ds4>>data.towns()[al].research[k];
				for (int k=0;k< UnitMax;++k) 
					ds4>>data.towns()[al].units[k];
				int gi = 0;
				ds4 >> gi;
				data.towns()[al].god = (GodIndex) gi;
				ds4 >> data.towns()[al].movement_boost;
				if (version>2) {
					ds4 >> gi;
					data.towns()[al].type = (TownTypeIndex) gi;
				}

			} else {
				for (int k=0;k< BuildingMax;++k) 
					ds4>>al;
				bool bb;
				for (int k=0;k< ResearchMax;++k) 
					ds4>>bb;
				for (int k=0;k< UnitMax;++k) 
					ds4>>al;
				ds4 >> al;
				ds4 >> bb;
				if (version>2) 
					ds4 >> al;
			}
		}
		if(version > 1) 
			ds4>>data.townsTableSetting ();
	}	

	return true;
}; 
