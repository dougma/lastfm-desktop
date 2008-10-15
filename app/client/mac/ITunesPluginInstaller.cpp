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

#include "ITunesPluginInstaller.h"
#include "lib/lastfm/core/mac/CFStringToQString.h"
#include <QDebug>
#include <QDir>
#include <QProcess>

static const char* kBundleName = "AudioScrobbler.bundle";
static const char* kPListFile = "Contents/Info.plist";


ITunesPluginInstaller::ITunesPluginInstaller()
        : k_shippedPluginDir( qApp->applicationDirPath() + "/PlugIns/" + kBundleName + "/" ),
          k_iTunesPluginDir( QDir::homePath() + "/Library/iTunes/iTunes Plug-ins/" + kBundleName + "/" ),
          m_needsTwiddlyBootstrap( false )
{}


void
ITunesPluginInstaller::install()
{
    qDebug() << "Found installed iTunes plugin?" << isPluginInstalled();

    QString installedVersion;
    QString shippedVersion;

    disableLegacyHelperApp();

    if ( isPluginInstalled() ) 
    {
        installedVersion = pListVersion( k_iTunesPluginDir + kPListFile );
        qDebug() << "Found installed iTunes plugin version:" << installedVersion;
    }
    else
        //TODO don't bootstrap if installation fails
        m_needsTwiddlyBootstrap = true;

    shippedVersion = pListVersion( k_shippedPluginDir + kPListFile );
    if ( shippedVersion.isEmpty() )
    {
        qDebug() << "Could not locate shipped iTunes plugin!";
    }
    else
    {
        qDebug() << "Found shipped iTunes plugin version:" << shippedVersion;

        if ( installedVersion != shippedVersion )
        {
            qDebug() << "Installing shipped iTunes plugin...";

            if ( !removeInstalledPlugin() )
            {
                qDebug() << "Removing installed plugin from" << k_iTunesPluginDir << "failed!";
            }
            else
            {
                qDebug() << "Successfully removed installed plugin.";

                if ( installPlugin() )
                {
                    qDebug() << "Successfully installed the plugin.";
                }
                else
                    qDebug() << "Installing the plugin failed!";
            }
        }
        else
            qDebug() << "Installed iTunes plugin is up-to-date.";
    }
}


bool
ITunesPluginInstaller::isPluginInstalled()
{
    return QDir( k_iTunesPluginDir ).exists();
}


QString
ITunesPluginInstaller::pListVersion( const QString& file )
{
    QFile f( file );
    if ( !f.open( QIODevice::ReadOnly ) )
        return "";

    const QString key( "<key>CFBundleVersion</key>" );
    const QString begin( "<string>" );
    const QString end( "</string>" );

    while ( !f.atEnd() )
    {
        QString line = f.readLine();
        if ( line.contains( key ) && !f.atEnd() )
        {
            QString versionLine = QString( ( f.readLine() ) ).trimmed();
            QString version = versionLine.section( begin, 1 );
            version = version.left( version.length() - end.length() );

            f.close();
            return version;
        }
    }

    f.close();
    return "";
}


static bool
deleteDir( QString path )
{
    if ( !path.endsWith( "/" ) )
        path += "/";

    QDir d( path );

    // Remove all files
    const QStringList files = d.entryList( QDir::Files );
    foreach( QString file, files )
    {
        if ( !d.remove( file ) )
            return false;
    }

    // Remove all dirs (recursive)
    const QStringList dirs = d.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
    foreach( QString dir, dirs )
    {
        if ( !deleteDir( path + dir ) )
            return false;
    }

    return d.rmdir( path );
}


bool
ITunesPluginInstaller::removeInstalledPlugin()
{
    if ( !isPluginInstalled() )
        return true;

    return deleteDir( k_iTunesPluginDir );
}


static bool
copyDir( QString path, QString dest )
{
    if ( !path.endsWith( '/' ) )
        path += '/';
    if ( !dest.endsWith( '/' ) )
        dest += '/';

    QDir( dest ).mkpath( "." );
    QDir d( path );

    const QStringList files = d.entryList( QDir::Files );
    foreach( QString file, files )
    {
        QFile f( path + file );
        if ( !f.copy( dest + file ) )
            return false;
    }

    const QStringList dirs = d.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
    foreach( QString dir, dirs )
    {
        if ( !copyDir( path + dir, dest + dir ) )
            return false;
    }

    return true;
}


bool
ITunesPluginInstaller::installPlugin()
{
    return copyDir( k_shippedPluginDir, k_iTunesPluginDir );
}


void
ITunesPluginInstaller::disableLegacyHelperApp()
{
    qDebug() << "Disabling old LastFmHelper";

    // EVEN MORE LEGACY: disable oldest helper auto-launch!
    QString oldplist = QDir::homePath() + "/Library/LaunchAgents/fm.last.lastfmhelper.plist";
    if ( QFile::exists( oldplist ) )
    {
        QProcess::execute( "/bin/launchctl", QStringList() << "unload" << oldplist );
        QFile::remove( oldplist );
    }

    // REMOVE LastFmHelper from loginwindow.plist
    CFArrayRef prefCFArrayRef = (CFArrayRef)CFPreferencesCopyValue( CFSTR( "AutoLaunchedApplicationDictionary" ), 
                                                                    CFSTR( "loginwindow" ), 
                                                                    kCFPreferencesCurrentUser, 
                                                                    kCFPreferencesAnyHost );
    if ( prefCFArrayRef == NULL ) return;
    CFMutableArrayRef tCFMutableArrayRef = CFArrayCreateMutableCopy( NULL, 0, prefCFArrayRef );
    if ( tCFMutableArrayRef == NULL ) return;

    for ( int i = CFArrayGetCount( prefCFArrayRef ) - 1; i >= 0 ; i-- )
    {
        CFDictionaryRef dict = (CFDictionaryRef)CFArrayGetValueAtIndex( prefCFArrayRef, i );
        QString path = CFStringToQString( (CFStringRef) CFDictionaryGetValue( dict, CFSTR( "Path" ) ) );

        if ( path.toLower().contains( "lastfmhelper" ) )
        {
            // Better make sure LastFmHelper is really dead
            QProcess::execute( "/usr/bin/killall", QStringList() << "LastFmHelper" );

            qDebug() << "Removing helper from LoginItems at position" << i;
            CFArrayRemoveValueAtIndex( tCFMutableArrayRef, (CFIndex)i );
        }
    }

    CFPreferencesSetValue( CFSTR( "AutoLaunchedApplicationDictionary" ), 
                           tCFMutableArrayRef, 
                           CFSTR( "loginwindow" ), 
                           kCFPreferencesCurrentUser, 
                           kCFPreferencesAnyHost );
    CFPreferencesSynchronize( CFSTR( "loginwindow" ), 
                              kCFPreferencesCurrentUser, 
                              kCFPreferencesAnyHost );

    CFRelease( prefCFArrayRef );
    CFRelease( tCFMutableArrayRef );
}


void
ITunesPluginInstaller::uninstall()
{
    QDir d = k_iTunesPluginDir + "Contents/Resources";
    
    // always use absolute paths to tools! - muesli
    QProcess::startDetached( "/bin/sh", 
                             QStringList() << d.filePath( "uninstall.sh" ),
                             d.path() );
}
