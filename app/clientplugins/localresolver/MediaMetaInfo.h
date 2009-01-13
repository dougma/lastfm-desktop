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

#ifndef MEDIA_META_INFO_H
#define MEDIA_META_INFO_H

#include <QString>

#ifdef WIN32
//#define FILENAME_CHAR unsigned short
#define FILENAME_CHAR char
#else
#define FILENAME_CHAR char
#endif

// interface to the information about the media file
class MediaMetaInfo
{
public:
    static class MediaMetaInfo *create(const QString &filename);

    virtual ~MediaMetaInfo() {};
    virtual QString artist() = 0;
    virtual QString album() = 0;
    virtual QString title() = 0;
    virtual unsigned duration() = 0;        // in seconds
    virtual unsigned kbps() = 0;            // nominal bitrate
};

#endif