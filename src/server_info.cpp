/* server_info.cpp - Info on available grepo servers

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

#include "server_info.h"
#include "app.h"
#include "json.h"
#include "setting.h"
#include "command_ids.h"
#include "defines.h"

GrepoServer::GrepoServer (const QString & server, const QString & world, Type t) : _server_name(server), _world_name(world), _type(t)
{
	_state = (t == Custom)? Unchecked : Valid;	
	_rep = 0;

	QString sb = QString("%1/%2/").arg(WORLDS).arg(server);

	_name_s = new Setting (this, sb+ "WorldName", world);
	_type_s = new Setting (this, sb+ "Type",       t);
	_state_s = new Setting (this, sb+"State",     (int) _state);
	_stamp_s = new Setting (this, sb+"LastVisit",  QDateTime());

	App::processEvents();
	_last_visit = _stamp_s->value().value<QDateTime>(); 
	_state      = (State) _state_s->value().toInt(); 

	if (server == "de41" || server == "de48") {
		QString lt = _last_visit.toString();
	}

	_act = new QAction (countryIcon(), QString ("(")+ server +") " + world, this);
	_act->setToolTip (countryLong() + " ("+ server +")");	
	_act->setCheckable (true);
	connect(_act, SIGNAL(triggered()),this,SLOT(triggerLoad()));
}

void GrepoServer::eraseSettings () 
{
	Setting::erase(_name_s->key());
	Setting::erase(_type_s->key());
	Setting::erase(_state_s->key());
	Setting::erase(_stamp_s->key());
	_name_s = 0;
	_type_s = 0;
	_state_s = 0;
	_stamp_s = 0;
}
void GrepoServer::updateStamp ()
{
	_last_visit = QDateTime::currentDateTime ();
	if (_stamp_s) _stamp_s->setValue (_last_visit);
};


static const char * __country_short [23] = {"br","cz","de","dk","en","es","fr","gr","hu","it","jp",
		"kr","nl","no","pl","pt","ro","ru","se","sk","th","tr","us"};

static int countryIndex (const QString& srv)
{
	for (int i= 0; i< 23; ++i) {
		if (srv == __country_short[i]) return i;
	}
	return -1;
}

static QString extractCountryFromServerName (const QString& s) 
{
	if (s.length()<3) return "";
	QString srv = s.left(2);
	bool ok = false;
	s.mid(2).toInt(&ok);
	if (!ok) return "";
	for (int i= 0; i< 23; ++i) {
		if (srv == __country_short[i]) return __country_short[i];
	}
	return "";
}

static int extractNumberFromServerName (const QString& s) 
{
	if (s.length()<3) return -1;
	QString srv = s.left(2);
	bool err = false;
	int n = s.mid(2).toInt(&err);
	if (err) return -1;
	return n;
}
QString GrepoServer::countryShort () const 
{
	return extractCountryFromServerName (_server_name );
}

QString GrepoServer::countryLong (const QString & cs)
{
	static const QString cn [23] = {tr("Brazil"),tr("Czech Republic"),tr(
		"Germany"),tr("Denmark"),tr("International"),tr("Spain"),tr("France"),tr("Greece"),tr(
		"Hungary"),tr("Italy"),tr("Japan"),tr("Korea"),tr("Netherlands"),tr("Norway"),tr(
		"Poland"),tr("Portugal"),tr("Romania"),tr("Russia"),tr("Sweden"),tr(
		"Slovakia"),tr("Thailand"),tr("Turkey"),tr("United States")};
	if (cs.isEmpty()) return tr ("Unknown country");
	return cn[countryIndex(cs)];
}

QIcon GrepoServer::countryIcon (const QString & cs)
{
	if (cs.isEmpty()) return QPixmap(":/icons/flag_unknown.png");
	return QPixmap(QString(":/icons/flag_%1.png").arg(cs));
}

QString GrepoServer::countryLong () const
{
	QString cs = extractCountryFromServerName(_server_name); 
	if (cs.isEmpty()) return tr ("Unknown country");
	return countryLong (cs);
}

QIcon GrepoServer::countryIcon () const
{
	QString cs = extractCountryFromServerName(_server_name); 
	return countryIcon(cs);	
}

QString GrepoServer::worldName () const
{
	return _world_name;
}

QString GrepoServer::serverName () const
{
	return _server_name;
}

void GrepoServer::setWorldName (const QString & n)
{
	_world_name = n;
}

GrepoServer::Type GrepoServer::type () const 
{
	QMutexLocker ml(&_m); 
	return _type;
};

GrepoServer::State GrepoServer::state () const 
{
	QMutexLocker ml(&_m); 
	return _state;
};

void GrepoServer::store   (QDataStream & s) const
{
	s << (int ) type();
	s << _server_name;
	s << _world_name;
};
void GrepoServer::restore (QDataStream & s) {
	int t;
	s >> t; _type = (Type) t;
	s >> _server_name;
	s >> _world_name;
}

void GrepoServer::run () {
	if (type() == Predefined ) return;
	QNetworkAccessManager netman;
	QNetworkRequest req ( QString("http://%1.grepolis.com/data/units.json").arg(serverName())) ;	
	_rep = netman.get( req );
	connect (_rep, SIGNAL(finished()), this, SLOT(pingFinished()), Qt::QueuedConnection);
	exec();
}; 

void GrepoServer::pingFinished ()
{
	_m.lock();
	_state = Invalid;
	if (_rep->error() == QNetworkReply::NoError) {

		QString s (_rep->readAll ());
		bool success = false;
		QVariant u = QtJson::parse(s, success);
		if(success) 
			_state = Valid;
	}
	QString sb = QString("%1/%2/").arg(WORLDS).arg(serverName());
	Setting::setValue (sb + "State", (int) _state);

	_m.unlock();
	emit stateChangedSignal (this);
	_rep->deleteLater();
	_rep = 0;
	exit();
}


void GrepoServer::triggerLoad () // action triggered
{
	App::instance->aquireNewWorld (_server_name, _world_name);
}


ServerList * ServerList::instance = 0;

ServerList::ServerList (QObject *p) : QObject(p), _tb(0) ,_menu(0){
	instance = this;

	_menu = new QMenu;
	
	new Setting (this, CURRENT_WORLD, QString ("de41"));
	Setting::connect(CURRENT_WORLD, this, SLOT(currentWorldChanged()));

	const char * de_worlds [] = { "Alpha","Beta","Gamma", "Delta","Epsilon","Zeta","Hyperborea", "Eta",
	 "Theta", "Iota", "Kappa", "Lambda", "My", "Ny", "Xi", "Omikron","Pi", "Rho", "Sigma",
	 "Tau", "Ypsilon", "Phi", "Chi", "Psi", "Omega", "Athen", "Byzanz", "Achilles","Corinth",
	 "Delphi", "Ephesus", "Gythium", "Heraklion","Bellerophon","Ithaca", "Juktas","Knossos","Lamia",
	 "Marathon", "Elysium", "Naxos", "Olympia", "Pella", "Rhethymnos", "Sparta","Troja", "Zakros",
	 "Actium", "Baris", "Calydon", "Delos", "Edessa", ""};

	QSettings s (appTitle(), appCfgKey());	
	s.beginGroup (WORLDS);
	int i=0;

	s.setValue ("de57/WorldName/value",  "Kos");		
	s.setValue ("de57/Type/value",       (int) GrepoServer::Predefined);

	while ( *(de_worlds[i])) {
		QString wn = QString("de%1").arg(i+1);

		QString nam = s.value (wn+"/WorldName/value",QString(de_worlds[i])).toString();
		int ty =  s.value (wn+"/Type/value",(int) GrepoServer::Predefined).toInt();

		s.setValue (wn+"/WorldName/value",  nam);		
		s.setValue (wn+"/Type/value",       ty);
		++i;
	}

	QStringList aw = s.childGroups();
	for (int i=0;i< aw.size(); ++i) {		
		QString nam = s.value (QString("%1/WorldName/value").arg(aw[i])).toString();
		int ty =  s.value (QString("%1/Type/value").arg(aw[i])).toInt();
		GrepoServer* srv = new GrepoServer (aw[i], nam, ( GrepoServer::Type )ty);
		_list.push_back(srv);
	}
};


ServerList::~ServerList()
{
	for (int i=0;i< _list.size();++i) _list[i]->deleteLater();
	delete _menu;
}

void ServerList::currentWorldChanged ()
{
	GrepoServer * cur = find (Setting::value (CURRENT_WORLD).toString());
	if(!cur) 
		return;
	cur->updateStamp ();
	updateServerMenu();
}

QStringList ServerList::allCountries ()
{
	QStringList r ;
	for (int i=0;i< _list.size(); ++i) {
		QString cs = _list[i]->countryShort ();
		if(!r.contains(cs)) r.push_back (cs);
	}
	r.sort();
	return r;
}

QVector <GrepoServer*> ServerList::allValidWorldsOfCountry (const QString& ctr)
{
	QStringList r ;
	for (int i=0;i< _list.size(); ++i) {
		QString cs = _list[i]->countryShort ();
		if (cs != ctr) continue;
		r.push_back (_list[i]->serverName());
	}
	r.sort();
	QVector <GrepoServer*> ret;

	for (int i=0;i< r.size(); ++i) {
		ret.push_back(find(r[i]));
	}
	
	return ret;
}

GrepoServer* ServerList::find (const QString& server_name)
{
	for (int i=0;i< _list.size(); ++i) {
		if(_list[i]->serverName () == server_name) return _list[i];
	}
	return 0;
}

void ServerList::serverStateChanged(QObject* o)
{
	GrepoServer * gs = dynamic_cast<GrepoServer*> (o);

	if (gs->state() == GrepoServer::Invalid) {
		_tryout_ok = false;
	} else {
		_tryout_ok = true;
	}
	_m.lock ();
	_tryout_complete = true;
	_m.unlock ();
}

bool ServerList::tryOutServer (const QString& srv, const QString& name)
{
	_tryout_ok = false;
	_tryout_complete = false;
	GrepoServer  *gs = new GrepoServer (srv, name);
	connect (gs, SIGNAL(stateChangedSignal(QObject*)), this, SLOT(serverStateChanged(QObject*))); 
	gs->start();
	_m.lock ();
	while (!_tryout_complete) {
		_m.unlock();
		App::processEvents();
		_m.lock ();
	}
	_m.unlock();
	if (_tryout_ok) {
		_list.push_back (gs);
		updateServerMenu ();
		return true;
	}

	gs->eraseSettings();
	QSettings s (appTitle(), appCfgKey());	
	s.beginGroup (WORLDS);
	s.remove (srv);
	delete gs;
	return false;
}


QVector <GrepoServer*> ServerList::mostRecent ()
{
	QVector <GrepoServer*> r;
	for (int i=0;i < _list.size(); ++i) {
		if(_list[i]->stamp ().isValid ())
			r.push_back(_list[i]);
	}
	if (r.size() < 2) return r;
	for (int i=0; i < (r.size()-1); ++i) {
		for (int j=i+1; j < r.size(); ++j) {
			if (r[i]->stamp() < r[j]->stamp()) {
				GrepoServer* d= r[i];
				r[i] = r[j]; 
				r[j]= d;
			}
		}
	}
	return r;
}


void ServerList::addToToolBar (QToolBar* tb)
{
	QString cs = Setting::value(CURRENT_WORLD).toString();
	GrepoServer * cur = find(cs);
	_tb = tb;
	_tb_ico = new QLabel (tb);
	_tb_bt = new QPushButton (tb);
	QSize s= tb->iconSize();
	QIcon pm = QIcon(":/icons/world.ico");
	_tb_ico->setPixmap (pm.pixmap(s));
	_tb_bt->setText (QString("<b>%1 %2</b>").arg(cur->serverName()).arg(cur->worldName()));
	tb->addWidget (_tb_ico);
	tb->addWidget (_tb_bt);
}

void ServerList::updateServerMenu ()
{
	if (_tb) {

		QString cs = Setting::value(CURRENT_WORLD).toString();

		GrepoServer * cur = find(cs);

		QSize s =  _tb->iconSize();
		QIcon pm = QIcon(":/icons/world.ico");
		_tb_ico->setPixmap (pm.pixmap(s));
		_tb_bt->setText (QString("%1 %2").arg(cur->serverName()).arg(cur->worldName()));
	}
	if(!_menu) return;

	_menu->clear();
	QMenu * cm = _menu->addMenu (tr("All worlds"));
	QStringList wl = allCountries ();
	for(int i=0;i< wl.size(); ++i) {
		QMenu * m = cm->addMenu(GrepoServer::countryIcon(wl[i]),GrepoServer::countryLong(wl[i]));
		QVector <GrepoServer*> s = allValidWorldsOfCountry (wl[i]); 
		for(int j = 0;j< s.size(); ++j) {
			m->addAction (s[j]->action());
		}
	}
	_menu->addAction (tr("Manual selection"),this,SLOT (tryNewWorld()));
	QVector <GrepoServer*> mr = mostRecent ();
	if (mr.size() > 1) {
		_menu->addSeparator();
 		cm = _menu->addMenu (tr("Recently visited"));
		for(int i=1; i < 10 && i < mr.size(); ++i) {
			cm->addAction (mr[i]->action());
		}
	}
}




DlgServer::DlgServer ():mode(0)
{
	ui.setupUi(this);
	ui._info->setText (tr("Please enter a valid server and click on proceed."));
	ui._btn->setText(tr("Proceed"));
	ui._btn->setEnabled (false);
	connect (ui._btn,SIGNAL(clicked()), this, SLOT(btnClicked()));
	connect (ui._name,SIGNAL(textChanged(const QString&)),this, SLOT(checkOk()));
	connect (ui._srv,SIGNAL(textChanged(const QString&)),this, SLOT(checkOk()));	
}

void DlgServer::checkOk ()
{
	QString srv = ui._srv->text();
	QString nam = ui._name->text();
	ui._info->setText (tr("Please enter a valid server and click on proceed."));
	ui._btn->setEnabled (!srv.isEmpty () && !nam.isEmpty());	
}

void DlgServer::btnClicked ()
{
	QString srv = ui._srv->text();
	QString nam = ui._name->text();
	setCursor(Qt::BusyCursor);
	ui._info->setText (tr("Validating server '%1'...").arg(srv));
	ui._name->setEnabled(false);
	ui._srv->setEnabled(false);
	if (ServerList::instance->tryOutServer (srv,nam)) {
		App::instance->aquireNewWorld (srv,nam);
		accept ();
	} else {
		ui._name->setEnabled(true);
		ui._srv->setEnabled(true);
		setCursor(Qt::ArrowCursor);
		ui._info->setText (tr("'%1' is not a valid Grepo server").arg(srv));
		ui._btn->setEnabled(false);
	}
}


void ServerList::tryNewWorld ()
{

	DlgServer dlg;
	dlg.exec ();

}





