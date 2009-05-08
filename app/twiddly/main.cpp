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

#include "IPod.h"
#include "app/twiddly.h"
#include "app/client/Settings.h"
#include "lib/unicorn/UnicornCoreApplication.h"
#include "lib/unicorn/UniqueApplication.h"
#include "plugins/iTunes/ITunesExceptions.h"
#include <lastfm/misc.h>
#include <QtCore>
#include <QtXml>
#include <iostream>

// until breakpad can be installed more easily
#undef NDEBUG

void writeXml( const QDomDocument&, const QString& path );
void logException( QString );

UniqueApplication gMoose( moose::id() );


/** @maintainer Max Howell <max@last.fm>
  *
  * Fake params: --device ipod --connection usb --pid 4611 --vid 1452 --serial 000000C8B035
  */
int
main( int argc, char** argv )
{
#ifdef NDEBUG
    google_breakpad::ExceptionHandler( CoreDir::save().path().toStdString(),
                                       0,
                                       breakPadExecUploader,
                                       this,
                                       HANDLER_ALL );
#endif

    QCoreApplication::setApplicationName( twiddly::applicationName() );
    QCoreApplication::setApplicationVersion( "2" );

    UniqueApplication uapp( twiddly::id() );
	bool const b = QByteArray(argv[0]) != "--bootstrap-needed?";
    if (b) uapp.init1();

	char* argv2[2];
	argv2[0] = qstrdup( argv[0] );
	argv2[1] = qstrdup( argv[1] );

    unicorn::CoreApplication app( argc, argv );
    if (b) uapp.init2( &app );
    
    try
    {
        if ( app.arguments().contains( "--bootstrap-needed?" ) )
        {
            return AutomaticIPod::PlayCountsDatabase().isBootstrapNeeded();
        }        

        if (uapp.isAlreadyRunning())
            throw "Twiddly already running!";

        QDateTime start_time = QDateTime::currentDateTime();
        QTime time;
        time.start();

        if ( app.arguments().contains( "--bootstrap" ) )
        {
            AutomaticIPod::PlayCountsDatabase().bootstrap();
        }
        else // twiddle!
        {
            gMoose.forward( "--twiddling" );

            IPod* ipod = IPod::fromCommandLineArguments( app.arguments() );

            qDebug() << "Twiddling device: " << ipod->serial;
            ipod->twiddle();

            //------------------------------------------------------------------
            IPodType previousType = ipod->settings().type();
            IPodType currentType = app.arguments().contains( "--manual" ) ? IPodManualType : IPodAutomaticType;

            if ( previousType == IPodManualType && currentType == IPodAutomaticType )
            {
                qDebug() << "iPod switched from manual to automatic - deleting manual db and ignoring scrobbles";

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

            if (ipod->scrobbles().count())
            {
                // create a unique storage location for the XML
                QDir dir = ipod->saveDir().filePath( "scrobbles" );
                QString filename = QDateTime::currentDateTime().toString( "yyyyMMddhhmmss" ) + ".xml";
                QString path = dir.filePath( filename );
                dir.mkpath( "." );

                QDomDocument xml = ipod->scrobbles().xml();
                xml.documentElement().setAttribute( "uid", ipod->uid() );
                writeXml( xml, path );

				QProcess::startDetached( moose::path(), QStringList() << "--twiddled" << path );
            }

            // do last so we don't record a sync if we threw and thus it "didn't" happen
            ipod->settings().setLastSync( start_time );
            delete ipod;           
        }
        
        qDebug() << "Procedure took:" << (time.elapsed() / 1000) << "seconds";
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
    qCritical() << "FATAL ERROR:" << message;
    
    // we do this because LfmApp splits on spaces in parseMessage()
    message.replace( ' ', '_' );
    gMoose.forward( "container://Notification/Twiddly/Error/" + message );
}
