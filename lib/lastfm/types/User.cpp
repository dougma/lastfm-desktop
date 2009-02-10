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

#include "User.h"
#include "lib/lastfm/ws/WsRequestBuilder.h"
#include "lib/lastfm/core/CoreUrl.h"


WsReply*
User::getFriends() const
{
    return WsRequestBuilder( "user.getFriends" ).add( "user", m_name ).get();
}


WsReply*
User::getTopTags() const
{
    return WsRequestBuilder( "user.getTopTags" ).add( "user", m_name ).get();
}


WsReply* 
User::getTopArtists() const
{
    return WsRequestBuilder( "user.getTopArtists" ).add( "user", m_name ).get();
}


WsReply* 
User::getRecentArtists() const
{
    return WsRequestBuilder( "user.getRecentArtists" ).add( "user", m_name ).get();
}


WsReply* 
User::getRecentTracks() const
{
    return WsRequestBuilder( "user.getRecentTracks" ).add( "user", m_name ).get();
}


WsReply* 
User::getNeighbours() const
{
	return WsRequestBuilder( "user.getNeighbours" ).add( "user", m_name ).get();
}


WsReply*
User::getPlaylists() const
{
    return WsRequestBuilder( "user.getPlaylists" ).add( "user", m_name ).get();
}


QList<User> //static
User::list( WsReply* r )
{
	QList<User> users;
    try
    {
        foreach (CoreDomElement e, r->lfm().children( "user" ))
        {
            User u( e["name"].text() );
            u.m_smallImage = e["image size=small"].text();
            u.m_mediumImage = e["image size=medium"].text();
            u.m_largeImage = e["image size=large"].text();
            u.m_realName = e.optional( "realname" ).text();
            users += u;
        }
    }
    catch (CoreDomElement::Exception& e)
    {
        qWarning() << e;
    }    
    return users;
}


WsReply*
AuthenticatedUser::getInfo()
{
	return WsRequestBuilder( "user.getInfo" ).get();
}


QUrl
User::www() const
{ 
	return CoreUrl( "http://www.last.fm/user/" + CoreUrl::encode( m_name ) ).localised(); 
}


QString //static
AuthenticatedUser::getInfoString( WsReply* reply )
{
    #define tr QObject::tr
    
    class Gender
    {
        QString s;

    public:
        Gender( const QString& ss ) :s( ss.toLower() )
        {}
 
        bool known() const { return male() || female(); }
        bool male() const { return s == "m"; }
        bool female() const { return s == "f"; }
 
        QString toString()
        {
            QStringList list;
            if (male())
                list << tr("boy") << tr("lad") << tr("chap") << tr("guy");
            else if (female())
                // I'm not sexist, it's just I'm gutless and couldn't think
                // of any other non offensive terms for women!
                list << tr("girl") << tr("lady") << tr("lass");
            else 
                return tr("person");
            
            return list.value( QDateTime::currentDateTime().toTime_t() % list.count() );
        }
    };

    QString text;
	try
	{
    	CoreDomElement e = reply->lfm()["user"];
    	Gender gender = e["gender"].text();
    	QString age = e["age"].text();
    	uint const scrobbles = e["playcount"].text().toUInt();
    	if (gender.known() && age.size() && scrobbles > 0)
    	{
    		text = tr("A %1, %2 years of age with %L3 scrobbles")
    				.arg( gender.toString() )
    				.arg( age )
    				.arg( scrobbles );
    	}
    	else if (scrobbles > 0)
    	{
            text = tr("%L1 scrobbles").arg( scrobbles );
    	}    
    }
	catch (CoreDomElement::Exception& e)
	{
        qWarning() << e;
	}
    return text;
    
    #undef tr
}
