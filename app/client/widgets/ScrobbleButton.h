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

#ifndef SCROBBLE_BUTTON_H
#define SCROBBLE_BUTTON_H

#include <QAbstractButton>
#include <QPointer>
class QMovie;
class QTimer;


class ScrobbleButton : public QAbstractButton
{
    Q_OBJECT

    QMovie* m_progressMovie;
    QMovie* m_glowMovie;
    QMovie* m_movie;
    QPointer<QTimer> m_timer;

public:
    ScrobbleButton();

protected:
    virtual void paintEvent( QPaintEvent* );
    
private slots:
    void onTrackSpooled( const class Track&, class StopWatch* );
    void onScrobbled();
    void advanceFrame();
    void updateToolTip( int );
};

#endif
