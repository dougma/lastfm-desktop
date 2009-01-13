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

#include "ScrobblerHttp.h"
#include "Scrobbler.h"
#include <QDebug>
#include <QTimer>

#include "mock/PrivateMockScrobblerHttp.h"


ScrobblerHttp::ScrobblerHttp( QObject* parent )
             : QHttp( parent ),
               m_id( -1 )
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
    qDebug() << "MOC -> ScrobblerHttp::onRequestFinished " << host();
    emit done( PrivateMockScrobblerHttp::s_instance->readResponse() );
}


void
ScrobblerPostHttp::setUrl( const QUrl& url )
{
    m_path = url.path();
    setHost( url.host(), url.port() );
}


void 
ScrobblerHttp::retry()
{
    /*int const i = m_retry_timer->interval();
    if (i < 120 * 60 * 1000)
        m_retry_timer->setInterval( i * 2 );

    qDebug() << "Retry in " << m_retry_timer->interval() / 1000 << " seconds";

    m_retry_timer->start();*/
}


void
ScrobblerHttp::resetRetryTimer()
{
    //m_retry_timer->setInterval( 60 * 1000 );
}


void
ScrobblerPostHttp::request()
{
    qDebug() << "MOCK -> ScrobblerPostHttp::request() " << host();
    if ( m_data.isEmpty() )
        return;

    qDebug() << "m_data " << m_data;
    
    if ( host() == "post.audioscrobbler.com" )
        PrivateMockScrobblerHttp::s_instance->nowPlaying( m_data );
    else if ( host() == "87.117.229.205" )
        PrivateMockScrobblerHttp::s_instance->submitTrack( m_data );
    
    emit ( requestFinished( 0, false ) );
}

int
ScrobblerHttp::get( QString url )
{
    qDebug() << "MOCK -> ScrobblerPostHttp::get() " << host();
    
    PrivateMockScrobblerHttp::s_instance->handshake( url );
    
    return 0;
}

