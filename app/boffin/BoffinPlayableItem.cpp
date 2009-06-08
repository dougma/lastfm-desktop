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
#include "BoffinPlayableItem.h"
#include "jsonGetMember.h"

BoffinPlayableItemData::BoffinPlayableItemData()
    :size(-1)
    ,bitrate(-1)
    ,duration(-1)
    ,weight(0)
    ,score(0)
    ,preference(0)
    ,workingweight(0)
    ,artistId(0)
{
}

BoffinPlayableItem::BoffinPlayableItem()
{
    d = new BoffinPlayableItemData;
}

//static
BoffinPlayableItem 
BoffinPlayableItem::fromTrackResolveResult(const QVariantMap& map)
{
    BoffinPlayableItem result;
    jsonGetMember(map, "album", result.d->album);
    jsonGetMember(map, "artist", result.d->artist);
    jsonGetMember(map, "bitrate", result.d->bitrate);
    jsonGetMember(map, "duration", result.d->duration);
    jsonGetMember(map, "mimetype", result.d->mimetype);
    jsonGetMember(map, "preference", result.d->preference);
    jsonGetMember(map, "score", result.d->score);
    jsonGetMember(map, "source", result.d->source);
    jsonGetMember(map, "size", result.d->size);
    jsonGetMember(map, "track", result.d->album);
    jsonGetMember(map, "url", result.d->album);
    return result;
}

//static
BoffinPlayableItem 
BoffinPlayableItem::fromBoffinRqlResult(const QVariantMap& map)
{
    BoffinPlayableItem result;
    jsonGetMember(map, "album", result.d->album);
    jsonGetMember(map, "artist", result.d->artist);
    jsonGetMember(map, "bitrate", result.d->bitrate);
    jsonGetMember(map, "duration", result.d->duration);
    jsonGetMember(map, "mimetype", result.d->mimetype);
    jsonGetMember(map, "preference", result.d->preference);
    jsonGetMember(map, "source", result.d->source);
    jsonGetMember(map, "size", result.d->size);
    jsonGetMember(map, "track", result.d->album);
    jsonGetMember(map, "url", result.d->album);
    jsonGetMember(map, "weight", result.d->weight);
    return result;
}
