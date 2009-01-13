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

#include <QtTest/QtTest>
#include <QMainWindow>

#include "app/client/StopWatch.h"

class TestStopWatch : public QObject
{
    Q_OBJECT

    private slots:
        void initTestCase()
        {
            
        }
        
        // FIXME: make the tests run without any waiting
        
        void testSimpleTimeout(); // will take 1.1 seconds to run
        void testPause(); // will take 1.1 seconds to run
        void testResume(); // will take 1.1 seconds to run
        
        void testTicking(); // will take 3.1 seconds to run
        void testNewStart(); // will take 1.1 seconds to run
        
};

void
TestStopWatch::testSimpleTimeout()
{
    StopWatch sw;
    QSignalSpy timeoutSpy( &sw, SIGNAL( timeout() ) );
    QSignalSpy tickSpy( &sw, SIGNAL( tick( int ) ) );
    
    sw.start( 0 );
    
    QTest::qWait( 1100 );
    
    QCOMPARE( timeoutSpy.count(), 1 );
    QCOMPARE( tickSpy.count(), 1 );
    QCOMPARE( sw.elapsed(), (unsigned int) 1 );
}

void
TestStopWatch::testPause()
{
    StopWatch sw;
    QSignalSpy timeoutSpy( &sw, SIGNAL( timeout() ) );
    QSignalSpy tickSpy( &sw, SIGNAL( tick( int ) ) );
    
    sw.start( 1 );
    sw.pause();
    
    QTest::qWait( 1100 );
    
    QCOMPARE( timeoutSpy.count(), 0 );
    QCOMPARE( tickSpy.count(), 0 );
    QCOMPARE( sw.elapsed(), (unsigned int) 0 );
}

void
TestStopWatch::testResume()
{
    StopWatch sw;
    QSignalSpy timeoutSpy( &sw, SIGNAL( timeout() ) );
    QSignalSpy tickSpy( &sw, SIGNAL( tick( int ) ) );
    
    sw.start( 0 );
    sw.pause();
    sw.resume();
    
    QTest::qWait( 1100 );
    
    QCOMPARE( timeoutSpy.count(), 1 );
    QCOMPARE( tickSpy.count(), 1 );
    QCOMPARE( sw.elapsed(), (unsigned int) 1 );
}

void 
TestStopWatch::testTicking()
{
    StopWatch sw;
    QSignalSpy timeoutSpy( &sw, SIGNAL( timeout() ) );
    QSignalSpy tickSpy( &sw, SIGNAL( tick( int ) ) );
    
    sw.start( 0 );
    
    QTest::qWait( 3100 );
    
    QCOMPARE( timeoutSpy.count(), 1 );
    QCOMPARE( tickSpy.count(), 3 );
    QCOMPARE( sw.elapsed(), (unsigned int) 3 );
}

void
TestStopWatch::testNewStart()
{
    StopWatch sw;
    QSignalSpy timeoutSpy( &sw, SIGNAL( timeout() ) );
    QSignalSpy tickSpy( &sw, SIGNAL( tick( int ) ) );
    
    sw.start( 0 );
    sw.start( 0 );
    
    QTest::qWait( 1100 );
    
    QCOMPARE( timeoutSpy.count(), 1 );
    QCOMPARE( tickSpy.count(), 1 );
    QCOMPARE( sw.elapsed(), (unsigned int) 1 );
}

QTEST_MAIN(TestStopWatch)
#include "TestStopWatch.moc"

