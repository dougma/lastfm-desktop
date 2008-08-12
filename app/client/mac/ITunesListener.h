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

#include <QtGlobal> //Q_WS_MAC
#ifndef Q_WS_MAC
#define ITUNES_SCRIPT_H
#endif

#ifndef ITUNES_SCRIPT_H
#define ITUNES_SCRIPT_H

#include <QThread>
#include <CoreFoundation/CoreFoundation.h>

/** @author Christian Muehlhaeuser <chris@last.fm>
  * @contributor Erik Jaelevik <erik@last.fm>
  * @rewrite Max Howell <max@last.fm>
  */
class ITunesListener : public QThread
{
    Q_OBJECT

public:
    ITunesListener( uint port, QObject* parent );

    enum State { Unknown = -1, Playing, Paused, Stopped };

    virtual void run();
    
private:
    /** to communicate with the player listener */
    uint const m_port;

    static bool iTunesIsPlaying();
    /** @returns true if the currently playing track is music, ie. not a podcast */
    static bool isMusic();
    /** iTunes notification center callback */
    static void callback( CFNotificationCenterRef, 
                          void*, 
                          CFStringRef, 
                          const void*, 
                          CFDictionaryRef );
    static void callback2( CFNotificationCenterRef, 
						 void*, 
						 CFStringRef, 
						 const void*, 
						 CFDictionaryRef );

    void callback( CFDictionaryRef );
    void transmit( const QString& data );
    void setupCurrentTrack();

    State m_state;
    QString m_previousPid;
};

#endif
