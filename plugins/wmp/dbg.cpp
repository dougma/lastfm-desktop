/*************************************************************************
 FILE :			dbg.cpp

 Author :		Spib

 Description:	CDebug
				Thread-safe logging routines for logging from multiple
				processes to one log file. 

				Class includes printf-style formatted output routines, as
				well as timestamping of entries, process id, and process execution time.

 Spib	- Allow max file size
				Allows debugging to be activated at runtime.

*************************************************************************/
#include <windows.h>
#include <process.h>
#include "dbg.h"      

#ifdef USE_DEBUG_PRINTF

#define MSG_BUFFER_SIZE		1024*50

const char DIVIDER[10] = { 13, 10, '-', '-', '-', '-', '-', '-', 13, 10 };
const char CrLf[3]	   = { 13, 10, 0 };

CDebug debug;

/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : CDebug::CDebug ()
 *  Description : 
 *  Parameters  :                   
 *  Return      : 
 *  History     : 
 */
CDebug::CDebug ()
{
	m_bEnabled			= FALSE;  
	m_nLogLevel			= DEBUG_ERROR;
	m_nMaxFileSize	= -1;
}

/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : CDebug::CDebug ()
 *  Description : 
 *  Parameters  :                   
 *  Return      : 
 *  History     : 
 */
CDebug::~CDebug ()
{
	CloseLog();
}

void CDebug::Init(const char* strFileName, BOOL bOverWrite /* = TRUE */, BOOL bEnable /* = TRUE */, BOOL bUseXML /*= FALSE*/)
{	
	if(strlen(strFileName) <= 0)
		return;

	m_bEnabled		= bEnable;
	m_bUseXML			= bUseXML;

	strncpy(m_strFilePath, strFileName, MAX_PATH);

	if(m_bUseXML)
		strcat(m_strFilePath, ".xml");

	InitializeCriticalSection(&m_hLock);
		
	if(OpenLogFile(bOverWrite))
	{
		m_dwStartTime = GetTickCount();
		WriteHeader(bOverWrite);
	}
}

/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : void CDebug::CloseLog ()
 *  Description : Closes log file and releases handles. 
 *  Parameters  :                   
 *  Return      : void
 *  History     : 
 */
void CDebug::CloseLog ()
{
	char buffer[9];
	DWORD x = 0;

	if(m_bEnabled && m_hFile != NULL)
	{
		if(m_bUseXML)
		{		
			strcpy(buffer, "</DEBUG>");
			WriteFile(m_hFile,	buffer,    lstrlen(buffer),    &x, 0 );
		}

		CloseHandle(m_hFile );

		DeleteCriticalSection(&m_hLock );
	}
}

/*-----------------------------------------------------------------------
 *  Function    : 
 *  Prototype   : void CDebug::printf (const char *fmt, ... )
 *  Description : Prints formatted string to log-file
 *  Parameters  : const char *fmt - format string
                  ...             - list of parameters
 *  Return      : void
 *  History     : 
 */
void CDebug::printf (int nLevel, const char* strMod, const char *fmt, ... )
{
	DWORD				x;
	char				buf[256];
	SYSTEMTIME	time;
	char				buffer[MSG_BUFFER_SIZE];
	char				buffer2[MSG_BUFFER_SIZE];
	va_list			arglist;
	DWORD				dwDiffTime;

	if((nLevel < m_nLogLevel) || (nLevel < 0) || !m_bEnabled)
		return;

	memset(buffer, 0, sizeof(buffer));
	memset(buffer2, 0, sizeof(buffer2));
	memset(buf, 0, sizeof(buf));

	EnterCriticalSection(&m_hLock );

	// Do this inside the section in case we have to delete 
	// the file
	CheckFileSize();

	// Format string
	va_start(arglist, fmt);
	wvsprintf(buffer, fmt, arglist);
	va_end(arglist);

	// Format time stamp
	GetLocalTime(&time);

	dwDiffTime = GetTickCount() - m_dwStartTime;

	SetFilePointer(m_hFile, 0, 0, FILE_END );
	
	// XML Version
	if( m_bUseXML )
	{
		ReplaceXMLChars(buffer, strlen(buffer), buffer2);

		wsprintf(buf, "<LINE><ELAPSED>%d.%03d</ELAPSED><DATE>%02d/%02d/%02d</DATE><TIME>%02d:%02d:%02d</TIME>", 
					dwDiffTime/1000, dwDiffTime%1000, 
					time.wMonth, time.wDay,    time.wYear,
					time.wHour,  time.wMinute, time.wSecond);
		WriteFile(m_hFile,	buf,    lstrlen(buf),    &x, 0 );
		wsprintf(buf, "<MODULE>%s</MODULE><MESSAGE>", strMod);
		WriteFile(m_hFile,	buf,    lstrlen(buf),    &x, 0 );
		if(nLevel == DEBUG_ERROR)
			WriteFile(m_hFile,	"ERROR - ", 8, &x, 0 );
		WriteFile(m_hFile,	buffer2, lstrlen(buffer2), &x, 0 );
		WriteFile(m_hFile,	"</MESSAGE></LINE>", 17, &x, 0 );
	}
	else
	{
		wsprintf(buf, "%d.%03d\t%02d/%02d/%02d %02d:%02d:%02d\t%s\t\t", 
					dwDiffTime/1000, dwDiffTime%1000, 
					time.wMonth, time.wDay,    time.wYear,
					time.wHour,  time.wMinute, time.wSecond, strMod);

		// Write time stamp and formatted string to log-file
		WriteFile(m_hFile,	buf,    lstrlen(buf),    &x, 0 );
		if(nLevel == DEBUG_ERROR)
			WriteFile(m_hFile,	"ERROR - ", 8, &x, 0 );
		WriteFile(m_hFile,	buffer, lstrlen(buffer), &x, 0 );	
		WriteFile(m_hFile,	CrLf,   lstrlen(CrLf),   &x, 0 );	
	}

	FlushFileBuffers(m_hFile);

	LeaveCriticalSection(&m_hLock );
}

/*-----------------------------------------------------------------------
 *  Function    : ShowLastError
 *  Prototype   : void CDebug::ShowLastError() 
 *  Description : 
 *  Parameters  :                   
 *  Return      : void 
 *  History     : 
 */
void CDebug::ShowLastError() 
{
	EnterCriticalSection(&m_hLock );

	char* lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,    
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,    
		0,    
		NULL );

	int len = lstrlen(lpMsgBuf)-1;

	if (len>0 )
	{
		while (len>=0 && (lpMsgBuf[len]==0x0d || lpMsgBuf[len]==0x0a) )
			len--;
		lpMsgBuf[len+1] = 0; 
		CDebug::printf (DEBUG_ERROR, "CDebug::ShowLastError", "GetLastError(): %s", (char*)lpMsgBuf );
		LocalFree(lpMsgBuf ); // Free the buffer.
	}

	LeaveCriticalSection(&m_hLock );
}

/*-----------------------------------------------------------------------
 *  Function    : WriteHeader
 *  Prototype   : void CDebug::WriteHeader() 
 *  Description : 
 *  Parameters  :                   
 *  Return      : void 
 *  History     : 
 */
void CDebug::WriteHeader(BOOL bNewFile /* = TRUE */)
{
	DWORD x;
	char buffer[MAX_PATH];
	
	if(m_hFile == NULL)
		return;

	EnterCriticalSection(&m_hLock );

	if ( m_bUseXML && bNewFile )
	{
		strcpy(buffer, "<?xml:stylesheet type=\"text/xsl\" href=\"debug.xsl\"?><DEBUG>");
	}
	else
	{
		strcpy(buffer, "Elapsed\tDate/Time\t\tModule\t\t\t\tDebug\r\n");
		strcat(buffer, "=====================================================================================================\r\n");
	}

	SetFilePointer(m_hFile, 0, 0, FILE_END );
	
	if(!WriteFile(m_hFile,	buffer,    lstrlen(buffer),    &x, 0 ))
	{
		// Failed but there's not a lot I can do about it.
	}
	LeaveCriticalSection(&m_hLock );
}

/*-----------------------------------------------------------------------
 *  Function    : CheckFileSize
 *  Prototype   : void CDebug::CheckFileSize() 
 *  Description : 
 *  Parameters  :                   
 *  Return      : void 
 *  History     : 
 */
void CDebug::CheckFileSize()
{
	if( ( m_nMaxFileSize <= 0 ) || ( m_hFile == NULL ) )
		return;

	EnterCriticalSection(&m_hLock );

	DWORD dwStartTime = m_dwStartTime;
	long nSize = GetFileSize (m_hFile, NULL); 

	// Check if the file size is valid
	if (nSize != 0xFFFFFFFF)
	{
		if(nSize > m_nMaxFileSize)
		{
			// File size is exceeded, wipe file and start again
			CloseHandle(m_hFile);
			OpenLogFile(TRUE);
			WriteHeader();
		}
	}
	LeaveCriticalSection(&m_hLock );
}

/*-----------------------------------------------------------------------
 *  Function    : OpenLogFile
 *  Prototype   : BOOL CDebug::OpenLogFile() 
 *  Description : 
 *  Parameters  :                   
 *  Return      : BOOL
 *  History     : 
 */
BOOL CDebug::OpenLogFile(BOOL bOverWrite /* = TRUE */ )
{
	DWORD x = 0;

	EnterCriticalSection(&m_hLock );

	// Open the file if it exists and truncate to zero bytes
	HANDLE hFile = CreateFile(m_strFilePath, 
														GENERIC_WRITE, 
														FILE_SHARE_READ|FILE_SHARE_WRITE, 
														0, 
														(bOverWrite ? CREATE_ALWAYS : OPEN_ALWAYS), 
														FILE_ATTRIBUTE_NORMAL, 
														0 );
		
	// The file isn't there. Disable debugging
	if(hFile == NULL)
	{
		m_bEnabled = FALSE;
		return FALSE;
	}

	m_hFile = hFile;

	if(!bOverWrite && m_bUseXML)
	{
		// Clear the </DEBUG> tag from the end
		SetFilePointer(m_hFile, 8, 0, FILE_END );
		WriteFile(m_hFile,	"        ",    8,    &x, 0 );
	}

	LeaveCriticalSection(&m_hLock );

	return TRUE;
}

/*-----------------------------------------------------------------------
 *  Function    : ReplaceXMLChars
 *  Prototype   : void CDebug::ReplaceXMLChars(const char* strInput, int nLen, char* strOutput) 
 *  Description : Replace invalid characters in XML data
 *  Parameters  :                   
 *  Return      : void
 *  History     : 
 */
void CDebug::ReplaceXMLChars(const char* strInput, int nLen, char* strOutput)
{
	int nOutIndx = 0;
	char ch;

	EnterCriticalSection(&m_hLock );

	if(nLen <= 0)
		return;

	for(int i = 0; i < nLen; i++)
	{
		ch = strInput[i];

		if(ch == '&')
			strcat(strOutput, "&amp;");
		else
			strncat(strOutput, &ch, 1);
	}

	LeaveCriticalSection(&m_hLock );
}

#endif
