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

#include "Tuner.h"
#include "Playlist.h"
#include "lib/unicorn/UnicornSettings.h"
#include <QBuffer>
#include <QDebug>
#include "lib/ws/WsRequestBuilder.h"
#include "lib/ws/WsReply.h"
#include <QtNetwork/QHttp> //TODO use our override
#include <QtXml>


//TODO discovery mode
//TODO skips left
//TODO multiple locations for the same track
//TODO set rtp flag in getPlaylist (whether user is scrobbling this radio session or not)


Tuner::Tuner( const RadioStation& station )
{
    WsReply* reply = WsRequestBuilder( "radio.tune" )
			.add( "station", station )
			.post();
	connect( reply, SIGNAL(finished( WsReply* )), SLOT(onTuneReturn( WsReply* )) );
}


void
Tuner::onTuneReturn( WsReply* reply )
{
	if (reply->error() == Ws::NoError)
	{
		try 
		{
			m_stationName = reply->lfm()["station"]["name"].text();
			emit stationName( m_stationName );
		}
		catch (UnicornException&)
		{}
		
		fetchFiveMoreTracks();
	}
	else
		emit error( 0 );
}


void
Tuner::fetchFiveMoreTracks()
{
    WsReply* reply = WsRequestBuilder( "radio.getPlaylist" ).add( "rtp", "1" ).get();
	connect( reply, SIGNAL(finished( WsReply* )), SLOT(onGetPlaylistReturn( WsReply* )) );
}


void
Tuner::onGetPlaylistReturn( WsReply* reply )
{
	Playlist p( reply );
	
	foreach (Track t, p.tracks())
		qDebug() << t << t.duration();
	
	emit tracks( p.tracks() );
}
