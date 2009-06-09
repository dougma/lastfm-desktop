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
#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <QObject>


// dll/shared lib exports this name
#define PLUGIN_ENTRYPOINT "lastfm_getService"


// PluginHost loads all the plugins, and is used to 
// obtain the plugins offering a particular service
class PluginHost : QObject
{
    Q_OBJECT;

    // lastfm_getService function pointer type
    typedef void *(*P_getService)(const char*);

    QList<P_getService> m_plugins;

public:
    PluginHost(const QString& pluginPath); 

    // get all plugins supporting the serviceName
    template<class ServiceInterface>
    QList<ServiceInterface *> getPlugins(const QString& serviceName)
    {
        QList<ServiceInterface *> result;
        foreach(P_getService getService, m_plugins) {
            ServiceInterface* ip = (ServiceInterface*) getService(serviceName.toUtf8().constData());
            if (ip)
                result << ip;
        }
        return result;
    }
};

#endif