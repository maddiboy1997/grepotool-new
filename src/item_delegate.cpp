/* towns_model.cpp  - Town data model

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
#include "table_model.h"


#include <QApplication>

ItemDelegate::ItemDelegate(QObject *parent ): QAbstractItemDelegate (parent)
{
}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,	const QModelIndex &index) const
{
	const TableModel * tm = dynamic_cast <const TableModel *> (index.model());

	if (!tm) return ;

	QRect chkr; // checkbox rect
	QRect ctr = option.rect;  // content rect

	painter->save();
	painter->setPen(Qt::NoPen);

	QVariant data    = index.model()->data(index, Qt::DisplayRole);
	QVariant chk     =  index.model()->data(index, Qt::CheckStateRole);
	QPixmap dec      =  index.model()->data(index, Qt::DecorationRole).value<QPixmap>();
	Qt::Alignment al = (Qt::AlignmentFlag) index.model()->data(index, Qt::TextAlignmentRole).toInt() | Qt::AlignVCenter;

	if(data.type() == QVariant::Color) {

		painter->fillRect(option.rect, data.value<QColor>());

	} else if (option.state & QStyle::State_Selected) {

		painter->fillRect(option.rect, option.palette.highlight());
		painter->setPen(option.palette.highlightedText().color());

	} else {
		painter->setPen(option.palette.text().color());
	}

	if (chk.isValid()) {	

		// get checkbox rect
		QStyleOptionButton optt;
        optt.QStyleOption::operator=(option);				
		chkr = QApplication::style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &optt, 0);

		// checkstate
		QStyleOptionViewItem opt(option);
		opt.rect = chkr;
		opt.state = opt.state & ~QStyle::State_HasFocus;		
		Qt::CheckState state = (Qt::CheckState) chk.toInt();
		switch (state) {
			case Qt::Unchecked:
				opt.state |= QStyle::State_Off;
				break;
			case Qt::PartiallyChecked:
		        opt.state |= QStyle::State_NoChange;
			    break;
			case Qt::Checked:
				opt.state |= QStyle::State_On;
				break;
		}
		QApplication::style()->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt, painter, 0);

		// resize the contents rect for the content
		ctr.setX (ctr.x() + chkr.width ());
	}

	if (!dec.isNull()) {
		int px = ctr.left();
		int py = ctr.top() + (ctr.height() - dec.height())/2;
		painter->drawPixmap (px, py, dec);
		ctr.setX (ctr.x() + dec.width ());
	}

	if (data.type() == QVariant::String) {
		
		QTextOption topt (al);
		topt.setWrapMode(QTextOption::NoWrap);		
		painter->drawText (ctr, QFontMetrics (option.font).elidedText (data.toString(), Qt::ElideRight, ctr.width()), topt);

	} else if(data.type() == QVariant::Pixmap) {

		QPixmap pm =  data.value<QPixmap> ();
		painter->setClipping (true);
		painter->setClipRect (ctr);
		int px = ctr.left() + (ctr.width() - pm.width())/2;
		int py = ctr.top() + (ctr.height() - pm.height())/2;
		painter->drawPixmap (px, py, pm);
	}
	painter->restore ();
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	const TableModel * tm = dynamic_cast <const TableModel *> (index.model());

	if (!tm) return QSize();
		
	QVariant data = index.model()->data(index, Qt::DisplayRole);
	QVariant chk  = index.model()->data(index, Qt::CheckStateRole);
	QPixmap dec   = index.model()->data(index, Qt::DecorationRole).value<QPixmap>();
	int w = 0;
	int h = tm->iconSize().height();

	if (chk.isValid()) {	

		QStyleOptionButton optt;

		optt.QStyleOption::operator=(option);				

		w += QApplication::style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &optt, 0).width();
	}

	if (!dec.isNull()) w += dec.width();

	if (data.type() == QVariant::String) {

		w  += QFontMetrics (option.font).width( data.toString() );

	} else if(data.type() == QVariant::Pixmap) {

		w += data.value<QPixmap> ().width();

	} else if(data.type() == QVariant::Color) {

		w += tm->iconSize().width();
	}
	return QSize(w,h);
}

// Handle Mouse events on the item
bool ItemDelegate::editorEvent ( QEvent * ev, QAbstractItemModel * model, 
		const QStyleOptionViewItem & option, const QModelIndex & index )
{
	TableModel * tm = dynamic_cast <TableModel *> (model);

	if (!tm) return false;
		
	QVariant chk  = tm->data(index, Qt::CheckStateRole);

	if (!chk.isValid()) return false;
	
    if ((ev->type() == QEvent::MouseButtonRelease)
		|| (ev->type() == QEvent::MouseButtonDblClick)) {
			
			QStyleOptionButton optt;
			optt.QStyleOption::operator=(option);				
			QRect br =  QApplication::style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &optt, 0);

			QMouseEvent *me = static_cast<QMouseEvent*>(ev);
			if (me->button() != Qt::LeftButton || !br.contains(me->pos()))
				return false;

			// eat the double click events inside the check rect
			if (ev->type() == QEvent::MouseButtonDblClick)
				return true;

			Qt::CheckState state = (Qt::CheckState) chk.toInt();
			
			state = (state == Qt::Checked) ? Qt::Unchecked : Qt::Checked;  
			
			tm->setItemCheckState (index, state);
	}
	return false;
}

