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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/


// pull in GetVolumeNameForVolumeMountPoint:
#define _WIN32_WINNT 0x0500
#include <windows.h>

#include <QString>


// return the volume name for a drive letter, or empty string if we can't work it out
//
// drive is a string in the form "C:\"
//
QString
getVolumeName(WCHAR* drive)
{
    UINT driveType = GetDriveTypeW(drive);
    if (driveType == DRIVE_REMOVABLE || driveType == DRIVE_FIXED)
    {
        WCHAR volumeName[256];
        if (GetVolumeNameForVolumeMountPointW(&drive[0], &volumeName[0], 255)) 
            return QString::fromUtf16(volumeName);
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
            return unc.endsWith("\\") ? unc : unc + "\\";
        } 
    }
    else
    {
        // ignore driveType == DRIVE_CDROM (also applies to Dvd-drives)
        // TODO... check to see where usb memory keys and usb hard-drives turn up
    }
    return "";
}

