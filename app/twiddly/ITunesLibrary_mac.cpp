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
#include "ITunesLibrary.h"
#include "IPodScrobble.h"
#include "lib/unicorn/mac/AppleScript.h"
#include <QDateTime>
#include <QFileInfo>
#include <QStringList>


ITunesLibrary::ITunesLibrary( const QString& pid, bool )
        : m_currentIndex( 0 )
{
    QString source;
    if (!pid.isEmpty())
        source = "tell first source whose persistent ID is \"" + pid + "\" to ";

    AppleScript script( "tell application \"iTunes\" to "
                        + source + 
                        "get {persistent ID, played count } of every track in library playlist 1" );
    QString out = script.exec();
    
    if (out.isEmpty())
        throw "Failed to get iTunes library contents";
    
    out.chop( 2 ); //remove encapsulating }}
    out.remove( 0, 2 ); //remove encapsulating {{
    
    QStringList parts = out.split( ", " );
    const int N = parts.count() / 2;
    parts[N-1].chop( 1 ); //remove encapsulating }
    parts[N].remove( 0, 1 ); //remove encapsulating {
    
    for (int i = 0; i < N; ++i)
    {
        QString const uid = parts[i].remove( '"' );
        QString const plays = parts[N+i];
        
        ITunesLibrary::Track t( uid, plays.toInt() );
        t.m_sourcePersistentId = pid;
        m_tracks += t;
    }
    
    qDebug() << "Found" << m_tracks.count() << "tracks";
}


ITunesLibrary::~ITunesLibrary()
{}


bool
ITunesLibrary::hasTracks() const
{
    return m_currentIndex < (uint)m_tracks.count();
}


ITunesLibrary::Track
ITunesLibrary::nextTrack()
{
    return m_tracks.value( m_currentIndex++ );
}


int 
ITunesLibrary::trackCount() const
{
    return m_tracks.count();
}


static QDateTime
qDateTimeFromScriptString( const QString& s )
{
    QList<int> parts;
    foreach (const QString& part, s.split( ':' ))
        parts += part.toInt();

    if (parts.count() < 4)
        return QDateTime();

    QDate d( parts[0], parts[1], parts[2] );

    QTime t;
    t = t.addSecs( parts[3] );

    return QDateTime( d, t );
}


::Track
ITunesLibrary::Track::lastfmTrack() const
{
    // NOTE we only what data we require for scrobbling, though we could fill in
    // more of the Track object
    IPodScrobble t;
    t.setSource( ::Track::MediaDevice );

    QString source;
    if (!m_sourcePersistentId.isEmpty())
        source = "tell first source whose persistent ID is '" + m_sourcePersistentId + "' to ";

    // TODO compile once and pass pid as argument
    // NOTE trust me, the code is ugly, but doesn't work any other way, don't clean it up!
    AppleScript script;
    script << "tell application 'iTunes'"
        <<     source + "set lib to library playlist 1"
        <<     "set t to first track of lib whose persistent ID is '" + persistentId() + "'"
        <<     "set d to played date of t"
        << "end tell"

        << "try"
        <<     "set d to (year of d) & ':' & "
        "((month of d) as integer) & ':' & "
        "(day of d) & ':' & "
        "(time of d)"
        << "end try"

        << "tell application 'iTunes' to tell t"
        <<     "set l to location"
        <<     "try" << "set l to POSIX path of l" << "end try"
        <<     "return artist & '\n' & name & '\n' & (duration as integer)  & '\n' & album & '\n' & played count  & '\n' & d & '\n' & l"
        << "end tell";

    QString out = script.exec();
    QTextStream s( &out, QIODevice::ReadOnly );

    t.setArtist( s.readLine() );
    t.setTitle( s.readLine() );
    t.setDuration( (uint) s.readLine().toFloat() );
    t.setAlbum( s.readLine() );
    t.setPlayCount( s.readLine().toInt() );
    t.setTimestamp( qDateTimeFromScriptString( s.readLine() ) );

    QFileInfo fileinfo( s.readLine() );
    t.setUrl( QUrl::fromLocalFile( fileinfo.absolutePath() ) );

    return t;
}
