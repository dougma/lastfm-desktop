/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole, Erik Jaelevik, 
        Christian Muehlhaeuser

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
#ifndef ITUNES_LIBRARY_H
#define ITUNES_LIBRARY_H

#include "ITunesLibraryTrack.h"
#include <lastfm/Track>
#include <QList>


/** @author Max Howell <max@last.fm> - Mac
  * @author <erik@last.fm> - Win
  *
  * This class offers easy access to the iTunes Library database
  * It uses AppleScript and COM to access and query iTunes
  */
class ITunesLibrary
{
public:
    /** the isIPod bool is for Windows only, the source, mac :( */
    ITunesLibrary( const QString& source = "", bool isIPod = false ); // throws
    ~ITunesLibrary();

    typedef ITunesLibraryTrack Track;

    bool hasTracks() const;
    Track nextTrack();
    int trackCount() const;

private:
    uint m_currentIndex;

  #ifdef WIN32
    class ITunesComWrapper* m_com;
    long m_trackCount;
    bool const m_isIPod;
  #else
    QList<Track> m_tracks;
  #endif

private:
    Q_DISABLE_COPY( ITunesLibrary );
};

#endif
