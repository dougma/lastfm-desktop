/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "AudioPlaybackEngine.h"
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#include <phonon/mediasource.h>
#include <phonon/backendcapabilities.h>
#include <QDebug>


AudioPlaybackEngine::AudioPlaybackEngine( QObject* parent )
                    :QObject( parent )
{
    m_audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
 
    m_mediaObject = new Phonon::MediaObject( this );
    m_mediaObject->setTickInterval( 1000 );
    connect( m_mediaObject, SIGNAL(stateChanged( Phonon::State, Phonon::State )), SLOT(onPhononStateChanged( Phonon::State, Phonon::State )) );
    connect( m_mediaObject, SIGNAL(currentSourceChanged( Phonon::MediaSource )), SLOT(onPhononSourceChanged( Phonon::MediaSource )) );
	connect( m_mediaObject, SIGNAL(aboutToFinish()), SLOT(onPhononAboutToFinish()) );
    Phonon::createPath( m_mediaObject, m_audioOutput );
}


void
AudioPlaybackEngine::queue( const QList<Track>& tracks )
{
    QList<QUrl> urls;
    foreach (const Track& t, tracks)
    {
        urls += t.url();
        m_queue[t.url()] = t;
    }

    m_mediaObject->enqueue( urls );
}


void
AudioPlaybackEngine::clearQueue()
{
    m_mediaObject->clearQueue();
}


void
AudioPlaybackEngine::skip()
{
    m_mediaObject->setCurrentSource( m_mediaObject->queue().front() );
    m_mediaObject->play();
}


void
AudioPlaybackEngine::stop()
{
    m_mediaObject->stop();
}


void
AudioPlaybackEngine::play()
{
    m_mediaObject->play();
}


void
AudioPlaybackEngine::onPhononStateChanged( Phonon::State newstate, Phonon::State oldstate )
{
    switch (newstate)
    {
        case Phonon::ErrorState:
            qCritical() << m_mediaObject->errorString();
            break;

        case Phonon::StoppedState:
          //  emit playbackEnded();
            break;

        case Phonon::BufferingState:
            emit buffering();
            break;
			
		case Phonon::PausedState:
			emit playbackEnded();
			break;

        case Phonon::PlayingState:
            if( oldstate == Phonon::LoadingState ||
                oldstate == Phonon::StoppedState )
            {
                Q_DEBUG_BLOCK << m_mediaObject->currentSource().url();

                Track t = m_queue.take( m_mediaObject->currentSource().url() );
                if( t.isEmpty() )
                    break;

                emit trackStarted( t );

                if (m_queue.isEmpty())
                    emit queueStarved();

                break;
            }
            if( oldstate == Phonon::BufferingState )
                emit finishedBuffering();
            break;
    }
}


void
AudioPlaybackEngine::onPhononSourceChanged( const Phonon::MediaSource& source )
{
    // This is not a particularly nice work around to force the scrobbler 
    // to change track when the radio track changes.
    // (The phonon state will not automatically change as the tracks will already 
    //  have been queued up and so it will seemlessly continue without changing state)
    
    // Another reason for this being a bad solution is that the track wont 
    // necessarily have started when the source changes but mearly started buffering!
    
    onPhononStateChanged( Phonon::PlayingState, Phonon::LoadingState );
}


void
AudioPlaybackEngine::onPhononAboutToFinish()
{
	emit playbackEnded();
}