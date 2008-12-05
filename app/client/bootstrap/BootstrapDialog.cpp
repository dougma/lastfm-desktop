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

#include "BootstrapDialog.h"
#include "iTunesBootstrapper.h"
#include "lib/lastfm/types/Track.h"

#ifdef WIN32
#include "PluginBootstrapper.h"
#include "player/PlayerListener.h"
#include "win/CKillProcessHelper.h"
#endif


BootstrapDialog::BootstrapDialog( PlayerListener* listener, QWidget* parent )
               : QProgressDialog( parent )
{
#ifdef WIN32
    m_plugins = Plugin::installed();
    connect( listener, SIGNAL(bootstrapCompleted( QString )), SLOT(onBootstrapCompleted( QString )) );
#else
    Q_UNUSED( listener );
#endif
}


void
BootstrapDialog::exec()
{
    show();

    iTunesBootstrapper* iTunes = new iTunesBootstrapper( this );
    connect( iTunes, SIGNAL(trackProcessed( int, Track )), SLOT(onITunesTrackProcessed( int, Track )) );        
    iTunes->bootStrap();
    
    connect( iTunes, SIGNAL(percentageUploaded( int )), SLOT(setValue( int )) );
    connect( iTunes, SIGNAL(done( int )), SLOT(onITunesBootstrapDone( int )) );

    QProgressDialog::exec();
}


void
BootstrapDialog::onITunesBootstrapDone( int status )
{
    Q_UNUSED( status );

#ifdef WIN32
    nextPluginBootstrap();
#endif    
}


void
BootstrapDialog::onITunesTrackProcessed( int percent, const Track& t )
{
    setLabelText( t.toString() );
    setValue( percent );
}


void
BootstrapDialog::nextPluginBootstrap()
{
#ifdef WIN32
    if (m_plugins.isEmpty()) return;
    
    Plugin p = m_plugins.takeFirst();
    PluginBootstrapper( p.id ).bootStrap();
    setLabelText( "Basically I can't complete this code until the new Windows installer is built. Restarting the media player will bootstrap it, sorta. Tho." );
#endif
}


void
BootstrapDialog::onBootstrapCompleted( const QString& )
{
    // technically wrong, but this code should be gone soon... *crosses fingers*
    nextPluginBootstrap();
}
