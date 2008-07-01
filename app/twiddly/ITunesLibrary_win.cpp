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

#include "ITunesLibrary.h"
#include "common/ITunesComWrapper.h"
#include "common/logger.h"

#include <cassert>

#include <QDateTime>
#include <QFileInfo>


using namespace std;


ITunesLibrary::ITunesLibrary( const QString&, bool const iPod ) :
    m_currentIndex( 0 ),
    m_isIPod( iPod ),
    m_trackCount( -1 )
{
    m_com = new ITunesComWrapper();

    // This will throw if it fails
    if ( !iPod )
        m_trackCount = m_com->libraryTrackCount();
    else
    {
        // loop 20 twenty times as sometimes the iPod isn't in iTunes yet and
        // the plugin has called us prematurely
        for( int x = 0; x < 20 && m_trackCount == -1; x++)
        {
            if (x) UnicornUtils::msleep( 500 );
            // the first ipod is used, which isn't perfect, but oh well
            m_trackCount = m_com->iPodLibraryTrackCount();
        }
    }

    LOGL( 3, "Found " << m_trackCount << " tracks in iTunes library" );

}


ITunesLibrary::~ITunesLibrary()
{
    delete m_com;
}
    

bool
ITunesLibrary::hasTracks() const
{
    return (int)m_currentIndex < m_trackCount;
}


int
ITunesLibrary::trackCount() const
{
    return m_trackCount;
}


ITunesLibrary::Track
ITunesLibrary::nextTrack()
{
    // Weird increment is because if this call throws we still want to
    // have increased m_currentIndex. Otherwise we can get stuck in an
    // infinite loop calling this function over and over.
    COM::ITunesTrack it = m_com->track( (++m_currentIndex) - 1 );

    ITunesLibrary::Track t;
    t.d = (m_isIPod)
            ? new IPodLibraryTrackData( it )
            : new ITunesLibraryTrackData( it );
    return t;
}


TrackInfo
ITunesLibrary::Track::trackInfo() const
{
    // TODO: why are we doing this? It will return true for manual tracks as it checks the path.
    if ( isNull() )
        return TrackInfo();

    COM::ITunesTrack i = d->i;

    // These will throw if something goes wrong
    TrackInfo t;
    t.setArtist( QString::fromStdWString( i.artist() ) );
    t.setTrack( QString::fromStdWString( i.track() ) );
    t.setDuration( i.duration() );
    t.setAlbum( QString::fromStdWString( i.album() ) );
    t.setPlayCount( i.playCount() );

    QDateTime stamp = QDateTime::fromString( QString::fromStdWString( i.lastPlayed() ), "yyyy-MM-dd hh:mm:ss" );
    if ( stamp.isValid() )
    {
        uint unixTime = stamp.toTime_t();

        // This is to prevent the spurious scrobble bug that can happen if iTunes is
        // shut during twiddling. The timestamp returned in that case was FFFFFFFF
        // so let's check for that.
        if ( unixTime == 0xFFFFFFFF )
        {
            LOGWL( 2, "Caught a 0xFFFFFFFF timestamp, assume it's the scrobble of spury: " << i.toString() );
            return TrackInfo();
        } 
        
        t.setTimeStamp( unixTime );
    }
    else
    {
        // If we don't have a valid timestamp, set to current time. Should work. We hope.
        LOGWL( 2, "Invalid timestamp, set to current: " << i.toString() );
        t.setTimeStamp( QDateTime::currentDateTime().toTime_t() );
    }

    const QString path = QString::fromStdWString( i.path() );
    QFileInfo fileinfo( path );

    t.setFileName( fileinfo.fileName() );
    t.setPath( fileinfo.absoluteFilePath() );

    t.setSource( TrackInfo::MediaDevice );

    return t;
}
