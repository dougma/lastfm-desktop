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

#ifndef LASTFM_PLAYLIST_H
#define LASTFM_PLAYLIST_H

#include <QString>
#include <QUrl>
#include <lastfm/types/Xspf.h>
class WsReply;


class LASTFM_TYPES_DLLEXPORT Playlist
{
	int m_id;
	
	Playlist() : m_id( -1 )
	{}

public:
	Playlist( int id ) : m_id( id )
	{}
	
	int id() const { return m_id; }

	WsReply* addTrack( const class Track& ) const;
	WsReply* fetch() const;

	static WsReply* create( const QString& title, const QString& description = "" );
	static WsReply* fetch( const QUrl& url );
	
	static Xspf fetch( WsReply* );
};

#endif
