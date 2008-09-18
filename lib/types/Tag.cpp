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

#include "Tag.h"
#include "User.h"
#include "lib/core/CoreUrl.h"
#include "lib/ws/WsRequestBuilder.h"

QUrl
Tag::www() const
{
	return CoreUrl( "http://www.last.fm/tag/" + CoreUrl::encode( m_name ) ).localised();
}


QUrl
Tag::www( const User& user ) const
{
	return CoreUrl( "http://www.last.fm/" + CoreUrl::encode( user ) + "/tags/" + CoreUrl::encode( m_name ) ).localised();
}


WsReply*
Tag::search() const
{
	return WsRequestBuilder( "tag.search" ).add( "tag", m_name ).get();
}


QStringList /* static */
Tag::search( WsReply* r )
{
	QStringList tags;
    foreach( EasyDomElement e, r->lfm().children( "tag" ))
    {
        try
        {
			tags += e["name"].text();
		}
        catch( EasyDomElement::Exception& e )
        {
            qWarning() << e;
        }
    }
	return tags;
}


WeightedStringList //static
Tag::list( WsReply* r )
{
	WeightedStringList tags;
    foreach (EasyDomElement e, r->lfm().children( "tag" ))
    {
        try
        {
            int weight = 0;

            //TODO non throwing version of EasyDomElement!
            try 
            {
                weight = e["count"].text().toInt();
            }
            catch( EasyDomElement::Exception& ex)
            {}
            
            // we toLower always as otherwise it is ugly mixed case, as first
            // ever tag decides case, and Last.fm is case insensitive about it 
            // anyway
            tags += WeightedString( e["name"].text().toLower(), weight );
        }
        catch( EasyDomElement::Exception& ex)
        {
            qWarning() << ex << '\n' << e;
        }
    }
    return tags;
}
