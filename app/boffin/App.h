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

#include "lib/unicorn/UnicornApplication.h"


class App : public Unicorn::Application
{
    Q_OBJECT
    
public:
    App( int& argc, char* argv[] );
    ~App();

    void openXspf( QString filename );
    void setMainWindow( class MainWindow* ) throw( int /*exitcode*/ );

private slots:
    void onOutputDeviceActionTriggered( QAction* );
    void onScanningFinished();

private:
    class LocalContentScannerThread* m_contentScannerThread;
    class LocalContentScanner* m_contentScanner;
    class TrackTagUpdater* m_trackTagUpdater;
    class ILocalRqlPlugin* m_localRql;
    class ITrackResolverPlugin* m_trackResolver;

    class Radio* m_radio;
    class Resolver* m_resolver;
    
    class MainWindow* m_mainwindow;
};

#endif //APP_H
