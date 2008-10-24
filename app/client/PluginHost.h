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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <QObject>


// dll/shared lib exports this name
#define PLUGIN_ENTRYPOINT "lastfm_getService"


// lastfm_getService function pointer:
typedef void *(*P_getService)(const char*);


// PluginHost loads all the plugins, and is used to 
// obtain the plugins offering a particular service
class PluginHost : QObject
{
    Q_OBJECT;

    // function pointers
    typedef void *(*P_getService)(const char*);

    QList<class Plugin *> m_plugins;

public:
    PluginHost(const QString& pluginPath); 

    // get all plugins supporting the serviceName
    template<class ServiceInterface>
    QList<ServiceInterface *> getPlugins(const QString& serviceName)
    {
        QList<ServiceInterface *> result;
        foreach(Plugin *plugin, m_plugins) {
            ServiceInterface *ip = 
                plugin->getService<ServiceInterface>(
                    serviceName.toUtf8().constData());
            if (ip)
                result << ip;
        }
        return result;
    }
};


// Plugin encapsulates the plugin entry point.
// We may extend this class to give each plugin a thread for extra 
// plugin isolation.
class Plugin : public QObject //: public QThread
{
    Q_OBJECT;

    P_getService m_getService;

public:
    Plugin(P_getService getService, QObject* parent);

    template<class I>
    I* getService(const char* serviceName)
    {
        return (I*) m_getService(serviceName);
    }
};

#endif