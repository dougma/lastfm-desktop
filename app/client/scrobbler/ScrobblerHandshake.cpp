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

#include "ScrobblerHandshake.h"
#include "version.h"
#include "lib/unicorn/UnicornCommon.h"
#include <QDateTime>
#include <QDebug>


ScrobblerHandshake::ScrobblerHandshake( const QString& username, const QString& session )
                  : m_username( username ),
                    m_session( session )
{
    setHost( "post.audioscrobbler.com" );
    request();
}


void
ScrobblerHandshake::request()
{
    QString timestamp = QString::number( QDateTime::currentDateTime().toTime_t() );
    QString auth_token = Unicorn::md5( (API_SECRET + timestamp).toUtf8() );

    QString query_string = QString() +
        "?hs=true" +
        "&p=1.2.1" + //protocol version
        "&c=" + THREE_LETTER_SCROBBLE_CODE
        "&v=" + VERSION +
        "&u=" + QString(QUrl::toPercentEncoding( m_username )) +
        "&t=" + timestamp +
        "&a=" + auth_token +
        "&api_key=" API_KEY +
        "&sk=" + m_session;

    m_id = get( '/' + query_string );

    qDebug() << "HTTP GET" << host() + '/' + query_string;
}
