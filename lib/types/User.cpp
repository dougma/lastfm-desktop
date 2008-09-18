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


QList<User>
User::getFriends( WsReply* r )
{
	QList<User> users;
    foreach (EasyDomElement e, r->lfm().children( "user" ))
    {
        try
        {
            User user( e["name"].text() );
			user.m_smallImage = e["image size=small"].text();
			user.m_mediumImage = e["image size=medium"].text();
			user.m_largeImage = e["image size=large"].text();
			users += user;
        }
        catch (EasyDomElement::Exception& e)
        {
            qWarning() << e;
        }
    }
    return users;
}


WsReply*
AuthenticatedUser::getInfo()
{
	return WsRequestBuilder( "user.getInfo" ).get();
}


QList<User> //static
User::getNeighbours( WsReply* r )
{
	QList<User> neighbours;
    foreach (EasyDomElement e, r->lfm().children( "user" ))
    {
        try
        {
			User user( e["name"].text() );
			user.m_match = e["match"].text().toFloat();
			user.m_smallImage = e["image size=small"].text();
			user.m_mediumImage = e["image size=medium"].text();
			user.m_largeImage = e["image size=large"].text();
			neighbours << user;
        }
        catch( EasyDomElement::Exception& e)
        {
            qWarning() << e;
        }
    }
	return neighbours;
}
