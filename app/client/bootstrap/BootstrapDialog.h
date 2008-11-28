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

#include <QProgressDialog>
#include <QList>
#ifdef WIN32
#include "Settings.h"
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
    void onITunesTrackProcessed( int, const class Track& );
    // private and passed to ctor because without it, the class would completely
    // fail to work
    void onBootstrapCompleted( const QString& plugin_id );
    void onITunesBootstrapDone( int );
    
private:
    void nextPluginBootstrap();
};
