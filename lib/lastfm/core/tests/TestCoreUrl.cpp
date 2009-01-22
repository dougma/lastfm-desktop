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

#include <QtTest/QtTest>
#include "lib/lastfm/core/CoreUrl.h"


class TestCoreUrl : public QObject
{
    Q_OBJECT

private slots:
    void encode();      /** @author <jono@last.fm> */
    void encode_data(); /** @^^ */
};

void TestCoreUrl::encode_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow( "ascii" ) << "Metallica" << "Metallica";
    QTest::newRow( "ascii alphanumeric" ) << "Apollo 440" << "Apollo%20440";
    QTest::newRow( "ascii with symbols" ) << "some track [original version]" << "some%20track%20%5Boriginal%20version%5D";
    QTest::newRow( "ascii with dblUrlEncode symbols" ) << "Survivalism [Revision #1]" << "Survivalism%2520%255BRevision%2520%25231%255D";
}

void TestCoreUrl::encode()
{
    QFETCH( QString, string );
    QFETCH( QString, result );

    QCOMPARE( CoreUrl::encode( string ), result );
}

QTEST_MAIN( TestCoreUrl )
#include "TestCoreUrl.moc"
