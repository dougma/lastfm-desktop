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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/
#include "TrackSource.h"


static Track toTrack(const BoffinPlayableItem& item)
{
    Track t;
    MutableTrack mt(t);
    mt.setArtist(item.m_artist);
    mt.setAlbum(item.m_album);
    mt.setTitle(item.m_track);
    mt.setDuration(item.m_duration);
    mt.setUrl(QUrl(item.m_url));
    mt.setSource(Track::Player);
    //QString m_source;
    //QString m_mimetype;
    return t;
}

void
TrackSource::onPlayableItem(BoffinPlayableItem item)
{
    emit ready(toTrack(item));
}

