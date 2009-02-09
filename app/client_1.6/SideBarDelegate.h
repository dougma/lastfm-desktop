/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef SIDE_BAR_DELEGATE_H
#define SIDE_BAR_DELEGATE_H

#include <QItemDelegate>
#include <QFont>


class SideBarDelegate : public QItemDelegate
{
	Q_OBJECT

	virtual void paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;

public:
	QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const;
	
	QString truncateString( QString str, QFont font, int sideBarWidth ) const;
	
	QFont font() { return m_font; };
	void setFont( QFont font ) { m_font = font; };
	int sideBarWidth() { return m_sideBarWidth; };
	void setSideBarWidth( int width ) { m_sideBarWidth = width; };

private:
	QFont m_font;
	int m_sideBarWidth;
};

#endif
