/* Copyright 2005-2009, Last.fm Ltd. <client@last.fm>                       
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

#define _WIN32_WINNT 0x0500

#include "windows.h"
#include "Sddl.h"       // for ConvertSidToString (_WIN32_WINNT = 0x0500)
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;


string
formatWin32Error(DWORD error)
{
    LPSTR buffer;
    if (FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        0,      // ignored
        error,
        0,      // default langid behaviour
        (LPSTR) &buffer,   // yay win32!
        0,      // min chars alloc'd in buffer
        0       // no arguments!
        ))
    {
        // got message
        string result;
        result.assign(buffer);
        LocalFree(buffer);
        return result;
    }

    ostringstream os;
    os << "0x" << setfill('0') << setw(8) << setbase(16) << error;
    return os.str();
}

// create a name (for the scrobsub pipe) based on the SID of the user running this process
// return win32 error code (0 == success)
DWORD
scrobSubPipeName(string* pipeName)
{
    #define PIPE_PREFIX "\\\\.\\pipe\\lastfm_scrobsub_"

    bool ok = false;

    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        DWORD outLen;
        #define BUFFER_SIZE 256
        char buffer[BUFFER_SIZE] = { 0 } ;  // init just to suppress warning 
        if (GetTokenInformation(hToken, TokenOwner, buffer, BUFFER_SIZE, &outLen))
        {
            LPSTR sidstring;
            TOKEN_OWNER* tokenOwner = (TOKEN_OWNER*) buffer;
            if (ConvertSidToStringSidA(tokenOwner->Owner, &sidstring))
            {
                ostringstream os;
                os << PIPE_PREFIX << sidstring;
                *pipeName = os.str();
                LocalFree(sidstring);
                ok = true;
            }
        }
        CloseHandle(hToken);
    }

    return ok ? 0 : GetLastError();
}

