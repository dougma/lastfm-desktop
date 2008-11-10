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
#include "UnicornCoreApplication.h"
#include "widgets/LoginDialog.h"
#include "lib/unicorn/UnicornSettings.h"
#include "lib/lastfm/core/CoreDir.h"
#include "lib/lastfm/core/QMessageBoxBuilder.h"
#include "lib/lastfm/ws/WsKeys.h"
#include <QDebug>
#include <QTranslator>


Unicorn::Application::Application( int& argc, char** argv ) throw( StubbornUserException, UnsupportedPlatformException )
                    : QApplication( argc, argv ),
                      m_logoutAtQuit( false )
{
    UnicornCoreApplication::init();
    
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

    CoreSettings s;
    if (s.value( "Username" ).toString().isEmpty() || s.value( "SessionKey" ).toString().isEmpty() || Unicorn::Settings().logOutOnExit())
    {
        LoginDialog d;
        if (d.exec() == QDialog::Accepted)
        {
            // if LogOutOnExit is enabled, really, we shouldn't store these,
            // but it means other Unicorn apps can log in while the client is 
            // loaded, and we delete the settings on exit if logOut is on
            s.setValue( "Username", d.username() );
            s.setValue( "SessionKey", d.sessionKey() );
            s.setValue( "Password", d.passwordHash() );
        }
        else
        {
            throw StubbornUserException();
        }
    }

    Ws::Username = s.value( "Username" ).toString();
    Ws::SessionKey = s.value( "SessionKey" ).toString();
}


void
Unicorn::Application::translate()
{
#ifdef NDEBUG
    QString const lang_code = CoreLocale::system().code();

#ifdef Q_WS_MAC
    QDir d = CoreDir::bundle().filePath( "Contents/Resources/qm" );
#else
    QDir d = qApp->applicationDirPath() + "/i18n";
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
        CoreSettings s;
        s.remove( "SessionKey" );
        s.remove( "Username" );
    }
}
