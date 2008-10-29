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

#ifdef WIN32
// for FindFirstFileEx and friends:
#define _WIN32_WINNT 0x0400
#include <windows.h>
#endif

#include "SearchLocation.h"
#include <QtCore>



#ifdef WIN32

static
unsigned 
fileTimeToUnixTime32(FILETIME * pft)
{
    // http://support.microsoft.com/kb/167296 for magic numbers:
    const unsigned __int64 intervals_per_second = 10000000;     // an interval is 100 nanoseconds
    const unsigned __int64 epoc_adjust = 116444736000000000;    // 100-nanosecondses between 1/1/1601 and 1/1/1970
    return (*(unsigned __int64*)pft - epoc_adjust ) / intervals_per_second;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////


SearchLocation::SearchLocation(const LocalCollection::Source& src, const QString& startPath, Exclusions& exclusions)
:m_src(src)
,m_startPath(startPath)
,m_exclusions(exclusions)
{
}

const LocalCollection::Source& 
SearchLocation::source() const
{
    return m_src;
}

// just mp3s now.. others todo
SearchLocation::FileTimeMap 
SearchLocation::audioFiles(const QString& path) const
{
    FileTimeMap result;

#ifdef WIN32
    // 2 problems: QDir is v.slow on windows and doesn't seem to handle \\volume\?\ paths
    WIN32_FIND_DATAW data;
    QString match(m_src.m_volume + path + "*.mp3");
    HANDLE hFind = FindFirstFileW(match.utf16(), &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            const unsigned skip = FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY;
            if (data.dwFileAttributes & skip)
                continue;

            if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                unsigned t = fileTimeToUnixTime32(&data.ftLastWriteTime);   // NT's FILETIME times are UTC
                result[QString::fromUtf16(&data.cFileName[0])] = t;
            }
        } while (FindNextFileW(hFind, &data));
        FindClose(hFind);
    }
#else
    QStringList nameFilters;
    nameFilters << "*.mp3";
    QFileInfoList list(
        QDir(m_src.m_volume + path).
            entryInfoList(nameFilters, QDir::Files | QDir::Readable));

	foreach (const QFileInfo& i, list) {
        map[i.fileName()] = i.lastModified().toUTC().toTime_t();
	}
#endif

    return result;
}

//static
DirList 
SearchLocation::subdirectories(const QString& path)
{
#ifdef WIN32
    DirList result;
    WIN32_FIND_DATAW data;
    HANDLE hFind = FindFirstFileExW(
        (path + "*").utf16(), 
        FindExInfoStandard,
        &data,
        FindExSearchLimitToDirectories,
        NULL,
        0 );
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            const unsigned skip = FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY;
            if (data.dwFileAttributes & skip)
                continue;

            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                wcscmp(L".", data.cFileName) != 0 &&
                wcscmp(L"..", data.cFileName) != 0)
            {
                result << QString::fromUtf16(&data.cFileName[0]);
            }

        } while (FindNextFileW(hFind, &data));
        FindClose(hFind);
    }
    return result;
#else
    return QDir(path).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
#endif
}

