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

#include "Tag.h"
#include "User.h"
#include "../core/UrlBuilder.h"
#include "../ws/WsRequestBuilder.h"


namespace lastfm {


QUrl
Tag::www() const
{
	return lastfm::UrlBuilder( "tag" ).slash( m_name ).url();
}


QUrl
Tag::www( const User& user ) const
{
	return lastfm::UrlBuilder( "user" ).slash( user.name() ).slash( "tags" ).slash( Tag::name() ).url();
}


WsReply*
Tag::search() const
{
	return WsRequestBuilder( "tag.search" ).add( "tag", m_name ).get();
}


QMap<int, QString> //static
Tag::list( WsReply* r )
{
	QMap<int, QString> tags;
    try
    {
        foreach (WsDomElement e, r->lfm().children( "tag" ))
        {
            int const count = e.optional("count").text().toInt();
            
            // we toLower always as otherwise it is ugly mixed case, as first
            // ever tag decides case, and Last.fm is case insensitive about it 
            // anyway
            tags.insertMulti( count, e["name"].text().toLower() );
        }
    }
    catch (std::runtime_error& e)
    {
        qWarning() << e.what();
    }
    return tags;
}


} //namespace lastfm
