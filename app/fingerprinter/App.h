/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Peter Grundstrom and Adam Renburg

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
#ifndef FINGERPRINTER_APP_H
#define FINGERPRINTER_APP_H

#include "lib/unicorn/UnicornApplication.h"
#include "MainWindow.h"
#include "FingerprinterProgressBar.h"


/** @author <petgru@last.fm>
  * @author <adam@last.fm>
  */
class App : public unicorn::Application
{
    Q_OBJECT

    public:
        App( int& argc, char** argv );

        void getTracksFromDirs( QStringList& dirs, QStringList& output);
        void getTracksFromDir( QString dir, QStringList& output );

    public slots:
        void onCloseEvent( QCloseEvent* event );
        void logout();
        void logTrack( QString filename, QString reason = "It's just plain wrong!" );
        
    protected:
        bool trackInfoFromFile( QString filename, TrackInfo& track );
        
        QMutex m_networkCountersMutex;

    protected slots:
        void abortFingerprint();
        void onFingerprintingStopped( bool finished = false );
        
        void sendTracksForFingerprinting( );
        void onNetworkError( FingerprintScheduler::NetworkErrors, QString );
        
        void onSchedulerCantFingerprintTrack( TrackInfo& track, QString reason );
        void onTrackFingerprinted( );

    private:
        void initTranslator();
        bool checkAbort();

        QStringList m_tracksToFingerprint;
        /*
            Number of tracks to send to fingerprinter simultaneously.
            Should not be too high, as it requires quite a bit of processing.
        */
        int m_chunkSize;
        
        MainWindow m_mainWindow;
        FingerprinterProgressBar m_progressBar;
        
        FingerprintScheduler m_fingerprintScheduler;
        
        bool m_abort;

    private slots:
        void init();
        void startFingerprinting( QStringList dirs );

};

#endif // FINGERPRINTERAPPLICATION_H
