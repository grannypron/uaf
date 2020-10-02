# Microsoft Developer Studio Project File - Name="UAFWinEd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=UAFWinEd - Win32 Debug PS
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UAFWinEd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UAFWinEd.mak" CFG="UAFWinEd - Win32 Debug PS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UAFWinEd - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "UAFWinEd - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "UAFWinEd - Win32 Debug RT" (based on "Win32 (x86) Application")
!MESSAGE "UAFWinEd - Win32 Debug PS" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UAFWinEd - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O1 /I "..\shared" /I "..\cdx" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEDITOR" /D "SIMPLE_STRUCTURE" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib cdx.lib bass.lib zlibr.lib libpngr.lib /nologo /subsystem:windows /map /machine:I386 /out:"Release/DungeonCraftEditor.exe" /libpath:"..\shared"
# SUBTRACT LINK32 /verbose /debug /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWinEd	copy release\DungeonCraftEditor.exe ..\..\UAFWinEd\DungeonCraftEditor.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /GX /Zi /Od /Gy /I "..\shared" /I "..\cdx" /D "_WIN32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEDITOR" /D "SIMPLE_STRUCTURE" /Fr /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib cdxd.lib bass.lib zlibr.lib libpngr.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"Debug/DungeonCraftEditor.exe" /libpath:"..\shared" /FIXED:NO
# SUBTRACT LINK32 /verbose /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWinEd	copy debug\DungeonCraftEditor.exe ..\..\UAFWinEd\DungeonCraftEditor.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug RT"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /Zi /Od /Gy /I "..\shared" /I "..\cdx" /D WINVER=0x0400 /D "_WIN32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEDITOR" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /Op /Gy /I "..\shared" /I "..\cdx" /D "_WIN32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEDITOR" /D "_AFXDLL" /D "SIMPLE_STRUCTURE" /Fr /YX"..\shared\stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib ddraw.lib dsound.lib dxguid.lib cdxd.lib shlwapi.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /libpath:"..\lib\debug" /FIXED:NO
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib shlwapi.lib bass.lib zlibd.lib libpngd.lib cdxd.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /libpath:"..\shared" /FIXED:NO
# SUBTRACT LINK32 /profile /pdb:none /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWinEd	del ..\..\UAFWinEd\UAFWinEd.exe	copy debug\UAFWinEd.exe ..\..\UAFWinEd\UAFWinEd.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug PS"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /GR /GX /Zi /Od /Gy /I "..\shared" /I "..\cdx" /D WINVER=0x0400 /D "_WIN32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEDITOR" /D "_AFXDLL" /Fr /YX"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /Gy /I "..\shared" /I "..\cdx" /D "_WIN32" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UAFEDITOR" /D "_AFXDLL" /Fr /Yu"..\Shared\stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib ddraw.lib dsound.lib dxguid.lib cdxd.lib shlwapi.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /libpath:"..\lib\debug" /FIXED:NO
# SUBTRACT BASE LINK32 /pdb:none /map
# ADD LINK32 vfw32.lib amstrmid.lib winmm.lib ddraw.lib dsound.lib dxguid.lib shlwapi.lib bass.lib zlibd.lib libpngd.lib cdxd.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept /libpath:"..\shared" /FIXED:NO
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\UAFWinEd	copy debug\UAFWinEd.exe ..\..\UAFWinEd\UAFWinEd.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "UAFWinEd - Win32 Release"
# Name "UAFWinEd - Win32 Debug"
# Name "UAFWinEd - Win32 Debug RT"
# Name "UAFWinEd - Win32 Debug PS"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AccessDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Addnpc.cpp
# End Source File
# Begin Source File

SOURCE=.\AddSpecialObject.cpp
# End Source File
# Begin Source File

SOURCE=.\AnimationStyle.cpp
# End Source File
# Begin Source File

SOURCE=.\ArrowHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\AskSureDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ASL.cpp
# End Source File
# Begin Source File

SOURCE=.\AttibuteDataEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\AttributeSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BkGndPicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Buttons.cpp
# End Source File
# Begin Source File

SOURCE=.\Campevnt.cpp
# End Source File
# Begin Source File

SOURCE=.\Cbattres.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Char.cpp
# End Source File
# Begin Source File

SOURCE=.\CharacterEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\Chardlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseCombatMonsters.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseNPC.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\ChooseSpeclAbDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ChurchDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\class.cpp
# End Source File
# Begin Source File

SOURCE=.\ClassDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Colorbox.cpp
# End Source File
# Begin Source File

SOURCE=.\CombatEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\CombatMonsterDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\CombatTreasureDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\ConfigFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CrossReferenceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Dbutils.cpp
# End Source File
# Begin Source File

SOURCE=.\DesignList.cpp
# End Source File
# Begin Source File

SOURCE=.\DesignStatsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DiffLvlData.cpp
# End Source File
# Begin Source File

SOURCE=.\DiffLvlSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPicture.cpp
# End Source File
# Begin Source File

SOURCE=.\EditAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\EditBGSlots.cpp
# End Source File
# Begin Source File

SOURCE=.\EditorOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\EditText.cpp
# End Source File
# Begin Source File

SOURCE=.\EditWallSlots.cpp
# End Source File
# Begin Source File

SOURCE=.\Encnter.cpp
# End Source File
# Begin Source File

SOURCE=.\EntryPointDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EventViewer.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\FileParse.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GameEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\GameResChange.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GameRules.cpp
# End Source File
# Begin Source File

SOURCE=.\GetMoney.cpp
# End Source File
# Begin Source File

SOURCE=.\GetMoneySackData.cpp
# End Source File
# Begin Source File

SOURCE=.\Givedmg.cpp
# End Source File
# Begin Source File

SOURCE=.\Giveexp.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GlobalData.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalSndChooser.cpp
# End Source File
# Begin Source File

SOURCE=.\Globtext.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\GPDLcomp.cpp

!IF  "$(CFG)" == "UAFWinEd - Win32 Release"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug RT"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug PS"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Shared\GPDLexec.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Graphics.cpp

!IF  "$(CFG)" == "UAFWinEd - Win32 Release"

# ADD CPP /O2 /Ob2

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug RT"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug PS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HealParty.cpp
# End Source File
# Begin Source File

SOURCE=.\Help.cpp
# End Source File
# Begin Source File

SOURCE=.\IconPicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IconPicPoolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportFRUAData.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportWarning.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemDB.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemDBDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Itemdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemKeyQuestDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Items.cpp
# End Source File
# Begin Source File

SOURCE=.\JournalAddEventDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\JournalDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\JournalEntryDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Level.cpp
# End Source File
# Begin Source File

SOURCE=.\ListSort.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\MagicalItemProp.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Mapstyle.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Money.cpp
# End Source File
# Begin Source File

SOURCE=.\MoneyConfigDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Monster.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterAttackDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterAttacksDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterDBDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Movie.cpp
# End Source File
# Begin Source File

SOURCE=.\Npcsays.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Party.cpp
# End Source File
# Begin Source File

SOURCE=.\PassTimeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Password.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\PicData.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\PicSlot.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayMovieDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PreviewFileDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Property.cpp
# End Source File
# Begin Source File

SOURCE=.\QButtonOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\QListOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\QuestEventData.cpp
# End Source File
# Begin Source File

SOURCE=.\QuestionYesNoData.cpp
# End Source File
# Begin Source File

SOURCE=.\RandomEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\regexp.cpp
# End Source File
# Begin Source File

SOURCE=.\Removnpc.cpp
# End Source File
# Begin Source File

SOURCE=.\RequestFilename.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\RunTimeIF.cpp
# End Source File
# Begin Source File

SOURCE=.\ScrollPicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SearchParamDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectFolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectPicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\shelllink.cpp
# End Source File
# Begin Source File

SOURCE=.\Shopdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SmallPicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SmallPicPoolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SmallTownEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\SortableListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundChooser.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundEventDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\SoundMgr.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Specab.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecAbMsgEnter.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecAbMsgs.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecialItemDlg1.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecialKeyBlockage.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Spell.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellAttribute.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellDBDlgEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Spelldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpellEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Stackwalker.cpp
# End Source File
# Begin Source File

SOURCE=.\StartingEquipDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\stdafx.cpp

!IF  "$(CFG)" == "UAFWinEd - Win32 Release"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug"

# ADD BASE CPP /Yc"..\Shared\stdafx.h"
# ADD CPP /Yc"..\Shared\stdafx.h"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug RT"

# ADD BASE CPP /Yc"..\Shared\stdafx.h"
# ADD CPP /Yc"..\Shared\stdafx.h"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug PS"

# ADD BASE CPP /Yc"..\Shared\stdafx.h"
# ADD CPP /Yc"..\Shared\stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StepEventDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\taglist.cpp
# End Source File
# Begin Source File

SOURCE=.\TakePartyItems.cpp
# End Source File
# Begin Source File

SOURCE=.\Tavdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TavernTalesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TeleportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TestPartySelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TextEvent.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleScreenConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\TitleScreenData.cpp
# End Source File
# Begin Source File

SOURCE=.\Tour.cpp
# End Source File
# Begin Source File

SOURCE=.\Traindlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\traits.cpp
# End Source File
# Begin Source File

SOURCE=.\TransferEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\UAFWinEd.cpp
# End Source File
# Begin Source File

SOURCE=.\UAFWinEd.rc

!IF  "$(CFG)" == "UAFWinEd - Win32 Release"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug RT"

!ELSEIF  "$(CFG)" == "UAFWinEd - Win32 Debug PS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UAFWinEdDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\UAFWinEdView.cpp
# End Source File
# Begin Source File

SOURCE=.\UAImport.cpp
# End Source File
# Begin Source File

SOURCE=.\UtilitiesEventData.cpp
# End Source File
# Begin Source File

SOURCE=.\VaultEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\VersionDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Shared\Viewport.cpp
# End Source File
# Begin Source File

SOURCE=.\WallPicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WhoPaysDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WhoTriesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XBrowseForFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\Zonedlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AccessDlg.h
# End Source File
# Begin Source File

SOURCE=.\Addnpc.h
# End Source File
# Begin Source File

SOURCE=.\AddSpecialObject.h
# End Source File
# Begin Source File

SOURCE=.\AnimationStyle.h
# End Source File
# Begin Source File

SOURCE=.\ArrowHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\AskSureDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ASL.h
# End Source File
# Begin Source File

SOURCE=.\AttibuteDataEdit.h
# End Source File
# Begin Source File

SOURCE=.\AttributeSelect.h
# End Source File
# Begin Source File

SOURCE=.\BaseDialog.h
# End Source File
# Begin Source File

SOURCE=.\BkGndPicDlg.h
# End Source File
# Begin Source File

SOURCE=.\Buttons.h
# End Source File
# Begin Source File

SOURCE=.\Campevnt.h
# End Source File
# Begin Source File

SOURCE=.\Cbattres.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Char.h
# End Source File
# Begin Source File

SOURCE=.\CharacterEditor.h
# End Source File
# Begin Source File

SOURCE=.\Chardlg.h
# End Source File
# Begin Source File

SOURCE=.\ChooseCombatMonsters.h
# End Source File
# Begin Source File

SOURCE=.\ChooseMonster.h
# End Source File
# Begin Source File

SOURCE=.\ChooseNPC.h
# End Source File
# Begin Source File

SOURCE=.\ChooseOptions.h
# End Source File
# Begin Source File

SOURCE=.\ChooseSpeclAbDlg.h
# End Source File
# Begin Source File

SOURCE=.\ChurchDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\class.h
# End Source File
# Begin Source File

SOURCE=.\ClassDlg.h
# End Source File
# Begin Source File

SOURCE=.\Colorbox.h
# End Source File
# Begin Source File

SOURCE=.\CombatEvent.h
# End Source File
# Begin Source File

SOURCE=.\CombatMonsterDetails.h
# End Source File
# Begin Source File

SOURCE=.\CombatTreasureDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ConfigFile.h
# End Source File
# Begin Source File

SOURCE=.\CrossReference.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\Include\CRTDBG.H"
# End Source File
# Begin Source File

SOURCE=.\Dbutils.h
# End Source File
# Begin Source File

SOURCE=.\DesignList.h
# End Source File
# Begin Source File

SOURCE=.\DesignStatsDlg.h
# End Source File
# Begin Source File

SOURCE=.\DiffLvlData.h
# End Source File
# Begin Source File

SOURCE=.\DiffLvlSettings.h
# End Source File
# Begin Source File

SOURCE=..\UAFWin\Disptext.h
# End Source File
# Begin Source File

SOURCE=.\DlgPicture.h
# End Source File
# Begin Source File

SOURCE=.\EditAttributes.h
# End Source File
# Begin Source File

SOURCE=.\EditBGSlots.h
# End Source File
# Begin Source File

SOURCE=.\EditorOptions.h
# End Source File
# Begin Source File

SOURCE=.\EditText.h
# End Source File
# Begin Source File

SOURCE=.\EditWallSlots.h
# End Source File
# Begin Source File

SOURCE=.\EdWindows.h
# End Source File
# Begin Source File

SOURCE=.\Encnter.h
# End Source File
# Begin Source File

SOURCE=.\EntryPointDlg.h
# End Source File
# Begin Source File

SOURCE=.\EventViewer.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Externs.h
# End Source File
# Begin Source File

SOURCE=..\Shared\FileParse.h
# End Source File
# Begin Source File

SOURCE=..\UAFWin\FormattedText.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GameEvent.h
# End Source File
# Begin Source File

SOURCE=.\GameResChange.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GameRules.h
# End Source File
# Begin Source File

SOURCE=.\GetMoney.h
# End Source File
# Begin Source File

SOURCE=.\GetMoneySackData.h
# End Source File
# Begin Source File

SOURCE=.\Givedmg.h
# End Source File
# Begin Source File

SOURCE=.\Giveexp.h
# End Source File
# Begin Source File

SOURCE=..\Shared\GlobalData.h
# End Source File
# Begin Source File

SOURCE=.\GlobalDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\GlobalSndChooser.h
# End Source File
# Begin Source File

SOURCE=.\Globtext.h
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

SOURCE=.\HealParty.h
# End Source File
# Begin Source File

SOURCE=.\Help.h
# End Source File
# Begin Source File

SOURCE=.\IconPicDlg.h
# End Source File
# Begin Source File

SOURCE=.\IconPicPoolDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImportFRUAData.h
# End Source File
# Begin Source File

SOURCE=.\ImportWarning.h
# End Source File
# Begin Source File

SOURCE=.\ItemDB.h
# End Source File
# Begin Source File

SOURCE=.\ItemDBDlg.h
# End Source File
# Begin Source File

SOURCE=.\Itemdlg.h
# End Source File
# Begin Source File

SOURCE=.\ItemEditor.h
# End Source File
# Begin Source File

SOURCE=.\ItemKeyQuestDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Items.h
# End Source File
# Begin Source File

SOURCE=.\JournalAddEventDlg.h
# End Source File
# Begin Source File

SOURCE=.\JournalDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\JournalEntryDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Level.h
# End Source File
# Begin Source File

SOURCE=.\ListSort.h
# End Source File
# Begin Source File

SOURCE=.\LogicBlock.h
# End Source File
# Begin Source File

SOURCE=.\magicalitemprop.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Mapstyle.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Money.h
# End Source File
# Begin Source File

SOURCE=.\MoneyConfigDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Monster.h
# End Source File
# Begin Source File

SOURCE=.\MonsterAttackDetails.h
# End Source File
# Begin Source File

SOURCE=.\MonsterAttacksDlg.h
# End Source File
# Begin Source File

SOURCE=.\MonsterDBDlg.h
# End Source File
# Begin Source File

SOURCE=.\MonsterEditor.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Movie.h
# End Source File
# Begin Source File

SOURCE=.\Npcsays.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Party.h
# End Source File
# Begin Source File

SOURCE=.\PassTimeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Password.h
# End Source File
# Begin Source File

SOURCE=..\Shared\PicData.h
# End Source File
# Begin Source File

SOURCE=..\Shared\PicSlot.h
# End Source File
# Begin Source File

SOURCE=.\PlayMovieDlg.h
# End Source File
# Begin Source File

SOURCE=.\PreviewFileDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\ProjectVersion.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Property.h
# End Source File
# Begin Source File

SOURCE=.\QButtonOptions.h
# End Source File
# Begin Source File

SOURCE=.\QListOptions.h
# End Source File
# Begin Source File

SOURCE=.\QuestEventData.h
# End Source File
# Begin Source File

SOURCE=.\QuestionYesNoData.h
# End Source File
# Begin Source File

SOURCE=.\RandomEvent.h
# End Source File
# Begin Source File

SOURCE=..\Shared\regexp.h
# End Source File
# Begin Source File

SOURCE=.\Removnpc.h
# End Source File
# Begin Source File

SOURCE=.\RequestFilename.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=..\Shared\RunTimeIF.h
# End Source File
# Begin Source File

SOURCE=.\ScrollPicDlg.h
# End Source File
# Begin Source File

SOURCE=.\SearchParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\SelectFolderDialog.h
# End Source File
# Begin Source File

SOURCE=.\SelectLevel.h
# End Source File
# Begin Source File

SOURCE=.\SelectPicDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SharedQueue.h
# End Source File
# Begin Source File

SOURCE=.\shelllink.h
# End Source File
# Begin Source File

SOURCE=.\Shopdlg.h
# End Source File
# Begin Source File

SOURCE=.\SmallPicDlg.h
# End Source File
# Begin Source File

SOURCE=.\SmallPicPoolDlg.h
# End Source File
# Begin Source File

SOURCE=.\SmallTownEvent.h
# End Source File
# Begin Source File

SOURCE=.\SortableListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SoundChooser.h
# End Source File
# Begin Source File

SOURCE=.\SoundEventDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SoundMgr.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Specab.h
# End Source File
# Begin Source File

SOURCE=.\SpecAbMsgEnter.h
# End Source File
# Begin Source File

SOURCE=.\SpecAbMsgs.h
# End Source File
# Begin Source File

SOURCE=.\SpecialItemDlg1.h
# End Source File
# Begin Source File

SOURCE=.\SpecialKeyBlockage.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Spell.h
# End Source File
# Begin Source File

SOURCE=.\SpellAttribute.h
# End Source File
# Begin Source File

SOURCE=.\SpellDBDlgEx.h
# End Source File
# Begin Source File

SOURCE=.\Spelldlg.h
# End Source File
# Begin Source File

SOURCE=.\SpellEditor.h
# End Source File
# Begin Source File

SOURCE=.\SpellSpecialChoose.h
# End Source File
# Begin Source File

SOURCE=.\SpellSpecialConfig.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Stackwalker.h
# End Source File
# Begin Source File

SOURCE=.\StartingEquipDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\StepEventDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\SurfaceMgr.h
# End Source File
# Begin Source File

SOURCE=.\TakePartyItems.h
# End Source File
# Begin Source File

SOURCE=.\Tavdlg.h
# End Source File
# Begin Source File

SOURCE=.\TavernTalesDlg.h
# End Source File
# Begin Source File

SOURCE=.\TeleportDlg.h
# End Source File
# Begin Source File

SOURCE=.\TestPartySelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\TextEvent.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Thread.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Timer.h
# End Source File
# Begin Source File

SOURCE=.\TitleScreenConfig.h
# End Source File
# Begin Source File

SOURCE=.\TitleScreenData.h
# End Source File
# Begin Source File

SOURCE=.\Tour.h
# End Source File
# Begin Source File

SOURCE=.\Traindlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\traits.h
# End Source File
# Begin Source File

SOURCE=.\TransferEvent.h
# End Source File
# Begin Source File

SOURCE=.\UAFWinEd.h
# End Source File
# Begin Source File

SOURCE=.\UAFWinEdDoc.h
# End Source File
# Begin Source File

SOURCE=.\UAFWinEdView.h
# End Source File
# Begin Source File

SOURCE=.\UAImport.h
# End Source File
# Begin Source File

SOURCE=.\UtilitiesEventData.h
# End Source File
# Begin Source File

SOURCE=.\VaultEvent.h
# End Source File
# Begin Source File

SOURCE=.\VersionDlg.h
# End Source File
# Begin Source File

SOURCE=..\Shared\Viewport.h
# End Source File
# Begin Source File

SOURCE=.\WallPicDlg.h
# End Source File
# Begin Source File

SOURCE=.\WhoPaysDlg.h
# End Source File
# Begin Source File

SOURCE=.\WhoTriesDlg.h
# End Source File
# Begin Source File

SOURCE=.\XBrowseForFolder.h
# End Source File
# Begin Source File

SOURCE=.\Zonedlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\PlayArrow.ico
# End Source File
# Begin Source File

SOURCE=.\res\playstop.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\UAFWinEd.ico
# End Source File
# Begin Source File

SOURCE=.\res\UAFWinEd.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\ability.txt
# End Source File
# Begin Source File

SOURCE=..\..\baseclass.txt
# End Source File
# Begin Source File

SOURCE=..\..\classes.txt
# End Source File
# Begin Source File

SOURCE=..\..\config.txt
# End Source File
# Begin Source File

SOURCE=..\..\items.txt
# End Source File
# Begin Source File

SOURCE=..\..\monsters.txt
# End Source File
# Begin Source File

SOURCE=..\..\races.txt
# End Source File
# Begin Source File

SOURCE=..\..\skills.txt
# End Source File
# Begin Source File

SOURCE=..\..\SpellGroups.txt
# End Source File
# Begin Source File

SOURCE=..\..\spells.txt
# End Source File
# Begin Source File

SOURCE=..\..\traits.txt
# End Source File
# End Target
# End Project
