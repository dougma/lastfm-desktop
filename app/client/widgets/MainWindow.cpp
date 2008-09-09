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
#include "PlayerManager.h"
#include "widgets/DiagnosticsDialog.h"
#include "scrobble/ScrobbleViewWidget.h"
#include "widgets/SettingsDialog.h"
#include "widgets/ShareDialog.h"
#include "widgets/TagDialog.h"
#include "widgets/RadioMiniControls.h"
#include "radio/RadioWidget.h"
#include "radio/FriendsTuner.h"
#include "Settings.h"
#include "version.h"
#include "the/radio.h"
#include "lib/types/User.h"
#include "lib/unicorn/widgets/AboutDialog.h"
#include "lib/ws/WsReply.h"
#include <QCloseEvent>
#include <QShortcut>
#include <QStackedWidget>
#include <phonon/volumeslider.h>

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

    QShortcut* close = new QShortcut( QKeySequence( "CTRL+W" ), this );
    connect( close, SIGNAL(activated()), SLOT(close()) );
   
    connect( ui.meta, SIGNAL(triggered()), SLOT(showMetaInfoView()) );
    connect( ui.about, SIGNAL(triggered()), SLOT(showAboutDialog()) );
    connect( ui.settings, SIGNAL(triggered()), SLOT(showSettingsDialog()) );
    connect( ui.diagnostics, SIGNAL(triggered()), SLOT(showDiagnosticsDialog()) );
    connect( ui.share, SIGNAL(triggered()), SLOT(showShareDialog()) );
	connect( ui.tag, SIGNAL(triggered()), SLOT(showTagDialog()) );
    connect( ui.quit, SIGNAL(triggered()), qApp, SLOT(quit()) );
    connect( ui.viewTuner, SIGNAL(triggered()), SLOT(showTuner()) );
	connect( ui.stack, SIGNAL(currentChanged( int )), SLOT(onStackIndexChanged( int )) );

    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
    connect( &The::radio(), SIGNAL(tuningIn( RadioStation )), SLOT(showNowPlaying()) );
    
    // set up window in default state
    onTrackSpooled( Track() );
}


void
MainWindow::onTrackSpooled( const Track& t )
{  
    m_track = t;
    
    if (!t.isNull())
    {
        ui.share->setEnabled( true );
        ui.tag->setEnabled( true );
        ui.love->setEnabled( true );
        
        if (t.source() == Track::LastFmRadio)
            ui.ban->setEnabled( true );
    }
    else {
        ui.share->setEnabled( false );
        ui.tag->setEnabled( false );
        ui.love->setEnabled( false );
        ui.ban->setEnabled( false );
    }
        
    #ifndef Q_WS_MAC
        setWindowTitle( t.isNull() 
                ? qApp->applicationName()
                : t.prettyTitle() );
    #endif
}


void
MainWindow::setupUi()
{
    ui.setupUi( this );
	
	ui.account->setTitle( The::settings().username() );
   	connect( User::getInfo(), SIGNAL(finished( WsReply* )), SLOT(onUserGetInfoReturn( WsReply* )) );
	
	QWidget* mainWidget = new QWidget;
	QVBoxLayout* mainLayout = new QVBoxLayout( mainWidget );
    mainLayout->setSpacing( 0 );
    mainLayout->setMargin( 0 );
	
	mainLayout->addWidget( ui.tabBar = new QTabBar );
    mainLayout->addWidget( ui.stack = new QStackedWidget );
	mainLayout->addWidget( ui.controls = new RadioMiniControls );

	ui.controls->ui.volume->setAudioOutput( The::radio().audioOutput() );
	
	connect( ui.tabBar, SIGNAL( currentChanged( int )), ui.stack, SLOT( setCurrentIndex( int )));
	connect( ui.stack, SIGNAL( currentChanged( int )), ui.tabBar, SLOT( setCurrentIndex( int )));
	
    connect( ui.controls->ui.skip, SIGNAL(clicked()), ui.skip, SLOT(trigger()) );
	connect( ui.controls->ui.toggle, SIGNAL(toggled( bool )), SLOT(setTunerToggled( bool )) );
	connect( ui.controls, SIGNAL(stop()), &The::radio(), SLOT(stop()) );
	
	setCentralWidget( mainWidget );

	ui.tabBar->setDrawBase( false );
	
	//FIXME: I'm not entirely happy with coupling the ScrobbleViewWidget with the MainWindow
	//		 by requiring the Ui object to be passed into the ScrobbleViewWidget but it works
	//		 for now and nicely wraps the love / ban / tag / share actions together.
	addTab( ui.scrobbler = new ScrobbleViewWidget( ui ), "Now Playing" );
	addTab( ui.friendTuner = new FriendsTuner, "Friends" );
	addTab( new QWidget(), "Library" );
	addTab( ui.tuner = new RadioWidget, "Radio");
	
	connect( ui.friendTuner, SIGNAL( tune(RadioStation)), &The::radio(), SLOT( play( RadioStation)));

#ifndef Q_WS_MAC
	delete ui.windowMenu;
#endif
}


void 
MainWindow::addTab( QWidget* w, const QString& t )
{
	ui.tabBar->addTab( t );
	ui.stack->addWidget( w );
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
	#define PER_TRACK_DIALOG( Type ) \
		static QPointer<Type> d; \
		if (d && d->track() == m_track) \
			d->activateWindow(); \
		else { \
			THROW_AWAY_DIALOG( Type ) \
			d->setTrack( m_track ); \
		}
	
	PER_TRACK_DIALOG( ShareDialog )
}


void
MainWindow::showTagDialog()
{
	PER_TRACK_DIALOG( TagDialog )
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


void
MainWindow::setTunerToggled( bool const show_tuner )
{
	ui.stack->setCurrentIndex( show_tuner ? 1 : 0 );
}


void
MainWindow::onStackIndexChanged( int const page )
{
	QAbstractButton* o = ui.controls->ui.toggle;
	o->blockSignals( true ); //avoid recursively calling this function
	o->setChecked( page == 1 );
	o->blockSignals( false );
	
#ifdef Q_WS_MAC
	switch (page)
	{
		case 0: setWindowTitle( "Last.fm" ); break;
		case 1: setWindowTitle( tr("Tuner" ) ); break;	
	}
#endif
}


QSize
MainWindow::sizeHint() const
{
    return QSize( 300, 300 );
}


static QList<QUrl> lastfmUrls( QList<QUrl> urls )
{
	QMutableListIterator<QUrl> i( urls );

	while (i.hasNext())
		if (i.next().scheme() != "lastfm")
			i.remove();
	return urls;
}


void
MainWindow::dragEnterEvent( QDragEnterEvent* e )
{
	if (!e->mimeData()->hasUrls())
		return;
	if (lastfmUrls( e->mimeData()->urls() ).count())
		e->accept();
}


void
MainWindow::dropEvent( QDropEvent* e )
{
	QList<QUrl> urls = lastfmUrls( e->mimeData()->urls() );
	
	if (urls.count())
		The::app().open( urls[0] );
}


void
MainWindow::onUserGetInfoReturn( WsReply* reply )
{
	try
	{
		EasyDomElement e = reply->lfm()["user"];
		QString gender = e["gender"].text();
		QString age = e["age"].text();
		uint scrobbles = e["playcount"].text().toUInt();
		if (gender.size() && gender.size() && scrobbles > 0)
		{
			gender = (gender == "m") ? "boy" : "girl";
			QString text = tr("A %1, %2 years of age with %L3 scrobbles").arg( gender ).arg( age ).arg( scrobbles );
			ui.account->addAction( text )->setEnabled( false );
		}
		else if (scrobbles > 0)
		{
			ui.account->addAction( tr("%L1 scrobbles").arg( scrobbles ) )->setEnabled( false );			
		}
	}
	catch (EasyDomElement::Exception&)
	{}
}
