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

#include "TaglibMetaInfo.h"
#include "tag.h"


// a factory method (currently we're using taglib to get out metaInfo)
MediaMetaInfo *
MediaMetaInfo::create(const QString &filename)
{
#ifdef WIN32
//    return new TaglibMetaInfo(filename.utf16());
    return new TaglibMetaInfo(filename.toUtf8().data());
#else
    return new TaglibMetaInfo(filename.toUtf8().data());
#endif
}

//////////////////////////////////////////////////////////////


TaglibMetaInfo::TaglibMetaInfo(const FILENAME_CHAR *filename)
:m_duration(0)
,m_kbps(0)
{
    readFile(filename);
}

void
TaglibMetaInfo::readFile(const FILENAME_CHAR *filename)
{
    readFile(TagLib::FileRef(filename));
}

void
TaglibMetaInfo::readFile(TagLib::FileRef fr)
{
    TagLib::Tag *tag = fr.tag();
	if (tag) {
        // to8Bit will convert to utf8
        m_artist = QString::fromUtf8(tag->artist().to8Bit(true).data());
		m_album = QString::fromUtf8(tag->album().to8Bit(true).data());
		m_title = QString::fromUtf8(tag->title().to8Bit(true).data());
	}

    TagLib::AudioProperties *audio = fr.audioProperties();
	if (audio) {
		m_duration = (unsigned) audio->length();
		m_kbps = audio->bitrate();		// yep, it's in kbps from TagLib
	}

	// fingerprints:
	// the generic Tag class is not good enough to get these tags
	// so we try a series of upcasts to obtain the actual File class.
    TagLib::File *f = fr.file();
	if (f) {
		tryGetFingerprintTags<TagLib::MPEG::File>(f) ||
		tryGetFingerprintTags<TagLib::Ogg::File>(f) ||
		tryGetFingerprintTags<TagLib::MPC::File>(f) ||
        tryGetFingerprintTags<TagLib::FLAC::File>(f) ||
		tryGetFingerprintTags<TagLib::WavPack::File>(f) ||
		tryGetFingerprintTags<TagLib::TrueAudio::File>(f);
	}
}

template<typename T>
bool 
TaglibMetaInfo::tryGetFingerprintTags(TagLib::File *f)
{
	T* p = dynamic_cast<T*>(f);
	if (p) {
		getFingerprintTags(p);
		return true;
	}
	return false;
}

void
TaglibMetaInfo::getFingerprintTags(TagLib::MPEG::File *f)
{
    f;    //todo
}

void
TaglibMetaInfo::getFingerprintTags(TagLib::Ogg::File *f)
{
    f;    //todo
}

void
TaglibMetaInfo::getFingerprintTags(TagLib::MPC::File *f)
{
    f;    //todo
}

void
TaglibMetaInfo::getFingerprintTags(TagLib::FLAC::File *f)
{
    f;    //todo
}

void
TaglibMetaInfo::getFingerprintTags(TagLib::WavPack::File *f)
{
    f;    //todo
}

void
TaglibMetaInfo::getFingerprintTags(TagLib::TrueAudio::File *f)
{
    f;    //todo
}

QString
TaglibMetaInfo::artist()
{
    return m_artist;
}

QString
TaglibMetaInfo::album()
{
    return m_album;
}

QString
TaglibMetaInfo::title()
{
    return m_title;
}

unsigned 
TaglibMetaInfo::duration()
{
    return m_duration;
}

unsigned 
TaglibMetaInfo::kbps()
{
    return m_kbps;
}

