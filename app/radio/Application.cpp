/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Application.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "Radio.h"
#include "ScrobSocket.h"

using moralistfad::Application;


Application::Application( int& argc, char** argv ) 
    : unicorn::Application( argc, argv )
{
}

// lastfmlib invokes this directly, for some errors:
void
Application::onWsError( lastfm::ws::Error e )
{
    switch (e)
    {
        case lastfm::ws::InvalidSessionKey:
            logout();
            break;
		default:
			break;
    }
}

enum Argument
{
    LastFmUrl,
    Pause, //toggles pause
    Skip,
    Unknown
};

Argument argument( const QString& arg )
{
    if (arg == "--pause") return Pause;
    if (arg == "--skip") return Skip;

    QUrl url( arg );
    //TODO show error if invalid schema and that
    if (url.isValid() && url.scheme() == "lastfm") return LastFmUrl;

    return Unknown;
}

void
Application::parseArguments( const QStringList& args )
{
    qDebug() << args;

    if (args.size() == 1)
        return;

    foreach (QString const arg, args.mid( 1 ))
        switch (argument( arg ))
        {
            case LastFmUrl:
                radio->play( RadioStation( QUrl( arg ) ) );
                break;

            case Skip:
            case Pause:
                qDebug() << "Unimplemented:" << arg;
                break;

            case Unknown:
                qDebug() << "Unknown argument:" << arg;
                break;
        }
}
