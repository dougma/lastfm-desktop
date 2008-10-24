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

#include "PluginHost.h"
#include <QLibrary>
#include <QDir>
#include <QThread>


PluginHost::PluginHost(const QString& pluginPath)
{
    QDir pluginDir(pluginPath);
    foreach(const QString& f, pluginDir.entryList(QDir::Files)) {
        if (QLibrary::isLibrary(pluginDir.filePath(f))) {
            // it looks like a shared library, so try loading it:
            QLibrary lib(pluginDir.filePath(f));
            P_getService get = (P_getService) lib.resolve(PLUGIN_ENTRYPOINT);
            if (get) {
                m_plugins << new Plugin(get, this);
                // and the lib will remain loaded until app terminates. ok?
            } else {
                lib.unload();
            }
        }
    }
}


Plugin::Plugin(P_getService getService, QObject *parent)
: QObject(parent)
, m_getService(getService)
{
}

