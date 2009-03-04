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

/*! \class LocalCollection
    \brief Represents a connection to the Local Collection db
           Instances of this class should not be shared across threads
           (due to limitations of QSqlDatabase class)
*/

#ifndef LOCAL_COLLECTION_H
#define LOCAL_COLLECTION_H

#include <memory>
#include <QObject>
#include <QMutex>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPair>
#include <QList>
#include <QMap>
#include <QVector>
#include <QStringList>
#include <QVariantList>
#include "ChainableQuery.h"
#include <memory>
#include <boost/function.hpp>

class LocalCollection : public QObject
{
    Q_OBJECT

public:
    enum Creation
    {
        Create, NoCreate
    };

    enum Availablity
    {
        AvailableSources, AllSources
    };

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
        Source(int id, const QString& volume, const QString& path, bool available)
            :m_id(id)
            ,m_volume(volume)
            ,m_path(path)
            ,m_available(available)
        {}

        int m_id;
        QString m_volume;
        QString m_path;
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

    struct FileResult
    {
        QString m_artist;
        QString m_album;
        QString m_title;
        QString m_filename;
        QString m_path;
        QString m_sourcename;
        unsigned m_duration;
    };

    class ResolveResult : public FileMeta
    {
    public:
        ResolveResult(
            const QString& artist, 
            const QString& album, 
            const QString& title, 
            float artistMatch, 
            float titleMatch,
            const QString& filename, 
            unsigned kbps, 
            unsigned duration, 
            const QString& path, 
            const QString& sourcename)
            : FileMeta( artist, album, title, kbps, duration )
            , m_matchQuality ( artistMatch * titleMatch )
            , m_filename( filename )
            , m_path( path )
            , m_sourcename ( sourcename )
        {}

        float m_matchQuality;
        QString m_filename;
        QString m_path;
        QString m_sourcename;
    };

    struct FilesToTagResult
    {
        unsigned fileId;
        QString artist;
        QString album;
        QString title;
    };

    // these types culminate in EntryList, as returned by allTags()
    typedef int ArtistId;
    typedef int TagId;
    typedef float TagWeight;
    typedef QVector< QPair< TagId, TagWeight > > TagVec;
    struct Entry {
        ArtistId artistId;
        TagVec tagVec;
        float norm;
    };
    typedef QList< Entry > EntryList;


    /** Be careful not to use the same connection name on different threads 
      * QSqlDatabase warns against this */
    static LocalCollection* create(QString connectionName);
    ~LocalCollection();

    void versionCheck();

    /** \brief Temp method: Gets a fingerprint id. Returns -1 if none found. */
    QString getFingerprint( const QString& filePath );
    /** \brief Temp method: Sets a fingerprint id. */
    void setFingerprint( const QString& filePath, QString fpId );

    QList<Source> getAllSources();
    int getSourceId( const QString& volume, const QString& path, Creation flag );
    void deleteSource( const QString& volume, const QString &path );
    void setSourceAvailability( int sourceId, bool bAvailable );

    int getArtistId(QString artistName, Creation flag);

    bool getDirectoryId(int sourceId, QString path, int &result);
    bool addDirectory(int sourceId, QString path, int &resultId);
    void removeDirectory(int directoryId);
    QList<Exclusion> getExcludedDirectories(int sourceId);

    QList<File> getFiles(int directoryId);
    void updateFile(int fileId, unsigned lastModified, const FileMeta& info);
    void addFile(int directoryId, QString name, unsigned lastModified, const FileMeta& info);
    void removeFiles(QList<int> ids);
    bool getCounts(int& outArtists, int& outFiles);
    void deleteAllFiles();

    QList<LocalCollection::ResolveResult> resolve(const QString artist, const QString album, const QString title);

    // rql support
    EntryList allTags();
    void filesWithTag(QString tag, Availablity flag, boost::function<void (int, int, float)> /* trackId, artistId, weight */ ); 
    void filesByArtist(QString artist, Availablity flag, boost::function<void (int, int)> /* trackId, artistId */ ); 
    QList<unsigned> filesByArtistId(int artistId, Availablity flag);
    bool getFileById(uint fileId, LocalCollection::FileResult &out);

    // tag handling
    int getTagId(QString tag, Creation flag);
    void setGlobalTagsForArtist(QString artist, QMap<int, QString> globalTags);

    void deleteUserTrackTagsForArtist(int artistId, unsigned userId);
    void deleteGlobalTrackTagsForArtist(int artistId);
    void deleteTrackTagsForArtist(int artistId, unsigned userId);

    void insertUserArtistTag(int artistId, int tagId, unsigned userId);
    void insertGlobalArtistTag(int artistId, int tagId, int weight);
    void insertTrackTag(int artistId, int tagId, unsigned userId, int weight);

    QList<FilesToTagResult> getFilesToTag(int maxTagAgeDays);
    void deleteTrackTags(QVariantList fileIds);
    void updateTrackTags(QVariantList fileIds, QVariantList tagIds, QVariantList weights);
    void setFileTagTime(QVariantList fileIds);
    QVariantList resolveTags(QStringList tagNames);
    QVariantList resolveTags(QStringList tagNames, QMap<QString, int>& map);
    QList< QPair< QString, float > > getTopTags(int limit);

    void transactionBegin();
    void transactionCommit();
    void transactionRollback();
    QMutex* getMutex();

private:
    LocalCollection(QString connectionName);

    typedef std::auto_ptr<ChainableQuery> AutoQueryPtr;

    /** the database version
        * version 1: from 2.?.? */
    int version() const;
    void initDatabase();

    void batch(QVariantList ids, void (LocalCollection::*op)(QString) );
    void deleteTrackTags_batch(QString ids);
    void setFileTagTime_batch(QString ids);

    QSqlDatabase m_db;
    static QMutex ms_activeQueryMutex;
    QString m_dbPath;
    QString m_connectionName;
};


#endif // COLLECTION_H
