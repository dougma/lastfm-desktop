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
#include <lastfm/global.h"
#include <QProgressDialog>
#include <QList>
#ifdef WIN32
#include "Settings>
#endif


class BootstrapDialog : public QProgressDialog
{
    Q_OBJECT
    
#ifdef WIN32
    QList<Plugin> m_plugins;
#endif
    
public:
    BootstrapDialog( class PlayerListener*, QWidget* parent );

    void exec();
    
private slots:
    void onITunesTrackProcessed( int, const Track& );
    // private and passed to ctor because without it, the class would completely
    // fail to work
    void onBootstrapCompleted( const QString& plugin_id );
    void onITunesBootstrapDone( int );
    
private:
    void nextPluginBootstrap();
};
