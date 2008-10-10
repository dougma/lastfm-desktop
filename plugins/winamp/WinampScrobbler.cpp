// WinampScrobbler.cpp: implementation of the CWinampScrobbler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinampScrobbler.h"
#include "WinampBootstrap.h"
#include "wa_ipc.h"

#define WA_STATE_CHANGE 0x0000029A
#define IPC_FF_FIRST 2000
#define IPC_FF_ISMAINWND IPC_FF_FIRST + 4


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWinampScrobbler::CWinampScrobbler()
{
	m_hWndWinamp					= NULL;
	m_dwLastPlay					= 0;
}

CWinampScrobbler::~CWinampScrobbler()
{
}

BOOL	CWinampScrobbler::StartScrobbling(HINSTANCE hInstance, 
										  HWND hWndWinamp, 
										  HWND hWndWinampML, 
										  CStdString strWorkingDir)
{
	if ( ( hWndWinamp != NULL) &&
		   ( ::IsWindow(hWndWinamp)) )
	{
		m_hWndWinamp		= hWndWinamp;
		m_hWndWinampML		= hWndWinampML;
		
		m_Controller.Init(FALSE, hWndWinamp, hWndWinampML);
		
		BOOL bRet =  CScrobbler::StartScrobbling(strWorkingDir, hInstance);

        EnumThreadWindows( GetCurrentThreadId(), CWinampScrobbler::enumWndProc, (LPARAM)this );

        if( m_bootstrap.bootStrapRequired() )
        {
            PRINTF(DEBUG_FLOW,
                "CWinampScrobbler::StartScrobbling", "Bootstrap is required" );

            m_bootstrap.setParentHwnd( m_mainWinAmpWnd );
            m_bootstrap.sethWndWinampML( m_hWndWinampML );
            m_bootstrap.setScrobSub( m_Submitter );
            m_bootstrap.setModuleHandle( hInstance );
            m_bootstrap.startBootStrap();
        }
        

		return bRet;
	}

	return FALSE;
} 

/*##############################################
##
## Module				: CWinampScrobbler
## Function			: WinampPEMessage
## Description	: Winamp Playlist Editor Window
##                hook to catch playback events
##
## Author(s)		: Spib
##
## Parameters		: N/A
## Return				: N/A
##
##############################################*/
BOOL CWinampScrobbler::WinampPEMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    // This function is only called in Winamp versions < 5
	if(message == WM_USER)
	{
		if(wParam == WA_STATE_CHANGE)
		{
			int nType = HIWORD(lParam);
			int nTrack = LOWORD(lParam);
			int nState = m_Controller.GetWinampState();

            // Ignore the double issue play message
			// This is just some undocumented number found by experimenting.
			if(nType != 16384)
				return TRUE;

			if(nState == WA_STATE_PLAY)
			{
				DWORD dwNow = GetTickCount();
				
				// Winamp Sends two quick song change notifications so 
				// here we check if the last notification arrived less than
				// 400 milliseconds after the last in which case we ignore it.
				if((dwNow - m_dwLastPlay) < 400)
				{
					// Ignore second notifcation
					PRINTF(DEBUG_DETAIL, "CWinampScrobbler::WinampPEMessage", "Duplicate notification arrived - Now %d, Then %d", dwNow, m_dwLastPlay);
				}
				else
				{
					m_dwLastPlay = dwNow;

					// Signal a track play without blocking
					m_bTrackPlayFlag = TRUE;
				}
			}
		}
	}
	
	return TRUE;
}

BOOL CWinampScrobbler::WinampMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    if ( message == WM_WA_IPC )
    {
        // We don't want to look for IPC_PLAYING_FILEW as it's only emitted for
        // the Winamp versions after Unicode support was introduced (5.x sthg)
        if ( lParam == IPC_PLAYING_FILE )
        {
            // Signal a track play without blocking
            m_bTrackPlayFlag = TRUE;
            m_dwLastPlay = GetTickCount();
        }
    }
    return TRUE;
}

CStdString CWinampScrobbler::GetPlayerVersion()
{
	CStdString strTemp;
	strTemp.Format("Winamp Version %s", m_Controller.GetPlayerVersion().c_str());
	return strTemp;
}


BOOL CWinampScrobbler::GetCurrentSong(SONG_INFO* pSong, bool utf8)
{
	if(pSong != NULL)
    {
        // There is a bug where Winamp sometimes mixes up the wrong artist name with the correct
        // track title. In an attempt to prevent this, we sanity check the data returned from
        // GetSongMetadata against the data returned by GetSongTitle (inside SanityCheckMetadata),
        // and if they differ, we retry the call up to 5 times.
        bool winampIsSane = false;
        int cnt = 0;
        while ( !winampIsSane && cnt++ < 5 )
        {
            if ( utf8 )
            {
                PRINTF(DEBUG_INFO, "CWinampScrobbler::GetCurrentSong", "Trying to get UTF8 data...");
                GetSongMetaDataUtf8(pSong);
            }
            else
            {
                PRINTF(DEBUG_INFO, "CWinampScrobbler::GetCurrentSong", "Getting ANSI data...");                    
                GetSongMetaData(pSong);
            }
    
            bool bInfoOk = strlen(pSong->m_strArtist) > 0 && strlen(pSong->m_strTrack) > 0;

            // Early out if we didn't get any metadata
            if ( !bInfoOk )
            {
                PRINTF(DEBUG_INFO, "CWinampScrobbler::GetCurrentSong", "Failed getting metadata...");
                return FALSE;
            }
        
            winampIsSane = SanityCheckMetadata(pSong, utf8);
        }

        // Workaround for when the -1 or -1000 bug hits
        if (SupportedFileType(pSong->m_strFileName)) // ugly, done twice
        {
            int count = 0;
            while (pSong->m_nLength < 0 && count < 10) 
            {
                PRINTF(DEBUG_ERROR_NC, "CWinampScrobbler::GetCurrentSong",
                    "Length of %d returned, retry attempt %d.", pSong->m_nLength, count );

                ::Sleep(100);            
                pSong->m_nLength = m_Controller.GetCurrentSongLength();
                
                count++;
            }
            
            if (pSong->m_nLength < 0)
            {
                PRINTF(DEBUG_ERROR_NC, "CWinampScrobbler::GetCurrentSong",
                    "Still -1, fudging to 31s.");
                pSong->m_nLength = 31;
            }
        }

    }
    else
        PRINTF(DEBUG_DETAIL, "CWinampScrobbler::GetSongInfo", "Invalid Song Pointer");

    return TRUE;
}


void CWinampScrobbler::GetSongMetaDataUtf8(SONG_INFO* pSong)
{
    CStdString artist = m_Controller.GetCurrentSongMetadataUtf8("artist");
	CStdString title = m_Controller.GetCurrentSongMetadataUtf8("title");
	CStdString album = m_Controller.GetCurrentSongMetadataUtf8("album");
	CStdString fileName = m_Controller.GetCurrentSongFileNameUtf8();

    strncpy(pSong->m_strArtist, artist, sizeof(pSong->m_strArtist));
    strncpy(pSong->m_strTrack, title, sizeof(pSong->m_strTrack));
	strncpy(pSong->m_strAlbum, album, sizeof(pSong->m_strAlbum));
	strncpy(pSong->m_strFileName, fileName, sizeof(pSong->m_strFileName));

    pSong->m_nLength = m_Controller.GetCurrentSongLength();
}

void CWinampScrobbler::GetSongMetaData(SONG_INFO* pSong)
{
	SongInfo info;
	if(m_Controller.GetCurrentSongInfo(info))
	{
        PRINTF(DEBUG_INFO, "CWinampScrobbler::GetSongMetaData", "Got metadata from ML");

		strncpy(pSong->m_strArtist, 
            info.strArtist,
            sizeof(pSong->m_strArtist));
		strncpy(pSong->m_strTrack, 
            info.strTitle,
            sizeof(pSong->m_strTrack));
		strncpy(pSong->m_strAlbum, 
            info.strAlbum,
            sizeof(pSong->m_strAlbum));
		strncpy(pSong->m_strGenre, 
            info.strGenre,
            sizeof(pSong->m_strGenre));
		strncpy(pSong->m_strComment, 
            info.strComment,
            sizeof(pSong->m_strComment));
		strncpy(pSong->m_strFileName, 
            info.strFileName,
            sizeof(pSong->m_strFileName));

		pSong->m_nLength	= info.nLength;
		pSong->m_nTrackNo	= info.nTrackNo;
		pSong->m_nYear		= info.nYear;
		
	}
	else
	{
		// Winamp 5 or Winamp 2.90 onwards.
		CStdString artist = m_Controller.GetCurrentSongMetadata("artist");
		strncpy(pSong->m_strArtist, artist, sizeof(pSong->m_strArtist));

		CStdString title = m_Controller.GetCurrentSongMetadata("title");
		strncpy(pSong->m_strTrack, title, sizeof(pSong->m_strTrack));

		CStdString album = m_Controller.GetCurrentSongMetadata("album");
		strncpy(pSong->m_strAlbum, album, sizeof(pSong->m_strAlbum));

		CStdString fileName = m_Controller.GetCurrentSongFileName();
		strncpy(pSong->m_strFileName, fileName, sizeof(pSong->m_strFileName));

		pSong->m_nLength = m_Controller.GetCurrentSongLength();

        PRINTF(DEBUG_INFO, "CWinampScrobbler::GetSongMetaData", "Got metadata not using ML");
	}
	
    bool bInfoOk = strlen(pSong->m_strArtist) > 0 && strlen(pSong->m_strTrack) > 0;
        
    if(!bInfoOk && m_bUseNowPlaying)
    {
        PRINTF(DEBUG_INFO, "CWinampScrobbler::GetSongMetaData", "Metadata incomplete, using title bar");

		// Winamp 2.x
        CStdString strTemp = m_Controller.GetCurrentSongTitle();
		int nPos = strTemp.Find(" - ");
		
		if (nPos != -1) 
		{
			strncpy(pSong->m_strArtist, strTemp.Left(nPos), SONG_INFO_FIELD_SIZE);

			if (strlen(pSong->m_strArtist) > 0) 
				nPos = 3; 
			else 
				nPos = 0;

			strncpy(pSong->m_strTrack, 
            strTemp.Right(strTemp.GetLength() - nPos - strlen(pSong->m_strArtist)),
                SONG_INFO_FIELD_SIZE);
        }
	}
}

bool CWinampScrobbler::SanityCheckMetadata(SONG_INFO* pSong, bool utf8)
{
    // Compare the retrieved metadata to the playlist title. If the artist name
    // isn't found in the playlist title, it's likely that we got the wrong
    // artist name in the metadata.

    CStdString title;
    if ( utf8 )
    {
        title = m_Controller.GetSongTitleUtf8( m_Controller.GetPlayListPos() );
    }
    else
    {
        title = m_Controller.GetSongTitle( m_Controller.GetPlayListPos() );
    }
    
    // If we failed getting anything we can't really sanity check
    if ( title.IsEmpty() )
    {
        PRINTF(DEBUG_ERROR_NC, "CWinampScrobbler::SanityCheckMetadata", "Couldn't get song title.");
        return true;
    }

    const char* cTitle = title.c_str();
    if ( strstr( cTitle, pSong->m_strArtist ) == 0 )
    {
        PRINTF(DEBUG_INFO, "CWinampScrobbler::SanityCheckMetadata", "Metadata workaround triggered. Metadata artist: %s. Song title: %s", pSong->m_strArtist, title.c_str());

        // not found
        return false;
    }

    return true;
}



BOOL CALLBACK CWinampScrobbler::enumWndProc(HWND hwnd, LPARAM lParam)
{
    CWinampScrobbler* scrobbler = (CWinampScrobbler*)( lParam );
    if(SendMessage(scrobbler->m_hWndWinamp,WM_WA_IPC,(WPARAM)hwnd,IPC_FF_ISMAINWND)){
        scrobbler->m_mainWinAmpWnd = hwnd;
        return 0;
    }
    return 1;
}