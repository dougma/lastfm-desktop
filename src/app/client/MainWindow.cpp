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
#include "widgets/SettingsDialog/SettingsDialog.h"
#include "lib/unicorn/Logger.h"
#include "lib/moose/TrackInfo.h"
#include <QLinearGradient>
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

    connect( ui.settings, SIGNAL(triggered()), SLOT(showSettingsDialog()) );
    connect( ui.quit, SIGNAL(triggered()), qApp, SLOT(quit()) );
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

    // Track bar blue bg colour
    const QColor k_trackBarBkgrBlueTop( 0xeb, 0xf0, 0xf2, 0xff );
    const QColor k_trackBarBkgrBlueMiddle( 0xe5, 0xe9, 0xec, 0xff );
    const QColor k_trackBarBkgrBlueBottom( 0xdc, 0xe2, 0xe5, 0xff );

    // Track bar progress bar colour
    const QColor k_trackBarProgressTop( 0xd6, 0xde, 0xe6, 0xff );
    const QColor k_trackBarProgressMiddle( 0xd0, 0xd9, 0xe2, 0xff );
    const QColor k_trackBarProgressBottom( 0xca, 0xd4, 0xdc, 0xff );

    // Track bar scrobbled colour
    const QColor k_trackBarScrobbledTop( 0xba, 0xc7, 0xd7, 0xff );
    const QColor k_trackBarScrobbledMiddle( 0xb8, 0xc4, 0xd5, 0xff );
    const QColor k_trackBarScrobbledBottom( 0xb5, 0xc1, 0xd2, 0xff );

    QLinearGradient g( 0, 0, 0, 20 );
    g.setColorAt( 0, k_trackBarProgressTop );
    g.setColorAt( 0.5, k_trackBarProgressMiddle );
    g.setColorAt( 0.51, k_trackBarProgressBottom );
    g.setColorAt( 1, k_trackBarProgressBottom );

    QPainter p( this );
    p.setPen( k_trackBarProgressTop );
    p.setBrush( g );
    p.drawRect( QRect( QPoint(), QPoint( m_progressDisplayTick, height() ) ) );
}


QMap<QString, QAction*>
MainWindow::actions() const
{
    QMap<QString, QAction*> map;
    foreach (QAction* a, QMainWindow::actions())
        map[a->objectName()] = a;
    return map;
}


void
MainWindow::showSettingsDialog()
{
    SettingsDialog().exec();
}