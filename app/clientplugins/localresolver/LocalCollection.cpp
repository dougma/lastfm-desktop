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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "LocalCollection.h"
#include "lib/lastfm/core/CoreDir.h"
#include <QStringList>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QVariant>


static const int k_dbVersion = 1;

// Singleton instance needs to be initialised
LocalCollection* LocalCollection::s_instance = NULL;


LocalCollection::LocalCollection()
{
    m_dbPath = CoreDir::data().path() + "/LocalCollection.db";
    initDatabase();
}


LocalCollection::~LocalCollection()
{
    m_db.close();
    m_db = QSqlDatabase(); // to make the db "not in use" (removes a qt warning)
    QSqlDatabase::removeDatabase( "LocalCollection" );
}


LocalCollection&
LocalCollection::instance()
{
    static QMutex mutex;
    QMutexLocker locker( &mutex );

    if ( !s_instance )
    {
        s_instance = new LocalCollection;
    }

    return *s_instance;
}


bool
LocalCollection::initDatabase()
{
    QMutexLocker locker_q( &m_mutex );

    if ( !m_db.isValid() )
    {
        m_db = QSqlDatabase::addDatabase( "QSQLITE", "LocalCollection" );
        m_db.setDatabaseName( m_dbPath );
    }
    m_db.open();

//    qDebug() << "Opening LocalCollection database" << ( m_db.isValid() ? "worked" : "failed" );
    if ( !m_db.isValid() )
        return false;

    if ( !m_db.tables().contains( "files" ) )
    {
//        qDebug() << "Creating LocalCollection database!";

        query( "CREATE TABLE files ("
                    "id                INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "directory         INTEGER NOT NULL,"
                    "filename          TEXT NOT NULL,"
                    "modificationDate  INTEGER,"
                    "title             TEXT NOT NULL,"
                    "artist            TEXT NOT NULL,"
                    "album             TEXT NOT NULL,"
                    "kbps              INTEGER,"
                    "duration          INTEGER,"
                    "mbid              VARCHAR( 36 ),"
                    "puid              VARCHAR( 36 ),"
                    "fingerprintId     INTEGER );" );
        query( "CREATE INDEX files_directory_idx ON files ( directory );" );
//        query( "CREATE INDEX files_filename_idx ON files ( filename );" );
//        query( "CREATE INDEX files_fpId_idx ON files ( fingerprintId );" );

        query( "CREATE TABLE directories ("
                    "id          INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "source      INTEGER,"          // sources foreign key
                    "path        TEXT NON NULL );" );

        query( "CREATE INDEX UNIQUE directories_path_idx ON directories ( path );" );

        query( "CREATE TABLE sources ("
                    "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "volume     TEXT UNIQUE NOT NULL,"  // on unix: "/", on windows: "\\?volume\..."
                    "available  INTEGER NOT NULL);" );

        query( "CREATE TABLE startDirs ("
                    "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "path       TEXT NON NULL,"
                    "source     INTEGER );" );      // sources foreign key

        query( "CREATE TABLE exclusions ("
                    "id         INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "path       TEXT NON NULL,"           
                    "startDir   INTEGER,"           // startDirs foreign key
                    "subDirs    INTEGER );" );     

        query( "CREATE TABLE metadata ("
                    "key         TEXT UNIQUE NOT NULL,"
                    "value       TEXT );" );

        query( "INSERT INTO metadata (key, value) VALUES ('version', '1');" );

    }
    
    int const v = version();
    if ( v < k_dbVersion )
    {
//        qDebug() << "Upgrading LocalCollection::db from" << v << "to" << k_dbVersion;

        /**********************************************
         * README!!!!!!!                              *
         * Ensure you use v < x                       *
         * Ensure you do upgrades in ascending order! *
         **********************************************/
    
        // do last, update DB version number
        query( "UPDATE metadata set key='version', value='" + QString::number( k_dbVersion ) + "';" );
    }

    return true;
}


int
LocalCollection::version() const 
{   
    QSqlQuery sql( m_db );
    sql.exec( "SELECT value FROM metadata WHERE key='version';" );

    if ( sql.next() )
    {
        return sql.value( 0 ).toInt();
    }

    return 0;
}

bool
LocalCollection::query( const QString& queryToken )
{
    QSqlQuery query( m_db );
    query.exec( queryToken );

    if ( query.lastError().isValid() )
    {
        //qDebug() << "SQL query failed:" << query.lastQuery() << endl
        //         << "SQL error was:"    << query.lastError().databaseText() << endl
        //         << "SQL error type:"   << query.lastError().type();

        return false;
    }

    return true;
}


QString
LocalCollection::fileURI( const QString& filePath )
{
    QString prefix( "file:/" );

#ifdef WIN32
    prefix = "file://";
#endif

    return prefix + QFileInfo( filePath ).absoluteFilePath();
}


QString
LocalCollection::getFingerprint( const QString& filePath )
{
    QSqlQuery query( m_db );
    query.prepare( "SELECT fpId FROM files WHERE uri = :uri" );
    query.bindValue( ":uri", fileURI( filePath ) );

    query.exec();
    if ( query.lastError().isValid() )
    {
        //qDebug() << "SQL query failed:" << query.lastQuery() << endl
        //         << "SQL error was:"    << query.lastError().databaseText() << endl
        //         << "SQL error type:"   << query.lastError().type();
    }
    else if ( query.next() )
        return query.value( 0 ).toString();

    return "";
}


bool
LocalCollection::setFingerprint( const QString& filePath, QString fpId )
{
    bool isNumeric;
    int intFpId = fpId.toInt( &isNumeric );
    Q_ASSERT( isNumeric );

    QSqlQuery query( m_db );
    query.prepare( "REPLACE INTO files ( uri, track, fpId ) VALUES ( :uri, 0, :fpId )" );
    query.bindValue( ":uri", fileURI( filePath ) );
    query.bindValue( ":fpId", intFpId );
    query.exec();

    if ( query.lastError().isValid() )
    {
        //qDebug() << "SQL query failed:" << query.lastQuery() << endl
        //         << "SQL error was:"    << query.lastError().databaseText() << endl
        //         << "SQL error type:"   << query.lastError().type();

        return false;
    }

    return true;
}

QList<LocalCollection::Source>
LocalCollection::getAllSources()
{
    QList<LocalCollection::Source> result;
    QSqlQuery query( m_db );
    query.prepare( "SELECT id, volume, available FROM sources" );
    query.exec();
    bool ok1 = false, ok2 = false;
    while (query.next()) {
        int id = query.value(0).toInt(&ok1);
        int available = query.value(2).toInt(&ok2);
        if (ok1 && ok2) {
            result << LocalCollection::Source(
                id, 
                query.value(1).toString(),
                available != 0);
        }
    }
    return result;
}

void
LocalCollection::setSourceAvailability(int sourceId, bool available)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE sources SET available = :available WHERE id = :sourceId" );
    query.bindValue(":available", available ? 1 : 0);
    query.bindValue(":sourceId", sourceId);
    query.exec();
}

QList<LocalCollection::Exclusion>
LocalCollection::getExcludedDirectories(int sourceId)
{
    QList<Exclusion> result;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT exclusions.path, exclusions.subDirs "
        "FROM exclusions "
        "INNER JOIN startDirs ON exclusions.startDir = startDirs.id "
        "WHERE startDirs.source = :sourceId" );
    query.bindValue(":sourceId", sourceId);
    query.exec();
    while (query.next()) {
        bool ok;
        int subdirsExcluded = query.value(1).toInt(&ok);
        if (ok) {
            result << Exclusion(query.value(0).toString(), subdirsExcluded != 0);
        }
    }
    return result;
}

QList<QString>
LocalCollection::getStartDirectories(int sourceId)
{
    QList<QString> result;
    QSqlQuery query(m_db);
    query.prepare("SELECT path FROM startDirs WHERE source = :sourceId");
    query.bindValue(":sourceId", sourceId);
    query.exec();    
    while (query.next()) {
        result << query.value(0).toString();
    }
    return result;
}

bool
LocalCollection::getDirectoryId(int sourceId, QString path, int &result)
{
    QSqlQuery query( m_db );
    query.prepare(
        "SELECT id FROM directories "
        "WHERE path = :path AND source = :sourceId");
    query.bindValue(":path", path);
    query.bindValue(":source", sourceId);
    query.exec();
    bool ok = false;
    if (query.next()) {
        result = query.value(0).toInt(&ok);
    }
    return ok;
}

bool 
LocalCollection::addDirectory(int sourceId, QString path, int &resultId)
{
    QSqlQuery query( m_db );
    query.prepare(
        "INSERT into directories ( id, source, path ) "
        "VALUES ( NULL, :sourceId, :path )" );
    query.bindValue(":sourceId", sourceId);
    query.bindValue(":path", path);
    query.exec();
    bool result;
    resultId = query.lastInsertId().toInt(&result);
    return result;
}

QList<LocalCollection::File> 
LocalCollection::getFiles(int directoryId)
{
    QList<LocalCollection::File> result;
    QSqlQuery query( m_db );
    query.setForwardOnly(true);
    query.prepare(
        "SELECT id, filename, modificationDate "
        "FROM files "
        "WHERE directory = :directoryId" );
    query.bindValue(":directoryId", directoryId);
    query.exec();
    if ( query.lastError().isValid() )
    {
        //qDebug() << "SQL query failed:" << query.lastQuery() << endl
        //    << "SQL error was:"    << query.lastError().databaseText() << endl
        //    << "SQL error type:"   << query.lastError().type();
    }

    bool ok1 = false, ok2 = false;
    while (query.next()) {
        int id = query.value(0).toInt(&ok1);
        uint modified = query.value(2).toUInt(&ok2);
        if (ok1 && ok2) {
            result << LocalCollection::File(id, query.value(1).toString(), modified);
        }
    }
    return result;
}

QList<LocalCollection::ResolveResult>
LocalCollection::resolve(const QString artist, const QString album, const QString title)
{
    QSqlQuery query( m_db );
    query.setForwardOnly( true );
    bool bPrepared = query.prepare(
        "SELECT f.artist, f.album, f.title, f.filename, f.kbps, f.duration, d.path, s.volume "
        "FROM files AS f "
        "INNER JOIN directories AS d ON f.directory = d.id "
        "INNER JOIN sources AS s ON d.source = s.id "
        "WHERE f.artist LIKE :artist "
        "AND f.album LIKE :album "
        "AND f.title LIKE :title " 
    );
    _ASSERT(bPrepared);

    query.bindValue( ":artist", artist.isEmpty() ? "%" : artist.toLower() );
    query.bindValue( ":album", album.isEmpty() ? "%" : album.toLower() );
    query.bindValue( ":title", title.isEmpty() ? "%" : title.toLower() );
    query.exec();

    QList<ResolveResult> result;
    while(query.next()) {
        result << ResolveResult(
            query.value(0).toString(),  // files.artist
            query.value(1).toString(),  // album
            query.value(2).toString(),  // title
            query.value(3).toString(),  // filename
            query.value(4).toUInt(),    // kbps
            query.value(5).toUInt(),    // duration
            query.value(6).toString(),  // directories.path
            query.value(7).toString()   // sources.name
            );
    }
    return result;
}


bool
LocalCollection::updateFile(int fileId, unsigned lastModified, const FileMeta& info)
{
    QSqlQuery query( m_db );
    if (info.m_artist.isEmpty() && info.m_title.isEmpty()) {
        bool bPrepared = query.prepare(
            "UPDATE INTO files (modificationDate, title, artist, album, kbps, duration) "
            "VALUES (:modificationDate, :title, :artist, :album, :kbps, :duration) "
            "WHERE id = :fileId" );

        _ASSERT(bPrepared);

        query.bindValue(":fileId", fileId);
        query.bindValue(":modificationDate", lastModified);
        query.bindValue(":title", info.m_title);
        query.bindValue(":artist", info.m_artist);
        query.bindValue(":album", info.m_album);
        query.bindValue(":kbps", info.m_kbps);
        query.bindValue(":duration", info.m_duration);
    } else {
        query.prepare("DELETE FROM files WHERE id = :fileId");
        query.bindValue(":fileId", fileId);
    }
    query.exec();
    bool bResult = !query.lastError().isValid();
    _ASSERT(bResult);
    return bResult;
}

bool
LocalCollection::addFile(int directoryId, QString filename, unsigned lastModified, const FileMeta& info)
{
    if (info.m_artist.isEmpty() && info.m_title.isEmpty())
        return true;

    QSqlQuery query( m_db );
    query.prepare(
        "INSERT INTO files (id, directory, filename, modificationDate, title, artist, album, kbps, duration) "
        "VALUES (NULL, :directory, :filename, :modificationDate, :title, :artist, :album, :kbps, :duration)" );
    query.bindValue(":directory", directoryId);
    query.bindValue(":filename", filename);
    query.bindValue(":modificationDate", lastModified);
    query.bindValue(":title", info.m_title);
    query.bindValue(":artist", info.m_artist);
    query.bindValue(":album", info.m_album);
    query.bindValue(":kbps", info.m_kbps);
    query.bindValue(":duration", info.m_duration);
    query.exec();
    return !query.lastError().isValid();
}

int
LocalCollection::addSource(const QString& volume)
{
    QSqlQuery query( m_db );
    query.prepare(
        "INSERT INTO sources (id, volume, available) "
        "VALUES (NULL, :volume, 1)" );
    query.bindValue(":volume", volume);
    query.exec();
    return query.lastError().isValid() ? -1 : query.lastInsertId().toInt();
}

void
LocalCollection::removeDirectory(int directoryId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM directories where id = :directoryId" );
    query.bindValue(":directoryId", directoryId);
    query.exec();
}

void 
LocalCollection::removeFiles(QList<int> ids)
{
    if (!ids.isEmpty()) {
        bool first = true;
        QString s = "";
        foreach(int i, ids) {
            if (first) {
                first = false;
            } else {
                s.append(',');
            }
            s.append(QString::number(i));
        }

        QSqlQuery query(m_db);
        query.prepare("DELETE FROM files where id IN (" + s + ")" );
        query.exec();
    }
}

void
LocalCollection::destroy()
{
    delete s_instance;
}


//////////////////////////////////////////////////////////////


LocalCollection::Exclusion::Exclusion(const QString path, bool bSubdirs /* = true */)
    : m_path(path)
    , m_bSubdirs(bSubdirs)
{
}

bool LocalCollection::Exclusion::operator==(const Exclusion& that) const
{
    return 0 == QString::compare(this->m_path, that.m_path, Qt::CaseInsensitive);
}

bool LocalCollection::Exclusion::subdirsToo() const
{
    return m_bSubdirs;
}


