/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd.                                      *
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

#include <QWidget>
#include "lib/radio/RadioStation.h"
#include "lib/radio/Tuner.h"

namespace Phonon{ class AudioOutput; }

class RadioWidget : public QWidget
{
    Q_OBJECT

public:
    RadioWidget( QWidget* parent = 0 );

    void play( const RadioStation& );

    struct Ui
    {
        class QLabel* spinner;
    } 
    ui;
    
    Phonon::AudioOutput* audioOutput();

signals:
    void trackStarted( const class Track& );
    void playbackEnded();
    void buffering();
    void finishedBuffering();
    void newStationStarted();


private slots:
    void onTunerReturnPressed();
    void queueMoreTracks();

private:
    class AudioPlaybackEngine* m_audio;
    Tuner m_tuner;
};
