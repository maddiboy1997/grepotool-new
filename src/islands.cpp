/* islands.cpp -  world data: Islands

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

Island::Island ()
{
}

Island& Island::operator = (const Island& o)
{
	id = o.id;
	x = o.x;
	y = o.y;
	type = o.type;
	image = o.image;
	towns = o.towns; 
	return *this;
}

bool WorldDataLoader::processIslands() {

	QString head = tr("Processing islands of %1").arg(data.world());
	bool err=false;
	emit progressSignal(-1,head);
	App::processEvents();
	QString msg;
	QByteArray ba = readGzipfile(data.islandFile(),err);
	if ( err ) {
		setError(data.islandFile(),tr("Failed to read island descriptions"), "");
		return false;
	}
	QTextStream ds(&ba, QIODevice::ReadOnly);
	if ( ds.status() != QTextStream::Ok ) {
		setError(data.islandFile(),tr("Failed to read island descriptions"), tr("Data is possibly corrupted"));
		return false;
	}

	int tot=0;
	int operc=-1;
	while (!ds.readLine().isEmpty()) ++tot;
	ds.resetStatus();
	ds.seek(0);

	data.islands().resize(tot);

	int i=0;
	QString li = ds.readLine();
	while ( ds.status() == QTextStream::Ok ) {

		int perc = 100 * (i+1)/(tot+1) ;
		if (perc>operc) {
			operc=perc;
			emit progressSignal (perc, head);
			App::instance->processEvents();
		}
		if(abortRequested()) {
			setError ("", tr("User abort"),tr("Loading was cancelled by user"));
			return false;
		};

		QStringList k = li.split(",");
		if(k.size()!= 5) {
		setError(data.islandFile(),
			tr("Failed to read island descriptions"), 
			tr("Corrupted file"));
			return false;
		}
		int id = k[0].toInt();
		data.islands()[i].id = id; 
		int x= k[1].toInt();
		int y= k[2].toInt();
		int hash_id = x*1000 + y;
		data.islands()[i].y = (x&1)? y*128+64 : y*128;
		data.islands()[i].x = x * 128;
		int ilt = k[3].toInt();;
		data.islands()[i].type = ilt;
		data.islands()[i].towns.clear();
		data.islands()[i].image = glob.island(ilt);

		data.islandPosHash()[hash_id] = i;
		data.islandIdHash()[id] = i;

		++i;
		li = ds.readLine();
		if(li.isEmpty()) break;
	}

	return true;
}; 
