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

#ifndef MOOSE_SETTINGS_H
#define MOOSE_SETTINGS_H

#include "TrackInfo.h"
#include "lib/unicorn/Settings.h"
#include "MooseDllExportMacro.h"
#include <QStringList>


namespace Moose
{
    //TODO use constant for organisation and application name and stick them in 
    // Moose, then use same for main setOrgName etc.
    typedef Unicorn::QSettings QSettings;

    class MOOSE_DLLEXPORT Settings : public Unicorn::Settings
    {
    public:
        /** The AudioScrobbler executable location */
        QString path() const { return QSettings().value( "Path" ).toString(); }
        /** The AudioScrobbler version number */
        QString version() const { return QSettings().value( "Version", "unknown" ).toString(); }

        // used by TrackInfo
        //TODO shouldn't be necessary
        /** is percentage */
        //TODO percentage is confusing in implementations, use float
        int scrobblePoint() const { return QSettings().value( "ScrobblePoint", TrackInfo::kDefaultScrobblePoint ).toInt(); }

        // used by Moose::sendToInstance
        // needed by Twiddly
        int  controlPort() const { return QSettings().value( "ControlPort", 32213 ).toInt(); }

        // used by TrackInfo
        // needed by Twiddly as it uses TrackInfo::isScrobblable()
        QStringList excludedDirs() const
        {
            QStringList paths = QSettings( ).value( "ExclusionDirs" ).toStringList();
            paths.removeAll( "" );
            return paths;
        }
    };
}

#endif
