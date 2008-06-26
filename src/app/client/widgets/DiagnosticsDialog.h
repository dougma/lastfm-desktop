/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#ifndef DIAGNOSTICS_DIALOG_H
#define DIAGNOSTICS_DIALOG_H

#include "ui_DiagnosticsDialog.h"

#include "MailLogsDialog.h"

#include <QDateTime>
#include <iostream>
#include <fstream>


/** @author <max@last.fm>
  * @brief Monitors the various parts of the application without requiring 
  * us to keep an entire dialog instantiated */
//TODO don't allocate whole object on global stack? lame for startup time
class DrWatson : public QObject
{
    Q_OBJECT

signals:
    void scrobblerStatusChanged( int );

private slots:
    void onScrobblerStatusChanged( int );

public:
    DrWatson();

    uint scrobbler_status;
    QDateTime scrobbler_handshake_time;

    void observe( class Scrobbler* o )
    {
        connect( (QObject*)o, SIGNAL(status( int, QVariant )), SLOT(onScrobblerStatusChanged( int )) );
    }
};


class DiagnosticsDialog : public QDialog
{
    Q_OBJECT

    static DrWatson watson;

public:
    DiagnosticsDialog( QWidget *parent = 0 );
    ~DiagnosticsDialog();

    static void observe( Scrobbler* p ) { watson.observe( p ); }

    Ui::DiagnosticsDialog ui;

    void show();

private:
    void populateScrobbleCacheView();
	void scrobbleIpod( bool isManual = false );
	QString diagnosticInformation();

    MailLogsDialog m_mailLogsDialog;

    class QTimer* m_logTimer;
    std::ifstream m_logFile;

private slots:
    void onScrobblerStatusChanged( int );
    void onAppEvent( int event, const QVariant& );

    void onHttpBufferSizeChanged( int bufferSize );
    void onDecodedBufferSizeChanged( int bufferSize );
    void onOutputBufferSizeChanged( int bufferSize );

    void onCopyToClipboard();

    void onTrackFingerprintingStarted( const class TrackInfo& );
    void onTrackFingerprinted( const TrackInfo& );
    void onCantFingerprintTrack( const TrackInfo&, QString reason );

	void onScrobbleIpodClicked();
	void onLogPoll();
	void onMailLogsClicked();

    void onRefresh();
};

#endif //DIAGNOSTICSDIALOG_H
