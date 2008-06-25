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

#include "DiagnosticsDialog.h"

#include "PlaybackEvent.h"
#include "Settings.h"
#include "scrobbler/Scrobbler.h"
#include "scrobbler/ScrobbleCache.h"
#include "lib/moose/MooseCommon.h"
#include "lib/moose/TrackInfo.h"

#include <QProcess>
#include <QClipboard>

#include <QByteArray>
#include <QTimer>
#include <QFile>

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
            case Scrobbler::ErrorBadAuthorisation:
            case Scrobbler::ErrorBadTime:
            case Scrobbler::ErrorThreeHardFailures:
                // the state is better represented to the user by the error
                // condition until we successfully rehandshake
                return;
        }

    switch (new_status)
    {
        case Scrobbler::Handshaken:
            scrobbler_handshake_time = QDateTime::currentDateTime();
            // FALL THROUGH
        default:
            scrobbler_status = new_status;
            break;
    }

    emit scrobblerStatusChanged( scrobbler_status ); 
}


static void smallFontise( QWidget* w )
{
    #ifdef WIN32
    return; //small fonts look wrong on Windows
    #endif

    QFont f = w->font();
    #if defined LINUX
        f.setPointSize( f.pointSize() - 2 );
    #else
        f.setPointSize( 10 );
    #endif
    w->setFont( f );
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

    // not possible to do this with designer, and varies by platform
    smallFontise( ui.cachedTracksLabel );
    smallFontise( ui.cachedTracksTitle );
    smallFontise( ui.fingerprintedTracksTitle );

    #ifdef Q_OS_MAC
    layout()->setMargin( 7 );
    ui.cachedTracksList->setAttribute( Qt::WA_MacShowFocusRect, false );
    ui.vboxLayout1->setSpacing( 18 );
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
    connect( The::app().m_fpCollector, SIGNAL( trackFingerprintingStarted( TrackInfo ) ),
        this,                     SLOT( onTrackFingerprintingStarted( TrackInfo ) ),
        Qt::QueuedConnection );
    connect( The::app().m_fpCollector, SIGNAL( trackFingerprinted( TrackInfo ) ),
        this,                     SLOT( onTrackFingerprinted( TrackInfo ) ),
        Qt::QueuedConnection );
    connect( The::app().m_fpCollector, SIGNAL( cantFingerprintTrack( TrackInfo, QString ) ),
        this,                     SLOT( onCantFingerprintTrack( TrackInfo, QString ) ),
        Qt::QueuedConnection );

#endif
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    connect( ui.copyToClipboardButton, SIGNAL( clicked() ), SLOT( onCopyToClipboard() ) );
    connect( ui.scrobbleIpodButton, SIGNAL( clicked() ), SLOT( onScrobbleIpodClicked() ) );

    //FIXME still needed for eg. cache view may not reflect truth
    connect( ui.refreshButton, SIGNAL( clicked() ), SLOT( onRefresh() ) );

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
        case Scrobbler::ErrorBadAuthorisation: return tr( "Your username or password is incorrect" );
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
    ui.scrobblingStatus->setText( s );

    populateScrobbleCacheView();
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
DiagnosticsDialog::onAppEvent( int event, const QVariant& /* data */ )
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
    Q_DEBUG_BLOCK;

    ScrobbleCache cache( The::settings().username() );

    qDebug() << cache.tracks().count();

    QList<QTreeWidgetItem *> items;
    foreach (TrackInfo t, cache.tracks())
        if ( t.isScrobbled() )
            items.append( new QTreeWidgetItem( QStringList() << t.artist() << t.track() << t.album() ) );

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


void
DiagnosticsDialog::onCopyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString clipboardText;

    //TODO should read "Last successful submission" - that's what it actually shows at least
    clipboardText.append( tr( "Last successful connection: " ) + ui.lastConnectionStatusLabel->text() + "\n\n" );
    clipboardText.append( tr( "Submission Server: " ) + ui.scrobblingStatus->text() + "\n" );
    clipboardText.append( ui.cachedTracksLabel->text() + ":\n\n" );

    // Iterate through cached tracks list and add to clipboard contents
    for(int row = 0; row < ui.cachedTracksList->topLevelItemCount(); row++)
    {
        QTreeWidgetItem *rowData = ui.cachedTracksList->topLevelItem( row );
        for(int col = 0; col < rowData->columnCount(); col++)
        {
            clipboardText.append( rowData->data( col, Qt::DisplayRole ).toString() );
            clipboardText.append( "\t:\t" );
        }
        //remove trailing seperators
        clipboardText.chop(3);
        clipboardText.append( "\n" );
    }

    #ifndef HIDE_RADIO
    clipboardText.append("\n" + tr( "Radio Server: " ) + ui.radioServerStatusLabel->text() + "\n" );
    #endif

    clipboard->setText( clipboardText );
}


void
DiagnosticsDialog::onTrackFingerprintingStarted( const TrackInfo& track )
{
#if 0
    ui.fpCurrentTrackLabel->setText( track.toString() );
    ui.fpQueueSizeLabel->setText( QString::number( The::app().m_fpCollector->queueSize() ) );
#endif
}


void
DiagnosticsDialog::onTrackFingerprinted( const TrackInfo& track  )
{
#if 0
    ui.fpCurrentTrackLabel->setText( "" );
    ui.fpQueueSizeLabel->setText( QString::number( The::app().m_fpCollector->queueSize() ) );
    
    new QTreeWidgetItem( ui.fingerprintedTracksList, QStringList() << track.artist() << track.track() << track.album() );
#endif
}


void
DiagnosticsDialog::onCantFingerprintTrack( const TrackInfo& /* track */, QString /* reason */ )
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
    //TODO: make DRY - this is replicated in WizardTwiddlyBootstrapPage.cpp
    #ifdef Q_OS_MAC
        #define TWIDDLY_EXECUTABLE_NAME "/../../Resources/iPodScrobbler"
    #else
        #define TWIDDLY_EXECUTABLE_NAME "/../iPodScrobbler.exe"
    #endif

    QStringList args = (QStringList() 
                    << "--device" << "diagnostic" 
                    << "--vid" << "0000" 
                    << "--pid" << "0000" 
                    << "--serial" << "UNKNOWN");

    bool isManual = ( ui.iPodScrobbleType->currentIndex() == 1 );
    if( isManual )
        args << "--manual";

    if( !m_logFile.is_open() )
    {
        #ifdef Q_WS_MAC
            QString twiddlyLogName = "Last.fm Twiddly.log";
        #else
            QString twiddlyLogName = "Twiddly.log";
        #endif
        
        qDebug() << "Watching log file: " << Moose::logPath( twiddlyLogName );       
 
        m_logFile.open(Moose::logPath( twiddlyLogName ).toStdString().c_str());

        m_logFile.seekg( 0, std::ios_base::end );
        m_logTimer->start( 10 );
    }

    QProcess::startDetached( The::settings().path() + TWIDDLY_EXECUTABLE_NAME, args );
}
