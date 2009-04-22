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

#include "LegacyTuner.h"
#include "lib/lastfm/ws/WsDomElement.h"
#include "lib/lastfm/core/CoreSettings.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include "lib/lastfm/types/Xspf.h"
#include <QCoreApplication>
#include <QtNetwork>

using lastfm::Track;
using lastfm::Xspf;


static inline QByteArray iso3166() { return QLocale().name().right( 2 ).toAscii().toLower(); }


LegacyTuner::LegacyTuner( const RadioStation& station, const QString& password_md5 )
     : m_nam( new WsAccessManager( this ) ),
       m_retry_counter( 0 ),
       m_station( station )
{    
#ifdef WIN32
    static const char *PLATFORM = "win32";
#elif defined Q_WS_X11
    static const char *PLATFORM = "linux";
#elif defined Q_WS_MAC
    static const char *PLATFORM = "mac";
#else
    static const char *PLATFORM = "unknown";
#endif

    QUrl url;
    url.setScheme( "http" );
    url.setHost( "ws.audioscrobbler.com" );
    url.setPath( "/radio/handshake.php" );
    url.addEncodedQueryItem( "version", QCoreApplication::applicationVersion().toAscii() );
    url.addEncodedQueryItem( "platform", PLATFORM );
    url.addEncodedQueryItem( "username", QUrl::toPercentEncoding(Ws::Username) );
    url.addEncodedQueryItem( "passwordmd5", password_md5.toAscii() );
    url.addEncodedQueryItem( "language", iso3166() );

    QNetworkRequest request( url );
    QNetworkReply* reply = m_nam->get( request );
    connect( reply, SIGNAL(finished()), SLOT(onHandshakeReturn()) );
}

    
static QByteArray replyParameter( const QByteArray& data, const QByteArray& key )
{
    foreach (QByteArray key_value_pair, data.split( '\n' ))
    {
        QList<QByteArray> pair = key_value_pair.split( '=' );
        if (pair.value( 0 ) == key)
            return pair.value( 1 );
    }

    return "";
}
    

void
LegacyTuner::onHandshakeReturn()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    reply->deleteLater();
    
    QByteArray data = reply->readAll();
    qDebug() << data;

    m_session = replyParameter( data, "session" );
    
    QUrl url;
    url.setScheme( "http" );
    url.setHost( "ws.audioscrobbler.com" );
    url.addEncodedQueryItem( "session", m_session );
    url.addQueryItem( "url", m_station.url() );
    
    if (m_station.isLegacyPlaylist())
    {
        // It's a preview/playlist, use getresourceplaylist
        url.setPath( "/1.0/webclient/getresourceplaylist.php" );
        url.addQueryItem( "desktop", "1" );

        QNetworkRequest request( url );
        reply = m_nam->get( request );
        connect( reply, SIGNAL(finished()), SLOT(onGetPlaylistReturn()) );        
    }
    else
    {
        url.setPath( "/radio/adjust.php" );
        url.addEncodedQueryItem( "lang", iso3166() );

        QNetworkRequest request( url );
        reply = m_nam->get( request );
        connect( reply, SIGNAL(finished()), SLOT(onAdjustReturn()) );
    }
}


void
LegacyTuner::onAdjustReturn()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    QByteArray data = reply->readAll();
    
    emit title( QString::fromUtf8( replyParameter( data, "stationname" ) ) );
    
    fetchFiveMoreTracks();

    reply->deleteLater();
}


Track
LegacyTuner::takeNextTrack()
{
    Track t;
    if (!m_queue.isEmpty()) {
        t = m_queue.takeFirst();
        if (m_queue.isEmpty())
            fetchFiveMoreTracks();
    }
    return t;
}

bool
LegacyTuner::fetchFiveMoreTracks()
{
    // legacy playlists return all tracks in the first xspf, so we can't ever
    // fetch more tracks
    if (m_station.isLegacyPlaylist())
        return false;
    
    QUrl url;
    url.setScheme( "http" );
    url.setHost( "ws.audioscrobbler.com" );
    url.setPath( "/radio/xspf.php" );
    url.addQueryItem( "sk", m_session );
    url.addQueryItem( "desktop", "1.5.3" );
    
    QNetworkRequest request( url );
    QNetworkReply* reply = m_nam->get( request );
    connect( reply, SIGNAL(finished()), SLOT(onGetPlaylistReturn()) );
    
    return true;
}


bool
LegacyTuner::tryAgain()
{
	if (++m_retry_counter > 5)
		return false;
	fetchFiveMoreTracks();
	return true;
}

    
void
LegacyTuner::onGetPlaylistReturn()
{
    QNetworkReply* reply = (QNetworkReply*)sender();
    reply->deleteLater();

    QByteArray data = reply->readAll();
    qDebug() << data;

    QDomDocument xml;
    xml.setContent( data );

    Xspf xspf( xml.documentElement() );
    QList<Track> tracks( xspf.tracks() );
    if (tracks.isEmpty())
    {
        // sometimes the recs service craps out and gives us a blank playlist

        if (!tryAgain())
        {
            // an empty playlist is a bug, if there is no content
            // NotEnoughContent should have been returned with the WsReply
            emit error( Ws::MalformedResponse );
        }
    }
    else {
        m_retry_counter = 0;
        m_queue << tracks;
        emit trackAvailable();
    }
}
