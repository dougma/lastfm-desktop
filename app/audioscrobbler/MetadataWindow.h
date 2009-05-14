/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *    This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#include "lib/unicorn/UnicornMainWindow.h"
#include <lastfm/Track>

class MetadataWindow : public unicorn::MainWindow
{
    Q_OBJECT

    QString previous_artist;
    struct{
        class QLabel* artist_image;
        class QLabel* album_image;
        class QLabel* title;
        class QTextBrowser* bio;

        class ImageButton* love;
        class ImageButton* tag;
        class ImageButton* share;
    } ui;

public:
    MetadataWindow(const Track&);

public slots:
    void onTrackStarted(const Track&);
    void onStopped();

private slots:
    void onArtistGotInfo();
//    void onAlbumGotInfo();
    void onArtistImageDownloaded();
};
