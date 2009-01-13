/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

static const int k_leftMargin = 10;
static const int k_stationIconWidth = 25;
static const int k_minHeight = 35;

void 
StationDelegate::paint(QPainter* painter, 
					   const QStyleOptionViewItem& option, 
					   const QModelIndex& index) const
{
	painter->save();
	painter->eraseRect( option.rect );

	QPalette palette;
	bool itemSelected = option.state & QStyle::State_Selected;
	
	if( itemSelected )
	{
		painter->fillRect( option.rect, palette.highlight() );
		painter->setPen( palette.highlightedText().color() );
	}
	
	//Draw popularity of tag / friend match etc bar graph
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
	
	//Draw Decoration ( Avatar, Artist Image etc.. )
	QRect iconRect;
	if( !index.data( Qt::DecorationRole ).isNull())
	{
		iconRect = option.rect;
		const QImage& icon = index.data( Qt::DecorationRole ).value< QImage >();
		iconRect.setWidth( icon.width() );
		iconRect.setHeight( icon.height() );
		
		if( iconRect.height() < k_minHeight )
		{
			iconRect.setY( (iconRect.y() + ( k_minHeight / 2 )) - ( iconRect.height() / 2 ) );
		}
		
		painter->drawImage( iconRect, icon );
	}
	
	//Draw tag text
	QRect textRect = option.rect;
	textRect.setX( textRect.x() + k_leftMargin + iconRect.width() );
	textRect.setY( textRect.y() + 10 );
	textRect.setWidth( textRect.width() - k_leftMargin - k_stationIconWidth - iconRect.width() );
	QString text = option.fontMetrics.elidedText( index.data().toString(), Qt::ElideRight, textRect.width() );
	painter->drawText( textRect, Qt::TextSingleLine, index.data().toString(), &textRect );
	
	
	//Draw the station icon
	QRect stationIconRect = option.rect;
	stationIconRect.setX( stationIconRect.width() - k_stationIconWidth );
	stationIconRect.setY( stationIconRect.y() + ( stationIconRect.height() / 2 ) - 7 );
	stationIconRect.setWidth( 18 );
	stationIconRect.setHeight( 14 );

	painter->setRenderHint( QPainter::SmoothPixmapTransform );
	painter->drawPixmap( stationIconRect, QPixmap( ":/station.png" ) );
	

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
	QRect iconRect;
	if( !index.data( Qt::DecorationRole ).isNull() )
	{
		const QImage& icon = index.data( Qt::DecorationRole ).value< QImage >();
		iconRect.setWidth( icon.width() );
		iconRect.setHeight( icon.height() );
	}
	
	QSize size = option.fontMetrics.size( Qt::TextSingleLine, index.data().toString() );
	
	size.setWidth( size.width() + k_leftMargin + k_stationIconWidth + iconRect.width() );

	size = size.expandedTo( QSize( 1, iconRect.height() ) );
	
	size = size.expandedTo( QSize( 1, k_minHeight ) ).expandedTo( QApplication::globalStrut() );
	
	return size;
}