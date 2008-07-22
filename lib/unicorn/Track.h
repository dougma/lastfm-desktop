/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef UNICORN_TRACK_H
#define UNICORN_TRACK_H

#include "Album.h"
#include "Artist.h"
#include "lib/DllExportMacro.h"
#include <QDateTime>
#include <QDomElement>
#include <QExplicitlySharedDataPointer>
#include <QString>
#include <QMap>
#include <QUrl>


struct TrackData : QSharedData
{
    TrackData();

    QString artist;
    QString album;
    QString title;
    int trackNumber;
    int playCount;
    int duration;
    short source;
    short ratingFlags;
    QString playerId;
    QString mbId; /// musicbrainz id
    QString fpId; /// fingerprint id
    QUrl url;
    QDateTime time; /// the time the track was started at

    //FIXME I hate this, but is used for radio trackauth etc.
    QMap<QString,QString> extras;
};


class UNICORN_DLLEXPORT Track
{
public:
    enum Source
    {
        // DO NOT UNDER ANY CIRCUMSTANCES CHANGE THE ORDER OR VALUES OF THIS ENUM!
        // you will cause broken settings and b0rked scrobbler cache submissions

        Unknown = -1,
        Radio,
        Player,
        MediaDevice
    };

    enum RatingFlag
    {
        // DO NOT UNDER ANY CIRCUMSTANCES CHANGE THE ORDER OR VALUES OF THIS ENUM!
        // you will cause broken settings and b0rked scrobbler cache submissions

        Skipped = 1,
        Loved = 2,
        Banned = 4,
        Scrobbled = 8
    };

    enum ScrobblableStatus
    {
        OkToScrobble,
        NoTimeStamp,
        TooShort,
        ArtistNameMissing,
        TrackNameMissing,
        ExcludedDir,
        ArtistInvalid,
        FromTheFuture,
        FromTheDistantPast
    };

    Track();
    Track( const QDomElement& );

    /** this track and that track point to the same object, so they are the same
      * in fact. This doesn't do a deep data comparison. So even if all the 
      * fields are the same it will return false if they aren't in fact spawned
      * from the same initial Track object */
    bool operator==( const Track& that ) const
    {
        return this->d == that.d;
    }

    /** not a great isEmpty check, but most services will complain if these two
      * are empty */
    bool isEmpty() const { return d->artist.isEmpty() && d->title.isEmpty(); }

    /** the standard representation of this object as an XML node */
    QDomElement toDomElement( class QDomDocument& ) const;

    Artist artist() const { return Artist( d->artist ); }
    Album album() const { return Album( artist(), d->album ); }
    QString title() const { return d->title; }
    int trackNumber() const { return d->trackNumber; }
    int playCount() const { return d->playCount; }
    int duration() const { return d->duration; }
    QString durationString() const;
    QString mbId() const { return d->mbId; }
    QUrl url() const { return d->url; }
    QDateTime timeStamp() const { return d->time; }
    QDateTime dateTime() const { return d->time; }
    Source source() const { return (Source)d->source; }
    /** scrobbler submission source string code */
    QString sourceString() const;
    QString playerId() const { return d->playerId; }
    QString fpId() const { return d->fpId; }

    bool isSkipped() const { return d->ratingFlags & Track::Skipped; }
    bool isLoved() const { return d->ratingFlags & Track::Loved; }
    bool isBanned() const { return d->ratingFlags & Track::Banned; }
    bool isScrobbled() const { return d->ratingFlags & Track::Scrobbled; }

    QString prettyTitle( const QChar& separator = QChar(8211) /*en dash*/ ) const;

    /** only one rating is possible, we have to figure out which from various flags applied */
    QString ratingCharacter() const;
    
    /** Works out if passed-in track can be scrobbled and returns the 
      * status. */
    ScrobblableStatus scrobblableStatus() const;

    /** Returns the second at which passed-in track reached the scrobble 
      * point. */
    int scrobblePoint() const;

    /** used to sort tracks into chronological order, used by scrobbling */
    static bool lessThan( const Track &t1, const Track &t2)
    {
        return t1.timeStamp() < t2.timeStamp();
    }

    // TODO not asyncronous! return a WsReply object!
    QStringList topTags() const;

    WsReply* share( const class User& recipient, const QString& message = "" );

protected:
    friend class MutableTrack; //FIXME wtf? but compiler error otherwise
    QExplicitlySharedDataPointer<TrackData> d;

public:
    // Limits for user-configurable scrobble point in percent
    static const int kScrobblePointMin = 50;
    static const int kScrobblePointMax = 100;
    // Shortest track length allowed to scrobble in seconds
    static const int kScrobbleMinLength = 31;
    // Upper limit for scrobble time in seconds
    static const int kScrobbleTimeMax = 240;
    // Percentage of track length at which to scrobble
    //TODO should be a float, percentages are meaningless in the middle of code
    static const int kDefaultScrobblePoint = 50;
};


class UNICORN_DLLEXPORT MutableTrack : public Track
{
public:
    MutableTrack()
    {}

    MutableTrack( const Track& that )
    {
        this->d = that.d;
    }

    void setArtist( QString artist ) { d->artist = artist.trimmed(); }
    void setAlbum( QString album ) { d->album = album.trimmed(); }
    void setTitle( QString title ) { d->title = title.trimmed(); }
    void setTrackNumber( int n ) { d->trackNumber = n; }
    void setPlayCount( int playCount ) { d->playCount = playCount; }
    void setDuration( int duration ) { d->duration = duration; }
    void setMbId( QString mbId ) { d->mbId = mbId; }
    void setUrl( QUrl url ) { d->url = url; }
    void setSource( Source s ) { d->source = s; }
    void setRatingFlag( RatingFlag flag ) { d->ratingFlags |= flag; }
    void setPlayerId( QString id ) { d->playerId = id; }
    void setFpId( QString id ) { d->fpId = id; }
    
    void setTimeStampNow() { d->time = QDateTime::currentDateTime(); }

    void setExtra( QString key, QString value ) { d->extras[key] = value; }
};


inline 
TrackData::TrackData() 
             : trackNumber( 0 ),
               playCount( 0 ),
               duration( 0 ),
               source( Track::Unknown ),
               ratingFlags( 0 )
{}


#include <QDebug>
inline QDebug operator<<( QDebug d, const Track& t )
{
    return d << t.prettyTitle( '-' );
}


#include <QMetaType>
Q_DECLARE_METATYPE( Track );

#endif
