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
#include "lib/lastfm/core/CoreUrl.h"
#include "lib/lastfm/types/User.h"
#include "lib/lastfm/ws/WsKeys.h"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QMenuBar>


MainWindow::MainWindow()
{
    setWindowTitle( tr("Last.fm Boffin") );
    
    ui.account = menuBar()->addMenu( Ws::Username );
    ui.profile = ui.account->addAction( tr("Visit Profile"), this, SLOT(openProfileUrl()) );
    ui.account->addSeparator();
    ui.account->addAction( tr("Log Out && Quit"), qApp, SLOT(logout()) );
#ifndef Q_OS_MAC
    ui.account->addAction( tr("Quit"), qApp, SLOT(quit()) );
#endif
    
    connect( qApp, SIGNAL(userGotInfo( WsReply* )), SLOT(onUserGotInfo( WsReply* )) );
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
