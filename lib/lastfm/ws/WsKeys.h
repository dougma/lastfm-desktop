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

#ifndef LASTFM_WS_KEYS_H
#define LASTFM_WS_KEYS_H

#include <lastfm/public.h>
#include <QString>


/** you need to assign these in your application, they start off 
  * assigned as null strings, if you are using UnicornApplication, don't
  * worry about it. All the strings should be utf8. 
  *
  * Some are const char*, some QString. The rationale, is performance and 
  * convenience respectively.
  */
namespace Ws
{
    /** both of these are provided when you register at http://last.fm/api */
    LASTFM_WS_DLLEXPORT extern const char* SharedSecret;
    LASTFM_WS_DLLEXPORT extern const char* ApiKey;

	/** optional, if you don't assign this, we create one for you, this is so
	  * we can create pretty logs with your app's usage information */
	LASTFM_WS_DLLEXPORT extern const char* UserAgent;

    /** You will need to assign this before using any webservices */
	LASTFM_WS_DLLEXPORT extern QString Username;

    /** Some webservices require authentication. See the following
      * documentation:
      * http://www.last.fm/api/authentication
      * http://www.last.fm/api/desktopauth
      * You have to authenticate and then assign to SessionKey, liblastfm does
      * not do that for you.
      */
    LASTFM_WS_DLLEXPORT extern QString SessionKey;
}


#define LASTFM_WS_HOSTNAME "ws.audioscrobbler.com"


#endif
