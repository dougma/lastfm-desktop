# Microsoft Developer Studio Project File - Name="Gen_AudioScrobbler" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Gen_AudioScrobbler - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Gen_AudioScrobbler.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gen_AudioScrobbler.mak" CFG="Gen_AudioScrobbler - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Gen_AudioScrobbler - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Gen_AudioScrobbler - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Gen_AudioScrobbler", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Gen_AudioScrobbler - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GEN_AUDIOSCROBBLER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../Common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GEN_AUDIOSCROBBLER_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib RASAPI32.LIB ws2_32.lib wininet.lib /nologo /dll /pdb:none /machine:I386 /out:"Release/ml_audioscrobbler.dll"
# SUBTRACT LINK32 /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\Release\ml_audioscrobbler.dll c:\progra~1\Winamp\Plugins
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Gen_AudioScrobbler - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GEN_AUDIOSCROBBLER_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GEN_AUDIOSCROBBLER_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib RASAPI32.LIB ws2_32.lib wininet.lib /nologo /dll /debug /machine:I386 /out:"Debug/ml_AudioScrobbler.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy        .\Debug\ml_audioscrobbler.dll        c:\progra~1\Winamp\Plugins\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Gen_AudioScrobbler - Win32 Release"
# Name "Gen_AudioScrobbler - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Gen_AudioScrobbler.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=.\Gen_AudioScrobbler.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Gen_AudioScrobbler.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Winamp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GEN.H
# End Source File
# Begin Source File

SOURCE=.\ml.h
# End Source File
# Begin Source File

SOURCE=.\wa_dlg.h
# End Source File
# Begin Source File

SOURCE=.\wa_ipc.h
# End Source File
# Begin Source File

SOURCE=.\winampcmd.h
# End Source File
# Begin Source File

SOURCE=.\WinampController.cpp
# End Source File
# Begin Source File

SOURCE=.\WinampController.h
# End Source File
# Begin Source File

SOURCE=.\WinampScrobbler.cpp
# End Source File
# Begin Source File

SOURCE=.\WinampScrobbler.h
# End Source File
# End Group
# Begin Group "Scrobbler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CacheManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CacheManager.h
# End Source File
# Begin Source File

SOURCE=.\ConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ConfigDlg.h
# End Source File
# Begin Source File

SOURCE=.\IncExcDirDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IncExcDirDlg.h
# End Source File
# Begin Source File

SOURCE=.\Scrobbler.cpp
# End Source File
# Begin Source File

SOURCE=.\Scrobbler.h
# End Source File
# Begin Source File

SOURCE=.\Submission.cpp
# End Source File
# Begin Source File

SOURCE=.\Submission.h
# End Source File
# Begin Source File

SOURCE=.\SubmissionLog.cpp
# End Source File
# Begin Source File

SOURCE=.\SubmissionLog.h
# End Source File
# End Group
# Begin Group "MD5"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MD5.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MD5.h
# End Source File
# Begin Source File

SOURCE=.\MD5Class.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\MD5Class.h
# End Source File
# Begin Source File

SOURCE=.\MD5Global.h
# End Source File
# End Group
# Begin Group "Library"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\comdate.h
# End Source File
# Begin Source File

SOURCE=.\dbg.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Dbg.h
# End Source File
# Begin Source File

SOURCE=.\DirDialog.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\DirDialog.h
# End Source File
# Begin Source File

SOURCE=.\OSVer.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\OSVer.h
# End Source File
# Begin Source File

SOURCE=.\Registry.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Registry.h
# End Source File
# Begin Source File

SOURCE=.\StdString.h
# End Source File
# Begin Source File

SOURCE=.\StdStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\StdStringArray.h
# End Source File
# Begin Source File

SOURCE=.\VersionApp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\VersionApp.h
# End Source File
# Begin Source File

SOURCE=.\WatchConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\WatchConnection.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\audioscrobbler.html
# End Source File
# Begin Source File

SOURCE=.\AudioScrobbler.nsi
# End Source File
# Begin Source File

SOURCE=.\Winamp_WhatsNew.txt
# End Source File
# End Target
# End Project
