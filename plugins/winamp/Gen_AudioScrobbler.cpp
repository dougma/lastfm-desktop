#include "stdafx.h"
#include "Resource.h"
#include "Gen_AudioScrobbler.h"
#include "WinampScrobbler.h"
#include "wa_hotkeys.h"

#define WA_DLG_IMPLEMENT
#include "wa_dlg.h"

CComModule	_Module;
CWinampScrobbler theScrobbler;
int nIdTimer = 0;

// Semaphore stuff
static char szSemName[] = "AudioscrobblerSemaphore";
HANDLE hSem;

// Status Window Stuff
HWND g_hwndParent = NULL;
HINSTANCE g_hDllInstance = NULL; 
HWND wa2 = 0;
#ifdef STRICT
#define PROC WNDPROC
#else
#define PROC FARPROC
#endif
PROC frameProc = 0;
PROC winampProc = 0;
#define AS_MENU_TEXT			AUDIOSCROBBLER_APPNAME "\tShift+H"
#define STATUS_TIMER			1
#define STATUS_TIMER_INTERVAL	1000
#define AS_MENU_ID				24767

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


// Global Functions
void config();
void quit();
int init();
int mlinit();

void config()
{
}

void quit()
{
	try
	{
        // Before shutdown remove semaphore
		ReleaseSemaphore (hSem, 1, NULL);
		CloseHandle (hSem);

		theScrobbler.StopScrobbling();
	}
	catch (std::exception& e)
	{
		PRINTF(DEBUG_ERROR, "quit()", e.what());
	}

	_Module.Term();
}

// Gets the playlist window
HWND GetPEWindow(HWND hwndWinamp)
{
	HWND hWnd = NULL;

	// First try Winamp internal handle
	if ((hWnd = (HWND)SendMessage(hwndWinamp,WM_WA_IPC,IPC_GETWND_PE,IPC_GETWND)) == NULL)
		if((hWnd = (HWND)FindWindow("Winamp PE", NULL)) == NULL)
			hWnd = NULL;

	return hWnd;
}

LRESULT (CALLBACK *lpWndProcOldPE)(HWND, UINT, WPARAM, LPARAM) = NULL;
LRESULT (CALLBACK *lpWndProcOldParent)(HWND, UINT, WPARAM, LPARAM) = NULL;

// Subclassed PE window procedure, calls the default lpWndProcOldPE if we're not
// handling the message
LRESULT CALLBACK WndProcPE(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(theScrobbler.WinampPEMessage(message, wParam, lParam))
	{
		return CallWindowProc(lpWndProcOldPE, hwnd, message, wParam, lParam);
	}
	
	return 0L;
}

// Subclassed main window procedure, calls the default lpWndProcOldParent if we're not
// handling the message
LRESULT CALLBACK WndProcParent(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if( theScrobbler.WinampMessage( message, wParam, lParam ) )
    {
        return CallWindowProc(lpWndProcOldParent, hwnd, message, wParam, lParam);
    }

    return 0L;
}


// Helper method for subclassing Window procedure. If we're not using the correct A/W
// versions, Unicode tracks won't display properly in the taskbar.
WNDPROC SetWndSubClass( HWND hwnd, WNDPROC newproc, WNDPROC* oldproc )
{
    if ( IsWindowUnicode(hwnd) )
    {
        *oldproc = (WNDPROC)GetWindowLongPtrW( hwnd, GWL_WNDPROC );
        return (WNDPROC)SetWindowLongPtrW( hwnd, GWL_WNDPROC, (LONG_PTR)newproc );
    }
    else
    {
        *oldproc = (WNDPROC)GetWindowLongPtrA( hwnd, GWL_WNDPROC );
        return (WNDPROC)SetWindowLongPtrA( hwnd, GWL_WNDPROC, (LONG_PTR)newproc );
    }
}

/*****************************************************
******************************************************
***
*** G E N E R A L   P L U G I N
***
******************************************************
*****************************************************/
winampGeneralPurposePlugin plugin =
{
	GPPHDR_VER,
	"",
	init,
	config,
	quit,
};

// Entry point for a general plugin
extern "C" __declspec( dllexport ) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin()
{
	return &plugin;
}

// Winamp calls this with plugin populated after the above has been called
int init()
{
	static char c[512];
	char filename[512];
	HWND hWnd = NULL;

	g_hDllInstance = plugin.hDllInstance;
	g_hwndParent = plugin.hwndParent;

	_Module.Init(NULL, plugin.hDllInstance, &LIBID_ATLLib);

	// Handle Multiple Instances
	hSem = CreateSemaphore (NULL, 1, 1,szSemName);
    if (WaitForSingleObject (hSem, 0) == WAIT_TIMEOUT)
	{
		CloseHandle (hSem);
		return -1;
	}

    int version = WINAMP_VERSION_MAJOR( SendMessage( plugin.hwndParent,WM_WA_IPC,0,IPC_GETVERSION) );

    // If the version is earlier than 5, we need to use a method of track play detection
    // that involves overriding the playlist editor's event handler.
    if ( version < 5 )
    {
        hWnd = GetPEWindow(plugin.hwndParent);
        if ( hWnd != NULL )
	    {
            // Stores previous playlist wndproc and sets new one to WndProcPE
		    SetWndSubClass( hWnd, WndProcPE, &lpWndProcOldPE );
   	    }
  	    else
    		PRINTF(DEBUG_ERROR, "Gen_AudioScrobbler::init", "Failed to find Winamp PE Window");
	}

    // From version 5, Winamp supplies a better way to detect track plays that goes
    // through the main window's event handler. From 5.5 the old PE method does
    // not even work any longer.
    if ( version >= 5 && plugin.hwndParent != NULL )
    {
        // Stores previous playlist wndproc and sets new one to WndProcParent
        SetWndSubClass( plugin.hwndParent, WndProcParent, &lpWndProcOldParent );
    }

	GetModuleFileName(plugin.hDllInstance,filename,sizeof(filename));

    // Launch config if we can't start scrobbling
	if(!theScrobbler.StartScrobbling(plugin.hDllInstance, 
									plugin.hwndParent, 
									NULL,
									filename))
		config();
	
	sprintf((plugin.description=c), "%s v%s", theScrobbler.GetAppName().c_str(), theScrobbler.GetVersionString().c_str());

	return 0;
}


/*****************************************************
******************************************************
***
*** M E D I A   L I B R A R Y   P L U G I N
***
******************************************************
*****************************************************/
int msgproc(int message_type, int param1, int param2, int param3)
{
	BOOL bRet = FALSE;

	switch(message_type)
	{
	case ML_MSG_CONFIG:
		config();
		bRet = TRUE;
		break;
	default:;
	}

	return bRet;
}

winampMediaLibraryPlugin mlplugin = 
{
	MLHDR_VER,
	"",
	mlinit,
	quit,
	msgproc,
};

extern "C" {

// Entry point for a media library plugin
__declspec( dllexport ) winampMediaLibraryPlugin * winampGetMediaLibraryPlugin()
{
	return &mlplugin;
}

};

// Winamp calls this with mlplugin populated after the above has been called
int mlinit()
{
	static char c[512];
	char filename[512];
	HWND hWnd = NULL;

	g_hDllInstance = mlplugin.hDllInstance;
	g_hwndParent = mlplugin.hwndWinampParent;

	_Module.Init(NULL, mlplugin.hDllInstance, &LIBID_ATLLib);

	// Handle Multiple Instances
	hSem = CreateSemaphore (NULL, 1, 1,szSemName);
	if (WaitForSingleObject (hSem, 0) == WAIT_TIMEOUT) 
	{
		CloseHandle (hSem);
		return -1;
	}

    int version = WINAMP_VERSION_MAJOR( SendMessage( mlplugin.hwndWinampParent,WM_WA_IPC,0,IPC_GETVERSION) );

    // If the version is earlier than 5, we need to use a method of track play detection
    // that involves overriding the playlist editor's event handler.
    if ( version < 5 )
    {
        hWnd = GetPEWindow(mlplugin.hwndWinampParent);
        if ( hWnd != NULL )
	    {
            SetWndSubClass( hWnd, WndProcPE, &lpWndProcOldPE );
   	    }
  	    else
    		PRINTF(DEBUG_ERROR, "Gen_AudioScrobbler::mlinit", "Failed to find Winamp PE Window");
	}

    // From version 5, Winamp supplies a better way to detect track plays that goes
    // through the main window's event handler. From 5.5 the old PE method does
    // not even work any longer.
    if ( version >= 5 && mlplugin.hwndWinampParent != NULL )
    {
        SetWndSubClass( mlplugin.hwndWinampParent, WndProcParent, &lpWndProcOldParent );
    }

	if(GetModuleFileName(mlplugin.hDllInstance,filename,sizeof(filename)) == 0)
	{
		CStdString strText;
		strText.Format("An error occurred loading the AS plugin during GetModuleFileName(), the return code was %d", GetLastError());

		::MessageBox(mlplugin.hwndWinampParent, strText.c_str(), "AudioScrobbler Error", MB_OK);
		return -1;
	}

	if(!theScrobbler.StartScrobbling(mlplugin.hDllInstance, 
		mlplugin.hwndWinampParent, 
		mlplugin.hwndLibraryParent,
		filename))
		config();

	sprintf((mlplugin.description=c), "%s v%s (ML Mode)", theScrobbler.GetAppName().c_str(), theScrobbler.GetVersionString().c_str());

	return 0;
}
