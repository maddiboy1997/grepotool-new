#ifndef __POPUP_H
#define __POPUP_H
/* popup.h  - Popup widgets

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

#include <QWidget>
#include <QEventLoop>
#include "ui_datetime.h"
#include "ui_completer.h"

#include "command.h"

/*! Baseclass for popup widgets that behave like a QMenu
*/
class Popup : public QWidget {
	Q_OBJECT;

public:
	Popup (QWidget* par);
	void popup (const QPoint & pos, QEventLoop * l);

	virtual void handleAccept () { accepted = true;};
	virtual void handleCancel () {};

	virtual QWidget * createWidget () = 0;

	virtual bool cancelBtn () const {return false;};
	virtual bool okBtn () const {return false;};
	virtual QString headline() const {return "";};

	bool isAccepted() const{return accepted;};

public slots:

	void accept (){handleAccept(); close ();};
	void cancel (){handleCancel(); close ();};

protected:
	bool event ( QEvent * ev );
	void exitLoop ();

	bool isInit;
	bool accepted;
	QEventLoop * loop;
};


class ColorPopup : public Popup {
	Q_OBJECT;
public:
	ColorPopup (QWidget * parent, const QColor &col, const QString & t="");
	QColor value() const;
	QWidget * createWidget ();

	bool cancelBtn () const {return true;};
	bool okBtn () const {return true;};
	QString headline() const {return tx;};

	static QColor selectColor (const QPoint & pos, const QColor &c, const QString & tx="");

public slots:
	void colorChanged( const QColor& c);
	
private:
	QColor val;	
	QString tx;
};

class TextPopup : public Popup {
	Q_OBJECT;
public:
	QString value() const {return val;};
	QWidget * createWidget ();

	bool cancelBtn () const {return !tx.isEmpty();};
	bool okBtn () const {return !tx.isEmpty();};
	QString headline() const {return tx;};

	TextPopup (QWidget * parent, const QString &col, const QString & t="");
	static QString selectText (const QPoint & pos, const QString &c, const QString & tx="");

public slots:
	void textChanged( const QString& c) {val=c;};
	
private:
	QString val;	
	QString tx;
};

class DialogPopup : public Popup {
	Q_OBJECT;
public:
	QWidget * createWidget ();
	bool cancelBtn () const {return bcnl;};
	bool okBtn () const {return bok;};
	QString headline() const {return tx;};

	DialogPopup (QWidget * parent, const QString &text, const QString & t, bool o, bool c);
	static bool show (const QPoint & pos, const QString &text, const QString & tx, bool o, bool c);

private:
	QString text;	
	QString tx;
	bool bok;
	bool bcnl;
};


class SelectorPopup : public Popup {
	Q_OBJECT;
public:
	int value() const {return val;};
	QWidget * createWidget ();

	bool cancelBtn () const {return false;};
	bool okBtn () const {return false;};
	QString headline() const {return tx;};

	SelectorPopup (QWidget * parent, const QStringList &col, const QString & t="");
	static int select (const QPoint & pos, const QStringList &items, const QString & tx="");

public slots:
	void itemSelected ( int c) {val=c; accept();};

	void textEdited (const QString &);
	void itemActivated ( QListWidgetItem * item ); 
private:
	int         val;	
	QStringList items;
	Ui::Completer ui;
	QString     tx;
};




class TimePopup : public Popup {
	Q_OBJECT;
public:
	TimePopup (QWidget * parent, QDateTime &t, const QString ti):Popup (parent) ,val(t),old(t),tit(ti){};
	static void selectTime (const QPoint & pos, QDateTime &t, const QString ti= QString()){
		TimePopup w(0, t, ti);
		QEventLoop l;
		w.popup (pos, &l);
	};

	virtual QWidget * createWidget () {
		QWidget * r = new QWidget(this);
		ui.setupUi(r);
		ui._date->setSelectedDate (val.date());
		ui._time->setDateTime (val);
		connect (ui._reset,SIGNAL(clicked()), this, SLOT(resetClicked()));
		connect (ui._time, SIGNAL (timeChanged (const QTime& ) ) , this, SLOT(timeChanged()));
		connect (ui._date, SIGNAL (selectionChanged()) , this, SLOT(dateChanged() ));	
		return r;
	};
	bool cancelBtn () const {return true;};
	bool okBtn () const {return true;};
	QString headline() const {return tit;};

public slots:

	void timeChanged () {
		val = ui._time->dateTime();
		ui._date->setSelectedDate(val.date());
	};

	void dateChanged () {
		ui._time->setDate(ui._date->selectedDate());
		val = ui._time->dateTime();
	};

	void resetClicked () {
		val = QDateTime::currentDateTime ();
		ui._time->setDateTime (val);
		ui._date->setSelectedDate (val.date());
	};

	void cancel () {val = old; };
	void ok () {val = ui._time->dateTime (); exitLoop(); };


private:
	Ui::DateTime ui;
	QDateTime &val;	
	QDateTime old;	
	QString tit;
};





#endif