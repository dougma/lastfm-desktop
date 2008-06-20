/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#include "VerifyUserRequest.h"
#include "../UnicornCommon.h"
#include "../Logger.h"


VerifyUserRequest::VerifyUserRequest()
                  : Request( TypeVerifyUser, "VerifyUser" ),
                    m_userAuthCode( AUTH_ERROR )
{}


void
VerifyUserRequest::setPassword( const QString& password )
{
    m_md5 = Unicorn::md5( password.toUtf8() );
    m_lowered_md5 = Unicorn::md5( password.toLower().toUtf8() );
}


void
VerifyUserRequest::start()
{
    QString const time = QString::number( QDateTime::currentDateTime().toTime_t() );

    QString const path = "/ass/pwcheck.php?"
                    "time=" + QString( QUrl::toPercentEncoding( time ) ) +
                    "&username=" + QString( QUrl::toPercentEncoding( m_username ) ) +
                    "&auth=" + Unicorn::md5( (m_md5 + time).toUtf8() ) +
                    "&auth2=" + Unicorn::md5( (m_lowered_md5 + time).toUtf8() ) +
                    "&defaultplayer="
                    #ifdef WIN32
                        + QString( QUrl::toPercentEncoding( Unicorn::findDefaultPlayer() ) )
                    #endif
                    ;

    get( path );
}


void
VerifyUserRequest::success( QByteArray data )
{
    QString response = data;
    response = response.trimmed();

    //TODO mxcl do in baseclass?
    LOGL( 4, "Verify response: " << response );

    m_bootstrapAllowed = response.contains( "BOOTSTRAP" );

    if (response.contains( "OK2" ))
        m_userAuthCode = AUTH_OK_LOWER;
    else if (response.contains( "OK" ))
        m_userAuthCode = AUTH_OK;
    else if (response.contains( "INVALIDUSER" ))
        m_userAuthCode = AUTH_BADUSER;
    else if (response.contains( "BADPASSWORD" ))
        m_userAuthCode = AUTH_BADPASS;
    else
        m_userAuthCode = AUTH_ERROR;
}


QString
VerifyUserRequest::password() const
{
    return m_userAuthCode == AUTH_OK_LOWER ? m_lowered_md5 : m_md5;
}
