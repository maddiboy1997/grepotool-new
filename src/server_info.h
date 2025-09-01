#ifndef ___SERVER_INFO_H
#define ___SERVER_INFO_H
/* server_info.h  - Server info manager

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
#include <QtNetwork>
#include <QThread>
#include <QDataStream>
#include <QToolBar>
#include <QMenu>

#include "ui_dlg_server.h"

class Setting;

class GrepoServer : public QThread {
	Q_OBJECT;
public:
	enum Type {
		Predefined, // server worldname combination is not editable no recheck
		Custom     // editable worldname - recheck on init
	};

	enum State {
		Unchecked,  // needs recheck
		Invalid,     // to be removed from list
		Valid
	};

	GrepoServer (const QString & server, const QString & world, Type = Custom);

	QString countryShort () const; // de, en ... Empty for unknown
	QString countryLong () const;  // translation of Germany, International ...
	QIcon   countryIcon () const;  // flag 
	QString worldName () const;    // Actium, Sparta, ...
	QString serverName () const;   // de48, en3 ...

	static QString countryLong (const QString & cs);  // translation of Germany, International ...
	static QIcon   countryIcon (const QString & cs);  // flag 

	QAction * action () {return _act;};

	void setWorldName (const QString & n);

	QDateTime stamp () {return _last_visit;};


	Type type () const;
	State state () const;

	void store   (QDataStream & s) const;
	void restore (QDataStream & s);

	void run (); 

signals:
	void stateChangedSignal (QObject*);

public slots:
	void pingFinished ();
	void triggerLoad ();   // action triggered
	void updateStamp ();
	void eraseSettings ();
private:
	QAction *      _act;
	QDateTime      _last_visit; 

	mutable QMutex _m;
	Type           _type;
	State		   _state;

	QNetworkReply *_rep;
	QString        _server_name;
	QString        _world_name;

	Setting * _name_s;
	Setting * _type_s;
	Setting * _state_s;
	Setting * _stamp_s;

};

inline QDataStream& operator << (QDataStream &st,const GrepoServer& s) {
	s.store(st);
	return st;
};

inline QDataStream& operator >> (QDataStream &st,GrepoServer& s) {
	s.restore(st);
	return st;
};

class ServerList : public QObject {
	Q_OBJECT;
public:

	ServerList (QObject *p);	
	~ServerList();
	QStringList allCountries (); // de, en, gr, ... empty if unknown countries are present
	QVector <GrepoServer*> allValidWorldsOfCountry (const QString& ctr); // ctr = de, en, gr, ...
	QVector <GrepoServer*> mostRecent (); 
	static ServerList * instance;

	GrepoServer* find (const QString& server_name);
	QMenu* serverMenu (){return _menu;};
	void addToToolBar (QToolBar* tb);

	bool tryOutServer (const QString& srv, const QString& name);

public slots:
	void updateServerMenu ();
	void currentWorldChanged ();
	void tryNewWorld ();
	void serverStateChanged(QObject*);

protected:
	QMenu * _menu;
	QMenu * _recent;
	QMenu * _countries;
	QVector <QMenu*> _cm;
	QVector <GrepoServer*> _list;
	QPushButton *_tb_bt;
	QLabel      *_tb_ico;
	QToolBar*   _tb;
	QMutex      _m;
	bool        _tryout_ok;
	bool        _tryout_complete;
};


class DlgServer : public QDialog {
	Q_OBJECT;
public:
	DlgServer ();

public slots: 
	void btnClicked ();
	void checkOk ();

protected:
	int mode;
	Ui::DlgServer ui;
};

#endif