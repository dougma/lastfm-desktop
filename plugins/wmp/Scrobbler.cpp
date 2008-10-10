#include "stdafx.h"
#include "Scrobbler.h"
#include <shellapi.h>
#include <io.h>
//#include <exception>
#include <sstream>

using namespace std;

/*#############################################
##
## Module       : CScrobbler
## Function     : CScrobbler
## Description  : Constructor
##
## Author(s)    : Spib
##
## Parameters   : N/A
## Return       : N/A
##
##############################################*/
CScrobbler::CScrobbler()
{
    m_hInstance             = NULL;
    m_strWorkingDir         = _T("");
    m_bTrackPlayFlag        = FALSE;
    m_nMaxLogFileSize       = -1;
    m_bUseNowPlaying        = FALSE;
   	m_bExitThread			= FALSE;
	m_bRunningCallback		= FALSE;
    m_bAllowHttpStreams     = FALSE;
    
    m_AsState = AS_STOPPED;
    
    memset(&m_CurrentSong, 0, sizeof(m_CurrentSong));
}

/*##############################################
##
## Module       : CScrobbler
## Function     : ~CScrobbler
## Description  : Destructor
##
## Author(s)    : Spib
##
## Parameters   : N/A
## Return       : N/A
##
##############################################*/
CScrobbler::~CScrobbler()
{
}

/*##############################################
##
## Module       : CScrobbler
## Function     : StartScrobbling
## Description  : Startup routine for class
##                Checks the data is valid and that
##                the clss is ready to startup
##
## Author(s)    : Spib
##
## Parameters   : strWorkingDir - Plugin dir path
##                hInstance - HINSTANCE of plugin
## Return       : BOOL (TRUE = success)
##
##############################################*/
BOOL CScrobbler::StartScrobbling(CStdString strWorkingDir, HINSTANCE hInstance /*=NULL*/)
{
    BOOL    bRet = FALSE;
    UINT    threadID = 0;

    if ( ( !strWorkingDir.IsEmpty() ) )
    {
        m_strWorkingDir = strWorkingDir;
        m_hInstance = hInstance;
        m_Version.Refresh(m_hInstance);

        int pos = strWorkingDir.ReverseFind('\\');
        m_strWorkingDir = strWorkingDir.Left(pos+1);
        m_strFileName = strWorkingDir.Mid(pos+1);
    }
    else
        PRINTF(DEBUG_ERROR, "CScrobbler::StartScrobbling", "Working dir is blank.");
    
    InitialiseLogging();

    PRINTF(DEBUG_FLOW,
        "CScrobbler::StartScrobbling", "Working dir %s", 
        strWorkingDir.c_str());

    m_Submitter.Init(GetAppId(), ScrobSubCallback, this);
   
    // Timer
    unsigned dwThreadId = 0;
    if(_beginthreadex(NULL, 0, TimeUpdaterThread, reinterpret_cast<void *>(this), 0, &dwThreadId) == 0)
    {
        PRINTF(DEBUG_ERROR, "CScrobbler::StartScrobbling", "Could not start timer thread.");
        bRet = FALSE;
    }
    
    PRINTF(DEBUG_RQD, "CScrobbler::StartScrobbling", "Startup Complete.");

    return bRet;
}
    
/*##############################################
##
## Module       : CScrobbler
## Function     : StopScrobbling
## Description  : Performs closedown functions
##
## Author(s)    : Spib
##
## Parameters   : N/A
## Return       : BOOL (TRUE = success)
##
##############################################*/
BOOL CScrobbler::StopScrobbling()
{
    PRINTF(DEBUG_FLOW, "CScrobbler::StopScrobbling", "Stopping...");

    ASStop();
    m_Submitter.Term();

    m_bExitThread = TRUE;

    ::SetEvent(m_hTimerEvent);
    
    return TRUE;
}

/*##############################################
##
## Module       : CScrobbler
## Function     : TimerCallback
## Description  : Callback function for the external
##                timer. Checks playback state and 
##                triggers submissions
##
## Author(s)    : Spib
##
## Parameters   : N/A
## Return       : BOOL (FALSE causes timer to be killed)
##
##############################################*/
BOOL CScrobbler::TimerCallback()
{
    BOOL bRet = TRUE;

    // If we are already checking or we aren't enabled don't continue.
    // We don't return FALSE. This would
    // cancel the timer which is OK when we are disabled but if the user enables us 
    // without restarting the player we have no way to restart the timer.
    // Get it? Good.
    if (m_bRunningCallback == TRUE)
        return bRet;

    m_bRunningCallback = TRUE;

    // Non-blocking track play detection
    if(m_bTrackPlayFlag)
    {
        // If we get here, a new track has just started

        SONG_INFO tmpSong;
        memset(&tmpSong, 0, sizeof(tmpSong));

        // Not currently using this return value as we want to send partial
        // info also and let the app validate it.
        BOOL bOK = GetCurrentSong(&tmpSong);
        memcpy(&m_CurrentSong, &tmpSong, sizeof(m_CurrentSong));

        m_bTrackPlayFlag = FALSE;

        if (!SupportedFileType(m_CurrentSong.m_strFileName))
        {
            ASStop();
            m_AsState = AS_STOPPED;
            m_bRunningCallback = FALSE;
            return bRet;
        }

        // Send Start
        ASStart();
        m_AsState = AS_PLAYING;
    }

    switch (m_AsState)
    {
        case AS_PLAYING:
            if (IsPaused())
            {
                ASPause();
                m_AsState = AS_PAUSED;
            }
            else if (IsStopped())
            {
                ASStop();
                m_AsState = AS_STOPPED;
            }
            break;
        
        case AS_STOPPED:
            break;

        case AS_PAUSED:
            if (IsPlaying())
            {
                ASResume();
                m_AsState = AS_PLAYING;
            }
            else if (IsStopped())
            {
                ASStop();
                m_AsState = AS_STOPPED;
            }
            break;
        
        default:
            ASSERT(false);
    }

    m_bRunningCallback = FALSE;
    return bRet;
}

/*##############################################
##
## Module		: CScrobbler
## Function		: SupportedFileType
## Description	: Checks that the file type is valid 
##                for sending to the server
##
## Author(s)	: Spib
##
## Parameters	: strPath - full file path
## Return		: TRUE if file is OK
##
##############################################*/
BOOL CScrobbler::SupportedFileType(CStdString strPath)
{
	BOOL		bRet = TRUE;
	CStdString	strTemp;
	CStdString	strReason;
	int			nPos = 0;

	strPath.MakeUpper();

	strReason = "Unknown";

	if(!strPath.IsEmpty() && (strPath.GetLength() > 3))
	{
		strTemp = strPath.Left(7);
		if((strTemp == "HTTP://") && !m_bAllowHttpStreams)
		{
			bRet = FALSE;
			strReason = "Cannot submit HTTP streams";
            PRINTF(DEBUG_INFO, "CScrobbler::SupportedFileType",
                "File '%s' not supported: %s", strPath.c_str(), strReason.c_str());
			return bRet;
		}
		strTemp = strPath.Left(6);
		if((strTemp == "MMS://") && !m_bAllowHttpStreams)
		{
			bRet = FALSE;
			strReason = "Cannot submit MMS streams";
            PRINTF(DEBUG_INFO, "CScrobbler::SupportedFileType",
                "File '%s' not supported: %s", strPath.c_str(), strReason.c_str());
			return bRet;
		}
		// Is this a file?
		if (strTemp.Mid(1,2) == ":\\" ||
		    strTemp.Mid(0,2) == "\\\\")
		{
			nPos = strPath.ReverseFind('.');

			if(nPos > 0)
			{
				strTemp = strPath.Mid(nPos+1);
		
				if ((strTemp == "NSV") || 
				    (strTemp == "AVI") ||
    				(strTemp == "M2V") ||
	    			(strTemp == "ASF") ||
		    		(strTemp == "WMV") ||
			    	(strTemp == "MPG") ||
				    (strTemp == "MPEG"))
				{
					strReason = "Cannot submit Video files";
					bRet = FALSE;
                    PRINTF(DEBUG_INFO, "CScrobbler::SupportedFileType",
                        "File '%s' not supported: %s", strPath.c_str(), strReason.c_str());
                    return bRet;
				}
			}

		}
	}

	return bRet;	
}


/*##############################################
##
## Module       : CScrobbler
## Function     : GetVersionString
## Description  : Gets a string with the app name and
##                version.
##
## Author(s)    : Spib
##
## Parameters   : N/A
## Return       : CStdString - version string
##
##############################################*/
CStdString CScrobbler::GetVersionString()
{
    CStdString strText;
    CStdString strVer;

#ifdef _DEBUG
    strVer = "INTERNAL DEBUG BUILD";
#else
    //strVer = "INTERNAL TEST BUILD";
#endif

    strText.Format("%s %s", AUDIOSCROBBLER_VERSION, strVer.c_str());

    return strText;
}

/*##############################################
##
## Module       : CScrobbler
## Function     : InitialiseLogging
## Description  : Sets up the logging stuff
##
## Author(s)    : Spib
##
## Parameters   : N/A
## Return       : N/A
##
##############################################*/
void CScrobbler::InitialiseLogging()
{       
    CStdString strVer;
    CStdString strTemp;
    CStdString strFileName;

    // Initialise logging stuff
    strTemp.Format("%s\\WmpPlugin.log", ScrobSubmitter::GetLogPath().c_str());

    // Here's how it works - 
    // If you have specified a log size > zero, the log
    // is appended to up to the maximum size until the limit
    // is reached. Then it is deleted and started again.
    // If you specify a size of zero, logging is disabled.
    // If you specify a size of < zero, logging is enabled but
    // the log is overwritten on each execution and there is no 
    // maximum size.
    if(m_nMaxLogFileSize > 0)
    {
        DEBUG_INIT(strTemp, FALSE, TRUE, FALSE);
    }
    else if(m_nMaxLogFileSize == 0)
    {
        DEBUG_INIT(strTemp, FALSE, FALSE, FALSE);
    }
    else
    {
        DEBUG_INIT(strTemp, TRUE, TRUE, FALSE);
    }

    DEBUG_SET_MAXSIZE(m_nMaxLogFileSize);

#ifdef _DEBUG
    DEBUG_SET_LEVEL(DEBUG_DETAIL);
#else
    DEBUG_SET_LEVEL(DEBUG_INFO);
#endif

    PRINTF(DEBUG_RQD, "Application Information", "%s v%s (Build %s) %s", GetAppName().c_str(), GetVersionString().c_str(), m_Version.GetAppPrivateBuild().c_str(), m_strFileName.c_str());
    PRINTF(DEBUG_RQD, "Application Information", "%s", GetPlayerVersion().c_str());
    PRINTF(DEBUG_RQD, "Application Information", "%s", m_Version.GetOSVersion().c_str());
}

/*##############################################
##
## Module       : CScrobbler
## Function     : OnTrackPlay
## Description  : Called when a new track has started
##
## Author(s)    : Spib
##
## Parameters   : N/A
## Return       : N/A
##
##############################################*/
void CScrobbler::OnTrackPlay()
{
    // Only called in WMP version, never in Winamp.
    m_bTrackPlayFlag = TRUE;
}


/*##############################################
##
## Module       : CScrobbler
## Function     : TimeUpdaterThread
## Description  : Acts as a rudimentary timer
##
## Author(s)    : Spib
##
## Parameters   : pParam - pScrobbler ptr
## Return       : DWORD
##
##############################################*/
unsigned __stdcall CScrobbler::TimeUpdaterThread(LPVOID pParam)
{ 
    CScrobbler* pScrobbler = reinterpret_cast<CScrobbler*>(pParam);
    
    PRINTF(DEBUG_DETAIL, "CScrobbler::TimeUpdaterThread", "Creating Event");

    // Before we enter the thread loop, let's check if a track got started
    // before we had a chance to handle it
    if ( pScrobbler->IsPlaying() )
    {
        PRINTF(DEBUG_DETAIL, "CScrobbler::TimeUpdaterThread",
            "Track playing at startup, will do extra OnTrackPlay");
        pScrobbler->OnTrackPlay();
    }

    pScrobbler->m_hTimerEvent = ::CreateEvent(NULL,// Default security
                                    TRUE,// Manual reset
                                    FALSE,// Initial state
                                    NULL // Name
                                    );
    
    // Start a 0.5 second timer, check if we need to exit
    // After AUDIOSCROBBLER_TIMER seconds, do the callback
    // Doing it this way means we can exit gracefully
    while (pScrobbler->m_bExitThread == FALSE)
    {
        // The event is only signalled on exit so this is essentially a Sleep(500)
        ::WaitForSingleObject(pScrobbler->m_hTimerEvent, 500);

        if(pScrobbler->m_bExitThread == FALSE)
        {
            pScrobbler->TimerCallback();
        }
    }
    
    return 0;
}

// AS communication functions    
void CScrobbler::ASStart()
{
    int id = m_Submitter.Start(
        m_CurrentSong.m_strArtist,
        m_CurrentSong.m_strTrack,
        m_CurrentSong.m_strAlbum,
        "",
        m_CurrentSong.m_nLength,
        m_CurrentSong.m_strFileName,
        ScrobSubmitter::UTF_8);

    ostringstream os;
    os << "ReqID " << id << ": " << "Sent Start for " << 
        m_CurrentSong.m_strArtist << " - " << m_CurrentSong.m_strTrack;
    PRINTF(DEBUG_INFO, "CScrobbler::ASStart", os.str().c_str());
}

void CScrobbler::ASStop()
{
    int id = m_Submitter.Stop();

    ostringstream os;
    os << "ReqID " << id << ": " << "Sent Stop";
    PRINTF(DEBUG_INFO, "CScrobbler::ASStop", os.str().c_str());
}

void CScrobbler::ASPause()
{
    int id = m_Submitter.Pause();

    ostringstream os;
    os << "ReqID " << id << ": " << "Sent Pause";
    PRINTF(DEBUG_INFO, "CScrobbler::ASPause", os.str().c_str());
}

void CScrobbler::ASResume()
{
    int id = m_Submitter.Resume();

    ostringstream os;
    os << "ReqID " << id << ": " << "Sent Resume";
    PRINTF(DEBUG_INFO, "CScrobbler::ASResume", os.str().c_str());
}

void CScrobbler::ScrobSubStatus(int reqID, bool error, std::string message)
{
    #ifdef _DEBUG
        if (error)
        {
            MessageBox(NULL, message.c_str(), NULL, MB_OK);
        }
    #endif

    ostringstream os;
    os << "ReqID " << reqID << ": " << message;
    
    int level = error ? DEBUG_ERROR_NC : DEBUG_INFO;
    PRINTF(level, "CScrobbler::ScrobSubStatus", os.str().c_str());
}

