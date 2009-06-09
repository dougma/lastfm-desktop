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
#include <QtTest>
#include "app/client/StopWatch.h"


class TestStopWatch : public QObject
{
    Q_OBJECT

private slots:
    void testTimeout()
    {
        ScrobblePoint scrobblepoint( 30 );
        StopWatch watch( scrobblepoint );
        
        QVERIFY( watch.m_timer->isActive() == false );
        QCOMPARE( watch.remaining(), uint(scrobblepoint * 1000) );
        QCOMPARE( watch.elapsed(), uint(0) );
        
        watch.resume();
        
        QSignalSpy spy( &watch, SIGNAL(timeout()) );
        // wait for twice timeout to check only times out once
        QTest::qWait( scrobblepoint * 2000 );
        
        QCOMPARE( spy.count(), 1 );
        QVERIFY( watch.m_timer->isActive() == false );
        QCOMPARE( watch.remaining(), uint(0) );
        QVERIFY( watch.elapsed() >= uint(scrobblepoint * 1000) );
        
        watch.resume();
        QVERIFY( watch.m_timer->isActive() == false );
        watch.pause();
        QVERIFY( watch.m_timer->isActive() == false );
    }
    
    void testPause()
    {
        ScrobblePoint scrobblepoint( 30 );
        StopWatch watch( scrobblepoint );
        watch.resume();

        QSignalSpy spy( &watch, SIGNAL(timeout()) );
        
        for (uint x = 0; x < scrobblepoint; ++x)
        {
            watch.pause();
            QTest::qWait( 1000 );
            watch.resume();
            QTest::qWait( 1000 );
        }
        
        QCOMPARE( spy.count(), 1 );
    }
};
