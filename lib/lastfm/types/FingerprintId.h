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

#ifndef LASTFM_FINGERPRINT_ID_H
#define LASTFM_FINGERPRINT_ID_H

#include <lastfm/Track>
#include <QDebug>
#include <QList>
#include <QString>
class QNetworkReply;
 

namespace lastfm
{
    class LASTFM_TYPES_DLLEXPORT FingerprintId
    {
        int id;

    public:
        FingerprintId() : id( -1 )
        {}

        FingerprintId( uint i ) : id( i )
        {}

        bool isNull() const { return id == -1; }

        /** we query Last.fm for suggested metadata, how awesome is that? 
          * @returns null if isNull() */
        QNetworkReply* getSuggestions() const;
        static QMap<float,Track> getSuggestions( QNetworkReply* );

        /** -1 if you need to generate it */
        operator int() const { return id; }
        /** isEmpty() if you need to generate it */
        operator QString() const { return id == -1 ? "" : QString::number( id ); }
    };
}


inline QDebug operator<<( QDebug d, lastfm::FingerprintId id)
{
    if (id.isNull())
        return d << "(null)";
    else
        return d << int(id);
}

#endif
