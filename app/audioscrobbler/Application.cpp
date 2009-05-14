/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "Application.h"
#include "MetadataWindow.h"
#include "StopWatch.h"
#include "lib/listener/legacy/LegacyPlayerListener.h"
#include "lib/listener/PlayerConnection.h"
#include "lib/listener/PlayerListener.h"
#include "lib/listener/PlayerMediator.h"
#include <lastfm/Audioscrobbler>
#include <QMenu>
using audioscrobbler::Application;
#define ELLIPSIS QString::fromUtf8("…")
#ifdef Q_WS_X11
    #define AS_TRAY_ICON ":16x16.png"
#else
    #define AS_TRAY_ICON ":22x22.png"
#endif

Application::Application(int& argc, char** argv) : unicorn::Application(argc, argv)
{
////// tray
    tray = new QSystemTrayIcon(this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));
    tray->setIcon(QIcon(AS_TRAY_ICON));
    tray->show();

////// tray menu
    QMenu* menu = new QMenu;
    m_title_action = menu->addAction(tr("Ready"));
    menu->addAction(tr("Love"));
    menu->addAction(tr("Tag")+ELLIPSIS);
    menu->addAction(tr("Share")+ELLIPSIS);
    menu->addSeparator();
    m_submit_scrobbles_toggle = menu->addAction(tr("Submit Scrobbles"));
#ifdef Q_WS_MAC
    menu->addAction(tr("Preferences")+ELLIPSIS);
#else
    menu->addAction(tr("Options")+ELLIPSIS);
#endif
    menu->addSeparator();
    QAction* quit = menu->addAction(tr("Quit Audioscrobbler"));
    
    connect(quit, SIGNAL(triggered()), SLOT(quit()));
        
    m_title_action->setEnabled( false );
    m_submit_scrobbles_toggle->setCheckable(true);
    m_submit_scrobbles_toggle->setChecked(true);
    tray->setContextMenu(menu);

////// scrobbler
    as = new Audioscrobbler("ass");

////// mediator
    mediator = new PlayerMediator(this);
    connect(mediator, SIGNAL(activeConnectionChanged( PlayerConnection* )), SLOT(setConnection( PlayerConnection* )) );
    connect(new LegacyPlayerListener(mediator), SIGNAL(newConnection(PlayerConnection*)), mediator, SLOT(follow(PlayerConnection*)) );

////// listeners
    try{
        PlayerListener* listener = new PlayerListener(mediator);
        connect(listener, SIGNAL(newConnection(PlayerConnection*)), mediator, SLOT(follow(PlayerConnection*)));
    }
    catch(std::runtime_error& e){
        qWarning() << e.what();
        //TODO user visible warning
    }
}

void
Application::onTrayActivated(QSystemTrayIcon::ActivationReason)
{
    if (!mw){
        Track t;
        if (mediator->activeConnection())
            t = mediator->activeConnection()->track();
        
        //testing
        MutableTrack mt(t);
        mt.setTitle("Falling Down");
        mt.setArtist("Scarlett Johansson");
        mt.setDuration(321);
        
        mw = new MetadataWindow(t);
        mw->setAttribute(Qt::WA_DeleteOnClose);
        mw->show();
    }
}

void
Application::setConnection(PlayerConnection*c)
{
    if(connection){
        disconnect(connection, 0, this, 0);
        if(watch)
            connection->setElapsed(watch->elapsed());
    }
    connect(c, SIGNAL(trackStarted(Track, Track)), SLOT(onTrackStarted(Track, Track)));
    connect(c, SIGNAL(paused()), SLOT(onTrackPaused()));
    connect(c, SIGNAL(resumed()), SLOT(onTrackResumed()));
    connect(c, SIGNAL(resumed()), SLOT(onTrackStopped()));
    connection = c;

    if(c->state() == Playing)
        onTrackStarted(c->track(), Track());
}

void
Application::onTrackStarted(const Track& t, const Track& oldtrack)
{
    Q_ASSERT(connection);
    
    //TODO move to playerconnection
    if(t == oldtrack){ 
        qWarning() << "Trying to start the same track as last time, assuming programmer error and doing nothing";
        return;
    }
    if(t.isNull()){
        qWarning() << "Can't start null track!";
        return;
    }
    
    m_title_action->setText( t.toString() + " [" + t.durationString()+']' );

    delete watch;
    as->submit();
    as->nowPlaying(t);
    
    ScrobblePoint timeout(t.duration()/2);
    watch = new StopWatch(timeout, connection->elapsed());
    connect(watch, SIGNAL(timeout()), SLOT(onStopWatchTimedOut()));
}

void
Application::onStopWatchTimedOut()
{
    Q_ASSERT(connection);    
    as->cache(connection->track());
}

void
Application::onPaused()
{
    Q_ASSERT(connection);
    Q_ASSERT(watch);
    if(watch)watch->pause();
}

void
Application::onResumed()
{
    Q_ASSERT(watch);
    Q_ASSERT(connection);    
    Q_ASSERT(connection->state() == Paused);

    if(watch)watch->resume();
}

void
Application::onStopped()
{
    Q_ASSERT(watch);
    Q_ASSERT(connection);
        
    delete watch;
    as->submit();
}
