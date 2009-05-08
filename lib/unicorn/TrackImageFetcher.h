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

#ifndef TRACK_IMAGE_FETCHER_H
#define TRACK_IMAGE_FETCHER_H

#include <QObject>
#include <lib/DllExportMacro.h>
#include <lastfm/Track>


/** @author <max@last.fm>
  * Fetches the album art for an album, via album.getInfo
  */
class UNICORN_DLLEXPORT TrackImageFetcher : public QObject
{
    Q_OBJECT

    Track m_track;

    void artistGetInfo();
    void fail();
    bool downloadImage( QNetworkReply*, const QString& root_node_name );
    
    Album album() const { return m_track.album(); }
    Artist artist() const { return m_track.artist(); }
    
public:
    TrackImageFetcher( const Track& t ) : m_track( t )
    {}

    void start();

signals:
    void finished( const class QImage& );

private slots:
    void onAlbumGotInfo();
    void onArtistGotInfo();
    void onAlbumImageDownloaded();
    void onArtistImageDownloaded();
};

#endif
