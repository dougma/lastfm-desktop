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
#include "ImageButton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QLayout>
#include <QAction>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QDebug>


ImageButton::ImageButton( const QPixmap& rest, QWidget* parent )
           : ActionButton( parent )
{
    init( rest );
}


ImageButton::ImageButton( const QString& path, QWidget* parent )
           : ActionButton( parent )
{
    init( QPixmap( path ) );
}


void
ImageButton::init( const QPixmap& p )
{
    setPixmap( p );
    m_sizeHint = p.size();
}


void
ImageButton::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    p.setClipRect( event->rect() );
    
    QIcon::Mode mode = isDown()
        ? QIcon::Active
        : isEnabled() 
            ? QIcon::Normal 
            : QIcon::Disabled;
    
    QIcon::State state = isChecked()
        ? QIcon::On 
        : QIcon::Off;
    
    QRect iconRect = rect();
    if( m_iconOffsets.contains( mode ) )
    {
        iconRect.setLeft( iconRect.left() + m_iconOffsets[ mode ].x() );
        iconRect.setBottom( iconRect.bottom() + m_iconOffsets[ mode ].y() - 3 );
    }
    
    icon().paint( &p, iconRect, Qt::AlignCenter, mode, state );
}


void
ImageButton::setPixmap( const QPixmap& p, const QIcon::State state, const QIcon::Mode mode )
{      
    QIcon i = icon();
    i.addPixmap( p, mode, state );
    setIcon( i );
}


void
ImageButton::moveIcon( int x, int y, QIcon::Mode m )
{
    m_iconOffsets.insert( m, QPoint( x, y ) );
}
