/***************************************************************************
*   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#ifndef TAGLIB_META_INFO_H
#define TAGLIB_META_INFO_H

#include "MediaMetaInfo.h"
// taglib:
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/flacfile.h>
#include <taglib/mpcfile.h>
#include <taglib/oggfile.h>
#include <taglib/trueaudiofile.h>
#include <taglib/wavpackfile.h>


// taglib specialisation of MediaMetaInfo

class TaglibMetaInfo : public MediaMetaInfo
{
public:
    TaglibMetaInfo(const FILENAME_CHAR *filename);

    virtual QString artist();
    virtual QString album();
    virtual QString title();
    virtual unsigned duration();
    virtual unsigned kbps();

private:
    void readFile(const FILENAME_CHAR *filename);
    void readFile(TagLib::FileRef fr);

    template<typename T> bool tryGetFingerprintTags(TagLib::File *);
    void getFingerprintTags(TagLib::MPEG::File *);
    void getFingerprintTags(TagLib::Ogg::File *);
    void getFingerprintTags(TagLib::MPC::File *);
    void getFingerprintTags(TagLib::FLAC::File *);
    void getFingerprintTags(TagLib::WavPack::File *);
    void getFingerprintTags(TagLib::TrueAudio::File *);

    QString m_artist;
    QString m_album;
    QString m_title;
    unsigned m_duration;
    unsigned m_kbps;
};

#endif