/* static_data.cpp - static world data

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


UnitInfo::UnitInfo ()
{
}

UnitInfo & UnitInfo::operator = (const UnitInfo& o)
{
	build_time = o.build_time;
	seaspeed = o.seaspeed;
	landspeed= o.landspeed;
	attack= o.attack;
	attack_type= o.attack_type;
	defence[0] = o.defence[0];
	defence[1] = o.defence[1];
	defence[2] = o.defence[2];
	defence[3] = o.defence[3];
	name = o.name;
	name_pl = o.name_pl;
	image = o.image;
	desc = o.desc;
	return *this;
}

static UnitInfo extractUnitInfo (const QVariant & u, UnitIndex i) {

	QVariantMap m = u.toMap();

	QVariantMap info;

	UnitInfo ui;
	ui.build_time    = 0.;
	ui.seaspeed       = 0.;
	ui.landspeed      = 0.;
	ui.attack         = 0;
	ui.attack_type    = Hack;
	ui.defence[Hack]     = 0;
	ui.defence[Pierce]   = 0;
	ui.defence[Distance] = 0;
	ui.defence[Naval]    = 0;
	ui.name           = "";
	ui.name_pl        = "";
	ui.desc           = "";

	switch (i) {
		case Militia:
			info = m["militia"].toMap();
			ui.image = QImage(":/icons/militia.png");
			ui.attack_type    = Pierce;
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();			
			break;
		case Sword:
			info = m["sword"].toMap();
			ui.attack_type    = Hack;
			ui.image = QImage(":/icons/sword.png");
			ui.attack         = info["attack"].toInt();;
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Slinger:
			info = m["slinger"].toMap();
			ui.attack_type    = Distance;
			ui.image = QImage(":/icons/slinger.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Archer:
			info = m["archer"].toMap();
			ui.attack_type    = Distance;
			ui.image = QImage(":/icons/archer.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Hoplite:
			info = m["hoplite"].toMap();
			ui.attack_type    = Pierce;
			ui.image = QImage(":/icons/hoplite.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Rider:
			info = m["rider"].toMap();
			ui.attack_type    = Hack;
			ui.image = QImage(":/icons/rider.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Chariot:
			info = m["chariot"].toMap();
			ui.attack_type    = Pierce;
			ui.image = QImage(":/icons/chariot.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();

			break;
		case Kata:
			info = m["catapult"].toMap();
			ui.attack_type    = Distance;
			ui.image = QImage(":/icons/catapult.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Transporter:
			info = m["big_transporter"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/big_transporter.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();
			break;
		case Bireme:
			info = m["bireme"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/bireme.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();
			break;
		case Attackship:
			info = m["attack_ship"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/attack_ship.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();
			break;
		case Brander:
			info = m["demolition_ship"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/demolition_ship.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();
			break;
		case FastTransporter:
			info = m["small_transporter"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/small_transporter.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();

			break;
		case Trireme:
			info = m["trireme"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/trireme.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();
			break;
		case Kolo:
			info = m["colonize_ship"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/colonize_ship.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();
			break;
		case Minotaur:
			info = m["minotaur"].toMap();
			ui.attack_type    = Hack;
			ui.image = QImage(":/icons/minotaur.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();

			break;
		case Manti:
			info = m["manticore"].toMap();
			ui.attack_type    = Pierce;
			ui.image = QImage(":/icons/manticore.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.seaspeed       = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Zyklop:
			info = m["zyklop"].toMap();
			ui.attack_type    = Distance;
			ui.image = QImage(":/icons/zyklop.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Hydra:
			info = m["sea_monster"].toMap();
			ui.attack_type    = Naval;
			ui.image = QImage(":/icons/hydra.png");
			ui.seaspeed        = info["speed"].toDouble ();
			ui.defence[Naval]    = info["defense"].toInt();
			break;
		case Harpy:
			info = m["harpy"].toMap();
			ui.attack_type    = Hack;
			ui.image = QImage(":/icons/harpy.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.seaspeed       = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Medusa:
			info = m["medusa"].toMap();
			ui.attack_type    = Pierce;
			ui.image = QImage(":/icons/medusa.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Centaur:
			info = m["centaur"].toMap();
			ui.attack_type    = Distance;
			ui.image = QImage(":/icons/centaur.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Pegasus:
			info = m["pegasus"].toMap();
			ui.attack_type    = Pierce;
			ui.image = QImage(":/icons/pegasus.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.seaspeed       = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Zerberus:
			info = m["cerberus"].toMap();
			ui.attack_type    = Hack;
			ui.image = QImage(":/icons/cerberus.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Eriny:
			info = m["fury"].toMap();
			ui.attack_type    = Distance;
			ui.image = QImage(":/icons/fury.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Griffin:
			info = m["griffin"].toMap();
			ui.attack_type    = Hack;
			ui.image = QImage(":/icons/griffin.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.seaspeed       = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Kalydonian:
			info = m["calydonian_boar"].toMap();
			ui.attack_type    = Pierce;
			ui.image = QImage(":/icons/calydonian_boar.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		case Godsent:
			info = m["godsent"].toMap();
			ui.attack_type    = Hack;
			ui.image = QImage(":/icons/godsent.png");
			ui.landspeed      = info["speed"].toDouble ();
			ui.defence[Hack]     = info["def_hack"].toInt();
			ui.defence[Pierce]   = info["def_pierce"].toInt();
			ui.defence[Distance] = info["def_distance"].toInt();
			break;
		default: 			
			break;
	}
	ui.build_time  = info["build_time"].toDouble();
	ui.name = info["name"].toString();
	ui.name_pl = info["name_plural"].toString();
	ui.desc = info["description"].toString();
	ui.attack = info["attack"].toInt();
	return ui;
};

bool WorldDataLoader::processUnits() {

	QString head = tr("Processing units of %1").arg(data.world());
	bool err=false;
	emit progressSignal(-1,head);
	App::processEvents();
	QString msg;
	QString s (readGzipfile(data.unitFile(),err));
	if ( err ) {
		setError(data.unitFile(),tr("Failed to read unit descriptions"), tr("GUnzip failed"));
		return false;
	}

	bool success = false;
	QVariant u = QtJson::parse(s, success);
	if ( !success ) {
		setError(data.unitFile(),tr("Failed to parse unit descriptions"),
			tr("Json parser failed."));
		return false;
	}
	for (int i = 0; i < UnitMax; ++i) {
		data.unitInfo((UnitIndex) i) = extractUnitInfo(u,(UnitIndex)i);		
	}
	return true;
}; 
bool WorldDataLoader::processBuildings() {
	return true;
}; 
bool WorldDataLoader::processResearches() {
	return true;
}; 
