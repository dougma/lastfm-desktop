/*
   Copyright 2005-2009 Last.fm Ltd. 
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
#ifndef UNICORN_APPLICATION_H
#define UNICORN_APPLICATION_H

#include "common/HideStupidWarnings.h"
#include "lib/DllExportMacro.h"
#include "UnicornSettings.h"
#include <QApplication>
class QNetworkReply;
 

namespace unicorn
{
    class UNICORN_DLLEXPORT Application : public QApplication
    {
        Q_OBJECT

        bool m_logoutAtQuit;

    public:
        class StubbornUserException
        {};

        /** will put up the log in dialog if necessary, throwing if the user
          * cancels, ie. they refuse to log in */
        Application( int&, char** ) throw( StubbornUserException );
        ~Application();

        /** Will return the actual stylesheet that is loaded if one is specified
           (on the command-line with -stylesheet or with setStyleSheet(). )
           Note. the QApplication styleSheet property will return the path 
                 to the css file unlike this method. */
        const QString& loadedStyleSheet() {
            return m_styleSheet;
        }

        struct Settings
        {
            bool logOutOnExit() { return GlobalSettings().value( "LogOutOnExit", false ).toBool(); }
            void setLogOutOnExit( bool b ) { GlobalSettings().setValue( "LogOutOnExit", b ); }
        };

    public slots:
        void logout()
        {
            m_logoutAtQuit = true;
            quit();
        }

    private:
        void translate();
        QString m_styleSheet;

    private slots:
        void onUserGotInfo();

    signals:
        void userGotInfo( QNetworkReply* );
    };
}

#endif
