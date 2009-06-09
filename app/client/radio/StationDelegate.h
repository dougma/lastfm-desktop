/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QItemDelegate>

class StationDelegate : public QItemDelegate
{
Q_OBJECT
	
public:
	StationDelegate( QObject* parent = 0 ):QItemDelegate( parent ), m_maxCount( 0 ){};
	
	void paint( QPainter* painter, 
				const QStyleOptionViewItem& option, 
			    const QModelIndex& index ) const;
	
	QSize sizeHint( const QStyleOptionViewItem& option, 
				    const QModelIndex& index ) const;
	
	void setMaxCount( float count ){ m_maxCount = count; }
	
	enum IndexDataRole{ CountRole = Qt::UserRole };
	
private:
	float m_maxCount;
};