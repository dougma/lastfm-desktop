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

#ifndef TRACK_INFO_H
#define TRACK_INFO_H

#include "lib/DllExportMacro.h"
#include <QDateTime>
#include <QDomElement>
#include <QExplicitlySharedDataPointer>
#include <QString>


struct TrackInfoData : QSharedData
{
    TrackInfoData();

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
    QString path;
    QDateTime time; /// the time the track was started at
};


class MOOSE_DLLEXPORT TrackInfo
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

    TrackInfo();
    TrackInfo( const QDomElement& );

    /** this track and that track point to the same object, so they are the same
      * in fact. This doesn't do a deep data comparison. So even if all the 
      * fields are the same it will return false if they aren't in fact spawned
      * from the same initial TrackInfo object */
    bool operator==( const TrackInfo& that ) const
    {
        return this->d == that.d;
    }

    /** not a great isEmpty check, but most services will complain if these two
      * are empty */
    bool isEmpty() const { return d->artist.isEmpty() && d->title.isEmpty(); }

    /** the standard representation of this object as an XML node */
    QDomElement toDomElement( class QDomDocument& ) const;

    QString artist() const { return d->artist; }
    QString album() const { return d->album; }
    QString track() const { return d->title; } //TODO rename title
    int trackNumber() const { return d->trackNumber; }
    int playCount() const { return d->playCount; }
    int duration() const { return d->duration; }
    QString durationString() const;
    QString mbId() const { return d->mbId; }
    QString path() const { return d->path; }
    QDateTime timeStamp() const { return d->time; }
    QDateTime dateTime() const { return d->time; }
    Source source() const { return (Source)d->source; }
    /** scrobbler submission source string code */
    QString sourceString() const;
    QString playerId() const { return d->playerId; }
    QString fpId() const { return d->fpId; }

    bool isSkipped() const { return d->ratingFlags & TrackInfo::Skipped; }
    bool isLoved() const { return d->ratingFlags & TrackInfo::Loved; }
    bool isBanned() const { return d->ratingFlags & TrackInfo::Banned; }
    bool isScrobbled() const { return d->ratingFlags & TrackInfo::Scrobbled; }

    QString toString() const;

    /** only one rating is possible, we have to figure out which from various flags applied */
    QString ratingCharacter() const;
    
    /** Works out if passed-in track can be scrobbled and returns the 
      * status. */
    ScrobblableStatus scrobblableStatus() const;

    /** Returns the second at which passed-in track reached the scrobble 
      * point. */
    int scrobblePoint() const;

    /** used to sort tracks into chronological order, used by scrobbling */
    static bool lessThan( const TrackInfo &t1, const TrackInfo &t2)
    {
        return t1.timeStamp() < t2.timeStamp();
    }

protected:
    friend class MutableTrackInfo; //FIXME wtf? but compiler error otherwise
    QExplicitlySharedDataPointer<TrackInfoData> d;

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


class MOOSE_DLLEXPORT MutableTrackInfo : public TrackInfo
{
public:
    MutableTrackInfo()
    {}

    MutableTrackInfo( const TrackInfo& that )
    {
        this->d = that.d;
    }

    void setArtist( QString artist ) { d->artist = artist.trimmed(); }
    void setAlbum( QString album ) { d->album = album.trimmed(); }
    void setTrack( QString track ) { d->title = track.trimmed(); }
    void setTrackNumber( int n ) { d->trackNumber = n; }
    void setPlayCount( int playCount ) { d->playCount = playCount; }
    void setDuration( int duration ) { d->duration = duration; }
    void setMbId( QString mbId ) { d->mbId = mbId; }
    void setPath( QString path ) { d->path = path; }
    void setSource( Source s ) { d->source = s; }
    void setRatingFlag( RatingFlag flag ) { d->ratingFlags |= flag; }
    void setPlayerId( QString id ) { d->playerId = id; }
    void setFpId( QString id ) { d->fpId = id; }
    
    void setTimeStampNow() { d->time = QDateTime::currentDateTime(); }
};


inline 
TrackInfoData::TrackInfoData() 
             : trackNumber( 0 ),
               playCount( 0 ),
               duration( 0 ),
               source( TrackInfo::Unknown ),
               ratingFlags( 0 )
{}


#include <QDebug>
inline QDebug operator<<( QDebug& d, const TrackInfo& t )
{
    return d << t.toString().replace( QChar(8211), '-' );
}


#include <QMetaType>
Q_DECLARE_METATYPE( TrackInfo );

#endif
