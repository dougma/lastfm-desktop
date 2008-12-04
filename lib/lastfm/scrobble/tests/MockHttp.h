/***************************************************************************
 *   Copyright 2008-2009 Last.fm Ltd.                                      *
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

#ifndef MOCK_HTTP_H
#define MOCK_HTTP_H


#include <QByteArray>
#include <QList>
#include <QObject>


class QHttpResponseHeader
{
public:
    int statusCode() const { return 200; }
};


class QHttpRequestHeader
{
public:
    QHttpRequestHeader( const char*, QString )
    {}
    int statusCode() const { return 200; }
    void setValue( QString, QString ) {}
    void setContentType( QString ) {}
};


class MockHttp : public QObject
{
    Q_OBJECT
    static const int ID = 100;

public:
    MockHttp( QObject* )
    {
        https += this;
    }

    static QList<MockHttp*> https;
    
    enum { Aborted };

    QByteArray data;
    void finish();
    QByteArray readAll() { return data; }

    int request( QHttpRequestHeader, QByteArray data ) { return ID; }

//////
    void setHost( QString, int ) {}
    int get( QString ) { return ID; }
    int post( QString, QByteArray ) { return ID; }
    bool hasPendingRequests() const { return false; }
    void abort() {}
    int error() { return 0; }

signals:
    void requestFinished( int, bool );
};


#define QHTTP_H
#define QHttp MockHttp

#endif