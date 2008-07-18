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

#include "ScrobbleProgressBar.h"
#include "PlayerEvent.h"
#include <QtGui>


ScrobbleProgressBar::ScrobbleProgressBar()
                   : m_progressDisplayTick( 0 ),
                     m_scrobblePoint( 0 )
{
    QHBoxLayout* h = new QHBoxLayout;
    h->addWidget( ui.time = new QLabel );
    h->addStretch();
    h->addWidget( ui.timeToScrobblePoint = new QLabel );
    setLayout( h );

    ui.time->setDisabled( true ); //aesthetics
    ui.timeToScrobblePoint->setDisabled( true ); //aesthetics

    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    m_progressDisplayTimer = new QTimer( this );
    connect( m_progressDisplayTimer, SIGNAL(timeout()), SLOT(onProgressDisplayTick()) );

    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
}


void
ScrobbleProgressBar::paintEvent( QPaintEvent* e )
{
    QWidget::paintEvent( e );

    if (!m_progressDisplayTick)
        return;

    static bool b = true;
    static QLinearGradient g( 0, 0, 0, 20 );
    const QColor k_trackBarProgressTop( 0xd6, 0xde, 0xe6, 0xff );
    if (b)
    {
        // Track bar blue bg colour
        const QColor k_trackBarBkgrBlueTop( 0xeb, 0xf0, 0xf2, 0xff );
        const QColor k_trackBarBkgrBlueMiddle( 0xe5, 0xe9, 0xec, 0xff );
        const QColor k_trackBarBkgrBlueBottom( 0xdc, 0xe2, 0xe5, 0xff );

        // Track bar progress bar colour
        const QColor k_trackBarProgressMiddle( 0xd0, 0xd9, 0xe2, 0xff );
        const QColor k_trackBarProgressBottom( 0xca, 0xd4, 0xdc, 0xff );

        // Track bar scrobbled colour
        const QColor k_trackBarScrobbledTop( 0xba, 0xc7, 0xd7, 0xff );
        const QColor k_trackBarScrobbledMiddle( 0xb8, 0xc4, 0xd5, 0xff );
        const QColor k_trackBarScrobbledBottom( 0xb5, 0xc1, 0xd2, 0xff );

        g.setColorAt( 0, k_trackBarProgressTop );
        g.setColorAt( 0.5, k_trackBarProgressMiddle );
        g.setColorAt( 0.51, k_trackBarProgressBottom );
        g.setColorAt( 1, k_trackBarProgressBottom );
    }

    QPainter p( this );
    p.setPen( k_trackBarProgressTop );
    p.setBrush( g );
    p.drawRect( QRect( QPoint(), QPoint( m_progressDisplayTick, height() ) ) );
}


void
ScrobbleProgressBar::determineProgressDisplayGranularity( const ScrobblePoint& g )
{
    m_progressDisplayTimer->setInterval( 1000 * g / width() );
}


void
ScrobbleProgressBar::onProgressDisplayTick()
{
    m_progressDisplayTick++;
    update();
}


void
ScrobbleProgressBar::onPlaybackTick( int s )
{
    if ((uint)s < scrobblePoint())
    {
        QTime t( 0, 0 );
        t = t.addSecs( scrobblePoint() );
        t = t.addSecs( -s );
        ui.timeToScrobblePoint->setText( t.toString( "-mm:ss" ) );
    }
    else
        ui.timeToScrobblePoint->setText( tr( "Scrobbled" ) );

    QTime t( 0, 0 );
    t = t.addSecs( s );
    ui.time->setText( t.toString( "mm:ss" ) );
}


void
ScrobbleProgressBar::resizeEvent( QResizeEvent* e )
{
    if (!scrobblePoint())
        return;

    // this is as exact as we can get it in milliseconds
    uint exactElapsedScrobbleTime = m_progressDisplayTick * m_progressDisplayTimer->interval();

    determineProgressDisplayGranularity( scrobblePoint() );

    if (e->oldSize().width() == 0)
    {
        m_progressDisplayTick = 0;
    }
    else
    {
        double f = exactElapsedScrobbleTime;
        f /= scrobblePoint() * 1000;
        f *= e->size().width();
        m_progressDisplayTick = ceil( f );
    }

    update();
}


void
ScrobbleProgressBar::onAppEvent( int e, const QVariant& v )
{
    switch (e)
    {
    case PlayerEvent::PlaybackStarted:
    case PlayerEvent::TrackChanged:
        {
            onPlaybackTick( 0 );
            m_progressDisplayTick = 0;
            ObservedTrack t = v.value<ObservedTrack>();
            determineProgressDisplayGranularity( t.scrobblePoint() );
            m_scrobblePoint = t.scrobblePoint();
            connect( t.watch(), SIGNAL(tick( int )), SLOT(onPlaybackTick( int )) );
            connect( t.watch(), SIGNAL(destroyed()), m_progressDisplayTimer, SLOT(stop()) );
            update();
            break;
        }
    }

    switch (e)
    {
        case PlayerEvent::PlaybackStarted:
        case PlayerEvent::TrackChanged:
        case PlayerEvent::PlaybackUnstalled:
        case PlayerEvent::PlaybackUnpaused:
            m_progressDisplayTimer->start();
            break;

        case PlayerEvent::PlaybackStalled:
        case PlayerEvent::PlaybackPaused:
            m_progressDisplayTimer->stop();
            break;
    }
}
