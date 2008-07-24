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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef UNICORN_ALBUM_H
#define UNICORN_ALBUM_H

#include "Artist.h"
#include "Mbid.h"
#include "lib/DllExportMacro.h"
#include <QString>
class WsReply;


class TYPES_DLLEXPORT Album
{
    Mbid m_mbid;
    Artist m_artist;
    QString m_title;

public:
    explicit Album( Mbid mbid ) : m_mbid( mbid )
    {}

    Album( Artist artist, QString title ) : m_artist( artist ), m_title( title )
    {}

    operator QString() const { return m_title; }
    QString title() const { return m_title; }
    Artist artist() const { return m_artist; }
    Mbid mbid() const { return m_mbid; }

    /** Album.getInfo WebService */
    WsReply* getInfo() const;

    /** downloads image, use QPixmap::loadFromData FIXME not synchronously! */
    QByteArray image();

    WsReply* share( const class User& recipient, const QString& message = "" );
};

#endif
