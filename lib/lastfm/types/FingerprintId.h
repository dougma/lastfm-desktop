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

#ifndef LASTFM_FINGERPRINT_ID_H
#define LASTFM_FINGERPRINT_ID_H

#include <lastfm/DllExportMacro.h>
#include <QDebug>
#include <QString>


class LASTFM_TYPES_DLLEXPORT FingerprintId
{
    int id;

public:
    FingerprintId() : id( -1 )
    {}

    FingerprintId( uint i ) : id( i )
    {}

    bool isNull() const { return id == -1; }

    /** -1 if you need to generate it */
    operator int() { return id; }
    /** isEmpty() if you need to generate it */
    operator QString() { return id == -1 ? "" : QString::number( id ); }
};


inline QDebug operator<<( QDebug d, FingerprintId id)
{
    return d << int(id);
}

#endif
