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

#include "App.h"
#include "PlayerListener.h"
#include "widgets/MainWindow.h"
#include "version.h"
#include "lib/moose/MooseCommon.h"
#include "lib/unicorn/Logger.h"
#include "lib/unicorn/UnicornCommon.h"
#include "lib/breakpad/BreakPad.h"
#include <QDir>


int main( int argc, char** argv )
{
#ifdef _BREAKPAD
    BreakPad breakpad;
    breakpad.setProductName( PRODUCT_NAME );
#endif

    // must be set before the Settings object is created
    QCoreApplication::setApplicationName( PRODUCT_NAME );
    QCoreApplication::setOrganizationName( "Last.fm" );
    QCoreApplication::setOrganizationDomain( "last.fm" );

    QDir().mkpath( Moose::cachePath() );
    QDir().mkpath( Moose::savePath() );
    QDir().mkpath( Moose::logPath() );
    
    Logger& logger = Logger::GetLogger();
    logger.Init( Moose::logPath( "Last.fm.log" ), false );
    logger.SetLevel( Logger::Debug );
    LOGL( 3, "Application: " << PRODUCT_NAME << " " << VERSION );
    LOGL( 3, "Platform: " << Unicorn::verbosePlatformString() );

    try
    {
        App app( argc, argv );
        
        MainWindow window;
        app.setMainWindow( &window );
        window.show();

        return app.exec();
    }
    catch (PlayerListener::SocketFailure& e)
    {
        //TODO
        //FIXME can't have it so that there is no radio option if listener socket fails!
        qDebug() << "Socket failure:" << e.what();
    }
    catch (int)
    {
        // user wouldn't log in
    }
}
