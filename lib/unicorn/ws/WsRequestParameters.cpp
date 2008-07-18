/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#include "WsRequestParameters.h"
#include "../UnicornUtils.h"
#include "../UnicornSettings.h"
#include "../../../app/client/version.h" //FIXME
#include <QDebug>


WsRequestParameters::WsRequestParameters()
{
    add( "api_key", API_KEY );

    // the branch prevents an assert in Unicorn::UserQSettings()
    Unicorn::Settings s;
    if (s.username().size())
        add( "sk", s.sessionKey() );
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

    return list << Pair( "api_sig", methodSignature() );
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
    s += API_SECRET;

    return Unicorn::md5( s.toUtf8() );
}
