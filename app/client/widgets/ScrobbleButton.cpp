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


class ScrobbleButtonToolTip : public QWidget
{

};


ScrobbleButton::ScrobbleButton()
              : m_movie( new QMovie(":/ScrobbleButton.mng") ),
                m_timer( 0 )
{
    setCheckable( true );
    setChecked( true );
    setFixedSize( 45, 31 );
    connect( m_movie, SIGNAL(frameChanged( int )), SLOT(update()) );

    onTrackSpooled( Track(), 0 );
    
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

    m_movie->start();
    m_movie->setPaused( true );
    m_movie->jumpToFrame( 0 );
    
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

        connect( watch, SIGNAL(paused()), m_timer, SLOT(stop()) );
        connect( watch, SIGNAL(resumed()), m_timer, SLOT(start()) );
        connect( watch, SIGNAL(tick( int )), SLOT(updateToolTip( int )) );
        connect( watch, SIGNAL(timeout()), SLOT(onScrobbled()) );
    }
}


void
ScrobbleButton::onScrobbled()
{
    m_timer->stop();
    m_movie->start();
}


void
ScrobbleButton::advanceFrame()
{
    int const i = m_movie->currentFrameNumber();
    Q_ASSERT( i < 25 ); //after 24 we start to glow
    
    m_movie->jumpToFrame( i + 1 );
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
