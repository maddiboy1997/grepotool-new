#ifndef ___TOWN_INFO_H
#define ___TOWN_INFO_H
/* town_info.h  - Town database widget

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
#include "toolwidget.h"
#include "popup.h"
#include "defines.h"
#include "ui_town_info.h"


#include <QToolButton>
#include <QSpinBox>


class NumEdit : public Popup {
	Q_OBJECT;
public:
	NumEdit (QToolButton * p, int *v, const QString& icon_file) : Popup (p) , btn(p), icon(icon_file), val(v),box(0) {
		connect (btn,SIGNAL(clicked()),this,SLOT(onClick()));
		updateButton ();
	};

	QWidget * createWidget () 
	{
		box = new QSpinBox (this);
		box->setRange (0, 50000);
		int vv = 0;
		if(val) vv= *val;
		box->setValue (vv);
		connect(box,SIGNAL(valueChanged(int)),this,SLOT(updateButton()));
		return box;
	};

	void setValuePtr (int*v) {
		val = v;
		if (box) box->setValue (*v);
		updateButton();
	};

public slots:	
	void onClick() {
		QEventLoop l;
		QPoint p (0,0);
		popup(btn->mapToGlobal(p),&l);
	};
	void updateButton () {
		QPixmap ic = QPixmap(icon).scaled(40,40);
		QImage im (ic.toImage());
		QPainter p (&im);
		int vv=0;
		if (val) vv = *val;
		if (box) {
			vv = box->value();;
			if (val) *val = vv;
		}
		QFont f = p.font();
		f.setBold(true);
		f.setPixelSize(14);
		if(vv > 9999) {
			f.setPixelSize(11);
		}
		p.setFont(f);
		renderText (&p, QPoint(2, im.height()-4), QString::number(vv), Qt::black, Qt::white);
		btn->setIcon(QIcon(QPixmap::fromImage(im)));
	};

private:
	QString      icon;
	QToolButton *btn;
	QSpinBox    *box;
	int         *val;
};



class TownInfo : public ToolWidget {
	Q_OBJECT;
public:

	TownInfo ();
	~TownInfo ();

	void fillContextMenu (QMenu*);

	void worldChanged ();
	void initWidget ();

public slots:

	void selectTown (int idx);
	void updateTown ();

	void godChanged (int);
	void gotoClicked () {
		if(town>=0) 
			emit gotoTownSignal (town);
	};

signals:
	void townChangedSignal(int);

private:
	Ui::TownInfo ui;
	int town;
	QVector <QToolButton*> units;
	QVector <NumEdit*>     edits;
	QVector <QToolButton*> gods;
	QVector <QToolButton*> special;
	QButtonGroup * bg;
};


#endif