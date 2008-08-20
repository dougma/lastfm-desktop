/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#include "StationDelegate.h"
#include <QPainter>


void 
StationDelegate::paint(QPainter* painter, 
					   const QStyleOptionViewItem& option, 
					   const QModelIndex& index) const
{
	painter->save();
	
	QRect textRect = option.rect;
	textRect.setX( textRect.x() + 10 );
	textRect.setY( textRect.y() + 10 );
	textRect.setWidth( textRect.width() - 35 );
	
	if( option.state & QStyle::State_Selected )
	{
		painter->fillRect( option.rect, QPalette().highlight() );
		painter->setPen( QPalette().highlightedText().color() );
	}
	
	painter->drawText( textRect, Qt::TextSingleLine, index.data().toString(), &textRect );
	
	
	//Draw the icon
	QRect pixmapRect = option.rect;
	pixmapRect.setX( pixmapRect.width() - 25 );
	pixmapRect.setY( pixmapRect.y() + ( pixmapRect.height() / 2 ) - 7 );
	pixmapRect.setWidth( 18 );
	pixmapRect.setHeight( 14 );

	painter->setRenderHint( QPainter::SmoothPixmapTransform );
	painter->drawPixmap( pixmapRect, QPixmap( ":/station.png" ) );
	

	//Draw seperating lines
	painter->setPen( QPalette().mid().color() );

	painter->drawLine( QPoint( option.rect.x(), option.rect.y()  ),
					  QPoint( option.rect.x() + option.rect.width(), option.rect.y() ) );
	
	painter->drawLine( QPoint( option.rect.x(), option.rect.y() + option.rect.height() ),
					  QPoint( option.rect.x() + option.rect.width(), option.rect.y() + option.rect.height() ) );
	

	painter->restore();
}


QSize 
StationDelegate::sizeHint( const QStyleOptionViewItem& option, 
						   const QModelIndex& index ) const
{
	return QSize( 1, 35 );
}