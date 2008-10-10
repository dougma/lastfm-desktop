// WinampScrobbler.h: interface for the CWinampScrobbler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINAMPSCROBBLER_H__7BFE93FD_744B_4D31_94DA_1A44EAA4C581__INCLUDED_)
#define AFX_WINAMPSCROBBLER_H__7BFE93FD_744B_4D31_94DA_1A44EAA4C581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define AUDIOSCROBBLER_APPNAME					"Audioscrobbler Winamp plugin"
#define AUDIOSCROBBLER_APPID					"wa2"
#define AUDIOSCROBBLER_REGKEY					"Software\\Last.fm\\Audioscrobbler Winamp Plugin"

#include "Scrobbler.h"
#include "WinampController.h"
#include "WinampBootstrap.h"

class CWinampScrobbler : public CScrobbler  
{
public:
	CWinampScrobbler();
	virtual ~CWinampScrobbler();

	BOOL		StartScrobbling(HINSTANCE hInstance, 
								HWND hWndWinamp,
								HWND hWndWinampML,
								CStdString strWorkingDir);

	// Overrides
	CStdString	GetAppName(){ return AUDIOSCROBBLER_APPNAME;}
	CStdString	GetAppId(){ return AUDIOSCROBBLER_APPID;}
	CStdString	GetRegKey(){ return AUDIOSCROBBLER_REGKEY;}
	CStdString	GetPlayerVersion();
	BOOL		IsPlaying(){ return m_Controller.IsPlaying();}
	BOOL		IsPaused(){ return m_Controller.IsPaused();}
	BOOL		IsStopped(){ return m_Controller.IsStopped();}
	CStdString	GetCurrentSongFileName(){ return m_Controller.GetCurrentSongFileName();}
	int			GetTrackLength(){ return m_Controller.GetTrackLength();}
	int			GetTrackPosition(){ return (m_Controller.GetPlayingTime() / 1000);}
	BOOL	    GetCurrentSong(SONG_INFO* pSong, bool utf8 = true);
	HWND		GetParentWnd(){ return m_hWndWinamp;}

	BOOL		WinampPEMessage(UINT message, WPARAM wParam, LPARAM lParam);
    BOOL        WinampMessage(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	CWinampController	m_Controller;	// Winamp Controller
	HWND				m_hWndWinamp;	// Winamp Main Window HWND
	HWND				m_hWndWinampML;	// Winamp Media Library Window HWND
	DWORD				m_dwLastPlay;	// TickCount

    void GetSongMetaDataUtf8(SONG_INFO* pSong);
    void GetSongMetaData(SONG_INFO* pSong);
    bool SanityCheckMetadata(SONG_INFO* pSong, bool utf8);

private:
    WinampBootStrap m_bootstrap;
    static BOOL CALLBACK enumWndProc(HWND hwnd, LPARAM lParam);
    
    HWND m_mainWinAmpWnd;
};

#endif // !defined(AFX_WINAMPSCROBBLER_H__7BFE93FD_744B_4D31_94DA_1A44EAA4C581__INCLUDED_)
