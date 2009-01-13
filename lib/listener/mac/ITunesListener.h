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

#ifndef ITUNES_SCRIPT_H
#define ITUNES_SCRIPT_H

#include <QThread>
#include <CoreFoundation/CoreFoundation.h>


/** @author Max Howell <max@last.fm> */
class ITunesListener : public QThread
{
    Q_OBJECT

    virtual void run();    
    
public:
    ITunesListener( QObject* parent );

    enum State { Unknown = -1, Playing, Paused, Stopped };

signals:
    void newConnection( class PlayerConnection* );
    
private:
    static bool iTunesIsPlaying();
    /** @returns true if the currently playing track is music, ie. not a podcast */
    static bool isMusic();
    /** iTunes notification center callback */
    static void callback( CFNotificationCenterRef, 
                          void*, 
                          CFStringRef, 
                          const void*, 
                          CFDictionaryRef );

    void callback( CFDictionaryRef );
    void setupCurrentTrack();

    State m_state;
    QString m_previousPid;
    class ITunesConnection* m_connection;
};

#endif
