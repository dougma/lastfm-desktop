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

#ifndef PLAYER_LISTENER_H
#define PLAYER_LISTENER_H

#include "common/HideStupidWarnings.h"
#include "PlayerConnection.h"
#include <QLocalServer>
#include <QMap>
#include "lib/DllExportMacro.h"
#include <stdexcept>


/** listens to external clients via a TcpSocket and notifies a receiver to their
  * commands */
class LISTENER_DLLEXPORT PlayerListener : public QLocalServer
{
    Q_OBJECT

public:
    PlayerListener( QObject* parent = 0 ) throw( std::runtime_error );
    
signals:
    void newConnection( class PlayerConnection* );
    void bootstrapCompleted( const QString& playerId );

private slots:
    void onNewConnection();
    void onDataReady();

private:    
    QMap<QString, PlayerConnection*> m_connections;
};


#endif
