/******************************************************************************
* Filename: Combatants.cpp
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
#include "..\Shared\stdafx.h"

//#include "externs.h"
#include "Dungeon.h"

#include <math.h>
#include "char.h"
#include "GameEvent.h"
#include "party.h"
#include "Combatants.h"
#include "DrawTile.h"
#include "FileParse.h"
#include "SoundMgr.h"
#include "Viewport.h"
#include "Screen.h"
#include "GlobalData.h"
#include "GameMenu.h"
#include "FormattedText.h"


extern int setPartyXY_x;
extern BOOL logDebuggingInfo;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
//  FILE *f;
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


BOOL IsLineOfSight(int x0, int y0, int x1, int y1);
void die(int errorNum);
int ScriptAtoI(const char **p, int min, int max);
int IntegerTableLookup(const CString& SAname, 
                       const CString& tableName, 
                       int   iValue,
                       const CString& function);


extern const CString Skill_Turn;
extern const CString SA_Turn;



MonsterArrangement monsterArrangement;

COMBAT_DATA combatData;
COMBAT_MSGS combatMsgs;

char *CombatantsStateText[NUM_COMBATANTS_STATES] = 
{
  "OCS_None",
  "OCS_Casting",
  "OCS_Attacking", 
  "OCS_Guarding",
  "OCS_Bandaging", 
  "OCS_Using",
  "OCS_Moving",
  "OCS_Turning",
  "OCS_Fleeing",
  "OCS_CombatRoundDelay",
  "OCS_CombatOver",
  "OCS_Delaying",
  "OCS_EndingTurn",
  "OCS_ForcingWin",
  "OCS_StartNewRound",
  "OCS_ActivateSpell",
  "OCS_Fled"
};

void StartCombatSummary(void);

//*****************************************************************************
// NAME: InitCombatData
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::InitCombatData(COMBAT_EVENT_DATA *event)
{
  if (event==NULL) return;
  Clear();
  StartCombatSummary();

  itemData.FreeArt();
  spellData.FreeArt();
  monsterData.ClearSounds();
  itemData.ClearSounds();
  spellData.ClearSounds();
  pathMgr.Clear();
  ClearEmptyCellSearch();
 
  m_bCombatActive = TRUE;
  m_bIsWilderness = event->m_UseOutdoorMap;

  m_combatRect.left   = CombatScreenX;
  m_combatRect.top    = CombatScreenY;
  //m_combatRect.right  = COMBAT_SCREEN_WIDTH;
  //m_combatRect.bottom = COMBAT_SCREEN_HEIGHT;
  m_combatRect.right  = CombatScreenX + TILES_HORZ*COMBAT_TILE_WIDTH;
  m_combatRect.bottom = CombatScreenY + TILES_VERT*COMBAT_TILE_HEIGHT;


  m_fullRect.left = 0;
  m_fullRect.top = 0;
  m_fullRect.right = SCREEN_WIDTH;
  m_fullRect.bottom = SCREEN_HEIGHT;

  ConfigFile.FindToken("NO_CAP_ON_TURN_UNDEAD_BY_TOTAL_ROLL", m_bNoCapOnTurnUndeadByTotalRoll, true);

  if (logDebuggingInfo)
  {
    ConfigFile.FindToken("Monster_NoMove", m_bMonsterNoMove, false); // don't remove these tokens
    ConfigFile.FindToken("Monster_AlwaysMiss", m_bMonsterAlwaysMiss, false);
    ConfigFile.FindToken("Monster_AlwaysHit", m_bMonsterAlwaysHit, false);
    ConfigFile.FindToken("Monster_NoCast", m_bMonsterNoCast, false);
    ConfigFile.FindToken("Monster_PlyrControl", m_bMonsterPlyrControl, false);
    ConfigFile.FindToken("Monster_NoDeath", m_bMonsterNoDeath, false);
    ConfigFile.FindToken("Party_AlwaysMiss", m_bPartyAlwaysMiss, false);
    ConfigFile.FindToken("Party_AlwaysHit", m_bPartyAlwaysHit, false);
    WriteDebugString("Combat Debug Settings:\n");
    WriteDebugString("       MonsNoMove = %u\n", m_bMonsterNoMove);
    WriteDebugString("    MonsAlwaysHit = %u\n", m_bMonsterAlwaysHit);
    WriteDebugString("   MonsAlwaysMiss = %u\n", m_bMonsterAlwaysMiss);
    WriteDebugString("       MonsNoCast = %u\n", m_bMonsterNoCast);
    WriteDebugString("      MonsNoDeath = %u\n", m_bMonsterNoDeath);
    WriteDebugString("  MonsPlyrControl = %u\n", m_bMonsterPlyrControl);
    WriteDebugString("  PartyAlwaysMiss = %u\n", m_bPartyAlwaysMiss);
    WriteDebugString("   PartyAlwaysHit = %u\n\n", m_bPartyAlwaysHit);
    WriteDebugString("     MapTileWidth = %u\n", TILES_HORZ);
    WriteDebugString("    MapTileHeight = %u\n\n", TILES_VERT);
  };
  m_bPartyNeverDies     = event->partyNeverDies;
  m_bPartyNoExperience  = event->partyNoExperience;
  m_bNoMagic            = event->noMagic;
  m_eSurprise           = event->m_eTEMPsurprise;
  m_eUndeadTurnModifier = event->turningMod;

  m_pEvent = event;
  //WriteDebugString("109 Calling LoadSounds()\n");
  LoadSounds();

  m_iPartyStartX = party.Posx;
  m_iPartyStartY = party.Posy;

  // this function adjusts the start x,y to reflect
  // x,y in combat terrain map
  if (!m_bIsWilderness)
    GenerateIndoorCombatMap(m_iPartyStartX, m_iPartyStartY, EventDirToPathDir(event->direction));
  else
    GenerateOutdoorCombatMap(m_iPartyStartX, m_iPartyStartY, EventDirToPathDir(event->direction));

  m_iStartTerrainX = 0;
  m_iStartTerrainY = 0;
  if (OBSTICAL_none != ObsticalType(m_iPartyStartX, m_iPartyStartY,1,1,FALSE, TRUE, NULL))
  {
    WriteDebugString("Starting x,y is not empty\n");
    die(0xab4cb);
  }

  m_iCurrInitiative = 1;  // Characters start with initiative=0 so we will need
                          // to start a new round before we can begin.
  m_iCurrRound = 0;
  m_iCurrTurn=0;
  m_bLastRound = FALSE;
  m_bCombatOver = FALSE;
  m_bStartingNewCombatRound = TRUE;
  
  
  int monsterCount; 
  monsterDistance = m_pEvent->distance;
  for (;;)  // Try distances until at least one monster in sincluded
  {
    int i;
    // Add all combatants to combatData.m_aCombatants.
    AddCombatants();
    for (i=0; i<m_iNumCombatants; i++)
    {
      WriteDebugString("DEBUG - Combatant %d starts at (%d,%d)\n", i, m_aCombatants[i].x, m_aCombatants[i].y);
    };

    // Position each combatant on the map.
    determineInitCombatPos();

    for (i=0; i<m_iNumCombatants; i++)
    {
      //WriteDebugString("DEBUG - Combatant %d starts at (%d,%d)\n", i, m_aCombatants[i].x, m_aCombatants[i].y);
    };
    if (logDebuggingInfo)
    {
      WriteDebugString("Finished placing combatants on map\n");
    };
    CheckAllAuraPlacements(NULL);
    // make sure every monster can reach the party
    pathMgr.SetPathSize(1,1); // 1x1 good enough to let party reach
    for (i=0; i<NumCombatants()/*m_aCombatants.GetSize()*/; i++)
    {      
      if (!m_aCombatants[i].IsPartyMember())
      {
        int hPath;
        if (   (m_aCombatants[i].x < 0)
            || (hPath = pathMgr.GetPath(m_aCombatants[i].x, m_aCombatants[i].y, 
                                        m_iPartyStartX, m_iPartyStartY, 
                                        m_iPartyStartX, m_iPartyStartY,
                                        FALSE, NULL, FALSE)) < 0)
        {
          if ((m_aCombatants[i].x != m_iPartyStartX) || (m_aCombatants[i].y != m_iPartyStartY))
          {
            // no path from monster to party, so remove monster.
              // Remove unreachable monster from combat map.
            placeCombatant(m_aCombatants[i].x, m_aCombatants[i].y,
                          NO_DUDE, 
                          m_aCombatants[i].width,
                          m_aCombatants[i].height);
     
            // Remove unreachable monster from array combatData.m_aCombatants
            // by marking position negative.
            m_aCombatants[i].x = -1;
            WriteDebugString("Combatant[%d] removed because party could not reach him\n", i);
          };
        }
          else
            pathMgr.FreePath(hPath);
      }
      //else
        //  WriteDebugString("origIndex %i, origKey %i\n", combatants[i].origIndex, combatants[i].origKey);
    }   
    // some combatants may not have been placed on the map
  
    // transfer valid combatants to temp
    int count = 0;
  
    for (i=0; i<NumCombatants()/*m_aCombatants.GetSize()*/; i++)
    {
      //WriteDebugString("DEBUG - Compatant %d starts at (%d,%d)\n", i, m_aCombatants[i].x, m_aCombatants[i].y);
      if (m_aCombatants[i].x >= 0)
      {
        {
          // We need to protect the m_pCharacter <-> m_pCombatant links so that 
          // they are consistently one-to-one.  The COMBATANT copy operator
          // will refuse to copy non-NULL links.
          CHARACTER *pSaveCharPointer;
          pSaveCharPointer = m_aCombatants[i].m_pCharacter;
          m_aCombatants[i].m_pCharacter = NULL;
          // Copy a NULL pointer to m_aTempCOMBATANTS
          m_aTempCOMBATANTS.SetAt(count, m_aCombatants[i]);
          // Move the pointer to m_aTempCOMBATANTS
          // Make sure they point at each other.
          m_aTempCOMBATANTS[count].m_pCharacter = pSaveCharPointer;
          pSaveCharPointer->m_pCombatant = &m_aTempCOMBATANTS[i];
        };
        count++;
      }
      else
      {
        CHARACTER *pSaveCharPointer;
        COMBATANT *pSaveCombatantPointer;
        pSaveCombatantPointer = &m_aCombatants[i];
        pSaveCharPointer = pSaveCombatantPointer->m_pCharacter;
        if (pSaveCharPointer->m_pCombatant != pSaveCombatantPointer)
        {
          die(0x3334d);
        };
        pSaveCharPointer->m_pCombatant = NULL;
        if (pSaveCombatantPointer->deleteChar)
        {
          delete pSaveCharPointer;
        };
        pSaveCombatantPointer->m_pCharacter = NULL;
      };
      //WriteDebugString("DEBUG - Compatant %d placed at (%d,%d)\n", i, m_aCombatants[i].x, m_aCombatants[i].y);
    }

    // check to see if any were left out
    if (count != NumCombatants()/*m_aCombatants.GetSize()*/)
    {
      TRACE("Adjusting combatant list from %i to %i\n",
                      NumCombatants()/*m_aCombatants.GetSize()*/, count);
      // now we need to clean up the self index
      // and the terrain references to existing combatants
      for (i=0; i<count; i++)
      {
        if (m_aTempCOMBATANTS[i].self != i)
        {
          // this one needs updating
          m_aTempCOMBATANTS[i].self = i;
          placeCombatant(m_aTempCOMBATANTS[i].x, m_aTempCOMBATANTS[i].y,
                         m_aTempCOMBATANTS[i].self, 
                         m_aTempCOMBATANTS[i].width,
                         m_aTempCOMBATANTS[i].height);
        }
      }
    }
    m_iNumCombatants = count; //m_aCombatants.GetSize();


    // now move corrected data from Temp back into combatants
    count = 0;
    for (i=0; i<m_iNumCombatants; i++)
    {
      CHARACTER *pSaveCharPointer;
      // Make sure the combatant <-> character links remain one-to-one
      pSaveCharPointer = m_aTempCOMBATANTS[i].m_pCharacter;
      m_aTempCOMBATANTS[i].m_pCharacter = NULL;
      m_aCombatants.SetAt(count, m_aTempCOMBATANTS[i]);
      // Make sure they point at each other.
      pSaveCharPointer->m_pCombatant = &m_aCombatants[count];
      m_aCombatants[count].m_pCharacter = pSaveCharPointer;
      TRACE("Start Data for combatant %i: self %i, x %i, y %i, f %i\n",
                      count, 
                      m_aCombatants[count].self,
                      m_aCombatants[count].x,
                      m_aCombatants[count].y,
                      m_aCombatants[count].GetIsFriendly());
      count++;
    };
    if (logDebuggingInfo)
    {
      WriteDebugString("Total combatants = %i\n", m_iNumCombatants);
    };
    monsterCount = 0;
    // Let us count the monsters.
    for (i=0; i<m_iNumCombatants; i++)
    {
      if (!m_aCombatants[i].IsPartyMember()) monsterCount++;
    };

    if ((monsterCount!=0) || (monsterDistance==UpClose)) break;
    RemoveAllCombatants();
    monsterDistance=(monsterDistance==FarAway)?Nearby:UpClose ;
       // Decrease distance until at leas one monster is reachable.
  };

  
    
  { // Moved here from AddCombatants so that we don't run the script 
    // for combatants that are removed from the combat.
    int i;
    int highPri, nextPri;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    ActorType actor;
    //This has been moved to AFTER the combatants have been placed on the map.
    //  Otherwise we run the scripts for monsters who did not fit on the map and
    //  were removed from the combat.
    //m_iNumCombatants = m_aCombatants.GetSize();
    for (i=0; i<m_iNumCombatants; i++)
    {
      CString priority;
      m_aCombatants[i].LoadCombatIcon();
      m_aCombatants[i].determineIconSize();
      m_aCombatants[i].GetContext(&actor);
      SetCharContext(&actor);
      scriptContext.SetCharacterContext(m_aCombatants[i].m_pCharacter);
      scriptContext.SetCombatantContext(&m_aCombatants[i]);
      scriptContext.SetClassContext(m_aCombatants[i].m_pCharacter->GetClass());
      if (m_aCombatants[i].IsPartyMember())
      {
        m_aCombatants[i].scriptPriority = 750;
      }
      else
      {
        m_aCombatants[i].scriptPriority = 250;
      };
      hookParameters[5].Format("%d", m_aCombatants[i].scriptPriority);
      priority = m_aCombatants[i].RunCombatantScripts(PRE_START_COMBAT,
                                                      ScriptCallback_RunAllScripts,
                                                      NULL,
                                                      "Initializing new combat");
      scriptContext.Clear();
      ClearCharContext();
      if (!priority.IsEmpty())
      {
        int k;
        k = atoi(priority);
        m_aCombatants[i].scriptPriority = k;
      };
    };
    highPri = 0x3fffffff;
    // Run through the combatants performing the highPri combatants.
    // At the same time we compute the nextPri.
    do
    {
      nextPri = 0x80000000;
      for (i=0; i<m_iNumCombatants; i++)
      {
        if (m_aCombatants[i].scriptPriority == highPri)
        {
          m_aCombatants[i].LoadCombatIcon();
          m_aCombatants[i].determineIconSize();
          m_aCombatants[i].GetContext(&actor);
          SetCharContext(&actor);
          scriptContext.SetCharacterContext(m_aCombatants[i].m_pCharacter);
          m_aCombatants[i].RunCombatantScripts(ON_START_COMBAT,
                                               ScriptCallback_RunAllScripts,
                                               NULL,
                                               "Initializing new combat");
          scriptContext.Clear();
          ClearCharContext();
        }
        else
        {
          if (    (m_aCombatants[i].scriptPriority < highPri)
               && (m_aCombatants[i].scriptPriority > nextPri))
          {
            nextPri = m_aCombatants[i].scriptPriority;
          };
        };
      };
      highPri = nextPri;
    } while (nextPri != 0x80000000);
  };
  
  
  QComb.Clear();
  //StartNewRound();
  //prevRndCombatant = GetCurrCombatant();
  PlaceCursorOnDude(0,TRUE);   
  ClearEmptyCellSearch(); // don't need this data anymore

  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    for (int i=0; i<m_iNumCombatants; i++)
    {
      scriptContext.SetCharacterContext(m_aCombatants[i].m_pCharacter);
      scriptContext.SetCombatantContext(&m_aCombatants[i]);
      m_aCombatants[i].RunCombatantScripts(GUARDING_BEGIN_COMBAT,
                                              ScriptCallback_RunAllScripts, 
                                              NULL,
                                              "Initializing new combat");
      ClearHookParameters();
      scriptContext.Clear();
    };
  };

}

//*****************************************************************************
// NAME:    COMBAT_DATA::AddCombatatants
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::AddCombatants()
{ 
  // always add party members first!
  int i;
  //int highPri, nextPri;
  //HOOK_PARAMETERS hookParameters;
  //SCRIPT_CONTEXT scriptContext;
  //ActorType actor;
  m_iNumCombatants = 0;
  AddCharsToCombatants(); 
  AddMonstersToCombatants();

/*  This has been moved to AFTER the combatants have been placed on the map.
 *    Otherwise we run the scripts for monsters who did not fit on the map and
 *    were removed from the combat.
 *  //m_iNumCombatants = m_aCombatants.GetSize();
 */
  for (i=0; i<m_iNumCombatants; i++)
  {
    CString priority;
    m_aCombatants[i].LoadCombatIcon();
    m_aCombatants[i].determineIconSize();
/*
 *    m_aCombatants[i].GetContext(&actor);
 *    SetCharContext(&actor);
 *    scriptContext.SetCharacterContext(m_aCombatants[i].m_pCharacter);
 *    scriptContext.SetCombatantContext(&m_aCombatants[i]);
 *    scriptContext.SetClassContext(m_aCombatants[i].m_pCharacter->GetClass());
 *    if (m_aCombatants[i].IsPartyMember())
 *    {
 *      m_aCombatants[i].scriptPriority = 750;
 *    }
 *    else
 *    {
 *      m_aCombatants[i].scriptPriority = 250;
 *    };
 *    hookParameters[5].Format("%d", m_aCombatants[i].scriptPriority);
 *    priority = m_aCombatants[i].RunCombatantScripts(PRE_START_COMBAT,
 *                                                    ScriptCallback_RunAllScripts,
 *                                                    NULL);
 *    scriptContext.Clear();
 *    ClearCharContext();
 *    if (!priority.IsEmpty())
 *    {
 *      int k;
 *      k = atoi(priority);
 *      m_aCombatants[i].scriptPriority = k;
 *    };
 */
  };
/*
 *  highPri = 0x3fffffff;
 *  // Run through the combatants performing the highPri combatants.
 *  // At the same time we compute the nextPri.
 *  do
 *  {
 *    nextPri = 0x80000000;
 *    for (i=0; i<m_iNumCombatants; i++)
 *    {
 *      if (m_aCombatants[i].scriptPriority == highPri)
 *      {
 *        m_aCombatants[i].LoadCombatIcon();
 *        m_aCombatants[i].determineIconSize();
 *        m_aCombatants[i].GetContext(&actor);
 *        SetCharContext(&actor);
 *        scriptContext.SetCharacterContext(m_aCombatants[i].m_pCharacter);
 *        m_aCombatants[i].RunCombatantScripts(ON_START_COMBAT,
 *                                             ScriptCallback_RunAllScripts,
 *                                             NULL);
 *        scriptContext.Clear();
 *        ClearCharContext();
 *      }
 *      else
 *      {
 *        if (    (m_aCombatants[i].scriptPriority < highPri)
 *             && (m_aCombatants[i].scriptPriority > nextPri))
 *        {
 *          nextPri = m_aCombatants[i].scriptPriority;
 *        };
 *      };
 *    };
 *    highPri = nextPri;
 *  } while (nextPri != 0x80000000);
 */
}


CString COMBAT_DATA::ListAdjacentCombatants(int currCombatant, BOOL onlyEnemy)
{
  BOOL thisDudeIsFriendly;
  CString adjacentCombatants, codedDude;
  int i, numCombatant;
  int minx, miny, maxx, maxy;
  if (!m_bCombatActive)
  {
    return "";
  };
  numCombatant = NumCombatants();
  COMBATANT *thisDude, *thatDude;
  thisDude = GetCombatant(currCombatant);
  thisDudeIsFriendly = thisDude->GetIsFriendly();
  minx = thisDude->x-1;
  miny = thisDude->y-1;
  maxx = thisDude->x + thisDude->width;
  maxy = thisDude->y + thisDude->height;
  for (i=0; i<numCombatant; i++)
  {
    if (i == currCombatant) continue;
    thatDude = GetCombatant(i);
    if (thatDude->x > maxx) continue;
    if (thatDude->y > maxy) continue;
    if (thatDude->x + thatDude->width <= minx) continue;
    if (thatDude->y + thatDude->height <= miny) continue;
    if (onlyEnemy)
    {
      if (thatDude->GetIsFriendly() == thisDudeIsFriendly) continue;
    };
    codedDude.Format("|%d", i);
    adjacentCombatants += codedDude;
  };
  return adjacentCombatants;
};



//*****************************************************************************
// NAME:    COMBAT_DATA::AddCharsToCombatants
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::AddCharsToCombatants()
{
  COMBATANT temp;  

  for (int i=0; i<party.numCharacters;i++)
  {
    charStatusType stype = party.characters[i].GetAdjStatus();
    if (   ((stype == Okay) || (stype == Dying))
        && (party.characters[i].GetAllowInCombat()))
    {       
       temp.Clear();
       temp.self = m_iNumCombatants;
       //temp.InitFromCharData(i); 
       
       // Add new combatant to array of combatants.
       //temp.InitFromCharData(i);
       m_aCombatants.SetAt(m_iNumCombatants, temp);
       m_iNumCombatants++;
       {
         int j;
         for (j=0; j<m_iNumCombatants; j++)
         {
           if (m_aCombatants[j].m_pCharacter == NULL) continue;
           m_aCombatants[j].m_pCharacter->m_pCombatant = &m_aCombatants[j];
         };
       }
       m_aCombatants[m_iNumCombatants-1].InitFromCharData(i);
    }
  }
}

//*****************************************************************************
// NAME:    COMBAT_DATA::AddMonstersToCombatants
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::AddMonstersToCombatants()
{
  COMBATANT temp;

  if (m_pEvent->monsters.GetCount() == 0)
  {
    WriteDebugString("No monsters in combat event %i\n", m_pEvent->GetEventId());
    return;
  }

  // how many monsters can we add?
  int MaxMonstersAllowed = MAX_COMBATANTS - NumCombatants()/*m_aCombatants.GetSize()*/ - 1;

  if (m_pEvent->randomMonster)
  {
    // pick just one monster/NPC from the list
    int result = RollDice(m_pEvent->monsters.GetCount(), 1, 0.0);
    COMBATANT *pCombatant;
    result -= 1; // make zero-based index;
    result = max(0, result);

    int count;
    if (m_pEvent->monsters.monsters[result].UseQty == MONSTER_EVENT::meUseQty)
    {
      count = m_pEvent->monsters.monsters[result].qty;
    }
    else
    {
      count = RollDice(m_pEvent->monsters.monsters[result].qtyDiceSides,
                       m_pEvent->monsters.monsters[result].qtyDiceQty,
                       m_pEvent->monsters.monsters[result].qtyBonus);
      WriteDebugString("Random quantity of combat monster: %i\n", count);
    }

    double mod = GetMonsterQtyMod();
    if (mod != 0.0) count += ((mod/100.0)*count);
    if (count > MaxMonstersAllowed) count = MaxMonstersAllowed;
    if (count <= 0) count=1;
    for (int j=0;j<count;j++)
    {     
/*
 *  // All of this has been duplicated below so that we add the combatant to the combat
 *  // before updating its properties.  This is so that any scripts that reference
 *  // the combatant will find him actually a part of the combat.
 *     temp.Clear();
 *     temp.self = m_iNumCombatants; // init relies on self index
 *
 *     if (m_pEvent->monsters.monsters[result].type == MONSTER_TYPE)
 *       temp.InitFromMonsterData(m_pEvent->monsters.monsters[result].monster,
 *                                m_pEvent->monsters.monsters[result].friendly,
 *                                m_pEvent->monsters.monsters[result].items,
 *                                m_pEvent->monsters.monsters[result].money);
 *     else
 *       temp.InitFromNPCData(m_pEvent->monsters.monsters[result].monster,
 *                            m_pEvent->monsters.monsters[result].friendly,
 *                            m_pEvent->monsters.monsters[result].items,
 *                            m_pEvent->monsters.monsters[result].money);
 *
 *     if (temp.origKey != -1)
 *     {         
 *       temp.self = m_iNumCombatants;
 *       temp.SetMorale(m_pEvent->monsterMorale);
 *       temp.SetMorale( temp.GetMorale() + m_pEvent->monsters.monsters[result].moraleAdj );
 *
 *       int PreSpecAbCount = temp.spellEffects.GetCount();
 *       // turn on the special abilities
 *       Not Implemented(0xc98f4a,FALSE);//temp.specAbs.EnableAllFor(&temp);
 *
 *       int PostSpecAbCount = temp.spellEffects.GetCount();
 *       int Diff = PostSpecAbCount - PreSpecAbCount;
 *       if (Diff > 0)
 *       {
 *         POSITION pos = temp.spellEffects.GetTailPosition();  
 *         while ((pos != NULL) && (Diff > 0))
 *         {        
 *           temp.spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;
 *           //temp.spellEffects.GetAt(pos).m_gsID = temp.self;
 *           temp.spellEffects.GetAt(pos).SourceChar_ID(temp.self);
 *           temp.spellEffects.GetPrev(pos);
 *           Diff--;
 *         }
 *       }
 *
 *       m_aCombatants.SetAtGrow(m_iNumCombatants, temp);
 *       m_iNumCombatants++;
 *     }
 */

    
    

   // All of this is a duplicate of code above so that we add the combatant to the combat  //*
   // before updating its properties.  This is so that any scripts that reference          //*
   // the combatant will find him actually a part of the combat.                           //*
      temp.Clear();                                                                        //*
      //Not Implemented(0xc54d30,FALSE); // I cannot see how this ever worked.              //*
                                      // Either it encountered a Not Implemented()          //*
                                      // or it failed to SetAtGrow().                      //*
      temp.self = m_iNumCombatants; // init relies on self index                           //*
      m_aCombatants.SetAt(m_iNumCombatants, temp);                                         //*
      pCombatant = &m_aCombatants[m_iNumCombatants];
      m_iNumCombatants++;                                                                  //*
                                                                                           //*
      if (m_pEvent->monsters.monsters[result].m_type == MONSTER_TYPE)                      //*
        //temp.InitFromMonsterData(m_pEvent->monsters.monsters[result].monster,            //*
        pCombatant->InitFromMonsterData(m_pEvent->monsters.monsters[result].monsterID,     //*
                                        m_pEvent->monsters.monsters[result].friendly,      //*
                                        m_pEvent->monsters.monsters[result].items,         //*
                                        m_pEvent->monsters.monsters[result].money);        //*
      else                                                                                 //*
        //temp.InitFromNPCData(m_pEvent->monsters.monsters[result].monster,                //*
        pCombatant->InitFromNPCData(m_pEvent->monsters.monsters[result].characterID,       //*
                                    m_pEvent->monsters.monsters[result].friendly,          //*
                                    m_pEvent->monsters.monsters[result].items,             //*
                                    m_pEvent->monsters.monsters[result].money);            //*
                                                                                           //*
      if (pCombatant->origKey != -1)                                                              //*
      {                                                                                    //*
        pCombatant->SetMorale(m_pEvent->monsterMorale);                                    //*
        pCombatant->SetMorale(  pCombatant->GetMorale() 
                              + m_pEvent->monsters.monsters[result].moraleAdj );           //*
                                                                                           //*
        int PreSpecAbCount = pCombatant->m_pCharacter->m_spellEffects.GetCount();          //*
        int PostSpecAbCount = pCombatant->m_pCharacter->m_spellEffects.GetCount();         //*
        // turn on the special abilities                                                   //*
        //Not Implemented(0xc98f4a,FALSE);//temp.specAbs.EnableAllFor(&temp);               //*
                                                                                           //*
        int Diff = PostSpecAbCount - PreSpecAbCount;                                       //*
        if (Diff > 0)                                                                      //*
        {                                                                                  //*
          POSITION pos = pCombatant->m_pCharacter->m_spellEffects.GetTailPosition();       //*
          while ((pos != NULL) && (Diff > 0))                                              //*
          {                                                                                //*
            pCombatant->m_pCharacter->m_spellEffects.GetAt(pos)->flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;   //*
            //temp.spellEffects.GetAt(pos).m_gsID = temp.self;                             //*
            pCombatant->m_pCharacter->m_spellEffects.GetAt(pos)->SourceChar_ID(pCombatant->self, true);   //*
            pCombatant->m_pCharacter->m_spellEffects.PeekPrev(pos);                                //*
            Diff--;                                                                        //*
          }                                                                                //*
        }                                                                                  //*
      }                                                                                    //*    
    }
  }
  else
  {
    // add all monsters/NPCs
    for (int i=0; i<m_pEvent->monsters.GetCount(); i++)
    {
      int count;
      if (m_pEvent->monsters.monsters[i].UseQty == MONSTER_EVENT::meUseQty)
      {
        count = m_pEvent->monsters.monsters[i].qty;
      }
      else
      {
        count = RollDice(m_pEvent->monsters.monsters[i].qtyDiceSides,
                         m_pEvent->monsters.monsters[i].qtyDiceQty,
                         m_pEvent->monsters.monsters[i].qtyBonus);
        WriteDebugString("Random quantity of combat monster: %i\n", count);
      }
 
      double mod = GetMonsterQtyMod();
      if (mod != 0.0) count += ((mod/100.0)*count);
      if (count <= 0) count=1;
 
      for (int j=0;j<count;j++)
      {     
        TRACE("%d countLoop\n", timeGetTime());
/*
 *  // All of this has been duplicated below so that we add the combatant to the combat
 *  // before updating its properties.  This is so that any scripts that reference
 *  // the combatant will find him actually a part of the combat.
 *       temp.Clear();
 *       temp.self = m_iNumCombatants; // init relies on self index
 *
 *       if (m_pEvent->monsters.monsters[i].type == MONSTER_TYPE)
 *         temp.InitFromMonsterData(m_pEvent->monsters.monsters[i].monster,
 *                                  m_pEvent->monsters.monsters[i].friendly,
 *                                  m_pEvent->monsters.monsters[i].items,
 *                                  m_pEvent->monsters.monsters[i].money);
 *       else
 *         temp.InitFromNPCData(m_pEvent->monsters.monsters[i].monster,
 *                              m_pEvent->monsters.monsters[i].friendly,
 *                              m_pEvent->monsters.monsters[i].items,
 *                              m_pEvent->monsters.monsters[i].money);
 *
 *       if (temp.origKey != -1)
 *       {         
 *         temp.self = m_iNumCombatants;
 *         temp.SetMorale(m_pEvent->monsterMorale);
 *         temp.SetMorale( temp.GetMorale() + m_pEvent->monsters.monsters[i].moraleAdj );
 *
 *         // int PreSpecAbCount = temp.spellEffects.GetCount();
 *         // turn on the special abilities
 *         // temp.specAbs.EnableAllFor(&temp);
 *
 *         // int PostSpecAbCount = temp.spellEffects.GetCount();
 *         // int Diff = PostSpecAbCount - PreSpecAbCount;
 *         // if (Diff > 0)
 *         // {
 *         //   POSITION pos = temp.spellEffects.GetTailPosition();  
 *         //   while ((pos != NULL) && (Diff > 0))
 *         //   {        
 *         //     temp.spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;
 *         //     temp.spellEffects.GetAt(pos).SourceChar_ID(temp.self);
 *         //     //temp.spellEffects.GetAt(pos).m_gsID = temp.self;
 *         //     temp.spellEffects.GetPrev(pos);
 *         //     Diff--;
 *         //   }
 *         // }
 *
 *         if (NumCombatants() < MaxMonstersAllowed)
 *         {
 *           m_aCombatants.SetAtGrow(m_iNumCombatants, temp);
 *           m_iNumCombatants++;
 *         }
 *       }
 */




   // All of this is a duplicate of code above so that we add the combatant to the combat      //*
   // before updating its properties.  This is so that any scripts that reference              //*
   // the combatant will find him actually a part of the combat.                               //*
        temp.Clear();                                                                          //*
        temp.self = m_iNumCombatants; // init relies on self index                             //*
                                                                                               //*
        if (NumCombatants() < MaxMonstersAllowed)                                              //* 
        {                                                                                      //* 
          COMBATANT *pCombatant;                                                               //*
          m_aCombatants.SetAt(m_iNumCombatants, temp);                                         //*
          TRACE("%d after SetAt\n", timeGetTime());
          m_iNumCombatants++;                                                                  //* 
          pCombatant = &m_aCombatants[m_iNumCombatants-1];                                     //*
                                                                                               //*
          if (m_pEvent->monsters.monsters[i].m_type == MONSTER_TYPE)                             //*
          {
            //pCombatant->InitFromMonsterData(m_pEvent->monsters.monsters[i].monster,          //*
            pCombatant->InitFromMonsterData(m_pEvent->monsters.monsters[i].monsterID,          //*
                                             m_pEvent->monsters.monsters[i].friendly,          //*
                                             m_pEvent->monsters.monsters[i].items,             //*
                                             m_pEvent->monsters.monsters[i].money);            //*
            TRACE("%d after InitFromMonsterData\n", timeGetTime());
          }
          else                                                                                 //*
            //pCombatant->InitFromNPCData(m_pEvent->monsters.monsters[i].monster,              //*
            pCombatant->InitFromNPCData(m_pEvent->monsters.monsters[i].characterID,            //*
                                        m_pEvent->monsters.monsters[i].friendly,               //*
                                        m_pEvent->monsters.monsters[i].items,                  //*
                                        m_pEvent->monsters.monsters[i].money);                 //*
                                                                                               //*
          //if (pCombatant->origKey != NO_DUDE)                                                              //*
          //if (pCombatant->m_pCharacter->monsterID != "")                                       //*
          if (    (pCombatant->origKey != NO_DUDE)                                                              //*
               && (pCombatant->m_pCharacter->monsterID != "")  )                                       //*
          {
                                                             //*

            /* The following has been replaced 20180131  PRS
            pCombatant->SetMorale(m_pEvent->monsterMorale);                                           //*
            pCombatant->SetMorale( pCombatant->GetMorale() + m_pEvent->monsters.monsters[i].moraleAdj );     //*
            */
            /* With this
            */
            pCombatant->SetMorale(pCombatant->GetMorale() + m_pEvent->monsterMorale);                                           //*

                                                                                               //*
            // int PreSpecAbCount = temp.spellEffects.GetCount();                              //*
            // turn on the special abilities                                                   //*
            // temp.specAbs.EnableAllFor(&temp);                                               //*
                                                                                               //*
            // int PostSpecAbCount = temp.spellEffects.GetCount();                             //*
            // int Diff = PostSpecAbCount - PreSpecAbCount;                                    //*
            // if (Diff > 0)                                                                   //*
            // {                                                                               //*
            //   POSITION pos = temp.spellEffects.GetTailPosition();                           //*
            //   while ((pos != NULL) && (Diff > 0))                                           //*
            //   {                                                                             //*
            //     temp.spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;//*          
            //     temp.spellEffects.GetAt(pos).SourceChar_ID(temp.self);                      //*
            //     //temp.spellEffects.GetAt(pos).m_gsID = temp.self;                          //*
            //     temp.spellEffects.GetPrev(pos);                                             //*
            //     Diff--;                                                                     //*
            //   }                                                                             //*
            // }                                                                               //*
                                                                                               //*
          }                                                                                    //*
          else
          {
            CHARACTER *pCharacter;
            pCharacter = pCombatant->m_pCharacter;
            if (pCharacter != NULL)
            {
              pCombatant->m_pCharacter->m_pCombatant = NULL;
              pCombatant->m_pCharacter = NULL;
              delete pCharacter;
            };
            m_iNumCombatants--;
          };
        }                                                                                      //*





      }; // for (j)
    }; //for (i)
  }
}

//*****************************************************************************
// NAME:    COMBAT_DATA::AddMonsterToCombatants
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::AddMonsterToCombatants(const CString& monsterName, int friendly)
{
  //int index;
  const MONSTER_DATA *pMonster;
//  int monsterPartyX, monsterPartyY;
  BOOL reset;
  MONSTER_ID monsterID;
  monsterID = monsterName;
  //index = monsterData.FindName(monsterName);
  pMonster = monsterData.PeekMonster(monsterID);
  //if (index > 0)
  if (pMonster != NULL)
  {
    ActorType actor;
    int index;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    //AddMonsterToCombatants(index);
    index = NumCombatants()/*m_aCombatants.GetSize()*/;
    AddMonsterToCombatants(pMonster);
    if (index < NumCombatants())
    {
      m_aCombatants[index].LoadCombatIcon();
      m_aCombatants[index].friendly = friendly;
      m_aCombatants[index].m_adjFriendly = 0;
      m_aCombatants[index].determineIconSize();
      m_aCombatants[index].GetContext(&actor);
      SetCharContext(&actor);
      scriptContext.SetCharacterContext(m_aCombatants[index].m_pCharacter);
      m_aCombatants[index].RunCombatantScripts(ON_START_COMBAT,
                                             ScriptCallback_RunAllScripts,
                                             NULL,
                                             "Adding a monster to combat");
      ClearCharContext();

      reset = false;
      determineInitCombatPos(&m_aCombatants[index], index, &reset, FALSE, "" /*, &monsterPartyX, &monsterPartyY*/);
    };
  };
}

CArray<COMBATANT *, COMBATANT*> newMonsters;

class NEW_MONSTER
{
public:
  NEW_MONSTER(COMBATANT *pCombatant)
  {
    int n;
    n = newMonsters.GetSize();
    newMonsters.SetAtGrow(n, pCombatant);
  };
  ~NEW_MONSTER(void)
  {
    newMonsters.RemoveAt(newMonsters.GetSize()-1);
  };
};

//*****************************************************************************
// NAME:    COMBAT_DATA::AddMonsterToCombatants
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//void COMBAT_DATA::AddMonsterToCombatants(int monsterDataIndex)
void COMBAT_DATA::AddMonsterToCombatants(const MONSTER_DATA *pMonster)
{
  COMBATANT temp;
  NEW_MONSTER newMonster(&temp);
  //MONSTER_DATA *pMonster;

  //pMonster = monsterData.GetMonsterData(monsterDataIndex);

  // how many monsters can we add?
 // int MaxMonstersAllowed = MAX_COMBATANTS - m_aCombatants.GetSize() - 1;

  temp.Clear();
  temp.self = COMBATANT::SELF_NewCombatant; // init relies on self index
  temp.x = newMonsters.GetSize()-1;

  //if (m_pEvent->monsters.monsters[result].type == MONSTER_TYPE)
        //temp.InitFromMonsterData(monsterDataIndex,  //m_pEvent->monsters.monsters[result].monster,
        temp.InitFromMonsterData(pMonster,          //m_pEvent->monsters.monsters[result].monster,
                                 FALSE,             //m_pEvent->monsters.monsters[result].friendly,
                                 pMonster->myItems, //_pEvent->monsters.monsters[result].items,
                                 pMonster->money);  //m_pEvent->monsters.monsters[result].money);
  //    else
  //      temp.InitFromNPCData(m_pEvent->monsters.monsters[result].monster,
  //                           m_pEvent->monsters.monsters[result].friendly,
  //                           m_pEvent->monsters.monsters[result].items,
  //                           m_pEvent->monsters.monsters[result].money);

  if (temp.origKey != -1)
  {         
/*
 *  // All of this has been duplicated below so that we add the combatant to the combat
 *  // before updating its properties.  This is so that any scripts that reference
 *  // the combatant will find him actually a part of the combat.
 *   temp.self = m_iNumCombatants;
 *   temp.SetMorale(m_pEvent->monsterMorale);
 *   temp.SetMorale( temp.GetMorale() + 0);//m_pEvent->monsters.monsters[result].moraleAdj );
 *
 *   int PreSpecAbCount = temp.spellEffects.GetCount();
 *   // turn on the special abilities
 *   //Not Implemented(0xc98f7c,FALSE);//temp.specAbs.EnableAllFor(&temp);
 *
 *   int PostSpecAbCount = temp.spellEffects.GetCount();
 *   int Diff = PostSpecAbCount - PreSpecAbCount;
 *   if (Diff > 0)
 *   {
 *     POSITION pos = temp.spellEffects.GetTailPosition();  
 *     while ((pos != NULL) && (Diff > 0))
 *     {        
 *       temp.spellEffects.GetAt(pos).flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;
 *       //temp.spellEffects.GetAt(pos).m_gsID = temp.self;
 *       temp.spellEffects.GetAt(pos).SourceChar_ID(temp.self);
 *       temp.spellEffects.GetPrev(pos);
 *       Diff--;
 *     }
 *   }
 *
 *   m_aCombatants.SetAtGrow(m_iNumCombatants, temp);
 *   m_iNumCombatants++;
 */
    COMBATANT *pCombatant;
   // All of this is a duplicate of code above so that we add the combatant to the combat    //*
   // before updating its properties.  This is so that any scripts that reference            //*
   // the combatant will find him actually a part of the combat.                             //*
    temp.self = m_iNumCombatants;                                                            //*
    temp.SetMorale(m_pEvent->monsterMorale);                                                 //*
    {
      CHARACTER *pSaveCharPointer;
      pSaveCharPointer = temp.m_pCharacter;
      temp.m_pCharacter = NULL;
      m_aCombatants.SetAt(m_iNumCombatants, temp);                                         //*
      m_aCombatants[m_iNumCombatants].m_pCharacter = pSaveCharPointer;
      pSaveCharPointer->m_pCombatant = &m_aCombatants[m_iNumCombatants];
    };
    m_iNumCombatants++;                                                                      //*
    pCombatant = &m_aCombatants[m_iNumCombatants-1];                                         //*
                                                                                             //*
    pCombatant->SetMorale( pCombatant->GetMorale() + 0);                                     //* 
    //m_pEvent->monsters.monsters[result].moraleAdj );                                       //*
                                                                                             //*
    int PreSpecAbCount = pCombatant->m_pCharacter->m_spellEffects.GetCount();                                //*
    // turn on the special abilities                                                         //*
    //Not Implemented(0xc98f7c,FALSE);//temp.specAbs.EnableAllFor(&temp);                     //*
                                                                                             //*
    int PostSpecAbCount = pCombatant->m_pCharacter->m_spellEffects.GetCount();                               //*
    int Diff = PostSpecAbCount - PreSpecAbCount;                                             //*
    if (Diff > 0)                                                                            //*
    {                                                                                        //*
      POSITION pos = pCombatant->m_pCharacter->m_spellEffects.GetTailPosition();                             //*
      while ((pos != NULL) && (Diff > 0))                                                    //*
      {                                                                                      //*
        pCombatant->m_pCharacter->m_spellEffects.GetAt(pos)->flags |= SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB;  //*
        pCombatant->m_pCharacter->m_spellEffects.GetAt(pos)->SourceChar_ID(temp.self, true);  //*
        pCombatant->m_pCharacter->m_spellEffects.GetPrev(pos);                               //*
        //temp.spellEffects.GetAt(pos).m_gsID = temp.self;                                   //*
        Diff--;                                                                              //*
      }                                                                                      //*
    }                                                                                        //*
  }
  else
  {
    if (temp.m_pCharacter != NULL)
    {
      if (temp.deleteChar)
      {
        temp.m_pCharacter->m_pCombatant = NULL;
        delete temp.m_pCharacter;
        temp.m_pCharacter = NULL;
      };
    };
  };
}


//*****************************************************************************
// NAME: getActiveCombatCharacter
//
// PURPOSE:  
//*****************************************************************************
COMBATANT& COMBAT_DATA::getActiveCombatCharacter()
{
  int curr = GetCurrCombatant();
  ASSERT(curr != NO_DUDE);
  
  // coding error
  if (curr==NO_DUDE) return m_aCombatants[0];

  // call getActiveCastingCharacter while casting a spell
  // since the caster may die. The caster info is stored
  // separately to make sure it is available until the 
  // spell effects have been delivered to all targets (if applicable).
  //ASSERT(combatants[curr].state!=ICS_Casting);

  if (m_aCombatants[curr].GetType() == MONSTER_TYPE)
  {
    WriteDebugString("getActiveCombatCharacter() called for monster combatant\n");
  }

  return m_aCombatants[curr];
}

//*****************************************************************************
//    NAME: *COMBAT_DATA::getActiveCastingCharacter
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *COMBAT_DATA::getActiveCastingCharacter()
{
  ASSERT(m_iCurrCaster != NO_DUDE);
  if (m_iCurrCaster==NO_DUDE) return NULL;
  return (&(m_aCombatants[m_iCurrCaster]));
}

//*****************************************************************************
//    NAME: COMBAT_DATA::SetActiveCastingCharacter
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::SetActiveCastingCharacter(int dude)
{
  m_iCurrCaster = dude;
  if (m_iCurrCaster==NO_DUDE) return;
  EnsureVisibleTarget(m_iCurrCaster);
}


//*****************************************************************************
// NAME: COMBAT_DATA::CanBandage
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBAT_DATA::CanBandage()
{
  int curr = GetCurrCombatant();
  if (curr == NO_DUDE)
    return FALSE;

  if (m_aCombatants[curr].CanBandage())
  {
    int i=0;    
    while (i < m_iNumCombatants)
    {
      if ((i != GetCurrCombatant()) && (m_aCombatants[i].GetAdjStatus() == Dying))
        return TRUE;  
      i++;
    }
  }

  return FALSE;
}


//*****************************************************************************
// NAME: COMBAT_DATA::SpecialActionName
//
// PURPOSE:  
//*****************************************************************************
CString COMBAT_DATA::SpecialActionName()
{
  int curr = GetCurrCombatant();
  if (curr == NO_DUDE)
    return "";

  return m_aCombatants[curr].SpecialActionName();
}

//*****************************************************************************
// NAME: COMBAT_DATA::SpecialActionIndex
//
// PURPOSE:  
//*****************************************************************************
int COMBAT_DATA::SpecialActionIndex()
{
  int curr = GetCurrCombatant();
  if (curr == NO_DUDE)
    return -1;

  return m_aCombatants[curr].SpecialActionIndex();
}


//*****************************************************************************
// NAME: COMBAT_DATA::Bandage
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::Bandage()
{
  if (CanBandage())
  {
    int count=0;
    int btarg=-1;
	int mosthurt = 0;

    for (int i=0; i<m_iNumCombatants; i++)
    {
      if (m_aCombatants[i].GetAdjStatus() == Dying)
      {
		  // Figure out which PC is the most hurt, and set them to be bandaged first.
		  if (m_aCombatants[i].GetHitPoints() <= m_aCombatants[mosthurt].GetHitPoints()) {
			mosthurt = i;
			btarg = i;
			count = 1;
		  }

	  }
	}
       
	// Now bandage the most hurt PC
	if (count ==1) 
  {
		m_aCombatants[mosthurt].isBandaged = TRUE;
		m_aCombatants[mosthurt].SetHitPoints(0);
		m_aCombatants[mosthurt].SetStatus(Unconscious);        
        //combatants[i].bandageWho = i;
  }
    
    // sets state to cs_bandage so status message is displayed
  m_aCombatants[GetCurrCombatant()].Bandage();
  // status message displays bandage targ if only one targ
  if (count == 1) 
      m_aCombatants[GetCurrCombatant()].bandageWho = btarg;      
  }
}

//*****************************************************************************
// NAME: COMBAT_DATA::ResetAim
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::ResetAim()
{
  SetUpdateFlag();
  m_iAimPos = GetCurrCombatant();

  if ((m_iAimPos >= 0) && (m_iAimPos < m_iNumCombatants))
  {
    m_iCursorX = m_aCombatants[m_iAimPos].x;
    m_iCursorY = m_aCombatants[m_iAimPos].y;
    EnsureVisibleCursor();
  }
}


BOOL COMBAT_DATA::IsValidTarget(COMBATANT *pAttacker, COMBATANT *pTarget, int *targetValidity)
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  CString result;
  int answer;
  if (pTarget==NULL) return FALSE;
  if (    (targetValidity != NULL) 
       && (*targetValidity >= 0)    ) return pAttacker->targetValidity==1;


  scriptContext.SetCombatantContext(pTarget);
  scriptContext.SetAttackerContext(pAttacker);
  scriptContext.SetTargetContext(pTarget);

  result = pTarget->RunCombatantScripts(
                     IS_VALID_TARGET,
                     ScriptCallback_LookForChar,
                     "YN",
                     "Determine if valid target");
  answer = 1;
  if (!result.IsEmpty())
  {
    if (result[0] == 'N') answer = 0;
  }
  if (targetValidity != NULL) *targetValidity = answer;
  return answer == 1;
}

//*****************************************************************************
// NAME: GetNextAim
//
// PURPOSE:  
//*****************************************************************************
int COMBAT_DATA::GetNextAim()
{
  if (m_iNumCombatants <= 0)
  {
    die(0xab4cc);
    m_iAimPos = NO_DUDE;
    return NO_DUDE;
  }
  //combatData.SetUpdateFlag();

  if (m_iAimPos == NO_DUDE)
    m_iAimPos = GetCurrCombatant();

  int count=0;
  while (count < m_iNumCombatants)
  {
    if ((m_iAimPos < 0) || (m_iAimPos >= m_iNumCombatants))
      m_iAimPos = 0;
    else
      m_iAimPos++;

    if ((m_iAimPos >= 0) && (m_iAimPos < m_iNumCombatants))
    {
      if (m_aCombatants[m_iAimPos].IsPartyMember()) continue;
      if (m_aCombatants[m_iAimPos].friendly) continue;
      if (m_aCombatants[m_iAimPos].charOnCombatMap(false, true))
      {
        COMBATANT *pTarget, *pAttacker;
        pTarget = &m_aCombatants[m_iAimPos];
        pAttacker = &m_aCombatants[GetCurrCombatant()];
        if (IsValidTarget(pAttacker , pTarget, NULL))
        {
          m_iCursorX = pTarget->x;
          m_iCursorY = pTarget->y;
          EnsureVisibleCursor();
          return m_iAimPos;
        };
      }
    }
    count++;
  }
  m_iAimPos = GetCurrCombatant();
  return m_iAimPos;
}

//*****************************************************************************
// NAME: GetPrevAim
//
// PURPOSE:  
//*****************************************************************************
int COMBAT_DATA::GetPrevAim()
{
  if (m_iNumCombatants <= 0)
  {
    die(0xab4cd);
    m_iAimPos = NO_DUDE;
    return NO_DUDE;
  }
  //combatData.SetUpdateFlag();

  if (m_iAimPos == NO_DUDE)
    m_iAimPos = GetCurrCombatant();

  int count=0;
  while (count < m_iNumCombatants)
  {
    if ((m_iAimPos < 0) || (m_iAimPos >= m_iNumCombatants))
      m_iAimPos = m_iNumCombatants-1;
    else
      m_iAimPos--;
    
    if ((m_iAimPos >= 0) && (m_iAimPos < m_iNumCombatants))
    {
      if (m_aCombatants[m_iAimPos].IsPartyMember()) continue;
      if (m_aCombatants[m_iAimPos].friendly) continue;
      if (m_aCombatants[m_iAimPos].charOnCombatMap(false, true))
      {
        COMBATANT *pTarget, *pAttacker;
        pTarget = &m_aCombatants[m_iAimPos];
        pAttacker = &m_aCombatants[GetCurrCombatant()];
        if (IsValidTarget(pAttacker , pTarget, NULL))
        {
          m_iCursorX = pTarget->x;
          m_iCursorY = pTarget->y;
          EnsureVisibleCursor();
          return m_iAimPos;
        };
      }
    }
    count++;
  }
  m_iAimPos = GetCurrCombatant();
  return m_iAimPos;

}

//*****************************************************************************
// NAME: getNumCombatants
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::getNumCombatants(int* pNumParty, int* pNumMons)
{
   *pNumParty = 0;
   *pNumMons = 0;
 
   // count friendly pc's and npc's in numParty
   // count non-friendly pc's, npc's, and monsters in numMons
   // do not count friendly monsters in numParty
 
   for (int i=0; i < m_iNumCombatants; i++)
   {
     if (m_aCombatants[i].charOnCombatMap(false, true))
     {
        if ((m_aCombatants[i].IsPartyMember()) || (m_aCombatants[i].GetIsFriendly())) (*pNumParty)++;
        else (*pNumMons)++;    
     }
   }
}

//*****************************************************************************
// NAME: DetermineCombatInitiative
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::DetermineCombatInitiative()
{  
   int i;
//   WriteDebugString("DEBUG - Rolling Initiatives: m_eSurprise = %d\n", m_eSurprise);
   for (i=0; i<m_iNumCombatants; i++)
   {
      m_aCombatants[i].RollInitiative(m_eSurprise);
//      WriteDebugString("DEBUG - Rolled Initiative for %d = %d\n", i, m_aCombatants[i].m_iInitiative);
   };

   // if one side was initially surprised, it only
   // works for the first round so reset it.
   m_eSurprise = Neither;
   {
      CString line;
      int swap;
      int combatant[MAX_COMBATANTS];
      for (i=0; i<m_iNumCombatants; i++)
      {
        combatant[i] = i;
      };
      for (swap=1; swap!=0; )
      {
        swap = 0;
        for (i=0; i<m_iNumCombatants-1; i++)
        {
          if (m_aCombatants[combatant[i]].m_iInitiative > m_aCombatants[combatant[i+1]].m_iInitiative)
          {
            int temp;
            temp = combatant[i];
            combatant[i] = combatant[i+1];
            combatant[i+1] = temp;
            swap = 1;
          };
        };
      };
      if (logDebuggingInfo)
      {
        line.Format("Initiatives for round %d: ", m_iCurrRound);
      };
      {
        SCRIPT_CONTEXT scriptContext;
        for (i=0; i< m_iNumCombatants; i++)
        {
          HOOK_PARAMETERS hookParameters;
          COMBATANT *pCombatant;
          CString result;
          scriptContext.SetCombatantContext(&m_aCombatants[i]);
          hookParameters[5].Format("%d", m_aCombatants[i].m_iInitiative);
          hookParameters[6].Format("%d", m_aCombatants[i].GetMorale());
          hookParameters[7].Format("%d", m_pEvent->monsterMorale);
          pCombatant = &m_aCombatants[i];
          result = pCombatant->RunCombatantScripts(
                             ADJUST_INITIATIVE,
                             ScriptCallback_RunAllScripts,
                             NULL,
                             "Determine combat initiative");
          result = pCombatant->m_pCharacter->RunCharacterScripts(
                             ADJUST_INITIATIVE,
                             ScriptCallback_RunAllScripts,
                             NULL,
                             "Determine combat initiative");
          if (pCombatant->GetType() == MONSTER_TYPE)
          {
            //int monsterIndex;
            //monsterIndex = pCombatant->origKey;
            MONSTER_ID monsterID;
            monsterID = pCombatant->m_pCharacter->monsterID;
            {
              MONSTER_DATA *pMonsterData;
              //pMonsterData = monsterData.GetMonsterData(monsterIndex);
              pMonsterData = monsterData.GetMonster(monsterID);
              if (pMonsterData != NULL)
              {
                result = pMonsterData->RunMonsterScripts(
                            ADJUST_INITIATIVE,
                            ScriptCallback_RunAllScripts,
                            &hookParameters,
                            "Determine combat initiative");
              };
            };
          };
          if (!result.IsEmpty())
          {
            int adj;
            if (sscanf(result,"%d", &adj) == 1)
            {
              int initiative;
              if (sscanf(hookParameters[5], "%d", &initiative) == 1)
              {
                m_aCombatants[i].m_iInitiative += adj;
                if (m_aCombatants[i].m_iInitiative < COMBATANT::INITIATIVE_AlwaysFirst) 
                {
                  m_aCombatants[i].m_iInitiative = COMBATANT::INITIATIVE_AlwaysFirst;
                };
                if (m_aCombatants[i].m_iInitiative > COMBATANT::INITIATIVE_Never) 
                {
                  m_aCombatants[i].m_iInitiative = COMBATANT::INITIATIVE_Never;
                };
              };
            };
          };
        };
      };
      for (i=0; i<m_iNumCombatants; i++)
      {
        char initiative[40];
        sprintf(initiative, "%i[%i] ",combatant[i],m_aCombatants[combatant[i]].m_iInitiative);
        line += initiative;
      };
      if (logDebuggingInfo)
      {
        WriteDebugString(line + '\n');
      };
   };
}

//*****************************************************************************
// NAME: getNextCombatant
//
// PURPOSE:  
//*****************************************************************************
int COMBAT_DATA::getNextCombatant()
{   
   int dude;   

   while ((dude = QComb.Top()) != NO_DUDE)
   {
     m_aCombatants[dude].m_isCombatReady = -1; // Force check before any moves
     if (m_aCombatants[dude].IsDone(false, "Determine next combatant to take turn"))
     {
       QComb.Pop();
       if (QComb.Top() != NO_DUDE)
       {
         if (QComb.DelayedX() >= 0)
         {
            /* Really */ NotImplemented(0x34c3, false);
         };
       };
     }
     else
     {
       TRACE("getNextCombatant() returns %u (queued)\n", dude);
       return dude;
     }
   }
 
   int i=0;
   BOOL found = FALSE;
   dude = NO_DUDE;

   while ((m_iCurrInitiative <= 22) && (!found))
   {
      if (pendingSpellList.ProcessTimeSensitiveData(0, m_iCurrInitiative))
      {
        dude = QComb.Top();
        if (dude != NO_DUDE)
        {
          m_aCombatants[dude].m_isCombatReady = -1; // Force check before any moves
          if (m_aCombatants[dude].IsDone(false, "Determine next combatant to take turn"))
          {
            QComb.Pop();
            if (QComb.Top() != NO_DUDE)
            {
              if (QComb.DelayedX() >= 0)
              {
                /* Really */ NotImplemented(0x34c4, false);
              };
            };
          }
          else
          {
            TRACE("getNextCombatant() returns %u (queued) for pending spell.\n", dude);
            return dude;
          }

        };
      };
        
      i=0;
      while ((i < m_iNumCombatants) && (!found))
      {
        if (m_aCombatants[i].m_iInitiative == m_iCurrInitiative) 
        {
          m_aCombatants[i].m_isCombatReady = -2;  // Value to indicate unknown.
          if  (!m_aCombatants[i].IsDone(false, "Determine next combatantant to take turn"))
          {
            dude = i;
            found = TRUE;
          };
        };
        i++;
      };
    
      if (!found)
         m_iCurrInitiative++;
   }
 
   if (dude != NO_DUDE)
   {
     QComb.Push(dude, TRUE, 0, 0);
     turningSummary.Clear();




     {
       int j = dude;
       if (m_aCombatants[j].State() == ICS_ContinueGuarding)
       {
         m_aCombatants[j].State(ICS_None);
       };

       {       
        /* The calculation of availAttacks has been moved from
         * StartNewRound() to getNextCombatant().
         * I did this because Manikus complained that a spell
         * given during a combat round that increased the number
         * of attacks on a target did not take effect until the
         * start of the following round.  PRS 20091215
         *
         * The code has been moved back to StartNewRound.
         * I did this because some things that tak place after
         * the start-of-round but before the combatant's turn
         * require that we know the number of available attacks.
         * In particular, 'Guarding Attacks' and 'Free Attacks'
         * are based on the number of available attacks.  We will
         * have to solve the original problem differently.
         * PRS 20111214
         *
         * double leftoverAttacks=m_aCombatants[j].availAttacks;             
         * m_aCombatants[j].determineAvailAttacks();
         * double maxAttacks = ceil(m_aCombatants[j].availAttacks);
         * m_aCombatants[j].availAttacks += leftoverAttacks;
         * m_aCombatants[j].availAttacks = min(m_aCombatants[j].availAttacks, maxAttacks);
         * m_aCombatants[j].m_iMovement = 0;      
         * m_aCombatants[j].m_iNumDiagonalMoves = 0;
         */
       };
     }; 
     
     
     //PlaceCursorOnCurrentDude();
     TRACE("getNextCombatant() returns %u\n", dude);
   }
   else
     TRACE("getNextCombatant() returns NO_DUDE\n");

   return dude;
}

//*****************************************************************************
// NAME: moveCharNW
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveNW()
{  
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveNW(); 
}

//*****************************************************************************
// NAME: moveCharNE
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveNE()
{
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveNE();
}

//*****************************************************************************
// NAME: moveCharSE
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveSE()
{
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveSE();
}

//*****************************************************************************
// NAME: moveCharSW
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveSW()
{
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveSW();
}

//*****************************************************************************
// NAME: moveCharLeft
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveLeft()
{
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveLeft();
}

//*****************************************************************************
// NAME: moveCharRight
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveRight()
{
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveRight();
}

//*****************************************************************************
// NAME: moveCharForward
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveForward()
{
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveForward();
}

//*****************************************************************************
// NAME: moveCharBackward
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveBackward()
{
   ASSERT(GetCurrCombatant() != NO_DUDE);
   m_aCombatants[GetCurrCombatant()].moveBackward();
}

//*****************************************************************************
//    NAME: COMBAT_DATA::CurrMoveWouldFleeMap
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CurrMoveWouldFleeMap()
{
  ASSERT(GetCurrCombatant() != NO_DUDE);
  return (m_aCombatants[GetCurrCombatant()].CurrMoveWouldFleeMap());
}

//*****************************************************************************
// NAME: moveCursorForward
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveCursorForward()
{
   int x = m_iCursorX;
   int y = m_iCursorY-1;
 
   if (y >= 0)
   {
     SetUpdateFlag();
     ForceMapToMoveAlong(x, y, PathNorth, 1);
     m_iCursorY = y;
   }
}


//*****************************************************************************
// NAME: moveCursorBackward
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveCursorBackward()
{
   int x = m_iCursorX;
   int y = m_iCursorY+1;
 
   if (y < MAX_TERRAIN_HEIGHT)
   {
     SetUpdateFlag();
     ForceMapToMoveAlong(x, y, PathSouth, 1);      
     m_iCursorY = y;
   }
}


//*****************************************************************************
// NAME: moveCursorLeft
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::moveCursorLeft()
{
   int x = m_iCursorX-1;
   int y = m_iCursorY;
 
   if (x >= 0)
   {
     SetUpdateFlag();
     ForceMapToMoveAlong(x, y, PathWest, 1);
     m_iCursorX = x;
   }
}


//*****************************************************************************
// NAME: moveCursorRight
//
// PURPOSE:  
//*****************************************************************************

bool COMBAT_DATA::CanMoveCursor(const COMBATANT *pCaster, int x, int y)
{
  CHARACTER *pChar;
  int dx, dy, d2, maxRange2;
  if ( (x < 0) || (x >= MAX_TERRAIN_WIDTH) ) return false;
  if ( (y < 0) || (y >= MAX_TERRAIN_HEIGHT) ) return false;
  pChar = pCaster->m_pCharacter;
  if (pChar == NULL) return false;
  dx = x - pCaster->x;
  dy = y - pCaster->y;
  d2 = dx*dx + dy*dy;
  if (pChar->targets.MaxRange() < 40000)  // Protect against the square overflowing 32 bits
  {
    maxRange2 =  pChar->targets.MaxRange() * pChar->targets.MaxRange();
    //if (d2 > pChar->targets.m_MaxRange * pChar->targets.m_MaxRange) return false;
    if (d2 > maxRange2) return false;
    if (abs(dx) > pChar->targets.m_maxRangeX) return false;
    if (abs(dy) > pChar->targets.m_maxRangeY) return false;
  };
  return true;
}

bool COMBAT_DATA::CanMoveCursorRight(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX+1, m_iCursorY);
}

bool COMBAT_DATA::CanMoveCursorLeft(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX-1, m_iCursorY);
}

bool COMBAT_DATA::CanMoveCursorForward(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX, m_iCursorY-1);
}

bool COMBAT_DATA::CanMoveCursorBackward(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX, m_iCursorY+1);
}

bool COMBAT_DATA::CanMoveCursorNE(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX+1, m_iCursorY-1);
}

bool COMBAT_DATA::CanMoveCursorSE(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX+1, m_iCursorY+1);
}

bool COMBAT_DATA::CanMoveCursorSW(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX-1, m_iCursorY+1);
}

bool COMBAT_DATA::CanMoveCursorNW(const COMBATANT *pCaster)
{
  return CanMoveCursor(pCaster, m_iCursorX-1, m_iCursorY-1);
}



void COMBAT_DATA::moveCursorRight()
{
   int x = m_iCursorX+1;
   int y = m_iCursorY;
 
   if (x < MAX_TERRAIN_WIDTH)
   {
     SetUpdateFlag();
     ForceMapToMoveAlong(x, y, PathEast, 1); 
     m_iCursorX = x;
   }
}

//*****************************************************************************
// NAME:    COMBAT_DATA::determinePartyOrientation
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int COMBAT_DATA::determinePartyOrientation(int x, int y)
{
   int facing = party.GetPartyFacing();
   int width = getHallWidth(x, y, facing);

   if (width < 2)
   {
      switch (facing) 
      {
      case FACE_NORTH:
      case FACE_SOUTH:
         facing = FACE_EAST;
         width = getHallWidth(x, y, facing);
         break;
      case FACE_EAST:
      case FACE_WEST:
         facing = FACE_NORTH;
         width = getHallWidth(x, y, facing);
         break;
      }
    
      if (width < 2)
         WriteDebugString("Cannot orient combat party properly, space too small!\n");
   }

   return facing;
}

#ifdef newPartyArrangement

static int Decode(char c)
{
  if ( (c>='A') && (c<='Z') ) return c - 'A';
  if ( (c>='a') && (c<='z') ) return 'a' - c;
  return 0;
}

#endif

#ifdef newMonsterArrangement



#ifdef newMonsterArrangement
//*****************************************************************************
// NAME: getNextMonsterCombatDirection
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::getNextMonsterCombatDirection(int i, BOOL *reset)
{
  // monsterPartyX is postion of party as seen by the monsters.
  // dir is direction of party from first monster.

  static eventDirType currDir = Any;   
 
  eventDirType lastDir;   

  if (*reset)
  {
    *reset = FALSE;
    currDir = Any;
  };

  int facing;
//  do 
//  {
    lastDir = currDir;
//    GetNextDirection(&currDir, &monster.m_iFacing);
    GetNextDirection(&currDir, &facing);
//    GetMonsterCoords(monster.x, monster.y,
//                     monsterPartyX, monsterPartyY, GetMonsterDistance(currDir), currDir);
//  } while ((monster.x == -1) && (currDir != lastDir));


  //if (monster.x < 0)
  //{
  //  monster.x = m_iPartyStartX;
  //  monster.y = m_iPartyStartY;
  //  TRACE("Setting monster start combat pos at party loc\n");
  //}
 
  // find first friendly combatant
  //int dude = NO_DUDE;
  //int i=0;
  //while ((dude == NO_DUDE) && (i < m_iNumCombatants))
  //{
  //  if (m_aCombatants[i].friendly)
  //    dude = i;
  // i++;
  //}

  int dir = -1;
   
  // find direction from monster to party to help guide 
  // search for empty cell when specified cell is not empty
  //if (dude != NO_DUDE)
  //dir = GetDir(monster.x,
  //             monster.y,
  //             monsterPartyX, //m_aCombatants[dude].x,
  //             monsterPartyY); //m_aCombatants[dude].y);

  //TRACE("Attempting to place %ux%u monster at %u,%u (%u)\n", 
  //                 monster.width, monster.height, monster.x, monster.y, dir);

  // calling findEmptyCell() will search for empty cell
  // in expanding circle.
  // try to find something in direction away from party first
  switch (currDir)
  {
  case North: dir = 0; monsterArrangement.numMonster[0]++; break;
  case East:  dir = 1; monsterArrangement.numMonster[1]++; break;
  case South: dir = 2; monsterArrangement.numMonster[2]++; break;
  case West:  dir = 3; monsterArrangement.numMonster[3]++; break;
  default:
    ASSERT(true);
    dir = PathNorth;
  };
  monsterArrangement.monsterPlacement[i].directionFromParty = dir;
  monsterArrangement.monsterPlacement[i].facing = facing;

  //if (findEmptyCell(monster.x, monster.y, monster.width, monster.height, dir, NULL))
  //{
  //   TRACE("Found empty cell for monster (%u,%u)\n", monster.x, monster.y);
  //}
  //else
  //{
  //   // now try to put the monster anywhere we can
  //   // does not check for reachable path to party, just place in first non-obstical location
  //   TRACE("First call to findEmptyCell() failed - widening search...\n");
  //   if (findEmptyCell(monster.x, monster.y, monster.width, monster.height, PathBAD, NULL))
  //   {
  //     TRACE("Found empty cell for monster (%u,%u)\n", monster.x, monster.y);
  //   }
  //   else
  //   {
  //     WriteDebugString("Did NOT find empty cell for monster\n");
  //   }
  //}
}
#endif





#endif

//*****************************************************************************
// NAME: determineInitCombatPos
//
// PURPOSE:  
//*****************************************************************************

void COMBAT_DATA::determineInitCombatPos(COMBATANT *pCombatant,
                                         int         i,
                                         BOOL       *reset,
                                         BOOL        ignoreCurrentCombatant,
                                         const char *partyArrangement
                                         //int *monsterPartyX,    // As seen by the monsters
                                         //int *monsterPartyY
                                         )
{
  //if (combatants[i].IsPartyMember())
  if (pCombatant->GetIsFriendly())
  {
#ifdef newPartyArrangement
    int facing, dirStr, partySize, partyArrangementIndex;
    // Save the position of the party itself as seen by the monsters.
    // Manikus says this is the position of the party member in the
    // case that the party consists of one character.
    facing = party.GetPartyFacing();

    switch (facing)
    {
    default:  // To keep the compiler quiet.
    case FACE_NORTH: dirStr = 0; break;
    case FACE_EAST:  dirStr = 1; break;
    case FACE_SOUTH: dirStr = 2; break;
    case FACE_WEST : dirStr = 3; break;
    };
    partyArrangementIndex = dirStr*(MAX_PARTY_MEMBERS+1)*(MAX_PARTY_MEMBERS);
    //partyArrangement += dirStr*(MAX_PARTY_MEMBERS+1)*(MAX_PARTY_MEMBERS);
    // partyArrangement points to one of four direction strings.
    //
    // Now find the string that corresponds to the party size.
    partySize = party.numCharacters;
    //partyArrangement += (partySize-1)*(partySize);   //n*(a+b)/2=(ps-1)*(2+2*(ps-1))/2=(ps-1)*(1+ps-1)
    partyArrangementIndex += (partySize-1)*(partySize);   //n*(a+b)/2=(ps-1)*(2+2*(ps-1))/2=(ps-1)*(1+ps-1)
    // Now we assume that the marching order is equal to i.
    //partyArrangement += 2*i;
    partyArrangementIndex += 2*i;

    //getNextCharCombatPos(pCombatant, reset, ignoreCurrentCombatant, partyArrangement);
    getNextCharCombatPos(pCombatant,
                         reset,
                         ignoreCurrentCombatant,
                         partyArrangement + partyArrangementIndex);
#else
     getNextCharCombatPos(pCombatant, reset, ignoreCurrentCombatant);
#endif
     if (pCombatant->x >= 0)
     {
       placeCombatant(pCombatant->x, 
                      pCombatant->y,
                      i, 
                      pCombatant->width,
                      pCombatant->height);
     }
  }
  else 
  {
#ifdef newMonsterArrangement
    /*
    if (i == 0)
    {
      *monsterPartyX = combatData.m_iPartyStartX + Decode(partyArrangement[0]);
      *monsterPartyY = combatData.m_iPartyStartY + Decode(partyArrangement[1]);
    };
    getNextMonsterCombatPos(*pCombatant, *monsterPartyX, *monsterPartyY, reset);
    if (pCombatant->x >= 0)
    {
      placeCombatant(pCombatant->x, 
                     pCombatant->y, 
                     i, 
                     pCombatant->width,
                     pCombatant->height);
    };
    */
#else
     getNextMonsterCombatPos(*pCombatant);
     if (pCombatant->x >= 0)
     {
       placeCombatant(pCombatant->x, 
                      pCombatant->y, 
                      i, 
                      pCombatant->width,
                      pCombatant->height);
     }
#endif
  };
}

extern const int DUNGEON_TILE_COUNT = 26;
extern const int DUNGEON_EMPTY_TILE = 23;
extern const int WILDERNESS_TILE_COUNT = 40;
extern const int WILDERNESS_EMPTY_TILE = 38;
extern const int MAX_TILES = max(WILDERNESS_TILE_COUNT, DUNGEON_TILE_COUNT);
extern TILE_DATA CurrentTileData[MAX_TILES+1];
extern int CurrentTileCount;

void ComputeDistanceFromParty(void)
{
  // Fill the monsterArrangement.distanceFromParty array.
  int *queue, queueStart, queueLen;
  queue = (int *)malloc(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT*sizeof(int));
  memset(monsterArrangement.distanceFromParty, 254, MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
  // 254 represents cells whose distance is unknown.
  // 255 represents a cell that is inaccessible.
  // 0 --> 253 represents a cell the party can reach.
  queueLen = 1;
  queueStart = 0;
  queue[queueStart] = MAX_TERRAIN_WIDTH*monsterArrangement.partyY + monsterArrangement.partyX;
  monsterArrangement.distanceFromParty[queue[queueStart]] = 0;
  while (queueLen > 0)
  {
    int d;
    int x, y, dx, dy;
    queueLen--;
    x = queue[queueStart] % MAX_TERRAIN_WIDTH;
    y = queue[queueStart] / MAX_TERRAIN_WIDTH;
    d = monsterArrangement.distanceFromParty[queue[queueStart]] + 1;
    queueStart = (queueStart+1)%(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
    if (d > 253) 
    {
      d = 253;
    };
    // Examine the 8 cells around x and y;
    for (dx=-1; dx<=1; dx++)
    {
      for (dy=-1; dy<=1; dy++)
      {
        int X, Y, index;
        if ((dx==0)&&(dy==0))continue;
        X = x+dx;
        Y = y+dy;
        if (X < 0) continue;
        if (Y < 0) continue;
        if (X >= MAX_TERRAIN_WIDTH) continue;
        if (Y >= MAX_TERRAIN_HEIGHT) continue;
        index = Y*MAX_TERRAIN_WIDTH + X;
        if (monsterArrangement.distanceFromParty[index] == 254)
        {
          // If this cell is accessible add it to stack; else mark it inaccessible.
          int cell;
          cell = terrain[y][x].cell;
          if ((cell < 1)||(cell > CurrentTileCount)) continue;  
          if (CurrentTileData[cell].tile_passable > 0)
          {
            int j;
            j = (queueStart+queueLen)%(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
            monsterArrangement.distanceFromParty[index] = d;
            queue[j] = index;
            queueLen++;
          }
          else
          {
            monsterArrangement.distanceFromParty[index] = 255;
          };
        };
      };
    };
  };
  free(queue);
};


char partyIndoorCombatArrangement[4*(MAX_PARTY_MEMBERS+1)*(MAX_PARTY_MEMBERS)+1] =
  // North
    "AB" "ABBB" "bBABBB" "ABBBBCCC" "bBABBBBCCC" "bBABBBACCCBC" "bBABBBCBACCCBC" "bBABBBCBACDCBCCC" "cBbBABBBCBACDCBCCC" "cBbBABBBCBbCACDCBCCC" "cBbBABBBCBDBbCACDCBCCC" "cBbBABBBCBDBbCACDCBCECCC"
  //East
    "bA" "bAAB" "cbbAAB" "bAABcAbB" "cbbAABcAbB" "cbbAABdbbBcA" "cbbAABBCdbbBcA" "cbbAABBCdbACcAbB" "dccbbAABBCdbACcAbB" "dccbbAABBCecACdbcAbB" "dccbbAABBCCDecACdbcAbB" "dccbbAABBCCDecBDdbcAbBAC"
  //South
    "AA" "AAbA" "BAAAbA" "AAbAbbcb" "BAAAbAbbcb" "BAAAbAAbcbbb" "CABAAAbAAbcbbb" "CABAAAbABbcbAbbb" "CABAAAbAcABbcbAbbb" "CABAAAbAcABbdbcbAbbb" "DACABAAAbAcABbdbcbAbbb" "DACABAAAbAcACbdbBbcbAbbb" 
  //West
    "BB" "BBAA" "CCBBAA" "BBAACBBA" "CCBBAACBBA" "CCBBAADCBACB" "CCBBAAbbDCBACB" "CCBBAAbbDCAbCBBA" "CCBBAAbbccDCAbCBBA" "CCBBAAbbccDCbcAbCBBA" "DDCCBBAAbbccDCbcAbCBBA" "DDCCBBAAbbccEDbcAbDCCBBA"
  //// North
  //  "AA" "AABA" "AABABC" "AABABCCC" "AABACABCCC" "AABACABCCCDC" "AABACABCCCDCDA" "AABACABCCCDCDAEC"
  ////East
  //  "AA" "AABB" "AABBcA" "AABBcAbB" "AABBCCcAbB" "AABBCCcAbBAC" "AABBCCcAbBACDD" "AABBCCcAbBACDDBD"
  ////South
  //  "AA" "AAbA" "AAbAbc" "AAbAbccc" "AAbAcAbccc" "AAbAcAbcccdc" "AAbAcAbcccdcdA" "AAbAcAbcccdcdAec"
  ////West
  //  "AA" "AAbB" "AAbBCA" "AAbBCABB" "AAbBcCCABB" "AAbBcCCABBAC" "AAbBcCCABBACdD" "AAbBcCCABBACdDbD"
;


char partyOutdoorCombatArrangement[4*(MAX_PARTY_MEMBERS+1)*(MAX_PARTY_MEMBERS)+1] =
  // North
  "AB"
  "ABBB"
  "bBABBB"
  "bBABBBCB"
  "cBbBABBBCB"
  "cBbbABBBCBDB"
  "dBcBbBABBBCBDB"
  "eBdBcBbBABBBCBDB"
  "eBdBcBbBABBBCBDBEB"
  "fBeBdBcBbBABBBCBDBEB"
  "fBeBdBcBbBABBBCBDBEBFB"
  "gBfBeBdBcBbBABBBCBDBEBFB"
//East
  "bA"
  "bAAB"
  "cbbAAB"
  "cbbAABBC"
  "dccbbAABBC"
  "dccbbAABBCCD"
  "eddccbbAABBCCD"
  "eddccbbAABBCCDDE"
  "feeddccbbAABBCCDDE"
  "feeddccbbAABBCCDDEEF"
  "gffeeddccbbAABBCCDDEEF"
  "gffeeddccbbAABBCCDDEEFFG"
  //South
  "AA"
  "AAbA"
  "BAAAbA"
  "CABAAAbA"
  "CABAAAbAbA"
  "DACABAAAbAcA"
  "DACABAAAbAcAdA"
  "EADACABAAAbAcAdA"
  "EADACABAAAbAcAdAeA"
  "FAEADACABAAAbAcAdAeA"
  "FAEADACABAAAbAcAdAeAfA"
  "GAFAEADACABAAAbAcAdAeAfA"
  //West
  "BB"
  "BBAA"
  "CCBBAA"
  "CCBBAAbb"
  "DDCCBBAAbb"
  "DDCCBBAAbbcc"
  "EEDDCCBBAAbbcc"
  "EEDDCCBBAAbbccdd"
  "FFEEDDCCBBAAbbccdd"
  "FFEEDDCCBBAAbbccddee"
  "GGFFEEDDCCBBAAbbccddee"
  "GGFFEEDDCCBBAAbbccddeeff"
  ;
  /*
  // North
  "AA"
  "AABA"
  "bAAABA"
  "bAAABACA"
  "cAbAAABACA"
  "cAbAAABACADA"
  "dAcAbAAABACADA"
  "dAcAbAAABACADAEA"

//East
  "AA"
  "AABB"
  "bbAABB"
  "bbAABBCC"
  "ccbbAABBCC"
  "ccbbAABBCCDD"
  "ddccbbAABBCCDD"
  "ddccbbAABBCCDDEE"
  
  //South
  "AA"
  "AAbA"
  "BAAAbA"
  "BAAAbAcA"
  "CABAAAbAcA"
  "CABAAAbAcAdA"
  "DACABAAAbAcAdA"
  "DACABAAAbAcAdAeA"

  //West
  "AA"
  "AAbb"
  "BBAAbb"
  "BBAAbbcc"
  "CCBBAAbbcc"
  "CCBBAAbbccdd"
  "DDCCBBAAbbccdd"
  "DDCCBBAAbbccddee";
*/

//*****************************************************************************
// NAME: determineInitCombatPos
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::determineInitCombatPos()
{
  int i;   
  BOOL pReset = TRUE, mReset=TRUE;
  const char *partyArrangement;
  
  monsterArrangement.Activate(NumCombatants());
  
  
  
  partyArrangement = partyIndoorCombatArrangement;
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    int offset;
    const char *pc;
    hookParameters[5] = "0";
    hookParameters[6] = "0";
    switch (party.facing)
    {
    case 0:
      RunGlobalScript("Global_Combat", "PartyOriginNorth", true);
      break;
    case 1:
      RunGlobalScript("Global_Combat", "PartyOriginEast", true);
      break;
    case 2:
      //WriteDebugString("DEBUG - Running PartyOriginSouth script\n");
      RunGlobalScript("Global_Combat", "PartyOriginSouth", true);
      break;
    case 3:
      RunGlobalScript("Global_Combat", "PartyOriginWest", true);
      break;
    };
    pc = (LPCTSTR)hookParameters[5];
    offset = ScriptAtoI(&pc, -8, +8);
    m_iPartyOriginX = m_iPartyStartX + offset;
    pc = (LPCTSTR)hookParameters[6];
    offset = ScriptAtoI(&pc, -8, +8);
    m_iPartyOriginY = m_iPartyStartY + offset;
  };


  //WriteDebugString("DEBUG - Initial Party position = (%d, %d)\n", m_iPartyOriginX, m_iPartyOriginY);

  
  
  partyArrangement = partyIndoorCombatArrangement;
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CString scriptResult;
    hookParameters[5] = "I";
    //partyArrangement = partyIndoorCombatArrangement;
    if (this->m_bIsWilderness)
    {
      hookParameters[5] = "O";
      partyArrangement = partyOutdoorCombatArrangement;
    }
    else
    {
      hookParameters[5] = "I";
      partyArrangement = partyIndoorCombatArrangement;
    };
    scriptResult = RunGlobalScript("Global_Combat", "PartyArrangement", true);
    if (!scriptResult.IsEmpty())
    {
      if (scriptResult.GetLength() == (signed)strlen(partyIndoorCombatArrangement))
      {
        partyArrangement = (LPCSTR)scriptResult;
      }
      else
      {
        if (!debugStrings.AlreadyNoted(CString("PASWS")))
        {
          debugSeverity = 9;
          WriteDebugString("Party arrangement string is wrong size\n");
          MsgBoxInfo("Party arrangement string is wrong size");
        }
      };
    };
  };

  //WriteDebugString("DEBUG - partyArrangement = %s\n", partyArrangement);
    // Allocate a global structure to contain information about monster placement.

  // PC's and NPC's should be listed in combatData by marching order
  for (i=0; i<NumCombatants()/*m_aCombatants.GetSize()*/; i++)
  {
    if (m_aCombatants[i].GetIsFriendly())
    {
      int rx, ry;
      determineInitCombatPos(&m_aCombatants[i], 
                              i, 
                             &pReset, 
                              TRUE, 
                              partyArrangement /*,
                              &monsterPartyX,
                              &monsterPartyY */);
      monsterArrangement.monsterPlacement[i].placeX = m_aCombatants[i].x;
      monsterArrangement.monsterPlacement[i].placeY = m_aCombatants[i].y;
      monsterArrangement.numParty++;
      rx = m_aCombatants[i].x -m_iPartyOriginX;
      ry = m_aCombatants[i].y -m_iPartyOriginY;
      if (rx > monsterArrangement.partyMaxX) monsterArrangement.partyMaxX = rx;
      if (rx < monsterArrangement.partyMinX) monsterArrangement.partyMinX = rx;
      if (ry > monsterArrangement.partyMaxY) monsterArrangement.partyMaxY = ry;
      if (ry < monsterArrangement.partyMinY) monsterArrangement.partyMinY = ry;
    }
    else
    {
      getNextMonsterCombatDirection(i, &mReset);
    }
  };


  monsterArrangement.totalNumMonster = monsterArrangement.numMonster[0]
                                      + monsterArrangement.numMonster[1]
                                      + monsterArrangement.numMonster[2]
                                      + monsterArrangement.numMonster[3];
  {
    // We are going to call the MonsterArrangement hook to determine the
    // monster arrangement in battle.
    // We will call the hook four times....once for north, east, south, and west.
    HOOK_PARAMETERS hookParameters;
    monsterArrangement.partyX = m_iPartyOriginX;
    monsterArrangement.partyY = m_iPartyOriginY;
    for (i=0; i<monsterArrangement.numParty; i++)
    {
      monsterArrangement.partyPositions[i].x = 
          m_aCombatants[i].x - monsterArrangement.partyX;
      monsterArrangement.partyPositions[i].y = 
          m_aCombatants[i].y - monsterArrangement.partyY;
    };
    ComputeDistanceFromParty();
    for (i=0; i<4; i++)  // each of four directions
    {
      if (monsterArrangement.numMonster[i] == 0) continue;
      monsterArrangement.curDirFromParty = i;
      monsterArrangement.currentMonster = -1;
      monsterArrangement.turtleWidth = 1;
      monsterArrangement.turtleHeight = 1;
      monsterArrangement.lineOfSight = false;
      monsterArrangement.partyAccessible = true;
      monsterArrangement.limitMinY = 0x80000000;
      monsterArrangement.limitMinX = 0x80000000;
      monsterArrangement.limitMaxY = 0x7fffffff;
      monsterArrangement.limitMaxX = 0x7fffffff;
      switch (i)
      {
      case 0: // Headed North
              monsterArrangement.dx['F'-'A'] = -1;
              monsterArrangement.dx['R'-'A'] = 1;
              monsterArrangement.dx['L'-'A'] = -1;
              monsterArrangement.dx['B'-'A'] = 1;
              monsterArrangement.dy['F'-'A'] = -1;
              monsterArrangement.dy['R'-'A'] = 0;
              monsterArrangement.dy['L'-'A'] = 0;
              monsterArrangement.dy['B'-'A'] = 1;
              break;
      case 1: // Headed East
              monsterArrangement.dx['F'-'A'] = 1;
              monsterArrangement.dx['R'-'A'] = 1;
              monsterArrangement.dx['L'-'A'] = -1;
              monsterArrangement.dx['B'-'A'] = -1;
              monsterArrangement.dy['F'-'A'] = 0;
              monsterArrangement.dy['R'-'A'] = 1;
              monsterArrangement.dy['L'-'A'] = -1;
              monsterArrangement.dy['B'-'A'] = 0;
              break;
      case 2: // Headed south
              monsterArrangement.dx['F'-'A'] = 1;
              monsterArrangement.dx['R'-'A'] = -1;
              monsterArrangement.dx['L'-'A'] = 1;
              monsterArrangement.dx['B'-'A'] = -1;
              monsterArrangement.dy['F'-'A'] = 1;
              monsterArrangement.dy['R'-'A'] = 0;
              monsterArrangement.dy['L'-'A'] = 0;
              monsterArrangement.dy['B'-'A'] = -1;
              break;
      case 3: // Headed West
              monsterArrangement.dx['F'-'A'] = -1;
              monsterArrangement.dx['R'-'A'] = -1;
              monsterArrangement.dx['L'-'A'] = 1;
              monsterArrangement.dx['B'-'A'] = 1;
              monsterArrangement.dy['F'-'A'] = 0;
              monsterArrangement.dy['R'-'A'] = -1;
              monsterArrangement.dy['L'-'A'] = 1;
              monsterArrangement.dy['B'-'A'] = 0;
              break;
      };
      monsterArrangement.turtleX = 0;
      monsterArrangement.turtleStack[0] = 0;
      monsterArrangement.turtleStack[1] = 0;
      monsterArrangement.turtleStack[2] = 0;
      monsterArrangement.turtleStack[3] = 0;
      monsterArrangement.turtleY = 0;
      monsterArrangement.distance = -1;
      // 31 July 2014 PRS switch (m_pEvent->distance) 
      switch (monsterDistance)
      {
      case UpClose:
        RunGlobalScript("CombatPlacement","PlaceMonsterClose", true);  // Callback function immediately below.
        break;
      case Nearby:
        RunGlobalScript("CombatPlacement","PlaceMonsterNear", true);  // Callback function immediately below.
        break;
      case FarAway:
        RunGlobalScript("CombatPlacement","PlaceMonsterFar", true);  // Callback function immediately below.
      };
    };
  };
  monsterArrangement.deActivate();
}

bool CanPlaceMonsterHere(int i, int j, int w, int h);

void FindCurrentMonsterToPlace(void)
{
  int i, n;
  n = combatData.NumCombatants();
  monsterArrangement.turtleWidth = 1;
  monsterArrangement.turtleHeight = 1;
  for (i=0; i<n; i++)
  {
    if (   (monsterArrangement.monsterPlacement[i].directionFromParty == monsterArrangement.curDirFromParty)
        && (monsterArrangement.monsterPlacement[i].placeX < 0))
    {
      monsterArrangement.currentMonster = i;
      return;
    };
  };
  monsterArrangement.currentMonster = -1;
}


bool WithinSight(int partyRelativeX, int partyRelativeY)
{
  //Is the turtle within sight of any party member or previously placed monster?
  int tx, ty;  //Turtle position.  Save some typing.
  int i, n;
  tx = monsterArrangement.partyX + partyRelativeX;
  ty = monsterArrangement.partyY + partyRelativeY;
  n = combatData.NumCombatants();
  for (i=0; i<n; i++)
  {
    MonsterPlacement *pMP;
    pMP = &monsterArrangement.monsterPlacement[i];
    if (pMP->placeX > 0)
    {
      if (IsLineOfSight(tx, 
                        ty, 
                        pMP->placeX ,
                        pMP->placeY))
      {
        return true;
      };
    };
  };
  return false;
}

bool PlantCombatant(int partyRelativeX, int partyRelativeY)
{
  int n;
  COMBATANT *pCombatant;
  n = monsterArrangement.currentMonster;
  pCombatant = combatData.GetCombatant(n);
  if (!CanPlaceMonsterHere(partyRelativeX + monsterArrangement.partyX,
                           partyRelativeY + monsterArrangement.partyY,
                           pCombatant->width,
                           pCombatant->height))
  {
    return false;
  };

  if (partyRelativeX-partyRelativeY < monsterArrangement.limitMinX) return false;
  if (partyRelativeX-partyRelativeY > monsterArrangement.limitMaxX) return false;
  if (partyRelativeY < monsterArrangement.limitMinY) return false;
  if (partyRelativeY > monsterArrangement.limitMaxY) return false;
  if (monsterArrangement.distance >= 0)
  {
    int i, dx, dy;
    for (i=0; i<monsterArrangement.numParty; i++)
    {
      dx = partyRelativeX - monsterArrangement.partyPositions[i].x;
      dy = partyRelativeY - monsterArrangement.partyPositions[i].y;
      if (dx*dx + dy*dy < monsterArrangement.distance)
      {
        return false;
      };
    };
  };
  if (monsterArrangement.lineOfSight)
  {
    if (!WithinSight(partyRelativeX, partyRelativeY))
    {
      return false;
    };
  };
  pCombatant->x = partyRelativeX + monsterArrangement.partyX;
  pCombatant->y = partyRelativeY + monsterArrangement.partyY;
  placeCombatant(partyRelativeX + monsterArrangement.partyX, 
                 partyRelativeY + monsterArrangement.partyY,
                 n,
                 pCombatant->width,
                 pCombatant->height);
  monsterArrangement.monsterPlacement[n].placeX = partyRelativeX + monsterArrangement.partyX;
  monsterArrangement.monsterPlacement[n].placeY = partyRelativeY + monsterArrangement.partyY;  
  monsterArrangement.currentMonster = -1;
  return true;
}


void MoveTurtleX(int newX)
{
  monsterArrangement.turtleX = newX;
}

void MoveTurtleY(int newY)
{
  monsterArrangement.turtleX += newY - monsterArrangement.turtleY;
  monsterArrangement.turtleY = newY;
}

CString MonsterPlacementCallback(const CString& turtlecode)
{
  CString result;

  static bool errorMessage = false;
  int i, n, repeat=0;
  static bool activeMessage = false;
  if (!monsterArrangement.active)
  {
    if (!activeMessage)
    {
      activeMessage = true;
      WriteDebugString("GPDL function $MonsterPlacement() called other than in response to MonsterPlacement hook\n");
    };
    return "0";
  };
  n = turtlecode.GetLength();
  for (i=0; i<n; i++)
  {
    char c = turtlecode[i];
    switch (c)
    {
    case 'V':
      monsterArrangement.lineOfSight = true;
      repeat = 0;
      break;
    case 'S':
      {
        if (monsterArrangement.currentMonster < 0) FindCurrentMonsterToPlace();
        if (monsterArrangement.currentMonster >= 0)
        {
          COMBATANT *pCombatant;
          pCombatant = combatData.GetCombatant(monsterArrangement.currentMonster);
          monsterArrangement.turtleWidth = pCombatant->width;
          monsterArrangement.turtleHeight = pCombatant->height;
        };
      };
      repeat = 0;
      break;
    case '?':
      {
        OBSTICAL_TYPE ot;
        if (monsterArrangement.currentMonster < 0) FindCurrentMonsterToPlace();
        if (monsterArrangement.currentMonster < 0)
        {
          result += '0';
        }
        else
        {
          char oc;
          //COMBATANT *pCombatant;
          //pCombatant = combatData.GetCombatant(monsterArrangement.currentMonster);
          ot = ObsticalType(monsterArrangement.turtleX + monsterArrangement.partyX,
                            monsterArrangement.turtleY + monsterArrangement.partyY,
                            monsterArrangement.turtleWidth,
                            monsterArrangement.turtleHeight,
                            TRUE,
                            TRUE,
                            NULL);
          switch(ot)
          {
          case OBSTICAL_wall:           oc = 'w'; break;
          case OBSTICAL_lingeringSpell: oc = 's'; break;
          case OBSTICAL_occupied:       oc = 'o'; break;
          case OBSTICAL_offMap:         oc = 'i'; break;                              
          default:                      oc = 'n'; break;
          };
          result += oc;
        };
      };
      repeat = 0;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      repeat = 10*repeat + c - '0';
      break;
    case 'L':
    case 'F':
    case 'R':
    case 'B':
      if (repeat == 0) repeat = 1;
      monsterArrangement.turtleX += repeat*monsterArrangement.dx[c-'A'];
      monsterArrangement.turtleY += repeat*monsterArrangement.dy[c-'A'];
      repeat = 0;
      break;
    case 'P':
      {
        // Is there an unplaced monster in this direction?
        int m;
        m = monsterArrangement.currentMonster;
        if (m < 0) FindCurrentMonsterToPlace();
        m = monsterArrangement.currentMonster;
        if (m < 0) 
        {
          result += '0';
        }
        else
        {
          PlantCombatant(monsterArrangement.turtleX, monsterArrangement.turtleY);
        };
      };
      repeat = 0;
      break;
    case 'b':
      {
        switch (monsterArrangement.curDirFromParty)
        {
        case 0: // North
          monsterArrangement.limitMaxY = monsterArrangement.turtleY;
          break;
        case 1: // East
          monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        case 2: // South
          monsterArrangement.limitMinY = monsterArrangement.turtleY;
          break;
        case 3: // West
          monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        };
      };
      repeat = 0;
      break;
    case 'f':
      {
        switch (monsterArrangement.curDirFromParty)
        {
        case 0: // North
          monsterArrangement.limitMinY = monsterArrangement.turtleY;
          break;
        case 1: // East
          monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        case 2: // South
          monsterArrangement.limitMaxY = monsterArrangement.turtleY;
          break;
        case 3: // West
          monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        };
      };
      repeat = 0;
      break;
    case 'l':  // 
      {
        switch (monsterArrangement.curDirFromParty)
        {
        case 0: // North
          monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        case 1: // East
          monsterArrangement.limitMinY = monsterArrangement.turtleY;
          break;
        case 2: // South
          monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        case 3: // West
          monsterArrangement.limitMaxY = monsterArrangement.turtleY;
          break;
        };
      };
      repeat = 0;
      break;
    case 'r':
      {
        switch (monsterArrangement.curDirFromParty)
        {
        case 0: // North
          monsterArrangement.limitMaxX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        case 1: // East
          monsterArrangement.limitMaxY = monsterArrangement.turtleY;
          break;
        case 2: // South
          monsterArrangement.limitMinX = monsterArrangement.turtleX - monsterArrangement.turtleY;
          break;
        case 3: // West
          monsterArrangement.limitMinY = monsterArrangement.turtleY;
          break;
        };
      };
      repeat = 0;
      break;
    case 'd':
      {
        int j, dx, dy, d2;
        monsterArrangement.distance = 0x7fffffff;
        for (j=0; j<monsterArrangement.numParty; j++)
        {
          dx = monsterArrangement.turtleX - monsterArrangement.partyPositions[j].x;
          dy = monsterArrangement.turtleY - monsterArrangement.partyPositions[j].x;
          d2 = dx*dx + dy*dy;
          if (d2 < monsterArrangement.distance) 
          {
            monsterArrangement.distance = d2;
          };
        };
      };
      repeat = 0;
      break;
    case 'E':
      {
        int radius, maxRadius;
        char *cellsAttempted;
        int cellAttIndx, maxCells;
        maxRadius = MAX_TERRAIN_WIDTH;
        if (maxRadius > MAX_TERRAIN_HEIGHT) maxRadius = MAX_TERRAIN_HEIGHT;
        maxRadius = maxRadius/4;
        maxCells = (2*maxRadius+1)*(2*maxRadius+1);
        cellsAttempted = (char *)malloc(maxCells);
        cellAttIndx = 2*maxRadius*(maxRadius+1);
        if (cellsAttempted == NULL)
        {
          die(0x439805);
        };
        memset(cellsAttempted,0,maxCells);
        FindCurrentMonsterToPlace();
        if (repeat == 0) repeat = 1;
        if (monsterArrangement.currentMonster >= 0)
        {
          for (radius=0;
               //radius<min(MAX_TERRAIN_WIDTH,MAX_TERRAIN_HEIGHT)/3; 
               radius<=maxRadius; 
               cellAttIndx -= (2*radius+2)*(2*maxRadius+1)+1, radius++)  // Gotta impose some limit!
          {
            int x, y, left, right, top, bottom;
            left = monsterArrangement.turtleX - radius;
            right = monsterArrangement.turtleX + radius;
            top = monsterArrangement.turtleY - radius;
            bottom = monsterArrangement.turtleY + radius;
            for (x=left; x<=right; x++, cellAttIndx+=2*(maxRadius-radius))
            {
              for (y=top; y<=bottom; y++, cellAttIndx++)
              {
                int dx, dy;
                if (monsterArrangement.currentMonster < 0) break;
                if (repeat <= 0) break;
                dx = x - monsterArrangement.turtleX;
                dy = y - monsterArrangement.turtleY;
                if (dx*dx + dy*dy > radius*radius) continue;
                if (cellsAttempted[cellAttIndx] != 0) continue;
                cellsAttempted[cellAttIndx] = 1;
                if (PlantCombatant(x, y))
                {
                  repeat--;
                  FindCurrentMonsterToPlace(); // See if there is another.
                };
              };
            };
          };
        };
        free(cellsAttempted);
      };
      repeat = 0;
      break;
    case 'w':
      switch (monsterArrangement.curDirFromParty)
      {
      case 0:
        MoveTurtleY(monsterArrangement.partyMinY); break;
        break;
      case 1:
        MoveTurtleX(monsterArrangement.partyMaxX); break;
        break;
      case 2:
        MoveTurtleY(monsterArrangement.partyMaxY); break;
        break;
      case 3:
        MoveTurtleX(monsterArrangement.partyMinX); break;
        break;
      }
      break;
    case 'n':
      switch (monsterArrangement.curDirFromParty)
      {
      case 0:
        MoveTurtleX(monsterArrangement.partyMaxY); break;
        break;
      case 1:
        MoveTurtleX(monsterArrangement.partyMinX); break;
        break;
      case 2:
        MoveTurtleY(monsterArrangement.partyMinY); break;
        break;
      case 3:
        MoveTurtleX(monsterArrangement.partyMaxX); break;
        break;
      }
      break;
    case 'p':
      switch (monsterArrangement.curDirFromParty)
      {
      case 0:
        MoveTurtleX(monsterArrangement.partyMinX); break;
        break;
      case 1:
        MoveTurtleY(monsterArrangement.partyMinY); break;
        break;
      case 2:
        MoveTurtleX(monsterArrangement.partyMaxX); break;
        break;
      case 3:
        MoveTurtleY(monsterArrangement.partyMaxY); break;
        break;
      }
      break;
    case 's':
      switch (monsterArrangement.curDirFromParty)
      {
      case 0:
        MoveTurtleX(monsterArrangement.partyMaxX); break;
        break;
      case 1:
        MoveTurtleY(monsterArrangement.partyMaxY); break;
        break;
      case 2:
        MoveTurtleX(monsterArrangement.partyMinX); break;
        break;
      case 3:
        MoveTurtleY(monsterArrangement.partyMinY); break;
        break;
      }
      break;
    case 'u':
      monsterArrangement.turtleStack[3] = monsterArrangement.turtleStack[1];
      monsterArrangement.turtleStack[2] = monsterArrangement.turtleStack[0];
      monsterArrangement.turtleStack[0] = monsterArrangement.turtleX;
      monsterArrangement.turtleStack[1] = monsterArrangement.turtleY;
      break;
    case 'o':
      monsterArrangement.turtleX = monsterArrangement.turtleStack[0];
      monsterArrangement.turtleY = monsterArrangement.turtleStack[1];
      monsterArrangement.turtleStack[0] = monsterArrangement.turtleStack[2];
      monsterArrangement.turtleStack[1] = monsterArrangement.turtleStack[3];
      monsterArrangement.turtleStack[3] = 0;
      monsterArrangement.turtleStack[2] = 0;
      break;
    default:
      if (!errorMessage)
      {
        WriteDebugString("Illegal command byte in $MonsterPlacement() code = %s\n", turtlecode);
        errorMessage = true;
      };
      result += 'e';
      repeat = 0;
      break;
    };
  };
  return result;
}

bool CanPlaceMonsterHere(int x, int y, int w, int h)
// See 'findEmptyCell' for the source of this algorithm
{
  if (OBSTICAL_none != ObsticalType(x, y, w, h, TRUE, TRUE, NULL))
  {
    return false;
  };
  return true;  // For now......no additional checks for distance, etc.
}

//*****************************************************************************
// NAME:    COMBAT_DATA::GetNextDirection
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::GetNextDirection(eventDirType *pCurrDir, int *pFacing)
{
   // Get the next direction to place monsters in
   switch (m_pEvent->direction) 
   {
   case Any:
     switch (*pCurrDir)
     {
     case North:
        *pCurrDir = East;
        break;
     case East:
        *pCurrDir = South;
        break;
     case South:
        *pCurrDir = West;
        break;
     case West:
        *pCurrDir = North;
        break;
     default:
       switch (party.GetPartyFacing()) 
       {
       case FACE_NORTH:
          *pCurrDir = North;
          break;
       case FACE_EAST:
          *pCurrDir = East;
          break;
       case FACE_SOUTH:
          *pCurrDir = South;
          break;
       case FACE_WEST:
          *pCurrDir = West;
          break;
       }
       break;
     }
     break;

   case InFront:
     switch (party.GetPartyFacing()) 
     {
     case FACE_NORTH:
        *pCurrDir = North;
        break;
     case FACE_EAST:
        *pCurrDir = East;
        break;
     case FACE_SOUTH:
        *pCurrDir = South;
        break;
     case FACE_WEST:
        *pCurrDir = West;
        break;
     }
     break;

   case N_W_E:
      if (*pCurrDir == West)
         *pCurrDir = North;
      else if (*pCurrDir == North)
         *pCurrDir = East;
      else
         *pCurrDir = West;
      break;
   case North:
      *pCurrDir = North;
      break;
   case W_S_E:
      if (*pCurrDir == East)
         *pCurrDir = South;
      else if (*pCurrDir == South)
         *pCurrDir = West;
      else
         *pCurrDir = East;
      break;
   case South:
      *pCurrDir = South;
      break;
   case N_S_E:
      if (*pCurrDir == North)
         *pCurrDir = East;
      else if (*pCurrDir == East)
         *pCurrDir = South;
      else
         *pCurrDir = North;
      break;
   case East:
      *pCurrDir = East;
      break;
   case N_S_W:
      if (*pCurrDir == South)
         *pCurrDir = West;
      else if (*pCurrDir == West)
         *pCurrDir = North;
      else
         *pCurrDir = South;
      break;
   case West:
      *pCurrDir = West;
      break;
   case N_S:
      if (*pCurrDir == North)
         *pCurrDir = South;
      else
         *pCurrDir = North;
      break;
   case N_E:
      if (*pCurrDir == North)
         *pCurrDir = East;
      else
         *pCurrDir = North;
      break;
   case N_W:
      if (*pCurrDir == West)
         *pCurrDir = North;
      else
         *pCurrDir = West;
      break;
   case S_E:
      if (*pCurrDir == East)
         *pCurrDir = South;
      else
         *pCurrDir = East;
      break;
   case S_W:
      if (*pCurrDir == South)
         *pCurrDir = West;
      else
         *pCurrDir = South;
      break;
   case E_W:
      if (*pCurrDir == East)
         *pCurrDir = West;
      else
         *pCurrDir = East;
      break;
   }

   switch (*pCurrDir) 
   {
   case North:
     *pFacing = FACE_WEST;
     TRACE("Placing monster NORTH of party\n");
     break;
   case East:
     *pFacing = FACE_WEST;
     TRACE("Placing monster EAST of party\n");
     break;
    
   case South:
     *pFacing = FACE_EAST;
     TRACE("Placing monster SOUTH of party\n");
     break;
   case West:
     *pFacing = FACE_EAST;
     TRACE("Placing monster WEST of party\n");
     break;   
   }
}

//*****************************************************************************
// NAME:    COMBAT_DATA::GetMonsterDistance
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int COMBAT_DATA::GetMonsterDistance(eventDirType dir)
{
   int dist = 0;
#ifdef newMonsterArrangement
   //31 July 2014 switch (m_pEvent->distance) 
   switch (monsterDistance) 
   {
   case UpClose:
      dist = 1;
      break;
   case Nearby:
      if ((dir == East) || (dir == West))
        dist = 9;
      else
        dist = 9;
      break;
   case FarAway:
      if ((dir == East) || (dir == West))
        dist = 16;
      else
        dist = 16;
      break;
   }
#else   
   //31 July 2014 switch (m_pEvent->distance) 
   switch (monsterDistance) 
   {
   case UpClose:
      dist = 2;
      break;
   case Nearby:
      if ((dir == East) || (dir == West))
        dist = TILES_HORZ/2;
      else
        dist = TILES_VERT/2;
      break;
   case FarAway:
      if ((dir == East) || (dir == West))
        dist = TILES_HORZ-2;
      else
        dist = TILES_VERT-2;
      break;
   }
#endif
   TRACE("Placing monster %u tiles away from party\n", dist);
   return dist;
}

#ifdef newMonsterArrangement
//*****************************************************************************
// NAME:    COMBAT_DATA::GetMonsterCoords
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::GetMonsterCoords(int &x, int &y, int partyX, int partyY, int dist, int dir)
{
   // take a guess about start coords
   switch (dir) 
   {
   case North:
      // account for isometric orientation
      x = partyX - (dist+(dist/2));
      if (x < 0)
        x = 0;
      y = partyY-dist;
      if (y < 0)
         y = 0;
      break;
    
   case East:
      x = partyX+dist;
      y = partyY;
      if (x >= MAX_TERRAIN_WIDTH)
         x = MAX_TERRAIN_WIDTH-5;
      break;
    
   case South:
     // account for isometric orientation
      x = partyX + (dist+(dist/2));
      if (x >= MAX_TERRAIN_WIDTH)
         x = MAX_TERRAIN_WIDTH-1;
      y = partyY+dist;
      if (y >= MAX_TERRAIN_HEIGHT)
         y = MAX_TERRAIN_HEIGHT-1;
      break;
    
   case West:
      x = partyX-dist;
      y = partyY;
      if (x < 0)
         x = 0;
      break;   
   }
 // make sure coords are in same room as party
 BOOL look = TRUE;
 
 double dX =  abs(partyX - x);
        dX /= (abs(partyY-y)-1);
 double deltaSum=0;

 while (   (look) 
        && (OBSTICAL_none == ObsticalType(x, y, 1, 1, FALSE, TRUE, NULL))
        && (!HaveLineOfSight(partyX, partyY, x, y,NULL)))

 {
   switch (dir) 
   {
   case North:
      deltaSum += dX;
      while (deltaSum >= 1.0)
      {
        x++;
        deltaSum -= 1.0;
      }
        
      y++;
      if (x == partyX)
      {
        x--;
        look = FALSE;
      }
      if (y == partyY)
      {
        y--;
        look = FALSE;
      }
      break;
  
   case East:
      x--;
      if (x == partyX)
      {
        x++;
        look = FALSE;
      }
      break;
  
   case South:
      deltaSum += dX;
      while (deltaSum >= 1.0)
      {
        x--;
        deltaSum -= 1.0;
      }

      y--;
      if (x == partyX)
      {
        x++;
        look = FALSE;
      }
      if (y == partyY)
      {
        y++;
        look = FALSE;
      }
      break;
  
   case West:
      x++;
      if (x == partyX)
      {
        x--;
        look = FALSE;
      }
      break;   
   }     
 
   if (x < 0) 
   {
     x = 0;
     look = FALSE;
   }
   if (y < 0)
   {
     look = FALSE;
     y = 0;
   }
   if (x >= MAX_TERRAIN_WIDTH)
   {
      x = MAX_TERRAIN_WIDTH-1;
      look = FALSE;
   }
   if (y >= MAX_TERRAIN_HEIGHT)
   {
      y = MAX_TERRAIN_HEIGHT-1;
      look = FALSE;
   }
 }

 // no line of sight in specified direction
 if (look == FALSE)
 {
   x=-1;y=-1;
 }
}
#else
//*****************************************************************************
// NAME:    COMBAT_DATA::GetMonsterCoords
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::GetMonsterCoords(int &x, int &y, int dist, int dir)
{
   // take a guess about start coords
   switch (dir) 
   {
   case North:
      // account for isometric orientation
      x = m_iPartyStartX - (dist+(dist/2));
      if (x < 0)
        x = 0;
      y = m_iPartyStartY-dist;
      if (y < 0)
         y = 0;
      break;
    
   case East:
      x = m_iPartyStartX+dist;
      y = m_iPartyStartY;
      if (x >= MAX_TERRAIN_WIDTH)
         x = MAX_TERRAIN_WIDTH-5;
      break;
    
   case South:
     // account for isometric orientation
      x = m_iPartyStartX + (dist+(dist/2));
      if (x >= MAX_TERRAIN_WIDTH)
         x = MAX_TERRAIN_WIDTH-1;
      y = m_iPartyStartY+dist;
      if (y >= MAX_TERRAIN_HEIGHT)
         y = MAX_TERRAIN_HEIGHT-1;
      break;
    
   case West:
      x = m_iPartyStartX-dist;
      y = m_iPartyStartY;
      if (x < 0)
         x = 0;
      break;   
   }
 // make sure coords are in same room as party
 BOOL look = TRUE;
 
 double dX =  abs(m_iPartyStartX - x);
        dX /= (abs(m_iPartyStartY-y)-1);
 double deltaSum=0;

 while (   (look) 
        && (OBSTICAL_none == ObsticalType(x, y, 1, 1, FALSE, TRUE, NULL))
        && (!HaveLineOfSight(m_iPartyStartX, m_iPartyStartY, x, y,NULL)))

 {
   switch (dir) 
   {
   case North:
      deltaSum += dX;
      while (deltaSum >= 1.0)
      {
        x++;
        deltaSum -= 1.0;
      }
        
      y++;
      if (x == m_iPartyStartX)
      {
        x--;
        look = FALSE;
      }
      if (y == m_iPartyStartY)
      {
        y--;
        look = FALSE;
      }
      break;
  
   case East:
      x--;
      if (x == m_iPartyStartX)
      {
        x++;
        look = FALSE;
      }
      break;
  
   case South:
      deltaSum += dX;
      while (deltaSum >= 1.0)
      {
        x--;
        deltaSum -= 1.0;
      }

      y--;
      if (x == m_iPartyStartX)
      {
        x++;
        look = FALSE;
      }
      if (y == m_iPartyStartY)
      {
        y++;
        look = FALSE;
      }
      break;
  
   case West:
      x++;
      if (x == m_iPartyStartX)
      {
        x--;
        look = FALSE;
      }
      break;   
   }     
 
   if (x < 0) 
   {
     x = 0;
     look = FALSE;
   }
   if (y < 0)
   {
     look = FALSE;
     y = 0;
   }
   if (x >= MAX_TERRAIN_WIDTH)
   {
      x = MAX_TERRAIN_WIDTH-1;
      look = FALSE;
   }
   if (y >= MAX_TERRAIN_HEIGHT)
   {
      y = MAX_TERRAIN_HEIGHT-1;
      look = FALSE;
   }
 }

 // no line of sight in specified direction
 if (look == FALSE)
 {
   x=-1;y=-1;
 }
}
#endif
#ifdef newMonsterArrangement
//*****************************************************************************
// NAME: getNextMonsterCombatPos
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::getNextMonsterCombatPos(COMBATANT &monster, int monsterPartyX, int monsterPartyY, BOOL *reset)
{
  // monsterPartyX is postion of party as seen by the monsters.
  // dir is direction of party from first monster.

   static eventDirType currDir = Any;   
 
   eventDirType lastDir;   

   do 
   {
     lastDir = currDir;
     GetNextDirection(&currDir, &monster.m_iFacing);
     monster.m_iMoveDir = monster.m_iFacing;
     GetMonsterCoords(monster.x, monster.y,
                      monsterPartyX, monsterPartyY, GetMonsterDistance(currDir), currDir);
   } while ((monster.x == -1) && (currDir != lastDir));


   if (monster.x < 0)
   {
     monster.x = m_iPartyStartX;
     monster.y = m_iPartyStartY;
     TRACE("Setting monster start combat pos at party loc\n");
   }
 
   // find first friendly combatant
   //int dude = NO_DUDE;
   //int i=0;
   //while ((dude == NO_DUDE) && (i < m_iNumCombatants))
   //{
   //  if (m_aCombatants[i].friendly)
   //    dude = i;
   // i++;
   //}

   PATH_DIR dir=PathBAD;
   
   // find direction from monster to party to help guide 
   // search for empty cell when specified cell is not empty
   //if (dude != NO_DUDE)
   //dir = GetDir(monster.x,
   //             monster.y,
   //             monsterPartyX, //m_aCombatants[dude].x,
   //             monsterPartyY); //m_aCombatants[dude].y);

   TRACE("Attempting to place %ux%u monster at %u,%u (%u)\n", 
                    monster.width, monster.height, monster.x, monster.y, dir);

   // calling findEmptyCell() will search for empty cell
   // in expanding circle.
   // try to find something in direction away from party first
   switch (currDir)
   {
   case North: dir = PathNorth;
   case East:  dir = PathEast;
   case South: dir = PathSouth;
   case West:  dir = PathWest;
   default:
     ASSERT(true);
     dir = PathNorth;
   };

   if (findEmptyCell(monster.x, monster.y, monster.width, monster.height, dir, NULL))
   {
      TRACE("Found empty cell for monster (%u,%u)\n", monster.x, monster.y);
   }
   else
   {
      // now try to put the monster anywhere we can
      // does not check for reachable path to party, just place in first non-obstical location
      TRACE("First call to findEmptyCell() failed - widening search...\n");
      if (findEmptyCell(monster.x, monster.y, monster.width, monster.height, PathBAD, NULL))
      {
        TRACE("Found empty cell for monster (%u,%u)\n", monster.x, monster.y);
      }
      else
      {
        WriteDebugString("Did NOT find empty cell for monster\n");
        die(0xab4ce);
      }
   }
}
#else
//*****************************************************************************
// NAME: getNextMonsterCombatPos
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::getNextMonsterCombatPos(COMBATANT &monster)
{
   static eventDirType currDir = Any;   
 
   eventDirType lastDir;   

   do 
   {
     lastDir = currDir;
     GetNextDirection(&currDir, &monster.m_iFacing);
     monster.m_iMoveDir = monster.m_iFacing;
     GetMonsterCoords(monster.x, monster.y, GetMonsterDistance(currDir), currDir);
   } while ((monster.x == -1) && (currDir != lastDir));


   if (monster.x < 0)
   {
     monster.x = m_iPartyStartX;
     monster.y = m_iPartyStartY;
     TRACE("Setting monster start combat pos at party loc\n");
   }
 
   // find first friendly combatant
   int dude = NO_DUDE;
   int i=0;
   while ((dude == NO_DUDE) && (i < m_iNumCombatants))
   {
     if (m_aCombatants[i].friendly)
       dude = i;
     i++;
   }

   PATH_DIR dir=PathBAD;
   
   // find direction from monster to party to help guide 
   // search for empty cell when specified cell is not empty
   if (dude != NO_DUDE)
     dir = GetDir(monster.x,
                  monster.y,
                  m_aCombatants[dude].x,
                  m_aCombatants[dude].y);

   TRACE("Attempting to place %ux%u monster at %u,%u (%u)\n", 
                    monster.width, monster.height, monster.x, monster.y, dir);

   // calling findEmptyCell() will search for empty cell
   // in expanding circle.
   // try to find something in direction of party first
   if (findEmptyCell(monster.x, monster.y, monster.width, monster.height, dir, NULL))
   {
      TRACE("Found empty cell for monster (%u,%u)\n", monster.x, monster.y);
   }
   else
   {
      // now try to put the monster anywhere we can
      // does not check for reachable path to party, just place in first non-obstical location
      TRACE("First call to findEmptyCell() failed - widening search...\n");
      if (findEmptyCell(monster.x, monster.y, monster.width, monster.height, PathBAD, NULL))
      {
        TRACE("Found empty cell for monster (%u,%u)\n", monster.x, monster.y);
      }
      else
      {
        WriteDebugString("Did NOT find empty cell for monster\n");
        die(0xab4cf);
      }
   }
}
#endif


//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCharacter
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBAT_DATA::PlaceCharacter(COMBATANT* pDude, int x, int y, BOOL ignoreCurrentCombatant)
{
   if (OBSTICAL_none == ObsticalType(x,y, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
   {
      pDude->x = x;
      pDude->y = y;
      return TRUE;
   }
   return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::CanSeeParty
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanSeeParty(int x, int y)
{
  // determine if at least one party member is in
  // sight on combat map
  BOOL success=FALSE;
  int i=0;
  while ((!success) && (i<m_iNumCombatants))
  {
    if ((m_aCombatants[i].x >= 0) && (m_aCombatants[i].y >= 0))
      success = HaveLineOfSight(m_aCombatants[i].GetCenterX(), m_aCombatants[i].GetCenterY(), x, y,NULL);
    i++;
  }

  if (!success)
    success = HaveLineOfSight(m_iPartyStartX, m_iPartyStartY, x, y,NULL);

  return success;
}

//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCharacterLeft
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBAT_DATA::PlaceCharacterLeft(COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant)
{
   if ((OBSTICAL_none == ObsticalType(*px-1, *py, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
       && (CanSeeParty(*px-1, *py)))
   {
      pDude->x = *px-1;
      pDude->y = *py;
      *px = *px - 1;
      return TRUE;
   }
   return FALSE;
}

//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCharacterRight
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBAT_DATA::PlaceCharacterRight(COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant)
{
   if ((OBSTICAL_none == ObsticalType(*px+1, *py, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
       && (CanSeeParty(*px+1, *py)))
   {
      pDude->x = *px+1;
      pDude->y = *py;
      *px = *px + 1;
      return TRUE;
   }
   return FALSE;
}

//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCharacterNorth
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBAT_DATA::PlaceCharacterNorth(COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant)
{
   if ((OBSTICAL_none == ObsticalType(*px, *py-1, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
       && (CanSeeParty(*px, *py-1)))
   {
      pDude->x = *px;
      pDude->y = *py-1;
      *py = *py - 1;
      return TRUE;
   }
   return FALSE;
}

//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCharacterSouth
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBAT_DATA::PlaceCharacterSouth(COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant)
{
   if ((OBSTICAL_none == ObsticalType(*px, *py+1, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
       && (CanSeeParty(*px, *py+1)))
   {
      pDude->x = *px;
      pDude->y = *py + 1;
      *py = *py + 1;
      return TRUE;
   }
   return FALSE;
}

//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCharacterHorz
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBAT_DATA::PlaceCharacterHorz(COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant)
{
  if (PlaceCharacter(pDude, *px, *py, ignoreCurrentCombatant))
    return TRUE;

  // get east/west width
  int width = getHallWidth(*px, *py, FACE_NORTH);
  if (width < 1)
    return FALSE;

  int newX; 
  int newY = *py;
  BOOL found = FALSE;
  int attempts = 0;
  int dist = 0;

  while ((!found) && (attempts < width))
  {
    newX = *px - dist;
    if (PlaceCharacterLeft(pDude, &newX, &newY, ignoreCurrentCombatant))
    {
      found = TRUE;
      *px = newX;
      *py = newY;
    }
    else
      attempts++;

    newX = *px + dist;
    if (PlaceCharacterRight(pDude, &newX, &newY, ignoreCurrentCombatant))
    {
      found = TRUE;
      *px = newX;
      *py = newY;
    }
    else
      attempts++;

    dist++;
  }

  return found;
}

//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCharacterVert
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBAT_DATA::PlaceCharacterVert(COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant)
{
  if (PlaceCharacter(pDude, *px, *py, ignoreCurrentCombatant))
    return TRUE;

  // get north/south width
  int width = getHallWidth(*px, *py, FACE_EAST);
  if (width < 1)
    return FALSE;

  int newX = *px; 
  int newY;
  BOOL found = FALSE;
  int attempts = 0;
  int dist = 0;

  while ((!found) && (attempts < width))
  {
    newY = *py - dist;
    if (PlaceCharacterNorth(pDude, &newX, &newY, ignoreCurrentCombatant))
    {
      found = TRUE;
      *px = newX;
      *py = newY;
    }
    else
      attempts++;

    newY = *py + dist;
    if (PlaceCharacterSouth(pDude, &newX, &newY, ignoreCurrentCombatant))
    {
      found = TRUE;
      *px = newX;
      *py = newY;
    }
    else
      attempts++;

    dist++;
  }

  return found;
}


#ifdef newPartyArrangement





//*****************************************************************************
// NAME: getNextCharCombatPos
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::getNextCharCombatPos(COMBATANT*   pDude,
                                       BOOL*        pReset,
                                       BOOL         ignoreCurrentCombatant,
                                       const char  *partyArrangement)
{
  static int initialX = 0, initialY = 0;
  int bestX, bestY;
  int x, y, dx, dy;
  int dir, delta;
  int i;
  static char searchOrder[] = {-1,-1,0,-1,1,-1,-1,0,1,0,-1,1,0,1,1,1};
  //static int charsInRow = 0;
 
  if (*pReset)
  {
    //charsInRow = 0;
    initialX = m_iPartyOriginX;
    initialY = m_iPartyOriginY;
    *pReset = FALSE;
  }
  bestX = initialX;
  bestY = initialY;
  bestX += Decode(partyArrangement[0]);
  bestY += Decode(partyArrangement[1]);
  //WriteDebugString("DEBUG - initialX=%d; initialY=%d; bestX=%d; bestY=%d\n",initialX, initialY, bestX, bestY);
  dir = 3;
  i = -1;
  for (delta=0; delta<8; delta++)  // search 225 cells.....enough is enough!
  {
    dx = 1;
    dy = 0;
    x = bestX-delta;
    y = bestY-delta;
    for (; dir<4; dir++)
    {
      for (; i<2*delta; i++)
      {
        if (PlaceCharacter(pDude, x, y, ignoreCurrentCombatant))
        {
          if (OBSTICAL_none != ObsticalType(pDude->x, pDude->y, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
            WriteDebugString("getNextCharCombatPos() returns a cell containing a wall\n");
          else
            TRACE("Placing char at combat pos %u,%u\n", pDude->x, pDude->y);
          return ;
        };
        x += dx;
        y += dy;
      };
      i = dx;
      dx = -dy;
      dy = i;
      i = 0;

    };
    dir = 0;
  };


/*
   switch (facing) 
   {      
   case FACE_NORTH:
   case FACE_SOUTH:
      while ((!found) && (lastY >= 0) && (lastY < MAX_TERRAIN_HEIGHT))
      {
         if (charsInRow < MAX_CHARS_IN_ROW)
         {
            found = PlaceCharacterHorz(pDude, &lastX, &lastY, ignoreCurrentCombatant);            
            if (!found)
               charsInRow = MAX_CHARS_IN_ROW; // advance to next row
         }
         else 
         {           
            charsInRow = 0;
            lastX = m_iPartyStartX;
            if (facing == FACE_NORTH)
               lastY++;
            else
               lastY--;
         }
      }
      break;
    
   case FACE_EAST:
   case FACE_WEST:
      while ((!found) && (lastX >= 0) && (lastX < MAX_TERRAIN_WIDTH))
      {
         if (charsInRow < MAX_CHARS_IN_ROW)
         {
            found = PlaceCharacterVert(pDude, &lastX, &lastY, ignoreCurrentCombatant);            
            if (!found)
               charsInRow = MAX_CHARS_IN_ROW; // advance to next row
         }
         else 
         {
            charsInRow = 0;
            lastY = m_iPartyStartY;
            if (facing == FACE_EAST)
               lastX--;
            else
               lastX++;
         }
      }
      break;
   }
 */
   if (OBSTICAL_none != ObsticalType(pDude->x, pDude->y, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
     WriteDebugString("getNextCharCombatPos() returns a cell containing a wall\n");
   else
     TRACE("Placing char at combat pos %u,%u\n", pDude->x, pDude->y);
}





#else  !newPartyArrangement

//*****************************************************************************
// NAME: getNextCharCombatPos
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::getNextCharCombatPos(COMBATANT* pDude, BOOL* pReset, BOOL ignoreCurrentCombatant)
{
   static int lastX = 0, lastY = 0;
   static int charsInRow = 0;
   BOOL found = FALSE;
 
   if (*pReset)
   {
      charsInRow = 0;
      lastX = m_iPartyStartX;
      lastY = m_iPartyStartY;
      *pReset = FALSE;
   }
   
   int facing = party.GetPartyFacing();

   switch (facing) 
   {      
   case FACE_NORTH:
   case FACE_SOUTH:
      while ((!found) && (lastY >= 0) && (lastY < MAX_TERRAIN_HEIGHT))
      {
         if (charsInRow < MAX_CHARS_IN_ROW)
         {
            found = PlaceCharacterHorz(pDude, &lastX, &lastY, ignoreCurrentCombatant);            
            if (!found)
               charsInRow = MAX_CHARS_IN_ROW; // advance to next row
         }
         else 
         {           
            charsInRow = 0;
            lastX = m_iPartyStartX;
            if (facing == FACE_NORTH)
               lastY++;
            else
               lastY--;
         }
      }
      break;
    
   case FACE_EAST:
   case FACE_WEST:
      while ((!found) && (lastX >= 0) && (lastX < MAX_TERRAIN_WIDTH))
      {
         if (charsInRow < MAX_CHARS_IN_ROW)
         {
            found = PlaceCharacterVert(pDude, &lastX, &lastY, ignoreCurrentCombatant);            
            if (!found)
               charsInRow = MAX_CHARS_IN_ROW; // advance to next row
         }
         else 
         {
            charsInRow = 0;
            lastY = m_iPartyStartY;
            if (facing == FACE_EAST)
               lastX--;
            else
               lastX++;
         }
      }
      break;
   }
 
   if (OBSTICAL_none != ObsticalType(pDude->x, pDude->y, pDude->width, pDude->height, TRUE, ignoreCurrentCombatant, NULL))
     WriteDebugString("getNextCharCombatPos() returns a cell containing a wall\n");
   else
     TRACE("Placing char at combat pos %u,%u\n", pDude->x, pDude->y);
}

#endif

//*****************************************************************************
// NAME:    COMBAT_DATA::RestoreToParty
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::RestoreToParty()
{
  for (int i=0; i<m_iNumCombatants; i++)
    m_aCombatants[i].RestoreToParty();
}

//*****************************************************************************
//    NAME: COMBAT_DATA::DetermineIfCombatOver
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::DetermineIfCombatOver()
{
  int dude = GetCurrCombatant();
  int numFriendly, numUnfriendly, numPartyMembers;
 
  {
    int i;
    numFriendly = 0;
    numUnfriendly = 0;
    numPartyMembers = 0;
  //getNumCombatants(&numParty, &numMons);      
    for (i=0; i < m_iNumCombatants; i++)
    {
      COMBATANT *pCombatant;
      pCombatant = &m_aCombatants[i];
      if (pCombatant->charOnCombatMap(false, false))
      {
        if (pCombatant->IsPartyMember())
        {
          numPartyMembers++;
        }
        if (pCombatant->GetIsFriendly()) numFriendly++;
        else numUnfriendly++;    
      }
    }
  };
   
  if (numUnfriendly == 0)
  {
    // party wins
    m_eCombatVictor = PartyWins;
    
// PRS 24 Oct 2009    if (dude != NO_DUDE)
      m_bLastRound = TRUE; 
// PRS 24 Oct 2009   else
      m_bCombatOver = TRUE;
  }
  else if ((numFriendly == 0) || (numPartyMembers == 0))
  {
    // monsters win, or party ran away
    m_eCombatVictor = MonsterWins;
    
    if (dude != NO_DUDE)
      m_bLastRound = TRUE; 
    else
      m_bCombatOver = TRUE;
  }
  else
    m_eCombatVictor = NoWinner;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::DetermineVictoryExpPoints
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::DetermineVictoryExpPoints()
{
  int lcv;
  if (m_eCombatVictor == PartyWins)
  {
    //WriteDebugString("Party wins combat\n");
    m_iExpPointTotal = 0;        
    if (!GetConfigPartyNoExperience())
    {
      for (lcv=0; lcv<m_iNumCombatants; lcv++)
      {
        if ((!m_aCombatants[lcv].GetIsFriendly())&&(!m_aCombatants[lcv].IsPartyMember()))
        {
          charStatusType status = m_aCombatants[lcv].GetAdjStatus();
          if (status == Dead)
            m_iExpPointTotal += m_aCombatants[lcv].getCharExpWorth();
        }
      }
      
      double mod = GetMonsterExpMod();
      if (mod != 0.0)
        m_iExpPointTotal += ((mod/100.0)*m_iExpPointTotal);
      if (m_iExpPointTotal < 0) m_iExpPointTotal=0;
    }
  }
  else if (m_eCombatVictor == MonsterWins)
  {
    // See if party fled, or all died
    lcv = 0;        
    while (lcv < m_iNumCombatants)
    {
      if (m_aCombatants[lcv].IsPartyMember())
      {
        if (m_aCombatants[lcv].GetAdjStatus()==Fled)
        {
          m_eCombatVictor = PartyRanAway;
          break;
        }
      }
      lcv++;
    }
  }
}

//*****************************************************************************
// NAME:    COMBAT_DATA::UpdateCombat
//
// PURPOSE: 
//
// /* PRS July 2009  RETURNS: TRUE if input should be forced by ProcInput.cpp */
//*****************************************************************************
void /* PRS July 2009 BOOL */ COMBAT_DATA::UpdateCombat()
{
  BOOL isUpdate = FALSE;
// PRS20110223  static int prevDude = NO_DUDE;

// PRS20110223  m_iPrevRndCombatant = prevDude;

  if (QComb.StartOfTurn() || QComb.RestartInterruptedTurn())
  {
    QComb.NotStartOfTurn();
    return;
  };
  
  
  
  m_bStartingNewCombatRound=FALSE;

  int dude = GetCurrCombatant();  


  
  //if (prevRndCombatant != dude)
  //  PlaceCursorOnCurrentDude();



  if (    (dude == NO_DUDE) 
       ||  m_aCombatants[dude].IsDone(
                                       (QComb.NumFreeAttacks()+QComb.NumGuardAttacks()) > 0,
                                       "Updating combat state" 
                                      )
     )
  {
    getNextCombatant();
    dude = GetCurrCombatant();
  
    if (dude == NO_DUDE)
    {  
      StartNewRound();  // Performs a GetNextCombatant();
      dude = GetCurrCombatant();
      if (dude != NO_DUDE)
      {
        m_aCombatants[dude].OnStartCombatantAction();
      };
      DetermineIfCombatOver();

      // **** PRS 20100317
      if (IsCombatOver())
      {     
        DetermineVictoryExpPoints();
        isUpdate = TRUE;
      };
      // **** PRS 20100317


      return;
    }

    if (dude != NO_DUDE)
    {
      isUpdate = TRUE;
      m_aCombatants[dude].OnStartCombatantAction();
      //PlaceCursorOnCurrentDude(TRUE);
    }
  } // if dude is done with his turn
  else
  {
  };


  DetermineIfCombatOver();  // PRS 24 Oct 2009

  if (CheckIdleTime())
  {
    // force end to combat if nobody does anything for awhile
    m_bCombatOver = TRUE;
  }

  if (IsCombatOver())
  {     
    DetermineVictoryExpPoints();
    isUpdate = TRUE;
  }
  else 
  {
    if (IsStartOfTurn()) 
    {
    }
    else
    {
      // there is a delay between combatant updates, if prev combatant is not
      // current combatant     
      if (dude != NO_DUDE)
      {

      // PRS July 2009 isUpdate |= m_aCombatants[dude].UpdateCombatant();
        m_aCombatants[dude].UpdateCombatant();
      };
    };
  };

  isUpdate |= m_bNeedUpdate;
  m_bNeedUpdate = FALSE;

  //ASSERT(GetCurrCombatant() != NO_DUDE);
  //PRS 20110223 prevDude = GetCurrCombatant();
  return; // PRS July 2009 isUpdate;
}

//*****************************************************************************
// NAME: COMBAT_DATA::CheckIdleTime
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBAT_DATA::CheckIdleTime()
{
  int minIdleTime = 1000000;
  int diff;
  for (int i=0; i<m_iNumCombatants; i++)
  {
    diff = m_iCurrRound - m_aCombatants[i].lastAttackRound;
    if (diff < minIdleTime)
      minIdleTime = diff;
  }

  return (minIdleTime > MAX_COMBAT_IDLE_ROUNDS);
}


//*****************************************************************************
// NAME: COMBAT_DATA::CheckMorale
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::CheckMorale()
{
  for (int i=0; i<m_iNumCombatants; i++)
    m_aCombatants[i].CheckMorale();
}

//*****************************************************************************
// NAME:    COMBAT_DATA::StartNewRound
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::StartNewRound()
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  SetUpdateFlag();

  if (m_bLastRound)
    m_bCombatOver = TRUE;

  //party.incrementClock(1); // handled in ProcessTimeSensitiveData()
  TRACE("Start new combat round\n");

  // battle rages on
  CheckDyingCombatants();
  CheckMorale();
  int i;
  for (i=0; i<m_iNumCombatants; i++)
  {
    bool continueGuarding;

    {
      scriptContext.SetCharacterContext(m_aCombatants[i].m_pCharacter);
      scriptContext.SetCombatantContext(&m_aCombatants[i]);
      hookParameters[5].Format("%d",m_aCombatants[i].State());

      m_aCombatants[i].RunCombatantScripts(START_COMBAT_ROUND,
                                           ScriptCallback_RunAllScripts, 
                                           NULL,
                                           "Starting a new combat round");
      m_aCombatants[i].RunCombatantScripts(GUARDING_START_OF_ROUND,
                                           ScriptCallback_RunAllScripts, 
                                           NULL,
                                           "Starting a new combat round");
      continueGuarding = hookParameters[6].Find('G') >= 0;
      ClearHookParameters();
      scriptContext.Clear();
    };


    m_aCombatants[i].m_isCombatReady = -1; // Force a search of SPECIAL_ABILITIES in IsDone()
    if ((m_aCombatants[i].charCanTakeAction()) && (m_aCombatants[i].IsDone(false,"Starting a new combat round")))
    {      
      if (m_aCombatants[i].State() != ICS_Casting)
      {
        m_aCombatants[i].turnIsDone = FALSE;
        // stay guarding until told otherwise
        if (m_aCombatants[i].OnAuto(false))
        {
          if (m_aCombatants[i].State() != ICS_Guarding)
            m_aCombatants[i].State(ICS_None);
        }
        else
        {
          if ( (m_aCombatants[i].State() == ICS_Guarding) && continueGuarding)
          {
            m_aCombatants[i].State(ICS_ContinueGuarding);
          }
          else
          {
            m_aCombatants[i].State(ICS_None);
          };
        };
      }
      {
        /* The calculation of availAttacks has been moved from
         * StartNewRound() to getNextCombatant().
         * I did this because Manikus complained that a spell
         * given during a combat round that increased the number
         * of attacks on a target did not take effect until the
         * start of the following round.  PRS 20091215
         *
         * The code has been moved back to StartNewRound.
         * I did this because some things that take place after
         * the start-of-round but before the combatant's turn
         * require that we know the number of available attacks.
         * In particular, 'Guarding Attacks' and 'Free Attacks'
         * are based on the number of available attacks.  We will
         * have to solve the original problem differently.
         * PRS 20111214
         */
        double leftoverAttacks=m_aCombatants[i].availAttacks;             
        m_aCombatants[i].determineNbrAttacks();
        m_aCombatants[i].determineAvailAttacks(m_aCombatants[i].GetNbrAttacks());
        double maxAttacks = ceil(m_aCombatants[i].availAttacks);
        m_aCombatants[i].availAttacks += leftoverAttacks;
        m_aCombatants[i].availAttacks = min(m_aCombatants[i].availAttacks, maxAttacks);
        m_aCombatants[i].m_iMovement = 0;      
        m_aCombatants[i].m_iNumDiagonalMoves = 0;
      };
    }

    if (m_aCombatants[i].charOnCombatMap(false, true))
    {
      if (activeSpellList.LingerSpellAffectsTarget(m_aCombatants[i].self, 
                                                   m_aCombatants[i].x,
                                                   m_aCombatants[i].y,
                                                   m_aCombatants[i].width,
                                                   m_aCombatants[i].height))
      {
        // A lingering spell is active at the location 
        // this dude is located at.
        TRACE("%i re-targeted by at least one active linger spell at location %i,%i\n",
              m_aCombatants[i].self, m_aCombatants[i].x, m_aCombatants[i].y);

        activeSpellList.ActivateLingerSpellsOnTarget(m_aCombatants[i].self,
                                                     m_aCombatants[i].x, 
                                                     m_aCombatants[i].y,
                                                     m_aCombatants[i].width,
                                                     m_aCombatants[i].height);
      };    
      m_aCombatants[i].m_pCharacter->ProcessLingeringSpellEffects();
    };
    {
      POSITION pos;
      for (pos=m_auras.GetHeadPosition(); pos!=NULL;)
      { 
        int n, j;
        AURA *pAURA;
        pAURA = m_auras.GetAt(pos);
        m_auras.GetNext(pos);
        n = pAURA->m_combatants.GetSize();
        for (j=0; j<n; j++)
        {
          if (pAURA->m_combatants[j] == i)
          {
            PushAuraReference(pAURA->auraID);
            pAURA->RunAuraScripts(
                                  AURA_EFFECT,
                                  ScriptCallback_RunAllScripts, 
                                  NULL,
                                  "Starting a new combat round");
            PopAuraReference();
            break;
          };
        };
      };
    };
  };

  {
    POSITION pos;
    HOOK_PARAMETERS hookParams;
    scriptContext.Clear();
    for (pos=m_auras.GetHeadPosition(); pos!=NULL;)
    {
      AURA *pAURA;
      pAURA = m_auras.GetAt(pos);
      PushAuraReference(pAURA->auraID);
      m_auras.GetNext(pos);
      pAURA->RunAuraScripts(
                     AURA_ROUND, 
                     ScriptCallback_RunAllScripts,
                     NULL,
                     "Starting a new combat round");
      PopAuraReference();
    };
  };
  m_iCurrInitiative = 1;
  m_iCurrRound++;
  if (m_iCurrRound%10 == 0) 
    m_iCurrTurn++;

  party.incrementClock(1);
  
  DetermineCombatInitiative();

  for (i=0; i<m_iNumCombatants; i++)
  {
  };

  getNextCombatant();

  //PRS 20110223 m_iPrevRndCombatant = GetCurrCombatant();
  m_bStartingNewCombatRound=TRUE;  
}

//*****************************************************************************
// NAME:    COMBAT_DATA::CheckDyingCombatants
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::CheckDyingCombatants()
{
  for (int i=0; i<m_iNumCombatants; i++)
  {
    if (m_aCombatants[i].GetAdjStatus() == Dying)        
    {      
      if (!m_aCombatants[i].isBandaged)
        m_aCombatants[i].TakeDamage(1, FALSE, NULL, false, NULL);
    }
  }
}


//*****************************************************************************
//    NAME: COMBAT_DATA::CheckForDeadGuys
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::CheckForDeadGuys()
{
  for (int i=0; i<m_iNumCombatants; i++)
  {
    if (m_aCombatants[i].GetAdjHitPoints() <= 0)
      m_aCombatants[i].blitDeadSprite();
  }
}

//*****************************************************************************
// NAME:    displayCursor
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BYTE aval=100;
void COMBAT_DATA::displayCursor(BOOL coverFullIcon)
{
  DETAILTRACE(0x2ab090,0);
  int i,j;
  int x = TerrainToScreenCoordX(m_iCursorX);
  int y = TerrainToScreenCoordY(m_iCursorY);

  if (x < 0) return;
  if (y < 0) return;

  // make sure entire cursor will fit in display area
  //if (   ((x+(CursorSrc.right-CursorSrc.left)) <= (COMBAT_SCREEN_WIDTH))
  //    && ((y+(CursorSrc.bottom-CursorSrc.top)) <= (SCREEN_HEIGHT-COMBAT_MENU_HEIGHT)))
  if (((x + (CursorSrc.right - CursorSrc.left)) <= (CombatScreenX+TILES_HORZ*COMBAT_TILE_WIDTH))
    && ((y + (CursorSrc.bottom - CursorSrc.top)) <= (CombatScreenY+TILES_VERT*COMBAT_TILE_HEIGHT)))
  {
    DETAILTRACE(0x2ab091,0);
    int dude = getCombatantInCell(m_iCursorX, m_iCursorY, 1,1);
  
    if (coverFullIcon)
    {
      DETAILTRACE(0x2ab092,0);
      if ((dude != NO_DUDE) && (dude < NumCombatants()/*m_aCombatants.GetSize()*/))
      {
        for (i=0;i<m_aCombatants[dude].height;i++)
        {
          for (j=0;j<m_aCombatants[dude].width;j++)
          {
            DETAILTRACE(0x2ab093,0);
            GraphicsMgr.SetClipRect(&m_combatRect);              
            DETAILTRACE(0x2ab095,0);
            GraphicsMgr.BlitAlphaImage(aval, // 50%
                                      TerrainToScreenCoordX(m_aCombatants[dude].x+j),
                                      TerrainToScreenCoordY(m_aCombatants[dude].y+i),
                                      CombatCursorArt.surface,
                                      &CursorSrc);
            DETAILTRACE(0x2ab096,0);
            GraphicsMgr.SetClipRect(&m_fullRect);
            DETAILTRACE(0x2ab097,0);
          }
        }      
        m_aCombatants[dude].displayCombatSprite();
      }
    }
    else 
    {
      DETAILTRACE(0x2ab094,0);
      GraphicsMgr.SetClipRect(&m_combatRect);  
      // destX, destY, SrcSurfaceNum, src RECT
      GraphicsMgr.BlitAlphaImage(aval, x,y, CombatCursorArt.surface, &CursorSrc);
      GraphicsMgr.SetClipRect(&m_fullRect);
      
      if ((dude != NO_DUDE) && (dude < NumCombatants()/*m_aCombatants.GetSize()*/))
        m_aCombatants[dude].displayCombatSprite();
    }
  }  
}

//*****************************************************************************
// NAME:    COMBAT_DATA::PlaceCursorOnCurrentDude
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::PlaceCursorOnCurrentDude(BOOL ForceCenter)
{
  int dude = GetCurrCombatant();
  if ((dude < 0) || (dude >= m_iNumCombatants)) return;
  if (    (m_iCursorX != m_aCombatants[dude].x)
       || (m_iCursorY != m_aCombatants[dude].y)
     )
  {
    m_iCursorX = m_aCombatants[dude].x;
    m_iCursorY = m_aCombatants[dude].y;
    EnsureVisibleTarget(dude, ForceCenter);
  };
}

//*****************************************************************************
//    NAME: COMBAT_DATA::PlaceCursorOnDude
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::PlaceCursorOnDude(int dude, BOOL ForceCenter)
{
  if ((dude < 0) || (dude >= m_iNumCombatants)) return;
  m_iCursorX = m_aCombatants[dude].x;
  m_iCursorY = m_aCombatants[dude].y;
  EnsureVisibleTarget(dude, ForceCenter);
}

//*****************************************************************************
// NAME: COMBAT_DATA::DisplayCombatants
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::DisplayCombatants()
{
  int i;
  // display dying dudes first  
  for (i=0; i<NumCombatants()/*m_aCombatants.GetSize()*/; i++)
  {
    if (m_aCombatants[i].charUnconscious())
      m_aCombatants[i].displayCombatSprite();
  }  

  for (i=0; i<NumCombatants()/*m_aCombatants.GetSize()*/; i++)
  {
    if (!m_aCombatants[i].charUnconscious())
      m_aCombatants[i].displayCombatSprite();
  }
}

int TerrainToWorldCoordX(int);
int TerrainToWorldCoordY(int);

void COMBAT_DATA::DisplayAURAs(void)
{
  /**************************************
   *   These are displayed by AnimateList
   **************************************
  COMBAT_SPRITE_DATA combatSprite;
  POSITION pos;
  pos = m_auras.GetHeadPosition();
  while (pos != NULL)
  {
    AURA *pAURA;
    int i, j, indx;
    pAURA = &m_auras.GetNext(pos);
    if (pAURA->spriteData.picData.picType == BogusDib) continue;
    indx = 0;
    for (j=0; j<MAX_TERRAIN_HEIGHT; j++)
    {
      for (i=0; i<MAX_TERRAIN_WIDTH; i++, indx++)
      {
        if (pAURA->cells[indx] != 0)
        {
          pAURA->spriteData.stx = i;
          pAURA->spriteData.sty = j;
          pAURA->spriteData.swx = TerrainToWorldCoordX(pAURA->spriteData.stx);
          pAURA->spriteData.swy = TerrainToWorldCoordY(pAURA->spriteData.sty);
          pAURA->spriteData.Display();
        };
      };
    };
  };
  */
}

//*****************************************************************************
//    NAME: COMBAT_DATA::HandleCurrState
//
// PURPOSE: 
//
// Returns non-zero if screen needs to be redrawn
//
//*****************************************************************************
int COMBAT_DATA::HandleCurrState(BOOL zeroMoveAttackOK)
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].HandleCurrState(zeroMoveAttackOK);
  return 0;
}

//*****************************************************************************
// NAME:    COMBAT_DATA::HandleTimeDelayMsgExpired
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBAT_DATA::HandleTimeDelayMsgExpired(int iDeathIndex)
{
  //PlaceCursorOnCurrentDude();
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].HandleTimeDelayMsgExpired(iDeathIndex);
}

//*****************************************************************************
// NAME:    COMBAT_DATA::HandleTimeDelayMsgBegin
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int COMBAT_DATA::HandleTimeDelayMsgBegin(int *pDeathIndex)
{  
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].HandleTimeDelayMsgBegin(IsFreeAttacker()||IsGuardAttacker(), pDeathIndex);
  return -1;
}

//*****************************************************************************
// NAME: COMBAT_DATA::getCharWeaponText
//
// PURPOSE:  
//*****************************************************************************
void COMBAT_DATA::getCharWeaponText(int index, CString &wpn, CString &dmg)
{
  m_aCombatants[index].m_pCharacter->getCharWeaponText(wpn, dmg);
}

//*****************************************************************************
//    NAME: COMBAT_DATA::NeedMissileAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::NeedMissileAnimation()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].NeedMissileAnimation();
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitMissileAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitMissileAnimation()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitMissileAnimation();  
    //PlaceCursorOnCurrentDude();
  }
}
//*****************************************************************************
//    NAME: COMBAT_DATA::NeedHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::NeedHitAnimation()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].NeedHitAnimation();
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitHitAnimation()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitHitAnimation();  
    //PlaceCursorOnDude(m_aCombatants[curr].target, FALSE);
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::NeedSpellCastAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::NeedSpellCastAnimation(int targ)
{
  ASSERT( (targ>=0) || (targ==-1) );
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);
  if (curr != NO_DUDE)
  {
    if (targ < 0) return FALSE;
    if (!m_aCombatants[targ].charOnCombatMap(false, true)) return FALSE;
    return (m_aCombatants[curr].NeedSpellCastAnimation());
  }
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitSpellCastAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitSpellCastAnimation(int targ)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);
  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitSpellCastAnimation(targ);  
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::NeedSpellInRouteAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::NeedSpellInRouteAnimation(int x, int y, BOOL allowSelf)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);
  if (curr != NO_DUDE)
    return (m_aCombatants[curr].NeedSpellInRouteAnimation(x,y, allowSelf));
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitSpellInRouteAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitSpellInRouteAnimation(int x, int y)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitSpellInRouteAnimation(x, y);  
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitSpellInRouteAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitSpellInRouteAnimation(int srcx, int srcy, int dstx, int dsty)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitSpellInRouteAnimation(srcx, srcy, dstx, dsty);  
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::NeedSpellCoverageAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::NeedSpellCoverageAnimation(int x, int y)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
    return m_aCombatants[curr].NeedSpellCoverageAnimation();
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitSpellCoverageAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitSpellCoverageAnimation(int x, int y)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitSpellCoverageAnimation(x, y);  
    //PlaceCursorOnCurrentDude();
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::NeedSpellHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::NeedSpellHitAnimation(int targ)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
    return m_aCombatants[curr].NeedSpellHitAnimation();
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitSpellHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitSpellHitAnimation(int targ)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitSpellHitAnimation(targ);  
    //PlaceCursorOnCurrentDude();
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::NeedSpellLingerAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::NeedSpellLingerAnimation()
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
    return m_aCombatants[curr].NeedSpellLingerAnimation();
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitSpellLingerAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitSpellLingerAnimation(/*int mapx, int mapy*/)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitSpellLingerAnimation(/*mapx,mapy*/);  
    //PlaceCursorOnCurrentDude();
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::InitSpellLingerAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InitSpellLingerAnimation(int targ)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr != NO_DUDE);

  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].InitSpellLingerAnimation(targ);  
    //PlaceCursorOnCurrentDude();
  }
}

//*****************************************************************************
//    NAME: FormatCombatDelayText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatDelayText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  

  CString temp;
  temp.Format("%s is delaying...",
              combatData.GetCombatant(dude)->GetName());
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatEndTurnText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatEndTurnText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  

  CString temp;
  temp.Format("%s ends turn...",
              combatData.GetCombatant(dude)->GetName());
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatForceWinText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatForceWinText()
{
  CString temp;
  temp = "The party has forced a victory";
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatMoveText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatMoveText()
{ 
  COMBATANT *pCombatant;
  ASSERT(combatData.GetCurrCombatant() != NO_DUDE);


  pCombatant = combatData.GetCombatant(combatData.GetCurrCombatant());
  int moves = pCombatant->GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, 
                                            "Format combat move text") 
              - pCombatant->m_iMovement;

  if ( moves < 0 ) moves = 0;

  CString temp;
  temp.Format("MOVES LEFT: %u", moves);
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatAttackText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatAttackText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  
  int target = combatData.GetCombatant(dude)->GetCurrTarget(true, true, true);
  CString msg("attacks");
  CString temp;

  if (combatData.GetCombatant(dude)->GetType()==MONSTER_TYPE)
  {    
    //int inv_item = combatData.GetCombatant(dude)->myItems.GetReady(WeaponHand);
    int inv_item = combatData.GetCombatant(dude)->m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
    if (inv_item != NO_READY_ITEM)
    {
      // using a weapon attack
      //GLOBAL_ITEM_ID giID = combatData.GetCombatant(dude)->m_pCharacter->myItems.GetItem(inv_item);
      ITEM_ID itemID = combatData.GetCombatant(dude)->m_pCharacter->myItems.GetItem(inv_item);
      //ITEM_DATA *idata = itemData.GetItemData(giID);
      ITEM_DATA *idata = itemData.GetItem(itemID);
      if (idata!=NULL) msg = idata->attackMsg;
    }
    else
    {
      // only use monster attack msgs when using natural, non-weapon, attacks
      int nbrattacks = combatData.GetCombatant(dude)->GetNbrAttacks();
      int availattacks = combatData.GetCombatant(dude)->availAttacks;
      int currAttack = nbrattacks - availattacks;

      //ASSERT( (currAttack>=0) && (currAttack<nbrattacks) );
      if ((currAttack < 0)||(currAttack>=nbrattacks)) currAttack=0;
      //monsterData.GetMonsterAttackMsg(combatData.GetCombatant(dude)->origKey, currAttack, msg);
      monsterData.GetMonsterAttackMsg(combatData.GetCombatant(dude)->m_pCharacter->monsterID, currAttack, msg);
    }
  }
  else
  {
    //int inv_item = combatData.GetCombatant(dude)->myItems.GetReady(WeaponHand);
    int inv_item = combatData.GetCombatant(dude)->m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
    if (inv_item != NO_READY_ITEM)
    {      
      //GLOBAL_ITEM_ID giID = combatData.GetCombatant(dude)->m_pCharacter->myItems.GetItem(inv_item);
      ITEM_ID itemID;
      itemID = combatData.GetCombatant(dude)->m_pCharacter->myItems.GetItem(inv_item);
      //ITEM_DATA *idata = itemData.GetItemData(giID);
      ITEM_DATA *idata = itemData.GetItem(itemID);
      if (idata!=NULL) msg = idata->attackMsg;      
    }
  }

  if (msg.GetLength()==0) msg = "attacks";
  temp.Format("%s %s %s", 
              combatData.GetCombatant(dude)->GetName(),
              getGameText(msg),
              combatData.GetCombatant(target)->GetName());

  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatGuardText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatGuardText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  

  CString temp;
  temp.Format("%s is guarding...",
              combatData.GetCombatant(dude)->GetName());
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatBandageText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatBandageText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  

  CString temp;
  int bandagetarget = combatData.GetCombatant(dude)->bandageWho;
  if (bandagetarget >= 0)
  {    
    temp.Format("%s is bandaging %s...",
                combatData.GetCombatant(dude)->GetName(),
                combatData.GetCombatant(bandagetarget)->GetName());
    combatData.GetCombatant(dude)->bandageWho = -1;
  }
  else
    temp.Format("%s is bandaging...",
                combatData.GetCombatant(dude)->GetName());
  
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatTurnUndeadText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatTurnUndeadText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  

  CString temp;
  temp.Format("%s turns undead...",
              combatData.GetCombatant(dude)->GetName());
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatFleeText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatFleeText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  

  CString temp;
  temp.Format("%s runs away...",
              combatData.GetCombatant(dude)->GetName());
  return temp;
}

//*****************************************************************************
//    NAME: FormatCombatCastText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatCombatCastText()
{ 
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  
  //GLOBAL_SPELL_ID charspellbookkey = combatData.m_aCombatants[dude].GetSpellKeyBeingCast();
  CHARACTER_SPELL charSp;
  if (!combatData.GetCombatant(dude)->FetchSpellBeingCast(&charSp))
    return CString("");

  //CString name(spellData.GetSpellName(cspell.spell));
  CString name(spellData.GetSpellName(charSp.spellID));
  if (name.IsEmpty()) name = "Unknown!";
  CString temp;
  temp.Format("%s begins casting %s...",
              combatData.GetCombatant(dude)->GetName(),
              name);
  return temp;
}

//*****************************************************************************
//    NAME: FormatActivateSpellText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatActivateSpellText()
{
  int dude = combatData.GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  
  //GLOBAL_SPELL_ID charspellbookkey = combatData.m_aCombatants[dude].GetSpellKeyBeingCast();
  CHARACTER_SPELL charSp;
  //GLOBAL_SPELL_ID secondarySpell;
  SPELL_ID secondarySpellID;
  if (!combatData.GetCombatant(dude)->FetchSpellBeingCast(&charSp))
    return CString("");

  CString name;
  //secondarySpell = combatData.GetCombatant(dude)->GetSecondarySpellKeyBeingCast();
  secondarySpellID = combatData.GetCombatant(dude)->GetSecondarySpellIDBeingCast();
  //if (secondarySpell.IsValidSpell())
  if (secondarySpellID.IsValidSpell())
  {
    name = (spellData.GetSpellName(secondarySpellID));
  }
  else
  {
    //name = (spellData.GetSpellName(cspell.spell));
    name = (spellData.GetSpellName(charSp.spellID));
  };
  if (name.IsEmpty()) name = "Unknown!";
  CString temp;
  temp.Format("%s casts %s...",
              combatData.GetCombatant(dude)->GetName(),
              name);
  return temp;
}

//*****************************************************************************
//    NAME: FormatSpellHitText
//
// PURPOSE: 
//
//*****************************************************************************
CString FormatSpellHitText(COMBATANT *pTarget, const CString *messageFormat)
{
  CString msg("");

  COMBATANT *pCaster = combatData.getActiveCastingCharacter();
  ASSERT(pCaster!=NULL);  
  if (pCaster==NULL) return msg;
  if (pTarget == NULL)
  {
    if (pCaster->m_pCharacter->targets.m_SelectingUnits)
    {
      int target = pCaster->m_pCharacter->targets.m_targets[0];
      pTarget = combatData.getCombatantPtr(target);
      ASSERT(pTarget!=NULL);  
      if (pTarget==NULL) return msg;
    }//else target is a map coordinate
  };

  //SPELLBOOK_SPELL_ID charspellbookkey = pCaster->GetSpellBookKeyBeingCast();
  CHARACTER_SPELL charSp;
  if (!pCaster->FetchSpellBeingCast(&charSp))
    return msg;
  //SPELL_DATA *pSpell = spellData.GetSpellData(cspell.spell);
  SPELL_DATA *pSpell = spellData.GetSpell(charSp.spellID);
  ASSERT(pSpell!=NULL);  
  if (pSpell==NULL) return msg;
  {
    if (messageFormat == NULL)
    {
      msg = pSpell->CastMsg;
    }
    else
    {
      msg = *messageFormat;
    };
    msg.Replace("/s", pSpell->Name);
    msg.Replace("/c", pCaster->GetName());
    if (pTarget!=NULL)
    {
      CString damageHP;
      damageHP.Format("%d", pTarget->m_spellDamageHP);
      msg.Replace("/t", pTarget->GetName());
      msg.Replace("/d", damageHP);
    }
    else
    {
      msg.Replace("/t", "");
      msg.Replace("/d", "");
    };
  };

  return msg;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::FormatSpecAbMsg
//
// PURPOSE: 
//
//*****************************************************************************
CString COMBAT_DATA::FormatSpecAbMsg(DWORD sa_state, int dude)
{
//  int dude = GetCurrCombatant();
  ASSERT(dude != NO_DUDE);  
  return (m_aCombatants[dude].FormatSpecAbMsg(sa_state));
}

//*****************************************************************************
//    NAME: COMBAT_DATA::FormatTimeDelayMsg
//
// PURPOSE: 
//
//*****************************************************************************
CString COMBAT_DATA::FormatTimeDelayMsg(overallCombatState state)
{
  if (state == OCS_None)
    state = (overallCombatState)GetCombatantState();

  CString msg="";
  switch (state)
  {
  case OCS_Casting:
    msg = FormatCombatCastText();
    break;
  case OCS_ActivateSpell:
    msg=FormatActivateSpellText();
    break;
  case OCS_Attacking:
    msg = FormatCombatAttackText();
    break;
  case OCS_Guarding: 
    msg = FormatCombatGuardText();
    break;
  case OCS_Bandaging:
    msg = FormatCombatBandageText();
    break;
  case OCS_Using:
    break;
  case OCS_Moving: // shouldn't happen
    die(0xab4d0);
    //msg = FormatCombatMoveText();
    break;
  case OCS_Turning:
    msg = FormatCombatTurnUndeadText();
    break;
  case OCS_Fleeing:
    msg = FormatCombatFleeText();
    break;
  case OCS_Delaying:
    msg = FormatCombatDelayText();
    break;
  case OCS_EndingTurn:
    msg = FormatCombatEndTurnText();
    break;
  case OCS_ForcingWin:
    msg = FormatCombatForceWinText();
    break;
  }
  return msg;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::RemoveAllCombatants(void)
{
  for (int i=0; i<m_iNumCombatants; i++)
  {
    CHARACTER *pChar;
    COMBATANT *pCombatant;
    pCombatant = &m_aCombatants[i];
    pChar = pCombatant->m_pCharacter;
    //CString msg;
    //msg.Format( "CombatData.Clear. i=%d; m_aCombatants[i]=%d; pChar=%d; pChar->m_pCombatant=%d",
    //             i,
    //             pCombatant,
    //             pChar,
    //             pChar->m_pCombatant);
    //MsgBoxError(msg, "Info", 99999);
    if (pChar == NULL)
    {
      CString msg;
      msg.Format("Combatant[%d] not linked to Character", i);
      MsgBoxError(msg, "Warning");
    }
    else
    {
      if (pChar->m_pCombatant != &m_aCombatants[i])
      {
        CString msg;
        msg.Format("Character not linked to Combatant[%d]", i);
        MsgBoxError(msg, "Warning");
      }
      else
      {
        pChar->m_pCombatant = NULL;
        if (pCombatant->deleteChar)
        {
          delete pChar;
        };
      };
      pCombatant->m_pCharacter = NULL;
    };
  };
  m_iNumCombatants        = 0; 
}

void COMBAT_DATA::Clear()
{
  m_bStartingNewCombatRound = FALSE;
  m_bIsWilderness      = FALSE;
  m_bCombatActive      = FALSE;
//  m_bPlayerTypedSpace  = FALSE;
  ResetAim();
  ClearSounds();
  m_hCharDeathSound      = -1;
  m_hCharMoveSound       = -1;
  m_hTurnUndeadSound     = -1;
  m_bLastRound         = FALSE;
  m_bCombatOver        = FALSE;
  m_eCombatVictor         = NoWinner;
  m_iExpPointTotal        = 0;
  m_iStartTerrainX        = -1;
  m_iStartTerrainY        = -1;
  m_iCursorX              = -1; 
  m_iCursorY              = -1;
  m_iPartyStartX          = -1;
  m_iPartyStartY          = -1;
  m_iCurrInitiative       = -1;
  m_iCurrRound            = -1;
  m_iCurrTurn             = -1;
  m_nextAuraID            = 1;
  m_auras.RemoveAll();
  RemoveAllCombatants();
  m_bNeedUpdate        = FALSE;
  m_bMonsterNoMove     = FALSE;
  m_bMonsterAlwaysMiss = FALSE;
  m_bMonsterAlwaysHit  = FALSE; 
  m_bMonsterNoCast     = FALSE;
  m_bMonsterNoFlee     = FALSE;
  m_bMonsterPlyrControl= FALSE;
  m_bMonsterNoDeath    = FALSE;
  m_iNumMonFlee           = 0;
  m_iNumMonSlain          = 0;
  m_iNumFrndFlee          = 0;
  m_iNumFrndSlain         = 0;
  m_bPartyAlwaysMiss   = FALSE; 
  m_bPartyAlwaysHit    = FALSE;
  m_bPartyNeverDies    = FALSE;
  m_bNoMagic           = FALSE;
  m_iCurrCaster            = -1;
  m_iRedirectedSpellTarget = -1;
  QComb.Clear();
  //m_aCombatants.RemoveAll();
  //m_aTempCOMBATANTS.RemoveAll();
  m_aCombatants.SetSize(MAX_COMBATANTS);
  m_aTempCOMBATANTS.SetSize(MAX_COMBATANTS);
  hurledWeapons.Clear();
  turningSummary.Clear();
}

//*****************************************************************************
//    NAME: COMBAT_DATA::GetCombatantState
//
// PURPOSE: 
//
//*****************************************************************************
individualCombatantState COMBAT_DATA::GetCombatantState()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].State(); 
  else
    return ICS_None;
}

void COMBAT_DATA::SetCombatantState(individualCombatantState ICS)
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].State(ICS); 
  else
    return;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::AddTarget
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::AddTarget(int target) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].AddTarget(target, false); 
}

/* 20111024
int COMBAT_DATA::Get_FreeAttack_Target(void)
{
  if (QComb.NumFreeAttacks())
  {
    return QComb.Target();
  }
  else
  {
    if (QComb.NumGuardAttacks())
    {
      return QComb.Target();
    }
  };
  return NO_DUDE;
}
*/
//*****************************************************************************
//    NAME: COMBAT_DATA::GetCurrTarget
//
// PURPOSE: 
//
//*****************************************************************************
int COMBAT_DATA::GetCurrTarget(bool updateTarget) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].GetCurrTarget(updateTarget, false, true); 
  else
    return NO_DUDE;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::GetNextTarget
//
// PURPOSE: 
//
//*****************************************************************************
int COMBAT_DATA::GetNextTarget() 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].GetNextTarget(); 
  else
    return NO_DUDE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::RemoveCurrTarget
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::RemoveCurrTarget()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].RemoveCurrTarget(); 
}

//*****************************************************************************
//    NAME: COMBAT_DATA::AttacksRemaining
//
// PURPOSE: 
//
//*****************************************************************************
double COMBAT_DATA::AttacksRemaining()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].AttacksRemaining(); 
  else
    return 0.0;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::IsAttacking
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::IsAttacking() 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].IsAttacking(); 
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::CanTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanTarget(bool freeAttack) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].CanTarget(freeAttack); 
  else
    return FALSE;
}



//*****************************************************************************
//    NAME: COMBAT_DATA::IsAttackPossible
//
// PURPOSE: Can the current combatant make any attack whatsoever.
//
//*****************************************************************************
BOOL COMBAT_DATA::IsAttackPossible(void) 
{ 
  int curr = GetCurrCombatant();
  if (curr == NO_DUDE) return FALSE;
  return m_aCombatants[curr].IsAttackPossible(); 
}


//*****************************************************************************
//    NAME: COMBAT_DATA::CanMove
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanMove()  
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].CanMove(FALSE); 
  else
    return FALSE;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::CanCast
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanCast()  
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].CanCast(); 
  else
    return FALSE;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::CanUse
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanUse()   
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)    
    return m_aCombatants[curr].CanUse(); 
  else
    return FALSE;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::CanGuard
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanGuard(GUARDING_CASE guardCase) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].CanGuard(guardCase); 
  else
    return FALSE;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::CanAttack
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanAttack(int target) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].canAttack(target, -1, -1, 0, Distance, false); 
  else
    return FALSE;
}

void TURN_DATA::Clear(void)
{
  monsterID.Empty();     // Type of monster to be turned.
  numMonster=0;
  monsterTurnType.Empty();
  successDie=21;           // What needs to be rolled for successful turning
  minAffected=0;
  maxAffected=0;
  whatToDo=0;             // 1='Turn'; 2='Destroy';
  numToTurn = 0;
  numTurned = 0;
}

int TURNING_SUMMARY::LocateTurnData(const MONSTER_ID& monsterID) const
{
  int i, n;
  n = turnDataSize;
  for (i=0; i<n; i++)
  {
    if (PeekTurnData(i)->monsterID == monsterID) return i;
  };
  return -1;
};

int TURNING_SUMMARY::LocateUndeadType(const CString& undeadType) const
{
  int i, n;
  n = undeadTypes.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekUndeadType(i)->monsterTurnType == undeadType) return i;
  };
  return -1;
};

BOOL TURNING_SUMMARY::IsTurningPossible(void)
{
  int i, n;
  n = turnDataSize;
  for (i=0; i<n; i++)
  {
    const TURN_DATA *pTurnData;
    pTurnData = PeekTurnData(i);
    if (pTurnData->maxAffected > pTurnData->numTurned)
    {
      return TRUE;
    };
  };
  return FALSE;
};

//*****************************************************************************
//    NAME: COMBAT_DATA::SummarizeTurningCapability
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::SummarizeTurningCapability()  
{
  int curr = GetCurrCombatant();
  int i, turnSkill;
  const COMBATANT       *pCombatant;
  const CHARACTER       *pCharacter;
  const COMBATANT       *pTarget;
  //SKILL_ID skillID;
  // The baseclass with the highest effective turning level.
  // This is the baseclassLevel - 'Turn level' (from Skill=Turn,n)

  //skillID = Skill_Turn;
  turningSummary.Clear();
  turningSummary.Validate();

  pCombatant = &m_aCombatants[curr];
  pCharacter = pCombatant->m_pCharacter;
  const BASE_CLASS_DATA *pBestBaseclass=NULL;

  //GetAdjSkillValue(skillID,
  //                 pCharacter,
  //                &pBestBaseclass,
  //                 NULL,
  //                &turnSkill,
  //                 false);
  {
    SKILL_COMPUTATION  SC(pCharacter, Skill_Turn, false, true);
    SC.ppBestBaseclass = &pBestBaseclass;
    GetAdjSkillValue(SC);
    if (pBestBaseclass == NULL) return;  // The skill must originate in a baseclass.
    // We now know that this dude (currCombatant) can turn.
    // Next we will see if there are any undead  enemy that might be turned.

    turningSummary.combatantIndex = curr;
    turningSummary.baseclassID = pBestBaseclass->BaseclassID();
    turningSummary.baseclassLevel = pCharacter->GetCurrLevel(turningSummary.baseclassID);
    turnSkill = SC.finalAdjustedValue + 0.5;
    turningSummary.turnLevel = turnSkill;
  };
  for (i=0; i<this->NumCombatants(); i++)
  {
    if (i == curr) continue;
    pTarget = &m_aCombatants[i];
    if (pTarget->IsPartyMember()) continue;
    if (pTarget->m_pCharacter->GetStatus() == Gone) continue;
    if (!pTarget->GetUndeadType().IsEmpty())
    {
      const MONSTER_DATA *pMonster;
      int effectiveTurnLevel;
      CString monsterTurnType;
      MONSTER_ID monsterID;
      monsterID = pTarget->m_pCharacter->monsterID;
      if (monsterID.IsEmpty()) continue;
      pMonster = monsterData.PeekMonster(monsterID);
      if (pMonster == NULL) continue;
      monsterTurnType = pMonster->undeadType;
      TURN_DATA *pTurnData;
      pTurnData = turningSummary.TurnData(monsterID);
      if (pTurnData == NULL)
      {
        int what;
        int range;
        effectiveTurnLevel =   turningSummary.baseclassLevel 
                             + turningSummary.turnLevel;
        what = IntegerTableLookup(SA_Turn,
                                  monsterTurnType,
                                  effectiveTurnLevel-1,
                                  "I");
        if (what < 0) what = 1100112;
        pTurnData = turningSummary.Insert();
        pTurnData->monsterID = monsterID;
        pTurnData->monsterTurnType = monsterTurnType;
        pTurnData->successDie = (what/10000)%100;
        pTurnData->minAffected = (what/100)%100;
        pTurnData->maxAffected = what%100;
        pTurnData->whatToDo = what/1000000;
        range = pTurnData->maxAffected - pTurnData->minAffected + 1;
        pTurnData->numToTurn = pTurnData->minAffected - 1;
        pTurnData->numToTurn += RollDice(6, range/6, 0);
        if ((range%6) != 0)
        {
          pTurnData->numToTurn += RollDice(range%6, 1,0);
        };
      };
      pTurnData->numMonster++;
    };
  };
  turningSummary.ClearUndeadTypes();
  for (i=0; i<turningSummary.GetSize(); i++)
  {
    const TURN_DATA *pTurnData;
    CString undeadType;
    pTurnData = turningSummary.PeekTurnData(i);
    undeadType  = pTurnData->monsterTurnType;
    int j, m;
    m = turningSummary.GetUndeadTypeCount();
    for (j=0; j<m; j++)
    {
      if (turningSummary.PeekUndeadType(j)->monsterTurnType == undeadType) break;
    };
    if (j == m)
    {
      UNDEAD_TYPE undeadEntry;
      undeadEntry.maxToTurn = 0;
      undeadEntry.numTurned = 0;
      undeadEntry.monsterTurnType = undeadType;
      turningSummary.Add(undeadEntry);
    };
  };

  if (turningSummary.GetSize() > 0)
  {
    turningSummary.rolled = RollDice(20,1,0);
  };
}

//*****************************************************************************
//    NAME: COMBAT_DATA::CanTurnUndead
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanTurnUndead()  
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
  {
    if (!turningSummary.IsValid())
    {
      SummarizeTurningCapability();
      // Run the hook "CAN_TURN" to see if our scripting
      // friends agree that this character can "TURN".
      HOOK_PARAMETERS hookParameters;
      CString result;
      SCRIPT_CONTEXT scriptContext;
      COMBATANT *dude;
      dude = GetCombatant(curr);
      scriptContext.SetCombatantContext(dude);
      //pClass = &classData[dude->GetClass()];
      if (turningSummary.IsTurningPossible())
      {
        int i;
        hookParameters[5] = "Y";
        for (i=0; i<turningSummary.GetUndeadTypeCount(); i++)
        {
          hookParameters[6] += ',';
          hookParameters[6] += turningSummary.PeekUndeadType(i)->monsterTurnType;
        };
      };
      result = dude->RunCombatantScripts(
                          CAN_TURN,
                          ScriptCallback_RunAllScripts,
                          NULL,
                          "Determine if attacker can 'TURN'");
      if (result.IsEmpty() || (result[0]!='Y'))
      {
        turningSummary.Validate();  // Makes turning impossible
      };
    };
    if (turningSummary.IsTurningPossible())
    {
      return TRUE;
    };
  };
  return FALSE;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::CanDelay
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::CanDelay() 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].CanDelay(); 
  else
    return FALSE;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::EndTurn
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::EndTurn(individualCombatantState newState) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].EndTurn(newState);     
  }
}
//*****************************************************************************
//    NAME: COMBAT_DATA::StopMoving
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::StopMoving() 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].StopMoving(); 
}
//*****************************************************************************
//    NAME: COMBAT_DATA::Quick
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::Quick(BOOL Enable) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].Quick(Enable); 
}
//*****************************************************************************
//    NAME: COMBAT_DATA::Guard
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::Guard() 
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].Guard(); 
}
//*****************************************************************************
//    NAME: COMBAT_DATA::StartInitialSpellCasting
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBAT_DATA::StartInitialSpellCasting(GLOBAL_SPELL_ID key, GLOBAL_SPELL_ID secondaryKey)
void COMBAT_DATA::StartInitialSpellCasting(const SPELL_ID& spellID, const SPELL_ID& secondarySpellID)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr!=NO_DUDE);
  if (curr != NO_DUDE)
    //m_aCombatants[curr].StartInitialSpellCasting(key, secondaryKey);
    m_aCombatants[curr].StartInitialSpellCasting(spellID, secondarySpellID);
}
//*****************************************************************************
//    NAME: COMBAT_DATA::StartInitialUSeSpellCasting
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBAT_DATA::StartInitialItemSpellCasting(GLOBAL_SPELL_ID key)
void COMBAT_DATA::StartInitialItemSpellCasting(const SPELL_ID& spellID)
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr!=NO_DUDE);
  if (curr != NO_DUDE)
    //m_aCombatants[curr].StartInitialItemSpellCasting(key);
    m_aCombatants[curr].StartInitialItemSpellCasting(spellID);
}
//*****************************************************************************
//    NAME: COMBAT_DATA::EndInitialSpellCasting
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::EndInitialSpellCasting()
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr!=NO_DUDE);
  if (curr != NO_DUDE)
    m_aCombatants[curr].EndInitialSpellCasting();
}

//*****************************************************************************
//    NAME: COMBAT_DATA::StopSpellCasting
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::StopSpellCasting()
{
  int curr = m_iCurrCaster;//GetCurrCombatant();
  ASSERT(curr!=NO_DUDE);
  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].StopCasting(TRUE, false); // end turn
    //m_aCombatants[curr].EndTurn();
  }
}

//*****************************************************************************
//    NAME: COMBAT_DATA::StartTurnUndead
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::StartTurnUndead()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].TurnUndead();
}

//*****************************************************************************
//    NAME: COMBAT_DATA::TurnUndead
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::TurnUndead() 
{
  int curr = GetCurrCombatant();
  if (curr == NO_DUDE) return;

  if (!m_aCombatants[curr].IsPartyMember()) 
  {
    m_aCombatants[curr].EndTurn();
    return;
  }
  
  // Find the largest 'turning' level in this character's
  // baseclasses.
  // The 'turning' level for a baseclass is the character's 
  // level in that baseclass minus the baseclass' m_turnUndeadLevel.
  //int level = m_aCombatants[curr].GetCurrentLevel(ClericFlag);
  //if (isPaladin(m_aCombatants[curr].GetClass()))
  //{
  //  int plevel = m_aCombatants[curr].GetCurrentLevel(PaladinFlag)-2;
  //  level = max(level,plevel);
  //}
  int result;
  int turningPass;
  //int turningLevel;
  //turningLevel = m_aCombatants[curr].m_pCharacter->GetTurnUndeadLevel();

  menu.setHorzOrient();
  menu.setUseActive(FALSE); // no highlighting


  //Get a list of the undead types that are to be turned.
  {
    HOOK_PARAMETERS hookParameters;
    CString scriptResult;
    //CString undeadType;
    SCRIPT_CONTEXT scriptCntxt;
    COMBATANT *dude;
    //const TURN_DATA *pTurnData;
    int i, col;
    hookParameters[5].Empty();
    for (i=0; i<turningSummary.GetUndeadTypeCount(); i++)
    {
      hookParameters[5] += ',';
      hookParameters[5] += turningSummary.PeekUndeadType(i)->monsterTurnType;
    };

    dude = GetCombatant(curr);
    scriptCntxt.SetCombatantContext(dude);
    //pClass = &classData[dude->GetClass()];
    scriptResult = dude->RunCombatantScripts(
                        TURN_ATTEMPT, 
                        ScriptCallback_RunAllScripts,
                        NULL,
                        "Determine what monsters can be turned");
    col = 0;
    while (col < scriptResult.GetLength()-1)
    {
      char delim;
      CString type;
      int end;
      UNDEAD_TYPE *pUndeadType;
      delim = scriptResult[col];
      end = scriptResult.Find(delim, col+1);
      if (end < 0) end = scriptResult.GetLength();
      type = scriptResult.Mid(col+1, end-col-1);
      col = end;
      // Find how many of this undead type can be turned.
      pUndeadType = turningSummary.GetUndeadType(type);
      if (pUndeadType == NULL) continue; // Skip illegal undead types
      {
        int j, n;
        n = turningSummary.GetTurnDataCount();
        for (j=0; j<n; j++)
        {
          const TURN_DATA *pTurnData;
          pTurnData = turningSummary.PeekTurnData(j);
          if (pTurnData->monsterTurnType == type)
          {
            if (pTurnData->numToTurn > pUndeadType->maxToTurn)
            {
              pUndeadType->maxToTurn = pTurnData->numToTurn;
              // We are going to break out here.
              // We could continue and find the maximum among
              // all monster types of the same undead type.
              // They were 'rolled' independently.  But I think
              // we were supposed to 'roll' for the single undead
              // type rather than for each monster type.  Breaking
              // out of the loop will have that effect.  PRS 20130118
              if (!m_bNoCapOnTurnUndeadByTotalRoll) { break; }
            };
          };
        };
      };
    };
  };

  for (turningPass=0; turningPass<2; turningPass++)
  {
    // Ignore 'Fleeing' monsters during turningPass 0.
    for (int i=0; i<m_iNumCombatants; i++)
    {
      const COMBATANT *pTarget;
      charStatusType status;
      MONSTER_ID monsterID;
      TURN_DATA *pTurnData;
      CString undeadType;
      UNDEAD_TYPE *pUndeadType;
      pTarget = &m_aCombatants[i];
      monsterID = pTarget->m_pCharacter->monsterID;
      pTurnData = turningSummary.TurnData(monsterID);
      if (pTurnData == NULL) continue;
      status = pTarget->m_pCharacter->GetStatus();
      if (status == Gone) continue;
      if (status == Dead) continue;
      if (turningPass == 0) 
      {
        if (status == Fled) continue;
        if (status == Running) continue;
      };
      undeadType = pTurnData->monsterTurnType;
      pUndeadType = turningSummary.GetUndeadType(undeadType);
      if (pUndeadType == NULL)
      {
        result = UNDEAD_NOEFFECT;
      }
      else
      {
        if (pUndeadType->maxToTurn > pUndeadType->numTurned)
        {
          result = UNDEAD_TURNED;
          if (pTurnData->whatToDo == 2) result = UNDEAD_DESTROY;
          pTurnData->numTurned++;
          pUndeadType->numTurned++;
        } 
        else
        {
          result = UNDEAD_NOEFFECT;
        };
      };
      CString turnMsg("");
      switch (result)
      {
      case UNDEAD_TURNED: // undead will flee the combat map (status=Running, then Fled)
        menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
        PlaceCursorOnDude(i,FALSE); // calls ensure visible
        //EnsureVisibleTarget(i);        
        UpdateCombatTimeDelayMsg();
        theApp.AppSleep(225); // give time to see normal icon before death animation
  

        turnMsg.Format("%s is turned by %s...", 
                      m_aCombatants[i].GetName(),m_aCombatants[curr].GetName());
        menu.changeMenuItem(1, turnMsg);
        UpdateCombatTimeDelayMsg();
        m_aCombatants[i].PlayCombatTurnUndead();
  
        theApp.AppSleep(500); // give time to see normal icon
  
        m_aCombatants[i].EndTurn();
        m_aCombatants[i].SetStatus(Running);
        m_aCombatants[i].isTurned=TRUE;
        m_aCombatants[i].m_iLastAttacker = curr; // so it knows who to flee from
        TRACE("Turn Undead: %i is turned and fleeing\n", m_aCombatants[i].self);
        break;
      case UNDEAD_DESTROY: // undead will be removed from the map (status=Gone)
        menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
        PlaceCursorOnDude(i,FALSE); // calls ensure visible
        //EnsureVisibleTarget(i);
        UpdateCombatTimeDelayMsg();
        theApp.AppSleep(225); // give time to see normal icon before death animation
        
        turnMsg.Format("%s is destroyed by %s...", 
                      m_aCombatants[i].GetName(),m_aCombatants[curr].GetName());
        menu.changeMenuItem(1, turnMsg);
        UpdateCombatMoveScreen();        
  
        m_aCombatants[i].EndTurn();
        m_aCombatants[i].SetStatus(Gone);
        m_aCombatants[i].SetHitPoints(-10);
        m_aCombatants[i].blitDeadSprite(); // built-in delay at the end, plays death sound
        placeCombatant(m_aCombatants[i].x, m_aCombatants[i].y, NO_DUDE, m_aCombatants[i].width, m_aCombatants[i].height);
        TRACE("Turn Undead: %i is turned and destroyed\n", m_aCombatants[i].self);
        break;
      default:
        // do nothing
        break;
      }
    };
  };
  PlaceCursorOnCurrentDude();
  m_aCombatants[curr].EndTurn();
}
//*****************************************************************************
//    NAME: COMBAT_DATA::DelayAction
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::DelayAction() 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].DelayAction(); 
}
//*****************************************************************************
//    NAME: COMBAT_DATA::AttackOver
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::AttackOver() 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].AttackOver(); 
}
//*****************************************************************************
//    NAME: COMBAT_DATA::StopAttack
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::StopAttack() 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].StopAttack(); 
}
//*****************************************************************************
//    NAME: COMBAT_DATA::MakeAttack
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBAT_DATA::MakeAttack() 
//{ 
//  int curr = GetCurrCombatant();
//  if (curr != NO_DUDE)
//    m_aCombatants[curr].makeAttack(m_aCombatants[curr].GetCurrTarget()); 
//}
//*****************************************************************************
//    NAME: COMBAT_DATA::StartAttack
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::StartAttack() 
{ 
  int curr = GetCurrCombatant();
  int currTarg;
  currTarg = m_aCombatants[curr].GetCurrTarget(true, true, true);

  if ( (curr != NO_DUDE) && (currTarg != NO_DUDE))
  {
    if (m_aCombatants[curr].StartAttack(currTarg, QComb.NumFreeAttacks() + QComb.NumGuardAttacks()))
    {
      return TRUE;
    };
  };
  return FALSE;
}


//*****************************************************************************
//    NAME: COMBAT_DATA::GetCombatState
//
// PURPOSE: 
//
//*****************************************************************************
overallCombatState COMBAT_DATA::GetCombatState() 
{ 
  ASSERT( m_iNumCombatants > 0 );

  // first check for some special cases
  // that override the current combatant state
  //
  if (IsCombatOver())
  {
    return OCS_CombatOver;
  }

  if (m_bStartingNewCombatRound)
  {    
    return OCS_StartNewRound;
  }

  //if (IsStartOfTurn() && (!IsFreeAttacker()))
  if (IsStartOfTurn())
  {
    return OCS_NewCombatant;
  }

  return (overallCombatState)GetCombatantState();
}

// have we changed to the next active combatant
//*****************************************************************************
//    NAME: COMBAT_DATA::IsStartOfTurn
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::IsStartOfTurn() 
{ 
  //PRS 20110223 int dude = GetCurrCombatant();
  //PRS 20110223 return ((m_iPrevRndCombatant != dude));  
  return QComb.StartOfTurn() || QComb.RestartInterruptedTurn();
}

// is there a queued combatant that is waiting to make a free
// attack.
//*****************************************************************************
//    NAME: COMBAT_DATA::IsFreeAttacker
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::IsFreeAttacker() 
{ 
  return     (QComb.Top() != NO_DUDE) 
         &&  !QComb.ChangeStats() 
         &&  QComb.NumFreeAttacks(); 
}

BOOL COMBAT_DATA::IsGuardAttacker() 
{ 
  return     (QComb.Top() != NO_DUDE) 
         &&  !QComb.ChangeStats() 
         &&  QComb.NumGuardAttacks(); 
}

//*****************************************************************************
//    NAME: COMBAT_DATA::DecrementFreeAttacks
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::DecrementFreeAttacks(void) 
{ 
  QComb.DecrementFreeAttacks() ;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::HandleCombatRoundMsgExpired
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::HandleCombatRoundMsgExpired()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    m_aCombatants[curr].HandleCombatRoundMsgExpired(); 
  //m_iPrevRndCombatant = curr;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::OnAuto
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::OnAuto(bool callAutoActionHook) 
{ 
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].OnAuto(callAutoActionHook); 
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::toggleReadyItem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::toggleReadyItem(int item)
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].toggleReadyItem(item); 
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::delCharacterItem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBAT_DATA::delCharacterItem(int index, int qty)
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
    return m_aCombatants[curr].delCharacterItem(index, qty); 
  else
    return FALSE;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::determineAvailAttacks
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::determineAvailAttacks()
{
  int curr = GetCurrCombatant();
  if (curr != NO_DUDE)
  {
    m_aCombatants[curr].determineNbrAttacks();
    m_aCombatants[curr].determineAvailAttacks(m_aCombatants[curr].GetNbrAttacks()); 
  };
}

//*****************************************************************************
//    NAME: COMBAT_DATA::PlayCombatDeath
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::PlayCombatDeath() 
{ 
  if (!PlaySoundEffects) return;
  if (!ValidSound(m_hCharDeathSound))
  {
    //WriteDebugString("105 Calling LoadSound(%s)\n", m_pEvent->DeathSound);
    m_hCharDeathSound = LoadSound(m_pEvent->DeathSound);
  };
  if (!PlaySound(m_hCharDeathSound))
    m_hCharDeathSound=-1;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::PlayCombatMove
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::PlayCombatMove() 
{ 
  if (!PlaySoundEffects) return;
  if (!ValidSound(m_hCharMoveSound))
    m_hCharMoveSound = LoadSound(m_pEvent->MoveSound);
  if (!PlaySound(m_hCharMoveSound))
    m_hCharMoveSound=-1;
}
//*****************************************************************************
//    NAME: COMBAT_DATA::PlayCombatTurnUndead
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::PlayCombatTurnUndead() 
{ 
  if (!PlaySoundEffects) return;
  if (!ValidSound(m_hTurnUndeadSound))
    m_hTurnUndeadSound = LoadSound(m_pEvent->TurnUndeadSound);
  if (!PlaySound(m_hTurnUndeadSound))
    m_hTurnUndeadSound=-1;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::LoadSounds
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::LoadSounds()
{
  ClearSounds();
  //WriteDebugString("106 Calling LoadSound(%s)\n", m_pEvent->DeathSound);
  m_hCharDeathSound  = LoadSound(m_pEvent->DeathSound);
  m_hCharMoveSound   = LoadSound(m_pEvent->MoveSound);
  m_hTurnUndeadSound = LoadSound(m_pEvent->TurnUndeadSound);
}
//*****************************************************************************
//    NAME: COMBAT_DATA::ClearSounds
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::ClearSounds()
{
  if (m_hCharDeathSound >= 0)
    ClearSound(&m_hCharDeathSound);
  if (m_hCharMoveSound >= 0)
    ClearSound(&m_hCharMoveSound);
  if (m_hTurnUndeadSound >= 0)
    ClearSound(&m_hTurnUndeadSound);
  m_hCharDeathSound = -1;
  m_hCharMoveSound = -1;
  m_hTurnUndeadSound=-1;
}

//
// 1 round = 1 minute
// 1 round has 10 initiative steps
// 10 rounds = 1 turn
//
//*****************************************************************************
//    NAME: COMBAT_DATA::GetCurrCombatant
//
// PURPOSE: 
//
//*****************************************************************************
int COMBAT_DATA::GetCurrCombatant() 
{ 
  return QComb.Top(); 
}

//*****************************************************************************
//    NAME: CurrCombatTurn
//
// PURPOSE: 
//
//*****************************************************************************
int CurrCombatTurn() 
{ 
  return combatData.m_iCurrTurn; 
}

/*  There is already a "GetCurrentRound()" to do this.  PRS 9 June 2010
//*****************************************************************************
//    NAME: CurrCombatRound
//
// PURPOSE: 
//
//*****************************************************************************
int CurrCombatRound()
{
  return combatData.m_iCurrRound;
}
*/
//*****************************************************************************
//    NAME: CurrCombatInitiative
//
// PURPOSE: 
//
//*****************************************************************************
int CurrCombatInitiative() 
{ 
  return combatData.m_iCurrInitiative; 
}

//*****************************************************************************
//    NAME: COMBAT_DATA::getCombatPartyCharacter
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT& COMBAT_DATA::getCombatPartyCharacter(int PartyIdx)
{
  if ((PartyIdx < 0) || (PartyIdx >= party.numCharacters))
    return getActiveCombatCharacter();

  for (int i=0; i<m_iNumCombatants; i++)
  {
    if ((m_aCombatants[i].IsPartyMember()) && (m_aCombatants[i].origKey == PartyIdx))
    {
      if (m_aCombatants[GetCurrCombatant()].GetType() == MONSTER_TYPE)
        WriteDebugString("getActiveCombatCharacter() called for monster combatant\n");
      return m_aCombatants[i];
    }
  }

  ASSERT( FALSE ); // shouldn't get here
  return m_aCombatants[0];
}

//*****************************************************************************
//    NAME: *COMBAT_DATA::getCombatantPtr
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *COMBAT_DATA::getCombatantPtr(int index)
{
  if ((index < 0) || (index >= m_iNumCombatants))
  {
    return NULL;
  }
  return &m_aCombatants[index];
}

//*****************************************************************************
//    NAME: COMBAT_DATA::ProcessTimeSensitiveData
//
// PURPOSE: 
//
//*****************************************************************************
static int lastUpdateRound=-1;
static int lastUpdateInitiative=-1;
bool COMBAT_DATA::ProcessTimeSensitiveData(LONGLONG currTime)
{
  if (IsCombatOver()) return false;
  // update once per combat round
  int currRnd = GetCurrentRound();//CurrCombatRound();
  int currInit = CurrCombatInitiative();
  bool UpdateScreen = false;

  bool update = ((lastUpdateRound != currRnd) || (lastUpdateInitiative != currInit));
  if (update)
  {     
    if ((lastUpdateRound >= 0) || (lastUpdateInitiative >= 0))
    {
      int roundDelta = currRnd-lastUpdateRound;

      if (roundDelta > 0) // 1 round per minute
      {
        activeSpellList.ProcessTimeSensitiveData(roundDelta);
        pendingSpellList.ProcessTimeSensitiveData(roundDelta, currInit);        
        // Moved to StartNewRound 20110920 PRS  
        // party.incrementClock(roundDelta);        

        for (int i=0; i<m_iNumCombatants; i++)
        {          
          m_aCombatants[i].m_pCharacter->CheckForExpiredSpellEffects();
/*          
          if (m_aCombatants[i].GetAdjSpecAb(SA_Poisoned))
          {
            double currhp = m_aCombatants[i].GetHitPoints();
            if (currhp > 0)
            {
              double rateofloss = 0.25;
              BOOL IsSlowed = m_aCombatants[i].GetAdjSpecAb(SA_SlowPoison);
              if (IsSlowed)              
                rateofloss = 0.12;
            
              double hploss = currhp * rateofloss;
              hploss += 0.5; // round up
              hploss = floor(hploss);
              if (hploss < 1.0)
                hploss = 1.0;
              int newhptotal = currhp - hploss;
              if ((IsSlowed) && (newhptotal < 1))
                newhptotal = 1;   
              hploss = currhp - newhptotal;              
              if (hploss > 0)
              {
                PlaceCursorOnDude(i,FALSE); // calls ensure visible
                CString msg;
                msg.Format("%s takes poison damage", m_aCombatants[i].GetName());
                if (m_aCombatants[i].IsSpellPending())
                  msg += ", pending spell is lost";
                FormatCombatText(combatTextData, msg);
                UpdateCombatTimeDelayMsg();
                m_aCombatants[i].PlayHit();
                m_aCombatants[i].TakeDamage(hploss, FALSE, NULL, false, NULL);
                theApp.AppSleep(GetMsgTimeDelay());
                ClearFormattedText(combatTextData);
              };
            };
          };
*/
        };
      };
    };
    lastUpdateRound=currRnd;
    lastUpdateInitiative = currInit;
  }

  return UpdateScreen;
}

//*****************************************************************************
//    NAME: COMBAT_DATA::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::GetContext(ActorType *pActor, int i)
{
  if ((i>=0)&&(i<m_iNumCombatants)) m_aCombatants[i].GetContext(pActor);
  else 
    //return NULL_ACTOR;
    *pActor = NULL_ACTOR;
}


COMBATANT *GetCombatant(ActorType *pActor)
{
  if (!IsCombatActive() || (pActor->instanceType != InstanceType_CombatantIndex))
  {
    die(0xa1bc44);
  };
  return combatData.GetCombatant(pActor->Instance);
}

// force immediate party victory
//*****************************************************************************
//    NAME: COMBAT_DATA::ForcePartyVictory
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::ForcePartyVictory()
{
  int partycnt = 0;
  for (int i=0; i<m_iNumCombatants; i++)
  {
    if ((!m_aCombatants[i].GetIsFriendly()) && (m_aCombatants[i].charOnCombatMap(false, true)) && (!m_aCombatants[i].IsPartyMember()))
    {
      PlaceCursorOnDude(i,FALSE); // calls ensure visible
      UpdateCombatTimeDelayMsg();      
      m_aCombatants[i].TakeDamage(2000000000, FALSE, NULL, false, NULL);
      theApp.AppSleep(GetMsgTimeDelay());
    }
    else
    {
#ifdef newCombatant
      if (m_aCombatants[i].m_pCharacter->IsAlive())
#else
      if (m_aCombatants[i].IsAlive())
#endif
        partycnt++;
    }
  }
  if (partycnt > 0) m_bLastRound = TRUE;
  PlaceCursorOnCurrentDude();
}

// apply the spell effects to targets
//*****************************************************************************
//    NAME: COMBAT_DATA::InvokePendingSpellOnCombatant
//
// PURPOSE: 
//
//*****************************************************************************
int COMBAT_DATA::InvokePendingSpellOnCombatant(int caster, 
                                               int targ, 
                                               int activeSpellKey, 
                                               bool abortIfSetPartyXY,
                                               CString *displayText,
                                               BOOL *spellSucceeded,
                                               //GLOBAL_SPELL_ID spellDbKey,
                                               const SPELL_ID& spellID,
                                               InvokeSpellOptions *pInvokeOptions,
                                               ToHitComputation *pToHitComputation)
{  


  if (caster == NO_DUDE) return activeSpellKey;  
  if (targ == NO_DUDE) return activeSpellKey;
  
  COMBATANT &cdude = m_aCombatants[caster];
  COMBATANT &tdude = m_aCombatants[targ];
 
  //GLOBAL_SPELL_ID spelldbkey = spellDbKey;
  SPELL_ID spellid = spellID;

  //if (!spelldbkey.IsValidSpell())
  if (!spellid.IsValidSpell())
  {
    if (cdude.GetType()!=MONSTER_TYPE)
    {
      CHARACTER_SPELL charSp;
      if (cdude.FetchSpellBeingCast(&charSp)) 
      {
        //GLOBAL_SPELL_ID secondarySpell;
        SPELL_ID secondarySpellID;
        //spelldbkey = cspell.spell;
        spellid = charSp.spellID;
        //secondarySpell = cdude.GetSecondarySpellKeyBeingCast();
        secondarySpellID = cdude.GetSecondarySpellIDBeingCast();
        //if (secondarySpell.IsValidSpell())
        if (secondarySpellID.IsValidSpell())
        {
          //spelldbkey = secondarySpell;
          spellid = secondarySpellID;
        };
      }
      else
      {
        //if (cdude.GetItemSpellKeyBeingCast().IsValidSpell())
        if (cdude.GetItemSpellIDBeingCast().IsValidSpell())
        {
          //spelldbkey = cdude.GetItemSpellKeyBeingCast();
          spellid = cdude.GetItemSpellIDBeingCast();
        }
        else
        {
          die(0xab4d1);
          return activeSpellKey;
        };
      }
    }
    else
    {
      // monsters don't have spell books
      //spelldbkey = cdude.GetSpellKeyBeingCast();
      spellid = cdude.GetSpellIDBeingCast();
      //if (!spelldbkey.IsValidSpell())
      cdude.m_pCharacter->m_spellCastingLevel = cdude.combatant_spellCastingLevel;
      if (!spellid.IsValidSpell())
      {
        //spelldbkey = cdude.GetItemSpellKeyBeingCast();
        spellid = cdude.GetItemSpellIDBeingCast();
        if (!spellid.IsValidSpell())
        {
          die(0xcabd9);
        };
        //cdude.combatant_spellCastingLevel = 0;
      };
      
    }
  }

  //SPELL_DATA *pSdata = spellData.GetSpellData(spelldbkey);
  SPELL_DATA *pSdata = spellData.GetSpell(spellid);
  if (pSdata==NULL) 
  {
    die(0xab4d2);
    return activeSpellKey;
  }
  if (logDebuggingInfo)
  {
    WriteDebugString("InvokePendingSpellOnCombatant(): caster %i, targ %i, key %i, spell %s\n",
      caster, targ, activeSpellKey, pSdata->Name);
  };
  
  setPartyXY_x = -1;
  activeSpellKey = cdude.m_pCharacter->InvokeSpellOnTarget(*tdude.m_pCharacter, 
                                             activeSpellKey, 
                                             //spelldbkey, 
                                             spellid, 
                                             displayText, 
                                             spellSucceeded,
                                             pInvokeOptions,
                                             abortIfSetPartyXY,
                                             pToHitComputation);
  if (setPartyXY_x >= 0)
  {
    return activeSpellKey;
  };
  
  return activeSpellKey;
}

bool COMBAT_DATA::IsAreaLingerSpell(COMBATANT *pCaster)
{
  if (pCaster == NULL) return false;  
  
 
  //GLOBAL_SPELL_ID spelldbkey; // Starts clear = -1;
  SPELL_ID spellID; // Starts clear = -1;
  if (pCaster->GetType()!=MONSTER_TYPE)
  {
    //CHARACTER_SPELL cspell;
    if (!pCaster->FetchSpellBeingCast()) 
    {
      die(0xab4d3);
      return false;
    }
    //spelldbkey = pCaster->GetEffectiveSpellKeyBeingCast();
    spellID = pCaster->GetEffectiveSpellIDBeingCast();
  }
  else
  {
    // monsters don't have spell books
    //spelldbkey = pCaster->GetSpellKeyBeingCast();
    spellID = pCaster->GetSpellIDBeingCast();
    if (!spellID.IsValidSpell())
    {
      spellID = pCaster->GetItemSpellIDBeingCast();
    };
  }

  //SPELL_DATA *pSdata = spellData.GetSpellData(spelldbkey);
  SPELL_DATA *pSdata = spellData.GetSpell(spellID);
  if (pSdata==NULL) 
  {
    die(0xab4d4);
    return false;
  };
  switch (pSdata->Targeting)
  { 
     // But we need this function only for spells that have a lingering
     // effect on the map.
  case SelectedByCount:
  case Self:
  case WholeParty:
  case TouchedTargets:
  case SelectByHitDice:
    return false;
  default:
    return true;
  };
}


// apply the spell effects to targets
//*****************************************************************************
//    NAME: COMBAT_DATA::InvokeAreaLingerSpellOnCombatant
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::InvokeAreaLingerSpellOnCombatant(int caster, 
                                                   int targ, 
                                                   int activeSpellKey, 
                                                   bool abortIfSetPartyXY,
                                                   CString *displayText,
                                                   BOOL *spellSucceeded,
                                                   //GLOBAL_SPELL_ID spellDbKey,
                                                   const SPELL_ID& spellID,
                                                   InvokeSpellOptions *invokeOptions,
                                                   ToHitComputation *pToHitComputation)
{  

  if (caster == NO_DUDE) return;  
  if (targ == NO_DUDE) return;
  
  COMBATANT &cdude = m_aCombatants[caster];
  COMBATANT &tdude = m_aCombatants[targ];
 
  //GLOBAL_SPELL_ID spelldbkey = spellDbKey;
  SPELL_ID spellid = spellID;

  //if (!spelldbkey.IsValidSpell())
  if (!spellid.IsValidSpell())
  {
    die(0xcabd9);
  }

  //SPELL_DATA *pSdata = spellData.GetSpellData(spelldbkey);
  SPELL_DATA *pSdata = spellData.GetSpell(spellid);
  if (pSdata==NULL) 
  {
    die(0x4c3c6);
  }

  WriteDebugString("InvokeAreaLingerSpellOnCombatant(): caster %i, targ %i, key %i, spell %s\n", 
                    caster, targ, activeSpellKey, pSdata->Name);
  
  setPartyXY_x = -1;
  cdude.m_pCharacter->InvokeLingerSpellOnTarget(
                               *tdude.m_pCharacter, 
                               activeSpellKey, 
                               //spelldbkey, 
                               spellid, 
                               displayText, 
                               spellSucceeded,
                               invokeOptions,
                               abortIfSetPartyXY,
                               pToHitComputation);
  return;
}


//*****************************************************************************
//    NAME: COMBAT_DATA::InvokePendingSpellOnMap
//
// PURPOSE: Casting spell with area effect, and it has no current targets
//          in map area. Still need to create an active spell record.
//
//*****************************************************************************
int COMBAT_DATA::InvokePendingSpellOnMap(COMBATANT *pCaster) //int caster, int activeSpellKey)
{
  int caster;
  int activeSpellKey;
  if (pCaster == NULL) return -1;
  caster = pCaster->self;
  activeSpellKey = pCaster->combatant_activeSpellKey;
  if (logDebuggingInfo)
  {
    WriteDebugString("InvokePendingSpellOnMap(): caster %i, key %i\n",
      caster, activeSpellKey);
  };
  
  //if (caster == NO_DUDE) return -1;  
  
  //COMBATANT &cdude = m_aCombatants[caster];
 
  //GLOBAL_SPELL_ID spelldbkey; // Starts clear = -1;
  SPELL_ID spellID; // Starts clear = "";
  if (pCaster->GetType()!=MONSTER_TYPE)
  {
    CHARACTER_SPELL charSp;
    if (!pCaster->FetchSpellBeingCast(&charSp)) 
    {
      die(0xab4d6);
      return -1;
    }
    //spelldbkey = cspell.spell;
    spellID = charSp.spellID;
  }
  else
  {
    // monsters don't have spell books
    //spelldbkey = pCaster->GetSpellKeyBeingCast();
    spellID = pCaster->GetSpellIDBeingCast();
    if (!spellID.IsValidSpell())
    {
      spellID = pCaster->GetItemSpellIDBeingCast();
    };
  }

  //SPELL_DATA *pSdata = spellData.GetSpellData(spelldbkey);
  SPELL_DATA *pSdata = spellData.GetSpell(spellID);
  if (pSdata==NULL) 
  {
    die(0xab4d7);
    return -1;
  }

  if (activeSpellKey < 0)
  {
    if (pSdata->Duration_Rate != Permanent)
      activeSpellKey=activeSpellList.GetNextKey();
  }
/*
#ifdef SpellInitiationScript
  {
    ActorType cActor;  //Caster
    ActorType tActor;  //Target
    bool result;
    //GetContext(&cActor, pSdata->schoolMask);
    GetContext(&cActor, pSdata->schoolID);
    if (m_spellCastingLevel != -1)
    {
      cActor.Level = m_spellCastingLevel;
    };
    SetCharContext(&cActor);
    targ.GetContext(&tActor);
    SetTargetContext(&tActor);
  
    setPartyXY_x = -1;
    result = pSdata->ExecuteSpellBeginScript(this, &targ, NULL);
    if (abortIfSetPartyXY && (setPartyXY_x >= 0))
    {
      pSdata->ExecuteSpellEndScript(this, &targ);
      return activeSpellKey;
    };
    if (!result)
    {
      pSdata->ExecuteSpellEndScript(this, &targ);
      return activeSpellKey;
    };
  };
#endif
*/




  // check result.
  // check set partyxy
  // if fail - run spellendscript and return

  if (pSdata->Duration_Rate != Permanent)
  {            
    ASSERT( activeSpellKey >= 0 );    
    ASSERT( activeSpellList.Get(activeSpellKey) == NULL );
      
    ACTIVE_SPELL aspell;
    //pCaster->GetContext(&aspell.caster, pSdata->schoolMask);
    pCaster->GetContext(&aspell.caster, pSdata->schoolID);
    aspell.key = activeSpellKey;
    //aspell.spellKey = pSdata->m_gsID; // spell db key
    aspell.spellID = pSdata->SpellID(); // spell db key
    aspell.CountTime = 0;
    aspell.StopTime = 0;
    aspell.Lingers = pSdata->Lingers;
    aspell.lingerData.OnceOnly = pSdata->LingerOnceOnly;
    aspell.casterLevel = pCaster->combatant_spellCastingLevel;

    
    // add affected map locations to active spell record
    aspell.lingerData.mapdata.RemoveAll();
    POSITION pos = pCaster->m_pCharacter->targets.m_maptarglist.GetHeadPosition();
    while (pos != NULL)
      aspell.lingerData.mapdata.AddTail(pCaster->m_pCharacter->targets.m_maptarglist.GetNext(pos));
    
    // add targets to active spell record so that linger spell checks
    // won't re-target same dudes once they move (unless allowed by OnceOnly flag)
    for (int i=0;i<pCaster->m_pCharacter->targets.m_targets.GetSize();i++)
      aspell.lingerData.affectedtargs.AddTail(pCaster->m_pCharacter->targets.m_targets[i]);
    
    WriteDebugString("Adding active spell cast by %i\n", aspell.caster.Instance);

    double val = 0.0;
    // determine duration value
    //val = pCaster->EvalDiceAsClass(pSdata->Duration, pSdata->schoolMask, NULL);
    {
      int rollerLevel = -1;
      { 
        CString result;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        scriptContext.SetSpellContext(pSdata);
        scriptContext.SetAttackerContext(pCaster);
        //hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
        result = pSdata->RunSpellScripts(
                       SPELL_CASTER_LEVEL,
                       ScriptCallback_RunAllScripts,
                       NULL,
                       "Invoking a pending spell on map area");
        if (result.GetLength()> 0)
        {
          sscanf(result,"%d",&rollerLevel);
        };
      }
      val = pCaster->EvalDiceAsClass(pSdata->Duration, 
                                     pSdata->schoolID, 
                                     pSdata->Level,
                                     &rollerLevel);
      //pCaster->combatant_spellCastingLevel = rollerLevel;
    };
    ASSERT( val >= 0.0 );                 

    // convert duration value into game minutes
    switch (pSdata->Duration_Rate)
    {
    case inRounds:
      val = max(1,val); // 1 round min
      aspell.StopTime = val + party.getElapsedMinutes();
      break;
    case byNbrAttacks:
    case byDamageTaken: 
      aspell.StopTime = val;
      break;
    case inHours:
      val = val * 60.0;
      val = max(1,val); // 1 round min
      aspell.StopTime = val + party.getElapsedMinutes();      
      break;
    case inDays:
      val = val * 24.0 * 60.0;
      val = max(1,val); // 1 round min
      aspell.StopTime = val + party.getElapsedMinutes();      
      break;
    default: 
      die(0xab4d8); 
      break;
    }
    activeSpellList.xAddWithCurrKey(aspell);
    //die(0xcc2234); 
    // No need to run SpellEndScript....simply return activeSpellKey
  }
  else
  {
    //die(0xcc4428); // Run spellEndScript here.
#ifdef SpellInitiationScript
    pSdata->ExecuteSpellEndScript(pCaster->m_pCharacter, NULL);
#endif
  };
  return activeSpellKey;
}


//*****************************************************************************
//    NAME: COMBAT_DATA::RemoveSpellEffect
//
// PURPOSE: 
//
//*****************************************************************************
void COMBAT_DATA::RemoveSpellEffect(DWORD parent, const SPELL_DATA *pSpell, bool endSpell)
{
  for (int i=0; i<m_iNumCombatants; i++)
    m_aCombatants[i].m_pCharacter->RemoveSpellEffect(parent, pSpell, endSpell);
}

//*****************************************************************************
//    NAME: *GetLastAttackerOf
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *GetLastAttackerOf(COMBATANT *pDude)
{
  if (pDude == NULL) return NULL;
  for (int i=0; i<combatData.NumCombatants(); i++)
  {
    if (combatData.GetCombatant(i)->m_iLastAttacked == pDude->self)
    {
      return (combatData.GetCombatant(i));
    }
  }
  return NULL;
}

//*****************************************************************************
//    NAME: *GetLeastDamagedFriendly
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *GetLeastDamagedFriendly()
{
  int HighestHP = -1;
  int HighestHPIndex = -1;  

  for (int i=0; i<combatData.NumCombatants(); i++)
  {
    if (combatData.GetCombatant(i)->GetIsFriendly())
    {
      int hp = combatData.GetCombatant(i)->GetAdjHitPoints();
      if (hp > HighestHP)
      {
        HighestHP = hp;
        HighestHPIndex = i;
      }      
    }
  }

  if (HighestHPIndex >= 0)
    return (combatData.GetCombatant(HighestHPIndex));

  return NULL;  
}

//*****************************************************************************
//    NAME: *GetLeastDamagedEnemy
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *GetLeastDamagedEnemy()
{
  int HighestHP = -1;
  int HighestHPIndex = -1;  
  
  for (int i=0; i<combatData.NumCombatants(); i++)
  {
    if (!combatData.GetCombatant(i)->GetIsFriendly())
    {
      int hp = combatData.GetCombatant(i)->GetAdjHitPoints();
      if (hp > HighestHP)
      {
        HighestHP = hp;
        HighestHPIndex = i;
      }      
    }
  }
  
  if (HighestHPIndex >= 0)
    return (combatData.GetCombatant(HighestHPIndex));
  
  return NULL;  
}

//*****************************************************************************
//    NAME: *GetMostDamagedFriendly
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *GetMostDamagedFriendly()
{
  int LowestHP = 999999999;
  int LowestHPIndex = -1;  
  
  for (int i=0; i<combatData.NumCombatants(); i++)
  {
    if (combatData.GetCombatant(i)->GetIsFriendly())
    {
      int hp = combatData.GetCombatant(i)->GetAdjHitPoints();
      if (hp < LowestHP)
      {
        LowestHP = hp;
        LowestHPIndex = i;
      }      
    }
  }
  
  if (LowestHPIndex >= 0)
    return (combatData.GetCombatant(LowestHPIndex));
  
  return NULL;  
}

//*****************************************************************************
//    NAME: *GetMostDamagedEnemy
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *GetMostDamagedEnemy()
{
  int LowestHP = 999999999;
  int LowestHPIndex = -1;  
  
  for (int i=0; i<combatData.NumCombatants(); i++)
  {
    if (!combatData.GetCombatant(i)->GetIsFriendly())
    {
      int hp = combatData.GetCombatant(i)->GetAdjHitPoints();
      if (hp < LowestHP)
      {
        LowestHP = hp;
        LowestHPIndex = i;
      }      
    }
  }
  
  if (LowestHPIndex >= 0)
    return (combatData.GetCombatant(LowestHPIndex));
  
  return NULL;  
}


//*****************************************************************************
//    NAME: *GetNearestTo
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *GetNearestTo(COMBATANT *pDude)
{
  if (pDude == NULL) return NULL;

  int NearestDist = 999999999;
  int NearestIndex = -1;  
  
  for (int i=0; i<combatData.NumCombatants(); i++)
  {
    int dist = Distance(pDude->self, 
                        pDude->x, 
                        pDude->y,
                        combatData.GetCombatant(i)->self,
                        combatData.GetCombatant(i)->x,
                        combatData.GetCombatant(i)->y);
    if (dist < NearestDist)
    {
      NearestDist = dist;
      NearestIndex = i;
    }      
  }
  
  if (NearestIndex >= 0)
    return (combatData.GetCombatant(NearestIndex));
  
  return NULL;  
}

//****************************************************************************
//    NAME  GetCombatantX and GetCombatantY
//                given: combatant index
//****************************************************************************
int GetCombatantX(int index)
{
  return combatData.GetCombatant(index)->x;
}

int GetCombatantY(int index)
{
  return combatData.GetCombatant(index)->y;
}



//*****************************************************************************
//    NAME: *GetNearestEnemyTo
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT *GetNearestEnemyTo(COMBATANT *pDude)
{
  if (pDude == NULL) return NULL;
  
  int NearestDist = 999999999;
  int NearestIndex = -1;  
  
  for (int i=0; i<combatData.NumCombatants(); i++)
  {
    if (!combatData.GetCombatant(i)->GetIsFriendly())
    {
      int dist = Distance(pDude->self, 
                          pDude->x, 
                          pDude->y,
                          combatData.GetCombatant(i)->self,
                          combatData.GetCombatant(i)->x,
                          combatData.GetCombatant(i)->y);

      if (dist < NearestDist)
      {
        NearestDist = dist;
        NearestIndex = i;
      }      
    }
  }
  
  if (NearestIndex >= 0)
    return (combatData.GetCombatant(NearestIndex));
  
  return NULL;  
}

//*****************************************************************************
//    NAME: GetDistanceBetween_ByPartyIndex
//
// PURPOSE: 
//
//*****************************************************************************
int COMBAT_DATA::GetDistanceBetween_ByPartyIndex(int PartyMember1Idx, int PartyMember2Idx)
{
  if (PartyMember1Idx == PartyMember2Idx)
    return 0; // distance from me to me

  int CombatantIdx1 = -1;
  int CombatantIdx2 = -1;

  for (int i=0; i<m_iNumCombatants; i++)
  {
    if (    (combatData.m_aCombatants[i].IsPartyMember())
        && ((combatData.m_aCombatants[i].GetType() == CHAR_TYPE) || (combatData.m_aCombatants[i].GetType() == NPC_TYPE))
        &&  (combatData.m_aCombatants[i].origKey >= 0))
    {
      if (combatData.m_aCombatants[i].origKey == PartyMember1Idx)
        CombatantIdx1 = i;
      else if (combatData.m_aCombatants[i].origKey == PartyMember2Idx)
        CombatantIdx2 = i;
    }
  }

  if ((CombatantIdx1 >= 0) && (CombatantIdx2 >= 0))
  {
    int dist = Distance(combatData.m_aCombatants[CombatantIdx1].self, 
                        combatData.m_aCombatants[CombatantIdx1].x,
                        combatData.m_aCombatants[CombatantIdx1].y, 

                        combatData.m_aCombatants[CombatantIdx2].self, 
                        combatData.m_aCombatants[CombatantIdx2].x,
                        combatData.m_aCombatants[CombatantIdx2].y);

    return (dist);
  }

  die(0xab4d9);
  return 1000;
}

#ifdef UAFEngine

void COMBAT_DATA::PushAuraReference(int auraID)
{
  m_auraReferenceStack.Add(auraID);
}

void COMBAT_DATA::PopAuraReference(void)
{
  int n;
  n =m_auraReferenceStack.GetSize();
  if (n < 1)
  {
    WriteDebugString("Illegal AURA reference stack pop\n");
    return;
  };
  m_auraReferenceStack.RemoveAt(n-1);
}

void COMBAT_DATA::DeleteAura(POSITION pos)
{
  if (pos == NULL) return;
  m_auras.RemoveAt(pos);
}

void COMBAT_DATA::AddAuraSA(POSITION pos, const CString& name, const CString& param)
{
  AURA *pAURA;
  if (pos == NULL) return;
  pAURA = m_auras.GetAt(pos);
  pAURA->specialAbilities.InsertAbility(name, param,"","");
}

CString COMBAT_DATA::GetAuraSA(POSITION pos, const CString& name)
{
  AURA *pAURA;
  const CStringPAIR *pPair;
  if (pos == NULL) return NO_SUCH_SA;
  pAURA = m_auras.GetAt(pos);
  pPair = pAURA->specialAbilities.FindAbility(name);
  if (pPair == NULL) return NO_SUCH_SA;
  return pPair->Value();
}

CString COMBAT_DATA::RemoveAuraSA(POSITION pos, const CString& name)
{
  AURA *pAURA;
  CString result;
  if (pos == NULL) return NO_SUCH_SA;
  pAURA = m_auras.GetAt(pos);
  result = pAURA->specialAbilities.DeleteAbility(name);
  return result;
}

POSITION COMBAT_DATA::GetAuraReference(void)
{
  int indx, id;
  POSITION result, pos;
  AURA *pAURA;
  indx = m_auraReferenceStack.GetSize();
  if (indx == 0) return NULL;
  id = m_auraReferenceStack[indx-1];
  pos = m_auras.GetHeadPosition();
  while (pos != NULL)
  {
    result = pos;
    pAURA = m_auras.GetNext(pos);
    if (pAURA->auraID == id)
    {
      return result;
    };
  }
  return NULL;
}


void COMBAT_DATA::LocateAuraCenters(AURA *pAURA, CArray<POINT, POINT&> *center)
{
  int index, w, h;
  POINT c;
  center->RemoveAll();
  index = pAURA->combatantIndex[0];
  switch(pAURA->attachment[0])
  {
  case AURA_ATTACH_COMBATANT:
  case AURA_ATTACH_COMBATANT_FACING:
    if (index >= m_iNumCombatants)
    {
      WriteDebugString("Illegal AURA combatant index\n");
      c.x = 0;
      c.y = 0;
      return;
    };
    for (w=0; w<m_aCombatants[index].width; w++)
    {
      for (h=0; h<m_aCombatants[index].height; h++)
      {
        if (     (w==0) || (w==m_aCombatants[index].width-1)
             ||  (h==0) || (h==m_aCombatants[index].height-1)
           )
        {
          c.x = m_aCombatants[index].x+w; 
          c.y = m_aCombatants[index].y+h;
          center->Add(c);
        };
      };
    };
    break;
  case AURA_ATTACH_NONE:
    break;
  case AURA_ATTACH_XY:
    c.x = pAURA->x[0]; 
    c.y = pAURA->y[0];
    center->Add(c);
    break;
  default:
    /* Really */ NotImplemented(0xb672, false);
  };
}

#pragma warning(push)
#pragma warning(disable:4701)


void COMBAT_DATA::AnnularOctantX(
                        AURA *pAURA,
                        int minRadius,
                        int maxRadius,
                        double minTangent,
                        double maxTangent,
                        int x0,
                        int y0,
                        int dirX,
                        int dirY
                        )
{
  int x, y;;
  int sy, dsy;
  int minD2, maxD2;
  double tanyx;
  minD2 = minRadius*minRadius;
  maxD2 = maxRadius*maxRadius;
  // Work our way around the outside of the circle.
  x = maxRadius;
  y = 0;
  dsy = 0;
  while (x > 0)
  {
    int msy, Dxy2;
    Dxy2 = x*x + y*y;
    msy = 2*x;
    while (Dxy2 <= maxD2)
    {
      tanyx = fabs(double(y)/x);
      if (tanyx > maxTangent) break;
      if (tanyx >= minTangent)
      {
        // Now see if (x,y) is visible from (x0,y0)
        bool stop;
        int dy, dx, i, numStep;
        sy = dsy;
        dx = 0;
        dy = 0;
        stop = false;
        numStep = abs(x);
        for (i=0; i<numStep; i++)
        {
          OBSTICAL_TYPE ot1, ot2;
          ot1 = ObsticalType(x0+dx, y0+dy, 1, 1, 
                             true, 
                             false, 
                             NULL);
          switch(pAURA->wavelength[0])
          {
          case AURA_WAVELENGTH_VISIBLE:
            {
              if (ot1==OBSTICAL_wall)
              {
                stop = true;
                break;
              }
              else 
              {
                pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
                if (ot1==OBSTICAL_occupied) 
                {
                  if (dx != 0) 
                  {
                    stop = true;  // Occupied by combatant
                    break;
                  };
                };
              };
              if (sy >= msy)
              {
                sy -= msy;
                dy += dirY;
              };
              ot2 = ObsticalType(x0+dx+dirX, y0+dy, 1,1,
                                 true,
                                 false,
                                 NULL);
              if (ot2 ==OBSTICAL_wall)
              {
                stop = true;
                break;
              }
              else
              {
                pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx+dirX] |= 1;  // AURA effects this cell
                if (ot2==OBSTICAL_occupied) 
                {
                  stop = true;  // Occupied by combatant
                  break;
                };
              };
            };
            break;
          case AURA_WAVELENGTH_XRAY:
            {
              if (ot1 == OBSTICAL_wall) 
              {
                stop = true;
                break;
              }
              else
              {
                pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
              };
              if (sy >= msy)
              {
                sy -= msy;
                dy += dirY;
              };
              ot2 = ObsticalType(x0+dx+dirX, y0+dy, 1,1,
                                 true,
                                 false,
                                 NULL);
              if (ot2 == OBSTICAL_wall)
              {
                stop = true;
              }
              else
              {
                pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx+dirX] |= 1;  // AURA effects this cell
              };
            };
            break;
          case AURA_WAVELENGTH_NEUTRINO:
            {
              pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
              if (sy >= msy)
              {
                sy -= msy;
                dy += dirY;
              };
              pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx+dirX] |= 1;  // AURA effects this cell
            };
            break;
          };
          if (stop) break;
          sy += dsy;\
          if (sy >= msy)
          {
            sy -= msy;
            dy += dirY;
          };
          dx += dirX;
        };
      };
      Dxy2 += dsy + 1;
      y++;
      dsy = 2*abs(y);
    };
    x--;
  };
}

void COMBAT_DATA::AnnularOctantY(
                        AURA *pAURA,
                        int minRadius,
                        int maxRadius,
                        double minTangent,
                        double maxTangent,
                        int x0,
                        int y0,
                        int dirX,
                        int dirY
                        )
{
  int x, y;;
  int sx, dsx;
  int minD2, maxD2;
  double tanxy;
  minD2 = minRadius*minRadius;
  maxD2 = maxRadius*maxRadius;
  // Work our way around the outside of the circle.
  y = maxRadius;
  x = 0;
  dsx = 0;
  while (y > 0)
  {
    int msx, Dyx2;
    Dyx2 = y*y + x*x;
    msx = 2*y;
    while (Dyx2 <= maxD2)
    {
      tanxy = fabs(double(x)/y);
      if (tanxy > maxTangent) break;
      if (tanxy >= minTangent)
      {
        // Now see if (x,y) is visible from (x0,y0)
        bool stop;
        int dx, dy, i, numStep;
        sx = dsx;
        dy = 0;
        dx = 0;
        stop = false;
        numStep = abs(y);
        for (i=0; i<numStep; i++)
        {
          OBSTICAL_TYPE ot1, ot2;
          ot1 = ObsticalType(x0+dx, y0+dy, 1, 1, 
                             true, 
                             false, 
                             NULL);
          switch(pAURA->wavelength[0])
          {
          case AURA_WAVELENGTH_VISIBLE:
            {
              if (ot1==OBSTICAL_wall)
              {
                stop = true;
                break;
              }
              else 
              {
                pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
                if (ot1==OBSTICAL_occupied) 
                {
                  if (dy != 0) 
                  {
                    stop = true;  // Occupied by combatant
                    break;
                  };
                };
              };
              if (sx >= msx)
              {
                sx -= msx;
                dx += dirX;
              };
              ot2 = ObsticalType(x0+dx, y0+dy+dirY, 1,1,
                                 true,
                                 false,
                                 NULL);
              if (ot2 ==OBSTICAL_wall)
              {
                stop = true;
                break;
              }
              else
              {
                pAURA->cells[(y0+dy+dirY)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
                if (ot2==OBSTICAL_occupied) 
                {
                  stop = true;  // Occupied by combatant
                  break;
                };
              };
            };
            break;
          case AURA_WAVELENGTH_XRAY:
            {
              if (ot1 == OBSTICAL_wall) 
              {
                stop = true;
                break;
              }
              else
              {
                pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
              };
              if (sx >= msx)
              {
                sx -= msx;
                dx += dirX;
              };
              ot2 = ObsticalType(x0+dx, y0+dy+dirY, 1,1,
                                 true,
                                 false,
                                 NULL);
              if (ot2 == OBSTICAL_wall)
              {
                stop = true;
              }
              else
              {
                pAURA->cells[(y0+dy+dirY)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
              };
            };
            break;
          case AURA_WAVELENGTH_NEUTRINO:
            {
              pAURA->cells[(y0+dy)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
              if (sx >= msx)
              {
                sx -= msx;
                dx += dirX;
              };
              pAURA->cells[(y0+dy+dirY)*MAX_TERRAIN_WIDTH+x0+dx] |= 1;  // AURA effects this cell
            };
            break;
          };
          if (stop) break;
          sx += dsx;
          if (sx >= msx)
          {
            sx -= msx;
            dx += dirX;
          };
          dy += dirY;
        };
      };
      Dyx2 += dsx + 1;
      x++;
      dsx = 2*abs(x);
    };
    y--;
  };
}


const double d2r = 3.1415926535/180.0;

void COMBAT_DATA::DetermineAnnularCoverage(AURA *pAURA)
{
//  int i;
  CArray <POINT, POINT&> centers;
  int c, x0, y0;
  int minRadius, maxRadius;
  int startAngle, sectorSize;
  minRadius  = pAURA->size1[0]; if (minRadius < 0) return;
  maxRadius  = pAURA->size2[0]; if (maxRadius < minRadius) return;
  startAngle = pAURA->size3[0]; if (startAngle < 0) return;
  sectorSize = pAURA->size4[0]; if (sectorSize <= 0) return;
  if (pAURA->attachment[0] == AURA_ATTACH_COMBATANT_FACING)
  {
    switch(pAURA->facing)
    {
    case FACE_NORTH:
      startAngle += 90;
      break;
    case FACE_EAST:
      break;
    case FACE_SOUTH:
      startAngle += 270;
      break;
    case FACE_WEST:
      startAngle += 180;
      break;
    case FACE_NE:
      startAngle += 45;
      break;
    case FACE_SE:
      startAngle += 315;
      break;
    case FACE_SW:
      startAngle += 225;
      break;
    case FACE_NW:
      startAngle += 135;
      break;
    default:
      /* Really */ NotImplemented(0xc453a, false);
    };
    startAngle = startAngle % 360;
  };
  memset(pAURA->cells, 0, MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
  startAngle = startAngle % 360;
  if (sectorSize > 360) sectorSize = 360;

  if ( (startAngle % 45) == 0)
  {
    startAngle += 359;
    sectorSize += 1;
    startAngle = startAngle % 360;
  };
  if ( ((startAngle + sectorSize) % 45) == 0)
  {
    sectorSize += 1;
  };
    

  LocateAuraCenters(pAURA, &centers);
  for (c=0; c<centers.GetSize(); c++)
  {
    x0 = centers[c].x;
    y0 = centers[c].y;
    if ( (x0 < 0) || (y0 < 0)) continue; //Probably not yet placed on combat map
    {
      int octant;
      double minTangent;
      double maxTangent;
      int skipAngle, remainingAngle;
      octant = startAngle/45;
      remainingAngle = sectorSize;
      skipAngle = startAngle - 45*octant;
      while (remainingAngle > 0)
      {
        switch(octant)
        {
        case 0: 
          if (skipAngle > 0)
          {
            minTangent = tan(skipAngle*d2r);
          }
          else
          {
            minTangent = 0;
          };
          if (remainingAngle < 45)
          {
            maxTangent = tan(remainingAngle*d2r);
          }
          else
          {
            maxTangent = 1.0;
          };
          AnnularOctantX(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0, 1, -1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
        case 1: 
          if (skipAngle > 0)
          {
            maxTangent = tan((45-skipAngle)*d2r);
          }
          else
          {
            maxTangent = 0.999999;
          };
          if (skipAngle+remainingAngle < 45)
          {
            minTangent = tan((45-skipAngle-remainingAngle)*d2r);
          }
          else
          {
            minTangent = 0.0;
          };
          AnnularOctantY(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0, 1, -1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
        case 2: 
          if (skipAngle > 0)
          {
            minTangent = tan(skipAngle*d2r);
          }
          else
          {
            minTangent = 0.000001;
          };
          if (skipAngle+remainingAngle < 45)
          {
            maxTangent = tan((45-skipAngle-remainingAngle)*d2r);
          }
          else
          {
            maxTangent = 0.999999;
          };
          AnnularOctantY(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0, -1, -1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
        case 3: 
          if (skipAngle > 0)
          {
            maxTangent = tan((45-skipAngle)*d2r);
          }
          else
          {
            maxTangent = 1.0;
          };
          if (skipAngle+remainingAngle < 45)
          {
            minTangent = tan((45-skipAngle-remainingAngle)*d2r);
          }
          else
          {
            minTangent = 0.000001;
          };
          AnnularOctantX(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0, -1, -1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
        case 4: 
          if (skipAngle > 0)
          {
            minTangent = tan(skipAngle*d2r);
          }
          else
          {
            minTangent = 0;
          };
          if (remainingAngle < 45)
          {
            maxTangent = tan(remainingAngle*d2r);
          }
          else
          {
            maxTangent = 1.0;
          };
          AnnularOctantX(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0, -1, 1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
          break;
        case 5: 
          if (skipAngle > 0)
          {
            maxTangent = tan((45-skipAngle)*d2r);
          }
          else
          {
            maxTangent = 0.999999;
          };
          if (skipAngle+remainingAngle < 45)
          {
            minTangent = tan((45-skipAngle-remainingAngle)*d2r);
          }
          else
          {
            minTangent = 0.000000;
          };
          AnnularOctantY(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0,-1, 1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
        case 6: 
          if (skipAngle > 0)
          {
            minTangent = tan(skipAngle*d2r);
          }
          else
          {
            minTangent = 0.000001;
          };
          if (skipAngle+remainingAngle < 45)
          {
            maxTangent = tan((45-skipAngle-remainingAngle)*d2r);
          }
          else
          {
            maxTangent = 0.999999;
          };
          AnnularOctantY(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0, 1, 1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
        case 7: 
          if (skipAngle > 0)
          {
            maxTangent = tan((45-skipAngle)*d2r);
          }
          else
          {
            maxTangent = 1.0;
          };
          if (skipAngle+remainingAngle < 45)
          {
            minTangent = tan((45-skipAngle-remainingAngle)*d2r);
          }
          else
          {
            minTangent = 0.000001;
          };
          AnnularOctantX(pAURA,
                         minRadius,
                         maxRadius,
                         minTangent,
                         maxTangent,
                         x0, y0, 1, 1
                        );
          remainingAngle = remainingAngle-45+skipAngle;
          skipAngle = 0;
          break;
        };
        octant = (octant+1) & 7;
      };
    };
  };
/*      if (octant & 1)
  for (c=0; c<centers.GetSize(); c++)
  {
    int j;
    x0 = centers[c].x;
    y0 = centers[c].y;
    if ( (x0 < 0) || (y0 < 0)) return; //Probably not yet placed on combat map
    if (s3 > s4) s4 += 360;
    for (j=s3; j<=s4; j++)
    {
      i = j % 360;
      int x=x0, y=y0;
      int lastX, lastY;
      int sx, sy;
      int rx, ry;
      int dsx=0, dsy=0;
      int dx, dy;
      int dist2; //distance squared
      switch (i / (360/8))
      {
        case 0:
          dsx = 280;
          dsy = 2*tangents[i];
          dx = 1;
          dy = -1;
          lastX = MAX_TERRAIN_WIDTH;
          lastY = -1;
          break;
        case 1:
          dsx = 2*tangents[NUMDIR/4 - i - 1];
          dsy = 280;
          dx = 1;
          dy = -1;
          lastX = MAX_TERRAIN_WIDTH;
          lastY = -1;
          break;
        case 2:
          dsx = 2*tangents[i - NUMDIR/4];
          dsy = 280;
          dx = -1;
          dy = -1;
          lastX = -1;
          lastY = -1;
          break;
        case 3:
          dsx = 280;
          dsy = 2*tangents[NUMDIR/2 - i -1];
          dx = -1;
          dy = -1;
          lastX = -1;
          lastY = -1;
          break;
        case 4:
          dsx = 280;
          dsy = 2*tangents[i-NUMDIR/2];
          dx = -1;
          dy = 1;
          lastX = -1;
          lastY = MAX_TERRAIN_HEIGHT;
          break;
        case 5:
          dsx = 2*tangents[3*NUMDIR/4 - i - 1];
          dsy = 280;
          dx = -1;
          dy = 1;
          lastX = -1;
          lastY = MAX_TERRAIN_HEIGHT;
          break;
        case 6:
          dsx = 2*tangents[i - 3*NUMDIR/4];
          dsy = 280;
          dx = 1;
          dy = 1;
          lastX = MAX_TERRAIN_WIDTH;
          lastY = MAX_TERRAIN_HEIGHT;
          break;
        case 7:
          dsx = 280;
          dsy = 2*tangents[NUMDIR - i - 1];
          dx = 1;
          dy = 1;
          lastX = MAX_TERRAIN_WIDTH;
          lastY = MAX_TERRAIN_HEIGHT;
          break;
      };
      rx = 0;
      ry = 0;
      sx = dsy/2;
      sy = dsx/2;
      for (dist2=0;dist2<=outerRadius2;)
      {
        if (pAURA->combatantIndex[0] == 5)
        {
          if (j == 21)
          {
            int kkk=1;
          };
        };
        if (dist2 >= innerRadius2)
        {
          bool stop = false;
          if (dist2 != 0)
          {
            OBSTICAL_TYPE ot;
            ot = ObsticalType(x, y, 1, 1, 
                              true, 
                              false, 
                              NULL);
            switch(pAURA->wavelength[0])
            {
            case AURA_WAVELENGTH_VISIBLE:
              {
                if (ot==OBSTICAL_wall)
                {
                  stop = true;
                }
                else 
                {
                  if (x < 27)
                  {
                    int kkk=1;
                  };
                  pAURA->cells[y*MAX_TERRAIN_WIDTH+x] |= 1;  // AURA effects this cell
                  if (ot==OBSTICAL_occupied) 
                  {
                    stop = true;  // Occupied by combatant
                  };
                };
              };
              break;
            case AURA_WAVELENGTH_XRAY:
              {
                if (ot == OBSTICAL_wall) 
                {
                  stop = true;
                }
                else
                {
                  pAURA->cells[y*MAX_TERRAIN_WIDTH+x] |= 1;  // AURA effects this cell
                };
              };
              break;
            case AURA_WAVELENGTH_NEUTRINO:
              {
                pAURA->cells[y*MAX_TERRAIN_WIDTH+x] |= 1;  // AURA effects this cell
              };
              break;
            };
            if (stop) break;
          };

        };
        sx += dsx;
        sy += dsy;
        if (sx >= dsy)
        {
          sx -= dsy;
          x += dx;
          dist2 += rx + 1;
          rx += 2;
        };
        if (sy >= dsx)
        {
          sy -= dsx;
          y += dy;
          dist2 += ry + 1;
          ry += 2;
        };
      };
  };*/
}
#pragma warning(pop)

void COMBAT_DATA::DetermineNULLCoverage(AURA *pAURA)
{
  memset(pAURA->cells, 0, MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
}

void COMBAT_DATA::DetermineGlobalCoverage(AURA *pAURA)
{
  /* Really */ NotImplemented(0x321abe, false);
}

bool COMBAT_DATA::CheckAuraPlacement(AURA *pAURA, MOVE_DATA *pMoveData)
{
  // If AURA has moved, notify Hooks.
  bool recomputeAura = true;  
  if (pAURA->attachment[0] == pAURA->attachment[1])
  {
    if (   (    (pAURA->attachment[0] == AURA_ATTACH_COMBATANT)
             && (pAURA->combatantIndex[0] == pAURA->combatantIndex[1])
             && (pAURA->x[0] == m_aCombatants[pAURA->combatantIndex[0]].x)
             && (pAURA->y[0] == m_aCombatants[pAURA->combatantIndex[0]].y)
           )
        ||
           (    (pAURA->attachment[0] == AURA_ATTACH_COMBATANT_FACING)
             && (pAURA->combatantIndex[0] == pAURA->combatantIndex[1])
             && (pAURA->x[0]   == m_aCombatants[pAURA->combatantIndex[0]].x)
             && (pAURA->y[0]   == m_aCombatants[pAURA->combatantIndex[0]].y)
             && (pAURA->facing == m_aCombatants[pAURA->combatantIndex[0]].m_iMoveDir)
           )
        || (    (pAURA->attachment[0] == AURA_ATTACH_NONE)
           )
       )
    {
      if (
               (pAURA->shape[0]      == pAURA->shape[1])
            && (pAURA->size1[0]      == pAURA->size1[1])
            && (pAURA->size2[0]      == pAURA->size2[1])
            && (pAURA->size3[0]      == pAURA->size3[1])
            && (pAURA->size4[0]      == pAURA->size4[1])
            && (pAURA->spellID[0]    == pAURA->spellID[1])
            && (pAURA->wavelength[0] == pAURA->wavelength[1])
         )
      {
        if ((pMoveData==NULL) || (pAURA->wavelength[0] != AURA_WAVELENGTH_VISIBLE))
        {
          recomputeAura = false;
        };
      };
    };
  };
  if (recomputeAura)
  {
    if (pAURA->cells == NULL)
    {
      pAURA->cells = (unsigned char *)malloc(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
    };
    pAURA->attachment[0]      = pAURA->attachment[1];
    pAURA->shape[0]           = pAURA->shape[1];
    pAURA->size1[0]           = pAURA->size1[1];
    pAURA->size2[0]           = pAURA->size2[1];
    pAURA->size3[0]           = pAURA->size3[1];
    pAURA->size4[0]           = pAURA->size4[1];
    pAURA->x[0]               = pAURA->x[1];
    pAURA->y[0]               = pAURA->y[1];
    pAURA->wavelength[0]      = pAURA->wavelength[1];
    pAURA->spellID[0]         = pAURA->spellID[1];
    pAURA->combatantIndex[0]  = pAURA->combatantIndex[1];
    if (     (pAURA->attachment[0] == AURA_ATTACH_COMBATANT)
          || (pAURA->attachment[0] == AURA_ATTACH_COMBATANT_FACING)
      )
    {
      pAURA->x[0]      = m_aCombatants[pAURA->combatantIndex[0]].x;
      pAURA->y[0]      = m_aCombatants[pAURA->combatantIndex[0]].y;
      pAURA->facing    = m_aCombatants[pAURA->combatantIndex[0]].m_iMoveDir;
    };
    switch(pAURA->shape[0])
    {
        // cells[] <= 1 where AURA has effect.
    case AURA_SHAPE_NULL:           DetermineNULLCoverage(pAURA);    break;
    case AURA_SHAPE_GLOBAL:         DetermineGlobalCoverage(pAURA);  break;
    case AURA_SHAPE_ANNULARSECTOR:  DetermineAnnularCoverage(pAURA); break;
    default:
      /* Really */ NotImplemented(0x5dd9, false);
      DetermineNULLCoverage(pAURA);    
      break;
    };
    pAURA->RemoveSprite();
    SPELL_DATA *pSpell;
    pSpell = spellData.GetSpell(pAURA->spellID[1]);
    if (pSpell != NULL)
    {
      pAURA->AddSprite(pSpell);
    };
  };
  // Now we need to determine if any combatants have entered
  // or exited the AURA's effects.
  // Here is how we will proceed:
  //  For each combatant.
  //  if (combatant in AURA   &&   combatant not in m_combatants list)
  //  {
  //      ENTER(combatant)
  //      Add to list.
  //  }
  //  else if (combatatnt in m_combatants list   &&  combatant not in AURA)
  //  {
  //      EXIT(combatant)
  //      Remove from list
  //  }
  PushAuraReference(pAURA->auraID);
  {
    int i;
    for (i=0; i<m_iNumCombatants; i++)
    {
      int x, y, cellIndex;
      x = m_aCombatants[i].x;
      y = m_aCombatants[i].y;
      if (x < 0) continue;
      cellIndex = y*MAX_TERRAIN_WIDTH+x;
      {
        if (pAURA->cells[cellIndex] & 1)
        {
          // He is now in the AURA.
          int j;
          for (j=0; j<pAURA->m_combatants.GetSize(); j++)
          {
            if (pAURA->m_combatants[j] == i) break;
          }
          if (j == pAURA->m_combatants.GetSize())
          {
            // He moved into the AURA.
            HOOK_PARAMETERS hookParameters;
            SCRIPT_CONTEXT scriptContext;
            scriptContext.SetCombatantContext(&m_aCombatants[i]);
            pAURA->RunAuraScripts(
                             AURA_ENTER, 
                             ScriptCallback_RunAllScripts,
                             NULL,
                             "Aura moved and now includes combatant");
            pAURA->m_combatants.Add(i);
          };
        }
        else
        {
          // He is not now in the AURA
          int j;
          for (j=0; j<pAURA->m_combatants.GetSize(); j++)
          {
            if (pAURA->m_combatants[j] == i) break;
          };
          if (j < pAURA->m_combatants.GetSize())
          {
            // He moved ou of the AURA
            HOOK_PARAMETERS hookParameters;
            SCRIPT_CONTEXT scriptContext;
            scriptContext.SetCombatantContext(&m_aCombatants[i]);
            pAURA->RunAuraScripts(
                               AURA_EXIT,
                               ScriptCallback_RunAllScripts,
                               NULL,
                               "Aura moved and now excludes combatant");
            pAURA->m_combatants.RemoveAt(j);
          }
        };
      };
    };
  };
  PopAuraReference();
  return false;
}

void COMBAT_DATA::CheckAllAuraPlacements(MOVE_DATA *pMoveData)
{
  POSITION pos;
  bool redraw;
  MOVE_DATA *pmd;
  for (pos=m_auras.GetHeadPosition(); pos!=NULL;)
  {
    AURA *pAURA;
    pmd = pMoveData;
    pAURA = m_auras.GetNext(pos);
    do 
    {
      // Why do we need to call 'CheckAuraPlacement' more than once?
      //
      redraw = CheckAuraPlacement(pAURA, pmd);
      pmd = NULL;
    } while (redraw);
  };
}

int COMBAT_DATA::CreateAura(const CString& s1,
                            const CString& s2,
                            const CString& s3,
                            const CString& s4,
                            const CString& s5)
{
  AURA newAura;
  int i;
  int auraID;
  POSITION pos;
  AURA *pAURA;
  pos = m_auras.AddTail(newAura);
  pAURA = m_auras.GetAt(pos);
  auraID = m_nextAuraID++;
  pAURA->auraID = auraID;
  pAURA->specialAbilities.InsertAbility(s1,s2,"$AURA_Create", s3);
  pAURA->userData[0] = s3;
  pAURA->userData[1] = s4;
  pAURA->userData[2] = s5;
  pAURA->cells = (unsigned char *)malloc(MAX_TERRAIN_WIDTH * MAX_TERRAIN_HEIGHT);
  memset(pAURA->cells, 0, MAX_TERRAIN_WIDTH * MAX_TERRAIN_HEIGHT);     
  pAURA->m_combatants.RemoveAll(); // Not necessary...a comment really
  for (i=0; i<2; i++)
  {
    pAURA->size1[i] = 0;
    pAURA->size2[i] = 0;
    pAURA->size3[i] = 0;
    pAURA->size4[i] = 0;
    pAURA->wavelength[i] = AURA_WAVELENGTH_VISIBLE;
    pAURA->shape[i] = AURA_SHAPE_NULL;
    pAURA->spellID[i] = "";
    pAURA->attachment[i] = AURA_ATTACH_NONE;
    pAURA->combatantIndex[i] = 0;
  };
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    combatData.PushAuraReference(pAURA->auraID);
    pAURA->RunAuraScripts(AURA_CREATE, 
                          ScriptCallback_RunAllScripts,
                          NULL,
                          "Newly created Aura");
    while (combatData.CheckAuraPlacement(pAURA, NULL)){};  // If it moved we need to notify Hooks.
    combatData.PopAuraReference();

  }
  return auraID; // May be gone!!!
}

AURA::~AURA(void)
{
  {
    if (cells != NULL) 
    {
      free(cells);
      cells = NULL;
    };
//    if (pSprite != NULL)
//    {
//      RemoveAnimationFromList(auraID);
//      delete pSprite;
//      pSprite = NULL;
//    };
  };
}

void AURA::AddSprite(SPELL_DATA *pSpell)
{
  int xPos, yPos, indx;
  indx = 0;
  for (yPos =0; yPos<MAX_TERRAIN_HEIGHT; yPos++)
  {
    for (xPos =0; xPos<MAX_TERRAIN_WIDTH; xPos++)
    {
      if (cells[indx++] & 1)
      {
        COMBAT_SPRITE_DATA *pSprite;
        pSprite = new COMBAT_SPRITE_DATA;
        pSprite->Init(pSpell->TargLingerArt, "", xPos, yPos);
        pSprite->activeSpellKey = auraID;
        AnimateList.AddTail(pSprite);
      };
    };
  };
}
        
void AURA::RemoveSprite(void)
{
  RemoveAnimationFromList(auraID);
}


void AURA::operator =(const AURA& src)
{
  int i;
  for (i=0; i<10; i++)
  {
    userData[i] = src.userData[i];
  };
  auraID = src.auraID;
  specialAbilities.Clear(); // = src.specialAbilities;
  if (    (src.specialAbilities.GetHeadPosition() != NULL)
      ||  (cells != NULL))
  {
    // Should not be copying AURAs!
    die("AURA::Operator ="); //Not Implemented(0x3ee34, false);
  };
  cells = NULL;
}

#endif