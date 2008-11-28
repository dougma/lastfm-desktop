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
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#include "WsConnectionMonitor.h"
#include "WsKeys.h"
#include <QDebug>
#ifdef __APPLE__
#include <QPointer>
#include <SystemConfiguration/SCNetworkReachability.h>
QList<QPointer<WsConnectionMonitor> > monitors;
#endif


WsConnectionMonitor::WsConnectionMonitor( QObject *parent )
                   : QObject( parent )
{
#ifdef __APPLE__
    if (monitors.isEmpty())
    {
        SCNetworkReachabilityRef ref = SCNetworkReachabilityCreateWithName( NULL, LASTFM_WS_HOSTNAME );
        SCNetworkReachabilityScheduleWithRunLoop( ref, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode );
        SCNetworkReachabilitySetCallback( ref, callback, NULL );
        CFRelease( ref );
    }
    
    QPointer<WsConnectionMonitor> p = this;
    monitors += p;
#endif
}


#ifdef __APPLE__
void
WsConnectionMonitor::callback( SCNetworkReachabilityRef, SCNetworkConnectionFlags flags, void* )
{
    static bool up = true;
    
    qDebug() << flags;    
    
    // I couldn't find any diffinitive usage examples for these flags
    // so I had to guess, since I can't test, eg. dial up :(
    
    bool b;
    if (flags & kSCNetworkFlagsConnectionRequired)
        b = false;
    else
        b = flags & (kSCNetworkFlagsReachable | kSCNetworkFlagsTransientConnection | kSCNetworkFlagsConnectionAutomatic);
    
    qDebug() << b;
    
    // basically, avoids telling everyone that we're up already on startup
    if (up == b) return;
    up = b;
    
    foreach (WsConnectionMonitor* monitor, monitors)
        if (monitor)
            if (b)
                emit monitor->up();
            else
                emit monitor->down();
}
#endif
