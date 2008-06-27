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

#include "lib/moose/MooseCommon.h"
#include "lib/unicorn/UnicornCommon.h"
#include "Settings.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>

#include <QTextCodec>
#include <QTextDecoder>


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

    #ifdef Q_WS_MAC
        url += "&attach=" + Moose::logPath( "Last.fm Twiddly.log" );
    #elif defined WIN32
        url += "&attach=" + Moose::logPath( "Twiddly.log" );
    #endif

    url += "&attach=" + Moose::logPath( "Last.fm.log" );
    
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
            body += "    " + dir;
        }
    }
    
    body += "\n";
    
    body += m_diagnosticsDialogInfo;
    
    body += "\n\n";

    body += "------------------ System information -----------------\n";
    body += "Operating system: " + Unicorn::verbosePlatformString() + "\n\n";
    
#ifdef Q_WS_X11
    body += getSystemInformationUnix();
#endif

    body += "\n\n\n";

    url += "&body=" + body;

    QDesktopServices::openUrl ( QUrl( url ) );
    
    ui.moreInfoTextEdit->clear();
    
    QDialog::accept();
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


#ifdef Q_WS_X11
QString
MailLogsDialog::getSystemInformationUnix()
{
    QString information;
    
    information += "CPU: \n";
    information += runCommand( "cat /proc/cpuinfo" );
    information += "\n";
    
    information += "Memory: \n";
    information += runCommand( "cat /proc/meminfo" );
    information += "\n";
    
    information += "Diskspace: \n";
    information += runCommand( "df -h" );
    information += "\n";
    
    return information;
}
#endif
