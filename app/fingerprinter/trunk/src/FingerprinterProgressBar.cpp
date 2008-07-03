/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
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

#include "FingerprinterProgressBar.h"
#include "TrackInfo.h"

#include <QDebug>
#include <QCloseEvent>

FingerprinterProgressBar::FingerprinterProgressBar( QWidget* parent, Qt::WindowFlags f )
        : QDialog( parent, f | Qt::Sheet ),
          m_stopped( false )
{
    ui.setupUi( this );
    setModal( true );
    setRunning( false );
    
    m_totalTracks = m_tracksFingerprinted = m_tracksSkipped = m_tracksWithErrors = 0;
    
    m_timer.setInterval( 1000 );
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(ui.fingerprintProgressBar, SIGNAL( valueChanged( int ) ), this, SLOT( progressBarChanged( int ) ) );   
 
    m_timeElapsed = m_etaCounter = m_timeRemaining = 0;
    
    setFixedHeight( sizeHint().height() );

    ui.okButton->setVisible( false );

    connect( ui.okButton, SIGNAL( clicked() ),
             this, SLOT( onOkClicked() ) );
    connect( ui.stopButton, SIGNAL( clicked() ),
             this, SLOT( onCancelClicked() ) );
}


FingerprinterProgressBar::~FingerprinterProgressBar()
{
    return;
}


void FingerprinterProgressBar::closeEvent( QCloseEvent *event )
{
    event->ignore();
    onCancelClicked();
}


void FingerprinterProgressBar::reset()
{
    ui.fingerprintProgressBar->setMaximum( 0 );
    ui.fingerprintProgressBar->setValue( 0 );
    ui.fingerprintProgressBar->reset();
    pokeProgressBar();

    m_timeElapsed = m_timeRemaining = m_timeOfFingerprintStart = m_etaCounter = 0;
    m_totalTracks = m_tracksFingerprinted = m_tracksSkipped = m_tracksWithErrors = 0;
    m_average = 0;
    
    m_running = m_paused = false;
    
    ui.infoLabel->setText( tr( "Idle" ) );
    ui.trackLabel->setText( "" );
    ui.timeLabel->setText( timeToString( 0 ) );
    ui.etaLabel->setText( tr( "N/A" ) );
    ui.averageLabel->setText( tr( "N/A" ) );
}


void FingerprinterProgressBar::pokeProgressBar()
{
    ui.fingerprintProgressBar->update();
}

void FingerprinterProgressBar::setTotalTracks( int tracks )
{
    m_totalTracks = tracks;

    ui.fingerprintProgressBar->setMaximum( tracks );
    ui.fingerprintProgressBar->setValue( m_tracksFingerprinted + m_tracksSkipped );
}

void FingerprinterProgressBar::setTracksSkipped( int tracks )
{
    m_tracksSkipped = tracks;
    ui.fingerprintProgressBar->setValue( m_tracksFingerprinted + m_tracksSkipped + m_tracksWithErrors );
}

void FingerprinterProgressBar::setTracksWithErrors( int tracks )
{
    m_tracksWithErrors = tracks;
    ui.fingerprintProgressBar->setValue( m_tracksFingerprinted + m_tracksSkipped + m_tracksWithErrors );
}

void FingerprinterProgressBar::setCurrentTrack( TrackInfo& track )
{
    QString elidedPath =
        ui.trackLabel->fontMetrics().elidedText(
            track.path(), Qt::ElideMiddle, ui.trackLabel->width() );
    ui.trackLabel->setText( elidedPath );
}

void FingerprinterProgressBar::trackFingerprinted()
{
    m_tracksFingerprinted++;
    m_etaCounter++;
    ui.fingerprintProgressBar->setValue( m_tracksFingerprinted + m_tracksSkipped + m_tracksWithErrors );

    updateEta();
    pokeProgressBar();
    
    m_average = (float)(m_timeElapsed - m_timeOfFingerprintStart) / m_tracksFingerprinted;
}

bool FingerprinterProgressBar::onCancelClicked()
{
    int ret = LastMessageBox::question( tr("Last.fm Fingerprinter"),
                   tr( "Are you sure you want to stop the fingerprinting?" ),
                   QMessageBox::Yes | QMessageBox::No );
    
    if ( ret == QMessageBox::Yes )
    {
        m_stopped = true;
        ui.infoLabel->setText( tr( "Stopping after the current track..." ) );
        ui.stopButton->setEnabled( false );
        emit abortFingerprinting();
        return true;
    }
    else
    {
        return false;
    }
}

void FingerprinterProgressBar::setCollectionPhaseOver()
{
    ui.infoLabel->setText( tr( "Fingerprinting..." ) );
    m_timeOfFingerprintStart = m_timeElapsed;
}

void FingerprinterProgressBar::start()
{
    ui.infoLabel->setText( tr( "Collecting files..." ) );
    setEnabled( true );
    setRunning( true );
    m_timer.start();

    ui.okButton->setVisible( false );
    ui.stopButton->setVisible( true );
    ui.stopButton->setEnabled( true );

    m_stopped = false;
}

void FingerprinterProgressBar::stop( bool finished )
{
    setRunning( false );
    m_timer.stop();
    
    if ( m_stopped )
    {
        m_stopped = false;

        // If we had asked it to stop, just close the dialog
        onOkClicked();
        ui.stopButton->setEnabled( true );
    }
    else
    {
        ui.infoLabel->setText( tr( "Done. Thanks!" ) );
        ui.trackLabel->setText( "" );
        ui.etaLabel->setText( timeToString( 0 ) );
        
        ui.okButton->setVisible( true );
        ui.stopButton->setVisible( false );
    }
}

void FingerprinterProgressBar::onOkClicked()
{
    hide();
    emit hidden();
}

QString FingerprinterProgressBar::timeToString ( int l )
{
    if (l < 0)
        return "N/A";
    else
    {
        QString time;
        int h_int = ( l / ( 60 * 60 ) );
        int m_int = ( ( l / 60 ) - ( 60 * h_int )  );
        int s_int = ( l - ( ( l / 60 ) * 60 ) );
        QString s = QString("%1").arg( s_int );
        QString m = QString("%1").arg( m_int );
        QString h = QString("%1").arg( h_int );
        if (h.size() == 1)
            time.append( "0" + h + ":" );
        else
            time.append( h + ":" );

        if (m.size() == 1)
            time.append( "0" + m + ":" );
        else
            time.append( m + ":" );

        if (s.size() == 1)
            time.append( "0" + s );
        else
            time.append( s );
        return time;
    }
}

void FingerprinterProgressBar::update()
{
    m_timeElapsed++;
    ui.timeLabel->setText( timeToString( m_timeElapsed ) );
    if ( m_timeRemaining )
    {
        m_timeRemaining--;
        ui.etaLabel->setText( timeToString( m_timeRemaining ) );
    }
    if ( m_average )
    {
        ui.averageLabel->setText( QString::number( m_average, 'f', 1 ) + " seconds" );
    }
}

void FingerprinterProgressBar::updateEta()
{
    m_timeRemaining = ( m_timeElapsed / m_etaCounter ) *
                      ( ui.fingerprintProgressBar->maximum() - ui.fingerprintProgressBar->value() );
}

void FingerprinterProgressBar::progressBarChanged( int value )
{
    int maxValue = ui.fingerprintProgressBar->maximum();

    if( maxValue < 1 )
    {
        ui.progressLabel->setText( "" );
        return;
    }
 
    QString text = "%1 of %2";
    ui.progressLabel->setText( text.arg( value ).arg( maxValue ) );
}
