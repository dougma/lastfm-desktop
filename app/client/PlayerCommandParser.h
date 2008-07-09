/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#ifndef PLAYER_COMMAND_PARSER_H
#define PLAYER_COMMAND_PARSER_H

#ifdef WIN32 //prevent warning with gcc (lol)
// ms admits its lousy compiler doesn't care about throw declarations
#pragma warning( disable : 4290 )
#endif

#include "lib/unicorn/Track.h"


class PlayerCommandParser
{
public:
    struct Exception : QString
    {
        Exception( QString s ) : QString( s )
        {}
    };

    PlayerCommandParser( QString line ) throw( Exception );

    enum Command
    {
        Start,
        Stop,
        Pause,
        Resume,
        Bootstrap
    };

    Command command() const { return m_command; }
    QString playerId() const { return m_playerId; }
    Track track() const { return m_track; }
    QString username() const { return m_username; }

private:
    Command extractCommand( QString& line );
    QMap<QChar, QString> extractArgs( const QString& line );
    QString requiredArgs( Command );
    Track extractTrack( const QMap<QChar, QString>& args );

    Command m_command;
    QString m_playerId;
    Track m_track;
    QString m_username;
};

#endif // PLAYERCOMMANDPARSER_H
