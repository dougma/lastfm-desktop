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

#ifndef SEARCH_LOCATION_H
#define SEARCH_LOCATION_H

#include <QtCore>
#include "LocalCollection.h"


typedef QStringList DirList;
typedef QList<LocalCollection::Exclusion> Exclusions;


// encapsulates the directory recursing and file finding business
// coupled to the types of LocalCollection
//
class SearchLocation
{
    LocalCollection::Source m_src;
    QString m_startPath;
    Exclusions m_exclusions;

public:
    SearchLocation(const LocalCollection::Source& src, const QString& startPath, const Exclusions& exclusions);

    // for mapping file names to their last modified times:
    typedef QMap<QString, unsigned> FileTimeMap;

    FileTimeMap audioFiles(const QString& path) const;
    const LocalCollection::Source& source() const;

    // Recurse into directories and subdirectories, beginning at 
    // m_startPath and calling the callback functor for each
    // directory entered.
    // Callback receives path for each directory entered, 
    // and returns false to abort the scan.
    // Directories in the exclusion list (and optionally, 
    // their subdirectories) are skipped.
    // recurseDirs returns false if it was aborted by the callback.
    template<typename T>
    bool recurseDirs(T callback) const
    {
        return _recurseDirs(m_startPath, callback);
    }

private:
    template<typename T>
    bool _recurseDirs(const QString& path, T callback) const
    {
        int idx = m_exclusions.indexOf(LocalCollection::Exclusion(path));
        bool excluded = idx != -1;
        if (excluded && m_exclusions.at(idx).subdirsToo()) {
            return true;
        }

        if (!excluded) {
            if (!callback(path))
                return false;
        }

        foreach(const QString& dir, subdirectories(m_src.m_volume + path)) {
            if (!_recurseDirs(path + dir + QDir::separator(), callback))
                return false;
        }

        return true;
    }

    static DirList subdirectories(const QString& path);
};

typedef QList<SearchLocation> SearchLocationList;

#endif