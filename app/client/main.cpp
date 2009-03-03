/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#ifdef __APPLE__
    // first to prevent compilation errors with Qt 4.5.0
    //TODO shorten this mother fucker
    //NOTE including Carbon/Carbon.h breaks things as it has sooo many symbols
    //     in the global namespace
    #include </System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/AE.framework/Versions/A/Headers/AppleEvents.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, long );
#endif

#include "App.h"
#include "Settings.h"
#include "widgets/MainWindow.h"
#include "_version.h"
#include "app/moose.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/unicorn/UniqueApplication.h"
#include <QDir>
#include <QTimer>

#define NBREAKPAD

#ifndef NBREAKPAD
    #include "app/breakpad/ExceptionHandler.h"
#endif


int main( int argc, char** argv )
{
#ifndef NBREAKPAD
    google_breakpad::ExceptionHandler( CoreDir::save().path().toStdString(),
                                       0,
                                       breakPadExecUploader,
                                       this,
                                       HANDLER_ALL );
#endif

    QCoreApplication::setApplicationName( moose::applicationName() );
    QCoreApplication::setApplicationVersion( VERSION );
    QCoreApplication::setOrganizationName( CoreSettings::organizationName() );
    QCoreApplication::setOrganizationDomain( CoreSettings::organizationDomain() );

#ifdef NDEBUG
    UniqueApplication uapp( moose::id() );
    if (uapp.isAlreadyRunning())
		return uapp.forward( argc, argv ) ? 0 : 1;
    uapp.init1();
#endif	
	
    try
    {
        App app( argc, argv );
      #ifdef NDEBUG
		uapp.init2( &app );
        app.connect( &uapp, SIGNAL(arguments( QStringList )), SLOT(parseArguments( QStringList )) );
      #endif

      #ifdef Q_WS_MAC
        AEEventHandlerUPP h = NewAEEventHandlerUPP( appleEventHandler );
        AEInstallEventHandler( 'GURL', 'GURL', h, 0, false );
      #endif
        
        MainWindow window;
        app.setMainWindow( &window );
      #ifndef NDEBUG
        window.setWindowTitle( "Last.am" );
      #endif

        if (!app.arguments().contains( "--tray" ))
            window.show();
        
        app.parseArguments( app.arguments() );

        return app.exec();
    }
    catch (App::UnsupportedPlatformException&)
    {
        // a message box was displayed to the user by Unicorn::Application
        qCritical() << "Unsupported platform";
        return 2;
    }
    catch (unicorn::Application::StubbornUserException&)
    {
        // user wouldn't log in
        return 0;
    }
}


#ifdef Q_WS_MAC
static pascal OSErr appleEventHandler( const AppleEvent* e, AppleEvent*, long )
{
    OSType id = typeWildCard;
    AEGetAttributePtr( e, keyEventIDAttr, typeType, 0, &id, sizeof(id), 0 );
    
    switch (id)
    {
        case 'GURL':
        {
            DescType type;
            Size size;

            char buf[1024];
            AEGetParamPtr( e, keyDirectObject, typeChar, &type, &buf, 1023, &size );
            buf[size] = '\0';

            static_cast<App&>(*qApp).open( QString::fromUtf8( buf ) );
            return noErr;
        }
            
        default:
            return unimpErr;
    }
}
#endif
