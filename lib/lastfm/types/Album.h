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

#ifndef LASTFM_ALBUM_H
#define LASTFM_ALBUM_H

#include <lastfm/Artist>
#include <lastfm/Mbid>
#include <QString>
#include <QUrl>


namespace lastfm
{
    class LASTFM_TYPES_DLLEXPORT Album
    {
        Mbid m_mbid;
        Artist m_artist;
        QString m_title;

    public:
    	Album()
    	{}

        explicit Album( Mbid mbid ) : m_mbid( mbid )
        {}

        Album( Artist artist, QString title ) : m_artist( artist ), m_title( title )
        {}

    	bool operator==( const Album& that ) const { return m_title == that.m_title && m_artist == that.m_artist; }
    	bool operator!=( const Album& that ) const { return m_title != that.m_title || m_artist != that.m_artist; }
	
        operator QString() const { return title(); }
        QString title() const { return m_title.isEmpty() ? "[unknown]" : m_title; }
        Artist artist() const { return m_artist; }
        Mbid mbid() const { return m_mbid; }

    	/** artist may have been set, since we allow that in the ctor, but should we handle untitled albums? */
    	bool isNull() const { return m_title.isEmpty() && m_mbid.isNull(); }
	
        /** Album.getInfo WebService */
        WsReply* getInfo() const;
        WsReply* share( const class User& recipient, const QString& message = "" );

        /** use Tag::list to get the tag list out of the finished reply */
        WsReply* getTags() const;
        WsReply* getTopTags() const;
        
        /** Last.fm dictates that you may submit at most 10 of these */
        WsReply* addTags( const QStringList& ) const;
    
        /** the Last.fm website url for this album */
    	QUrl www() const;
    };
}

#endif //LASTFM_ALBUM_H
