
#include "TrackInfo.h"
#include "lib/unicorn/Settings.h"
#include "common/DllExportMacro.h"

#ifndef MOOSE_SETTINGS_H
#define MOOSE_SETTINGS_H


namespace Moose
{
    //TODO use constant for organisation and application name and stick them in 
    // Moose, then use same for main setOrgName etc.
    typedef Unicorn::Settings QSettings;

    class DLLEXPORT Settings : public Unicorn::QSettings
    {
    public:
        /** The AudioScrobbler executable location */
        QString path() const { return QSettings().value( "Path" ).toString(); }
        /** The AudioScrobbler version number */
        QString version() const { return QSettings().value( "Version", "unknown" ).toString(); }

        // used by TrackInfo
        //TODO shouldn't be necessary
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
