/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
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

#include "TrackInfo.h"
#include "Settings.h"

#include "lib/unicorn/Logger.h"

#include <QDir>
#include <QDomElement>
#include <QMimeData>


TrackInfo::TrackInfo() :
        m_trackNumber( 0 ),
        m_playCount( 0 ),
        m_duration( 0 ),
        m_timeStamp( 0 ),
        m_source( Unknown ),
        m_ratingFlags( 0 )
{}


#if 0
TrackInfo::TrackInfo( const QDomElement& e )
{
    setArtist( e.namedItem( "artist" ).toElement().text() );
    setAlbum( e.namedItem( "album" ).toElement().text() );
    setTrack( e.namedItem( "track" ).toElement().text() );
    setTrackNr( 0 );
    setDuration( e.namedItem( "duration" ).toElement().text() );
    setPlayCount( e.namedItem( "playcount" ).toElement().text().toInt() );
    setFileName( e.namedItem( "filename" ).toElement().text() );
    setUniqueID( e.namedItem( "uniqueID" ).toElement().text() );
    setSource( (Source)e.namedItem( "source" ).toElement().text().toInt() );
    setAuthCode( e.namedItem( "authorisationKey" ).toElement().text() );
    m_ratingFlags = e.namedItem( "userActionFlags" ).toElement().text().toUInt();
    m_mediaDeviceId = e.namedItem( "mediaDeviceId" ).toElement().text();

    // this is necessary because the default return for toInt() is 0, and that
    // corresponds to Radio not Unknown :( oops.
    QString const source = e.namedItem( "source" ).toElement().text();
    if (source.isEmpty())
        setSource( Unknown );
    else
        setSource( (Source)source.toInt() );

    // support 1.1.3 stringed timestamps, and 1.3.0 Unix Timestamps
    QString const timestring = e.namedItem( "timestamp" ).toElement().text();
    QDateTime const timestamp = QDateTime::fromString( timestring, "yyyy-MM-dd hh:mm:ss" );
    if (timestamp.isValid())
        setTimeStamp( timestamp.toTime_t() );
    else
        setTimeStamp( timestring.toUInt() );

    setPath( e.namedItem( "path" ).toElement().text() );
    setFpId( e.namedItem( "fpId" ).toElement().text() );
    setMbId( e.namedItem( "mbId" ).toElement().text() );
    setPlayerId( e.namedItem( "playerId" ).toElement().text() );
}


QDomElement
TrackInfo::toDomElement( QDomDocument& document ) const
{
    QDomElement item = document.createElement( "item" );

    #define makeElement( tagname, getter ) \
    { \
        QDomElement e = document.createElement( tagname ); \
        e.appendChild( document.createTextNode( getter ) ); \
        item.appendChild( e ); \
    }

    makeElement( "artist", m_artist );
    makeElement( "album", m_album );
    makeElement( "track", m_track );
    makeElement( "duration", QString::number( m_duration ) );
    makeElement( "timestamp", QString::number( m_timeStamp ) );
    makeElement( "playcount", QString::number( m_playCount ) );
    makeElement( "filename", m_fileName );
    makeElement( "uniqueID", m_uniqueID );
    makeElement( "source", QString::number( m_source ) );
    makeElement( "authorisationKey", m_authCode );
    makeElement( "userActionFlags", QString::number(m_ratingFlags) );
    makeElement( "path", path() );
    makeElement( "fpId", fpId() );
    makeElement( "mbId", mbId() );
    makeElement( "playerId", playerId() );
    makeElement( "mediaDeviceId", m_mediaDeviceId );

    return item;
}


void
TrackInfo::merge( const TrackInfo& that )
{
    m_ratingFlags |= that.m_ratingFlags;

    if ( m_artist.isEmpty() ) setArtist( that.artist() );
    if ( m_track.isEmpty() ) setTrack( that.track() );
    if ( m_trackNr == 0 ) setTrackNr( that.trackNr() );
    // can't do this, we don't know
    //if ( m_playCount == 0 ) setPlayCount( that.playCount() );
    if ( m_duration == 0 ) setDuration( that.duration() );
    if ( m_fileName.isEmpty() ) setFileName( that.fileName() );
    if ( m_mbId.isEmpty() ) setMbId( that.mbId() );
    if ( m_timeStamp == 0 ) setTimeStamp( that.timeStamp() );
    if ( m_source == Unknown ) setSource( that.source() );
    if ( m_authCode.isEmpty() ) setAuthCode( that.authCode() );
    if ( m_uniqueID.isEmpty() ) setUniqueID( that.uniqueID() );
    if ( m_playerId.isEmpty() ) setPlayerId( that.playerId() );
    if ( m_powerPlayLabel.isEmpty() ) setPowerPlayLabel( that.powerPlayLabel() );
    if ( m_paths.isEmpty() ) setPaths( that.m_paths );
    // can't do this
    //if ( m_nextPath.isEmpty() )
    if ( m_username.isEmpty() ) setUsername( that.username() );
    if ( m_fpId.isEmpty() ) setFpId( that.fpId() );
    if ( m_mediaDeviceId.isEmpty() ) setMediaDeviceId( that.mediaDeviceId() );
}
#endif


QString
TrackInfo::toString() const
{
    if ( m_artist.isEmpty() )
    {
        if ( m_track.isEmpty() )
            return QFileInfo( m_path ).fileName();
        else
            return m_track;
    }

    if ( m_track.isEmpty() )
        return m_artist;

    return m_artist + ' ' + QChar(8211) /*en dash*/ + ' ' + m_track;
}


QString
TrackInfo::ratingCharacter() const
{
    if (isBanned()) return "B";
    if (isLoved()) return "L";
    if (isScrobbled()) return "";
    if (isSkipped()) return "S";

    return "";
}


void
MutableTrackInfo::timeStampMe()
{
    setTimeStamp( QDateTime::currentDateTime().toTime_t() );
}


#if 0
QString
TrackInfo::sourceString() const
{
    switch (m_source)
    {
        case Radio: return "L" + authCode();
        case Player: return "P" + playerId();
        case MediaDevice: return "P" + mediaDeviceId();
        default: return "U";
    }
}
#endif


QString
TrackInfo::durationString() const
{
    QTime t = QTime().addSecs( m_duration );
    if (m_duration < 60*60)
        return t.toString( "m:ss" );
    else
        return t.toString( "hh:mm:ss" );
}



TrackInfo::ScrobblableStatus
TrackInfo::scrobblableStatus() const
{
    const TrackInfo& track = *this; //FIXME

    // Check duration
    if ( track.duration() < kScrobbleMinLength )
    {
        LOG( 3, "Track length is " << track.duration() << " s which is too short, will not submit.\n" );
        return TooShort;
    }

    // Radio tracks above preview length always scrobble
    if ( track.source() == TrackInfo::Radio )
    {
        return OkToScrobble;
    }

    // Check timestamp
    if ( track.timeStamp() == 0 )
    {
        LOG( 3, "Track has no timestamp, will not submit.\n" );
        return NoTimeStamp;
    }

    // actual spam prevention is something like 12 hours, but we are only
    // trying to weed out obviously bad data, server side criteria for
    // "the future" may change, so we should let the server decide, not us
    if ( track.timeStamp() > QDateTime::currentDateTime().addMonths( 1 ).toTime_t() )
    {
        LOGL( 3, "Track is more than a month in the future, will not submit" );
        return FromTheFuture;
    }

    if ( track.timeStamp() < QDateTime::fromString( "2003-01-01", Qt::ISODate ).toTime_t() )
    {
        LOGL( 3, "Track was played before the Audioscrobbler project was founded! Will not submit" );
        return FromTheDistantPast;
    }

    // Check if any required fields are empty
    if ( track.artist().isEmpty() )
    {
        LOG( 3, "Artist was missing, will not submit.\n" );
        return ArtistNameMissing;
    }
    if ( track.track().isEmpty() )
    {
        LOG( 3, "Artist, track or duration was missing, will not submit.\n" );
        return TrackNameMissing;
    }

    // Check if dir excluded
    if ( isDirExcluded( track.path() ) )
    {
        LOG( 3, "Track is in excluded directory `" << track.path() << "', " << "will not submit.\n" );
        return ExcludedDir;
    }

    QStringList invalidList;
    invalidList << "unknown artist"
        << "unknown"
        << "[unknown]"
        << "[unknown artist]";

    // Check if artist name is an invalid one like "unknown"
    foreach( QString invalid, invalidList )
    {
        if ( track.artist().toLower() == invalid )
        {
            LOG( 3, "Artist '" << track.artist() << "' is an invalid artist name, will not submit.\n" );
            return ArtistInvalid;
        }
    }

    // All tests passed!
    return OkToScrobble;
}


bool
TrackInfo::isDirExcluded( const QString& path ) const
{
    QString pathToTest = QDir( path ).absolutePath();
#ifdef WIN32
    pathToTest = pathToTest.toLower();
#endif

    if (pathToTest.isEmpty())
        return false;

    foreach ( QString bannedPath, Moose::Settings().excludedDirs() )
    {
        bannedPath = QDir( bannedPath ).absolutePath();
#ifdef WIN32
        bannedPath = bannedPath.toLower();
#endif

        // Try and match start of given path with banned dir
        if ( pathToTest.startsWith( bannedPath ) )
        {
            // Found, this path is from a banned dir
            return true;
        }
    }

    // The path wasn't found in exclusions list
    return false;
}


int
TrackInfo::scrobbleTime() const
{
    // If we don't have a length or it's less than the minimum, return the
    // threshold
    if ( duration() <= 0 || duration() < kScrobbleMinLength )
        return kScrobbleTimeMax;

    float scrobPoint = qBound( kScrobblePointMin,
                               Moose::Settings().scrobblePoint(),
                               kScrobblePointMax );
    scrobPoint /= 100.0f;

    return qMin( kScrobbleTimeMax, int( duration() * scrobPoint ) );
}

