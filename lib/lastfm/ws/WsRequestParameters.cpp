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

#include "../core/CoreSettings.h"
#include "WsRequestParameters.h"
#include "WsKeys.h"
#include "common/qt/md5.cpp"
#include <QDebug>


WsRequestParameters::WsRequestParameters()
{
    add( "api_key", Ws::ApiKey );
    add( "lang", CoreSettings().locale().iso639() );
    if (Ws::SessionKey.size()) add( "sk", Ws::SessionKey );
}


WsRequestParameters::operator const QList<QPair<QString, QString> >() const
{
    typedef QPair<QString,QString> Pair;

    QList<Pair> list;
    QMapIterator<QString, QString> i( m_map );
    while (i.hasNext()) {
        i.next();
        list += Pair( i.key(), i.value() );
    }

    return ( list << Pair( "api_sig", methodSignature() ) );
}


QString
WsRequestParameters::methodSignature() const
{
    QString s;

    QMapIterator<QString, QString> i( m_map );
    while (i.hasNext()) {
        i.next();
        s += i.key() + i.value();
    }
    s += Ws::SharedSecret;

    return Qt::md5( s.toUtf8() );
}


namespace Ws
{
    QString SessionKey;
    QString Username;
    
    /** we leave these unset as you can't use the webservices without them
      * so lets make the programmer aware of it during testing by crashing */
    const char* SharedSecret;
    const char* ApiKey;
       
    /** if this is found set to "" we conjure ourselves a suitable one */
	const char* UserAgent = "";
}
