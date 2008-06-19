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


TrackInfo::TrackInfo( const QDomElement& e )
{
    m_artist = e.namedItem( "artist" ).toElement().text();
    m_album =  e.namedItem( "album" ).toElement().text();
    m_title = e.namedItem( "track" ).toElement().text();
    m_trackNumber = 0;
    m_duration = e.namedItem( "duration" ).toElement().text().toInt();
    m_playCount = e.namedItem( "playcount" ).toElement().text().toInt();
    m_path = e.namedItem( "filename" ).toElement().text();
    m_ratingFlags = e.namedItem( "userActionFlags" ).toElement().text().toUInt();

    // this is necessary because the default return for toInt() is 0, and that
    // corresponds to Radio not Unknown :( oops.
    QString const source = e.namedItem( "source" ).toElement().text();
    if (source.isEmpty())
        m_source = Unknown;
    else
        m_source = (Source)source.toInt();

    // support 1.1.3 stringed timestamps, and 1.3.0 Unix Timestamps
    QString const timestring = e.namedItem( "timestamp" ).toElement().text();
    QDateTime const timestamp = QDateTime::fromString( timestring, "yyyy-MM-dd hh:mm:ss" );
    if (timestamp.isValid())
        m_timeStamp = timestamp.toTime_t();
    else
        m_timeStamp = timestring.toUInt();
#if 0
    setPath( e.namedItem( "path" ).toElement().text() );
    setFpId( e.namedItem( "fpId" ).toElement().text() );
    setMbId( e.namedItem( "mbId" ).toElement().text() );
    setPlayerId( e.namedItem( "playerId" ).toElement().text() );
#endif
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
    makeElement( "track", m_title );
    makeElement( "duration", QString::number( m_duration ) );
    makeElement( "timestamp", QString::number( m_timeStamp ) );
    makeElement( "playcount", QString::number( m_playCount ) );
    makeElement( "filename", m_path );
    makeElement( "source", QString::number( m_source ) );
    makeElement( "userActionFlags", QString::number(m_ratingFlags) );
    makeElement( "path", path() );
    makeElement( "fpId", fpId() );
    makeElement( "mbId", mbId() );
    makeElement( "playerId", playerId() );

    return item;
}


void
TrackInfo::merge( const TrackInfo& that )
{
    m_ratingFlags |= that.m_ratingFlags;

    if ( m_artist.isEmpty() ) m_artist = that.artist();
    if ( m_title.isEmpty() ) m_title = that.track();
    if ( m_trackNumber == 0 ) m_trackNumber = that.trackNumber();
}


QString
TrackInfo::toString() const
{
    if ( m_artist.isEmpty() )
    {
        if ( m_title.isEmpty() )
            return QFileInfo( m_path ).fileName();
        else
            return m_title;
    }

    if ( m_title.isEmpty() )
        return m_artist;

    return m_artist + ' ' + QChar(8211) /*en dash*/ + ' ' + m_title;
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


QString
TrackInfo::sourceString() const
{
    switch (m_source)
    {
        case Radio: return "L" /*+ authCode()*/;
        case Player: return "P" /*+ playerId()*/;
        case MediaDevice: return "P" /*+ mediaDeviceId()*/;
        default: return "U";
    }
}


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

