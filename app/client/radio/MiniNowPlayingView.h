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

#ifndef MINI_NOW_PLAYING_VIEW_H
#define MINI_NOW_PLAYING_VIEW_H

#include <QWidget>
#include "lib/lastfm/types/Track.h"
class QLabel;


class MiniNowPlayingView : public QWidget
{
	Q_OBJECT
	
public:
	MiniNowPlayingView( QWidget* p = 0 ) : QWidget( p ) { setupUi(); }
	MiniNowPlayingView( const Track& t );
	
private:
	void setupUi();
	
	struct {
		QLabel *albumArt;
		QLabel *artist;
		QLabel *track;
		QLabel *album;
	} ui;
	
private slots:
	void onAlbumImageDownloaded( QByteArray b );
};

#endif //MINI_NOW_PLAYING_VIEW_H
