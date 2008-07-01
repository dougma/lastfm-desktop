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

#ifndef ITUNES_PLUGIN_INSTALLER_H
#define ITUNES_PLUGIN_INSTALLER_H

#include <QCoreApplication> //Q_DECLARE_TR_FUNCTIONS
#include <QString>


/** @author Christian Muehlhaeuser <chris@last.fm>
  * @contributor Max Howell <max@last.fm>
  */
class ITunesPluginInstaller
{
    Q_DECLARE_TR_FUNCTIONS( ITunesPluginInstaller )

public:
    ITunesPluginInstaller();
    
    void install();
    void uninstall();

    // NOTE this is only valid after calling install()
    bool needsTwiddlyBootstrap() const { return m_needsTwiddlyBootstrap; }

private:
    bool isPluginInstalled();
    QString pListVersion( const QString& file );

    bool removeInstalledPlugin();
    bool installPlugin();

    // Legacy code: removes old LastFmHelper for updates
    void disableLegacyHelperApp();

    QString const k_shippedPluginDir;
    QString const k_iTunesPluginDir;
    bool m_needsTwiddlyBootstrap;
};

#endif
