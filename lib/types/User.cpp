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

#include "User.h"
#include "lib/ws/WsRequestBuilder.h"


WsReply*
User::getFriends()
{
    return WsRequestBuilder( "user.getFriends" ).add( "user", m_name ).get();
}


WsReply*
User::getTopTags()
{
    return WsRequestBuilder( "user.getTopTags" ).add( "user", m_name ).get();
}


WsReply* 
User::getNeighbours()
{
	return WsRequestBuilder( "user.getNeighbours" ).add( "user", m_name ).get();
}


UserList
User::getFriends( WsReply* r )
{
	UserList users;
    try
    {
        foreach (EasyDomElement e, r->lfm().children( "user" ))
		{
            User user( e["name"].text() );
			user.m_smallImage = e["image size=small"].text();
			user.m_mediumImage = e["image size=medium"].text();
			user.m_largeImage = e["image size=large"].text();
			users += user;
		}
    }
    catch (EasyDomElement::Exception& e)
    {
        qWarning() << e;
    }
    return users;
}


WsReply*
User::getInfo()
{
	return WsRequestBuilder( "user.getInfo" ).get();
}


WeightedStringList //static
User::getTopTags( WsReply* r )
{
	WeightedStringList tags;
	try
	{
		foreach (EasyDomElement e, r->lfm().children( "tag" ))
		{
			QString tagname = e["name"].text();
			int count = e["count"].text().toInt();
			tags.push_back( WeightedString( tagname, count ));
		}
		
	}
	catch( EasyDomElement::Exception& e)
	{
		qWarning() << e;
	}
	return tags;
}


WeightedStringList //static
User::getNeighbours( WsReply* r )
{
	WeightedStringList neighbours;
	try
	{
		foreach (EasyDomElement e, r->lfm().children( "user" ))
		{
			QString name = e["name"].text();
			int match = e["match"].text().toFloat();
			neighbours.push_back( WeightedString( name, match ));
		}
		
	}
	catch( EasyDomElement::Exception& e)
	{
		qWarning() << e;
	}
	return neighbours;
}
