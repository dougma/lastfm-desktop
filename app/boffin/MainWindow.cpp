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
#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QComboBox>
#include <QStatusBar>
#include <lastfm/Track>
#include "PlaydarHostsModel.h"
#include "PlaydarStatus.h"

MainWindow::MainWindow()
{   
    ui.outputdevice = menuBar()->addMenu( tr("Output Device") );

    QMenu* tools = menuBar()->addMenu( tr("Tools") );
    ui.xspf = tools->addAction( "Resolve XSPF" );
    ui.rescan = tools->addAction( tr("&Scan Music Again") );
    tools->addAction( tr("Show &Log"), this, SLOT(openLog()) );
    ui.wordle = tools->addAction( tr("Wordlize"));

    QToolBar* toolbar = new QToolBar;
    toolbar->setIconSize( QSize( 41, 41 ) );
    ui.play = toolbar->addAction( tr("Play") );
    ui.play->setIcon( QPixmap(":/play.png") );
    ui.pause = toolbar->addAction( tr("Pause") );
    ui.pause->setIcon( QPixmap(":/pause.png") );
    ui.pause->setCheckable( true );
    ui.skip = toolbar->addAction( tr("Skip") );
    ui.skip->setIcon( QPixmap(":/skip.png") );
    addToolBar( toolbar );

    setWindowTitle( Track() );
    setUnifiedTitleAndToolBarOnMac( true );
    resize( 750, 550 );
    
    QStatusBar* status = new QStatusBar();
    ui.playdarHosts = new QComboBox();
    ui.playdarStatus = new QLabel();
    status->addPermanentWidget(ui.playdarStatus);
    status->addPermanentWidget(ui.playdarHosts);
    setStatusBar(status);

    finishUi();
}


void
MainWindow::setWindowTitle( const Track& t )
{
    if (t.isNull())
        QMainWindow::setWindowTitle( tr("Last.fm Boffin") );
    else
        QMainWindow::setWindowTitle( t.toString() );
}
