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

#include "ScrobSocket.h"
#include <QThread>
#include <QHostAddress>
#include <QUrl>
#include <QTextStream>

static int const kDefaultPort = 33367;
 

ScrobSocket::ScrobSocket( QObject* parent ) : QTcpSocket( parent )
{
    connect( this, SIGNAL(readyRead()), SLOT(onReadyRead()) );    
    connect( this, SIGNAL(error( QAbstractSocket::SocketError )), SLOT(onError( QAbstractSocket::SocketError )) );
    transmit( "INIT c=bof\n" );
}


ScrobSocket::~ScrobSocket()
{
    if (!m_track.isNull()) stop();
}


void
ScrobSocket::transmit( const QString& data )
{
    qDebug() << data.trimmed();
    connectToHost( QHostAddress::LocalHost, kDefaultPort );
    if (waitForConnected( 5000 )) write( data.toUtf8() ); //lol blocking
}


void
ScrobSocket::onError( SocketError error )
{
    switch (error)
    {
        case RemoteHostClosedError:
            // expected
            break;
        
        default: // may as well
            qDebug() << lastfm::qMetaEnumString<QAbstractSocket>( error, "SocketError" );
        case ConnectionRefusedError: // happens if client isn't running
            break;
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
    transmit( "START c=bof" "&"
                    "a=" + encodeAmp( t.artist() ) + "&"
                    "t=" + encodeAmp( t.title() ) + "&"
                    "b=" + encodeAmp( t.album() ) + "&"
                    "l=" + QString::number( t.duration() ) + "&"
                    "p=" + encodeAmp( t.url().path() ) + '\n' );
}


void
ScrobSocket::pause()
{
    transmit( "PAUSE c=bof\n" );
}


void
ScrobSocket::resume()
{
    transmit( "RESUME c=bof\n" );
}


void
ScrobSocket::stop()
{
    transmit( "STOP c=bof\n" );
}


void
ScrobSocket::onReadyRead()
{
    QByteArray bytes = readAll();
    if (bytes != "OK\n") qWarning() << bytes.trimmed();
    disconnectFromHost();
}
