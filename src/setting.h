#ifndef ___SETTING_H_DEFINED
#define ___SETTING_H_DEFINED
/* setting.h  - Cfg settings

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

#include <QObject>
#include <QVariant>


class WorldData;
class QEvent;

class Setting : public QObject  {
	Q_OBJECT;
public:
	Setting (QObject * par, const QString& key, const QVariant& def);
	~Setting ();

	QString key () const {return _id;};

	QVariant value () const;
	void     setValue (const QVariant & v);

	static void erase (const QString& key);
	static bool exists (const QString& key);
	static QVariant value (const QString& key);
	static void connect  (const QString& key, QObject*, const char * slot);
	static void connect2  (const QString& key, QObject*, const char * slot);
	static void setValue (const QString& key, const QVariant & v);

	static void setWorldData (const WorldData & w);
	static WorldData & worldData ();

public slots:
	void     reset ();
	void     store ();
	void     restore ();
	void     restoreSilent ();

signals:
	void valueAboutToChange ();
	void valueChanged ();

protected:
	bool event ( QEvent * ev );

private:
	mutable QMutex m;
	QString     _id;
	bool        _dirty; 
	QVariant    _nval;
	QVariant    _val;
	QVariant    _def;
};



#endif