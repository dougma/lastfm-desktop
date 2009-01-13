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

#include "TrackImageFetcher.h"
#include "lib/lastfm/types/Track.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QImage>


void
TrackImageFetcher::start()
{
    if (!album().isNull()) 
    {
        WsReply* reply = album().getInfo();
        connect( reply, SIGNAL(finished( WsReply* )), SLOT(onAlbumGotInfo( WsReply* )) );                
    }
    else
        artistGetInfo();
}


void
TrackImageFetcher::onAlbumGotInfo( WsReply* reply )
{
	if (!downloadImage( reply, "album" ))
        artistGetInfo();
}


void
TrackImageFetcher::onAlbumImageDownloaded()
{
    QImage i;
    i.loadFromData( ((QNetworkReply*)sender())->readAll() );
    if (!i.isNull())
        emit finished( i );
    else
        artistGetInfo();
    
    sender()->deleteLater(); //always deleteLater from slots connected to sender()
}


void
TrackImageFetcher::artistGetInfo()
{
    if (!artist().isNull())
    {
        WsReply* reply = artist().getInfo();
        connect( reply, SIGNAL(finished( WsReply* )), SLOT(onArtistGotInfo( WsReply* )) );
    }
    else
        fail();
}


void
TrackImageFetcher::onArtistGotInfo( WsReply* reply )
{
    if (!downloadImage( reply, "artist" ))
        fail();
}


void
TrackImageFetcher::onArtistImageDownloaded()
{
    QImage i;
    i.loadFromData( ((QNetworkReply*)sender())->readAll() );
    if (!i.isNull())
        emit finished( i );
    else
        fail();
    
    sender()->deleteLater(); //always deleteLater from slots connected to sender()
}


bool
TrackImageFetcher::downloadImage( WsReply* reply, const QString& root_node )
{    
    if (reply->failed())
        return false;
    
    foreach (QString size, QStringList() << "extralarge" << "large")
    {
        try 
        {
            QUrl const url = reply->lfm()[root_node]["image size="+size].text();
            
            // we seem to get a load of album.getInfos where the node exists
            // but the value is ""
            if (!url.isValid())
                return false;
            
            QNetworkReply* get = nam->get( QNetworkRequest( url ) );
            connect( get, SIGNAL(finished()), SLOT(onArtistImageDownloaded()) );
            return true;
        }
        catch (CoreDomElement::Exception& e)
        {}
    }
    
    return false;
}


void
TrackImageFetcher::fail()
{
    emit finished( QImage( ":/lastfm/no/cover.png" ) );
}
