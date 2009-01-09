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
#include "StopWatch.h" //for QPointer
#include "lib/listener/PlayerConnection.h" //for QPointer
#include "lib/lastfm/types/Track.h"
#include "lib/unicorn/UnicornApplication.h"
#include <QPointer>


class StateMachine : public QObject
{
    Q_OBJECT
    
public:
    StateMachine( QObject* parent );
    
    void setScrobbleFraction( float f ) { m_scrobbleFraction = f; }
    
    Track track() const { return m_track; }
    State state() const { return m_state; }
    
    public slots:
    void setConnection( class PlayerConnection* );
    
signals:
    /** you won't get this one for radio starting, you get
      * stateChanged( TuningIn ) instead */
    void playerChanged( const QString& name );
    /** you really only get this signal for state changes, so when a track was
      * playing and a new track starts, you don't get this signal as that is
      * Playing -> Playing! */
    /** You get the stateChanged signal _last_, this is so you can get the
      * associated data for the state before you handle the state, eg TuningIn
      * station, trackStarted track, etc. */
    void stateChanged( State newstate, const Track& = Track() );
    
    void stopped();
    /** @p t will be played soon. If the track isNull, state is no longer
      * Playing. Note that a null track does not correspond with the Unspooled
      * PlayingEvent. StopWatch is always valid unless track isNull()
      * The default of 0 is there so your connect() calls are more concise */
    void trackSpooled( const Track&, StopWatch* = 0 );
    void trackUnspooled( const Track& );
    
    void scrobblePointReached( const Track& );
    
protected:
    // not slots as that is not safe IMO
    void start(); // if m_track.isNull() everything is thoroughly undefined
    void unspoolTrack();
    
private slots:
    void spoolTrack( const Track&, uint elapsed = 0 );
    void pause();
    void resume();
    void stop();
    
    void onStopWatchTimedOut();
    void onTrackStarted( const Track& );
    
public slots:
    void onRadioTuningIn( const class RadioStation& );
    void onRadioTrackSpooled( const Track& );
    void onRadioTrackStarted( const Track& );
    void onRadioBuffering( int );
    void onRadioStopped();    
    
private:
    float m_scrobbleFraction;

    QPointer<StopWatch> m_watch;
    QPointer<PlayerConnection> m_connection;

    State m_state;
    Track m_track;
    
    bool m_radioIsActive;
};
