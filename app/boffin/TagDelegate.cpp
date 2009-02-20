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

#include "TagDelegate.h"
#include "TagCloudModel.h"
#include <QPainter>


static inline QFont font( QFont f, float const weight )
{
    static const float k_factor = 16;

    f.setPointSize( f.pointSize() + (k_factor * weight ));
    f.setWeight( 99 * weight );
    return f;
}


static inline QSize margins( float const weight )
{
    return QSize( 17 + (11*weight), 20 - (10*weight) );
}


TagDelegate::TagDelegate( QObject* parent ) 
            : QAbstractItemDelegate( parent )
{}


void 
TagDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QPen p( Qt::NoPen );
    QBrush b( Qt::NoBrush );
    QColor const dark = option.palette.color( QPalette::Highlight ).darker();    
    
    if( option.state & (QStyle::State_Selected | QStyle::State_Active) )
    {
        b = option.state & QStyle::State_Enabled
                ? option.palette.highlight()
                : QColor(0x646464);
    }
    if( option.state & QStyle::State_MouseOver )
        p = option.palette.color( QPalette::Highlight );

    if( option.state & QStyle::State_Active )
        p = dark;

    p.setWidth( 3 );
    painter->setPen( p );
    painter->setBrush( b );
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->drawRoundedRect( option.rect.adjusted( 2, 2, -2, -2 ), 5.0f, 5.0f );

    const float weight = index.data( TagCloudModel::LinearWeightRole ).value<float>();
    painter->setFont( font( option.font, weight ) );

    painter->setRenderHint( QPainter::Antialiasing, false );
    painter->setPen( option.state & (QStyle::State_Selected|QStyle::State_Active)
            ? option.palette.color( QPalette::HighlightedText )
            : option.palette.color( QPalette::Text ) );

    QString const text = index.data().toString();
    QFontMetrics const metrics = painter->fontMetrics();

    QPoint pt;
    pt.setX( option.rect.x() + (option.rect.width() - metrics.width( text ))/2 );
    pt.setY( option.rect.y() + margins( weight ).height()/2 + metrics.ascent() );
    painter->drawText( pt, text );
}


extern int gBaseline;
extern int gLeftMargin;
QSize 
TagDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    const float weight = index.data( TagCloudModel::LinearWeightRole ).value<float>();
    QFontMetrics fm( font( option.font, weight ) );
    
    const QSize margin = margins( weight );
    gBaseline = margin.height()/2 + fm.ascent();
    gLeftMargin = margin.width()/2;
    return fm.size( Qt::TextSingleLine, index.data().toString() ) + margin;
}
