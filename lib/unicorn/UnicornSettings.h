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

#ifndef UNICORN_SETTINGS_H
#define UNICORN_SETTINGS_H

#include <QSettings>
#include <QString>


namespace unicorn
{
    /** Clearly no use until a username() has been assigned. But this is
      * automatic if you use unicorn::Application anyway. */
    class UserSettings : public QSettings
    {
    public:
        UserSettings()
        {
            QString const username = value( "Username" ).toString();
            beginGroup( username );
            // it shouldn't be possible, since unicorn::Application enforces
            // assignment of the username parameter before anything else
            Q_ASSERT( !username.isEmpty() );
        }

        static const char* subscriptionKey() { return "subscription"; }
    };
}

#endif
