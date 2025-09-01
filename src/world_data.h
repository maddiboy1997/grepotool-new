#ifndef ___WORLD_STATIC_DATA_H
#define ___WORLD_STATIC_DATA_H
/* world_static.h  - data handling

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
#include <QtGui>
#include <QObject>
#include <QImage>
#include "indexes.h"
#include "data.h"
#include "command_ids.h"
#include "global_data.h"
#include "database.h"



class Island {
public:
	Island ();
	Island (const Island& o){*this = o;};
	Island& operator = (const Island& o);
	int           id;
	int           x;
	int           y;
	int           type;
	QImage        image;
	QVector <int> towns; 
};

class Ally {
public:
	Ally ();
	Ally (const Ally& o){*this = o;};
	Ally& operator = (const Ally& o);
	int           id;
	QString       name;
	int           points;
	int           off;
	int           deff;
	int           wonders;
	QColor        color;
	bool          show;
	bool          sel;
	QVector <int> players;
	QVector <int> towns;
};


class Player {
public:
	Player();
	Player(const Player& o){*this = o;};;
	Player& operator = (const Player& o);

	int           id;
	QString       name;
	int           ally;
	int           points;
	int           off;
	int           deff;
	QColor        color;
	bool          show;
	int           premium;
	QVector <int> towns;
};

class Town {
public:
	Town();
	Town(const Town& o){*this = o;};
	Town& operator = (const Town& o);
	QPoint pos () const {return QPoint(x,y);};
	TownTypeIndex guessType ();
	
	int           id;
	QString       name;
	int           points;
	int           player;
	int           ally;
	int           island;
	int           x;
	int           y;
	int           no;// on island;
	GodIndex      god;
	bool          movement_boost;
	int           buildings[BuildingMax];
	int           units[UnitMax];
	bool          research[ResearchMax];
	TownTypeIndex type;

};

class Plan {
public:
	Plan(){};
	Plan(int mt, int tgt, UnitIndex p, uint tim, bool arr):my_town(mt), target(tgt), unit(p), time (tim), arrival(arr) {};
	Plan(const Plan& o){*this = o;};
	Plan& operator = (const Plan& o){ 
		my_town= o.my_town;
		target = o.target;
		unit   = o.unit;
		time = o.time;
		arrival = o.arrival;
		return *this;
	};
	void store (QDataStream & s) const {
		s<< my_town;
		s<<target;
		s<<(int) unit;
		s<< time;
		s<< arrival;
	};
	void restore (QDataStream & s) {
		int dmy;
		s>> my_town;
		s>> target;
		s>> dmy; unit = (UnitIndex) dmy;
		s>> time;
		s>> arrival;
	};
	int       my_town;
	int       target;
	UnitIndex unit;
	uint      time;
	bool      arrival;
};

inline QDataStream& operator << (QDataStream &st,const Plan& s) {
	s.store(st);
	return st;
};
inline QDataStream& operator >> (QDataStream &st,Plan& s) {
	s.restore(st);
	return st;
};

class MyTown {
public:
	MyTown(){};
	MyTown(int t, UnitIndex p):town_index(t), slowest_unit(p){};
	MyTown(const MyTown& o){*this = o;};
	MyTown& operator = (const MyTown& o){town_index = o.town_index; slowest_unit=o.slowest_unit; return *this;};
	void store (QDataStream & s) const {
		s<< town_index;
		s<<(int) slowest_unit;
	};
	void restore (QDataStream & s) {
		int dmy;
		s>> town_index;
		s>> dmy; slowest_unit = (UnitIndex) dmy;
	};
	int       town_index;
	UnitIndex slowest_unit;
};

inline QDataStream& operator << (QDataStream &st,const MyTown& s) {
	s.store(st);
	return st;
};
inline QDataStream& operator >> (QDataStream &st,MyTown& s) {
	s.restore(st);
	return st;
};


class Target {
public:
	Target(){};
	Target(int t):town_index(t){};
	Target(const Target& o){*this = o;};
	Target& operator = (const Target& o){town_index = o.town_index; return *this;};
	void store (QDataStream & s) const {s<<town_index;};
	void restore (QDataStream & s) {s>>town_index;};
	int       town_index;	
};

inline QDataStream& operator << (QDataStream &st,const Target& s) {
	s.store(st);
	return st;
};
inline QDataStream& operator >> (QDataStream &st, Target& s) {
	s.restore(st);
	return st;
};



class TableSetting {
public:
	TableSetting () {};

	TableSetting (const TableSetting & o) : selection (o.selection),state(o.state){};
	QList <int>      selection;
	QByteArray       state;

	void store ( QDataStream &s )const {
		int version = 2;
		s<< version;
		s<<selection;
		s<<state;
	};
	void restore ( QDataStream &s ) {
		int version = 1;

		s>> version;
		if(version <2) {
			QList <int>      colOrder;
			QList <bool>     colVisibility;
			int              sortCol;
			Qt::SortOrder    sortOrder;
			s>>colOrder;
			s>>selection;
			s>>colVisibility;
			s>>sortCol;
			int dmy; 
			s>>dmy;  sortOrder = (Qt::SortOrder) dmy ;
		}else{
			s>>selection;
			s>>state;
		}
	};
};

inline QDataStream& operator << (QDataStream &st,const TableSetting& s) {
	s.store(st);
	return st;
};
inline QDataStream& operator >> (QDataStream &st,TableSetting& s) {
	s.restore(st);
	return st;
};




class TargetList {
public:
	TargetList(const QString &n = QString ("")):name(n),
		time(QDateTime::currentDateTime().toTime_t()),
		arrival(false),
		unit(Attackship),
		town(-1){};
	TargetList(const TargetList& o){*this = o;};
	TargetList& operator = (const TargetList& o){
		l= o.l; 
		name = o.name; 
		time = o.time;
		arrival = o.arrival;
		unit = o.unit;
		town = o.town;
		return *this;
	};
	void store (QDataStream & s) const {
		s<<name;
		s<<time;
		s<<arrival;
		s<<	(int) unit;
		s<<town;
		s<< l.size();
		for(int i =0;i< l.size(); ++i) 
			s<<l[i];
		s<<set;
	};
	void restore (QDataStream & s) {
		s>>name;
		s>>time;
		s>>arrival;
		int dmy;
		s>>dmy; unit = (UnitIndex) dmy;
		s>>town;
		int n;
		s>>n;
		l.resize(n);
		for(int i =0;i< n; ++i) 
			s>>l[i];
		s>>set;
	};
	QVector <Target>   l;
	QString            name;
	uint               time;
	bool               arrival;
	UnitIndex          unit;  // attacking unit
	int                town;  // attacking town
	TableSetting       set;
};

inline QDataStream& operator << (QDataStream &st,const TargetList& s) {
	s.store(st);
	return st;
};
inline QDataStream& operator >> (QDataStream &st,TargetList& s) {
	s.restore(st);
	return st;
};


class MyTownList {
public:
	MyTownList(const QString &n = QString ("")):name(n),
		time(QDateTime::currentDateTime().toTime_t()),
		arrival(false),target(-1){};
	MyTownList(const MyTownList& o){*this = o;};
	MyTownList& operator = (const MyTownList& o){
		l= o.l; 
		name = o.name; 
		time = o.time;
		arrival = o.arrival;
		target = o.target;
		return *this;};
	void store (QDataStream & s) const {
		s<<name;
		s<<time;
		s<<arrival;
		s<<target;
		s<< l.size();
		for(int i =0;i< l.size(); ++i) 
			s<<l[i];
		s<<set;
	};
	void restore (QDataStream & s) {
		s>>name;
		s>>time;
		s>>arrival;
		s>>target;

		int n;
		s>>n;
		l.resize(n);
		for(int i =0;i< n; ++i) 
			s>>l[i];
		s>>set;
	};

	QVector <MyTown> l;
	QString          name;
	int              time;
	bool             arrival;
	int              target;
	TableSetting       set;
};

inline QDataStream& operator << (QDataStream &st,const MyTownList& s) {
	s.store(st);
	return st;
};
inline QDataStream& operator >> (QDataStream &st,MyTownList& s) {
	s.restore(st);
	return st;
};


class PlanList {
public:
	PlanList(const QString &n = QString ("")):name(n){};
	PlanList(const PlanList& o){*this = o;};
	PlanList& operator = (const PlanList& o){
		l= o.l; 
		return *this;};
	void store (QDataStream & s) const {
		s<<name;
		s<< l.size();
		for(int i =0;i< l.size(); ++i) 
			s<<l[i];
		s<<set;
	};
	void restore (QDataStream & s) {
		s>>name;
		int n;
		s>>n;
		l.resize(n);
		for(int i =0;i< n; ++i) 
			s>>l[i];
		s>>set;
	};
	QVector <Plan> l;
	QString        name;
	TableSetting       set;
};

inline QDataStream& operator << (QDataStream &st,const PlanList& s) {
	s.store(st);
	return st;
};
inline QDataStream& operator >> (QDataStream &st, PlanList& s) {
	s.restore(st);
	return st;
};

class UnitInfo {
public:
	UnitInfo ();
	UnitInfo (const UnitInfo &o) {*this = o;};
	UnitInfo & operator = (const UnitInfo& o);

	double          build_time;
	double          seaspeed;
	double          landspeed;
	int             attack;
	AttackTypeIndex attack_type;
	int             defence[4];
	QString         name;
	QString         name_pl;
	QImage          image;
	QString         desc;
};


class WorldDataPrivate : public DataPriv {
public:
	WorldDataPrivate (){};

	// Static data
	QString            server;
	QString            name;

	// static world data 
	QVector <Island>   islands;	
	UnitInfo           units [UnitMax];

	// dynamic data
	QVector <Player>   players;	
	QVector <Ally>     allys;	
	QVector <Town>     towns;	

	TableSetting       players_set;
	TableSetting       towns_set;
	TableSetting       allys_set;

	// editable data
	QVector <MyTownList> mytowns;
	QVector <TargetList> targets;
	QVector <PlanList>   plans;


	// global world stats database
	Stats              stats;

	// Hash tables for faster access
	QHash<int, int> island_hash;
	QHash<int, int> island_id_hash;
	QHash<int, int> ally_hash;
	QHash<int, int> player_hash;
	QHash<int, int> town_hash;
};

class WorldData {
	Q_DECLARE_TR_FUNCTIONS (WorldData);
public:
	WorldData (const QString &s="", const QString &w="") : _d(new WorldDataPrivate) {
		_d->incRef();
		_d->server = s;
		_d->name = w;
	};
	WorldData (const WorldData & o):_d(const_cast <WorldDataPrivate*> (o._d)){_d->incRef();};
	WorldData & operator = (const WorldData & o) {
		if (_d == o._d) return *this;
		if (_d->decRef()) delete _d;
		_d = const_cast <WorldDataPrivate*> (o._d);
		_d->incRef();
		return *this;
	};
	bool operator == (const WorldData & o) const {return _d == o._d;};

	int townSpecials (int town) const;
	void setTownSpecials (int town, int spc) ;
	QString townSpecialText (int town, bool text=false) const;
	QPixmap townSpecialPixmap (int town) const;
	QString townUnitsText (int town, bool text=false) const;
	QPixmap townUnitsPixmap (int town) const;

	QString playerLabelText (int p) const;
	QString allyLabelText (int a) const;
	QString TownLabelText (int t, bool tooltipp) const;

	QVector <QPoint> townPositionsOfAlly (int a) const; 

	QString &server () {return _d->server;};
	QString &world () {return _d->name;};

	Stats & stats () {return _d->stats;};
	const Stats & stats () const {return _d->stats;};

	QVector<Island> &islands () {return _d->islands;};
	QVector<Ally> &allys () {return _d->allys;};
	QVector<Town> &towns () {return _d->towns;};
	QVector<Player> &players () {return _d->players;};
	QVector <MyTownList> &mytowns() {return _d->mytowns;};
	QVector <TargetList> &targets() {return _d->targets;};
	QVector <PlanList>   &plans() {return _d->plans;};
	TableSetting & playersTableSetting () {return _d->players_set;};
	TableSetting & allysTableSetting () {return _d->allys_set;};
	TableSetting & townsTableSetting () {return _d->towns_set;};
	 
	const QString &server () const {return _d->server;};
	const QString &world () const {return _d->name;};
	const QVector<Island> &islands () const {return _d->islands;};
	const QVector<Ally> &allys () const {return _d->allys;};
	const QVector<Town> &towns () const {return _d->towns;};
	const QVector<Player> &players () const {return _d->players;};
	const QVector <MyTownList> &mytowns() const {return _d->mytowns;};
	const QVector <TargetList> &targets() const {return _d->targets;};
	const QVector <PlanList>   &plans() const {return _d->plans;};
	const TableSetting & playersTableSetting () const {return _d->players_set;};
	const TableSetting & allysTableSetting () const {return _d->allys_set;};
	const TableSetting & townsTableSetting () const {return _d->towns_set;};
	 

	const UnitInfo & unitInfo (UnitIndex u) const {return _d->units[(int) u];};
	UnitInfo & unitInfo (UnitIndex u) {return _d->units[(int) u];};

	QString dataDir () const;
	QString dataUrl () const;

	QString islandFile() const {return dataDir()+"/islands.txt.gz";};
	QString townFile() const {return dataDir()+"/towns.txt.gz";};
	QString playerFile() const {return dataDir()+"/players.txt.gz";};
	QString allyFile() const {return dataDir()+"/alliances.txt.gz";};
	QString conquerFile() const {return dataDir()+"/conquers.txt.gz";};
	QString unitFile() const {return dataDir()+"/units.json.gz";};
	QString buildingFile() const {return dataDir()+"/buildings.json.gz";};
	QString researchFile() const {return dataDir()+"/researches.json.gz";};
	QString offFile() const {return dataDir()+"/player_kills_att.txt.gz";};
	QString deffFile() const {return dataDir()+"/player_kills_def.txt.gz";};

	QString islandUrl() const {return dataUrl()+"/islands.txt.gz";};
	QString townUrl() const {return dataUrl()+"/towns.txt.gz";};
	QString playerUrl() const {return dataUrl()+"/players.txt.gz";};
	QString allyUrl() const {return dataUrl()+"/alliances.txt.gz";};
	QString conquerUrl() const {return dataUrl()+"/conquers.txt.gz";};
	QString unitUrl() const {return dataUrl()+"/units.json.gz";};
	QString buildingUrl() const {return dataUrl()+"/buildings.json.gz";};
	QString researchUrl() const {return dataUrl()+"/researches.json.gz";};
	QString offUrl() const {return dataUrl()+"/player_kills_att.txt.gz";};
	QString deffUrl() const {return dataUrl()+"/player_kills_def.txt.gz";};

	QString statsFile() const {return dataDir()+"/stats.dat";};
	QString historyFile() const {return dataDir()+"/history.dat";};
	QString settingsFile() const {return dataDir()+"/settings.dat";};
	QString allySettingsFile() const {return dataDir()+"/ally_settings.dat";};
	QString playerSettingsFile() const {return dataDir()+"/player_settings.dat";};
	QString townSettingsFile() const {return dataDir()+"/town_settings.dat";};

	QString myTownsFile() const {return dataDir()+"/mytowns.dat";};
	QString targetsFile() const {return dataDir()+"/targets.dat";};
	QString plansFile() const {return dataDir()+"/plans.dat";};

	void storeSettings ();

	int playerIndex (int id) const {if(_d->player_hash.contains(id)) return _d->player_hash[id]; return -1;};
	int allyIndex (int id) const {if(_d->ally_hash.contains(id)) return _d->ally_hash[id]; return -1;};
	int townIndex (int id) const {if(_d->town_hash.contains(id)) return _d->town_hash[id]; return -1;};
	int islandIndex (int id) const {if(_d->island_id_hash.contains(id)) return _d->island_id_hash[id]; return -1;};
	int islandIndex (int x, int y) const {if(_d->island_hash.contains(x*1000 + y)) return _d->island_hash[x*1000 + y]; return -1;};

	int townIndex (const QString& t, const QString& p) const;

	QHash<int,int> & playerHash() {return _d->player_hash;};
	QHash<int,int> & allyHash() {return _d->ally_hash;};
	QHash<int,int> & townHash() {return _d->town_hash;};
	QHash<int,int> & islandIdHash() {return _d->island_id_hash;};
	QHash<int,int> & islandPosHash() {return _d->island_hash;};

	const QHash<int,int> & playerHash() const {return _d->player_hash;};
	const QHash<int,int> & allyHash() const {return _d->ally_hash;};
	const QHash<int,int> & townHash() const {return _d->town_hash;};
	const QHash<int,int> & islandIdHash()const  {return _d->island_id_hash;};
	const QHash<int,int> & islandPosHash()const  {return _d->island_hash;};

	~WorldData () {if (_d->decRef()) delete _d;};

	static QString unitName (UnitIndex i,bool plural=false) ;
	static UnitIndex unitIndex (const QString & name) ;
	static bool isLandUnit (UnitIndex i) ;
	static QString unitIconFile (UnitIndex i); 
	static bool isFlyingUnit (UnitIndex i);

	static QPixmap townTypeSymbol(TownTypeIndex);
	static void renderTownTypeSymbol(QPainter* paint, TownTypeIndex typ, const QPoint& pos, const double &radius);
	static QString townTypeText(TownTypeIndex t, bool tooltipp);

	QDateTime lastPlayerActivity (int pl) const; 
	QDateTime lastPlayerOffActivity (int pl) const; 
	QDateTime lastPlayerDefActivity (int pl) const; 
	QDateTime lastPlayerPointsActivity (int pl) const; 
	static QString activityText (const QDateTime & ti);

	int handleReport (const QString & tx);
	
	int calculateRuntime (int source, int target, UnitIndex u) const;

	QPoint townCoordinates (int idx) const;
	QPoint islandCoordinates (int idx) const;
	static int oceanNumberFromMapCoords  (const QPoint& p);
	static int oceanNumberFromTownCoords  (const QPoint& p);
	int oceanNumberOfTown (int idx) const;
	int oceanNumberOfIsland (int idx) const;

private:
	WorldDataPrivate *_d;
};

class WorldDataLoader : public DataLoader {
	Q_OBJECT;
public:
	WorldDataLoader (QObject * p, GlobalData g, const QString &s, const QString &w) : DataLoader (p) ,data(s,w),glob(g) {};
	QVector <Download*> processFiles (bool force_download);

	bool processUnits(); 
	bool processBuildings(); 
	bool processResearches(); 
	bool processIslands(); 
	bool processAllys(); 
	bool processPlayers(); 
	bool processTowns(); 
	bool processBashies(); 
	bool processSettings ();
	bool processConquests ();

	WorldData  data;
	GlobalData glob;
};




#endif