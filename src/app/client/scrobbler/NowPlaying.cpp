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

#include "NowPlaying.h"
#include "Scrobbler.h"
#include "lib/moose/TrackInfo.h"
#include <QDebug>
#include <QTimer>


NowPlaying::NowPlaying( Scrobbler* parent )
          : ScrobblerPostHttp( parent )
{
    m_timer = new QTimer( this );
    m_timer->setInterval( 5000 );
    m_timer->setSingleShot( true );
    connect( m_timer, SIGNAL(timeout()), SLOT(request()) );
}


void
NowPlaying::request( const TrackInfo& track )
{
    if (track.isEmpty()) {
        qDebug() << "Won't perform np request for an empty track";
        return;
    }

    #define e( x ) QUrl::toPercentEncoding( x )
    QString data =  "s=" + e(manager()->session())
                 + "&a=" + e(track.artist())
                 + "&t=" + e(track.track())
                 + "&b=" + e(track.album())
                 + "&l=" + QString::number( track.duration() )
                 + "&n=" + QString::number( track.trackNumber() )
                 + "&m=" + e(track.mbId());
    #undef e

    m_data = data.toUtf8();
    m_timer->start();
}
