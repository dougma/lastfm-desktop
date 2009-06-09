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
#include "UnicornSizeGrip.h"

UnicornSizeGrip::UnicornSizeGrip( QWidget* w, Qt::Alignment o ) 
                :QSizeGrip( w ), 
                 m_orientation( o )
{
    Q_ASSERT( o & Qt::AlignHorizontal_Mask );
    Q_ASSERT( o & Qt::AlignVertical_Mask );
    
    w->installEventFilter( this );
    setFixedSize( sizeHint() );
    if( m_orientation & Qt::AlignTop ) 
    {
        if( m_orientation & Qt::AlignRight )
            m_transformMatrix.rotate( 270 );
        else if( m_orientation & Qt::AlignLeft )
            m_transformMatrix.rotate( 180 );
    }
    else if( m_orientation & Qt::AlignBottom ) 
    {
        if( m_orientation & Qt::AlignRight )
            ;
        else if( m_orientation & Qt::AlignLeft )
            m_transformMatrix.rotate( 90 );
    }
    
    m_xAdjust1 = ((m_orientation & Qt::AlignRight) ? 1 : 0);
    m_yAdjust1 = ((m_orientation & Qt::AlignBottom) ? 1 : 0);
    
    m_xAdjust2 = ((m_orientation & Qt::AlignRight) ? 0 : -1);
    m_yAdjust2 = ((m_orientation & Qt::AlignBottom) ? 0 : -1);
}


void 
UnicornSizeGrip::paintEvent( QPaintEvent* e )
{
    QPainter p( this );
    
    p.setClipRect( e->rect() );
    p.setPen( 0x838383 );
    
    QRect r = rect().adjusted( 2, 2, -2, -2 );
    
    
    while( r.isValid() )
    {
        int x1 = ((m_orientation & Qt::AlignRight) ? r.left() : r.right());
        int x2 = ((m_orientation & Qt::AlignRight) ? r.right() : r.left());
        int y1 = ((m_orientation & Qt::AlignBottom ) ? r.bottom() : r.top());
        int y2 = ((m_orientation & Qt::AlignBottom ) ? r.top() : r.bottom());
        p.drawLine( x1, y1, x2, y2 );
        r.adjust( m_xAdjust1*4, m_yAdjust1*4, m_xAdjust2*4, m_yAdjust2*4 );
    }
}


bool 
UnicornSizeGrip::eventFilter( QObject* o, QEvent* e )
{
    if( o != parentWidget() )
        return false;
    
    
    if( e->type() != QEvent::Resize )
        return false;
    
    QResizeEvent* rEvent = static_cast<QResizeEvent*>( e );
    
    QPoint pos;
    
    if( m_orientation & Qt::AlignLeft )
        pos.setX( 0 );
    else
        pos.setX( rEvent->size().width() - width());
    
    if( m_orientation & Qt::AlignTop )
        pos.setY( 0 );
    else
        pos.setY( rEvent->size().height() - height());
    
    move( pos );
    
    return false;
}