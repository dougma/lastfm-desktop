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

#include "PlayerState.h"
#include "lib/types/Track.h"
#include "lib/unicorn/UnicornApplication.h"
#include "lib/ws/WsError.h"


class App : public Unicorn::Application
{
    Q_OBJECT

public:
    App( int, char** );
    ~App();

    void setMainWindow( class MainWindow* );
    MainWindow& mainWindow() const { return *m_mainWindow; }
    
    /** the currently observed track */
    Track track() const;
    
    void open( const class QUrl& url );

public slots:
    void onBootstrapCompleted( const QString& playerId, const QString& username );

    void love();
    void ban();

    /** all webservices connect to this and emit in the case of bad errors that
      * need to be handled at a higher level */
    void onWsError( Ws::Error );
    
    void onScrobblerStatusChanged( int );

    /** currently also quits, needs fixing! */
    void logout();

private slots:
    void onAppEvent( int, const QVariant& );

    void onRadioTrackStarted( const class Track& );
    void onRadioPlaybackEnded();
    void onRadioBuffering();
    void onRadioFinishedBuffering();

signals:
    void event( int, const QVariant& );

private:
    class PlayerListener* m_playerListener;
    class PlayerManager* m_playerManager;
    class Scrobbler* m_scrobbler;
    class RadioWidget* m_radio;
    class DrWatson* m_watson;
    class MainWindow* m_mainWindow;

    class QSystemTrayIcon* m_trayIcon;
};


namespace The
{
    inline App& app() { return *(App*)qApp; }
}
