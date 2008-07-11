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

#include "DiagnosticsDialog.h"
#include "ObservedTrack.h"
#include "PlaybackEvent.h"
#include "Settings.h"
#include "scrobbler/Scrobbler.h"
#include "scrobbler/ScrobbleCache.h"
#include "widgets/SendLogsDialog.h"
#include "lib/unicorn/UnicornDir.h"
#include "lib/unicorn/UnicornFile.h"
#include <QByteArray>
#include <QClipboard>
#include <QFile>
#include <QTimer>
#include <QProcess>

DrWatson DiagnosticsDialog::watson;


DrWatson::DrWatson()
{
    scrobbler_status = Scrobbler::Connecting;
}


void
DrWatson::onScrobblerStatusChanged( int const new_status )
{
    int const old_status = scrobbler_status;

    if (new_status == Scrobbler::Connecting)
        switch (old_status)
        {
            case Scrobbler::ErrorBadSession:
            case Scrobbler::ErrorBannedClient:
            case Scrobbler::ErrorInvalidSessionKey:
            case Scrobbler::ErrorBadTime:
            case Scrobbler::ErrorThreeHardFailures:
                // the state that is already set better represents the actual
                // state then "Connecting..." does
                return;
        }

    switch (new_status)
    {
        case Scrobbler::TracksScrobbled:
            scrobbler_status = Scrobbler::Handshaken;
            break;

        case Scrobbler::Handshaken:
            scrobbler_handshake_time = QDateTime::currentDateTime();
            // FALL THROUGH
        default:
            scrobbler_status = new_status;
            break;
    }

    emit scrobblerStatusChanged( scrobbler_status ); 
}


DiagnosticsDialog::DiagnosticsDialog( QWidget *parent )
        : QDialog( parent )
{
    ui.setupUi( this );

#ifdef HIDE_RADIO
    ui.tabWidget->removeTab( 1 );
#endif
    
#ifdef Q_WS_X11
    ui.tabWidget->removeTab( 3 );
#endif


    #ifdef Q_OS_MAC
    ui.cachedTracksLabel->setAttribute( Qt::WA_MacSmallSize );
    ui.fingerprintedTracksTitle->setAttribute( Qt::WA_MacSmallSize );
    ui.cachedTracksList->setAttribute( Qt::WA_MacSmallSize );
    ui.cachedTracksList->setAttribute( Qt::WA_MacShowFocusRect, false );
    #endif

    ui.httpBufferLabel->setMinimumWidth( ui.httpBufferProgress->fontMetrics().width( "100.0k" ) );

    ui.httpBufferProgress->setMaximum( 500000 );
    ui.decodedBufferProgress->setMaximum( 100000 );
    ui.outputBufferProgress->setMaximum( 100000 );

#if 0 
    //TODO
    connect( &The::scrobbler(), SIGNAL( status( int, QVariant ) ), SLOT( onScrobblerEvent() ) );
    connect( ui.viewLogButton,  SIGNAL( clicked() ), &The::container(), SLOT( onAltShiftL() ) );
    
    // Fingerprint collector
    ui.fpQueueSizeLabel->setText( "0" );
    connect( The::app().m_fpCollector, SIGNAL( trackFingerprintingStarted( Track ) ),
        this,                     SLOT( onTrackFingerprintingStarted( Track ) ),
        Qt::QueuedConnection );
    connect( The::app().m_fpCollector, SIGNAL( trackFingerprinted( Track ) ),
        this,                     SLOT( onTrackFingerprinted( Track ) ),
        Qt::QueuedConnection );
    connect( The::app().m_fpCollector, SIGNAL( cantFingerprintTrack( Track, QString ) ),
        this,                     SLOT( onCantFingerprintTrack( Track, QString ) ),
        Qt::QueuedConnection );

#endif
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    connect( ui.copyToClipboardButton, SIGNAL( clicked() ), SLOT( onCopyToClipboard() ) );
    connect( ui.scrobbleIpodButton, SIGNAL( clicked() ), SLOT( onScrobbleIpodClicked() ) );
    connect( ui.sendLogsButton, SIGNAL( clicked() ), SLOT( onSendLogsClicked() ) );

    m_logTimer = new QTimer( this );
    connect( m_logTimer, SIGNAL(timeout()), SLOT(onLogPoll()) );

    connect( &watson, SIGNAL(scrobblerStatusChanged( int )), SLOT(onScrobblerStatusChanged( int )) );

    onScrobblerStatusChanged( watson.scrobbler_status );
}


#if 0 
//TODO
void
DiagnosticsDialog::show()
{
    //initialize the progress bars to 0
    onDecodedBufferSizeChanged( 0 );
    onHttpBufferSizeChanged( 0 );
    onOutputBufferSizeChanged( 0 );

    connect( &The::audioController().m_thread, SIGNAL( httpBufferSizeChanged( int ) ),
        this,                              SLOT( onHttpBufferSizeChanged( int ) ),
        Qt::QueuedConnection );

    connect( &The::audioController().m_thread, SIGNAL( decodedBufferSizeChanged( int ) ),
        this,                              SLOT( onDecodedBufferSizeChanged( int ) ),
        Qt::QueuedConnection );

    connect( &The::audioController().m_thread, SIGNAL( outputBufferSizeChanged( int ) ),
        this,                              SLOT( onOutputBufferSizeChanged( int ) ),
        Qt::QueuedConnection );

    onRefresh();

    QDialog::show();
}
#endif


DiagnosticsDialog::~DiagnosticsDialog()
{
    if( m_logFile.is_open() )
    {
        m_logFile.close();
    }
}


static QString scrobblerStatusText( int const i )
{
    #define tr QObject::tr
    switch (i)
    {
        case Scrobbler::ErrorBadSession: return tr( "Your session expired, it is being renewed." );
        case Scrobbler::ErrorBannedClient: return tr( "Your client too old, you must upgrade." );
        case Scrobbler::ErrorInvalidSessionKey: return tr( "Your username or password is incorrect" );
        case Scrobbler::ErrorBadTime: return tr( "Your timezone or date are incorrect" );
        case Scrobbler::ErrorThreeHardFailures: return tr( "The submissions server is down" );

        case Scrobbler::Connecting: return tr( "Connecting to Last.fm" );
        case Scrobbler::Scrobbling: return tr( "Scrobbling" );

        case Scrobbler::TracksScrobbled:
        case Scrobbler::Handshaken:
            return tr( "OK" );
    }
    #undef tr

    return "";
}


void
DiagnosticsDialog::onScrobblerStatusChanged( int v )
{
    QDateTime const &d = watson.scrobbler_handshake_time;
    if (d.isValid())
        ui.lastConnectionStatusLabel->setText( d.toString( "d/M/yyyy h:mm" ) );

    QString const s = scrobblerStatusText( v );
    ui.subsLight->setToolTip( s );

    populateScrobbleCacheView();

    switch (v)
    {
        case Scrobbler::ErrorBadSession:
            //TODO flashing
        case Scrobbler::Connecting:
            //NOTE we only get this on startup
            ui.subsLight->setColor( Qt::yellow );
            break;
        case Scrobbler::Handshaken:
        case Scrobbler::Scrobbling:
        case Scrobbler::TracksScrobbled:
            ui.subsLight->setColor( Qt::green );
            break;
        
        case Scrobbler::ErrorBannedClient:
        case Scrobbler::ErrorInvalidSessionKey:
        case Scrobbler::ErrorBadTime:
        case Scrobbler::ErrorThreeHardFailures:
            ui.subsLight->setColor( Qt::red );
            break;
    }
}


void
DiagnosticsDialog::onHttpBufferSizeChanged(int bufferSize)
{
    ui.httpBufferProgress->setValue( bufferSize );
    ui.httpBufferLabel->setText( QString::number( bufferSize / 1000.0f, 'f', 1 ) + "k" );
}


void
DiagnosticsDialog::onDecodedBufferSizeChanged(int bufferSize)
{
    ui.decodedBufferProgress->setValue( bufferSize );
    ui.decodedBufferLabel->setText( QString::number( bufferSize / 1000.0f, 'f', 1 ) + "k" );
}


void
DiagnosticsDialog::onOutputBufferSizeChanged(int bufferSize)
{
    ui.outputBufferProgress->setValue( bufferSize );
    ui.outputBufferLabel->setText( QString::number( bufferSize / 1000.0f, 'f', 1 ) + "k" );
}


void
DiagnosticsDialog::onAppEvent( int event, const QVariant& )
{
    switch (event)
    {
    case PlaybackEvent::ScrobblePointReached:
        populateScrobbleCacheView();
        break;

    default:
        break;
    }
}


#if 0
void
DiagnosticsDialog::radioHandshakeReturn( Request* req )
{
    Handshake* handshake = static_cast<Handshake*>( req );
    if ( handshake->failed() )
    {
        ui.radioServerStatusLabel->setText( tr( "Error: " ) + handshake->errorMessage() );
    } else
    {
        ui.radioServerStatusLabel->setText( tr( "OK" ) );
    }
}
#endif


void 
DiagnosticsDialog::populateScrobbleCacheView()
{
    ScrobbleCache cache( The::settings().username() );

    QList<QTreeWidgetItem *> items;
    foreach (Track t, cache.tracks())
        if ( t.isScrobbled() )
            items.append( new QTreeWidgetItem( QStringList() << t.artist() << t.title() << t.album() ) );

    ui.cachedTracksList->clear();
    ui.cachedTracksList->insertTopLevelItems( 0, items );
    ui.cachedTracksLabel->setText( cache.tracks().isEmpty()
            ? tr( "The cache is empty" )
            : tr( "%n cached tracks", "", items.count() ) );
}


void 
DiagnosticsDialog::onRefresh()
{
    populateScrobbleCacheView();
}

QString
DiagnosticsDialog::diagnosticInformation()
{
    QString informationText;

    //TODO should read "Last successful submission" - that's what it actually shows at least
    informationText.append( tr( "Last successful connection: " ) + ui.lastConnectionStatusLabel->text() + "\n\n" );
    informationText.append( tr( "Submission Server: " ) + ui.subsLight->toolTip() + "\n" );
    informationText.append( ui.cachedTracksLabel->text() + ":\n\n" );

    // Iterate through cached tracks list and add to clipboard contents
    for(int row = 0; row < ui.cachedTracksList->topLevelItemCount(); row++)
    {
        QTreeWidgetItem *rowData = ui.cachedTracksList->topLevelItem( row );
        for(int col = 0; col < rowData->columnCount(); col++)
        {
            informationText.append( rowData->data( col, Qt::DisplayRole ).toString() );
            informationText.append( "\t:\t" );
        }
        //remove trailing seperators
        informationText.chop(3);
        informationText.append( "\n" );
    }

    #ifndef HIDE_RADIO
    informationText.append( tr( "Radio Server: " ) + ui.radioServerStatusLabel->text() + "\n" );
    #endif
    
    return informationText;
}

void
DiagnosticsDialog::onCopyToClipboard()
{
    QApplication::clipboard()->setText( diagnosticInformation() );
}


void
DiagnosticsDialog::onTrackFingerprintingStarted( const Track& track )
{
#if 0
    ui.fpCurrentTrackLabel->setText( track.toString() );
    ui.fpQueueSizeLabel->setText( QString::number( The::app().m_fpCollector->queueSize() ) );
#endif
}


void
DiagnosticsDialog::onTrackFingerprinted( const Track& track  )
{
#if 0
    ui.fpCurrentTrackLabel->setText( "" );
    ui.fpQueueSizeLabel->setText( QString::number( The::app().m_fpCollector->queueSize() ) );
    
    new QTreeWidgetItem( ui.fingerprintedTracksList, QStringList() << track.artist() << track.track() << track.album() );
#endif
}


void
DiagnosticsDialog::onCantFingerprintTrack( const Track& /* track */, QString /* reason */ )
{
#if 0
    ui.fpCurrentTrackLabel->setText( "" );
    ui.fpQueueSizeLabel->setText( QString::number( The::app().m_fpCollector->queueSize() ) );
#endif
}


void 
DiagnosticsDialog::onLogPoll()
{
    //Clear all state flags on the file stream
    //this avoids stale state information
    m_logFile.clear();

    //This will reset the state information to !good
    //if at end of file.
    m_logFile.peek();

    //early out if at EOF or other error
    if( !m_logFile.good() )
        return;
    
    char* dataBuffer = new char[1000];

    //Read the log file in batches of 10 lines
    //(this is not ideal but avoids hanging the ui thread too much)
    for( int i = 0; i < 10 && m_logFile.good(); ++i )
    {
        QString data;
        m_logFile.getline( dataBuffer, 1000 );
        data = dataBuffer;
        data = data.trimmed();

        //ignore empty lines 
        if( data.isEmpty() )
            continue;

        ui.ipodInfoList->addItem( data );
        ui.ipodInfoList->scrollToBottom();
    }
    delete[] dataBuffer;
}

void 
DiagnosticsDialog::onScrobbleIpodClicked()
{
#ifndef Q_WS_X11
    QStringList args = (QStringList() 
                    << "--device" << "diagnostic" 
                    << "--vid" << "0000" 
                    << "--pid" << "0000" 
                    << "--serial" << "UNKNOWN");

    bool const isManual = ( ui.iPodScrobbleType->currentIndex() == 1 );
    if (isManual)
        args += "--manual";

    if (!m_logFile.is_open())
    {
        m_logFile.open( UnicornFile::log( Unicorn::Twiddly ).toStdString().c_str() );
        m_logFile.seekg( 0, std::ios_base::end );
        m_logTimer->start( 10 );
    }

    QProcess::startDetached( UnicornFile::executable( Unicorn::Twiddly ) );
#endif
}


void 
DiagnosticsDialog::onSendLogsClicked()
{
    SendLogsDialog( this ).exec();
}
