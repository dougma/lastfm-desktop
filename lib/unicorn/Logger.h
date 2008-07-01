/***************************************************************************
 *   Copyright (C) 2005 - 2008 by                                          *
 *      Christian Muehlhaeuser, Last.fm Ltd <chris@last.fm>                *
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

#ifndef LOGGER_H
#define LOGGER_H

#include "lib/DllExportMacro.h"

#include <string>
#include <fstream>
#include <sstream>
#include <ios>
#include <iomanip>

#include <ctime>
#include <cstdlib>

#ifdef QT_CORE_LIB
    #include <QString>
#endif

#ifdef WIN32
    #define LFM_LOGGER_STRING std::wstring
    #include <windows.h>
#else
    #define LFM_LOGGER_STRING std::string
#endif

#define FUNCTION_NAME ( std::string( __FUNCTION__ ) )
#define LINE_NUMBER ( __LINE__ )

// Global LOG macro
#define LOG(level, msg)                                                      \
{                                                                            \
    std::ostringstream ss;                                                   \
    ss << msg;                                                               \
    Logger& lg = Logger::GetLogger();                            \
    lg.Log( (Logger::Severity)level, ss.str(), FUNCTION_NAME, LINE_NUMBER ); \
}

#define LOGW(level, msg)                                                      \
{                                                                            \
    std::wostringstream ss;                                                   \
    ss << msg;                                                               \
    Logger& lg = Logger::GetLogger();                            \
    lg.LogW( (Logger::Severity)level, ss.str(), FUNCTION_NAME, LINE_NUMBER ); \
}

// Like LOG but with added line break
#define LOGL(level, msg) LOG(level, msg << "\n")

#define LOGWL(level, msg) LOGW(level, msg << L"\n")

// Global VERIFY macro
// Wrap these around statements that should always be executed to verify that
// they're always true (connects are a good example)
#define VERIFY(statement)                                                    \
{                                                                            \
    bool success = (statement);                                              \
    Q_ASSERT( success );                                                     \
}

/*************************************************************************/ /**
    Simple logging class
******************************************************************************/
class UNICORN_DLLEXPORT Logger
{
public:

    enum Severity
    {
        Critical = 1,
        Warning,
        Info,
        Debug
    };

    std::ofstream mFileOut;

#ifdef WIN32
    CRITICAL_SECTION mMutex;
#else
    pthread_mutex_t mMutex;
#endif

    /*********************************************************************/ /**
        Ctor
    **************************************************************************/
    Logger() : mLevel(Warning)
    #ifdef QT_CORE_LIB
        , mDefaultMsgHandler(NULL)
    #endif
    {
        #ifdef WIN32
            InitializeCriticalSection( &mMutex );
        #endif
    }

    /*********************************************************************/ /**
        Dtor
    **************************************************************************/
    virtual
    ~Logger()
    {
        mFileOut.close();
        #ifdef WIN32
            DeleteCriticalSection( &mMutex );
        #endif
    }

    /*********************************************************************/ /**
        Initialises the logger.

        @param[in] sFilename The file to log to.
        @param[in] bOverwrite If true, the file is wiped before starting.
    **************************************************************************/
    void
    Init(
        LFM_LOGGER_STRING sFilename,
        bool bOverwrite = true);
        
        
    #ifdef QT_CORE_LIB
    void
    Init( 
        QString path,
        bool overwrite = true )
    {
        #ifdef WIN32
            Init( path.toStdWString(), overwrite );
        #else
            Init( path.toStdString(), overwrite );
        #endif
    }
    #endif

    /*********************************************************************/ /**
        Returns the static Logger object.
    **************************************************************************/
    static Logger&
    GetLogger();

    /*********************************************************************/ /**
        Called by LOG macro to do the outputting.
    **************************************************************************/
    void
    Log(
        Severity level,
        std::string message,
        std::string function,
        int line );

    /*********************************************************************/ /**
        Used by redirected qDebug in release builds. Doesn't add any text.
    **************************************************************************/
    void
    Log(
        const char* message );

    #ifdef WIN32
    /*********************************************************************/ /**
        Called by LOGW macro to do the outputting of wide characters.
    **************************************************************************/
    void
    LogW(
        Severity level,
        std::wstring message,
        std::string function,
        int line );
    #endif
    
    /*********************************************************************/ /**
        Sets debug level.
    **************************************************************************/
    void
    SetLevel(
        Severity level) { mLevel = level; }

    /*********************************************************************/ /**
        Gets debug level.
    **************************************************************************/
    int
    GetLevel() { return mLevel; }

    /*********************************************************************/ /**
        Gets current logfile path.
    **************************************************************************/
    LFM_LOGGER_STRING
    GetFilePath() const { return mFilePath; }

    /*********************************************************************/ /**
        Returns formatted time string.
    **************************************************************************/
    static std::string
    GetTime()
    {
        time_t now;
        time(&now);
        struct tm* tmnow;
        tmnow = gmtime(&now);
        char acTmp[128];
        strftime(acTmp, 127, "%y%m%d %H:%M:%S", tmnow);
        std::string sTime(acTmp);
        return sTime;
    }

private:
    Severity mLevel;
    LFM_LOGGER_STRING mFilePath;

public:
  #ifdef QT_CORE_LIB
    QtMsgHandler mDefaultMsgHandler;
  #endif

};


#ifdef QT_CORE_LIB

#include <QThread>
#include <QDebug>
#include <QtGlobal>

/*************************************************************************/ /**
    Extra inserter to handle QStrings.
******************************************************************************/
inline std::ostream&
operator<<(std::ostream& os, const QString& qs)
{
    os << qs.toAscii().data();
    return os;
}

#endif //QT_CORE_LIB

#endif //LOGGER_H
