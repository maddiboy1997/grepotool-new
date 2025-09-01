/* world_data.cpp -  world data

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

#include "defines.h"
#include "world_data.h"
#include "json.h"
#include "app.h"

QPoint WorldData::townCoordinates (int idx)  const
{
	QPoint p = towns()[idx].pos();
	return QPoint (p.x()/12800, p.y()/12800);
}
QPoint WorldData::islandCoordinates (int idx) const 
{
	return QPoint (islands()[idx].x, islands()[idx].y) +App::globalData().getIslandCenterOffset (idx);
}
int WorldData::oceanNumberOfTown (int idx)  const
{
	return oceanNumberFromMapCoords (towns()[idx].pos());
}
int WorldData::oceanNumberOfIsland (int idx)  const
{
	return oceanNumberFromMapCoords (islandCoordinates (idx));
}

int WorldData::oceanNumberFromTownCoords  (const QPoint& p)
{
	return p.x() * 10 + p.y(); 
}

int WorldData::oceanNumberFromMapCoords  (const QPoint& p)
{
	return oceanNumberFromTownCoords (QPoint (p.x()/12800,p.y()/12800)); 
}

QString WorldData::playerLabelText (int p) const
{
	if (p<0) return QString("<img src=':/icons/ghost.png' width=16 height=16> %1 ").arg(tr("Ghost town"));
	return QString("<img src=':/icons/player.png' width=16 height=16> %1 ").arg(players()[p].name);
}

QString WorldData::allyLabelText (int a) const
{
	if(a<0) return "";
	return QString("<img src=':/icons/ally.png' width=16 height=16> %1 ").arg(allys()[a].name);
}

QString WorldData::TownLabelText (int t, bool tooltipp) const
{
	if (t<0) return "";
	QString tn = QString("<img src=':/icons/town.png' width=16 height=16> %1 ").arg(towns()[t].name);
	QString tpn = QString("<img src=':/icons/points.png' width=16 height=16> %1 ").arg(towns()[t].points);
	int pi = towns()[t].player;
	QString pn = playerLabelText (pi);
	QString an = "";
	if (pi>=0) an = allyLabelText (players()[pi].ally);		

	if (tooltipp) {
		QString r ("<b>");
		r += tn; 
		r += "<br>";
		r += tpn;
		r += "<br>";
		r += pn;
		if (an.length()) {
			r += "<br>";
			r += an;
		}
		r+= "</b>";
		return r;
	}
	QString r ("<b>");
	r += tn; 
	r += "<br>";
	r += pn;
	if (an.length() ){
		r += an;
	}
	r+= "</b>";
	return r;
}


static QMap <QString, QString> splitMap (const QStringList& l, const QString& sep) {
	QMap <QString, QString> r;
	for (int i=0;i< l.size(); ++i) {
		QStringList ll = l[i].split(sep);
		QString k = ll[0];
		QString v = (ll.size()>1)?ll[1]:"";
		r[k] = v;
	}
	return r;
};

static QMap <QString, QString> splitString2Map (const QString& l, const QString& sep) {
	return splitMap  (l.split(";",QString::SkipEmptyParts),sep);
};


static int extractUnits (const QString& ul, QVector < int > & u) {
	QMap <QString, QString> un = splitString2Map (ul,":");	

	u.resize(UnitMax);
	for(int i=0;i<UnitMax; ++i) u[i]=0;
	static const char * uni [UnitMax] = {
		"militia",		"sword",		"slinger",		"archer",		"hoplite",
		"rider",		"chariot",		"catapult",		"big_transporter",		"bireme",
		"attack_ship",	"demolition_ship","small_transporter",		"trireme",	"colonize_ship",
		"minotaur",		"manticore",	"zyklop",		"sea_monster",		"harpy",
		"medusa",		"centaur",		"pegasus",		"cerberus",		"fury",
		"griffin",		"calydonian_boar",		"godsent"
	};
	static int uni_v [UnitMax] = {0,1,1,1,1,
		3,4,10,7,8,
		10,8,5,16,170,
		30,45,40,50,14,
		18,12,20,30,55,35,20,3};

	int ret = 0;
	for (int i = 0; i < UnitMax; ++i) {
		if (un.contains(QString(uni[i]))) {
			int cnt = un[QString(uni[i])].toInt();
			u[i] = cnt;
			ret += uni_v[i]*cnt;
		}
	}
	return ret;
}


int WorldData::townIndex (const QString& t,const QString& p) const
{
	
	if (p.isEmpty() || t.isEmpty ()) return -1;

	bool ok;
	int ti = t.toInt(&ok);
	
	if (ok) {
		p.toInt(&ok);
		if (ok) 
			return townIndex (ti);
	}

	for (int i=0;i< players().size();++i) {
		const Player & pla= players()[i];			
		if (pla.name == p) {
			for (int j =0; j < pla.towns.size(); ++j) {
				if (towns()[pla.towns[j]].name == t) { 
					return pla.towns[j];
				}
			}
			break;
		}
	}

	return -1;
}

QDateTime WorldData::lastPlayerActivity (int pi) const
{
	const Player & pl = players()[pi];
	QDateTime t1=stats().lastIncrease (Stats::PlayerPoints, pl.id);
	QDateTime t2=stats().lastIncrease (Stats::PlayerOff, pl.id);
	return (t1>t2)?t1:t2;
}

QDateTime WorldData::lastPlayerOffActivity (int pi) const 
{
	const Player & pl = players()[pi];
	return stats().lastIncrease (Stats::PlayerOff, pl.id);
}

QDateTime WorldData::lastPlayerDefActivity (int pi) const
{
	const Player & pl = players()[pi];
	return stats().lastIncrease (Stats::PlayerDeff, pl.id);
}
QDateTime WorldData::lastPlayerPointsActivity (int pi) const
{
	const Player & pl = players()[pi];
	return stats().lastIncrease (Stats::PlayerPoints, pl.id);
}
QString WorldData::activityText (const QDateTime & ti)
{
	QDateTime time = ti;
	if (!time.isValid()) {
		return "---";
	}
	QDateTime ct = QDateTime::currentDateTime();
	int da = ti.daysTo(ct);
	int ho = ti.secsTo(ct);
	ho /= 60; // mins
	if (da) {
		if(da>1) return tr("%1 days ago").arg(da);
		else return tr("1 day ago");
	} else if (ho) {
		if (ho > 120) {
			ho /= 60; // hours
			return tr("%1 hours ago").arg(ho);				
		}
	} 
	return tr("active");
}



// Example:
/*
[GrepoTool=1.1;server=de41;language=de_DE;date=am 01.12.13 um 18:18 Uhr;source=forum;attacker_name=;attacker_town=58012;attacker_units=(harpy:5;);defender_name=743001;defender_town=50892;defender_units=(bireme:926;)]
*/
static QMap <QString, QString> parse2Map (const QString & s) {
	QMap <QString, QString> r;
	if(!s.length()) return r;
	QString nam;
	QString val;
	bool is_name = true;
	int i= 0;
	while (i< s.length()) {
		QChar c = s.at(i);
		if (c=='[') {++i; continue;};
		if (c==']') {
			r[nam] = val;
			break;
		};
		if (!is_name && c=='(') {
			++i;
			while (i<s.length()){
				c = s.at(i);
				if (c == ')') {
					++i;
					break;
				}
				val += c;
				++i;
			}
			continue;
		};
		if (is_name && c == '=') {
			++i;
			is_name = false;
			// nam = "";
			val = "";
			continue;
		}
		if (!is_name && c == ';') {
			r[nam] = val;
			is_name = true;
			nam = "";
			val = "";
			++i; 
			continue;
		}
		if (is_name) {
			nam+=c;
		} else {
			val+=c;
		}
		++i;		
	}	
	return r;
}

// [GrepoTool=1.0;server=de41;locale_lang=de_DE;attacker=.Pete.;attackerTown=M46 Shiny Beach;attack_date=16.11.2013 17:39;units=(attack_ship:1;small_transporter:5;slinger:20;hoplite:10;rider:10;catapult:1;)]
// [GrepoTool=1.1;server=de41;language=de_DE;date=am 01.12.13 um 18:18 Uhr;source=forum;attacker_name=;attacker_town=58012;attacker_units=(harpy:5;);defender_name=743001;defender_town=50892;defender_units=(bireme:926;)]
int WorldData::handleReport (const QString & tx)
{

	QMap <QString, QString> at = parse2Map (tx);  
	if (!at.contains("GrepoTool")) return -1;
	if (!at.contains("server")) return -1;
	if (at["server"] != server()) return -1;

	if (at["GrepoTool"]== "1.0") {

		if (!at.contains("attacker")) return -1;
		if (!at.contains("attackerTown")) return -1;
		QString pn = replaceStrangeChars(at["attacker"]);
		QString to = replaceStrangeChars(at["attackerTown"]);

		int ti = townIndex (to,pn);
		if (ti<0) return ti;

		QVector <int> unn;
		if(!extractUnits (at["units"],unn)) return -1;
		for (int i = 0; i < UnitMax; ++i) {
			towns()[ti].units[i] = unn[i];
		}
		if(towns()[ti].type == TownTypeUndefined) 
			towns()[ti].type = towns()[ti].guessType();

		return ti;
	} else if (at["GrepoTool"]== "1.1") {

		QString src = at["source"];

		int att_town = townIndex (at["attacker_town"], at["attacker_name"]);
		QVector<int> att_u;
		int natt = extractUnits (at["attacker_units"], att_u);

		int def_town = townIndex (at["defender_town"], at["defender_name"]);
		QVector <int> def_u;
		// int ndef =
		extractUnits (at["defender_units"], def_u);

		if (att_town < 0 && def_town <0) return -1;

		if (def_town < 0) { 
			// att against me == store the attacker units
			for (int i = 0; i < UnitMax; ++i) {
				towns()[att_town].units[i] = att_u[i];
			}
			if(towns()[att_town].type == TownTypeUndefined) 
					towns()[att_town].type = towns()[att_town].guessType();
			return att_town;

		} if (att_town < 0) { 

			// att by me ==> store the defender units
			//if(ndef < 3000) {
				for (int i = 0; i < UnitMax; ++i) {
					towns()[def_town].units[i] = def_u[i];
					if(towns()[def_town].type == TownTypeUndefined) 
						towns()[def_town].type = towns()[def_town].guessType();
				}
			//}
			return def_town;
		} else {

			// store the defender units if the town is not deffed
			//if(ndef < 3000) {
				for (int i = 0; i < UnitMax; ++i) {
					towns()[def_town].units[i] = def_u[i];
					if(towns()[def_town].type == TownTypeUndefined) 
						towns()[def_town].type = towns()[def_town].guessType();
				}
			//}
			// it's an att if the number of attacking units > 250
			if (natt > 250) {
				for (int i = 0; i < UnitMax; ++i) {
					towns()[att_town].units[i] = att_u[i];
				}
				if (towns()[att_town].type == TownTypeUndefined) 
					towns()[att_town].type = towns()[att_town].guessType();
				return att_town;
			}
			return def_town;
		}
	
	}
	return -1;

}

UnitIndex WorldData::unitIndex (const QString & name) 
{
	static const char * un [UnitMax] = {
		"militia",		"sword",		"slinger",		"archer",		"hoplite",
		"rider",		"chariot",		"catapult",		"big_transporter",		"bireme",
		"attack_ship",	"demolition_ship","small_transporter",		"trireme",	"colonize_ship",
		"minotaur",		"manticore",	"zyklop",		"sea_monster",		"harpy",
		"medusa",		"centaur",		"pegasus",		"cerberus",		"fury",
		"griffin",		"calydonian_boar",		"godsent"	};
		for (int i=0; i < UnitMax; ++i) if (name == un[i]) return (UnitIndex) i;
		return UnitMax;
}


QString WorldData::unitName (UnitIndex i,bool plural) 
{
	if(!plural) {
	if(i == Militia) return tr("Militia");
	else if (i == Sword) return tr("Sword");
	else if (i == Slinger) return tr("Slinger");
	else if (i == Archer) return tr("Archer");
	else if (i == Hoplite) return tr("Hoplite");
	else if (i == Rider) return tr("Rider");
	else if (i == Chariot) return tr("Chariot");
	else if (i == Kata) return tr("Catapult");
	else if (i == Transporter) return tr("Big transporter");
	else if (i == Bireme) return tr("Bireme");
	else if (i == Attackship) return tr("Attack ship");
	else if (i == Brander) return tr("Demolition ship");
	else if (i == FastTransporter) return tr("Small transporter");
	else if (i == Trireme) return tr("Trireme");
	else if (i == Kolo) return tr("Colonize ship");
	else if (i == Minotaur) return tr("Minotaur");
	else if (i == Manti) return tr("Manticore");
	else if (i == Zyklop) return tr("Zyklop");
	else if (i == Hydra) return tr("Sea monster");
	else if (i == Harpy) return tr("Harpy");
	else if (i == Medusa) return tr("Medusa");
	else if (i == Centaur) return tr("Centaur");
	else if (i == Pegasus) return tr("Pegasus");
	else if (i == Zerberus) return tr("Cerberus");
	else if (i == Eriny) return tr("Fury");
	else if (i == Griffin) return tr("Griffin");
	else if (i == Kalydonian) return tr("Kalydonian boar");
	else if (i == Godsent) return tr("Godsent");
	return tr("Unknown unit");
	}else {
	if(i == Militia) return tr("Militias");
	else if (i == Sword) return tr("Swords");
	else if (i == Slinger) return tr("Slingers");
	else if (i == Archer) return tr("Archers");
	else if (i == Hoplite) return tr("Hoplites");
	else if (i == Rider) return tr("Riders");
	else if (i == Chariot) return tr("Chariots");
	else if (i == Kata) return tr("Catapults");
	else if (i == Transporter) return tr("Big transporters");
	else if (i == Bireme) return tr("Biremes");
	else if (i == Attackship) return tr("Attack ships");
	else if (i == Brander) return tr("Demolition ships");
	else if (i == FastTransporter) return tr("Small transporters");
	else if (i == Trireme) return tr("Triremes");
	else if (i == Kolo) return tr("Colonize ships");
	else if (i == Minotaur) return tr("Minotaurs");
	else if (i == Manti) return tr("Manticores");
	else if (i == Zyklop) return tr("Zyklops");
	else if (i == Hydra) return tr("Sea monsters");
	else if (i == Harpy) return tr("Harpys");
	else if (i == Medusa) return tr("Medusas");
	else if (i == Centaur) return tr("Centaurs");
	else if (i == Pegasus) return tr("Pegasoi");
	else if (i == Zerberus) return tr("Cerberi");
	else if (i == Eriny) return tr("Furys");
	else if (i == Griffin) return tr("Griffins");
	else if (i == Kalydonian) return tr("Kalydonian boars");
	else if (i == Godsent) return tr("Godsents");
	return tr("Unknown units");
	}
};

QString WorldData::unitIconFile (UnitIndex i) 
{
	if(i == Militia) return ":/icons/militia.png";
	else if (i == Sword) return ":/icons/sword.png";
	else if (i == Slinger) return ":/icons/slinger.png";
	else if (i == Archer) return ":/icons/archer.png";
	else if (i == Hoplite) return ":/icons/hoplite.png";
	else if (i == Rider) return ":/icons/rider.png";
	else if (i == Chariot) return ":/icons/chariot.png";
	else if (i == Kata) return ":/icons/catapult.png";
	else if (i == Transporter) return ":/icons/big_transporter.png";
	else if (i == Bireme) return ":/icons/bireme.png";
	else if (i == Attackship) return ":/icons/attack_ship.png";
	else if (i == Brander) return ":/icons/demolition_ship.png";
	else if (i == FastTransporter) return ":/icons/small_transporter.png";
	else if (i == Trireme) return ":/icons/trireme.png";
	else if (i == Kolo) return ":/icons/colonize_ship.png";
	else if (i == Minotaur) return ":/icons/minotaur.png";
	else if (i == Manti) return ":/icons/manticore.png";
	else if (i == Zyklop) return ":/icons/zyklop.png";
	else if (i == Hydra) return ":/icons/hydra.png";
	else if (i == Harpy) return ":/icons/harpy.png";
	else if (i == Medusa) return ":/icons/medusa.png";
	else if (i == Centaur) return ":/icons/centaur.png";
	else if (i == Pegasus) return ":/icons/pegasus.png";
	else if (i == Zerberus) return ":/icons/cerberus.png";
	else if (i == Eriny) return ":/icons/fury.png";
	else if (i == Griffin) return ":/icons/griffin.png";
	else if (i == Kalydonian) return ":/icons/calydonian_boar.png";
	else if (i == Godsent) return ":/icons/godsent.png";
	return ":/icons/Unknown unit.png";
};

bool WorldData::isFlyingUnit (UnitIndex i) 
{
	if (i == Manti)  return true;
	else if (i == Harpy) return true;
	else if (i == Pegasus) return true;
	else if (i == Griffin) return true;

	return false;
}
bool WorldData::isLandUnit (UnitIndex i) 
{
	if (i == Sword) return true;
	else if (i == Slinger) return true;
	else if (i == Archer) return true;
	else if (i == Hoplite) return true;
	else if (i == Rider) return true;
	else if (i == Chariot) return true;
	else if (i == Kata) return true;
	else if (i == Minotaur) return  true;
	else if (i == Zyklop) return  true;
	else if (i == Medusa) return  true;
	else if (i == Centaur) return  true;
	else if (i == Zerberus) return  true;
	else if (i == Eriny) return  true;
	else if (i == Kalydonian) return  true;
	else if (i == Godsent) return  true;
	return false;
}

int  WorldData::townSpecials (int town) const
{
	int r = 0;
	if(towns()[town].research[ResearchMeteorology]) r += 1;
	if(towns()[town].research[ResearchCartography]) r += 2;
	if(towns()[town].movement_boost)                r += 4;
	if(towns()[town].buildings[Lighthouse])         r += 8;
	return r;
}

void  WorldData::setTownSpecials (int town, int spc)
{
	towns()[town].research[ResearchMeteorology] = (spc & 1);
	towns()[town].research[ResearchCartography] = (spc & 2);
	towns()[town].movement_boost =                (spc & 4);
	towns()[town].buildings[Lighthouse] =         (spc & 8)? 1 : 0;
}

QString  WorldData::townUnitsText (int town, bool text) const
{
	const Town &to = towns()[town];
	QString r;
	if(text) {
		for (int i=1;i< UnitMax; ++i) {
			if (to.units[i]) {
				if(!r.isEmpty()) r+= ";";
				r += unitName ((UnitIndex) i);
				r += ":";
				r+= QString::number(to.units[i]);
			}
		}
		return r;
	} else {
		for (int i=1;i< UnitMax; ++i) {
			if (to.units[i]) {
				if(!r.isEmpty()) r+= "<br>";
				r+= QString("<img src='%1' width=20 height=20> %3 %4").arg(unitIconFile((UnitIndex)i)).arg(to.units[i]).arg(unitName((UnitIndex)i,to.units[i]>1));
			}
		}
	}

	return r;
}

QString  WorldData::townSpecialText (int town, bool text) const
{
	QString r ;
	if (text) {
		if(towns()[town].research[ResearchMeteorology]) r += tr("Meteorology");
		if(towns()[town].research[ResearchCartography]) {
			if (!r.isEmpty()) r+=";";
			r += tr("Cartography");
		}
		if(towns()[town].movement_boost) {
			if (!r.isEmpty()) r+=";";
			r += tr("Movement boost");
		}
		if(towns()[town].buildings[Lighthouse])  {
			if (!r.isEmpty()) r+=";";
			r += tr("Lighthouse");
		}
		return r;
	} else {
		if(towns()[town].research[ResearchMeteorology]){
			r+= QString("<img src=':/icons/meteorology.png' width=20 height=20>:%1").arg(tr("Meteorology"));
		}
		if(towns()[town].research[ResearchCartography]){
			if (!r.isEmpty()) r+="<br>"; 
			r+= QString("<img src=':/icons/cartography.png' width=20 height=20>:%1").arg(tr("Cartography"));
		}
		if(towns()[town].movement_boost){
			if (!r.isEmpty()) r+="<br>";
			r+= QString("<img src=':/icons/movement_boost.gif' width=20 height=20>:%1").arg(tr("Movement boost"));
		}
		if(towns()[town].buildings[Lighthouse]){
			if (!r.isEmpty()) r+="<br>"; 
			r+= QString("<img src=':/icons/lighthouse.png' width=20 height=20>:%1").arg(tr("Lighthouse"));
		}
	}
	return QString("<p align=vcenter>")+r+"</p>";
}

QString WorldData::townTypeText(TownTypeIndex t, bool tooltipp)
{
	QString r; 
	if(t == TownTypeUndefined) {
		r = tr("Undefined");
	} else if (t == TownTypeAtt) {      // rect  
		r = tr("Off");
	} else if (t == TownTypeDeff) {      // Triangle
		r = tr("Def");
	} else if (t == TownTypeAttShip) {   // raute
		r = tr("Attships");
	} else if (t == TownTypeAttKolo) {   // rect+hat
		r = tr("Att + Kolo");
	} else if (t == TownTypeBireme) {   // Tri down
		r = tr("Biris");
	} else if (t == TownTypeManti) {     // star
		r = tr("Mantis");
	} else if (t == TownTypePega) {      // Rect with slash
		r = tr("Pegasoi");
	} else if (t == TownTypeHarpy) {    //  rect with backslash
		r = tr("Harpys");
	}
	if (!tooltipp) return r;
	return QString("<p align=vcenter><img src=':/icons/town_type%1.png' width=20 height=20> %2</p>").arg((int)t).arg(r);
}



QPixmap  WorldData::townUnitsPixmap (int town) const
{
	const Town &to = towns()[town];

	int w= 0;
	int s = 16;

	for (int i=1;i< UnitMax; ++i) {
		if (to.units[i]) {
			w += s;
		}
	}
	if(!w) return QPixmap();
	
	QImage r(QSize (w,s), QImage::Format_ARGB32_Premultiplied);
	r.fill (0);
	QPainter p(&r);
	w = 0;
	for (int i=1;i< UnitMax; ++i) {
		if (to.units[i]) {
			QPixmap p1 (unitIconFile((UnitIndex)i));
			p.drawPixmap(w,0,p1.scaled(QSize(s,s)));
			w += s;
		}
	}
	return QPixmap::fromImage(r);
}

QPixmap  WorldData::townSpecialPixmap (int town) const
{
	QPixmap p1;
	QPixmap p2;
	QPixmap p3;
	QPixmap p4;
	int w= 0;
	int s = 16;
	if(towns()[town].research[ResearchMeteorology]) {p1 = QPixmap(":/icons/meteorology.png");++w;}
	if(towns()[town].research[ResearchCartography]) {p2 = QPixmap(":/icons/cartography.png");++w;}
	if(towns()[town].movement_boost)                {p3 = QPixmap(":/icons/movement_boost.gif");++w;}
	if(towns()[town].buildings[Lighthouse])         {p4 = QPixmap(":/icons/lighthouse.png");++w;}
	if(!w) return p4;

	QImage r(QSize (w*s,s), QImage::Format_ARGB32);
	QPainter p(&r);
	w = 0;
	if (p1.width()) { p.drawPixmap(w,0,p1.scaled(QSize(s,s))); w+=s;}
	if (p2.width()) { p.drawPixmap(w,0,p2.scaled(QSize(s,s))); w+=s;}
	if (p3.width()) { p.drawPixmap(w,0,p3.scaled(QSize(s,s))); w+=s;}
	if (p4.width()) { p.drawPixmap(w,0,p4.scaled(QSize(s,s))); w+=s;}
	return QPixmap::fromImage(r);
}

static double unitMovementFactor (int spc, UnitIndex u) {

	double factor = 1.0;
	if (spc & 1) { // meteorology    land  +10%
		if (WorldData::isLandUnit(u) || WorldData::isFlyingUnit(u))
			factor *= 1.1;
	}
	if (spc & 2) { // cartography    sea +10%
		if (!WorldData::isLandUnit(u) && !WorldData::isFlyingUnit(u))
			factor *= 1.1;
	}
	if (spc & 4) { // movement boost all  +30% 
		factor *= 1.3;
	}
	if (spc & 8) { // lighthouse     sea  +15%
		if (!WorldData::isLandUnit(u) && !WorldData::isFlyingUnit(u))
			factor *= 1.15;
	}
	return factor;
}

int WorldData::calculateRuntime (int source, int target, UnitIndex u) const
{
	if (source<0 || target<0) return -1;
	if (u == Militia) return -1;
	if (source==target) return -1;
	if (isLandUnit(u) && towns()[source].island != towns()[target].island) return -1;
	const UnitInfo ui = _d->units[u];
	QPoint p0 = towns()[source].pos();
	QPoint p1 = towns()[target].pos();
	double dx = p0.x()-p1.x();
	double dy = p0.y()-p1.y();
	double d = sqrt (dx*dx+dy*dy);

	// Kata speed1: 21600 speed2: 10800 speed3: 7200
	double worldspeed = 21600. / _d->units[Kata].build_time;

	// Sword speed1: 8   speed2: 16
	// double unitspeed = (_d->units[Sword].landspeed / 8);

	// preparation time is 15 mins / worldspeed
	double ruest = (15. * 60.) / worldspeed;  // seconds

	double us = 0;
	if (isLandUnit(u)) us = ui.landspeed;
	else us = ui.seaspeed;

	// t = d* 50 / us 
	// [s] =  [pixel] 
	//       ------------  
	//         pixel
	//        ------------  
	//         50 * s


	// add the town specials to unit speed
	us *= unitMovementFactor(townSpecials(source), u);

	// Unit speed is given in pixels per 50 secs
	double ti = ruest + (int)d * 50. / us;

	return ti;
}


QVector<QPoint> WorldData::townPositionsOfAlly (int i) const 
{
	Ally a = allys()[i];
	QVector<QPoint> r(a.towns.size());
	for(int i = 0;i< a.towns.size(); ++i) {
		r[i] = towns()[a.towns[i]].pos();
	}
	return r;
}

QString WorldData::dataDir () const {
	if (_d->server.isEmpty()) return "";
	return App::worldDataDir (_d->server);
};


QString WorldData::dataUrl () const {
	if (_d->server.isEmpty()) return "";
	return QString("http://%1.grepolis.com/data").arg(_d->server);
};


QVector <Download*> WorldDataLoader::processFiles (bool force_download)
{
	// Static data 
	// These need to be downloaded only once per world

	int di = 0;

	QVector <Download*> r;
	if (force_download || !QFileInfo(data.islandFile()).exists()) {
		r.push_back(new Download (data.islandUrl(), data.islandFile(), tr("Islands of %1").arg(data.world()),di++));
	}
	if (force_download || !QFileInfo(data.unitFile()).exists()) {
		r.push_back(new Download (data.unitUrl() , data.unitFile(), tr("Units of %1").arg(data.world()),di++));
	}
	if (force_download || !QFileInfo(data.buildingFile()).exists()) {
		r.push_back(new Download (data.buildingUrl() , data.buildingFile(), tr("Buildings of %1").arg(data.world()),di++));
	}
	if (force_download || !QFileInfo(data.researchFile()).exists()) {
		r.push_back(new Download (data.researchUrl() , data.researchFile(), tr("Researches of %1").arg(data.world()),di++));
	}

	// Dynamic data
	// It is not allowed to download the worlddata more often than once per hour

	QFileInfo fi (data.playerFile());
	if (fi.exists()) {
		QDateTime ft = fi.lastModified();
		int sec = ft.secsTo(QDateTime::currentDateTime());
		if (sec > 3600) // 1 hour
			force_download = true;
	}
	fi = QFileInfo (data.allyFile());
	if (fi.exists()) {
		QDateTime ft = fi.lastModified();
		int sec = ft.secsTo(QDateTime::currentDateTime());
		if (sec > 3600) // 1 hour
			force_download = true;
	}
	fi = QFileInfo(data.townFile());
	if (fi.exists()) {
		QDateTime ft = fi.lastModified();
		int sec = ft.secsTo(QDateTime::currentDateTime());
		if (sec > 3600) // 1 hour
			force_download = true;
	}

	if (force_download || !QFileInfo(data.playerFile()).exists()) {
		r.push_back(new Download (data.playerUrl() , data.playerFile(), tr("Players of %1").arg(data.world()),di++));
	}

	if (force_download || !QFileInfo(data.allyFile()).exists()) {
		r.push_back(new Download (data.allyUrl() , data.allyFile(), tr("Alliances of %1").arg(data.world()),di++));
	}

	if (force_download || !QFileInfo(data.townFile()).exists()) {
		r.push_back(new Download (data.townUrl() ,data.townFile(), tr("Towns of %1").arg(data.world()),di++));
	}

	if ((force_download || !QFileInfo(data.conquerFile()).exists())&& r.size()) {
		r.push_back(new Download (data.conquerUrl() , data.conquerFile(), tr("Conquests of %1").arg(data.world()),di++,false));
	}

	if (force_download || !QFileInfo(data.offFile()).exists()) {
		r.push_back(new Download (data.offUrl() , data.offFile(), tr("Off bashies of %1").arg(data.world()),di++));
	}

	if (force_download || !QFileInfo(data.deffFile()).exists()) {
		r.push_back(new Download (data.deffUrl() , data.deffFile(), tr("Deff bashies of %1").arg(data.world()),di++));
	}

	if (r.size()) return r;

	// static world data first
	if (!processUnits()) return r; 
	if (!processBuildings()) return r; 
	if (!processResearches()) return r; 

	// Islands
	if (!processIslands()) return r; 

	// Alliances 
	if (!processAllys()) return r; 

	// players are added to alliances
	if (!processPlayers()) return r; 

	// Towns are added to allys and players
	if (!processTowns()) return r; 

	// Off and deff are set for players and allys
	if (!processBashies ()) return r; 

	// Global settings, colors and show-flags of playaes and allys
	if (!processSettings ()) return r; 

	// load's the conquer.txt.gz
	if (!processConquests ()) return r; 

	data.storeSettings ();

	return r;
}
