/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QThread>
#include <QTimer>
#include <cmath>
#include <lastfm/RadioTuner>
#include "Radio.h"

#ifdef Q_OS_UNIX
    #include <phonon/mediaobject.h>
    #include <phonon/backendcapabilities.h>
#else
    #include <phonon>
#endif

Radio::Radio( )
     : m_audioOutput( 0 ),
       m_mediaObject( 0 ),
       m_state( Radio::Stopped ),
       m_bErrorRecover( false )
{
}


Radio::~Radio()
{    
    // I'm not confident about the sleep code on Windows --mxcl
#ifndef WIN32
	if (m_mediaObject->state() != Phonon::PlayingState)
        return;

    qreal starting_volume = m_audioOutput->volume();
    //sigmoid curve
    for (int x = 18; x >= -60; --x)
    {
        qreal y = x;
        y /= 10;
        y = qreal(1) / (qreal(1) + std::exp( -y ));
        y *= starting_volume;
        m_audioOutput->setVolume( y );

		struct Thread : QThread { using QThread::msleep; };
		Thread::msleep( 7 );
    }
#endif
}

// fixme:
// todo:
// note:
// if the station is the same as current station (ie: the user hit stop then start)
// then we *don't* retune.  norman is quite emphatic about this.  :)
void
Radio::play( const RadioStation& station )
{
    if (m_state != Stopped)
    {
        //FIXME filthy! get us to a clean slate
        State oldstate = m_state;
        m_state = Stopped;    //prevents stateChanged() doing anything
        stop();
        clear();
        m_state = oldstate;
    }

	delete m_tuner;
    m_tuner = 0;

    if (0 == m_audioOutput)  {
        if (!initRadio()) {
            changeState( Stopped );
            return;
        }
    }

	m_station = station;
    m_tuner = new lastfm::RadioTuner(station);

	connect( m_tuner, SIGNAL(title( QString )), SLOT(setStationNameIfCurrentlyBlank( QString )) );
	connect( m_tuner, SIGNAL(trackAvailable()), SLOT(enqueue()) );
    connect( m_tuner, SIGNAL(error( lastfm::ws::Error )), SLOT(onTunerError( lastfm::ws::Error )) );

    changeState( TuningIn );
}


void
Radio::enqueue()
{  
    if (m_state == Stopped) {
        // this should be impossible. If we are stopped, then the GUI looks
        // stopped too, and receiving tracks to play will result in a playing
        // radio and a stopped GUI. NICE.
        Q_ASSERT( 0 );
        return;
    }
	
    phononEnqueue();
}


void
Radio::skip()
{
    if (!m_mediaObject)
        return;

    if (m_track.extra( "rating" ).isEmpty())
        MutableTrack( m_track ).setExtra( "rating", "S" );
    
    // attempt to refill the phonon queue if it's empty
	if (m_mediaObject->queue().isEmpty())
        phononEnqueue();
    
	QList<Phonon::MediaSource> q = m_mediaObject->queue();
    if (q.size())
	{
		Phonon::MediaSource source = q.takeFirst();
		m_mediaObject->setQueue( q );
		m_mediaObject->setCurrentSource( source );
		m_mediaObject->play();
	}
    else if (m_state != Stopped)
    {
        qDebug() << "queue empty";
	    // we are still waiting for a playlist to come back from the tuner
	    m_mediaObject->blockSignals( true );    //don't tell outside world that we stopped
	    m_mediaObject->stop();
	    m_mediaObject->setCurrentSource( QUrl() );
	    m_mediaObject->blockSignals( false );
	    changeState( TuningIn );
    }
}


void
Radio::onTunerError( lastfm::ws::Error e )
{
    // otherwise leave things be, we'll stop when we run out of content
    if (m_state == TuningIn)
		stop();

	emit error( e );
}


void
Radio::stop()
{
    delete m_tuner;
    
    m_mediaObject->blockSignals( true ); //prevent the error state due to setting current source to null
	m_mediaObject->stop();
	m_mediaObject->clearQueue();
	m_mediaObject->setCurrentSource( QUrl() );
    m_mediaObject->blockSignals( false );

    clear();
    
    changeState( Stopped );
}


void
Radio::clear()
{
    m_track = Track();
    m_station = RadioStation();
    delete m_tuner;
}


void
Radio::onPhononStateChanged( Phonon::State newstate, Phonon::State oldstate )
{
    qDebug() << oldstate << " -> " << newstate;
    if (m_mediaObject == 0) {
        qDebug() << "m_mediaObject is null!";
        return;
    }

    switch (newstate)
    {
        case Phonon::ErrorState:
			if (m_mediaObject->errorType() == Phonon::FatalError) {
                qWarning() << "Phonon fatal error:" << m_mediaObject->errorString();
                emit error( lastfm::ws::UnknownError, QVariant( m_mediaObject->errorString() ));
                deInitRadio();
                changeState( Radio::Stopped );
            } else {
                // seems we need to clear the error state before trying to play again.
                m_bErrorRecover = true;
                m_mediaObject->stop();
            }
            break;
			
		case Phonon::PausedState:
			// if the play queue runs out we get this for some reason
			// this means we are fetching new tracks still, we should show a 
			// tuning in state;
            if (m_mediaObject->queue().size() == 0) {
                qDebug() << "queue empty, going to TuningIn";
                changeState( TuningIn );
            }
			break;
			
        case Phonon::StoppedState:
            if (m_bErrorRecover) {
                m_bErrorRecover = false;
                skip();
            }
            break;
			
        case Phonon::BufferingState:
            changeState( Buffering );
            break;

		case Phonon::PlayingState:
            changeState( Playing );
            break;

		case Phonon::LoadingState:
			break;
    }
}


void
Radio::phononEnqueue()
{
    if (m_mediaObject->queue().size() || !m_tuner)
        return;

    // keep only one track in the phononQueue
    // Loop until we get a null url or a valid url.
    for (;;)
    {
        // consume next track from the track source. a null track 
        // response means wait until the trackAvailable signal
        Track t = m_tuner->takeNextTrack();
        if (t.isNull()) break;

        // Invalid urls won't trigger the correct phonon
        // state changes, so we must filter them.
        if (!t.url().isValid()) continue;
        
        m_track = t;
        Phonon::MediaSource ms( t.url() );

        // if we are playing a track now, enqueue, otherwise start now!
        if (m_mediaObject->currentSource().url().isValid()) {
            qDebug() << "enqueuing " << t;
            m_mediaObject->enqueue( ms );
        } else {
            qDebug() << "starting " << t;
            m_mediaObject->setCurrentSource( ms );
            m_mediaObject->play();
        }
        break;
    }
}


// onPhononCurrentSourceChanged happens always (even if the source is
// unplayable), so we use it to update our now playing track.
void
Radio::onPhononCurrentSourceChanged( const Phonon::MediaSource& )
{
    MutableTrack( m_track ).stamp();
    if (m_mediaObject->state() == Phonon::PlayingState) {
        emit trackSpooled( m_track );
    } else {
        changeState( Buffering );
        emit trackSpooled( m_track );
    }
}


void
Radio::changeState( Radio::State const newstate )
{
	State const oldstate = m_state;

    if (oldstate == newstate) 
        return;

    qDebug().nospace() << newstate << " (was " << oldstate << ')';
     
    m_state = newstate; // always assign state properties before you tell other
                        // objects about it
    
	switch (newstate)
	{
        case TuningIn:
            qDebug() << "Tuning to:" << m_station;
            emit tuningIn( m_station );
            break;
            
        case Buffering:
            break;
		case Playing:
            break;

		case Stopped:
            emit stopped();
            break;
	}
}


void
Radio::setStationNameIfCurrentlyBlank( const QString& s )
{
    if (m_station.title().isEmpty())
    {
        m_station.setTitle( s );
        emit tuningIn( m_station );
    }
}

void
Radio::onBuffering( int percent_filled )
{
    Q_UNUSED(percent_filled);
}

void
Radio::onMutedChanged(bool muted)
{
    Q_UNUSED(muted);
}

void
Radio::onOutputDeviceChanged(const Phonon::AudioOutputDevice& newDevice)
{
    qDebug() << "name: " << newDevice.name() << " description: " << newDevice.description();
}

void
Radio::onVolumeChanged(qreal vol)
{
    Q_UNUSED(vol)
}

void
Radio::onFinished()
{
    // the play queue has come to a natural end
    qDebug() << ".";
}


// returns true on successful initialisation
bool 
Radio::initRadio()
{
	Phonon::AudioOutput* audioOutput = new Phonon::AudioOutput( Phonon::MusicCategory, this );
//	audioOutput->setVolume( QSettings().value( "Volume", 80 ).toUInt() );

    qDebug() << audioOutput->name();
    qDebug() << audioOutput->outputDevice().description();
    qDebug() << audioOutput->outputDevice().name();
    qDebug() << (audioOutput->isMuted() ? "muted,": "not-muted,") <<
                (audioOutput->isValid() ? "valid,": "not-valid,") <<
                audioOutput->volumeDecibel() << "db " <<
                audioOutput->volume();
    foreach (QByteArray a, audioOutput->outputDevice().propertyNames()) {
        qDebug() << a << ":" << audioOutput->outputDevice().property(a);
    }

    QString audioOutputDeviceName = "";//TODO moose::Settings().audioOutputDeviceName();
    if (audioOutputDeviceName.size())
    {
        foreach (Phonon::AudioOutputDevice d, Phonon::BackendCapabilities::availableAudioOutputDevices())
            if (d.name() == audioOutputDeviceName) {
                audioOutput->setOutputDevice( d );
                break;
            }
    }

    Phonon::MediaObject* mediaObject = new Phonon::MediaObject;
    m_path = Phonon::createPath( mediaObject, audioOutput );
    if (!m_path.isValid()) {
        qDebug() << "Phonon::createPath failed";
// can't delete the mediaObject after a failed Phonon::createPath without a crash in phonon...  (Qt 4.4.3)
// so, we leak it:
//        mediaObject->deleteLater();
        audioOutput->deleteLater();
        return false;
    } 

    mediaObject->setTickInterval( 1000 );
    connect( mediaObject, SIGNAL(stateChanged( Phonon::State, Phonon::State )), SLOT(onPhononStateChanged( Phonon::State, Phonon::State )) );
    connect( mediaObject, SIGNAL(bufferStatus(int)), SLOT(onBuffering(int)));
    connect( mediaObject, SIGNAL(currentSourceChanged( Phonon::MediaSource )), SLOT(onPhononCurrentSourceChanged( Phonon::MediaSource )) );
    connect( mediaObject, SIGNAL(aboutToFinish()), SLOT(phononEnqueue()) ); // this fires when the whole queue is about to finish
    connect( mediaObject, SIGNAL(finished()), SLOT(onFinished()));
    connect( mediaObject, SIGNAL(tick(qint64)), SIGNAL(tick(qint64)));
    connect( audioOutput, SIGNAL(mutedChanged(bool)), SLOT(onMutedChanged(bool)));
    connect( audioOutput, SIGNAL(outputDeviceChanged(Phonon::AudioOutputDevice)), SLOT(onOutputDeviceChanged(Phonon::AudioOutputDevice)));
    connect( audioOutput, SIGNAL(volumeChanged(qreal)), SLOT(onVolumeChanged(qreal)));

    m_audioOutput = audioOutput;
    m_mediaObject = mediaObject;
    return true;
}

void
Radio::deInitRadio()
{
    // try to deleteLater and phonon crashes. poo.
    // leak em...  :(
    m_audioOutput = 0;
    m_mediaObject = 0;
}
