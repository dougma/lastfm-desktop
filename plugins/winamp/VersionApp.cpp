// VersionApp.cpp: Implementierung der Klasse CVersionApp.
//
//////////////////////////////////////////////////////////////////////
#include "VersionApp.h"
#include <winver.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CVersionApp::CVersionApp()
{
	Refresh();
}

CVersionApp::~CVersionApp()
{
}

void CVersionApp::Refresh(HINSTANCE hInstance /* = NULL */)
{
	if(hInstance != NULL)
		m_hInstance = hInstance;

	m_strOSVersion = "";
	
	ReadResourceInformation();
	ReadFileTime();
}

BOOL CVersionApp::ReadResourceInformation()
{
	DWORD	dwLength, dwSize, dwNullHandle;
	TCHAR	szFileName [MAX_PATH];
	LPVOID	lpVersionPtr;
	UINT	uiVerLength;
	
	
	dwSize = sizeof (szFileName) / sizeof (szFileName [0]);
	dwLength = ::GetModuleFileName (m_hInstance, szFileName, dwSize);
	if (dwLength <= 0) 
		return FALSE;

	m_VersionInfo.sFilePath=szFileName;
	dwLength = ::GetFileVersionInfoSize (szFileName, &dwNullHandle);
	if (dwLength <= 0) 
		return FALSE;

	BYTE* pVersionInfo = new BYTE [dwLength];
	if (!::GetFileVersionInfo (szFileName, NULL, dwLength, pVersionInfo)) 
		return FALSE;	
	if (!::VerQueryValue (pVersionInfo, _T ("\\"), &lpVersionPtr, &uiVerLength)) 
		return FALSE;

	m_VersionInfo.vsFixedFileInfo = *(VS_FIXEDFILEINFO*)lpVersionPtr;
	
	if (!GetDynamicInfo(pVersionInfo)) 
		return FALSE;	
	if (!GetFixedFileInfo ()) 
		return FALSE;
	
	delete pVersionInfo;	

	return TRUE;
}

BOOL CVersionApp::GetFixedFileInfo()
{
	m_VersionInfo.stMinorVersion.Format("%d%d",HIWORD (m_VersionInfo.vsFixedFileInfo.dwFileVersionLS), LOWORD (m_VersionInfo.vsFixedFileInfo.dwFileVersionLS));
	m_VersionInfo.stMajorVersion.Format("%d",LOWORD (m_VersionInfo.vsFixedFileInfo.dwFileVersionMS));
	m_VersionInfo.stDllVersion.Format ("%d.%d.%d.%d", 
		HIWORD (m_VersionInfo.vsFixedFileInfo.dwFileVersionMS), LOWORD (m_VersionInfo.vsFixedFileInfo.dwFileVersionMS),
		HIWORD (m_VersionInfo.vsFixedFileInfo.dwFileVersionLS), LOWORD (m_VersionInfo.vsFixedFileInfo.dwFileVersionLS));

	if (m_VersionInfo.vsFixedFileInfo.dwFileType == VFT_DRV)
	{
		switch (m_VersionInfo.vsFixedFileInfo.dwFileSubtype) 
		{
			case VFT2_DRV_DISPLAY:
				m_VersionInfo.stFileType = _T ("Display driver");
				break;
			case VFT2_DRV_INSTALLABLE:
				m_VersionInfo.stFileType = _T ("Installable driver");
				break;
			case VFT2_DRV_KEYBOARD:
				m_VersionInfo.stFileType = _T ("Keyboard driver");
				break;
			case VFT2_DRV_LANGUAGE:
				m_VersionInfo.stFileType = _T ("Language driver");
				break;
			case VFT2_DRV_MOUSE:
				m_VersionInfo.stFileType = _T ("Mouse driver");
				break;
			case VFT2_DRV_NETWORK:
				m_VersionInfo.stFileType = _T ("Network driver");
				break;
			case VFT2_DRV_PRINTER:
				m_VersionInfo.stFileType = _T ("Printer driver");
				break;
			case VFT2_DRV_SOUND:
				m_VersionInfo.stFileType = _T ("Sound driver");
				break;
			case VFT2_DRV_SYSTEM:
				m_VersionInfo.stFileType = _T ("System driver");
				break;
			case VFT2_UNKNOWN:
				m_VersionInfo.stFileType = _T ("Unknown driver");
				break;
		}
	}
	else if (m_VersionInfo.vsFixedFileInfo.dwFileType == VFT_FONT) 
	{
		switch (m_VersionInfo.vsFixedFileInfo.dwFileSubtype) 
		{
			case VFT2_FONT_RASTER:
				m_VersionInfo.stFileType = _T ("Raster font");
				break;
			case VFT2_FONT_TRUETYPE:
				m_VersionInfo.stFileType = _T ("Truetype font");
				break;
			case VFT2_FONT_VECTOR:
				m_VersionInfo.stFileType = _T ("Vector font");
				break;
			case VFT2_UNKNOWN:
				m_VersionInfo.stFileType = _T ("Unknown font");
				break;
		}
	}
	else if (m_VersionInfo.vsFixedFileInfo.dwFileType == VFT_APP) 
	{
		m_VersionInfo.stFileType = _T ("Application");
	}
	else if (m_VersionInfo.vsFixedFileInfo.dwFileType == VFT_DLL) 
	{
		m_VersionInfo.stFileType = _T ("Dynamic link library");
	}
	else if (m_VersionInfo.vsFixedFileInfo.dwFileType == VFT_STATIC_LIB) 
	{
		m_VersionInfo.stFileType = _T ("Static link library");
	}
	else if (m_VersionInfo.vsFixedFileInfo.dwFileType == VFT_VXD) 
	{
		m_VersionInfo.stFileType = _T ("Virtual device");
	}
	else if (m_VersionInfo.vsFixedFileInfo.dwFileType == VFT_UNKNOWN) 
	{
		m_VersionInfo.stFileType = _T ("Unknown type");
	}

	switch (m_VersionInfo.vsFixedFileInfo.dwFileOS) 
	{
		case VOS_DOS:
			m_VersionInfo.stFileOS = _T ("MS-DOS");
			break;
		case VOS_DOS_WINDOWS16:
			m_VersionInfo.stFileOS = _T ("16-bit windows running on MS-DOS");
			break;
		case VOS_DOS_WINDOWS32:
			m_VersionInfo.stFileOS = _T ("Win32 API running on MS-DOS");
			break;
		case VOS_OS216:
			m_VersionInfo.stFileOS = _T ("16-bit OS/2");
			break;
		case VOS_OS216_PM16:
			m_VersionInfo.stFileOS = _T ("16-bit Presentation manager running on 16-bit OS/2");
			break;
		case VOS_OS232:
			m_VersionInfo.stFileOS = _T ("32-bit OS/2");
			break;
		case VOS_NT:
			m_VersionInfo.stFileOS = _T ("Windows NT");
			break;
		case VOS_NT_WINDOWS32:
			m_VersionInfo.stFileOS = _T ("Win32 API on Windows NT");
			break;
		case VOS_UNKNOWN:
			m_VersionInfo.stFileOS = _T ("Unknown OS");
			break;
	}

	return (true);
}

BOOL CVersionApp::GetDynamicInfo(BYTE *pVersionInfo)
{
	UINT	uiVerLength;
	LPVOID	lpPtr;
	CStdString	sQuery;
		

	if (::VerQueryValue (pVersionInfo, "\\VarFileInfo\\Translation", &lpPtr, &uiVerLength))
		m_VersionInfo.Translation = *(VERSION_INFORMATION::TRANSLATE*)lpPtr;
	
	sQuery.Format (	_T ("\\StringFileInfo\\%04x%04x\\CompanyName"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stCompany=CStdString((LPCTSTR)lpPtr);
	
	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\LegalCopyRight"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stCopyRight=CStdString((LPCTSTR)lpPtr);

	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\ProductName"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stProductName=CStdString((LPCTSTR)lpPtr);

	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\ProductVersion"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stProductVersion=CStdString((LPCTSTR)lpPtr);

	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\OriginalFileName"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stOriginalName=CStdString((LPCTSTR)lpPtr);

	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\FileDescription"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stDescription=CStdString((LPCTSTR)lpPtr);
	
	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\FileVersion"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stFileVersion=CStdString((LPCTSTR)lpPtr);

	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\InternalName"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stInternalName=CStdString((LPCTSTR)lpPtr);

	sQuery.Format (_T ("\\StringFileInfo\\%04x%04x\\PrivateBuild"),m_VersionInfo.Translation.languageId, m_VersionInfo.Translation.characterSet);
	::VerQueryValue (pVersionInfo, (LPTSTR)(LPCTSTR)sQuery, (LPVOID*)&lpPtr,&uiVerLength);
	m_VersionInfo.stPrivateBuild=CStdString((LPCTSTR)lpPtr);

	return TRUE;
}

BOOL CVersionApp::ReadFileTime()
{
	FILE* pFile = NULL;

	if( (!m_VersionInfo.sFilePath.IsEmpty()) && 
		  (pFile = fopen(m_VersionInfo.sFilePath,"r")) != NULL)
	{
		BOOL bRet = GetFileTime(pFile,&m_CreationTime,&m_LastAccessTime,&m_LastWriteTime);
		fclose(pFile);
		return bRet;
	}
	return FALSE;
}

BOOL CVersionApp::RestoreAssociation(CStdString Ext, CStdString Name, int index)
{
	CStdString strRegAppPath, strAppPath = GetAppFilepath();
	CRegistry reg;
	CStdString Temp, Temp1;
	CStdString KeyName;

	// Remove Whitespaces
	for(int i = 0; i < Name.GetLength(); i++)
	{
		if(Name[i] != 0x20)
			KeyName += Name[i];
	}

	if(Ext.Left(1) != ".")
		Ext = "." + Ext;

	if(!reg.VerifyKey(HKEY_CLASSES_ROOT, Ext))  // Check to see if the key exists
	{
		if(reg.CreateKey(HKEY_CLASSES_ROOT, Ext))  // if it doesn’t then create it
		{
			reg.Open(HKEY_CLASSES_ROOT, Ext);  // Open the Registry
			reg.Write("", KeyName);
			reg.Close();
		}
		else
		{
			return FALSE;
		}
	}

	if(!reg.VerifyKey(HKEY_CLASSES_ROOT, KeyName))  // Check to see if the key exists
	{
		reg.CreateKey(HKEY_CLASSES_ROOT, KeyName);  // if it doesn’t then create it
		reg.Open(HKEY_CLASSES_ROOT, KeyName); 
		reg.Write("", Name);
		reg.Close();
		Temp = KeyName + "\\DefaultIcon";
		reg.CreateKey(HKEY_CLASSES_ROOT, Temp);  // if it doesn’t then create it
		reg.Open(HKEY_CLASSES_ROOT, Temp);  
		Temp.Format("%s,%d", strAppPath.c_str(), index);
		reg.Write("",Temp);
		reg.Close();
	}

	Temp = KeyName + "\\DefaultIcon";
	if(!reg.Open(HKEY_CLASSES_ROOT, Temp))
	{
		return FALSE;
	}

	reg.Read("", Temp);

	// Comapre the path without the comma
	int pos = Temp.Find(",");
	if(pos >= 0)
		strRegAppPath = Temp.Left(pos);

	if(strRegAppPath != strAppPath)
	{
		Temp1.Format("%s,%d", strAppPath.c_str(), index);
		reg.Write("", Temp1);
	}

	if(atoi(Temp.Right(1)) != index)
	{
		Temp1.Format("%s,%d", strAppPath.c_str(), index);
		reg.Write("", Temp1);
	}

	reg.Close();

	return TRUE;
}

CStdString CVersionApp::GetOSVersion()
{
	TCHAR sText[512];
  TCHAR sBuf[100];
  COSVersion os;
	COSVersion::OS_VERSION_INFO osvi;
  memset(&osvi, 0, sizeof(osvi));
    
  if (os.GetVersion(&osvi))
  {
    _stprintf(sText, _T(""));
    
    switch (osvi.UnderlyingPlatform)
    {
      case COSVersion::Dos:               
      {
        _tcscat(sText, _T("DOS"));                
        break;
      }
      case COSVersion::Windows3x:         
      {
        _tcscat(sText, _T("Windows"));        
        if (os.IsWin32sInstalled(&osvi))
          _tcscat(sText, _T(" (Win32s)"));
        break;
      }
      case COSVersion::WindowsCE:
      {
        _tcscat(sText, _T("Windows CE"));        
        break;
      }
      case COSVersion::Windows9x:
      {
        if (os.IsWindows95(&osvi))
          _stprintf(sBuf, _T("Windows 95"));
        else if (os.IsWindows95SP1(&osvi))
          _stprintf(sBuf, _T("Windows 95 SP1"));
        else if (os.IsWindows95OSR2(&osvi))
          _stprintf(sBuf, _T("Windows 95 OSR2"));
        else if (os.IsWindows98(&osvi))
          _stprintf(sBuf, _T("Windows 98"));
        else if (os.IsWindows98SP1(&osvi))
          _stprintf(sBuf, _T("Windows 98 SP1"));
        else if (os.IsWindows98SE(&osvi))
          _stprintf(sBuf, _T("Windows 98 Second Edition"));
        else if (os.IsWindowsME(&osvi))
          _stprintf(sBuf, _T("Windows Millenium Edition"));
        else
          _stprintf(sBuf, _T("Windows ??"));
        _tcscat(sText, sBuf);          
        break;
      }
      case COSVersion::WindowsNT:
      {
        if (os.IsNTPreWin2k(&osvi))
        {
          _tcscat(sText, _T("Windows NT"));          

          if (os.IsNTWorkstation(&osvi))
            _tcscat(sText, _T(" (Workstation)"));
          else if (os.IsNTStandAloneServer(&osvi))
            _tcscat(sText, _T(" (Server)"));
          else if (os.IsNTPDC(&osvi))
            _tcscat(sText, _T(" (Primary Domain Controller)"));
          else if (os.IsNTBDC(&osvi))
            _tcscat(sText, _T(" (Backup Domain Controller)"));

          if (os.IsNTDatacenterServer(&osvi))
            _tcscat(sText, _T(", (Datacenter)"));
          else if (os.IsNTEnterpriseServer(&osvi))
            _tcscat(sText, _T(", (Enterprise)"));
        }
        else if (os.IsWindows2000(&osvi))
        {
          _tcscat(sText, _T("Windows 2000"));          

          if (os.IsWin2000Professional(&osvi))
            _tcscat(sText, _T(" (Professional)"));
          else if (os.IsWin2000Server(&osvi))
            _tcscat(sText, _T(" (Server)"));
          else if (os.IsWin2000DomainController(&osvi))
            _tcscat(sText, _T(" (Domain Controller)"));

          if (os.IsWin2000DatacenterServer(&osvi))
            _tcscat(sText, _T(", (Datacenter)"));
          else if (os.IsWin2000AdvancedServer(&osvi))
            _tcscat(sText, _T(", (Advanced Server)"));
        }
        else if (os.IsWindowsXPOrWindowsServer2003(&osvi))
        {
          if (os.IsXPPersonal(&osvi))
            _tcscat(sText, _T("Windows XP (Personal)"));          
          else if (os.IsXPProfessional(&osvi))
            _tcscat(sText, _T("Windows XP (Professional)"));          
          else if (os.IsWindowsServer2003(&osvi))
            _tcscat(sText, _T("Windows Server 2003"));          
          else if (os.IsDomainControllerWindowsServer2003(&osvi))
            _tcscat(sText, _T("Windows Server 2003 (Domain Controller)"));          

          if (os.IsDatacenterWindowsServer2003(&osvi))
            _tcscat(sText, _T(", (Datacenter Edition)"));
          else if (os.IsEnterpriseWindowsServer2003(&osvi))
            _tcscat(sText, _T(", (Enterprise Edition)"));
          else if (os.IsWebWindowsServer2003(&osvi))
            _tcscat(sText, _T(", (Web Edition)"));
          else if (os.IsWindowsServer2003(&osvi))
            _tcscat(sText, _T(", (Standard Edition)"));
        }

/*        if (os.IsTerminalServicesInstalled(&osvi))
          _tcscat(sText, _T(", (Terminal Services)"));
        if (os.ISSmallBusinessEditionInstalled(&osvi))
          _tcscat(sText, _T(", (BackOffice Small Business Edition)"));
        if (os.IsEmbedded(&osvi))
          _tcscat(sText, _T(", (Embedded)"));
        if (os.IsTerminalServicesInRemoteAdminMode(&osvi))
          _tcscat(sText, _T(", (Terminal Services in Remote Admin Mode)"));
        if (os.IsEmulated64Bit(&osvi))
          _tcscat(sText, _T(", (64 Bit Edition)"));*/

        if (osvi.dwSuiteMask & OSSUITE_UNIPROCESSOR_FREE)
          _tcscat(sText, _T(", (Uniprocessor Free)"));
        else if (osvi.dwSuiteMask & OSSUITE_UNIPROCESSOR_CHECKED)
          _tcscat(sText, _T(", (Uniprocessor Checked)"));
        else if (osvi.dwSuiteMask & OSSUITE_MULTIPROCESSOR_FREE)
          _tcscat(sText, _T(", (Multiprocessor Free)"));
        else if (osvi.dwSuiteMask & OSSUITE_MULTIPROCESSOR_CHECKED)
          _tcscat(sText, _T(", (Multiprocessor Checked)"));

        break;
      }
      default: 
      {
        _stprintf(sBuf, _T("Unknown OS"));
        break;
      }
    }                     
    _stprintf(sBuf, _T(" v%d."), osvi.dwEmulatedMajorVersion);
    _tcscat(sText, sBuf);     
    if (osvi.dwEmulatedMinorVersion % 10)
    {
      if (osvi.dwEmulatedMinorVersion > 9)
        _stprintf(sBuf, _T("%02d"), osvi.dwEmulatedMinorVersion);
      else
        _stprintf(sBuf, _T("%01d"), osvi.dwEmulatedMinorVersion);
    }
    else
      _stprintf(sBuf, _T("%01d"), osvi.dwEmulatedMinorVersion / 10);
    _tcscat(sText, sBuf);                           
    if (osvi.dwEmulatedBuildNumber)
    {
      _stprintf(sBuf, _T(" Build:%d"), osvi.dwEmulatedBuildNumber);
      _tcscat(sText, sBuf);           
    }
    if (osvi.wEmulatedServicePackMajor)       
    {
      if (osvi.wEmulatedServicePackMinor)
      {
        //Handle the special case of NT 4 SP 6a which Dtwinver ver treats as SP 6.1
        if (os.IsNTPreWin2k(&osvi) && (osvi.wEmulatedServicePackMajor == 6) && (osvi.wEmulatedServicePackMinor == 1))
          _stprintf(sBuf, _T(" Service Pack: 6a"));
        //Handle the special case of XP SP 1a which Dtwinver ver treats as SP 1.1
        else if (os.IsWindowsXP(&osvi) && (osvi.wEmulatedServicePackMajor == 1) && (osvi.wEmulatedServicePackMinor == 1))
          _stprintf(sBuf, _T(" Service Pack: 1a"));
        else       
          _stprintf(sBuf, _T(" Service Pack:%d.%d"), osvi.wEmulatedServicePackMajor, osvi.wEmulatedServicePackMinor);
      }
      else
        _stprintf(sBuf, _T(" Service Pack:%d"), osvi.wEmulatedServicePackMajor);
      _tcscat(sText, sBuf);
    }                            
    else
    {
      if (osvi.wEmulatedServicePackMinor)       
        _stprintf(sBuf, _T(" Service Pack:0.%d"), osvi.wEmulatedServicePackMinor);
    }
	}
	else
		_tcscat(sText, "Failed in call to GetOSVersion");

	return CStdString(sText);
}

CStdString CVersionApp::GetWorkingDir()
{
	char filename[512];
	GetModuleFileName(m_hInstance,filename,sizeof(filename));
	CStdString Temp = filename;
	int pos = Temp.ReverseFind('\\');
	Temp = Temp.Left(pos + 1);

	return Temp;
}