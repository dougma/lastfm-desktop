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

#ifndef PLAYBACK_CONTROLS_WIDGET_H_
#define PLAYBACK_CONTROLS_WIDGET_H_

#include "StylableWidget.h"

class QPushButton;

namespace lastfm{ class RadioStation; };

using lastfm::RadioStation;

class PlaybackControlsWidget : public StylableWidget
{
	Q_OBJECT

public:
    PlaybackControlsWidget( QWidget* parent = 0 );

	struct Ui
    {
        QPushButton* love;
        QPushButton* ban;
        QPushButton* play;
        QPushButton* skip;
    } 
	ui;

private slots:
	void onRadioStopped();
    void onRadioTuningIn( const class RadioStation& );
	void onPlayClicked();
    
	
signals:
	void stop();
    void play();
    void skip();

};


#endif //PLAYBACK_CONTROLS_WIDGET_H_
