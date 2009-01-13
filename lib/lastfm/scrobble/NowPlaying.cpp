/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "NowPlaying.h"
#include "Scrobbler.h"
#include "lib/lastfm/types/Track.h"
#include <QDebug>
#include <QTimer>


NowPlaying::NowPlaying( const QByteArray& data )
{
    // will be submitted after the handshake, if there is some data that is
    m_data = data;

    // we wait 5 seconds to prevent the server panicking when people skip a lot
    // tracks in succession
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    connect( m_timer, SIGNAL(timeout()), SLOT(request()) );
}


void
NowPlaying::reset()
{
    m_timer->stop();
    m_data.clear();
}


void
NowPlaying::submit( const Track& track )
{   
    if (track.isNull())
        return;

    #define e( x ) QUrl::toPercentEncoding( x )
    m_data = "&a=" + e(track.artist()) +
             "&t=" + e(track.title()) +
             "&b=" + e(track.album()) +
             "&l=" + QByteArray::number( track.duration() ) +
             "&n=" + QByteArray::number( track.trackNumber() ) +
             "&m=" + e(track.mbid());
    #undef e

    // m_time is initialised to midnight, so a bug exists that if the app is 
    // started after 00:00 and before 00:04 we trigger via the timer. But meh!
    uint ms = m_delay.elapsed();
    
    if (ms < 10000) {
        m_timer->setInterval( 10000 - ms );
        m_timer->start();
    }
    else {
        m_delay.restart();
        request();
    }
}
