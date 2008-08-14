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
	
#ifdef Q_WS_MAC
	QPalette p( Qt::white, Qt::black ); //Qt-4.4.1 on mac sucks
	m_nowPlayingIndicator->setPalette( p );
	m_playerDescription->setPalette( p );
	m_playerDescription->setText( "<b><font color=#343434>listening to</font> iTunes" );
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
			break;
		}
		case PlayerEvent::PlaybackEnded:
		case PlayerEvent::PlaybackPaused:
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
    m_playerDescription->setText( "<font color=#343434>listening to</font> " + playerName );
}


void
MediaPlayerIndicator::mediaPlayerDisconnected( const QString& id )
{
	if( m_playerDescription->text() == id )
		m_playerDescription->setText( "" );
}