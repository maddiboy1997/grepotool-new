/* explorer.cpp - settings explorer

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

#include "explorer.h"
#include "defines.h"
#include "world_data.h"

TableItem::TableItem (const QString &tx , int idx) : QTableWidgetItem (tx,UserType), index(idx) {};

NumItem::NumItem (int num, int idx) : TableItem (QString::number(num),idx), n(num) {
	setTextAlignment (Qt::AlignRight|Qt::AlignVCenter);
	setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
};
bool NumItem::operator < (const QTableWidgetItem & o) const {
	const NumItem * nn= dynamic_cast<const NumItem *>(&o);
	if(!nn) return false;
	return n < nn->n;
};

NumTextItem::NumTextItem (int num, const QString &txt, int idx) : TableItem (txt,idx), n(num) {
	setTextAlignment (Qt::AlignRight|Qt::AlignVCenter);
	setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
};
bool NumTextItem::operator < (const QTableWidgetItem & o) const {
	const NumTextItem * nn= dynamic_cast<const NumTextItem *>(&o);
	if(!nn) return false;
	return n < nn->n;
};

TextItem::TextItem (QString s, int idx, bool checkable, bool ch) : TableItem (s,idx) ,check (checkable){
	if(checkable) {
		setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
		data(Qt::CheckStateRole);			
		setCheckState(ch?Qt::Checked:Qt::Unchecked);
	} else {
		setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	}
};
bool TextItem::operator < (const QTableWidgetItem & o) const
{
	return (text().compare(o.text(),Qt::CaseInsensitive)<0) ;
}


ColorItem::ColorItem (QColor c, int idx) : TableItem ("",idx), col(c) {
	setFlags (Qt::ItemIsEnabled);
	setBackground (c);		
};
bool ColorItem::operator < (const QTableWidgetItem & o) const {
	const ColorItem * nn= dynamic_cast<const ColorItem *>(&o);
	if (!nn) return false;
	int ca = col.red()+col.green()+col.blue();
	int cb = nn->col.red()+nn->col.green()+nn->col.blue();
	return ca<cb;
};
void ColorItem::setColor (QColor c) {
	this->setBackground (c);
	col = c;
}

UnitItem::UnitItem (UnitIndex u, int idx) : TableItem ("",idx), unit(u){		
	setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	setUnit (u);
};
void UnitItem::setUnit (UnitIndex c) {
	unit = c;
	setIcon (QIcon(QPixmap(WorldData::unitIconFile(c)).scaled(QSize(16,16))));
	setText (WorldData::unitName(c));
};

DateTimeItem::DateTimeItem (QDateTime t, int idx) :TableItem ("",idx), time(t){
	setTextAlignment (Qt::AlignCenter);
	setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	setDateTime (t);
};
bool DateTimeItem::operator < (const QTableWidgetItem & o) const {
	const DateTimeItem * nn= dynamic_cast<const DateTimeItem *>(&o);
	if (!nn) return false;
	return time < nn->time;
};
void DateTimeItem::setDateTime (QDateTime c) {
	time = c;
	if(!time.isValid()) setText ("---");
	else setText (time.toString ("dd.MM-hh:mm:ss"));
};

TimeItem::TimeItem (int secs, int idx) : TableItem ("",idx), time(secs) {
	setTextAlignment (Qt::AlignCenter);
	setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	setTime (secs);
};
bool TimeItem::operator < (const QTableWidgetItem & o) const {
	const TimeItem * nn= dynamic_cast<const TimeItem *>(&o);
	if (!nn) return false;
	if (time <0)  {
		return false;
	} else if(nn->time<0) {
		return true;
	}
	return time < nn->time;
};
void TimeItem::setTime (uint c) {
	time = c;
	if(time < 0){
		setText ("---");
		return;
	}
	int k = time;
	int ss = k % 60; // secs
	k /= 60;
	int mm = k % 60; // minutes
	k /= 60;
	int hh = k;  // hours
	setText (QString("%1:%2:%3").arg(hh,2,10,QChar('0')).arg(mm,2,10,QChar('0')).arg(ss,2,10,QChar('0')));
};

ActTimeItem::ActTimeItem (QDateTime t, int idx) :TableItem ("",idx), time(t){
	setTextAlignment (Qt::AlignCenter);
	setFlags (Qt::ItemIsEnabled|Qt::ItemIsSelectable);
	setDateTime (t);
};
bool ActTimeItem::operator < (const QTableWidgetItem & o) const {
	const ActTimeItem * nn= dynamic_cast<const ActTimeItem *>(&o);
	if (!nn) return false;
	return time < nn->time;
};
void ActTimeItem::setDateTime (QDateTime c) {

	time = c;
	if (!time.isValid()) {
		setText ("---");
		return;
	}
	QDateTime ct = QDateTime::currentDateTime();
	int da = c.daysTo(ct);
	int ho = c.secsTo(ct);
	ho /= 60; // mins
	if (da) {
		if(da>1) setText(tr("%1 days ago").arg(da));
		else setText(tr("1 day ago"));
	} else if (ho) {
		if (ho > 120) {
			ho /= 60; // hours
			setText(tr("%1 hours ago").arg(ho));				
		} else {
			setText(tr("active"));
		}
	} else setText(tr("active"));
};


