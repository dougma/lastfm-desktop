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

#include "Track.h"
#include "User.h"
#include "lib/core/CoreUrl.h"
#include "lib/unicorn/UnicornSettings.h" //FIXME
#include "lib/ws/WsRequestBuilder.h"
#include "lib/ws/WsReply.h"
#include <QDir>


Track::Track()
{
    d = new TrackData;
}


Track::Track( const QDomElement& e )
{
    d = new TrackData;

    d->artist = e.namedItem( "artist" ).toElement().text();
    d->album =  e.namedItem( "album" ).toElement().text();
    d->title = e.namedItem( "track" ).toElement().text();
    d->trackNumber = 0;
    d->duration = e.namedItem( "duration" ).toElement().text().toInt();
    d->playCount = e.namedItem( "playcount" ).toElement().text().toInt();
    d->url = e.namedItem( "url" ).toElement().text();
    d->rating = e.namedItem( "rating" ).toElement().text().toUInt();
	d->extras["trackauth"] = e.namedItem( "auth" ).toElement().text();

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
Track::toDomElement( QDomDocument& document ) const
{
    QDomElement item = document.createElement( "item" );

    #define makeElement( tagname, getter ) { \
		QString v = getter; \
		if (!v.isEmpty())\
		{ \
			QDomElement e = document.createElement( tagname ); \
			e.appendChild( document.createTextNode( v ) ); \
			item.appendChild( e ); \
		} \
	}

    makeElement( "artist", d->artist );
    makeElement( "album", d->album );
    makeElement( "track", d->title );
    makeElement( "duration", QString::number( d->duration ) );
    makeElement( "timestamp", QString::number( d->time.toTime_t() ) );
    makeElement( "playcount", QString::number( d->playCount ) );
    makeElement( "url", d->url.toString() );
    makeElement( "source", QString::number( d->source ) );
    makeElement( "rating", QString::number(d->rating) );
    makeElement( "fpId", fpId() );
    makeElement( "mbId", mbId() );
    makeElement( "playerId", playerId() );
	makeElement( "auth", d->extras["trackauth"] );

    return item;
}


QString
Track::prettyTitle( const QChar& separator ) const
{
    if ( d->artist.isEmpty() )
    {
        if ( d->title.isEmpty() )
            return QFileInfo( d->url.path() ).fileName();
        else
            return d->title;
    }

    if ( d->title.isEmpty() )
        return d->artist;

    return d->artist + ' ' + separator + ' ' + d->title;
}


QString
Track::ratingCharacter() const
{
	switch (d->rating)
	{
		case NotScrobbled: return "";
		case Scrobbled: return "";
		case Skipped: return "S";
		case Loved: return "L";
		case Banned: return "B";
		default:
			Q_ASSERT_X( 0, "ratingCharacter()", "Unhandled rating enum value" );
			return "";
	}
}


QString
Track::sourceString() const
{
    switch (d->source)
    {
        case LastFmRadio: return "L" + d->extras["trackauth"];
        case Player: return "P" /*+ playerId()*/;
        case MediaDevice: return "P" /*+ mediaDeviceId()*/;
		case NonPersonalisedBroadcast: return "R";
		case PersonalisedRecommendation: return "E";
        default: return "U";
    }
}


QString
Track::durationString() const
{
    QTime t = QTime().addSecs( d->duration );
    if (d->duration < 60*60)
        return t.toString( "m:ss" );
    else
        return t.toString( "hh:mm:ss" );
}


#if 0 
Track::ScrobblableStatus
Track::scrobblableStatus() const
{
    if ( duration() < ScrobblePoint::kScrobbleMinLength )
    {
        LOGL( 3, "Duration is too short (" << duration() << "s), will not submit.\n" );
        return TooShort;
    }

    // Radio tracks above preview length always scrobble
    if ( source() == Track::Radio )
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
    if ( d->artist.isEmpty() )
    {
        LOGL( 3, "Artist was missing, will not submit" );
        return ArtistNameMissing;
    }
    if ( d->title.isEmpty() )
    {
        LOGL( 3, "Artist, track or duration was missing, will not submit" );
        return TrackNameMissing;
    }

    QStringList invalidList;
    invalidList << "unknown artist"
                << "unknown"
                << "[unknown]"
                << "[unknown artist]";

    foreach( QString invalid, invalidList )
    {
        if ( d->artist.toLower() == invalid )
        {
            LOG( 3, "Artist '" << d->artist << "' is an invalid artist name, will not submit.\n" );
            return ArtistInvalid;
        }
    }

    // All tests passed!
    return OkToScrobble;
}
#endif


WsReply*
Track::share( const User& recipient, const QString& message )
{
    return WsRequestBuilder( "track.share" )
        .add( "recipient", recipient )
        .add( "artist", d->artist )
        .add( "track", d->title )
        .addIfNotEmpty( "message", message )
        .post();
}


WsReply*
Track::love()
{
	return WsRequestBuilder( "track.love" )
		.add( "artist", d->artist )
		.add( "track", d->title )
		.post();
}


WsReply*
Track::ban()
{
	return WsRequestBuilder( "track.ban" )
		.add( "artist", d->artist )
		.add( "track", d->title )
		.post();
}


struct TrackWsRequestBuilder : WsRequestBuilder
{
	TrackWsRequestBuilder( const char* p ) : WsRequestBuilder( p )
	{}
	
	TrackWsRequestBuilder& add( const Track& t )
	{
		if (t.mbId().isEmpty()) 
		{
			WsRequestBuilder::add( "artist", t.artist() );
			WsRequestBuilder::add( "track", t.title() );
		}
		else
			WsRequestBuilder::add( "mbid", t.mbId() );

		return *this;
	}
};


WsReply*
Track::getTopTags()
{
	return TrackWsRequestBuilder( "track.getTopTags" ).add( *this ).get();
}


WeightedStringList /* static */
Track::getTopTags( WsReply* r )
{
	WeightedStringList tags;
	try
	{
		foreach (EasyDomElement e, r->lfm().children( "tag" ))
		{
			QString tagname = e["name"].text();
			int count = e["count"].text().toInt();
			tags.push_back( WeightedString( tagname, count ));
		}
			
	}
	catch( EasyDomElement::Exception& e)
	{
		qWarning() << e;
	}
	return tags;
}


WsReply*
Track::getTags()
{
	return TrackWsRequestBuilder( "track.getTags" ).add( *this ).get();
}


QUrl
Track::www() const
{
	QString const artist = CoreUrl::encode( d->artist );
	QString const track = CoreUrl::encode( d->title );
	return CoreUrl( "http://www.last.fm/music/" + artist + "/_/" + track ).localised();
}
