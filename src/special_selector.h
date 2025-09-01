#ifndef __SPECIAL_SELECTOR_H
#define __SPECIAL_SELECTOR_H
/* special_selector.h  - Town special info selector

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

#include "indexes.h"
#include "popup.h"
#include "ui_specials.h"


class SpecialSelector : public Popup {
	Q_OBJECT;
public:
	SpecialSelector (QWidget * parent, int special);
	int value() const {return spc;};

	static int selectSpecials (const QPoint & pos, int spc);

	QWidget * createWidget ();

public slots:
	void specialClicked (int);

private:
	QVector <QToolButton*> ss;
	QButtonGroup * bg;
	int spc;
	Ui::Specials ui;
};




#endif