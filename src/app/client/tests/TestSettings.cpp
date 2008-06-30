/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
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

void
TestSettings::testControlPort()
{
    MutableSettings mSettings( The::settings() );
    mSettings.setControlPort( 10 );
    QEXPECT_FAIL( "", "We Cannot make moose use test-settings-path, so we cannot test it's settings", Continue );
    QCOMPARE( The::settings().controlPort(), 10 );
    
    mSettings.setControlPort( 8090 );
    QEXPECT_FAIL( "", "We Cannot make moose use test-settings-path, so we cannot test it's settings", Continue );
    QCOMPARE( The::settings().controlPort(), 8090 );
}

void
TestSettings::testScrobblePoint()
{
    MutableSettings mSettings( The::settings() );
    mSettings.setScrobblePoint( 51 );
    QEXPECT_FAIL( "", "We Cannot make moose use test-settings-path, so we cannot test it's settings", Continue );
    QCOMPARE( The::settings().scrobblePoint(), 51 );
    
    mSettings.setScrobblePoint( 85 );
    QEXPECT_FAIL( "", "We Cannot make moose use test-settings-path, so we cannot test it's settings", Continue );
    QCOMPARE( The::settings().scrobblePoint(), 85 );
}

void
TestSettings::testContainerWindowState()
{
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

