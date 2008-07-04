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
#include <QMainWindow>

#include "app/client/Settings.h"

class TestSettings : public QObject
{
    Q_OBJECT

    private slots:
        void initTestCase()
        {
            qApp->setOrganizationName( "Last.fm.test" );
            qApp->setOrganizationDomain( "Last.fm.test" );
            qApp->setApplicationName( "Last.fm.test" );
            
            The::settings();
            
            QSettings().clear();
        }
        
        void cleanupTestCase()
        {
            QSettings().clear();
        }
        
        void testLogOutOnExit();
        void testControlPort();
        void testScrobblePoint();
        void testContainerWindowState();
        void testContainerGeometry();
};


void
TestSettings::testLogOutOnExit()
{
    MutableSettings mSettings( The::settings() );
    mSettings.setLogOutOnExit( true );
    QCOMPARE( The::settings().logOutOnExit(), true );
    
    mSettings.setLogOutOnExit( false );
    QCOMPARE( The::settings().logOutOnExit(), false );
}

// the following three methods cannot be tested properly, as 
// the setting is done in client/Settings, and the getting is
// done in moose/Settings, and they point to different Settings-
// paths. it is not desirable to do testing on the same path as
// the release build, but currently you cannot change the path
// of moose/Settings or unicorn/Settings

void
TestSettings::testControlPort()
{
    QWARN( "Cannot test controlPort() in Settings" );
    /*MutableSettings mSettings( The::settings() );
    mSettings.setControlPort( 10 );
    QCOMPARE( The::settings().controlPort(), 10 );
    
    mSettings.setControlPort( 8090 );
    QCOMPARE( The::settings().controlPort(), 8090 );*/
}

void
TestSettings::testScrobblePoint()
{
    QWARN( "Cannot test scrobblePoint() in Settings" );
    /*MutableSettings mSettings( The::settings() );
    mSettings.setScrobblePoint( 51 );
    QCOMPARE( The::settings().scrobblePoint(), 51 );
    
    mSettings.setScrobblePoint( 85 );
    QCOMPARE( The::settings().scrobblePoint(), 85 );*/
}

void
TestSettings::testContainerWindowState()
{
    QWARN( "Cannot test containerWindowState() in Settings" );
    /*QMainWindow mw;
    mw.setWindowState( Qt::WindowMinimized | Qt::WindowActive );
    
    MutableSettings mSettings( The::settings() );
    mSettings.setContainerWindowState( mw.windowState() );
    
    QCOMPARE( The::settings().containerWindowState(), mw.windowState() );*/
}

void
TestSettings::testContainerGeometry()
{
    QMainWindow mw;
    mw.resize( 457, 678 );
    QByteArray state = mw.saveState();
    
    MutableSettings mSettings( The::settings() );
    mSettings.setContainerGeometry( state );
    QCOMPARE( The::settings().containerGeometry(), mw.saveState() );
    
    mw.resize( 680, 440 );
    
    state = mw.saveState();
    
    mSettings.setContainerGeometry( state );
    
    QCOMPARE( The::settings().containerGeometry(), mw.saveState() );
}

QTEST_MAIN(TestSettings)
#include "TestSettings.moc"

