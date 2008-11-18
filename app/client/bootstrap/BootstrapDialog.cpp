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
#include "Settings.h"
#include "iTunesBootstrapper.h"
#include "PluginBootstrapper.h"
#include "lib/lastfm/types/Track.h"


BootstrapDialog::BootstrapDialog( QWidget* parent )
               : QProgressDialog( parent )
{}


void
BootstrapDialog::exec()
{
    show();
    
    {
        iTunesBootstrapper iTunes;
        connect( &iTunes, SIGNAL(trackProcessed( int, Track )), SLOT(onITunesTrackProcessed( int, Track )) );        
        iTunes.bootStrap();
    }

#ifdef WIN32
    foreach (Plugin p, Plugin::installed())
    {
//        PluginBootstrapper bootstrapper( p.id );
//        qDebug() << p.id;
    }
#endif
}


void
BootstrapDialog::onITunesTrackProcessed( int percent, const Track& t )
{
    setLabelText( t.toString() );
    setValue( percent );
}