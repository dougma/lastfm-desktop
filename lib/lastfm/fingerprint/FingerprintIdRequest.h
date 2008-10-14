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

#ifndef FPID_FETCHER_H
#define FPID_FETCHER_H

#include "FingerprintGenerator.h"

#include "lib/lastfm/types/Track.h"
#include "lib/lastfm/ws/WsError.h"

#include "FingerprintDllExportMacro.h"

#include <QObject>
#include <QFileInfo>
#include <QNetworkReply>

/**
 *   @brief Check if the Fingerprint ID for the track has been cached in 
 *          the local collection db. If not it generates a Query fingerprint
 *          (using the FingerprintGenerator class) and emits one of the following signals:
 *          1) fpidFetched
 *          2) unknownFingerprint
 *
 **/
 
class FINGERPRINT_DLLEXPORT FingerprintIdRequest : public QObject
{
    Q_OBJECT
    
    public:

        FingerprintIdRequest( const Track&, QObject* parent = 0, bool debug = false );
        ~FingerprintIdRequest();
    
        void start( Fp::Mode = Fp::QueryMode );
        
        const Track& track() const{ return m_track; }
        
        void setAutoDelete( bool );
    
        QNetworkReply::NetworkError networkError(){ return m_networkError; }
        
    signals:
        /* This fingerprint has been found (either in the cache or from the servers ) */
        void FpIDFound( QString );
        
        /* This fingerprint was previously unknown - a new FPID gets assigned but
         * it's probably a good idea to submit the full fingerprint to the servers.
         */
        void unknownFingerprint( QString );
        
        /**
          * The track was found in the local collection database a queuedconnection
          * to FpIDFound will also be made. Unless you want to handle cached FpID's differently, 
          * use the FpIDFound signal.
          **/
        void cachedFpIDFound( QString );
        
        void failed( Fp::Error );
        
    protected slots:
        void onGeneratorSuccess( const QByteArray&, QString sha256 );
        void onFingerprintQueryFetched();
        
        void onFailed( Fp::Error );
        void onSuccess( QString );
        
    private:
        FingerprintGenerator* m_fingerprinter;

        Track m_track;
		class WsAccessManager* m_networkManager;
        QNetworkReply::NetworkError m_networkError;
    
        bool m_debug;
    
        bool m_autoDelete;

};

#endif //FPID_FETCHER_H
