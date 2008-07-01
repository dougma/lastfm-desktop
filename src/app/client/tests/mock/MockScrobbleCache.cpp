/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "ScrobbleCache.h"
#include "version.h"
#include "lib/moose/MooseCommon.h"
#include <QFile>
#include <QDomElement>
#include <QDomDocument>


//FIXME! Doesn't suit multi user feel of this class as it is currently
static QList<TrackInfo> g_tracks;


ScrobbleCache::ScrobbleCache( const QString& username ) : m_tracks( g_tracks )
{
    Q_ASSERT( username.length() );

    m_path = "";//Moose::savePath( username + "_submissions.xml" );
    m_username = username;

    //HACK due to bad design with this m_tracks global instance thing
    if (m_tracks.isEmpty()) read();
}


void
ScrobbleCache::read()
{
    /*m_tracks.clear();

    QFile file( m_path );
    file.open( QFile::Text | QFile::ReadOnly );
    QTextStream stream( &file );
    stream.setCodec( "UTF-8" );

    QDomDocument xml;
    xml.setContent( stream.readAll() );

    for (QDomNode n = xml.documentElement().firstChild(); !n.isNull(); n = n.nextSibling())
        if (n.nodeName() == "item")
            m_tracks += TrackInfo( n.toElement() );*/
}


void
ScrobbleCache::write()
{
    /*if (m_tracks.isEmpty())
    {
        QFile::remove( m_path );
    }
    else {
        QDomDocument xml;
        QDomElement e = xml.createElement( "submissions" );
        e.setAttribute( "product", PRODUCT_NAME );
        e.setAttribute( "version", "1.2" );

        foreach (TrackInfo i, m_tracks)
            e.appendChild( i.toDomElement( xml ) );

        xml.appendChild( e );

        QFile file( m_path );
        file.open( QIODevice::WriteOnly | QIODevice::Text );

        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        stream << "<?xml version='1.0' encoding='utf-8'?>\n";
        stream << xml.toString( 2 );
    }*/
}


void
ScrobbleCache::add( const TrackInfo& track )
{
    add( QList<TrackInfo>() << track );
}


void
ScrobbleCache::add( const QList<TrackInfo>& tracks )
{
    foreach (const TrackInfo& track, tracks)
    {
        if (track.isEmpty()) 
            qDebug() << "Will not cache an empty track";
        else
            m_tracks += track;
    }
    //write();
}


int
ScrobbleCache::remove( const QList<TrackInfo>& toremove )
{
    QMutableListIterator<TrackInfo> i( m_tracks );
    while (i.hasNext()) {
        TrackInfo t = i.next();
        for (int x = 0; x < toremove.count(); ++x)
            if (toremove[x] == t)
                i.remove();
    }

    //write();

    // yes we return # remaining, rather # removed, but this is an internal 
    // function and the behaviour is documented so it's alright imo --mxcl
    return m_tracks.count();
}
