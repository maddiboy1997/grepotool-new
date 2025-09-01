#ifndef __WINDOW_H
#define __WINDOW_H
/* window.h  - windows

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
#include <QMainWindow>

class CmdAct;

class Window  : public QMainWindow {
	Q_OBJECT;
public:
	Window (const QString& id);
	virtual ~Window ();

	QString widgetKey () const;
	QString worldWidgetKey () const;
	QString globalWidgetKey () const;

	QString stateKey () const;
	QString globalStateKey () const;
	QString worldStateKey () const;

	QString geometryKey () const;
	QString globalGeometryKey () const;
	QString worldGeometryKey () const;

	virtual void worldAboutToChange () {};
	virtual void worldChanged () {};
	virtual void initWidget () {};
	virtual void handleCmd (const QString& ) {};
	virtual void updateCommands () {};
	virtual void updateContent () {};

public slots:
	void cmdTriggered (const QString& key) {handleCmd(key);};

	void selectionChanged () {updateCommands ();};
	void dataChanged () {updateCommands (); updateContent ();};

	void restoreWindowState ();
	void storeWindowState ();
	void restoreWindowGeometry ();
	void storeWindowGeometry ();

protected:
	CmdAct * addCommand (QObject * par, const QString& key,
		const QString & title,		
		const QString & tt,
		const QString & wt,
		const QIcon   & ic= QIcon());

	bool      _is_init;
	QString   _id;
};

#endif