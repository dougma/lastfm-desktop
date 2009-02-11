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

#include "Playlist.h"
#include "Track.h"
#include "../ws/WsRequestBuilder.h"


WsReply*
lastfm::Playlist::addTrack( const Track& t ) const
{
	return WsRequestBuilder( "playlist.addTrack" )
			.add( "playlistID", m_id )
			.add( "artist", t.artist() )
			.add( "track", t.title() )
			.post();
}


WsReply*
lastfm::Playlist::fetch() const
{
	return fetch( QUrl("lastfm://playlist/" + QString::number( m_id )) );
}


WsReply* //static
lastfm::Playlist::fetch( const QUrl& url )
{
	return WsRequestBuilder( "playlist.fetch" )
			.add( "playlistURL", url.toString() )
			.get();
}


WsReply* //static
lastfm::Playlist::create( const QString& title, const QString& description /*=""*/ )
{
	return WsRequestBuilder( "playlist.create" )
			.add( "title", title )
			.addIfNotEmpty( "description", description )
			.post();
}
