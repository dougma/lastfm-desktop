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

#ifndef LOCAL_CONTENT_CONFIGURATOR_H
#define LOCAL_CONTENT_CONFIGURATOR_H

#include <QSet>
#include <QString>
#include <QStringList>

// this class is for manipulating the sources table of the LocalCollection database
//
class LocalContentConfigurator
{
public:
    struct VolPath
    {
        QString m_vol, m_path;

        VolPath(QString vol, QString path)
            : m_vol(vol)
            , m_path(path)
        {}

        bool operator==(const VolPath& other) const
        {
            return (m_vol == other.m_vol) && (m_path == other.m_path);
        }
    };

private:
    class LocalCollection* m_collection;

    QSet<VolPath> m_sourceSet;

public:
    LocalContentConfigurator();
    ~LocalContentConfigurator();

    int getFileCount();
    int getArtistCount();
    QStringList getScanDirs();
    void changeScanDirs(QStringList dirs);
    void deleteAllFiles();
    void updateVolumeAvailability();
};

int qHash(const LocalContentConfigurator::VolPath &vp);


#endif