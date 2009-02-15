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
 
#ifndef FINGERPRINTERPROGRESSBAR_H
#define FINGERPRINTERPROGRESSBAR_H

#include <QDialog>
#include "lib/unicorn/QMessageBoxBuilder.h"
#include <QTimer>
#include <QPushButton>
#include <QtCore>

#ifdef Q_OS_MAC
    #undef Q_OS_MAC
    #include "ui_progressbar.h"
    #define Q_OS_MAC
#else
    #include "ui_progressbar.h"
#endif



class TrackInfo;

/**
    @author <petgru@last.fm>
*/
class FingerprinterProgressBar : public QDialog
{
    Q_OBJECT
    
    signals:
        void abortFingerprinting();
        void pauseFingerprinting();
        void resumeFingerprinting();
        
        void hidden( bool isHidden = true );
        void minimize();
        
    public:
        static QString timeToString ( int l );
    
    public:
        FingerprinterProgressBar( QWidget* parent = 0, Qt::WindowFlags f = 0 );
        ~FingerprinterProgressBar();
        
        void setTotalTracks( int tracks );
        void setTracksSkipped( int tracks );
        void setTracksWithErrors( int tracks );
        
        int totalTracks() { return m_totalTracks; }
        int totalTracksDone() { return m_tracksFingerprinted + m_tracksSkipped + m_tracksWithErrors; }
        int tracksFingerprinted() { return m_tracksFingerprinted; }
        int tracksSkipped() { return m_tracksSkipped; }
        int tracksWithErrors() { return m_tracksWithErrors; }
        int timeElapsed() { return m_timeElapsed; }
        
        bool isRunning() { return m_running; }
        bool isPaused() { return m_paused; }
        
        void start();
        void stop( bool finished );
        void reset();
        
        void setCollectionPhaseOver();
        void pokeProgressBar();
        
    public slots:
        void setCurrentTrack( TrackInfo& track );
        void trackFingerprinted();
        bool onCancelClicked();
        void onOkClicked();
        void progressBarChanged( int value );

    protected:
        void updateEta();
        
        void setRunning( bool r ) { m_running = r; }
        
        virtual void closeEvent( QCloseEvent *event );
    
        Ui::ProgressBar ui;
        
        int m_totalTracks, m_tracksSkipped, m_tracksWithErrors;
        int m_tracksFingerprinted;
        int m_timeElapsed, m_timeRemaining, m_timeOfFingerprintStart;
        int m_etaCounter;
        
        double m_average;
        
        bool m_running, m_paused, m_stopped;
        QTimer m_timer;
        
    protected slots:
        void update();
        
       
};

#endif
