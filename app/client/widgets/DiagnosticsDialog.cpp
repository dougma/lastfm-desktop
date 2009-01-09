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
#include "widgets/SendLogsDialog.h"
#include "lib/lastfm/core/CoreDir.h"
#include "lib/lastfm/scrobble/Scrobbler.h"
#include "lib/lastfm/scrobble/ScrobbleCache.h"
#include "lib/lastfm/scrobble/Scrobble.h"
#include "lib/lastfm/ws/WsKeys.h"
#include <QByteArray>
#include <QClipboard>
#include <QFile>
#include <QHeaderView>
#include <QTimer>
#include <QProcess>


DiagnosticsDialog::DiagnosticsDialog( QWidget *parent )
        : QDialog( parent )
{
    ui.setupUi( this );

    ui.cached->header()->setResizeMode( QHeaderView::Stretch );
    ui.fingerprints->header()->setResizeMode( QHeaderView::Stretch );
    
    m_delay = new DelayedLabelText( ui.subs_status );
    
#ifdef Q_WS_X11
    ui.tabs->removeTab( 3 );
#endif

#ifdef Q_OS_MAC
    ui.subs_status->setAttribute( Qt::WA_MacSmallSize );
    ui.subs_cache_count->setAttribute( Qt::WA_MacSmallSize );
    ui.fingerprints_title->setAttribute( Qt::WA_MacSmallSize );
    ui.cached->setAttribute( Qt::WA_MacSmallSize );
    ui.cached->setAttribute( Qt::WA_MacShowFocusRect, false );
    ui.fingerprints->setAttribute( Qt::WA_MacSmallSize );
    ui.fingerprints->setAttribute( Qt::WA_MacShowFocusRect, false );
#endif
    
    connect( qApp, SIGNAL(scrobblePointReached( Track )), SLOT(onScrobblePointReached()), Qt::QueuedConnection ); // queued because otherwise cache isn't filled yet
    connect( ui.ipod_scrobble_button, SIGNAL(clicked()), SLOT(onScrobbleIPodClicked()) );

    m_logTimer = new QTimer( this );
    connect( m_logTimer, SIGNAL(timeout()), SLOT(onLogPoll()) );

    onScrobblePointReached();
}


DiagnosticsDialog::~DiagnosticsDialog()
{
    if (m_logFile.is_open())
        m_logFile.close();
}


static QString scrobblerStatusText( int const i )
{
    #define tr QObject::tr
    switch (i)
    {
        case Scrobbler::ErrorBadSession: return tr( "Your session expired, it is being renewed." );
        case Scrobbler::ErrorBannedClientVersion: return tr( "Your client too old, you must upgrade." );
        case Scrobbler::ErrorInvalidSessionKey: return tr( "Your username or password is incorrect" );
        case Scrobbler::ErrorBadTime: return tr( "Your timezone or date are incorrect" );
        case Scrobbler::ErrorThreeHardFailures: return tr( "The submissions server is down" );

        case Scrobbler::Connecting: return tr( "Connecting to Last.fm..." );
        case Scrobbler::Scrobbling: return tr( "Scrobbling..." );

        case Scrobbler::TracksScrobbled:
        case Scrobbler::Handshaken:
            return tr( "Ready" );
    }
    #undef tr

    return "";
}


void
DiagnosticsDialog::scrobbleActivity( int msg )
{
    m_delay->add( scrobblerStatusText( msg ) );

    if (msg == Scrobbler::TracksScrobbled)
        QTimer::singleShot( 1000, this, SLOT(onScrobblePointReached()) );

    switch (msg)
    {
        case Scrobbler::ErrorBadSession:
            //TODO flashing
        case Scrobbler::Connecting:
            //NOTE we only get this on startup
            ui.subs_light->setColor( Qt::yellow );
            break;
        case Scrobbler::Handshaken:
        case Scrobbler::Scrobbling:
        case Scrobbler::TracksScrobbled:
            ui.subs_light->setColor( Qt::green );
            break;
        
        case Scrobbler::ErrorBannedClientVersion:
        case Scrobbler::ErrorInvalidSessionKey:
        case Scrobbler::ErrorBadTime:
        case Scrobbler::ErrorThreeHardFailures:
            ui.subs_light->setColor( Qt::red );
            break;
    }
}


void
DiagnosticsDialog::onScrobblePointReached()
{    
    ScrobbleCache cache( Ws::Username );

    QList<QTreeWidgetItem *> items;
    foreach (Track t, cache.tracks())
        items.append( new QTreeWidgetItem( QStringList() << t.artist() << t.title() << t.album() ) );
    ui.cached->clear();
    ui.cached->insertTopLevelItems( 0, items );

    if (items.count())
        ui.subs_cache_count->setText( tr("%1 locally cached tracks").arg( items.count() ) );
    else
        ui.subs_cache_count->clear();
}

void
DiagnosticsDialog::fingerprinted( const Track& t )
{
    new QTreeWidgetItem( ui.fingerprints,
                         QStringList() << t.artist() << t.title() << t.album() );
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

        ui.ipod_log->addItem( data );
        ui.ipod_log->scrollToBottom();
    }
    delete[] dataBuffer;
}


#include "app/twiddly.h"
#include "lib/unicorn/UnicornCoreApplication.h"
#include "lib/lastfm/core/UniqueApplication.h"
void
DiagnosticsDialog::onScrobbleIPodClicked()
{
#ifndef Q_WS_X11
    QStringList args = (QStringList() 
                    << "--device" << "diagnostic" 
                    << "--vid" << "0000" 
                    << "--pid" << "0000" 
                    << "--serial" << "UNKNOWN");

    bool const isManual = ( ui.ipod_type->currentIndex() == 1 );
    if (isManual)
        args += "--manual";

    if (!m_logFile.is_open())
    {
        QString const path = UnicornCoreApplication::log( twiddly::applicationName() ).absoluteFilePath();
        m_logFile.open( path.toStdString().c_str() );
        m_logFile.seekg( 0, std::ios_base::end );
        m_logTimer->start( 10 );
    }

    QProcess::startDetached( twiddly::path(), args );
#endif
}


void 
DiagnosticsDialog::onSendLogsClicked()
{
    SendLogsDialog( this ).exec();
}
