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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "ITunesListener.h"
#include "lib/core/AppleScript.h"
#include "lib/moose/MooseCommon.h"
#include "lib/core/UnicornUtils.h"
#include <QTcpSocket>
#include <QThread>
#include <QHostAddress>
#include <QUrl>
#include <QTextStream>


ITunesListener::ITunesListener( uint const port, QObject* parent )
            : m_port( port )
{
    // you can't child QThreads to other threads, so do this instead
    connect( parent, SIGNAL(destroyed()), SLOT(deleteLater()) );
    start();
}


void
ITunesListener::run()
{
    setupCurrentTrack();

    CFNotificationCenterAddObserver( CFNotificationCenterGetDistributedCenter(), 
                                     this,
                                     callback, 
                                     CFSTR( "com.apple.iTunes.playerInfo" ), 
                                     NULL, 
                                     CFNotificationSuspensionBehaviorDeliverImmediately );
    
    exec();
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

    void determineTrackrmation();
        
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
    return Unicorn::CFStringToQString( s );
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
ITunesDictionaryHelper::determineTrackrmation()
{
    duration = token<int>( CFSTR("Total Time") ) / 1000;
    artist = token<QString>( CFSTR("Artist") );
    album = token<QString>( CFSTR("Album") );
    name = token<QString>( CFSTR("Name") );
    
    // Get path decoded - iTunes encodes the file location as URL
    CFStringRef location = token<CFStringRef>( CFSTR("Location") );
    QUrl url = QUrl::fromEncoded( Unicorn::CFStringToUtf8( location ) );
    path = url.toString().remove( "file://localhost" );
    
    static AppleScript script( "tell application \"iTunes\" to return persistent ID of current track & player position" );
    QString const out = script.exec();
    pid = out.left( 16 );
    position = out.mid( 16 ).toInt();
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
            transmit( "PAUSE c=osx\n" );
            break;

        case Stopped:
            transmit( "STOP c=osx\n" );
            break;
            
        case Playing:
            if (!isMusic())
            {
                qDebug() << "Ignoring current track because it isn't music.";
                break;
            }

            dict.determineTrackrmation();

            // if the track is restarted it has the same pid and a position of 0
            if (m_previousPid == dict.pid && dict.position != 0)
            {
                if (previousState == Paused)
                    transmit( "RESUME c=osx\n" );
                //else the user changed some metadata or the track's rating etc.
            }
            else
            {
                transmit( "START c=osx"
                               "&a=" + encodeAmp( dict.artist ) +
                               "&t=" + encodeAmp( dict.name ) +
                               "&b=" + encodeAmp( dict.album ) +
                               "&l=" + QString::number( dict.duration ) +
                               "&p=" + encodeAmp( dict.path ) + '\n' );

                m_previousPid = dict.pid;
            }
            break;

        default:
          qWarning() << "Unknown state.";
          break;
    }
}


void
ITunesListener::transmit( const QString& data )
{
    QTcpSocket socket;
    socket.connectToHost( QHostAddress::LocalHost, m_port );
    if (socket.waitForConnected( 1000 ))
    {
        int bytesWritten = socket.write( data.toUtf8() );
        socket.flush();
        socket.waitForDisconnected( 1000 );

        if (bytesWritten == -1)
        {
            qCritical() << "Sending submission through socket failed.";
        }
    }
}


bool //static
ITunesListener::isMusic()
{
    const char* code = "with timeout of 1 seconds\n"
                           "tell application \"iTunes\" to return video kind of current track\n"
                       "end timeout";

    static AppleScript script( code ); //compile once
    QString const r = script.exec();

    return r == "none" || r == "music video" || r == "";
}


bool //static
ITunesListener::iTunesIsPlaying()
{
    const char* code = "tell application \"iTunes\" to return player state is playing";
    return AppleScript( code ).exec() == "true";
}


void
ITunesListener::setupCurrentTrack()
{
    if (!Unicorn::iTunesIsOpen() || !iTunesIsPlaying() || !isMusic())
        return;
    
    #define ENDL " & \"\n\" & "
    const char* code =
        "tell application \"iTunes\" to tell current track\n"
            "set L to location\n"
            "return artist" ENDL
                   "album" ENDL
                   "name" ENDL
                   "(duration as integer)" ENDL
                   "POSIX path of L" ENDL
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
        
    if (!path.isEmpty())
    {
        QString data = "START c=osx"
                            "&a=" + encodeAmp( artist ) +
                            "&t=" + encodeAmp( track ) +
                            "&b=" + encodeAmp( album ) +
                            "&l=" + duration +
                            "&p=" + encodeAmp( path ) + '\n';

        transmit( data );
    }
}
