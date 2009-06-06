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
#include "Shuffler.h"

static Track toTrack(const BoffinPlayableItem& item)
{
    Track t;
    MutableTrack mt(t);
    mt.setArtist(item.artist());
    mt.setAlbum(item.album());
    mt.setTitle(item.track());
    mt.setDuration(item.duration());
    mt.setUrl(QUrl(item.url()));
    mt.setSource(Track::Player);
    return t;
}

TrackSource::TrackSource(Shuffler* shuffler, QObject *parent)
: QObject(parent)
, m_shuffler(shuffler)
, m_maxSize(1)
{
}

Track
TrackSource::takeNextTrack()
{
    fillBuffer();
    if (m_buffer.isEmpty()) 
        return Track();

    Track result = toTrack(m_buffer.takeFirst());
    fillBuffer();
    emit changed();
    return result;
}

void
TrackSource::fillBuffer()
{
    while (m_buffer.size() < m_maxSize) {
        BoffinPlayableItem item = m_shuffler->sampleOne();
        if (item.isValid())
            m_buffer.append(item);
        else
            break;
    }
}

BoffinPlayableItem
TrackSource::peek(unsigned index)
{
    return (index >= 0 && index < (unsigned) m_buffer.size()) ? m_buffer[index] : BoffinPlayableItem();
}

int
TrackSource::size()
{
    return m_buffer.size();
}

void
TrackSource::setSize(unsigned maxSize)
{
    m_maxSize = maxSize;
}

void
TrackSource::clear()
{
    if (!m_buffer.isEmpty()) {
        m_buffer.clear();
        emit changed();
    }
}
