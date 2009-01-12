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

#ifndef LASTFM_PUBLIC_H
#define LASTFM_PUBLIC_H

/** Exports symbols when compiled as part of the lib
  * Imports when included from some other target */
#if defined(_WIN32) || defined(WIN32)
    #ifdef _RADIO_DLLEXPORT
        #define LASTFM_RADIO_DLLEXPORT __declspec(dllexport)
    #else
        #define LASTFM_RADIO_DLLEXPORT __declspec(dllimport)
    #endif

    #ifdef _FINGERPRINT_DLLEXPORT
        #define LASTFM_FINGERPRINT_DLLEXPORT __declspec(dllexport)
    #else
        #define LASTFM_FINGERPRINT_DLLEXPORT __declspec(dllimport)
    #endif

    #ifdef _WS_DLLEXPORT
        #define LASTFM_WS_DLLEXPORT __declspec(dllexport)
    #else
        #define LASTFM_WS_DLLEXPORT __declspec(dllimport)
    #endif

    #ifdef _TYPES_DLLEXPORT
        #define LASTFM_TYPES_DLLEXPORT __declspec(dllexport)
    #else
        #define LASTFM_TYPES_DLLEXPORT __declspec(dllimport)
    #endif
    
    #ifdef _CORE_DLLEXPORT
        #define LASTFM_CORE_DLLEXPORT __declspec(dllexport)
    #else
        #define LASTFM_CORE_DLLEXPORT __declspec(dllimport)
    #endif

    #ifdef _SCROBBLE_DLLEXPORT
        #define LASTFM_SCROBBLE_DLLEXPORT __declspec(dllexport)
    #else
        #define LASTFM_SCROBBLE_DLLEXPORT __declspec(dllimport)
    #endif


#else
    #define LASTFM_RADIO_DLLEXPORT
    #define LASTFM_FINGERPRINT_DLLEXPORT
    #define LASTFM_WS_DLLEXPORT
    #define LASTFM_CORE_DLLEXPORT
    #define LASTFM_TYPES_DLLEXPORT
    #define LASTFM_SCROBBLE_DLLEXPORT
#endif



#include <QMetaEnum>
#include <QLatin1String>

namespace lastfm
{
  /** http://labs.trolltech.com/blogs/2008/10/09/coding-tip-pretty-printing-enum-values
    * Tips for making this take a single parameter welcome! :)
    */
  template <typename T> static inline QString qMetaEnumString( int enum_value, const char* enum_name )
  {
      QMetaObject meta = T::staticMetaObject;
      for (int i=0; i < meta.enumeratorCount(); ++i)
      {
          QMetaEnum m = meta.enumerator(i);
          if (m.name() == QLatin1String(enum_name))
              return QLatin1String(m.valueToKey(enum_value));
      }
      return "Unknown enum value: " + QString::number( enum_value );
  }
}

#endif //LASTFM_PUBLIC_H
