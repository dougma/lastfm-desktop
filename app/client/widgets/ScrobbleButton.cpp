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

#include "ScrobbleButton.h"
#include "StopWatch.h"
#include "lib/types/Track.h"
#include <QApplication>
#include <QPainter>
#include <QMovie>
#include <QTimer>


ScrobbleButton::ScrobbleButton()
              : m_progressMovie( new QMovie(":/ScrobbleButton/progress.mng") ),
                m_glowMovie( new QMovie(":/ScrobbleButton/glow.mng") ),
                m_timer( 0 )
{
    setCheckable( true );
    setChecked( true );
    setFixedSize( 45, 31 );
    m_progressMovie->start();
    m_progressMovie->setPaused( true );
    connect( m_progressMovie, SIGNAL(frameChanged( int )), SLOT(update()) );
    connect( m_glowMovie, SIGNAL(frameChanged( int )), SLOT(update()) );
    m_movie = m_progressMovie;
    
    connect( qApp, SIGNAL(trackSpooled( Track, StopWatch* )), SLOT(onTrackSpooled( Track, StopWatch* )) );
}


void
ScrobbleButton::paintEvent( QPaintEvent* )
{
    QRect r;
    QPixmap p;
    
    if (isChecked())
    {
        p = m_movie->currentPixmap();
        r = rect();
    }
    else
    {
        p = QPixmap( ":/MainWindow/scrobbling_off.png" );
        r = QRect( 2, 3, 41, 25 );
    }

    QPainter( this ).drawPixmap( r, p );
}


void
ScrobbleButton::onTrackSpooled( const Track& t, class StopWatch* watch )
{
    delete m_timer;

    m_movie = m_progressMovie;
    m_progressMovie->jumpToFrame( 0 );
    m_glowMovie->stop();
    
    if (t.isNull())
    {
        setToolTip( "" );
    }
    else
    {
        m_timer = new QTimer( watch );
        connect( m_timer, SIGNAL(timeout()), SLOT(advanceFrame()) );
        m_timer->setInterval( watch->scrobblePoint() * 1000 / 24 );
        m_timer->start();
        
        connect( watch, SIGNAL(tick( int )), SLOT(updateToolTip( int )) );
        connect( watch, SIGNAL(timeout()), SLOT(onScrobbled()) );
    }
}


void
ScrobbleButton::onScrobbled()
{
    m_movie = m_glowMovie;
    m_glowMovie->start();
}


void
ScrobbleButton::advanceFrame()
{
    m_movie->jumpToFrame( m_movie->currentFrameNumber() + 1 );
}


void
ScrobbleButton::updateToolTip( int s )
{
    int m = s / 60;
    s = s % 60;
    
    #define f( x ) QString::number( x ).leftJustified( 2, '0' )
    setToolTip( f( m ) + ':' + f( s ) );
    #undef f
}
