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

#include <QtGlobal>
#ifdef QT_DBUS_LIB

#include "lib/DllExportMacro.h"
#include <QMap>
#include <QObject>
class PlayerConnection;


/** listens for dbus connections */
class LISTENER_DLLEXPORT DBusListener : public QObject
{
    Q_OBJECT

public:
    DBusListener( QObject* parent );
    
signals:
    void newConnection( class PlayerConnection* );
    
private slots:
    void start( const QString& id );
    void pause( const QString& id );
    void resume( const QString& id );
    void stop( const QString& id );

private:
    QMap<QString, PlayerConnection*> m_connections;
};

#endif
