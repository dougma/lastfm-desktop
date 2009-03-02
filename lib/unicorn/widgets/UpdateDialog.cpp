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
#include <QNetworkReply>


UpdateDialog::UpdateDialog( QWidget* parent ) : QProgressDialog( parent ), checking( 0 )
{
    setMinimumDuration( 60 * 60 * 1000 ); //don't fucking show mother fucker
    setRange( 0, 0 ); //indeterminate
    setLabelText( tr("Checking for updates...") );
    setAutoClose( false );
    setAttribute( Qt::WA_DeleteOnClose );

    QUrl url( "http://cdn.last.fm/client/" + qApp->applicationName().toLower() + '/' + qApp->applicationVersion() + ".txt" );
    checking = nam.get( QNetworkRequest(url) );
    checking->setParent( this );

    connect( checking, SIGNAL(finished()), SLOT(onGot()) );
}    

void
UpdateDialog::onGot()
{
    QByteArray data = static_cast<QNetworkReply*>(sender())->readAll();
    url = QUrl::fromEncoded( data.mid( 32 ) );
    md5 = data.left( 32 );

    if (url.isEmpty() || !url.isValid()) 
    {
        if (!isVisible())
            deleteLater();
        else {
            setLabelText( tr("No updates available") );
            setRange( 0, 100 );
            setValue( 100 );
        }            
        qDebug() << "No updates available";
    }
    else {
        QNetworkReply* reply = nam.get( QNetworkRequest(url) );
        reply->setParent( this );
        connect( reply, SIGNAL(downloadProgress( qint64, qint64 )), SLOT(onProgress( qint64, qint64 )) );
        connect( reply, SIGNAL(finished()), SLOT(onDownloaded()) );
    }
}

void
UpdateDialog::onProgress( qint64 received, qint64 total )
{
    setMaximum( total );
    setValue( received );
}

void 
UpdateDialog::onDownloaded()
{    
    QByteArray data = static_cast<QNetworkReply*>(sender())->readAll();
    
    if (Qt::md5( data ) != md5) {
        qWarning() << "Downloaded" << data.size() << "bytes from" << url << ", but md5 was not" << md5;
        if (isVisible()) setLabelText( tr( "Download failed. Please try again later.") );
        return;
    }
    
    setMaximum( 100 );
    setValue( 100 );
    setLabelText( tr( "Download complete" ) );
#ifdef Q_OS_MAC
    setCancelButtonText( tr("Quit") ); //will open DMG file now
#else
    setCancelButtonText( tr("Quit & Install") );
#endif

    show();

    tmp.open();
    tmp.write( data );
    tmp.close();

    tmp.setAutoRemove( false );
    
    connect( this, SIGNAL(canceled()), SLOT(install()) );
}

void 
UpdateDialog::install()
{
#ifdef __APPLE__
    QDesktopServices::openUrl( QUrl::fromLocalFile( tmp.fileName() ) );
    //TODO auto shut this instance if possible
#endif
#ifdef WIN32
    // Must use ShellExecute because otherwise the elevation dialog
    // doesn't appear when launching the installer on Vista.

    QString const path = tmp.fileName();

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
        setLabelText( tr("The installer could not be launched") );
    }
    
    qApp->quit();
#endif
}
