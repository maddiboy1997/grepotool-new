/* table_model.cpp  - Table data model

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

TableModel::TableModel(QObject *parent) :  QAbstractTableModel(parent), _icon_size (20,20)
{
}

TableModel::~TableModel ()
{
}

int TableModel::rowCount ( const QModelIndex & ) const 
{ 
	return _idx.size(); 
}

void  TableModel::resetIndex (const QVector <int> & nidx, const QVariant& tabno)
{
	beginResetModel ();
	_idx   = nidx;
	_tab_no = tabno;
	endResetModel ();
}

QSize TableModel::iconSize () const
{
	return _icon_size;
}

void  TableModel::setIconSize (const QSize & s) 
{
	_icon_size = s; 
	triggerRedraw ();
}

bool TableModel::canHideColumn (int) const
{
	return true;
}

bool TableModel::canMoveColumn (int)  const
{
	return true;
}

bool TableModel::canSortByColumn   (int) const
{
	return true;
}

bool  TableModel::itemCheckable   (int ) const 
{
	return false;
}

bool TableModel::itemChecked  (int , int) const
{
	return false;
}

void TableModel::setItemChecked  (int,  int , bool)
{
}

Qt::AlignmentFlag TableModel::itemAlignment   (int) const
{
	return Qt::AlignLeft;
}

bool TableModel::lessThan (int , int , int ) const 
{
	return false;
}

int TableModel::columnCount(const QModelIndex &) const
{
	return numColumns ();
}


int TableModel::rowOfIndex (int idx) const
{
	for(int i = 0 ; i < _idx.size(); ++i) 

		if (_idx[i] == idx)
		
			return i;

	return -1;
}

int TableModel::indexOfRow (int row) const
{
	if ( row < 0 || row >= _idx.size() ) 
	
		return -1;

	return _idx[row];
}


QModelIndexList TableModel::rowsOfItems (const QVector<int>&idx)  const
{
	QModelIndexList r;
	for (int i=0;i< idx.size(); ++i) {
		int ro = rowOfIndex (idx[i]);
		if(ro < 0) continue;
		r.push_back (index (ro,0));
	}
	return r;
}



void  TableModel::storeSelection (const QModelIndexList & idx)
{
	_sel.resize (_idx.size());
	_sel.fill (false);
	for (int i=0;i<idx.size();++i) 
		_sel[idx[i].row()] = true;
}

QModelIndexList TableModel::restoreSelection () const
{
	QModelIndexList r;
	for (int i=0;i< _sel.size(); ++i) 
		if (_sel[i]) 
			r.push_back(index(i,0));
	return r;
}

void TableModel::setItemCheckState (const QModelIndex & index, Qt::CheckState state)
{
	if ( !index.isValid() ) 
		return;

	int idx = indexOfRow (index.row());
	setItemChecked (idx, index.column(), state == Qt::Checked);

	emit dataChanged(index,index);

}


QVariant TableModel::data ( const QModelIndex &index, int role ) const
{
	if ( !index.isValid() ) 
		return QVariant();

	int idx = indexOfRow (index.row());

	if (idx < 0 || idx >= maxIndex())  
		return QVariant();

	if ( role == Qt::DisplayRole) {

		return itemDisplayData (idx, index.column());

	} else if (role == Qt::ToolTipRole) {

		QVariant r = itemTooltip (idx, index.column());
		if(r.isValid()) return r;
		return headerTooltip ( index.column() ) ;

	} else if (role == Qt::TextAlignmentRole) {
		
		return itemAlignment (index.column () );

	} else if (role == Qt::CheckStateRole) {

		if (itemCheckable(index.column())) {

			return (itemChecked(idx, index.column())) ? Qt::Checked : Qt::Unchecked;
		}

	} else if (role == Qt::StatusTipRole) {

	} else if (role == Qt::WhatsThisRole) {

	} else if (role == Qt::DecorationRole) {
		
		return itemDecoration (idx, index.column());

	} else if (role == Qt::SizeHintRole) {

	}
	return QVariant ();
}

QVariant TableModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal) {

		if (role == Qt::DisplayRole) {

			return headerText (section) ;
		
		} else if (role == Qt::DecorationRole) {
		
			return headerIcon (section) ;

		} else if (role == Qt::ToolTipRole) {

			return  headerTooltip   (section) ;
		}
	} else {

		if (role == Qt::DisplayRole) 

			return QString::number (section+1);
	}

	return QVariant ();
}

void TableModel::sort ( int column, Qt::SortOrder order )
{
	emit layoutAboutToBeChanged ();

	if (_sel.size() != _idx.size()) 
		
		_sel.resize(_idx.size());

	for ( int i = 0 ; i <  ( _idx.size() - 1 ); ++i ) {

		for ( int j = i+1; j < _idx.size(); ++j ) {

			bool dochg = false;
						
			if (order == Qt::AscendingOrder) {
			
				dochg = lessThan (_idx[j], _idx[i], column);

			} else {

				dochg = lessThan (_idx[i], _idx[j], column);
			}
				
			if (dochg) {

				int dmy = _idx[i];
				_idx[i] = _idx[j];
				_idx[j] = dmy;

				bool b  = _sel[i];
				_sel[i] = _sel[j];
				_sel[j] = b;
			}
		
		}
	}
	emit layoutChanged ();
}

bool TableModel::customEditOfItems (const QModelIndexList & idx)
{
	if(!idx.size()) return false;
	QVector<int> ix;
	for (int i=0;i< idx.size();++i) 
		ix.push_back (indexOfRow(idx[i].row()));
	if (!itemCustomEdit(ix,idx[0].column())) return false;

	emit dataChanged (idx[0],idx[idx.size()-1]);

	return true;
}

bool TableModel::itemCustomEdit  (const QVector<int>&, int )
{
	return false;
}

void TableModel::fillContextMenu (QMenu* , const QVector<int>&) 
{
};

void TableModel::fillContextMenu (QMenu* m, const QModelIndexList & idx)
{
	QVector<int> ix;
	for (int i=0;i< idx.size();++i) 
		ix.push_back (indexOfRow(idx[i].row()));
	fillContextMenu (m, ix);
}
