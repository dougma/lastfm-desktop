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

#include "Xspf.h"
#include "../ws/WsReply.h"
#include "../ws/WsDomElement.h"
#include <QUrl>


lastfm::Xspf::Xspf( const QDomElement& playlist_node, Track::Source src )
{
    try
    {
        WsDomElement e( playlist_node );
        
        m_title = e.optional( "title" ).text();
            
        //FIXME should we use UnicornUtils::urlDecode()?
        //The title is url encoded, has + instead of space characters 
        //and has a + at the begining. So it needs cleaning up:
        m_title.replace( '+', ' ' );
        m_title = QUrl::fromPercentEncoding( m_title.toAscii());
        m_title = m_title.trimmed();
        
        foreach (WsDomElement e, e.optional( "trackList" ).children( "track" ))
        {
            MutableTrack t;
            try
            {
                t.setUrl( e.optional( "location" ).text() );
                t.setExtra( "trackauth", e.optional( "extension" ).optional( "trackauth" ).text() );
                t.setTitle( e.optional( "title" ).text() );
                t.setArtist( e.optional( "creator" ).text() );
                t.setAlbum( e.optional( "album" ).text() );
                t.setDuration( e.optional( "duration" ).text().toInt() / 1000 );
                t.setSource( src );
            }
            catch (std::runtime_error& exception)
            {
                qWarning() << exception.what() << e;
            }
            
            m_tracks += t; // outside since location is enough basically
        }
    }
    catch (std::runtime_error& e)
    {
        qWarning() << e.what();
    }
}
