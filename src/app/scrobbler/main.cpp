//Copyright header goes here

#include "App.h"
#include "Settings.h"
#include "MainWindow.h"
#include "PlayerListener.h"
#include "version.h"


int main( int argc, char** argv )
{
    // used by some Qt stuff, eg QSettings
    // leave first! As Settings object is created quickly
    QCoreApplication::setApplicationName( "AudioScrobbler" );
    QCoreApplication::setOrganizationName( "Last.fm" );
    QCoreApplication::setOrganizationDomain( "last.fm" );

    Settings settings( VERSION, QCoreApplication::applicationFilePath() );
    Settings::instance = &settings;

    App app( argc, argv );

    MainWindow window;
    window.show();

    CPlayerListener listener;
    window.connect( &listener, SIGNAL(trackChanged( TrackInfo )), SLOT(onTrackChanged( TrackInfo )) );

    return app.exec();
}
