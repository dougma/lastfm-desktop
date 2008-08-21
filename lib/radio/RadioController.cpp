#include "RadioController.h"
#include "Tuner.h"
#include "AudioPlaybackEngine.h"


RadioController::RadioController()
			   : m_tuner( 0 )
{
    m_audio = new AudioPlaybackEngine( this );
    
    connect( m_audio, SIGNAL(trackStarted( Track )), SIGNAL(trackStarted( Track )) );
    connect( m_audio, SIGNAL(playbackEnded()), SIGNAL(playbackEnded()) );
    connect( m_audio, SIGNAL(buffering()), SIGNAL(buffering()) );
    connect( m_audio, SIGNAL(finishedBuffering()), SIGNAL(playbackResumed()) );
}


void
RadioController::play( const RadioStation& station )
{
	qInfo() << "Tuning to:" << station;
	
	m_audio->clearQueue();
	delete m_tuner;
	
	emit tuningIn( station );
    m_tuner = new Tuner( station );
	connect( m_tuner, SIGNAL(stationName( QString )), SIGNAL(tuned( QString )) );
	connect( m_tuner, SIGNAL(tracks( QList<Track> )), SLOT(enqueue( QList<Track> )) );

    connect( m_audio, SIGNAL(queueStarved()), m_tuner, SLOT(fetchFiveMoreTracks()) );
}


void
RadioController::enqueue( const QList<Track>& tracks )
{
    m_audio->queue( tracks );
    m_audio->play();
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
