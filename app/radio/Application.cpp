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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "Application.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include <lastfm/WsError>
#include "Radio.h"
using moralistfad::Application;


Application::Application( int& argc, char** argv ) : unicorn::Application( argc, argv )
{}

void
Application::onWsError( Ws::Error e )
{
    switch (e)
    {
        case Ws::OperationFailed:
            //TODOCOPY
            //TODO use the non intrusive status messages
            QMessageBoxBuilder( 0 ) //TODO window pointer
                    .setTitle( "Oops" )
                    .setText( "Last.fm is b0rked" )
                    .exec();
            break;

        case Ws::InvalidSessionKey:
            logout();
            break;
			
		default:
			break;
    }
}

void
Application::onRadioError( int code, const QVariant& data )
{
    switch (code)
    {
        case Ws::NotEnoughContent:
            emit error( tr("Sorry, there is no more content available for this radio station.") );
            break;
            
        case Ws::MalformedResponse:
        case Ws::TryAgainLater:
            emit error( tr("Sorry, there was a radio error at Last.fm. Please try again later.") );
            break;
            
        case Ws::SubscribersOnly:
            emit error( tr("Sorry, this station is only available to Last.fm subscribers. "
                           "<A href='http://www.last.fm/subscribe'>Sign up</a>.") );
            break;

        case Ws::UnknownError:
            // string contains Phonon generated user readable error message
            emit error( data.toString() );
            break;

        default:
            emit error( tr("Sorry, an unexpected error occurred.") );
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
