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

#ifndef CLIENT_SETTINGS_H
#define CLIENT_SETTINGS_H

#include "lib/moose/MooseSettings.h"


/** usage, only put interesting settings in here, which mostly means ones set in
  * in the settings dialog. For class local settings, just make a small 
  * QSettings local derived class */
class Settings : public Moose::Settings
{
    Settings( const QString& version, const QString& path );

    friend class App;

private:
    bool m_weWereJustUpgraded;
};


class MutableSettings : public Unicorn::MutableSettings
{
public:
    MutableSettings( const ::Settings& )
    {}

    void setControlPort( int v ) { QSettings().setValue( "ControlPort", v ); }
    void setScrobblePoint( int scrobblePoint ) { QSettings().setValue( "ScrobblePoint", scrobblePoint ); }
};

#endif
