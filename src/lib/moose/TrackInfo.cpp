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

#include "TrackInfo.h"
#include "Settings.h"
#include "lib/unicorn/Logger.h"
#include <QDir>


TrackInfo::TrackInfo()
{
    d = new TrackInfoData;
}


TrackInfo::TrackInfo( const QDomElement& e )
{
    d = new TrackInfoData;

    d->artist = e.namedItem( "artist" ).toElement().text();
    d->album =  e.namedItem( "album" ).toElement().text();
    d->title = e.namedItem( "track" ).toElement().text();
    d->trackNumber = 0;
    d->duration = e.namedItem( "duration" ).toElement().text().toInt();
    d->playCount = e.namedItem( "playcount" ).toElement().text().toInt();
    d->path = e.namedItem( "filename" ).toElement().text();
    d->ratingFlags = e.namedItem( "userActionFlags" ).toElement().text().toUInt();

    // this is necessary because the default return for toInt() is 0, and that
    // corresponds to Radio not Unknown :( oops.
    QString const source = e.namedItem( "source" ).toElement().text();
    if (source.isEmpty())
        d->source = Unknown;
    else
        d->source = (Source)source.toInt();

    // support 1.1.3 stringed timestamps, and 1.3.0 Unix Timestamps
    QString const t130 = e.namedItem( "timestamp" ).toElement().text();
    QDateTime const t113 = QDateTime::fromString( t130, "yyyy-MM-dd hh:mm:ss" );
    if (t113.isValid())
        d->time = t113;
    else
        d->time = QDateTime::fromTime_t( t130.toUInt() );

#if 0
    //old code, most likely unused
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

    makeElement( "artist", d->artist );
    makeElement( "album", d->album );
    makeElement( "track", d->title );
    makeElement( "duration", QString::number( d->duration ) );
    makeElement( "timestamp", QString::number( d->time.toTime_t() ) );
    makeElement( "playcount", QString::number( d->playCount ) );
    makeElement( "filename", d->path );
    makeElement( "source", QString::number( d->source ) );
    makeElement( "userActionFlags", QString::number(d->ratingFlags) );
    makeElement( "path", path() );
    makeElement( "fpId", fpId() );
    makeElement( "mbId", mbId() );
    makeElement( "playerId", playerId() );

    return item;
}


QString
TrackInfo::toString() const
{
    if ( d->artist.isEmpty() )
    {
        if ( d->title.isEmpty() )
            return QFileInfo( d->path ).fileName();
        else
            return d->title;
    }

    if ( d->title.isEmpty() )
        return d->artist;

    return d->artist + ' ' + QChar(8211) /*en dash*/ + ' ' + d->title;
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


QString
TrackInfo::sourceString() const
{
    switch (d->source)
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
    QTime t = QTime().addSecs( d->duration );
    if (d->duration < 60*60)
        return t.toString( "m:ss" );
    else
        return t.toString( "hh:mm:ss" );
}



TrackInfo::ScrobblableStatus
TrackInfo::scrobblableStatus() const
{
    if ( duration() < kScrobbleMinLength )
    {
        LOGL( 3, "Duration is too short (" << duration() << "s), will not submit.\n" );
        return TooShort;
    }

    // Radio tracks above preview length always scrobble
    if ( source() == TrackInfo::Radio )
    {
        return OkToScrobble;
    }

    if ( !timeStamp().isValid() )
    {
        LOGL( 3, "Invalid timestamp, will not submit" );
        return NoTimeStamp;
    }

    // actual spam prevention is something like 12 hours, but we are only
    // trying to weed out obviously bad data, server side criteria for
    // "the future" may change, so we should let the server decide, not us
    if ( timeStamp() > QDateTime::currentDateTime().addMonths( 1 ) )
    {
        LOGL( 3, "Track is more than a month in the future, will not submit" );
        return FromTheFuture;
    }

    if ( timeStamp() < QDateTime::fromString( "2003-01-01", Qt::ISODate ) )
    {
        LOGL( 3, "Track was played before the Audioscrobbler project was founded! Will not submit" );
        return FromTheDistantPast;
    }

    // Check if any required fields are empty
    if ( artist().isEmpty() )
    {
        LOGL( 3, "Artist was missing, will not submit" );
        return ArtistNameMissing;
    }
    if ( track().isEmpty() )
    {
        LOGL( 3, "Artist, track or duration was missing, will not submit" );
        return TrackNameMissing;
    }

    QStringList invalidList;
    invalidList << "unknown artist"
                << "unknown"
                << "[unknown]"
                << "[unknown artist]";

    // Check if artist name is an invalid one like "unknown"
    foreach( QString invalid, invalidList )
    {
        if ( artist().toLower() == invalid )
        {
            LOG( 3, "Artist '" << artist() << "' is an invalid artist name, will not submit.\n" );
            return ArtistInvalid;
        }
    }

    // All tests passed!
    return OkToScrobble;
}


int
TrackInfo::scrobblePoint() const
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

