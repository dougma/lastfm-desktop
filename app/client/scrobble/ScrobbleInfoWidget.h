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

#ifndef SCROBBLE_INFO_WIDGET_H
#define SCROBBLE_INFO_WIDGET_H

#include "PlayerState.h"
#include <QWidget>
#include <QAction>


class ScrobbleInfoWidget : public QWidget
{
	Q_OBJECT
	
public:
	ScrobbleInfoWidget();
	
	virtual void resizeEvent( QResizeEvent* );
	virtual QSize sizeHint() const { return QSize( 362, 326 ); }
	virtual void paintEvent( class QPaintEvent* );
	
	//Most of the ui should only be touched by the Widget itself
	//however the actionbar is created by the window so it can
	//share QActions for use in the menus etc.
	struct 
	{
		friend class ScrobbleInfoWidget;
		class QWidget* actionbar;
	
	private:
        class QLabel* text;
		class TrackInfoWidget* cover;
		class ScrobbleProgressBar* progress;
		class MediaPlayerIndicator* playerIndicator;
	} 
	ui;
	
private slots:
    void onTrackSpooled( const class Track& );
    void onStateChanged( State );
};

#endif //SCROBBLE_INFO_WIDGET_H
