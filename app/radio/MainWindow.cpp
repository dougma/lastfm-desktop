#include "MainWindow.h"
#include "MainWidget.h"
#include "widgets/MultiStarterWidget.h"
#include "widgets/PlaybackControlsWidget.h"
#include <lastfm/RadioStation>
#include <QLineEdit>
#include <QStatusBar>
#include <QSizeGrip>

MainWindow::MainWindow()
{
    setUnifiedTitleAndToolBarOnMac( true );

    QStatusBar* status = new QStatusBar( this );
    PlaybackControlsWidget* pcw = new PlaybackControlsWidget( status );

    //In order to compensate for the sizer grip on the bottom right
    //of the window, an empty QWidget is added as a spacer.
    QSizeGrip* sg = status->findChild<QSizeGrip *>();
    if( sg ) {
        int gripWidget = sg->sizeHint().width();
        QWidget* w = new QWidget( status );
        w->setFixedWidth( gripWidth );
        status->addWidget( w );
    }

    //Seemingly the only way to get a central widget in a QStatusBar
    //is to add an empty widget either side with a stretch value.
    status->addWidget( new QWidget( status), 1 );
    status->addWidget( pcw );
    status->addWidget( new QWidget( status), 1 );

    setStatusBar( status );

    MainWidget* mw = new MainWidget( this );
    connect(mw, SIGNAL( startRadio( RadioStation )), SIGNAL( startRadio( RadioStation )));
    setCentralWidget( mw );

    finishUi();
}
