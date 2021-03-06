 /*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

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
#include "MainWindow.h"
#include "radio/buckets/Amp.h"
#include "radio/buckets/SeedsWidget.h"
#include "Settings.h"
#include "the/app.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/Firehose.h"
#include "widgets/ImageButton.h"
#include "widgets/MessageBar.h"
#include "widgets/PlaylistDialog.h"
#include "widgets/SettingsDialog.h"
#include "widgets/LocalRqlDialog.h"
#include "widgets/ShareDialog.h"
#include "widgets/TagDialog.h"
#include "widgets/TrackDashboard.h"
#include "widgets/TrackDashboardHeader.h"
#include "layouts/SideBySideLayout.h"
#include <lastfm/User>
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include <lastfm/WsReply>
#include <QtGui>

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef WIN32
#include "windows.h"
#endif


MainWindow::MainWindow()
{
    setupUi();
    finishUi();

    connect( ui.settings, SIGNAL(triggered()), SLOT(showSettingsDialog()) );
    connect( ui.viewLocalRql, SIGNAL(triggered()), SLOT(showLocalRqlDialog()) );
    connect( ui.diagnostics_action, SIGNAL(triggered()), ui.diagnostics, SLOT(show()) );
    connect( ui.share, SIGNAL(triggered()), SLOT(showShareDialog()) );
	connect( ui.tag, SIGNAL(triggered()), SLOT(showTagDialog()) );
    connect( ui.playlist, SIGNAL(triggered()), SLOT(showPlaylistDialog()) );

    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
	connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );
    connect( qApp, SIGNAL(error( QString )), ui.messagebar, SLOT(show( QString )) );
    connect( qApp, SIGNAL(status( QString, QString )), ui.messagebar, SLOT(show( QString, QString )) );

    // set up window in default state
    onTrackSpooled( Track() );

	setAcceptDrops( true );

#ifdef SUPER_MEGA_DEBUG
	// causes all events from the whole application to get logged
	qApp->installEventFilter( this );
#endif
}


void
MainWindow::onTrackSpooled( const Track& t )
{  
    m_track = t;
    
    if (!t.isNull())
    {
        ui.share->setEnabled( true );
        ui.tag->setEnabled( true );
        ui.playlist->setEnabled( true );
        ui.love->setEnabled( true );
		ui.love->setChecked( false );

        if (t.source() == Track::LastFmRadio)
            ui.ban->setEnabled( true );

        ui.dashboard->setTrack( t );
    }
    else {
        ui.share->setEnabled( false );
        ui.tag->setEnabled( false );
        ui.love->setEnabled( false );
        ui.playlist->setEnabled( false );
        ui.ban->setEnabled( false );
        
        ui.dashboard->clear();
    }

    #ifndef Q_WS_MAC
        // it's ugly on any platform, but useful on non-mac as the taskbar
        // shows the track that is playing
        setWindowTitle( t.isNull() 
                ? qApp->applicationName()
                : t.toString() );
    #endif
}


void
MainWindow::onStateChanged( State s )
{
	if (s == TuningIn)
    {
        ui.dashboard->tuningIn();
        ui.viewDashboard->trigger();
    }
}


void
MainWindow::setupUi()
{
    ui.setupUi( this );
    
    {
        QActionGroup* ag = new QActionGroup( this );
        ag->addAction( ui.viewSources );
        ag->addAction( ui.viewDashboard );
    }

    setDockOptions( AnimatedDocks | AllowNestedDocks );
    setCentralWidget( new QWidget );
    new QVBoxLayout( centralWidget() );
    centralWidget()->layout()->setContentsMargins( 0, 0, 0, 0 );
    centralWidget()->layout()->setSpacing( 0 );

    QWidget* sourceDashboard = new QWidget( centralWidget() );
    SideBySideLayout* sourceDashboardLayout = new SideBySideLayout( sourceDashboard );

    sourceDashboard->layout()->addWidget( ui.seeds = new SeedsWidget );
    sourceDashboard->layout()->addWidget( ui.dashboard = new TrackDashboard );

    connect( ui.viewSources, SIGNAL( triggered()), sourceDashboardLayout, SLOT( moveBackward()));
    connect( ui.viewDashboard, SIGNAL( triggered()), sourceDashboardLayout, SLOT( moveForward()));
    centralWidget()->layout()->addWidget( sourceDashboard );
    
    // these ones should be on top, so created last
    centralWidget()->layout()->addWidget( ui.dashboardHeader = new TrackDashboardHeader );
    centralWidget()->layout()->addWidget( ui.amp = new Amp );
    
    QVBoxLayout* v = new QVBoxLayout( ui.dashboard );
    v->addWidget( ui.messagebar = new MessageBar );
    v->addStretch();
    v->setMargin( 0 );
    v->setSpacing( 0 );

    addDragHandleWidget( ui.dashboardHeader );
    addDragHandleWidget( ui.amp );
    
    ui.seeds->connectToAmp( ui.amp );
    ui.dashboardHeader->ui.ban->setAction( ui.ban );
    ui.dashboardHeader->ui.love->setAction( ui.love );
    ((ActionButton*)ui.dashboardHeader->ui.scrobbleButton)->setAction( ui.scrobble );
    
    connect( ui.seeds->ui.dashboard, SIGNAL( clicked()), ui.viewDashboard, SLOT( trigger()));
    connect( ui.dashboard->ui.sources, SIGNAL( clicked()), ui.viewSources, SLOT( trigger()));
    
    ui.amp->show();
    ui.dashboardHeader->show();
    
    QMenu* menu = new QMenu( this );
    menu->addAction( ui.tag );
    menu->addAction( ui.share );
    menu->addAction( ui.playlist );
    ui.dashboardHeader->setCogMenu( menu );
    
#ifndef Q_WS_MAC
	delete ui.windowMenu;
#endif

    ui.dashboard->setMinimumHeight( 100 );
    

    setMinimumWidth( 300 );

    ui.seeds->show();
    ui.dashboard->show();

    
    ui.viewSources->setChecked( true );
    
    ui.messagebar->raise();
    
    menu = ui.menubar->addMenu( "Radio Mode" );
    QAction* a1 = menu->addAction( "Local" );
    QAction* a2 = menu->addAction( "Streaming" );

    QActionGroup* g = new QActionGroup( menu );
    g->addAction( a1 );
    g->addAction( a2 );
    g->setExclusive( true );
    
    a1->setCheckable( true );
    a2->setCheckable( true );
    a1->setData( 'L' );
    a2->setData( 'S' );
    
    a2->setChecked( true );
    
    ui.localRadio = a1;
    
    ui.diagnostics = new DiagnosticsDialog( this );
}


void
MainWindow::showSettingsDialog()
{
    if (!ui.settingsDialog) ui.settingsDialog = new SettingsDialog( this );
    ui.settingsDialog.show();
}


void
MainWindow::showLocalRqlDialog()
{
    if (!ui.localRqlDialog) ui.localRqlDialog = new LocalRqlDialog( this );
    ui.localRqlDialog.show();
}


void
MainWindow::showShareDialog()
{
    if (!ui.shareDialog || ui.shareDialog->track() != m_track) ui.shareDialog = new ShareDialog( m_track, this );
    ui.shareDialog.show();
}


void
MainWindow::showTagDialog()
{
    if (!ui.tagDialog || ui.tagDialog->track() != m_track) ui.tagDialog = new TagDialog( m_track, this );
    ui.tagDialog.show();
}


void 
MainWindow::showPlaylistDialog()
{
    if (!ui.playlistDialog || ui.playlistDialog->track() != m_track) ui.playlistDialog = new PlaylistDialog( m_track, this );
    ui.playlistDialog.show();
}


#ifdef WIN32
void
MainWindow::closeEvent( QCloseEvent* event )
{
    //TEST this works on XP as it sure doesn't on Vista

    hide();

    event->ignore();

    // Do animation and fail gracefully if not possible to find systray
    RECT rectFrame;    // animate from
    RECT rectSysTray;  // animate to

    ::GetWindowRect( (HWND)winId(), &rectFrame );

    // Get taskbar window
    HWND taskbarWnd = ::FindWindow( L"Shell_TrayWnd", NULL );
    if ( taskbarWnd == NULL )
        return;

    // Use taskbar window to get position of tray window
    HWND trayWnd = ::FindWindowEx( taskbarWnd, NULL, L"TrayNotifyWnd", NULL );
    if ( trayWnd == NULL )
        return;

    ::GetWindowRect( trayWnd, &rectSysTray );
    ::DrawAnimatedRects( (HWND)winId(), IDANI_CAPTION, &rectFrame, &rectSysTray );

    // Make it release memory as when minimised
    HANDLE h = ::GetCurrentProcess();
    SetProcessWorkingSetSize( h, -1 ,-1 );
}
#endif


void
MainWindow::onSystemTrayIconActivated( const QSystemTrayIcon::ActivationReason reason )
{
    switch (reason)
    {
        case QSystemTrayIcon::Unknown:
        case QSystemTrayIcon::Context:
        case QSystemTrayIcon::MiddleClick:
            break;

        case QSystemTrayIcon::DoubleClick:
          #ifdef WIN32
            show();
            activateWindow();
            raise();
          #endif
            break;

        case QSystemTrayIcon::Trigger:
          #ifdef Q_WS_X11
            if (!isVisible()) 
            {
                show();

                //NOTE don't raise, as this won't work with focus stealing prevention
                //raise();

                QX11Info const i;
                Atom const _NET_ACTIVE_WINDOW = XInternAtom( i.display(), "_NET_ACTIVE_WINDOW", False);

                // this sends the correct demand for window activation to the Window 
                // manager. Thus forcing window activation.
                ///@see http://standards.freedesktop.org/wm-spec/wm-spec-1.3.html#id2506353
                XEvent e;
                e.xclient.type = ClientMessage;
                e.xclient.message_type = _NET_ACTIVE_WINDOW;
                e.xclient.display = i.display();
                e.xclient.window = winId();
                e.xclient.format = 32;
                e.xclient.data.l[0] = 1; // we are a normal application
                e.xclient.data.l[1] = i.appUserTime();
                e.xclient.data.l[2] = qApp->activeWindow() ? qApp->activeWindow()->winId() : 0;
                e.xclient.data.l[3] = 0l;
                e.xclient.data.l[4] = 0l;

                // we send to the root window per fdo NET spec
                XSendEvent( i.display(), i.appRootWindow(), false, SubstructureRedirectMask | SubstructureNotifyMask, &e );
            }
            else
                hide();
          #endif
            break;
    }
}


static QList<QUrl> lastfmUrls( QList<QUrl> urls )
{
	QMutableListIterator<QUrl> i( urls );

	while (i.hasNext())
		if (i.next().scheme() != "lastfm")
			i.remove();
	return urls;
}


static bool couldBeXspf( const QMimeData* md )
{
    if (md->hasFormat("application/xspf+xml"))
        return true;
    foreach(QUrl q, md->urls()) {
        QString path(q.path());
        if (q.path().endsWith(".xspf"))
            return true;
    }
    return false;
}


void
MainWindow::dragEnterEvent( QDragEnterEvent* e )
{
	if (!e->mimeData()->hasUrls())
		return;
	if (lastfmUrls( e->mimeData()->urls() ).count())
		e->accept();
    if (couldBeXspf(e->mimeData()))
        e->accept();
}


void
MainWindow::dropEvent( QDropEvent* e )
{
	QList<QUrl> const urls = lastfmUrls( e->mimeData()->urls() );
	if (urls.count())
		The::app().open( urls[0] );
    else {
        foreach(QUrl q, e->mimeData()->urls()) {
            if (q.path().endsWith(".xspf")) {
                The::app().openXspf( q );
                break;
            }
        }
    }
}


QSize
MainWindow::sizeHint() const
{
    QSize s = ui.amp->sizeHint();
    s.setHeight( s.height() + ui.dashboardHeader->sizeHint().height() 
#ifdef WIN32
		+ menuBar()->sizeHint().height()
#endif	
	);
    return s;
}


bool 
MainWindow::eventFilter( QObject* o, QEvent* event )
{
#ifdef SUPER_MEGA_DEBUG
	qDebug() << o << event;
#endif
	
    QWidget* obj = qobject_cast<QWidget*>( o );
    if (!obj)
        return false;
    
    QMouseEvent* e = static_cast<QMouseEvent*>( event );
    
    switch ((int)e->type())
    {
        case QEvent::MouseButtonPress:
            m_dragHandleMouseDownPos[ obj ] = e->globalPos() - pos();
            break;

        case QEvent::MouseButtonRelease:
            m_dragHandleMouseDownPos[ obj ] = QPoint();
            break;
            
        case QEvent::MouseMove:
            if (m_dragHandleMouseDownPos.contains( obj ) && !m_dragHandleMouseDownPos[ obj ].isNull())
                move( e->globalPos() - m_dragHandleMouseDownPos[ obj ]);
            break;
    }

    return false;
}


void 
MainWindow::addDragHandleWidget( QWidget* w )
{
    w->installEventFilter( this );
}
