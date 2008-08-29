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

#include "PlayerState.h"
#include "ObservedTrack.h"
#include "StopWatch.h"
#include <QMutableVectorIterator>
#include <QStack>
#include <QVariant>


class PlayerManager : public QObject
{
    Q_OBJECT

public:
    PlayerManager( class PlayerListener* parent );

    ObservedTrack track() const { return m_track; }

public slots:
    void onTrackStarted( const Track& );
    void onPlaybackEnded( const QString& playerId );
    void onPlaybackPaused( const QString& playerId );
    void onPlaybackResumed( const QString& playerId );
    void onPlayerConnected( const QString& playerId );
    void onPlayerDisconnected( const QString& playerId );

	/** these are optional, and for the radio basically, as only that can have
	  * periods inbetween tracks where a sesion is still active */
	void onTrackEnded( const QString& playerId );
	void onPlaybackSessionStarted( const QString& playerId );
	
signals:
    /** the int is a PlaybackEvent, @data is documented with the enum */
    void event( int, const QVariant& data = QVariant() );

private slots:
    void onStopWatchTimedOut();

private:
    ObservedTrack m_track;
    PlayerState::Enum m_state;
	
	QString m_playerId;
};
