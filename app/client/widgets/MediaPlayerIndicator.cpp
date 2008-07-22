#include "MediaPlayerIndicator.h"
#include "PlayerEvent.h"
#include <QVariant>
#include <QLayout>
#include <QLabel>
#include <QApplication>


MediaPlayerIndicator::MediaPlayerIndicator(QWidget *parent)
    : QWidget(parent)
{
    setLayout( new QHBoxLayout( this ) );
    connect( qApp, SIGNAL(event(int, QVariant)), SLOT( onAppEvent( int, QVariant )) );
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

    QLabel* l = new QLabel( playerName, this );
    l->setObjectName( id );
    layout()->addWidget( l );
}


void
MediaPlayerIndicator::mediaPlayerDisconnected( const QString& id )
{
    QLabel* l;
    if( !(l = findChild<QLabel *>( id )) )
        return;

    delete( l );
}