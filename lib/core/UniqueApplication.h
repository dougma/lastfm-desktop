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

#ifndef UNIQUE_APPLICATION_H
#define UNIQUE_APPLICATION_H

#include <QObject>
#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif


/** This is a stub because on OS X, making it derive QApplication causes the 
  * dock icon to flash up temporarily, so you have to do more work as a 
  * consequence. Prolly the same on Windows.
  *
  * Basically, make a unique app, try to forward the command line arguments
  * over, if that fails, continue, otherwise exit( 0 )
  */
class UniqueApplication : public QObject
{
    Q_OBJECT
    
public:
    UniqueApplication( const char* id );

    bool isAlreadyOpen() const { return m_alreadyRunning; }
    
    /** forwards arguments to the running instance, @returns success 
      * we convert using QString::fromLocal8Bit() */
    bool forward( int argc, char** argv );
    bool forward( const class QStringList& );

signals:
    /** the first one is argv[0] ie. the application path */
    void arguments( const class QStringList& );
    
private:
    bool m_alreadyRunning;

#ifdef Q_WS_MAC
    static CFDataRef MacCallBack( CFMessagePortRef, SInt32, CFDataRef data, void* info );
    CFMessagePortRef m_port;
#endif
#ifdef WIN32
    HWND m_hwnd;
#endif
};

#endif
