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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include <QDir>
#include <QString>
#include <QStringList>
#include "LocalCollection.h"

#ifdef WIN32

// pull in GetVolumeNameForVolumeMountPoint:
#define _WIN32_WINNT 0x0500
#include <windows.h>

extern QString getVolumeName(WCHAR* drive);

class FailCriticalErrors
{
public:
    FailCriticalErrors()
    {
        m_old = SetErrorMode(0);
        SetErrorMode(m_old | SEM_FAILCRITICALERRORS);
    }

    ~FailCriticalErrors()
    {
        SetErrorMode(m_old);
    }
private:
    UINT m_old;
};

#endif


QStringList getAvailableVolumes()
{
    QStringList result;

#ifdef WIN32
    // iterate over drive letters A -> Z;
    // look at the drives present and use the
    // volume name if we can (likely for local storage)
    // otherwise a unc is good (for mapped network drives)

    DWORD drives = GetLogicalDrives();      // GetLogicalDrives returns a bitmask of available drive letters
    DWORD mask = 1;
    WCHAR drive[4];
    FailCriticalErrors errMode;   // stop error popup for missing cd-roms, floppies, etc
    for (wcscpy(drive, L"A:\\"); drive[0] <= 'Z'; mask <<= 1, drive[0]++) {
        if (drives & mask) {
            QString volumeName = getVolumeName( drive );
            if (volumeName.length()) {
                result << volumeName;
            }
        }
    }
#endif

    return result;
}


// on windows this is for mapping UNC names and big ugly volume names back to drive letters
// necessary because URLs seem to work better with drive letters...
// eg: \\?\volume{1DD57AA2-D680-4542-8F6C-CC1311D1BEDC}\  --> "C:\"
// on other platforms, it's an identity mapping.
QString 
remapVolumeName(const QString& volume)
{
    QString result = volume;

#ifdef WIN32
    DWORD drives = GetLogicalDrives();
    DWORD mask = 1;
    WCHAR drive[4];
    FailCriticalErrors errMode;   // stop error popup for missing cd-roms, floppies, etc
    for (wcscpy(drive, L"A:\\"); drive[0] <= 'Z'; mask <<= 1, drive[0]++) 
    {
        if (drives & mask) 
        {
            UINT driveType = GetDriveTypeW(drive);
            if (driveType == DRIVE_REMOVABLE || driveType == DRIVE_FIXED)
            {
                WCHAR volumeName[256];
                if (GetVolumeNameForVolumeMountPointW(&drive[0], &volumeName[0], 255))
                {
                    if (QString::fromUtf16(volumeName) == volume) 
                    {
                        result = QString::fromUtf16(drive);
                        break;
                    }
                }
            }
            else if (driveType == DRIVE_REMOTE)
            {
                // WNetGetUniversalName is one ugly call
                #define BUFFERSIZE 512
                char buffer[BUFFERSIZE];
                DWORD bytes = BUFFERSIZE;
                if (NO_ERROR == WNetGetUniversalName(drive, UNIVERSAL_NAME_INFO_LEVEL, buffer, &bytes))
                {
                    UNIVERSAL_NAME_INFO *punc = (UNIVERSAL_NAME_INFO *) &buffer;
                    QString unc = QString::fromUtf16(punc->lpUniversalName);
                    if (!unc.endsWith("\\")) 
                    {
                        unc += "\\";
                    }
                    if (unc == volume) 
                    {
                        result = QString::fromUtf16(drive);
                        break;
                    }
                } 
            }
            else
            {
                // ignore driveType == DRIVE_CDROM (also applies to Dvd-drives)
                int ii = 0;  // todo... check to see where usb memory keys and usb hard-drives turn up
            }
        }
    }
#elif defined(Q_WS_MAC)
    // not necessary on mac...
#endif

    return result;
}

#include <QDebug>
// recognise the sources which don't appear in
// getAvailableVolumes(), but are available
bool
isSourceAvailable(const LocalCollection::Source& s)
{
#ifdef Q_OS_WIN
    return true;
#else
    if( QFile::exists( s.m_volume + s.m_path ))
        return true;
    return false;
#endif
}

