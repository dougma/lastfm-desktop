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
 
#ifndef PLAYABLE_MIME_DATA
#define PLAYABLE_MIME_DATA

#include <QMimeData>
#include "lib/lastfm/types/Artist.h"
#include "lib/lastfm/types/Tag.h"
#include "lib/lastfm/types/User.h"


class PlayableMimeData : public QMimeData
{
	Q_OBJECT
public:
	enum Type { ArtistType = 0, TagType, UserType };
	
	static PlayableMimeData* createFromArtist( const Artist& a )
	{
		PlayableMimeData* data = new PlayableMimeData();
		data->setText( a );
		data->setUrls( QList<QUrl>() << a.www() );
		data->setData( "text/x-lfm-entity-type", "Artist" );
		data->m_type = ArtistType;
		return data;
	}
	
	static PlayableMimeData* createFromTag( const Tag& t )
	{
		PlayableMimeData* data = new PlayableMimeData();
		data->setText( t );
		data->setUrls( QList<QUrl>() << t.www() );
		data->setData( "text/x-lfm-entity-type", "Tag" );
		data->m_type = TagType;
		return data;
	}
	
	static PlayableMimeData* createFromUser( const User& u )
	{
		PlayableMimeData* data = new PlayableMimeData();
		data->setText( u );
		data->setUrls( QList<QUrl>() << u.www() );
		data->setData( "text/x-lfm-entity-type", "User" );
		data->m_type = UserType;
		return data;
	}
	
	int type() const
	{
		return m_type;
	}
	
	void setType( const int t )
	{
		m_type = t;
	}
	
private:
	int m_type;
	
};

#endif //PLAYABLE_MIME_DATA