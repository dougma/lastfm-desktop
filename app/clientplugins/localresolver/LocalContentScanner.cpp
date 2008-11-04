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


#include <QThreadPool>
#include "LocalContentScanner.h"
#include "LocalCollection.h"
#include "MediaMetaInfo.h"
#include "SearchLocation.h"
#include <memory>

// we make use of QThreadPool priorities to allow some tasks to queue jump
#define PRIORITY_VOLUMEAVAILABLE 0
#define PRIORITY_FULLSCAN 1

QStringList getAvailableVolumes();
bool isVolumeImplicitlyAvailable(const QString& volume);

LocalContentScanner::LocalContentScanner()
    : m_col( LocalCollection::instance() )
    , m_bStopping( false )
{
    m_pool = new QThreadPool();
    m_pool->setMaxThreadCount(1);       // todo: LocalCollection needs work before removing this line.
    startFullScan();
}

LocalContentScanner::~LocalContentScanner()
{
    m_bStopping = true;     // cause runnables to exit
    delete m_pool;          // waits for queued runnables to exit
}


void 
LocalContentScanner::startFullScan()
{
    QStringList volumes = getAvailableVolumes();

    foreach(const LocalCollection::Source &src, m_col.getAllSources()) 
    {
        bool available = volumes.contains(src.m_volume) || isVolumeImplicitlyAvailable(src.m_volume);
        if (available != src.m_available) {
            m_col.setSourceAvailability(src.m_id, available);
        }
        if (available) {
            Exclusions exclusions(m_col.getExcludedDirectories(src.m_id));
            QStringList startdirs(m_col.getStartDirectories(src.m_id));
            if (startdirs.isEmpty()) {
                startdirs << "";    // scan from the root
            }
            foreach (const QString &dir, startdirs) {
                m_pool->start(
                    new FullScan(SearchLocation(src, dir, exclusions), this),
                    PRIORITY_FULLSCAN );
            }
            volumes.removeAll(src.m_volume);
        }
    }

    // remaining entries in 'volumes' are new
    bool bAddNewVolumesAutomatically = true;    // todo... optional?
    if (bAddNewVolumesAutomatically) {
        foreach(const QString v, volumes) {
            m_col.addSource(v);
        }
    }
}

void
LocalContentScanner::test_initSources()
{
    int id;
    QStringList volumes = getAvailableVolumes();
    foreach(QString v, volumes) {
        id = m_col.addSource(v);
    }
}

// reconcile the files on disk (at path, under SearchLocation sl) with the files in the db
//
void
LocalContentScanner::dirScan(const SearchLocation& sl, const QString& path)
{
    const int sourceId = sl.source().m_id;
	const QString fullPath(sl.source().m_volume + path);

	emit dirScanStart(sl, fullPath);

    SearchLocation::FileTimeMap map(sl.audioFiles(path));

    int directoryId;
    bool dirInDb = m_col.getDirectoryId(sourceId, path, directoryId);
    if (map.isEmpty()) {
        if (dirInDb)
            m_col.removeDirectory(directoryId);
        return;
    }

    if (dirInDb || m_col.addDirectory(sourceId, path, directoryId)) {
		// loop over all files (for this directory) in the db:
		// rescan those that are newer on disk,
		// and build a list of those missing from disk

		QList<int> missing;		// missing files (db id's)

		foreach (const LocalCollection::File& file, m_col.getFiles(directoryId)) {
			SearchLocation::FileTimeMap::iterator it(map.find(file.name()));
			if (it == map.end()) {
				// file not found in this directory
				missing << file.id();
			} else {
				if (stopping()) break;	// bail out now because a rescan can be slow

				if (file.lastModified() < it.value()) {
					// file on disk is newer than our db entry
					oldFileRescan(
						fullPath + file.name(),
						file.id(),
						it.value());
				}
				map.erase(it);       // done
			}
		}

		// files not found on disk, are removed from the db:
        m_col.removeFiles(missing);

		// files remaining in the map are new:
        for (SearchLocation::FileTimeMap::const_iterator it = map.constBegin(); 
            it != map.constEnd() && !stopping(); 
            it++) 
        {
			newFileScan(
				fullPath,
				it.key(),       // filename
				directoryId,
				it.value());    // last modified time
		}
    }
}

void 
LocalContentScanner::newFileScan(const QString& fullpath, const QString& filename, int directoryId, unsigned lastModified)
{
    try {
        bool good = false;
        QString pathname(fullpath + filename);
        emit fileScanStart(pathname);
        try {
            std::auto_ptr<MediaMetaInfo> p( MediaMetaInfo::create( pathname ) );
            MediaMetaInfo *info = p.get();
            if (info) {
                m_col.addFile(
					directoryId,
					filename,
					lastModified, 
                    LocalCollection::FileMeta(
                        info->artist(), info->album(), info->title(), info->kbps(), info->duration() ));
                good = true;
            }
        }
        catch (...) {
            exception("NewFileScan::run scanning file");
        }
        emit fileScanFinished(pathname, good);
    } 
    catch (...) {
        exception("NewFileScan::run signalling");
    }
}

void
LocalContentScanner::oldFileRescan(const QString& pathname, int fileId, unsigned lastModified)
{
    try {
        bool good = false;
        emit fileScanStart(pathname);
        try {
            std::auto_ptr<MediaMetaInfo> p(MediaMetaInfo::create(pathname));
            MediaMetaInfo *info = p.get();
            if (info) {
                m_col.updateFile(
					fileId, 
					lastModified, 
                    LocalCollection::FileMeta(
                        info->artist(), info->album(), info->title(), info->kbps(), info->duration() ));
                good = true;
            }
        }
        catch (...) {
            exception("OldFileRescan::run scanning file");
        }
        emit fileScanFinished(pathname, good);
    } 
    catch (...) {
        exception("OldFileRescan::run signalling");
    }
}

void 
LocalContentScanner::exception(const char *msg) const
{
    // todo: something useful
    msg;
}


////////////////////////////////////////////////////////////////////

LocalContentScanner::FullScan::FullScan(const SearchLocation& sl, LocalContentScanner* lcs)
:m_sl(sl)
,m_lcs(lcs)
{}

void 
LocalContentScanner::FullScan::run()
{
    bool completed;
	try {
        completed = m_sl.recurseDirs(*this);
	} catch (...) {
		m_lcs->exception("Fullscan::run");
	}
    emit m_lcs->fullScanFinished(m_sl, completed);
}

// this is the callback for the recurseDirs call in FullScan::run
// bridging the SearchLocation object back to the LocalContentScanner
bool
LocalContentScanner::FullScan::operator()(const QString &path)
{
    if (m_lcs->stopping())
        return false;
    m_lcs->dirScan(m_sl, path);
    return true;
}
