#include "RadioController.h"
#include "Tuner.h"
#include "AudioPlaybackEngine.h"


RadioController::RadioController()
{
    m_audio = new AudioPlaybackEngine( this );
    
    connect( m_audio, SIGNAL(trackStarted(const Track&)), SIGNAL(trackStarted(const Track&)) );
    connect( m_audio, SIGNAL(playbackEnded()), SIGNAL(playbackEnded()) );
    connect( m_audio, SIGNAL(buffering()), SIGNAL(buffering()) );
    connect( m_audio, SIGNAL(finishedBuffering()), SIGNAL(finishedBuffering()) );
    
    connect( m_audio, SIGNAL(queueStarved()), SLOT(onQueueStarved()) );
}


void
RadioController::play( const RadioStation& s )
{
    Tuner t( s );
	emit tuningStateChanged( true );
    QList<Track> tracks = t.fetchNextPlaylist();
	m_audio->clearQueue();
    m_audio->queue( tracks );
	m_audio->skip();
	emit tuningStateChanged( false );
	m_currentStation = t.stationName();
	emit newStationTuned( m_currentStation );
    m_audio->play();
}


void
RadioController::onQueueStarved()
{
    Tuner t;
    QList<Track> tracks = t.fetchNextPlaylist();
	if( t.stationName() != m_currentStation )
	{
		m_currentStation = t.stationName();
		emit( newStationTuned( m_currentStation ));
	}
			 
    m_audio->queue( tracks );
}


void
RadioController::stop()
{
    m_audio->stop();
}


void 
RadioController::skip()
{
    m_audio->skip();
}
