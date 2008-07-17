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
#include "lib/unicorn/UnicornSettings.h"
#include <QBuffer>
#include <QDebug>
#include "lib/unicorn/ws/WsRequestBuilder.h"
#include "lib/unicorn/ws/WsReply.h"
#include <QtNetwork/QHttp> //TODO use our override
#include <QtXml>

namespace Radio
{
    QList<Track> getPlaylist();
}

//TODO discovery mode
//TODO skips left
//TODO multiple locations for the same track
//TODO set rtp flag in getPlaylist (whether user is scrobbling this radio session or not)


Tuner::Tuner( const RadioStation& station )
{
    WsReply* reply = WsRequestBuilder( "radio.tune" )
        .add( "station", station )
        .post();
    reply->finish();
}


QList<Track>
Tuner::fetchNextPlaylist()
{
    QList<Track> tracks = Radio::getPlaylist();
    foreach (Track t, tracks)
        qDebug() << t;
    return tracks;
}


QList<Track>
Radio::getPlaylist()
{
    WsReply* reply = WsRequestBuilder( "radio.getPlaylist" ).get();
    reply->finish();

    QList<Track> tracks;
    try
    {
        foreach (EasyDomElement e, reply->lfm()["playlist"][ "trackList" ].children( "track" ))
        {
            MutableTrack t;
            try
            {
                //TODO we don't want to throw an exception for any of these really,
                //TODO only if we couldn't get any, but even then so what
                t.setUrl( e[ "location" ].text() ); //location first due to exception throwing
                t.setExtra( "trackauth", e[ "extension" ][ "trackauth" ].text() );
                t.setTitle( e[ "title" ].text() );
                t.setArtist( e[ "creator" ].text() );
                t.setAlbum( e[ "album" ].text() );
                t.setDuration( e[ "duration" ].text().toInt() );
            }
            catch (EasyDomElement::Exception& e)
            {
            	
            }
            tracks += t; // outside since location is enough basically
        }
    }
    catch (EasyDomElement::Exception& e)
    {

    }

    return tracks;
}
