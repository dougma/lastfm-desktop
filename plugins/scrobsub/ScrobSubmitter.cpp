/* Copyright 205-2009, Last.fm Ltd. <client@last.fm>                       
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY <copyright holder> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Stops warnings about deprecated CRT functions
#ifndef _CRT_SECURE_NO_DEPRECATE
    #define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _SCL_SECURE_NO_DEPRECATE
    #define _SCL_SECURE_NO_DEPRECATE
#endif

#include "ScrobSubmitter.h"
#include "EncodingUtils.h"

#include <windows.h>
#include <tchar.h>
#include <process.h>
#include <shfolder.h>

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

extern DWORD scrobSubPipeName(string* pipeName);
extern string formatWin32Error(DWORD error);

/******************************************************************************
    Constants
******************************************************************************/
static const char* kVersion = "1.1.1";
static const int   kLaunchWait = 60000; // in ms

/******************************************************************************
    ScrobSubmitter
******************************************************************************/
ScrobSubmitter::ScrobSubmitter() :
    mNextId(0),
    mStopThread(false),
    mLaunchTime(0)
{
    InitializeCriticalSection(&mMutex);
    
    mRequestAvailable = CreateEvent( 
        NULL,         // no security attributes
        TRUE,         // manual reset event
        FALSE,        // initial state is non-signaled
        NULL);        // object name

    mExit = CreateEvent( 
        NULL,         // no security attributes
        TRUE,         // manual reset event
        FALSE,        // initial state is non-signaled
        NULL);        // object name
}

/******************************************************************************
   ~ScrobSubmitter
******************************************************************************/
ScrobSubmitter::~ScrobSubmitter()
{
    // Not using dtor for thread termination as we can't be sure the client
    // is still alive so the status callbacks might fail.

    DeleteCriticalSection(&mMutex);

    CloseHandle(mRequestAvailable);
    CloseHandle(mExit);
}

/******************************************************************************
   Term
******************************************************************************/
void
ScrobSubmitter::Term()
{
    SendToAS( "TERM c=" + mPluginId + '\n' );
    // Signal socket thread to exit
    mStopThread = true;
    SetEvent(mExit);

    // Wait for it to die before leaving
    DWORD waitResult = WaitForSingleObject(mSocketThread, 5000);
    if ( waitResult == WAIT_TIMEOUT || waitResult == WAIT_FAILED )
    {
        ostringstream os;
        os << "Wait for ScrobSubmitter thread termination " <<
            ( ( waitResult == WAIT_TIMEOUT ) ? "timed out. " : "failed. " ) <<
            "GetLastError() " << GetLastError();
        ReportStatus( -1, true, os.str() );
    }
    
    CloseHandle(mSocketThread);
}    

/******************************************************************************
    Init
******************************************************************************/
void
ScrobSubmitter::Init(
    const string& pluginId,
    StatusCallback callback,
    void* userData)
{
    mPluginId = pluginId;
    mpReportStatus = callback;
    mpUserData = userData;
        
    bool isAutoLaunchEnabled = true;

    // Read autolaunch option from registry
    HKEY regKeyAS;
    LONG lResult = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        _T("Software\\Last.fm\\Client"),
        0,              // reserved
        KEY_READ,       // access mask
        &regKeyAS);

    if (lResult == ERROR_SUCCESS)
    {
        TCHAR acBoolAsString[10];
        DWORD nSize = 10;
        lResult = RegQueryValueEx(
            regKeyAS,                                 // key
            _T("LaunchWithMediaPlayer"),              // value to query
            NULL,                                     // reserved
            NULL,                                     // tells you what type the value is
            reinterpret_cast<LPBYTE>(acBoolAsString), // store result here
            &nSize);

        if (lResult == ERROR_SUCCESS)
        {
            isAutoLaunchEnabled = _tcscmp(acBoolAsString, _T("true")) == 0;
        }

        RegCloseKey(regKeyAS);
    }
    
    if ( isAutoLaunchEnabled )
    {
        LaunchClient();
    }

    DWORD err = scrobSubPipeName(&mPipeName);
    if (err)
    {
        ReportStatus(-1, true, formatWin32Error(err));
    }

    // Start socket thread
    unsigned threadId;

    mSocketThread = reinterpret_cast<HANDLE>(_beginthreadex( 
        NULL,                           // security crap
        0,                              // stack size
        SendToASThreadMain,             // start function
        reinterpret_cast<void*>(this),  // argument to thread
        0,                              // run straight away
        &threadId));                    // thread id out
    
    if (mSocketThread == 0) // Error
    {
        ReportStatus(-1, true, strerror(errno));
    }

    ConnectToAS( 0 );
}

/******************************************************************************
    LaunchClient
******************************************************************************/
bool
ScrobSubmitter::LaunchClient()
{
    TCHAR acExe[MAX_PATH + 1];
    DWORD nPathSize = MAX_PATH + 1;
    string sPath;

    acExe[0] = _T('\0');

    // Open HKCU key
    HKEY regKeyAS;
    LONG lResult = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        _T("Software\\Last.fm\\Client"),
        0,              // reserved
        KEY_READ,       // access mask
        &regKeyAS);
    
    if (lResult == ERROR_SUCCESS)
    {
        // Try and read exe path
        lResult = RegQueryValueEx(
            regKeyAS,                           // key
            _T("Path"),                         // value to query
            NULL,                               // reserved
            NULL,                               // tells you what type the value is
            reinterpret_cast<LPBYTE>(acExe),    // store result here
            &nPathSize);

        RegCloseKey(regKeyAS);
    }

    if ( _tcslen( acExe ) == 0 )
    {
        // Couldn't read path from HKCU, try HKLM
        lResult = RegOpenKeyEx(
            HKEY_LOCAL_MACHINE,
            _T("Software\\Last.fm\\Client"),
            0,              // reserved
            KEY_READ,       // access mask
            &regKeyAS);
    
        if (lResult == ERROR_SUCCESS)
        {
            nPathSize = MAX_PATH + 1;

            // Try and read exe path
            lResult = RegQueryValueEx(
                regKeyAS,                           // key
                _T("Path"),                         // value to query
                NULL,                               // reserved
                NULL,                               // tells you what type the value is
                reinterpret_cast<LPBYTE>(acExe),    // store result here
                &nPathSize);

            RegCloseKey(regKeyAS);
        }
    }

    if ( _tcslen( acExe ) == 0 )
    {
        ReportStatus(-1, true,
            "Couldn't read the Last.fm exe path from the registry.");
        return false;
    }

    // Work out what the app dir is
    TCHAR defaultDir[MAX_PATH + 1];
    _tcsncpy( defaultDir, acExe, _tcsclen( acExe ) + 1 ); // +1 for null terminator

    // Look for last backslash
    TCHAR* pos = _tcsrchr( defaultDir, _T('\\') );
    if ( pos == NULL )
    {
        // Path might also use forward slashes
        pos = _tcsrchr( defaultDir, _T('/') );

        if ( pos == NULL )
        {
            ReportStatus(-1, true,
                "Last.fm exe path from registry invalid.");
            return false;
        }
    }
    
    // Truncate so that defaultDir just contains the dir
    pos[0] = _T('\0');
    
    // Since the client doesn't allow multiple instances, we can just kick it off
    // regardless of whether it's running or not.
    HINSTANCE h = ShellExecute(
        NULL, _T("open"), acExe, _T("--tray"), defaultDir, SW_SHOWNORMAL);
        
    if ( h <= reinterpret_cast<void*>( 32) ) // Error
    {
        // Invalid handle means it didn't launch
        ostringstream os;
        os << "Failed launching Last.fm client. ShellExecute error: " << h;
        ReportStatus(-1, true, os.str());
        return false;
    }
    
    ReportStatus(-1, false, "Launched Last.fm client.");
    
    // Store time of launch
    mLaunchTime = GetTickCount();
    
    return true;
}

/******************************************************************************
    Start
******************************************************************************/
int
ScrobSubmitter::Start(
    string artist,
    string track,
    string album,
    string mbId,
    int    length,
    string filename,
    Encoding encoding)
{
    if (encoding != UTF_8)
    {
        ConvertToUTF8(artist, encoding);
        ConvertToUTF8(track, encoding);
        ConvertToUTF8(album, encoding);
        ConvertToUTF8(mbId, encoding);
        ConvertToUTF8(filename, encoding);
    }

    ostringstream osCmd;
    osCmd << "START c=" << mPluginId        << "&" <<
                   "a=" << Escape(artist)   << "&" <<
                   "t=" << Escape(track)    << "&" <<
                   "b=" << Escape(album)    << "&" <<
                   "m=" << Escape(mbId)     << "&" <<
                   "l=" << length           << "&" <<
                   "p=" << Escape(filename) << endl;

    string cmd = osCmd.str();

    return SendToAS(cmd);
}

/******************************************************************************
    Stop
******************************************************************************/
int
ScrobSubmitter::Stop()
{
    ostringstream osCmd;
    osCmd << "STOP c=" << mPluginId << endl;
    string sCmd = osCmd.str();

    return SendToAS(sCmd);
}

/******************************************************************************
    Pause
******************************************************************************/
int
ScrobSubmitter::Pause()
{
    ostringstream osCmd;
    osCmd << "PAUSE c=" << mPluginId << endl;
    string sCmd = osCmd.str();

    return SendToAS(sCmd);
}

/******************************************************************************
    Resume
******************************************************************************/
int
ScrobSubmitter::Resume()
{
    ostringstream osCmd;
    osCmd << "RESUME c=" << mPluginId << endl;
    string sCmd = osCmd.str();

    return SendToAS(sCmd);
}

// FIXME: give functions verb names to make it clear what they do
/******************************************************************************
    BootstrapComplete
******************************************************************************/
int
ScrobSubmitter::BootstrapComplete( const WCHAR* username )
{
    char usernameUtf8[255];
    usernameUtf8[EncodingUtils::UnicodeToUtf8( username, static_cast<int>( wcslen( username ) ), usernameUtf8, 254 )] = '\0';
    ostringstream osCmd;
    osCmd << "BOOTSTRAP c=" << mPluginId << "&" << "u=" << usernameUtf8 << endl;

    return SendToAS( osCmd.str() );
}

/******************************************************************************
    GetVersion
******************************************************************************/
string
ScrobSubmitter::GetVersion()
{
    return kVersion;
}

/******************************************************************************
    GetLogPath
******************************************************************************/
string
ScrobSubmitter::GetLogPath()
{
    // This might not work on Win98 and earlier but we're not officially
    // supporting them anyway. Upgrading to IE5 will solve the problem.
    char acPath[MAX_PATH];
    HRESULT h = SHGetFolderPathA( NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
                                  NULL, 0, acPath );
	std::string sPath;
	if ( h == S_OK )
    {
        sPath = acPath;
		sPath += "\\Last.fm\\Client";
    }
    else
    {
		sPath = "C:\\Last.fm\\Client";
    }

    return sPath;
}

/******************************************************************************
    GetLogPathW
******************************************************************************/
wstring
ScrobSubmitter::GetLogPathW()
{
    // This might not work on Win98 and earlier but we're not officially
    // supporting them anyway. Upgrading to IE5 will solve the problem.
    WCHAR acPath[MAX_PATH];
    HRESULT h = SHGetFolderPathW( NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
        NULL, 0, acPath );
    std::wstring sPath;
    if ( h == S_OK )
    {
        sPath = acPath;
        sPath += L"\\Last.fm\\Client";
    }
    else
    {
        sPath = L"C:\\Last.fm\\Client";
    }

    return sPath;
}

/******************************************************************************
    SendToAS
******************************************************************************/
int
ScrobSubmitter::SendToAS(
    const std::string& cmd)
{
    if (mPluginId == "")
    {
        ReportStatus(-1, true,
            "Init hasn't been called with a plugin ID");
        return -1;
    }

    EnterCriticalSection(&mMutex);

    // Push the cmd on the queue
    mRequestQueue.push_back(make_pair(++mNextId, cmd));

    // If queue was empty, signal socket thread
    if (mRequestQueue.size() == 1)
    {
        SetEvent(mRequestAvailable);
    }
    
    LeaveCriticalSection(&mMutex);
    
    return mNextId;
}

/******************************************************************************
    SendToASThread
******************************************************************************/
void
ScrobSubmitter::SendToASThread()
{
    // By giving mRequestAvailable the lower index, it will take priority
    // when calling WaitForMultipleObjects and both events are signalled.
    HANDLE eventArray[2] = { mRequestAvailable, mExit };

    while (mRequestQueue.size() > 0 || !mStopThread)
    {
        DWORD signalledHandle = WaitForMultipleObjects(
            2, eventArray, FALSE, INFINITE);
        
        // Check if it's the exit event
        if ((signalledHandle - WAIT_OBJECT_0) == 1) { continue; }

        EnterCriticalSection(&mMutex);

        // Pick first request from queue
        pair<int, string> reqPair = mRequestQueue.front();
        mRequestQueue.pop_front();

        // This means we will eat all the requests in the queue before
        // waiting again
        if (mRequestQueue.size() == 0)
        {
            ResetEvent(mRequestAvailable);
        }

        LeaveCriticalSection(&mMutex);
        
        int nId = reqPair.first;
        string& sCmd = reqPair.second;

        #define RESPONSE_BUFFER_SIZE 1024
        #define PIPE_TIMEOUT 3000   // ms

        char responseBuffer[RESPONSE_BUFFER_SIZE];
        DWORD bytesRead;

        BOOL success = CallNamedPipeA(
            mPipeName.data(), 
            (LPVOID) sCmd.data(), (DWORD) sCmd.size(),
            responseBuffer, RESPONSE_BUFFER_SIZE, 
            &bytesRead,
            PIPE_TIMEOUT);

        if (success)
        {
            std::string sResponse(responseBuffer, bytesRead);
            bool error = sResponse.substr(0, 2) != "OK";
            ReportStatus(nId, error, sResponse);
        }
        else
        {
            // the only acceptable error is file not found 
            // (probably meaning the client is not running)
            DWORD err = GetLastError();
            if (err != ERROR_FILE_NOT_FOUND) 
            {
                ReportStatus(nId, true, formatWin32Error(err));
                break;
            }
        }

    } // end while

    _endthreadex( 0 );
}

/******************************************************************************
    ConnectToAS
******************************************************************************/
bool
ScrobSubmitter::ConnectToAS(
    int reqId)
{
    LPSTR cmdLine = GetCommandLineA();

    ostringstream init;
    init << "INIT c="
         << mPluginId
         << "&f="
         << cmdLine
         << endl;

    SendToAS( init.str() );
    return true;
}

/******************************************************************************
    ReportStatus
******************************************************************************/
void
ScrobSubmitter::ReportStatus(
    int reqId,
    bool error,
    const string& msg)
{
    if (!mStopThread && (mpReportStatus != NULL))
    {
        (*mpReportStatus)(reqId, error, msg, mpUserData);
    }
}    

/******************************************************************************
    ConvertToUTF8
******************************************************************************/
void
ScrobSubmitter::ConvertToUTF8(
	string&  text,
	Encoding encoding)
{
    switch (encoding)
    {
        case ISO_8859_1:
        {
            // A UTF-8 string can be up to 4 times as big as the ANSI
            size_t nUtf8MaxLen = text.size() * 4 + 1;
            char* pcUtf8 = new char[nUtf8MaxLen];
            EncodingUtils::AnsiToUtf8(text.c_str(),
                                      pcUtf8,
                                      static_cast<int>(nUtf8MaxLen));
            text = pcUtf8;
            delete[] pcUtf8;
        }
        break;
        
    }
}	

/******************************************************************************
    Escape
******************************************************************************/
string&
ScrobSubmitter::Escape(
    string& text)
{
    string::size_type nIdx = text.find("&");
    while (nIdx != string::npos)
    {
        text.replace(nIdx, 1, "&&");
        nIdx = text.find("&", nIdx + 2);
    }

    return text;
}    

