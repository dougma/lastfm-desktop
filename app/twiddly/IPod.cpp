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

#include "IPod.h"
#include "IPodScrobble.h"
#include "ITunesLibrary.h"
#include "PlayCountsDatabase.h"
#include "common/qt/msleep.cpp"
#include "plugins/iTunes/ITunesExceptions.h"
#include "lib/lastfm/core/CoreDir.h"
#include "lib/lastfm/core/mac/AppleScript.h"
#include "lib/lastfm/types/Track.h"
#include <QtCore>
#include <QtXml>
#include <iostream>


IPod* //static
IPod::fromCommandLineArguments( const QStringList& args )
{
#if 0
    // handy for debug
    QStringList myArgs = QString( "--device ipod --connection usb --pid 4611 --vid 1452 --serial 000000C8B035" ).split( ' ' );
    //QStringList myArgs = QString( "--bootstrap" ).split( ' ' );
    #define args myArgs
#endif

    QMap<QString, QString> map;
    QListIterator<QString> i( args );
    while (i.hasNext())
    {
        QString arg = i.next();
        if (!arg.startsWith( "--" ) || !i.hasNext() || i.peekNext().startsWith( "--" ))
            continue;
        arg = arg.mid( 2 );
        map[arg] = i.next();
    }

    IPod* ipod;
    if (args.contains( "--manual" ))
        ipod = new ManualIPod;
    else
        ipod = new AutomaticIPod;

    #define THROW_IF_EMPTY( x ) ipod->x = map[#x]; if (ipod->x.isEmpty()) throw "Could not resolve argument: --" #x;

    THROW_IF_EMPTY( device );
    THROW_IF_EMPTY( vid );
    THROW_IF_EMPTY( pid );
    THROW_IF_EMPTY( serial );
    
    #undef THROW_IF_EMPTY
    
    return ipod;
}


QString
IPod::scrobbleId() const
{
#ifdef Q_OS_MAC
    const char* os = "Macintosh";
#elif defined Q_OS_WIN
    const char* os = "Windows";
#else
    #error What kind of operating system are you?! \
           I am a home-made one.
#endif

    return vid + '-' + pid + '-' + device + '-' + os;
}


QDir
IPod::saveDir() const 
{
    QDir d = CoreDir::data().filePath( "devices/" + uid() );
    d.mkpath( "." );
    return d;
}


QDomDocument
IPod::ScrobbleList::xml() const
{    
    QDomDocument xml;
    QDomElement root = xml.createElement( "submissions" );
    root.setAttribute( "product", "Twiddly" );

    QListIterator<Track> i( *this );
    while (i.hasNext())
        root.appendChild( i.next().toDomElement( xml ) );
    
    xml.appendChild( root );
    
    return xml;
}


/** iPod plays are determined using our iTunes Plays sqlite database and
  * comparing that with the actual state of the iTunes Library database after
  * an iPod is synced with it
  */
void
IPod::twiddle()
{
    PlayCountsDatabase& db = *playCountsDatabase();
    ITunesLibrary& library = *iTunesLibrary();
    
    db.beginTransaction();

    #ifdef Q_OS_WIN32
        QSet<QString> diffedTrackPaths;
    #endif
    // If creation of the library class failed due to a dialog showing in iTunes
    // or COM not responding for some other reason, hasTracks will just return false
    // and we will quit this run.
    while ( library.hasTracks() )
    {
        try
        {
            ITunesLibrary::Track track = library.nextTrack();

            if ( track.isNull() )
            {
                qWarning() << "Failed to read current iTunes track. Either something went wrong, "
                              "or the track was not found on the disk despite being in the iTunes library.";
                continue;
            }

            #ifdef Q_OS_WIN32
                if( diffedTrackPaths.contains( track.uniqueId() ) )
                {
                    //This is a duplicate entry in the iTunes library.
                    //For sanity this track AND the previous identical track
                    //will be ignored. - This is due to no pids on windows.

                    m_scrobbles.removeAllWithUniqueId( track.uniqueId() );
                    db.remove( track );
                    qDebug() << "Multiple tracks were found with the same unique id / path, this track won't be scrobbled from the iPod:" << track.uniqueId();
                    continue;
                }
                diffedTrackPaths.insert( track.uniqueId() );
            #endif

            QString id = track.uniqueId();

            // We don't know about this track yet, this means either:-
            //   1. The track was added to iTunes since the last sync. thus it is
            //      impossible for it to have been played on the iPod
            //   2. On Windows, the path of the track changed since the last sync.
            //      Since we don't have persistent IDs on Windows we have no way of 
            //      matching up this track up with its previous incarnation. Thus
            //      we don't scrobble it as we have no idea if it was played or not
            //      chances are, it wasn't
            if ( db[id].isNull() )
            {
	            db.insert( track );  // can throw
                continue;
            }

            const int diff = track.playCount() - db[id].playCount(); // can throw
        
            if ( diff > 0 )
            {
                ::Track t = track.lastfmTrack(); // can throw

                if (!t.isNull())
                {
                    IPodScrobble t2( t );
                    t2.setPlayCount( diff );
                    t2.setMediaDeviceId( scrobbleId() );
                    t2.setUniqueId( track.uniqueId() );
                    m_scrobbles += t2;
                    qDebug() << diff << "scrobbles found for" << t;
                }
                else
                {
                    qWarning() << "Couldn't get Track for" << id;
                    
                    // We get here if COM fails to populate the Track for whatever reason.
                    // Therefore we continue and don't let the local db update. That way we
                    // maintain the diff and we should be picking up on it next time twiddly
                    // runs.
                    continue;
                }
            }

            // a worthwhile optimisation since updatePlayCount() is really slow
            // NOTE negative diffs *are* possible
            if ( diff != 0 )
                db.update( track ); // can throw
        }
        catch ( ITunesException& )
        {
            // Carry on...
        }
    }

    db.endTransaction();

    delete &db;
    delete &library;
}


#ifdef Q_OS_MAC
    ManualIPod::ManualIPod()
               : m_pid( firstPid() )
    {}


    QString //static
    ManualIPod::firstPid()
    {
        AppleScript script;
        script << "tell application 'iTunes' to "
                  "return persistent ID of some source whose kind is iPod";
                  
        // wait for iPod to exist, since we're launched early by the plugin
        // and it may not be in iTunes yet, 10 seconds is more than enough
        // without being excessive
        QString pid;
        for( int x = 0; x < 20 && (pid = script.exec()).isEmpty(); ++x )
        {
            Qt::msleep( 500 );
        }
                
        return pid;
    }


#else
    ManualIPod::ManualIPod()
    {}
#endif


#ifdef WIN32
	void
	IPod::ScrobbleList::removeAllWithUniqueId( const QString& uniqueId )
	{
		QList<Track>::Iterator i;
		for( i = begin(); i != end(); ++i ) 
		{
			IPodScrobble s( *i );
			if( s.uniqueId() == uniqueId )
			{
				m_realCount -= s.playCount();
				erase( i );
			}
		}
	}
#endif
