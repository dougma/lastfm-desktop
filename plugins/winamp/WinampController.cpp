// WinampController.cpp: implementation of the CWinampController class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "WinampController.h"
#include <shellapi.h>
#include "dbg.h"
#include "UTF.h"

//////////////////////////////////////////////////////////////////////
// General Stuff
//////////////////////////////////////////////////////////////////////

CWinampController::CWinampController()
{
	m_hWndWinamp		= NULL;
	m_hWndWinampEQ		= NULL;
	m_hWndWinampPE		= NULL;
	m_hWndWinampMB		= NULL;
	m_hWndWinampML		= NULL;
	m_strVersionString	= "";		// Winamp Version String
	m_strWinampDir		= "";		// Winamp Directory
}

CWinampController::~CWinampController()
{
}

BOOL CWinampController::Init(BOOL bStartWinampIfNotRunning,
							 HWND WinampWnd /* = NULL */,
							 HWND WinampWndML /* = NULL */)
{	
	if(WinampWnd == NULL)
		m_hWndWinamp = FindWindow("Winamp v1.x", NULL);
	else
		m_hWndWinamp = WinampWnd;

	if(m_hWndWinamp == NULL)
	{
		if(bStartWinampIfNotRunning)
		{
			::ShellExecute(NULL, "open", "c:\\progra~1\\winamp\\winamp.exe", NULL, "c:\\progra~1\\winamp\\", SW_MINIMIZE);

			//Give it time to show itself
			Sleep(1000);
			m_hWndWinamp = FindWindow("Winamp v1.x", NULL);

			if(m_hWndWinamp == NULL)
				return FALSE;
		}
	}
	else
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp Window");			
	
	if((m_hWndWinampEQ = FindWindow("Winamp EQ", NULL)) == NULL)
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp EQ Window");

	if((m_hWndWinampPE = FindWindow("Winamp PE", NULL)) == NULL)
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp PE Window");

	if((m_hWndWinampMB = FindWindow("Winamp MB", NULL)) == NULL)
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp MB Window");

	if(WinampWndML == NULL)
	{
		long libhwndipc = (LONG)SendMessage(m_hWndWinamp, WM_WA_IPC, (WPARAM)"LibraryGetWnd", IPC_REGISTER_WINAMP_IPCMESSAGE);

		if((m_hWndWinampML = (HWND)SendMessage(m_hWndWinamp,WM_WA_IPC,-1,(LPARAM)libhwndipc)) == NULL)
			PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp ML Window");
	}
	else
		m_hWndWinampML = WinampWndML;

	GetWinampDir();
	GetPlayerVersion();

    return TRUE;
}

BOOL CWinampController::WindowsReady()
{
	BOOL bReady = TRUE;

	if(m_hWndWinamp == NULL || !IsWindow(m_hWndWinamp))
		bReady = FALSE;

	if(m_hWndWinampEQ == NULL || !IsWindow(m_hWndWinampEQ))
		bReady = FALSE;

	if(m_hWndWinampPE == NULL || !IsWindow(m_hWndWinampPE))
		bReady = FALSE;

	if(m_hWndWinampMB == NULL || !IsWindow(m_hWndWinampMB))
		bReady = FALSE;

	if(!bReady)
		Init(TRUE);

	return TRUE;
}

void CWinampController::SendCommand(long Code)
{
	SendMessage(m_hWndWinamp, WM_COMMAND, Code, 0);
}


//////////////////////////////////////////////////////////////////////
// General Winamp Stuff
//////////////////////////////////////////////////////////////////////
long CWinampController::GetVersion()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETVERSION);
}

long CWinampController::GetStatus()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_ISPLAYING);
}

void CWinampController::SetStatus(long newVal)
{
	
	switch(newVal)
	{
	case WINAMP_STOPPED:
		SendCommand(WINAMP_STOP);
		break;
	case WINAMP_PAUSED:
		SendCommand(WINAMP_PAUSE);
		break;
	default:
		Play();
		break;
	}
}


void CWinampController::NextTrack()
{
	SendCommand(WINAMP_FASTFWD);
}

void CWinampController::PrevTrack()
{
	SendCommand(WINAMP_REWIND);
}

long CWinampController::GetCurrentPos()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME);
}

void CWinampController::SetCurrentPos(long newVal)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_JUMPTOTIME);
}

void CWinampController::SetVolume(long newVal)
{
	
	if(newVal < 0)
	{
		newVal = 0;
	}
	if(newVal > 255)
	{
		newVal = 255;
	}
	long CurrentValue(SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETVOLUME));
}

void CWinampController::IncreaseVolume()
{
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_VOLUMEUP, 0);
}

void CWinampController::DecreaseVolume()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_VOLUMEDOWN, 0);
}

long CWinampController::GetSampleRate()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETINFO);
}

long CWinampController::GetBitrate()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,1,IPC_GETINFO);
}

long CWinampController::GetChannels()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,2,IPC_GETINFO);
}

void CWinampController::SetPanning(long PanIndex)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,PanIndex,IPC_SETPANNING);
}

void CWinampController::ChangeDirectory(CStdString NewDir)
{
	if(!NewDir.IsEmpty())
	{
		COPYDATASTRUCT cds;
		cds.dwData = IPC_CHDIR;
		cds.lpData = (void *) NewDir.GetBuffer(0);
		cds.cbData = NewDir.GetLength(); // include space for null char
		SendMessage(m_hWndWinamp,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
	}
}

CStdString CWinampController::GetSongFileName(long PlayListPos)
{	
	CStdString Name = (char *)SendMessage(m_hWndWinamp,WM_WA_IPC,PlayListPos,IPC_GETPLAYLISTFILE);
	return Name;
}

CStdString CWinampController::GetSongFileNameUtf8(long PlayListPos)
{	
	CStdStringW FileNameW = (wchar_t *)SendMessage(m_hWndWinamp, WM_WA_IPC, PlayListPos, IPC_GETPLAYLISTFILEW);
	wchar_t Filename[MAX_PATH];
	wcscpy(Filename, FileNameW);

    std::string fileNameUtf8;
	UTF::wchartoutf8(Filename, fileNameUtf8);
    
    return CStdString(fileNameUtf8.c_str());
}

CStdString CWinampController::GetSongTitle(long PlayListPos)
{
	CStdString Name = (char*)SendMessage(m_hWndWinamp,WM_WA_IPC,PlayListPos,IPC_GETPLAYLISTTITLE);
	return Name;
}

CStdString CWinampController::GetSongTitleUtf8(long PlayListPos)
{
	wchar_t* name = (wchar_t*)SendMessage(m_hWndWinamp,WM_WA_IPC,PlayListPos,IPC_GETPLAYLISTTITLEW);

	std::string sbuffer;
	UTF::wchartoutf8(name, sbuffer);
    return CStdString(sbuffer.c_str());
}

CStdString CWinampController::GetCurrentSongFileName()
{
	return GetSongFileName(GetPlayListPos());
}

CStdString CWinampController::GetCurrentSongFileNameUtf8()
{
	return GetSongFileNameUtf8(GetPlayListPos());
}

CStdString CWinampController::GetCurrentSongTitle()
{
	return GetSongTitle(GetPlayListPos());
}

CStdString CWinampController::GetSkinName()
{
	
	CStdString Temp;
	SendMessage(m_hWndWinamp,WM_WA_IPC,(WPARAM)Temp.GetBuffer(0),IPC_GETSKIN);
	return Temp;
}

void CWinampController::SetSkinName(CStdString Name)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,(WPARAM)Name.GetBuffer(0),IPC_SETSKIN);	
}

void CWinampController::StartPlugIn(CStdString PlugInName)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,(WPARAM)PlugInName.GetBuffer(0),IPC_EXECPLUG);

}

void CWinampController::ToggleShufflePlay()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FILE_SHUFFLE, 0);
}

void CWinampController::ToggleRepeatPlay()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FILE_REPEAT, 0);
}

void CWinampController::Stop()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_STOP, 0);
}

void CWinampController::Pause()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_PAUSE, 0);
}

void CWinampController::FastForward()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FASTFWD, 0);
}

void CWinampController::Rewind()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_REWIND, 0);
}

void CWinampController::CloseWinamp()
{
	
	SendMessage(m_hWndWinamp, WM_CLOSE, 0, 0);
}

void CWinampController::SongChange()
{
}

long CWinampController::GetCurrentSongLength()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC, 1,IPC_GETOUTPUTTIME);
}

void CWinampController::FastFwd5Secs()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FFWD5S, 0);
}

void CWinampController::Rewind5Secs()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_REW5S, 0);
}

BOOL CWinampController::GetWinampVisible()
{
	return IsWindowVisible(m_hWndWinamp);
}

void CWinampController::SetWinampVisible(BOOL newVal)
{
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinamp))
		{
			ShowWindow(m_hWndWinamp, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinamp))
		{
			ShowWindow(m_hWndWinamp, SW_HIDE);
		}
	}
}

BOOL CWinampController::GetShuffleStatus()
{
	
	if(GetMenuState(GetOptionMenu(), WINAMP_MENU_SHUFFLE, MF_BYCOMMAND) == MF_CHECKED)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CWinampController::SetShuffleStatus(BOOL newVal)
{
	
	BOOL CurVal = GetShuffleStatus();
	if(CurVal != newVal)
	{
		ToggleShufflePlay();
	}
}

BOOL CWinampController::GetRepeatStatus()
{
	
	if(GetMenuState(GetOptionMenu(), WINAMP_MENU_REPEAT, MF_BYCOMMAND) == MF_CHECKED)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CWinampController::SetRepeatStatus(BOOL newVal)
{
	
	BOOL CurVal = GetRepeatStatus();
	if(CurVal != newVal)
	{
		ToggleRepeatPlay();
	}
}

HMENU CWinampController::GetOptionMenu()
{
	
	// Get System menu handle
	HMENU hMenuSystem = GetSystemMenu(m_hWndWinamp, 0);

	//Get Winamp sub-menu handle
	HMENU hMenuWinamp = GetSubMenu(hMenuSystem, 0);

	//Get Options sub-menu handle
	return GetSubMenu(hMenuWinamp, WINAMP_MENU_OPTIONS_POS);
}

//////////////////////////////////////////////////////////////////////
// Playlist Stuff
//////////////////////////////////////////////////////////////////////

void CWinampController::ClearPlaylist()
{
	SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_DELETE);
}

long CWinampController::GetPlayListPos()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETLISTPOS);
}

void CWinampController::SetPlayListPos(long newVal)
{
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETPLAYLISTPOS);
}

long CWinampController::GetPlayListCount()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH);
}

BOOL CWinampController::GetPlaylistVisible()
{
	return IsWindowVisible(m_hWndWinampPE);
}

void CWinampController::SetPlaylistVisible(BOOL newVal)
{
	
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinampPE))
		{
			ShowWindow(m_hWndWinampPE, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinampPE))
		{
			ShowWindow(m_hWndWinampPE, SW_HIDE);
		}
	}
}

void CWinampController::DeletePEItem(int Pos)
{
	MovePEHighlight(Pos);
	DeletePECurHighlight();
}

void CWinampController::DeletePECurHighlight()
{
	SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_DEL_FRM_PE, 0 );
}

void CWinampController::MovePEHighlight(int Pos)
{
	int PELength = GetPlayListCount();

	if(Pos < PELength)
	{
		SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_HLT_UP, 0);

		// Move the selector down to the song to remove
		for(int i = 0; i < Pos; i++ )
			SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_HLT_DOWN, 0 );
	}
}

bool CWinampController::MovePEItem(int curPos, int NewPos)
{
	int diff = 0;

	if(curPos == NewPos)
		return false;

	MovePEHighlight(curPos);

	if(curPos > NewPos)
	{
		diff = curPos - NewPos;

		for(int i = 0; i < diff; i++)
			SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_ITEM_UP, 0 );
	}
	else
	{
		diff = NewPos - curPos;

		for(int i = 0; i < diff; i++)
			SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_ITEM_DOWN, 0 );
	}

	return true;
}

typedef struct 
{ 
	char file[MAX_PATH]; 
	int index; 
} fileinfo; 
 
void CWinampController::PLInsertTrack(CStdString strFile, int nPos)
{
/*	int nCount = GetPlayListCount();

	nCount++;
	Enqueue(strFile);
	MovePEItem(nCount, nPos);*/

	COPYDATASTRUCT cds; 
	fileinfo f; 
	cds.dwData = IPC_PE_INSERTFILENAME; 
	strcpy(f.file, strFile); // Path to the new entry 
	f.index=nPos;                 // Position to insert at 
	cds.lpData = (void *)&f; 
	cds.cbData = sizeof(fileinfo); 
	::SendMessage(m_hWndWinampPE,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
}

//////////////////////////////////////////////////////////////////////
// EQ Stuff
//////////////////////////////////////////////////////////////////////
long CWinampController::GetEqPosition(long Band)
{
	
	if(Band < 0)
	{
		Band = 0;
	}
	if(Band > 9)
	{
		Band = 9;
	}
	return SendMessage(m_hWndWinamp,WM_WA_IPC,Band,IPC_GETEQDATA);
}

void CWinampController::SetEqPosition(long Band, long newVal)
{
	
	if(Band < 0)
	{
		Band = 0;
	}
	if(Band > 9)
	{
		Band = 9;
	}
	if(newVal < 0)
	{
		newVal = 0;
	}
	if(newVal > 63)
	{
		newVal = 63;
	}
	SendMessage(m_hWndWinamp,WM_WA_IPC,Band,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}

long CWinampController::GetPreAmpPosition()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,10,IPC_GETEQDATA);
	
}

void CWinampController::SetPreAmpPosition(long newVal)
{
	
	if(newVal < 0)
	{
		newVal = 0;
	}
	if(newVal > 63)
	{
		newVal = 63;
	}
	SendMessage(m_hWndWinamp,WM_WA_IPC,10,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}

BOOL CWinampController::GetEqEnabled()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,11,IPC_GETEQDATA);
}

void CWinampController::SetEqEnabled(BOOL newVal)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,11,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}

BOOL CWinampController::GetAutoloadEnabled()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,12,IPC_GETEQDATA);
}

void CWinampController::SetAutoloadEnabled(BOOL newVal)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,12,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}
BOOL CWinampController::GetEqualizerVisible()
{
	
	return IsWindowVisible(m_hWndWinampEQ);
}

void CWinampController::SetEqualizerVisible(BOOL newVal)
{
	
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinampEQ))
		{
			ShowWindow(m_hWndWinampEQ, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinampEQ))
		{
			ShowWindow(m_hWndWinampEQ, SW_HIDE);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// MB Stuff
//////////////////////////////////////////////////////////////////////

BOOL CWinampController::GetMiniBrowserVisible()
{
	
	return IsWindowVisible(m_hWndWinampMB);
}

void CWinampController::SetMiniBrowserVisible(BOOL newVal)
{
	
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinampMB))
		{
			ShowWindow(m_hWndWinampMB, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinampMB))
		{
			ShowWindow(m_hWndWinampMB, SW_HIDE);
		}
	}
}

void CWinampController::SetMiniBrowserURL(CStdString strUrl, BOOL bForceOpen)
{
	char* lpszUrl = NULL;
	
	// Force open the mini-browser window
	if(bForceOpen)
		::SendMessage(m_hWndWinamp, WM_WA_IPC, NULL, IPC_MBOPEN);
	
	//Build the url string to be displayed
	lpszUrl = new char[strUrl.GetLength()+1];
	
	sprintf(lpszUrl, "%s", strUrl.c_str());
	
	//Make the mini-browser navigate to the URL 
	::SendMessage(m_hWndWinamp,WM_WA_IPC, reinterpret_cast<WPARAM>(lpszUrl), IPC_MBOPENREAL);
	
	delete[] lpszUrl;
}

BOOL CWinampController::IsInetAvailable()
{
	return ::SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_INETAVAILABLE);
}

void CWinampController::JumpToTrack(unsigned int Number)
{
	
	PostMessage(m_hWndWinamp,WM_COMMAND,WINAMP_BUTTON4,0);
	Number--;
	PostMessage(m_hWndWinamp,WM_WA_IPC,Number,IPC_SETPLAYLISTPOS);
	PostMessage(m_hWndWinamp,WM_COMMAND,WINAMP_BUTTON2,0);
}

BOOL CWinampController::GetWinampStats(WA_STATS& stats)
{
	CStdString strVal, strTemp;
	int nPos;

	// Get the data
	char buffer[MAX_PATH];
	char buffer2[MAX_PATH];
	GetWindowsDirectory(buffer, MAX_PATH);
	strcat(buffer, "\\winamp.ini");

	DWORD dwRes = GetPrivateProfileString("WinampReg", "Stats",	NULL, buffer2, MAX_PATH, buffer);
	
	if(dwRes <= 0)
		return FALSE;

	strTemp = buffer2;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
		stats.nWinampRuns =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
	//	stats.tRunning =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
	//	stats.tRunningMB =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
//		stats.tRunningNoneMin =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
	//	stats.tRunningPlay =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
//		stats.tRunningPlayNonMin =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	return TRUE;
}

CStdString CWinampController::GetWinampDir()
{
	PRINTF(DEBUG_FLOW, "CWinampController::GetWinampDir", "Getting Winamp Dir.");
	if(!m_strWinampDir.IsEmpty())
		return m_strWinampDir;

	char filename[512];
	CStdString Path;
	GetModuleFileName(NULL,filename,sizeof(filename));
	Path = filename;

	int pos = Path.ReverseFind('\\');
	if(pos > 0)
	{
		// Get Winamp Directory
		m_strWinampDir = Path.Left(pos + 1);
		return m_strWinampDir;
	}
	else
		PRINTF(DEBUG_ERROR, "CWinampController::GetWinampDir", "Failed to Get Winamp Dir.");

	return "";
}

void CWinampController::SetVolume(int nVolume)
{
	if(nVolume < 0 || nVolume > 255)
		return;

	SendMessage(m_hWndWinamp,WM_WA_IPC,nVolume,IPC_SETVOLUME);
}

CStdString CWinampController::GetPlayerVersion()
{
	if(m_strVersionString.IsEmpty())
	{
		CStdString Text;
		
		LRESULT version = SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETVERSION);
		
		Text.Format("%x", SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETVERSION));
		m_strVersionString.Format("%s.%s%s", Text.Left(1).c_str(), Text.Mid(2, 1).c_str(), Text.Mid(3).c_str());
	}

	return m_strVersionString;
}

BOOL CWinampController::Enqueue(CStdString strFileName)
{
	COPYDATASTRUCT cds;
	cds.dwData = IPC_PLAYFILE;
	cds.lpData = (void *)strFileName.GetBuffer(0);
	cds.cbData = strlen((char *) cds.lpData) + 1; // include space for null char

	int nRes = ::SendMessage(m_hWndWinamp, WM_COPYDATA,1,(LPARAM)&cds);

	return TRUE;
}

BOOL CWinampController::Play(CStdString strFileName /*= ""*/, BOOL bKeepList /* = TRUE */)
{
	if(!strFileName.IsEmpty())
	{
		if(!bKeepList)
		{
			ClearPlaylist();
			Enqueue(strFileName);
		}
		else
		{
			PLInsertTrack(strFileName, 0);
		}
	}

	::SendMessage(m_hWndWinamp, WM_COMMAND,WINAMP_BUTTON4, 0);
	::SendMessage(m_hWndWinamp, WM_WA_IPC,0,IPC_STARTPLAY);

	return TRUE;
}

CStdString CWinampController::GetFileNameFromPos(int nPos)
{
	// What are we playing?
	CStdString Text;
	char buffer[MAX_PATH];
	memset(buffer, 0, MAX_PATH);
	char* p = buffer;
	p = reinterpret_cast<char*>(SendMessage(m_hWndWinamp,WM_WA_IPC,nPos,IPC_GETPLAYLISTFILE));
	Text = p;
	return Text;
}

BOOL CWinampController::IsStopped()
{
	return (GetWinampState() == WA_STATE_STOP);
}

BOOL CWinampController::IsPlaying()
{
	return (GetWinampState() == WA_STATE_PLAY);
}

BOOL CWinampController::IsPaused()
{
	return (GetWinampState() == WA_STATE_PAUSE);
}

// Returned in SECONDS
int	CWinampController::GetTrackLength()
{
	int nTrackLength = SendMessage(m_hWndWinamp,WM_WA_IPC,1,IPC_GETOUTPUTTIME);
	
	return nTrackLength;
}

//Returned in Milliseconds
int	CWinampController::GetPlayingTime()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME);
}

CStdString CWinampController::GetCurrTrackPath()
{
	int nPos = SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETLISTPOS);
	return GetFileNameFromPos(nPos);
}

int CWinampController::GetWinampState()
{
	// Are actually playing?
	int ret = SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_ISPLAYING);
	if(ret == 1)
		return WA_STATE_PLAY;
	else if(ret == 3)
		return WA_STATE_PAUSE;
	else if(ret == 0)
		return WA_STATE_STOP;

	return -1;
}

BOOL CWinampController::IsPEWindowWinshade()
{
	RECT rcPEWnd;
	
	::GetWindowRect(m_hWndWinampPE, &rcPEWnd);

	if((rcPEWnd.bottom - rcPEWnd.top) < 116)
		return TRUE;
	
	return FALSE;
}

int CWinampController::GetVolume()
{
	return -1;
}


CStdString CWinampController::GetCurrentSongMetadataUtf8(char* MetaData)
{
	CStdStringW FileNameW = (wchar_t *)SendMessage(m_hWndWinamp, WM_WA_IPC, 
		GetPlayListPos(), IPC_GETPLAYLISTFILEW);
	wchar_t Filename[MAX_PATH];
	wcscpy(Filename, FileNameW);

    wchar_t buffer[1024];
	memset(buffer, 0, sizeof(buffer));

    extendedFileInfoStructW Info;
	Info.ret = buffer;
	Info.retlen = 1024;
	Info.filename = Filename;
	std::wstring wMeta;
	UTF::utf8towstring(MetaData, wMeta);
	wchar_t* metadup = _wcsdup(wMeta.c_str());
	Info.metadata = metadup;
    LRESULT lRet = SendMessage(m_hWndWinamp, WM_WA_IPC, (WPARAM)&Info, IPC_GET_EXTENDED_FILE_INFOW);
	free(metadup);
    if (lRet != 1)
    {
        PRINTF(DEBUG_DETAIL, 
              "CWinampController::GetCurrentSongMetadata", 
              "FAILED - metadata '%s', ret is %d", 
              MetaData,lRet);
        return CStdString();
    }
	std::string sbuffer;
	UTF::wchartoutf8(buffer, sbuffer);
    return CStdString(sbuffer.c_str());
}

// Only works for Winamp 2.90 upwards
// Not using Media Library
CStdString CWinampController::GetCurrentSongMetadata(char* MetaData)
{
    char                    buffer[1024];
    char                    Filename[MAX_PATH];
    extendedFileInfoStruct  Info;
    LRESULT                 lRet = 0;

    memset(buffer, 0, sizeof(buffer));
    strcpy(Filename, GetCurrentSongFileName());

    Info.ret = buffer;
    Info.retlen = sizeof(buffer);
    Info.filename = Filename;
    Info.metadata = strdup(MetaData);

    lRet = SendMessage(m_hWndWinamp,WM_WA_IPC, (WPARAM)&Info, IPC_GET_EXTENDED_FILE_INFO);
	free((void*)Info.metadata);
    if (lRet != 1)
    {
        PRINTF(DEBUG_DETAIL, 
              "CWinampController::GetCurrentSongMetadata", 
              "FAILED - metadata '%s', ret is %d", 
              MetaData,lRet);
    }

    return CStdString(buffer);
}

// Media Library version
BOOL CWinampController::GetCurrentSongInfo(SongInfo& info)
{
	itemRecord	rec;
	char        Filename[MAX_PATH];
	int			Result = 0;
	BOOL		bRet = FALSE;			

	memset(&rec, 0, sizeof(itemRecord));
	strcpy(Filename, GetCurrentSongFileName());
	rec.filename = Filename;

	Result = ::SendMessage(m_hWndWinampML,WM_ML_IPC,(WPARAM)&rec,ML_IPC_GETFILEINFO);

	if(Result != 0)
	{
		// Do Stuff
		info.strAlbum		= rec.album;
		info.strArtist		= rec.artist;
		info.strComment		= rec.comment;
		info.strGenre		= rec.genre;
		info.strTitle		= rec.title;
		info.nLength		= rec.length;
		info.nTrackNo		= rec.track;
		info.nYear			= rec.year;
		info.strFileName	= Filename;
		
		Result = ::SendMessage(m_hWndWinampML,WM_ML_IPC,(WPARAM)&rec,ML_IPC_FREEFILEINFO);
		bRet = TRUE;
	}
	
	return bRet;
}

