#ifndef ___EXPLORER_H
#define ___EXPLORER_H
/* explorer.h  - settings tool

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
#include <QTableWidget>
#include <QColor>
#include <QDateTime>
#include <QPixmap>
#include <QByteArray>

class TableItem : public QTableWidgetItem {
public:
	TableItem (const QString &tx , int idx);
	int index;
};

class NumItem : public TableItem {
public:
	NumItem (int num, int idx);
	bool operator < (const QTableWidgetItem & o) const;
	int n;
};

class NumTextItem : public TableItem {
public:
	NumTextItem (int num, const QString &txt, int idx);
	bool operator < (const QTableWidgetItem & o) const;
	int n;
};

class TextItem : public TableItem {
public:
	TextItem (QString s, int idx, bool checkable, bool ch);
	bool operator < (const QTableWidgetItem & o) const;
	bool check;
};

class ColorItem : public TableItem {
public:
	ColorItem (QColor c, int idx);
	bool operator < (const QTableWidgetItem & o) const;
	void setColor (QColor c);
	QColor col;
};

class UnitItem : public TableItem {
public:
	UnitItem (UnitIndex u, int idx);
	void setUnit (UnitIndex c);
	UnitIndex unit;
};

class DateTimeItem : public TableItem {
public:
	DateTimeItem (QDateTime t, int idx);
	bool operator < (const QTableWidgetItem & o) const;
	void setDateTime (QDateTime c);
	QDateTime time;
};

class TimeItem : public TableItem {
public:
	TimeItem (int secs, int idx);
	bool operator < (const QTableWidgetItem & o) const;
	void setTime (uint c);
	int time;
};

class ActTimeItem : public TableItem {
	Q_DECLARE_TR_FUNCTIONS(ActTimeItem);
public:
	ActTimeItem (QDateTime t, int idx);
	bool operator < (const QTableWidgetItem & o) const;
	void setDateTime (QDateTime c);
	QDateTime time;
};


#endif