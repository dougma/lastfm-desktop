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

#ifndef UNICORN_APPLICATION_H
#define UNICORN_APPLICATION_H

#include "common/HideStupidWarnings.h"
#include "lib/DllExportMacro.h"
#include "../liblastfm/src/core/CoreSettings.h"
#include <QApplication>
class WsReply;


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

        struct Settings
        {
            bool logOutOnExit() { return CoreSettings().value( "LogOutOnExit", false ).toBool(); }
            void setLogOutOnExit( bool b ) { CoreSettings().setValue( "LogOutOnExit", b ); }
        };

    public slots:
        void logout()
        {
            m_logoutAtQuit = true;
            quit();
        }

    private:
        void translate();

    private slots:
        void onUserGotInfo( WsReply* );

    signals:
        void userGotInfo( WsReply* );
    };
}

#endif
