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
#include "ITunesListener.h"
#include "../PlayerConnection.h"
#include <lastfm/misc.h>
#include "lib/unicorn/mac/AppleScript.h"
#include <QThread>


struct ITunesConnection : PlayerConnection
{
    ITunesConnection() : PlayerConnection( "osx", "iTunes" )
    {}
    
    void start( const Track& t )
    {
        MutableTrack mt( t );
        mt.setSource( Track::Player );
        mt.setExtra( "playerId", id() );
        mt.stamp();
        handleCommand( CommandStart, t ); 
    }

    void pause() { handleCommand( CommandPause ); }
    void resume() { handleCommand( CommandResume ); }
    void stop() { handleCommand( CommandStop ); }
};


ITunesListener::ITunesListener( QObject* parent )
              : m_connection( 0 )
{
    qRegisterMetaType<Track>("Track");
    connect( parent, SIGNAL(destroyed()), SLOT(deleteLater()) ); //FIXME safe?
}


void
ITunesListener::run()
{
    emit newConnection( m_connection = new ITunesConnection );
    
    setupCurrentTrack();

    CFNotificationCenterAddObserver( CFNotificationCenterGetDistributedCenter(), 
                                    this,
                                    callback, 
                                    CFSTR( "com.apple.iTunes.playerInfo" ), 
                                    NULL, 
                                    CFNotificationSuspensionBehaviorDeliverImmediately );

    

    exec();

	delete m_connection;
}

    
void
ITunesListener::callback( CFNotificationCenterRef, 
                        void* observer, 
                        CFStringRef, 
                        const void*, 
                        CFDictionaryRef info )
{    
    static_cast<ITunesListener*>(observer)->callback( info );
}


/*******************************************************************************
  * code-clarity-class used by callback() */
class ITunesDictionaryHelper
{   
    CFDictionaryRef const m_info; // leave here or risk an initialisation bug
    
public:
    ITunesDictionaryHelper( CFDictionaryRef info ) 
            : m_info( info ), 
              state( getState() )
    {}

    void determineTrackInformation();
        
    QString artist, name, album, path, pid;
    int duration, position;
    ITunesListener::State const state; // *MUST* be after m_info
    
private:
    template <typename T> T
    token( CFStringRef t )
    {
        return (T) CFDictionaryGetValue( m_info, t );
    }

    ITunesListener::State
    getState()
    {
        CFStringRef state = token<CFStringRef>( CFSTR("Player State") );

        #define compare( x ) if (CFStringCompare( state, CFSTR( #x ), 0 ) == kCFCompareEqualTo) return ITunesListener::x
        compare( Playing );
        compare( Paused );
        compare( Stopped );
        #undef compare
        
        return ITunesListener::Unknown;
    }
};


template <> QString
ITunesDictionaryHelper::token<QString>( CFStringRef t )
{
    CFStringRef s = token<CFStringRef>( t );
    return lastfm::CFStringToQString( s );
}   


template <> int
ITunesDictionaryHelper::token<int>( CFStringRef t )
{
    int i = 0;
    CFNumberRef n = token<CFNumberRef>( t );
    if (n) CFNumberGetValue( n, kCFNumberIntType, &i );
    return i;
}


void
ITunesDictionaryHelper::determineTrackInformation()
{
    duration = token<int>( CFSTR("Total Time") ) / 1000;
    artist = token<QString>( CFSTR("Artist") );
    album = token<QString>( CFSTR("Album") );
    name = token<QString>( CFSTR("Name") );
    pid = QString::number( token<int>( CFSTR("PersistentID") ) );

    
    // Get path decoded - iTunes encodes the file location as URL
    CFStringRef location = token<CFStringRef>( CFSTR("Location") );
    QUrl url = QUrl::fromEncoded( lastfm::CFStringToUtf8( location ) );
    path = url.toString().remove( "file://localhost" );
    
    static AppleScript script( "tell application \"iTunes\" to return player position" );
    position = script.exec().toInt();
}
/******************************************************************************/


static inline QString
encodeAmp( QString data ) 
{ 
    return data.replace( '&', "&&" );
}


void
ITunesListener::callback( CFDictionaryRef info )
{
    ITunesDictionaryHelper dict( info );
    State const previousState = m_state;
    m_state = dict.state;
    
    switch (m_state)
    {
        case Paused:
            m_connection->pause();
            break;

        case Stopped:
            m_connection->stop();
            break;
            
        case Playing:
            if (!isMusic())
            {
                qDebug() << "Ignoring current track because it isn't music.";
                break;
            }

            dict.determineTrackInformation();

            // if the track is restarted it has the same pid and a position of 0
            if (m_previousPid == dict.pid && dict.position != 0)
            {
                if (previousState == Paused)
                    m_connection->resume();
                //else the user changed some metadata or the track's rating etc.
            }
            else
            {
                MutableTrack t;
                t.setArtist( dict.artist );
                t.setTitle( dict.name );
                t.setAlbum( dict.album );
                t.setDuration( dict.duration );
                t.setUrl( QUrl::fromLocalFile( dict.path ) );
                m_connection->start( t );
                
                m_previousPid = dict.pid;
            }
            break;

        default:
          qWarning() << "Unknown state.";
          break;
    }
}


bool //static
ITunesListener::isMusic()
{
    const char* code = "with timeout of 1 seconds\n"
                           "tell application \"iTunes\" to if running then return video kind of current track\n"
                       "end timeout";

    static AppleScript script( code ); //compile once
    QString const r = script.exec();

    return r == "none" || r == "music video" || r == "";
}


bool //static
ITunesListener::iTunesIsPlaying()
{
    const char* code = "tell application \"iTunes\" to if running then return player state is playing";
    return AppleScript( code ).exec() == "true";
}


void
ITunesListener::setupCurrentTrack()
{
    qDebug() << "hi";
    
    if (!iTunesIsPlaying() || !isMusic())
        return;
    
    qDebug() << "hi";
    
    #define ENDL " & \"\n\" & "
    const char* code =
        "tell application \"iTunes\" to tell current track\n"
            "try\n"
                "set L to location\n"
                "set L to POSIX path of L\n"
            "on error\n"
                "set L to \"\"\n"
            "end try\n"
            "return artist" ENDL
                   "album" ENDL
                   "name" ENDL
                   "(duration as integer)" ENDL
                   "L" ENDL
                   "persistent ID\n"
        "end tell";
    #undef ENDL
    
    QString output = AppleScript( code ).exec();
    QTextStream s( &output, QIODevice::ReadOnly | QIODevice::Text );

    QString artist = s.readLine();
    QString album = s.readLine();
    QString track = s.readLine();
    QString duration = s.readLine();
    QString path = s.readLine();

    m_previousPid = s.readLine();
        
    if (!artist.isEmpty())
    {
        MutableTrack t;
        t.setArtist( artist );
        t.setTitle( track );
        t.setAlbum( album );
        t.setDuration( duration.toInt() );
        t.setUrl( QUrl::fromLocalFile( path ) );
        m_connection->start( t );
    }
}
