/***************************************************************************
 *   Copyright 2007-2008 Last.fm Ltd.                                      *
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

#ifndef SCROBBLE_SHEPHERD_H
#define SCROBBLE_SHEPHERD_H

/** @author Max Howell <max@last.fm>
  * @brief You should be able to drop this file into another project and submit scrobbles to Last.fm
  *
  * Instantiate a ScrobbleManager class. Pass it username and password
  * information.
  *
  * Songs that have passed the scrobble point should be submitted to Last.fm
  * when they _end_, using submit(), you can announce just started tracks using,
  * announce() - this is the NowPlaying notification.
  * 
  * Determining the scrobble point and what constitutes a legal scrobble is up
  * to you!
  *
  * http://www.audioscrobbler.net/development/protocol/
  */

#include <QHttp>
#include <QList>
#include <QVariant>

#include "lib/moose/TrackInfo.h"

class ScrobbleCache;
class ScrobblerHandshakeRequest;
class ScrobblerNowPlayingRequest;
class ScrobblerHttpPostRequest;


namespace Scrobbler
{
    enum Status
    {
        Connecting,
        Handshaken,
        Scrobbling,
        TracksScrobbled,
        TracksNotScrobbled,
        StatusMax
    };

    enum Error
    {
        /** the following will show via the status signal, the scrobbler will
          * not submit this session (np too), however caching will continue */
        ErrorBadSession = StatusMax,
        ErrorBannedClient,
        ErrorBadAuthorisation,
        ErrorBadTime,
        ThreeHardFailures,

        /** while the handshake is occuring */
        ErrorNotInitialized,

        NoError
    };
}


class ScrobblerManager : public QObject
{
    Q_OBJECT

    QString const m_username;
    QString const m_password;
    QString m_session;

    class ScrobblerHandshake* m_handshake;
    class NowPlaying* m_np;
    class ScrobblerSubmitter* m_submitter;
    uint m_hard_failures;

public:
    /** password should be already a 32 character md5 hash */
    ScrobblerManager( const QString& username, const QString& password );
    ~ScrobblerManager();

    /** will ask Last.fm to update the now playing information for username() */
    void nowPlaying( const TrackInfo& );
    /** will submit the ScrobbleCache for this user */
    void submit();

    QString session() const { return m_session; }
    QString username() const { return m_username; }

signals:
    /** the controller should show status in an appropriate manner */
    void status( int code, QVariant data = QVariant() );

private:
    void handshake();
    void onError( Scrobbler::Error );

private slots:
    void onHandshakeReturn( const QString& );
    void onNowPlayingReturn( const QString& );
    void onSubmissionReturn( const QString& );

    void onHandshakeHeaderReceived( const QHttpResponseHeader& );
};


class ScrobblerHttp : public QHttp
{
    Q_OBJECT

protected:
    ScrobblerHttp( QObject* parent = 0 );

    int m_id;
    class QTimer *m_retry_timer;

private slots:
    void onRequestFinished( int id, bool error );

signals:
    void done( const QString& data );

protected slots:
    virtual void request() = 0;

public:
    void resetRetryTimer();
    void retry();

    int id() const { return m_id; }
};


class ScrobblerHandshake : public ScrobblerHttp
{
    QString const m_username;
    QString const m_password;

public:
    ScrobblerHandshake( const QString& username, const QString& password );

    virtual void request();
};


class ScrobblerHttpPostRequest : public ScrobblerHttp
{
    QString m_path;

protected:
    QByteArray m_data;

    ScrobblerManager* manager() const { return (ScrobblerManager*)parent(); }
    QString session() const { return manager()->session(); }

public:
    ScrobblerHttpPostRequest( ScrobblerManager* parent ) : ScrobblerHttp( parent )
    {}

    /** if you reimplement call the base version after setting m_data */
    virtual void request();

    void setUrl( const QUrl& );
};


class NowPlaying : public ScrobblerHttpPostRequest
{
    QTimer* m_timer;

public:
    NowPlaying( ScrobblerManager* );

    void request( const TrackInfo& );
};


class ScrobblerSubmitter : public ScrobblerHttpPostRequest
{
    QList<TrackInfo> m_batch;

public:
    ScrobblerSubmitter( ScrobblerManager* parent ) : ScrobblerHttpPostRequest( parent )
    {}

    virtual void request();
    QList<TrackInfo> batch() const { return m_batch; }

    void clearBatch() { m_batch.clear(); }
};


/** absolutely not thread-safe */
class ScrobbleCache
{
    QString m_path;
    QString m_username;
    QList<TrackInfo>& m_tracks;

    ScrobbleCache(); //used by tracksForPath()

    void read();  /// reads from m_path into m_tracks
    void write(); /// writes m_tracks to m_path

public:
    explicit ScrobbleCache( const QString& username );

    /** note this is unique for TrackInfo::sameAs() and equal timestamps 
      * obviously playcounts will not be increased for the same timestamp */
    void append( const TrackInfo& );
    void append( const QList<TrackInfo>& );

    /** returns the number of tracks left in the queue */
    int remove( const QList<TrackInfo>& );

    QList<TrackInfo> tracks() const { return m_tracks; }
    QString path() const { return m_path; }
    QString username() const { return m_username; }

    /** a track list from an XML file in the ScrobbleCache format at path */
    static QList<TrackInfo> tracksForPath( const QString& path )
    {
        ScrobbleCache cache;
        cache.m_path = path;
        cache.read();
        return cache.m_tracks;
    }

private:
    bool operator==( const ScrobbleCache& ); //undefined
};

#endif /* SCROBBLER_H */
