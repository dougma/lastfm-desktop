/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#include "ScrobbleStatus.h"
#include "StopWatch.h"
#include "Application.h"
#include "lib/listener/PlayerConnection.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QMovie>

ScrobbleStatus::ScrobbleStatus( QWidget* parent )   
               :StylableWidget( parent ),
                m_stopWatch( 0 ),
                m_timer( 0 )
{
    new QHBoxLayout( this );
    
    layout()->setSpacing( 0 );
    layout()->setContentsMargins( 0, 0, 0, 0 );

    ui.as = new QLabel();
    ui.as->setObjectName( "as_logo" );

    QMovie* scrobbler_as = new QMovie( ":/scrobbler_as.mng" );
    
    ui.as->setMovie( scrobbler_as );
    layout()->addWidget( ui.as );

    ui.title = new QLabel();
    layout()->addWidget( ui.title );

    ((QBoxLayout*)layout())->addStretch( 1 );

    ui.playerStatus = new QLabel();
    layout()->addWidget( ui.playerStatus );

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), SLOT(update()));
    m_timer->start(500);
}

void
ScrobbleStatus::paintEvent( QPaintEvent* event )
{
    StylableWidget::paintEvent( event );
    
    m_stopWatch = ((audioscrobbler::Application*)qApp)->stopWatch();
    if( !m_stopWatch )
        return;

    QPainter p( this );
    p.setPen( QColor( Qt::transparent ));
    p.setBrush( QColor( 0, 0, 0, 100 ));
    
    float percentage = (m_stopWatch->elapsed()/1000.0f) / m_stopWatch->scrobblePoint(); 
    p.drawRect( 0, 0, width() * percentage , height());
}

void 
ScrobbleStatus::onWatchPaused( bool isPaused )
{
    if( !isPaused ) {
        ui.as->movie()->start();
        return; 
    }
    
    ui.as->movie()->jumpToFrame( 1 );
    ui.as->movie()->stop();
}

void
ScrobbleStatus::onWatchFinished()
{

}

void 
ScrobbleStatus::onTrackStarted( const Track& track, const Track& previousTrack )
{
    qDebug() << "New Track " << track.toString();
    ui.title->setText( track.toString() );
    ui.playerStatus->setText( ((audioscrobbler::Application*)qApp)->currentConnection()->name());

    if( m_stopWatch ) {
        disconnect( m_stopWatch, 0, this, 0 );
    }

    m_stopWatch = ((audioscrobbler::Application*)qApp)->stopWatch();

    connect( m_stopWatch, SIGNAL(paused(bool)), SLOT( onWatchPaused(bool)) );
    connect( m_stopWatch, SIGNAL(timeout()), SLOT( onWatchFinished()));
}

void
ScrobbleStatus::onTrackStopped()
{
    ui.title->setText("");
    ui.playerStatus->setText("");
}