/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#include "MooseCommon.h"
#include "MooseSettings.h"

#include "lib/core/UnicornUtils.h"
#include "lib/core/MessageBoxBuilder.h"

#include <QCoreApplication>
#include <QDir>
#include <QHostAddress>
#include <QLibrary>
#include <QMap>
#include <QPluginLoader>
#include <QProcess>
#include <QTcpSocket>


#if 0
QString
Moose::servicePath( QString name )
{
    QString dirPath;
    #ifdef WIN32
        // Hack to get it working with VS2005
        dirPath = qApp->applicationDirPath();
    #else
        dirPath = qApp->applicationDirPath() + "/services";
    #endif

    if ( name.isEmpty() )
    {
        return dirPath;
    }

    QDir servicesDir( dirPath );
    QString fileName = SERVICE_PREFIX + name + LIB_EXTENSION;

    return servicesDir.absoluteFilePath( fileName );
}


// This is the kind of beastly function that Max will throw a hissy fit if he can't have.
static void
loadServiceError( QString name )
{
    LastMessageBox::critical(
        QCoreApplication::translate( "Container", "Error" ),
        QCoreApplication::translate( "Container", "Couldn't load service: %1. The application won't be able to start." ).arg( name ) );

    QCoreApplication::exit( 1 );
}


QObject*
Moose::loadService( QString name )
{
    QString path = servicePath( name );

    qDebug() << "Loading service: " << name << "at" << path;

    QObject* plugin = QPluginLoader( path ).instance();

    if ( plugin == NULL )
    {
        loadServiceError( name );
        return NULL;
    }

    return plugin;
}
#endif


bool
Moose::isAlreadyRunning()
{
  #ifdef WIN32
    QString id( "Lastfm-F396D8C8-9595-4f48-A319-48DCB827AD8F" );
    ::CreateMutexA( NULL, false, id.toAscii() );

    // The call fails with ERROR_ACCESS_DENIED if the Mutex was 
    // created in a different users session because of passing
    // NULL for the SECURITY_ATTRIBUTES on Mutex creation);
    return ::GetLastError() == ERROR_ALREADY_EXISTS || ::GetLastError() == ERROR_ACCESS_DENIED;
  #else
    return sendToInstance( "", Moose::DontStartNewInstance );
  #endif
}


//FIXME this can take up to 500ms to start a new instance!
bool
Moose::sendToInstance( const QString& data, Moose::StartNewInstanceBehaviour behaviour )
{
    QTcpSocket socket;
    socket.connectToHost( QHostAddress::LocalHost, Moose::Settings().controlPort() );

    if ( socket.waitForConnected( 500 ) )
    {   
        if ( data.length() > 0 )
        {
            QByteArray utf8Data = data.toUtf8();
            socket.write( utf8Data, utf8Data.length() );
            socket.flush();
        }

        socket.close();
        return true;
    }
    else if ( behaviour == Moose::StartNewInstance )
    {
        qDebug() << "Starting instance" << Moose::Settings().path();
        //FIXME doesn't work for some reason
        return QProcess::startDetached( Moose::Settings().path(), QStringList() << data );
    }

    return false;
}


#if 0
QStringList
extensionPaths()
{
    QStringList paths;
    
  #ifdef WIN32
    // Hack to get it working with VS2005
    QString path = qApp->applicationDirPath();
  #else
    QString path = qApp->applicationDirPath() + "/extensions";
  #endif

  #ifndef QT_NO_DEBUG
    path += "/debug";
  #endif

    QDir d( path );
    foreach( QString fileName, d.entryList( QDir::Files ) )
    {
        if (!fileName.startsWith( EXTENSION_PREFIX ))
            continue;
        QString const path = d.absoluteFilePath( fileName );
        if (!QLibrary::isLibrary( path ))
            continue;
        paths += path;
    }

    return paths;
}
#endif
