/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "AbstractFileBootstrapper.h"
#include "app/twiddly/IPodScrobble.h"
#include <lastfm/CoreDir>
#include <lastfm/Track>
#include <QFile>

static const int k_maxPlaysPerTrack = 10000;
static const int k_maxTotalPlays = 300000;

static const QString XML_VERSION = "1.0";


AbstractFileBootstrapper::AbstractFileBootstrapper( QString product, QObject* parent )
                        : AbstractBootstrapper( parent ),
                          m_runningPlayCount( 0 )
{
    m_bootstrapElement = m_xmlDoc.createElement( "bootstrap" );
    m_xmlDoc.appendChild( m_bootstrapElement );
    m_bootstrapElement.setAttribute( "product", product );
    m_bootstrapElement.setAttribute( "version", XML_VERSION );

    m_savePath = CoreDir::data().filePath( product + "_bootstrap.xml" );
}


bool
AbstractFileBootstrapper::appendTrack( const Track& t )
{
    IPodScrobble track( t );
    
    m_runningPlayCount += track.playCount();
    if ( track.playCount() > k_maxPlaysPerTrack ||
        m_runningPlayCount > k_maxTotalPlays )
    {
        qDebug() << "Playcount for bootstrap exceeded maximum allowed. Track: " << track.playCount() << ", total: " << m_runningPlayCount;

        emit done( Bootstrap_Spam );
        return false;
    }

    QDomElement i = toDomElement( m_xmlDoc, track );
    m_bootstrapElement.appendChild( i );
    return true;
}


void
AbstractFileBootstrapper::zipAndSend()
{
    QFile file( m_savePath );
    file.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream stream( &file );

    stream.setCodec( "UTF-8" );
    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    stream << m_xmlDoc.toString();

    file.close();

    QString zipPath = m_savePath + ".gz";
    zipFiles( m_savePath, zipPath );

    sendZip( zipPath );
}


QDomElement
AbstractFileBootstrapper::toDomElement( QDomDocument& document, const IPodScrobble& t )
{
    QDomElement item = document.createElement( "item" );
    
    #define makeElement( tagname, getter ) \
        { \
            QDomElement e = document.createElement( tagname ); \
            e.appendChild( document.createTextNode( getter ) ); \
            item.appendChild( e ); \
        }

    makeElement( "artist", t.artist() );
    makeElement( "album", t.album() );
    makeElement( "track", t.title() );
    makeElement( "duration", QString::number( t.duration() ) );
    makeElement( "playcount", QString::number( t.playCount() ) );
    makeElement( "filename", t.url().path() );
    makeElement( "timestamp", QString::number( t.timestamp().toTime_t() ) );
    makeElement( "mbId", t.mbid() );

    return item;
}