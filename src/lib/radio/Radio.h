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

#include "common/DllExportMacro.h"
#include <QList>
#include <QThread>
#include <QUrl>
#include <QWaitCondition>


class RADIO_DLLEXPORT Radio : public QThread
{
    Q_OBJECT

    virtual void run();

    QString const m_username;
    QString const m_password;
    QString m_session;
    QString m_host;
    QString m_base_path;
    QString m_station_url;
    
    QWaitCondition m_waitCondition;

public:
    struct Track
    {
        Track() : duration( 0 )
        {}

        QString authcode;
        QString title;
        QString artist;
        QString album;
        uint duration;
        QString sponsor;
        QString location;
    };
    
private:
    QByteArray get( QString path );

    void handshake();
    void adjust();
    QList<Track> fetchPlaylist();

public:
    Radio( const QString& username, const QString& password );

    /** eg lastfm://user/mxcl/ */
    void tuneIn( const QString& station );
    void fetchNextPlaylist();

    bool m_done;
    
    QList<Track> m_tracks;
    
signals:
    void tracksReady();
};
