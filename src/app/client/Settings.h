/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#ifndef AUDIOSCROBBLER_SETTINGS_H
#define AUDIOSCROBBLER_SETTINGS_H

#include "lib/moose/Settings.h"

int main( int, char** );
class Settings;
namespace The { Settings& settings(); }


class Settings : public Moose::Settings
{
    Settings( const QString& version, const QString& path );

    static Settings* instance;
    friend int main( int, char** );
    friend Settings& The::settings();

protected:
    Settings( const Settings& )
    {
        // we appear odd, but it enforces encapsulation, since the compiler
        // requires an instance of Settings to use this ctor, and only main()
        // can create one, well you could cast something else, but we may 
        // actually require something from the Settings instance at some point
        // and then your code will crash :P
    }

public:
    QByteArray containerGeometry() const { return QSettings().value( "MainWindowGeometry" ).toByteArray(); }
    Qt::WindowState containerWindowState() const { return (Qt::WindowState) QSettings().value( "MainWindowState" ).toInt(); }
    bool logOutOnExit() const { return QSettings().value( "LogOutOnExit", false ).toBool(); }

private:
    bool m_weWereJustUpgraded;
};


class MutableSettings : private Settings
{
public:
    MutableSettings( const Settings& that ) : Settings( that )
    {}

    void setLogOutOnExit( bool b ) { QSettings().setValue( "LogOutOnExit", b ); }
    void setControlPort( int v ) { QSettings().setValue( "ControlPort", v ); }
    void setScrobblePoint( int scrobblePoint ) { QSettings().setValue( "ScrobblePoint", scrobblePoint ); }
    void setContainerWindowState( int state ) { QSettings().setValue( "MainWindowState", state ); }
    void setContainerGeometry( QByteArray state ) { QSettings().setValue( "MainWindowGeometry", state ); }
};


namespace The
{
    inline Settings& settings()
    {
        return *Settings::instance;
    }
}

#endif