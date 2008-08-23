#include "App.h"
#include "MediaPlayerIndicator.h"
#include "PlayerEvent.h"
#include "Settings.h"
#include "lib/types/Track.h"
#include <QVariant>
#include <QLayout>
#include <QLabel>
#include <QApplication>


MediaPlayerIndicator::MediaPlayerIndicator()
{
    setLayout( new QHBoxLayout );
	layout()->setMargin( 0 );
	layout()->addWidget( m_nowPlayingIndicator = new QLabel( "<b>" + The::settings().username() ) );
	static_cast<QBoxLayout*>(layout())->addStretch();
	layout()->addWidget( m_playerDescription = new QLabel );
    connect( qApp, SIGNAL(event(int, QVariant)), SLOT(onAppEvent( int, QVariant )) );

	m_playerDescription->setAttribute( Qt::WA_MacMiniSize );
	m_nowPlayingIndicator->setAttribute( Qt::WA_MacMiniSize );
	
	// prevent the text length resizing the window!
	m_nowPlayingIndicator->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	
#ifdef Q_WS_MAC
	QPalette p( Qt::white, Qt::black ); //Qt-4.4.1 on mac sucks
	m_nowPlayingIndicator->setPalette( p );
	m_playerDescription->setPalette( p );
	mediaPlayerConnected( "osx" );
#endif
}


void
MediaPlayerIndicator::setTuningIn( const QString& /*title*/ )
{
	m_playerDescription->setText( tr("<b><font color=#343434>tuning in...") );
}


void
MediaPlayerIndicator::onAppEvent( int e, const QVariant& v )
{
    switch( e )
    { 
		// we are guarenteed that the playerid will not change without a disconnected first
        case PlayerEvent::PlayerDisconnected:
#ifndef Q_WS_MAC //FIXME
			m_playerDescription->setText( tr("<b><font color=#343434>no player connection") );
#endif
            break;

		case PlayerEvent::PlaybackPaused:
			m_playerDescription->setText( tr("<b>%1 <font color=#343434>is paused</font>").arg( m_playerName ) );
			break;

		case PlayerEvent::PlaybackEnded:
            m_playerDescription->clear();
            m_playbackCommencedString.clear();
            break;
            
		case PlayerEvent::PlayerConnected:
            mediaPlayerConnected( v.toString() );
			// fall through
		case PlayerEvent::PlaybackStarted:
		case PlayerEvent::PlaybackUnpaused:
			m_playerDescription->setText( m_playbackCommencedString );
			break;
			
		case PlayerEvent::PlayerChangedContext:
			m_playerName = tr("Last.fm radio");
			m_playbackCommencedString = tr( "<b><font color=#343434>%1 on</font> Last.fm", "eg. Recommendation Radio on Last.fm" ).arg( v.toString() );
			// don't set m_playerDescription until we actually start playing

        default:
            return;
    }
}


void
MediaPlayerIndicator::mediaPlayerConnected( const QString& id )
{
    QString playerName = "Unknown";
	
    if( id == "foo" )
        playerName = "Foobar";
	
	else if( id == "osx" || id == "itw" )
		playerName = "iTunes";
	
	else if( id == "wmp" )
		playerName = "Windows Media Player";
	
	else if( id == "wa2" )
		playerName = "Winamp";
	
	else if( id == "ass" )
		playerName = "Last.fm";

	m_playerName = playerName;
	m_playbackCommencedString = tr("<b><font color=#343434>listening to</font> %1").arg( m_playerName );
}
