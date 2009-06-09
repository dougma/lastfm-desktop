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
#ifndef LEGACY_PLAYER_LISTENER_H
#define LEGACY_PLAYER_LISTENER_H

#include "lib/DllExportMacro.h"
#include "../PlayerConnection.h"
#include <QTcpServer>
#include <QMap>
class PlayerConnection;


/** listens to external clients via a TcpSocket and notifies a receiver to their
  * commands */
class LISTENER_DLLEXPORT LegacyPlayerListener : public QTcpServer
{
    Q_OBJECT

public:
    LegacyPlayerListener( QObject* parent );
    
    static uint port() { return 33367; }
    
signals:
    void newConnection( class PlayerConnection* );
    
private slots:
    void onNewConnection();
    void onDataReady();

private:
    /** handles the TERM command as conecerning this class */
    void term( QTcpSocket* );

    QMap<QString, PlayerConnection*> m_connections;
};

#endif
