/* setting.cpp - Cfg settings

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

#include "setting.h"
#include "defines.h"
#include "world_data.h"
#include "command_ids.h"
#include "app.h"

static QVector < Setting* > __settings;
static QString __settings_server;
WorldData __world;

#define NotifierEventId QEvent::User

class ChgEvent : public QEvent {
public:
	ChgEvent () : QEvent (NotifierEventId) {};
};



static QString settingsKey (const QString& key) {
	QStringList l = key.split("/");

	bool def_set = __settings_server.isEmpty();

	QString wn = def_set?"Default":__settings_server; 

	if (l.size() > 1) {
		if (l[0] == "World") {
			l[0] = wn;
		} 
	} else {
		l.push_front("Application");
	}
	l.push_back("value");
	return l.join("/");
}


Setting::Setting (QObject * par, const QString& key, const QVariant& def):QObject (par), _id(key), _val(def), _def(def) ,_dirty(false)
{
	
	QStringList l = key.split("/");
	if (l[0] != "World") {
		restore ();
	}
	__settings.push_back(this);
};

Setting::~Setting ()
{
	store ();
	for (int i=0; i< __settings.size();++i) {
		if (__settings[i] && __settings[i]->_id == _id) __settings[i] = 0;
	}
};

QVariant Setting::value () const
{
	return _val;
};

void Setting::setValue (const QVariant & v)
{
	if ( v == _nval ) return;
	_nval = v;
	if(!_dirty) {
		_dirty = true;
		App::postEvent(this, new ChgEvent);
	}
}

bool Setting::event ( QEvent * ev )
{
	if (ev->type() == NotifierEventId) {
		if(_dirty) {
			emit valueAboutToChange();
			_val = _nval;
			emit valueChanged ();
			_dirty = false;
		}
		ev->accept();
		return true;	
	}
	return QObject::event(ev);
}

WorldData & Setting::worldData () 
{
	return __world;
}
void Setting::setWorldData (const WorldData & w)
{
	bool prenote = !__settings_server.isEmpty();
	bool postnote = !w.server().isEmpty();

	if (prenote) {
		for (int i=0; i< __settings.size();++i) {
			__settings[i]->store ();
			if (__settings[i] && (__settings[i]->_id.startsWith("World") ||  __settings[i]->_id == CURRENT_WORLD)) {
				__settings[i]->emit valueAboutToChange();
			}
		}
		App::processEvents();
		__world.storeSettings ();
	}

	__world = w;
	__settings_server = w.server();

	if (postnote) {		
		for (int i=0; i< __settings.size();++i) {
			if (__settings[i] && __settings[i]->_id == CURRENT_WORLD) {
				__settings[i]->_val = __settings_server;
				break;
			}
		}
		for (int i=0; i< __settings.size();++i) {
			if (__settings[i] && __settings[i]->_id.startsWith("World")) {
				__settings[i]->restoreSilent ();
			}
		}
		for (int i=0; i< __settings.size();++i) {
			if (__settings[i] && __settings[i]->_id == CURRENT_WORLD) {
				__settings[i]->emit valueChanged();				
				break;
			}
		}
		for (int i=0; i< __settings.size();++i) {
			if (__settings[i] && __settings[i]->_id.startsWith("World")) {
				__settings[i]->emit valueChanged();				
			}
		}
	}
}


void Setting::store ()
{
	if(_id.startsWith("World/") && __settings_server.isEmpty ()) return;
	QSettings s (appTitle(), appCfgKey());	
	s.setValue (settingsKey(_id), _val);
}

void Setting::restore ()
{
	if(_id.startsWith("World/") && __settings_server.isEmpty ()) {
		setValue (_def);
		return;
	}
	QSettings s (appTitle(), appCfgKey());	
	setValue ( s.value(settingsKey(_id), _def) );
	App::processEvents();
}

void Setting::restoreSilent ()
{
	if(_id.startsWith("World/") && __settings_server.isEmpty ()) {
		_val = _def;
		return;
	}
	QSettings s (appTitle(), appCfgKey());	
	_val = s.value(settingsKey(_id), _def);
}

void Setting::reset ()
{
	setValue (_def);
}

QVariant Setting::value (const QString& key) 
{
	for (int i=0; i< __settings.size();++i) {
		if (__settings[i] && __settings[i]->_id == key) return __settings[i]->value();
	}
	return QVariant ();
}


bool Setting::exists (const QString& key)
{
	for (int i=0; i< __settings.size();++i) {
		if (__settings[i] && __settings[i]->_id == key) return true;
	}
	return false;
}

void Setting::erase (const QString& key)
{
	for (int i=0; i< __settings.size();++i) {
		if (__settings[i] && __settings[i]->_id == key) {
			delete __settings[i];
			__settings.erase (__settings.begin()+i);
			QSettings s (appTitle(), appCfgKey());	
			s.remove(settingsKey(key));
			return;
		}
	}
}


void Setting::setValue (const QString& key, const QVariant & v)
{
	for (int i=0; i< __settings.size();++i) {
		if (__settings[i] && __settings[i]->_id == key) __settings[i]->setValue(v);
	}
}


void Setting::connect (const QString& key, QObject*o, const char * slot)
{
	for (int i=0; i< __settings.size();++i) {
		if (__settings[i] && __settings[i]->_id == key) 
			QObject::connect (__settings[i], SIGNAL(valueChanged()), o, slot);
	}
}

void Setting::connect2 (const QString& key, QObject*o, const char * slot)
{
	for (int i=0; i< __settings.size();++i) {
		if (__settings[i] && __settings[i]->_id == key) 
			QObject::connect (__settings[i], SIGNAL(valueAboutToChange()), o, slot);
	}
}

