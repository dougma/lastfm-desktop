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
 
#ifndef APP_H
#define APP_H

#include <lastfm/global.h>
#include "lib/unicorn/UnicornApplication.h"


class App : public unicorn::Application
{
    Q_OBJECT
    
public:
    App( int& argc, char* argv[] );
    ~App();

    void init( class MainWindow* ) throw( int /*exitcode*/ );

    void openXspf( QString filename );
    void play( QStringList tags );
    
public slots:
    void play();
    
private slots:
    void onOutputDeviceActionTriggered( QAction* );
    void onScanningFinished();
    void onTrackSpooled( const lastfm::Track& );
    void onPlayActionToggled( bool );
    void onRadioStopped();
    
private:
    class LocalContentScannerThread* m_contentScannerThread;
    class LocalContentScanner* m_contentScanner;
    class TrackTagUpdater* m_trackTagUpdater;
    class ILocalRqlPlugin* m_localRqlPlugin;
    class LocalRql* m_localRql;
    class ITrackResolverPlugin* m_trackResolver;

    class Radio* m_radio;
    class Resolver* m_resolver;
    
    class MainWindow* m_mainwindow;
    
    class TagCloudView* m_cloud;
};

#endif //APP_H
