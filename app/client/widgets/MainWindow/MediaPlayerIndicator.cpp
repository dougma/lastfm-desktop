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

#include "MediaPlayerIndicator.h"
#include "the/radio.h"
#include "lib/lastfm/ws/WsKeys.h"
#include "lib/lastfm/radio/RadioStation.h"
#include "lib/lastfm/types/Track.h"
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLabel>
#define PRE "<b><font color=#5B5D5F>"


MediaPlayerIndicator::MediaPlayerIndicator()
{
    QLabel* username;
    
    QHBoxLayout* h = new QHBoxLayout( this );
	h->setMargin( 0 );
	h->addWidget( username = new QLabel( QString(PRE) + Ws::Username ) );
	h->addStretch();
	h->addWidget( m_status = new QLabel );

    connect( qApp, SIGNAL(playerChanged( QString )), SLOT(onPlayerChanged( QString )) );
    connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
    connect( &The::radio(), SIGNAL(tuningIn( RadioStation )), SLOT(onTuningIn( RadioStation )) );
    
	// prevent the text length resizing the window!
	username->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    m_status->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    
#ifdef Q_WS_MAC
	QPalette p( Qt::white, Qt::black ); //Qt-4.4.1 on mac sucks
	username->setPalette( p );
	m_status->setPalette( p );
    username->setAttribute( Qt::WA_MacMiniSize );
	m_status->setAttribute( Qt::WA_MacMiniSize );
#endif
}


void
MediaPlayerIndicator::onPlayerChanged( const QString& name )
{   
    m_playerName = name;
    
    m_playbackCommencedString = !name.isEmpty()
            ? m_playbackCommencedString = PRE + tr("listening to %1").arg( "</font>" + name )
            : "";
}


void
MediaPlayerIndicator::onTuningIn( const RadioStation& station )
{
    m_status->setText( PRE + tr( "tuning to %1").arg( station.url() ) );
      
    m_playbackCommencedString = station.title().isEmpty()
            ? "Last.fm"
            : PRE + tr( "%1 on</font> Last.fm", "eg. Recommendation Radio on Last.fm" ).arg( station.title() );
}


void
MediaPlayerIndicator::onStateChanged( State state )
{   
    switch (state)
    {
        case Stopped:
            m_status->clear();
            break;
            
        case Playing:
            m_status->setText( m_playbackCommencedString );
            break;

        case Paused:
            m_status->setText( "<b>" + tr("%1 is paused").arg( m_playerName + PRE ) );
            break;
            
        default:
            break;
    }
}
