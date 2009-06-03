/***************************************************************************
 *   Copyright 2009 Last.fm Ltd.                                           *
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
#ifndef BOFFIN_PLAYABLE_ITEM_H
#define BOFFIN_PLAYABLE_ITEM_H

#include <QSharedData>

struct BoffinPlayableItemData : QSharedData
{
    QString artist;
    QString album;
    QString track;
    QString source;
    QString mimetype;
    QString url;
    int duration;
    float weight;

    float workingweight;
    int artistId;
};

class BoffinPlayableItem
{
public:
    BoffinPlayableItem()
    {
        d = new BoffinPlayableItemData;
    }

    BoffinPlayableItem(
        QString artist,
        QString album,
        QString track,
        QString source,
        QString mimetype,
        QString url,
        int duration,
        float weight)
    {
        d = new BoffinPlayableItemData;
        d->artist = artist;
        d->album = album;
        d->track = track;
        d->source = source;
        d->mimetype = mimetype;
        d->url = url;
        d->duration = duration;
        d->weight = weight;
    }

    bool isValid() const { return d->url.length() > 0; }

    QString artist() const { return d->artist; }
    QString album() const { return d->album; }
    QString track() const { return d->track; }
    QString source() const { return d->source; }
    QString mimetype() const { return d->mimetype; }
    QString url() const { return d->url; }
    int duration() const { return d->duration; }
    float weight() const { return d->weight; }

    float workingweight() const { return d->workingweight; }
    int artistId() const { return d->artistId; }

    // mutable:
    float& workingweight() { return d->workingweight; }
    int& artistId() { return d->artistId; }

protected:
    QExplicitlySharedDataPointer<BoffinPlayableItemData> d;
};


#endif
