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

#include "IPodScrobbleCache.h"
#include "app/twiddly/IPodSettings.h"
#include <QFile>


IPodScrobbleCache::IPodScrobbleCache( const QString& path ) 
{
    m_path = path;
    QDomDocument xml;
    read( xml );
    m_uid = xml.documentElement().attribute( "uid" );

    QMutableListIterator<Track> i( m_tracks );
    while (i.hasNext())
        if (!Scrobble( i.next() ).isValid())
            i.remove();
}


bool
IPodScrobbleCache::insane() const
{   
    QDateTime last_sync = IPodSettings( m_uid ).lastSync();

    foreach (Track t, m_tracks)
        if (t.timestamp() < last_sync)
            return true;

    return false;
}


void
IPodScrobbleCache::remove()
{
    QFile::remove( m_path );
}


#if 0
bool
IPodScrobbler::scrobble()
{
    if (m_tracks.isEmpty())
        return true;
    
    if (m_confirm)
    {
        static bool b = false;
        if (b) 
            return false;
        
        // if a second request for a dialog comes in, ignore it, since this is
        // a real edge case, and confirm dialogs are off by default, and the 
        // the scrobbles will not be lost, but presented next time the user
        // scrobbles their iPod
        b = true;
        MediaDeviceConfirmDialog d( m_tracks, m_username, m_parent );
        int const r = d.exec();
        b = false;
        
        if ( r != QDialog::Accepted )
            return true;
        
        m_tracks = d.tracks();
    }
    
    // copy tracks playCount() times so we submit the correct number of plays
    QList<TrackInfo> tracksToScrobble;
    foreach (TrackInfo t, m_tracks)
    {
        int const playCount = t.playCount();
        t.setPlayCount( 1 );
        for (int y = 0; y < playCount; ++y)
            tracksToScrobble += t;
        
        // will add each track (but only once, not playCount() times!) to
        // the recently listened tracks
        emit The::app().event( Event::MediaDeviceTrackScrobbled, QVariant::fromValue( t ) );
    }
    
    qDebug() << "TRACKS:" << tracksToScrobble.count();
    
    ScrobbleCache cache( m_username );
    cache.append( tracksToScrobble );
    
    if (The::scrobbler().canScrobble( m_username ))
    {
        The::scrobbler().scrobble( cache );
    }
    else {
        Scrobbler::Init init;
        init.username = m_username;
        init.password = The::settings().user( m_username ).password();
        init.client_version = The::settings().version();
        
        The::scrobbler().handshake( init );
        
        // the cache is automatically scrobbled once the scrobbler is handshaken
    }
    
    return true;
}
#endif