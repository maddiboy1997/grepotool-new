/* bashies.cpp -  world data: Bashpoints

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

#include "world_data.h"
#include "defines.h"
#include "app.h"
#include "json.h"


bool WorldDataLoader::processBashies() {
	QString head = tr("Processing bashpoints of %1").arg(data.world());
	bool err=false;
	emit progressSignal(-1,head);
	App::processEvents();
	QString msg;

	QByteArray ba = readGzipfile(data.offFile(),err);
	if ( err ) {
		setError(data.offFile(),tr("Failed to read attack bashpoints"), tr("GUnzip failed"));
		return false;
	}
	QTextStream ds(&ba, QIODevice::ReadOnly);
	if ( ds.status() != QTextStream::Ok ) {
		setError(data.offFile(),tr("Failed to read attack bashpoints"), tr("Data is possibly corrupted"));
		return false;
	}
	
	int tot=0;
	int operc=-1;
	while (!ds.readLine().isEmpty()) ++tot;
	ds.resetStatus();
	ds.seek(0);

	int i=0;
	QString li = ds.readLine();
	while ( ds.status() == QTextStream::Ok ) {
		QStringList k = li.split(",");
		int perc = 50 * (i+1)/(tot+1) ;
		if (perc>operc) {
			operc=perc;
			emit progressSignal (perc, head);
			App::instance->processEvents();
		}

		if(abortRequested()) {
			setError ("", tr("User abort"),tr("Loading was cancelled by user"));
			return false;
		};
		if(k.size()!= 3) {
		setError(data.offFile(),
			tr("Failed to read attack bashpoints"), 
			tr("Corrupted file"));
			return false;
		}
		int id = data.playerHash()[k[1].toInt()];
		int pts = k[2].toInt();
		data.players() [id].off = pts;		
		int al = data.players() [id].ally;
		if(al>=0) data.allys()[al].off +=pts;
		++i;
		li = ds.readLine();
		if (li.isEmpty()) break;
	}

	ba = readGzipfile(data.deffFile(),err);
	if ( err ) {
		setError(data.deffFile(),tr("Failed to read defence bashpoints"), tr("GUnzip failed"));
		return false;
	}
	QTextStream nds (&ba, QIODevice::ReadOnly);
	if ( nds.status() != QTextStream::Ok ) {
		setError(data.deffFile(),tr("Failed to read defence bashpoints"), tr("Data is possibly corrupted"));
		return false;
	}
	
	tot=0;
	operc=-1;
	while (!nds.readLine().isEmpty()) ++tot;
	nds.resetStatus();
	nds.seek(0);

	i=0;
	li = nds.readLine();
	while ( ds.status() == QTextStream::Ok ) {
		QStringList k = li.split(",");
		int perc = 50 + 50 * (i+1)/(tot+1) ;
		if (perc>operc) {
			operc=perc;
			emit progressSignal (perc, head);
			App::instance->processEvents();
		}

		if(abortRequested()) {
			setError ("", tr("User abort"),tr("Loading was cancelled by user"));
			return false;
		};
		if(k.size()!= 3) {
		setError(data.offFile(),
			tr("Failed to read defence bashpoints"), 
			tr("Corrupted file"));
			return false;
		}
		int id = data.playerHash()[k[1].toInt()];
		int pts = k[2].toInt();
		data.players() [id].deff = pts;		
		int al = data.players() [id].ally;
		if(al>=0) data.allys()[al].deff +=pts;
		++i;
		li = nds.readLine();
		if (li.isEmpty()) break;
	}
	return true;
}; 
