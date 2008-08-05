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

#include "UnicornApplication.h"
#include "UnicornSettings.h"
#include "widgets/LoginDialog.h"
#include "common/c++/Logger.h"
#include "lib/core/MessageBoxBuilder.h"
#include "lib/core/StoreDir.h"
#include "lib/core/UnicornUtils.h"
#include "lib/ws/WsKeys.h"
#include <QDebug>
#include <QTranslator>

#ifdef WIN32
extern void qWinMsgHandler(QtMsgType t, const char* str);
#endif


Unicorn::Application::Application( int argc, char** argv ) throw( StubbornUserException, UnsupportedPlatformException )
                    : QApplication( argc, argv ),
                      m_logoutAtQuit( false )
{
    // HI! DON'T USE OURS! GET YOUR OWN! http://www.last.fm/api
    Ws::SharedSecret = "73582dfc9e556d307aead069af110ab8";
    Ws::ApiKey = "c8c7b163b11f92ef2d33ba6cd3c2c3c3";

    QCoreApplication::setOrganizationName( "Last.fm" );
    QCoreApplication::setOrganizationDomain( "last.fm" );    

    StoreDir::mkpaths();

    qInstallMsgHandler( qMsgHandler );
#ifdef WIN32
    QString bytes = StoreDir::mainLog();
    const wchar_t* path = bytes.utf16();
#else
    QByteArray bytes = StoreDir::mainLog().toLocal8Bit();
    const char* path = bytes.data();
#endif
    m_log = new Logger( path );
    qInfo() << "Introducing" << applicationName()+'-'+applicationVersion();
    qInfo() << "Directed by" << Unicorn::verbosePlatformString();

    translate();

#ifdef Q_WS_MAC
    if (QSysInfo::MacintoshVersion < QSysInfo::MV_10_4)
    {
        MessageBoxBuilder( 0 )
            .setIcon( QMessageBox::Critical )
            .setTitle( tr("Unsupported OS X Version") )
            .setText( tr( "We are sorry, but Last.fm requires OS X Tiger or above.") )
            .exec();
        throw UnsupportedPlatformException();
    }
#endif

    Settings s;
    if (s.username().isEmpty() || s.sessionKey().isEmpty() || s.logOutOnExit())
    {
        LoginDialog d;
        if (d.exec() == QDialog::Accepted)
        {
            // if LogOutOnExit is enabled, really, we shouldn't store these,
            // but it means other Unicorn apps can log in while the client is 
            // loaded, and we delete the settings on exit if logOut is on
            Unicorn::QSettings().setValue( "Username", d.username() );
            Unicorn::UserQSettings s;
            s.setValue( "Username", d.username() );
            s.setValue( "SessionKey", d.sessionKey() );
        }
        else
        {
            throw StubbornUserException();
        }
    }

    Ws::SessionKey = qstrdup( s.sessionKey().toAscii().data() );
}


void
Unicorn::Application::translate()
{
#ifdef NDEBUG
    QString const lang_code = Unicorn::Settings().language();

#ifdef Q_WS_MAC
    QDir d = SystemDir::bundle().filePath( "Contents/Resources/qm" );
#else
    QDir d = qApp->applicationFolderPath() + "/i18n";
#endif

    //TODO need a unicorn/core/etc. translation, plus policy of no translations elsewhere or something!
    QTranslator* t1 = new QTranslator;
    t1->load( d.filePath( "lastfm_" + lang_code ) );

    QTranslator* t2 = new QTranslator;
    t2->load( d.filePath( "qt_" + lang_code ) );

    installTranslator( t1 );
    installTranslator( t2 );
#endif
}


Unicorn::Application::~Application()
{
    // we do this here, rather than when the setting is changed because if we 
    // did it then the user would be unable to change their mind
    if (Unicorn::Settings().logOutOnExit() || m_logoutAtQuit)
    {
        Unicorn::UserQSettings s;
        s.remove( "Password" );
        s.remove( "SessionKey" );
        Unicorn::QSettings().remove( "Username" ); // do after the UserQSettings or it doesn't work!
    }

    delete m_log;
}


void
Unicorn::Application::qMsgHandler( QtMsgType type, const char* msg )
{
    Logger::Severity level;
    switch (type)
    {
        case QtDebugMsg: 
            level = Logger::Debug; 
            break;
        case QtWarningMsg: 
            level = Logger::Warning; 
            break;
        case QtFatalMsg:
        case QtCriticalMsg: 
            level = Logger::Critical; 
            break;
        default:
            level = Logger::Info;
            break;
    }

    Logger::the().log( msg );

	// it crashes on mac if you call arguments()! Qt 4.4.1 --mxcl	
#ifndef Q_WS_MAC && defined NDEBUG
	#if QT_VERSION > 0x00040401
		#error check if the next bit works yet
	#endif
    if (arguments().contains( "--debug" ))
#endif
#ifdef WIN32
        qWinMsgHandler( type, msg );
#else
        fprintf( stderr, "%s\n", msg );
        fflush( stderr );
#endif
}
