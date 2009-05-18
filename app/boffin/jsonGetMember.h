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
 
#ifndef JSON_GET_MEMBER_H
#define JSON_GET_MEMBER_H

#include <boost/foreach.hpp>
#include "json_spirit/json_spirit.h"


// gets the named property from the json value
//
// returns true if it's got ok.
//
// todo: support nested objects,    eg: "playable.result.id"
// todo: support arrays,            eg: "results[1].id"
//
// ( T can be any type acceptable to json_spirit's get_value method )
//
template <typename T>
bool
jsonGetMember(const json_spirit::Value& value, const char* name, T& out)
{
    using namespace json_spirit;

    if (value.type() == obj_type) {
        // yeah, only objects have values.
        BOOST_FOREACH(const Pair& pair, value.get_obj()) {
            if (pair.name_ == name) {
                out = pair.value_.get_value<T>();
                return true;
            }
        }
    }
    return false;
}

#include <QVariant>
bool jsonGetMember(const QVariantMap& o, const char* key, QString& out);
bool jsonGetMember(const QVariantMap& o, const char* key, int& out);
bool jsonGetMember(const QVariantMap& o, const char* key, double& out);


#endif
