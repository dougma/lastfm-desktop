/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PLAYABLE_MIME_DATA
#define PLAYABLE_MIME_DATA

#include <QMimeData>
#include <lastfm/Artist>
#include <lastfm/Tag>
#include <lastfm/User>
#include "Seed.h"


class PlayableMimeData : public QMimeData
{
	Q_OBJECT
public:
	
	static PlayableMimeData* createFromArtist( const Artist& a )
	{
		PlayableMimeData* data = new PlayableMimeData();
		data->setText( a );
		data->setUrls( QList<QUrl>() << a.www() );
		data->setData( "text/x-lfm-entity-type", "Artist" );
		data->m_type = Seed::ArtistType;
		return data;
	}
	
	static PlayableMimeData* createFromTag( const Tag& t )
	{
		PlayableMimeData* data = new PlayableMimeData();
		data->setText( t );
		data->setUrls( QList<QUrl>() << t.www() );
		data->setData( "text/x-lfm-entity-type", "Tag" );
		data->m_type = Seed::TagType;
		return data;
	}
	
	static PlayableMimeData* createFromUser( const User& u )
	{
		PlayableMimeData* data = new PlayableMimeData();
		data->setText( u );
		data->setUrls( QList<QUrl>() << u.www() );
		data->setData( "text/x-lfm-entity-type", "User" );
		data->m_type = Seed::UserType;
		return data;
	}
    
    static PlayableMimeData* createFromPredefined( const QString& s )
    {
		PlayableMimeData* data = new PlayableMimeData();
		data->setText( s );
		data->setData( "text/x-lfm-entity-type", "Predefined" );
		data->m_type = Seed::PreDefinedType;
		return data;
    }
	
    Seed::Type type() const
	{
		return m_type;
	}
	
	void setType( Seed::Type t )
	{
		m_type = t;
	}
    
    void setRQL( const QString& rql ){ m_rql = rql; }

    QString rql() const{ return m_rql; }
	
private:
    Seed::Type m_type;
    QString m_rql;
	
};

#endif //PLAYABLE_MIME_DATA