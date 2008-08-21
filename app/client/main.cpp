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
#include "lib/core/StoreDir.h"
#include "lib/core/MessageBoxBuilder.h"
#include <QDir>
#include <QTimer>

#undef NDEBUG //tmp

#ifdef NDEBUG
    #include "app/breakpad/ExceptionHandler.h"
#endif

#ifdef Q_WS_MAC
    #include <ApplicationServices/ApplicationServices.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, long );
#endif


int main( int argc, char** argv )
{
#ifdef NDEBUG
    google_breakpad::ExceptionHandler( UnicornDir::save().path().toStdString(),
                                       0,
                                       breakPadExecUploader,
                                       this,
                                       HANDLER_ALL );
#endif

    // must be set before the Settings object is created
    QCoreApplication::setApplicationName( PRODUCT_NAME );
    QCoreApplication::setApplicationVersion( VERSION );
    QCoreApplication::setOrganizationName( "Last.fm" );
    QCoreApplication::setOrganizationDomain( "last.fm" );

    try
    {
        App app( argc, argv );

      #ifdef Q_WS_MAC
        AEEventHandlerUPP h = NewAEEventHandlerUPP( appleEventHandler );
        AEInstallEventHandler( 'GURL', 'GURL', h, 0, false );
        //AEInstallEventHandler( kCoreEventClass, kAEQuitApplication, h, 0, false ); //QCoreApplication handles this for us
        AEInstallEventHandler( kCoreEventClass, kAEReopenApplication, h, 0, false );
      #endif
        
        MainWindow window;
        app.setMainWindow( &window );

      #ifdef Q_WS_X11 // HACK: Should not be needed. But otherwise crashes. Qt bug?
        QTimer::singleShot( 0, &window, SLOT( show() ) );
      #else
        window.show();
      #endif

        return app.exec();
    }
    catch (PlayerListener::SocketFailure& e)
    {
        //TODO message to user
        //FIXME can't have it so that there is no radio option if listener socket fails!
        qDebug() << "Socket failure:" << e.what();
		
		MessageBoxBuilder( 0 )
			.setTitle( "Sorry Old Boy" )
			.setText( "You can't run the old client and the new client at once yet!" )
			.exec();
		
        return 1;
    }
    catch (Unicorn::Application::UnsupportedPlatformException&)
    {
        // a message box was displayed to the user by Unicorn::Application
        qDebug() << "Unsupport platform"; 
        return 2;
    }
    catch (Unicorn::Application::StubbornUserException&)
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
        case kAEQuitApplication:
            qApp->quit();
            return noErr;

        case kAEReopenApplication:
        {
            MainWindow& w = static_cast<App&>(*qApp).mainWindow();
            w.show();
            w.raise();
            return noErr;
        }
            
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
