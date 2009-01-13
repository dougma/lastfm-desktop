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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "PluginHost.h"
#include <QLibrary>
#include <QDir>
#include <QThread>
#include <QDebug>


PluginHost::PluginHost(const QString& path)
{
    QDir d( path );
    foreach(QString plugin, d.entryList(QDir::Files)) 
    {
        qDebug() << plugin;
        
        plugin = d.filePath( plugin );
        
        if (!QLibrary::isLibrary( plugin )) continue;

        qDebug() << plugin;        
        
        // it looks like a shared library, so try loading it:
        QLibrary lib( plugin );
        P_getService get = (P_getService) lib.resolve( PLUGIN_ENTRYPOINT );
        
        if (get)
            m_plugins << get;
            // and the lib will remain loaded until app terminates. ok?
        else
        {
            qWarning() << lib.errorString();
            lib.unload();
        }
    }
    
    qDebug() << "Found" << m_plugins.count() << "plugins in:" << path;
}


