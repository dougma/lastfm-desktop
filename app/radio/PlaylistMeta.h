/*
   Copyright 2005-2009 Last.fm Ltd. 

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

#ifndef PLAYLIST_META_H
#define PLAYLIST_META_H

#include <QString>
#include <lastfm/XmlQuery>

// for carrying the metadata (at least, the fields we're interested in) of a playlist 
class PlaylistMeta
{
public:
    PlaylistMeta(const XmlQuery& xml);

    int id;
    QString title;
    QString description;
    int size;
    bool streamable;
    QString imgUrlSmall;
};

#endif
