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


Radio::Radio( Phonon::AudioOutput* output )
			   : m_tuner( 0 ),
			     m_audioOutput( output )
{
    m_mediaObject = new Phonon::MediaObject( this );
    m_mediaObject->setTickInterval( 1000 );
    connect( m_mediaObject, SIGNAL(stateChanged( Phonon::State, Phonon::State )), SLOT(onPhononStateChanged( Phonon::State, Phonon::State )) );
	connect( m_mediaObject, SIGNAL(aboutToFinish()), SLOT(onPhononAboutToFinish()) );
	connect( m_mediaObject, SIGNAL(bufferStatus( int )), SIGNAL(buffering( int )) );
    Phonon::createPath( m_mediaObject, m_audioOutput );
}


void
Radio::play( const RadioStation& station )
{
	qInfo() << "Tuning to:" << station;
	
	stop();
	delete m_tuner;

	emit tuningIn( station );
    m_tuner = new Tuner( station );
	connect( m_tuner, SIGNAL(stationName( QString )), SIGNAL(tuned( QString )) );
	connect( m_tuner, SIGNAL(tracks( QList<Track> )), SLOT(enqueue( QList<Track> )) );
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
		start();
	}
	
	virtual void run()
	{
		m_o->enqueue( m_urls );
		m_o->play();
	}
}; 


void
Radio::enqueue( const QList<Track>& tracks )
{
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


void
Radio::skip()
{
	QList<Phonon::MediaSource> q = m_mediaObject->queue();
	if (q.size())
	{
		Phonon::MediaSource source = q.front();
		q.pop_front();
		m_mediaObject->setQueue( q );
		m_mediaObject->setCurrentSource( source );
		m_mediaObject->play();
	}
	//else we already asked for more tracks, so wait I guess
}


void
Radio::stop()
{
	m_mediaObject->stop();
	m_mediaObject->clearQueue();
	m_mediaObject->setCurrentSource( QUrl() );
}



void
Radio::pause()
{
	m_mediaObject->pause();
}


void
Radio::unpause()
{
	m_mediaObject->play();
}



namespace Phonon
{
	static inline QString debug( Phonon::State state )
	{
	#define _( x ) x: return #x;
		switch (state)
		{
			case _(Phonon::LoadingState)
			case _(Phonon::StoppedState)
			case _(Phonon::PlayingState)
			case _(Phonon::BufferingState)
			case _(Phonon::PausedState)
			case _(Phonon::ErrorState)
		}
		return "Unknown";
	#undef _
	}
	
	void debug( Phonon::State newstate, Phonon::State oldstate )
	{
		qDebug() << "Now is" << debug( newstate ) << "but was" << debug( oldstate );
	}
}


void
Radio::onPhononStateChanged( Phonon::State newstate, Phonon::State oldstate )
{
	Phonon::debug( newstate, oldstate );
	
	QUrl const url = m_mediaObject->currentSource().url();
	
    switch (newstate)
    {
        case Phonon::ErrorState:
			if (m_mediaObject->errorType() == Phonon::FatalError)
			{
				qCritical() << m_mediaObject->errorString();
				emit playbackEnded();
			}
			else 
				skip();
            break;
			
        case Phonon::StoppedState:
			emit playbackEnded();
            break;
			
        case Phonon::BufferingState:
            emit buffering( 0 );
            break;
			
		case Phonon::PausedState:
			// if the phono play queue runs out we get this for some reason
			emit playbackEnded();
			break;
			
		case Phonon::LoadingState:
			emit preparing( m_queue[url] );
			break;
			
        case Phonon::PlayingState:
			switch (oldstate)
			{
				default:
				{
					qDebug() << "PlaybackStarted:" << url;
					
					Track t = m_queue.take( url );
					if( t.isEmpty() )
						break;
					
					MutableTrack( t ).stamp();
					
					emit trackStarted( t );
					
					if (m_queue.isEmpty() && m_tuner)
						m_tuner->fetchFiveMoreTracks();
				}
				break;

				case Phonon::PausedState:
				case Phonon::BufferingState:
					emit playbackResumed();
					break;
			}
			break;
    }
}


void
Radio::onPhononAboutToFinish()
{
//	emit playbackEnded();
}
