#include "App.h"
#include "MediaPlayerIndicator.h"
#include "PlayerEvent.h"
#include "lib/types/Track.h"
#include <QVariant>
#include <QLayout>
#include <QLabel>
#include <QApplication>


MediaPlayerIndicator::MediaPlayerIndicator()
{
    setLayout( new QHBoxLayout );
	layout()->setMargin( 0 );
	layout()->addWidget( m_nowPlayingIndicator = new QLabel( "Now playing" ));
	static_cast<QBoxLayout*>(layout())->addStretch();
	layout()->addWidget( m_playerDescription = new QLabel );
    connect( qApp, SIGNAL(event(int, QVariant)), SLOT(onAppEvent( int, QVariant )) );

	m_playerDescription->setPalette( QPalette( Qt::white, Qt::black ) );
	m_playerDescription->setAttribute( Qt::WA_MacMiniSize );
	m_nowPlayingIndicator->setDisabled( true );
	m_nowPlayingIndicator->setAttribute( Qt::WA_MacMiniSize );
	m_nowPlayingIndicator->hide();
	
#ifdef Q_WS_MAC
	m_playerDescription->setText( "iTunes" );
#endif
}


void
MediaPlayerIndicator::onAppEvent( int e, const QVariant& v )
{
    switch( e )
    { 
        case PlayerEvent::PlayerConnected:
            mediaPlayerConnected( v.toString() );
            break;
        case PlayerEvent::PlayerDisconnected:
            mediaPlayerDisconnected( v.toString() );
            break;
		case PlayerEvent::PlaybackStarted:
		case PlayerEvent::PlaybackUnpaused:
		{
			const Track& t = v.value<Track>();
			QString playerId = t.playerId();
			if( m_playerDescription->objectName() != v.toString() )
				mediaPlayerConnected( v.toString() );

			if( m_playerDescription->objectName() == "ass" )
				formatRadioStationString();
			
			m_nowPlayingIndicator->show();
			break;
		}
		case PlayerEvent::PlaybackEnded:
		case PlayerEvent::PlaybackPaused:
			m_nowPlayingIndicator->hide();			
			break;
			
		case PlayerEvent::PlayerChangedContext:
			m_currentContext = v.toString();
			formatRadioStationString();
        default:
            return;
    }
}


void
MediaPlayerIndicator::formatRadioStationString()
{
	m_playerDescription->setText( m_currentContext + " on Last.fm" ); 
}


void
MediaPlayerIndicator::mediaPlayerConnected( const QString& id )
{
    QString playerName = "Unknown";
	
    if( id == "foo" )
        playerName = "Foobar";
	
	else if( id == "osx" ||
			 id == "itw" )
		playerName = "iTunes";
	
	else if( id == "wmp" )
		playerName = "Windows Media Player";
	
	else if( id == "wa2" )
		playerName = "Winamp";
	
	else if( id == "ass" )
		playerName = "Last.fm";
	qDebug() << id;
	
	m_playerDescription->setObjectName( id );
    m_playerDescription->setText( playerName );
}


void
MediaPlayerIndicator::mediaPlayerDisconnected( const QString& id )
{
	if( m_playerDescription->text() == id )
		m_playerDescription->setText( "" );
}