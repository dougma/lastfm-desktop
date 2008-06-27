/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "PlaybackState.h"
#include <QApplication>


class App : public QApplication
{
    Q_OBJECT

public:
    App( int, char** );
    ~App();

    PlaybackState::Enum state() const;

    void setMainWindow( class MainWindow* );

    //TODO remove
    class PlayerManager& playerManager() { return *m_playerManager; }

public slots:
    void onBootstrapCompleted( const QString& playerId, const QString& username );

    void love();
    void ban();

private slots:
    void onAppEvent( int, const QVariant& );

signals:
    void event( int, const QVariant& );

private:
    class PlayerListener* m_playerListener;
    class PlayerManager* m_playerManager;
    class Scrobbler* m_scrobbler;
    class Radio* m_radio;
    class DrWatson* m_watson;
};
