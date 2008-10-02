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
#include "FirehoseModel.h" //for our custom ModelRoles
#include "lib/core/CoreLocale.h"
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
    
    QColor bg;
    if (index.data( CumulativeCountRole ).toInt() % 2 != index.row() % 2)
    {
        bg = option.palette.color( QPalette::AlternateBase );
        painter->fillRect( option.rect, bg );
    }
    else
        bg = option.palette.color( QPalette::Base );

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
    QRect r( 5, 5, option.rect.width() - 10, option.rect.height() - 10 );
    //painter->setClipRect( r );
    
    QColor primary = isSelected ? (isActive ? Qt::white : Qt::darkGray) : Qt::white;
    QColor secondary = isSelected && isActive ? Qt::white : Qt::darkGray; 
    
    painter->setPen( secondary );
    painter->drawText( m_metric, 5 + S + 2 + S, index.data( TrackRole ).value<Track>().toString() );
    
    QFont f = painter->font();
    f.setBold( true );
    painter->setFont( f );
    painter->setPen( primary );
    painter->drawText( m_metric, 5 + S, index.data().toString() );
    
    QDateTime timestamp = index.data( TimestampRole ).toDateTime();
    if (timestamp.isValid())
    {
        int const h = painter->fontMetrics().height();
        int const w_name = painter->fontMetrics().width( index.data().toString() );
        
        QString const s = timestamp.toString( CoreLocale::system().qlocale().timeFormat( QLocale::ShortFormat ) );        

        f.setPointSize( 8 );
        f.setBold( false );
        painter->setFont( f );
        int const w = painter->fontMetrics().width( s );
        
        QRect const r_text_bounds( option.rect.width() - 5 - w, 10, w, h );
        QRect const r_text = r_text_bounds.adjusted( -5, 0, 0, 0 );
        QRect r_g = r_text;
        r_g.translate( -10, 0 );
        r_g.setWidth( 10 );
        
        if (m_metric + w_name > r_text.left())
        {
            QLinearGradient g( r_g.topLeft(), r_g.topRight() );
            g.setColorAt( 0, Qt::transparent );
            g.setColorAt( 1, bg );
            
            painter->fillRect( r_text, bg );
            painter->fillRect( r_g, g );

        }

        painter->setPen( secondary );        
        painter->drawText( r_text_bounds, Qt::AlignVCenter | Qt::AlignRight, s );
    }
    
    QRect r3( option.rect.right() - 14, 5 + S + 2, 15, 2*S );
    QLinearGradient g( r3.topLeft(), r3.topRight() );
    g.setColorAt( 0, Qt::transparent );
    g.setColorAt( 0.9, bg );
    painter->fillRect( r3, g );
    
    painter->restore();
}


QSize
FirehoseDelegate::sizeHint( const QStyleOptionViewItem& o, const QModelIndex& ) const
{
    if (m_metric == 0) m_metric = 1 /*top margin*/ + QFontMetrics( o.font ).lineSpacing() * 3 + 2 /*text spacing*/ + 4 /*bottom margin*/;
    return QSize( m_metric + 150, m_metric );
}
