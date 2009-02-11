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

#ifndef LASTFM_URL_BUILDER_H
#define LASTFM_URL_BUILDER_H

#include <lastfm/public.h>
#include <QString>
#include <QUrl>
class CoreLocale;
  

namespace lastfm
{
    /** For building www.last.fm urls. We have special rules for encoding and that */
    class LASTFM_CORE_DLLEXPORT UrlBuilder
    {
        QByteArray path;

    public:
        /** Careful, the base is not encoded at all, we assume it is ASCII!
          * If you need it encoded at all you must use the slash function.
          * eg. UrlBuilder( "user" ).slash( "mxcl" ) ==> http://last.fm/user/mxcl
          */
        UrlBuilder( const QString& base ) : path( '/' + base.toAscii() )
        {}

        UrlBuilder& slash( const QString& path ) { this->path += '/' + encode( path ); return *this; }

        QUrl url() const;

    	/** www.last.fm becomes the local version, eg www.lastfm.de */
    	static QUrl localize( QUrl );
    	/** www.last.fm becomes m.last.fm, localisation is preserved */
    	static QUrl mobilize( QUrl );

    	/** Use this to URL encode any database item (artist, track, album). It
    	  * internally calls UrlEncodeSpecialChars to double encode some special
    	  * symbols according to the same pattern as that used on the website.
    	  *
    	  * &, /, ;, +, #
    	  *
    	  * Use for any urls that go to www.last.fm
    	  * Do not use for ws.audioscrobbler.com
    	  */
    	static QByteArray encode( QString );

    	/** returns eg. www.lastfm.de */
    	static QString hostForLocale( const CoreLocale& );
    };
}

#endif
