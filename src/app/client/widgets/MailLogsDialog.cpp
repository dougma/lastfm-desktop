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

#include "MailLogsDialog.h"

#include "Settings.h"

#include "lib/moose/MooseCommon.h"

#include "lib/unicorn/UnicornCommon.h"
#include "lib/unicorn/LastMessageBox.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QClipboard>
#include <QFile>

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif


MailLogsDialog::MailLogsDialog( QWidget *parent )
        : QDialog( parent )
{
    ui.setupUi( this );

    connect( ui.mailButton, SIGNAL( clicked() ), SLOT( onCreateMailClicked() ) );
}

MailLogsDialog::~MailLogsDialog()
{}

void
MailLogsDialog::onCreateMailClicked()
{
    QString url, body;
    url += "mailto:client@last.fm";
    url += "?subject=Client logs from user " + The::settings().username();
    url += "&attach=" + Moose::logPath( "Last.fm.log" );
    
    // -------------------- Log files ------------------------------
    
    QString twiddlyLog;
    #ifdef Q_WS_MAC
        twiddlyLog = Moose::logPath( "Last.fm Twiddly.log" );
    #elif defined WIN32
        twiddlyLog = Moose::logPath( "Twiddly.log" );
    #endif

    QString logFiles = Moose::logPath( "Last.fm.log" ); 
    if ( QFile::exists( twiddlyLog ) )
    {
        url += "&attach=" + twiddlyLog;
        logFiles += " " + tr("and") + " " + twiddlyLog;
    }

    body += "\n" + tr("PLEASE attach %1 if not already attached").arg(logFiles) + "\n\n";

    body += "-------------- User supplied information --------------\n";

    body += ui.moreInfoTextEdit->toPlainText();
    body += "\n\n\n";
    
    body += "------------------ Client information -----------------\n";

    body += "User: " + The::settings().username() + "\n";
    body += "Is using proxy: " + QString::number( The::settings().isUseProxy() ) + "\n";
    if ( The::settings().isUseProxy() )
    {
        body += "Proxy Host: " + The::settings().proxyHost() + "\n";
        body += "Proxy Port: " + QString::number( The::settings().proxyPort() ) + "\n";
    }
    body += "Path: " + The::settings().path() + "\n";
    body += "Version: " + The::settings().version() + "\n";
    body += "Scrobble Point: " + QString::number( The::settings().scrobblePoint() ) + "\n";
    body += "Control Port: " + QString::number( The::settings().controlPort() ) + "\n";
    if ( !The::settings().excludedDirs().isEmpty() )
    {
        body += "Excluded dirs:\n";
        foreach( QString dir, The::settings().excludedDirs() )
        {
            body += "    " + dir + "\n";
        }
    }

    body += "\n" +m_diagnosticsDialogInfo + "\n\n";

    body += "------------------ System information -----------------\n";
    body += getSystemInformation() + "\n\n\n";

    url += "&body=" + body;
    ui.moreInfoTextEdit->clear();

    if ( QDesktopServices::openUrl ( QUrl( url ) ) )
        QDialog::accept();
    else
    {
        LastMessageBox::warning( tr( "Can't open mail client" ), 
                                 tr( "Failed to open your mail client. I stored the information "
                                     "in the clipboard instead so you can paste it in your mail "
                                     "client manualy. Please send it to client@last.fm" ) );
        QApplication::clipboard()->setText( body );
        
        QDialog::accept();
    }
}

QString
MailLogsDialog::runCommand( QString cmd )
{
    QProcess process;

    process.start( cmd );
    process.closeWriteChannel();
    process.waitForFinished();

    return QString( process.readAll() );
}

QString
MailLogsDialog::getSystemInformation()
{
    QString information;
    
    information += "Operating system: " + Unicorn::verbosePlatformString() + "\n\n";

#ifdef Q_WS_X11
    information += "CPU: \n";
    information += runCommand( "cat /proc/cpuinfo" );
    information += "\n";
    
    information += "Memory: \n";
    information += runCommand( "cat /proc/meminfo" );
    information += "\n";
    
    information += "Diskspace: \n";
    information += runCommand( "df -h" );
    information += "\n";

#elif defined WIN32
    // CPU
    SYSTEM_INFO siSysInfo;
    GetSystemInfo(&siSysInfo); 

    information += "CPU: \n";  
    information += "Number of processors: " + QString::number( siSysInfo.dwNumberOfProcessors ) + "\n";
    information += "Page size: " + QString::number( siSysInfo.dwPageSize ) + "\n";
    information += "Processor type: " + QString::number( siSysInfo.dwProcessorType ) + "\n";
    information += "Active processor mask: " + QString::number( siSysInfo.dwActiveProcessorMask ) + "\n";
    information += "\n";

    // Memory
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);

    information += "Memory used: " + QString::number( statex.dwMemoryLoad ) + "%\n";
    information += "Total memory: " + QString::number( statex.ullTotalPhys/(1024*1024) ) + "MB\n";
    information += "Free memory: " + QString::number( statex.ullAvailPhys/(1024*1024) ) + "MB\n";
    information += "Total virtual memory: " + QString::number( statex.ullTotalVirtual/(1024*1024) ) + "MB\n";
    information += "Free virtual memory: " + QString::number( statex.ullAvailVirtual/(1024*1024) ) + "MB\n";

    // Disk space
    __int64 lpFreeBytesAvailable, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes;
    DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;

    GetDiskFreeSpaceEx( NULL,
                        (PULARGE_INTEGER)&lpFreeBytesAvailable,
                        (PULARGE_INTEGER)&lpTotalNumberOfBytes,
                        (PULARGE_INTEGER)&lpTotalNumberOfFreeBytes
                        ); 

    information += "Drive C:\\ \n";
    information += "   Total diskspace: " + QString::number( lpTotalNumberOfBytes/(1024*1024) )+ "MB\n";
    information += "   Free diskspace: " + QString::number( lpFreeBytesAvailable/(1024*1024) )  + "MB\n";

#elif defined Q_WS_MAC
    information += "CPU and Memory: \n";
    information += runCommand( "hostinfo" );
    information += "\n";
    
    information += "Diskspace: \n";
    information += runCommand( "df -h" );
    information += "\n";

#endif

    return information;
}

