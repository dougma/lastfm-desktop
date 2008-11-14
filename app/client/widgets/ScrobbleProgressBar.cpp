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
#include "StopWatch.h"
#include "lib/lastfm/scrobble/ScrobblePoint.h"
#include "lib/lastfm/types/Track.h"
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

#ifdef Q_WS_MAC
    ui.time->setAttribute( Qt::WA_MacMiniSize );
    ui.timeToGo->setAttribute( Qt::WA_MacMiniSize );

	QPalette p( Qt::white, Qt::black );
	ui.time->setPalette( p );
	ui.timeToGo->setPalette( p );
#endif

    const int N = ui.time->fontMetrics().width( "00:00" );
    ui.timeToGo->setMinimumWidth( N );
    ui.time->setMinimumWidth( N );
    
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    m_progressPaintTimer = new QTimer( this );
    connect( m_progressPaintTimer, SIGNAL(timeout()), SLOT(onProgressDisplayTick()) );

    connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
    connect( qApp, SIGNAL(trackSpooled( Track, StopWatch* )), SLOT(onTrackSpooled( Track, StopWatch* )) );
}


uint
ScrobbleProgressBar::progressBarWidth() const
{
	return width() - ui.time->width() - ui.timeToGo->width() - 14;
}


void
ScrobbleProgressBar::paintEvent( QPaintEvent* )
{
    if (ui.timeToGo->text().isEmpty())
    {
        return;
    }

	uint const h = height();
	
	uint x1 = ui.time->rect().right() + 7;
	uint w = progressBarWidth();
	
    QPainter p( this );
    p.setCompositionMode( QPainter::CompositionMode_Overlay );
    p.setBackgroundMode( Qt::TransparentMode );
    p.fillRect( x1, 1, w, h-2, QColor( 0xff, 0xff, 0xff ) );

    p.setCompositionMode( QPainter::CompositionMode_SourceOver );	
    p.setPen( QColor( 174, 174, 174 ) );
    p.setBrush( Qt::NoBrush );
    for (uint x = 0, n = qMin( m_scrobbleProgressTick, progressBarWidth() - 4 ); x < n; x += 2)
	{
		uint const i = x+x1+2;
        p.drawLine( i, 3, i, i + h-4 );
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
	if (uint(s) > m_scrobblePoint)
		ui.timeToGo->setText( ":)" );
	else {
		t = t.addSecs( m_scrobblePoint );
		t = t.addSecs( -s );
		ui.timeToGo->setText( t.toString( "mm:ss" ) );
	}

    t = QTime( 0, 0 );
    t = t.addSecs( s );
    ui.time->setText( t.toString( "mm:ss" ) );
}


void
ScrobbleProgressBar::resizeEvent( QResizeEvent* e )
{
    if (!m_scrobblePoint || e->oldSize().width() == e->size().width())
        return;

    // this is as exact as we can get it in milliseconds
    uint exactElapsedScrobbleTime = m_scrobbleProgressTick * m_progressPaintTimer->interval();

    determineProgressDisplayGranularity( m_scrobblePoint );

    if (e->oldSize().width() == 0)
    {
        m_scrobbleProgressTick = 0;
    }
    else
    {
        double f = exactElapsedScrobbleTime;
        f /= m_scrobblePoint * 1000;
        f *= progressBarWidth();
        m_scrobbleProgressTick = ceil( f );
    }

    update();
}


void
ScrobbleProgressBar::onTrackSpooled( const Track& t, StopWatch* watch )
{
    m_scrobbleProgressTick = 0;
    ui.time->clear();
    ui.timeToGo->clear();
    
    if (t.isNull())
    {
        m_progressPaintTimer->stop();
    }
    else {
        m_scrobblePoint = ScrobblePoint( watch->scrobblePoint() );
        
        determineProgressDisplayGranularity( m_scrobblePoint );
        
        connect( watch, SIGNAL(tick( int )), SLOT(onPlaybackTick( int )) );
        connect( watch, SIGNAL(destroyed()), m_progressPaintTimer, SLOT(stop()) );
        connect( watch, SIGNAL(paused()), m_progressPaintTimer, SLOT(stop()) );
        connect( watch, SIGNAL(resumed()), m_progressPaintTimer, SLOT(start()) );
        connect( watch, SIGNAL(resumed()), SLOT(update()) );
    }
    
    update();
}


void
ScrobbleProgressBar::onStateChanged( State state )
{
    switch (state)
    {                  
        case Buffering:
            if (m_scrobbleProgressTick > 0)
                ui.timeToGo->setText( "buffering..." );
            break;

        default:
            break;
    }
}
