/***************************************************************************
 *   Copyright (C) 2005 - 2008 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
 *      Erik Jaelevik, Last.fm Ltd <erik@last.fm>                          *
 *      Max Howell, Last.fm Ltd <max@last.fm>                              *
 *      Jono Cole, Last.fm Ltd <jono@last.fm>                              *
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

#include "container.h"

#if 0
#include "aboutdialog.h"
#include "autoupdater.h"
#include "configwizard.h"
#endif
#include "confirmdialog.h"
#if 0
#include "MooseCommon.h"
#include "deleteuserdialog.h"
#include "failedlogindialog.h"
#endif
#include "iconshack.h"
#if 0
#include "lastfmapplication.h"
#include "LastMessageBox.h"
#include "logger.h"
#include "loginwidget.h"
#include "mediadevices/ipod/IpodDevice.h"
#include "MediaDeviceScrobbler.h"
#include "metadata.h"
#include "MetaDataWidget.h"
#include "playerlistener.h"
#include "Radio.h"
#include "ShareDialog.h"
#include "settingsdialog.h"
#include "DiagnosticsDialog.h"
#include "Scrobbler-1.2.h"
#include "LastFmSettings.h"
#endif
#include "MetaDataWidget.h"
#include "RestStateWidget.h"
#include "SideBarView.h"
#include "systray.h"
#include "volumeslider.h"
#if 0
#include "tagdialog.h"
#include "RestStateWidget.h"
#include "updatewizard.h"
#include "User.h"

#include "WebService.h"
#include "WebService/Request.h"
#endif

#include "UglySettings.h"

#include "lib/unicorn/widgets/AboutDialog.h"
#include "lib/lastfm/scrobble/Scrobble.h"

#include <QShortcut>
#include <QLabel>
#include <QCloseEvent>
#include <QDragMoveEvent>
#include <QDesktopServices>
#include <QFileDialog>

#if 0
#ifndef Q_WS_MAC
    #include "winstyleoverrides.h"
#else
    #include "macstyleoverrides.h"
#endif
#endif


Container::Container()
        : QMainWindow(),
          m_userCheck( false ),
          m_sidebarEnabled( false ),
          m_sidebarWidth( 190 )
#ifndef Q_WS_MAC
        , m_styleOverrides( 0 )
#endif
{    
//TODO    m_shareDialog = new ShareDialog( this );
//TODO    m_diagnosticsDialog = new DiagnosticsDialog( this );
//TODO    m_updater = new CAutoUpdater( this );

    setupUi();
    setupTimeBar();
    setupTrayIcon();
    updateAppearance();
    applyPlatformSpecificTweaks();
    applyMenuTweaks();
    setupConnections();
    restoreState();
        
    // we must restore state here as we save it in toggleSidebar in order to get
    // round the bug in Qt where saveState for the splitter is lost for hidden widgets
    m_sidebarEnabled = !The::currentUser().sidebarEnabled();
    toggleSidebar();

    QPixmap pix = m_sidebarEnabled
            ? IconShack::instance().GetGoodUserIconCollapsed( The::currentUser().icon() )
            : IconShack::instance().GetGoodUserIconExpanded( The::currentUser().icon() );
    ui.actionMyProfile->setIcon( QIcon( pix ) );

    bool const enabled = The::currentUser().isLogToProfile();
    ui.songTimeBar->setScrobblingEnabled( enabled );
    ui.actionToggleScrobbling->setChecked( enabled );
    ui.actionToggleDiscoveryMode->setChecked( The::currentUser().isDiscovery() );
    ui.scrobbleLabel->setEnabled( enabled );    

//TODO    ui.actionToggleDiscoveryMode->setEnabled( The::user().isSubscriber() );
    
    connect( qApp, SIGNAL(stateChanged( State, Track )), SLOT(onAppStateChanged( State, Track )) );
    connect( qApp, SIGNAL(trackSpooled( Track, StopWatch* )), SLOT(onTrackSpooled( Track, StopWatch* )) );
    connect( qApp, SIGNAL(scrobblePointReached( Track )), SLOT(onScrobblePointReached( Track )) );
    
    ui.stack->setBackgroundRole( QPalette::Base );
}


void
Container::setupUi()
{
    ui.setupUi( this );

    ui.actionCheckForUpdates->setMenuRole( QAction::ApplicationSpecificRole );
    ui.actionAboutLastfm->setMenuRole( QAction::AboutRole );
    ui.actionSettings->setMenuRole( QAction::PreferencesRole );
    ui.actionStop->setVisible( false );
    ui.statusbar->addPermanentWidget( ui.scrobbleLabel = new ScrobbleLabel );

////// PlayControls
    QWidget *playControls = new QWidget;
    ui.playcontrols.setupUi( playControls );
    ui.toolbar->addWidget( playControls );

////// splitter
    ui.splitter->setCollapsible( 0, false );
    ui.splitter->setCollapsible( 1, false );
    ui.splitter->setStretchFactor( 0, 0 );
    ui.splitter->setStretchFactor( 1, 1 );

////// force a white background as we don't support colour variants, soz 
    QPalette p = centralWidget()->palette();
    p.setBrush( QPalette::Base, Qt::white );
    p.setBrush( QPalette::Text, Qt::black );
    centralWidget()->setPalette( p );

////// SideBar
    ui.sidebar = new SideBarTree( this );
    ui.sidebarFrame->layout()->addWidget( ui.sidebar );
    ui.sidebarFrame->hide();
    connect( ui.sidebar, SIGNAL( statusMessage( QString ) ), SLOT( statusMessage( QString ) ) );
    connect( ui.sidebar, SIGNAL( plsShowRestState() ), SLOT( showRestState() ) );
    connect( ui.sidebar, SIGNAL( plsShowNowPlaying() ), SLOT( showMetaDataWidget() ) );
    
///// Main bit
    ui.stack->addWidget( ui.rest = new RestStateWidget );
    ui.stack->addWidget( ui.meta = new MetaDataWidget );
    
    connect( ui.rest, SIGNAL(play( QUrl )), SIGNAL(play( QUrl )) );
}


void
Container::setupTimeBar()
{
    // Station bar grey bg colour
    const QColor k_stationBarGreyTop( 0xba, 0xba, 0xba, 0xff );
    const QColor k_stationBarGreyMiddle( 0xe2, 0xe2, 0xe2, 0xff );
    const QColor k_stationBarGreyBottom( 0xff, 0xff, 0xff, 0xff );

    // Track bar blue bg colour
    const QColor k_trackBarBkgrBlueTop( 0xeb, 0xf0, 0xf2, 0xff );
    const QColor k_trackBarBkgrBlueMiddle( 0xe5, 0xe9, 0xec, 0xff );
    const QColor k_trackBarBkgrBlueBottom( 0xdc, 0xe2, 0xe5, 0xff );

    // Track bar progress bar colour
    const QColor k_trackBarProgressTop( 0xd6, 0xde, 0xe6, 0xff );
    const QColor k_trackBarProgressMiddle( 0xd0, 0xd9, 0xe2, 0xff );
    const QColor k_trackBarProgressBottom( 0xca, 0xd4, 0xdc, 0xff );

    // Track bar scrobbled colour
    const QColor k_trackBarScrobbledTop( 0xba, 0xc7, 0xd7, 0xff );
    const QColor k_trackBarScrobbledMiddle( 0xb8, 0xc4, 0xd5, 0xff );
    const QColor k_trackBarScrobbledBottom( 0xb5, 0xc1, 0xd2, 0xff );

    ////// song bar and scrobble bar
    ui.songTimeBar->setEnabled( false );
    ui.songTimeBar->setItemType( UnicornEnums::ItemTrack );

    QLinearGradient trackBarBkgrGradient( 0, 0, 0, ui.songTimeBar->height() );
    trackBarBkgrGradient.setColorAt( 0, k_trackBarBkgrBlueTop );
    trackBarBkgrGradient.setColorAt( 0.5, k_trackBarBkgrBlueMiddle );
    trackBarBkgrGradient.setColorAt( 0.51, k_trackBarBkgrBlueBottom );
    trackBarBkgrGradient.setColorAt( 1, k_trackBarBkgrBlueBottom );

    QLinearGradient trackBarProgressGradient( 0, 0, 0, ui.songTimeBar->height() );
    trackBarProgressGradient.setColorAt( 0, k_trackBarProgressTop );
    trackBarProgressGradient.setColorAt( 0.5, k_trackBarProgressMiddle );
    trackBarProgressGradient.setColorAt( 0.51, k_trackBarProgressBottom );
    trackBarProgressGradient.setColorAt( 1, k_trackBarProgressBottom );

    QLinearGradient trackBarScrobbledGradient( 0, 0, 0, ui.songTimeBar->height() );
    trackBarScrobbledGradient.setColorAt( 0, k_trackBarScrobbledTop );
    trackBarScrobbledGradient.setColorAt( 0.5, k_trackBarScrobbledMiddle );
    trackBarScrobbledGradient.setColorAt( 0.51, k_trackBarScrobbledBottom );
    trackBarScrobbledGradient.setColorAt( 1, k_trackBarScrobbledBottom );

    ui.songTimeBar->setBackgroundGradient( trackBarBkgrGradient );
    ui.songTimeBar->setForegroundGradient( trackBarProgressGradient );
    ui.songTimeBar->setScrobbledGradient( trackBarScrobbledGradient );

    ui.stationTimeBar->setProgressEnabled( false );
    ui.stationTimeBar->setVisible( false );
    ui.stationTimeBar->setItemType( UnicornEnums::ItemStation );

    QLinearGradient stationBarGradient( 0, 0, 0, ui.stationTimeBar->height() );
    stationBarGradient.setColorAt( 0, k_stationBarGreyTop );
    stationBarGradient.setColorAt( 0.07, k_stationBarGreyMiddle );
    stationBarGradient.setColorAt( 0.14, k_stationBarGreyBottom );
    stationBarGradient.setColorAt( 1, k_stationBarGreyBottom );

    ui.stationTimeBar->setBackgroundGradient( stationBarGradient );
//TODO    ui.stationTimeBar->setStopWatch( &The::radio().stationStopWatch() );

    if ( qApp->arguments().contains( "--sanity" ) )
        stationBarGradient = trackBarBkgrGradient;
}


void
Container::setupTrayIcon()
{
    QMenu *menu = new QMenu( this );
    menu->addAction( tr( "Open" ), this, SLOT( restoreWindow() ) );

  #ifdef Q_WS_MAC
    menu->addSeparator();
    menu->addAction( ui.actionSettings );
  #endif
    menu->addSeparator();
    menu->addAction( ui.actionDashboard );
    menu->addAction( ui.actionToggleScrobbling );
    (menu->addAction( tr( "Change User" ) ) )->setMenu( ui.menuUser );
    menu->addSeparator();
    menu->addAction( ui.actionShare );
    menu->addAction( ui.actionTag );
    menu->addAction( ui.actionPlaylist );
    menu->addSeparator();
    menu->addAction( ui.actionLove );
    menu->addAction( ui.actionBan );
  #ifndef HIDE_RADIO
    menu->addSeparator();
    menu->addAction( ui.actionPlay );
    menu->addAction( ui.actionStop );
    menu->addAction( ui.actionSkip );
  #endif
    menu->addSeparator();
    menu->addAction( ui.actionQuit );

  #ifdef Q_WS_MAC
    // strangely text is amended in Application menu, but nowhere else
    ui.actionSettings->setText( tr( "Preferences..." ) );
    ui.actionQuit->setText( tr( "Quit Last.fm" ) );
  #endif

    m_trayIcon = new TrayIcon( this );
    m_trayIcon->setContextMenu( menu );

    connect( m_trayIcon,
             SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             SLOT( onTrayIconActivated( QSystemTrayIcon::ActivationReason ) ) );
}


void
Container::applyMenuTweaks()
{
    // Qt 4.3.4, setSeparatorsCollapsible doesn't work as advertised :(
    foreach( QAction* a, menuBar()->actions() )
    {
        bool lastItemWasSeparator = false;
        foreach( QAction* b, a->menu()->actions() )
        {
            if ( !b->isVisible() )
                continue;

            if ( b->isSeparator() )
            {
                if ( lastItemWasSeparator )
                    delete b;

                lastItemWasSeparator = true;
            }
            else
                lastItemWasSeparator = false;
        }
    }
}


void
Container::applyPlatformSpecificTweaks()
{
#ifdef Q_WS_X11
    ui.actionCheckForUpdates->setVisible( false );
    ui.actionQuit->setShortcut( tr( "CTRL+Q" ) );
    ui.actionQuit->setText( tr( "&Quit" ) );

    setWindowIcon( QIcon( MooseUtils::dataPath( "icons/as.png" ) ) );
    //aesthetics, separates the statusbar and central widget slightly
    static_cast<QVBoxLayout*>(ui.centralwidget->layout())->addSpacing( 2 );
#else
    ui.actionScrobbleManualIPod->setVisible( false );
#endif

#ifdef WIN32
    // Can't use 32 for height as labels get truncated on Win classic
    ui.toolbar->setIconSize( QSize( 50, 34 ) );
    ui.menuTools->insertSeparator( ui.actionPlaylist );
#endif

#ifdef Q_WS_MAC
    setUnifiedTitleAndToolBarOnMac( true );
    ui.toolbar->setAttribute( Qt::WA_MacNoClickThrough );
    ui.actionFAQ->setShortcut( tr( "Ctrl+?" ) );
    ui.splitter->setHandleWidth( 1 );
    ui.actionMute->setShortcut( QKeySequence( Qt::CTRL + Qt::ALT + Qt::Key_Down ) );
    ui.menuFile->menuAction()->setVisible( false );

//TODO    qApp->setStyle( new MacStyleOverrides() );

    QPalette p = ui.centralwidget->palette();
    p.setColor( QPalette::Window, QColor( 0xe9, 0xe9, 0xe9 ) );
    ui.centralwidget->setPalette( p );

    p = statusBar()->palette();
    p.setColor( QPalette::WindowText, QColor( 0x59, 0x59, 0x59 ) );
    ui.scrobbleLabel->label()->setPalette( p );

    QLinearGradient g( 0.5, 0.0, 0.5, 20.0 );
    g.setColorAt( 0.0, QColor( 0x8c, 0x8c, 0x8c ) );
    g.setColorAt( 0.05, QColor( 0xf7, 0xf7, 0xf7 ) );
    g.setColorAt( 1.0, QColor( 0xe4, 0xe4, 0xe4 ) );
    p.setBrush( QPalette::Window, QBrush( g ) );
    ui.statusbar->setPalette( p );

    // mac specific mainwindow adjustments
    QFrame *hline = new QFrame;
    hline->setFrameStyle( QFrame::HLine | QFrame::Plain );
    p.setColor( QPalette::WindowText, QColor( 140, 140, 140 ) );
    hline->setPalette( p );
    ui.vboxLayout1->insertWidget( ui.vboxLayout1->indexOf( ui.stack ), hline );
    ui.stack->setFrameStyle( QFrame::NoFrame );

    //alter the spacings and that
    ui.vboxLayout2->setMargin( 14 );
    for ( int x = 0; x < ui.vboxLayout1->count(); ++x )
        if ( ui.vboxLayout1->itemAt( x )->spacerItem() )
            delete ui.vboxLayout1->takeAt( x );

    QFont f = ui.statusbar->font();
    f.setPixelSize( 10 );
    ui.statusbar->setFont( f );
    ui.scrobbleLabel->label()->setFont( f );
#endif

#ifndef WIN32
    ui.actionGetPlugin->setEnabled( false );
    ui.actionGetPlugin->setVisible( false );
#endif
}


void
Container::setupConnections()
{
    connect( ui.actionDashboard, SIGNAL( triggered() ), SLOT( gotoProfile() ) );
    connect( ui.actionSettings, SIGNAL( triggered() ), SLOT( showSettingsDialog() ) );
    connect( ui.actionGetPlugin, SIGNAL( triggered() ), SLOT( getPlugin() ) );
    connect( ui.actionCheckForUpdates, SIGNAL( triggered() ), SLOT( checkForUpdates() ) );
    connect( ui.actionAddUser, SIGNAL( triggered() ), SLOT( addUser() ) );
    connect( ui.actionDeleteUser, SIGNAL( triggered() ), SLOT( deleteUser() ) );
    connect( ui.actionToggleScrobbling, SIGNAL( triggered() ), SLOT( toggleScrobbling() ) );
    connect( ui.actionToggleDiscoveryMode, SIGNAL( triggered() ), SLOT( toggleDiscoveryMode() ) );
    connect( ui.actionAboutLastfm, SIGNAL( triggered() ), SLOT( about() ) );
    connect( ui.menuUser, SIGNAL( aboutToShow() ), SLOT( onAboutToShowUserMenu() ) );
    connect( ui.menuUser, SIGNAL( triggered( QAction* ) ), SLOT( onUserSelected( QAction* ) ) );
    connect( ui.actionSkip, SIGNAL( triggered() ), SIGNAL(skip() ) );
    connect( ui.actionStop, SIGNAL( triggered() ), SIGNAL(stop() ) );
    connect( ui.actionPlay, SIGNAL( triggered() ), SLOT(onPlayClicked()) );
    connect( ui.scrobbleLabel, SIGNAL( clicked() ), SLOT( toggleScrobbling() ) );
    connect( ui.actionVolumeUp, SIGNAL( triggered() ), SLOT( volumeUp() ) );
    connect( ui.actionVolumeDown, SIGNAL( triggered() ), SLOT( volumeDown() ) );
    connect( ui.actionMute, SIGNAL( triggered() ), SLOT( mute() ) );
    connect( ui.actionFAQ, SIGNAL( triggered() ), SLOT( showFAQ() ) );
    connect( ui.actionForums, SIGNAL( triggered() ), SLOT( showForums() ) );
    connect( ui.actionInviteAFriend, SIGNAL( triggered() ), SLOT( inviteAFriend() ) );
    connect( ui.actionDiagnostics, SIGNAL( triggered() ), SLOT( showDiagnosticsDialog() ) );
    connect( ui.actionSendToTray, SIGNAL( triggered() ), SLOT( close() ) );
    connect( ui.actionQuit, SIGNAL( triggered() ), SLOT( quit() ) );
//TODO    connect( ui.playcontrols.volume, SIGNAL( valueChanged( int ) ), &The::radio(), SLOT( setVolume( int ) ) );
//TODO    connect( The::webService(), SIGNAL( success( Request* ) ), SLOT( webServiceSuccess( Request* ) ) );
//TODO    connect( The::webService(), SIGNAL( failure( Request* ) ), SLOT( webServiceFailure( Request* ) ), Qt::QueuedConnection );
//TODO    connect( &The::settings(), SIGNAL( appearanceSettingsChanged() ), SLOT( updateAppearance() ) );
    connect( ui.actionMyProfile, SIGNAL( triggered() ), SLOT( toggleSidebar() ) );
    connect( ui.actionPlaylist, SIGNAL( triggered() ), SLOT( addToMyPlaylist() ) );
    connect( ui.actionTag, SIGNAL( triggered() ), SLOT( showTagDialog() ) );
    connect( ui.actionShare, SIGNAL( triggered() ), SLOT( showShareDialog() ) );
    connect( ui.actionLove, SIGNAL( triggered() ), SLOT( love() ) );
    connect( ui.actionBan, SIGNAL( triggered() ), SLOT( ban() ) );
//TODO    connect( ui.metaDataWidget, SIGNAL( tagButtonClicked() ), SLOT( showTagDialogMD() ) );
//TODO    connect( ui.metaDataWidget, SIGNAL( urlHovered( QUrl ) ), SLOT( displayUrlInStatusBar( QUrl ) ) );
    connect( ui.actionScrobbleManualIPod, SIGNAL( triggered() ), SLOT( scrobbleManualIpod() ) );
    connect( ui.splitter, SIGNAL( splitterMoved( int, int ) ), SLOT( splitterMoved( int ) ) );

////// important connections
//TODO    connect( &The::scrobbler(),
//TODO             SIGNAL( status( int, QVariant ) ),
//TODO             SLOT( onScrobblerStatusChange( int, QVariant ) ) );
//TODO    connect( &The::radio(),
//TODO             SIGNAL( error( RadioError, QString ) ),
//TODO             SLOT( onRadioError( RadioError, QString ) ), Qt::QueuedConnection /* crash if direct connect */ );
//TODO    connect( &The::radio(),
//TODO             SIGNAL( stateChanged( RadioState ) ),
//TODO             SLOT( onRadioStateChanged( RadioState ) ) );
//TODO    connect( &The::radio(),
//TODO             SIGNAL( buffering( int, int ) ),
//TODO             SLOT( onRadioBuffering( int, int ) ) );
//TODO    connect( qApp, SIGNAL( event( int, QVariant ) ), SLOT( onAppEvent( int, QVariant ) ) );

////// we don't use CTRL because that varies by platform, NOTE don't tr()!
    QObject* hides  = new QShortcut( QKeySequence( "Ctrl+W" ), this );
    QObject* openL = new QShortcut( QKeySequence( "Alt+Shift+L" ), this );
    QObject* openF = new QShortcut( QKeySequence( "Alt+Shift+F" ), this );
    connect( hides, SIGNAL( activated() ), SLOT( hide() ) );
    connect( openL, SIGNAL( activated() ), SLOT( onAltShiftL() ) );
    connect( openF, SIGNAL( activated() ), SLOT( onAltShiftF() ) );
#ifdef WIN32
    QObject* openP = new QShortcut( QKeySequence( "Alt+Shift+P" ), this );
    connect( openP, SIGNAL( activated() ), SLOT( onAltShiftP() ) );
#endif
}


void
Container::restoreState()
{
    //NOTE it is important that the connections are done first
    ui.playcontrols.volume->setValue( The::settings().volume() );

    if ( The::currentUser().sidebarWidth() > 0 )
    {
        m_sidebarWidth = The::currentUser().sidebarWidth();
        qDebug() << "Restoring sidebar width: " << m_sidebarWidth ;
    }

    if ( The::currentUser().sidebarEnabled() )
    {
        ui.splitter->setSizes( QList<int>() << m_sidebarWidth );
    }

    // figure out minimum width of our toolbar (this will change due to translations!)
    int w = width();
    QByteArray geometry = The::settings().containerGeometry();
    if ( geometry.isEmpty() )
    {
        w = 0;
        QList<QAction*> a = ui.toolbar->actions();

        for( QList<QAction*>::iterator i = a.begin(); i != a.end(); ++i )
        {
            if ( (*i)->isSeparator() || !(*i)->isVisible() )
                continue;

            QWidget *widget = ui.toolbar->widgetForAction( *i );

            // don't adjust the playcontrols, their size-policy is expanding and will mess up this calculation
            if ( widget->objectName() != "PlayControls" )
            {
                widget->adjustSize();
                w += widget->width();
            }
            else
            {
                w += 140; // HACK: i couldn't figure out a proper way to get the mnimum width of the playcontrols
            }
        }

        w += 16; // spacing
        if ( w < width() )
            w = width();
    }

#ifdef Q_WS_MAC
    // on mac qt returns height without unified toolbar height included :(
    // so first time we have to do it like this
    if ( geometry.isEmpty() )
        resize( w, 496 );
    else
        restoreGeometry( geometry );
#else
    restoreGeometry( The::settings().containerGeometry() );

    if ( geometry.isEmpty() )
        resize( w, height() );
#endif
    setWindowState( The::settings().containerWindowState() );
}


Container::~Container()
{
    qDebug() << "Saving app state" ;

    The::settings().setContainerWindowState( windowState() );
    The::settings().setVolume( ui.playcontrols.volume->value() );
    The::currentUser().setSidebarEnabled( m_sidebarEnabled );
    The::currentUser().setSidebarWidth( m_sidebarWidth );

    qDebug() << "Saving config" ;
    QSettings().sync();
}


void
Container::quit()
{
    if ( ConfirmDialog::quit( this ) )
    {
        qApp->quit();
    }
}


void
Container::closeEvent( QCloseEvent *event )
{
    bool quit = false;

    #ifdef Q_WS_MAC
    if ( !event->spontaneous() )
        quit = true;
    else
    #endif

    // ELSE FOR OSX ABOVE!
    // FIXME check for showDockIcon too, or this is totally stupid
    // NOTE there is no setting, it's all determined in settingsDialog
    if ( !The::settings().showTrayIcon() )
    {
      #ifdef Q_WS_X11
        quit = true;
      #else
        if ( !ConfirmDialog::hide( this ) )
        {
            event->ignore();
            return;
        }
      #endif
    }

    if ( quit )
    {
        qApp->quit();
        event->accept();
    }
    else
    {
        // Just minimise to tray
        minimiseToTray();
        event->ignore();
    }
}


void
Container::minimiseToTray()
{
    hide();

  #ifdef WIN32
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
  #endif // WIN32
}


void
Container::dragMoveEvent( QDragMoveEvent* event )
{
  #ifndef HIDE_RADIO
    QString url = event->mimeData()->urls().value( 0 ).toString();
    if ( url.startsWith( "lastfm://" ) )
    {
        event->acceptProposedAction();
    }
    else
        event->ignore();
  #endif // HIDE_RADIO
}


void
Container::dragEnterEvent( QDragEnterEvent* event )
{
  #ifndef HIDE_RADIO
    QString url = event->mimeData()->urls().value( 0 ).toString();
    if ( url.startsWith( "lastfm://" ) )
    {
        event->acceptProposedAction();
    }
    else
        event->ignore();
  #endif // HIDE_RADIO
}


void
Container::dropEvent( QDropEvent* event )
{
  #ifndef HIDE_RADIO
    QString url = event->mimeData()->urls().value( 0 ).toString();
    if ( url.startsWith( "lastfm://" ) )
    {
        emit play( url );
    }
  #endif // HIDE_RADIO
}


bool
Container::event( QEvent* e )
{
    #ifdef Q_WS_MAC
    //TODO remove when Qt is fixed!
    if ( e->type() == QEvent::Resize )
    {
        // hack as Qt 4.3.1 is b0rked for unified toolbars
        ui.toolbar->setMaximumWidth( width() );
    }
    #endif

    if ( e->type() == QEvent::Move || e->type() == QEvent::Resize )
    {
        // Again, Qt is broken, if maximised the saveGeometry function fails to
        // save the geometry for the non-maximised state. So instead we must save it
        // for *every* resize and move event. Yay!

        if (windowState() != Qt::WindowMaximized)
            The::settings().setContainerGeometry( saveGeometry() );
    }

    if ( e->type() == QEvent::Show )
    {
        emit becameVisible();
    }

    return QMainWindow::event( e );
}


void
Container::toggleDiscoveryMode()
{
//TODO    bool enabled = ui.actionToggleDiscoveryMode->isChecked();
//TODO    The::radio().setDiscoveryMode( enabled );
}


void
Container::displayUrlInStatusBar( const QUrl& url )
{
    statusBar()->showMessage( UnicornUtils::urlDecodeItem( url.toString() ) );
}


void
Container::toggleSidebar()
{
    QIcon icon;
    m_sidebarEnabled = !m_sidebarEnabled;

    if ( !m_sidebarEnabled )
    {
        icon = IconShack::instance().GetGoodUserIconExpanded( The::currentUser().icon() );

      #ifndef Q_WS_MAC
        centralWidget()->setContentsMargins( 5, 0, 5, 0 ); //aesthetics
      #endif
    }
    else
    {
      #ifndef Q_WS_MAC
        centralWidget()->setContentsMargins( 0, 0, 5, 0 ); //aesthetics
      #endif

        icon = IconShack::instance().GetGoodUserIconCollapsed( The::currentUser().icon() );
        ui.splitter->setSizes( QList<int>() << m_sidebarWidth );
    }

    ui.actionMyProfile->setChecked( m_sidebarEnabled );
    ui.actionMyProfile->setIcon( icon );
    ui.sidebar->parentWidget()->setVisible( m_sidebarEnabled );
}


void
Container::showFAQ()
{
    QDesktopServices::openUrl( "http://" + UnicornUtils::localizedHostName( The::settings().appLanguage() ) + "/help/faq/" );
}


void
Container::showForums()
{
    QDesktopServices::openUrl( "http://" + UnicornUtils::localizedHostName( The::settings().appLanguage() ) + "/forum/34905/" );
}


void
Container::inviteAFriend()
{
    QByteArray user = QUrl::toPercentEncoding( The::settings().currentUsername() );
    QDesktopServices::openUrl( "http://" + UnicornUtils::localizedHostName( The::settings().appLanguage() ) + "/user/" + user + "/inviteafriend/" );
}



void
Container::about()
{
    AboutDialog* d = findChild<AboutDialog*>();
    if ( !d )
    {
        (d = new AboutDialog( this ))->show();
        d->setAttribute( Qt::WA_DeleteOnClose );
    }
    else
        d->raise();
}


void
Container::toggleScrobbling()
{
    The::settings().currentUser().toggleLogToProfile();
}


void
Container::updateWindowTitle( const Track& data )
{
    QString title;

    // on mac it is atypical to show document info for non multi document UIs
    #ifndef Q_OS_MAC
        QString track = data.toString();
    #else
        Q_UNUSED( data );
        QString track;
    #endif

    if ( track.isEmpty() )
        title += "Last.fm";
    else
        title += track;

    if ( qApp->arguments().contains( "--debug" ) )
        title += " [debug]";

    setWindowTitle( title );
}


void
Container::updateAppearance()
{
    if ( !The::settings().showTrayIcon() )
        m_trayIcon->hide();
    else
        m_trayIcon->show();
}


void
Container::onRadioBuffering( int size, int total )
{
    bool finished = size == total;

    if ( finished )
    {
        statusBar()->clearMessage();
    }
    else
    {
        int percent = (int)( ( (float)size / total ) * 100 );
        statusBar()->showMessage( tr( "Buffering... (%1%)", "%1 is the percentage buffering is complete" ).arg( percent ) );
    }
}

#if 0
void
Container::showTagDialog( int defaultTagType )
{
    TagDialog d( m_track, this );
    if ( defaultTagType >= 0 )
        d.setTaggingType( defaultTagType );

    d.exec();
}


void
Container::showTagDialogMD()
{
    showTagDialog( 0 );
}
#endif

void
Container::volumeUp()
{
    if ( ui.playcontrols.volume->value() != 100 )
    {
        if ( ui.playcontrols.volume->value() + 5 > 100 )
            ui.playcontrols.volume->setValue( 100 );
        else
            ui.playcontrols.volume->setValue( ui.playcontrols.volume->value() + 5 );
    }
}


void
Container::volumeDown()
{
    if ( ui.playcontrols.volume->value() != 0 )
    {
        if ( ui.playcontrols.volume->value() - 5 < 0 )
            ui.playcontrols.volume->setValue( 0 );
        else
            ui.playcontrols.volume->setValue( ui.playcontrols.volume->value() - 5 );
    }
}


void
Container::mute()
{
    if ( ui.playcontrols.volume->value() != 0 )
    {
        m_lastVolume = ui.playcontrols.volume->value();
        ui.playcontrols.volume->setValue( 0 );
    }
    else
        ui.playcontrols.volume->setValue( m_lastVolume );
}


void
Container::addToMyPlaylist()
{
    // Make copy in case dialog stays on screen across track change
    TrackInfo track = m_track;

    if ( ConfirmDialog::playlist( track, this ) )
    {
        ui.actionPlaylist->setEnabled( false );
//TODO        (new AddToMyPlaylistRequest( track ))->start();
    }
}


void
Container::onLoveClicked()
{
    // Make copy in case dialog stays on screen across track change
    MetaData track = m_track;

    if (ConfirmDialog::love( track, this ))
    {
        ui.actionLove->setEnabled( false );

//TODO        (new LoveRequest( track ))->start();
    }

    // HACK: Only reason it's OK to do this is because the scrobbler will
    // make sure to not scrobble duplicates.
//TODO    track.setRatingFlag( TrackInfo::Loved );
//TODO    The::app().scrobbler().scrobble( track );
}


void
Container::onBanClicked()
{
    // Make copy in case dialog stays on screen across track change
    MetaData track = m_track;

    if ( ConfirmDialog::ban( track, this ) )
    {
        ui.actionSkip->setEnabled( false );
        ui.actionLove->setEnabled( false );
        ui.actionBan->setEnabled( false );
        ui.actionPlaylist->setEnabled( false );

//TODO        (new BanRequest( track ))->start();

        // Listener needs to know about banned tracks so that we can prevent
        // them from getting submitted.
//TODO        CPlayerConnection* player = The::app().listener().GetActivePlayer();
//TODO        if ( player != NULL )
//TODO        {
//TODO            player->ban();
//TODO        }

        // HACK: Only reason it's OK to do this is because the scrobbler will
        // make sure to not scrobble duplicates.
//TODO        track.setRatingFlag( TrackInfo::Banned );
//TODO        The::app().scrobbler().scrobble( track );

//TODO        The::radio().skip();
    }
}


void
Container::onPlayClicked()
{}


void
Container::gotoProfile()
{
    QByteArray user = QUrl::toPercentEncoding( The::settings().currentUsername() );
    QDesktopServices::openUrl( "http://" + UnicornUtils::localizedHostName( The::settings().appLanguage() ) + "/user/" + user );
}


#if 0
void
Container::showShareDialog()
{
    m_shareDialog->setSong( m_track );
    m_shareDialog->exec();
}


void
Container::showSettingsDialog( int startPage )
{
    SettingsDialog settingsDialog( this );
    settingsDialog.exec( startPage );
}


void
Container::showDiagnosticsDialog()
{
    m_diagnosticsDialog->show();
}
#endif

void 
Container::showNotification( const QString& title, const QString& message )
{
    m_trayIcon->showMessage( title, message );
}


#if 0
void
Container::scrobbleManualIpod()
{
    QObject* o = QPluginLoader( MooseUtils::servicePath( "Ipod_device" ) ).instance();
    MyMediaDeviceInterface* plugin = qobject_cast<MyMediaDeviceInterface*>(o);

    if ( plugin )
    {
        QString path;

        QString const settings_path = "IpodDevice/" + plugin->uniqueId() + "/MountLocation";
        path = QFileDialog::getExistingDirectory(
                                     this,
                                     tr( "Where is your iPod mounted?" ),
                                     QSettings().value( settings_path, "/" ).toString() );
        if ( path.isEmpty() )
            return; // user pressed cancel

        QSettings().setValue( settings_path, path );

        plugin->setMountPath( path );

        qApp->setOverrideCursor( Qt::WaitCursor );
        QList<TrackInfo> tracks = plugin->tracksToScrobble();
        qApp->restoreOverrideCursor();

        qDebug() << "Manual iPod scrobbling found" << tracks.count();

        if ( tracks.count() )
        {
            IPodScrobbler s( The::user().name(), this );
            s.setTracks( tracks );
            s.setAlwaysConfirm( true );
            s.exec();
        }
        else if ( plugin->error().isEmpty() )
        {
            LastMessageBox::information(
                    tr( "Nothing to Scrobble" ),
                    tr( "You have not played anything since you last scrobbled this iPod." ) );
        }
        else
            LastMessageBox::information( tr( "Plugin Error" ), plugin->error() );
    }
    else
        LastMessageBox::warning(
                tr( "Warning" ),
                tr( "There was an error loading the IpodDevice plugin." ) );

    delete o;
}
#endif

void
Container::onTrayIconActivated( QSystemTrayIcon::ActivationReason reason )
{
    // typical linux behavior is single clicking tray icon toggles the main window

    #ifdef Q_WS_X11
    if (reason == QSystemTrayIcon::Trigger)
        toggleWindowVisibility();
    #else
    if (reason == QSystemTrayIcon::DoubleClick)
        toggleWindowVisibility();
    #endif
}


#ifdef Q_WS_X11
    // includes only relevent to this function - please leave here :)
    #include <QX11Info>
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
#endif

void
Container::toggleWindowVisibility()
{
    //TODO really we should check to see if the window itself is obscured?
    // hard to say as exact desire of user is a little hard to predict.
    // certainly we should raise the window if it isn't active as chances are it
    // is behind other windows

    if ( isVisible() )
        hide();
    else
    {
        #ifndef Q_WS_X11
            show(), activateWindow(), raise();
        #else
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
        #endif
    }
}


void
Container::onAltShiftL()
{
    #ifdef WIN32
        // The QDesktopServices call doesn't work on Windows
//TODO        QString file = QString::fromStdWString( Logger::GetLogger().GetFilePath() );
        ShellExecuteW( 0, 0, (TCHAR*)file.utf16(), 0, 0, SW_SHOWNORMAL );
    #else
//TODO        QDesktopServices::openUrl( QUrl::fromLocalFile( QString::fromStdString( Logger::GetLogger().GetFilePath() ) ) );
    #endif
}


void
Container::onAltShiftF()
{
    #ifdef WIN32
        // The QDesktopServices call doesn't work on Windows
//TODO        QString file = MooseUtils::logPath( "" );
        ShellExecuteW( 0, 0, (TCHAR*)file.utf16(), 0, 0, SW_SHOWNORMAL );
    #else
//TODO        QDesktopServices::openUrl( QUrl::fromLocalFile( MooseUtils::logPath( "" ) ) );
    #endif
}


#ifdef WIN32
void
Container::onAltShiftP()
{
    ShellExecuteW( 0, 0, (TCHAR*)UnicornUtils::globalAppDataPath().utf16(), 0, 0, SW_SHOWNORMAL );
}
#endif


void
Container::onAppStateChanged( State state, const Track& track )
{
    switch (state)
    {
        case TuningIn:
//TODO            statusBar()->showMessage( tr( "Starting station %1..." ).arg( The::radio().stationUrl() ) );

            ui.stack->setCurrentIndex( 1 );
            ui.meta->setTuningIn();

//TODO            if ( The::radio().stationUrl().isPlaylist() )
//TODO                ui.stationTimeBar->setText( tr( "Connecting to playlist..." ) );
//TODO            else
//TODO                ui.stationTimeBar->setText( tr( "Starting station..." ) );

            ui.actionPlay->setVisible( false );
            ui.actionStop->setVisible( true );
        
            ui.songTimeBar->clear();
            ui.stationTimeBar->setClockText( "" );
            ui.stationTimeBar->setEnabled( true );
            ui.stationTimeBar->setVisible( true );
            break;
            
        case Playing:
            if (track.source() != Track::LastFmRadio)
            {
                // with radio we already changed to this, and there is a noticeable
                // gap between tuning in and starting playback so the user may have
                // already switched back to the change station tab
                showMetaDataWidget();
                ui.actionPlay->setVisible( false );
                ui.actionStop->setVisible( false );                
            }
            break;
            
        case Stopped:
//TODO            ui.actionToggleDiscoveryMode->setEnabled( The::user().isSubscriber() );
            statusBar()->clearMessage();
            showRestState();

            ui.actionShare->setEnabled( false );
            ui.actionTag->setEnabled( false  );
            ui.actionPlaylist->setEnabled( false );
            ui.actionLove->setEnabled( false );

            ui.songTimeBar->setEnabled( false );
            ui.songTimeBar->setClockText( "" );
            ui.songTimeBar->setClockEnabled( false );
            ui.songTimeBar->setText( "" );
            
            ui.stationTimeBar->clear();
            ui.stationTimeBar->setEnabled( false );
            ui.stationTimeBar->hide();
            
            ui.actionPlay->setVisible( true );
            ui.actionStop->setVisible( false );
            break;
            
        case Buffering:
        case Paused:
            break;
    }
    
    ui.actionPlay->setEnabled( ui.actionPlay->isVisible() );
    ui.actionStop->setEnabled( ui.actionStop->isVisible() );
    ui.actionPlay->setShortcut( ui.actionPlay->isEnabled() ? Qt::Key_Space : QKeySequence() );
    ui.actionStop->setShortcut( ui.actionStop->isEnabled() ? Qt::Key_Space : QKeySequence() );
}


void
Container::onTrackSpooled( const Track& t, StopWatch* watch )
{
    ui.meta->setTrack( t );
    updateWindowTitle( t );    
    m_trayIcon->setTrack( t );

    if (t.isNull())
    {
        ui.actionPlay->setEnabled( true );
        ui.actionStop->setEnabled( false );
    }
    else if (t.source() == Track::LastFmRadio)
    {
        ui.actionPlay->setEnabled( false );
        ui.actionStop->setEnabled( true );
    }
    else //track from media player, we have no control
    {        
        ui.actionPlay->setEnabled( false );
        ui.actionStop->setEnabled( false );        
    }
    

    if (t.isNull()) return;
    
    TrackInfo track( t );
    
    qDebug() << track;
    
    ui.songTimeBar->setTrack( track );
    m_trayIcon->setTrack( track );

    if (!Scrobble(track).isValid() && track.source() != Track::LastFmRadio)
    {
        ui.actionTag->setEnabled( false );
        ui.actionShare->setEnabled( false );
        ui.actionLove->setEnabled( false );
    }
    else
    {
        ui.songTimeBar->setStopWatch( watch );
        ui.songTimeBar->setReverse( true );

        if (watch->isTimedOut())
            ui.songTimeBar->pushClockText( tr( "scrobbled" ), 5 );
        else
            ui.songTimeBar->setClockText( "" );

        ui.actionShare->setEnabled( true );
        ui.actionTag->setEnabled( true );
        ui.actionPlaylist->setEnabled( true );
        ui.actionLove->setEnabled( true );
        ui.songTimeBar->setEnabled( true );

        // Clear any status messages on the start of a new track or it will look odd
        if (track.source() != Track::LastFmRadio)
        {
            ui.actionSkip->setEnabled( false );
            ui.actionBan->setEnabled( false );            
            statusBar()->clearMessage();
        }
        else {
            ui.actionSkip->setEnabled( true );
            ui.actionBan->setEnabled( true );
        }
    }

    ui.stack->setCurrentIndex( 1 );
}


void
Container::onScrobblePointReached( const Track& t )
{
    ui.songTimeBar->pushClockText( tr( "scrobbled" ), 5 );
    ui.sidebar->addRecentlyPlayedTrack( t );
}


void
Container::restoreWindow()
{
    showNormal();
    activateWindow();
    raise();
}


//////////////////////////////////////////////////////////////////////////////
ScrobbleLabel::ScrobbleLabel()
{
    QHBoxLayout *hbox = new QHBoxLayout( this );
    hbox->setMargin( 0 );
    hbox->setSpacing( 6 );

    hbox->addWidget( m_label = new QLabel );
  #ifdef Q_WS_MAC
    QLabel* l;
    hbox->addWidget( l = new QLabel );
    l->setPixmap( QPixmap(":/icons/scrobbling_graphic.png") );
  #endif
    hbox->addWidget( m_image = new QLabel );

    setEnabled( false );
    setToolTip( tr( "Click to enable/disable scrobbling" ) );
    setAutoFillBackground( false );
}


void
ScrobbleLabel::setEnabled( bool const on )
{
    QIcon icon( ":/icons/scrobble16.png" );
    m_label->setText( ' ' + tr( "Scrobbling %1" ).arg( on ? tr( "on" ) : tr( "off" ) ) + ' ' );
    m_image->setPixmap( icon.pixmap( 16, 16, on ? QIcon::Normal : QIcon::Disabled ) );
}
