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
#define CORE_SETTINGS_H
typedef QSettings CoreSettings;
#include "app/client/Settings.h"


class TestSettings : public QObject
{
    Q_OBJECT
    
    void reset()
    {
        qApp->setOrganizationName( "Test.fm" );
        qApp->setOrganizationDomain( "Test.fm" );
        qApp->setApplicationName( "Test.fm" );

        QSettings s;
        s.remove( "" );
        s.clear();
        s.sync();

        // prevent Assert in moose::UserSettings
        QSettings().setValue( "Username", "QtTester" );
    }

private slots:
    void initTestCase()
    {
        reset();
    }
    
    void cleanupTestCase()
    {
        reset();
    }
    
    void testScrobblePoint()
    {
        for (int x = 0; x <= 100; ++x)
        {
            reset();
            moose::MutableSettings().setScrobblePoint( x );
            QVERIFY( moose::Settings().scrobblePoint() == x );
        }
        
        reset();
        moose::MutableSettings().setScrobblePoint( 101 );
        QVERIFY( moose::Settings().scrobblePoint() == 100 );

        reset();
        moose::MutableSettings().setScrobblePoint( -1 );
        QVERIFY( moose::Settings().scrobblePoint() == 0 );
    }
    
    #define BASIC_SETTINGS_TEST( getter, setter, value ) \
        reset(); \
        moose::MutableSettings().setter( value ); \
        QVERIFY( moose::Settings().getter() == value );
    
    void testFingerprintingEnabled()
    {        
        BASIC_SETTINGS_TEST( fingerprintingEnabled, setFingerprintingEnabled, false )
        BASIC_SETTINGS_TEST( fingerprintingEnabled, setFingerprintingEnabled, true )
    }
    
    void testAlwaysConfirmIPodScrobbles()
    {
        BASIC_SETTINGS_TEST( alwaysConfirmIPodScrobbles, setAlwaysConfirmIPodScrobbles, false )
        BASIC_SETTINGS_TEST( alwaysConfirmIPodScrobbles, setAlwaysConfirmIPodScrobbles, true )
    }

    void testIPodScrobblingEnabled()
    {
        BASIC_SETTINGS_TEST( iPodScrobblingEnabled, setIPodScrobblingEnabled, false )
        BASIC_SETTINGS_TEST( iPodScrobblingEnabled, setIPodScrobblingEnabled, true )
    }
    
    void testAudioOutputDeviceName()
    {
        BASIC_SETTINGS_TEST( audioOutputDeviceName, setAudioOutputDeviceName, "external awesome boom box" )
        BASIC_SETTINGS_TEST( audioOutputDeviceName, setAudioOutputDeviceName, "internal manky thing" )
    }
    
    void testLogOutOnExit()
    {
        bool b = false;
        do { b = !b;
             reset();
             moose::MutableSettings().setLogOutOnExit( b );
             QVERIFY( unicorn::Settings().logOutOnExit() == b );
        } while (b);
    }
};
