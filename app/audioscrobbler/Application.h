/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#include <lastfm/global.h>
#include "lib/unicorn/UnicornApplication.h"
#include <QPointer>
#include <QSystemTrayIcon>

class PlayerMediator;
class PlayerConnection;
class MetadataWindow;
class StopWatch;
class QAction;

namespace audioscrobbler
{
    class Application : public unicorn::Application
    {
        Q_OBJECT
        
        // we delete these so QPointers
        QPointer<QSystemTrayIcon> tray;
        QPointer<Audioscrobbler> as;
        QPointer<PlayerMediator> mediator;
        QPointer<PlayerConnection> connection;
        QPointer<StopWatch> watch;
        QPointer<MetadataWindow> mw;
        
        QAction* m_submit_scrobbles_toggle;
        QAction* m_title_action;
        QAction* m_love_action;
        QAction* m_tag_action;
        QAction* m_share_action;
        
    public:
        Application(int& argc, char** argv);
        
        QAction* loveAction() const{ return m_love_action; }
        QAction* tagAction() const{ return m_tag_action; }
        QAction* shareAction() const{ return m_share_action; }
        StopWatch* stopWatch() const;
        PlayerConnection* currentConnection() const;
        
    signals:
        void trackStarted( const Track&, const Track& );

    private slots:
        void onTrayActivated(QSystemTrayIcon::ActivationReason);
        void onStopWatchTimedOut();
        void setConnection(PlayerConnection*);
        
        void onLoveTriggered();
        void onTagTriggered();
        void onShareTriggered();
        
        void onTrackStarted(const Track&, const Track&);
        void onPaused();
        void onResumed();
        void onStopped();
    };
}
