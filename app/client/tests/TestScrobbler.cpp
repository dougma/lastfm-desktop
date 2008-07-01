/***************************************************************************
 *   Copyright (C) 2005 - 2008 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
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

#include <QtTest/QtTest>

#include "mock/PrivateMockScrobblerHttp.h"

#include "scrobbler/Scrobbler.h"

#include "lib/moose/TrackInfo.h"
#include "lib/unicorn/UnicornCommon.h"

QByteArray ok_handshake_response = "OK\n"
                                   "6ea5e7de120242e3b6165d3ad8a83fc8\n"
                                   "http://post.audioscrobbler.com:80/np_1.2\n"
                                   "http://87.117.229.205:80/protocol_1.2\n";

class TestScrobbler : public QObject
{
    Q_OBJECT

    private slots:
        void initTestCase()
        {
            
        }
        
        void testHandshake();
        void testNowPlaying();
        void testSubmitTrack();

};

void
TestScrobbler::testHandshake()
{
    PrivateMockScrobblerHttp privateMock;
    privateMock.setNextResponse( ok_handshake_response );
    Scrobbler scrobbler( "mctester", Unicorn::md5( "testerpassword" ) );
    
    QApplication::processEvents(); // send handshake
    QApplication::processEvents(); // process successful handshake emits
    
    QCOMPARE( true,  privateMock.handshake() );
    QCOMPARE( privateMock.protocolVersion(), QString( "1.2" ) );
    QCOMPARE( privateMock.clientName(), QString( "ass" ) );
    QCOMPARE( privateMock.clientVersion(), QString( "2.0.0" ) );
    QCOMPARE( privateMock.username(), QString( "mctester" ) );
    
    QString token = Unicorn::md5(
                                 ( 
                                   Unicorn::md5( "testerpassword" ) + 
                                   QString::number( privateMock.timestamp() ) 
                                  ).toLatin1() 
                                 );

    QCOMPARE( privateMock.authToken(), token );
}

void
TestScrobbler::testNowPlaying()
{
    PrivateMockScrobblerHttp privateMock;
    
    privateMock.setNextResponse( ok_handshake_response ); // set handshake response
    Scrobbler scrobbler( "mctester", Unicorn::md5( "testerpassword" ) );
    
    QApplication::processEvents(); // send handshake
    privateMock.setNextResponse( QByteArray( "OK" ) ); // Reset response to ok
    QApplication::processEvents(); // process successful handshake emits
    
    MutableTrackInfo track;
    track.setArtist( "Test Artist" );
    track.setTrack( "The Tester Song" );
    track.setAlbum( "The Most Tested Album Ever" );
    track.setDuration( 1234 );

    scrobbler.nowPlaying( track );

    QTest::qWait( 6000 );
    
    QApplication::processEvents(); // send np
    
    QCOMPARE( privateMock.artist(), QString( "Test Artist" ) );
    QCOMPARE( privateMock.track(), QString( "The Tester Song" ) );
    QCOMPARE( privateMock.album(), QString( "The Most Tested Album Ever" ) );
    QCOMPARE( privateMock.duration(), 1234 );
}

void
TestScrobbler::testSubmitTrack()
{
    PrivateMockScrobblerHttp privateMock;
    
    privateMock.setNextResponse( ok_handshake_response ); // set handshake response
    Scrobbler scrobbler( "mctester", Unicorn::md5( "testerpassword" ) );
    
    QApplication::processEvents(); // send handshake
    privateMock.setNextResponse( QByteArray( "OK" ) ); // Reset response to ok
    QApplication::processEvents(); // process successful handshake emits
    
    MutableTrackInfo track;
    track.setArtist( "Test Artist" );
    track.setTrack( "The Tester Song" );
    track.setAlbum( "The Most Tested Album Ever" );
    track.setDuration( 1234 );

    scrobbler.cache( track );
    
    QApplication::processEvents(); // cache it
    
    scrobbler.submit( );
    
    QApplication::processEvents(); // submit it
    
    QCOMPARE( privateMock.artist(), QString( "Test Artist" ) );
    QCOMPARE( privateMock.track(), QString( "The Tester Song" ) );
    QCOMPARE( privateMock.album(), QString( "The Most Tested Album Ever" ) );
    QCOMPARE( privateMock.duration(), 1234 );
}

QTEST_MAIN(TestScrobbler)
#include "TestScrobbler.moc"

