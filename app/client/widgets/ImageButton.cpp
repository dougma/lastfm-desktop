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

#include "ImageButton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QLayout>
#include <QAction>
#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QDebug>


ImageButton::ImageButton( QWidget* parent )
           : QAbstractButton( parent )
{
    resize( 0, 0 );
}


ImageButton::ImageButton( const QString& path, QWidget* parent )
            : QAbstractButton( parent )
{   
    resize( 0, 0 );
    setPixmap( path );
    
    QPixmap disabled( path.left( path.length() - 6 ) + "disabled.png" );
    if (!disabled.isNull())
    {
        setPixmap( disabled, QIcon::Disabled );
    }
}


void
ImageButton::setAction( QAction* action )
{
    connect( this, SIGNAL(clicked()), action, SLOT(trigger()) );
    connect( action, SIGNAL(changed()), SLOT( actionChanged()) );
    setEnabled( action->isEnabled() );
    setChecked( action->isChecked() );
}


void
ImageButton::paintEvent( QPaintEvent* event )
{
    QPainter p( this );
    
    QIcon::Mode mode = isEnabled() ? QIcon::Normal : QIcon::Disabled;
    if( isDown() )
        mode = QIcon::Active;
    
    QIcon::State state = isChecked() ? QIcon::On : QIcon::Off;
    
    p.setClipRect( event->rect() );
    
    m_backgroundIcon.paint( &p, rect(), Qt::AlignCenter, mode, state );
    
    QRect iconRect = rect();
    if( m_iconOffsets.contains( mode ) )
    {
        iconRect.setLeft( iconRect.left() + m_iconOffsets[ mode ].x() );
        iconRect.setBottom( iconRect.bottom() + m_iconOffsets[ mode ].y() - 3 );
    }
    
    icon().paint( &p, iconRect, Qt::AlignCenter, mode, state );
    
}


QSize
ImageButton::sizeHint() const
{
    return size();
}


void
ImageButton::setPixmap( const QPixmap& p, const QIcon::State st )
{
    
    resize( p.size().expandedTo( size()) );
    
    setIconSize( p.size().expandedTo( iconSize() ) );
    
    QIcon i = icon();
    i.addPixmap( p, QIcon::Normal, st );
    setIcon( i );
}


void 
ImageButton::setPixmap( const QPixmap& p, const QIcon::Mode m )
{
    
    resize( p.size().expandedTo( size()) );
    
    setIconSize( p.size().expandedTo( iconSize() ) );
    
    QIcon i = icon();
    i.addPixmap( p, m );
    setIcon( i );
}


void 
ImageButton::setPixmap( const QPixmap& p, const QIcon::State s, const QIcon::Mode m )
{
    
    resize( p.size().expandedTo( size()) );
    
    setIconSize( p.size().expandedTo( iconSize() ) );
    
    QIcon i = icon();
    i.addPixmap( p, m, s );
    setIcon( i );
}


void
ImageButton::setBackgroundPixmap( const QString& path, const QIcon::Mode m )
{
    QPixmap p( path );
    
    resize( p.size().expandedTo( size()) );
    
    setIconSize( p.size().expandedTo( iconSize() ) );
    m_backgroundIcon.addPixmap( p, m );
}


void
ImageButton::moveIcon( int x, int y, QIcon::Mode m )
{
    m_iconOffsets.insert( m, QPoint( x, y ) );
}


void
ImageButton::actionChanged()
{
    QAction* action = static_cast<QAction*>(sender());
    setEnabled( action->isEnabled());
    setChecked( action->isChecked());
}
