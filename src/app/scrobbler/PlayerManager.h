/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "PlaybackState.h"
#include "StopWatch.h"
#include "lib/moose/TrackInfo.h"
#include <QMutableVectorIterator>
#include <QStack>
#include <QVariant>


struct Player
{
    Player()
    {}

    Player( QString _id ) : id( _id )
    {}

    QString id;
    TrackInfo track;
    StopWatch watch;
    PlaybackState::Enum state;
};


class PlayerManager : public QObject
{
    Q_OBJECT

public:
    PlayerManager();

    PlaybackState::Enum state() const { return m_state; }
    TrackInfo track() const { return m_players.top()->track; }

    /** will ban or love the current track */
    void ban();
    void love();

public slots:
    void onTrackStarted( const TrackInfo& );
    void onPlaybackEnded( const QString& playerId );
    void onPlaybackPaused( const QString& playerId );
    void onPlaybackResumed( const QString& playerId );

signals:
    /** the scrobble point tick, connect it to some gui representation */
    void tick( int );
    /** the int is a PlaybackEvent, @data is documented with the enum */
    void event( int, const QVariant& data = QVariant() );

private:
    void handleStateChange( PlaybackState::Enum, const TrackInfo& t = TrackInfo() );

    class UniquePlayerStack : public QStack<Player*>
    {
        bool find( const QString& id, QMutableVectorIterator<Player*>& i )
        {
            while (i.hasNext())
                if (i.next()->id == id)
                    return true;
            return false;
        }

    public:
        Player* operator[]( const QString& id )
        {
            QMutableVectorIterator<Player*> i( *this );
            if (!find( id, i ))
            {
                push( new Player( id ) );
                return top();
            }

            return i.value();
        }

        void remove( const QString& id )
        {
            QMutableVectorIterator<Player*> i( *this );
            find( id, i );
            delete i.value();
            i.remove();
        }
    };

    UniquePlayerStack m_players;
    PlaybackState::Enum m_state;
};


namespace The
{
    PlayerManager& playerManager(); //defined in App.cpp
}
