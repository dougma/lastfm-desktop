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

#ifndef NOW_PLAYING_WIDGET_H
#define NOW_PLAYING_WIDGET_H

#include <QWidget>
#include <lastfm/Track>
#include <lastfm/RadioStation>

class TrackWidget;
class RadioProgressBar;
class QLabel;
class QImage;

class NowPlayingWidget : public QWidget
{
    Q_OBJECT;

public:
    NowPlayingWidget();

signals:
    void tick( qint64 );
public slots:
    // all from the radio:
    void onTuningIn( const RadioStation& );
    void onTrackSpooled( const Track& );
    void onBuffering( int );
    void onStopped();
    
private slots:
    void onImageFinished( const QImage& );

private:
    struct {
        RadioProgressBar* bar;
        QLabel* cover;
        QLabel* track;
        QLabel* album;
    } ui;
};

#endif
