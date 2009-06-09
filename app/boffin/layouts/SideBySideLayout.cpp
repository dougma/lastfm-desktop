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
#include "SideBySideLayout.h"
#include <QWidget>
#include <QLayoutItem>
#include <QTimeLine>

SideBySideLayout::SideBySideLayout( QWidget* parent )
           : QLayout( parent ), m_currentItem( 0 ), m_timeLine( new QTimeLine( 1500, this ) )
{
    m_timeLine->setUpdateInterval( 25 );
    connect( m_timeLine, SIGNAL( frameChanged( int )), SLOT( onFrameChanged( int )));
    connect( m_timeLine, SIGNAL( finished() ), SIGNAL( animationFinished() ));
}


SideBySideLayout::~SideBySideLayout()
{
    while( QLayoutItem* i = takeAt( 0 ) )
        delete i;
}


void 
SideBySideLayout::addItem(QLayoutItem *item)
{
    m_itemList.push_back( item );
    if( !m_currentItem )
        m_currentItem = item;
}


Qt::Orientations 
SideBySideLayout::expandingDirections() const
{
    return (Qt::Horizontal | Qt::Vertical);
}


bool 
SideBySideLayout::hasHeightForWidth() const
{
    return false;
}


int
SideBySideLayout::count() const
{
    return m_itemList.count();
}


QLayoutItem* 
SideBySideLayout::itemAt( int index ) const
{
    if( index >= m_itemList.count() || index < 0 || m_itemList.isEmpty() )
        return 0;
    
    return m_itemList.at( index );
}


QSize 
SideBySideLayout::minimumSize() const
{
    QSize minSize;
    
    foreach( QLayoutItem* i, m_itemList )
        minSize = minSize.expandedTo( i->minimumSize() );
    
    return minSize;
}


void 
SideBySideLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry( rect );
    if( !m_currentItem || m_timeLine->state() == QTimeLine::Running )
        return;
    
    doLayout( rect );
}


void 
SideBySideLayout::doLayout( const QRect& rect, int hOffset )
{
    m_currentItem->setGeometry( rect );
    
    foreach( QLayoutItem* i, m_itemList )
    {
        const int hUnits = m_itemList.indexOf( i ) - m_itemList.indexOf( m_currentItem );
        i->setGeometry( rect.translated( ( rect.width() * hUnits ) + hOffset, 0 ));
    }
}


QSize 
SideBySideLayout::sizeHint() const
{
    QSize sh;
    foreach( QLayoutItem* i, m_itemList )
        sh = sh.expandedTo( i->sizeHint() );
    
    return sh;
}


QLayoutItem* 
SideBySideLayout::takeAt( int index )
{
    if( index >= m_itemList.count() || index < 0 || m_itemList.isEmpty() )
        return 0;
    
    return m_itemList.takeAt( index );
}


void
SideBySideLayout::moveForward()
{
    int nextIndex = m_itemList.indexOf( m_currentItem ) + 1;
    if( nextIndex >= m_itemList.count() )
        return;

    m_currentItem = m_itemList.at( nextIndex );
    
    if( m_timeLine->state() == QTimeLine::Running && 
        m_timeLine->direction() == QTimeLine::Backward )
        m_timeLine->setDirection( QTimeLine::Forward );
    else
    {
        m_timeLine->setDirection( QTimeLine::Forward );
        m_timeLine->setStartFrame( geometry().width() );
        m_timeLine->setEndFrame( 0 );
        m_timeLine->start();
    }
 
}


void 
SideBySideLayout::moveBackward()
{
    int nextIndex = m_itemList.indexOf( m_currentItem ) - 1;
    if( nextIndex < 0 )
        return;
    
    m_currentItem = m_itemList.at( nextIndex );

    if( m_timeLine->state() == QTimeLine::Running && 
       m_timeLine->direction() == QTimeLine::Forward )
        m_timeLine->setDirection( QTimeLine::Backward );
    else
    {
        m_timeLine->setDirection( QTimeLine::Backward );
        m_timeLine->setStartFrame( 0 );
        m_timeLine->setEndFrame( -geometry().width() );
        m_timeLine->start();
    }
}


void 
SideBySideLayout::onFrameChanged( int frame )
{
    doLayout( geometry(), frame );
}


QWidget* 
SideBySideLayout::currentWidget()
{
    return m_currentItem->widget();
}

int
SideBySideLayout::currentItemIndex()
{
    return m_itemList.indexOf( m_currentItem );
}