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

#include "Logger.h"

#ifndef WIN32
    #include <sys/stat.h>
    #include <pthread.h>
#endif

using namespace std;

/******************************************************************************
    Init
******************************************************************************/
void
Logger::Init(
    LFM_LOGGER_STRING sFilename,
    bool bOverwrite)
{
    mFilePath = sFilename;
    int fileSize = 0;

    // Trim file size if above 500k
    #ifdef WIN32
        HANDLE hFile = ::CreateFileW(mFilePath.c_str(), 
                                     GENERIC_READ,
                                     0, 
                                     NULL,
                                     OPEN_EXISTING, 
                                     FILE_ATTRIBUTE_NORMAL, 
                                     NULL);
        fileSize = ::GetFileSize( hFile, NULL );
        ::CloseHandle( hFile );
    #else
        struct stat st;
        if ( !stat( sFilename.c_str(), &st ) )
            fileSize = st.st_size;
    #endif

    LOG( 3, "Log File Size:" << fileSize );

    if ( fileSize > 500000 )
    {
        ifstream inFile( sFilename.c_str() );
        inFile.seekg( static_cast<streamoff>( fileSize - 400000 ) );
        istreambuf_iterator<char> bufReader( inFile ), end;
        string sFile;
        sFile.reserve( 400005 );
        sFile.assign( bufReader, end );
        inFile.close();
        ofstream outFile( sFilename.c_str() );
        outFile << sFile << flush;
        outFile.close();
    }

    ios::openmode flags = ios::out;
    if (!bOverwrite)
    {
        flags |= ios::app;
    }
    mFileOut.open(sFilename.c_str(), flags);

    if (!mFileOut)
    {
//         qWarning() << "Could not open log file" << sFilename;
        return;
    }

    SetLevel(Debug);

    // Print some initial startup info
//     QString osVer = UnicornUtils::getOSVersion();
    LOG(1, "************************************* STARTUP ********************************************\n");
    LOG(1, "File-size: " << fileSize << "\n");

}

/******************************************************************************
    GetLogger
******************************************************************************/
Logger&
Logger::GetLogger()
{
    // This does only construct one instance of the logger object even if
    // called from lots of different modules. Yay!
    static Logger logger;
    return logger;
}

/******************************************************************************
    Log
******************************************************************************/
void
Logger::Log(
    Severity level,
    string message,
    string function,
    int line)
{
    #ifdef WIN32
        mMutex = ::CreateMutexA( NULL, false, "Logger-12C67D98-22D8-4421-B91C-1CED31B3F3DE" );
    #else
        pthread_mutexattr_t attr;
        pthread_mutexattr_init( &attr );
        pthread_mutex_init( &mMutex, &attr );
    #endif

    if (mFileOut && level <= GetLevel())
    {
        mFileOut <<
            GetTime() << " - " <<
#ifdef QT_CORE_LIB
            std::setw(4) << QThread::currentThreadId() << " - " <<
#endif
            function << "(" << line << ") - " <<
            "L" << level << "\n  " << message << std::endl;
    }

    #ifdef WIN32
        ::CloseHandle( mMutex );
    #else
        pthread_mutex_destroy( &mMutex );
    #endif
}


#ifdef WIN32
/******************************************************************************
    LogW
******************************************************************************/
void
Logger::LogW(
    Severity level,
    wstring message,
    string function,
    int line)
{
    // first call works out required buffer length
    int recLen = WideCharToMultiByte( CP_ACP, 0, message.c_str(), (int)(message.size()), NULL, NULL, NULL, NULL );

    char* buffer = new char[recLen + 1];
    memset(buffer,0,recLen+1);

    // second call actually converts
    WideCharToMultiByte(CP_ACP,0,message.c_str(),(int)(message.size()),buffer,recLen,NULL,NULL);
    std::string s = buffer;
    delete[] buffer;

    Log( level, s, function, line );
}

#endif
