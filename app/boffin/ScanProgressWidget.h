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
#include "lib/lastfm/ws/WsAccessManager.h"
#include "lib/lastfm/ws/WsReply.h"
#include "lib/lastfm/types/Track.h"
#include <QHash>
/** @author Max Howell <max@last.fm> */

class ImageFucker : public QObject
{
    Q_OBJECT

    WsAccessManager nam;

public:
    ImageFucker( const Artist& artist )
    {
        this->height = 0;
        this->opacity = x = 0;
        this->artist = artist;
        
        connect( artist.getInfo(), SIGNAL(finished( WsReply* )), SLOT(onArtistGotInfo( WsReply* )) );
    }
    
    QImage pixmap;
    QString artist;
    qreal x;
    qreal opacity;
    uint height;
    int y;

signals:
    void fucked();

private slots:
    void onArtistGotInfo( class WsReply* );
    void onImageDownloaded();
};

 
class ScanProgressWidget : public QWidget
{
    Q_OBJECT

    QList<ImageFucker*> images;
    QList<Track> tracks;
    QHash<QString, int> track_counts;
    
    int& count( const Artist& artist ) { return track_counts[ artist.name().toLower() ]; }

public:
    ScanProgressWidget();
    
    virtual void paintEvent( QPaintEvent* );
    virtual void timerEvent( QTimerEvent* );

public slots:
    void onNewTrack( const class Track& );

private slots:
    void onImageFucked();
};


#include <QList>
#include <QTimer>
class ScanProgressMock : public QTimer
{
    Q_OBJECT
    
    QList<Artist> artists;
    
public:
    ScanProgressMock()
    {
        artists << Artist("Nirvana") << Artist("Blur") << Artist("Foo Fighters") << Artist(QString::fromUtf8("安室奈美恵")) << Artist("The Cinematic Orchestra");
        
        connect( this, SIGNAL(timeout()), SLOT(onTimeout()) );
        setInterval( 5000 );
        start();
    }
    
private slots:
    void onTimeout()
    {
        if (artists.size())
        {
            MutableTrack t;
            t.setArtist( artists.takeFirst() );
            t.setUrl( QUrl("file://arse/arse") );
            emit track( t );
        }
    }
    
signals:
    void track( const Track& );
};
