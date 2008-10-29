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

#ifndef TRACK_INFO_WIDGET_H
#define TRACK_INFO_WIDGET_H

#include <QImage>
#include <QWidget>
#include "lib/lastfm/types/Track.h"


class PrettyCoverWidget : public QWidget
{
    Q_OBJECT

public:
    PrettyCoverWidget();

	void setTrack( const Track& );
	void clear();
	
	struct Ui
	{
		class SpinnerLabel* spinner;
	};
	
	Ui ui;
	
private slots:
	void onAlbumImageDownloaded( const QByteArray& );

private:
    void paintEvent( QPaintEvent* );

    QImage m_cover;
	Track m_track;
	
	static QImage addReflection( const QImage& );
};

#endif //TRACK_INFO_WIDGET_H
