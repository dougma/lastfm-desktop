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
                   : m_scrobbleProgressTick( 0 ),
                     m_scrobblePoint( 0 )
{
    QHBoxLayout* h = new QHBoxLayout;
    h->addWidget( ui.time = new QLabel );
    h->addStretch();
    h->addWidget( ui.timeToGo = new QLabel );
	h->setMargin( 0 );
	h->setSpacing( 0 );
    setLayout( h );

    ui.time->setAttribute( Qt::WA_MacMiniSize );
    ui.timeToGo->setAttribute( Qt::WA_MacMiniSize );

#ifdef Q_WS_MAC
	QPalette p( Qt::white, Qt::black );
	ui.time->setPalette( p );
	ui.timeToGo->setPalette( p );
#endif
	
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    m_progressPaintTimer = new QTimer( this );
    connect( m_progressPaintTimer, SIGNAL(timeout()), SLOT(onProgressDisplayTick()) );

    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
}


uint
ScrobbleProgressBar::progressBarWidth() const
{
	return width() - ui.time->width() - ui.timeToGo->width() - 14;
}


void
ScrobbleProgressBar::paintEvent( QPaintEvent* )
{
    uint const h = height();

    if (ui.time->text().isEmpty())
        return;
    
	uint x1 = ui.time->rect().right() + 7;
	uint w = progressBarWidth();
	
    QPainter p( this );
    p.fillRect( x1, 0, w, h-1, QColor( 0x23, 0x23, 0x23 ) );
	
    p.setPen( QColor( 174, 174, 174 ) );
    p.setBrush( Qt::transparent );
    for (uint x = 0, n = qMin( m_scrobbleProgressTick, progressBarWidth() - 4 ); x < n; x += 2)
	{
		uint const i = x+x1+2;
        p.drawLine( i, 2, i, h-4 );
	}
}


void
ScrobbleProgressBar::determineProgressDisplayGranularity( const ScrobblePoint& g )
{
    m_progressPaintTimer->setInterval( 1000 * g / progressBarWidth() );
}


void
ScrobbleProgressBar::onProgressDisplayTick()
{
    m_scrobbleProgressTick++;
    update();
}


void
ScrobbleProgressBar::onPlaybackTick( int s )
{
	QTime t( 0, 0 );
	if (s > scrobblePoint())
		ui.timeToGo->setText( ":)" );
	else {
		t = t.addSecs( scrobblePoint() );
		t = t.addSecs( -s );
		ui.timeToGo->setText( t.toString( "-mm:ss" ) );
	}

    t = QTime( 0, 0 );
    t = t.addSecs( s );
    ui.time->setText( t.toString( "mm:ss" ) );
}


void
ScrobbleProgressBar::resizeEvent( QResizeEvent* e )
{
    if (!scrobblePoint() || e->oldSize().width() == e->size().width())
        return;

    // this is as exact as we can get it in milliseconds
    uint exactElapsedScrobbleTime = m_scrobbleProgressTick * m_progressPaintTimer->interval();

    determineProgressDisplayGranularity( scrobblePoint() );

    if (e->oldSize().width() == 0)
    {
        m_scrobbleProgressTick = 0;
    }
    else
    {
        double f = exactElapsedScrobbleTime;
        f /= scrobblePoint() * 1000;
        f *= progressBarWidth();
        m_scrobbleProgressTick = ceil( f );
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
            m_scrobbleProgressTick = 0;
            ObservedTrack t = v.value<ObservedTrack>();
            determineProgressDisplayGranularity( t.scrobblePoint() );
            m_scrobblePoint = t.scrobblePoint();
            connect( t.watch(), SIGNAL(tick( int )), SLOT(onPlaybackTick( int )) );
            connect( t.watch(), SIGNAL(destroyed()), m_progressPaintTimer, SLOT(stop()) );
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
            m_progressPaintTimer->start();
            break;

        case PlayerEvent::PlaybackStalled:
        case PlayerEvent::PlaybackPaused:
            m_progressPaintTimer->stop();
            break;
        
        case PlayerEvent::PlaybackEnded:
			m_scrobbleProgressTick = 0;
			m_progressPaintTimer->stop();
			ui.time->clear();
			ui.timeToGo->clear();
			update();
            break;
    }
}
