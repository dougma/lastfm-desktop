/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
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

#ifndef SYS_TRAY_H
#define SYS_TRAY_H

#include <QSystemTrayIcon>
#include <QTimer>


class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    TrayIcon( QObject* parent );

public slots:
    void setTrack( const MetaData& );
    void setUser( class LastFmUserSettings& currentUser );

private:
    void refreshToolTip();

  #ifdef Q_WS_MAC
    QPixmap m_pixmap;
  #endif
    
    QString m_artist;
    QString m_track;
    QString m_user;
};

#endif // SYSTRAY_H
