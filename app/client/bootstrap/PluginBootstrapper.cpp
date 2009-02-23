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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "PluginBootstrapper.h"
#include "lib/unicorn/QMessageBoxBuilder.h"
#include "lib/lastfm/core/CoreDir.h"
#include "lib/lastfm/ws/WsKeys.h"
#include <QSettings>
#include <QFile>


PluginBootstrapper::PluginBootstrapper( QString pluginId, QObject* parent )
                   :AbstractBootstrapper( parent ),
                    m_pluginId( pluginId )
{
    connect( this, SIGNAL( done( int ) ),
             this, SLOT( onUploadCompleted( int ) ) );
}


void
PluginBootstrapper::bootStrap()
{
    QSettings bootstrap( QSettings::NativeFormat, QSettings::UserScope, "Last.fm", "Bootstrap", this );

    bootstrap.setValue( m_pluginId, Ws::Username );
    bootstrap.setValue( "data_path",  CoreDir::data().path() );

    bootstrap.setValue( "Strings/progress_label",       tr("Last.fm is importing your current media library...") );
    bootstrap.setValue( "Strings/complete_label",       tr("Last.fm has imported your media library.\n\n Click OK to continue.") );
    bootstrap.setValue( "Strings/progress_title",       tr("Last.fm Library Import") );
    bootstrap.setValue( "Strings/cancel_confirmation",  tr("Are you sure you want to cancel the import?") );
    bootstrap.setValue( "Strings/no_tracks_found",      tr("Last.fm couldn't find any played tracks in your media library.\n\n Click OK to continue.") );
}


void
PluginBootstrapper::submitBootstrap()
{
    QString savePath = CoreDir::data().filePath( Ws::Username + "_" + m_pluginId + "_bootstrap.xml" );
    QString zipPath = savePath + ".gz";

    zipFiles( savePath, zipPath );
    sendZip( zipPath );
}


void
PluginBootstrapper::onUploadCompleted( int status )
{
    QString savePath = CoreDir::data().filePath( Ws::Username + "_" + m_pluginId + "_bootstrap.xml" );
    QString zipPath = savePath + ".gz";

    if( status == Bootstrap_Ok  )
    {
        QFile::remove( savePath );

        QMessageBoxBuilder( 0 )
                .setIcon( QMessageBox::Information )
                .setTitle( tr("Media Library Import Complete") )
                .setText( tr( "Last.fm has submitted your listening history to the server.\n"
                              "Your profile will be updated with the new tracks in a few minutes.") );
    }
    else if( status == Bootstrap_Denied )
    {
        QMessageBoxBuilder( 0 )
            .setIcon( QMessageBox::Warning )
            .setTitle( tr("Library Import Failed") )
            .setText( tr( "Sorry, Last.fm was unable to import your listening history. "
                          "This is probably because you've already scrobbled too many tracks. "
                          "Listening history can only be imported to brand new profiles.") );
        QFile::remove( savePath );
    }
}
