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

#ifndef LASTFM_XSPF_H
#define LASTFM_XSPF_H

#include <lastfm/public.h>
#include <lastfm/types/Track.h>
#include <QList>


class LASTFM_TYPES_DLLEXPORT Xspf
{
public:
    /** pass in the playlist node! */
	Xspf( const class QDomElement& playlist_node, Track::Source src = Track::LastFmRadio );

	QList<Track> tracks() const { return m_tracks; }
	QString title() const{ return m_title; }

private:
	QList<Track> m_tracks;
	QString m_title;
};

#endif
