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

#ifndef APP_H
#define APP_H

#include "playdar/PlaydarApi.h"
#include "playdar/BoffinPlayableItem.h"
#include "lib/unicorn/UnicornApplication.h"
#include <lastfm/global.h>
#include <QPointer>
#include <lastfm/Track>


namespace Phonon { class AudioOutput; }

class TagCloudView;
class QItemSelection;
class PlaydarTagCloudModel;
class Shuffler;
class TrackSource;

namespace lastfm{ class Track; }

class App : public unicorn::Application
{
    Q_OBJECT

public:
    App( int& argc, char* argv[] );
    ~App();

    void init( class MainWindow* ) throw( int /*exitcode*/ );
    void play( class TrackSource *);

public slots:
    void play();
    void xspf(); //prompts to choose a xspf to resolve

    void playPause();

private slots:
    void onOutputDeviceActionTriggered( class QAction* );

    void tagsChanged();
    void onPlaydarConnected();
    void onReadyToPlay();
    void onPlaydarTracksReady( BoffinPlayableItem );
    void onPlaydarAuth(const QString&);
    void onPreparing();
    void onStarted( const Track& );
    void onResumed();
    void onPaused();
    void onStopped();

    void onScanningFinished();
    void onPlaybackError( const QString& );
    void onWordle();

private:
    void cleanup();

    class MainWindow* m_mainwindow;
    class TagBrowserWidget* m_tagcloud;
    class ScrobSocket* m_scrobsocket;
    class MediaPipeline* m_pipe;        // pipe pulls from tracksource
    class TrackSource* m_tracksource;   // tracksource pulls from shuffler
    class Shuffler* m_shuffler;         // shuffler is fed from 
    class PlaydarConnection* m_playdar;
    class Playlist* m_playlist;
    class BoffinRqlRequest* m_req;      // current boffin rql request

    Phonon::AudioOutput* m_audioOutput;

    bool m_playing;

    PlaydarApi m_api;
    lastfm::NetworkAccessManager* m_wam;

};

#endif //APP_H
