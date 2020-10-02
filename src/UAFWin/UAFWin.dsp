# Microsoft Developer Studio Project File - Name="UAFWin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=UAFWin - Win32 Debug PS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UAFWin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UAFWin.mak" CFG="UAFWin - Win32 Debug PS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UAFWin - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "UAFWin - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "UAFWin - Win32 Debug RT" (based on "Win32 (x86) Application")
!MESSAGE "UAFWin - Win32 Debug PS" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UAFWin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /O1 /I "..\shared" /I "..\cdx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEngine" /D "TASKTRACE" /D "SIMPLE_STRUCTURE" /D "TRACE_TIMER_DETAILS" /D "xTRACE_EVENT_TIMING" /D "TraceFreeAttacks" /D "CDX_LOG" /D "D20140707" /FAcs /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib cdx.lib dinput.lib bass.lib zlibr.lib libpngr.lib /nologo /subsystem:windows /map /debug /machine:I386 /out:"Release/DungeonCraft.exe" /libpath:"..\shared"
# SUBTRACT LINK32 /verbose /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWin	copy release\DungeonCraft.exe ..\..\UAFWin\DungeonCraft.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /GX /Zi /Od /Gy /I "..\shared" /I "..\cdx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEngine" /D "TRACE_TIMER_DETAILS" /D "TASKTRACE" /D "SIMPLE_STRUCTURE" /D "TRACE_EVENT_TIMING" /D "TraceFreeAttacks" /D "CDX_LOG" /D "D20140707" /FAcs /Fr /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib cdxd.lib bass.lib zlibr.lib libpngr.lib /nologo /subsystem:windows /profile /debug /debugtype:both /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"Debug/DungeonCraft.exe" /libpath:"..\shared" /fixed:no
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWin	copy debug\DungeonCraft.exe ..\..\UAFWin\DungeonCraft.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug RT"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /Zi /Od /Gy /I "..\shared_new" /I "..\cdx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEngine" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /Gy /I "..\shared" /I "..\cdx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEngine" /D "_AFXDLL" /YX"..\Shared\stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib ddraw.lib dsound.lib dxguid.lib cdxd.lib dinput.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"..\lib\debug" /fixed:no
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib dinput.lib bass.lib zlibd.lib libpngd.lib cdxd.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /libpath:"..\shared" /fixed:no
# SUBTRACT LINK32 /profile /pdb:none /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWin	del ..\..\UAFWin\UAFWin.exe	copy debug\UAFWin.exe ..\..\UAFWin\UAFWin.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug PS"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /Zi /Od /Gy /I "..\shared_new" /I "..\cdx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEngine" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /GX /Zi /Od /I "..\shared" /I "..\cdx" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEngine" /D "_AFXDLL" /D "TASKTRACE" /Yu"..\Shared\stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib ddraw.lib dsound.lib dxguid.lib cdxd.lib dinput.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"..\lib\debug" /fixed:no
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib dinput.lib bass.lib zlibd.lib libpngd.lib cdxd.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"..\shared" /fixed:no
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWin	copy debug\UAFWin.exe ..\..\UAFWin\UAFWin.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "UAFWin - Win32 Release"
# Name "UAFWin - Win32 Debug"
# Name "UAFWin - Win32 Debug RT"
# Name "UAFWin - Win32 Debug PS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Shared\ASL.cpp
# End Source File
# Begin Source File

SOURCE=.\CDXBitmapFont.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Char.cpp
# End Source File
# Begin Source File

SOURCE=.\CharStatsForm.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\class.cpp
# End Source File
# Begin Source File

SOURCE=.\Combatant.cpp
# End Source File
# Begin Source File

SOURCE=.\Combatants.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ConfigFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CProcinp.cpp
# End Source File
# Begin Source File

SOURCE=.\Dgngame.cpp
# End Source File
# Begin Source File

SOURCE=.\Disptext.cpp
# End Source File
# Begin Source File

SOURCE=.\Drawtile.cpp
# End Source File
# Begin Source File

SOURCE=.\Dungeon.cpp
# End Source File
# Begin Source File

SOURCE=.\Dungeon.rc

!IF  "$(CFG)" == "UAFWin - Win32 Release"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug RT"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug PS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Shared\FileParse.cpp
# End Source File
# Begin Source File

SOURCE=.\FormattedText.cpp
# End Source File
# Begin Source File

SOURCE=.\Forth.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GameEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMenu.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GameRules.cpp
# End Source File
# Begin Source File

SOURCE=.\Getinput.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GlobalData.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Globals.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GPDLcomp.cpp

!IF  "$(CFG)" == "UAFWin - Win32 Release"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug RT"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug PS"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Shared\GPDLexec.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Graphics.cpp
# End Source File
# Begin Source File

SOURCE=..\UAFWinEd\ImportWarning.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Items.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemsForm.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Level.cpp
# End Source File
# Begin Source File

SOURCE=.\Mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Money.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Monster.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseThread.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Movie.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectScripts.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Party.cpp
# End Source File
# Begin Source File

SOURCE=.\path.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\PicData.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\PicSlot.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Property.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\regexp.cpp
# End Source File
# Begin Source File

SOURCE=.\RestTimeForm.cpp
# End Source File
# Begin Source File

SOURCE=.\RunEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RunTimeIF.cpp
# End Source File
# Begin Source File

SOURCE=.\Screen.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SoundMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Specab.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Spell.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellForm.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Stackwalker.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\stdafx.cpp

!IF  "$(CFG)" == "UAFWin - Win32 Release"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug RT"

!ELSEIF  "$(CFG)" == "UAFWin - Win32 Debug PS"

# ADD CPP /Yc"..\Shared\stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Shared\taglist.cpp
# End Source File
# Begin Source File

SOURCE=.\TextForm.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Timer.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\traits.cpp
# End Source File
# Begin Source File

SOURCE=.\Treas.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\XBrowseForFolder.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\MFC\Include\AFXTEMPL.H"
# End Source File
# Begin Source File

SOURCE=..\Shared\ASL.h
# End Source File
# Begin Source File

SOURCE=..\Shared\bass.h
# End Source File
# Begin Source File

SOURCE=.\CDXBitmapFont.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Char.h
# End Source File
# Begin Source File

SOURCE=.\CharStatsForm.h
# End Source File
# Begin Source File

SOURCE=..\Shared\class.h
# End Source File
# Begin Source File

SOURCE=.\Combatant.h
# End Source File
# Begin Source File

SOURCE=.\Combatants.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ConfigFile.h
# End Source File
# Begin Source File

SOURCE=.\CProcinp.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\CRTDBG.H"
# End Source File
# Begin Source File

SOURCE=.\Dgngame.h
# End Source File
# Begin Source File

SOURCE=.\Disptext.h
# End Source File
# Begin Source File

SOURCE=.\Drawtile.h
# End Source File
# Begin Source File

SOURCE=.\Dungeon.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Externs.h
# End Source File
# Begin Source File

SOURCE=..\Shared\FileParse.h
# End Source File
# Begin Source File

SOURCE=.\FormattedText.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GameEvent.h
# End Source File
# Begin Source File

SOURCE=.\GameMenu.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GameRules.h
# End Source File
# Begin Source File

SOURCE=.\Getinput.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GlobalData.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GPDLcomp.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GPDLexec.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GPDLOpCodes.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Graphics.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Items.h
# End Source File
# Begin Source File

SOURCE=.\ItemsForm.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Level.h
# End Source File
# Begin Source File

SOURCE=.\Mainfrm.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Money.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Monster.h
# End Source File
# Begin Source File

SOURCE=.\MouseThread.h
# End Source File
# Begin Source File

SOURCE=.\ObjectScripts.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Party.h
# End Source File
# Begin Source File

SOURCE=.\path.h
# End Source File
# Begin Source File

SOURCE=..\Shared\PicData.h
# End Source File
# Begin Source File

SOURCE=..\Shared\PicSlot.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ProjectVersion.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Property.h
# End Source File
# Begin Source File

SOURCE=..\Shared\regexp.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RestTimeForm.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RunTimeIF.h
# End Source File
# Begin Source File

SOURCE=.\Screen.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedQueue.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SoundMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Specab.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Spell.h
# End Source File
# Begin Source File

SOURCE=.\SpellForm.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Stackwalker.h
# End Source File
# Begin Source File

SOURCE=..\Shared\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SurfaceMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Taglist.h
# End Source File
# Begin Source File

SOURCE=.\TextForm.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Thread.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Timer.h
# End Source File
# Begin Source File

SOURCE=..\Shared\traits.h
# End Source File
# Begin Source File

SOURCE=.\Treas.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Version.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Viewport.h
# End Source File
# Begin Source File

SOURCE=.\XBrowseForFolder.h
# End Source File
# Begin Source File

SOURCE=..\Shared\zconf.h
# End Source File
# Begin Source File

SOURCE=..\Shared\zlib.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\config.txt
# End Source File
# Begin Source File

SOURCE=.\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\UAFWin.ico
# End Source File
# End Target
# End Project
