#ifndef ___WORLDDATA_H
#define ___WORLDDATA_H
/* worlddata.h  - data container 

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
#include <QtGui>
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Download;
class WorldData;

class SnapLoader : public QThread {
	Q_OBJECT;
public:
	SnapLoader (WorldData *p);

public slots:
	void load (const QString & f);
	void save (const QString & f);

	void progress (int, QString);

signals:
	void progressSignal (int, QString);
	void errorSignal (QString);
	void finishedSignal ();

private:
	void setupData ();
	WorldData* world;
};


/*! World data
*/
class WorldData  : public QObject {
	Q_OBJECT;
public:
	WorldData (	QString server, QString name );
	~WorldData ();

	bool loadComplete () const {
		return (islands_init && players_init && towns_init && allys_init && off_init && deff_init);
	};
	Town & town (int i) {return towns [i];};

	bool restoreLastSnapshot ();
	bool storeSnapshot (const QString & file);
	bool restoreSnapshot (const QString & file);

	int allyIndex (int id) const{for (int i=0;i< allys.size();++i) if (allys[i].id==id) return i; return -1;};  
	int playerIndex (int id) const{for (int i=0;i< players.size();++i) if (players[i].id==id) return i; return -1;};  
	int townIndex (int id) const{for (int i=0;i< towns.size();++i) if (towns[i].id==id) return i; return -1;};  
	int islandIndex (int x,int y) const {
		for (int i=0;i< islands.size();++i) 
			if (islands[i].x==x &&islands[i].y==y) 
				return i;
		return -1;
	}; 

public slots:

	void statusUpdate ();

	void downloadFailed         (QObject *);
	void deffDownloadComplete   (QObject *);
	void offDownloadComplete    (QObject *);
	void playerDownloadComplete (QObject *);
	void allyDownloadComplete   (QObject *);
	void townDownloadComplete   (QObject *);
	void islandDownloadComplete (QObject *);

	void downloadProgress (qint64 a);

	void initialize (QNetworkAccessManager * nm);

	void loaderProgress (int, QString);
	void loaderError (QString);
	void loaderFinished ();

signals:
	void statusChangedSignal (QString tx);
	void initCompleteSignal (bool error);

	void loadSignal (const QString & f);
	void saveSignal (const QString & f);

	void loadSaveProgressSignal (int, QString);

protected:
	void setupLinks ();

private:
	QMutex             m;
	SnapLoader *       loader;
	QDateTime          stamp;

	QStringList        err;
	qint64             bytes_downloaded;
	
	bool               islands_init;
	bool               players_init;
	bool               towns_init;
	bool               allys_init;
	bool               off_init;
	bool               deff_init;

	QVector <Download*> downloads;

public:
	QString            server_id;
	QString            world_name;
	// islands are considered static data and are stored in a separate 
	// binary file per world
	QVector <Island>   islands; 

	// The actual data are stored in a binary snapshot file 
	// in the worlds subdirectory. Named: 'snap-YYYY-MM-DD-hh-mm'
	QVector <Player>    players; 
	QVector <Alliance>  allys; 
	QVector <Town>      towns; 
	QVector <BashData>  deff;
	QVector <BashData>  off;

};

/*! Settings for the MapViewer
*/
class Settings {
public:
	Settings (const WorldData * d);
	~Settings ();
	bool load ();
	bool save ();
		
	const WorldData *world;
	bool show_grid;
	bool show_inf_map;
	int  map_opacity;

	QColor ocean_color;
	bool   show_ocean_labels;
	bool   show_ghosts;
	QColor ghost_color;

	QColor unsel_color;
	int    unsel_mode;
	
	
	int    ally_sort_column;
	bool   ally_sort_down;
	int    player_sort_column;
	bool   player_sort_down;

	QVector <QColor>    ally_cols;
	QVector <int >      ally_show;
	QVector <QColor>    player_cols;
	QVector <int >      player_show;

};




#endif