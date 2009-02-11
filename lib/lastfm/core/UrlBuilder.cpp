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

#include "UrlBuilder.h"
#include "CoreLocale.h"
#include "CoreSettings.h"
#include <QRegExp>
#include <QStringList>


QUrl
lastfm::UrlBuilder::url() const
{
    QUrl url;
    url.setScheme( "http" );
    url.setHost( hostForLocale( CoreSettings().locale() ) );
    url.setEncodedPath( path );
    return url;
}


QByteArray //static
lastfm::UrlBuilder::encode( QString s )
{
    foreach (QChar c, QList<QChar>() << '&' << '/' << ';' << '+' << '#' << '%')
        if (s.contains( c ))
            // the middle step may seem odd but this is what the site does
            // eg. search for the exact string "Radiohead 2 + 2 = 5"
            return QUrl::toPercentEncoding( s ).replace( "%20", "+" ).toPercentEncoding( "", "+" );;

    return QUrl::toPercentEncoding( s.replace( ' ', '+' ), "+" );
}


QString //static
lastfm::UrlBuilder::hostForLocale( const CoreLocale& locale )
{
	QString const code = locale.code();
	
    if (code == "en") return "www.last.fm"; //first as optimisation
    if (code == "pt") return "www.lastfm.com.br";
    if (code == "tr") return "www.lastfm.com.tr";
    if (code == "cn") return "cn.last.fm";
    if (code == "sv") return "www.lastfm.se";
	
    QStringList const simple_hosts = QStringList()
		<< "fr" << "it" << "de" << "es" << "pl"
		<< "ru" << "jp" << "se";
	
    if (simple_hosts.contains( code ))
        return "www.lastfm." + code;
	
    // else default to english site
    return "www.last.fm";
}


QUrl //static
lastfm::UrlBuilder::localize( QUrl url)
{
	url.setHost( url.host().replace( QRegExp("^(www.)?last.fm"), hostForLocale( CoreSettings().locale() ) ) );
	return url;
}


QUrl //static
lastfm::UrlBuilder::mobilize( QUrl url )
{
	url.setHost( url.host().replace( QRegExp("^(www.)?last"), "m.last" ) );
	return url;
}
