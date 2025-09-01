/* dataloader.cpp -  loading of data items

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
#include "dataloader.h"


DataLoader::DataLoader (QObject* par) : QThread(par),netman (0),force(false) {
	setTerminationEnabled ( true );
	abort_requested = false;

};

DataLoader::~DataLoader () {
	qDebug()<<"deleting DataLoader start \n";
	if(netman) 	
		netman->deleteLater();
	qDebug()<<"deleting DataLoader end \n";
};


void DataLoader::setError (const QString& err_ti,const QString& err_desc, const QString& err_det)
{
	error_title  = err_ti;
	error_desc   = err_desc;
	error_detail = err_det;
	// abort ();
};

void DataLoader::loadAsync (bool force_download) {
	sync = false;
	force = force_download;
	start();
}

void DataLoader::loadSync (bool force_download) {
	sync = true;
	force = force_download;
	netman = new QNetworkAccessManager ; 
	abort_requested = false;
	downloaded = false;
	downloads = processFiles(force);
	if (!downloads.size()) {
		downloaded = false;
		// 		QVector <Download*> d = processFiles(force);
		emit completedSignal(!error_desc.isEmpty());
		return;
	}
	startDownloads();
}

void DataLoader::run () { 
	netman = new QNetworkAccessManager ; 
	netman->moveToThread (this);
	abort_requested = false;
	downloaded = false;
	downloads = processFiles(force);
	if (!downloads.size()) {
		emit completedSignal(!error_desc.isEmpty());
		return;
	}
	startDownloads();
	exec();
	netman->deleteLater();
	netman = 0;
};


void DataLoader::downloadsComplete () {
	downloaded = true;
	QVector <Download*> d = processFiles(force);
	emit completedSignal(!error_desc.isEmpty());
	if(!sync) quit();
};

void DataLoader::downloadProgress()
{
	m.lock ();
	bytes = 0;
	for (int i=0;i<downloads.size(); ++i) {
		bytes  += downloads[i]->bytes();
	}
	int secs = start_time.secsTo(QTime::currentTime());	
	int nc   = bytes;	
	int perc = (100 * cnt) / downloads.size();
	m.unlock ();

	qint64 bps = 0;
	nc        *= 100;
	nc        /= 1024;
	if (secs) bps = nc/secs;
	emit progressSignal (perc, tr("Downloading (%1 kb at %2 kb/s)").arg((double)nc/100,8).arg((double)bps/100,8));
};


void DataLoader::startDownloads()
{
	cnt = downloads.size();

	for (int i=0;i<downloads.size(); ++i) {
		connect (downloads[i], SIGNAL(finishedSignal(int, int)),
			this,SLOT(downloadFinished(int , int )),Qt::DirectConnection);
		connect (downloads[i], SIGNAL(progressSignal()),
			this,SLOT(downloadProgress()));
		downloads[i]->get (netman);
	}
}

void DataLoader::downloadFinished(int index,int err)
{
	QNetworkReply::NetworkError er = (QNetworkReply::NetworkError) (err);

	m.lock ();
	--cnt;
	if (abort_requested) {
		for (int i=0;i< downloads.size(); ++i) {
			if (i==index) continue;
			downloads[i]->abort();
		}
		m.unlock();
		return;
	}
	int counter = cnt;
	QString url = downloads[index]->url();
	QString filename = downloads[index]->file();
	QString descript = downloads[index]->description();

	QString txt;
	if (er != QNetworkReply::NoError ) {
		downloads[index]->setFailed();

		if (downloads[index]->neccessary()) {
			for (int i=0;i< downloads.size(); ++i) {
				if (i==index) continue;
				downloads[i]->abort();		
			}

			// only notify the first error
			if (error_desc.isEmpty()) { 
				setError (tr("Data download failed"),
					tr("While downloading %1 %2").arg(descript).arg(url),
					downloads[index]->errorString());
				net_err = er;
			}
		}
	} else {
		QByteArray d = downloads[index]->readAll();
		QFile f(filename);
		bool fail = false;
		if (f.open (QIODevice::WriteOnly)) {
			if (-1 == f.write(d)) fail = true;
		} else fail=true;

		if(fail&&downloads[index]->neccessary()) {
			for (int i=0;i< downloads.size(); ++i) {
				if(i!= index)
					downloads[i]->abort();
			}
			// only notify the first error
			if (error_desc.isEmpty()) { 
				setError (tr("File storage failed"),
					tr("While saving %1 %2").arg(descript).arg(filename),
					f.errorString());
				file_err = f.error();
			}
		}
	}
	m.unlock();
	if (counter == 0) {
		downloadsComplete ();
	}
}







