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
    
    // these fields are required
    MutableTrack t;
    t.setArtist( "Air" );
    t.setTitle( "Redhead Girl" );
    t.setAlbum( "Pocket Symphony" );
    t.setUrl( QUrl::fromLocalFile( "/Users/mxcl/Music/iTunes/iTunes Music/Air/Pocket Symphony/1-11 Redhead Girl.mp3") );
    
    try
    {
        Fingerprint fp( t );
        
        // we cache FingerprintIds in an sqlite3 db, as the generate() function
        // is expensive
        if (fp.id().isNull())
        {
            // this generates the full fingerprint hash, which is about 20kB
            fp.generate();
            
            // this asks Last.fm for a FingerprintId
            // the finish function is a Qt hack to allow syncronous HTTP
            finish( fp.submit() );
            
            // the decode step sets the FingerprintId
            // the FingerprintId is required to obtain suggestions
            // id will now be valid, or this function throws
            fp.decode( reply );
        }

        finish( fp.id().getSuggestions() );

        qDebug() << FingerprintId::getSuggestions( reply );
    }
    catch (Fingerprint::Error e)
    {
        qWarning() << e; //TODO enum debug thing
    }    
}
