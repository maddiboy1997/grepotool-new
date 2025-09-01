/* popup.cpp - Popup widgets

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

#include "popup.h"
#include "app.h"
#include <QLabel>
#include <QPushButton>
#include <QDesktopWidget>
#include <QColorDialog>
#include <QKeyEvent>


Popup::Popup (QWidget* par):QWidget (par, Qt::Popup|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint), loop(0),isInit(false),accepted(false) {};


void Popup::popup (const QPoint & pos, QEventLoop * l)
{
	accepted = false;
	if (!isInit) {
		isInit = true;
		QGridLayout * l1 = new QGridLayout;
		QString h = headline();
		int r = 0;
		if (!h.isEmpty()) {
			QLabel * lab = new QLabel(h);
			l1->addWidget(lab,0,0,1,3);
			++r;
		}
		QWidget *wid = createWidget();
		l1->addWidget(wid,r,0,1,3);
		l1->setMargin(0);
		++r;
		bool ok = okBtn();
		bool cnl = cancelBtn();
		if (ok || cnl) {
			int bc=2;
			if (cnl){
				QPushButton *bt = new QPushButton(tr("Cancel"));
				l1->addWidget(bt, r, bc--);
				connect (bt,SIGNAL(clicked()), this, SLOT(cancel()));
			}
			if (ok){
				QPushButton *bt = new QPushButton(tr("Ok"));
				l1->addWidget(bt, r,bc);
				connect (bt,SIGNAL(clicked()), this, SLOT(accept()));
			}
			l1->addItem(new QSpacerItem(0,0),r,0);
		}
		setLayout (l1);
		
	}
	updateGeometry();
	show();
	
	updateGeometry();
	loop = l;
	QSize s= size();
	QSize ds= App::desktop()->size();
	int x = pos.x();
	int y = pos.y();
	if (x+s.width() > ds.width()) x-=s.width();
	if (y+s.height() > ds.height()) y-=s.height();
	move(QPoint(x,y));
	show();	
	l->exec();
}

bool Popup::event ( QEvent * ev ) {
	if (loop) {
		if (ev->type() == QEvent::Close ) 
			exitLoop();
		if (ev->type() == QEvent::FocusOut) 
			exitLoop();			
	}
	if (ev->type()== QEvent::KeyPress) {
		QKeyEvent * k= dynamic_cast<QKeyEvent * >(ev);
		if(k->key() == Qt::Key_Escape) {
			cancel();
		} else if (k->key() == Qt::Key_Enter ||k->key() == Qt::Key_Return) {
			accept();
		}
	}
	return QWidget::event(ev);
};	

void Popup::exitLoop () {
	if (!loop) return;
	loop->exit();
	loop = 0;
};




ColorPopup::ColorPopup (QWidget * parent, const QColor &col, const QString & t) : Popup (parent), val (col),tx(t)
{
}

QWidget * ColorPopup::createWidget ()
{
	QColorDialog * dlg = new QColorDialog(val,0);
	dlg->setOption (QColorDialog::NoButtons);
	connect (dlg, SIGNAL (currentColorChanged ( const QColor & )), this, SLOT(colorChanged ( const QColor & ) ));
	return dlg;
}


QColor ColorPopup::value() const 
{
	return val;
}

QColor ColorPopup::selectColor (const QPoint & pos, const QColor &c, const QString & tx)
{
	ColorPopup w(0, c, tx);
	QEventLoop l;
	w.popup (pos, &l);
	return w.isAccepted()?w.value():c;	
}

void ColorPopup::colorChanged( const QColor& c)
{
	val = c;
}

TextPopup::TextPopup (QWidget * parent, const QString &col, const QString & t)  : Popup (parent), val (col),tx(t)
{
};


QWidget * TextPopup::createWidget () {
	QLineEdit *l = new QLineEdit(val);
	l->selectAll();
	connect (l,SIGNAL(textEdited ( const QString & )), this, SLOT(textChanged(const QString &)));
	connect (l,SIGNAL(editingFinished ( )), this, SLOT(accept()));
	return l;
};


QString TextPopup::selectText (const QPoint & pos, const QString &c, const QString & tx)
{
	TextPopup w(0, c, tx);
	QEventLoop l;
	w.popup (pos, &l);
	return w.isAccepted()?w.value():c;	
};

QWidget * SelectorPopup::createWidget ()
{
	QWidget * ret = new QWidget;
	
	ui.setupUi (ret);
	connect (ui._line,SIGNAL (textEdited(const QString&)), this, SLOT(textEdited(const QString&)));
	connect (ui._list, SIGNAL (itemActivated ( QListWidgetItem *) ), this, SLOT(itemActivated ( QListWidgetItem *  ) ));
	 ui._list->hide();
	return ret;
}

void SelectorPopup::itemActivated ( QListWidgetItem * i ) 
{
	val = i->data(Qt::UserRole).toInt();
	accept ();
}
	
SelectorPopup::SelectorPopup (QWidget * parent, const QStringList &col, const QString & t) : Popup (parent), val (-1), items(col), tx(t)
{
};

int SelectorPopup::select (const QPoint & pos, const QStringList &items, const QString & tx)
{
	SelectorPopup w(0, items, tx);
	QEventLoop l;
	w.popup (pos, &l);
	return w.isAccepted()?w.value():-1;	
};

void SelectorPopup::textEdited (const QString &s)
{
	if (s.length()< 2) {
		 ui._list->hide();
		return;
	}

	ui._list->clear();

	int mxl = 50;
	QVector <bool> add (items.size());

	for (int i=0;i< items.size()&& ui._list->count()<mxl; ++i) {
		add[i] = false;
		if(!items[i].startsWith(s,Qt::CaseInsensitive)) continue;
		add[i] = true;
		QListWidgetItem *nit= new QListWidgetItem (items[i]);
		nit->setData(Qt::UserRole, i);
		ui._list->addItem (nit);
	}
	
	for (int i=0;i< items.size()&& ui._list->count()<mxl; ++i) {
		if(!items[i].contains (s,Qt::CaseInsensitive)) continue;	
		if(add[i]) continue;
		QListWidgetItem *nit= new QListWidgetItem (items[i]);
		nit->setData(Qt::UserRole, i);
		ui._list->addItem (nit);
	}
	
	if(!ui._list->count()) {
		ui._list->hide();
	} else {
		ui._list->show();
	}

}



QWidget * DialogPopup::createWidget ()
{
	return new QLabel (text);
}

DialogPopup::DialogPopup (QWidget * parent, const QString &col, const QString & t, bool o, bool c) :  Popup (parent), tx(t),text(col), bok(o),bcnl(c)
{
}

bool DialogPopup::show (const QPoint & pos, const QString &tx, const QString & ti, bool o, bool c)
{
	DialogPopup w(0, tx,ti,o,c);
	QEventLoop l;
	w.popup (pos, &l);
	return w.isAccepted();
}

