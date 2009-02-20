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
#include "lib/unicorn/widgets/AboutDialog.h"
#include <lastfm/Track>
#include <lastfm/User>
#include <lastfm/WsKeys>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QMenuBar>
#include <QShortcut>
#include <QToolBar>
#include <QVBoxLayout>


MainWindow::MainWindow()
{   
    QAction* showlog;
    QAction* about; 
    
    ui.account = menuBar()->addMenu( Ws::Username );
    ui.profile = ui.account->addAction( tr("Visit &Profile"), this, SLOT(openProfileUrl()) );
    ui.account->addSeparator();
    ui.account->addAction( tr("Log &Out && Quit"), qApp, SLOT(logout()) );
    ui.outputdevice = menuBar()->addMenu( tr("Output Device") );

    QMenu* tools = menuBar()->addMenu( tr("Tools") );
    ui.xspf = tools->addAction( "Resolve XSPF" );
    ui.rescan = tools->addAction( tr("&Scan Music Again") );
    showlog = tools->addAction( tr("Show &Log"), this, SLOT(openLog()) );
    about = menuBar()->addMenu( "Help" )->addAction( "About", this, SLOT(about()) );
    
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

    connect( qApp, SIGNAL(userGotInfo( WsReply* )), SLOT(onUserGotInfo( WsReply* )) );
    
#ifdef __APPLE__
    about->setText( "About " + qApp->applicationName() );
    about->setMenuRole( QAction::AboutRole );
    new QShortcut( QKeySequence(Qt::CTRL+Qt::Key_W), this, SLOT(close()) );
#else
    ui.account->addAction( tr("&Quit"), qApp, SLOT(quit()) );
#endif
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


#include <QDesktopServices>
#include "lib/unicorn/UnicornCoreApplication.h"
void
MainWindow::openLog()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( unicorn::CoreApplication::log().absoluteFilePath() ) );
}


void
MainWindow::about()
{
    (new AboutDialog( this ))->show();
}
