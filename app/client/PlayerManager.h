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

    /** will ban or love the current track */
    void ban();
    void love();

public slots:
    void onTrackStarted( const Track& );
    void onPlaybackEnded( const QString& playerId );
    void onPlaybackPaused( const QString& playerId );
    void onPlaybackResumed( const QString& playerId );
    void onPlayerInit( const QString& playerId );
    void onPlayerTerm( const QString& playerId );

signals:
    /** the int is a PlaybackEvent, @data is documented with the enum */
    void event( int, const QVariant& data = QVariant() );

private slots:
    void onStopWatchTimedOut();

private:
    void handleStateChange( PlayerState::Enum, const ObservedTrack& t = ObservedTrack() );

    ObservedTrack m_track;
    PlayerState::Enum m_state;
};
