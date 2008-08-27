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
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QFontMetrics>

static const int kLeftMargin = 10;
static const int kIconWidth = 25;

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
		
	}
	
	//Draw tag text
	QRect textRect = option.rect;
	textRect.setX( textRect.x() + kLeftMargin );
	textRect.setY( textRect.y() + 10 );
	textRect.setWidth( textRect.width() - kLeftMargin - kIconWidth );
	QString text = option.fontMetrics.elidedText( index.data().toString(), Qt::ElideRight, textRect.width() );
	painter->drawText( textRect, Qt::TextSingleLine, index.data().toString(), &textRect );
	
	
	//Draw the icon
	QRect pixmapRect = option.rect;
	pixmapRect.setX( pixmapRect.width() - kIconWidth );
	pixmapRect.setY( pixmapRect.y() + ( pixmapRect.height() / 2 ) - 7 );
	pixmapRect.setWidth( 18 );
	pixmapRect.setHeight( 14 );

	painter->setRenderHint( QPainter::SmoothPixmapTransform );
	painter->drawPixmap( pixmapRect, QPixmap( ":/station.png" ) );
	

	//Draw seperating lines
	painter->setPen( QPalette().mid().color() );

	int lineL = option.rect.x();
	int lineR = lineL + option.rect.width();
	
	int topY = option.rect.y();
	int bottomY = topY + option.rect.height();
	
	painter->drawLine( QPoint( lineL, topY ),
					   QPoint( lineR, topY ) );
	
	painter->drawLine( QPoint( lineL, bottomY ),
					   QPoint( lineR, bottomY ) );
	

	painter->restore();
}


QSize 
StationDelegate::sizeHint( const QStyleOptionViewItem& option, 
						   const QModelIndex& index ) const
{
	QSize size = option.fontMetrics.size( Qt::TextSingleLine, index.data().toString() );

	size.setWidth( size.width() + kLeftMargin + kIconWidth );
	
	size = size.expandedTo( QSize( 1, 35 ) ).expandedTo( QApplication::globalStrut() );
	
	return size;
}