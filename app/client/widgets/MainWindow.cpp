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
#include "MainWindow/CogButtonPopup.h"
#include "MainWindow/MediaPlayerIndicator.h"
#include "MainWindow/PrettyCoverWidget.h"
#include "radio/RadioWidget.h"
#include "radio/buckets/PrimaryBucket.h"
#include "Settings.h"
#include "the/app.h"
#include "widgets/BottomBar.h"
#include "widgets/DiagnosticsDialog.h"
#include "widgets/Firehose.h"
#include "widgets/ImageButton.h"
#include "widgets/MetaInfoView.h"
#include "widgets/PlaylistDialog.h"
#include "widgets/SettingsDialog.h"
#include "widgets/ShareDialog.h"
#include "widgets/TagDialog.h"
#include "lib/lastfm/types/User.h"
#include "lib/unicorn/widgets/AboutDialog.h"
#include "lib/unicorn/widgets/SpinnerLabel.h"
#include "lib/lastfm/ws/WsReply.h"
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDockWidget>
#include <QShortcut>

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef WIN32
#include "windows.h"
#endif

#define SETTINGS_POSITION_KEY "MainWindowPosition"



MainWindow::MainWindow()
{    
    setupUi();

    QShortcut* close = new QShortcut( QKeySequence( "CTRL+W" ), this );
    close->setContext( Qt::ApplicationShortcut );
    connect( close, SIGNAL(activated()), SLOT(closeActiveWindow()) );

    connect( ui.about, SIGNAL(triggered()), SLOT(showAboutDialog()) );
    connect( ui.settings, SIGNAL(triggered()), SLOT(showSettingsDialog()) );
    connect( ui.diagnostics, SIGNAL(triggered()), SLOT(showDiagnosticsDialog()) );
    connect( ui.share, SIGNAL(triggered()), SLOT(showShareDialog()) );
	connect( ui.tag, SIGNAL(triggered()), SLOT(showTagDialog()) );
    connect( ui.quit, SIGNAL(triggered()), qApp, SLOT(quit()) );
    connect( ui.cog, SIGNAL(clicked()), SLOT(showCogMenu()) );

    connect( qApp, SIGNAL(trackSpooled( Track )), SLOT(onTrackSpooled( Track )) );
	connect( qApp, SIGNAL(stateChanged( State )), SLOT(onStateChanged( State )) );

    // set up window in default state
    onTrackSpooled( Track() );

    QVariant v = QSettings().value( SETTINGS_POSITION_KEY );
    if (v.isValid()) move( v.toPoint() ); //if null, let Qt decide
    
    // doing this properly and trying to manipulate the sizeHint and that is
    // nigh on impossible because Qt sucks at this stuff
    ui.cover->setMinimumHeight( 278 );
    ui.info->setMinimumHeight( 200 );
    resize( 100, 100 );
}


MainWindow::~MainWindow()
{
    QSettings().setValue( SETTINGS_POSITION_KEY, pos() );
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
		ui.love->setChecked( false );
        ui.cog->setEnabled( true );
        
        if (t.source() == Track::LastFmRadio)
            ui.ban->setEnabled( true );
    }
    else {
        ui.share->setEnabled( false );
        ui.tag->setEnabled( false );
        ui.love->setEnabled( false );
        ui.ban->setEnabled( false );
        ui.cog->setEnabled( false );
    }
        
    #ifndef Q_WS_MAC
        // it's ugly on any platform, but useful on non-mac as the taskbar
        // shows the track that is playing
        setWindowTitle( t.isNull() 
                ? qApp->applicationName()
                : t.toString() );
    #endif

    if (!t.isNull())
    {
        //TODO handle bad data! eg no artist, no track
        ui.text->setText( "<div style='margin-bottom:3px'>" + t.artist() + "</div><div><b>" + t.title() );
        ui.cover->setTrack( t );
    }
    else {
        ui.text->clear();
        ui.cover->clear();        
    }    
}


void
MainWindow::onStateChanged( State s )
{
	switch (s)
	{            
        case TuningIn:
			ui.cover->ui.spinner->show();
			break;
            
        default:
            break;
	}
}


void
MainWindow::setupUi()
{
    ui.setupUi( this );
	
    AuthenticatedUser user;
    ui.account->setTitle( user );
   	connect( user.getInfo(), SIGNAL(finished( WsReply* )), SLOT(onUserGetInfoReturn( WsReply* )) );
    
	setupCentralWidget();
    setDockOptions( AnimatedDocks | AllowNestedDocks );
    /** hah! works :) But I'm sure is hideously dangerous, etc. */
    setStatusBar( (QStatusBar*) (ui.bottombar = new BottomBar) );

    QDockWidget* dw;
    dw = new QDockWidget;
    dw->setWindowTitle( tr("Radio") );
    dw->setWidget( ui.primaryBucket = new PrimaryBucket );
    addDockWidget( Qt::BottomDockWidgetArea, dw, Qt::Vertical );
    ui.bottombar->ui.radio->setWidget( dw );
    ui.bottombar->ui.radio->setToolTip( dw->windowTitle() );
    dw->hide();
    connect( ui.viewRadio, SIGNAL(triggered()), dw, SLOT(show()) );
    
    dw = new QDockWidget;
    dw->setWindowTitle( "Friends" );
    dw->setWidget( new Firehose );
    addDockWidget( Qt::LeftDockWidgetArea, dw, Qt::Vertical );
    ui.bottombar->ui.friends->setWidget( dw );
    ui.bottombar->ui.friends->setToolTip( dw->windowTitle() );
    dw->hide();
    
    dw = new QDockWidget;
    dw->setWindowTitle( "Info" );
    dw->setWidget( ui.info = new MetaInfoView );
    addDockWidget( Qt::RightDockWidgetArea, dw, Qt::Vertical );
    ui.bottombar->ui.library->setWidget( dw );
    ui.bottombar->ui.library->setToolTip( dw->windowTitle() );
    dw->hide();
    connect( ui.viewInfo, SIGNAL(triggered()), dw, SLOT(show()) );
    
#ifndef Q_WS_MAC
	delete ui.windowMenu;
#endif
}


void
MainWindow::setupCentralWidget()
{       
    QWidget* actionbar, *indicator;
    ImageButton* love, *ban, *share, *tag;

    setCentralWidget( new QWidget );
    
	QVBoxLayout* v = new QVBoxLayout( centralWidget() );
	v->addWidget( indicator = new MediaPlayerIndicator );
	v->addSpacing( 10 );
    v->addWidget( ui.cover = new PrettyCoverWidget );
    v->setStretchFactor( ui.cover, 1 );
    v->setContentsMargins( 9, 9, 9, 0 );
    v->setSpacing( 0 );
    
    indicator->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
    
    QVBoxLayout* v2 = new QVBoxLayout( ui.cover );
	v2->addStretch();
    v2->addWidget( ui.text = new QLabel );
	v2->addSpacing( 4 );
    v2->addWidget( actionbar = new QWidget );
	v2->addSpacing( 5 );
    v2->setAlignment( actionbar, Qt::AlignCenter );    
    v2->setMargin( 0 );
    v2->setSpacing( 0 );

	QHBoxLayout* h = new QHBoxLayout( actionbar );
    h->addWidget( love = new ImageButton( ":/MainWindow/button/love/up.png" ) );
	h->addWidget( ban = new ImageButton( ":/MainWindow/button/ban/up.png" ) );
	h->addWidget( ui.cog = new ImageButton( ":/MainWindow/button/cog/up.png") );
    h->addWidget( tag = new ImageButton( ":/MainWindow/button/tag/up.png" ) );
    h->addWidget( share = new ImageButton( ":/MainWindow/button/share/up.png" ) );   
	h->setSpacing( 24 );
    h->setSizeConstraint( QLayout::SetFixedSize );
    
    ban->moveIcon( 0, 1, QIcon::Active );
    ban->setAction( ui.ban );
    tag->moveIcon( 0, 1, QIcon::Active );
    tag->setAction( ui.tag );
    love->moveIcon( 0, 1, QIcon::Active );
    love->setPixmap( ":/MainWindow/button/love/checked.png", QIcon::On );
	love->setCheckable( true );
    love->setAction( ui.love );
    share->moveIcon( 0, 1, QIcon::Active );
    share->setAction( ui.share );

    ui.text->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
    ui.text->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
    ui.text->setTextFormat( Qt::RichText );            
    
    UnicornWidget::paintItBlack( centralWidget() );
    centralWidget()->setAutoFillBackground( true );

    QPalette p = centralWidget()->palette();
    p.setColor( QPalette::Text, Qt::white );
    p.setColor( QPalette::WindowText, Qt::white );
    ui.text->setPalette( p );

#ifdef Q_WS_MAC
    ui.text->setAttribute( Qt::WA_MacSmallSize );
    
    // Qt-mac sucks for some reason
    foreach (QLabel* l, indicator->findChildren<QLabel*>())
        l->setPalette( p );
#else
    indicator->setPalette( p );
#endif
}


void
MainWindow::showCogMenu()
{
    int const w = centralWidget()->sizeHint().width();
    CogButtonPopup* cog = new CogButtonPopup( w, centralWidget() );
    cog->show();
    
    connect( cog, SIGNAL(addToPlaylistClicked()), SLOT(showPlaylistDialog()) );
}


void
MainWindow::showSettingsDialog()
{
    UNICORN_UNIQUE_DIALOG( SettingsDialog )
}


void
MainWindow::showDiagnosticsDialog()
{
    UNICORN_UNIQUE_DIALOG( DiagnosticsDialog )
}


void
MainWindow::showAboutDialog()
{
    AboutDialog( this ).exec();
}


void
MainWindow::showPlaylistDialog()
{
    UNICORN_UNIQUE_PER_TRACK_DIALOG( PlaylistDialog, m_track )
}


void
MainWindow::showShareDialog()
{
	UNICORN_UNIQUE_PER_TRACK_DIALOG( ShareDialog, m_track )
}


void
MainWindow::showTagDialog()
{
	UNICORN_UNIQUE_PER_TRACK_DIALOG( TagDialog, m_track )
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
	QList<QUrl> const urls = lastfmUrls( e->mimeData()->urls() );
	
	if (urls.count())
		The::app().open( urls[0] );
}


void
MainWindow::onUserGetInfoReturn( WsReply* reply )
{
	try
	{
		CoreDomElement e = reply->lfm()["user"];
		QString gender = e["gender"].text();
		QString age = e["age"].text();
		uint const scrobbles = e["playcount"].text().toUInt();
		if (gender.size() && age.size() && scrobbles > 0)
		{
			QString text = tr("A %1, %2 years of age with %L3 scrobbles")
					.arg( gender == "m" ? tr("boy") : tr("girl") )
					.arg( age )
					.arg( scrobbles );

			ui.account->addAction( text )->setEnabled( false );
		}
		else if (scrobbles > 0)
		{
			ui.account->addAction( tr("%L1 scrobbles").arg( scrobbles ) )->setEnabled( false );
		}
    }
	catch (CoreDomElement::Exception&)
	{}
}


void
MainWindow::closeActiveWindow()
{
    // I hummed and haaa'd about putting this here or in App.cpp, but it seems
    // like if I was a n00b, I'd look here first, so I put it here
    
    QWidget* w = qApp->activeWindow();
    if (w) w->close();
}
