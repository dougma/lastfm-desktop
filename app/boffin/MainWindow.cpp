/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "MainWindow.h"
#include "lib/lastfm/core/UrlBuilder.h"
#include "lib/lastfm/types/Track.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsKeys.h"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>


MainWindow::MainWindow()
{    
    ui.account = menuBar()->addMenu( Ws::Username );
    ui.profile = ui.account->addAction( tr("Visit &Profile"), this, SLOT(openProfileUrl()) );
    ui.account->addSeparator();
    ui.account->addAction( tr("Log &Out && Quit"), qApp, SLOT(logout()) );
#ifndef Q_OS_MAC
    ui.account->addAction( tr("&Quit"), qApp, SLOT(quit()) );
#endif
    ui.outputdevice = menuBar()->addMenu( tr("Output Device") );

    ui.rescan = menuBar()->addMenu( tr("Tools") )->addAction( tr("&Scan Music Again") );

    setUnifiedTitleAndToolBarOnMac( true );
    QToolBar* toolbar;
    addToolBar( toolbar = new QToolBar );
    ui.play = toolbar->addAction( tr("Play") );
    ui.pause = toolbar->addAction( tr("Pause") );
    ui.skip = toolbar->addAction( tr("Skip") );
    ui.play->setCheckable( true );

    toolbar->setIconSize( QSize( 41, 41 ) );

//    toolbar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    QIcon playicon( QPixmap(":/play.png") );
    playicon.addPixmap( QPixmap(":/stop.png"), QIcon::Normal, QIcon::On );

    ui.play->setIcon( playicon );
    ui.pause->setIcon( QPixmap(":/pause.png") );
    ui.skip->setIcon( QPixmap(":/skip.png") );
            
    setWindowTitle( Track() );

    connect( qApp, SIGNAL(userGotInfo( WsReply* )), SLOT(onUserGotInfo( WsReply* )) );
    
    resize( 750, 550 );
}


void
MainWindow::onUserGotInfo( WsReply* reply )
{
    QString const text = AuthenticatedUser::getInfoString( reply );

    if (text.size())
    {
        QAction* act = ui.account->addAction( text );
        act->setEnabled( false );
        ui.account->insertAction( ui.profile, act );
    }
}


void
MainWindow::openProfileUrl()
{
    QDesktopServices::openUrl( AuthenticatedUser().www() );
}


void
MainWindow::setWindowTitle( const Track& t )
{
    if (t.isNull())
        QMainWindow::setWindowTitle( tr("Last.fm Boffin") );
    else
        QMainWindow::setWindowTitle( t.toString() );
}
