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

#include "Album.h"
#include "Artist.h"
#include "User.h"
#include "lib/ws/WsRequestBuilder.h"
#include <QEventLoop>


WsReply*
Album::getInfo() const
{
    return WsRequestBuilder( "album.getInfo" )
            .add( "artist", m_artist )
            .add( "album", m_title )
            .get();
}


WsReply*
Album::share( const User& recipient, const QString& message )
{
    return WsRequestBuilder( "album.share" )
	.add( "recipient", recipient )
	.add( "artist", m_artist )
	.add( "album", m_title )
	.addIfNotEmpty( "message", message )
	.post();
}


AlbumImageFetcher::AlbumImageFetcher( const Album& album, Album::ImageSize size )
				 : m_size( size ),
				   m_manager( 0 )
{
	qDebug() << "Fetching image for" << album;
	
    WsReply* reply = album.getInfo();
	connect( reply, SIGNAL(finished( WsReply* )), SLOT(onGetInfoFinished( WsReply* )) );
}


void
AlbumImageFetcher::onGetInfoFinished( WsReply* reply )
{
	if (reply->failed())
		return; // error is reported higher up
	
    try
    {
        QUrl url = reply->lfm()["album"]["image size="+size()].text();
		
		m_manager = new QNetworkAccessManager( this );
		
        QNetworkReply* get = m_manager->get( QNetworkRequest( url ) );
		connect( get, SIGNAL(finished()), SLOT(onImageDataDownloaded()) );
	}
    catch (EasyDomElement::Exception& e)
    {
        qWarning() << e;
		qWarning() << reply->lfm();
		emit finished( QByteArray() );
    }
}


void
AlbumImageFetcher::onImageDataDownloaded()
{
	QNetworkReply* reply = (QNetworkReply*)sender();
	emit finished( reply->readAll() );
	reply->deleteLater(); //never delete an object in a slot connected to it
						  //always call deleteLater _after_ emit
}
