/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
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
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02111-1307, USA.          *
 ***************************************************************************/

#include "ScrobSubmitter.h"
#include "Bootstrap.h"
#include "resource.h"
#include "EncodingUtils.h"

#include <process.h> // for beginthreadex
#include <commctrl.h> // for PBM_SETRANGE

#include <iostream>
#include <sstream>

std::map< HWND, BootStrap* > BootStrap::m_hwndMap;

BootStrap::BootStrap( const WCHAR* mediaPlayerName )
          :m_mediaPlayerName( mediaPlayerName ),
           m_hwndParent( NULL ),
           m_bootStrapRequired( false ),
           m_bootStrapHkey( NULL ),
           m_dialogReady( false ),
           m_cancel( false ),
           m_paused( false ),
           m_scrobSub( NULL ),
           m_progressRange( 0 ),
           m_hwndDialog( NULL ),
           m_hwndProgressBar( NULL ),
           m_progressDialogShown( NULL ),
           m_pauseStateChanged( NULL ),
           m_tracksFound( false )
{
    log( "BootStrap ctor" );

    xmlBuffer << L"<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl;
    xmlBuffer << L"<bootstrap version=\"1.0\" product=\"" << mediaPlayerName << "\" >\n";
   
    // If this fails the Bootstrap Required status will be set to false therefore it is safe to continue;
    try
    {
        m_bootStrapHkey = RegistryUtils::GetKey( HKEY_CURRENT_USER, "Software\\Last.fm\\Bootstrap" );
    }
    catch( RegistryUtils::CRegistryException e )
    {
        log( "Warning: bootstrap registry key not found" );
    }

    //Bootstrap is not required therefore don't bother reading the localised strings
    if( !(m_bootStrapRequired = queryBootStrapStatus()) )
        return;

    // FIXME: error handling. If no strings, it just prints a message and continues. Yet these strings
    // are used throughout the class. We should fall back on defaults in case of errors.
    try
    {
        HKEY stringsKey = RegistryUtils::GetKey( HKEY_CURRENT_USER, "Software\\Last.fm\\Bootstrap\\Strings" );
        
        WCHAR progressLabelBuffer[255];
        RegistryUtils::QueryString( stringsKey, L"progress_label", progressLabelBuffer, 255, false );
        m_strProgressLabel = progressLabelBuffer;

        WCHAR completeLabelBuffer[255];
        RegistryUtils::QueryString( stringsKey, L"complete_label", completeLabelBuffer, 255, false );
        m_strCompleteLabel = completeLabelBuffer;

        WCHAR progressTitleBuffer[255];
        RegistryUtils::QueryString( stringsKey, L"progress_title", progressTitleBuffer, 255, false );
        m_strProgressTitle = progressTitleBuffer;

        WCHAR cancelConfirmationBuffer[255];
        RegistryUtils::QueryString( stringsKey, L"cancel_confirmation", cancelConfirmationBuffer, 255, false );
        m_strCancelConfirmation = cancelConfirmationBuffer;
        
        WCHAR noTracksFoundBuffer[255];
        RegistryUtils::QueryString( stringsKey, L"no_tracks_found", noTracksFoundBuffer, 255, false );
        m_strNoTracksFoundLabel = noTracksFoundBuffer;

    }
    catch( const RegistryUtils::CRegistryException& e )
    {
        std::ostringstream os;
        os << "Error loading strings from registry: " << e.what();
        log( os.str() );
        
        //Set default strings to english
        m_strCancelConfirmation = L"Are you sure you want to cancel the import?";
        m_strCompleteLabel = L"Last.fm has imported your media library.\n\n Click OK to continue.\n\n Click OK to continue.";
        m_strProgressLabel = L"Last.fm is importing your current media library...";
        m_strProgressTitle = L"Last.fm Library Import";
        m_strNoTracksFoundLabel = L"Last.fm couldn't find any played tracks in your media library.";
    }

    // FIXME: error handling. What will happen if it fails to get the version numbers?
    try
    {
        HKEY localClientKey = RegistryUtils::GetKey( HKEY_LOCAL_MACHINE, "Software\\Last.fm\\Client", false );

        char strVersionBuffer[25];
        try
        {
            RegistryUtils::QueryString( localClientKey, "Version", strVersionBuffer, 25, false  );
        }
        catch( RegistryUtils::CRegistryException& e )
        {
           log( "Registry exception when querying for version number" );
        }

        std::string strVersion = strVersionBuffer;
        
        int prevDotPos = 0;
        int dotPos = strVersion.find( '.', prevDotPos );
        m_clientVerMajor = atoi( strVersion.substr( prevDotPos, dotPos - prevDotPos).c_str() );

        prevDotPos = dotPos + 1;
        dotPos = strVersion.find( '.', prevDotPos );
        m_clientVerMinor = atoi( strVersion.substr( prevDotPos, dotPos - prevDotPos).c_str() );

        prevDotPos = dotPos + 1;
        dotPos = strVersion.find( '.', prevDotPos );
        m_clientVerRelease = atoi( strVersion.substr( prevDotPos, dotPos - prevDotPos).c_str() );

        prevDotPos = dotPos + 1;
        dotPos = strVersion.find( '.', prevDotPos );
        m_clientVerFix = atoi( strVersion.substr( prevDotPos, dotPos - prevDotPos).c_str() );

    }
    catch ( RegistryUtils::CRegistryException e )
    {
        log( "Error reading version from registry" );
    }

    m_replacementMap['&'] = L"&amp;";
    m_replacementMap['<'] = L"&lt;";
    m_replacementMap['>'] = L"&gt;";

    m_progressDialogShown = CreateEvent( 
        NULL,         // no security attributes
        TRUE,         // manual reset event
        FALSE,        // initial state is non-signaled
        NULL);        // object name
        
    m_pauseStateChanged = CreateEvent( 
        NULL,         // no security attributes
        TRUE,        // automatic reset event
        FALSE,        // initial state is non-signaled
        NULL);        // object name
}

BootStrap::~BootStrap()
{
    log( "BootStrap dtor" );

    if ( !m_dialogThreadEnd || !m_cancel )
        closeWindow();

    if ( m_progressDialogShown != NULL )
        CloseHandle( m_progressDialogShown );
    
    if ( m_pauseStateChanged != NULL  )
        CloseHandle( m_pauseStateChanged );
}

// FIXME: good principle is to give functions names that reflect what they do. You
// wouldn't expect a function called queryX() to modify the state of the object...
bool BootStrap::queryBootStrapStatus()
{
    if( m_bootStrapHkey == NULL )
        return false;

    WCHAR result[255];
    try
    {
        RegistryUtils::QueryString( m_bootStrapHkey, m_mediaPlayerName, result, 255, false );
    } 
    catch ( RegistryUtils::CRegistryException e )
    {
        //No such value
        return false;
    }
    
    m_userName = result;

    return result != L"";
}

bool BootStrap::bootStrapRequired()
{
    return m_bootStrapRequired;
}

/************************************************************************/
/* Process the window callback messages                                 */
/************************************************************************/
LRESULT CALLBACK BootStrap::DialogProc(HWND hwndDlg, 
                                       UINT message, 
                                       WPARAM wParam, 
                                       LPARAM lParam) 
{
    switch( message )
    {
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_BUTTONCANCEL:
            {
                BootStrap* bootstrap = m_hwndMap[ hwndDlg ];

                bootstrap->log( "Cancel clicked" );

                bootstrap->cancel();
                break;
            }

        case IDC_BUTTONOK:
            {
                BootStrap* bootstrap = m_hwndMap[ hwndDlg ];

                bootstrap->log( "OK clicked" );
                
                bootstrap->closeWindow();
                break;
            }
        }
        break;

    default:
        return FALSE;
    }
    return FALSE;
}

/************************************************************************/
/* Close the progress window and end the thread                         */
/************************************************************************/
void BootStrap::closeWindow()
{
    log( "BootStrap::closeWindow" );

    int resultCode;
    BOOL result = EndDialog( m_hwndDialog, (INT_PTR)&resultCode );
    if( resultCode == 0 )
    {
        // FIXME: what's this code for, I wonder :)
        // And what's the result BOOL?
        DWORD error = GetLastError();
        
        std::ostringstream os;
        os << "EndDialog failed, error: " << error;
        log( os.str() );
    }

    m_dialogThreadEnd = true;
}

/************************************************************************/
/* Setup thread to display the progress window                          */
/************************************************************************/
void BootStrap::showProgress()
{
    log( "Starting progress dialog thread" );

    unsigned int threadId;
    m_hDialogThread = reinterpret_cast<HANDLE>(_beginthreadex( NULL,
                                                0,
                                                BootStrap::dialogThreadEntry,
                                                this,
                                                0,
                                                &threadId ) );

    // FIXME: Ideally should do some error checking here                                                

    std::ostringstream os;
    os << "Progress dialog thread started, handle: " << m_hDialogThread;
    log( os.str() );
}


bool BootStrap::startBootStrap()
{
    log( "Starting bootstrap thread" );

    unsigned int threadId;
    m_bootstrapThread = reinterpret_cast<HANDLE>(_beginthreadex( NULL,
                                                                 0,
                                                                 BootStrap::bootstrapThreadEntry,
                                                                 this,
                                                                 0,
                                                                 &threadId));

    std::ostringstream os;
    os << "Bootstrap thread started, handle: " << m_bootstrapThread;
    log( os.str() );

    // FIXME: Ideally should do some error checking here                                                
    return true;
}

/************************************************************************/
/* static entry point for bootstrap thread                              */
/************************************************************************/
unsigned __stdcall BootStrap::bootstrapThreadEntry( LPVOID lpParam )
{
    BootStrap* bootstrapper = static_cast<BootStrap*>(lpParam);
    bootstrapper->showProgress();
    
    bootstrapper->log( "Bootstrapper called showProgress, will now wait for event" );

    WaitForSingleObject( bootstrapper->m_progressDialogShown, INFINITE );
    
    bootstrapper->log( "Event signalled, calling start" );

    bootstrapper->start();

    bootstrapper->log( "start returned, bootstrapper finished" );

    return 0;
}


/************************************************************************/
/* static entry point for window thread                                 */
/************************************************************************/
unsigned __stdcall BootStrap::dialogThreadEntry( LPVOID lpParam )
{
    BootStrap* bootstrapper = static_cast<BootStrap*>(lpParam);
    bootstrapper->dialogEventLoop();
    return 0;
}

/************************************************************************/
/* Create window and process messages                                   */
/************************************************************************/
void BootStrap::dialogEventLoop()
{
    log( "Starting dialogEventLoop" );

    m_hwndDialog = CreateDialogW( m_module,
                                  MAKEINTRESOURCEW( IDD_PROGRESSFORM ),
                                  m_hwndParent,
                                  (DLGPROC)BootStrap::DialogProc );

    m_hwndMap[ m_hwndDialog ] = this;

    // FIXME: error never used
    DWORD error = GetLastError();

    m_hwndProgressBar = GetDlgItem( m_hwndDialog, IDC_PROGRESS1 );

    HWND hwndLabel = GetDlgItem( m_hwndDialog, IDC_LABEL );

    // EJ: Just out of interest, why do you read the string into a member at construct time and then
    // access the member here, instead of just calling a function that reads it straight here?
    //set the label from the registry key
    SetWindowTextW( hwndLabel, (LPCWSTR)m_strProgressLabel.c_str() );

    //set the window title from the registry key
    SetWindowTextW( m_hwndDialog, (LPCWSTR)m_strProgressTitle.c_str() );

    //Set Dialog Position to Parent position
    if( m_hwndParent )
    {
        RECT parentPos;
        GetWindowRect( m_hwndParent, &parentPos );

        RECT dialogPos;
        GetWindowRect( m_hwndDialog, &dialogPos );

        SetWindowPos( m_hwndDialog, m_hwndParent,
            ( ( parentPos.left + parentPos.right ) / 2 ) - ( ( dialogPos.right - dialogPos.left ) / 2 ),
            ( ( parentPos.top + parentPos.bottom ) / 2 ) - ( ( dialogPos.top - dialogPos.bottom ) / 2 ),
            NULL, NULL, SWP_NOSIZE || SWP_ASYNCWINDOWPOS );
    }

    setProgressRange( m_progressRange );
    setProgressValue( m_progressValue );

    ShowWindow( m_hwndDialog, SW_SHOWNORMAL );

    log( "Dialog shown, setting event to signalled" );

    SetEvent( m_progressDialogShown );
    
    MSG message;
    BOOL messageVal;
    m_dialogThreadEnd = false;
    while( messageVal = GetMessage( &message, NULL, 0, 0 ) && !m_dialogThreadEnd && !m_cancel )
    {
        if( messageVal < 0 )
        {
            // FIXME: what about localisation here? And why are we using T-strings?
            std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > errorTxt;
            errorTxt << _T( "Event loop error " ) << GetLastError();
            MessageBox( NULL, errorTxt.str().c_str(), _T( "Error..." ), MB_OK | MB_ICONEXCLAMATION );
        }
        else
        {
            TranslateMessage( &message );
            DispatchMessage( &message );
        }
    }

    std::ostringstream os;
    os << "Dialog loop exited\nmessageVal: " << messageVal << ", m_dialogThreadEnd: " << m_dialogThreadEnd << ", m_cancel: " << m_cancel;
    log( os.str() );
    
    if( m_cancel )
    {
        closeWindow();
    }
}

void BootStrap::setProgressRange( int maxValue )
{
    if( maxValue == 0 )
    {
        // no tracks to bootstrap so just set progress bar to 100%
        setProgressRange( 1 );
        setProgressValue( 1 );
        return;
    }

    if( m_hwndProgressBar )
        SendMessage( m_hwndProgressBar, PBM_SETRANGE, 0, MAKELPARAM( 0, maxValue ) );

    m_progressRange = maxValue;
}

void BootStrap::setProgressValue( int value )
{
    if( m_hwndProgressBar )
        SendMessage( m_hwndProgressBar, PBM_SETPOS, value, 0 );

    m_progressValue = value;
}

void BootStrap::setScrobSub( ScrobSubmitter* scrobSub )
{
    std::ostringstream os;
    os << "setScrobSub: " << scrobSub;
    log( os.str() );

    m_scrobSub = scrobSub;
}

void BootStrap::setParentHwnd( HWND parentHwnd )
{
    std::ostringstream os;
    os << "setParentHwnd: " << parentHwnd;
    log( os.str() );

    m_hwndParent = parentHwnd;
}


// FIXME: what's this?
//  1. A function called by someone else that does something?
//    Call it complete() or some other verb
//  2. An event handler called when something else completes?
//    Call it onThingComplete()
void BootStrap::completed()
{
    WaitForSingleObject( m_progressDialogShown, INFINITE );
    
    HWND hwndOkButton = GetDlgItem( m_hwndDialog, IDC_BUTTONOK );
    HWND hwndCancelButton = GetDlgItem( m_hwndDialog, IDC_BUTTONCANCEL );
    EnableWindow( hwndOkButton, true );
    EnableWindow( hwndCancelButton, false );

    RegistryUtils::DeleteValue( m_bootStrapHkey, m_mediaPlayerName );
    
    if( !m_tracksFound )
    {
        SetWindowTextW( GetDlgItem( m_hwndDialog, IDC_LABEL ), m_strNoTracksFoundLabel.c_str() );
        RegistryUtils::CloseRegKey( m_bootStrapHkey );
        return;
    }
    
    
    xmlBuffer << L"</bootstrap>\n";
    
    int maxUtf8Size = xmlBuffer.str().size() * 2;
    char* utf8Data = new char[maxUtf8Size];

    int utf8Size = EncodingUtils::UnicodeToUtf8( xmlBuffer.str().c_str(), xmlBuffer.str().length(), utf8Data, maxUtf8Size );

    FILE* file = _wfopen( getOutputFilename().c_str(), L"wb" );
    fwrite( utf8Data, sizeof(char), utf8Size, file );
    fclose( file );

    if( m_scrobSub )
        m_scrobSub->BootstrapComplete( m_userName.c_str() );


    SetWindowTextW( GetDlgItem( m_hwndDialog, IDC_LABEL ), m_strCompleteLabel.c_str() );
    
    RegistryUtils::CloseRegKey( m_bootStrapHkey );

    m_bootStrapHkey = NULL;

    SetFocus( hwndOkButton );

    log( "bootstrap completed, will wait for dialogthread event" );

    WaitForSingleObject( m_hDialogThread, INFINITE );

    log( "dialogthread event wait over" );
}

void BootStrap::cancel()
{
    pause();
    int response = MessageBoxW( m_hwndDialog, 
        m_strCancelConfirmation.c_str(), 
        m_strProgressTitle.c_str(), 
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 );

    if ( response == IDYES )
    {
        RegistryUtils::DeleteValue( m_bootStrapHkey, m_mediaPlayerName );
        RegistryUtils::CloseRegKey( m_bootStrapHkey );
               
        m_cancel = true;
        pause( false );
    }
    else
    {
        // FIXME: a small detail, but ideally, jave either pause()/resume(), or setPaused( bool )
        pause( false );
    }
}

void BootStrap::pause( bool pauseState )
{
    m_paused = pauseState;
    SetEvent( m_pauseStateChanged );
}


std::wstring BootStrap::getOutputFilename() const
{
    WCHAR clientPathBuffer[MAX_PATH];
    RegistryUtils::QueryString( m_bootStrapHkey, L"data_path", clientPathBuffer, MAX_PATH, false );
   
    // FIXME: no error handling. If the path is wrong/missing, it will go ahead anyway.

    std::wstring dirPath;
    dirPath = clientPathBuffer;

    std::wstringstream filePath;
    filePath << dirPath << "/";
    filePath << m_userName << "_";
    filePath << m_mediaPlayerName;
    filePath << "_bootstrap.xml";

    return filePath.str();
}

void BootStrap::find_and_replace( std::wstring &input, std::map<WCHAR, std::wstring> &replacements) const
{
    std::wostringstream oss;
    std::map<WCHAR, std::wstring>::iterator specialCharIt;

    for( std::wstring::iterator stringIter = input.begin(); stringIter != input.end(); stringIter++ )
    {
        if( *stringIter < 0x20 )
        {
            oss << ' ';
            continue;
        }

        specialCharIt = replacements.find( *stringIter );
        if ( specialCharIt == replacements.end() )
            oss << *stringIter;
        else
            oss << specialCharIt->second;
        
    }

    input = oss.str();
}

// FIXME: name makes it look like a Qt-style getter
void BootStrap::xmlentities( std::wstring &input )
{
    find_and_replace( input, m_replacementMap );
}

void BootStrap::appendTrack( struct BootStrapItem* item )
{
    // Ignore tracks with 0 play count
    if( _wtoi( item->playcount.c_str() ) <= 0 )
        return;

    //Ignore tracks with no artist / track names
    if( item->artist.length() < 1 ||
        item->track.length() < 1 )
        return;       

    m_tracksFound = true;



    xmlentities( item->artist );
    xmlentities( item->album );
    xmlentities( item->track );
    xmlentities( item->duration );
    xmlentities( item->playcount );
    xmlentities( item->filename );

    xmlBuffer << " <item>\n";

    xmlBuffer << "  <artist>"    << item->artist    << "</artist>\n";
    xmlBuffer << "  <album>"     << item->album     << "</album>\n";
    xmlBuffer << "  <track>"     << item->track     << "</track>\n";
    xmlBuffer << "  <duration>"  << item->duration  << "</duration>\n";
    xmlBuffer << "  <playcount>" << item->playcount << "</playcount>\n";
    xmlBuffer << "  <filename>"  << item->filename  << "</filename>\n";
    xmlBuffer << "  <timestamp>" << item->timestamp << "</timestamp>\n";

    xmlBuffer << " </item>\n";
}

void
BootStrap::log( std::string message )
{
    if ( m_scrobSub != NULL )
    {
        m_scrobSub->ReportStatus( -1, false, message );
    }
}
