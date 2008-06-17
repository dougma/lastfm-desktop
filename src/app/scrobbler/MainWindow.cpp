/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "MainWindow.h"
#include "PlaybackEvent.h"
#include "PlayerManager.h"
#include "lib/unicorn/Logger.h"
#include "lib/moose/TrackInfo.h"
#include <QPainter>
#include <QResizeEvent>
#include <QTime>
#include <QTimer>
#include <QVariant>
#include <cmath>


MainWindow::MainWindow()
          : m_progressDisplayTick( 0 )
{
    m_progressDisplayTimer = new QTimer( this );
    connect( m_progressDisplayTimer, SIGNAL(timeout()), SLOT(onProgressDisplayTick()) );

    ui.setupUi( this );

    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    connect( &The::playerManager(), SIGNAL(tick( int )), SLOT(onPlaybackTick( int )) );
}


void
MainWindow::onAppEvent( int e, const QVariant& v )
{
    switch (e)
    {
        case PlaybackEvent::PlaybackStarted:
        case PlaybackEvent::TrackChanged:
            ui.track->setText( v.value<TrackInfo>().toString() );
            ui.source->setText( "00:00" );
            break;

        case PlaybackEvent::PlaybackEnded:
            qDebug() << "ENDED";
            ui.track->clear();
            ui.source->clear();
            break;
    }

    // progress display timer
    switch (e)
    {
        case PlaybackEvent::PlaybackStarted:
        case PlaybackEvent::PlaybackUnstalled:
        case PlaybackEvent::PlaybackUnpaused:
            determineProgressDisplayGranularity( The::observed().scrobblePoint() );
            m_progressDisplayTimer->start();
            break;
        
        case PlaybackEvent::PlaybackStalled:
        case PlaybackEvent::PlaybackPaused:
        case PlaybackEvent::PlaybackEnded:
            m_progressDisplayTimer->stop();
            break;
    }
    switch (e)
    {
        case PlaybackEvent::PlaybackStarted:
        case PlaybackEvent::TrackChanged:
        case PlaybackEvent::PlaybackEnded:
            m_progressDisplayTick = 0;
            update();
    }
}


void
MainWindow::onPlaybackTick( int s )
{
    QTime t( 0, 0 );
    t = t.addSecs( s );
    ui.source->setText( t.toString( "mm:ss" ) );
}


void
MainWindow::resizeEvent( QResizeEvent* e )
{
    if (The::playerManager().state() == PlaybackState::Stopped)
        return;

    // this is as exact as we can get it in milliseconds
    uint exactElapsedScrobbleTime = m_progressDisplayTick * m_progressDisplayTimer->interval();

    determineProgressDisplayGranularity( The::observed().scrobblePoint() );
    
    if (e->oldSize().width() == 0)
    {
        m_progressDisplayTick = 0;
    }
    else
    {
        double f = exactElapsedScrobbleTime;
        f /= The::observed().scrobblePoint() * 1000;
        f *= e->size().width();
        m_progressDisplayTick = ceil( f );
    }

    update();
}


void
MainWindow::determineProgressDisplayGranularity( uint g )
{
    if (g == 0)
    {
        //TODO #error better handling with gui stuff but non intrusive
    }
    else
        m_progressDisplayTimer->setInterval( 1000 * g / width() );
}


void
MainWindow::onProgressDisplayTick()
{
    m_progressDisplayTick++;
    update();
}


void
MainWindow::paintEvent( QPaintEvent* e )
{
    QMainWindow::paintEvent( e );

    if (!m_progressDisplayTick)
        return;

    QPainter p( this );
    p.setPen( Qt::gray );
    p.setBrush( Qt::lightGray );
    p.drawRect( QRect( QPoint(), QPoint( m_progressDisplayTick, height() ) ) );
}
