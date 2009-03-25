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

#include <lastfm/Fingerprint>
#include <lastfm/FingerprintId>
#include <QtCore>
#include <QtNetwork>

using namespace lastfm;


static void finish( QNetworkReply* reply )
{
    QEventLoop loop;
    loop.connect( reply, SIGNAL(finished()), SLOT(quit()) );
    loop.exec();
}


int main( int argc, char** argv )
{
    QCoreApplication app( argc, argv );
    
    MutableTrack t;
    t.setArtist( "Air" );
    t.setTitle( "Redhead Girl" );
    t.setAlbum( "Pocket Symphony" );
    t.setUrl( QUrl::fromLocalFile( "/Users/mxcl/Music/iTunes/iTunes Music/Air/Pocket Symphony/1-11 Redhead Girl.mp3") );

    Fingerprint fp( t );
    
    if(fp.id().isNull()){
        fp.generate();
        finish( fp.submit() );
        fp.decode( reply );
    }
    
    if (fp.id().isNull()) { 
        qWarning() << "Still null :(";
        return 1;
    }
    
    finish( fp.id().getSuggestions() );

    qDebug() << FingerprintId::getSuggestions( reply );
}
