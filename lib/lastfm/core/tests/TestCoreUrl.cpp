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
    
    void no404();       /** @author <max@last.fm> */
    void no404_data(); /** @^^ */
    
    void test404();
};


void
TestCoreUrl::encode_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("output");

    QTest::newRow( "ascii" ) << "Metallica" << "Metallica";
    QTest::newRow( "ascii alphanumeric" ) << "Apollo 440" << "Apollo+440";
    QTest::newRow( "ascii with symbols" ) << "some track [original version]" << "some+track+%5Boriginal+version%5D";
    QTest::newRow( "ascii with last.fm-special symbols" ) << "Survivalism [Revision #1]" << "Survivalism%2B%255BRevision%2B%25231%255D";
}


void
TestCoreUrl::encode()
{
    QFETCH( QString, input );
    QFETCH( QString, output );    
    QCOMPARE( lastfm::UrlBuilder::encode( input ), output.toAscii() );
}


#include <lastfm/public.h>
#include <QtNetwork>
#include <QEventLoop>
static inline int getResponseCode( const QUrl& url )
{
    QNetworkAccessManager nam;
    QNetworkReply* reply = nam.head( QNetworkRequest(url) );
    
    QEventLoop loop;
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();

    int const code = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();

    if (reply->error() != QNetworkReply::NoError)
        qDebug() << url << lastfm::qMetaEnumString<QNetworkReply>( reply->error(), "NetworkError" ) << code;

    return code;
}


void
TestCoreUrl::no404_data()
{
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("track");

    #define NEW_ROW( x, y ) QTest::newRow( x " - " y ) << x << y;
    NEW_ROW( "Air", "Radio #1" );
    NEW_ROW( "Pink Floyd", "Speak to Me / Breathe" );
    NEW_ROW( "Radiohead", "2 + 2 = 5" );
    NEW_ROW( "Above & Beyond", "World On Fire (Maor Levi Remix)" );
    #undef NEW_ROW
}

void
TestCoreUrl::no404()
{
    QFETCH( QString, artist );
    QFETCH( QString, track );
    
    QUrl url = lastfm::UrlBuilder( "music" ).slash( artist ).slash( "_" ).slash( track ).url();
        
    QCOMPARE( getResponseCode( url ), 200 );
}


void
TestCoreUrl::test404()
{
    QCOMPARE( getResponseCode( QUrl("http://www.last.fm/404") ), 404 );
}


QTEST_MAIN( TestCoreUrl )
#include "TestCoreUrl.moc"
