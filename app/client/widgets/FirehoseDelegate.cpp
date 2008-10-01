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

#include "FirehoseDelegate.h"
#include "lib/types/Track.h"
#include <QPainter>


static inline QPixmap fitInSquare( const QPixmap& in, const int m )
{
    QPixmap out( m, m );
    
    if (in.width() < m || in.height() < m)
    {
        out.fill( QColor( 100, 100, 100 ) );
        QPainter( &out ).drawPixmap( out.rect().center() - in.rect().center(), in );
    }
    else 
        out = in.scaled( m, m, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
    
    return out;
}


void
FirehoseDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    bool const isSelected = option.state & QStyle::State_Selected;
    bool const isActive = option.state & QStyle::State_Active;
    
    painter->save();
    
    if (isSelected)
    {
        QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
        ? QPalette::Normal
        : QPalette::Disabled;
        
        if (cg == QPalette::Normal && !isActive)
            cg = QPalette::Inactive;
        
        painter->fillRect( option.rect, option.palette.brush(cg, QPalette::Highlight));
    } 
    else if (index.row() % 2 != index.model()->rowCount() % 2)
    {
        painter->fillRect(option.rect, option.palette.brush( QPalette::AlternateBase ) );
    }
    
    QTransform t;
    QPoint p = option.rect.topLeft();
    t.translate( p.x(), p.y() );
    painter->setTransform( t );
    
    const int S = painter->fontMetrics().lineSpacing();
    
    QPixmap px = index.data( Qt::DecorationRole ).value<QPixmap>();
    const int n = option.rect.height() - 20;
    px = fitInSquare( px, n );
    painter->drawPixmap( 10, 10, px );
    painter->setPen( QColor( 100, 100, 100 ) );
    painter->setBrush( Qt::NoBrush );
    painter->drawRect( 10, 10, n, n );
    
    // keep text within the rects we drew
    painter->setClipRect( 5, 5, option.rect.width() - 10, option.rect.height() - 10 );
    
    QColor primary = isSelected ? (isActive ? Qt::white : Qt::darkGray) : Qt::white;
    QColor secondary = isSelected && isActive ? Qt::white : Qt::darkGray; 
    
    painter->setPen( secondary );
    painter->drawText( metric() , 5 + S + 2 + S, index.data( TrackRole ).value<Track>().toString() );
    
    QFont f = painter->font();
    f.setBold( true );
    painter->setFont( f );
    painter->setPen( primary );
    painter->drawText( metric() , 5 + S, index.data().toString() );
    
    painter->restore();
}


QSize
FirehoseDelegate::sizeHint( const QStyleOptionViewItem& o, const QModelIndex& ) const
{
    if (metric() == 0) metric() = 1 /*top margin*/ + QFontMetrics( o.font ).lineSpacing() * 3 + 2 /*text spacing*/ + 4 /*bottom margin*/;
    return QSize( metric() + 150, metric() );
}
