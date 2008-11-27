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

#include "AbstractBootstrapper.h"
#include "common/qt/md5.cpp"
#include "lib/lastfm/core/CoreSettings.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QtNetwork/QHttp>
#include <QTextStream>
#include <time.h>

#define ZLIB_DLL
#include "zlib.h"

static const QString k_host = "bootstrap.last.fm";


AbstractBootstrapper::AbstractBootstrapper( QObject* parent )
                    : QObject( parent ),
                      m_http( 0 )
{}


bool
AbstractBootstrapper::zipFiles( const QString& inFileName, const QString& outFileName ) const
{
    QDir temp = QDir::temp();

    temp.remove( outFileName );

    gzFile outFile = gzopen( outFileName.toLocal8Bit(), "wb" );
    if ( !outFile )
        return false;

    QFile inFile( inFileName );
    if ( !inFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return false;

    if ( gzputs( outFile, inFile.readAll().data() ) < 1 )
        return false;

    gzclose( outFile );
    inFile.close();

    return true;
}


void
AbstractBootstrapper::sendZip( const QString& inFile )
{
    QString username = CoreSettings().value( "Username" ).toString();
    QString passMd5 = CoreSettings().value( "Password" ).toString();

    delete m_http;

    m_http = new QHttp( k_host, 80, this );
    connect( m_http, SIGNAL(requestFinished( int, bool )), SLOT(onUploadDone( int, bool )), Qt::QueuedConnection );
    connect( m_http, SIGNAL(dataSendProgress( int, int )), SLOT(onUploadProgress( int, int )) );

    // Get Unix time
    time_t now;
    time( &now );
    QString time = QString::number( now );

    // Concatenate pw hash with time
    QString auth = passMd5 + time;
    QString authLower = passMd5.toLower() + time;
    // Hash the concatenated string to create auth code
    QString authMd5 = Qt::md5( auth.toUtf8() );
    QString authMd5Lower = Qt::md5( authLower.toUtf8() );

    QString const path = "/bootstrap/index.php?user="+username+"&time="+time+"&auth="+authMd5+"&authlower="+authMd5Lower;

    QFile* zipFile = new QFile( this );
    zipFile->setFileName( inFile );
    zipFile->open( QIODevice::ReadOnly );

    QHttpRequestHeader header( "POST", path, 1, 1 );
    header.setValue( "Host", k_host );
    header.setValue( "Content-type", "multipart/form-data, boundary=AaB03x" );
    header.setValue( "Cache-Control", "no-cache" );
    header.setValue( "Accept", "*/*" );

    QByteArray bytes;
    bytes.append( "--AaB03x\r\n" );
    bytes.append( "content-disposition: " );
    bytes.append( "form-data; name=\"agency\"\r\n" );
    bytes.append( "\r\n" );
    bytes.append( "0\r\n" );
    bytes.append( "--AaB03x\r\n" );
    bytes.append( "content-disposition: " );
    bytes.append( "form-data; name=\"bootstrap\"; filename=\"" + zipFile->fileName() + "\"\r\n" );
    bytes.append( "Content-Transfer-Encoding: binary\r\n" );
    bytes.append( "\r\n" );

    bytes.append( zipFile->readAll() );
    zipFile->close();

    bytes.append( "\r\n" );
    bytes.append( "--AaB03x--" );
    header.setContentLength( bytes.length() );

    qDebug() << "Sending " << path;

    emit percentageUploaded( 0 );

    m_reqId = m_http->request( header, bytes );
}


void
AbstractBootstrapper::onUploadProgress( int done, int total )
{
    emit percentageUploaded( int( float(done / total) * 100.0 ) );
}


void
AbstractBootstrapper::onUploadDone( int id, bool error )
{
    if (id != m_reqId)
        return;

    if( m_http->lastResponse().statusCode() == 403 )
    {
        emit done( Bootstrap_Denied );
        return;
    }

    if( error )
    {
        qDebug() << m_http->errorString();
        emit done( Bootstrap_UploadError );
        return;
    }

    qDebug() << "Bootstrap.zip sent to last.fm!";
    emit done( Bootstrap_Ok );
}
