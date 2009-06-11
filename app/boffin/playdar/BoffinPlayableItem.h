/*
   Copyright 2009 Last.fm Ltd. 
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
#ifndef BOFFIN_PLAYABLE_ITEM_H
#define BOFFIN_PLAYABLE_ITEM_H

#include <QSharedData>
#include <QVariant>
#include <QString>

struct BoffinPlayableItemData : QSharedData
{
    BoffinPlayableItemData();

    QString artist;
    QString album;
    QString track;
    QString source;
    QString mimetype;
    QString url;
    int size;           // file size in bytes
    int bitrate;        // nominal kb/sec of encoding
    int duration;       // track length in seconds
    float weight;       // boffin rql items have a weight
    float score;        // resolved items have a score
    int preference;     // plugin preference weighting (percentage?)

    float workingweight;
    int artistId;
};

class BoffinPlayableItem
{
public:
    BoffinPlayableItem();

    // we set the bar low; if it has a url we can try to play it:
    bool isValid() const { return d->url.length() > 0; }

    QString artist() const { return d->artist; }
    QString album() const { return d->album; }
    QString track() const { return d->track; }
    QString source() const { return d->source; }
    QString mimetype() const { return d->mimetype; }
    QString url() const { return d->url; }
    int size() const { return d->size; }
    int bitrate() const { return d->bitrate; }
    int duration() const { return d->duration; }
    float weight() const { return d->weight; }
    float score() const { return d->score; }
    int preference() const { return d->preference; }

    float workingweight() const { return d->workingweight; }
    int artistId() const { return d->artistId; }

    // mutable:
    float& workingweight() { return d->workingweight; }
    int& artistId() { return d->artistId; }

    static BoffinPlayableItem fromTrackResolveResult(const QVariantMap& map);
    static BoffinPlayableItem fromBoffinRqlResult(const QVariantMap& map);

protected:
    QExplicitlySharedDataPointer<BoffinPlayableItemData> d;
};


#endif
