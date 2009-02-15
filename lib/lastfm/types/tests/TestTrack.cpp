/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include <QtTest>
#include "Track.h"

using lastfm::Track;


class TestTrack : public QObject
{
    Q_OBJECT
    
    Track example()
    {
        lastfm::MutableTrack t;
        t.setTitle( "Test Title" );
        t.setArtist( "Test Artist" );
        t.setAlbum( "Test Album" );
        return t;
    }
    
private slots:
    void testClone()
    {
        Track original = example();
        Track copy = original.clone();
        
        #define TEST( x ) QVERIFY( original.x == copy.x )
        TEST( title() );
        TEST( artist() );
        TEST( album() );
        #undef TEST
    }
};


QTEST_MAIN( TestTrack )
#include "TestTrack.moc"
