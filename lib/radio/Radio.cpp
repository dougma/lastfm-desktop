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

#include "Radio.h"
#include "Tuner.h"
#include "phonon"

static bool eat_stop = false;


Radio::Radio( Phonon::AudioOutput* output )
			   : m_tuner( 0 ),
			     m_audioOutput( output ),
			     m_state( Radio::Stopped )
{
    m_mediaObject = new Phonon::MediaObject( this );
    m_mediaObject->setTickInterval( 1000 );
    connect( m_mediaObject, SIGNAL(stateChanged( Phonon::State, Phonon::State )), SLOT(onPhononStateChanged( Phonon::State, Phonon::State )) );
    Phonon::createPath( m_mediaObject, m_audioOutput );
}


void
Radio::play( const RadioStation& station )
{
	m_station = station;
	qDebug() << "Tuning to:" << station;
	
	m_mediaObject->blockSignals( true );
	stop(); //don't emit stateChanged to Stopped
	m_mediaObject->blockSignals( false );
	delete m_tuner;

	changeState( TuningIn );
    m_tuner = new Tuner( station );
	connect( m_tuner, SIGNAL(stationName( QString )), SLOT(setStationNameIfCurrentlyBlank( QString )) );
	connect( m_tuner, SIGNAL(tracks( QList<Track> )), SLOT(enqueue( QList<Track> )) );
	connect( m_tuner, SIGNAL(error( Ws::Error )), SLOT(onTunerError( Ws::Error )) );
}


#include <QThread>
/** my gosh, this is dangerous, FIXME!
  * done because on mac Phonon would hang in enqueu for a few seconds, which
  * sucked */
class EnqueueThread : public QThread
{
	QList<QUrl> m_urls;
	Phonon::MediaObject* m_o;
	
public:
	EnqueueThread( const QList<QUrl>& urls, Phonon::MediaObject* object )
	{
		m_o = object;
		m_urls = urls;
		connect( this, SIGNAL(finished()), SLOT(deleteLater()) );
		start();
	}
	
	virtual void run()
	{
		eat_stop = true; //hack because threading it seems to break it a little
		m_o->enqueue( m_urls );
		m_o->play();
		eat_stop = false;
	}
}; 


void
Radio::enqueue( const QList<Track>& tracks )
{
	if (tracks.isEmpty()) {
		qWarning() << "Received blank playlist, Last.fm is b0rked";
		stop();
		return;
	}
	
    QList<QUrl> urls;
    foreach (const Track& t, tracks)
    {
        urls += t.url();
        m_queue[t.url()] = t;
    }
	
#ifdef Q_WS_MAC
	new EnqueueThread( urls, m_mediaObject );
#else
	m_mediaObject->enqueue( urls );
	m_mediaObject->play();
#endif
}


class SkipThread : public QThread
{
	Phonon::MediaObject* m_mediaObject;
	
public:
	SkipThread( Phonon::MediaObject* object )
	{
		m_mediaObject = object;
		connect( this, SIGNAL(finished()), SLOT(deleteLater()) );
		start();
	}
	
	virtual void run()
	{
		QList<Phonon::MediaSource> q = m_mediaObject->queue();
		Phonon::MediaSource source = q.front();
		q.pop_front();
	
		eat_stop = true; //hack because threading it seems to break it a little
		m_mediaObject->setCurrentSource( source );
		m_mediaObject->setQueue( q );
		m_mediaObject->play();
		eat_stop = false;
	}
};


void
Radio::skip()
{
	QList<Phonon::MediaSource> q = m_mediaObject->queue();
	if (q.size())
	{
#ifdef Q_WS_MAC
		new SkipThread( m_mediaObject );
#else
		Phonon::MediaSource source = q.front();
		q.pop_front();
		m_mediaObject->setQueue( q );
		m_mediaObject->setCurrentSource( source );
		m_mediaObject->play();
#endif
	}
	else
	{
		// we are still waiting for a playlist to come back from the tuner
		
		m_mediaObject->blockSignals( true ); //dont' tell outside world that we stopped
		stop();
		m_mediaObject->blockSignals( false );
		changeState( TuningIn );
	}
}


void
Radio::onTunerError( Ws::Error e )
{
	if (m_mediaObject->state() == Phonon::StoppedState)
		// we left the state set to TuningIn, see skip()
		changeState( Stopped );

	emit error( e );
}


void
Radio::stop()
{
	m_mediaObject->stop();
	m_mediaObject->clearQueue();
	m_mediaObject->setCurrentSource( QUrl() );
	
	if (m_state == TuningIn)
		changeState( Stopped );
}


void
Radio::onPhononStateChanged( Phonon::State newstate, Phonon::State oldstate )
{
	if (eat_stop && newstate == Phonon::StoppedState) {
		qDebug() << "Eating Phonon's naughty stop";
		return;
	}
	
	qDebug() << newstate << "but was:" << oldstate;
	
    switch (newstate)
    {
        case Phonon::ErrorState:
			if (m_mediaObject->errorType() == Phonon::FatalError)
			{
				qCritical() << m_mediaObject->errorString();
				stop(); // just in case phonon is broken
			}
			else
				skip();
            break;
			
		case Phonon::PausedState:
			// if the phono play queue runs out we get this for some reason
        case Phonon::StoppedState:
			changeState( Stopped );
            break;
			
        case Phonon::BufferingState:
            changeState( Rebuffering );
            break;

		case Phonon::LoadingState:
		case Phonon::PlayingState:
		{
			QUrl const url = m_mediaObject->currentSource().url();
			Track t = m_queue.take( url );
			if (t.isNull())
			{
				qWarning() << "Some bug as got null track for" << url;
				return;
			}
			
			MutableTrack( t ).stamp();
			m_track = t;
			
			if (m_queue.isEmpty() && m_tuner)
				;//m_tuner->fetchFiveMoreTracks();
			
			changeState( Playing );
			break;
		}
    }
}


void
Radio::changeState( Radio::State newstate )
{
	switch (newstate)
	{
		case Playing:
			break;

		case Stopped:
			m_station = RadioStation();
			m_track = Track();
			// fall through
			
		case TuningIn:
		case Rebuffering:
			if (m_state == newstate)
				return;
			break;	
	}
	
	State oldstate = m_state;
	m_state = newstate; // always assign state properties before you claim a new state
	
	emit stateChanged( oldstate, newstate );
	
	if (m_state == Playing)
		emit trackStarted( m_track );
}
