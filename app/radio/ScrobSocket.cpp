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
#include "ScrobSocket.h"
#include <QThread>
#include <QHostAddress>
#include <QUrl>
#include <QTextStream>

#ifdef WIN32
    #include "common/c++/win/scrobSubPipeName.cpp"
#endif


ScrobSocket::ScrobSocket( const QString& clientId, QObject* parent ) 
: QLocalSocket( parent )
, m_bInConnect( false )
, m_clientId( clientId )
{
    connect( this, SIGNAL(readyRead()), SLOT(onReadyRead()) );    
    connect( this, SIGNAL(error( QLocalSocket::LocalSocketError )), SLOT(onError( QLocalSocket::LocalSocketError )) );
    connect( this, SIGNAL(connected()), SLOT(onConnected()) );
    connect( this, SIGNAL(disconnected()), SLOT(onDisconnected()) );
    transmit( "INIT c=" + clientId + "\n" );
}


ScrobSocket::~ScrobSocket()
{
    if (!m_track.isNull()) stop();
}


void
ScrobSocket::transmit( const QString& data )
{
    m_msgQueue.enqueue( data );
    qDebug() << "Connection state == " << state();
    if( state() == QAbstractSocket::UnconnectedState ) {
        doConnect();
    }
}


void 
ScrobSocket::onConnected()
{
    if( !m_msgQueue.empty() )
    {
        qDebug() << m_msgQueue.head().trimmed();
        write( m_msgQueue.takeFirst().toUtf8());
    }
}

void
ScrobSocket::doConnect()
{
    if (!m_bInConnect) {
        #ifdef WIN32
            std::string s;
            DWORD r = scrobSubPipeName( &s );
            if (r != 0) throw std::runtime_error( formatWin32Error( r ) );
            QString const name = QString::fromStdString( s );
        #else
            QString const name = "lastfm_scrobsub";
        #endif

        // avoid stack-overflow connect/disconnect loop with m_bInConnect
        m_bInConnect = true;
        connectToServer( name );
        m_bInConnect = false;
    }
}

void 
ScrobSocket::onDisconnected()
{
    if( !m_msgQueue.empty())
        doConnect();
}


void
ScrobSocket::onError( QLocalSocket::LocalSocketError error )
{
    switch (error)
    {
        case SocketTimeoutError:
            // TODO look, really we should store at least one start message forever
            // then if last time we didn't connect and this time it's a pause we 
            // send the start first
            m_msgQueue.clear();
            break;
        
        case PeerClosedError:
            // expected
            break;
        
        case ConnectionRefusedError: // happens if client isn't running
            break;

        default: // may as well
            qDebug() << lastfm::qMetaEnumString<QAbstractSocket>( error, "SocketError" );
    }
}


static inline QString encodeAmp( QString data )
{ 
    return data.replace( '&', "&&" );
}


void
ScrobSocket::start( const Track& t )
{
    m_track = t;
    transmit( "START c=" + m_clientId + "&"
                    "a=" + encodeAmp( t.artist() ) + "&"
                    "t=" + encodeAmp( t.title() ) + "&"
                    "b=" + encodeAmp( t.album() ) + "&"     // todo: and when album.isNull?
                    "l=" + QString::number( t.duration() ) + "&"
                    "p=" + encodeAmp( t.url().path() ) + '\n' );
}


void
ScrobSocket::pause()
{
    transmit( "PAUSE c=" + m_clientId + "\n" );
}


void
ScrobSocket::resume()
{
    transmit( "RESUME c=" + m_clientId + "\n" );
}


void
ScrobSocket::stop()
{
    transmit( "STOP c=" + m_clientId + "\n" );
}


void
ScrobSocket::onReadyRead()
{
    QByteArray bytes = readAll();
    if (bytes != "OK\n") 
        qWarning() << bytes.trimmed();
    disconnectFromServer();
}
