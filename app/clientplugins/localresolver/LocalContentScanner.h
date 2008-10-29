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

#ifndef LOCAL_CONTENT_SCANNER_H
#define LOCAL_CONTENT_SCANNER_H

#include <QRunnable>
#include "SearchLocation.h"
//#include "MediaMetaInfo.h"


class LocalContentScanner : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LocalContentScanner)

    volatile bool m_bStopping;
    class QThreadPool* m_pool;
    class LocalCollection& m_col;

    class ChangeNotification : public QRunnable
    {
    public:
        ChangeNotification();
        void run();

    private:
        SearchLocation m_sl;
    };

    class FullScan : public QRunnable
    {
    public:
        FullScan(const SearchLocation&, LocalContentScanner *);
        void run();
        bool operator()(const QString& path);

    private:
        SearchLocation m_sl;
        LocalContentScanner *m_lcs;
    };

    void startFullScan();
    void dirScan(const SearchLocation& sl, const QString& path);
    void newFileScan(const QString& fullpath, const QString& filename, int directoryId, unsigned lastModified);
    void oldFileRescan(const QString& pathname, int fileId, unsigned lastModified);
    void exception(const char *msg) const;
    bool stopping() { return m_bStopping; }
    LocalCollection& collection() { return m_col; }

public:
    LocalContentScanner();
    ~LocalContentScanner();

    void test_initSources();

signals:
    void fullScanStart(const SearchLocation&);
    void fullScanFinished(const SearchLocation&, bool);
    void dirScanStart(const SearchLocation&, const QString&);
    void fileScanStart(const QString&);
    void fileScanFinished(const QString&, bool);
};

#endif
