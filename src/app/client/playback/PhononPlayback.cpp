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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/

#include "PhononPlayback.h"
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/backendcapabilities.h>
#include <QStringList>


PhononPlayback::PhononPlayback()
{
    foreach (QString s, Phonon::BackendCapabilities::availableMimeTypes())
        qDebug() << "Mime: " << s;
    
    m_audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
    m_mediaObject = new Phonon::MediaObject( this );
    
    Phonon::createPath( m_mediaObject, m_audioOutput );
    
    connect( m_mediaObject, SIGNAL(stateChanged( Phonon::State, Phonon::State )), SLOT(onStateChanged( Phonon::State, Phonon::State )) );
    connect( m_mediaObject, SIGNAL(bufferStatus( int )), SIGNAL(bufferStatusChanged( int )) );
    connect( m_mediaObject, SIGNAL(prefinishMarkReached( qint32 )), SIGNAL(almostFinished()) );

    connect( m_mediaObject, SIGNAL( currentSourceChanged( const Phonon::MediaSource& ) ),
             this, SLOT( privateOnCurrentSourceChanged( const Phonon::MediaSource& ) ) );
}


void
PhononPlayback::setVolume( int v )
{
    m_audioOutput->setVolume( (qreal)v/(qreal)100 );
}


void
PhononPlayback::enqueueTrack( QUrl url )
{
    if (!url.isValid())
    {
        qDebug() << "URL: " << url << " is invalid!";
        qDebug() << "Reason: " << url.errorString();
        emit error( Phonon::NormalError, url.errorString() );
        return;
    } 
    else if ( !Phonon::BackendCapabilities::isMimeTypeAvailable( "audio/x-mp3" ) ) 
    {
        qDebug() << "Mimetype: audio/x-mp3 not available";
        emit error( Phonon::FatalError, tr( "Mimetype audio/x-mp3 is not available" ) );
    }
    qDebug() << "Enqueing track: " << url;
    m_mediaObject->enqueue( QList<QUrl>() << url );
}


void
PhononPlayback::startPlayback()
{
    m_mediaObject->play();
}


void
PhononPlayback::pausePlayback()
{
    m_mediaObject->pause();
}


void
PhononPlayback::stopPlayback()
{
    m_mediaObject->stop();
}


void
PhononPlayback::skip()
{
    qDebug() << "PhononPlayback::skip()";
    stopPlayback();
    QList<Phonon::MediaSource> queue = m_mediaObject->queue();
    clearQueue();
    
    // FIXME: this is not a very nice way to remove the current playing source,
    // but i can't find any other way
    m_mediaObject->setCurrentSource( QString( "" ) );
    
    if ( queue.isEmpty() ) {
    } else {
        m_mediaObject->setQueue( queue );
        startPlayback();
    }
    
    emit skipped();
}


void
PhononPlayback::reset()
{
    stopPlayback();
    m_mediaObject->clearQueue();
    m_mediaObject->setCurrentSource( QString( "" ) );
}


void
PhononPlayback::privateOnStateChanged( Phonon::State newState, Phonon::State oldState )
{
    qDebug() << "PhononPlayback::privateOnStateChanged()";
    
    if ( newState == oldState ) {
        qDebug() << "newState == oldState (" << newState << ")";
        return;
    }
    
    switch ( newState ) {
        case Phonon::ErrorState:
            qDebug() << "Error in PhononPlayback";
            qDebug() << m_mediaObject->errorType();
            qDebug() << m_mediaObject->errorString();
            emit error( m_mediaObject->errorType(), m_mediaObject->errorString() );
            break;
            
        case Phonon::PlayingState:
            qDebug() << "Playback started";
            emit playbackStarted();
            break;
            
        case Phonon::PausedState:
            qDebug() << "Playback paused";
            emit playbackPaused();
            break;
            
        case Phonon::StoppedState:
            qDebug() << "Playback stopped";
            emit playbackStopped();
            break;
            
        case Phonon::BufferingState:
            qDebug() << "Buffering";
            emit buffering();
            break;
            
        case Phonon::LoadingState:
            qDebug() << "Loading";
            emit loading();
            break;
            
        default:
            qDebug() << "????????";
            ;
    }
}


void
PhononPlayback::privateOnCurrentSourceChanged( const Phonon::MediaSource& source )
{
    qDebug() << "PhononPlayback::privateOnCurrentSourceChanged()";
    qDebug() << source.url();
    emit currentTrackChanged( source.url() );
}

