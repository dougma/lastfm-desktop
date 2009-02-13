/***************************************************************************
 *   Copyright 2008-2009 Last.fm Ltd.                                      *
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

#include "LocalContentConfig.h"
#include "LocalCollection.h"
#include <QDir>

//todo: header file for these:
extern QString remapVolumeName(const QString& volume);
QStringList getAvailableVolumes();
bool isVolumeImplicitlyAvailable(const QString& volume);


int qHash(const LocalContentConfig::VolPath &vp)
{
    return qHash(vp.m_vol + vp.m_path);
}


static
QString
removeLeadingSlash(QString s)
{
    return (s.length() && s[0] == QDir::separator()) ? s.mid(1) : s;
}


#ifdef Q_OS_WIN
#include <stdlib.h>
#include <windows.h>

extern QString getVolumeName(WCHAR* drive);

QString trimTrailingSlashes(const QString& s)
{
    QString result = s;
    while (result.endsWith("\\")) {
        result = result.left( result.length() - 1 );
    }
    return result;
}


// for windows: split path and return a pair of volume-name and path
//
static
QPair<QString, QString> 
splitPath(const QString& p)
{
    WCHAR drive[_MAX_DRIVE + 2];
    WCHAR dir[_MAX_DIR];

    // we need to add a trailing slash so _wsplitpath_s takes the last 
    // portion as a directory name and not a filename

    QString pp = p.endsWith("\\") ? p : (p + "\\");

    errno_t err = _wsplitpath_s( pp.utf16(), 
        drive, _MAX_DRIVE,
        dir, _MAX_DIR,
        NULL, 0,
        NULL, 0);

    QString volume, path;
    if (!err) {
        volume = getVolumeName( wcscat(drive, L"\\") );
        path = removeLeadingSlash( QString::fromUtf16( dir ) );
    }

    return QPair<QString, QString>( volume, path );
}

#else

// a (do-very-little) path splitting function for unix friends
//
static
QPair<QString, QString> 
splitPath(const QString& path)
{
    // need a trailing slash on the path component
    QString p = path.endsWith("/") ? path : ( path + "/" );
    return QPair<QString, QString>( QString("/"), removeLeadingSlash( p ) );
}

#endif


LocalContentConfig::LocalContentConfig()
{
    m_collection = LocalCollection::create("LocalContentConfig");
}

LocalContentConfig::~LocalContentConfig()
{
    delete m_collection;
}

QStringList
LocalContentConfig::getScanDirs()
{
    QStringList result;
    m_sourceSet.clear();

    QList<LocalCollection::Source> sources = m_collection->getAllSources();
    foreach (const LocalCollection::Source &src, sources) {
        m_sourceSet.insert( VolPath( src.m_volume, src.m_path ) );
        QString remappedVolume = remapVolumeName( src.m_volume );
        result << remappedVolume + src.m_path;
    }

    return result;
}

void
LocalContentConfig::setScanDirs(QStringList dirs)
{
    foreach (const QString& dir, dirs) {
        QPair<QString, QString> split = splitPath( dir );
        VolPath key( split.first, split.second );
        if (m_sourceSet.contains( key )) {
            // it's a known source, remove it from the set
            m_sourceSet.remove( key );
        } else {
            // it's a new source; create it:
            m_collection->getSourceId( split.first, split.second, LocalCollection::Create );
        }
    }

    // remaining VolPath objects are ones the user no longer wants.  :(
    foreach (const VolPath& vp, m_sourceSet) {
        m_collection->deleteSource( vp.m_vol, vp.m_path );
    }
    m_sourceSet.clear();
}



void
LocalContentConfig::updateVolumeAvailability()
{
    QStringList volumes = getAvailableVolumes();

    foreach(const LocalCollection::Source &src, m_collection->getAllSources()) {
        bool available = volumes.contains( src.m_volume ) || isVolumeImplicitlyAvailable( src.m_volume );
        if (available != src.m_available) {
            m_collection->setSourceAvailability( src.m_id, available );
        }
    }
}