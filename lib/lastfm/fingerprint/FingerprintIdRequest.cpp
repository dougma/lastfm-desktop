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

#include "FingerprintIdRequest.h"
#include "FingerprintGenerator.h"
#include "Collection.h"
#include "lib/lastfm/ws/WsAccessManager.h"

#include <QApplication>
#include <QNetworkRequest>
#include <QNetworkReply>


FingerprintIdRequest::FingerprintIdRequest( const Track& track, QObject* parent /* = 0 */, bool debug /* = false */ ) 
            :QObject( parent ),
             m_track( track ),
             m_networkError( QNetworkReply::NoError ),
             m_debug( debug ),
             m_autoDelete( true )
{
    qRegisterMetaType<Fp::Error>("Fp::Error");
    
    m_networkManager = new WsAccessManager( this );
    
    connect( this, SIGNAL( cachedFpIDFound( QString)), 
                   SIGNAL( FpIDFound( QString)), Qt::QueuedConnection );
    
    
    QString fpId = Collection::instance().getFingerprint( track.url().toLocalFile() );
    
    if ( !fpId.isEmpty() )
    {
        qDebug() << "Fingerprint found in cache for" << track;
        emit cachedFpIDFound( fpId );
        return;
    }
    
}


FingerprintIdRequest::~FingerprintIdRequest()
{}


void
FingerprintIdRequest::start( Fp::Mode mode /* = QueryMode */ )
{
    m_fingerprinter = new FingerprintGenerator( m_track.url().toLocalFile(), mode, this );

    connect( m_fingerprinter, SIGNAL( failed( Fp::Error )), SIGNAL( failed( Fp::Error )));

    //connect all success / failures to a private slot so the object can 
    //be autoDeleted if appropriate.
    connect( this, SIGNAL( failed( Fp::Error )), SLOT( onFailed( Fp::Error )));
    connect( this, SIGNAL( FpIDFound( QString )), SLOT( onSuccess( QString )));
    connect( this, SIGNAL( unknownFingerprint( QString )), SLOT( onSuccess( QString )));
    
    connect( m_fingerprinter, SIGNAL( success( QByteArray, QString )), 
                              SLOT( onGeneratorSuccess( QByteArray, QString )) );
}


void
FingerprintIdRequest::onGeneratorSuccess( const QByteArray& fp, QString sha256 )
{
    time_t now;
    time( &now );
    QString time = QString::number( now );
    
    QUrl queryUrl( "http://www.last.fm/fingerprint/query/" );
    

	//Parameters understood by the server according to the MIR team: 
	//{ "trackid", "recordingid", "artist", "album", "track", "duration", 
	//  "tracknum", "username", "sha256", "ip", "fpversion", "mbid", 
	//  "filename", "genre", "year", "samplerate", "noupdate", "fulldump" }
	
	#define QUERYLIST QList<QPair<QString, QString> >()
    #define QUERYITEM( X, Y ) QPair<QString, QString>( #X, Y )
    #define QUERYITEMENCODED( X, Y ) QUERYITEM( X, QUrl::toPercentEncoding( Y ))
    queryUrl.setQueryItems( QUERYLIST <<
                            QUERYITEMENCODED( artist,        m_track.artist() ) <<
                            QUERYITEMENCODED( album,         m_track.album() ) <<
                            QUERYITEMENCODED( track,         m_track.title() ) <<
                            QUERYITEM(        duration,      QString::number( m_track.duration()) ) <<
                            QUERYITEM(        mbid,          m_track.mbid() ) <<
                            QUERYITEMENCODED( filename,      QFileInfo( m_track.url().toLocalFile() ).completeBaseName() ) <<
                            QUERYITEM(        tracknum,      QString::number( m_track.trackNumber() ) ) <<
                            QUERYITEM(        sha256,        sha256 ) <<
                            QUERYITEM(        time,          time ) <<
							
						    QUERYITEMENCODED( fpversion,     QString::number( fingerprint::FingerprintExtractor::getVersion() ) ) <<
                            QUERYITEM(        fulldump,      "false" ) <<
                            QUERYITEM(        noupdate,       m_debug ? "true" : "false" ));
	//FIXME: talk to mir about submitting fplibversion
                            
    #undef QUERYITEMENCODED
    #undef QUERYITEM
    #undef QUERYLIST
    
    QNetworkRequest fingerprintQueryRequest( queryUrl );
    fingerprintQueryRequest.setHeader( QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=----------------------------8e61d618ca16" );

    QByteArray bytes;
    bytes.append( "------------------------------8e61d618ca16\r\n" );
    bytes.append( "Content-Disposition: " );
    bytes.append( "form-data; name=\"fpdata\"" );
    bytes.append( "\r\n\r\n" );

    bytes.append( fp );
    bytes.append( "\r\n");
    bytes.append( "------------------------------8e61d618ca16--\r\n");
    
    QNetworkReply* reply = m_networkManager->post( fingerprintQueryRequest, bytes );
    connect( reply, SIGNAL( finished()), SLOT( onFingerprintQueryFetched()) );

}


void
FingerprintIdRequest::onFailed( Fp::Error e )
{
    Q_UNUSED( e );
    if( m_autoDelete )
        deleteLater();
}


void
FingerprintIdRequest::onSuccess( QString s )
{
    Q_UNUSED( s );
    if( m_autoDelete )
        deleteLater();
}


void
FingerprintIdRequest::onFingerprintQueryFetched()
{
    QNetworkReply* queryReq = static_cast<QNetworkReply*>( sender() );

    if ( queryReq->error() )
    {
        qDebug() << "Network error: " << queryReq->error();

        m_networkError = queryReq->error();
        
        emit failed( Fp::NetworkError );
        return;
    }

    // The response data will consist of a number and a string.
    // The number is the fpid and the string is either FOUND or NEW
    // (or NOT FOUND when noupdate was used). NEW means we should
    // schedule a full fingerprint.
    //
    // In the case of an error, there will be no initial number, just
    // an error string.

    QString response( queryReq->readAll() );
    QStringList list = response.split( " " );
    
    if( list.isEmpty() )
    {
        emit failed( Fp::MalformedResponse );
        return;
    }
    
    QString fpid = list.at( 0 );
    bool isANumber;
    fpid.toUInt( &isANumber );
    if ( !isANumber )
    {
        qDebug() << "Malformed resonse: " << response;
        emit failed( Fp::MalformedResponse );
        return;
    }

    QString status = list.at( 1 );

    Collection::instance().setFingerprint( m_track.url().toLocalFile(), fpid );
    
    if( status == "NEW" )
    {
        qDebug() << "NEW fingerprint found for track: " << m_track;
        emit unknownFingerprint( fpid );
    }
    else
    {
        qDebug() << "fingerprint found for track: " << m_track << " fpid: " << fpid;
        emit FpIDFound( fpid );
    }
}


void 
FingerprintIdRequest::setAutoDelete( bool autoDelete )
{
    m_autoDelete = autoDelete;
}
