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

#ifndef _BOOTSTRAP_H_
#define _BOOTSTRAP_H_

#include "RegistryUtils.h"

#define _WINSOCKAPI_
#include <windows.h>

#include <sstream>
#include <fstream>
#include <map>

// FIXME: As this will be part of a public library, we should add Doxygen comments and
// format it like ScrobSubmitter. Also, the name Bootstrap is a little ambiguous, what
// does it do? Is it a bootstrapper, is it a data repository for bootstrapping data?
//
// It's a slightly incoherent class in that it does a whole load of different stuff
// so it's difficult to give it a good name. It might be an idea to extract out the
// dialog into its own class and give each class distinct responsibilities. It's quite
// difficult also to know what thread you're in. Everything that runs in a separate
// thread might also be a candidate for a separate class to make it obvious when
// we're in a different thread. Also don't see any thread synchronisation primitives
// in here, but I haven't looked carefully enough to see if it's a problem in practice...
class BootStrap
{
public:
    virtual ~BootStrap();

    bool bootStrapRequired();
    bool startBootStrap();
    void cancel();
    void pause( bool pauseState = true );
    void setScrobSub( class ScrobSubmitter* scrobSub );
    void setParentHwnd( HWND parentHwnd );
    void setModuleHandle( HINSTANCE instance ) { m_module = instance; }

protected:

    BootStrap( const WCHAR* mediaPlayerName );

    void closeWindow();
    void completed();
    void setProgressRange( int maxValue );
    void setProgressValue( int value );

    void find_and_replace( std::wstring &input, std::map<WCHAR, std::wstring> &replacements ) const;
    void xmlentities( std::wstring &input );
    
    struct BootStrapItem{
        std::wstring artist;
        std::wstring track;
        std::wstring album;
        std::wstring duration;
        std::wstring playcount;
        std::wstring filename;
        std::wstring timestamp;
    };

    void appendTrack( BootStrapItem* item );

    const WCHAR* m_mediaPlayerName;
    bool queryBootStrapStatus();

    void log( std::string message );

    bool m_dialogThreadEnd;

    int m_progressRange;
    int m_progressValue;


    bool m_dialogReady;
    bool m_cancel;
    bool m_paused;

    int m_clientVerMajor;
    int m_clientVerMinor;
    int m_clientVerRelease;
    int m_clientVerFix;

    std::wstring getOutputFilename() const;
    HANDLE m_pauseStateChanged;

private:
    BootStrap( BootStrap& ){};

    bool m_bootStrapRequired;
    HKEY m_bootStrapHkey;

    std::wstring m_strProgressLabel;
    std::wstring m_strCompleteLabel;
    std::wstring m_strNoTracksFoundLabel;
    std::wstring m_strProgressTitle;
    std::wstring m_strCancelConfirmation;
    std::wstring m_userName;

    virtual bool start() { log( "base class start called, this should be impossible" ); return true; }

    void showProgress();
    void dialogEventLoop();

    static  unsigned __stdcall bootstrapThreadEntry( LPVOID lpParam );
    static  unsigned __stdcall dialogThreadEntry( LPVOID lpParam );
    
    static LRESULT CALLBACK DialogProc(HWND hwndDlg, 
                                       UINT message, 
                                       WPARAM wParam, 
                                       LPARAM lParam);

    static std::map< HWND, BootStrap* > m_hwndMap;
    std::map< WCHAR, std::wstring > m_replacementMap;

    ScrobSubmitter* m_scrobSub;

    std::wstringstream xmlBuffer;

    HWND m_hwndDialog;
    HWND m_hwndProgressBar;
    HWND m_hwndParent;

    HANDLE  m_bootstrapThread;
    HANDLE  m_hDialogThread;

    HINSTANCE m_module;
    bool m_tracksFound;

    HANDLE m_progressDialogShown;
};


#endif // _BOOTSTRAP_H_

