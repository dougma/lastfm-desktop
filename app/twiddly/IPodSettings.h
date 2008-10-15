/***************************************************************************
 *   Copyright 2008 Last.fm Ltd.                                           *
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

#include "lib/lastfm/core/CoreSettings.h"

enum IPodType { IPodUnknownType, IPodAutomaticType, IPodManualType };


/** Used by app/client so don't add anything specific to Twiddly 
  * <max@last.fm>
  */
class IPodSettings
{
    class Settings : public CoreSettings
    {
    public:
        Settings( IPodSettings const * const s )
        {
            beginGroup( "device/iPod" + s->m_uid );
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
