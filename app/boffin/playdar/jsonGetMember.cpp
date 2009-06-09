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

#include "jsonGetMember.h"

bool jsonGetMember(const QVariantMap& o, const char* key, QString& out)
{
    QVariantMap::const_iterator it = o.find(key);
    if (it != o.end() && it->type() == QVariant::String) {
        out = it->toString();
        return true;
    }
    return false;
}

bool jsonGetMember(const QVariantMap& o, const char* key, int& out)
{
    QVariantMap::const_iterator it = o.find(key);
    if (it != o.end() && it->type() == QVariant::LongLong) {
        out = it->toLongLong();
        return true;
    }
    return false;
}

bool jsonGetMember(const QVariantMap& o, const char* key, double& out)
{
    QVariantMap::const_iterator it = o.find(key);
    if (it != o.end() && it->type() == QVariant::Double) {
        out = it->toDouble();
        return true;
    }
    return false;
}

bool jsonGetMember(const QVariantMap& o, const char* key, float& out)
{
    QVariantMap::const_iterator it = o.find(key);
    if (it != o.end() && it->type() == QVariant::Double) {
        out = (float) it->toDouble();
        return true;
    }
    return false;
}
