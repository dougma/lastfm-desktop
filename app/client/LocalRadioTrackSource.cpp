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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "LocalRadioTrackSource.h"
#include "../clientplugins/ILocalRql.h"
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>


class RqlTrackCallback : public MutableTrack, public ILocalRqlTrackCallback
{

public:
    RqlTrackCallback( Track& t )
        : MutableTrack( t )
    {
    }

    void title( const char* title )
    {
        setTitle( QString::fromUtf8( title ) );
    }

    void album( const char* album )
    {
        setAlbum( QString::fromUtf8( album ) );
    }

    void artist( const char* artist )
    {
        setArtist( QString::fromUtf8( artist ) );
    }

    void url( const char* url )
    {
        setUrl( QString::fromUtf8( url ) );
    }

    void duration( unsigned duration )
    {
        setDuration( duration );
    }
};



LocalRadioTrackSource::LocalRadioTrackSource(ILocalRqlTrackSource* rqlSrc)
: m_rqlSrc(rqlSrc)
{
    Q_ASSERT(rqlSrc);
}

LocalRadioTrackSource::~LocalRadioTrackSource()
{
    if (m_rqlSrc) m_rqlSrc->finished();
}

Track
LocalRadioTrackSource::takeNextTrack()
{
    Track t;
    RqlTrackCallback callback( t );
    if ( m_rqlSrc ) m_rqlSrc->nextTrack( &callback );
    return t;
}

void 
LocalRadioTrackSource::start()
{
    emit trackAvailable();
}