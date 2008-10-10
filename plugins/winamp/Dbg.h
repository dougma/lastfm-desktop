/*##############################################
##
## Module				: CDebug
## Description	: Thread-safe logging routines for logging 
##                from multiple processes to one log file. 
##
##               	Class includes printf-style formatted output 
##                routines, as well as timestamping of entries, 
##                process id, and process execution time.
##
##								Also includes maxfilesize attribute which restricts
##								the size of the file. Setting this to 0 disables logging
##								setting it to -1 disables max size.
##
## Author(s)		: Spib
##
##############################################*/

#ifndef ______DBG_H______
#define ______DBG_H______

#define DEBUG_DETAIL		1			// Low level detailed info
#define DEBUG_FLOW			3			// Low level program flow Info
#define DEBUG_INFO			5			// General Info
#define DEBUG_ERROR_NC	7			// Non-Critical Error
#define DEBUG_RQD				8			// Required Info
#define DEBUG_ERROR			9			// Critical Error

#define USE_DEBUG_PRINTF

#ifdef USE_DEBUG_PRINTF

//	Macro definitions
///////////////////////////
#define IFDEBUG( doit )	doit

#define DEBUG_INIT					debug.Init
#define DEBUG_ENABLE				debug.Enable
#define DEBUG_IS_ENABLED		debug.IsEnabled
#define DEBUG_GET_LEVEL			debug.GetLevel
#define DEBUG_SET_LEVEL			debug.SetLevel
#define DEBUG_SET_MAXSIZE		debug.SetMaxFileSize
#define DEBUG_GET_MAXSIZE		debug.GetMaxFileSize
#define DEBUG_GET_FILEPATH	debug.GetLogFilePath
#define PRINTF							debug.printf
#define SHOW_LASTERROR			debug.ShowLastError();
#define CLOSE_LOG						debug.CloseLog();


/*-----------------------------------------------------------------------
 *  Class        : CDebugPrintf
 *  Prototype    : class CDebugPrintf
 *  Description  : 
 *  Parent class : 
 *  History      : 
 */
class CDebug
{
public:
	CDebug();
	virtual ~CDebug();

	void Init(const char * strFileName, BOOL bOverWrite = TRUE, BOOL bEnable = TRUE, BOOL bUseXML = FALSE);

	void printf(int nLevel, const char * module, const char *fmt, ... );

	inline BOOL SetLevel(int nLevel)
	{
		if(nLevel <= DEBUG_ERROR && nLevel >= DEBUG_DETAIL)
		{
			m_nLogLevel = nLevel;
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetMaxFileSize(int nSize)
	{
		if(nSize >= 0)
		{
			if(nSize == 0)
				Enable(FALSE);
			else
				Enable(TRUE);

			m_nMaxFileSize = (nSize * 1024);
			return TRUE;
		}
		else if(nSize == -1)
		{
			m_nMaxFileSize = -1;
			Enable(TRUE);
			return TRUE;
		}

		CheckFileSize();

		return FALSE;
	}

	inline void Enable(BOOL bEnable = TRUE)
	{
		m_bEnabled = bEnable;
	}

	void							CloseLog(); 
	void							ShowLastError();
	BOOL							IsEnabled(){return m_bEnabled;}

	int								GetLevel(){return m_nLogLevel;}
	DWORD							GetMaxFileSize(){return m_nMaxFileSize;}
	char*							GetLogFilePath(){return m_strFilePath;}

private:
	BOOL							OpenLogFile(BOOL bOverWrite = TRUE);
	void							WriteHeader(BOOL bNewFile = TRUE);
	void							CheckFileSize();
	void							ReplaceXMLChars(const char* strInput, int nLen, char* strOutput);

	HANDLE						m_hFile;									// Log-file handle
	CRITICAL_SECTION	m_hLock;									// Lock for thread-safe access
	char							m_strFilePath[MAX_PATH];	// File Path

	DWORD							m_dwStartTime;						// Application start time 
	int								m_nLogLevel;							// Log Level
	BOOL							m_bEnabled;								// Enable / Disable Debug
	long							m_nMaxFileSize;					// Maximum size of log (in bytes)
	BOOL							m_bUseXML;								// Output the data as XML?
};

extern CDebug debug;


#else

#define IFDEBUG( doit )	

#define SLASH /
#define COMMENT ;SLASH/

#define PRINTF			COMMENT
#define SHOW_CONSOLE	COMMENT
#define HIDE_CONSOLE	COMMENT
#define SHOW_LASTERROR	COMMENT
#define CLOSE_LOG		COMMENT

#endif


#endif




