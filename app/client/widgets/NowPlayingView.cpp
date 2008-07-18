#include "NowPlayingView.h"
#include "PlayerEvent.h"

NowPlayingView::NowPlayingView(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
}

NowPlayingView::~NowPlayingView()
{

}

void
NowPlayingView::setCurrentTrack( const ObservedTrack& track )
{
    ui.artist->setText( track.artist() );
    ui.title->setText( track.title() );
    ui.artwork->setPixmap( track.album().image() );
}

void
NowPlayingView::onAppEvent( int e, const QVariant& v )
{
    switch (e)
    {
    case PlayerEvent::PlaybackStarted:
    case PlayerEvent::TrackChanged:
        setCurrentTrack( v.value<ObservedTrack>() );
        break;

    case PlayerEvent::PlaybackEnded:
        break;
    }

    // progress display timer
    switch (e)
    {
    case PlayerEvent::PlaybackStarted:
    case PlayerEvent::PlaybackUnstalled:
    case PlayerEvent::PlaybackUnpaused:
    {
//         ObservedTrack t = v.value<ObservedTrack>();
//         if (t.isEmpty())
//         {
//             ui.progress->ui.timeToScrobblePoint->setText( ":(" );
//             ui.progress->ui.time->clear();
//             ui.progress->m_progressDisplayTimer->stop();
//             return;
//         }
//         else {
//             ui.progress->determineProgressDisplayGranularity( t.scrobblePoint() );
//             ui.progress->m_progressDisplayTimer->start();
//         }
        break;
    }

    case PlayerEvent::PlaybackStalled:
    case PlayerEvent::PlaybackPaused:
    case PlayerEvent::PlaybackEnded:
//        ui.progress->m_progressDisplayTimer->stop();
        break;
    }
    
    switch (e)
    {
    case PlayerEvent::PlaybackStarted:
    case PlayerEvent::TrackChanged:
    case PlayerEvent::PlaybackEnded:
//         ui.progress->onPlaybackTick( 0 );
//         ui.progress->m_progressDisplayTick = 0;
//         ui.progress->update();
        break;
    }
}