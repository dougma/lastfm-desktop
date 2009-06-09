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
#include "PlayerCommandParser.h"


class TestPlayerCommandParser : public QObject
{
    Q_OBJECT

private slots:
    void testStart();
    void testStop();
    void testResume();
    void testPause();
    void testBootstrap();
    void testEmptyLine();
    void testMissingArgument();
    void testInvalidCommand();
    void testDuplicatedArgument();
    void testUnicode();
};


void
TestPlayerCommandParser::testStart()
{
    PlayerCommandParser pcp ( "START c=testapp"
                                   "&a=Test Artist"
                                   "&t=Test Title"
                                   "&b=Test Album"
                                   "&l=100"
                                   "&p=/home/tester/test.mp3" );

    QCOMPARE( pcp.command(), CommandStart );
    QCOMPARE( pcp.playerId(), QString( "testapp" ) );
    QCOMPARE( pcp.track().artist(), Artist( "Test Artist" ) );
    QCOMPARE( pcp.track().title(), QString( "Test Title" ) );
    QCOMPARE( pcp.track().album().title(), QString( "Test Album" ) );
    QCOMPARE( pcp.track().duration(), 100u );
    QCOMPARE( pcp.track().url().path(), QString( "/home/tester/test.mp3" ) );
}

void
TestPlayerCommandParser::testStop()
{
    PlayerCommandParser pcp ( "STOP c=testapp" );

    QCOMPARE( pcp.command(), CommandStop );
    QCOMPARE( pcp.playerId(), QString( "testapp" ) );
}

void
TestPlayerCommandParser::testResume()
{
    PlayerCommandParser pcp ( "RESUME c=testapp" );

    QCOMPARE( pcp.command(), CommandResume );
    QCOMPARE( pcp.playerId(), QString( "testapp" ) );
}

void
TestPlayerCommandParser::testPause()
{
    PlayerCommandParser pcp ( "PAUSE c=testapp" );

    QCOMPARE( pcp.command(), CommandPause );
    QCOMPARE( pcp.playerId(), QString( "testapp" ) );
}

void
TestPlayerCommandParser::testBootstrap()
{
    PlayerCommandParser pcp ( "BOOTSTRAP c=testapp&u=TestUser" );

    QCOMPARE( pcp.command(), CommandBootstrap );
    QCOMPARE( pcp.playerId(), QString( "testapp" ) );
    QCOMPARE( pcp.username(), QString( "TestUser" ) );
}

void
TestPlayerCommandParser::testEmptyLine()
{
    // The PlayerCommandParser should throw an exception on empty lines
    try
    {
        PlayerCommandParser pcp ( "" );
        
        QFAIL( "PlayerCommandParser did not throw an exception on an empty line." );
    }
    catch ( PlayerCommandParser::Exception e )
    {
        // Success
    }
}

void
TestPlayerCommandParser::testMissingArgument()
{
    // The PlayerCommandParser should throw an exception when arguments are missing
    try
    {
        PlayerCommandParser pcp ( "START c=testap" );
        
        QFAIL( "PlayerCommandParser did not throw an exception when arguments are missing." );
    }
    catch ( PlayerCommandParser::Exception e )
    {
        // Success
    }
}

void
TestPlayerCommandParser::testInvalidCommand()
{
    // The PlayerCommandParser should throw an exception when passed a invalid command
    try
    {
        PlayerCommandParser pcp ( "SUPERSTART c=testap" );
        
        QFAIL( "PlayerCommandParser did not throw an exception when passed a invalid command." );
    }
    catch ( PlayerCommandParser::Exception e )
    {
        // Success
    }
}

void
TestPlayerCommandParser::testDuplicatedArgument()
{
    // The PlayerCommandParser should throw an exception when arguments are duplicated
    try
    {
        PlayerCommandParser pcp ( "START c=testap&c=testapp2" );
        
        QFAIL( "PlayerCommandParser did not throw an exception when arguments are duplicated." );
    }
    catch ( PlayerCommandParser::Exception e )
    {
        // Success
    }
}

void
TestPlayerCommandParser::testUnicode()
{
    PlayerCommandParser pcp( "START c=testapp"
                                   "&a=佐橋俊彦"
                                   "&t=対峙"
                                   "&b=TV Animation ジパング original Soundtrack"
                                   "&l=123"
                                   "&p=/home/tester/15 対峙.mp3" );

    QCOMPARE( pcp.command(), CommandStart );
    QCOMPARE( pcp.playerId(), QString( "testapp" ) );
    QCOMPARE( pcp.track().artist(), Artist( "佐橋俊彦" ) );
    QCOMPARE( pcp.track().title(), QString( "対峙" ) );
    QCOMPARE( pcp.track().album().title(), QString( "TV Animation ジパング original Soundtrack" ) );
    QCOMPARE( pcp.track().duration(), 123u );
    QCOMPARE( pcp.track().url().path(), QString( "/home/tester/15 対峙.mp3" ) );
}

QTEST_APPLESS_MAIN(TestPlayerCommandParser)
#include "TestPlayerCommandParser.moc"

