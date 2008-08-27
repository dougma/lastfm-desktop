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
#include "the/definitions.h"


class App : public Unicorn::Application
{
    Q_OBJECT

    friend Radio& The::radio();
    friend MainWindow& The::mainWindow();
    friend Settings& The::settings();
    
public:
    App( int, char** );
    ~App();

    void setMainWindow( class MainWindow* ); //access via The::mainWindow()

    /** the currently observed track */
    Track track() const;
    
	/** if it isn't a lastfm url, we'll try to start it anyway, but it won't
	  * work, and the user will get an error message */
    void open( const class QUrl& url );

public slots:
    void onBootstrapCompleted( const QString& playerId, const QString& username );

    /** all webservices connect to this and emit in the case of bad errors that
      * need to be handled at a higher level */
    void onWsError( Ws::Error );
    
    void onScrobblerStatusChanged( int );

    /** currently also quits, needs fixing! */
    void logout();

	void love();
	void ban();
	
private slots:
    void onAppEvent( int, const QVariant& );

    void onRadioTrackStarted( Track );
    void onRadioPlaybackEnded();
	void onRadioStationTuned( const QString& );
	void onRadioPlaybackStalled();
	void onRadioPlaybackUnstalled();
	
signals:
    void event( int, const QVariant& );

private:
    class PlayerListener* m_playerListener;
    class PlayerManager* m_playerManager;
    class Scrobbler* m_scrobbler;
    class DrWatson* m_watson;
    class MainWindow* m_mainWindow;
    class Radio* m_radio;
    class Settings* m_settings;

    class QSystemTrayIcon* m_trayIcon;
};


namespace The
{
    inline App& app() { return *(App*)qApp; }
}
