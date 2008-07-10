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

#include "WsRequestParameters.h"
#include "../UnicornCommon.h"
#include "../../../app/client/version.h"


WsRequestParameters::WsRequestParameters( QObject* parent )
                    : QObject( parent )
{}


WsRequestParameters::operator const QList< QPair< QString,QString > >()
{
    QList< QPair < QString, QString > > params;

    add( "api_key", API_KEY );
    add( "api_sig", methodSignature() );

    for( QMap< QString, QString >::iterator iter = m_paramList.begin();
         iter != m_paramList.end();
         iter++ )
    {
        params.push_back( QPair< QString, QString >( iter.key(), iter.value() ) );
    }

    return params;
}


WsRequestParameters&
WsRequestParameters::add( const QString& key, const QString& value )
{
    m_paramList.insert( key, value );
    return *this;
}


QString 
WsRequestParameters::methodSignature()
{
    QString paramString;

    for( QMap<QString, QString>::iterator iter = m_paramList.begin();
         iter != m_paramList.end();
         iter++ )
    {
        paramString += iter.key() + iter.value();
    }

    paramString += API_SECRET;
    return Unicorn::md5( paramString.toUtf8() );
}
