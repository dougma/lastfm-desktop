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

#include "Album.h"
#include "Artist.h"
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../ws/WsRequestBuilder.h"
#include <QFile>
#include <QTimer>


WsReply*
lastfm::Album::getInfo() const
{
    return WsRequestBuilder( "album.getInfo" )
            .add( "artist", m_artist )
            .add( "album", m_title )
            .get();
}



WsReply*
lastfm::Album::getTags() const
{
	return WsRequestBuilder( "album.getTags" )
            .add( "artist", m_artist )
            .add( "album", m_title )
            .get();
}


WsReply*
lastfm::Album::share( const User& recipient, const QString& message )
{
    return WsRequestBuilder( "album.share" )
		.add( "recipient", recipient )
		.add( "artist", m_artist )
		.add( "album", m_title )
		.addIfNotEmpty( "message", message )
		.post();
}


QUrl
lastfm::Album::www() const
{
    return lastfm::UrlBuilder( "music" ).slash( m_artist ).slash( m_title ).url();
}


WsReply*
lastfm::Album::addTags( const QStringList& tags ) const
{
    if (tags.isEmpty())
        return 0;

    return WsRequestBuilder( "album.addTags" )
            .add( "artist", m_artist )
            .add( "album", m_title )
            .add( "tags", tags.join( QChar(',') ) )
            .post();
}
