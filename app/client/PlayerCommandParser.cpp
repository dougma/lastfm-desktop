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

#include "PlayerCommandParser.h"
#include <QStringList>
#include <QUrl>


PlayerCommandParser::PlayerCommandParser( QString line ) throw( PlayerCommandParser::Exception )
{
    line = line.trimmed();
    if (line.isEmpty()) throw Exception( "Command string seems to be empty" );

    qDebug() << line;

    m_command = extractCommand( line ); //removes the command string from line

    line = line.trimmed();

    QMap<QChar, QString> const args = extractArgs( line );
    QString const required = requiredArgs( m_command );

    for (int i = 0; i < required.length(); ++i)
    {
        QChar const c = required[i];
        if (!args.contains( c ))
            throw Exception( "Mandatory argument unspecified: " + QString(c) );
    }

    m_playerId = args['c'];

    switch (m_command)
    {
        case Start:
            m_track = extractTrack( args );
            break;
        case Bootstrap:
            m_username = args['u'];
            break;
        case Init:
            m_applicationPath = args['f'];
        default:
            break;
    }
}


PlayerCommandParser::Command
PlayerCommandParser::extractCommand( QString& line )
{
    int const n = line.indexOf( ' ' );
    if (n == -1) throw Exception( "Unable to parse" );

    QString const command = line.left( n ).toUpper();

    // Trim off command from passed in string
    line = line.mid( n + 1 );

    if (command == "START") return Start;
    if (command == "STOP") return Stop;
    if (command == "PAUSE") return Pause;
    if (command == "RESUME") return Resume;
    if (command == "BOOTSTRAP") return Bootstrap;
    if (command == "INIT") return Init;
    if (command == "TERM") return Term;

    throw Exception( "Invalid command" );
}


namespace mxcl
{
    /** replaces '&&' and splits on remaining single '&' */
    QStringList static inline split( QString line )
    {
        QStringList parts;
        int start = 0, i = 0;
        int end = 0;
        while ((end = line.indexOf( '&', i )) != -1)
        {
            i = end + 1;
            if (line[i] == '&') {
                line.remove( end, 1 ); //convert && to &
                continue;
            }
            parts += line.mid( start, end - start );
            start = i;
        }
        return parts << line.mid( start );
    }
}


QMap<QChar, QString>
PlayerCommandParser::extractArgs( const QString& line )
{
    QMap<QChar, QString> map;

    // split by single & only, doubles are in fact &
    foreach (QString pair, mxcl::split( line ))
    {
        QStringList parts = pair.split( '=' );

        if (parts.count() != 2 || parts[0].length() > 1) 
            throw Exception( "Invalid pair: " + pair );
                
        QChar id = parts[0][0];

        if (map.contains( id ))
            throw Exception( "Field identifier occurred twice in request: " + QString(id) );

        map[id] = parts[1].trimmed();
    }

    return map;
}


QString
PlayerCommandParser::requiredArgs( PlayerCommandParser::Command c )
{
    switch (c)
    {   
        case Start: 
            return "catblp";
        case Bootstrap:
            return "cu";
        case Init:
            return "cf";
        default:
        case Stop:
        case Pause:
        case Resume:
        case Term:
            return "c";
    }
}


Track
PlayerCommandParser::extractTrack( const QMap<QChar, QString>& args )
{
    MutableTrack track;
    track.setArtist( args['a'] );
    track.setTitle( args['t'] );
    track.setAlbum( args['b'] );
    track.setMbId( args['m'] );
    track.setDuration( args['l'].toInt() );
    track.setUrl( QUrl::fromLocalFile( QUrl::fromPercentEncoding( args['p'].toUtf8() ) ) );
    track.setSource( Track::Player );
    track.setPlayerId( args['c'] );

    //TODO should be done earlier, NOTE don't get the plugin to send a stamp 
    // time as this is prolly unecessary, and I bet you get new bugs!
    track.stamp();
    
    return track;
}
