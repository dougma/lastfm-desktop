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

#include "ScrobbleShepherd.h"
#include "version.h"

#include "lib/unicorn/Logger.h"
#include "lib/unicorn/UnicornCommon.h"
#include "lib/moose/MooseCommon.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTimer>
#include <QUrl>

// NOTE use your own! Ask russ@last.fm for a code.
#define THREE_LETTER_SCROBBLE_CODE "ass"


///////////////////////////////////////////////////////////////////////////////>
Scrobbler::Scrobbler( const QString& username, const QString& password )
        : m_username( username ),
          m_password( password ),
          m_handshake( 0 ), 
          m_np( 0 ), 
          m_submitter( 0 ),
          m_hard_failures( 0 )
{
    handshake();
}


Scrobbler::~Scrobbler()
{
    delete m_handshake;
    delete m_np;
    delete m_submitter;
}


void
Scrobbler::handshake()
{
    m_session = "";
    m_hard_failures = 0;

    delete m_handshake;
    delete m_np;
    delete m_submitter;
    
    m_handshake = new ScrobblerHandshake( m_username, m_password );
    connect( m_handshake, SIGNAL(done( QString )), SLOT(onHandshakeReturn( QString )) );
    connect( m_handshake, SIGNAL(responseHeaderReceived( QHttpResponseHeader )), SLOT(onHandshakeHeaderReceived( QHttpResponseHeader )) );
    m_np = new NowPlaying( this );
    connect( m_np, SIGNAL(done( QString )), SLOT(onNowPlayingReturn( QString )) );
    m_submitter = new ScrobblerSubmitter( this );
    connect( m_submitter, SIGNAL(done( QString )), SLOT(onSubmissionReturn( QString )) );
}


void
Scrobbler::submit()
{
    m_submitter->request();
}


void
Scrobbler::nowPlaying( const TrackInfo& track )
{
    m_np->request( track );
}


void
Scrobbler::onError( Scrobbler::Error code )
{
    Q_DEBUG_BLOCK << code; //TODO error text

    switch (code)
    {
        case Scrobbler::ErrorBannedClient:
        case Scrobbler::ErrorBadAuthorisation:
        case Scrobbler::ErrorBadTime:
            //TEST that np and submit don't fuck everything up, ie you may need to abort
            break;

        default:
            Q_ASSERT( false );

        case Scrobbler::ThreeHardFailures:
        case Scrobbler::ErrorBadSession:
            handshake();
            break;
    }

    emit status( code );
}


void
Scrobbler::onHandshakeReturn( const QString& result ) //TODO trim before passing here
{
    Q_DEBUG_BLOCK << result.trimmed();
    QStringList const results = result.split( '\n' );
    QString const code = results.value( 0 );

    if (code == "OK" && results.count() >= 4)
    {
        m_session = results[1];
        m_np->setUrl( results[2] );
        m_submitter->setUrl( results[3] );

        emit status( Scrobbler::Handshaken, m_username );

        m_np->ScrobblerHttpPostRequest::request();
        m_submitter->request();
    }
    else if (code == "BANNED")
    {
        onError( Scrobbler::ErrorBannedClient );
    }
    else if (code == "BADAUTH")
    {
        onError( Scrobbler::ErrorBadAuthorisation );
    }
    else if (code == "BADTIME")
    {
        onError( Scrobbler::ErrorBadTime );
    }
    else
        m_handshake->retry(); //TODO increasing time up to 2 hours
}


void
Scrobbler::onNowPlayingReturn( const QString& result )
{
    Q_DEBUG_BLOCK << result.trimmed();
    QString const code = result.split( '\n' ).value( 0 );

    if (code == "OK")
    {
        // yay
    }
    else if (code == "BADSESSION")
    {
        onError( Scrobbler::ErrorBadSession );
    }

    // we don't hard fail for what would be the else case here, because:
    //  1) if just np is failing and subs are still ok, then we should submit!
    //  2) np is minimal load relative to subs, so who cares
    //  3) if everything is broken, subs will cause hard failure too, so everything is still fine

    // TODO retry if server replies with busy, at least
    // TODO you need a lot more error handling for the scrobblerHttp returns "" case
}


void
Scrobbler::onSubmissionReturn( const QString& result )
{
    Q_DEBUG_BLOCK << result.trimmed();
    QString const code = result.split( '\n' ).value( 0 );

    if (code == "OK")
    {
        ScrobbleCache( m_username ).remove( m_submitter->batch() );
        m_submitter->clearBatch();

        //TODO emit status of submissions
        m_hard_failures = 0;
        m_submitter->resetRetryTimer();

        // try to submit another batch;
        m_submitter->request();
    }
    else if (code == "BADSESSION")
    {
        onError( Scrobbler::ErrorBadSession );
    }
    else if (++m_hard_failures >= 3)
    {
        onError( Scrobbler::ThreeHardFailures );
    }
    else
        m_submitter->retry();
}


///////////////////////////////////////////////////////////////////////////////>
//FIXME! Doesn't suit multi user feel of this class as it is currently
static QList<TrackInfo> g_tracks;


ScrobbleCache::ScrobbleCache() : m_tracks( g_tracks )
{}


ScrobbleCache::ScrobbleCache( const QString& username ) : m_tracks( g_tracks )
{
    Q_ASSERT( username.length() );

    m_path = MooseUtils::savePath( username + "_submissions.xml" );
    m_username = username;
}


void
ScrobbleCache::read()
{
    m_tracks.clear();

    QFile file( m_path );
    file.open( QFile::Text | QFile::ReadOnly );
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );

    QDomDocument xml;
    xml.setContent( stream.readAll() );

    for (QDomNode n = xml.documentElement().firstChild(); !n.isNull(); n = n.nextSibling())
        if (n.nodeName() == "item")
            m_tracks += TrackInfo( n.toElement() );
}


void
ScrobbleCache::write()
{
    if (m_tracks.isEmpty())
    {
        QFile::remove( m_path );
        qDebug() << m_path << "is now empty";
    }
    else {
        QDomDocument xml;
        QDomElement e = xml.createElement( "submissions" );
        e.setAttribute( "product", "Audioscrobbler" );
        e.setAttribute( "version", "1.2" );

        foreach (TrackInfo i, m_tracks)
            e.appendChild( i.toDomElement( xml ) );

        xml.appendChild( e );

        QFile file( m_path );
        file.open( QIODevice::WriteOnly | QIODevice::Text );

        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << "<?xml version='1.0' encoding='utf-8'?>\n";
        stream << xml.toString( 2 );

        qDebug() << "Wrote" << m_tracks.count() << "tracks to" << m_path;
    }
}


void
ScrobbleCache::append( const TrackInfo& track )
{
    append( QList<TrackInfo>() << track );
}


void
ScrobbleCache::append( const QList<TrackInfo>& tracks )
{
    foreach (const TrackInfo& track, tracks)
    {
        // we can't scrobble empties
        if (track.isEmpty()) {
            LOGL( 3, "Will not cache an empty track" );
            continue;
        }

        if (QDateTime::fromTime_t(track.timeStamp()) < QDateTime::fromString( "2003-01-01", Qt::ISODate ))
        {
            LOGL( 3, "Won't scrobble track from before the date Audioscrobbler project was founded!" );
            continue;
        }

        m_tracks += track;
    }
    write();
}


int
ScrobbleCache::remove( const QList<TrackInfo>& toremove )
{
    qDebug() << m_tracks.count() << "track." << toremove.count() << "to remove";

    QMutableListIterator<TrackInfo> i( m_tracks );
    while (i.hasNext()) {
        TrackInfo t = i.next();
        for (int x = 0; x < toremove.count(); ++x)
            if (toremove[x] == t)
            {
                qDebug() << "Removing" << t.toString();
                i.remove();
            }
    }

    qDebug() << m_tracks.count();

    write();

    // yes we return # remaining, rather # removed, but this is an internal 
    // function and the behaviour is documented so it's alright imo --mxcl
    return m_tracks.count();
}

#if 0
QString
Scrobbler::errorDescription( Scrobbler::Error error )
{
    switch ( error )
    {
        case Scrobbler::ErrorBadSession:
            return tr( "Bad session" );

        case Scrobbler::ErrorBannedClient:
            return tr( "Client too old" );

        case Scrobbler::ErrorBadAuthorisation:
            return tr( "Wrong username / password" );

        case Scrobbler::ErrorBadTime:
            return tr( "Wrong timezone" );

        case Scrobbler::ErrorNotInitialized:
            return tr( "Could not reach server" );

        default:
            return "OK";
    }
}
#endif


///////////////////////////////////////////////////////////////////////////////>


///////////////////////////////////////////////////////////////////////////////>
ScrobblerHttp::ScrobblerHttp( QObject* parent )
            : QHttp( parent ),
              m_id( 0 )
{
    m_retry_timer = new QTimer( this );
    m_retry_timer->setSingleShot( true );
    connect( m_retry_timer, SIGNAL(timeout()), SLOT(request()) );
    resetRetryTimer();

    connect( this, SIGNAL(requestFinished( int, bool )), SLOT(onRequestFinished( int, bool )) );
}


void
ScrobblerHttp::onRequestFinished( int id, bool error )
{
    if (error && this->error() == QHttp::Aborted)
        return;

    if (id == m_id)
    {
        if (error)
            qDebug() << this;

        m_id = 0;
        emit done( error ? QString() : QString( readAll() ) );
    }
}


void
ScrobblerHttpPostRequest::setUrl( const QUrl& url )
{
    m_path = url.path();
    setHost( url.host(), url.port() );
}


void 
ScrobblerHttp::retry()
{
    int const i = m_retry_timer->interval();
    if (i < 120 * 60 * 1000)
        m_retry_timer->setInterval( i * 2 );

    qDebug() << "Retry in " << m_retry_timer->interval() / 1000 << " seconds";

    m_retry_timer->start();
}


void
ScrobblerHttp::resetRetryTimer()
{
    m_retry_timer->setInterval( 60 * 1000 );
}


ScrobblerHandshake::ScrobblerHandshake( const QString& username, const QString& password )
                   : m_username( username ),
                     m_password( password )
{
    setHost( "post.audioscrobbler.com" );
    request();
}


void
ScrobblerHandshake::request()
{
    QString timestamp = QString::number( QDateTime::currentDateTime().toTime_t() );
    QString auth_token = Unicorn::md5( (m_password + timestamp).toUtf8() );

    QString query_string = QString() +
                            "?hs=true" +
                            "&p=1.2" + //protocol version
                            "&c=" + THREE_LETTER_SCROBBLE_CODE
                            "&v=" + VERSION +
                            "&u=" + QString(QUrl::toPercentEncoding( m_username )) +
                            "&t=" + timestamp +
                            "&a=" + auth_token;

    m_id = get( '/' + query_string );

    qDebug() << "HTTP GET" << host() + '/' + query_string;
}


void
Scrobbler::onHandshakeHeaderReceived( const QHttpResponseHeader& header )
{
    if (header.statusCode() != 200)
    {
        abort(); //TEST
        m_handshake->retry();
    }
}

///////////////////////////////////////////////////////////////////////////////>


///////////////////////////////////////////////////////////////////////////////>
void
ScrobblerHttpPostRequest::request()
{
    if (m_data.isEmpty() || manager()->session().isEmpty())
        return;

    QHttpRequestHeader header( "POST", m_path );
    header.setValue( "Host", host() ); //Qt makes me LOL today
    header.setContentType( "application/x-www-form-urlencoded" );

    qDebug() << "HTTP POST" << host() + m_path << m_data;

    m_id = QHttp::request( header, m_data );
}


NowPlaying::NowPlaying( Scrobbler* parent )
          : ScrobblerHttpPostRequest( parent )
{
    m_timer = new QTimer( this );
    m_timer->setInterval( 5000 );
    m_timer->setSingleShot( true );
    connect( m_timer, SIGNAL(timeout()), SLOT(request()) );
}


void
NowPlaying::request( const TrackInfo& track )
{
    Q_ASSERT( session().size() );

    if (track.isEmpty()) {
        LOGL( 3, "Won't perform np request for an empty track" );
        return;
    }

    #define e( x ) QUrl::toPercentEncoding( x )
    QString data =  "s=" + e(session())
                 + "&a=" + e(track.artist())
                 + "&t=" + e(track.track())
                 + "&b=" + e(track.album())
                 + "&l=" + QString::number( track.duration() )
                 + "&n=" + QString::number( track.trackNumber() )
                 + "&m=" + e(track.mbId());
    #undef e

    m_data = data.toUtf8();
    m_timer->start();
}


void
ScrobblerSubmitter::request()
{
    //TODO if (!canSubmit()) return;
    if (m_batch.size()) return;

    ScrobbleCache cache( manager()->username() );
    QList<TrackInfo> tracks = cache.tracks();

    if (tracks.isEmpty())
        return;

    // we need to put the tracks in chronological order or the Scrobbling Service
    // rejects the ones that are later than previously submitted tracks
    // this is only relevent if the cache is greater than 50 in size as then
    // submissions are done in batches, but better safe than sorry
    //TODO sort in the persistent cache
    qSort( tracks.begin(), tracks.end(), TrackInfo::lessThan );
    tracks = tracks.mid( 0, 50 );
    m_batch = tracks;

    Q_ASSERT( session().size() );
    Q_ASSERT( tracks.size() <= 50 );

    //////
    QString data = "s=" + session();
    bool portable = false;
    int n = 0;

    foreach (TrackInfo const i, tracks)
    {
        QString const N = QString::number( n++ );
        #define e( x ) QUrl::toPercentEncoding( x )
        data += "&a[" + N + "]=" + e(i.artist()) +
                "&t[" + N + "]=" + e(i.track()) +
                "&i[" + N + "]=" + QString::number( i.timeStamp() ) +
                "&o[" + N + "]=" + i.sourceString() +
                "&r[" + N + "]=" + i.ratingCharacter() +
                "&l[" + N + "]=" + e(QString::number( i.duration() )) +
                "&b[" + N + "]=" + e(i.album()) +
                "&n[" + N + "]=" + //position in album if known, and we don't generally
                "&m[" + N + "]=" + i.mbId();
        #undef e

        if (i.source() == TrackInfo::MediaDevice)
            portable = true;
    }

    if (portable)
        data += "&portable=1";

    m_data = data.toUtf8();
    ScrobblerHttpPostRequest::request();
};

