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

#include "Artist.h"
#include "User.h"
#include "../core/CoreUrl.h"
#include "../ws/WsRequestBuilder.h"


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
	return "http://www.last.fm/music/" + CoreUrl::encode( m_name );
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
		foreach (CoreDomElement e, r->lfm().children( "artist" ))
		{
			QString artistName = e["name"].text();
			float match = e["match"].text().toFloat();
			artists.push_back( WeightedString( artistName, match ));
		}
		
	}
	catch( CoreDomElement::Exception& e)
	{
		qWarning() << e;
	}
	return artists;
}


QList<Artist> /* static */
Artist::list( WsReply* r )
{
	QList<Artist> results;
	try
	{
		foreach( CoreDomElement e, r->lfm().children( "artist" ))
		{
            Artist a( e["name"].text());
            a.m_smallImage = e.optional( "image_small" ).text();
            a.m_image = e.optional( "image" ).text();
			results += a;
		}
	}
	catch( CoreDomElement::Exception& e)
	{
		qWarning() << e;
	}
	return results;
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
