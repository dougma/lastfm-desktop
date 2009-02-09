/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Erik Jalevik, Last.fm Ltd <erik@last.fm>                           *
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

#ifndef TRACKPROGRESSFRAME_H
#define TRACKPROGRESSFRAME_H

#include "progressframe.h"
#include <QToolTip>

class TrackProgressFrame : public ProgressFrame
{
    Q_OBJECT

public:
    TrackProgressFrame( QWidget *parent = 0 );

    void setScrobbledGradient( const QLinearGradient& sg ) { m_scrobbledGradient = sg; }
    void setScrobblingEnabled( bool en );
    void setTrack( TrackInfo& track );

protected:
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );

    virtual void paintEvent( QPaintEvent * event );
    virtual QRect paintClock( QPainter* painter, QRect rect );

private:
    TrackInfo::Source m_source;
    int m_trackLength;
    bool m_scrobblingEnabled;
    bool m_scrobbled;
    bool m_trackIsScrobblable;

    bool isScrobblable() const;
    QString textForScrobblableStatus( TrackInfo& track );

    QLinearGradient m_scrobbledGradient;
    QRect m_clockRect;

    // Keeps track of the user clicks of the clock, if false shows scrobble time
    bool m_clockShowsTrackTime;
};

#endif // TRACKPROGRESSFRAME_H
