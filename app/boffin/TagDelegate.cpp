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
#include <math.h>

TagDelegate::TagDelegate( QObject* parent ) 
            : QAbstractItemDelegate( parent )
{

}

#include <QDebug>
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
    f.setPointSize( 10 * pow( f.pointSize(), 0.4 * ( index.data( TagCloudModel::WeightRole ).value<float>() + 1 ) ));
    f.setWeight( 99 * index.data( TagCloudModel::WeightRole ).value<float>());
    painter->setFont( f );
    QFontMetrics fm( f );
    painter->drawText( option.rect, Qt::AlignHCenter | Qt::AlignBottom , index.data().toString());
}


QSize 
TagDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QFont f = option.font;
    f.setPointSize( 10 * pow( f.pointSize(), 0.4 * ( index.data( TagCloudModel::WeightRole ).value<float>() + 1 )));
    f.setWeight( 99 * index.data( TagCloudModel::WeightRole ).value<float>());
    QFontMetrics fm( f );
    return fm.size( Qt::TextSingleLine, index.data().toString() + "  " );
}

