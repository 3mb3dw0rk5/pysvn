# Microsoft Developer Studio Project File - Name="pysvn" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=pysvn - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pysvn.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pysvn.mak" CFG="pysvn - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pysvn - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "pysvn - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pysvn - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "pyds"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zm200 /MD /W3 /GR /GX /Zi /Od /I "$(PY)\include" /I "." /I "$(SVNCPP)" /I "$(PYCXX)\CXX" /I "$(SUBVERSION)/apr/include" /I "$(SUBVERSION)/apr-util/include" /I "$(SUBVERSION)/apr-util/xml/expat/lib" /I "$(SUBVERSION)/subversion/include" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "noPY_WIN32_DELAYLOAD_PYTHON_DLL" /FR /YX /FD /c /Tp
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib Mswsock.lib svncpp.lib $(SUBVERSION)\Debug\subversion\libsvn_client\libsvn_client-1.lib $(SUBVERSION)\Debug\subversion\libsvn_delta\libsvn_delta-1.lib $(SUBVERSION)\Debug\subversion\libsvn_diff\libsvn_diff-1.lib $(SUBVERSION)\Debug\subversion\libsvn_fs\libsvn_fs-1.lib $(SUBVERSION)\Debug\subversion\libsvn_ra\libsvn_ra-1.lib $(SUBVERSION)\Debug\subversion\libsvn_ra_dav\libsvn_ra_dav-1.lib $(SUBVERSION)\Debug\subversion\libsvn_ra_local\libsvn_ra_local-1.lib $(SUBVERSION)\Debug\subversion\libsvn_ra_svn\libsvn_ra_svn-1.lib $(SUBVERSION)\Debug\subversion\libsvn_repos\libsvn_repos-1.lib $(SUBVERSION)\Debug\subversion\libsvn_subr\libsvn_subr-1.lib $(SUBVERSION)\Debug\subversion\libsvn_wc\libsvn_wc-1.lib $(SUBVERSION)\apr-iconv\LibD\apriconv.lib $(SUBVERSION)\apr-util\LibD\aprutil.lib $(SUBVERSION)\apr-util\xml\expat\lib\LibD\xml.lib $(SUBVERSION)\apr\LibD\apr.lib $(SUBVERSION)\db4-win32\lib\libdb42.lib $(SUBVERSION)\neon\libneonD.lib ws2_32.lib $(PYLIB).lib /nologo /subsystem:windows /dll /pdb:"sept" /debug /machine:I386 /out:"pyds\pysvn.pyd" /libpath:"$(PY)\libs" /libpath:"$(SVNCPP)\Debug" /export:initpysvn
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "pysvn - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "pyds"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zm200 /MD /W3 /GR /GX /Zi /Od /Ob2 /I "$(PY)\include" /I "." /I "$(SVNCPP)" /I "$(PYCXX)\CXX" /I "$(SUBVERSION)/apr/include" /I "$(SUBVERSION)/apr-util/include" /I "$(SUBVERSION)/apr-util/xml/expat/lib" /I "$(SUBVERSION)/subversion/include" /D "WIN32" /D "_WINDOWS" /D "NDEBUG" /D "noPY_WIN32_DELAYLOAD_PYTHON_DLL" /FR /YX /FD /c /Tp
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Rpcrt4.lib Mswsock.lib svncpp.lib $(SUBVERSION)\Release\subversion\libsvn_client\libsvn_client-1.lib $(SUBVERSION)\Release\subversion\libsvn_delta\libsvn_delta-1.lib $(SUBVERSION)\Release\subversion\libsvn_diff\libsvn_diff-1.lib $(SUBVERSION)\Release\subversion\libsvn_fs\libsvn_fs-1.lib $(SUBVERSION)\Release\subversion\libsvn_ra\libsvn_ra-1.lib $(SUBVERSION)\Release\subversion\libsvn_ra_dav\libsvn_ra_dav-1.lib $(SUBVERSION)\Release\subversion\libsvn_ra_local\libsvn_ra_local-1.lib $(SUBVERSION)\Release\subversion\libsvn_ra_svn\libsvn_ra_svn-1.lib $(SUBVERSION)\Release\subversion\libsvn_repos\libsvn_repos-1.lib $(SUBVERSION)\Release\subversion\libsvn_subr\libsvn_subr-1.lib $(SUBVERSION)\Release\subversion\libsvn_wc\libsvn_wc-1.lib $(SUBVERSION)\apr-iconv\LibR\apriconv.lib $(SUBVERSION)\apr-util\LibR\aprutil.lib $(SUBVERSION)\apr-util\xml\expat\lib\LibR\xml.lib $(SUBVERSION)\apr\LibR\apr.lib $(SUBVERSION)\db4-win32\lib\libdb42.lib $(SUBVERSION)\neon\libneon.lib ws2_32.lib $(PYLIB).lib /nologo /subsystem:windows /dll /pdb:"sept" /debug /machine:I386 /out:"pyds\pysvn.pyd" /libpath:"$(PY)\libs" /libpath:"$(SVNCPP)\Release" /export:initpysvn

!ENDIF 

# Begin Target

# Name "pysvn - Win32 Debug"
# Name "pysvn - Win32 Release"
# Begin Source File

SOURCE="$(PYCXX)\CXX\Src\cxx_extensions.cxx"
# End Source File
# Begin Source File

SOURCE="$(PYCXX)\CXX\Src\cxxextensions.c"
# End Source File
# Begin Source File

SOURCE="$(PYCXX)\CXX\Src\cxxsupport.cxx"
# End Source File
# Begin Source File

SOURCE="$(PYCXX)\CXX\Src\IndirectPythonInterface.cxx"
# End Source File
# Begin Source File

SOURCE=pysvn.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_client.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_docs.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_revision.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_status.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_enum_string.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_callbacks.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_entry.cpp
# End Source File
# Begin Source File

SOURCE=pysvn_path.cpp
# End Source File
# End Target
# End Project
