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

#include <QWidget>
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/types/Track.h"
#include <QGLWidget>
#include <QHash>
/** @author Max Howell <max@last.fm> */


class ImageFucker : public QObject
{
    Q_OBJECT

public:
    ImageFucker( const Artist& artist )
    {
        this->steps = 0;
        this->height = 0;
        this->x = this->opacity = 0;
        this->artist = artist;
        
        connect( artist.getInfo(), SIGNAL(finished( WsReply* )), SLOT(onArtistGotInfo( WsReply* )) );
    }
    
    QImage pixmap;
    QString artist;
    qreal x;
    qreal opacity;
    uint height;
    int y;
    int steps;

signals:
    void fucked();

private slots:
    void onArtistGotInfo( class WsReply* );
    void onImageDownloaded();
};

 
class ScanProgressWidget : public QGLWidget
{
    Q_OBJECT

    bool m_done;
    uint m_artist_count;
    uint m_track_count;

    QList<ImageFucker*> images;
    QList<Track> tracks;
    QHash<QString, int> track_counts;
    
    int& count( const Artist& artist ) { return track_counts[ artist.name().toLower() ]; }

public:
    ScanProgressWidget();
    
    virtual void paintEvent( QPaintEvent* );
    virtual void timerEvent( QTimerEvent* );

public slots:
    void onNewTrack( const Track&, int, int );
    void onFinished();

private slots:
    void onImageFucked();
};
