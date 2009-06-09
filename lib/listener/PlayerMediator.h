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
#include "lib/DllExportMacro.h"
#include "PlayerConnection.h"
#include <QList>
class PlayerConnection;


/** Usage, add PlayerConnections, seek() for the active one, when active 
  * connection is available the newActiveConnection() signal will be emitted
  * the ActionConnection will then stop seeking until you next call seek() */
class LISTENER_DLLEXPORT PlayerMediator : public QObject
{
    Q_OBJECT

    QList<PlayerConnection*> m_connections;

protected:
    PlayerConnection* m_active;
    virtual bool assess( PlayerConnection* );

public:
    PlayerMediator( QObject* parent );
        
    PlayerConnection* activeConnection() const { return m_active; }

public slots:
    void follow( PlayerConnection* );
    
signals:
    void activeConnectionChanged( PlayerConnection* );

private slots:
    void onActivity();
    void onDestroyed();
};
