/* database.cpp - databases

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
#include <iostream>
#include "database.h"


static void coutDate (const QDateTime & v) {
	QString k = v.toString("[dd.MM.-HH:mm:ss]");	
	std::cout << k.toLatin1().constData();
}
void DataBase::dump() {
	std::cout << "------------- start DataBase:\n";
	for(int i=0;i< size(); ++i) {
		std::cout << i <<": ";
		coutDate(_db[i].startDateTime());
		std::cout << "-";			
		coutDate(_db[i].endDateTime());
		std::cout << " : ";			
		QString k = _db[i].data().toString();
		std::cout << k.toLatin1().constData();
		std::cout << "\n";
	}
	std::cout << "------------- end DataBase:\n";

	std::cout << "last increase: ";		
	if(_db.size()) 
		coutDate(lastIncrease());
	else
		std::cout << " unknown";
	std::cout << "\n";
	std::cout << "-----\n";
};


void DataBase::test() 
{

	coutDate (QDateTime::fromString("13.11.13 12:30","dd.MM.yy HH:mm"));
	std::cout << "\n";

	DataBase d1(true);	
	d1.addValue (1, QDateTime::fromString("03.09.2013 12:30","dd.MM.yyyy HH:mm"));
	d1.addValue (3, QDateTime::fromString("10.09.2013 12:30","dd.MM.yyyy HH:mm"));
	d1.addValue (5, QDateTime::fromString("11.09.2013 12:30","dd.MM.yyyy HH:mm"));
	d1.addValue (2, QDateTime::fromString("13.10.2013 12:30","dd.MM.yyyy HH:mm"));
	d1.addValue (6, QDateTime::fromString("13.11.2013 12:30","dd.MM.yyyy HH:mm"));
	d1.addValue (2, QDateTime::fromString("13.11.2013 15:30","dd.MM.yyyy HH:mm"));
	d1.addValue (3, QDateTime::fromString("13.11.2013 19:30","dd.MM.yyyy HH:mm"));
	d1.addValue (3, QDateTime::fromString("13.11.2013 19:50","dd.MM.yyyy HH:mm"));
	d1.addValue (4, QDateTime::fromString("13.11.2013 20:30","dd.MM.yyyy HH:mm"));
	d1.addValue (4, QDateTime::fromString("14.11.2013 00:30","dd.MM.yyyy HH:mm"));
	d1.addValue (5, QDateTime::fromString("14.11.2013 09:30","dd.MM.yyyy HH:mm"));
	d1.addValue (4, QDateTime::fromString("14.11.2013 13:30","dd.MM.yyyy HH:mm"));
	d1.addValue (8, QDateTime::fromString("14.11.2013 15:30","dd.MM.yyyy HH:mm"));
	d1.addValue (6, QDateTime::fromString("14.11.2013 16:30","dd.MM.yyyy HH:mm"));
	d1.addValue (7, QDateTime::fromString("17.11.2013 09:30","dd.MM.yyyy HH:mm"));
	d1.addValue (8, QDateTime::fromString("17.11.2013 10:30","dd.MM.yyyy HH:mm"));
	d1.addValue (9, QDateTime::fromString("17.11.2013 11:30","dd.MM.yyyy HH:mm"));
	d1.addValue (7, QDateTime::fromString("17.11.2013 12:30","dd.MM.yyyy HH:mm"));
	d1.addValue (12, QDateTime::fromString("18.11.2013 12:30","dd.MM.yyyy HH:mm"));
	d1.addValue (11, QDateTime::fromString("18.11.2013 13:30","dd.MM.yyyy HH:mm"));
	d1.addValue (12, QDateTime::fromString("18.11.2013 15:30","dd.MM.yyyy HH:mm"));
	d1.addValue (13, QDateTime::fromString("19.11.2013 08:30","dd.MM.yyyy HH:mm"));
	d1.addValue (12, QDateTime::fromString("19.11.2013 09:30","dd.MM.yyyy HH:mm"));
	d1.addValue (11, QDateTime::fromString("19.11.2013 10:00","dd.MM.yyyy HH:mm"));
	d1.dump();
	d1.addValue (18, QDateTime::fromString("19.11.2013 09:45","dd.MM.yyyy HH:mm"));
	d1.addValue (18, QDateTime::fromString("19.11.2013 09:50","dd.MM.yyyy HH:mm"));
	d1.addValue (34, QDateTime::fromString("13.11.2013 22:30","dd.MM.yyyy HH:mm"));
	d1.dump();
	d1.condenseOldEntries ();
	d1.dump();
}

void DataBase::store (QDataStream & s) const
{
	s << _db.size();
	for (int i=0; i < _db.size(); ++i) {
		s << _db[i].startDateTime();
		s << _db[i].endDateTime();
		s << _db[i].data();
	}
}

void DataBase::restore (QDataStream & s)
{
	int n = 0;
	_db.clear ();
	s >> n;
	for(int i=0; i< n ; ++i) {
		QDateTime st; 
		QDateTime et; 
		QVariant v;
		s >> st;
		s >> et;
		s >> v;
		DbEntry k(v,st);
		k.setEnd (et);
		_db.push_back (k);
	}
	condenseOldEntries ();
}

void DataBase::addValue (const QVariant& v, const QDateTime & tim)
{
	if(! _db.size()) {
		_db.push_back (DbEntry(v,tim));
		return;
	}

	for (int i = 0; i< _db.size(); ++i) {
		if (_db[i].endDateTime () >= tim && _db[i].startDateTime () <= tim ) {

			if (_db[i].data() == v) return;

			bool endmatch = _db[i].endDateTime () == tim;
			bool startmatch = _db[i].startDateTime () == tim;

			if (endmatch && startmatch) {
				_db[i].data() = v;
				return;
			} 
			if (endmatch) {
				_db[i].setEnd (_db[i].startDateTime ());
				_db.insert(i+1, DbEntry (v, tim));
				return;
			}
			if (startmatch) {
				_db[i].setStart (_db[i].endDateTime());
				_db.insert(i, DbEntry (v, tim));
				return;
			}
			_db.insert(i+1, DbEntry (_db[i].data(), _db[i].endDateTime()));
			_db[i].setEnd (_db[i].startDateTime());			
			_db.insert(i+1, DbEntry (v, tim));
			return;
		}
	}

	for (int i = _db.size()-1; i>=0; --i) {
		if (_db[i].endDateTime () < tim ) {
			if (_db[i].data() == v) {
				_db[i].setEnd (tim);
				return;
			}
			_db.insert(i+1, DbEntry (v,tim));
			return;
		}
	}

	for (int i = 0; i< _db.size(); ++i) {
		if (_db[i].startDateTime () > tim ) {
			if (_db[i].data() == v) {
				_db[i].setStart (tim);
				return;
			}
			_db.insert(i, DbEntry(v,tim));
			return;
		}
	}


};

int DataBase::lastIncreaseEntry () const
{
	if (!_db.size()) return -1;
	if (_db.size() == 1) return 0;
	if (!_interpolatable) return lastChangeEntry ();
	int k = _db.size()-1;
	int val = _db[k].data().toInt();
	int nval = val;
	--k;
	while (k >= 0) {
		val = nval;
		nval = _db[k].data().toInt();
		if (nval < val) break;
		--k;
	}
	return k+1;
};

QVariant DataBase::valueAt (const QDateTime & tim) const 
{
	QVariant r;
	if (!_db.size() ) return r;
	int ne = 0;
	for (int i = 0;i< _db.size(); ++i) {
		if (_db[i].startDateTime() > tim) break;
		ne = i;
	}		
	if (ne == 0 )  // before the very first entry
		r = _db[ne].data(); 
	else if (ne == (_db.size()-1)) // after the last entry
		r = _db[ne].data(); 
	else if ( tim <= _db[ne].endDateTime()) // somewhere inside an entry
		r = _db[ne].data(); 
	else { // try interpolate linear between two entries
		if (!_interpolatable) {
			return _db[ne].data();
		}
		double v0 = _db[ne].data().toInt(); 
		double v1 = _db[ne+1].data().toInt(); 
		double dv = v1-v0;     
		double dt = _db[ne].endDateTime().secsTo(_db[ne+1].startDateTime());
		double t = _db[ne].endDateTime().secsTo(tim);
		r = v0 + dv * t / dt;
	}
	return r;
};


/*! The first entry of a certain day has to end on that day
*/
int DataBase::firstOfDay(const QDate & d) const
{
	for (int i=0;i<_db.size();++i) 
		if (_db[i].endDate() == d) 
			return i;
	return -1;
};

/*! The last entry of a certain day has to start on that day
*/
int DataBase::lastOfMonth(const QDate & d) const 
{
	if (!_db.size()) return -1;
	for (int i = _db.size()-1;i>=0;--i) {
		QDate dd = _db[i].startDate();
		if (dd.year() == d.year() && dd.month() == d.month()) 
			return i;
	}
	return -1;
};

/*! The last entry of a certain day has to start on that day
*/
int DataBase::lastOfDay(const QDate & d) const 
{
	if (!_db.size()) return -1;
	for (int i = _db.size()-1;i>=0;--i) {
		if (_db[i].startDate() == d) 
			return i;
	}
	return -1;
};

/*! Keep at most 2 entries per day if the stamp is older than 2 days and
at most 2 per month if the entry is older than 2 months
*/
void DataBase::condenseOldEntries () 
{
	if (!_interpolatable) return;
	if (_db.size() < 4) return;
	QVector < DbEntry > ndb;
	QDate today = QDate::currentDate ();

	QString ts = today.toString("dd.MM.yy");
	int i = 0;
	while (i < _db.size()) {

		// store the entry if its value differs from the last stored value
		if (i == 0 || ndb[ndb.size()-1].data() != _db[i].data()) 
			ndb.push_back(_db[i]);
		else {
			ndb[ndb.size()-1].setEnd (_db[i].endDateTime());
		}

		QDate ed = _db[i].endDate();
		QString tes = ed.toString("dd.MM.yy");
		int age = ed.daysTo (today);
		if (age < 2) {
			// keep all younger entries
			++i;
			continue;
		}
		int ld = -1;
		if (age > 60) {
			ld = lastOfMonth (ed);
		} 
		if (ld < 0) {
			ld = lastOfDay (ed);
		}
		// find next entry starting on the day this entry ends on
		if (ld < 0 || ld == i) {
			// no end entry found ==> process next
			++i;
			continue;
		}

		i = ld;
	}
	_db = ndb;
};

QDataStream& operator << (QDataStream &st,const DataBase& s){s.store(st);return st;};
QDataStream& operator >> (QDataStream &st,DataBase& s){s.restore(st);return st;};
QDataStream& operator << (QDataStream &st,const PlayerStats& s){s.store(st);return st;};
QDataStream& operator >> (QDataStream &st,PlayerStats& s){s.restore(st);return st;};
QDataStream& operator << (QDataStream &st,const TownStats& s){s.store(st);return st;};
QDataStream& operator >> (QDataStream &st,TownStats& s){s.restore(st);return st;};
QDataStream& operator << (QDataStream &st,const AllyStats& s){s.store(st);return st;};
QDataStream& operator >> (QDataStream &st,AllyStats& s){s.restore(st);return st;};


void Stats::handlePlayer (int id, const QString &name, int points, int ally_id, int ntowns, int off, int deff)
{
	PlayerStats &p = player(id);
	p._name.addValue(name);
	p._points.addValue(points);
	p._ally_id.addValue(ally_id);
	p._ntowns.addValue(ntowns);
	p._off.addValue(off);
	p._deff.addValue(deff);
}

void Stats::handleAlly   (int id, const QString &name, int points, int nplayers, int ntowns, int off, int deff)
{
	AllyStats & p = ally (id);
	p._name.addValue(name);
	p._points.addValue(points);
	p._nplayers.addValue(nplayers);
	p._ntowns.addValue(ntowns);
	p._off.addValue(off);
	p._deff.addValue(deff);
}

void Stats::handleTown   (int id, const QString &name, int points, int player_id, int ally_id)
{
	TownStats &p = town(id);
	p._name.addValue(name);
	p._points.addValue(points);
	p._player_id.addValue(player_id);
	p._ally_id.addValue(ally_id);
}

void Stats::handleConquest  (int timestamp, int town_id, int points, int old_player_id, int old_ally_id, int new_player_id, int new_ally_id)
{
	QDateTime ti = QDateTime::fromTime_t(timestamp);
	QDateTime lti = QDateTime::fromTime_t(timestamp-1);
	TownStats & p = town(town_id);
	p._points.addValue (points,ti);
	p._player_id.addValue (old_player_id,lti);
	p._player_id.addValue (new_player_id,ti);
	p._ally_id.addValue (old_ally_id,lti);
	p._ally_id.addValue (new_ally_id,ti);
}

void Stats::handlePlayerHistory  (int timestamp, int stamp_end, int player_id,  int ally_id, int ntowns, int points, int off, int deff)
{
	QDateTime ti = QDateTime::fromTime_t(timestamp);
	QDateTime et = QDateTime::fromTime_t(stamp_end);
	PlayerStats &p = player(player_id);
	p._points.addValue(points,ti);
	p._points[p._points.size()-1].setEnd (et);
	p._ally_id.addValue(ally_id,ti);
	p._ally_id[p._ally_id.size()-1].setEnd (et);
	p._off.addValue(off,ti);
	p._off[p._off.size()-1].setEnd (et);
	p._ntowns.addValue(ntowns,ti);
	p._ntowns[p._ntowns.size()-1].setEnd (et);
	p._deff.addValue(deff,ti);
	p._deff[p._deff.size()-1].setEnd (et);
}


Stats::Stats ()
{
}

void Stats::restore (QDataStream & s)
{
	int version = 0;
	s >> version;
	QList<int> aid;
	QList<int> pid;
	QList<int> tid;
	s>>aid;
	s>>pid;
	s>>tid;
	for (int i=0;i< aid.size();++ i) {AllyStats a; s>>a; _allys.push_back(a); _ally_hash[aid[i]] = i;}
	for (int i=0;i< pid.size();++ i) {PlayerStats a; s>>a; _players.push_back(a); _player_hash[pid[i]] = i;}
	for (int i=0;i< tid.size();++ i) {TownStats a; s>>a; _towns.push_back( a) ; _town_hash[tid[i]] = i;}
	s >> _num_players;
	s >> _num_allyless;
	s >> _num_towns;
	s >> _num_ghosts;
	s >> _num_allys;
}

void Stats::store (QDataStream & s) const
{
	int version = 1;
	s << version;
	QList<int> aid = _ally_hash.keys();
	QList<int> pid = _player_hash.keys();
	QList<int> tid = _town_hash.keys();
	s<<aid;
	s<<pid;
	s<<tid;
	for (int i=0;i< aid.size();++ i) s << _allys[_ally_hash[aid[i]]];
	for (int i=0;i< pid.size();++ i) s << _players[_player_hash[pid[i]]];
	for (int i=0;i< tid.size();++ i) s << _towns[_town_hash[tid[i]]];
	s << _num_players;
	s << _num_allyless;
	s << _num_towns;
	s << _num_ghosts;
	s << _num_allys;
}


PlayerStats & Stats::player (int id) {
	if(!_player_hash.contains(id)) {
		_player_hash[id] = _players.size();
		PlayerStats pl;
		_players.push_back (pl);
	}
	return _players[_player_hash[id]];
};

AllyStats & Stats::ally (int id) {
	if(!_ally_hash.contains(id)) {
		_ally_hash[id] = _allys.size();
		AllyStats al;
		_allys.push_back (al);
	}
	return _allys[_ally_hash[id]];
};
TownStats & Stats::town (int id) {
	if(!_town_hash.contains(id)) {
		_town_hash[id] = _towns.size();
		TownStats to;
		_towns.push_back (to);
	}
	return _towns[_town_hash[id]];
};

PlayerStats Stats::player (int id) const {
	if(!_player_hash.contains(id)) {
		PlayerStats pl;
		return pl;
	}
	return _players[_player_hash[id]];
};
AllyStats Stats::ally (int id) const {
	if(!_ally_hash.contains(id)) {
		AllyStats al;
		return al;
	}
	return _allys[_ally_hash[id]];
};
TownStats Stats::town (int id)const  {
	if(!_town_hash.contains(id)) {
		TownStats to;
		return to;
	}
	return _towns[_town_hash[id]];
};



QDataStream& operator << (QDataStream &st,const Stats& s){s.store(st);return st;};
QDataStream& operator >> (QDataStream &st,Stats& s){s.restore(st);return st;};

DataBase & Stats::dataBase (Entry t, int id)
{
	if (t == PlayerName) {
		return player(id)._name;
	} else if ( t == PlayerPoints) {
		return player(id)._points;
	} else if ( t == PlayerAllyId) {
		return player(id)._ally_id;
	} else if ( t == PlayerOff) {
		return player (id)._off;
	} else if ( t == PlayerDeff) {		
		return player(id)._deff;
	} else if ( t == PlayerNTowns) {
		return player(id)._ntowns;

	} else if ( t == AllyName) {
		return ally(id)._name;
	} else if ( t == AllyPoints) {
		return ally(id)._points;
	} else if ( t == AllyNPlayers) {
		return ally(id)._nplayers;
	} else if ( t == AllyNTowns) {
		return ally(id)._ntowns;
	} else if ( t == AllyDeff) {
		return ally(id)._deff;
	} else if ( t == AllyOff) {
		return ally(id)._off;

	} else if ( t == TownName) {
		return town(id)._name;
	} else if ( t == TownPoints) {
		return town(id)._points;
	} else if ( t == TownPlayerId) {
		return town(id)._player_id;
	} else if ( t == TownAllyId) {
		return town(id)._ally_id;

	} else if ( t == WorldNumPlayers) {
		return _num_players;
	} else if ( t == WorldNumAllyless) {
		return _num_allyless;
	} else if ( t == WorldNumAllys) {
		return _num_allys;
	} else if ( t == WorldNumTowns) {
		return _num_towns;
	} else if ( t == WorldNumGhosts) {
		return _num_ghosts;
	} 
	return _dmy;
}

DataBase Stats::dataBase (Entry t, int id) const 
{
	if (t == PlayerName) {
		return player(id)._name;
	} else if ( t == PlayerPoints) {
		return player(id)._points;
	} else if ( t == PlayerAllyId) {
		return player(id)._ally_id;
	} else if ( t == PlayerOff) {
		return player (id)._off;
	} else if ( t == PlayerDeff) {		
		return player(id)._deff;
	} else if ( t == PlayerNTowns) {
		return player(id)._ntowns;

	} else if ( t == AllyName) {
		return ally(id)._name;
	} else if ( t == AllyPoints) {
		return ally(id)._points;
	} else if ( t == AllyNPlayers) {
		return ally(id)._nplayers;
	} else if ( t == AllyNTowns) {
		return ally(id)._ntowns;
	} else if ( t == AllyDeff) {
		return ally(id)._deff;
	} else if ( t == AllyOff) {
		return ally(id)._off;

	} else if ( t == TownName) {
		return town(id)._name;
	} else if ( t == TownPoints) {
		return town(id)._points;
	} else if ( t == TownPlayerId) {
		return town(id)._player_id;
	} else if ( t == TownAllyId) {
		return town(id)._ally_id;

	} else if ( t == WorldNumPlayers) {
		return _num_players;
	} else if ( t == WorldNumAllyless) {
		return _num_allyless;
	} else if ( t == WorldNumAllys) {
		return _num_allys;
	} else if ( t == WorldNumTowns) {
		return _num_towns;
	} else if ( t == WorldNumGhosts) {
		return _num_ghosts;
	} 
	return _dmy;
}


