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

TagDelegate::TagDelegate( QObject* parent ) 
            : QAbstractItemDelegate( parent )
{

}


void 
TagDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QFont f = option.font;
    f.setPointSize( f.pointSize() * ( index.data( TagCloudModel::WeightRole ).value<float>() +0.5) );
    {
        painter->save();
        painter->setRenderHint( QPainter::Antialiasing );
        QPen p = ((option.state & QStyle::State_MouseOver) ? QColor( 200, 200, 200 ): QColor( 255,255,255));
        p.setWidth( 3 );
        painter->setPen( p );
        painter->drawRoundedRect( option.rect.adjusted( 4, 4, -4, -4 ), 10.0f,160.0f );
        painter->restore();
    } 

    painter->setFont( f );
    painter->drawText( option.rect, Qt::AlignCenter, index.data().toString());
}


QSize 
TagDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QFont f = option.font;
    f.setPointSize( f.pointSize() * ( index.data( TagCloudModel::WeightRole ).value<float>() + 0.6 ));
    QFontMetrics fm( f );
    return fm.size( Qt::TextSingleLine, index.data().toString() );
}

