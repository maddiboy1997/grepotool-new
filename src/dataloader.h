#ifndef ___DATALOADER_H
#define ___DATALOADER_H
/* dataloader.h  - data downloader prototype

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
#include <QObject>
#include <QByteArray>
#include <QFile>


class Download : public QObject{
	Q_OBJECT;
public:
	Download(const QString & url,const QString & file, const QString & descript, int index,bool nec=true):_url(url),_file(file),_descript(descript),_rep(0),_bytes(0), _idx(index),_neccessary(nec),_failed(false){};
	~Download(){if(_rep) _rep->deleteLater();};
	const QString& url () const {return _url;};
	const QString& file () const {return _file;};
	const QString& description () const {return _descript;};
	qint64 bytes () {QMutexLocker ml(&_m); return _bytes;};
	QString errorString() {return _rep->errorString();};
	bool neccessary() const {return _neccessary;};
	bool failed() const {return _failed;};
	void setFailed()  {_failed = true;};
	QByteArray readAll () {return _rep->readAll ();};

public slots:
	void abort () {if(_rep) _rep->abort();};
	void get (QNetworkAccessManager *nm) {
		QNetworkRequest req ( _url ) ;			
		_rep = nm->get( req );
		connect (_rep, SIGNAL(downloadProgress ( qint64 , qint64 )),
			this, SLOT(progress(qint64 , qint64 )), Qt::QueuedConnection);
		connect (_rep, SIGNAL(finished()),
			this, SLOT(finished()), Qt::QueuedConnection);

	};
	void progress ( qint64 a, qint64 ) {
		_m.lock();
		_bytes = a;
		_m.unlock ();
		emit progressSignal ();
	};

	void finished () {
		emit finishedSignal (_idx, (int)_rep->error());
	};

signals:
	void progressSignal ();
	void finishedSignal (int,int);

private:
	QMutex         _m;  
	QString        _url;
	QString        _file;
	QString        _descript;
	QNetworkReply *_rep;
	qint64         _bytes;
	int            _idx;
	bool           _neccessary;
	bool           _failed ;

};

/*! Loading of data is done in two up to three steps:

1. Try to load locally stored files (in processFiles)
2. Initiate a download if the files are missing
3. Retry loading and process the local files
*/

class DataLoader : public QThread {
	Q_OBJECT;
public:
	DataLoader (QObject* par);
	virtual ~DataLoader ();

	virtual QVector <Download*> processFiles (bool force_download) = 0;

	QString errorTitle () const {return error_title;};
	QString errorDescription () const {return error_desc;};
	QString errorDetails () const {return error_detail;};
	bool    filesDownloaded () const {return downloaded;};
	void    run (); 
	void    abort () { abort_requested=true ; };
	bool    abortRequested () {QMutexLocker ml(&m); return abort_requested;};
	QNetworkReply::NetworkError netError() const;	
	QFile::FileError            fileError() const;

protected:
	void setError (const QString& err_tit, const QString& err_desc, const QString& err_det);
	void startDownloads();
	void downloadsComplete();

public slots: 
	void loadAsync (bool force_download);
	void loadSync (bool force_download);

protected slots: 

	void downloadProgress();
	void downloadFinished(int,int);

signals:
	void completedSignal (bool error);
	void progressSignal (int perc, const QString& msg);

private:
	QMutex                      m;
	QTime                       start_time;
	bool                        abort_requested;
	bool                        force;
	QString                     error_title;
	QString                     error_desc;
	QString                     error_detail;
	bool                        downloaded; 
	QVector<Download*>          downloads;
	QNetworkAccessManager      *netman;
	QNetworkReply::NetworkError net_err;
	QFile::FileError            file_err; 
	int                         cnt;
	qint64                      bytes;
	bool                        sync;
};


#endif