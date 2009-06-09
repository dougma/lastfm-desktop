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
#include "ActionButton.h"
#include <QPointer>
#include <lastfm/Scrobble>
#include <lastfm/ScrobblePoint>
class ScrobbleButtonAnimation;


class ScrobbleButton : public ActionButton
{
    Q_OBJECT

    QPointer<ScrobbleButtonAnimation> m_animation;
    class Scrobble m_scrobble;
	class WsConnectionMonitor* m_connectionMonitor;
	class QLabel* m_tip;
	ScrobblePoint m_scrobblePoint;
    QPixmap m_pixmap;

public:
    ScrobbleButton();

protected:
    virtual void paintEvent( QPaintEvent* );
	virtual bool event( QEvent* );
    
private slots:
    void onTrackSpooled( const Track&, class StopWatch* );
    void setPixmap( const QPixmap& );
	void update();
    void onChecked( bool );
};
