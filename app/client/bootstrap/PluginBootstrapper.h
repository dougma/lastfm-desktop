/***************************************************************************
*   Copyright (C) 2005 - 2007 by                                          *
*      Jono Cole, Last.fm Ltd <jono@last.fm>                              *
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

#ifndef PLUGIN_BOOTSTRAPPER_H_
#define PLUGIN_BOOTSTRAPPER_H_

#include "AbstractBootstrapper.h"

class PluginBootstrapper :  public AbstractBootstrapper
{
    Q_OBJECT

public:
    PluginBootstrapper( QString pluginId, QObject* parent = NULL );

    void bootStrap();
    void submitBootstrap();

private:
    QString m_pluginId;

private slots:
    void onUploadCompleted( int status );

};

#endif //PLUGIN_BOOTSTRAPPER_H_
