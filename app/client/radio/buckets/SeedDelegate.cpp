/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "SeedDelegate.h"
#include "Seed.h"

SeedDelegate::SeedDelegate( QObject* parent )
             :QAbstractItemDelegate( parent )
{ 
    m_selectedOverlay = QPixmap( ":buckets/avatar_overlay_selected.png" ); 
    m_overlay = QPixmap( ":buckets/avatar_overlay.png" ); 
}


void 
SeedDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    painter->save();
    
    const QIcon icon = index.data( Qt::DecorationRole ).value<QIcon>();
    
    int flashValue = index.data( moose::HighlightRole ).isValid() ? index.data( moose::HighlightRole ).toInt() : 0;

    QRect iconRect = option.rect;
    iconRect.setSize( icon.actualSize( iconRect.size()));
    iconRect.translate( -(iconRect.width() - option.rect.width())/2, 0);
    
    QRect overlayRect = iconRect.adjusted( 1, 1, -1, -3 );
    
    QRect textRect = option.rect.adjusted( 0, iconRect.height(), 0, 0 );
    
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    
    icon.paint( painter, iconRect);
    
    if( option.state & QStyle::State_Selected &&
        index.data( moose::TypeRole ) != Seed::TagType)
    {
        painter->drawPixmap( overlayRect, m_selectedOverlay );
    }
    
    if( flashValue > 0 )
        painter->fillRect( option.rect, QColor( 0x0, 0x78, 0xff, ( 0xff * ((qreal)flashValue / 100.0f) )));
    
    painter->setPen( Qt::white );
    QFont f = painter->font();
    f.setPointSize( 10 );
    painter->setFont( f );
    
    
    QFontMetrics fm( f);
    QString text = fm.elidedText( index.data().toString(), Qt::ElideRight, textRect.width());
    textRect = fm.boundingRect(textRect, Qt::AlignCenter, text );
    
    if( option.state & QStyle::State_Selected )
        painter->fillRect( textRect, QColor( 0x056e99 ) );
    
    painter->setRenderHint( QPainter::Antialiasing );
    painter->drawText( textRect, Qt::AlignCenter, text );
    
    painter->restore();
}


QSize 
SeedDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    Q_UNUSED( index );
    return option.rect.size();
}