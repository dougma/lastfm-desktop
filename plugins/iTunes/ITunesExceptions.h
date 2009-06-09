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
#ifndef ITUNES_EXCEPTIONS_H
#define ITUNES_EXCEPTIONS_H

#include <stdexcept>

class ITunesException : public std::runtime_error
{
public:
    ITunesException( const char* s = "ITunesException" ) : std::runtime_error( s ) { }
};


class PlayCountException : public ITunesException
{
public:
    PlayCountException() : ITunesException( "PlayCountException" ) { }
};

#endif // ITUNESEXCEPTIONS_H
