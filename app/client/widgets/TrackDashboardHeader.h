/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TRACK_DASHBOARD_HEADER_H
#define TRACK_DASHBOARD_HEADER_H

#include <lastfm/global.h>
#include <QWidget>
#include <QMouseEvent>


class TrackDashboardHeader : public QWidget
{
    Q_OBJECT
    
public:
    TrackDashboardHeader();
    
    void setCogMenu( class QMenu* menu );
    
    struct {
        class ScrobbleButton* scrobbleButton;
        class RadioProgressBar* radioProgress;
        class ImageButton* love;
        class ImageButton* ban;
        class ImageButton* cog;
        class QLabel* track;
    } ui;
    
protected:
    void paintEvent( QPaintEvent* e );
    void resizeEvent( QResizeEvent* e );
    
private slots:
    void onTrackSpooled( const Track&, class StopWatch* );
    void onContextMenuRequested( const QPoint& pos );
    
    void onPraiseClientTeam();
    void onCurseClientTeam();
    void onCogMenuClicked();
    
    void onTrackLinkActivated( const QString& );
    
private:
    class QMenu* m_cogMenu;
};


#endif //TRACK_DASHBOARD_HEADER_H