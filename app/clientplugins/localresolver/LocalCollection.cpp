/***************************************************************************
 *   Copyright 2009 Last.fm Ltd.                                           *
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
#include "QueryError.h"
#include "AutoTransaction.h"
#include "lib/lastfm/core/CoreDir.h"
#include <QFileInfo>
#include <QVariant>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QtDebug>

extern void addUserFuncs(QSqlDatabase db);

#define LOCAL_COLLECTION_SCHEMA_VERSION_INT 3
#define LOCAL_COLLECTION_SCHEMA_VERSION_STR "3"
#define LEVENSHTEIN_ARTIST_THRESHOLD "0.7"
#define LEVENSHTEIN_TITLE_THRESHOLD "0.7"

#define QUERY(a) ChainableQuery(m_db, &ms_activeQueryMutex).prepare((a),(Q_FUNC_INFO)).exec()


QMutex LocalCollection::ms_activeQueryMutex(QMutex::Recursive);



LocalCollection*
LocalCollection::create(QString connectionName)
{
    static QMutex createMutex;
    QMutexLocker locker( &createMutex );
    return new LocalCollection( connectionName );
}

LocalCollection::LocalCollection(QString connectionName)
: m_dbPath( CoreDir::data().path() + "/LocalCollection.db" )
, m_connectionName( connectionName )
{
    initDatabase();
}

LocalCollection::~LocalCollection()
{
//  don't close... more testing.
//    m_db.close();

// removeDatabase seems to cause other instances of LocalCollection to die :(
// maybe i don't understand removeDatabase.  todo.

//    m_db = QSqlDatabase(); // to make the db "not in use" (removes a qt warning)
//    QSqlDatabase::removeDatabase( m_connectionName );
}

void
LocalCollection::versionCheck()
{
    // let version() throw... that would suggest a generic db access problem or a 
    // vastly incompatible db (without metadata table) which we don't want to touch
    if ( version() < LOCAL_COLLECTION_SCHEMA_VERSION_INT ) {
        // upgrading!
        // until release we will just blow away the old db and recreate
        foreach ( QString table, m_db.tables() ) {
            QUERY( "DROP TABLE " + table );
        }
        initDatabase();
    }
}

void
LocalCollection::initDatabase()
{
    if ( !m_db.isValid() )  {
        m_db = QSqlDatabase::addDatabase( "QSQLITE", m_connectionName );
        m_db.setDatabaseName( m_dbPath );
    }
    m_db.open();

    if ( !m_db.tables().contains( "metadata" ) ) {
        QUERY( "CREATE TABLE files ("
                    "id                INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                    "directory         INTEGER NOT NULL,"
                    "filename          TEXT NOT NULL,"
                    "modification_date INTEGER,"
                    "lowercase_title   TEXT NOT NULL,"
                    "artist            INTEGER,"
                    "album             TEXT NOT NULL,"
                    "kbps              INTEGER,"    
                    "duration          INTEGER,"
                    "mbid              VARCHAR( 36 ),"
                    "puid              VARCHAR( 36 ),"
                    "lastfm_fpid       INTEGER, "
                    "tag_time          INTEGER);" );
        QUERY( "CREATE INDEX files_directory_idx ON files ( directory );" );
        QUERY( "CREATE INDEX files_artist_idx ON files ( artist );" );

        QUERY( "CREATE TABLE artists ("
                    "id                 INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                    "lowercase_name     TEXT NOT NULL UNIQUE );" );
        QUERY( "CREATE INDEX artists_name_idx ON artists ( lowercase_name );" );

        // artist a has similar artist b with weight
        QUERY( "CREATE TABLE simartists ("
                    "artist_a           INTEGER,"
                    "artist_b           INTEGER,"
                    "weight             INTEGER );" );
        QUERY( "CREATE INDEX simartists_artist_a_idx ON simartists ( artist_a );" );

        QUERY( "CREATE TABLE tags ("
                    "id                 INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                    "name               TEXT UNIQUE NOT NULL );" );
        QUERY( "CREATE INDEX tags_name_idx ON tags ( name );" );

        // file has tag with weight
        QUERY( "CREATE TABLE tracktags ("
                    "file               INTEGER NOT NULL,"      // files foreign key
                    "tag                INTEGER NOT NULL,"      // tags foreign key
                    "weight             FLOAT NOT NULL);" );    // 0-1
        QUERY( "CREATE INDEX tracktags_file_idx ON tracktags ( file ); ");
        QUERY( "CREATE INDEX tracktags_tag_idx ON tracktags ( tag ); ");

        QUERY( "CREATE TABLE directories ("
                    "id          INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                    "source      INTEGER,"          // sources foreign key
                    "path        TEXT NON NULL );" );

        QUERY( "CREATE INDEX directories_path_idx ON directories ( path );" );

        QUERY( "CREATE TABLE sources ("
                    "id         INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                    "volume     TEXT NOT NULL,"  // on unix: "/", on windows: "\\?volume\..."
                    "path       TEXT,"           // appended to the volume (empty string is null in sqlite)
                    "available  INTEGER NOT NULL);" );

        QUERY( "CREATE TABLE exclusions ("
                    "id         INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                    "path       TEXT NON NULL,"           
                    "source     INTEGER,"           // sources foreign key
                    "subDirs    INTEGER );" );     

        QUERY( "CREATE TABLE metadata ("
                    "key         TEXT UNIQUE NOT NULL,"
                    "value       TEXT );" );

        QUERY( "INSERT INTO metadata (key, value) VALUES ('version', '" LOCAL_COLLECTION_SCHEMA_VERSION_STR "');" );
    }

    versionCheck();

    addUserFuncs(m_db);
}

int
LocalCollection::version() const 
{   
    ChainableQuery q(m_db, &ms_activeQueryMutex);
    q.prepare( "SELECT value FROM metadata WHERE key='version'" ).exec();

    if ( q.next() ) {
        bool ok = false;
        int version = q.value( 0 ).toInt( &ok );
        if ( ok )
            return version;
    }
    throw QueryError("no version");
}

QString
LocalCollection::getFingerprint( const QString& filePath )
{
    Q_UNUSED(filePath);
    Q_ASSERT(!"todo");
    return "";
}

void
LocalCollection::setFingerprint( const QString& filePath, QString fpId )
{
    Q_UNUSED(filePath);
    Q_UNUSED(fpId);
    Q_ASSERT(!"todo");
}

QList<LocalCollection::Source>
LocalCollection::getAllSources()
{
    ChainableQuery q(m_db, &ms_activeQueryMutex);
    q.prepare( "SELECT id, volume, path, available FROM sources" ).exec();

    QList<LocalCollection::Source> result;
    while ( q.next() ) {
        bool ok1 = false, ok2 = false;

        int id = q.value( 0 ).toInt( &ok1 );
        QString volume( q.value( 1 ).toString() );
        QString path( q.value( 2 ).toString() );
        int available = q.value( 3 ).toInt( &ok2 );

        if ( ok1 && ok2 ) {
            result << LocalCollection::Source(id, volume, path, available != 0);
        }
    }
    return result;
}

void
LocalCollection::setSourceAvailability(int sourceId, bool available)
{
    ChainableQuery q(m_db, &ms_activeQueryMutex);
    q.prepare( "UPDATE sources SET available = :available WHERE id = :sourceId" ).
    bindValue( ":available", available ? 1 : 0 ).
    bindValue( ":sourceId", sourceId ).
    exec();
}

QList<LocalCollection::Exclusion>
LocalCollection::getExcludedDirectories(int sourceId)
{
    ChainableQuery q(m_db, &ms_activeQueryMutex);
    q.prepare(
        "SELECT path, subDirs FROM exclusions "
        "WHERE exclusions.source = :sourceId" ).
    bindValue( ":sourceId", sourceId ).
    exec();

    QList<Exclusion> result;
    while (q.next()) {
        bool ok;
        int subdirsExcluded = q.value( 1 ).toInt( &ok );
        if ( ok ) {
            result << Exclusion(q.value( 0 ).toString(), subdirsExcluded != 0);
        }
    }
    return result;
}

bool
LocalCollection::getDirectoryId(int sourceId, QString path, int &result)
{
    // mess about to avoid Qt from treating an empty path as a NULL
    bool pathIsEmpty = path == "";
    QString queryText( pathIsEmpty ?
        "SELECT id FROM directories "
        "WHERE path = \"\" AND source = :sourceId"
        :
        "SELECT id FROM directories "
        "WHERE path = :path AND source = :sourceId" );

    ChainableQuery q( m_db, &ms_activeQueryMutex );
    q.prepare( queryText ).
    bindValue( ":sourceId", sourceId );

    if (!pathIsEmpty) {
        // would be nice if we could simply add this always, but then Qt
        // complains about parameter mismatch in the pathIsEmpty case
        q.bindValue( ":path", path );
    }

    bool ok = false;
    if (q.exec().next()) {
        result = q.value( 0 ).toInt( &ok );
    }
    return ok;
}

bool 
LocalCollection::addDirectory(int sourceId, QString path, int &resultId)
{
    // mess about to avoid Qt from treating an empty path as a NULL
    bool pathIsEmpty = path == "";
    QString queryText( pathIsEmpty ?
        "INSERT into directories ( id, source, path ) "
        "VALUES ( NULL, :sourceId, \"\" )" 
        :
        "INSERT into directories ( id, source, path ) "
        "VALUES ( NULL, :sourceId, :path )" );

    ChainableQuery q( m_db, &ms_activeQueryMutex );
    q.prepare( queryText ).
    bindValue( ":sourceId", sourceId );

    if (!pathIsEmpty) {
        // would be nice if we could simply add this always, but then Qt
        // complains about parameter mismatch in the pathIsEmpty case
        q.bindValue( ":path", path );
    }

    bool ok;
    resultId = q.exec().lastInsertId().toInt( &ok );
    return ok;
}

QList<LocalCollection::File> 
LocalCollection::getFiles(int directoryId)
{
    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare(
        "SELECT id, filename, modification_date "
        "FROM files "
        "WHERE directory = :directoryId" ).
    bindValue( ":directoryId", directoryId ).
    setForwardOnly( true ).
    exec();

    QList<LocalCollection::File> result;
    bool ok1 = false, ok2 = false;
    while ( query.next() ) {
        int id = query.value( 0 ).toInt( &ok1 );
        QString filename = query.value( 1 ).toString();
        uint modified = query.value( 2 ).toUInt( &ok2 );
        if ( ok1 && ok2 ) {
            result << LocalCollection::File( id, filename, modified );
        }
    }
    return result;
}

QList<LocalCollection::ResolveResult>
LocalCollection::resolve(const QString artist, const QString album, const QString title)
{
    if ( artist.isEmpty() || title.isEmpty() )
        return QList<LocalCollection::ResolveResult>();

    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare(
        // original, naive and slow:

        //"SELECT a.lowercase_name, f.album, f.lowercase_title, "
        //"   levenshtein(a.lowercase_name, :artist) AS aq, "
        //"   levenshtein(f.lowercase_title, :title) AS tq, "
        //"   f.filename, f.kbps, f.duration, d.path, s.volume "
        //"FROM files AS f "
        //"INNER JOIN artists AS a on f.artist = a.id "
        //"INNER JOIN directories AS d ON f.directory = d.id "
        //"INNER JOIN sources AS s ON d.source = s.id "
        //"WHERE s.available = 1 "
        //"AND aq > "LEVENSHTEIN_ARTIST_THRESHOLD" "
        //"AND tq > "LEVENSHTEIN_TITLE_THRESHOLD" " 

        // subselect version faster, but can do better by caching
        // artist match distance in temp table, something like:
        // (TODO)
        // temp table will allow us to preserve 'aq' too...

        //"CREATE TEMP TABLE IF NOT EXISTS :tablename AS "
        //"SELECT id, levenshtein(lowercase_name, :artist) AS aq "
        //"FROM artists "
        //"WHERE aq > "LEVENSHTEIN_ARTIST_THRESHOLD";"

        "SELECT a.lowercase_name, f.album, f.lowercase_title, "
        "   1.0 as aq, "
        "   levenshtein(f.lowercase_title, :title) AS tq, "
        "   f.filename, f.kbps, f.duration, d.path, s.volume "
        "FROM files AS f "
        "INNER JOIN artists AS a on f.artist = a.id "
        "INNER JOIN directories AS d ON f.directory = d.id "
        "INNER JOIN sources AS s ON d.source = s.id "
        "WHERE s.available = 1 "
        "AND a.id IN ("
        " SELECT id FROM artists "
        " WHERE levenshtein(lowercase_name, :artist) > "LEVENSHTEIN_ARTIST_THRESHOLD") "
        "AND tq > "LEVENSHTEIN_TITLE_THRESHOLD 
    ).
    setForwardOnly( true ).
    bindValue( ":artist", artist.simplified().toLower() ).
    bindValue( ":title", title.simplified().toLower() ).
    exec();

    QList<ResolveResult> result;
    while(query.next()) {
        result << ResolveResult(
            query.value(0).toString(),  // artist
            query.value(1).toString(),  // album
            query.value(2).toString(),  // title
            query.value(3).toDouble(),  // artistMatchQuality
            query.value(4).toDouble(),  // titleMatchQuality
            query.value(5).toString(),  // filename
            query.value(6).toUInt(),    // kbps
            query.value(7).toUInt(),    // duration
            query.value(8).toString(),  // directories.path
            query.value(9).toString()   // sources.name
            );
    }
    return result;
}


void
LocalCollection::updateFile(int fileId, unsigned lastModified, const FileMeta& info)
{
    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare(
        "UPDATE files SET "
        "modification_date = :modification_date , "
        "lowercase_title = :lowercase_title , "
        "artist = :artist , "
        "album = :album , "
        "kbps = :kbps , "
        "duration = :duration "
        "WHERE id == :fileId" ).
    bindValue( ":fileId", fileId ).
    bindValue( ":modification_date", lastModified ).
    bindValue( ":lowercase_title", info.m_title.simplified().toLower() ).
    bindValue( ":artist", getArtistId( info.m_artist, Create ) ).
    bindValue( ":album", info.m_album ).
    bindValue( ":kbps", info.m_kbps ).
    bindValue( ":duration", info.m_duration ).
    exec();
}

bool 
LocalCollection::getCounts(int& outArtists, int& outFiles)
{
    ChainableQuery q(m_db, &ms_activeQueryMutex);
    q.prepare( "SELECT COUNT(DISTINCT artist), COUNT(id) FROM files" ).exec();
    bool ok1 = false, ok2 = false;
    if (q.next()) {
        outArtists = q.value( 0 ).toInt( &ok1 );
        outFiles = q.value( 1 ).toInt( &ok2 );
    }
    return (ok1 && ok2);
}


// returns 0 if the artistName does not exist and bCreate is false
int
LocalCollection::getArtistId(QString artistName, Creation flag)
{
    QString lowercase_name( artistName.simplified().toLower() );

    {
        ChainableQuery query(m_db, &ms_activeQueryMutex);
        query.prepare( "SELECT id FROM artists where lowercase_name = :lowercase_name" ).
        bindValue( ":lowercase_name", lowercase_name ).
        exec();
        if ( query.next() ) {
            int artistId = query.value( 0 ).toInt();
            Q_ASSERT( artistId > 0 );
            return artistId;
        }
    }

    if ( flag == Create ) {
        ChainableQuery query(m_db, &ms_activeQueryMutex);
        int artistId = query.prepare(
            "INSERT INTO artists (lowercase_name) "
            "VALUES (:lowercase_name)" ).
        bindValue( ":lowercase_name", lowercase_name ).
        exec().
        lastInsertId().toInt();

        Q_ASSERT( artistId > 0 );
        return artistId;
    }
    return 0;
}


void
LocalCollection::addFile(int directoryId, QString filename, unsigned lastModified, const FileMeta& info)
{
    int artistId = getArtistId( info.m_artist, Create );
    Q_ASSERT( artistId > 0 );

    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare(
        "INSERT INTO files (id, directory, filename, modification_date, lowercase_title, artist, album, kbps, duration) "
        "VALUES (NULL, :directory, :filename, :modification_date, :lowercase_title, :artist, :album, :kbps, :duration)" ).
    bindValue( ":directory", directoryId ).
    bindValue( ":filename", filename ).
    bindValue( ":modification_date", lastModified ).
    bindValue( ":lowercase_title", info.m_title.simplified().toLower() ).
    bindValue( ":artist", artistId ).
    bindValue( ":album", info.m_album ).
    bindValue( ":kbps", info.m_kbps ).
    bindValue( ":duration", info.m_duration ).
    exec();
}

int
LocalCollection::getSourceId(const QString& volume, const QString& path, Creation flag)
{
    {
        ChainableQuery q(m_db, &ms_activeQueryMutex);
        q.prepare("SELECT id FROM sources WHERE volume = :volume AND path = :path").
        bindValue( ":volume", volume ).
        bindValue( ":path", path ).
        exec();
        if (q.next()) {
            int id = q.value( 0 ).toInt();
            Q_ASSERT( id );
            return id;
        }
    }

    if ( flag == Create ) {
        ChainableQuery q(m_db, &ms_activeQueryMutex);
        q.prepare("INSERT INTO sources (id, volume, path, available) "
            "VALUES (NULL, :volume, :path, 1)" ).
        bindValue( ":volume", volume ).
        bindValue( ":path", path ).
        exec();
        int id = q.lastInsertId().toInt();
        Q_ASSERT(id > 0);
        return id;
    }
    return 0;
}

void 
LocalCollection::deleteSource( const QString& volume, const QString &path )
{
    int id = getSourceId( volume, path, LocalCollection::NoCreate );
    if (id) {
        // todo: can we have cascading deletes?
        QMutexLocker locker( &ms_activeQueryMutex );
        AutoTransaction<LocalCollection> trans(*this);

        ChainableQuery(m_db, 0).
        prepare( "DELETE FROM tracktags WHERE file IN ("
            "SELECT files.id FROM files "
            "INNER JOIN directories ON directories.id = files.directory "
            "WHERE directories.source = :id )").
        bindValue( ":id", id).exec();
        
        ChainableQuery(m_db, 0).
        prepare( "DELETE FROM files WHERE directory IN ("
            "SELECT directories.id FROM directories "
            "WHERE directories.source = :id )" ).
        bindValue( ":id", id ).exec();
        
        ChainableQuery(m_db, 0).
        prepare( "DELETE FROM directories WHERE source = :id ").
        bindValue( ":id", id ).exec();
    
        ChainableQuery(m_db, 0).
        prepare( "DELETE FROM sources WHERE id = :id " ).
        bindValue( ":id", id ).exec();

        trans.commit();
    }
}

void
LocalCollection::removeDirectory(int directoryId)
{
    ChainableQuery(m_db, &ms_activeQueryMutex).
    prepare( "DELETE FROM directories WHERE id = :directoryId" ).
    bindValue( ":directoryId", directoryId ).
    exec();
}

void 
LocalCollection::removeFiles(QList<int> ids)
{
    if ( !ids.isEmpty() ) {
        bool first = true;
        QString s = "";
        foreach(int i, ids) {
            if ( first ) {
                first = false;
            } else {
                s.append( ',' );
            }
            s.append( QString::number( i ) );
        }
        QUERY( "DELETE FROM files WHERE id IN (" + s + ")" );
        QUERY( "DELETE FROM tracktags WHERE file IN (" + s + ")" );
    }
}

int
LocalCollection::getTagId(QString tag, Creation flag)
{
    tag = tag.simplified().toLower();

    {
        ChainableQuery query(m_db, &ms_activeQueryMutex);
        query.prepare( "SELECT id FROM tags WHERE name = :name" ).
        bindValue( ":name", tag ).
        exec();
        if ( query.next() ) {
            int id = query.value( 0 ).toInt();
            Q_ASSERT( id > 0 );
            return id;
        }
    }

    if ( flag == Create ) {
        int id = ChainableQuery(m_db, &ms_activeQueryMutex).
        prepare( "INSERT INTO tags (name) VALUES (:name)" ).
        bindValue( ":name", tag ).
        exec().lastInsertId().toInt();

        Q_ASSERT( id > 0 );
        return id;
    }
    return 0;
}

void
LocalCollection::deleteGlobalTrackTagsForArtist(int artistId)
{
    deleteTrackTagsForArtist( artistId, 0 );
}

void
LocalCollection::deleteTrackTagsForArtist(int artistId, unsigned userId)
{
    Q_ASSERT( artistId > 0 );
    ChainableQuery(m_db, &ms_activeQueryMutex).prepare(
        "DELETE FROM tracktags "
        "WHERE user_id == :userId "
        "AND file IN "
        "(SELECT id FROM files WHERE artist == :artistId) " ).
    bindValue( ":artistId", artistId ).
    bindValue( ":userId", userId ).
    exec();
}

void
LocalCollection::setGlobalTagsForArtist(QString artist, WeightedStringList globalTags)
{
    int artistId = getArtistId( artist, Create );
    deleteGlobalTrackTagsForArtist( artistId );
    foreach(const WeightedString& tag, globalTags) {
        insertGlobalArtistTag( 
            artistId, 
            getTagId( tag, Create ),
            tag.weighting() );
    }
}

void
LocalCollection::insertUserArtistTag(int artistId, int tagId, unsigned userId)
{
    insertTrackTag(artistId, tagId, userId, 100);
}

void
LocalCollection::insertGlobalArtistTag(int artistId, int tagId, int weight)
{
    insertTrackTag(artistId, tagId, 0, weight);
}

void
LocalCollection::insertTrackTag(int artistId, int tagId, unsigned userId, int weight)
{
    Q_ASSERT(artistId > 0 && tagId > 0);

    ChainableQuery(m_db, &ms_activeQueryMutex).prepare(
        "INSERT INTO tracktags (file, tag, weight, user_id) "
        "SELECT id, :tagId, :weight, :userId "
        "FROM files WHERE artist == :artistId " ).
    bindValue( ":artistId", artistId ).
    bindValue( ":tagId", tagId ).
    bindValue( ":weight", weight ).
    bindValue( ":userId", userId ).
    exec();
}

QList<QPair<unsigned, float> >
LocalCollection::filesWithTag(QString tag, Availablity flag)
{
    QList<QPair<unsigned, float> > result;

    int tagId = getTagId( tag, NoCreate );
    if ( tagId > 0 ) {
        QString queryString;

        if (flag == AllSources) {
            queryString = "SELECT file, weight FROM tracktags WHERE tag = :tagId";
        } else {
            Q_ASSERT( flag == AvailableSources );
            queryString = 
                "SELECT tracktags.file, tracktags.weight FROM tracktags "
                "INNER JOIN files on tracktags.file = files.id "
                "INNER JOIN directories on files.directory = directories.id "
                "INNER JOIN sources on directories.source = sources.id "
                "WHERE tag = :tagId AND sources.available = 1 ";
        }
        ChainableQuery query(m_db, &ms_activeQueryMutex);
        query.prepare( queryString ).
        setForwardOnly( true ).
        bindValue( ":tagId", tagId ).
        exec();
        while ( query.next() ) {
            uint id = query.value( 0 ).toUInt();
            float weight = query.value( 1 ).toDouble();
            result << qMakePair(id, weight);
        }
    }

    return result;
}

// get all the files by an artist, fuzzy match on the artist's name
QList<unsigned> 
LocalCollection::filesByArtist(QString artist, Availablity flag)
{
    QString queryString;
    if (flag == AllSources) {
        queryString = 
            "SELECT id FROM files WHERE artist IN "
            "(SELECT id FROM artists "
            " WHERE levenshtein(lowercase_name, :artist) > "LEVENSHTEIN_ARTIST_THRESHOLD")";
    } else {
        Q_ASSERT( flag == AvailableSources );
        queryString = 
            "SELECT files.id FROM files "
            "INNER JOIN directories on files.directory = directories.id "
            "INNER JOIN sources on directories.source = sources.id "
            "WHERE files.artist IN "
            "(SELECT id FROM artists "
            " WHERE levenshtein(lowercase_name, :artist) > "LEVENSHTEIN_ARTIST_THRESHOLD") "
            "AND sources.available = 1 ";
    }

    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare( queryString ).
    setForwardOnly( true ).
    bindValue( ":artist", artist.simplified().toLower() ).
    exec();

    QList<unsigned> results;
    while (query.next()) {
        results << query.value( 0 ).toUInt();
    }
    return results;
}

QList<unsigned> 
LocalCollection::filesByArtistId(int artistId, Availablity flag)
{
    QString queryString;
    if (flag == AllSources) {
        queryString = "SELECT id FROM files WHERE artist = :artistId";
    } else {
        Q_ASSERT( flag == AvailableSources );
        queryString = 
            "SELECT files.id FROM files "
            "INNER JOIN directories on files.directory = directories.id "
            "INNER JOIN sources on directories.source = sources.id "
            "WHERE files.artist = :artistId AND sources.available = 1 ";
    }

    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare( queryString ).
    setForwardOnly( true ).
    bindValue( ":artistId", artistId ).
    exec();

    QList<unsigned> results;
    while (query.next()) {
        results << query.value( 0 ).toUInt();
    }
    return results;
}

LocalCollection::EntryList
LocalCollection::allTags()
{
    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare(
            "SELECT artist, tag, avg(weight) "
            "FROM tracktags "
            "INNER JOIN files on tracktags.file = files.id "
            "GROUP BY artist, tag "
            "ORDER BY artist, tag ").
        setForwardOnly( true ).
        exec();

    EntryList result;
    {
        int prevArtistId = 0;
        TagVec currentArtistTags;

        while ( query.next() ) {
            int artistId = query.value(0).toInt();
            int tag = query.value(1).toInt();
            float weight = query.value(2).toDouble();

            if (prevArtistId == 0) // first run through the loop
                prevArtistId = artistId;

            if (prevArtistId != artistId) {
                Entry e;
                e.artistId = prevArtistId;
                e.tagVec = currentArtistTags;
                result << e;
                currentArtistTags = TagVec();
                prevArtistId = artistId;
            }

            currentArtistTags << qMakePair(tag, weight);
        }

        if (currentArtistTags.size()) {
            Entry e;
            e.artistId = prevArtistId;
            e.tagVec = currentArtistTags;
            result << e;
        }
    }
    return result;
}

bool 
LocalCollection::getFileById(uint fileId, LocalCollection::FileResult &out)
{
    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare(
        "SELECT album, artists.lowercase_name, lowercase_title, "
        "sources.volume, directories.path, filename, duration "
        "FROM files "
        "INNER JOIN artists on files.artist = artists.id "
        "INNER JOIN directories on files.directory = directories.id "
        "INNER JOIN sources on directories.source = sources.id "
        "WHERE files.id = :fileId" ).
        bindValue(":fileId", fileId).
        exec();
    if (query.next()) {
        out.m_album = query.value(0).toString();
        out.m_artist = query.value(1).toString();
        out.m_title = query.value(2).toString();
        out.m_sourcename = query.value(3).toString();
        out.m_path = query.value(4).toString();
        out.m_filename = query.value(5).toString();
        out.m_duration = query.value(6).toUInt();
        return true;
    }
    return false;
}

QList<LocalCollection::FilesToTagResult> 
LocalCollection::getFilesToTag(int maxTagAgeDays)
{
    time_t oldTagAge = 
        QDateTime::currentDateTime().toUTC().toTime_t() 
        - maxTagAgeDays * 24* 60* 60;

    ChainableQuery query(m_db, &ms_activeQueryMutex);
    query.prepare(
        "SELECT files.id, artists.lowercase_name, files.album, files.lowercase_title "
        "FROM files "
        "INNER JOIN artists ON artists.id = files.artist "
        "WHERE tag_time IS NULL " 
        "OR tag_time < :oldTagAge").
        bindValue(":oldTagAge", (uint)oldTagAge).
        setForwardOnly( true ).
        exec();
    QList<FilesToTagResult> results;
    while (query.next()) {
        FilesToTagResult r;
        r.fileId = query.value( 0 ).toUInt();
        r.artist = query.value( 1 ).toString();
        r.album = query.value( 2 ).toString();
        r.title = query.value( 3 ).toString();
        results << r;
    }
    return results;
}

void
LocalCollection::deleteTrackTags_batch(QString ids)
{
    QUERY("DELETE FROM tracktags WHERE file IN (" + ids + ")" );
}

void
LocalCollection::setFileTagTime_batch(QString ids)
{
    ChainableQuery(m_db, &ms_activeQueryMutex).prepare(
        "UPDATE files SET tag_time = :tagTime WHERE id IN (" + ids + ")" ).
        bindValue( ":tagTime", QDateTime::currentDateTime().toUTC().toTime_t() ).
        exec();
}

void
LocalCollection::setFileTagTime(QVariantList fileIds)
{
    // do it in a transaction to attempt to speed it up
    QMutexLocker locker( &ms_activeQueryMutex );
    AutoTransaction<LocalCollection> trans(*this);
    batch(fileIds, &LocalCollection::setFileTagTime_batch);
    trans.commit();
}

void
LocalCollection::deleteTrackTags(QVariantList fileIds)
{
    // do it in a transaction to attempt to speed it up
    QMutexLocker locker( &ms_activeQueryMutex );
    AutoTransaction<LocalCollection> trans(*this);
    batch(fileIds, &LocalCollection::deleteTrackTags_batch);
    trans.commit();
}

void 
LocalCollection::batch(QVariantList fileIds, void (LocalCollection::*batchFunc)(QString))
{
    // sqlite driver turns some batchExec commands into a series of execs.
    // eg, the command: DELETE FROM tracktags WHERE file IN (:fileIds)
    // it's slow...
    
    int count = 0;
    QString ids = "";
    bool first = true;
    foreach (const QVariant& v, fileIds) {
        if (first) {
            first = false;
        } else {
            ids.append(',');
        }
        ids.append(QString::number(v.toInt()));

        if (++count % 100 == 0) {
            (*this.*batchFunc)(ids);
            first = true;
            ids = "";
        }
    }
    if (!first) {
        (*this.*batchFunc)(ids);
    }
}


void
LocalCollection::updateTrackTags(QVariantList fileIds, QVariantList tagIds, QVariantList weights)
{
    ChainableQuery(m_db, &ms_activeQueryMutex).prepare(
        "INSERT INTO tracktags (file, tag, weight) "
        "VALUES (:fileIds, :tags, :weights)" ).
        bindValue( ":fileIds", fileIds ).
        bindValue( ":tags", tagIds ).
        bindValue( ":weights", weights ).
        execBatch();
}

// returns a VariantList of (integer) tag IDs givem a list of tag names
QVariantList
LocalCollection::resolveTags(QStringList tags)
{
    QMap<QString, int> map;
    return resolveTags(tags, map);
}


QVariantList
LocalCollection::resolveTags(QStringList tags, QMap<QString, int>& map)
{
    // too slow hitting the db, build our own cache in front of it
    QVariantList result;
    foreach(const QString tag, tags) {
        QMap<QString, int>::iterator it = map.find(tag);
        if (it != map.end()) {
            result << it.value();
        } else {
            int id = getTagId(tag, Create);
            map.insert(tag, id);
            result << id;
        }
    }
    return result;
}

QList< QPair< QString, float > >
LocalCollection::getTopTags(int limit)
{
    QList< QPair< QString, float > > result;

    ChainableQuery q(m_db, &ms_activeQueryMutex);
    q.prepare(
        "SELECT name, sum(weight) FROM tracktags "
        "INNER JOIN tags WHERE tracktags.tag = tags.id "
        "GROUP BY tags.id "
        "ORDER BY sum(weight) DESC "
        "LIMIT :limit ").
        bindValue(":limit", limit).
        exec();
    while ( q.next() ) {
        result << ( QPair< QString, float >( q.value(0).toString(), q.value(1).toDouble() ) );
    }
    return result;
}


void 
LocalCollection::transactionBegin()
{
    bool ok = m_db.transaction();
    Q_ASSERT( ok );
//    QUERY("BEGIN IMMEDIATE");
}

void 
LocalCollection::transactionCommit()
{
    bool ok = m_db.commit();
    Q_ASSERT( ok );
//    QUERY("COMMIT");
}

void 
LocalCollection::transactionRollback()
{
    bool ok = m_db.rollback();
    Q_ASSERT( ok );
//    QUERY("ROLLBACK");
}

QMutex*
LocalCollection::getMutex()
{
    return &ms_activeQueryMutex;
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


