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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifdef __APPLE__
    // first to prevent compilation errors with Qt 4.5.0-beta1
    #include <Carbon/Carbon.h>
    #include <ApplicationServices/ApplicationServices.h>
    static pascal OSErr appleEventHandler( const AppleEvent*, AppleEvent*, long );
    #include <QMainWindow>
    extern void qt_mac_set_menubar_icons( bool );    
#endif

#include "UnicornApplication.h"
#include "QMessageBoxBuilder.h"
#include "UnicornCoreApplication.h"
#include "widgets/LoginDialog.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/lastfm/core/CoreDir.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsKeys.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QDebug>
#include <QLocale>
#include <QTranslator>


unicorn::Application::Application( int& argc, char** argv ) throw( StubbornUserException )
                    : QApplication( argc, argv ),
                      m_logoutAtQuit( false )
{
#ifdef Q_WS_MAC
    qt_mac_set_menubar_icons( false );
#endif    
    
    CoreApplication::init();

#ifdef __APPLE__
    AEEventHandlerUPP h = NewAEEventHandlerUPP( appleEventHandler );
    AEInstallEventHandler( kCoreEventClass, kAEReopenApplication, h, 0, false );
#endif

    translate();

    CoreSettings s;
    if (s.value( "Username" ).toString().isEmpty() || s.value( "SessionKey" ).toString().isEmpty() || Settings().logOutOnExit())
    {
        LoginDialog d( s.value( "Username" ).toString() );
        if (d.exec() == QDialog::Accepted)
        {
            // if LogOutOnExit is enabled, really, we shouldn't store these,
            // but it means other Unicorn apps can log in while the client is 
            // loaded, and we delete the settings on exit if logOut is on
            s.setValue( "Username", d.username() );
            s.setValue( "SessionKey", d.sessionKey() );
            s.setValue( "Password", d.passwordHash() );
            
            UserSettings().setValue( UserSettings::subscriptionKey(), d.isSubscriber() );
        }
        else
        {
            throw StubbornUserException();
        }
    }

    Ws::Username = s.value( "Username" ).toString();
    Ws::SessionKey = s.value( "SessionKey" ).toString();
    
    connect( AuthenticatedUser().getInfo(), SIGNAL(finished( WsReply* )), SLOT(onUserGotInfo( WsReply* )) );

#ifdef __APPLE__
    setQuitOnLastWindowClosed( false );
#endif
}


void
unicorn::Application::translate()
{
#ifdef NDEBUG
    QString const iso3166 = QLocale().name().right( 2 ).toLower();

#ifdef Q_WS_MAC
    QDir const d = CoreDir::bundle().filePath( "Contents/Resources/qm" );
#else
    QDir const d = qApp->applicationDirPath() + "/i18n";
#endif

    //TODO need a unicorn/core/etc. translation, plus policy of no translations elsewhere or something!
    QTranslator* t1 = new QTranslator;
    t1->load( d.filePath( "lastfm_" + iso3166 ) );

    QTranslator* t2 = new QTranslator;
    t2->load( d.filePath( "qt_" + iso3166 ) );

    installTranslator( t1 );
    installTranslator( t2 );
#endif
}


unicorn::Application::~Application()
{
    // we do this here, rather than when the setting is changed because if we 
    // did it then the user would be unable to change their mind
    if (Settings().logOutOnExit() || m_logoutAtQuit)
    {
        CoreSettings s;
        s.remove( "SessionKey" );
        s.remove( "Password" );
    }
}


void
unicorn::Application::onUserGotInfo( WsReply* r )
{
    try
    {
        const char* key = UserSettings::subscriptionKey();
        bool const value = r->lfm()["user"]["subscriber"].text().toInt() == 1;
        UserSettings().setValue( key, value );
    }
    catch (WsDomElement& e)
    {
        qWarning() << e;
    }
    
    emit userGotInfo( r );
}


#ifdef __APPLE__
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
            foreach (QWidget* w, qApp->topLevelWidgets())
                if (qobject_cast<QMainWindow*>(w))
                    w->show(), w->raise();
            return noErr;
        }

        default:
            return unimpErr;
    }
}
#endif
