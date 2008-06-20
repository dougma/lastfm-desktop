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

#include "Request.h"


enum UserAuthCode
{
    AUTH_OK = 0,
    AUTH_OK_LOWER,
    AUTH_BADUSER,
    AUTH_BADPASS,
    AUTH_ERROR
};


/** @author <max@last.fm>
  * @short Verify with server that a supplied user/pass combo is valid. Password
  *        should be MD5 hashed. */
class UNICORN_DLLEXPORT VerifyUserRequest : public Request
{
    PROP_GET_SET( QString, username, Username );

    PROP_GET( bool, bootstrapAllowed );
    PROP_GET( UserAuthCode, userAuthCode );

    QString m_password;
    QString m_md5;
    QString m_lowered_md5;

public:
    VerifyUserRequest();

    virtual void start();
    virtual void success( QByteArray data );

    void setPassword( const QString& );

    /** user after the request returns, if you use before, result is undefined */
    QString password() const;
};
