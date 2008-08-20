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
#include "lib/radio/RadioController.h"
#include "PlayerEvent.h"
#include "PlayerManager.h"
#include "widgets/DiagnosticsDialog.h"
#include "Scrobble/MetaInfoView.h"
#include "widgets/SettingsDialog.h"
#include "widgets/ShareDialog.h"
#include "widgets/TagDialog.h"
#include "widgets/RadioMiniControls.h"
#include "radio/RadioWidget.h"
#include "version.h"
#include "lib/unicorn/widgets/AboutDialog.h"
#include "lib/radio/AudioPlaybackEngine.h"
#include <QCloseEvent>
#include <QPointer>
#include <QShortcut>
#include <QStackedWidget>
#include <QSplitter>
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
    connect( qApp, SIGNAL(event( int, QVariant )), SLOT(onAppEvent( int, QVariant )) );
    connect( ui.tunein, SIGNAL(triggered()), SLOT(showTuner()) );
}


void
MainWindow::onAppEvent( int e, const QVariant& v )
{
    Q_UNUSED( v ); //--warning mac
    
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
            setWindowTitle( v.value<ObservedTrack>().prettyTitle() );
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
    
	QWidget* mainWidget = new QWidget;
	QVBoxLayout* mainLayout = new QVBoxLayout( mainWidget );
    mainLayout->setSpacing( 0 );
    mainLayout->setMargin( 0 );
    mainLayout->addWidget( ui.stack = new QStackedWidget );
	mainLayout->addWidget( ui.controls = new RadioMiniControls );

    connect( ui.controls->ui.skip, SIGNAL(clicked()), ui.skip, SLOT(trigger()) );
	connect( ui.controls->ui.toggle, SIGNAL(toggled( bool )), SLOT(setTunerToggled( bool )) );
	
	setCentralWidget( mainWidget );

    setupScrobbleView(); //TODO its own widget too
	
	ui.tuner = new RadioWidget;
	connect( ui.tuner, SIGNAL(newStationStarted()), SLOT(showNowPlaying()) );
	
	ui.stack->addWidget( ui.np );
	ui.stack->addWidget( ui.tuner );
}


void 
MainWindow::setupScrobbleView()
{
    ScrobbleViewWidget* w = new ScrobbleViewWidget;
	QHBoxLayout* h = new QHBoxLayout( w->ui.actionbar );
    h->addStretch();
    h->addWidget( new SimpleButton( ":/MainWindow/love.png", ui.love ));
    h->addWidget( new SimpleButton( ":/MainWindow/ban.png", ui.ban ));
    h->addWidget( new SimpleButton( ":/MainWindow/tag.png", ui.tag ));
    h->addWidget( new SimpleButton( ":/MainWindow/share.png", ui.share ));
    h->setSpacing( 0 );
    h->setMargin( 0 );
    h->setSizeConstraint( QLayout::SetFixedSize );
	
	QSplitter* s = new QSplitter( Qt::Vertical );
	s->addWidget( w );
	s->addWidget( new MetaInfoView );
	s->setSizes( QList<int>() << 80 << 80 );
	s->setStretchFactor( 0, 0 );
	s->setStretchFactor( 1, 1 );

	ui.np = s;
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
		Track t = The::app().track(); \
		if (d && d->track() == t) \
			d->activateWindow(); \
		else { \
			THROW_AWAY_DIALOG( Type ) \
			d->setTrack( t ); \
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
MainWindow::setTunerToggled( bool show_tuner )
{
	ui.stack->setCurrentIndex( show_tuner ? 1 : 0 );
	
	if (sender() != ui.controls->ui.toggle)
		ui.controls->ui.toggle->setChecked( show_tuner );
}


void
MainWindow::setRadio( RadioController* r )
{
    Q_ASSERT( r );

	//FIXME encapsulation!!!!!
    ui.controls->ui.volume->setAudioOutput( r->audioPlaybackEngine()->audioOutput() );
	ui.tuner->setRadioController( r );
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
