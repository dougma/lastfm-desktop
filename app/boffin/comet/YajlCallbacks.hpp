/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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

#ifndef YAJL_CALLBACKS_HPP
#define YAJL_CALLBACKS_HPP

#include <yajl/yajl_parse.h>

// trampoline yajl callbacks to "json_" method calls on class T
// strings are converted using Policy::stringize(const char*, unsigned int len)
// we use the 'number' callback instead of the integer/double callbacks.

template <typename T, typename Policy>
class TYajlCallbacks 
    : private Policy
{
private:
    static int _null(void *ctx)
    {
        return ((T*)ctx)->json_null();
    }

    static int _boolean(void *ctx, int boolVal)
    {
        return ((T*)ctx)->json_boolean(boolVal);
    }

    static int _number(void *ctx, const char *numberVal, unsigned int numberLen)
    {
        return ((T*)ctx)->json_number(Policy::stringize(numberVal, numberLen));
    }

    static int _string(void *ctx, const unsigned char *stringVal, unsigned int stringLen)
    {
        return ((T*)ctx)->json_string(Policy::stringize((const char *)stringVal, stringLen));
    }

    static int _start_map(void *ctx)
    {
        return ((T*)ctx)->json_start_map();
    }

    static int _map_key(void *ctx, const unsigned char *key, unsigned int stringLen)
    {
        return ((T*)ctx)->json_map_key(Policy::stringize((const char *)key, stringLen));
    }

    static int _end_map(void *ctx)
    {
        return ((T*)ctx)->json_end_map();
    }

    static int _start_array(void *ctx)
    {
        return ((T*)ctx)->json_start_array();
    }

    static int _end_array(void *ctx)
    {
        return ((T*)ctx)->json_end_array();
    }

public:
    static yajl_callbacks callbacks;
    
};


template <typename T, typename Policy>
yajl_callbacks TYajlCallbacks<T, Policy>::callbacks = {
    TYajlCallbacks<T, Policy>::_null,
    TYajlCallbacks<T, Policy>::_boolean,
    0,      // integer
    0,      // double
    TYajlCallbacks<T, Policy>::_number,
    TYajlCallbacks<T, Policy>::_string,
    TYajlCallbacks<T, Policy>::_start_map,
    TYajlCallbacks<T, Policy>::_map_key,
    TYajlCallbacks<T, Policy>::_end_map,
    TYajlCallbacks<T, Policy>::_start_array,
    TYajlCallbacks<T, Policy>::_end_array
};


#endif

