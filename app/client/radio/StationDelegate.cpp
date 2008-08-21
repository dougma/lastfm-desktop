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
	QPalette palette;
	bool itemSelected = option.state & QStyle::State_Selected;
	
	if( itemSelected )
	{
		painter->fillRect( option.rect, palette.highlight() );
		painter->setPen( palette.highlightedText().color() );
	}

	//Draw popularity of tag bar graph
	if( m_maxCount > 0 )
	{
		float count = index.data( CountRole ).value<float>();
		QRect tagRect = option.rect;
		tagRect.setWidth( ( (float)tagRect.width() / (float)m_maxCount ) * count );
		tagRect.setY( tagRect.y() + 2 );
		tagRect.setHeight( tagRect.height() - 1 );

		if( !itemSelected )
			painter->fillRect( tagRect, QBrush( QColor( 210, 210, 210 ) ) );
		else
			painter->fillRect( tagRect, QBrush( QColor( 200, 200, 255 ) ) );
		
			qDebug() << index.data().toString() << " == (" << count << " / " << m_maxCount << ")";
	}
	
	//Draw tag text
	QRect textRect = option.rect;
	textRect.setX( textRect.x() + 10 );
	textRect.setY( textRect.y() + 10 );
	textRect.setWidth( textRect.width() - 35 );
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
StationDelegate::sizeHint( const QStyleOptionViewItem& /* option */, 
						   const QModelIndex& /* index */ ) const
{
	return QSize( 1, 35 );
}