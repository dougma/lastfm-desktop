/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "LocalRql.h"
#include "lib/lastfm/types/Track.h"


LocalRqlResult::LocalRqlResult()
:m_trackSource(0)
{
}

LocalRqlResult::~LocalRqlResult()
{
    if (m_trackSource)
        m_trackSource->finished();
}

void
LocalRqlResult::parseOk(ILocalRqlTrackSource* trackSource, unsigned trackCount)
{
    m_trackSource = trackSource;
    emit parseGood(trackCount);
}

void
LocalRqlResult::parseFail(int errorLineNumber, const char *errorLine, int errorOffset)
{
    emit parseBad(errorLineNumber, QString(errorLine), errorOffset);
}

void
LocalRqlResult::getNextTrack()
{
    if (m_trackSource) {
        // the callbacks occur on random (plugin) threads, and therefore 
        // produce signals on random threads, so they are wired up here 
        // as queued connections.

        LocalRqlTrackCallback *cb = new LocalRqlTrackCallback();
        connect(cb, SIGNAL(track( Track )), SIGNAL(track( Track )), Qt::QueuedConnection );
        connect(cb, SIGNAL(endOfTracks()), SIGNAL(endOfTracks()), Qt::QueuedConnection );
        m_trackSource->getNextTrack(cb);
    }
}


////////////////////////////////////////////

void
LocalRqlTrackCallback::trackOk(const char* title, const char* album, const char* artist, const char* url, unsigned duration)
{
    MutableTrack mt;
    mt.setTitle( QString::fromUtf8( title ) );
    mt.setAlbum( QString::fromUtf8( album ) );
    mt.setArtist( QString::fromUtf8( artist ) );
    mt.setUrl( QString::fromUtf8( url ) );
    mt.setDuration( duration );
    emit track( mt );
    delete this;
}

void 
LocalRqlTrackCallback::trackFail()
{
    emit endOfTracks();
    delete this;
}



////////////////////////////////////////////


LocalRql::LocalRql(const QList<ILocalRqlPlugin*>& plugins) : m_plugin( 0 )
{
    //TODO: make a list of plugins make sense
    
    if (!plugins.size()) return;

    m_plugin = plugins[0];
    m_plugin->init();
}

LocalRql::~LocalRql()
{
    if (m_plugin)
        m_plugin->finished();
}

bool
LocalRql::isAvailable()
{
    return m_plugin;
}

LocalRqlResult*
LocalRql::startParse(QString rql)
{
    LocalRqlResult* result = 0;
    if (m_plugin) {
        result = new LocalRqlResult();
        m_plugin->parse(rql.toUtf8(), result);
    }
    return result;
}

void 
LocalRql::testTag(QString url)
{
    if (m_plugin) {
        m_plugin->testTag(url.toUtf8());
    }
}