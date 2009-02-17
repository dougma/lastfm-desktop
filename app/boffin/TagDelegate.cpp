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

static const float k_factor = 16;
static const float k_margin = 7;

TagDelegate::TagDelegate( QObject* parent ) 
            : QAbstractItemDelegate( parent )
{

}


void 
TagDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{

    painter->save();
    painter->setRenderHint( QPainter::Antialiasing );
    QPen p = ((option.state & QStyle::State_MouseOver) ? QColor( 100, 100, 100 ): Qt::transparent);
    if( option.state & QStyle::State_Selected )
    {
        painter->setBrush( option.palette.highlight() );
        painter->setPen( option.palette.highlightedText().color() );
    }
    else
    {
        painter->setBrush( Qt::transparent );
    }
  
  
    p.setWidth( 3 );
    painter->setPen( p );

    painter->drawRoundedRect( option.rect.adjusted( 4, 4, -4, -4 ), 10.0f, 10.0f );
    painter->restore();

    QFont f = option.font;
    
    const float weight = index.data( TagCloudModel::LinearWeightRole ).value<float>();

    f.setPointSize( f.pointSize() + (k_factor * weight ));
    f.setWeight( 99 * weight );
    
    painter->setFont( f );
    QFontMetrics fm( f );
    painter->drawText( option.rect.translated(  0, -(fm.descent() / 2) - ( 4 * 1.5)), Qt::AlignHCenter | Qt::AlignBottom , index.data().toString());
}


QSize 
TagDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QFont f = option.font;
    
    const float weight = index.data( TagCloudModel::LinearWeightRole ).value<float>();

    f.setPointSize( f.pointSize() + (k_factor * weight));
    f.setWeight( 99 * weight );
    QFontMetrics fm( f );
    const QSize fmSize = fm.size( Qt::TextSingleLine, index.data().toString() ) + QSize( 2* k_margin, 2* k_margin );
    return fmSize; 
}

