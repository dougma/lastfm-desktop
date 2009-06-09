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
#include "UpdateDialog.h"
#include <lastfm/misc.h>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDir>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QNetworkReply>
#include <QVBoxLayout>

#ifdef WIN32
#include <QApplication>
#include <windows.h>
#include <process.h>
#include <shellapi.h>
#endif


UpdateDialog::UpdateDialog( QWidget* parent ) : QDialog( parent ), checking( 0 )
{
    QVBoxLayout* v = new QVBoxLayout( this );
    v->addWidget( text = new QLabel( tr("Checking for updates...") ) );
    v->addWidget( bar = new QProgressBar );
    v->addSpacing( 12 );
    v->addWidget( button = new QPushButton( tr("Cancel") ) );
    v->setSizeConstraint( QLayout::SetFixedSize );
    v->setAlignment( button, Qt::AlignRight );
    
    bar->setMinimumWidth( text->sizeHint().width() * 3 / 2 );
        
    bar->setRange( 0, 0 ); //indeterminate
    setAttribute( Qt::WA_DeleteOnClose );

#ifdef __APPLE__
    #define PLATFORM "/mac/"
#endif
#ifdef WIN32
    #define PLATFORM "/win/"
#endif
#ifdef Q_WS_X11
    #define PLATFORM "/x11/"
#endif

    QUrl url( "http://cdn.last.fm/client/" + qApp->applicationName().toLower() + PLATFORM + qApp->applicationVersion() + ".txt" );
//    QUrl url( "http://static.last.fm/client/update_test/201.txt" );
    checking = lastfm::nam()->get( QNetworkRequest(url) );
    checking->setParent( this );

    connect( checking, SIGNAL(finished()), SLOT(onGot()) );
    connect( button, SIGNAL(clicked()), SLOT(close()) );
}    

void
UpdateDialog::onGot()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    if (reply->error() != QNetworkReply::NoError)
        return;
    
    QByteArray data = reply->readAll().trimmed();
    url = QUrl::fromEncoded( data.mid( 32 ) );
    md5 = data.left( 32 );

    if (url.isEmpty() || !url.isValid()) 
    {
        text->setText( tr("No updates available") );
        bar->setRange( 0, 100 );
        bar->setValue( 100 );
        qDebug() << "No updates available";
        if (!isVisible()) deleteLater();
    }
    else {
        QNetworkReply* reply = lastfm::nam()->get( QNetworkRequest(url) );
        reply->setParent( this );
        connect( reply, SIGNAL(downloadProgress( qint64, qint64 )), SLOT(onProgress( qint64, qint64 )) );
        connect( reply, SIGNAL(finished()), SLOT(onDownloaded()) );
    }
}

void
UpdateDialog::onProgress( qint64 received, qint64 total )
{
    bar->setRange( 0, total );
    bar->setValue( received );
}

void 
UpdateDialog::onDownloaded()
{    
    QByteArray data = static_cast<QNetworkReply*>(sender())->readAll();
    
    if (lastfm::md5( data ) != md5) {
        qWarning() << "Downloaded" << data.size() << "bytes from" << url << ", but md5 was not" << md5;
        text->setText( tr( "Download failed, please try again later.") );
        if (!isVisible()) deleteLater();
        return;
    }
    
    bar->setRange( 0, 100 );
    bar->setValue( 100 );
    text->setText( tr( "A new version of %1 is available" ).arg( qApp->applicationName() ) );
#ifdef __APPLE__
    button->setText( tr("Thanks!") ); //will open DMG file now
#else
    button->setText( tr("Quit && Install") );
#endif

    show();

    // ensure the extension is still at end (so we can launch it)
    QString templ = QDir::tempPath() + "/" + "XXXXXX_" + QFileInfo( url.path() ).fileName();
    {
        QTemporaryFile temp( templ );
        temp.setAutoRemove( false );
        temp.open();
        temp.write( data );
        tmpFileName = QDir::toNativeSeparators( temp.fileName() );
    }

    disconnect( button, SIGNAL(clicked()), this, SLOT(close()) );
    connect( button, SIGNAL(clicked()), SLOT(install()) );
}

void 
UpdateDialog::install()
{
#ifdef __APPLE__
    QDesktopServices::openUrl( QUrl::fromLocalFile( tmpFileName ) );
    qApp->quit();
    //TODO auto shut this instance if possible
#endif
#ifdef WIN32
    bool good = false;

    // for testing msi launching:
    // tmpFileName = "C:\\tmp\\boffin.msi";

    if (tmpFileName.endsWith(".msi")) {
        // to do a so-called "minor upgrade", we need to use msiexec
        // see: http://www.tramontana.co.hu/wix/lesson4.php
        // msiexec /i SampleUpgrade2.msi REINSTALL=ALL REINSTALLMODE=vomus
        if (-1 != _wspawnlp(_P_NOWAIT, L"msiexec.exe", 
            L"msiexec.exe", L"/i", tmpFileName.toStdWString().data(), 
            L"REINSTALL=ALL", L"REINSTALLMODE=vomus", 
            NULL)) 
        {
            good = true;
        }
    } else {
        // Must use ShellExecute because otherwise the elevation dialog
        // doesn't appear when launching the installer on Vista.

        SHELLEXECUTEINFOW sei;
        memset(&sei, 0, sizeof(sei));

        sei.cbSize = sizeof(sei);
        sei.fMask  = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT;
        sei.hwnd   = GetForegroundWindow();
        sei.lpVerb = L"open";
        sei.lpFile = reinterpret_cast<LPCWSTR>( tmpFileName.utf16() );
        sei.lpParameters = 0;
        sei.nShow  = SW_SHOWNORMAL;

        BOOL const b = ShellExecuteExW( &sei );
        if (b) {
            QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
            WaitForSingleObject(sei.hProcess, 10*1000);
            CloseHandle(sei.hProcess);
            QApplication::restoreOverrideCursor();
            good = true;
        }
    }

    if (!good) {
        qWarning() << "Couldn't open" << tmpFileName;
        text->setText( tr("The installer could not be launched") );
    } else {
        qApp->quit();
    }

#endif

    // TODO: remove downloaded msi/exe/dmg turd, somehow
}

