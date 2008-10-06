/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "State.h"
#include "StopWatch.h"
#include "PlayerConnection.h"
#include "lib/types/Track.h"
#include <QPointer>


/** mediates a single source of music from many inputs */
class PlayerMediator : public QObject
{
    Q_OBJECT

public:
    PlayerMediator( class PlayerListener* );
    
    Track track() const { return m_track; }

public slots:
    void onRadioTuningIn( const class RadioStation& );
    void onRadioTrackSpooled( const Track& );
    void onRadioTrackStarted( const Track& );
    void onRadioBuffering( int );
    void onRadioStopped();

private slots:
    void onPlayerConnectionCommandReceived( const PlayerConnection& );

private:
    void changeState( State );
    void endTrack();
    void stop();
    
    /** replays the last command parsed by this connection, and the Init
      * command first */ 
    void replay( const PlayerConnection& );
    
signals:
    void playerChanged( const QString& name );
    /** you really only get this signal for state changes, so when a track was
      * playing and a new track starts, you don't get this signal as that is
      * Playing -> Playing! */
    /** You get the stateChanged signal _last_, this is so you can get the 
      * associated data for the state before you handle the state, eg TuningIn
      * station, trackStarted track, etc. */
    void stateChanged( State newstate, const Track& );
    
    void stopped();
    /** @p t will be played soon. If the track isNull, state is no longer
      * Playing. Note that a null track does not correspond with the Unspooled
      * PlayingEvent. StopWatch is always valid unless track isNull()
      * The default of 0 is there so your connect() calls are more concise */
    void trackSpooled( const Track&, StopWatch* = 0 );
    void trackUnspooled( const Track& );
    void scrobblePointReached( const Track& );

private slots:
    void onStopWatchTimedOut();

private:
    State m_state;  
    QPointer<StopWatch> m_watch;
    Track m_track;
    bool m_radioIsActive;
    PlayerConnection m_connection;
};
