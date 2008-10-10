/*
Module : DTWINVER.H
Purpose: Interface of a function to perform
         version detection on OS
Created: PJN / 11-05-1996


Copyright (c) 1997 - 2003 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/

#ifndef __DTWINVER_H__                                          


////////////////////////////////// defines ////////////////////////////////////

#define OSSUITE_SMALLBUSINESS             0x00000001
#define OSSUITE_ENTERPRISE                0x00000002
#define OSSUITE_PRIMARY_DOMAIN_CONTROLLER 0x00000004
#define OSSUITE_BACKUP_DOMAIN_CONTROLLER  0x00000008
#define OSSUITE_TERMINAL                  0x00000010
#define OSSUITE_DATACENTER                0x00000020
#define OSSUITE_PERSONAL                  0x00000040
#define OSSUITE_WEBEDITION                0x00000080
#define OSSUITE_EMBEDDEDNT                0x00000100
#define OSSUITE_REMOTEADMINMODE_TERMINAL  0x00000200
#define OSSUITE_UNIPROCESSOR_FREE         0x00000400
#define OSSUITE_UNIPROCESSOR_CHECKED      0x00000800
#define OSSUITE_MULTIPROCESSOR_FREE       0x00001000
#define OSSUITE_MULTIPROCESSOR_CHECKED    0x00002000



////////////////////////////////// Classes ////////////////////////////////////

class COSVersion
{
public:
//Enums
  enum OS_PLATFORM
  {
    Dos = 0,
    Windows3x = 1,
    Windows9x = 2,
    WindowsNT = 3,
    WindowsCE = 4,
  };

  enum OS_TYPE
  {
    Workstation = 0,
    Server = 1,
    DomainController = 2,
  };

//defines
  typedef struct _OS_VERSION_INFO
  {
  #ifndef UNDER_CE
    //What version of OS is being emulated
    DWORD dwEmulatedMajorVersion;
    DWORD dwEmulatedMinorVersion;
    DWORD dwEmulatedBuildNumber;
    OS_PLATFORM EmulatedPlatform;
  #ifdef _WIN32                    
    TCHAR szEmulatedCSDVersion[128];
  #else
    char szEmulatedCSDVersion[128];
  #endif  
    WORD wEmulatedServicePackMajor;
    WORD wEmulatedServicePackMinor;
  #endif

    //What version of OS is really running                 
    DWORD dwUnderlyingMajorVersion;
    DWORD dwUnderlyingMinorVersion;
    DWORD dwUnderlyingBuildNumber;
    OS_PLATFORM UnderlyingPlatform;   
  #ifdef _WIN32                      
    TCHAR szUnderlyingCSDVersion[128];
  #else  
    char szUnderlyingCSDVersion[128];
  #endif  
    WORD wUnderlyingServicePackMajor;
    WORD wUnderlyingServicePackMinor;
    DWORD dwSuiteMask;
    OS_TYPE OSType;

  #ifdef UNDER_CE
    TCHAR szOEMInfo[256];
    TCHAR szPlatformType[256];
  #endif
  } OS_VERSION_INFO, *POS_VERSION_INFO, FAR *LPOS_VERSION_INFO;

//Constructors / Destructors
  COSVersion(); 
  ~COSVersion();

//Methods:
  BOOL GetVersion(LPOS_VERSION_INFO lpVersionInformation);
                                 
//Please note that the return values for all the following functions 
//are mutually exclusive for example if you are running on 
//95 OSR2 then IsWindows95 will return FALSE
  BOOL IsWindows30(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows31(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows311(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsForWorkgroups(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsCE(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows95(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows95SP1(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows95OSR2(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows98(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows98SP1(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows98SE(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsME(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT31(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT35(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT351(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsNT4(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindows2000(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsXPOrWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);

//Returns the various flavours of the "os" that is installed. Note that these
//functions are not completely mutually exlusive
  BOOL IsWin32sInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTPreWin2k(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsNTWorkstation(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTStandAloneServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTPDC(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTBDC(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTEnterpriseServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsNTDatacenterServer(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsWin2000Professional(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWin2000Server(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWin2000AdvancedServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWin2000DatacenterServer(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWin2000DomainController(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsXPPersonal(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsXPProfessional(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsXP(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWebWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEnterpriseWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDatacenterWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsDomainControllerWindowsServer2003(LPOS_VERSION_INFO lpVersionInformation);

  BOOL IsTerminalServicesInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL ISSmallBusinessEditionInstalled(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEmulated64Bit(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsUnderlying64Bit(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsEmbedded(LPOS_VERSION_INFO lpVersionInformation);
  BOOL IsTerminalServicesInRemoteAdminMode(LPOS_VERSION_INFO lpVersionInformation);

protected:
//Defines / typedefs
#if (defined(_WINDOWS) || defined(_DOS)) && (!defined(_WIN32) && !defined(_WIN64))  
  #define CPEX_DEST_STDCALL        0x00000000L
  #define HKEY32                   DWORD                                                               
  #define HKEY_LOCAL_MACHINE       (( HKEY32 ) 0x80000002 )    
  #define TCHAR                    char
  #define _T
  #define _tcsicmp                 strcmpi
  #define _tcscpy                  strcpy
  #define _tcslen                  strlen
  #define _istdigit                isdigit
  #define _ttoi                    atoi
  #define _tcsupr                  strupr
  #define _tcsstr                  strstr
  #define LPCTSTR                  LPCSTR
  #define ERROR_CALL_NOT_IMPLEMENTED  120
  #define REG_DWORD                ( 4 )                                                               
  #define REG_MULTI_SZ             ( 7 )
  #define VER_PLATFORM_WIN32s             0
  #define VER_PLATFORM_WIN32_WINDOWS      1
  #define VER_PLATFORM_WIN32_NT           2
  #define VER_PLATFORM_WIN32_CE           3
#endif                      
                      
#if defined(_WINDOWS) && !defined(_WIN32) && !defined(_WIN64)  
//Defines / Macros
  #define LPTSTR LPSTR

  typedef struct OSVERSIONINFO
  { 
    DWORD dwOSVersionInfoSize; 
    DWORD dwMajorVersion; 
    DWORD dwMinorVersion; 
    DWORD dwBuildNumber; 
    DWORD dwPlatformId; 
    char szCSDVersion[128]; 
  } OSVERSIONINFO, *POSVERSIONINFO, FAR *LPOSVERSIONINFO; 
  
//Methods
  DWORD GetVersion();
  BOOL GetVersionEx(LPOSVERSIONINFO lpVersionInfo);
  LONG RegQueryValueEx(HKEY32 hKey, LPSTR  lpszValueName, LPDWORD lpdwReserved, LPDWORD lpdwType, LPBYTE  lpbData, LPDWORD lpcbData);
  static BOOL WFWLoaded();                                     

//Function Prototypes
  typedef DWORD (FAR PASCAL  *lpfnLoadLibraryEx32W) (LPCSTR, DWORD, DWORD);
  typedef BOOL  (FAR PASCAL  *lpfnFreeLibrary32W)   (DWORD);
  typedef DWORD (FAR PASCAL  *lpfnGetProcAddress32W)(DWORD, LPCSTR);
  typedef DWORD (FAR __cdecl *lpfnCallProcEx32W)    (DWORD, DWORD, DWORD, DWORD, ...);
  typedef WORD  (FAR PASCAL  *lpfnWNetGetCaps)      (WORD);

//Member variables
  lpfnLoadLibraryEx32W  m_lpfnLoadLibraryEx32W;
  lpfnFreeLibrary32W    m_lpfnFreeLibrary32W;
  lpfnGetProcAddress32W m_lpfnGetProcAddress32W;
  lpfnCallProcEx32W     m_lpfnCallProcEx32W;
  DWORD                 m_hAdvApi32;    
  DWORD                 m_hKernel32;
  DWORD                 m_lpfnRegQueryValueExA;
  DWORD                 m_lpfnGetVersionExA;
  DWORD                 m_lpfnGetVersion;
#endif //defined(_WINDOWS) && (!defined(_WIN32) && !defined(_WIN64))

#ifdef _WIN32
//Function Prototypes
  typedef BOOL (WINAPI *lpfnIsWow64Process)(HANDLE, PBOOL);  
#endif

#if defined(_WIN32) || defined(_WINDOWS)
//Defines / Macros
  typedef struct OSVERSIONINFOEX 
  {  
    DWORD dwOSVersionInfoSize;  
    DWORD dwMajorVersion;  
    DWORD dwMinorVersion;  
    DWORD dwBuildNumber;  
    DWORD dwPlatformId;  
    TCHAR szCSDVersion[128];  
    WORD wServicePackMajor;  
    WORD wServicePackMinor;  
    WORD wSuiteMask;  
    BYTE wProductType;  
    BYTE wReserved;
  } OSVERSIONINFOEX, *POSVERSIONINFOEX, *LPOSVERSIONINFOEX;
                  
//Function Prototypes                  
  typedef BOOL (WINAPI *lpfnGetVersionEx) (LPOSVERSIONINFO);  
#endif  

#ifndef _DOS         
//Methods
  void GetNTOSTypeFromRegistry(LPOS_VERSION_INFO lpVersionInformation, BOOL bOnlyUpdateDCDetails);
  void GetProductSuiteDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation) ;
  void GetTerminalServicesRemoteAdminModeDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation);
  void GetNTSP6aDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation, BOOL bUpdateEmulatedAlso);
  void GetXPSP1aDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation, BOOL bUpdateEmulatedAlso);
  void GetNTHALDetailsFromRegistry(LPOS_VERSION_INFO lpVersionInformation);
#else
  BOOL GetInfoBySpawingWriteVer(COSVersion::LPOS_VERSION_INFO lpVersionInformation);
  void GetWinInfo();
#endif

#if defined(_WIN32) || defined(_WIN64)
  WORD GetNTServicePackFromRegistry();
#endif    
  WORD GetNTServicePackFromCSDString(LPCTSTR pszCSDVersion);
};

#endif //__DTWINVER_H__