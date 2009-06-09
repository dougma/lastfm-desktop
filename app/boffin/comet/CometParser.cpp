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
#include "CometParser.h"

CometParser::CometParser(QObject *parent)
: QObject(parent)
{
    yajl_parser_config cfg = { 1 /* allow comments */, 0 /* don't check the incoming utf8 */ };
    m_handle = yajl_alloc(&CometCallbacks::callbacks, &cfg, NULL, (void *) this);
}

CometParser::~CometParser()
{
    if (m_handle)
        yajl_free(m_handle);
}

bool
CometParser::push(const QByteArray& ba)
{
    const yajl_status status = yajl_parse(m_handle, (const unsigned char*) ba.constData(), ba.length());
    return status == yajl_status_ok || status == yajl_status_insufficient_data;
}


//static
void
CometParser::objectInserter(CometParser::Object* o, const QString& name, const QVariant& value)
{
    o->insert(name, value);
}

//static
void
CometParser::arrayInserter(CometParser::Array* a, const QString&, const QVariant& value)
{
    a->push_back(value);
}

//static
void
CometParser::postObjectInserter(CometParser::Inserter i, const QString& name, Object* o)
{
    i(name, *o);
    delete o;
}

//static
void
CometParser::postArrayInserter(CometParser::Inserter i, const QString& name, Array* a)
{
    i(name, *a);
    delete a;
}

void
CometParser::haveObject(const QString&, const QVariant& v)
{
    if (v.type() == QVariant::Map) {
        emit haveObject(v.toMap());
    }
}

//static
void
CometParser::nop()
{
}

////////////////////////////////

// yajl callbacks:
int
CometParser::json_null()
{
    m_insertStack.top()( m_key, QVariant() );
    return 1;
}

int
CometParser::json_boolean(int boolVal)
{
    m_insertStack.top()( m_key, QVariant(boolVal ? true : false) );
    return 1;
}

int
CometParser::json_number(const QString& s)
{
    bool ok;
    qlonglong l;
    double d;

    if ((l = s.toLongLong(&ok), ok)) {
        m_insertStack.top()( m_key, QVariant(l) );
    } else if ((d = s.toDouble(&ok), ok)) {
        m_insertStack.top()( m_key, QVariant(d) );
    }
    return 1;
}

int
CometParser::json_string(const QString& s)
{
    m_insertStack.top()( m_key, QVariant(s) );
    return 1;
}

int
CometParser::json_start_map()
{
    if (m_insertStack.size() == 0)
        return 0;       // the comet stream we expect has objects wrapped in an array, thx.

    Object *o = new Object();
    m_atEndStack.push( boost::bind(&CometParser::postObjectInserter, m_insertStack.top(), m_key, o) );
    m_insertStack.push( boost::bind(&CometParser::objectInserter, o, _1, _2) );
    return 1;
}

int
CometParser::json_map_key(const QString& s)
{
    m_key = s;
    return 1;
}

int
CometParser::json_start_array()
{
    Array *a = new Array();
    if (m_insertStack.size() == 0) {
        // the first enclosing array...
        // has a special inserter to call the function to emit the haveObject signal
        m_atEndStack.push( boost::bind(&CometParser::nop) );
        m_insertStack.push( boost::bind(&CometParser::haveObject, this, _1, _2) );
    } else {
        m_atEndStack.push( boost::bind(&CometParser::postArrayInserter, m_insertStack.top(), m_key, a) );
        m_insertStack.push( boost::bind(&CometParser::arrayInserter, a, _1, _2) );
    }
    return 1;
}

int
CometParser::json_end_map()
{
    m_insertStack.pop();
    m_atEndStack.pop()();
    return 1;
}

int
CometParser::json_end_array()
{
    m_insertStack.pop();
    m_atEndStack.pop()();
    return 1;
}

