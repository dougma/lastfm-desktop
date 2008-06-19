/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "common/DllExportMacro.h"
#include <QDomElement>
#include <QString>

//TODO shared data pointer


class DLLEXPORT TrackInfo
{
    // undefined because really what consititutes the same track varies depending
    // on usage. Some Qt templates require this operator though so maybe you'll
    // want to add it at some point in the future, my suggestion is don't, it'll
    // cause bugs --mxcl
    bool operator==( const TrackInfo& ) const;

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

    /** not a great isEmpty check, but most services will complain if these two
      * are empty */
    bool isEmpty() const { return m_artist.isEmpty() && m_title.isEmpty(); }

    //TODO remove
    bool sameAs( const TrackInfo& that ) const
    {
        // This isn't an ideal check, but it's the best we can do until we introduce
        // unique IDs for tracks. Since the artist/track info of a track can change
        // after we receive metadata, just comparing on metadata isn't reliable. So
        // we use the paths instead if we have them. And if not, fall back on metadata.

        if ( !this->path().isEmpty() && !that.path().isEmpty() )
            return this->path() == that.path();

        if ( this->artist() != that.artist() )
            return false;

        if ( this->track() != that.track() )
            return false;

        return true;
    }

    //TODO remove
    void merge( const TrackInfo& that );
    //TODO remove
    QDomElement toDomElement( class QDomDocument& ) const;
    //TODO remove
    TrackInfo( const QDomElement& );

    QString artist() const { return m_artist; }
    QString album() const { return m_album; }
    QString track() const { return m_title; }
    int trackNumber() const { return m_trackNumber; }
    int playCount() const { return m_playCount; }
    int duration() const { return m_duration; }
    QString durationString() const;
    QString mbId() const { return m_mbId; }
    QString path() const { return m_path; }
    time_t timeStamp() const { return m_timeStamp; }
    Source source() const { return m_source; }
    /** scrobbler submission source string code */
    QString sourceString() const;
    QString playerId() const { return m_playerId; }

    bool isSkipped() const { return m_ratingFlags & TrackInfo::Skipped; }
    bool isLoved() const { return m_ratingFlags & TrackInfo::Loved; }
    bool isBanned() const { return m_ratingFlags & TrackInfo::Banned; }
    bool isScrobbled() const { return m_ratingFlags & TrackInfo::Scrobbled; }

    QString toString() const;

    /** only one rating is possible, we have to figure out which from various flags applied */
    QString ratingCharacter() const;
    QString fpId() const { return m_fpId; }
    
    /** Checks whether the passed-in path is in a directory that the user has
      * excluded from scrobbling. */
    bool isDirExcluded( const QString& path ) const;

    /** Works out if passed-in track can be scrobbled and returns the 
      * status. */
    ScrobblableStatus scrobblableStatus() const;

    /** Returns the second at which passed-in track reached the scrobble 
      * point. */
    int scrobbleTime() const; //FIXME remove
    int scrobblePoint() const { return scrobbleTime(); }

protected:
    QString m_artist;
    QString m_album;
    QString m_title;
    int     m_trackNumber;
    int     m_playCount;
    int     m_duration;
    short   m_ratingFlags;
    time_t  m_timeStamp;
    Source  m_source;
    QString m_playerId;
    QString m_mbId;
    QString m_fpId; /// fingerprint id
    QString m_path;

public:
    //TODO remove?
    // Limits for user-configurable scrobble point (%)
    static const int kScrobblePointMin = 50;
    static const int kScrobblePointMax = 100;
    // Shortest track length allowed to scrobble (s)
    static const int kScrobbleMinLength = 31;
    // Upper limit for scrobble time (s)
    static const int kScrobbleTimeMax = 240;
    // Percentage of track length at which to scrobble
    //TODO should be a float, percentages are meaningless in the middle of code
    static const int kDefaultScrobblePoint = 50;
};


class DLLEXPORT MutableTrackInfo : public TrackInfo
{
public:
    MutableTrackInfo()
    {}

    void setArtist( QString artist ) { m_artist = artist.trimmed(); }
    void setAlbum( QString album ) { m_album = album.trimmed(); }
    void setTrack( QString track ) { m_title = track.trimmed(); }
    void setTrackNr( int n ) { m_trackNumber = n; }
    void setPlayCount( int playCount ) { m_playCount = playCount; }
    void setDuration( int duration ) { m_duration = duration; }
    void setMbId( QString mbId ) { m_mbId = mbId; }
    void setPath( QString path ) { m_path = path; }
    void setTimeStamp( time_t timestamp ) { m_timeStamp = timestamp; }
    void timeStampMe();
    void setSource( Source s ) { m_source = s; }
    void setRatingFlag( RatingFlag flag ) { m_ratingFlags |= flag; }
    void setPlayerId( QString id ) { m_playerId = id; }
    void setFpId( QString id ) { m_fpId = id; }
};


#include <QDebug>
inline QDebug operator<<( QDebug& d, const TrackInfo& t )
{
    return d << t.toString();
}


#include <QMetaType>
Q_DECLARE_METATYPE( TrackInfo );

#endif
