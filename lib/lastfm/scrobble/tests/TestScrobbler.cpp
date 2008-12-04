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

#include "lib/lastfm/ws/WsKeys.h"
#include "../Scrobbler.cpp"
#include "../Scrobble.cpp"
#include "../ScrobbleCache.cpp"
#include "../ScrobblerHttp.cpp"
#include "../ScrobblerHandshake.cpp"
#include "../ScrobblerSubmission.cpp"
#include "../NowPlaying.cpp"
#include <QTest>

QList<MockHttp*> MockHttp::https;


class Status : public QObject 
{
    Q_OBJECT
    int s;
public slots:
    void onStatusChanged( int i ) { qDebug() << i; s = i; }
public:
    operator Scrobbler::Status() { return (Scrobbler::Status)s; }
};


class TestScrobbler: public QObject
{
    Q_OBJECT
    
    QList<Track> tracks;

public:
    TestScrobbler()
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
        {
            QFile::remove( ScrobbleCache( Ws::Username ).path() );

            ScrobbleCache cache( Ws::Username );
        
            cache.add( tracks );
            QVERIFY( cache.tracks().count() == tracks.count() );
            QVERIFY( cache.tracks() == tracks );

            cache.remove( tracks );
            QVERIFY( cache.tracks().count() == 0 );
        }
        
        
        Scrobbler s( "ass" );
        Status status;
        connect( &s, SIGNAL(status( int, QVariant )), &status, SLOT(onStatusChanged( int )) );
        
        MockHttp::https[0]->data = "OK\n"
                         "6ea5e7de120242e3b6165d3ad8a83fc8\n"
                         "http://post.audioscrobbler.com:80/np_1.2\n"
                         "http://87.117.229.205:80/protocol_1.2\n";
        MockHttp::https[0]->finish();
        qApp->processEvents();
        QVERIFY( status == Scrobbler::Handshaken );
                        
        s.cache( tracks );
        ScrobbleCache cache( Ws::Username );

        QVERIFY( cache.tracks().count() == tracks.count() );
    }
};


void
MockHttp::finish()
{
    emit requestFinished( ID, false );
}


QTEST_MAIN( TestScrobbler )
#include "TestScrobbler.moc"
