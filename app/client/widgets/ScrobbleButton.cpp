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

#include "ScrobbleButton.h"
#include "ScrobbleButton/PausableTimer.h"
#include "StopWatch.h"
#include <QApplication>
#include <QPainter>
#include <QTimeLine>


ScrobbleButton::ScrobbleButton()
{
    setCheckable( true );
    setChecked( true );
    setFixedSize( 51, 37 );
    
    connect( this, SIGNAL(toggled( bool )), SLOT(onChecked( bool )) );
    connect( qApp, SIGNAL(trackSpooled( Track, StopWatch* )), SLOT(onTrackSpooled( Track, StopWatch* )) );
}


void
ScrobbleButton::paintEvent( QPaintEvent* )
{
    QPixmap p;
    
    if (!isChecked()) {
        p = QPixmap( ":/ScrobbleButton/off.png" );
    }
    else if (m_track.isNull()) { //no track playing
        p = QPixmap( ":/ScrobbleButton/on.png" );
    }
    else if (!m_track.isValid()) { // not a valid scrobble that is
        p = QPixmap( ":/ScrobbleButton/SADFACE.png" );
    }
    else
        p = m_pixmap;

    QPainter( this ).drawPixmap( rect(), p );
}


void
ScrobbleButton::onTrackSpooled( const Track& t, class StopWatch* watch )
{
    m_track = t;
    delete m_animation;

    if (t.isNull())
    {
        setToolTip( "" );
        update();
    }
    else if (m_track.isValid())
    {
        m_animation = new ScrobbleButtonAnimation( watch->elapsed(), watch->scrobblePoint() );
        m_animation->setParent( watch );
        connect( m_animation, SIGNAL(pixmap( QPixmap )), SLOT(setPixmap( QPixmap )) );

        connect( watch, SIGNAL(paused( bool )), m_animation, SLOT(setPaused( bool )) );
        connect( watch, SIGNAL(timeout()), m_animation, SLOT(glow()) );

        setPixmap( m_animation->pixmap() );
        
        // this happens if we respool a track that we were observing already before
        if (watch->remaining() == 0)
            m_animation->glow( 0 );
    }
}


void
ScrobbleButton::setPixmap( const QPixmap& p )
{
    m_pixmap = p;
    update();
}


void
ScrobbleButton::updateToolTip( int const s )
{
    setToolTip( QString::number( s / 60 ) +
                ':' +
                QString::number( s % 60 ).rightJustified( 2, '0' ) );
}


void
ScrobbleButton::onChecked( bool b )
{
    if (b && m_animation && m_animation->done()) 
        m_animation->glow( 0 );
    else if (!b)
        m_pixmap = scrobbleButtonPixmap( 25 );
}
