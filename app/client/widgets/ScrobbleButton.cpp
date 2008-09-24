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
#include <QApplication>
#include <QPainter>
#include <QMovie>
#include <QTimeLine>


class ScrobbleButtonToolTip : public QWidget
{

};


ScrobbleButton::ScrobbleButton()
              : m_movie( new QMovie( this ) )
{
    m_timer = 0;
    
    setCheckable( true );
    setChecked( true );
    setFixedSize( 51, 37 );
    connect( m_movie, SIGNAL(frameChanged( int )), SLOT(update()) );
    
    connect( qApp, SIGNAL(trackSpooled( Track, StopWatch* )), SLOT(onTrackSpooled( Track, StopWatch* )) );
}


void
ScrobbleButton::paintEvent( QPaintEvent* )
{
    QPixmap p;
    
    if (m_track.isNull()) {
        p = QPixmap( ":/ScrobbleButton/scrobbling_on.png" );
    }
    else if (!m_track.isValid()) {
        p = QPixmap( ":/ScrobbleButton/cant_scrobble.png" );
    }
    else if (isChecked()) {
        p = m_movie->currentPixmap();
    }
    else
        p = QPixmap( ":/ScrobbleButton/scrobbling_off.png" );

    QPainter( this ).drawPixmap( rect(), p );
}


void
ScrobbleButton::onTrackSpooled( const Track& t, class StopWatch* watch )
{
    m_track = t;
    
    delete m_timer;
    m_timer = 0;
    
    //only way to make the movie restart with Qt 4.4.2
    //TODO optimise, this can't be cheap to do if unecessary
    m_movie->setFileName( ":/ScrobbleButton.mng" );
    m_movie->start();
    m_movie->setPaused( true );
    m_movie->jumpToFrame( 0 );
    
    if (t.isNull())
    {
        setToolTip( "" );
    }
    else if (m_track.isValid())
    {
        m_timer = new QTimeLine;
        m_timer->setParent( this );
        m_timer->setCurveShape( QTimeLine::LinearCurve );
        connect( m_timer, SIGNAL(frameChanged( int )), SLOT(setFrame( int )) );
        
        m_timer->setFrameRange( 0, 23 );
        m_timer->setDuration( watch->scrobblePoint() * 1000 );
        m_timer->start();

        connect( watch, SIGNAL(paused( bool )), m_timer, SLOT(setPaused( bool )) );
        connect( watch, SIGNAL(tick( int )), SLOT(updateToolTip( int )) );
        connect( watch, SIGNAL(timeout()), SLOT(onScrobbled()) );
    }
}


void
ScrobbleButton::onScrobbled()
{    
    delete m_timer;
    m_movie->start();
}


void
ScrobbleButton::setFrame( int i )
{
    m_movie->jumpToFrame( i );
}


void
ScrobbleButton::updateToolTip( int const s )
{
    setToolTip( QString::number( s / 60 ) +
                ':' +
                QString::number( s % 60 ).rightJustified( 2, '0' ) );
}
