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
#include "../core/CoreUrl.h"
#include "../ws/WsRequestBuilder.h"
#include "../ws/WsReply.h"
#include <QFileInfo>


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
    d->url = e.namedItem( "url" ).toElement().text();
    d->rating = e.namedItem( "rating" ).toElement().text().toUInt();
    d->source = e.namedItem( "source" ).toElement().text().toInt(); //defaults to 0, or Track::Unknown
    d->time = QDateTime::fromTime_t( e.namedItem( "timestamp" ).toElement().text().toUInt() );
    
    QDomNodeList nodes = e.namedItem( "extras" ).childNodes();
    for (int i = 0; i < nodes.count(); ++i)
    {
        QDomNode n = nodes.at(i);
        QString key = n.nodeName();
        d->extras[key] = n.toElement().text();
    }
    
    qDebug() << d->extras;
}


QDomElement
Track::toDomElement( QDomDocument& xml ) const
{
    QDomElement item = xml.createElement( "track" );

    #define makeElement( tagname, getter ) { \
		QString v = getter; \
		if (!v.isEmpty()) \
		{ \
			QDomElement e = xml.createElement( tagname ); \
			e.appendChild( xml.createTextNode( v ) ); \
			item.appendChild( e ); \
		} \
	}

    makeElement( "artist", d->artist );
    makeElement( "album", d->album );
    makeElement( "track", d->title );
    makeElement( "duration", QString::number( d->duration ) );
    makeElement( "timestamp", QString::number( d->time.toTime_t() ) );
    makeElement( "url", d->url.toString() );
    makeElement( "source", QString::number( d->source ) );
    makeElement( "rating", QString::number(d->rating) );
    makeElement( "fpId", fingerprintId() );
    makeElement( "mbId", mbid() );

    QDomElement extras = xml.createElement( "extras" );
    QMapIterator<QString, QString> i( d->extras );
    while (i.hasNext()) {
		QDomElement e = xml.createElement( i.next().key() );
		e.appendChild( xml.createTextNode( i.value() ) );
		extras.appendChild( e );
	}
    item.appendChild( extras );

    return item;
}


QString
Track::toString( const QChar& separator ) const
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
Track::durationString() const
{
    QTime t = QTime().addSecs( d->duration );
    if (d->duration < 60*60)
        return t.toString( "m:ss" );
    else
        return t.toString( "hh:mm:ss" );
}


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
MutableTrack::love()
{
    if (d->extras.value("rating").size())
        return 0;
    
    d->extras["rating"] = "L";
    
	return WsRequestBuilder( "track.love" )
		.add( "artist", d->artist )
		.add( "track", d->title )
		.post();
}


WsReply*
MutableTrack::ban()
{
    d->extras["rating"] = "B";
    
	return WsRequestBuilder( "track.ban" )
		.add( "artist", d->artist )
		.add( "track", d->title )
		.post();
}


void
MutableTrack::unlove()
{
    QString& r = d->extras["rating"];
    if (r == "L") r = "";
}


struct TrackWsRequestBuilder : WsRequestBuilder
{
	TrackWsRequestBuilder( const char* p ) : WsRequestBuilder( p )
	{}
	
	TrackWsRequestBuilder& add( Track const * const t )
	{
		if (t->mbid().isEmpty()) 
		{
			WsRequestBuilder::add( "artist", t->artist() );
			WsRequestBuilder::add( "track", t->title() );
		}
		else
			WsRequestBuilder::add( "mbid", t->mbid() );

		return *this;
	}
};


WsReply*
Track::getTopTags() const
{
	return TrackWsRequestBuilder( "track.getTopTags" ).add( this ).get();
}


WsReply*
Track::getTags() const
{
	return TrackWsRequestBuilder( "track.getTags" ).add( this ).get();
}


WsReply*
Track::addTags( const QStringList& tags ) const
{
    if (tags.isEmpty())
        return 0;
    
    QString comma_separated_tags;
    foreach( QString const tag, tags)
        comma_separated_tags += tag;
    
    return WsRequestBuilder( "track.addTags" )
            .add( "artist", d->artist )
            .add( "track", d->title )
            .add( "tags", comma_separated_tags )
            .post();
}


QUrl
Track::www() const
{
	QString const artist = CoreUrl::encode( d->artist );
	QString const track = CoreUrl::encode( d->title );
	return CoreUrl( "http://www.last.fm/music/" + artist + "/_/" + track ).localised();
}
