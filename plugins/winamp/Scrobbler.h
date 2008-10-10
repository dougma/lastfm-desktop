  // Scrobbler.h: interface for the CScrobbler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCROBBLER_H__E3C26903_1FF2_4C2B_BF28_DF1F2B003DE1__INCLUDED_)
#define AFX_SCROBBLER_H__E3C26903_1FF2_4C2B_BF28_DF1F2B003DE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*##############################################
##
## Module           : CAudioScrobbler
## Description      : This class is the core of the 
##                    AudioScrobbler.
##
## Author(s)        : RJ, Spib
##                  : Butchered by Erik for new AS plugin
##
## Modifications
##
## 07-04-2003 - Mods to integrate the new client protocol
##
##############################################*/

#pragma once

#include "VersionApp.h"
#include "StdStringArray.h"
#include "ScrobSubmitter.h"

#define AUDIOSCROBBLER_VERSION              m_Version.GetAppVersion().c_str()
#define SONG_INFO_FIELD_SIZE                1024

typedef struct 
{
    char    m_strFileName[MAX_PATH];
    char    m_strArtist[SONG_INFO_FIELD_SIZE];
    char    m_strTrack [SONG_INFO_FIELD_SIZE];
    char    m_strAlbum [SONG_INFO_FIELD_SIZE];
    char    m_strGenre [SONG_INFO_FIELD_SIZE];
    char    m_strComment [SONG_INFO_FIELD_SIZE];
    int     m_nYear;
    int     m_nTrackNo;
    int     m_nLength;
}SONG_INFO;

class CScrobbler  
{
public:
    CScrobbler();
    virtual ~CScrobbler();

    BOOL                StartScrobbling(CStdString strWorkingDir, HINSTANCE hInstance = NULL); // Init Routine
    BOOL                StopScrobbling();                           // Init Routine

    //Virtual Overrides
    virtual CStdString  GetAppName() = 0;
    virtual CStdString  GetAppId() = 0;
    virtual CStdString  GetRegKey() = 0;
    virtual CStdString  GetPlayerVersion() = 0; // Returns the player specific version string
    virtual BOOL        IsPlaying() = 0;
    virtual BOOL        IsPaused() = 0;
    virtual BOOL        IsStopped() = 0;
    virtual CStdString  GetCurrentSongFileName() = 0;
    virtual int         GetTrackLength() = 0;
    virtual int         GetTrackPosition() = 0;
    virtual BOOL        GetCurrentSong(SONG_INFO* pSong, bool utf8 = true) = 0;
    virtual HWND        GetParentWnd() = 0;
    
    // Other
    CStdString          GetVersionString();     // Get the AudioScrobbler's version info
    CStdString          GetBuild(){return m_Version.GetAppPrivateBuild();}

    static              unsigned __stdcall TimeUpdaterThread(LPVOID p);
    BOOL                TimerCallback();

    static void         ScrobSubCallback(int reqID, bool error, std::string message, void* userData)
    { reinterpret_cast<CScrobbler*>(userData)->ScrobSubStatus(reqID, error, message); }

protected:  
	virtual BOOL	    SupportedFileType(CStdString strPath);	// Is this a supported file type?
    void                OnTrackPlay();                              // A new track has started
    void                InitialiseLogging();                                // Initialise the logging
    CStdString          ConvertFileSize(int nBytes);                // Convert file size in bytes to KB/MB
    ScrobSubmitter*     m_Submitter;
    
public:
    // Internal Variables
    CVersionApp             m_Version;
    CStdString              m_strWorkingDir;            // Current working dir
    CStdString              m_strFileName;              // Module File Name
    HINSTANCE               m_hInstance;
    BOOL                    m_bExitThread;
    HANDLE                  m_hTimerEvent;              // Timer Handle

    // Song playback stuff
    SONG_INFO               m_CurrentSong;          // Currently Playing Song
    BOOL                    m_bTrackPlayFlag;

    // State Flags
    BOOL                    m_bRunningCallback;     // Are we currently in the timer callback?

    // Configurable Variables
    int                     m_nMaxLogFileSize;      // Max size of debug log file
    BOOL                    m_bAllowHttpStreams;
    BOOL                    m_bUseNowPlaying;
    
private:

    void ASStart( ScrobSubmitter::Encoding enc );
    void ASStop();
    void ASPause();
    void ASResume();

    void ScrobSubStatus(int reqID, bool error, std::string message);
    
    enum EAsState
    {
        AS_STOPPED,
        AS_PLAYING,
        AS_PAUSED
    };
    
    EAsState        m_AsState;
};

#endif // !defined(AFX_SCROBBLER_H__E3C26903_1FF2_4C2B_BF28_DF1F2B003DE1__INCLUDED_)
