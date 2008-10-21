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
#include "app/moose.h"
#include "lib/lastfm/core/CoreLocale.h"
#include "lib/lastfm/types/Track.h"
#include <QPainter>

#define AGING_MOOSE 0x646464


static inline QPixmap fitInSquare( const QPixmap& in, const int m )
{
    QPixmap out( m, m );
    
    if (in.width() < m || in.height() < m)
    {
        out.fill( QColor(AGING_MOOSE) );
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
    bool const isEnabled = option.state & QStyle::State_Enabled;
    
    painter->save();
    
    QColor bg;
    if(isSelected)
    {
        QPalette::ColorGroup cg = isActive ? QPalette::Active : QPalette::Inactive;
        bg = option.palette.color( cg, QPalette::Highlight );
        painter->fillRect( option.rect, bg );
    }
    else if (index.data( moose::CumulativeCountRole ).toInt() % 2 != index.row() % 2)
    {
        bg = option.palette.color( QPalette::AlternateBase );
        painter->fillRect( option.rect, bg );
    }
    else
        bg = option.palette.color( QPalette::Base );
    
    QVariant weighting = index.data( moose::WeightingRole );
    if (weighting.isValid())
    {
        QRect r = option.rect;
        r.setWidth( r.width() * weighting.toDouble() );
        QLinearGradient g( r.topRight() - QPoint(5,0), r.topRight() );
        g.setColorAt( 0, Qt::transparent );
        g.setColorAt( 1, bg.lighter() );
        painter->fillRect( r, g );
    }    
    
    QTransform t;
    QPoint p = option.rect.topLeft();
    t.translate( p.x(), p.y() );
    painter->setTransform( t );
    
    const int S = painter->fontMetrics().lineSpacing();

    // yes, I know the following two lines are horrendous, a pint if you can
    // make it simpler without refactoring to a function! :)
    QColor const primary = isEnabled
            ? (isActive ? Qt::white : (isSelected ? Qt::black : Qt::white))
            : QColor(AGING_MOOSE);    
    QColor const secondary = isSelected ? (isActive ? Qt::white : Qt::black) : QColor(AGING_MOOSE);

    QPixmap px = index.data( Qt::DecorationRole ).value<QPixmap>();
    const int n = option.rect.height() - 20;
    px = fitInSquare( px, n );
    painter->drawPixmap( 10, 10, px );
    painter->setPen( secondary );
    painter->setBrush( Qt::NoBrush );
    painter->drawRect( 10, 10, n, n );

    // keep text within the rects we drew
    QRect r( 5, 5, option.rect.width() - 10, option.rect.height() - 10 );
    //painter->setClipRect( r );

    painter->drawText( m_metric, 5 + S + 2 + S, index.data( moose::SecondaryDisplayRole ).toString() );

    QFont f = painter->font();
    f.setBold( true );
    painter->setFont( f );
    painter->setPen( primary );
    painter->drawText( m_metric, 5 + S, index.data().toString() );

    QString const small = index.data( moose::SmallDisplayRole ).toString();
    if (small.size())
    {
        int const h = painter->fontMetrics().height();
        int const w_name = painter->fontMetrics().width( index.data().toString() );

        f.setPointSize( 8 );
        f.setBold( false );
        painter->setFont( f );
        int const w = painter->fontMetrics().width( small );

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
        painter->drawText( r_text_bounds, Qt::AlignVCenter | Qt::AlignRight, small );
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
