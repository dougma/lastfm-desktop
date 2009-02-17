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
    connect( this, SIGNAL(error( QAbstractSocket::SocketError )), SLOT(onError( QAbstractSocket::SocketError )) );
    transmit( "INIT c=bof\n" );
}


ScrobSocket::~ScrobSocket()
{
    stop();
}


void
ScrobSocket::transmit( const QString& data )
{
    qDebug() << data.trimmed();
    connectToHost( QHostAddress::LocalHost, kDefaultPort );
    if (waitForConnected( 5000 )) write( data.toUtf8() ); //lol blocking
    disconnect();
}


void
ScrobSocket::onError( SocketError error )
{
    qDebug() << lastfm::qMetaEnumString<QAbstractSocket>( error, "SocketError" );
    
    switch (error)
    {
        case RemoteHostClosedError:
            // expected
            break;
        
        case ConnectionRefusedError:
        default: // may as well
            //if (portstep()) write( m_written );
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
