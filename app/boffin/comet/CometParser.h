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
#ifndef COMET_PARSER_H
#define COMET_PARSER_H

#include <QMap>
#include <QList>
#include <QStack>
#include <QVariant>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "YajlCallbacks.hpp"

// parses a neverending json comet stream like:
//      "[ {}, {}, ...."
//
// signal emitted for each top-level object
//
// yajl does the json (developed using yajl 1.0.5)
// this class transforms the tokens into QVariantMap types
//
class CometParser : public QObject
{
    Q_OBJECT;
    Q_DISABLE_COPY(CometParser);

    struct QStringPolicy
    {
        static QString stringize(const char* s, unsigned int len)
        {
            return QString::fromUtf8(s, len);
        }
    };

    typedef TYajlCallbacks<CometParser, QStringPolicy> CometCallbacks;
    typedef QVariantMap Object;
    typedef QVariantList Array;
    typedef boost::function<void(const QString&, const QVariant&)> Inserter;
    typedef boost::function<void()> AtEnd;

    friend class TYajlCallbacks<CometParser, QStringPolicy>;

public:
    CometParser(QObject *parent = 0);
    ~CometParser();

    bool push(const QByteArray& ba);        // push data in...

signals:
    void haveObject(QVariantMap o);         // ...and objects pop out

private:

    // inserters and post-inserters

    static void objectInserter(Object* o, const QString& name, const QVariant& value);
    static void arrayInserter(Array* a, const QString&, const QVariant& value);
    static void postObjectInserter(Inserter i, const QString& name, Object* o);
    static void postArrayInserter(Inserter i, const QString& name, Array* a);
    void haveObject(const QString&, const QVariant& v);
    static void nop();

    ////////////////////////////////
    // yajl callbacks

    int json_null();
    int json_boolean(int boolVal);
    int json_number(const QString& s);
    int json_string(const QString& s);
    int json_start_map();
    int json_map_key(const QString& s);
    int json_start_array();
    int json_end_map();
    int json_end_array();

    /////////////////

    QString m_key;                      // the last key we saw
    QStack<Inserter> m_insertStack;     // inserters are used to put values into objects and arrays
    QStack<AtEnd> m_atEndStack;         // we call these at the end of an object or array

    yajl_handle m_handle;
};


#endif

