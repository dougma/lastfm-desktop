/***************************************************************************
 *   Copyright (C) 2007 by                                                 *
 *      Last.fm Ltd <client@last.fm>                                       *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

/*! \class LocalCollection
    \brief Local Content Resolution db interface.
    Db stores metadata for on-disk media
*/

#ifndef LOCOCOLLECTION_H
#define LOCOCOLLECTION_H

#include <QObject>
#include <QMutex>
#include <QSqlDatabase.h>
#include <QSqlQuery>
#include <QSqlError>


class LocalCollection : public QObject
{
    Q_OBJECT

public:
    class File
    {
    public:
        File(int id, QString name, unsigned lastModified)
            :m_id(id)
            ,m_name(name)
            ,m_lastModified(lastModified)
        {}

        int id() const { return m_id; }
        QString name() const { return m_name; }
        unsigned lastModified() const { return m_lastModified; }

    private:
        int m_id;
        QString m_name;
        unsigned m_lastModified;
    };

    class Source
    {
    public:
        Source(int id, const QString& volume, bool available)
            :m_id(id)
            ,m_volume(volume)
            ,m_available(available)
        {}

        int m_id;
        QString m_volume;
        bool m_available;
    };

    class Exclusion
    {
        QString m_path;
        bool m_bSubdirs;    // subdirectories excluded too

    public:
        Exclusion(const QString path, bool bSubdirs = true);
        bool operator==(const Exclusion& that) const;
        bool subdirsToo() const;
    };

    class FileMeta
    {
    public:
        FileMeta(const QString& artist, const QString& album, const QString& title, 
            unsigned kbps, unsigned duration)
            : m_artist( artist )
            , m_album( album )
            , m_title( title )
            , m_kbps( kbps )
            , m_duration ( duration )
        {}

        QString m_artist;
        QString m_album;
        QString m_title;
        unsigned m_kbps;
        unsigned m_duration;
    };

    class ResolveResult : public FileMeta
    {
    public:
        ResolveResult(
            const QString& artist, const QString& album, const QString& title, const QString& filename, 
            unsigned kbps, unsigned duration, const QString& path, const QString& sourcename)
            : FileMeta( artist, album, title, kbps, duration )
            , m_filename( filename )
            , m_path( path )
            , m_sourcename ( sourcename )
        {}

        QString m_filename;
        QString m_path;
        QString m_sourcename;
    };



    /** \brief Returns the singleton instance to the controller. */
    static LocalCollection&
    instance();

    /** \brief Terminates and deletes the collection instance. */
    void
    destroy();

    /** \brief Temp method: Gets a fingerprint id. Returns -1 if none found. */
    QString
    getFingerprint( const QString& filePath );

    /** \brief Temp method: Sets a fingerprint id. */
    bool
    setFingerprint( const QString& filePath, QString fpId );

    int
    addSource(const QString& volume);

    QList<Source> 
    getAllSources();

    void
    setSourceAvailability(int sourceId, bool bAvailable);

    bool
    removeSource();

    bool 
    getSourceId(QString name, int &result);

    bool 
    getDirectoryId(int sourceId, QString path, int &result);

    bool 
    addDirectory(int sourceId, QString path, int &resultId);

    void
    removeDirectory(int directoryId);

    QList<Exclusion>
    getExcludedDirectories(int sourceId);

    QList<QString>
    getStartDirectories(int sourceId);

    QList<File> 
    getFiles(int directoryId);

    bool 
    updateFile(int fileId, unsigned lastModified, const FileMeta& info);

    bool
    addFile(int directoryId, QString name, unsigned lastModified, const FileMeta& info);

    void
    removeFiles(QList<int> ids);

    QList<LocalCollection::ResolveResult>
    resolve(const QString artist, const QString album, const QString title);

private:
    LocalCollection();
    ~LocalCollection();

    /** the database version
        * version 1: from 2.?.? */
    int
    version() const;

    bool
    initDatabase();

    bool
    query( const QString& queryToken );

    QString
    fileURI( const QString& filePath );

    static LocalCollection* s_instance;

    QMutex m_mutex;
    QSqlDatabase m_db;
    QString m_dbPath;
};

#endif // COLLECTION_H
