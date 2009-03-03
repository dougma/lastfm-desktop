/***************************************************************************
 *   Copyright 2005-2009 Last.fm Ltd.                                      *
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

#include "UpdateDialog.h"
#include "common/qt/md5.cpp"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QNetworkReply>
#include <QVBoxLayout>

#ifdef WIN32
#include <QApplication>
#include <windows.h>
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

    QUrl url( "http://cdn.last.fm/client/" + qApp->applicationName().toLower() + '/' + qApp->applicationVersion() + ".txt" );
//    QString temp = url.toString();
//   QUrl url( "http://static.last.fm/client/update_test/200.txt" );
    checking = nam.get( QNetworkRequest(url) );
    checking->setParent( this );

    connect( checking, SIGNAL(finished()), SLOT(onGot()) );
    connect( button, SIGNAL(clicked()), SLOT(close()) );
}    

void
UpdateDialog::onGot()
{
    QByteArray data = static_cast<QNetworkReply*>(sender())->readAll().trimmed();
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
        QNetworkReply* reply = nam.get( QNetworkRequest(url) );
        reply->setParent( this );
        connect( reply, SIGNAL(downloadProgress( qint64, qint64 )), SLOT(onProgress( qint64, qint64 )) );
        connect( reply, SIGNAL(finished()), SLOT(onDownloaded()) );
        
        // ensure the extension still is at end, so QDesktopServices works
        tmp.setFileTemplate( "XXXXXX_" + QFileInfo( url.path() ).fileName() );
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
    
    if (Qt::md5( data ) != md5) {
        qWarning() << "Downloaded" << data.size() << "bytes from" << url << ", but md5 was not" << md5;
        text->setText( tr( "Download failed, please try again later.") );
        if (!isVisible()) deleteLater();
        return;
    }
    
    bar->setRange( 0, 100 );
    bar->setValue( 100 );
    text->setText( tr( "A new version of Last.fm is available" ) );
#ifdef __APPLE__
    button->setText( tr("Thanks!") ); //will open DMG file now
#else
    button->setText( tr("Quit && Install") );
#endif

    show();

    tmp.setAutoRemove( false ); //TODO naughty! maybe a script that waits on the dmg open complete and then deletes
    tmp.open();
    tmp.write( data );

    disconnect( button, SIGNAL(clicked()), this, SLOT(close()) );
    connect( button, SIGNAL(clicked()), SLOT(install()) );
}

void 
UpdateDialog::install()
{
#ifdef __APPLE__
    qDebug() << tmp.fileName();

    QDesktopServices::openUrl( QUrl::fromLocalFile( tmp.fileName() ) );
    qApp->quit();
    //TODO auto shut this instance if possible
#endif
#ifdef WIN32
    // Must use ShellExecute because otherwise the elevation dialog
    // doesn't appear when launching the installer on Vista.

    QString const path = tmp.fileName();
    tmp.close();
    
    SHELLEXECUTEINFOW sei;
    memset(&sei, 0, sizeof(sei));

    sei.cbSize = sizeof(sei);
    sei.fMask  = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT;
    sei.hwnd   = GetForegroundWindow();
    sei.lpVerb = L"open";
    sei.lpFile = reinterpret_cast<LPCWSTR>(path.utf16());
    sei.lpParameters = 0;
    sei.nShow  = SW_SHOWNORMAL;

    BOOL const b = ShellExecuteExW( &sei );
    if (b) {
        QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
        WaitForSingleObject(sei.hProcess, 10*1000);
        CloseHandle(sei.hProcess);
        QApplication::restoreOverrideCursor();
    }
    else {
        qWarning() << "Couldn't open" << path;
        text->setText( tr("The installer could not be launched") );
    }
    
    qApp->quit();
#endif
}
