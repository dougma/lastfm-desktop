/***************************************************************************
 *   Copyright 2007-2008 Last.fm Ltd.                                      *
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
 
#define QHTTP_H
#define QHttp MockHttp
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
QList<class MockHttp*> https;
class MockHttp : public QObject
{
    Q_OBJECT
    static const int ID = 100;

public:
    MockHttp( QObject* )
    {
        https += this;
    }
    
    enum { Aborted };

    QByteArray data;
    void finish() { emit requestFinished( ID, false ); }
    QByteArray readAll() { return data; }

    int request( QHttpRequestHeader, QByteArray ) { return ID; }

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

#include "lib/lastfm/ws/WsKeys.h"
#include "../Scrobbler.cpp"
#include "../Scrobble.cpp"
#include "../ScrobbleCache.cpp"
#include "../ScrobblerHttp.cpp"
#include "../ScrobblerHandshake.cpp"
#include "../ScrobblerSubmission.cpp"
#include "../NowPlaying.cpp"
#include <QTest>

class Status : public QObject
{
    Q_OBJECT
    
    int s;

public slots:
    void onStatusChanged( int i )
    {
        qDebug() << i;
        s = i;
    }
    
public:
    operator Scrobbler::Status() { return (Scrobbler::Status)s; }
};


class ScrobblerTest: public QObject
{
    Q_OBJECT
    
    QList<Track> tracks;

public:
    ScrobblerTest()
    {
        Ws::Username     = "4girls1cup";
        Ws::UserAgent    = "MooseTestFramework";
        Ws::ApiKey       = "4f416bb10aaf87d2293029f4f52fdd96";
        Ws::SharedSecret = "4734bf8d5475953f1c5c9a99b5caf827";

        MutableTrack t;
        t.setArtist( "Testy McTesterson" );
        t.setTitle( "Fire in the Test Station" );
        tracks += t;
        t.setArtist( "Cathy Testlocks" );
        t.setTitle( "Do I pass the love test?" );
        tracks += t;
    }

private slots:
    void test()
    {
        Scrobbler s( "ass" );
        Status status;
        connect( &s, SIGNAL(status( int, QVariant )), &status, SLOT(onStatusChanged( int )) );
        
        https[0]->data = "OK\n"
                         "6ea5e7de120242e3b6165d3ad8a83fc8\n"
                         "http://post.audioscrobbler.com:80/np_1.2\n"
                         "http://87.117.229.205:80/protocol_1.2\n";
        https[0]->finish();
        
        qApp->processEvents();

        QVERIFY( status == Scrobbler::Handshaken );
    }
    
};

QTEST_MAIN( ScrobblerTest )
#include "Scrobbler.test.moc"
