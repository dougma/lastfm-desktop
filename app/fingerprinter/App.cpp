/*
   Copyright 2005-2009 Last.fm Ltd. 
      - Primarily authored by Peter Grundstrom and Adam Renburg

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
#ifdef WIN32
    #include <windows.h>
    #include <winnls.h>
#endif

#include <QTimer>
#include <QDebug>
#include <QLocale>
#include <QFile>

#include "libLastFmTools/containerutils.h"

#include "App.h"
#include "FingerprinterSettings.h"

#include "tag.h"
#include "fileref.h"



App::App( int& argc, char** argv )
    : unicorn::Application( argc, argv ),
      m_mainWindow(),
      m_progressBar( &m_mainWindow ),
      m_fingerprintScheduler( 1 /*number of threads*/)
{
    m_chunkSize = 20;
    
    connect( &m_mainWindow, SIGNAL(logout()), SLOT(logout()) );
    connect( &m_mainWindow, SIGNAL(wantsToClose( QCloseEvent* )), SLOT(onCloseEvent( QCloseEvent* )) );
}


void
App::onCloseEvent( QCloseEvent* event )
{
    if ( m_progressBar.isRunning() )
    {
        if ( !m_progressBar.onCancelClicked() )
        {
            event->ignore();
            return;
        }
    }
    
    FingerprinterSettings::instance().setSize ( m_mainWindow.size() );
    FingerprinterSettings::instance().setPosition ( m_mainWindow.pos() );
    FingerprinterSettings::instance().save ();
    event->accept();
}

void App::logout()
{
    qDebug() << "Logging out";
    FingerprinterSettings::instance().setRememberPassword( false );
    FingerprinterSettings::instance().setPassword( "empty" );
    FingerprinterSettings::instance().setLPassword( "empty" );
    m_mainWindow.hide();
    login();
}

void App::login()
{
    qDebug() << "Opening LoginDialog";
    
    LoginWidget::Mode mode = LoginWidget::LOGIN;
    if (FingerprinterSettings::instance().rememberPassword())
        mode = LoginWidget::AUTO_LOGIN;
        
    if (m_loginDialog == 0)
    {
        m_loginDialog = new LoginDialog( 0, mode );
        connect( m_loginDialog, SIGNAL( rejected() ), this, SLOT( exit() ), Qt::QueuedConnection );
        connect( m_loginDialog, SIGNAL( accepted() ), this, SLOT( init() ) );
    }
    else
    {
        m_loginDialog->setMode( mode );
        connect( m_loginDialog, SIGNAL( rejected() ), this, SLOT( exit() ), Qt::QueuedConnection );
        connect( m_loginDialog, SIGNAL( accepted() ), &m_mainWindow, SLOT( start() ) );
    }

    m_loginDialog->start();
}

void App::init()
{
    m_mainWindow.start();

    connect( &m_mainWindow, SIGNAL( startFingerprinting( QStringList ) ),
             this, SLOT( startFingerprinting( QStringList ) ) );
                      
    connect( &m_progressBar, SIGNAL( hidden( bool ) ),
             &m_mainWindow, SLOT( setEnabled( bool ) ) );
    
    connect( &m_progressBar, SIGNAL( abortFingerprinting() ),
             this, SLOT( abortFingerprint() ) );

    connect( &m_fingerprintScheduler, SIGNAL( trackFingerprintingStarted( TrackInfo& ) ),
             &m_progressBar, SLOT( setCurrentTrack( TrackInfo& ) ) );

    connect( &m_fingerprintScheduler, SIGNAL( trackFingerprinted( TrackInfo& ) ),
             &m_progressBar, SLOT( trackFingerprinted() ) );

    connect( &m_fingerprintScheduler, SIGNAL( stopped( bool /*finished*/) ),
             this, SLOT( onFingerprintingStopped( bool  /*finished*/) ), Qt::QueuedConnection );

    connect( &m_fingerprintScheduler, SIGNAL( queueIsEmpty( ) ),
             this, SLOT( sendTracksForFingerprinting( ) ), Qt::QueuedConnection );

    connect( &m_fingerprintScheduler, SIGNAL( trackFingerprinted( TrackInfo& ) ),
             this, SLOT( onTrackFingerprinted( ) ) );

    connect( &m_fingerprintScheduler, SIGNAL( cantFingerprintTrack( TrackInfo&, QString ) ),
             this, SLOT( onSchedulerCantFingerprintTrack( TrackInfo&, QString ) ) );

    qRegisterMetaType<FingerprintScheduler::NetworkErrors>("FingerprintScheduler::NetworkErrors");
    connect( &m_fingerprintScheduler, SIGNAL( networkError ( FingerprintScheduler::NetworkErrors, QString ) ),
             this, SLOT( onNetworkError ( FingerprintScheduler::NetworkErrors, QString ) ), Qt::QueuedConnection );

    m_fingerprintScheduler.setUsername( FingerprinterSettings::instance().currentUsername() );
    m_fingerprintScheduler.setPasswordMd5( FingerprinterSettings::instance().currentPassword() );
    m_fingerprintScheduler.setPasswordMd5Lower( FingerprinterSettings::instance().currentPasswordLower() );
}

void App::initTranslator()
{
    QString langCode;

    // First check settings
    langCode = FingerprinterSettings::instance().appLanguage();

    if ( langCode.isEmpty() )
    {
        // If none found, use system locale
        QLocale::Language qtLang = QLocale::system().language();
        langCode = qtLanguageToLfmLangCode( qtLang );
        FingerprinterSettings::instance().setAppLanguage( langCode );
    }

    setLanguage( langCode );
}

void App::setLanguage( QString langCode )
{
    m_lang = langCode;

    // Discards previously loaded translations
    m_translatorApp.load( dataPath( "i18n/lastfm_fingerprinter_%1" ).arg( langCode ) );
    m_translatorQt.load( dataPath( "i18n/qt_%1" ).arg( langCode ) );
    
    installTranslator( &m_translatorApp );
    installTranslator( &m_translatorQt );
}

void App::getTracksFromDirs( QStringList& dirs, QStringList& output)
{
    foreach( QString dir, dirs )
    {
        if ( checkAbort() ) return;
        getTracksFromDir( dir, output );
    }
}

void App::getTracksFromDir( QString dir, QStringList& output )
{
    QDir realDir( dir );
        
    realDir.setFilter( QDir::Dirs | QDir::NoDotAndDotDot );
    QFileInfoList subdirs = realDir.entryInfoList();
    
    realDir.setFilter( QDir::Files | QDir::NoDotAndDotDot );
    QStringList nameFilters;
    nameFilters << "*.mp3";
    realDir.setNameFilters( nameFilters );
    
    QFileInfoList tracks = realDir.entryInfoList();
    
    foreach( QFileInfo fileinfo, subdirs )
    {
        if ( checkAbort() ) return;
        getTracksFromDir( fileinfo.absoluteFilePath(), output );
    }
        
    foreach( QFileInfo fileinfo, tracks )
    {
        if ( checkAbort() ) return;
        output.append( QDir::toNativeSeparators( fileinfo.absoluteFilePath() ) );
    }
}

bool App::checkAbort()
{
    QApplication::processEvents();
    return m_abort;
}

bool App::trackInfoFromFile( QString filename, TrackInfo& track )
{
#ifdef Q_WS_MAC
    TagLib::FileRef f( filename.toUtf8().data() );
#elif defined(WIN32)
    char filePathName[1024];
    int res = WideCharToMultiByte( CP_UTF8, 0,
                                   filename.toStdWString().c_str(), -1,
                                   filePathName, 1024,
                                   NULL, NULL );

    TagLib::FileRef f( filePathName );
#else //linux
    TagLib::FileRef f( filename.toLocal8Bit().data() );
#endif

    if (f.isNull())
    {
        logTrack( filename, "TagLib can't open file" );
        return false;
    }

    if ( !f.audioProperties() )
    {
        logTrack( filename, "The track could not be read properly" );
        return false;
    }
    else if ( f.audioProperties()->channels() != 1 && f.audioProperties()->channels() != 2 )
    {
        logTrack( filename, "The track is neither mono nor stereo" );
        return false;
    }
    else if ( f.audioProperties()->sampleRate() < 5512 )
    {
        logTrack( filename, "The track has a samplerate below 5512Hz" );
        return false;
    }

    track.setPath( filename );

    track.setTrack( TStringToQString( f.tag()->title() ).simplified() );
    track.setArtist( TStringToQString( f.tag()->artist() ).simplified() );
    track.setAlbum( TStringToQString( f.tag()->album() ).simplified() );
    track.setTrackNr( f.tag()->track() );
    track.setDuration( f.audioProperties()->length() );

    track.setSource( TrackInfo::Player );

    if (track.track().isEmpty())
        track.setTrack( "Unknown" );
    if (track.artist().isEmpty())
        track.setArtist( "Unknown" );

    return true;
}

void App::startFingerprinting( QStringList dirs )
{
    m_tracksToFingerprint.clear();
    m_progressBar.reset();
    m_abort = false;

    if ( dirs.size() == 0 )
    {
        QString message = tr( "Please select one or several folders containing tracks, in MP3 format, to fingerprint." );
        LastMessageBox::information( tr( "No Folders Selected" ), message );
        return;
    }
    
    m_mainWindow.setEnabled( false );

    m_progressBar.show();
    m_progressBar.start();

    QStringList filenames;
    getTracksFromDirs( dirs, filenames );

    if ( m_abort ) return;
    
    if ( filenames.size() == 0 )
    {
        m_progressBar.stop( true );
        m_progressBar.hide();
        
        QApplication::restoreOverrideCursor();
        QString message = tr( "The folders you selected didn't contain any tracks in MP3 format." );
        LastMessageBox::information( tr( "No Tracks Selected" ), message );
        return;
    }
    
    unsigned int size = m_fingerprintScheduler.calcFingerprintsToSend( filenames, m_tracksToFingerprint );
    
    if ( m_abort ) return;

    m_progressBar.setTotalTracks( filenames.size() );
    m_progressBar.setTracksSkipped( filenames.size() - size );
    m_progressBar.pokeProgressBar();
    m_progressBar.setCollectionPhaseOver();
    
    sendTracksForFingerprinting();
}

void App::sendTracksForFingerprinting()
{
    QList<TrackInfo> tracks;
    
    for ( int i = 0; !m_tracksToFingerprint.isEmpty() && i < m_chunkSize; /* empty */ )
    {
        TrackInfo ti;
        if ( trackInfoFromFile( m_tracksToFingerprint.first(), ti ) )
        {
            tracks.append( ti );
            i++;
        }
        else
            m_progressBar.setTracksWithErrors( m_progressBar.tracksWithErrors()+1 );
            
        m_tracksToFingerprint.removeFirst();
    }
    
    if ( !tracks.isEmpty() )
    {
        FingerprintReturnInfo retinfo = m_fingerprintScheduler.fingerprint( tracks );
        
        m_progressBar.setTracksSkipped( m_progressBar.tracksSkipped() + retinfo.tracksSkipped );
        m_progressBar.setTracksWithErrors( m_progressBar.tracksWithErrors() + retinfo.tracksWithErrors );
    }
}

void App::abortFingerprint()
{
    m_abort = true;

    m_tracksToFingerprint.clear();
    m_fingerprintScheduler.stop();
}

void App::onFingerprintingStopped( bool finished )
{
    // This function will be called when each chunk finishes, so it doesn't
    // mean we're totally done. However, the refilling of the scheduler is
    // done by the sendTracksForFingerprinting function above which is triggered
    // by the scheduler emitting queueIsEmpty.

    if ( !finished )
        return;

    // Only finish proper when there are no tracks left in our main queue
    if ( m_tracksToFingerprint.isEmpty() && m_fingerprintScheduler.isStopped() )
        m_progressBar.stop( true /*finished*/ );

}

void App::onTrackFingerprinted()
{
}

void App::onNetworkError( FingerprintScheduler::NetworkErrors error, QString msg )
{
    QApplication::changeOverrideCursor( Qt::ArrowCursor );

    switch (error)
    {
        case FingerprintScheduler::RequestAborted:
        {
            abortFingerprint();
            LastMessageBox::critical( tr( "Network Error" ),
                tr( "The request was aborted. This is probably due to a \nnetwork error. Check your connection and try again." ) );
            break;
        }

        case FingerprintScheduler::ServersBusy:
        {
            QMutexLocker locker ( &m_networkCountersMutex );

            abortFingerprint();
            LastMessageBox::critical( tr( "Network Error" ), msg );
        }
        break;

        case FingerprintScheduler::OtherError:
        {
            QMutexLocker locker ( &m_networkCountersMutex );

            abortFingerprint();
            LastMessageBox::critical( tr( "Network Error" ), msg );
         }
         break;
         
         case FingerprintScheduler::BadRequest:
         {
            //We do nothing.
         }
         break;
    }
}

void App::onSchedulerCantFingerprintTrack( TrackInfo& track, QString reason )
{
    logTrack( track.path(), reason );
}

void App::logTrack( QString path, QString reason )
{
    qDebug() << QString( "BadTrack: \"%1\"               %2" ).arg( path ).arg( reason );
}
