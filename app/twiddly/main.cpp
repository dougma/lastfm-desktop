/***************************************************************************
 *   Copyright 2008 Last.fm Ltd.                                           *
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

#include "IPod.h"

#include "common/logger.h"
#include "common/ITunesExceptions.h"
#include "breakpad/BreakPad.h"
#include "libMoose/MooseCommon.h"

#include <iostream>

#include <QtCore>
#include <QtXml>


/** @originator Max Howell <max@last.fm>
  */

void writeXml( const QDomDocument&, const QString& path );
void initLogger();
void logException( QString );


int
main( int argc, char** argv )
{
    #ifndef NBREAKPAD
        BreakPad breakpad( MooseUtils::savePath() );
        breakpad.setProductName( "Twiddly" );
    #endif
    
    // FIXME these names are wrong, but Moose::Settings is broken
    QCoreApplication::setApplicationName( "Last.fm" );
    QCoreApplication::setOrganizationName( "Last.fm" );
    QCoreApplication::setOrganizationDomain( "last.fm" );
    QCoreApplication app( argc, argv );

    initLogger();
    LOGL( 3, app.arguments().join( " " ) );

    try
    {
        if ( app.arguments().contains( "--bootstrap-needed?" ) )
        {
            return AutomaticIPod::PlayCountsDatabase().isBootstrapNeeded();
        }        

      #ifdef WIN32
        // Create mutex so that plugin doesn't load > 1 twiddly instance
        // We don't know why we go via a QString, but it's old code that we're
        // leaving alone! --mxcl
        QString mutexId( "Twiddly-05F67299-64CC-4775-A10B-0FBF41B6C4D0" );
        HANDLE mutex = ::CreateMutexA( NULL, false, mutexId.toAscii() );
        DWORD const e = ::GetLastError();
        if( e == ERROR_ALREADY_EXISTS || e == ERROR_ACCESS_DENIED )
        {
            LOGL( 3, "Detected another instance of iPodScrobbler running. Aborting..." );
            return 0;
        }
      #endif

        QTime time;
        time.start();

        if ( app.arguments().contains( "--bootstrap" ) )
        {
            AutomaticIPod::PlayCountsDatabase().bootstrap();
        }
        else // twiddle!
        {
            Moose::sendToInstance( "container://Notification/Twiddly/Started" );

            IPod* ipod = IPod::fromCommandLineArguments( app.arguments() );

            LOGL( 3, "Twiddling device: " << ipod->serial );
            ipod->twiddle();

            //------------------------------------------------------------------
            IPod::Type previousType = ipod->settings().type();
            IPod::Type currentType = app.arguments().contains( "--manual" ) ? IPod::ManualType : IPod::AutomaticType;

            if ( previousType == IPod::ManualType && currentType == IPod::AutomaticType )
            {
                LOGL( 3, "iPod switched from manual to automatic"
                         " - deleting manual db and ignoring scrobbles" );

                // The iPod was manual, but is now automatic, we must:
                // 1. remove the manual db, to avoid misscrobbles if it ever becomes
                //    manual again
                QString path = ManualIPod::PlayCountsDatabase( ipod ).path();
                QFile( path ).remove();

                // 2. not scrobble this time :( because any tracks that were on the
                //    the iPod and are also in the iTunes library will be merged
                //    and if they ever played on the iPod, will increase the iTunes
                //    library playcounts. We need to sync the Automatic playcountsdb
                //    but not scrobble anything.
                ipod->scrobbles().clear();
            }

            ipod->settings().setType( currentType );
            //------------------------------------------------------------------

            Moose::sendToInstance( "container://Notification/Twiddly/Finished/" +
                                   QString::number( ipod->scrobbles().count() ) );

            if ( ipod->scrobbles().count() )
            {
                // create a unique storage location for the XML
                QDir dir = ipod->saveDir().filePath( "scrobbles" );
                QString filename = QDateTime::currentDateTime().toString( "yyyyMMddhhmmss" ) + ".xml";
                QString path = dir.filePath( filename );
                dir.mkpath( "." );

                writeXml( ipod->scrobbles().xml(), path );

                Moose::sendToInstance( "container://SubmitScrobbleCache/Device/" + 
                                       ipod->device + '/' + ipod->serial + '/' + ipod->vid + '/' + ipod->pid, 
                                       Moose::StartNewInstance );                  
            }

            delete ipod;           
        }
        
        LOGL( 3, "Procedure took: " << (time.elapsed() / 1000) << " seconds" );
    }
    catch( QString& s )
    {
        logException( s );
    }
    catch( std::string& s )
    {
        logException( QString::fromStdString( s ) );
    }
    catch( const char* s )
    {
        logException( s );
    }
    catch( ITunesException& e )
    {
        logException( e.what() );
    }
    
    return 0;
}


void
writeXml( const QDomDocument& xml, const QString& path )
{
    // we write to a temporary file, and then do an atomic move
    // this prevents the client from potentially reading a corrupt XML file
    QTemporaryFile f;
    
    if (!f.open())
        throw "Couldn't write XML";

    QTextStream s( &f );
    xml.save( s, 2 );
    
    if ( !f.rename( path ) )
        throw QString("Couldn't move to ") + path;
}


void
logException( QString message )
{
    std::string m = message.toStdString();
    std::cout << m << std::endl;
    LOGL( 1, "FATAL ERROR: " << m );
    
    // we do this because LfmApp splits on spaces in parseMessage()
    message.replace( ' ', '_' );
    Moose::sendToInstance( "container://Notification/Twiddly/Error/" + message );
}


void
initLogger()
{
    #ifdef Q_OS_MAC
        #define FILENAME "Last.fm Twiddly.log"
    #else
        #define FILENAME "Twiddly.log"
    #endif

    Logger& logger = Logger::GetLogger();
    logger.Init( MooseUtils::logPath( FILENAME ), false );
    //commented out as was hanging windows debug builds
//    logger.SetLevel( Logger::Debug );
    LOGL( 3, "Log file created" );
}
