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

#include "lib/DllExportMacro.h"
#include "common/HideStupidWarnings.h"
#include <QApplication>


namespace Unicorn
{
    class UNICORN_DLLEXPORT Application : public QApplication
    {
        Q_OBJECT

        bool m_logoutAtQuit;

    public:
        // shows a message box advising user of error before throwing
        class UnsupportedPlatformException
        {};

        class StubbornUserException
        {};
        
        /** will put up the log in dialog if necessary, throwing if the user
          * cancels, ie. they refuse to log in */
        Application( int&, char** ) throw( StubbornUserException, UnsupportedPlatformException );
        ~Application();

        /** when the application exits, the user will be logged out 
          * the verb is "to log out", not "to logout". Demonstrated by, eg. "He
          * logged out", or, "she logs out" */
        void logoutAtQuit() { m_logoutAtQuit = true; }
                       
    private:
        void translate();
    };
}

#endif
