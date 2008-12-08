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
#include "QueryError.h"
#include <memory>
#include <QDebug>


// we make use of QThreadPool priorities to allow some tasks to queue jump
#define PRIORITY_INIT 0
#define PRIORITY_VOLUMEAVAILABLE 1
#define PRIORITY_FULLSCAN 2

QStringList getAvailableVolumes();
bool isVolumeImplicitlyAvailable(const QString& volume);

LocalContentScanner::LocalContentScanner()
    : m_pCol( 0 )
    , m_bStopping( false )
{
    m_pool = new QThreadPool();
    // just 1 thread in the pool because the db connection 
    // shouldn't be shared between threads
    m_pool->setMaxThreadCount(1);       
    m_pool->start(new Init(this), PRIORITY_INIT);
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

    foreach(const LocalCollection::Source &src, m_pCol->getAllSources()) {
        bool available = volumes.contains(src.m_volume) || isVolumeImplicitlyAvailable(src.m_volume);
        if (available != src.m_available) {
            m_pCol->setSourceAvailability(src.m_id, available);
        }
        if (available) {
            Exclusions exclusions(m_pCol->getExcludedDirectories(src.m_id));
            QStringList startdirs(m_pCol->getStartDirectories(src.m_id));
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
            LocalCollection::Source src(m_pCol->addSource(v));
            m_pool->start(
                new FullScan(SearchLocation(src, "", Exclusions()), this),
                PRIORITY_FULLSCAN );
        }
    }
}

// reconcile the files on disk (at path, under SearchLocation sl) with the files in the db
//
void
LocalContentScanner::dirScan(const SearchLocation& sl, const QString& path)
{
    qDebug() << path;
    
    const int sourceId = sl.source().m_id;
	const QString fullPath(sl.source().m_volume + path);

	emit dirScanStart(sl, fullPath);

    SearchLocation::FileTimeMap map(sl.audioFiles(path));

    int directoryId;
    bool dirInDb = m_pCol->getDirectoryId(sourceId, path, directoryId);
    if (map.isEmpty()) {
        if (dirInDb)
            m_pCol->removeDirectory(directoryId);
        return;
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
					oldFileRescan(
						fullPath + file.name(),
						file.id(),
						it.value());
				}
				map.erase(it);       // done
			}
		}

		// files not found on disk, are removed from the db:
        m_pCol->removeFiles(missing);

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
                m_pCol->addFile(
					directoryId,
					filename,
					lastModified, 
                    LocalCollection::FileMeta(
                        info->artist(), info->album(), info->title(), info->kbps(), info->duration() ));
                good = true;
            }
        }
        catch (QueryError &e) {
            exception("QueryError: " + e.text());
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
                m_pCol->updateFile(
					fileId, 
					lastModified, 
                    LocalCollection::FileMeta(
                        info->artist(), info->album(), info->title(), info->kbps(), info->duration() ));
                good = true;
            }
        }
        catch (QueryError& e) {
            exception("QueryError: " + e.text());
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
LocalContentScanner::exception(const QString& msg) const
{
    qCritical() << msg;
}

void
LocalContentScanner::init()
{
    m_pCol = LocalCollection::create("LocalContentScanner");
    startFullScan();
}

////////////////////////////////////////////////////////////////////

LocalContentScanner::Init::Init(LocalContentScanner* lcs)
:m_lcs(lcs)
{}

void 
LocalContentScanner::Init::run()
{
	try {
        m_lcs->init();
    } catch (QueryError& e) {
        m_lcs->exception("QueryError: " + e.text());
	} catch (...) {
		m_lcs->exception("unknown exception in ThreadInit::run");
	}
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
        QThread::currentThread()->setPriority(QThread::LowPriority);
        completed = m_sl.recurseDirs(*this);
    } catch (QueryError& e) {
        m_lcs->exception(" " + e.text());
	} catch (...) {
		m_lcs->exception("unknown exception in Fullscan::run");
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
