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

#include "Fingerprint.h"
#include "Collection.h"
#include "Sha256.h"
#include "MP3_Source_Qt.h"
#include "fplib/include/FingerprintExtractor.h"
#include <QFileInfo>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <fstream>

using lastfm::Track;

static const uint k_bufferSize = 1024 * 8;
static const int k_minTrackDuration = 30;


Fingerprint::Fingerprint( const Track& t )
           : m_track( t ),
             m_extractor( 0 ),
             m_id( -1 ),
             m_complete( false )
{
    QString id = Collection::instance().getFingerprintId( t.url().toLocalFile() );   
    if (id.size()) {
        bool b;
        m_id = id.toInt( &b );
        if (!b) m_id = -1;
    }
}


Fingerprint::~Fingerprint()
{
    delete m_extractor;
}


void
Fingerprint::generate() throw( Fp::Error )
{
    #define CATCH( y ) catch (const std::exception& e) { qWarning() << e.what(); throw( y ); }
    
    QString const path = m_track.url().toLocalFile();
    
    if (!QFileInfo( path ).isReadable())
        throw Fp::ReadError;

    int duration, sampleRate, bitrate, numChannels;
    MP3_Source ms;
    
    try
    {
        MP3_Source::getInfo( path, duration, sampleRate, bitrate, numChannels );
        ms.init( path );
    }
    CATCH( Fp::GetInfoError )
    
    if (duration < k_minTrackDuration)
        throw Fp::TrackTooShortError;
    
    ms.skipSilence();
    
    bool fpDone = false;
    try
    {
        delete m_extractor;
        m_extractor = new fingerprint::FingerprintExtractor;
        
        if (m_complete)
        {
            m_extractor->initForFullSubmit( sampleRate, numChannels );
        }
        else 
        {
            m_extractor->initForQuery( sampleRate, numChannels, duration );
            
            // Skippety skip for as long as the skipper sez (optimisation)
            ms.skip( m_extractor->getToSkipMs() );
            float secsToSkip = m_extractor->getToSkipMs() / 1000.0f;
            fpDone = m_extractor->process( 0,
                                         (size_t) sampleRate * numChannels * secsToSkip,
                                         false );
        }
    }
    CATCH( Fp::ExtractorInitError)
    
    const size_t PCMBufSize = 131072; 
    short* pPCMBuffer = new short[PCMBufSize];
    
    while (!fpDone)
    {
        size_t readData = ms.updateBuffer( pPCMBuffer, PCMBufSize );
        if (readData == 0)
            break;
        
        try
        {
            fpDone = m_extractor->process( pPCMBuffer, readData, ms.eof() );
        }
        catch ( const std::exception& e )
        {
            qWarning() << e.what();
            delete[] pPCMBuffer;
            throw Fp::ExtractorProcessError;
        }
    }
    
    delete[] pPCMBuffer;
    
    if (!fpDone)
        throw Fp::ExtractorNotEnoughDataError;
    
    // We succeeded
    std::pair<const char*, size_t> fpData = m_extractor->getFingerprint();
    
    if (fpData.first == NULL || fpData.second == 0)
        throw Fp::ExtractorNotReadyError;
    
    m_data = QByteArray::fromRawData( fpData.first, fpData.second );
}


static QString sha256( const QString& path )
{
    // no clue why this is static, there was no comment when I refactored it
    // initially --mxcl
    static uint8_t pBuffer[SHA_BUFFER_SIZE+7];
    
    unsigned char hash[SHA256_HASH_SIZE];

    {
        QByteArray path8 = QFile::encodeName( path );
        std::ifstream inFile( path8.data(), std::ios::binary);
        
        SHA256Context sha256;
        SHA256Init( &sha256 );
        
        uint8_t* pMovableBuffer = pBuffer;
        
        // Ensure it is on a 64-bit boundary. 
        INTPTR offs;
        if ((offs = reinterpret_cast<INTPTR>(pBuffer) & 7L))
            pMovableBuffer += 8 - offs;
        
        unsigned int len;
        
        for (;;)
        {
            inFile.read( reinterpret_cast<char*>(pMovableBuffer), SHA_BUFFER_SIZE );
            len = inFile.gcount();
            
            if (len == 0)
                break;
            
            SHA256Update( &sha256, pMovableBuffer, len );
        }
        
        SHA256Final( &sha256, hash );
    }

    QString sha;
    for (int i = 0; i < SHA256_HASH_SIZE; ++i) 
    {
        QString hex = QString("%1").arg(uchar(hash[i]), 2, 16,
                                        QChar('0'));
        sha.append(hex);
    }

    return sha;
}


QNetworkReply*
Fingerprint::submit( QNetworkAccessManager* nam ) const
{
    if (m_data.isEmpty())
        return 0;
    
	//Parameters understood by the server according to the MIR team: 
	//{ "trackid", "recordingid", "artist", "album", "track", "duration", 
	//  "tracknum", "username", "sha256", "ip", "fpversion", "mbid", 
	//  "filename", "genre", "year", "samplerate", "noupdate", "fulldump" }
	
    Track const t = m_track;
    QString const path = t.url().toLocalFile();
    QFileInfo const fi( path );

    #define e( x ) QUrl::toPercentEncoding( x )
    QUrl url( "http://www.last.fm/fingerprint/query/" );
    url.addEncodedQueryItem( "artist", e(t.artist()) );
    url.addEncodedQueryItem( "album", e(t.album()) );
    url.addEncodedQueryItem( "track", e(t.title()) );
    url.addEncodedQueryItem( "duration", QByteArray::number(t.duration()) );
    url.addEncodedQueryItem( "mbid", e(t.mbid()) );
    url.addEncodedQueryItem( "filename", e(fi.completeBaseName()) );
    url.addEncodedQueryItem( "fileextension", e(fi.completeSuffix()) );
    url.addEncodedQueryItem( "tracknum", QByteArray::number( t.trackNumber() ) );
    url.addEncodedQueryItem( "sha256", sha256( path ).toAscii() );
    url.addEncodedQueryItem( "time", QByteArray::number(QDateTime::currentDateTime().toTime_t()) );
    url.addEncodedQueryItem( "fpversion", QByteArray::number((int)fingerprint::FingerprintExtractor::getVersion()) );
    url.addEncodedQueryItem( "fulldump", m_complete ? "true" : "false" );
    url.addEncodedQueryItem( "noupdate", "false" );
    #undef e

	//FIXME: talk to mir about submitting fplibversion

    QNetworkRequest request( url );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=----------------------------8e61d618ca16" );

    QByteArray bytes;
    bytes += "------------------------------8e61d618ca16\r\n";
    bytes += "Content-Disposition: ";
    bytes += "form-data; name=\"fpdata\"";
    bytes += "\r\n\r\n";
    bytes += m_data;
    bytes += "\r\n";
    bytes += "------------------------------8e61d618ca16--\r\n";

    qDebug() << url;
    qDebug() << "Fingerprint size:" << bytes.size() << "bytes";

    return nam->post( request, bytes );
}


void
Fingerprint::decode( QNetworkReply* reply, bool* complete_fingerprint_requested )
{
    // The response data will consist of a number and a string.
    // The number is the fpid and the string is either FOUND or NEW
    // (or NOT FOUND when noupdate was used). NEW means we should
    // schedule a full fingerprint.
    //
    // In the case of an error, there will be no initial number, just
    // an error string.
    
    QString const response( reply->readAll() );
    QStringList const list = response.split( ' ' );
       
    if (response.isEmpty() || list.count() < 2) {
        qWarning() << "Null response";
        return;
    }

    QString const fpid = list.value( 0 );
    QString const status = list.value( 1 );
    
    bool b;
    uint fpid_as_uint = fpid.toUInt( &b );
    if (!b) return;
    
    Collection::instance().setFingerprintId( m_track.url().toLocalFile(), fpid );
    
    if (complete_fingerprint_requested)
        *complete_fingerprint_requested = (status == "NEW");

    m_id = (int)fpid_as_uint;
}
