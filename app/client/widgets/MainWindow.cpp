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

#include "MainWindow.h"
#include "App.h"
#include "PlayerEvent.h"
#include "PlayerManager.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MediaPlayerIndicator.h"
#include "widgets/MetaInfoView.h"
#include "widgets/NowPlayingView.h"
#include "widgets/ScrobbleProgressBar.h"
#include "widgets/SettingsDialog.h"
#include "widgets/ShareDialog.h"
#include "version.h"
#include "lib/unicorn/widgets/AboutDialog.h"
#include <QCloseEvent>
#include <QPointer>
#include <QShortcut>

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef WIN32
#include "windows.h"
#endif


QLabel* label( const QString& path ) { QPixmap p( path ); QLabel*l = new QLabel; l->setPixmap( p ); l->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ); return l; }


MainWindow::MainWindow()
{
    setupUi();

    QShortcut* close = new QShortcut( QKeySequence( "CTRL+W" ), this );
    connect( close, SIGNAL(activated()), SLOT(close()) );
   
    connect( ui.meta, SIGNAL(triggered()), SLOT(showMetaInfoView()) );
    connect( ui.about, SIGNAL(triggered()), SLOT(showAboutDialog()) );
    connect( ui.settings, SIGNAL(triggered()), SLOT(showSettingsDialog()) );
    connect( ui.diagnostics, SIGNAL(triggered()), SLOT(showDiagnosticsDialog()) );
    connect( ui.share, SIGNAL(triggered()), SLOT(showShareDialog()) );
    connect( ui.quit, SIGNAL(triggered()), qApp, SLOT(quit()) );
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
}


void
MainWindow::onAppEvent( int e, const QVariant& v )
{
    switch (e)
    {
    case PlayerEvent::PlaybackStarted:
    case PlayerEvent::TrackChanged:
        {
            ui.share->setEnabled( true );
            ui.tag->setEnabled( true );
            ui.love->setEnabled( true );
            ui.ban->setEnabled( true );

        #ifndef Q_WS_MAC
            Track t = v.value<ObservedTrack>();
            setWindowTitle( t.prettyTitle() );
        #endif
            break;
        }

    case PlayerEvent::PlaybackEnded:
        ui.share->setEnabled( false );
        ui.tag->setEnabled( false );
        ui.love->setEnabled( false );
        ui.ban->setEnabled( false );

    #ifndef Q_WS_MAC
        setWindowTitle( qApp->applicationName() );
    #endif
        break;
    }
}


void
MainWindow::setupUi()
{
    ui.setupUi( this );
    ui.toolbar->hide(); // no longer used

    ui.actionbar = new QWidget;
    QHBoxLayout* h2 = new QHBoxLayout( ui.actionbar );
    h2->addStretch();
    h2->addWidget( label( ":/MainWindow/love.png" ) );
    h2->addWidget( label( ":/MainWindow/ban.png" ) );
    h2->addWidget( label( ":/MainWindow/tag.png" ) );
    h2->addWidget( label( ":/MainWindow/share.png" ) );
    h2->setSpacing( 1 );
    h2->setMargin( 0 );
    h2->setSizeConstraint( QLayout::SetFixedSize );

#if 0
    QWidget* w = ui.nowPlaying = new NowPlayingView;
    w->setPalette( QPalette( Qt::white, Qt::black ) );
    w->setAutoFillBackground( true );

    QHBoxLayout* h = new QHBoxLayout;
    h->addStretch();
    h->addWidget( ui.label = new QLabel );
    h->setMargin( 10 );

    QVBoxLayout* v2 = new QVBoxLayout;
    v2->addWidget( new MediaPlayerIndicator );
    v2->addWidget( ui.progress = new ScrobbleProgressBar );
    v2->setMargin( 11 );

    QVBoxLayout* v = new QVBoxLayout( w );
    v->addLayout( h );
    v->setStretchFactor( h, 1 );
    v->addLayout( v2 );
    v->addWidget( actionbar );
    v->setAlignment( actionbar, Qt::AlignCenter );
    v->setMargin( 0 );
#endif

    QWidget* w = new QWidget;

    QVBoxLayout* iv = new QVBoxLayout;
    iv->addWidget( ui.cover = new NowPlayingView );
    iv->addWidget( ui.progress = new ScrobbleProgressBar );
    iv->setMargin( 10 );
    iv->setSpacing( 0 );

    QVBoxLayout* v = new QVBoxLayout( w );
    v->addLayout( iv );
    v->addWidget( ui.actionbar );
    v->setMargin( 0 );
    v->setAlignment( ui.actionbar, Qt::AlignCenter );

    setCentralWidget( w );

    // make minimum and default size so the content area is square
    QSize const s = ui.actionbar->sizeHint();
    w->setMinimumSize( s.width(), s.width() );

    QLinearGradient g( QPoint(), w->rect().bottomLeft() );
    g.setColorAt( 0, Qt::black );
    g.setColorAt( 0.75, QColor( 0x2b, 0x2b, 0x2b ) );

    QPalette p( Qt::white, Qt::black );
    p.setBrush( QPalette::Window, g );
    w->setPalette( p );
    w->setAutoFillBackground( true );
}


void
MainWindow::showSettingsDialog()
{
	#define THROW_AWAY_DIALOG( Type ) \
		d = new Type( this ); \
		d->setAttribute( Qt::WA_DeleteOnClose ); \
		d->setWindowFlags( Qt::Dialog ); \
		d->setModal( false ); \
		d->show(); \
	
    #define NON_MODAL_MACRO( Type ) \
        static QPointer<Type> d; \
        if (!d) { \
			THROW_AWAY_DIALOG( Type ); \
        } else \
            d->activateWindow();

    NON_MODAL_MACRO( SettingsDialog )
}


void
MainWindow::showDiagnosticsDialog()
{
    NON_MODAL_MACRO( DiagnosticsDialog )
}


void
MainWindow::showAboutDialog()
{
    AboutDialog( VERSION, this ).exec();
}


void
MainWindow::showMetaInfoView()
{

}


void
MainWindow::showShareDialog()
{
	// Show non modal ShareDialogs, one for every track played
	// As the user requests them anyway...
	
	static QPointer<ShareDialog> d;
	Track t = The::app().track();
	if (d && d->track() == t)
		d->activateWindow();
	else {
		THROW_AWAY_DIALOG( ShareDialog )
		d->setTrack( t );
	}
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
