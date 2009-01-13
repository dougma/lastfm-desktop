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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/

#ifndef FINGERPRINTER_APP_H
#define FINGERPRINTER_APP_H

#include "lib/unicorn/UnicornApplication.h"
#include <QTranslator>

#include "MainWindow.h"
#include "FingerprinterProgressBar.h"
#include "logindialog.h"

#include "libFingerprint/FingerprintScheduler.h"
#include "libLastFmTools/TrackInfo.h"
#include "libLastFmTools/LastMessageBox.h"

/** @author <petgru@last.fm>
  * @author <adam@last.fm>
  */
class App : public Unicorn::Application
{
    Q_OBJECT

    public:
        FingerprinterApplication( int& argc, char** argv );
        ~FingerprinterApplication ();

        void getTracksFromDirs( QStringList& dirs, QStringList& output);
        void getTracksFromDir( QString dir, QStringList& output );

    public slots:
        // Switch the app to a different language
        void setLanguage( QString langCode );
        void exit();
        
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

        QString m_lang;
        QStringList m_tracksToFingerprint;
        /*
            Number of tracks to send to fingerprinter simultaneously.
            Should not be too high, as it requires quite a bit of processing.
        */
        int m_chunkSize;

        QTranslator m_translatorApp;
        QTranslator m_translatorQt;
        
        LoginDialog* m_loginDialog;
        
        MainWindow m_mainWindow;
        FingerprinterProgressBar m_progressBar;
        
        FingerprintScheduler m_fingerprintScheduler;
        
        bool m_abort;

    private slots:
        void init();
        void login();
        void startFingerprinting( QStringList dirs );

};

#endif // FINGERPRINTERAPPLICATION_H
