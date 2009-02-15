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

#include "Artist.h"
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../ws/WsRequestBuilder.h"


namespace lastfm {


WsReply* 
Artist::share( const User& user, const QString& message )
{
    return WsRequestBuilder( "artist.share" )
        .add( "recipient", user )
        .add( "artist", m_name )
        .addIfNotEmpty( "message", message )
        //TODO this must be post! you're testing here
        .get();
}


QUrl 
Artist::www() const
{
    return lastfm::UrlBuilder( "music" ).slash( Artist::name() ).url();
}


WsReply* 
Artist::getInfo() const
{
	return WsRequestBuilder( "artist.getInfo" ).add( "artist", m_name ).get();
}


WsReply* 
Artist::getTags() const
{
	return WsRequestBuilder( "artist.getTags" ).add( "artist", m_name ).get();
}

WsReply* 
Artist::getTopTags() const
{
	return WsRequestBuilder( "artist.getTopTags" ).add( "artist", m_name ).get();
}


WsReply* 
Artist::getSimilar() const
{
	return WsRequestBuilder( "artist.getSimilar" ).add( "artist", m_name ).get();
}


WsReply* 
Artist::search( int limit ) const
{
    WsRequestBuilder r( "artist.search" );
    r.add( "artist", m_name );

    if( limit > 0 ) r.add( "limit", limit );
    
    return r.get();
}


WeightedStringList /* static */
Artist::getSimilar( WsReply* r )
{
	WeightedStringList artists;
	try
	{
		foreach (WsDomElement e, r->lfm().children( "artist" ))
		{
			QString artistName = e["name"].text();
			float match = e["match"].text().toFloat();
			artists.push_back( WeightedString( artistName, match ));
		}
		
	}
	catch( WsDomElement::Exception& e)
	{
		qWarning() << e;
	}
	return artists;
}


static inline void images( QList<QUrl>& images, const WsDomElement& e )
{
    images.clear();
    images += e.optional( "image size=small" ).text();
    images += e.optional( "image size=medium" ).text();
    images += e.optional( "image size=large" ).text();
}


QList<Artist> /* static */
Artist::list( WsReply* r )
{
	QList<Artist> artists;
	foreach (WsDomElement e, r->lfm().children( "artist" ))
	{
    	try
    	{    
            Artist artist( e["name"].text());
            images( artist.m_images, e );
    		artists += artist;
	    }
    	catch (WsDomElement::Exception& e)
    	{
    		qWarning() << e;
    	}
	}
	return artists;
}


Artist
Artist::getInfo( WsReply* r )
{
    Artist artist( r->lfm()["artist"]["name"].text() );
    images( artist.m_images, r->lfm()["artist"] );
    return artist;
}


WsReply*
Artist::addTags( const QStringList& tags ) const
{
    if (tags.isEmpty())
        return 0;

    return WsRequestBuilder( "artist.addTags" )
            .add( "artist", m_name )
            .add( "tags", tags.join( QChar(',') ) )
            .post();
}


} //namespace lastfm
