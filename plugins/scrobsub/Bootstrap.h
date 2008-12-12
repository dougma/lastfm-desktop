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

