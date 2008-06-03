#include "MainWindow.h"
#include "TrackInfo.h"

MainWindow::MainWindow()
{
    ui.setupUi( this );
}


void
MainWindow::onTrackChanged( const TrackInfo& t )
{
    ui.track->setText( t.toString() );
}
