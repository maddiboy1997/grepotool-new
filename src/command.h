#ifndef ___COMMAND_H
#define ___COMMAND_H
/* command.h  - Menu commands

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
#include <QIcon>
#include <QString>
#include <QToolBar>
#include <QMenu>

#include "setting.h"

class WorldData;
class QAbstractButton;


/*! Abstract command interface class
*/
class Cmd : public QObject {
	Q_OBJECT;
public:
	Cmd (QObject * par, const QString& key,
		const QString & title,		
		const QString & tt,
		const QString & wt,
		const QIcon   & icon = QIcon());

	virtual ~Cmd();

	static Cmd* get      (const char *id);
	static Cmd* get      (const QString & id);
	static void setWorld (const WorldData & w);
	static WorldData & worldData ();

	virtual void addToMenu    (QMenu *m)        = 0; 
	virtual void addToToolBar (QToolBar *m)     = 0; 
	virtual void updateAll    ()                = 0;

	virtual QString title () const     {return _title;};
	virtual QString tooltip () const   {return _tip;};
	virtual QString what () const      {return _what;};
	virtual QIcon   icon() const       {return _icon;};
	virtual bool    isEnabled () const   {return _enbl;};

public slots:

	void setTitle   (const QString &b) {_title = b; update();};
	void setTooltip (const QString &b) {_tip   = b; update();};
	void setWhat    (const QString &b) {_what  = b; update();};
	void setIcon    (const QIcon &b)   {_icon  = b; update();};
	void setEnabled (bool b)           {_enbl  = b; update();};
	void update     ()                 { updateAll (); };

protected:
	QString                     _id; 
	QString                     _title;
	QIcon                       _icon;
	QString                     _tip;
	QString                     _what;
	bool                        _enbl;
};


/*! Action command - behaves like a button and can be linked to a button 

emits a triggeredSignal   

CmdAct adds a simple QAction to the Menu and Toolbar.

Overwrite addToToolbar in a Subclass if custom Toolbar-Widgeds are desired
*/
class CmdAct : public Cmd {
	Q_OBJECT;
public:
	CmdAct (QObject * par, const QString& key,
		const QString & title,		
		const QString & tt,
		const QString & wt,
		const QIcon   & ic= QIcon());

	void         updateAll            ();
	void         addToMenu            (QMenu *m); 
	virtual void addToToolBar         (QToolBar *m); 
	virtual void updateWidgets        ()                 {}; 

	void connectTo (QObject*o, const char * slot) {
		connect (this, SIGNAL(triggeredSignal()),o,slot);
	};
	void connectToId (QObject*o, const char * slot) {
		connect (this, SIGNAL(triggeredSignal(const QString &)),o,slot);
	};

public slots: 
	void triggered () {
		emit triggeredSignal ();
		emit triggeredSignal (_id);
	};

signals:
	void triggeredSignal ();
	void triggeredSignal (const QString & key);
	
protected:
	QAction         *_act;
	QAbstractButton *_btn;
};

class CmdColor : public Cmd {
	Q_OBJECT;
public:
	CmdColor (QObject * par, const QString& key,
		const QColor & col,
		const QString & title,		
		const QString & tt,
		const QString & wt);

	void updateAll            ();
	void addToMenu            (QMenu *m); 
	void addToToolBar         (QToolBar *m); 

	QColor value () const {return _set.value().value<QColor>();};
	void setValue (const QColor & c) {_set.setValue(c);};

public slots: 
	void triggered ();
	void valueChanged() {
		updateAll ();
		emit valueChangedSignal ();
		emit valueChangedSignal (_id);
	};

signals:
	void valueChangedSignal ();
	void valueChangedSignal (const QString &);
	
protected:
	Setting      _set;
	QAction     *_act;
	QToolButton *_btn;
};


class QCheckBox;

/* A checkable Action stored in the Cfg

*/
class CmdCheck : public CmdAct {
	Q_OBJECT;
public:
	CmdCheck (QObject * par, 
		const QString& key,
		bool  def,
		const QString & title,		
		const QString & tt,
		const QString & wt,
		const QIcon   & ic= QIcon());

	void addToToolBar         (QToolBar *m); 
	void updateWidgets        (); 

	bool isChecked () const {return _set.value().toBool();}; 
	void setChecked (bool c) {_set.setValue(c);};

public slots:
	void toggled (bool);

signals:
	void valueChanged ();
	void toggledSignal (bool);
	void toggledSignal (const QString & , bool);

protected:
	Setting    _set;
	QCheckBox *_cb;
};

class QActionGroup;
class QComboBox ;
class QMenu ; 
class QLabel;

/*! Behaves like a QActionGroup 

emits a triggeredSignal   

CmdGroup adds a QActionGroup to the Menu and Toolbar.

If an icon is set, the group get's added to a new submenu and a 
ComboBox is created in the toolbar

Overwrite addToToolbar and UpdateWidgets in a Subclass if custom Toolbar-Widgets are desired
*/
class CmdGroup : public Cmd {
	Q_OBJECT;
public:
	CmdGroup (QObject * par, const QString& key,
		int             def,
		const QString & title,		
		const QString & tt,
		const QString & wt,
		const QIcon   & ic= QIcon());

	~CmdGroup ();
	QAction* addAction (const QString & ti,
		const QString & tt=0,
		const QString & wt=0,
		const QIcon  & ic=QIcon());


	void         updateAll            ();
	void         addToMenu            (QMenu *m); 
	virtual void addToToolBar         (QToolBar *m); 
	virtual void updateWidgets        ()                 {}; 

	int value () {return _set.value().toInt();};
	void setValue (int v) {_set.setValue(v);};

public slots: 
	void triggered (QAction *);
	void selectionChanged (int);

signals:
	void valueChanged ();
	void selectionChangedSignal (int);
	void selectionChangedSignal (const QString &, int);
	
protected:
	Setting          _set;
	QActionGroup    *_ag;
	QComboBox       *_cb;
	QMenu           *_sm; 
	QLabel          *_la;
};



#endif
