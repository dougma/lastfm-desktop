/*
   Copyright 2005-2009 Last.fm Ltd. 
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
#include <QList>

namespace Qt
{
    template <class T> static inline QList<T> reverse( QList<T> list )
    {
        const int N = list.size();
        const int n = N/2;
        for (int x = 0; x < n; ++x)
            list.swap( x, N-1-x );
        return list;
    }
    
    //"specialise" for QStringList
    static inline QStringList reverse( QList<QString> strings )
    {
        return (QStringList)reverse<QString>( strings );
    }
}
