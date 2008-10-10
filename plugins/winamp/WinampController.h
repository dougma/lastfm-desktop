/*##############################################
##
## Module				: CWinampController
## Description	: This class takes total control
##								of Winamp 1.x - 2.x
##
## Author(s)		: Spib
##
## Modifications
##
##############################################*/
#pragma once

#include "Gen.h"
#include "wa_ipc.h"
#include "WinampCmd.h"
#include "ml.h"

#define WINAMP_MENU_OPTIONS_POS		11
#define WINAMP_MENU_REPEAT			40022
#define WINAMP_MENU_SHUFFLE			40023
#define WINAMP_MAIN_WINDOW          40258

#define WINAMP_MV_PE_HLT_UP			105825
#define WINAMP_MV_PE_HLT_DOWN		105826

#define WINAMP_DEL_FRM_PE			66570
#define WINAMP_MV_PE_ITEM_DOWN		105752
#define WINAMP_MV_PE_ITEM_UP		105751

#define WINAMP_PE_SORT_TITLE		40209
#define WINAMP_PE_SORT_FNAME		40210
#define WINAMP_PE_SORT_FNAME_PATH	40211

#define WA_STATE_PLAY	100
#define WA_STATE_PAUSE	101
#define WA_STATE_STOP	102

//PE Stuff
#define IPC_PE_GETCURINDEX			100 
#define IPC_PE_GETINDEXTOTAL		101 
#define IPC_PE_GETINDEXINFO			102 
#define IPC_PE_GETINDEXINFORESULT	103 
#define IPC_PE_DELETEINDEX			104 
#define IPC_PE_SWAPINDEX			105 
#define IPC_PE_INSERTFILENAME		106 
#define IPC_PE_GETDIRTY				107 
#define IPC_PE_SETCLEAN				108 

typedef struct
{
	CStdString	strArtist;
	CStdString	strAlbum;
	CStdString	strComment;
	CStdString	strGenre;
	CStdString	strTitle;
	CStdString	strFileName;
	int			nYear;
	int			nLength;
	int			nTrackNo;
}SongInfo;

typedef struct
{
	int nWinampRuns;			// How many times Winamp has been run.
	CComDATE tRunning;			// How long Winamp has been running. 
	CComDATE tRunningMB;			// How long Winamp has been running with the Minibrowser window open. 
	CComDATE tRunningNoneMin;		// How long Winamp has been running in a non minimized state. 
	CComDATE tRunningPlay;			// How long Winamp has been running and playing audio files. 
	CComDATE tRunningPlayNonMin;	// How long Winamp has been running, playing, in a non minimized state.
}WA_STATS;

class CWinampController  
{
public:
	CWinampController();
	virtual ~CWinampController();

	BOOL	Init(BOOL bStartWinamp, HWND Winamp = NULL, HWND WinampML = NULL);

	HWND	GetWinampWnd(){return m_hWndWinamp;}
	HWND    GetPEWnd(){return m_hWndWinampPE;}
	BOOL	GetWinampStats(WA_STATS& stats);

	//Main Window - Operation
	BOOL	Play(CStdString strFile = "", BOOL bKeepList = FALSE);
	BOOL	Enqueue(CStdString strFile);
	void	Stop();
	void	Pause();
	void	FastForward();
	void	NextTrack();
	void	PrevTrack();
	void	Rewind();
	void	FastFwd5Secs();
	void	Rewind5Secs();
	void	CloseWinamp();
	void	RestartWinamp();
	void	IncreaseVolume();
	void	DecreaseVolume();

	BOOL	GetShuffleStatus();
	void	SetShuffleStatus(BOOL newVal);
	BOOL	GetRepeatStatus();
	void	SetRepeatStatus(BOOL newVal);
	HMENU	GetOptionMenu();
	long	GetVersion();
	long	GetStatus();
	void	SetStatus(long newVal);
	long	GetCurrentPos();
	void	SetCurrentPos(long newVal);
	void	SetVolume(long newVal);
	void	SetPanning(long PanIndex);
	void	ChangeDirectory(CStdString NewDir);
	CStdString GetSkinName();
	void	SetSkinName(CStdString Name);
	void	StartPlugIn(CStdString PlugInName);
	void	ToggleShufflePlay();
	void	ToggleRepeatPlay();
	void	SongChange();
	long	GetCurrentSongLength();
	BOOL	GetWinampVisible();
	void	SetWinampVisible(BOOL newVal);

	//Song Information
	long	GetSampleRate();
	long	GetBitrate();
	long	GetChannels();
	CStdString GetSongFileName(long PlayListPos);
    CStdString GetSongFileNameUtf8(long PlayListPos);
	CStdString GetSongTitle(long PlayListPos);
    CStdString GetSongTitleUtf8(long PlayListPos);
    CStdString GetSongMetaData(CStdString MetaData, long PlayListPos);
	CStdString GetCurrentSongFileName();
    CStdString GetCurrentSongFileNameUtf8();
    CStdString GetCurrentSongTitle();
	CStdString GetCurrentSongMetadata(char* MetaData);
	CStdString GetCurrentSongMetadataUtf8(char* MetaData);
	BOOL		GetCurrentSongInfo(SongInfo& Info);

	//Playlist Functions
	void	ClearPlaylist();
	long	GetPlayListPos();
	void	SetPlayListPos(long newVal);
	long	GetPlayListCount();
	void	DeletePEItem(int Pos);
	void	DeletePECurHighlight();
	void	MovePEHighlight(int Pos);
	bool	MovePEItem(int curPos, int NewPos);
	BOOL	GetPlaylistVisible();
	void	SetPlaylistVisible(BOOL newVal);
	void	JumpToTrack(unsigned int Number);
	void	SortByName(){SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_PE_SORT_TITLE,0);}
	void	SortByFName(){SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_PE_SORT_FNAME,0);}
	void	SortByFNamePath(){SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_PE_SORT_FNAME_PATH,0);}
	void	PLInsertTrack(CStdString strFile, int nPos);
	BOOL	IsPEWindowWinshade();
	
	// Equaliser Functions
	long	GetEqPosition(long Band);
	void	SetEqPosition(long Band, long newVal);
	long	GetPreAmpPosition();
	void	SetPreAmpPosition(long newVal);
	BOOL	GetEqualizerVisible();
	void	SetEqualizerVisible(BOOL newVal);
	BOOL	GetEqEnabled();
	void	SetEqEnabled(BOOL newVal);
	BOOL	GetAutoloadEnabled();
	void	SetAutoloadEnabled(BOOL newVal);

	//Minibrowser stuff
	BOOL	GetMiniBrowserVisible();
	void	SetMiniBrowserVisible(BOOL newVal);
	void	SetMiniBrowserURL(CStdString strUrl, BOOL bForceOpen);
	BOOL  IsInetAvailable();

	int		GetTrackLength();
	int		GetPlayingTime();
	CStdString GetCurrTrackPath();
	int		GetWinampState();
	CStdString GetWinampDir();

	BOOL	IsPlaying();
	BOOL	IsPaused();
	BOOL	IsStopped();
	CStdString	GetPlayerVersion();
	void	SetVolume(int nValue);
	int     GetVolume();
	CStdString GetFileNameFromPos(int nPos);
	
protected:
	BOOL	WindowsReady();
	void	SendCommand(long command);
	
	HWND	m_hWndWinamp;
	HWND	m_hWndWinampEQ;
	HWND	m_hWndWinampPE;
	HWND	m_hWndWinampMB;
	HWND	m_hWndWinampML;
	int		m_nVolume;
	CStdString	m_strVersionString;		// Winamp Version String
	CStdString	m_strWinampDir;			// Winamp Directory
};