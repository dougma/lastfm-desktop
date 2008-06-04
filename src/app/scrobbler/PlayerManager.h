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

#include "PlaybackEvent.h"
#include "StopWatch.h"
#include <QMutableVectorIterator>
#include <QStack>


struct Player
{
    Player( QString _id ) : id( _id )
    {}

    Player( const Player that ) : id( that.id )
    {}

    QString const id;
    TrackInfo track;
    PlaybackState::Enum state;
    StopWatch watch;
};


class PlayerManager : public QObject
{
    Q_OBJECT

public:
    PlayerManager();

    PlaybackState state() const { return m_state; }
    TrackInfo track() const { return m_tracks.top(); }

    /** will ban or love the current track */
    void ban();
    void love();

public slots:
    void onTrackStarted( const TrackInfo& );
    void onPlaybackEnded( const QString& playerId );
    void onPlaybackPaused( const QString& playerId );
    void onPlaybackResumed( const QString& playerId );

signals:
    /** the int is a PlaybackEvent, @data is documented with the enum */
    void event( int, const QVariant& data );

private:
    class UniquePlayerStack : public QStack<Player>
    {
        bool find( const QString& id, QMutableVectorIterator& i )
        {
            while (i.hasNext())
                if (i.next().id == id)
                    return true;
            return false;
        }

    public:
        Player& operator[]( const QString& id )
        {
            QMutableVectorIterator<Player> i( m_tracks );
            if (!find( id, i ))
            {
                push( Player( id ) );
                return top();
            }

            return i.value();
        }

        void remove( const QString& id )
        {
            QMutableVectorIterator<Player> i( m_tracks );
            find( id, i );
            i.remove();
        }
    };

    UniquePlayerStack m_players;
    PlaybackState m_state;
};