/******************************************************************************
* Filename: Dgngame.cpp
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "..\shared\stdafx.h"
//#include "..\Shared\ProjectVersion.h"
//#include "..\shared\externs.h"
#include "Dungeon.h"

//#include <math.h>
#include "items.h"
#include "char.h"
#include "GlobalData.h"
#include "level.h"
#include "GameEvent.h"
#include "party.h"
#include "Graphics.h"
#include "disptext.h"
#include "GetInput.h"
#include "CProcInp.h"
#include "dgngame.h"
#include "specab.h"
#include "treas.h"
#include "monster.h"
#include "Screen.h"
#include "viewport.h"
#include "combatants.h"
#include "drawtile.h"
#include "FormattedText.h"
#include "FileParse.h"
#include "SoundMgr.h"
#include "MouseThread.h"

void My_free(void *addr);
void gpdlCleanup(void);

extern const double ENGINE_VER;
extern const double VersionSpellNames;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CProcessInput ProcInput;


#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
  FILE *f;
public:
  DETAIL_TRACE(void);
  ~DETAIL_TRACE(void);
//  DWORD    m_id[1000];
//  DWORD    m_data[1000];
//  DWORD    m_time[1000];
//  DWORD    m_virtualTime[1000];
//  int      m_nextIndex;
  void     trace (DWORD id, DWORD data);
};
extern DETAIL_TRACE detailTrace;
#endif


#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif

///////////////////////////////////////////////////
//*****************************************************************************
//    NAME: serializeGame
//
// PURPOSE: 
//
//*****************************************************************************
BOOL serializeGame(BOOL save, int num)
{
  TRACE("Load game start\n");
  BOOL success = TRUE;
  CFile myFile;
  
  //char fullPath[_MAX_PATH+1];  
  char numText[2];
  wsprintf(numText, "%c", 'A' + num);

  CreateSaveDirectory();

  CString fullPath;
  //sprintf(fullPath, "%sSave", global_designDir);   
  //strcat(fullPath, numText);
  //strcat(fullPath, ".pty"); 
  
  fullPath = GetSavePath();
  fullPath += "Save";
  fullPath += numText;
  fullPath += ".pty";

  if (!save)
  {
    if (!myFile.Open(fullPath, CFile::modeRead))
    {
      success = FALSE;
      WriteDebugString("Unable to open the save game %s\n", fullPath);
    }
    else
    {       
      BOOL VersionError = FALSE;
      
      try 
      {
        DWORD filePos;
        // Our goal is to make a temporary, in-memory copy of the entire
        // save file.  If we can load the entire file without error
        // then we will copy this data to the working game structures.
        // When there is a list of objects we will attempt to match up
        // the names of the restored objects with the names of the
        // objects in the design.  In this way, objects can be added
        // to the design and we can still use the old
        // save files in most cases.  Deleting things from the design
        // will likely change IDs and such and probably should not
        // be attempted if the old save files are to be used.

        PARTY tempParty;
        QUEST_LIST tempQuest;
        SPECIAL_OBJECT_LIST tempIList;
        SPECIAL_OBJECT_LIST tempKList;
        GLOBAL_VAULT_DATA tempVData[MAX_GLOBAL_VAULTS];
        ACTIVE_SPELL_LIST tempACTIVESPELLS;




        // check for save game version
       filePos = myFile.GetPosition();
       myFile.Read(&globalData.SaveGameVersion, sizeof(globalData.SaveGameVersion));
       filePos = myFile.GetPosition();
      
        if (globalData.SaveGameVersion < _VERSION_0573_)
        {
          // not compatible at all, abort load
          miscError = SaveGameVersionError;
          WriteDebugString("Save game version pre-dates event conversion, unable to load it\n");
          return FALSE;
        }

        if (globalData.SaveGameVersion < VersionSpellNames)
        {
          // not compatible at all, abort load
          miscError = SaveGameVersionError;
          WriteDebugString("We cannot load saved games created before version %f\n", VersionSpellNames);
          return FALSE;
        }

        if (globalData.SaveGameVersion > ENGINE_VER)
        {
          VersionError = TRUE;
          WriteDebugString("Possible save game version error: file %s, file ver %4.7f, game ver %4.7f",
                           fullPath, globalData.SaveGameVersion, ENGINE_VER);
        }

        if (globalData.SaveGameVersion >= VersionSpellNames)
        {
          //DWORD filePos;
          CAR car(&myFile, CArchive::load);
  
          car.Compress(true);
          filePos = car.GetPosition();

          tempParty.Serialize(car);
          filePos = car.GetPosition();
          tempQuest.Serialize(car,globalData.SaveGameVersion);
          filePos = car.GetPosition();
  
          {
            tempIList.Serialize(car,globalData.SaveGameVersion);
            filePos = car.GetPosition();
            tempKList.Serialize(car,globalData.SaveGameVersion);
            filePos = car.GetPosition();
          }
  
          // vaults were not saved prior to 0.661
          {
            // only one vault was stored prior to 0.910
            if (globalData.SaveGameVersion < _VERSION_0910_)
            {
              tempVData[0].Serialize(car);
            }
            else
            {
              int numvaults;
              car >> numvaults;
              if (numvaults != MAX_GLOBAL_VAULTS)
              {
                TRACE("*** ERROR: Invalid numvaults read from saved game file\n");
                die(0xab4da);
                if (numvaults > MAX_GLOBAL_VAULTS)
                  numvaults = MAX_GLOBAL_VAULTS;
              }
              for (int i=0;i<numvaults;i++)
                tempVData[i].Serialize(car);
            }
          }
  
          {
            tempACTIVESPELLS.Serialize(car,globalData.SaveGameVersion); //Read activeSpellList
          }

          spellData.Restore(car);
          globalData.Restore(car);
          globalData.levelInfo.Restore(car);
          // See comments about LEVEL in the IsStoring() code.
          //      levelData.Restore(ar);
          globalData.keyData.Restore(car);
          globalData.specialItemData.Restore(car);
          itemData.Restore(car);
          monsterData.Restore(car);
        
          car.Close();
        }
        else
        {
          CArchive ar(&myFile, CArchive::load);
  
  
          tempParty.Serialize(ar);
          tempQuest.Serialize(ar,globalData.SaveGameVersion);
  
          if (globalData.SaveGameVersion >= _VERSION_0575_)
          {
            tempIList.Serialize(ar,globalData.SaveGameVersion);
            tempKList.Serialize(ar,globalData.SaveGameVersion);
          }
          else
          {
            tempIList.Clear();
            tempKList.Clear();
          }
  
          // vaults were not saved prior to 0.661
          if (globalData.SaveGameVersion >= _VERSION_0661_)
          {
            // only one vault was stored prior to 0.910
            if (globalData.SaveGameVersion < _VERSION_0910_)
            {
              tempVData[0].Serialize(ar);
            }
            else
            {
              int numvaults;
              ar >> numvaults;
              if (numvaults != MAX_GLOBAL_VAULTS)
              {
                TRACE("*** ERROR: Invalid numvaults read from saved game file\n");
                die(0xab4db);
                if (numvaults > MAX_GLOBAL_VAULTS)
                  numvaults = MAX_GLOBAL_VAULTS;
              }
              for (int i=0;i<numvaults;i++)
                tempVData[i].Serialize(ar);
            }
          }
  
          if (globalData.SaveGameVersion >= _VERSION_06991_)
          {
            tempACTIVESPELLS.Serialize(ar,globalData.SaveGameVersion); //Read activeSpellList
          }

          spellData.Restore(ar);
          globalData.Restore(ar);
          globalData.levelInfo.Restore(ar);
          // See comments about LEVEL in the IsStoring() code.
          //      levelData.Restore(ar);
          globalData.keyData.Restore(ar);
          globalData.specialItemData.Restore(ar);
          itemData.Restore(ar);
          monsterData.Restore(ar);
        
          if (globalData.SaveGameVersion >= _VERSION_0564_)
            if (globalData.SaveGameVersion < _VERSION_0569_) 
              levelData.eventData.Restore(ar);
          ar.Close();
        }







        party = tempParty;   
        
        //globalData.questData.Clear();  //20130508 PRS
        POSITION pos = tempQuest.GetHeadPosition();
        while (pos != NULL)
        {
          ///* 20130508 PRS
          if (globalData.questData.HaveQuest(tempQuest.PeekAtPos(pos).id))
          {
            // quest in saved game that also exists in quest list,
            // update quest value
            globalData.questData.SetQuest(tempQuest.PeekAtPos(pos).id,
                                          tempQuest.PeekAtPos(pos));
          }
          else //*/
          {
            // quest in saved game that doesn't exist in quest list
            // add quest
            globalData.questData.AddQuestWithCurrKey(tempQuest.PeekAtPos(pos));
          }

          tempQuest.GetNext(pos);
        }

        pos = tempIList.GetHeadPosition();
        while (pos != NULL)
        {
          if (globalData.specialItemData.HaveItem(tempIList.PeekAtPos(pos).id))
          {
            // quest in saved game that also exists in quest list,
            // update quest value
            globalData.specialItemData.SetItem(tempIList.PeekAtPos(pos).id,
                                                tempIList.PeekAtPos(pos));
          }
          else
          {
            // quest in saved game that doesn't exist in quest list
            // add quest
            globalData.specialItemData.AddItemWithCurrKey(tempIList.PeekAtPos(pos));
          }

          tempIList.GetNext(pos);
        }

        pos = tempKList.GetHeadPosition();
        while (pos != NULL)
        {
          if (globalData.keyData.HaveItem(tempKList.PeekAtPos(pos).id))
          {
            // quest in saved game that also exists in quest list,
            // update quest value
            globalData.keyData.SetItem(tempKList.PeekAtPos(pos).id,
                                       tempKList.PeekAtPos(pos));
          }
          else
          {
            // quest in saved game that doesn't exist in quest list
            // add quest
            globalData.keyData.AddItemWithCurrKey(tempKList.PeekAtPos(pos));
          }

          tempKList.GetNext(pos);
        }

        for (int v=0;v<MAX_GLOBAL_VAULTS;v++)
          globalData.vault[v] = tempVData[v];

        activeSpellList = tempACTIVESPELLS;

        spellData.CommitRestore();
        globalData.CommitRestore();
        globalData.levelInfo.CommitRestore();
        //      levelData.CommitRestore();
        globalData.keyData.CommitRestore();
        globalData.specialItemData.CommitRestore();
        itemData.CommitRestore();
        monsterData.CommitRestore();
        
        if (globalData.SaveGameVersion >= _VERSION_0564_)
          if (globalData.SaveGameVersion < _VERSION_0569_) 
            levelData.eventData.CommitRestore();

      }
      catch (...) 
      {
        success = FALSE;
        WriteDebugString("Unable to load the save game %s\n", fullPath);
      }
      
      if (!success)
      {
        if (VersionError)
          miscError = SaveGameVersionError;
        else
          miscError = FileLoadError;
      }
      else
        tempChars.RemoveNPCIfInParty(); // strip NPCs from tempChars if already in party
    }
  }
  else 
  { // save game

    // remove read-only flags
    SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
    if (!myFile.Open(fullPath, CFile::modeWrite | CFile::modeCreate))
    {
      success = FALSE;
      WriteDebugString("Unable to create/open the save game file %s\n", fullPath);
    }
    else
    {
      CAR car(&myFile, CArchive::store);
      
      try 
      {
        // Our intent is to write to the save file those things
        // that might be changed during gameplay.  Constant data
        // will come from the design files.

        globalData.SaveGameVersion = ENGINE_VER;
        //car.Write(&globalData.SaveGameVersion, sizeof(globalData.SaveGameVersion));
        car.Serialize((char *)&globalData.SaveGameVersion, sizeof(globalData.SaveGameVersion));
        car.Compress(true);

        party.Serialize(car);
        globalData.questData.Serialize(car,globalData.SaveGameVersion);
        globalData.specialItemData.Serialize(car,globalData.SaveGameVersion);
        globalData.keyData.Serialize(car,globalData.SaveGameVersion);
     
        car << MAX_GLOBAL_VAULTS;
        for (int v=0;v<MAX_GLOBAL_VAULTS;v++)
          globalData.vault[v].Serialize(car);
        
        activeSpellList.Serialize(car,globalData.SaveGameVersion);

        //spellData.Save(car);
        spellData.Save(car);
        //globalData.Save(car);
        globalData.Save(car);
        //globalData.levelInfo.Save(car);
        globalData.levelInfo.Save(car);
        // Do not save LEVEL attributes.  Any modifiable attributes associated
        // with a level that we want to save will be put into and saved with 
        // globalData.LevelInfo.stats[level#]
        // LEVEL may contain attributes created by the editor and they may be modified
        // during gameplay but the modifications will disappear when the 
        // game is restarted or a different level is loaded.
        //      levelData.Save(ar);
        //globalData.keyData.Save(car);
        globalData.keyData.Save(car);
        //globalData.specialItemData.Save(car);
        globalData.specialItemData.Save(car);
        //itemData.Save(car);
        itemData.Save(car);
        //monsterData.Save(car);
        monsterData.Save(car);
        // levelData.eventData.Save(ar);
      }
      catch (...)
      {
        success = FALSE;
        WriteDebugString("Unable to write the save game file %s\n", fullPath);
      }
      
      car.Close();

      if (!success)
        miscError = FileSaveError;
    }
  }
  TRACE("Load game end\n");
  return success;
}

///////////////////////////////////////////////
/*
BOOL readDatabase(void)
{
  char fileDir[_MAX_PATH+1];
  
  strcpy(fileDir, m_installDir);
  strcat(fileDir, ITEM_DB_NAME);
  
  if (!loadData(itemData, fileDir))
  {
    WriteDebugString("Failed to read Item DAT file\n");
    return FALSE;
  }
  
  if (itemData.GetCount() == 0)
  {
    WriteDebugString("Empty Item DB\n");
    //return FALSE;
  }
  
  strcpy(fileDir, m_installDir);
  strcat(fileDir, MONSTER_DB_NAME);
  
  if (!loadData(monsterData, fileDir))
  {
    WriteDebugString("Failed to read monster DAT file\n");
    return FALSE;
  }
  
  if (monsterData.GetCount() <= 0)
  {
    WriteDebugString("Empty Monster DB\n");
    //return FALSE;
  }
  
  strcpy(fileDir, m_installDir);
  strcat(fileDir, SPELL_DB_NAME);
  
  if (!loadData(spellData, fileDir))
  {
    WriteDebugString("Failed to read spell DAT file\n");
    return FALSE;
  }
  
  if (spellData.GetCount() <= 0)
  {
    WriteDebugString("Empty Spell DB\n");
    //return FALSE;
  }
  
  WriteDebugString("Database read successfully\n");
  return TRUE;
}
*/

///////////////////////////////////////////////////////////
//*****************************************************************************
//    NAME: InitGameStructures
//
// PURPOSE: 
//
//*****************************************************************************
void InitGameStructures(void)
{
  //AnimateList.RemoveAll();
  ClearAnimateList();
  party.Clear();  
  character.Clear();   
  tempChars.Clear();
  clearData(spellData);
  clearData(monsterData);
  clearData(itemData);   
  clearDesign(FALSE);   
  currPic.Clear();
  ClearFormattedText(textData);
  activeSpellList.xClear();
  pendingSpellList.Clear();
}

// after party dies and restarts from beginning
//*****************************************************************************
//    NAME: ResetAdventureData
//
// PURPOSE: 
//
//*****************************************************************************
void ResetAdventureData(void)
{
  // clear the attributes too?

  party.clearPartyFlags();
  for (int i=0;i<party.numCharacters;i++)
    party.characters[i].blockageData.Clear();
  globalData.questData.Reset();

  

  combatData.Clear();
/*
  combatData.m_bCombatActive=FALSE;
  {
    int i;
    for (i=0; i<combatData.NumCombatants(); i++)
    {
      CHARACTER *pChar;
      COMBATANT *pCombatant;
      pCombatant = combatData.GetCombatant(i);
      pChar = pCombatant->m_pCharacter;
      if (pChar != NULL)
      {
        if (!pChar->IsPartyMember())
        {
          delete pChar;
        }
        else
        {
          pChar->m_pCombatant = NULL;
        };
        pCombatant->m_pCharacter = NULL;
      };
    };
  };
  combatData.m_iNumCombatants = 0;
*/

  activeSpellList.xClear();
  pendingSpellList.Clear();
}

//*****************************************************************************
//    NAME: DesignAvail
//
// PURPOSE: 
//
//*****************************************************************************
BOOL DesignAvail(const char *path)
{
  if (path == NULL) return FALSE;
  CString temp;
  temp.Format("%s%s", path, "game.dat");

  if (!FileExists(temp))
  {
    debugSeverity = 9;
    WriteDebugString("%s does not exist\n", temp);
    return FALSE;
  }
  
  temp.Format("%s%s", path, "items.dat");
  
  if (!FileExists(temp))
  {
    debugSeverity = 9;
    WriteDebugString("%s does not exist\n", temp);
    return FALSE;
  }
  
  temp.Format("%s%s", path, "monsters.dat");
  
  if (!FileExists(temp))
  {
    debugSeverity = 9;
    WriteDebugString("%s does not exist\n", temp);
    return FALSE;
  }
  
  temp.Format("%s%s", path, "spells.dat");
  
  if (!FileExists(temp))
  {
    debugSeverity = 9;
    WriteDebugString("%s does not exist\n", temp);
    return FALSE;
  }
  SetCurrentDirectory(path);
  return TRUE;
}

//*****************************************************************************
//    NAME: InitDesignData
//
// PURPOSE: 
//
//*****************************************************************************
BOOL InitDesignData()
{
  ProcInput.AllowInput(FALSE);
  if (!loadDesign(NULL))
  {
    debugSeverity = 9;
    WriteDebugString("Failed to load design in InitDesignData()\n");
    return FALSE;
  }  
  WriteDebugString("Design Version: %4.7f\n", globalData.version);
  WriteDebugString("PlayAs Version: %4.7f\n", GetRunAsVersion(RunAsVersion));
  WriteDebugString("GuidedTour Version: %4.7f\n", GetRunAsVersion(GuidedTourVersion));
  WriteDebugString("ItemUseEvent Version: %4.7f\n", GetRunAsVersion(ItemUseEventVersion));
  WriteDebugString("SpecialItemKeyQty Version: %4.7f\n", GetRunAsVersion(SpecialItemKeyQtyVersion));  
  
  CreateSaveDirectory();
  CreateCommonCharDirectory();

  MouseRender.LoadSurfaces();
  MouseRender.InitInstance();

  party.setPartyTime(globalData.startTime);
  party.days = 1;
  party.skillLevel = globalData.m_diffLvlData.m_defaultLvl;
  
  LoadOffscreenWalkingSurfaces();

  // call this at least once after graphics init to setup
  // the textbox vars based on screen config.
  if ((GetTextBoxCharWidth() <= 0) || (GetTextBoxCharHeight() <= 0))
  {
    WriteDebugString("Invalid text box definition in config.txt\n");
    return FALSE;
  }
  
  if (globalData.titleData.Titles.GetCount()>0)
  {
    PlayIntro(TRUE);
    ProcInput.InitializeToSplash();
  }
  else
    ProcInput.InitializeToMain();

  ProcInput.AllowInput(TRUE);  
  WriteDebugString("Game initialized successfully\n");

#ifdef _DEBUG
  CString temp;
  temp.Format("Sizeof PARTY = %u\n", sizeof(party));
  TRACE(temp);
  temp.Format("Sizeof CHARACTER = %u\n", sizeof(character));
  TRACE(temp);
  temp.Format("Sizeof TEMP_CHARS = %u\n", sizeof(tempChars));
  TRACE(temp);
  temp.Format("Sizeof SPELL_DATA = %u\n", spellData.GetDataSize());
  TRACE(temp);
  temp.Format("Sizeof MONSTER_DATA = %u\n", monsterData.GetDataSize());
  TRACE(temp);
  temp.Format("Sizeof ITEM_DATA = %u\n", itemData.GetDataSize());
  TRACE(temp);
  temp.Format("Sizeof GLOBAL_DATA = %u\n", sizeof(globalData));
  TRACE(temp);
  temp.Format("Sizeof LEVEL_DATA = %u\n", sizeof(levelData));
  TRACE(temp);
  temp.Format("Sizeof RACE_DATA = %u\n", GetDataSize(raceData));
  TRACE(temp);
  temp.Format("Sizeof ABILITY_DATA = %u\n", GetDataSize(abilityData));
  TRACE(temp);
  temp.Format("Sizeof BASECLASS_DATA = %u\n", GetDataSize(baseclassData));
  TRACE(temp);
  temp.Format("Sizeof CLASS_DATA = %u\n", GetDataSize(classData));
  TRACE(temp);
  temp.Format("Sizeof SPELLGROUP_DATA = %u\n", GetDataSize(spellgroupData));
  TRACE(temp);
#endif
  RECT fullRect;
  fullRect.left = 0;
  fullRect.top = 0;
  fullRect.right = SCREEN_WIDTH;
  fullRect.bottom = SCREEN_HEIGHT;
  GraphicsMgr.SetClipRect(&fullRect);
  return TRUE;
}

/////////////////////////////////////////////////////////
//*****************************************************************************
//    NAME: InitGame
//
// PURPOSE: 
//
//*****************************************************************************
BOOL InitGame(HWND hwnd)
{
  {
    RECT r;
    GetClientRect(hwnd, &r);
    WriteDebugString("InitGame ClientRect = %d %d %d %d\n", r.left, r.top, r.right, r.bottom);
  };
  StopAllSounds();
  ReleaseAllSounds();
  if (pSndMgr != NULL)
  {
    delete pSndMgr;
    pSndMgr = NULL;
  }  
  MouseRender.ExitInstance();
  GraphicsMgr.Clear();


  CreateShutdownEvent();
  ProcInput.AllowInput(FALSE);
  InitGameStructures();  
  
#ifndef _DEBUG
  BOOL okwindow=FALSE;

  if (!RunFullscreen)
  {
    okwindow = GraphicsMgr.InitGraphicsWindowed(hwnd, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (okwindow)
      WriteDebugString("InitGraphics windowed at %u x %u\n",SCREEN_WIDTH, SCREEN_HEIGHT);
  }

  if (!okwindow)
  {
    if (!GraphicsMgr.InitGraphicsFullScreen(hwnd, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BITS))
    {
      WriteDebugString("Failed to InitGraphics fullscreen at %u x %u x %u\n",
        SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BITS);
      return FALSE;
    }
    else
    {
      WriteDebugString("InitGraphics fullscreen at %u x %u x %u\n",
        SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BITS);
    }
  }
#else
  if (!GraphicsMgr.InitGraphicsWindowed(hwnd, SCREEN_WIDTH, SCREEN_HEIGHT))
    return FALSE;
  WriteDebugString("InitGraphics windowed at %u x %u\n",SCREEN_WIDTH, SCREEN_HEIGHT);
#endif

  RECT fullRect;
  fullRect.left = 0;
  fullRect.top = 0;
  fullRect.right = SCREEN_WIDTH;
  fullRect.bottom = SCREEN_HEIGHT;
  GraphicsMgr.SetClipRect(&fullRect);
  
  if (pWallSlotMgr==NULL)
    pWallSlotMgr = new ImageCacheMgr(WALL_CACHE_SIZE, "WallSlot");
  if (pBgSlotMgr==NULL)
    pBgSlotMgr = new ImageCacheMgr(BACKGROUND_CACHE_SIZE, "BgSlot");

  // initialize the DirectSound object
  pSndMgr = new SoundMgr(hwnd);
  
  if (pSndMgr == NULL)
  {
    WriteDebugString("Failed to allocate SoundMgr\n");
    return FALSE;
  }
  
  // Seed the random-number generator with current time so that
  // the numbers will be different every time we run.
  srand( (unsigned)time( NULL ) );
  if (!DesignAvail(rte.DataDir())) 
  {
// prs 20191221    globalData.LoadFonts(Green, Yellow, Red, Blue, Orange, KeyboardCharColor, BACKGROUND_FILL, BACKGROUND_FILL, CustomColor);  
    globalData.LoadSystemFonts(KeyboardCharColor, 
                               BACKGROUND_FILL, 
                               BACKGROUND_FILL, 
                               CustomColor);
    ProcInput.InitializeToDesignSelect();
    ProcInput.AllowInput(TRUE);  
    return TRUE;
  }

  if (!InitDesignData())
    return FALSE;

  HRESULT hRes = ::CoInitialize(NULL);
  if (!SUCCEEDED(hRes))
  {
    WriteDebugString("Failed to CoInitialize OLE\n");
    return FALSE;
  }
  

  return TRUE;
}

//*****************************************************************************
//    NAME: StopBackgroundMusic
//
// PURPOSE: 
//
//*****************************************************************************
void StopBackgroundMusic()
{
  if (pSndMgr == NULL) return;
  pSndMgr->StopBgndQueue();
}

//*****************************************************************************
//    NAME: StartBackgroundMusic
//
// PURPOSE: 
//
//*****************************************************************************
void StartBackgroundMusic()
{
  if ((pSndMgr != NULL) && (PlayEventSounds))
  {  
    if (!pSndMgr->GetMusicEnable())
    {
      pSndMgr->StopBgndQueue();
      return;
    };
    int currLevel = GetCurrentLevel();
   
    // There is no sense copying a LEVEL_STATUS structure just to make the reference shorter!!!
    // We will simply provide a reference.
    // PRS 20181216
    //LEVEL_STATS data;

    const LEVEL_STATS& data = globalData.levelInfo.stats[currLevel];
    ZONE zdata;

    if (currLevel < 0)
    {
      pSndMgr->StopBgndQueue();
      return;
    };
    pSndMgr->StopBgndQueue();
    int currZone = levelData.GetCurrZone(party.Posx, party.Posy);
    zdata = levelData.zoneData.zones[currZone];

    // See definition of 'data' above.   PRS20181216
    //data = globalData.levelInfo.stats[currLevel];

    // zone music over-rides level music
    // night music over-rides day music

    BOOL znight = (   (zdata.bgSounds.UseNightMusic) 
                   && (zdata.bgSounds.nightSounds.sounds.GetCount()>0)
                   && (IsTriggerTimeInCurrentTime(zdata.bgSounds.StartTime, zdata.bgSounds.EndTime)));

    BOOL zday = (zdata.bgSounds.backgroundSounds.sounds.GetCount()>0);


    BOOL lnight = (   (data.bgSounds.UseNightMusic) 
                   && (data.bgSounds.nightSounds.sounds.GetCount()>0)
                   && (IsTriggerTimeInCurrentTime(data.bgSounds.StartTime, data.bgSounds.EndTime)));
    
    BOOL lday = (data.bgSounds.backgroundSounds.sounds.GetCount()>0);


    if (znight)
    {
      POSITION pos = zdata.bgSounds.nightSounds.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(zdata.bgSounds.nightSounds.sounds.GetNext(pos), FALSE);
    }
    else if (lnight)
    {
      POSITION pos = data.bgSounds.nightSounds.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(data.bgSounds.nightSounds.sounds.GetNext(pos),TRUE);
    }
    else if (zday)
    {
      POSITION pos = zdata.bgSounds.backgroundSounds.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(zdata.bgSounds.backgroundSounds.sounds.GetNext(pos),FALSE);
    }
    else if (lday)
    {
      POSITION pos = data.bgSounds.backgroundSounds.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(data.bgSounds.backgroundSounds.sounds.GetNext(pos),TRUE);
    }

    pSndMgr->PlayBgndQueue();
  }  
}

// start zone music if available,
// else start level music if not playing
// else let level music continue playing
//
// called when zone changes, but not for
// level changes (teleport, etc), or for day/night changes
//*****************************************************************************
//    NAME: StartBackgroundZoneOnlyMusic
//
// PURPOSE: 
//
//*****************************************************************************
void StartBackgroundZoneOnlyMusic()
{
  BOOL playq = FALSE;

  if ((pSndMgr != NULL) && (PlayEventSounds))
  {     
    if (!pSndMgr->GetMusicEnable()) 
    {
      pSndMgr->StopBgndQueue();
      return;
    }
   
    LEVEL_STATS *pLevelData;
    ZONE zdata;

    int currLevel = GetCurrentLevel();
    int currZone = levelData.GetCurrZone(party.Posx, party.Posy);
    zdata = levelData.zoneData.zones[currZone];
    pLevelData = &globalData.levelInfo.stats[currLevel];

    // zone music over-rides level music
    // night music over-rides day music

    BOOL znight = (   (zdata.bgSounds.UseNightMusic) 
                   && (zdata.bgSounds.nightSounds.sounds.GetCount()>0)
                   && (IsTriggerTimeInCurrentTime(zdata.bgSounds.StartTime, zdata.bgSounds.EndTime)));

    BOOL zday = (zdata.bgSounds.backgroundSounds.sounds.GetCount()>0);


    BOOL lnight = (   (pLevelData->bgSounds.UseNightMusic) 
                   && (pLevelData->bgSounds.nightSounds.sounds.GetCount()>0)
                   && (IsTriggerTimeInCurrentTime(pLevelData->bgSounds.StartTime, pLevelData->bgSounds.EndTime)));
    
    BOOL lday = (pLevelData->bgSounds.backgroundSounds.sounds.GetCount()>0);


    if (znight)
    {
      pSndMgr->StopBgndQueue();
      POSITION pos = zdata.bgSounds.nightSounds.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(zdata.bgSounds.nightSounds.sounds.GetNext(pos),FALSE);
      playq=TRUE;
    }
    else if (zday)
    {
      pSndMgr->StopBgndQueue();
      POSITION pos = zdata.bgSounds.backgroundSounds.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(zdata.bgSounds.backgroundSounds.sounds.GetNext(pos),FALSE);
      playq=TRUE;
    }
    else
    {
      // no zone music to play, either start level music or let it
      // continue playing.

      if (pSndMgr->IsBgndQueueActive())
      {
        if (pSndMgr->IsBgndQueueLevelMusic())
        {
          // let level music continue playing
        }
        else
        {
          // must be zone music, stop it and start the level music.
          // if no level music, just stop the zone music
          pSndMgr->StopBgndQueue();
          if (lnight)
          {
            POSITION pos = pLevelData->bgSounds.nightSounds.sounds.GetHeadPosition();
            while (pos != NULL)
              pSndMgr->QueueBgndSound(pLevelData->bgSounds.nightSounds.sounds.GetNext(pos),TRUE);
            playq=TRUE;
          }
          else if (lday)
          {
            POSITION pos = pLevelData->bgSounds.backgroundSounds.sounds.GetHeadPosition();
            while (pos != NULL)
              pSndMgr->QueueBgndSound(pLevelData->bgSounds.backgroundSounds.sounds.GetNext(pos),TRUE);
            playq=TRUE;
          }
        }
      }
      else
      {
        if (lnight)
        {
          pSndMgr->StopBgndQueue();
          POSITION pos = pLevelData->bgSounds.nightSounds.sounds.GetHeadPosition();
          while (pos != NULL)
            pSndMgr->QueueBgndSound(pLevelData->bgSounds.nightSounds.sounds.GetNext(pos),TRUE);
          playq=TRUE;
        }
        else if (lday)
        {
          pSndMgr->StopBgndQueue();
          POSITION pos = pLevelData->bgSounds.backgroundSounds.sounds.GetHeadPosition();
          while (pos != NULL)
            pSndMgr->QueueBgndSound(pLevelData->bgSounds.backgroundSounds.sounds.GetNext(pos),TRUE);
          playq=TRUE;
        }
      }
    }

    if (playq)
      pSndMgr->PlayBgndQueue();
  }  
}
///////////////////////////////////////////////////////
// load surfaces used for combat
//
//*****************************************************************************
//    NAME: LoadOffscreenCombatSurfaces
//
// PURPOSE: 
//
//*****************************************************************************
BOOL LoadOffscreenCombatSurfaces(BOOL UseOutdoor)
{ 
  int i;
  for (i=0; i<MAX_WALLSETS;i++)
    WallSets[i].ClearResources();
  for (i=0; i<MAX_BACKGROUNDS;i++)
    BackgroundSets[i].ClearResources();

  if (pWallSlotMgr!=NULL) pWallSlotMgr->Clear();
  if (pBgSlotMgr!=NULL) pBgSlotMgr->Clear();

  itemData.FreeArt();
  spellData.FreeArt();
  //AnimateList.RemoveAll();
  ClearAnimateList();
  levelData.zoneData.ReleaseSurfaces();
  globalData.ReleaseSurfaces();
  GraphicsMgr.ReleaseSurfaceTypes(AllSurfTypes);
  
  ReleaseAllSounds();
  
  AREA_MAP_DATA spot = levelData.area[party.Posy][party.Posx]; 
   
  //
  // icon surfaces loaded in initCombatData()
  // for all combatants (pc,npc,monsters)
  //
  
// prs 20191221  globalData.LoadFonts(CombatGreen, CombatYellow, CombatRed, CombatBlue, CombatOrange, KeyboardCharColor, BACKGROUND_COMBAT_FILL, BACKGROUND_COMBAT_FILL, CustomColor);    
  globalData.LoadSystemFonts(KeyboardCharColor, 
                             BACKGROUND_COMBAT_FILL, 
                             BACKGROUND_COMBAT_FILL, 
                             CustomColor);

  LoadAreaViewArt(globalData.m_MapArt, globalData.MapArtSurf, globalData.UseAVZones);

  // figure out which tile image type to use
  if (UseOutdoor)
    CombatWallArt.name = levelData.zoneData.zones[spot.zone].outdoorCombatArt;
  else
    CombatWallArt.name = levelData.zoneData.zones[spot.zone].indoorCombatArt;
  HBarVPArt.surface = GraphicsMgr.AddFileSurface(HBarVPArt.name, CommonDib, rte.WindowArtDir());  
  if (HBarVPArt.surface < 0)
  {    
    WriteDebugString("HB Art %s not found\n",HBarVPArt.name);
    //if (HBarVPArt.name.Find(DEFAULT_HB)<0)
    if (HBarVPArt.name.CompareNoCase(DEFAULT_HB)!=0)
    { 
      //HBarVPArt.name = rte.WindowArtDir() + DEFAULT_HB;
      HBarVPArt.name = DEFAULT_HB;
      WriteDebugString("HB Art now trying default %s\n", HBarVPArt.name);
      HBarVPArt.surface = GraphicsMgr.AddFileSurface(HBarVPArt.name, CommonDib, rte.WindowArtDir());
      if (HBarVPArt.surface < 0)
        WriteDebugString("HB Art default also failed\n");
    }
  }  

  if (HBarVPArt.surface >= 0)
    CheckMultHorzBarFormat();
  VBarVPArt.surface = GraphicsMgr.AddFileSurface(VBarVPArt.name, CommonDib, rte.WindowArtDir());
  if (VBarVPArt.surface < 0)
  {    
    WriteDebugString("VB Art %s not found\n",VBarVPArt.name);
    //if (VBarVPArt.name.Find(DEFAULT_VB)<0)
    if (VBarVPArt.name.CompareNoCase(DEFAULT_VB)!=0)
    {      
      //VBarVPArt.name = rte.WindowArtDir() + DEFAULT_VB;
      VBarVPArt.name = DEFAULT_VB;
      WriteDebugString("VB Art now trying default %s\n", VBarVPArt.name);
      VBarVPArt.surface = GraphicsMgr.AddFileSurface(VBarVPArt.name, CommonDib, rte.WindowArtDir());
      if (VBarVPArt.surface < 0)
        WriteDebugString("VB Art default also failed\n");
    }
  }  

  FrameVPArt.surface = GraphicsMgr.AddFileSurface(FrameVPArt.name, CommonDib, rte.WallArtDir());

  if (FrameVPArt.surface < 0)
  {    
    WriteDebugString("Frame Art %s not found\n",FrameVPArt.name);
    //if (FrameVPArt.name.Find(DEFAULT_FR)<0)
    if (FrameVPArt.name.CompareNoCase(DEFAULT_FR)!=0)
    {      
      FrameVPArt.name = DEFAULT_FR;
      WriteDebugString("Frame Art now trying default %s\n", FrameVPArt.name);
      FrameVPArt.surface = GraphicsMgr.AddFileSurface(FrameVPArt.name, CommonDib, rte.WindowArtDir());
      if (FrameVPArt.surface < 0)
        WriteDebugString("Frame Art default also failed\n");
    }
  }  

  CharViewFrameVPArt.surface = GraphicsMgr.AddFileSurface(CharViewFrameVPArt.name, CommonDib, rte.WallArtDir());

  if (CharViewFrameVPArt.surface < 0)
  {
      WriteDebugString("Character View Frame Art %s not found\n", CharViewFrameVPArt.name);
      //if (FrameVPArt.name.Find(DEFAULT_FR)<0)
      if (CharViewFrameVPArt.name.CompareNoCase(DEFAULT_FR) != 0)
      {
          CharViewFrameVPArt.name = DEFAULT_FR;
          WriteDebugString("Character View Frame Art now trying default %s\n", CharViewFrameVPArt.name);
          CharViewFrameVPArt.surface = GraphicsMgr.AddFileSurface(CharViewFrameVPArt.name, CommonDib, rte.WindowArtDir());
          if (CharViewFrameVPArt.surface < 0)
              WriteDebugString("Character View Character View Frame Art default also failed\n");
      }
  }

  CombatCursorArt.surface = GraphicsMgr.AddFileSurface(CombatCursorArt.name, CommonDib, rte.CursorArtDir());
  if (CombatCursorArt.surface < 0)
  {    
    WriteDebugString("ComCurs Art %s not found\n",CombatCursorArt.name);
    //if (CombatCursorArt.name.Find(DEFAULT_CC)<0)
    if (CombatCursorArt.name.CompareNoCase(DEFAULT_CC)!=0)
    {      
      //CombatCursorArt.name = rte.CursorArtDir() + DEFAULT_CC;
      CombatCursorArt.name = DEFAULT_CC;
      WriteDebugString("ComCurs Art now trying default %s\n", CombatCursorArt.name);
      CombatCursorArt.surface = GraphicsMgr.AddFileSurface(CombatCursorArt.name, CommonDib, rte.CursorArtDir());
      if (CombatCursorArt.surface < 0)
        WriteDebugString("ComCurs Art default also failed\n");
    }
  }  

  CombatDeathIconArt.surface = GraphicsMgr.AddFileSurface(CombatDeathIconArt.name, IconDib, rte.IconArtDir());
  if (CombatDeathIconArt.surface < 0)
  {    
    WriteDebugString("ComDthIco Art %s not found\n",CombatDeathIconArt.name);
    //if (CombatDeathIconArt.name.Find(DEFAULT_CDI)<0)
    if (CombatDeathIconArt.name.CompareNoCase(DEFAULT_CDI)!=0)
    {      
      //CombatDeathIconArt.name = rte.IconArtDir() + DEFAULT_CDI;
      CombatDeathIconArt.name = DEFAULT_CDI;
      WriteDebugString("ComDthIco Art now trying default %s\n", CombatDeathIconArt.name);
      CombatDeathIconArt.surface = GraphicsMgr.AddFileSurface(CombatDeathIconArt.name, IconDib, rte.IconArtDir());
      if (CombatDeathIconArt.surface < 0)
        WriteDebugString("ComDthIco Art default also failed\n");
    }
  }  

  CombatPetrifiedIconArt.surface = GraphicsMgr.AddFileSurface(CombatPetrifiedIconArt.name, IconDib, rte.IconArtDir());
  if (CombatPetrifiedIconArt.surface < 0)
  {    
    WriteDebugString("ComPtrIco Art %s not found\n",CombatPetrifiedIconArt.name);
    //if (CombatPetrifiedIconArt.name.Find(DEFAULT_CPI)<0)
    if (CombatPetrifiedIconArt.name.CompareNoCase(DEFAULT_CPI)!=0)
    {      
      //CombatPetrifiedIconArt.name = rte.IconArtDir() + DEFAULT_CPI;
      CombatPetrifiedIconArt.name = DEFAULT_CPI;
      WriteDebugString("ComPtrIco Art now trying default %s\n", CombatPetrifiedIconArt.name);
      CombatPetrifiedIconArt.surface = GraphicsMgr.AddFileSurface(CombatPetrifiedIconArt.name, IconDib, rte.IconArtDir());
      if (CombatPetrifiedIconArt.surface < 0)
        WriteDebugString("ComPtrIco Art default also failed\n");
    }
  }  

  CombatDeathArt.surface = GraphicsMgr.AddFileSurface(CombatDeathArt.name, CommonDib, rte.CombatArtDir());
  if (CombatDeathArt.surface < 0)
  {    
    WriteDebugString("ComDthArt Art %s not found\n",CombatDeathArt.name);
    //if (CombatDeathArt.name.Find(DEFAULT_CD)<0)
    if (CombatDeathArt.name.CompareNoCase(DEFAULT_CD)!=0)
    {      
      //CombatDeathArt.name = rte.CombatArtDir() + DEFAULT_CD;
      CombatDeathArt.name = DEFAULT_CD;
      WriteDebugString("ComDthArt Art now trying default %s\n", CombatDeathArt.name);
      CombatDeathArt.surface = GraphicsMgr.AddFileSurface(CombatDeathArt.name, CommonDib, rte.CombatArtDir());
      if (CombatDeathArt.surface < 0)
        WriteDebugString("ComDthArt Art default also failed\n");
    }
  }  
  CombatWallArt.surface = GraphicsMgr.AddFileSurface(CombatWallArt.name, CombatDib, rte.CombatArtDir());      
  if (CombatWallArt.surface < 0)
  {    
    WriteDebugString("ComWallArt Art %s not found\n",CombatWallArt.name);
    //if (CombatWallArt.name.Find(DEFAULT_CWD)<0)
    if (CombatWallArt.name.CompareNoCase(DEFAULT_CWD)!=0)
    {  
      //CombatWallArt.name = rte.WallArtDir() + DEFAULT_CWD;
      CombatWallArt.name = DEFAULT_CWD;
      WriteDebugString("ComWallArt Art now trying default %s\n", CombatWallArt.name);
      CombatWallArt.surface = GraphicsMgr.AddFileSurface(CombatWallArt.name, CommonDib, rte.CombatArtDir());
      if (CombatWallArt.surface < 0)
        WriteDebugString("ComWallArt Art default also failed\n");
    }
  }  
  VBarCombArt.surface = GraphicsMgr.AddFileSurface(VBarCombArt.name, CommonDib, rte.CombatArtDir());
  if (VBarCombArt.surface < 0)
  {    
    WriteDebugString("VB CombArt %s not found\n",VBarCombArt.name);
    //if (VBarCombArt.name.Find(DEFAULT_VB)<0)
    if (VBarCombArt.name.CompareNoCase(DEFAULT_VB)!=0)
    {      
      //VBarCombArt.name = rte.CombatArtDir() + DEFAULT_VB;
      VBarCombArt.name = DEFAULT_VB;
      WriteDebugString("VB CombArt now trying default %s\n", VBarCombArt.name);
      VBarCombArt.surface = GraphicsMgr.AddFileSurface(VBarCombArt.name, CommonDib, rte.CombatArtDir());
      if (VBarCombArt.surface < 0)
        WriteDebugString("VB CombArt default also failed\n");
    }
  }  
  HBarCombArt.surface = GraphicsMgr.AddFileSurface(HBarCombArt.name, CommonDib,rte.CombatArtDir());
  if (HBarCombArt.surface < 0)
  {    
    WriteDebugString("HB CombArt %s not found\n",HBarCombArt.name);
    //if (HBarCombArt.name.Find(DEFAULT_HB)<0)
    if (HBarCombArt.name.CompareNoCase(DEFAULT_HB)!=0)
    {      
      //HBarCombArt.name = rte.CombatArtDir() + DEFAULT_HB;
      HBarCombArt.name = DEFAULT_HB;
      WriteDebugString("HB CombArt now trying default %s\n", HBarCombArt.name);
      HBarCombArt.surface = GraphicsMgr.AddFileSurface(HBarCombArt.name, CommonDib,rte.CombatArtDir());
      if (HBarCombArt.surface < 0)
        WriteDebugString("HB CombArt default also failed\n");
    }
  }  

  //if (HBarCombArt.surface >= 0)
  //  CheckMultHorzBarFormat();
  
  globalData.sounds.hCharHit = LoadSound(globalData.sounds.CharHit);
  if (globalData.sounds.hCharHit < 0)
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load combat char hit sound %s\n",globalData.sounds.CharHit);
  };

  globalData.sounds.hCharMiss = LoadSound(globalData.sounds.CharMiss);
  if (globalData.sounds.hCharMiss < 0)
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load combat char miss sound %s\n",globalData.sounds.CharMiss);
  };

  BOOL success = TRUE;

  if (  (CombatWallArt.surface < 0)
    || (CombatCursorArt.surface < 0)
    || (CombatDeathIconArt.surface < 0)
    || (CombatPetrifiedIconArt.surface < 0)
    || (CombatDeathArt.surface < 0)
    || (VBarCombArt.surface < 0)
    || (HBarCombArt.surface < 0))
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load combat art\n");
    success = FALSE;
  }
    
  if ((FrameVPArt.surface < 0) || (HBarVPArt.surface < 0) || (VBarVPArt.surface < 0) || (CharViewFrameVPArt.surface < 0))
  {
    debugSeverity = 5;
    WriteDebugString("Failed to load all combat viewport art\n");
    success = FALSE;
  }
    
  return success;
}

///////////////////////////////////////////////////////
// load surfaces used for a level
//
//*****************************************************************************
//    NAME: LoadOffscreenWalkingSurfaces
//
// PURPOSE: 
//
//*****************************************************************************
BOOL LoadOffscreenWalkingSurfaces()
{ 
  int i;
  for (i=0; i<MAX_WALLSETS;i++)
    WallSets[i].ClearResources();
  for (i=0; i<MAX_BACKGROUNDS;i++)
    BackgroundSets[i].ClearResources();

  if (pWallSlotMgr!=NULL) pWallSlotMgr->Clear();
  if (pBgSlotMgr!=NULL) pBgSlotMgr->Clear();
  
  itemData.FreeArt();
  spellData.FreeArt();
  //AnimateList.RemoveAll();
  ClearAnimateList();
  levelData.zoneData.ReleaseSurfaces();
  globalData.ReleaseSurfaces();
  GraphicsMgr.ReleaseSurfaceTypes(AllSurfTypes);
  
  globalData.sounds.ClearSounds();  
  itemData.ClearSounds();
  spellData.ClearSounds();
  monsterData.ClearSounds();
  ReleaseAllSounds();
  
// PRS 20191221  globalData.LoadFonts(Green, Yellow, Red, Blue, Orange, KeyboardCharColor, BACKGROUND_FILL, BACKGROUND_FILL, CustomColor);  
  globalData.LoadSystemFonts(KeyboardCharColor, 
                             BACKGROUND_FILL, 
                             BACKGROUND_FILL, 
                             CustomColor);

  BOOL success = TRUE;

  if (!loadWallSetSurfaces())
    success = FALSE;
  
  // level sounds over-ride global sounds for step and bump
  CString tmp;
  if (globalData.levelInfo.stats[globalData.currLevel].BumpSound != "")
    tmp = globalData.levelInfo.stats[globalData.currLevel].BumpSound;
  else
    tmp = globalData.sounds.PartyBump;
  if (!tmp.IsEmpty())
  {
    globalData.sounds.hPartyBump = LoadSound(tmp);

    if (globalData.sounds.hPartyBump < 0)
    {
      debugSeverity = 5;
      WriteDebugString("Failed to load global party bump sound '%s'\n",tmp);
    };
  };

  if (globalData.levelInfo.stats[globalData.currLevel].StepSound != "")
    tmp = globalData.levelInfo.stats[globalData.currLevel].StepSound;
  else
    tmp = globalData.sounds.PartyStep;
  if (!tmp.IsEmpty())
  {
    globalData.sounds.hPartyStep = LoadSound(tmp);

    if (globalData.sounds.hPartyStep < 0)
    {
      debugSeverity = 5;
      WriteDebugString("Failed to load global party step sound '%s'\n",tmp);
    };
  };

  LoadAreaViewArt(globalData.m_MapArt, globalData.MapArtSurf, globalData.UseAVZones);

  if (globalData.BackgroundArt != "")
  {
    globalData.BackgroundSurf = GraphicsMgr.AddFileSurface(globalData.BackgroundArt, TransBufferDib, rte.BackgroundArtDir());
    if (globalData.BackgroundSurf < 0)
    {
      if (!debugStrings.AlreadyNoted(CString("FTLBS02")+globalData.BackgroundArt))
      {
        debugSeverity = 5;
        WriteDebugString("Failed to load background art: %s\n", globalData.BackgroundArt);
      };
    };
  }
  else
    globalData.BackgroundSurf=-1;

  HBarVPArt.surface = GraphicsMgr.AddFileSurface(HBarVPArt.name, CommonDib, rte.WindowArtDir());
  if (HBarVPArt.surface < 0)
  {    
    WriteDebugString("HB Art %s not found\n",HBarVPArt.name);
    if (HBarVPArt.name.CompareNoCase(DEFAULT_HB)!=0)
    {      
      HBarVPArt.name = DEFAULT_HB;
      WriteDebugString("HB Art now trying default %s\n", HBarVPArt.name);
      HBarVPArt.surface = GraphicsMgr.AddFileSurface(HBarVPArt.name, CommonDib, rte.WindowArtDir());
      if (HBarVPArt.surface < 0)
        WriteDebugString("HB Art default also failed\n");
    }
  }  

  if (HBarVPArt.surface >= 0)
    CheckMultHorzBarFormat();

  VBarVPArt.surface = GraphicsMgr.AddFileSurface(VBarVPArt.name, CommonDib, rte.WindowArtDir());

  if (VBarVPArt.surface < 0)
  {    
    WriteDebugString("VB Art %s not found\n",VBarVPArt.name);
    if (VBarVPArt.name.CompareNoCase(DEFAULT_VB)!=0)
    {      
      VBarVPArt.name = DEFAULT_VB;
      WriteDebugString("VB Art now trying default %s\n", VBarVPArt.name);
      VBarVPArt.surface = GraphicsMgr.AddFileSurface(VBarVPArt.name, CommonDib, rte.WindowArtDir());
      if (VBarVPArt.surface < 0)
        WriteDebugString("VB Art default also failed\n");
    }
  }  

  FrameVPArt.surface = GraphicsMgr.AddFileSurface(FrameVPArt.name, CommonDib, rte.WindowArtDir());

  if (FrameVPArt.surface < 0)
  {    
    WriteDebugString("Frame Art %s not found\n",FrameVPArt.name);
    if (FrameVPArt.name.CompareNoCase(DEFAULT_FR)!=0)
    {      
      FrameVPArt.name = DEFAULT_FR;
      WriteDebugString("Frame Art now trying default %s\n", FrameVPArt.name);
      FrameVPArt.surface = GraphicsMgr.AddFileSurface(FrameVPArt.name, CommonDib, rte.WindowArtDir());
      if (FrameVPArt.surface < 0)
        WriteDebugString("Frame Art default also failed\n");
    }
  }  

  CharViewFrameVPArt.surface = GraphicsMgr.AddFileSurface(CharViewFrameVPArt.name, CommonDib, rte.WindowArtDir());

  if (CharViewFrameVPArt.surface < 0)
  {
      WriteDebugString("Character View Frame Art %s not found\n", CharViewFrameVPArt.name);
      if (CharViewFrameVPArt.name.CompareNoCase(DEFAULT_FR) != 0)
      {
          CharViewFrameVPArt.name = DEFAULT_FR;
          WriteDebugString("Character ViewFrame Art now trying default %s\n", CharViewFrameVPArt.name);
          CharViewFrameVPArt.surface = GraphicsMgr.AddFileSurface(CharViewFrameVPArt.name, CommonDib, rte.WindowArtDir());
          if (CharViewFrameVPArt.surface < 0)
              WriteDebugString("Character ViewFrame Art default also failed\n");
      }
  }
  if (   (FrameVPArt.surface < 0) || (HBarVPArt.surface < 0) 
      || (VBarVPArt.surface < 0)  || (globalData.MapArtSurf < 0)
      || (CharViewFrameVPArt.surface < 0))
  {
    debugSeverity = 6;
    WriteDebugString("Failed to load all viewport art\n");
    success = FALSE;
  }
   
  return success;
}

///////////////////////////////////////
//  Game Scheduler
///////////////////////////////////////

//*****************************************************************************
//    NAME: scheduler
//
// PURPOSE: 
//
//*****************************************************************************
void scheduler()
{  
  // An HWND was needed when the input was
  // processed using DirectInput, but is
  // not needed for the current input system.
  //
  //static HWND GameWnd=NULL;

  if (IsShutdownSignaled())
  {
    ExitSignaled = 1;
    //gpdlCleanup();
  };
  
  //if (GameWnd == NULL)
  //  GameWnd = AfxGetMainWnd()->GetSafeHwnd();
  DETAILTRACE(0x8a32,0);
  ProcInput.ProcessInput(NULL);
  DETAILTRACE(0x833,0);
}


//*****************************************************************************
//    NAME: ShutdownGame
//
// PURPOSE: 
//
//*****************************************************************************
void ShutdownGame(void)
{   
  SignalShutdown();
  //GraphicsMgr.StopMovie();
  ProcInput.AllowInput(FALSE);
  taskList.Clear();
  tempChars.Clear();
  party.Clear();  
  character.Clear();
  clearDesign(FALSE);
  combatData.Clear();
  itemData.Clear();
  monsterData.Clear();
  spellData.Clear();
  currPic.Clear();
  ClearFormattedText(textData);
  activeSpellList.xClear();
  pendingSpellList.Clear();
  ClearAnimateList();
  
  if (terrain != NULL)
  {
    for (int i = 0; i< MAX_TERRAIN_HEIGHT; i++)
      My_free(terrain[i]);
    My_free(terrain); 
  }
  terrain=NULL;

  StopAllSounds();
  ReleaseAllSounds();

  if (pSndMgr != NULL)
  {
    delete pSndMgr;
    pSndMgr = NULL;
  }  
  
  MouseRender.ExitInstance();
  GraphicsMgr.Clear();

 ::CoUninitialize();

  if (pWallSlotMgr!=NULL)
    delete pWallSlotMgr;
  if (pBgSlotMgr!=NULL)
    delete pBgSlotMgr;
  pWallSlotMgr=NULL;
  pBgSlotMgr=NULL;
}


static int mallocSequence=0;
void *My_malloc(int size)
{
  int *result;
  result = (int *)malloc(size + sizeof (int));
  if (result == NULL)
  {
    exit(1);
  };
  *result = ++mallocSequence;
  return (void *)(result + 1);
}

void My_free(void *addr)
{
  free((int *)addr - 1);
}

void *My_realloc(void *addr, int size)
{
  int *result;
  if (addr == NULL) return My_malloc(size);
  result = (int *)realloc((int *)addr-1, size + sizeof (int));
  if (result == NULL)
  {
    exit(1);
  };
  *result = ++mallocSequence;
  return (void *)(result + 1);
}
 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Old source follows. Not used, but keep around for now
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
// facingDirection - Determine if player at source coordinates is facing the
//                   destination coordinates.
// inputs:  sourcex = source X coordinate.
//          sourcey = source Y coordinate.
//          destx   = destination X coordinate.
//          desty   = destination Y coordinate.
//          sourceface = facing of source at source coordinates.
//////////////////////////////////////////////////////////////////////////////////
/*
BOOL facingDirection(int sourcex, int sourcey, int destx, int desty, int sourceface)
{
  
  double pi = 3.1415926535;
  
  double  slope, radian;
  
  // Make destx and desty become the deltas in the x & y directions.
  destx = destx - sourcex;
  desty = desty - sourcey;
  // Make sourcex and sourcey relative to zero.
  sourcex = 0;
  sourcey = 0;
  
  // If source and destination are in the same square then must be able
  // to see it.
  if (destx == 0 && desty == 0)
    return TRUE;
  
  // If source x and destination x are equal then the slope will be zero.
  // Zero is a big no-no with arctangent.
  if (destx != sourcex)
  {
    // Slope is either positive or negative.
    slope = double(desty - sourcey) / double(destx - sourcex);
    
    // radian will be the arctangent of the slope. Remember this value
    // is returned as a radian. 
    radian = atan(slope);
    
    // Check to see if source y and destination y are the same.  Basically
    // if they are the same then the radian will be returned as a zero.
    if (radian != 0)
    {
      if (destx < sourcex) // Destination is in quadrant 2 or 3.
      {
        if (desty < sourcey) // Destination is in quadrant 2.
          radian = pi-radian;
        else
          radian = (radian-(2*radian))+pi; //Destination is quadrant 3.
      }
      else
      {
        // Destination is in quadrant 1 or 4.
        if (desty < sourcey)
          // Destination is in quadrant 1
          radian = radian-(2*radian);                
        else
          // Destination is in quadrant 4.
          radian = (2*pi) - radian;
      }
    }
    else
    {
      // If radian is zero, then both points have the 
      // same y coordinate.  Find out which quadrant
      // other point is.
      if (destx < sourcex)
        radian = pi;
    }
  }
  else
  {
    // destx and sourcex are the same.  The radian can only be 2 values
    // depending in the relative delta between the y coordinates.
    if (desty < sourcey)
      radian = pi / 2;
    else
      radian = (3 * pi) /2;
  }
  
  // Now the radian from source to destination is known.  Depending on the facing
  // of the source player we can determine if source is facing destination.
  switch(sourceface)
  {
  case FACE_NORTH:
    if (radian <= pi && radian >= 0)
      return TRUE;
    break;
    
  case FACE_EAST:
    if ((radian <= (pi / 2) && radian >= 0) || (radian <= (2 * pi) && radian >= ((3*pi)/2)))
      return TRUE;
    break;
    
  case FACE_SOUTH:
    if ((radian <= (2 * pi) && radian >= pi) || radian == 0)
      return TRUE;
    break;
    
  case FACE_WEST:
    if (radian <= ((3*pi)/2) && radian >= (pi/2))
      return TRUE;
    break;
  }
  
  // Source is not facing destination.
  return FALSE;
}
*/