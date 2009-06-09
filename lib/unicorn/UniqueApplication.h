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
#ifndef UNIQUE_APPLICATION_H
#define UNIQUE_APPLICATION_H

#include "lib/DllExportMacro.h"
#include <QObject>
#include <QStringList>
#ifdef Q_WS_MAC
#include <CoreFoundation/CFMessagePort.h>
#endif
#ifdef WIN32
#include <windows.h>
#endif


/** This is a stub because on OS X, making it derive QApplication causes the 
  * dock icon to flash up temporarily, so you have to do more work as a 
  * consequence. Prolly the same on Windows.
  *
  * Basically, make a unique app, try to forward the command line arguments
  * over, or test isAlreadyRunning() if forwarding fails or !isAlreadyRunning(),
  * continue, otherwise exit( 0 )
  *
  * I added this to core because it could be generally useful to Qt 
  * applications. Even though it's a little heavy --mxcl
  */
class UNICORN_DLLEXPORT UniqueApplication : public QObject
{
    Q_OBJECT
    
public:
    UniqueApplication( const char* id );

    /** do this one as soon as possible if you _are_ the unique application,
      * this one sets up the application mutex, so other apps and instances 
      * will know they should forward args or exit */
    void init1();
    /** this one sets up forwarding */
	void init2( const class QCoreApplication* );

	bool isAlreadyRunning() const { return m_alreadyRunning; }

    /** forwards arguments to the running instance, @returns success 
      * we convert using QString::fromLocal8Bit() */
    bool forward( int argc, char** argv );
    bool forward( const class QStringList& );
	/** this function adds a fake argv[0] */
    bool forward( const QString& arg ) { return forward( QStringList() << "fake_argv[0]" << arg ); }

signals:
    /** the first one is argv[0] ie. the application path */
    void arguments( const class QStringList& );

private:
	const char* m_id;
	bool m_alreadyRunning;

#ifdef Q_WS_MAC
    static CFDataRef MacCallBack( CFMessagePortRef, SInt32, CFDataRef data, void* info );
    CFMessagePortRef m_port;
#endif
#ifdef WIN32
	friend class UniqueApplicationWidget;
    HWND m_hwnd;

	QString windowTitle() const { return QString(m_id) + "_UniqueApplicationWidget"; }
#endif
};

#endif
