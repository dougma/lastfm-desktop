// VersionApp.h: Schnittstelle für die Klasse CVersionApp.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VersionApp_H__910C010B_5EC2_11D3_9EB9_0000E87CD125__INCLUDED_)
#define AFX_VersionApp_H__910C010B_5EC2_11D3_9EB9_0000E87CD125__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdString.h"
#include "ComDATE.h"
#include "Registry.h"
#include "OSVer.h"

#pragma comment(lib,"version.lib") 

typedef struct tagVersionInformation
{
	CStdString sFilePath;
	CStdString	stCompany;
	CStdString	stCopyRight;
	CStdString	stDescription;
	CStdString	stFileVersion;
	CStdString stMajorVersion;
	CStdString stMinorVersion;
	CStdString	stInternalName;
	CStdString	stOriginalName;
	CStdString	stProductName;
	CStdString	stProductVersion;
	CStdString	stPrivateBuild;
	CStdString	stDllVersion;
	CStdString	stFileOS;
	CStdString	stFileType;
	CStdString stLanguageId;
	CStdString stCharSet;
	VS_FIXEDFILEINFO vsFixedFileInfo;
	struct TRANSLATE 
	{
		WORD languageId;
		WORD characterSet;
	} Translation;
}VERSION_INFORMATION, *LPVERSION_INFORMATION;

class CVersionApp  
{
public:	
	CVersionApp();

	void Refresh(HINSTANCE hInstance = NULL);

	CStdString GetAppVersion(){return m_VersionInfo.stFileVersion;}
	CStdString GetAppBuildDate(){return CComDATE(m_CreationTime).Format("%D/%m/%Y %H:%M:%S");}
	CStdString GetAppLastAccessDate(){return CComDATE(m_LastAccessTime).Format("%D/%m/%Y %H:%M:%S");}
	CStdString GetAppWriteDate(){return CComDATE(m_LastWriteTime).Format("%D/%m/%Y %H:%M:%S");}
	CStdString GetAppFilepath(){return m_VersionInfo.sFilePath;}
	CStdString GetAppCopyright(){return m_VersionInfo.stCopyRight;}
	CStdString GetAppOriginalname(){return m_VersionInfo.stOriginalName;}
	CStdString GetAppCompany(){return m_VersionInfo.stCompany;}
	CStdString	GetAppDescription(){return m_VersionInfo.stDescription;}
	CStdString GetAppInternalName(){return m_VersionInfo.stInternalName;}
	CStdString GetAppProductName(){return m_VersionInfo.stProductName;}
	CStdString GetAppProductVersion(){return m_VersionInfo.stProductVersion;}
	CStdString GetAppPrivateBuild(){return m_VersionInfo.stPrivateBuild;}
	CStdString GetAppMajorVersion(){return m_VersionInfo.stMajorVersion;}
	CStdString GetAppMinorVersion(){return m_VersionInfo.stMinorVersion;}

	CStdString GetOSVersion();
	CStdString GetWorkingDir();

	BOOL RestoreAssociation(CStdString Ext, CStdString Name, int index);

	virtual ~CVersionApp();

protected:
	BOOL ReadFileTime();
	BOOL GetFixedFileInfo();
	BOOL GetDynamicInfo(BYTE* pVersionInfo);
	BOOL ReadResourceInformation();
	VERSION_INFORMATION m_VersionInfo;
	FILETIME m_CreationTime,m_LastAccessTime,m_LastWriteTime;
	CStdString m_strOSVersion;
	HINSTANCE m_hInstance;
};

#endif // !defined(AFX_VersionApp_H__910C010B_5EC2_11D3_9EB9_0000E87CD125__INCLUDED_)
