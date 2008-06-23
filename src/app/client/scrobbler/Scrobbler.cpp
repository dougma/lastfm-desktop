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

#include "Scrobbler.h"
#include "NowPlaying.h"
#include "ScrobbleCache.h"
#include "ScrobblerHandshake.h"
#include "ScrobblerSubmission.h"


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
    m_submitter = new ScrobblerSubmission( this );
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
            Q_ASSERT( false ); //what aren't you handling?

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

        // submit any queued work
        m_np->ScrobblerPostHttp::request();
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


void
Scrobbler::onHandshakeHeaderReceived( const QHttpResponseHeader& header )
{
    if (header.statusCode() != 200)
    {
        m_handshake->abort(); //TEST
        m_handshake->retry();
    }
}


QString //static
Scrobbler::errorDescription( Scrobbler::Error error )
{
    switch (error)
    {
        case ErrorBadSession: return tr( "Bad session" );
        case ErrorBannedClient: return tr( "Client too old" );
        case ErrorBadAuthorisation: return tr( "Wrong username / password" );
        case ErrorBadTime: return tr( "Wrong timezone" );
        case ThreeHardFailures: return tr( "Could not reach server" );
        case ErrorNotInitialized: return tr( "Contacting Last.fm" );
        case NoError: return "OK";
    }

    // Visual studio is ghae
    return "Unknown";
}
