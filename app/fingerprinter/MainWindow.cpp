/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/

#include "MainWindow.h"
#include "Settings.h"
#include "version.h"
#include "lib/unicorn/AboutDialog.h"
#include <QUrl>
#include <QDebug>


MainWindow::MainWindow()
          : QMainWindow()
{
    ui.setupUi( this );

    setWindowTitle( tr( "Last.fm Fingerprinter" ) );
    setStatusBar( 0 );
    setAcceptDrops(true);
    
    resize( FingerprinterSettings::instance().size() );
    move( FingerprinterSettings::instance().position() );
    
    #ifndef Q_WS_MAC
        QMenu* fileMenu = new QMenu( tr( "&File" ), this );
        QAction* exitAction = fileMenu->addAction( tr( "E&xit" ), this, SLOT( close() ) );
    #ifdef Q_WS_X11
        exitAction->setShortcut( tr( "CTRL+Q" ) );
        exitAction->setText( tr("&Quit") );
    #endif
        menuBar()->insertMenu( menuBar()->actions().at( 0 ), fileMenu );
    #endif
    
    // Help shortcut
    #ifdef Q_WS_MAC
        ui.actionFAQ->setShortcut( tr( "Ctrl+?" ) );
    #else
        ui.actionFAQ->setShortcut( tr( "F1") );
    #endif
    
    QString info = tr(
        "<ol>"
        "<li>Select the folders containing the tracks that you wish to fingerprint."
        "<li>Press the Fingerprint button at the bottom to start."
        "</ol>"
        "This operation will not modify your files in any way, it will only analyse them "
        "and post a unique ID (fingerprint) to Last.fm."
        "<p>Please note that you can stop at any time; the app will remember how far it got, "
        "and automatically resume from that point the next time." );
    ui.informationLabel->setText( info );

    connect( ui.actionFAQ, SIGNAL( triggered() ),
             this, SLOT( showFAQ() ) );
             
    connect( ui.actionAbout, SIGNAL( triggered() ),
             this, SLOT( aboutDialog() ) );
    
    connect( ui.actionLogout, SIGNAL( triggered() ),
             this, SIGNAL( logout() ) );
             
    connect( ui.fingerprintButton, SIGNAL( clicked() ),
             this, SLOT( fingerprintButtonClicked() ) );
    
}


void
MainWindow::start()
{
    show();
    ui.dirTree->init();
}


void
MainWindow::closeEvent( QCloseEvent *event )
{
    emit wantsToClose( event );
}


void
MainWindow::fingerprintButtonClicked()
{
    emit startFingerprinting( ui.dirTree->getInclusions() );
}


void
MainWindow::dragEnterEvent (QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("text/plain"))
        event->acceptProposedAction();
}


void
MainWindow::dropEvent (QDropEvent* event)
{
    QStringList files = QUrl::fromEncoded(event->mimeData()->text().toAscii()).toString().split( "\n", QString::SkipEmptyParts );
    
    foreach( QString file, files )
    {
        if (!file.startsWith( "file://" ))
            continue;
            
        file.remove(file.size()-1,1);
        file.remove(0,7);
        QFileInfo fileinfo( file );
        
        QString path = fileinfo.absoluteFilePath();
        if ( !fileinfo.isDir() )
            path = fileinfo.absolutePath();
        
        ui.dirTree->checkPath( path, Qt::Checked );
        ui.dirTree->expandPath( path );
        ui.dirTree->showPath( path, QAbstractItemView::PositionAtCenter );
    }
}


void
MainWindow::showFAQ()
{
    new BrowserThread( "http://www.last.fm/help/faq/?category=Fingerprinting" );
}


void
MainWindow::aboutDialog()
{
    AboutDialog( VERSION, this ).exec();
}
