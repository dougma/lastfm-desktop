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

#ifndef LASTFM_SCROBBLE_CACHE_H
#define LASTFM_SCROBBLE_CACHE_H

#include <DllExportMacro.h>
#include "Scrobble.h"
#include <QList>
#include <QString>


/** absolutely not thread-safe */
class LASTFM_SCROBBLE_DLLEXPORT ScrobbleCache
{
    QString m_username;

    void write(); /// writes m_tracks to m_path

    friend class ScrobblerSubmission;

protected:
    ScrobbleCache()
    {}

    QString m_path;
    QList<Track> m_tracks;
    
    void read( QDomDocument& xml );  /// reads from m_path into m_tracks   
    
public:
    explicit ScrobbleCache( const QString& username );

    /** note this is unique for Track::sameAs() and equal timestamps 
      * obviously playcounts will not be increased for the same timestamp */
    void add( const Scrobble& );
    void add( const QList<Track>& );

    /** returns the number of tracks left in the queue */
    int remove( const QList<Track>& );

    QList<Track> tracks() const { return m_tracks; }
    QString path() const { return m_path; }
    QString username() const { return m_username; }

private:
    bool operator==( const ScrobbleCache& ); //undefined
};

#endif
