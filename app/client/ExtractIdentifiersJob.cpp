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

#include "ExtractIdentifiersJob.h"
#include "lib/lastfm/fingerprint/Fingerprint.h"
#include "lib/lastfm/ws/WsAccessManager.h"
#include <QDebug>
#include <QEventLoop>
#include <QNetworkReply>
#include <QDebug>


ExtractIdentifiersJob::ExtractIdentifiersJob( const Track& t )
             : m_gui_track( t ),
               m_track( t.clone() ), 
               m_path( t.url().path() )
{   
    connect( this, SIGNAL(mbid( QString )), SLOT(onMbid( QString )) );
}


ExtractIdentifiersJob::~ExtractIdentifiersJob()
{
    m_gui_track = m_track;
}


void
ExtractIdentifiersJob::run()
{
    MutableTrack t( m_track );
    t.setMbid( Mbid::fromLocalFile( m_path ) );
    
    if (!t.mbid().isNull())
    {
        qDebug() << "Got mbid:" << t.mbid();
        emit mbid( t.mbid() );
    }
    
    t.setFingerprintId( fpid() );
    qDebug() << "Got fpid:" << t.fingerprintId();
}


FingerprintId
ExtractIdentifiersJob::fpid() const
{
    WsAccessManager wam;
    Fingerprint fp( m_track );
    
    try
    {
        qDebug() << "Doing partial fingerprint for" << m_track;
        fp.generate();

        QNetworkReply* reply = fp.submit( &wam );
        waitForFinished( reply );
        
        bool complete_fp_required = false;
        fp.decode( reply, &complete_fp_required );
        
        if (complete_fp_required)
        {
            qDebug() << "Doing complete fingerprint for" << m_track;
            CompleteFingerprint fp( m_track );
            fp.generate();
            fp.submit( &wam );
            waitForFinished( reply );
        }

    }
    catch (Fp::Error e)
    {
        qWarning() << e;
    }
    
    return fp.id();
}


void
ExtractIdentifiersJob::onMbid( const QString& mbid )
{
    MutableTrack( m_gui_track ).setMbid( Mbid(mbid) );
}


void
ExtractIdentifiersJob::waitForFinished( QNetworkReply* reply )
{
    QEventLoop loop;
    connect( reply, SIGNAL(finished()), &loop, SLOT(quit()) );
    loop.exec();    
}
