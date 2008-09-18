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

#include "UniqueApplication.h"
#include <QDebug>
#include <QStringList>

#ifdef WIN32
class UniqueApplicationWidget : public QWidget
{    
    UniqueApplication* app;
    
    bool winEvent( MSG *msg, long *result )
    {
        if (msg->message != WM_COPYDATA)
            return false;
        
        Q_ASSERT(msg->hwnd == winId());
        COPYDATASTRUCT *data = (COPYDATASTRUCT*)msg->lParam;
        QString message = QString::fromLatin1( (char*)data->lpData, data->cbData / 2 );
        
        emit app->messageReceived( message.split( '\0' ) );
        
        if (result)
            *result = 0;

        return true;
    }
    
};
#endif


UniqueApplication::UniqueApplication( const char* id )
{
#ifdef WIN32
    ::CreateMutexA( NULL, false, id ); //extern const char*

    // The call fails with ERROR_ACCESS_DENIED if the Mutex was 
    // created in a different users session because of passing
    // NULL for the SECURITY_ATTRIBUTES on Mutex creation);
    m_alreadyRunning = ::GetLastError() == ERROR_ALREADY_EXISTS || ::GetLastError() == ERROR_ACCESS_DENIED;
    
    QString const wid = QString(id) + "_UniqueApplicationWidget";

    if (!m_alreadyRunning)
    {
        UniqueApplicationWidget* w = new UniqueApplicationWidget;
        w->setParent( this ); //autodelete
        w->app = this;
        w->setWindowTitle( wid );
        m_hwnd = w->winId();
    }
    else
    {
        m_hwnd = ::FindWindow( L"QWidget", (TCHAR*)wid.utf16() );        
    }
#endif
    
#ifdef Q_WS_MAC
    CFStringRef cfid = CFStringCreateWithCString( NULL, id, kCFStringEncodingISOLatin1 );
    m_port = CFMessagePortCreateRemote( kCFAllocatorDefault, cfid );

    m_alreadyRunning = m_port != 0;

    if (!m_alreadyRunning)
    {
        CFMessagePortContext context;
        context.version = 0;
        context.info = this;
        context.retain = 0;
        context.release = 0;
        context.copyDescription = 0;
        m_port = CFMessagePortCreateLocal( kCFAllocatorDefault, 
                                           cfid, 
                                           MacCallBack, 
                                           &context, 
                                           0 );
        CFRunLoopRef runloop = CFRunLoopGetCurrent();
        if (m_port && runloop) {
            CFRunLoopSourceRef source = CFMessagePortCreateRunLoopSource( 0, m_port, 0 );
            if (source)
                CFRunLoopAddSource( runloop, source, kCFRunLoopCommonModes );
            CFRelease( source );
        }
        CFRelease( cfid );
    }    
#endif
    
#ifdef Q_WS_X11
    qWarning() << "Single application instance code still unwritten!";
    bool const up = false;
#endif
}


bool
UniqueApplication::forward( int argc, char** argv )
{
    QStringList args;
    for (int i = 0; i < argc; ++i)
        args += QString::fromLocal8Bit( argv[i] );
    return forward( args );
}


bool
UniqueApplication::forward( const QStringList& args )
{
    if (args.isEmpty() || !m_alreadyRunning)
        return false;
    
    const uint timeout = 5000; //milliseconds    
    
#ifdef Q_WS_MAC
    if (!m_port) {
        qWarning() << "No CFMessagePort available";
        return false;
    }
    
    QByteArray message;
    foreach (QString const arg, args)
    {
        message += arg.toLatin1();
        message += '\0';
    }
        
    static SInt32 msgid = 0;
    CFDataRef data = CFDataCreate( 0, (UInt8*)message.constData(), message.length() );
    CFDataRef reply = 0;
    SInt32 result = CFMessagePortSendRequest( m_port, ++msgid, data, timeout / 1000, timeout / 1000, 0, &reply );
    CFRelease( data );
    if (reply)
        CFRelease( reply );
    return result == kCFMessagePortSuccess;
#endif
    
#ifdef WIN32
    if (!m_hwnd) {
        qDebug() << "No previous instance found";
        return;
    }

    QString message;
    foreach (QString const arg, args)
    {
        message += arg();
        message += '\0';
    }
    
    COPYDATASTRUCT data;
    data.dwData = 0;
    data.cbData = (message.length()+1) * sizeof(QChar);
    data.lpData = (void*)message.utf16();
    DWORD result;
    LRESULT res = SendMessageTimeoutA( m_hwnd, 
                                       WM_COPYDATA, 
                                       0/*hwnd sender*/, 
                                       (LPARAM)&data,
                                       SMTO_ABORTIFHUNG,
                                       timeout,
                                       &result );
    return res != 0;
#endif
}


#ifdef Q_WS_MAC
CFDataRef //static
UniqueApplication::MacCallBack( CFMessagePortRef, SInt32, CFDataRef data, void *info )
{
    CFIndex index = CFDataGetLength(data);
    const UInt8 *p = CFDataGetBytePtr(data);
    QByteArray ba( index, 0 );
    for (int i = 0; i < index; ++i)
        ba[i] = p[i];

    QString message = QString::fromLatin1( ba.data(), ba.length() - 1 ); //remove ending \0
    emit static_cast<UniqueApplication*>(info)->arguments( message.split( QChar('\0') ) );

    return 0;
}
#endif
