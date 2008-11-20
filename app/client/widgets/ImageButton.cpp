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


ImageButton::ImageButton( const QPixmap& rest, QWidget* parent )
           : QAbstractButton( parent )
{
    init( rest );
}


ImageButton::ImageButton( const QString& path, QWidget* parent )
           : QAbstractButton( parent )
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
ImageButton::setAction( QAction* action )
{
    const bool b = action->isCheckable();
    setCheckable( b );
    
    // only do one or the other or you trigger it all twice
    if (b)
        connect( this, SIGNAL(toggled( bool )), action, SLOT(setChecked( bool )) );
    else
        connect( this, SIGNAL(clicked()), action, SLOT(trigger()) );

    connect( action, SIGNAL(changed()), SLOT(onActionChanged()) );
    onActionChanged( action );
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


void
ImageButton::onActionChanged( QAction* action )
{
    if (!action) action = (QAction*) sender();
    setEnabled( action->isEnabled());
    setChecked( action->isChecked());
}
