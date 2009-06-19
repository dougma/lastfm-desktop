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
#include <QWidget>
#include <lastfm/Track>
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
        
        connect( (QObject*)artist.getInfo(), SIGNAL(finished()), SLOT(onArtistGotInfo()) );
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
    void onArtistGotInfo();
    void onImageDownloaded();
};

 
class ScanProgressWidget :
#ifdef Q_WS_WIN
    public QWidget
#else
    public QGLWidget
#endif
{
    Q_OBJECT

    bool m_done;
    uint m_artist_count;
    uint m_track_count;

    QList<ImageFucker*> images;
    QStringList paths;
    QHash<QString, int> track_counts;

    int& count( const Artist& artist ) { return track_counts[ artist.name().toLower() ]; }

public:
    ScanProgressWidget();

    virtual void paintEvent( QPaintEvent* );
    virtual void timerEvent( QTimerEvent* );

signals:
    void statusMessage( QString );

public slots:
    void onNewDirectory( const QString& );
    void onNewTrack( const Track& );
    void onFinished();

private slots:
    void onImageFucked();

private:
    void updateStatusMessage();
};
