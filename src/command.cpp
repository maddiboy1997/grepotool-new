/* command.cpp - Menu command

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
#include "command.h"
#include "world_data.h"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>


static QVector <Cmd*> __cmd_list;

Cmd::Cmd (QObject * par, const QString& id, 
		  const QString & title,		
		  const QString & tt,
		  const QString & wt,
		  const QIcon   & icon): QObject (par),_id(id), _title(title),
		  _icon(icon), _tip (tt), _what(wt), _enbl(true)
{
	if(_id.isEmpty ()) return;
	__cmd_list.push_back(this);
}

Cmd::~Cmd()
{
	if(_id.isEmpty ()) return;
	for(int i = 0;i< __cmd_list.size(); ++i) {
		if (__cmd_list[i] && __cmd_list[i]->_id == _id) {__cmd_list[i] = 0; return;};
	}
}

Cmd* Cmd::get (const char *id)
{
	for(int i = 0;i< __cmd_list.size(); ++i) {
		if (__cmd_list[i] && __cmd_list[i]->_id == id) return __cmd_list[i];
	}
	return 0;
}

Cmd* Cmd::get (const QString & id)
{
	for(int i = 0;i< __cmd_list.size(); ++i) {
		if (__cmd_list[i] && __cmd_list[i]->_id == id) return __cmd_list[i];
	}
	return 0;
}

void Cmd::setWorld (const WorldData & w)
{
	Setting::setWorldData (w);
	for(int i = 0;i< __cmd_list.size(); ++i) {
		if (__cmd_list[i] ) __cmd_list[i]->update();
	}
}

WorldData & Cmd::worldData ()
{
	return Setting::worldData ();
}


CmdAct::CmdAct (QObject * par, const QString& key,
				const QString & title,		
				const QString & tt,
				const QString & wt,
				const QIcon   & ic) : Cmd (par,key,title,tt,wt,ic) 
{
	_btn = qobject_cast<QAbstractButton*> (par);
	_act = new QAction (title, this);
	_act->setToolTip(tt);
	_act->setWhatsThis(wt);
	if (!ic.isNull()) _act->setIcon(ic);
	connect (_act,SIGNAL(triggered()),this, SLOT(triggered()));
	if (!_btn) return;
	connect (_btn,SIGNAL(clicked()),this, SLOT(triggered()));
	_btn->setToolTip(tt);
	_btn->setWhatsThis(wt);
	QToolButton * tb = qobject_cast<QToolButton *> (par);
	if (!tb) return;
	if (!ic.isNull()) tb->setIcon(ic);
	updateWidgets ();
}

void  CmdAct::updateAll  ()
{
	_act->setText (title());
	_act->setToolTip(tooltip());
	_act->setWhatsThis(what());
	_act->setEnabled (isEnabled ());
	if (!icon().isNull()) _act->setIcon(icon());
	if (_btn) {
		_btn->setToolTip(tooltip());
		_btn->setWhatsThis(what());
		_btn->setEnabled (isEnabled ());
		QToolButton * tb = qobject_cast<QToolButton *> (_btn);
		if (tb && !icon().isNull()) tb->setIcon(icon());
	}
	updateWidgets();
}

void  CmdAct::addToMenu           (QMenu *m)
{
	m->addAction (_act);
}

void  CmdAct::addToToolBar        (QToolBar *m) 
{
	m->addAction (_act);
}





CmdColor::CmdColor (QObject * par,
					const QString& key,
					const QColor & col,
					const QString & title,		
					const QString & tt,
					const QString & wt) : Cmd (par,key,title,tt,wt,QIcon()), _set(0,key,col)
{
	_btn = qobject_cast<QToolButton*> (par);
	_act = new QAction (title, this);
	_act->setToolTip(tt);
	_act->setWhatsThis(wt);
	QPixmap pm(32,32);
	pm.fill(col);
	_act->setIcon(pm);
	connect (_act,SIGNAL(triggered()),this, SLOT(triggered()));
	connect (&_set, SIGNAL(valueChanged()), this, SLOT(update()));
	connect (&_set, SIGNAL(valueChanged()), this, SLOT(valueChanged()));

	if (!_btn) return;
	_btn->setToolTip(tt);
	_btn->setWhatsThis(wt);
	_btn->setIcon(pm);
	connect (_btn,SIGNAL(clicked()),this, SLOT(triggered()));
};

void CmdColor::updateAll            ()
{
	_act->setToolTip(tooltip());
	_act->setWhatsThis(what());
	_act->setText (title());
	QPixmap pm(32,32);
	pm.fill(value());
	_act->setIcon(pm);

	if (!_btn) return;
	_btn->setToolTip(tooltip());
	_btn->setWhatsThis(what());
	_btn->setIcon(pm);
}

void CmdColor::addToMenu  (QMenu *m)
{
	m->addAction (_act);
}
void CmdColor::addToToolBar (QToolBar *m) 
{
	m->addAction (_act);
}

void CmdColor::triggered ()
{
	setValue(ColorPopup::selectColor (QCursor::pos(), value(), title()));
}




CmdCheck::CmdCheck (QObject * par, 
					const QString& key,
					bool  def,
					const QString & title,		
					const QString & tt,
					const QString & wt,
					const QIcon   & ic): CmdAct (par,key,title,tt,wt,ic),_set(0,key,def),_cb(0)
{
	_act->setCheckable(true);
	_act->setChecked(_set.value().toBool());
	connect (_act, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
	connect (&_set, SIGNAL(valueChanged()), this, SLOT(update()));
	connect (&_set, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));

	if(!_btn) return;
	_btn->setCheckable(true);
	_btn->setChecked(_set.value().toBool());
	connect (_btn, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
}

void CmdCheck::addToToolBar (QToolBar *m) 
{
	if (!icon().isNull()) {
		m->addAction (_act);
		return;
	}
	_cb = new QCheckBox;
	_cb->setChecked(_set.value().toBool());
	_cb->setEnabled(isEnabled());
	_cb->setText(title());
	_cb->setToolTip (tooltip());
	_cb->setWhatsThis(what());
	m->addWidget (_cb);
	connect (_cb, SIGNAL(toggled(bool)), this, SLOT(toggled(bool)));
}

void CmdCheck::updateWidgets () 
{
	bool ov = isChecked();
	if (_act->isChecked() != ov) _act->setChecked(ov);
	if (_btn) {
		if (_btn->isChecked() != ov) _btn->setChecked(ov);
	}
	if (_cb) {
		if (_cb->isChecked() != ov) _cb->setChecked(ov);
		_cb->setEnabled(isEnabled());
		_cb->setText(title());
		_cb->setToolTip (tooltip());
		_cb->setWhatsThis(what());
	}
}

void CmdCheck::toggled (bool b) 
{
	bool ov = isChecked();
	setChecked (b);
	if (ov == b) update ();
	else {
		emit toggledSignal (b);
		emit toggledSignal (_id , b);
	}
}














CmdGroup::CmdGroup (QObject * par,
					const QString& key,
					int            def,
					const QString & title,		
					const QString & tt,
					const QString & wt,
					const QIcon   & ic) : Cmd (par,key,title,tt,wt,ic),_set(0,key,def) ,_cb(0),_sm(0),_la (0)
{
	_ag = new QActionGroup (this);

	if (!icon().isNull()) {		
		_sm = new QMenu;
		_sm->setTitle(title);
		_sm->setIcon(ic);
		_sm->setToolTip (tooltip());
		_sm->setWhatsThis(what());
	}
	connect (_ag, SIGNAL (triggered(QAction*)), this, SLOT(triggered(QAction*)));

	connect (&_set, SIGNAL(valueChanged()), this, SLOT(update()));
	connect (&_set, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));

}
CmdGroup::~CmdGroup ()
{
	if(_sm) delete _sm;
}

QAction* CmdGroup::addAction (const QString & ti,
							  const QString & tt,
							  const QString & wt,
							  const QIcon  & ic) 
{
	QAction *a = new QAction (ti,this);
	a->setCheckable (true);
	a->setChecked(_ag->actions().size() == value());
	a->setToolTip (tt);
	a->setWhatsThis(wt);
	a->setData(_ag->actions().size());
	if (!ic.isNull()) a->setIcon(ic);
	_ag->addAction(a);
	return a;
}

void CmdGroup::updateAll ()
{
	bool enbl = isEnabled();
	int v = value();
	for (int i=0;i< _ag->actions().size(); ++i) {
		_ag->actions()[i]->setEnabled (enbl);
	}
	if (!_ag->actions()[v]->isChecked()) _ag->actions()[v]->setChecked(true);
	
	if (_sm) {
		_sm->setTitle(title());
		_sm->setIcon (icon());
		_sm->setToolTip (tooltip());
		_sm->setWhatsThis(what());
		_sm->setEnabled (enbl);
	} 
	if (_cb) {
		_cb->setEnabled (enbl);
		_cb->setToolTip (tooltip());
		_cb->setWhatsThis(what());
		if(_cb->currentIndex() != v) _cb->setCurrentIndex(v);
		_la->setEnabled (enbl);		
		_la->setText(title());
		_la->setToolTip (tooltip());
		_la->setWhatsThis(what());
	}
	updateWidgets();
}

void CmdGroup::addToMenu (QMenu *m)
{
	if (_sm) {		
		m->addMenu (_sm);
		_sm->clear();
		_sm->setToolTip (tooltip());
		_sm->setWhatsThis(what());
		QList <QAction*> l = _ag->actions();
		for (int i=0;i< l.size(); ++i)
			_sm->addAction (l[i]);
		m->addMenu (_sm);
	} else {	
		QList <QAction*> l = _ag->actions();
		for (int i=0;i< l.size(); ++i)
			m->addAction (l[i]);
	}
}

void CmdGroup::addToToolBar (QToolBar *m)
{
	if (icon().isNull()) {

		for (int i=0; i < _ag->actions().size(); ++i) {
			m->addAction(_ag->actions() [i]);
		}

	} else {

		// add a label and acombobox
		_la  = new QLabel;				
		_la->setPixmap(icon().pixmap(m->iconSize()));
		_la->setToolTip (tooltip());
		_la->setWhatsThis(what());
		m->addWidget (_la);

		_cb = new QComboBox;
		_cb->setEditable(false);		
		for (int i=0;i< _ag->actions().size(); ++i) {
			if(_ag->actions()[i]->icon().isNull())
				_cb->addItem (_ag->actions()[i]->text());
			else
				_cb->addItem (_ag->actions()[i]->icon(),_ag->actions()[i]->text());
		}
		_cb->setToolTip (tooltip());
		_cb->setWhatsThis(what());
		_cb->setCurrentIndex(value());
		connect (_cb, SIGNAL (currentIndexChanged(int)), this, SLOT(selectionChanged(int)));
		connect (this, SIGNAL(selectionChangedSignal(int)),_cb,SLOT(setCurrentIndex(int)));
		m->addWidget (_cb);
		return;
	}
}

void CmdGroup::triggered (QAction *a)
{
	selectionChanged (a->data().toInt());
}

void CmdGroup::selectionChanged (int a)
{
	int ov = value();
	setValue (a);  // triggers an update 
	if (ov == a) update();
	else {
		emit selectionChangedSignal (a);
		emit selectionChangedSignal (_id, a);
	}
}


