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
#include "widgets/DiagnosticsDialog.h"
#include "widgets/MetaInfoView.h"
#include "widgets/SettingsDialog.h"
#include "widgets/TrackListView.h"
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


MainWindow::MainWindow()
{
    ui.setupUi( this );

    setCentralWidget( m_trackListView = new TrackListView );
//    setUnifiedTitleAndToolBarOnMac( true );
    delete ui.toolbar;

    QShortcut* close = new QShortcut( QKeySequence( "CTRL+W" ), this );
    connect( close, SIGNAL(triggered()), SLOT(close()) );
    
    connect( ui.meta, SIGNAL(triggered()), SLOT(showMetaInfoView()) );
    connect( ui.about, SIGNAL(triggered()), SLOT(showAboutDialog()) );
    connect( ui.settings, SIGNAL(triggered()), SLOT(showSettingsDialog()) );
    connect( ui.diagnostics, SIGNAL(triggered()), SLOT(showDiagnosticsDialog()) );
    connect( ui.quit, SIGNAL(triggered()), qApp, SLOT(quit()) );
}


void
MainWindow::showSettingsDialog()
{
    SettingsDialog( this ).exec();
}


void
MainWindow::showDiagnosticsDialog()
{
    #define NON_MODAL_MACRO( Type ) \
        static QPointer<Type> d; \
        if (!d) { \
            d = new Type( this ); \
            d->setAttribute( Qt::WA_DeleteOnClose ); \
            d->setWindowFlags( Qt::Dialog ); \
            d->show(); \
        } else \
            d->activateWindow();
            
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
    NON_MODAL_MACRO( MetaInfoView )
}


void
MainWindow::closeEvent( QCloseEvent* event )
{
#ifdef WIN32
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
#else
    QMainWindow::closeEvent( event );
#endif
}


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
