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

#ifndef SCROBBLER_H
#define SCROBBLER_H

#include <QByteArray>
#include <QList>
#include <QString>
#include <QVariant>


/** @author Max Howell <max@last.fm>
  * An implementation of the Audioscrobbler Realtime Submissions Protocol 
  * version 1.2 for a single Last.fm user
  * http://www.audioscrobbler.net/development/protocol/
  */
class Scrobbler : public QObject
{
    Q_OBJECT

    QString const m_username;
    QString const m_password;

    class ScrobblerHandshake* m_handshake;
    class NowPlaying* m_np;
    class ScrobblerSubmission* m_submitter;
    class ScrobbleCache* m_cache;
    uint m_hard_failures;

public:
    /** password should be already a 32 character md5 hash */
    Scrobbler( const QString& username, const QString& password );
    ~Scrobbler();

    /** will ask Last.fm to update the now playing information for username() */
    void nowPlaying( const class Track& );
    /** will cache the track, but we won't submit it until you call submit() */
    void cache( const Track& );
    /** will submit the ScrobbleCache for this user */
    void submit();

    QString username() const { return m_username; }

    enum Status
    {
        Connecting,
        Handshaken,
        Scrobbling,
        TracksScrobbled,
        StatusMax
    };

    enum Error
    {
        /** the following will show via the status signal, the scrobbler will
        * not submit this session (np too), however caching will continue */
        ErrorBadSession = StatusMax,
        ErrorBannedClient,
        ErrorInvalidSessionKey,
        ErrorBadTime,
        ErrorThreeHardFailures,

        NoError
    };

signals:
    /** the controller should show status in an appropriate manner */
    void status( int code, QVariant data = QVariant() );

private:
    void handshake();
    void onError( Scrobbler::Error );

private slots:
    void onHandshakeReturn( const QByteArray& );
    void onNowPlayingReturn( const QByteArray& );
    void onSubmissionReturn( const QByteArray& );
    void onSubmissionStarted( int );
    void onHandshakeHeaderReceived( const class QHttpResponseHeader& );
};

#endif /* SCROBBLER_H */
