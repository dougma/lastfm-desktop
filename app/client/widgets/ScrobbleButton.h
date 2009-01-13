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

#include "ActionButton.h"
#include <QPointer>
#include "lib/lastfm/scrobble/Scrobble.h"
class ScrobbleButtonAnimation;


class ScrobbleButton : public ActionButton
{
    Q_OBJECT

    QPointer<ScrobbleButtonAnimation> m_animation;
    Scrobble m_track;
    QPixmap m_pixmap;

public:
    ScrobbleButton();

protected:
    virtual void paintEvent( QPaintEvent* );
    
private slots:
    void onTrackSpooled( const class Track&, class StopWatch* );
    void setPixmap( const QPixmap& );
    void updateToolTip( int );
    void onChecked( bool );
};
