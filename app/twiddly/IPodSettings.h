/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole, Erik Jaelevik, 
        Christian Muehlhaeuser

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
#include "lib/unicorn/UnicornSettings.h"

enum IPodType { IPodUnknownType, IPodAutomaticType, IPodManualType };


/** Used by app/client so don't add anything specific to Twiddly 
  * <max@last.fm>
  */
class IPodSettings
{
    class Settings : public unicorn::GlobalSettings
    {
    public:
        Settings( IPodSettings const * const s )
        {
            beginGroup( "device/" + s->m_uid );
        }
    };

	friend class IPod;
	friend class IPodScrobbleCache;
    friend class Settings;

    QString m_uid;
    
    IPodSettings( const QString& uid ) : m_uid( uid )
    {}
    
public:
    IPodType type() const { return (IPodType)Settings( this ).value( "type" ).toInt(); }
    void setType( IPodType t ) { Settings( this ).setValue( "type", (int)t ); }
    
    QDateTime lastSync() const { return Settings( this ).value( "LastSync" ).toDateTime(); }
    void setLastSync( const QDateTime& time ) { Settings( this ).setValue( "LastSync", time ); }
};
