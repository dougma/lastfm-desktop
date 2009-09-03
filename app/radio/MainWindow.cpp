/*
   Copyright 2005-2009 Last.fm Ltd. 

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MainWindow.h"
#include "MainWidget.h"
#include "widgets/MessageBar.h"
#include "widgets/MultiStarterWidget.h"
#include "widgets/PlaybackControlsWidget.h"
#include <lastfm/RadioStation>
#include <QLineEdit>
#include <QSizeGrip>
#include <QStatusBar>
#include <QStackedLayout>

MainWindow::MainWindow()
{
    setUnifiedTitleAndToolBarOnMac( true );

    QStatusBar* status = new QStatusBar( this );
    PlaybackControlsWidget* pcw = new PlaybackControlsWidget( status );

    //FIXME: this code is duplicated in the audioscrobbler app too
    //In order to compensate for the sizer grip on the bottom right
    //of the window, an empty QWidget is added as a spacer.
    QSizeGrip* sg = status->findChild<QSizeGrip *>();
    if( sg ) {
        int gripWidth = sg->sizeHint().width();
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

    MainWidget* mw;

    QWidget* w = new QWidget();
    // a stacked layout so the messagebar can intrude over the top
    QStackedLayout* sl = new QStackedLayout(w);
    sl->setStackingMode(QStackedLayout::StackAll);
    sl->addWidget(m_messageBar = new MessageBar());
    sl->addWidget(mw = new MainWidget());
    sl->setCurrentWidget(m_messageBar);     // make sure it's on top.
    connect(mw, SIGNAL(startRadio(RadioStation)), SIGNAL(startRadio(RadioStation)));

    AuthenticatedUser user;
    connect(user.getFriends(), SIGNAL(finished()), mw, SLOT(onUserGotFriends()));
    connect(user.getTopTags(), SIGNAL(finished()), mw, SLOT(onUserGotTopTags()));
    connect(user.getPlaylists(), SIGNAL(finished()), mw, SLOT(onUserGotPlaylists()));
    connect(user.getRecentStations(), SIGNAL(finished()), mw, SLOT(onUserGotRecentStations()));

    setCentralWidget( w );

    finishUi();
}

void
MainWindow::onRadioError(int code, const QVariant& data)
{
    switch (code)
    {
        case lastfm::ws::NotEnoughContent:
            m_messageBar->show(tr("Sorry, there is no more content available for this radio station."));
            break;
            
        case lastfm::ws::MalformedResponse:
        case lastfm::ws::TryAgainLater:
            m_messageBar->show(tr("Sorry, there was a radio error at Last.fm. Please try again later."));
            break;
            
        case lastfm::ws::SubscribersOnly:
            m_messageBar->show(tr("Sorry, this station is only available to Last.fm subscribers. "
                           "<A href='http://www.last.fm/subscribe'>Sign up</a>."));
            break;

        case lastfm::ws::UnknownError:
            // string contains Phonon generated user readable error message
            m_messageBar->show(data.toString());
            break;

        default:
            m_messageBar->show("Sorry, an unexpected error occurred.");
            break;
    }
}

