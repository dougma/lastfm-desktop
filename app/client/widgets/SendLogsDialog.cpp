/***************************************************************************
 *   Copyright 2005-2008 Last.fm Ltd                                       *
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

#include "SendLogsDialog.h"
#include "SendLogsRequest.h"
#include "Settings.h"
#include "lib/core/MessageBoxBuilder.h"
#include "lib/unicorn/UnicornDir.h"
#include "lib/unicorn/UnicornUtils.h"
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QMovie>

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif

#include "version.h"


SendLogsDialog::SendLogsDialog( QWidget *parent )
              : QDialog( parent )
{
    ui.setupUi( this );
    
    ui.spinner->setMovie( new QMovie( ":/spinner.mng" ) );
    ui.spinner->movie()->setParent( this );
    ui.spinner->hide();
    
    ui.buttonBox->addButton( "Send", QDialogButtonBox::AcceptRole );

    connect( ui.buttonBox, SIGNAL( accepted() ), SLOT( onSendClicked() ) );
}


static QString clientInformationString()
{
    Moose::Settings settings;
    QString s;

    s += "User: " + settings.username() + "\n";
    s += "Is using proxy: " + QString::number( settings.isUseProxy() ) + "\n";
    if ( settings.isUseProxy() )
    {
        s += "Proxy Host: " + settings.proxyHost() + "\n";
        s += "Proxy Port: " + QString::number( settings.proxyPort() ) + "\n";
    }
    s += "Path: " + settings.path() + "\n";
    s += "Version: " + settings.version() + "\n";
    s += "Scrobble Point: " + QString::number( settings.scrobblePoint() ) + "\n";
    s += "Control Port: " + QString::number( settings.controlPort() ) + "\n";
    if ( !settings.excludedDirs().isEmpty() )
    {
        s += "Excluded dirs:\n";
        foreach( QString dir, settings.excludedDirs() )
        {
            s += "    " + dir + "\n";
        }
    }

    return s;
}


static QString systemInformationString()
{
    QString s;

    s += "Operating system: " + Unicorn::verbosePlatformString() + "\n\n";

#ifdef Q_WS_X11
    s += "CPU: \n";
    s += runCommand( "cat /proc/cpuinfo" );
    s += "\n";

    s += "Memory: \n";
    s += runCommand( "cat /proc/meminfo" );
    s += "\n";

    s += "Diskspace: \n";
    s += runCommand( "df -h" );
    s += "\n";

#elif defined WIN32
    // CPU
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo); 

    s += "CPU: \n";  
    s += "Number of processors: " + QString::number( siSysInfo.dwNumberOfProcessors ) + "\n";
    s += "Page size: " + QString::number( siSysInfo.dwPageSize ) + "\n";
    s += "Processor type: " + QString::number( siSysInfo.dwProcessorType ) + "\n";
    s += "Active processor mask: " + QString::number( siSysInfo.dwActiveProcessorMask ) + "\n";
    s += "\n";

    // Memory
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);

    s += "Memory used: " + QString::number( statex.dwMemoryLoad ) + "%\n";
    s += "Total memory: " + QString::number( statex.ullTotalPhys/(1024*1024) ) + "MB\n";
    s += "Free memory: " + QString::number( statex.ullAvailPhys/(1024*1024) ) + "MB\n";
    s += "Total virtual memory: " + QString::number( statex.ullTotalVirtual/(1024*1024) ) + "MB\n";
    s += "Free virtual memory: " + QString::number( statex.ullAvailVirtual/(1024*1024) ) + "MB\n";

    // Disk space
    __int64 lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;

    GetDiskFreeSpaceEx( NULL,
        (PULARGE_INTEGER)&lpFreeBytesAvailable,
        (PULARGE_INTEGER)&lpTotalNumberOfBytes,
        (PULARGE_INTEGER)&lpTotalNumberOfFreeBytes
        ); 

    s += "Drive C:\\ \n";
    s += "   Total diskspace: " + QString::number( lpTotalNumberOfBytes/(1024*1024) )+ "MB\n";
    s += "   Free diskspace: " + QString::number( lpFreeBytesAvailable/(1024*1024) )  + "MB\n";

#elif defined Q_WS_MAC
    s += "CPU and Memory: \n";
    s += Unicorn::runCommand( "hostinfo" );
    s += "\n";

    s += "Diskspace: \n";
    s += Unicorn::runCommand( "df -h" );
    s += "\n";

#endif

    return s;
}


void
SendLogsDialog::onSendClicked()
{
    SendLogsRequest* request = new SendLogsRequest( PRODUCT_NAME, VERSION, ui.moreInfoTextEdit->toPlainText() );
    connect( request, SIGNAL( success() ), SLOT( onSuccess() ) );
    connect( request, SIGNAL( error() ), SLOT( onError() ) );

    QDir logDir = UnicornDir::logs();
    QStringList logExt( "*.log" );
        
    // find logs
    logDir.setFilter( QDir::Files | QDir::Readable );
    logDir.setNameFilters( logExt );
    QList<QFileInfo> logFiles = logDir.entryInfoList();
    
    foreach( QFileInfo log, logFiles )
        request->addLog( log.completeBaseName(), log.absoluteFilePath() );

    request->addLogData( "clientinfo", clientInformationString() );
    request->addLogData( "sysinfo", systemInformationString() );
    
    request->send();
    
    ui.buttonBox->setEnabled( false );
    ui.moreInfoTextEdit->setEnabled( false );
    ui.spinner->show();
    ui.spinner->movie()->start();
}


void
SendLogsDialog::onSuccess()
{
    ui.spinner->movie()->stop();
    ui.spinner->hide();

    MessageBoxBuilder( this )
        .setIcon( QMessageBox::Information )
        .setTitle( tr("Logs sent") )
        .setText( tr( "Thank you, we will get back to you as soon as possible.") )
        .exec();

    ui.moreInfoTextEdit->clear();
    ui.buttonBox->setEnabled( true );
    ui.moreInfoTextEdit->setEnabled( true );
    QDialog::accept();
}


void
SendLogsDialog::onError()
{
    MessageBoxBuilder( this )
        .setIcon( QMessageBox::Information )
        .setTitle( tr("Couldn't send logs") )
        .setText( tr("Please try again later.") )
        .exec();

    ui.spinner->movie()->stop();
    ui.spinner->hide();
    ui.buttonBox->setEnabled( true );
    ui.moreInfoTextEdit->setEnabled( true );
}

