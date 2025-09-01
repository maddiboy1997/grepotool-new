#ifndef __DATABASE_H
#define __DATABASE_H
/* database.h  - database handling

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

#include <QDateTime>
#include <QVariant>
#include <QDataStream>
#include "indexes.h"
#include "defines.h"


/*! Database entry 
*/
class DbEntry {
public: 
	DbEntry():_start(QDateTime::currentDateTime()){_end = _start;};
	DbEntry(const QVariant &v, const QDateTime & tim = QDateTime::currentDateTime()):_start(tim),_end(tim),_data(v){};
	DbEntry(const DbEntry& o):_start(o._start),_end(o._end),_data(o._data){};

	bool operator == (const DbEntry& o){
		return _data == o._data;
	};

	DbEntry & operator = (const DbEntry& o){
		_start  = o._start;
		_end    = o._end;
		_data   = o._data;
		return *this;
	};

	QDateTime startDateTime () const {return _start;};
	QDateTime endDateTime () const {return _end;};
	QDate     startDate () const {return _start.date();};
	QDate     endDate () const {return _end.date();};

	QVariant & data () {return _data;};
	const QVariant & data () const {return _data;};

	void setEnd (const QDateTime & e) {_end = e;};
	void setStart (const QDateTime & e) {_start = e;};

private:
	QDateTime _start;
	QDateTime _end;
	QVariant  _data;
};

/*! Database class

Data is either a numeric interpolatable value or a unique identifier such as
an id or a name
*/
class DataBase {
public:
	bool interpolatable() const {return _interpolatable;};
	DataBase (bool interpolatable = false): _interpolatable(interpolatable) {};
	int size () const {return _db.size();};
	DataBase (const DataBase & o) : _interpolatable(o._interpolatable),_db(o._db){};
	DataBase & operator = (const DataBase & o) {
		_interpolatable = o._interpolatable;
		_db = o._db;
		return *this;
	};
	const DbEntry& operator [] (int i) const {return _db[i];};
	DbEntry& operator [] (int i){return _db[i];};

	void addValue (const QVariant& v, const QDateTime & tim = QDateTime::currentDateTime());
	int lastChangeEntry () const {return _db.size() -1;};
	int lastIncreaseEntry () const;
	
	QDateTime lastChange () const {if (!_db.size()) return QDateTime(); return _db[lastChangeEntry()].startDateTime();};
	QDateTime lastObservation () const {if (!_db.size()) return QDateTime(); return _db[lastChangeEntry()].endDateTime();};
	QDateTime firstObservation () const {if (!_db.size()) return QDateTime(); return _db[0].startDateTime();};
	QDateTime lastIncrease () const {if (!_db.size()) return QDateTime(); return _db[lastIncreaseEntry()].startDateTime();};

	QVariant valueAt (const QDateTime & tim) const;
	QVariant currentValue () const {if (!_db.size()) return QVariant(); return _db[_db.size()-1].data();};

	int firstOfDay(const QDate & d) const;
	int lastOfDay(const QDate & d) const;
	int lastOfMonth (const QDate & d) const;

	void condenseOldEntries ();

	static void test();
	void dump ();
	
	void store (QDataStream & s) const;
	void restore (QDataStream & s);

private:
	bool _interpolatable;
	QVector < DbEntry > _db;
};
QDataStream& operator << (QDataStream &st,const DataBase& s);
QDataStream& operator >> (QDataStream &st,DataBase& s);

class PlayerStats {
public:
	PlayerStats ():_points(true),_off(true),_deff(true){};
	void restore (QDataStream & s) {
		s >> _name;
		s >> _ally_id;
		s >> _ntowns;
		s >> _points;
		s >> _off;
		s >> _deff;
	};
	void store (QDataStream & s) const {
		s << _name;
		s << _ally_id;
		s << _ntowns;
		s << _points;
		s << _off;
		s << _deff;
	};
	DataBase _name;
    DataBase _ally_id;
    DataBase _ntowns;
    DataBase _points;
    DataBase _off;
    DataBase _deff;
};
QDataStream& operator << (QDataStream &st,const PlayerStats& s);
QDataStream& operator >> (QDataStream &st,PlayerStats& s);

class AllyStats {
public:
	AllyStats ():_points(true),_off(true),_deff(true){};
	void restore (QDataStream & s) {
		s >> _name;
		s >> _nplayers;
		s >> _ntowns;
		s >> _points;
		s >> _off;
		s >> _deff;
	};
	void store (QDataStream & s) const {
		s << _name;
		s << _nplayers;
		s << _ntowns;
		s << _points;
		s << _off;
		s << _deff;
	};
	DataBase _name;
    DataBase _nplayers;
    DataBase _ntowns;
    DataBase _points;
    DataBase _off;
    DataBase _deff;
};
QDataStream& operator << (QDataStream &st,const AllyStats& s);
QDataStream& operator >> (QDataStream &st,AllyStats& s);

class TownStats {
public:
	TownStats ():_points(true){};
	void restore (QDataStream & s) {
		s >> _name;
		s >> _player_id;
		s >> _ally_id;
		s >> _points;
	};
	void store (QDataStream & s) const {
		s << _name;
		s << _player_id;
		s << _ally_id;
		s << _points;
	};
	DataBase _name;
    DataBase _player_id;
    DataBase _ally_id;
    DataBase _points;
};
QDataStream& operator << (QDataStream &st,const TownStats& s);
QDataStream& operator >> (QDataStream &st,TownStats& s);

class Stats {
public:
	enum Entry {
		PlayerName,
		PlayerPoints,
		PlayerAllyId,
		PlayerOff,
		PlayerDeff,
		PlayerNTowns,
		
		AllyName,
		AllyPoints,
		AllyNPlayers,
		AllyNTowns,
		AllyDeff,
		AllyOff,

		TownName,
		TownPoints,
		TownPlayerId,
		TownAllyId,

		WorldNumPlayers,
		WorldNumAllyless,
		WorldNumAllys,
		WorldNumTowns,
		WorldNumGhosts
	};

	Stats ();
	void restore (QDataStream & s);
	void store (QDataStream & s) const;

	DataBase & dataBase (Entry t, int id=-1); 
	DataBase dataBase (Entry t, int id=-1) const; 

	QVariant valueAt ( Entry t, int id, const QDateTime & ti = QDateTime::currentDateTime()) {return dataBase(t,id).valueAt(ti);};
	QVariant currentValue ( Entry t, int id) const {return dataBase(t,id).currentValue();};
	QDateTime lastChange (Entry t, int id) const {return dataBase(t,id).lastChange();};
	QDateTime lastObservation (Entry t, int id) const {return dataBase(t,id).lastObservation();};
	QDateTime firstObservation (Entry t, int id) const {return dataBase(t,id).firstObservation();};
	QDateTime lastIncrease (Entry t, int id) const {return dataBase(t,id).lastIncrease();};

	void handlePlayer (int id, const QString &name, int points, int ally_id, int ntowns, int off, int deff);
	void handleAlly   (int id, const QString &name, int points, int nplayers, int ntowns, int off, int deff);
	void handleTown   (int id, const QString &name, int points, int player_id, int ally_id);
	void handleConquest  (int timestamp, int town_id, int points, int old_player_id, int old_ally_id, int new_player_id, int new_ally_id);
	void handlePlayerHistory  (int timestamp,int stamp_end, int player_id,  int ally_id, int ntowns, int points, int off, int deff);

private:
	QHash <int,int>   _player_hash;
	QHash <int,int>   _ally_hash;
	QHash <int,int>   _town_hash;

	PlayerStats & player (int id);
	AllyStats & ally (int id);
	TownStats & town (int id);

	PlayerStats  player (int id)const ;
	AllyStats  ally (int id)const ;
	TownStats  town (int id)const ;

	QVector <PlayerStats> _players;
	QVector <AllyStats>   _allys;
	QVector <TownStats>   _towns;

	DataBase                _num_players;
	DataBase                _num_allyless;
	DataBase                _num_towns;
	DataBase                _num_ghosts;
	DataBase                _num_allys;
	DataBase                _dmy;
};

QDataStream& operator << (QDataStream &st,const Stats& s);
QDataStream& operator >> (QDataStream &st,Stats& s);



#endif