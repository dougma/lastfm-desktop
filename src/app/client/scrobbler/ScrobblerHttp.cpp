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

#include "ScrobblerHttp.h"
#include "Scrobbler.h"
#include <QDebug>
#include <QTimer>


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
ScrobblerPostHttp::setUrl( const QUrl& url )
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


void
ScrobblerPostHttp::request()
{
    if (m_data.isEmpty() || manager()->session().isEmpty())
        return;

    QHttpRequestHeader header( "POST", m_path );
    header.setValue( "Host", host() ); //Qt makes me LOL today
    header.setContentType( "application/x-www-form-urlencoded" );

    qDebug() << "HTTP POST" << host() + m_path << m_data;

    m_id = QHttp::request( header, m_data );
}