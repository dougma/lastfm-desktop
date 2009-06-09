/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "TrackImageFetcher.h"
#include <lastfm/Track>
#include <lastfm/ws.h>
#include <lastfm/XmlQuery>
#include <QImage>
#include <QStringList>


void
TrackImageFetcher::start()
{
    if (!album().isNull()) 
    {
        QNetworkReply* reply = album().getInfo();
        connect( reply, SIGNAL(finished()), SLOT(onAlbumGotInfo()) );
    }
    else
        artistGetInfo();
}


void
TrackImageFetcher::onAlbumGotInfo()
{
	if (!downloadImage( (QNetworkReply*)sender(), "album" ))
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
        QNetworkReply* reply = artist().getInfo();
        connect( reply, SIGNAL(finished()), SLOT(onArtistGotInfo()) );
    }
    else
        fail();
}


void
TrackImageFetcher::onArtistGotInfo()
{
    if (!downloadImage( (QNetworkReply*)sender(), "artist" ))
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
TrackImageFetcher::downloadImage( QNetworkReply* reply, const QString& root_node )
{
    try {
        XmlQuery lfm = lastfm::ws::parse( reply );
        foreach (QString size, QStringList() << "extralarge" << "large")
        {
            QUrl const url = lfm[root_node]["image size="+size].text();

            // we seem to get a load of album.getInfos where the node exists
            // but the value is ""
            if (!url.isValid())
                return false;

            QNetworkReply* get = lastfm::nam()->get( QNetworkRequest( url ) );
            connect( get, SIGNAL(finished()), SLOT(onArtistImageDownloaded()) );
            return true;
        }
    }
    catch (std::runtime_error& e)
    {
        qWarning() << e.what();
    }
    return false;
}


void
TrackImageFetcher::fail()
{
    emit finished( QImage( ":/lastfm/no/cover.png" ) );
}
