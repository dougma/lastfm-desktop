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

#include <QThreadPool>
#include "LocalContentScanner.h"
#include "LocalCollection.h"
#include "AutoTransaction.h"
#include "MediaMetaInfo.h"
#include "SearchLocation.h"
#include "QueryError.h"
#include <memory>
#include <QDebug>
#include "boost/bind.hpp"


LocalContentScanner::LocalContentScanner()
    : m_pCol( 0 )
    , m_bStopping( false )
{
}

LocalContentScanner::~LocalContentScanner()
{
    Q_ASSERT(m_bStopping);      // probably you want to stop it first
}

void
LocalContentScanner::stop()
{
    m_bStopping = true;
}

void
LocalContentScanner::run()
{
    m_pCol = LocalCollection::create("LocalContentScanner");
    announceStarted();
    startFullScan();
    m_bStopping = true;
    emit finished();
}

void 
LocalContentScanner::announceStarted()
{
    QStringList scanLocations;

    foreach(const LocalCollection::Source &src, m_pCol->getAllSources()) {
        if (src.m_available) {
            scanLocations << (src.m_volume + src.m_path);
        }
    }

    emit started( scanLocations );
}


void 
LocalContentScanner::startFullScan()
{
    foreach(const LocalCollection::Source &src, m_pCol->getAllSources()) {
        if (src.m_available) {
            Exclusions exclusions( m_pCol->getExcludedDirectories( src.m_id ) );
            if (stopping()) return;
            scan( SearchLocation( src, src.m_path, exclusions ) );      // todo: fix SeachLocation constructor
        }
    }
}

void
LocalContentScanner::scan(const SearchLocation& sl)
{
    sl.recurseDirs(boost::bind(
        &LocalContentScanner::dirScan,
        this,
        sl,
        _1) );
}


// reconcile the files on disk (at path, under SearchLocation sl) with the files in the db
//
// returns true to indicate stopping
//
bool
LocalContentScanner::dirScan(const SearchLocation& sl, const QString& path)
{
    const int sourceId = sl.source().m_id;
	const QString fullPath(sl.source().m_volume + path);

	emit dirScanStart(fullPath);

    SearchLocation::FileTimeMap map(sl.audioFiles(path));

    int directoryId;
    bool dirInDb = m_pCol->getDirectoryId(sourceId, path, directoryId);
    if (map.isEmpty()) {
        if (dirInDb)
            m_pCol->removeDirectory(directoryId);
        return true;
    }

    if (dirInDb || m_pCol->addDirectory(sourceId, path, directoryId)) {
		// loop over all files (for this directory) in the db:
		// rescan those that are newer on disk,
		// and build a list of those missing from disk

		QList<int> missing;		// missing files (db id's)

		foreach (const LocalCollection::File& file, m_pCol->getFiles(directoryId)) {
			SearchLocation::FileTimeMap::iterator it(map.find(file.name()));
			if (it == map.end()) {
				// file not found in this directory
				missing << file.id();
			} else {
				if (stopping()) break;	// bail out now because a rescan can be slow

				if (file.lastModified() < it.value()) {
					// file on disk is newer than our db entry
                    oldFileRescan( fullPath + file.name(), file.id(), it.value() );
				}
				map.erase(it);       // done
			}
		}

		// files not found on disk, are removed from the db:
        m_pCol->removeFiles(missing);

		// files remaining in the map are new:

        // this bit is a little bit optimised:
        // 1. newFileScan returns an AddFileFunc object to perform the db insert
        // 2. build up a list of AddFileFuncs
        // 3. call them all within a transaction

        QList<AddFileFunc> ops;
        for (SearchLocation::FileTimeMap::const_iterator it = map.constBegin(); 
            it != map.constEnd() && !stopping(); 
            it++) 
        {
            ops << newFileScan( fullPath, it.key() /* filename */, it.value() /* last modified time */ );
		}

        {
            QMutexLocker locker( m_pCol->getMutex() );
            AutoTransaction<LocalCollection> trans( *m_pCol );
            foreach ( const AddFileFunc& func, ops ) {
                func( m_pCol, directoryId );
            }
            trans.commit();
        }
    }
    return !stopping();
}


void doNothing(LocalCollection *, int)
{
}


LocalContentScanner::AddFileFunc
LocalContentScanner::newFileScan(const QString& fullpath, const QString& filename, unsigned lastModified)
{
    AddFileFunc result = boost::bind(&doNothing, _1, _2);

    try {
        bool good = false;
        Track track;

        QString pathname(fullpath + filename);
        emit fileScanStart(pathname);
        try {
            std::auto_ptr<MediaMetaInfo> p( MediaMetaInfo::create( pathname ) );
            MediaMetaInfo *info = p.get();
            if (info) {
                LocalCollection::FileMeta fileMeta( info->artist(), info->album(), info->title(), info->kbps(), info->duration() );

                result = boost::bind(
                    &LocalCollection::addFile, 
                    _1, /* LocalCollection* */
                    _2, /* int directoryId */
					filename,
					lastModified, 
                    fileMeta);
                        
                track = mediaMetaToTrack( info, pathname );
                good = true;
            }
        }
        catch (QueryError &e) {
            exception("QueryError: " + e.text());
        }
        catch (...) {
            exception("NewFileScan::run scanning file");
        }

        if (good) {
            emit trackScanned( track );
        }
    } 
    catch (...) {
        exception("NewFileScan::run signalling");
    }
    return result;
}

void
LocalContentScanner::oldFileRescan(const QString& pathname, int fileId, unsigned lastModified)
{
    try {
        bool good = false;
        Track track;

        emit fileScanStart(pathname);
        try {
            std::auto_ptr<MediaMetaInfo> p( MediaMetaInfo::create(pathname) );
            MediaMetaInfo *info = p.get();
            if (info) {
                m_pCol->updateFile(
					fileId, 
					lastModified, 
                    LocalCollection::FileMeta(
                        info->artist(), info->album(), info->title(), info->kbps(), info->duration() ));
                track = mediaMetaToTrack( info, pathname );
                good = true;
            }
        }
        catch (QueryError& e) {
            exception("QueryError: " + e.text());
        }
        catch (...) {
            exception("OldFileRescan::run scanning file");
        }

        if (good) {
            emit trackScanned( track );
        }
    } 
    catch (...) {
        exception("OldFileRescan::run signalling");
    }
}

Track
LocalContentScanner::mediaMetaToTrack(MediaMetaInfo *i, QString file)
{
    MutableTrack t;
    t.setArtist( i->artist() );
    t.setAlbum( i->album() );
    t.setTitle( i->title() );
    t.setDuration( i->duration() );
    t.setUrl( QUrl::fromLocalFile( file ) );
    return t;
}


void 
LocalContentScanner::exception(const QString& msg) const
{
    qCritical() << msg;
}

