/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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
