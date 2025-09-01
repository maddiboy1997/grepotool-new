/* window.cpp - widgets

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

#include "window.h"
#include "defines.h"
#include "setting.h"
#include "command_ids.h"
#include "command.h"
#include <QSettings>

Window::Window (const QString& id):_id(id),_is_init(false)
{
}
Window::~Window ()
{
	// storeWindowState ();
}

void Window::restoreWindowState ()
{
	if (!_is_init) initWidget ();
	_is_init = true;
	QSettings s (appTitle(), appCfgKey());	
	restoreState(s.value(stateKey(), saveState()).toByteArray());
	worldChanged ();
}

void Window::storeWindowState ()
{
	QSettings s (appTitle(), appCfgKey());	
	s.setValue(globalStateKey(), saveState());
	s.setValue(worldStateKey(), saveState());
}
void Window::restoreWindowGeometry ()
{
	QSettings s (appTitle(), appCfgKey());	
	restoreGeometry(s.value(geometryKey(), saveGeometry()).toByteArray());
}
void Window::storeWindowGeometry ()
{
	QSettings s (appTitle(), appCfgKey());	
	s.setValue(globalGeometryKey(), saveGeometry());
	s.setValue(worldGeometryKey(), saveGeometry());
}


// World/Mytowns
QString Window::widgetKey () const
{
	if (Setting::value(STORE_SETTINGS_PER_WORLD).toBool())
		return QString ("World/") + _id;
	return QString ("Global/") + _id;
}

QString Window::globalWidgetKey () const
{
	return QString ("Global/") + _id;
}

QString Window::worldWidgetKey () const
{

	return QString ("World/") + _id;
}

// World/Mytowns/WindowState
QString Window::stateKey () const
{
	return widgetKey () + "/WindowState";
}
QString Window::globalStateKey () const
{
	return globalWidgetKey () + "/WindowState";
}
QString Window::worldStateKey () const
{
	return worldWidgetKey () + "/WindowState";
}

// World/Mytowns/WindowGeometry
QString Window::geometryKey () const
{
	return widgetKey () + "/WindowGeometry";
}
QString Window::globalGeometryKey () const
{
	return globalWidgetKey () + "/WindowGeometry";
}
QString Window::worldGeometryKey () const
{
	return worldWidgetKey () + "/WindowGeometry";
}


CmdAct * Window::addCommand (QObject * par, const QString& key,
		const QString & ti,		
		const QString & tt,
		const QString & wt,
		const QIcon   & ic)
{
	CmdAct * cmd = new CmdAct (par,key,ti,tt,wt,ic);
	cmd->connectToId(this, SLOT(cmdTriggered(const QString&)));
	return cmd;
}

