# Microsoft Developer Studio Project File - Name="CDX" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CDX - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cdx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cdx.mak" CFG="CDX - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CDX - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CDX - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CDX - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Shared" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "xTRACE_TIMER_DETAILS" /D "CDX_LOG" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\cdx.lib ..\Shared\cdx.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CDX - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /GX /Zi /Od /Gy /I "..\shared" /D "_WIN32" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "CDX_LOG" /Fr /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"debug\cdxd.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\cdxd.lib ..\Shared\cdxd.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CDX - Win32 Release"
# Name "CDX - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cdxfile.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximage.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximagebase.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximagebmp.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximagejpg.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximagepcx.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximagepng.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximagepsd.cpp
# End Source File
# Begin Source File

SOURCE=.\cdximagetga.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxinput.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxiso.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxlog.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxmap.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxmapcell.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxmusic.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxmusiccd.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxresource.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxscreen.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxsound.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxsoundbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxsprite.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxspritelist.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxsurface.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxtile.cpp
# End Source File
# Begin Source File

SOURCE=.\ddutils.cpp
# End Source File
# Begin Source File

SOURCE=.\dxerrors.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cdx.h
# End Source File
# Begin Source File

SOURCE=.\cdxclist.h
# End Source File
# Begin Source File

SOURCE=.\cdxfile.h
# End Source File
# Begin Source File

SOURCE=.\cdximage.h
# End Source File
# Begin Source File

SOURCE=.\cdximagebase.h
# End Source File
# Begin Source File

SOURCE=.\cdximagebmp.h
# End Source File
# Begin Source File

SOURCE=.\cdximagejpg.h
# End Source File
# Begin Source File

SOURCE=.\cdximagepcx.h
# End Source File
# Begin Source File

SOURCE=.\cdximagepng.h
# End Source File
# Begin Source File

SOURCE=.\cdximagepsd.h
# End Source File
# Begin Source File

SOURCE=.\cdximagetga.h
# End Source File
# Begin Source File

SOURCE=.\cdxinput.h
# End Source File
# Begin Source File

SOURCE=.\cdxlayer.h
# End Source File
# Begin Source File

SOURCE=.\cdxlink.h
# End Source File
# Begin Source File

SOURCE=.\cdxlog.h
# End Source File
# Begin Source File

SOURCE=.\cdxmap.h
# End Source File
# Begin Source File

SOURCE=.\cdxmidi.h
# End Source File
# Begin Source File

SOURCE=.\cdxmusic.h
# End Source File
# Begin Source File

SOURCE=.\cdxresource.h
# End Source File
# Begin Source File

SOURCE=.\cdxscreen.h
# End Source File
# Begin Source File

SOURCE=.\cdxsound.h
# End Source File
# Begin Source File

SOURCE=.\cdxsprite.h
# End Source File
# Begin Source File

SOURCE=.\cdxstructs.h
# End Source File
# Begin Source File

SOURCE=.\cdxsurface.h
# End Source File
# Begin Source File

SOURCE=.\cdxtile.h
# End Source File
# Begin Source File

SOURCE=.\cdxtypes.h
# End Source File
# End Group
# End Target
# End Project
