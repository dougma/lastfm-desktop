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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#include "lib/unicorn/UnicornApplication.h"


class App : public unicorn::Application
{
    Q_OBJECT
    
    class StateMachine* machine;
    class Scrobbler* scrobbler;
    class Container* container;
    class QSystemTrayIcon* trayicon;
    
public:
    App( int&, char** );
    ~App();

    class Radio* radio;

    void setContainer( class Container* );
    
public slots:
    void open( const QUrl& );
    void parseArguments( const QStringList& );
    void setScrobblingEnabled( bool );
    void logout();

signals:
    /** documented in PlayerManager */
    void playerChanged( const QString& name );
    void trackSpooled( const Track&, class StopWatch* = 0 );
    void trackUnspooled( const Track& );
    void stopped();
    void stateChanged( State newstate, const Track& = Track() ); //convenience
    void scrobblePointReached( const Track& );

private slots:
    /** all webservices connect to this and emit in the case of bad errors that
     * need to be handled at a higher level */
    void onWsError( Ws::Error );
    void onRadioError( int, const class QVariant& );
    
	void onScrobblerStatusChanged( int );
    void onTrackSpooled( const Track& );
    void onTrackUnspooled( const Track& );
    void onUserGotInfo( WsReply* );

};
