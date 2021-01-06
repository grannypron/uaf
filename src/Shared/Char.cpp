/******************************************************************************
* Filename: Char.cpp
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

#ifdef UAFEDITOR
//#include "..\UAFWinEd\UAFWinEd.h"
#include "..\UAFWinEd\resource.h"
#include "class.h"
#include "..\UAFWinEd\CrossReference.h"
#else
#include "Externs.h"

// prs 20191221
#include "GlobalData.h"


#include "..\UAFWin\Dungeon.h"
#include "GameEvent.h"
#include "..\UAFWin\disptext.h"
#include "..\UAFWin\FormattedText.h"
#include "..\UAFWin\Combatant.h"
#endif

#include "items.h"
#include "char.h"
#include "monster.h"
#include "class.h"

#include "GlobalData.h"
#include "traits.h"
#include "GameRules.h"

#ifdef UAFEngine
#include "RunTimeIF.h"
#include "..\UAFWin\FormattedText.h"
#include <math.h>
#include "GPDLexec.h"
#include "GPDLComp.h"
#endif

#include "RunTimeIF.h"
//#include "ProjectVersion.h"


extern BOOL logDebuggingInfo;
extern int setPartyXY_x;
extern CHARACTER FakeCharacter;
BOOL MyCopyFile(LPCSTR src, LPCSTR dest, BOOL failIfExists);
#ifdef UAFEngine
void die(int);
#endif

double ModifyByDouble(CHARACTER *pChar, IF_KEYWORD_INDEX ifKey, double modification);

extern const CString Skill_PickPockets;
extern const CString Skill_OpenLocks;
extern const CString Skill_FindTraps;
extern const CString Skill_MoveSilent;
extern const CString Skill_HideInShadows; 
extern const CString Skill_RangerBonusLevel;
extern const CString Skill_BackstabMultiplier;
extern const CString Skill_MaxLevel;

extern const CString Ability_Dexterity;
extern const CString Ability_Constitution;
extern const CString Ability_Strength;
extern const CString Ability_Intelligence;
extern const CString Ability_Charisma;
extern const CString Ability_Wisdom;

extern const CString Save_Vs_PPDM;
extern const CString Save_Vs_PP;
extern const CString Save_Vs_RSW;
extern const CString Save_Vs_BR;
extern const CString Save_Vs_SP;

extern const CString SA_XP_Value;

extern const int CHARACTER_VERSION;
extern const double ENGINE_VER;
extern const double VersionSpellNames;


COMBATANT *GetCombatant(ActorType *pActor);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_SERIAL( CHARACTER, CObject, 1 )
//IMPLEMENT_SERIAL( CHAR_LIST, CObject, 1 )

CHARACTER character;

#ifdef _DEBUG // debug only diagnostic
//#define DUMP_CHAR_SPELL_ATTS 1
#endif

//*****************************************************************************
// NAME:    addSpecialItemsToItemList
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
#ifdef UAFEngine
void addSpecialItemsToItemList(ITEM_TEXT_LIST &list)
{
  POSITION pos;
  pos = globalData.specialItemData.GetHeadPosition();
  while (pos != NULL)
  {
    if (party.hasSpecialItem(globalData.specialItemData.PeekAtPos(pos).id))
    {
      CString name;
      if (EditorMode()) // add diagnostic data if testing design from editor
        name.Format("%s: stage %u", 
                    GetSpecialItemName(globalData.specialItemData.PeekAtPos(pos).id),
                    globalData.specialItemData.PeekAtPos(pos).stage);
      else
        name = GetSpecialItemName(globalData.specialItemData.PeekAtPos(pos).id);

      list.Add(1,
               SpecialItemType,
               name,
               globalData.specialItemData.PeekAtPos(pos).id);
    }
    globalData.specialItemData.GetNext(pos);
  }

  pos = globalData.keyData.GetHeadPosition();
  while (pos != NULL)
  {
    if (party.hasSpecialKey(globalData.keyData.PeekAtPos(pos).id))
    {
      CString name;
      if (EditorMode()) // add diagnostic data if testing design from editor
        name.Format("%s: stage %u", 
                    GetSpecialKeyName(globalData.keyData.PeekAtPos(pos).id),
                    globalData.keyData.PeekAtPos(pos).stage);
      else
        name = GetSpecialKeyName(globalData.keyData.PeekAtPos(pos).id);

      list.Add(1,
               SpecialKeyType,
               name,
               globalData.keyData.PeekAtPos(pos).id);
    }
    globalData.keyData.GetNext(pos);
  }

  // add diagnostic data if testing design from editor.
  // quests only show up in inventory while testing design.
  if (EditorMode())
  {
    pos = globalData.questData.GetHeadPosition();
    while (pos != NULL)
    {
      if (globalData.questData.IsPresent(globalData.questData.PeekAtPos(pos).id))
      {
        CString name;
        name.Format("%s: stage %u", 
                    globalData.questData.PeekAtPos(pos).name,
                    globalData.questData.PeekAtPos(pos).stage);
        list.Add(1,
                 QuestType,
                 name,
                 globalData.questData.PeekAtPos(pos).id);
      }
      globalData.questData.GetNext(pos);
    }
  }

  ItemsOnPage = list.GetCount();
	ItemsOnPage = min(ItemsOnPage, Items_Per_Page);
}

#endif
//*****************************************************************************
// NAME:    tradeItemMoney
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void tradeItemMoney(itemClassType type, int qty, CHARACTER& giver, CHARACTER& taker)
{
  if (!itemIsMoney(type))
  {
    WriteDebugString("Wrong item type passed to tradeItemMoney()\n");
    return;
  }

  if ((qty + taker.GetEncumbrance()) > taker.GetAdjMaxEncumbrance())
  {
		SetMiscError(TooMuchWeight);
    return;
  }
  int i;
  switch (type) 
  {
  case GemType:
    {
      i=0;
      while ( i < qty )
      {
        POSITION pos = giver.money.Gems.GetHeadPosition();
        if (pos != NULL)
        {
          GEM_TYPE gem = giver.money.Gems.GetAt(pos);
          giver.money.RemoveGem(gem.id);
          taker.money.AddGem(gem);
        };
        i++;
      }
    }
    break;
  case JewelryType:
    {
      i=0;
      while ( i < qty)
      {
        POSITION pos = giver.money.Jewelry.GetHeadPosition();
        if (pos != NULL)
        {
          GEM_TYPE gem = giver.money.Jewelry.GetAt(pos);
          giver.money.RemoveJewelry(gem.id);
          gem.id = -1;
          taker.money.AddJewelry(gem);
        }        ;
        i++;
      };
    }
    break;
  default:
    taker.money.Add(type, qty);
    giver.money.Subtract(type, qty);
    break;
  }

 	taker.SetEncumbrance(taker.determineEffectiveEncumbrance());
  giver.SetEncumbrance(giver.determineEffectiveEncumbrance());
	taker.determineMaxMovement();
  giver.determineMaxMovement();
}

//*****************************************************************************
// NAME:    tradeItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//void tradeItem(GLOBAL_ITEM_ID giID, int qty, int charges, int id, int g_index, CHARACTER& giver, CHARACTER& taker)
void tradeItem(const ITEM_ID& itemID, int qty, int charges, int id, int g_index, CHARACTER& giver, CHARACTER& taker)
{
  if (giver.uniquePartyID==taker.uniquePartyID)
  {
    // trade with self (form of inventory re-arrangement by player)
    giver.delCharacterItem(g_index, qty);
    taker.addCharacterItem(itemID, qty, charges, id, giver.myItems.GetPurchasePrice(g_index));
  }
  else
  {
    if (taker.addCharacterItem(itemID, qty, charges, id, giver.myItems.GetPurchasePrice(g_index)))
      giver.delCharacterItem(g_index, qty);
  }
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************


//*****************************************************************************
//    NAME: BlockageDataType::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void BlockageDataType::Serialize(CArchive &ar)
{
  if (ar.IsStoring())
    ar << level << x << y << Stats.StatsFull;
  else
    ar >> level >> x >> y >> Stats.StatsFull;
}

//*****************************************************************************
//    NAME: BlockageDataType::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void BlockageDataType::Serialize(CAR &ar)
{
  if (ar.IsStoring())
  {
    ar << level;
    ar << x;
    ar << y;
    ar << Stats.StatsFull;
  }
  else
  {
    ar >> level; 
    ar >> x;
    ar >> y;
    ar >> Stats.StatsFull;
  }
}

#ifdef UAFEDITOR
extern const char *JKEY_LEVEL;
extern const char *JKEY_XY;
const char *JKEY_STATS = "stats";

void BlockageDataType::Export(JWriter& jw)
{
  int i[2];
  jw.StartList();
  jw.Linefeed(false);
  jw.NameAndValue(JKEY_LEVEL, level);
  i[0] = x;
  i[1] = y;
  jw.NameAndValue(JKEY_XY, i, 2);
  jw.NameAndValue(JKEY_STATS, Stats.StatsFull);
  jw.EndList();
  jw.Linefeed(true);
}
void BlockageDataType::Import(JReader& jr)
{
  int i[2];
  jr.StartList();
  jr.Linefeed(false);
  jr.NameAndValue(JKEY_LEVEL, level);
  jr.NameAndValue(JKEY_XY, i, 2);
  x = i[0];
  y = i[1];
  jr.NameAndValue(JKEY_STATS, Stats.StatsFull);
  jr.EndList();
  jr.Linefeed(true);
}
#endif


//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::operator==
//
// PURPOSE: 
//
//*****************************************************************************
bool BLOCKAGE_STATUS::operator==(const BLOCKAGE_STATUS& src)
{
  if (data.GetCount() != src.data.GetCount()) return false;
  POSITION pos = data.GetHeadPosition();
  bool success = true;
  while (pos != NULL)
  {
    POSITION spos = src.data.GetHeadPosition();
    bool found = false;
    while (spos != NULL)
    {
      if (src.data.PeekAt(spos)->Stats.StatsFull == data.PeekAt(pos)->Stats.StatsFull)
      {
        found = true;
        break;
      }
      src.data.PeekNext(spos);
    }
    if (!found)
    {
      success = false;
      break;
    }
    data.GetNext(pos);
  }
  return success;
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::operator=
//
// PURPOSE: 
//
//*****************************************************************************
BLOCKAGE_STATUS& BLOCKAGE_STATUS::operator=(const BLOCKAGE_STATUS& src) 
{ 
  if (&src == this)
    return *this;

  Clear();

  POSITION pos = src.data.GetHeadPosition();
  BlockageDataType temp;

  while (pos != NULL)
  {
    temp = *src.data.PeekNext(pos);
    data.AddTail(temp);
  }  
  return *this; 
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void BLOCKAGE_STATUS::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);
  POSITION pos;

  if (ar.IsStoring())  
  {
    ar << data.GetCount();
    pos = data.GetHeadPosition();
    while (pos != NULL)
    {
      data.GetAt(pos)->Serialize(ar);
      data.GetNext(pos);
    }
  }
  else
  {
    BlockageDataType temp;
    int count;
    ar >> count;

    for (int i=0;i<count;i++)
    {
       temp.Serialize(ar);
       data.AddTail(temp);
    }
  }
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void BLOCKAGE_STATUS::Serialize(CAR &ar)
{
  POSITION pos;
  if (ar.IsStoring())  
  {
    ar << data.GetCount();
    pos = data.GetHeadPosition();
    while (pos != NULL)
    {
      data.GetAt(pos)->Serialize(ar);
      data.GetNext(pos);
    }
  }
  else
  {
    BlockageDataType temp;
    int count;
    ar >> count;

    for (int i=0;i<count;i++)
    {
       temp.Serialize(ar);
       data.AddTail(temp);
    }
  }
}

#ifdef UAFEDITOR

const char *JKEY_BLOCKAGESTATUS = "blockageStatus";
void BLOCKAGE_STATUS::Export(JWriter& jw)
{
  POSITION pos;
  pos = data.GetHeadPosition();
  jw.StartArray(JKEY_BLOCKAGESTATUS);
  while (pos != NULL)
  {
    jw.NextEntry();
    data.GetAt(pos)->Export(jw);
    data.GetNext(pos);
  };
  jw.EndArray();
}
void BLOCKAGE_STATUS::Import(JReader& jr)
{
  POSITION pos;
  pos = data.GetHeadPosition();
  jr.StartArray(JKEY_BLOCKAGESTATUS);
  while (jr.Optional(), jr.NextEntry())
  {
    BlockageDataType bdt;
    bdt.Import(jr);
    data.AddTail(bdt);
  };
  jr.EndArray();
}
#endif

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::FindData
//
// PURPOSE: 
//
//*****************************************************************************
POSITION BLOCKAGE_STATUS::FindData(int level, int x, int y)
{
  POSITION pos = data.GetHeadPosition();
  BOOL found = FALSE;
  while ((pos != NULL) && (!found))
  {
    BlockageDataType temp = *data.PeekAt(pos);
    if (   (temp.x == x)
        && (temp.y == y)
        && (temp.level == level))
    {
      found = TRUE;
    }
    else
      data.GetNext(pos);
  }
  if (!found)
    pos = NULL;
  return pos;
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::IsSecret
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BLOCKAGE_STATUS::IsSecret(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL)
  {
    switch (facing)
    {
    case FACE_NORTH:
      return (data.PeekAt(pos)->Stats.flags.NorthSecret > 0); 
    case FACE_EAST:
      return (data.PeekAt(pos)->Stats.flags.EastSecret > 0); 
    case FACE_SOUTH:
      return (data.PeekAt(pos)->Stats.flags.SouthSecret > 0); 
    case FACE_WEST:
      return (data.PeekAt(pos)->Stats.flags.WestSecret > 0); 
    }
  }

  // not found means party has not cleared secret bit for this spot yet
  return TRUE;
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::ClearSecret
//
// PURPOSE: 
//
//*****************************************************************************
void BLOCKAGE_STATUS::ClearSecret(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL) // already have data for this spot
  {
    switch (facing)
    {
    case FACE_NORTH:
      data.GetAt(pos)->Stats.flags.NorthSecret = 0;
      break;
    case FACE_EAST:
      data.GetAt(pos)->Stats.flags.EastSecret = 0; 
      break;
    case FACE_SOUTH:
      data.GetAt(pos)->Stats.flags.SouthSecret = 0; 
      break;
    case FACE_WEST:
      data.GetAt(pos)->Stats.flags.WestSecret = 0; 
      break;    
    }
  }
  else // add data for this spot with secret bit clear
  {
    BlockageDataType temp; // all bits default to 1
    temp.level = level;
    temp.x = x;
    temp.y = y;
    switch (facing) // clear secret bit
    {
    case FACE_NORTH:
      temp.Stats.flags.NorthSecret = 0;
      break;
    case FACE_EAST:
      temp.Stats.flags.EastSecret = 0; 
      break;
    case FACE_SOUTH:
      temp.Stats.flags.SouthSecret = 0; 
      break;
    case FACE_WEST:
      temp.Stats.flags.WestSecret = 0; 
      break;    
    }
    data.AddTail(temp);
  }
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::IsLocked
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BLOCKAGE_STATUS::IsLocked(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL)
  {
    switch (facing)
    {
    case FACE_NORTH:
      return (data.PeekAt(pos)->Stats.flags.NorthLocked > 0); 
    case FACE_EAST:
      return (data.PeekAt(pos)->Stats.flags.EastLocked > 0); 
    case FACE_SOUTH:
      return (data.PeekAt(pos)->Stats.flags.SouthLocked > 0); 
    case FACE_WEST:
      return (data.PeekAt(pos)->Stats.flags.WestLocked > 0); 
    }
  }
  return TRUE;
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::ClearLocked
//
// PURPOSE: 
//
//*****************************************************************************
void BLOCKAGE_STATUS::ClearLocked(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL) // already have data for this spot
  {
    switch (facing)
    {
    case FACE_NORTH:
      data.GetAt(pos)->Stats.flags.NorthLocked = 0;
      break;
    case FACE_EAST:
      data.GetAt(pos)->Stats.flags.EastLocked = 0; 
      break;
    case FACE_SOUTH:
      data.GetAt(pos)->Stats.flags.SouthLocked = 0; 
      break;
    case FACE_WEST:
      data.GetAt(pos)->Stats.flags.WestLocked = 0; 
      break;    
    }
  }
  else // add data for this spot with locked bit clear
  {
    BlockageDataType temp; // all bits default to 1
    temp.level = level;
    temp.x = x;
    temp.y = y;
    switch (facing) // clear locked bit
    {
    case FACE_NORTH:
      temp.Stats.flags.NorthLocked = 0;
      break;
    case FACE_EAST:
      temp.Stats.flags.EastLocked = 0; 
      break;
    case FACE_SOUTH:
      temp.Stats.flags.SouthLocked = 0; 
      break;
    case FACE_WEST:
      temp.Stats.flags.WestLocked = 0; 
      break;    
    }
    data.AddTail(temp);
  }
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::IsSpelled
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BLOCKAGE_STATUS::IsSpelled(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL)
  {
    switch (facing)
    {
    case FACE_NORTH:
      return (data.PeekAt(pos)->Stats.flags.NorthSpelled > 0); 
    case FACE_EAST:
      return (data.PeekAt(pos)->Stats.flags.EastSpelled > 0); 
    case FACE_SOUTH:
      return (data.PeekAt(pos)->Stats.flags.SouthSpelled > 0); 
    case FACE_WEST:
      return (data.PeekAt(pos)->Stats.flags.WestSpelled > 0); 
    }
  }
  return TRUE;
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::IsSpelledSecret
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BLOCKAGE_STATUS::IsSpelledSecret(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL)
  {
    switch (facing)
    {
    case FACE_NORTH:
      return    (data.PeekAt(pos)->Stats.flags.NorthSpelled > 0) 
             || (data.PeekAt(pos)->Stats.flags.NorthSecret > 0); 
    case FACE_EAST:
      return    (data.PeekAt(pos)->Stats.flags.EastSpelled > 0) 
             || (data.PeekAt(pos)->Stats.flags.EastSecret > 0); 
    case FACE_SOUTH:
      return    (data.PeekAt(pos)->Stats.flags.SouthSpelled > 0) 
             || (data.PeekAt(pos)->Stats.flags.SouthSecret > 0); 
    case FACE_WEST:
      return    (data.PeekAt(pos)->Stats.flags.WestSpelled > 0) 
             || (data.PeekAt(pos)->Stats.flags.WestSecret > 0); 
    }
  }
  return TRUE;
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::IsLockedSecret
//
// PURPOSE: 
//
//*****************************************************************************
BOOL BLOCKAGE_STATUS::IsLockedSecret(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL)
  {
    switch (facing)
    {
    case FACE_NORTH:
      return    (data.PeekAt(pos)->Stats.flags.NorthSecret > 0) 
             || (data.PeekAt(pos)->Stats.flags.NorthLocked > 0); 
    case FACE_EAST:
      return    (data.PeekAt(pos)->Stats.flags.EastSecret > 0) 
             || (data.PeekAt(pos)->Stats.flags.EastLocked > 0); 
    case FACE_SOUTH:
      return    (data.PeekAt(pos)->Stats.flags.SouthSecret > 0) 
             || (data.PeekAt(pos)->Stats.flags.SouthLocked > 0); 
    case FACE_WEST:
      return    (data.PeekAt(pos)->Stats.flags.WestSecret > 0) 
             || (data.PeekAt(pos)->Stats.flags.WestLocked > 0); 
    }
  }
  return TRUE;
}

//*****************************************************************************
//    NAME: BLOCKAGE_STATUS::ClearSpelled
//
// PURPOSE: 
//
//*****************************************************************************
void BLOCKAGE_STATUS::ClearSpelled(int level, int x, int y, int facing)
{ 
  POSITION pos = FindData(level, x, y);

  if (pos != NULL) // already have data for this spot
  {
    switch (facing)
    {
    case FACE_NORTH:
      data.GetAt(pos)->Stats.flags.NorthSpelled = 0;
      break;
    case FACE_EAST:
      data.GetAt(pos)->Stats.flags.EastSpelled = 0; 
      break;
    case FACE_SOUTH:
      data.GetAt(pos)->Stats.flags.SouthSpelled = 0; 
      break;
    case FACE_WEST:
      data.GetAt(pos)->Stats.flags.WestSpelled = 0; 
      break;    
    }
  }
  else // add data for this spot with spelled bit clear
  {
    BlockageDataType temp; // all bits default to 1
    temp.level = level;
    temp.x = x;
    temp.y = y;
    switch (facing) // clear spelled bit
    {
    case FACE_NORTH:
      temp.Stats.flags.NorthSpelled = 0;
      break;
    case FACE_EAST:
      temp.Stats.flags.EastSpelled = 0; 
      break;
    case FACE_SOUTH:
      temp.Stats.flags.SouthSpelled = 0; 
      break;
    case FACE_WEST:
      temp.Stats.flags.WestSpelled = 0; 
      break;    
    }
    data.AddTail(temp);
  }
}


int SPELL_ABILITY::LocateSchoolAbility(const SCHOOL_ID& schoolID) const
{
  int i, n;
  n = schoolAbilities.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekSchoolAbility(i)->schoolID == schoolID) return i;
  }
  return -1;
}


void SPELL_ABILITY::operator =(const SPELL_ABILITY& src)
{
  valid = src.valid;
  schoolAbilities.SetSize(src.schoolAbilities.GetSize());
  schoolAbilities.Copy(src.schoolAbilities);
};



//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//
//
//  CHARACTER functions
//
//

int CHARACTER::LocateBaseclassStats(const BASECLASS_ID& baseclassID) const
{
  int i, n;
  n = GetBaseclassStatsCount();
  for (i=0; i<n; i++)
  {
    if (PeekBaseclassStats(i)->baseclassID == baseclassID) return i;
  };
  return -1;
}

int CHARACTER::LocateSkillAdj(const CString& skillName, const CString& adjName) const
{
  int i, n;
  n = GetSkillAdjCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJ *ps;
    ps = PeekSkillAdj(i);
    if (ps->skillID == skillName)
    {
      if (ps->adjID == adjName)
      {
        return i;
      };
    };
  }
  return -1;
}

//void CHARACTER::ApplyTempSkillAdjustments(const CString& skillID, int *skillValue) const
void CHARACTER::ApplyTempSkillAdjustments(SKILL_COMPUTATION& SC) const
{
  int i, n;
  char adjType;
  double adj;
  n = GetSkillAdjCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJ *ps;
    ps = PeekSkillAdj(i);
    if (ps->skillID == SC.skillID)
    {
      adjType = ps->type;
      switch (adjType)
      {
      case '%':  adj = (SC.baseVal * ps->value + 50) / 100.0 - SC.baseVal; break;
      case '+':  adj = ps->value;                                          break;
      case '*':  adj = SC.baseVal*(ps->value - 1.0);                       break;
      case '-':  adj = -ps->value;                                         break; 
      case '=':  adj = ps->value - SC.baseVal;                             break;
      default:
        {
          if (!debugStrings.AlreadyNoted("CATSAIA"))
          {
            CString msg;
            msg.Format("CHARACTER::ApplyTempSkillAdjustments - Illegal adjType= %c", adjType);
            MsgBoxInfo(msg);
            WriteDebugString(msg);
          };
          return;
        };
      };
      if (SC.tempAdj == NoSkillAdj) SC.tempAdj = 0.0;
      SC.tempAdj += adj;
    };
  };
}

void CHARACTER::DeleteSkillAdj(int indx) 
{
  skillAdjustments.RemoveAt(indx);
}

int CHARACTER::InsertSkillAdj(const CString& skillName, const CString& adjName, char saType, int value) 
{
  SKILL_ADJ skillAdj;
  int i, n;
  skillAdj.skillID = skillName;
  skillAdj.adjID = adjName;
  skillAdj.type = saType;
  skillAdj.value = value;
  n = GetSkillAdjCount();
  for (i=0; i<n; i++)
  {
    if (*PeekSkillAdj(i) > skillAdj)
    {
      skillAdjustments.InsertAt(i,skillAdj);
      return i;
    };
  };
  skillAdjustments.Add(skillAdj);
  return n;
}

const RACE_DATA *CHARACTER::PeekRaceData(void) const
{
  const RACE_DATA *pRace;
  pRace = raceData.PeekRace(race);
  if (pRace != NULL) return pRace;
  if (this == &FakeCharacter) return raceData.PeekRace(0);
  if (!debugStrings.AlreadyNoted("CHARPRD"))
  {
    CString msg;
    int i, n;
    n = raceData.GetCount();
    msg.Format("Character '%s' has an illegal race.\nRace : %s\nAvailable races are:\n",
                name, race);
    for(i=0; i<n; i++)
    {
      if (i >= 12)
      {
        msg += ". . . .etc.";
        break;
      };
      msg += raceData.PeekRace(i)->Name();
      if (i < n-1) msg += ",\n";
    };
    MsgBoxInfo(msg, "ALERT");
  };
  return NULL;
}


/*
//double CHARACTER::UpdateSkillValue(const SKILL_ID& skillID, double val) const
void CHARACTER::UpdateSkillValuex(SKILL_COMPUTATION& SC) const
{
  SC.tempAdj = 0;
  die("Not Implemented"); // compute adjustment
  //ApplyTempSkillAdjustments(skillID, &value);
  ApplyTempSkillAdjustments(SC);
}
*/
int CHARACTER::GetAdjSkillValue(const CString& skillName, bool minimize, bool includeTempAdj) const
{
  SKILL_COMPUTATION SC(this, skillName, minimize, includeTempAdj);
  //if (::GetAdjSkillValue(skillID, this, NULL, NULL, &result, minimize))
  ::GetAdjSkillValue(SC);
  return SC.finalAdjustedValue==NoSkillAdj?NoSkill:SC.finalAdjustedValue+0.5;
}



int CHARACTER::GetLevelCap(const BASE_CLASS_DATA *pBaseclass) const
{
  SKILL_COMPUTATION SC(this, Skill_MaxLevel, true, true);
  //Limit baseclass search to this one baseclass.


  SC.baseclassID = pBaseclass->BaseclassID();
// Should already have been done in SC constructor   SC.pRace = raceData.PeekRace(this->race);
  ::GetAdjSkillValue(SC);
  if (SC.finalAdjustedValue == NoSkillAdj) return NoSkill;
  return SC.finalAdjustedValue + 0.5;
  //GetAdjSkillValue();
/*
// the old way.  Why did we do it this way?
  int levelCap, maxLevelBaseclass, maxLevelRace;
  // See if this character has any MaxLevel skills defined.
  // First, find the smallest definition of MaxLevel in this baseclass and race.
  const RACE_DATA *pRace;
  SKILL_COMPUTATION SC;
  SC.skillID = Skill_MaxLevel;
  SC.pBaseclass = pBaseclass;
  maxLevelBaseclass = SC.pBaseclass->GetSkillValue(SC);
  levelCap = maxLevelBaseclass;  // Perhaps == NoSkill
  pRace = raceData.PeekRace(RaceID());
  if (pRace != NULL)
  {
    maxLevelRace = pRace->GetSkillValue(SC);
    if (maxLevelRace != NoSkill)
    {  
      if (    (maxLevelRace < levelCap) 
           || (levelCap == NoSkill))
      {
        levelCap = maxLevelRace;
      };
    };
  };
  // Now we can make any required adjustments.
  double baseVal;
  //double bestRaceAdj, bestBaseclassAdj, abilityAdj, tempAdj;
  SC.baseclassID = pBaseclass->BaseclassID();
  baseVal = levelCap;
  //bestRaceAdj = bestBaseclassAdj = NoSkillAdj;
  abilityData.ClearAdjustments(NoSkillAdj);
  //pRace->UpdateSkillValue(this, 
  //                        skillMaxLevel,
  //                        &baseclassID,
  //                        baseVal, 
  //                        &bestRaceAdj, 
  //                        &bestBaseclassAdj,
  //                        true);
  pRace->UpdateSkillValue(SC); 
  //pBaseclass->UpdateSkillValue(this, 
  //                             skillMaxLevel, 
  //                             baseVal, 
  //                             &bestRaceAdj, 
  //                             &bestBaseclassAdj,
  //                             true);
  SC.pBaseclass->UpdateSkillValue(SC);
  SC.tempAdj = NoSkillAdj;
  this->UpdateSkillValue(SC);
  SC.abilityAdj = abilityData.TotalAdjustments(NoSkillAdj);
  if (levelCap == NoSkill)
  {
    if (     (SC.bestBaseclassAdj != NoSkillAdj)
         ||  (SC.bestRaceAdj != NoSkillAdj) 
         ||  (SC.tempAdj != NoSkillAdj)
         ||  (SC.abilityAdj != NoSkillAdj)  ) levelCap = 0;
  };
  if (SC.bestBaseclassAdj != NoSkillAdj) levelCap += SC.bestBaseclassAdj;
  if (SC.bestRaceAdj != NoSkillAdj)      levelCap += SC.bestRaceAdj;
  if (SC.abilityAdj != NoSkillAdj)       levelCap += SC.abilityAdj;
  if (SC.tempAdj != NoSkillAdj)          levelCap += SC.tempAdj;
  return levelCap;
*/
}



//*****************************************************************************
//    NAME: CHARACTER::HaveSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL CHARACTER::HaveSpell(GLOBAL_SPELL_ID spelldbkey, BOOL checkmemorized) const
BOOL CHARACTER::HaveSpell(const SPELL_ID& spellID, BOOL checkmemorized) const
{
  //if (!spelldbkey.IsValidSpell()) return FALSE;
  if (!spellID.IsValidSpell()) return FALSE;
/*
  POSITION pos = m_spellbook.GetHeadPosition();
  while (pos != NULL)
  {
    //if (m_spellbook.PeekAtPos(pos).spell == spelldbkey)
    if (m_spellbook.PeekAtPos(pos).spellID == spellID)
    {
      if (checkmemorized)
        return (m_spellbook.PeekAtPos(pos).IsMemorized());
      else
        return TRUE;
    }
    m_spellbook.PeekNext(pos);
  }
  return FALSE;
*/
  const CHARACTER_SPELL *pCS;
  pCS = m_spellbook.PeekCharacterSpell(spellID);
  if (pCS == NULL) return FALSE;
  return pCS->IsMemorized();
}

//bool CHARACTER::HasKnowableSpell(const CString& uniqueName) const
//{
//  die(0x0cecdae);
//  return false;
//}

bool CHARACTER::AddKnowableSpell(const CString& uniqueName)
{
  CString list;
  CString entry;
  SPELL_ID spellID;
  spellID = uniqueName;
  // If already known, don't add it to $KnowableSpells$
  if (GetSpellBookIndex(spellID, false)) return false;
  entry = CString("?") + uniqueName;
  list = char_asl.Lookup("$KnowableSpells$");
  if (list.IsEmpty())
  {
    list = entry;
  }
  else
  {
    if (list.Find(entry) >= 0)
    {
      return false;
    }
    else 
    {
      list = list + entry;
    };
  };
  char_asl.Insert("$KnowableSpells$", list, ASLF_MODIFIED);
  return true;
}

bool CHARACTER::ClrKnowableSpell(void)
{
  char_asl.Delete("$KnowableSpells$");
  return false;
}

bool CHARACTER::DelKnowableSpell(const CString& spellName)
{
  CString list, str;
  bool result = false;
  str = "?" + spellName;
  list = char_asl.Lookup("$KnowableSpells$");
  if (list.GetLength() < str.GetLength()) return false;
  if (list.Right(str.GetLength()) == str)
  {
    list = list.Left(list.GetLength() - str.GetLength());
    result = true;
  }
  else
  {
    int n;
    str += "?";
    n = list.Find(str);
    if (n >= 0)
    {
      list = list.Left(n) + list.Right(list.GetLength() - n - str.GetLength() + 1);
      result = true;
    };
  };
  if (result)
  {
    char_asl.Insert("$KnowableSpells$", list, ASLF_MODIFIED);
  };
  return result;
}

void CHARACTER::CreateAllowedSchoolList(SCHOOL_LIST *pSchoolList) const
{
  const CLASS_DATA *pClass;
  pClass = classData.PeekClass(classID);
  pSchoolList->Clear();
  if (pClass != NULL) pClass->CreateAllowedSchoolList(pSchoolList);
}



//bool CHARACTER::DelKnowableSpell(const CString& uniqueName)
//{
//  die(0xce3ce4);
//  return false;
//}

//*****************************************************************************
//    NAME: CHARACTER::GetSpellBookKey
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID CHARACTER::GetSpellBookKey(GLOBAL_SPELL_ID gsID, BOOL checkmemorized)
//GLOBAL_SPELL_ID CHARACTER::GetSpellBookKey(const CString& spellName, BOOL checkmemorized)
int CHARACTER::GetSpellBookIndex(const SPELL_ID& spellID, BOOL checkmemorized) const
{
  //if (!gsID.IsValidSpell()) return GLOBAL_SPELL_ID();
  if (!spellID.IsValidSpell()) return -1;
/*
  POSITION pos = m_spellbook.GetHeadPosition();
  while (pos != NULL)
  {
    //if (m_spellbook.PeekAtPos(pos).spell == gsID)
    //if (m_spellbook.PeekAtPos(pos).spell == spellNAme)
    if (m_spellbook.PeekAtPos(pos).spellID == spellID)
    {
      if (checkmemorized)
      {
        if (m_spellbook.PeekAtPos(pos).IsMemorized())
          //return (m_spellbook.GetSpellKeyAt(pos));
          return (m_spellbook.GetSpellIDAt(pos));
        else
          return SPELL_ID();
      }
      else
        //return (m_spellbook.GetSpellKeyAt(pos));
        return (m_spellbook.GetSpellIDAt(pos));
    }
    m_spellbook.GetNext(pos);
  }
  //return GLOBAL_SPELL_ID();
  return SPELL_ID();
*/
  int i;
  i = m_spellbook.LocateCharacterSpell(spellID);
  if (i < 0) return FALSE;
  return !checkmemorized || m_spellbook.PeekCharacterSpell(i)->IsMemorized();
}

//*****************************************************************************
//    NAME: CHARACTER::operator=
//
// PURPOSE: 
//
//*****************************************************************************
CHARACTER& CHARACTER::operator=(const CHARACTER& dude)
{
  if (&dude == this) return *this;
#ifdef newCombatant
  if ((dude.m_pCombatant != NULL) || (m_pCombatant != NULL))
  {
    // You need to take care of this yourself.  If one of these pointers
    // is valid, then you need to set them to NULL before the copy and
    // relink the combatant to the proper character via its m_pCharacter pointer.
    // There should always be a one-to-one link between a combatant and
    // a character.
#ifdef UAFEngine
    die (0xee398);
#else
    ASSERT(0);
#endif
  };
#endif
   Clear();
   preSpellNamesKey        = dude.preSpellNamesKey;
   type                    = dude.type;
   race                    = dude.race;
   gender                  = dude.gender;
   classID                 = dude.classID;
   alignment               = dude.alignment;
   allowInCombat           = dude.allowInCombat;
   status                  = dude.status;
   undeadType              = dude.undeadType;
   creatureSize            = dude.creatureSize;
   if (!dude.name.IsEmpty())
      name                 = dude.name;
   else
      name                 = "";

   DisableTalkIfDead       = dude.DisableTalkIfDead;
   TalkEvent               = dude.TalkEvent;
   TalkLabel               = dude.TalkLabel;
   ExamineEvent            = dude.ExamineEvent;
   ExamineLabel            = dude.ExamineLabel;

   THAC0                   = dude.THAC0;
   morale                  = dude.morale;
   encumbrance             = dude.encumbrance;
   maxEncumbrance          = dude.maxEncumbrance;
   m_AC                    = dude.m_AC;
   hitPoints               = dude.hitPoints;
   maxHitPoints            = dude.maxHitPoints;
   hitpointSeed            = dude.hitpointSeed;
   nbrHitDice              = dude.nbrHitDice;
   m_iUsedCureDisease      = dude.m_iUsedCureDisease;
   age                     = dude.age;
   maxAge                  = dude.maxAge;
   birthday                = dude.birthday;
   m_iMaxCureDisease       = dude.m_iMaxCureDisease;
   unarmedDieS             = dude.unarmedDieS;
   unarmedNbrDieS          = dude.unarmedNbrDieS;
   unarmedBonus            = dude.unarmedBonus;
   unarmedDieL             = dude.unarmedDieL;
   unarmedNbrDieL          = dude.unarmedNbrDieL;
   maxMovement             = dude.maxMovement;
   readyToTrain            = dude.readyToTrain;
   canTradeItems           = dude.canTradeItems;
   strength                = dude.strength;
   strengthMod             = dude.strengthMod;
   intelligence            = dude.intelligence;
   wisdom                  = dude.wisdom;
   dexterity               = dude.dexterity;
   constitution            = dude.constitution;
   charisma                = dude.charisma;
   strength_adjustment     = dude.strength_adjustment;
   strengthMod_adjustment  = dude.strengthMod_adjustment;
   intelligence_adjustment = dude.intelligence_adjustment;
   wisdom_adjustment       = dude.wisdom_adjustment;
   dexterity_adjustment    = dude.dexterity_adjustment;
   constitution_adjustment = dude.constitution_adjustment;
   charisma_adjustment     = dude.charisma_adjustment;
   openDoors               = dude.openDoors;
   openMagicDoors          = dude.openMagicDoors;
   BB_LG                   = dude.BB_LG;
   automatic               = dude.automatic;
   allowPlayerControl      = dude.allowPlayerControl;
   detectingInvisible      = dude.detectingInvisible;
   detectingTraps          = dude.detectingTraps;
   hitBonus                = dude.hitBonus;
   dmgBonus                = dude.dmgBonus;
   magicResistance         = dude.magicResistance;

   baseclassStats.RemoveAll();
   baseclassStats.Append(dude.baseclassStats);

   skillAdjustments.RemoveAll();
   skillAdjustments.Append(dude.skillAdjustments);

   spellAdjustments.RemoveAll();
   spellAdjustments.Append(dude.spellAdjustments);


   //  This is cleared at the end of this function...why copy it?   spellAbility = dude.spellAbility;

   IsPreGen                = dude.IsPreGen;
   CanBeSaved              = dude.CanBeSaved;
   HasLayedOnHandsToday    = dude.HasLayedOnHandsToday;
   money                   = dude.money;
   NbrAttacks              = dude.NbrAttacks;
   myItems                 = dude.myItems;
   icon                    = dude.icon;
   iconIndex               = dude.iconIndex;
   smallPic                = dude.smallPic;
   origIndex               = dude.origIndex;
   uniquePartyID           = dude.uniquePartyID;
   characterID             = dude.characterID;
   monsterID               = dude.monsterID;
   specAbs.Copy(dude.specAbs);   
   blockageData            = dude.blockageData;
   m_spellbook             = dude.m_spellbook;
   char_asl.Copy(dude.char_asl);
   *m_traits               = *dude.m_traits;
   m_spellEffects.RemoveAll();
   m_spellCastingLevel     = dude.m_spellCastingLevel;
   AIBaseclass             = dude.AIBaseclass;
   POSITION pos            = dude.m_spellEffects.GetHeadPosition();
   SPELL_EFFECTS_DATA tmp;
   while (pos != NULL)
   {     
     const SPELL_EFFECTS_DATA tmpref = *dude.m_spellEffects.PeekNext(pos);
     tmp=tmpref;
     m_spellEffects.AddTail(tmp);
   }

  specAbQueue.RemoveAll();
  pos = dude.specAbQueue.GetHeadPosition();
  while (pos!=NULL)
  {
    SQueuedSpecAb data = *dude.specAbQueue.PeekNext(pos);
    specAbQueue.AddTail(data);
  }

  spellAbility.Clear();
  return *this;
}
#ifdef UAFEDITOR
//*****************************************************************************
//    NAME: CHARACTER::operator==
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::operator==(const CHARACTER& dude)
{
  if (&dude == this) return TRUE;
  if (preSpellNamesKey != dude.preSpellNamesKey) return FALSE;
  if (GetType() != dude.GetType()) return FALSE;
  if (race != dude.race) return FALSE;
  if (gender != dude.gender) return FALSE;
  //if (charClass != dude.charClass) return FALSE;
  if (classID != dude.classID) return FALSE;
  if (alignment != dude.alignment) return FALSE;
  if (allowInCombat != dude.allowInCombat) return FALSE;
  if (status != dude.status) return FALSE;
  //if (undead != dude.undead) return FALSE;
  if (undeadType != dude.undeadType) return FALSE;
  if (creatureSize != dude.creatureSize) return FALSE;
  if (name != dude.name) return FALSE;
  if (THAC0 != dude.THAC0) return FALSE;
  if (morale != dude.morale) return FALSE;
  if (encumbrance != dude.encumbrance) return FALSE;
  if (maxEncumbrance != dude.maxEncumbrance) return FALSE;
  if (m_AC != dude.m_AC) return FALSE;
  if (hitPoints != dude.hitPoints) return FALSE;
  if (maxHitPoints != dude.maxHitPoints) return FALSE;
  if (!(nbrHitDice == dude.nbrHitDice)) return FALSE;
  if (age != dude.age) return FALSE;
  if (maxAge != dude.maxAge) return FALSE;
  if (birthday!=dude.birthday) return FALSE;
  if (m_iMaxCureDisease != dude.m_iMaxCureDisease) return FALSE;
  if (unarmedDieS != dude.unarmedDieS) return FALSE;
  if (unarmedNbrDieS != dude.unarmedNbrDieS) return FALSE;
  if (unarmedBonus != dude.unarmedBonus) return FALSE;
  if (unarmedDieL != dude.unarmedDieL) return FALSE;
  if (unarmedNbrDieL != dude.unarmedNbrDieL) return FALSE;
  //if (ItemMask != dude.ItemMask) return FALSE;
  if (maxMovement != dude.maxMovement) return FALSE;
  if (readyToTrain != dude.readyToTrain) return FALSE;
  if (canTradeItems != dude.canTradeItems) return FALSE;
  if (GetPermStr() != dude.GetPermStr()) return FALSE;
  if (GetPermStrMod() != dude.GetPermStrMod()) return FALSE;
  if (GetPermInt() != dude.GetPermInt()) return FALSE;
  if (GetPermWis() != dude.GetPermWis()) return FALSE;
  if (GetPermDex() != dude.GetPermDex()) return FALSE;
  if (GetPermCon() != dude.GetPermCon()) return FALSE;
  if (GetPermCha() != dude.GetPermCha()) return FALSE;
  if (openDoors != dude.openDoors) return FALSE;
  if (openMagicDoors != dude.openMagicDoors) return FALSE;
  if (BB_LG != dude.BB_LG) return FALSE;
  if (automatic != dude.automatic) return FALSE;
  if (allowPlayerControl != dude.allowPlayerControl) return FALSE;
  if (detectingInvisible!=dude.detectingInvisible) return FALSE;
  if (detectingTraps!=dude.detectingTraps) return FALSE;
  if (hitBonus != dude.hitBonus) return FALSE;
  if (dmgBonus != dude.dmgBonus) return FALSE;
  if (magicResistance != dude.magicResistance) return FALSE;
  {
    int i, j, n;
    n = dude.baseclassStats.GetSize();
    if (n != baseclassStats.GetSize()) return FALSE;
    for (i=0; i<n; i++)
    {
      const BASECLASS_STATS *pi;
      pi = dude.PeekBaseclassStats(i);
      for (j=0; j<n; j++)
      {
        const BASECLASS_STATS *pj;
        pj = PeekBaseclassStats(j);
        if (*pi == *pj) break;
      };
      if (j == n) return FALSE;
    };
  };

  {
    int i, j, n;
    n = dude.skillAdjustments.GetSize();
    if (n != skillAdjustments.GetSize()) return FALSE;
    for (i=0; i<n; i++)
    {
      const SKILL_ADJ *pi;
      pi = dude.PeekSkillAdj(i);
      for (j=0; j<n; j++)
      {
        const SKILL_ADJ *pj;
        pj = PeekSkillAdj(j);
        if (*pi == *pj) break;
      };
      if (j == n) return FALSE;
    };
  };
  {
    int i, j, n;
    n = dude.spellAdjustments.GetSize();
    if (n != spellAdjustments.GetSize()) return FALSE;
    for (i=0; i<n; i++)
    {
      const SPELL_ADJ *pi;
      pi = dude.PeekSpellAdj(i);
      for (j=0; j<n; j++)
      {
        const SPELL_ADJ *pj;
        pj = PeekSpellAdj(j);
        if (*pi == *pj) break;
      };
      if (j == n) return FALSE;
    };
  };

  if (IsPreGen != dude.IsPreGen) return FALSE;
  if (CanBeSaved != dude.CanBeSaved) return FALSE;
  if (HasLayedOnHandsToday != dude.HasLayedOnHandsToday) return FALSE;
  if (DisableTalkIfDead != dude.DisableTalkIfDead) return FALSE;
  if (TalkEvent != dude.TalkEvent) return FALSE;
  if (TalkLabel!=dude.TalkLabel) return FALSE;
  if (ExamineEvent != dude.ExamineEvent) return FALSE;
  if (ExamineLabel!=dude.ExamineLabel) return FALSE;
  if (!(money==dude.money) ) return FALSE;
  if (!(NbrAttacks == dude.NbrAttacks)) return FALSE;
  //if (uniqueKey != dude.uniqueKey) return FALSE;
  if (origIndex != dude.origIndex) return FALSE;
  if (uniquePartyID != dude.uniquePartyID) return FALSE;
  if (!(characterID == dude.characterID)) return FALSE;
  if (!(monsterID == dude.monsterID)) return FALSE;
  if ((char_asl == dude.char_asl)==FALSE) return FALSE;
  if ((*m_traits == *dude.m_traits)==FALSE) return FALSE;
  if ((specAbs == dude.specAbs)==FALSE) return FALSE;
  if ((blockageData == dude.blockageData)==FALSE) return FALSE;
  if ((myItems.operator==(dude.myItems))==FALSE) return FALSE;    
  if ((m_spellbook == dude.m_spellbook)==FALSE) return FALSE;  

  if (!(icon == dude.icon)) return FALSE;
  if (iconIndex != dude.iconIndex) return FALSE;
  if ((smallPic == dude.smallPic)==FALSE) return FALSE;
#ifdef UAFEngine
  if (m_spellEffects.GetCount() != dude.m_spellEffects.GetCount()) return false;
  POSITION pos = dude.m_spellEffects.GetHeadPosition();
  while (pos != NULL)
    if (!HaveEffectAttr(dude.m_spellEffects.GetNext(pos))) return false;
#endif

    //if (m_Properties != dude.m_Properties) return false;
    
  return TRUE;
}
#endif


//*****************************************************************************
//    NAME: CHARACTER::IsSameCharacter
//
// PURPOSE: When testing for duplicate characters, not all fields can
//          be compared, and an exact comparison is not desired.
//          For this test, only compare data that will not change
//          over time. Things that change as a character adventures
//          and levels up should not be included.
//
//*****************************************************************************
BOOL CHARACTER::IsSameCharacter(const CHARACTER& dude) const
{
  if (&dude == this) return TRUE;
  if (GetType() != dude.GetType()) return FALSE;
  if (IsPreGen != dude.IsPreGen) return FALSE;
  if (name != dude.name) return FALSE;  
  return TRUE;
}

//*****************************************************************************
// NAME:    CHARACTER::CHARACTER
//
// PURPOSE: Constructor for CHARACTER class
//
// RETURNS: None
//*****************************************************************************
CHARACTER::CHARACTER(void): 
  // ASL named "CHARACTER_ATTRIBUTES"
  char_asl(),
  TalkEvent(0),
  ExamineEvent(0),
  specAbs(false) 
{
  m_traits=new TRAITS;
  Clear(TRUE);

  /*
  m_Properties.Clear();
  for (int i=FIRST_CHAR_KEYWORD; i<=LAST_CHAR_KEYWORD; i++)
  {
    if (!IsDataReadOnly(i))
    {
      CBaseProperty prop(NumToKeywordText(i));
      m_Properties.UpdateProperty(prop);
    }
  }
  */
}
//*****************************************************************************
// NAME:    CHARACTER::CHARACTER
//
// PURPOSE: Copy Constructor for CHARACTER class
//
// RETURNS: None
//*****************************************************************************
CHARACTER::CHARACTER(const CHARACTER& dude) : 
  // ASL named "CHARACTER_ATTRIBUTES"
  char_asl(),
  TalkEvent(0),
  ExamineEvent(0),
  specAbs(false)
{
  m_traits=new TRAITS;
  Clear(TRUE);  
  *this = dude;
}

//*****************************************************************************
// NAME:    CHARACTER::~CHARACTER
//
// PURPOSE: Destructor of CHARACTER class
//
// RETURNS: None
//*****************************************************************************
CHARACTER::~CHARACTER(void)
{
  Clear(TRUE);
  if (m_traits!=NULL) delete m_traits;
  m_traits=NULL;
}

//*****************************************************************************
// NAME:    CHARACTER::Clear
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::Clear(BOOL IsConstructor)
{
   //if (TalkEvent > 0)
   //  globalData.eventData.DeleteEvents(TalkEvent);
   DisableTalkIfDead=TRUE;
   TalkEvent=0;
   TalkLabel="TALK";
   //if (ExamineEvent > 0)
   //  globalData.eventData.DeleteEvents(ExamineEvent);
   ExamineEvent=0;
   ExamineLabel="EXAMINE";
   //uniqueKey = 0;
   type = CHAR_TYPE;
   //race(Human);
   race = "";
   gender = Male;
   //charClass = Fighter;
   classID = "";
   alignment = ChaoticNeutral;
   allowInCombat = TRUE;
   status = Unconscious;
   //undead = NotUndead;
   undeadType.Empty();
   creatureSize = Medium;
   name="";
   THAC0 = 20;
   morale = 50; // 0.100%
   encumbrance = 0;
   maxEncumbrance = 0;
   m_AC = 10;
   hitPoints = 0;
   maxHitPoints = 0;
   nbrHitDice = 0;
   age = 0;
   maxAge = 0;
   SetBirthday(1);
   m_iMaxCureDisease = 0;
   m_iUsedCureDisease=0;
   unarmedDieS = 0;
   unarmedNbrDieS = 0;
   unarmedBonus = 0;
   unarmedDieL = 0;
   unarmedNbrDieL = 0;
   //ItemMask = 0;
   maxMovement = 0;
   readyToTrain = FALSE;
   canTradeItems=TRUE;
   strength = 0;
   strengthMod = 0;
   intelligence = 0;
   wisdom = 0;
   dexterity = 0;
   constitution = 0;
   charisma = 0;
   strength_adjustment = 0x7fffffff;
   strengthMod_adjustment = 0x7fffffff;
   intelligence_adjustment = 0x7fffffff;
   wisdom_adjustment = 0x7fffffff;
   dexterity_adjustment = 0x7fffffff;
   constitution_adjustment = 0x7fffffff;
   charisma_adjustment = 0x7fffffff;
   spellAbility.valid = FALSE;
   openDoors = 0;
   openMagicDoors = 0;
   BB_LG = 0;
   automatic = FALSE;
   allowPlayerControl=TRUE;
   //detectingMagic=FALSE;
   detectingInvisible=FALSE;
   detectingTraps=FALSE;
   hitBonus = 0;
   dmgBonus = 0;
   magicResistance = 0;
   baseclassStats.RemoveAll();

   IsPreGen = FALSE;
   CanBeSaved=TRUE;
   HasLayedOnHandsToday=FALSE;
   money.Clear();
   NbrAttacks = 0.0;
   icon.Clear();
   iconIndex=1;
   smallPic.Clear();
   origIndex = -1;
   monsterID.Clear();
   characterID.Clear();
   uniquePartyID = 0xff;
   specAbs.Clear();
   specAbQueue.RemoveAll();
   blockageData.Clear();
   m_spellbook.Clear();
   myItems.Clear();
   char_asl.Clear();
   m_spellCastingLevel = -1;
   AIBaseclass = -1;
   m_spellEffects.RemoveAll();
   spellAdjustments.RemoveAll();
   skillAdjustments.RemoveAll();
   m_pCombatant = NULL;
#ifdef OldDualClass20180126
   20180126 temp__canUsePrevClass = -1;
#endif
}

#ifdef UAFEDITOR
CLASS_ID ConvertClassTypeToClassID(int type)
{
  CLASS_ID classID;
  switch (type)
  {
  case Fighter:
    classID = "Fighter";
    break;
  case Cleric:
    classID =	"Cleric";
    break;
  case Ranger:
    classID =	"Ranger";
    break;
  case Paladin:
    classID =	"Paladin";
    break;
  case MagicUser:
    classID =	"Magic User";
    break;
  case Thief:
    classID =	"Thief";
    break;
  case Druid:
    classID =	"Druid";
    break;
  case C_F:
    classID =  "Cleric/Fighter";
    break;
  case C_F_MU:
    classID =  "Cleric/Fighter/Magic User";
    break;
  case C_R:
    classID =  "Cleric/Ranger";
    break;
  case C_MU:
    classID =  "Cleric/Magic User";
    break;
  case C_T:
    classID =  "Cleric/Thief";
    break;
  case F_MU:
    classID =  "Fighter/Magic User";
    break;
  case F_T:
    classID =  "Fighter/Thief";
    break;
  case F_MU_T:
    classID =  "Fighter/Magic User/Thief";
    break;
  case MU_T:
    classID =  "Magic User/Thief";
    break;
  case F_D:
    classID =  "Fighter/Druid";
    break;
  case D_MU:
    classID =  "Druid/Magic User";
    break;
  case F_MU_D:
    classID =  "Fighter/Magic User/Druid";
    break;
  };
  return classID;
}
#endif


//*****************************************************************************
// NAME:    CHARACTER::Serialize
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::Serialize(CArchive &ar, double version)
{
  // This data can be serialized along with the globalData, and
  // also separately as a 'CharName.CHR' file.
  // The version specified distinguishes between saved char file 
  // version and game.dat file version
  //
//  CObject::Serialize(ar);
  int characterVersion=0;
  bool raceProblem = false;
  DWORD raceProblemTemp=999;
  bool classProblem = false;
  DWORD classProblemTemp=999;
  if (ar.IsStoring())
  {
    int temp;
    ar << CHARACTER_VERSION;
    //ar << uniqueKey;
    ar << type;
    //temp = m_race;
    //ar << temp;
    ar << race;
    temp = gender;
    ar << temp;
    //temp = charClass;
    //ar << temp;
    ar << classID;
    temp = alignment;
    ar << temp;
    
    ar << allowInCombat;

    temp = status;
    ar << temp;
    //temp = undead;
    //temp = undeadLevel;
    ar << undeadType;
    temp = creatureSize;
    ar << temp;

    AS(ar,name);
/*
    ar << FighterExperience;
    ar << ClericExperience;
    ar << RangerExperience;
    ar << PaladinExperience;
    ar << MUExperience;
    ar << ThiefExperience;
    ar << DruidExperience;
*/
    ar << THAC0;
    ar << morale;
    ar << encumbrance;
    ar << maxEncumbrance;
    ar << m_AC;
    ar << hitPoints;
    ar << maxHitPoints;
    ar << nbrHitDice;
    ar << age;
    ar << maxAge;
    ar << birthday;
    ar << m_iMaxCureDisease;
    ar << unarmedDieS;
    ar << unarmedNbrDieS;
    ar << unarmedBonus;
    ar << unarmedDieL;
    ar << unarmedNbrDieL;
    //ar << ItemMask;
    ar << maxMovement;
    ar << readyToTrain;
    ar << canTradeItems;   

    /*
    ar << GetStr();
    ar << GetStrMod();
    ar << GetInt();
    ar << GetWis();
    ar << GetDex();
    ar << GetCon();
    ar << GetCha();
    */
    die ("Not Needed?"); //Not Implemented(0x442bc, false); // Should not be using CArchive.

    ar << openDoors;
    ar << openMagicDoors;
    ar << BB_LG;
    ar << hitBonus;
    ar << dmgBonus;
    ar << magicResistance;
    {
      int i, count;
      count = GetBaseclassStatsCount();
      ar << count;
      for (i=0; i<count; i++)
      {
        GetBaseclassStats(i)->Serialize(ar);
        //baseclassStats[i].Serialize(ar);
      };
    };

    ar << IsPreGen;
    ar << CanBeSaved;
    ar << HasLayedOnHandsToday;
    money.Serialize(ar, version);
    ar << NbrAttacks;
    icon.Serialize(ar,version, rte.IconArtDir());
    ar << iconIndex;
    ar << origIndex;
    ar << uniquePartyID;
    ar << DisableTalkIfDead;
    ar << TalkEvent;
    AS(ar, TalkLabel);
    ar << ExamineEvent;
    AS(ar, ExamineLabel);
    
    //ar << spells;
    m_spellbook.Serialize(ar, version);

    //ar << detectingMagic;
    ar << detectingInvisible;
    ar << detectingTraps;
    ar << m_spellEffects.GetCount();
    POSITION pos = m_spellEffects.GetHeadPosition();
    while (pos!=NULL)
    {
      m_spellEffects.GetNext(pos)->Serialize(ar,version);
    };
    blockageData.Serialize(ar);
  }
  else
  {
#ifdef UAFEDITOR
    BYTE FighterLevel;
	  BYTE ClericLevel;
	  BYTE RangerLevel;
	  BYTE PaladinLevel;
	  BYTE MULevel;
	  BYTE ThiefLevel;
    BYTE DruidLevel;

    	// levels attained before changing class (Dual-Class)
	  BYTE prevFighterLevel;
	  BYTE prevClericLevel;
	  BYTE prevRangerLevel;
	  BYTE prevPaladinLevel;
	  BYTE prevMULevel;
	  BYTE prevThiefLevel;
    BYTE prevDruidLevel;

	    // pre-drain levels
	  BYTE pdFighterLevel;
	  BYTE pdClericLevel;
	  BYTE pdRangerLevel;
	  BYTE pdPaladinLevel;
	  BYTE pdMULevel;
	  BYTE pdThiefLevel;
    BYTE pdDruidLevel;

    int FighterExperience=0;
	  int ClericExperience=0;
	  int RangerExperience=0;
	  int PaladinExperience=0;
	  int MUExperience=0;
	  int ThiefExperience=0;
    int DruidExperience=0;
#endif

    int temp;
    ar >> temp;  // Possible character version
    if ( (temp & 0x80000000) != 0)
    {
      // This is a character version, not an index.
      characterVersion = temp;
#ifdef UAFEDITOR
      if (version < VersionSpellNames)
      {
        ar >> preSpellNamesKey;
      };
#endif
    }
    else
    {
      characterVersion = 0;   // This is a key....not a version
      preSpellNamesKey = temp;
      //uniqueKey = temp;
    };
    ar >> type;
#ifdef UAFEDITOR
    if (version <VersionSpellNames)
    {
      RACE_ID raceID;
      ar >> temp;
      //race((raceType)temp);

      raceID = raceData.FindPreVersionSpellNamesRaceID(temp);
      if (raceID.IsEmpty())
      {
        raceProblem = true;
        raceProblemTemp = temp;
        switch(temp)
        {
        default:
        case  Human:
          race = "Human";
          break;
        case  Elf: 
          race = "Elf";
          break;
        case  Dwarf:
          race = "Dwarf";
          break;
        case  Gnome:
          race = "Gnome";
          break;
        case  HalfElf:
          race = "HalfElf";
          break;
        case  Halfling:
          race = "Halfling";
          break;
        };
      }
      else
      {
        race = raceID;
      };
    }
    else
#endif
    {
      ar >> race;
    };
    ar >> temp;
    gender = (genderType)temp;
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      //int temp;
      ar >> temp;
      classID = classData.FindPreVersionSpellNamesClassID(temp);
      if (classID.IsEmpty())
      {
        classProblem = true;
        classProblemTemp = temp;
        classID = ConvertClassTypeToClassID(temp);
      };
    //charClass = (classType)temp;   
    }
    else
#endif
    {
      ar >> classID;
    };
    ar >> temp;
    alignment = (alignmentType)temp;   
    
    if (version >= _VERSION_0912_)
      ar >> allowInCombat;
    ar >> temp;
    status = (charStatusType)temp;   
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      ar >> temp;
      //undead = (undeadType)temp;
      //undeadLevel = temp;
      undeadType.Empty();
    }
    else
#endif
    {
      ar >> undeadType;
    };
    ar >> temp;
    creatureSize = (creatureSizeType)temp;   
    DAS(ar,name);
    if (version < VersionSpellNames)
    {
      characterID = name;
    };
    if (raceProblem)
    {
      CString msg;
      msg.Format("Character %s has race number %d.  "
                 "We cannot determine which race this should be so we "
                 "set the race to %s", name, raceProblemTemp, race);
      MsgBoxInfo(msg, "Information");
    };

    if (classProblem)
    {
      CString msg;
      msg.Format("Character %s has class number %d.  "
                 "We cannot determine which class this should be so we "
                 "set the class to %s", name, classProblemTemp, classID);
      MsgBoxInfo(msg, "Information");
    };

#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      ar >> FighterExperience;
      ar >> ClericExperience;
      ar >> RangerExperience;
      ar >> PaladinExperience;
      ar >> MUExperience;
      ar >> ThiefExperience;
      ar >> DruidExperience;
    };
#endif
    ar >> THAC0;
    ar >> morale;
    ar >> encumbrance;
    ar >> maxEncumbrance;
    ar >> m_AC;
    ar >> hitPoints;
    ar >> maxHitPoints;
    ar >> nbrHitDice;
    ar >> age;
    ar >> maxAge;
    if (version >= _VERSION_0830_)
    {
      ar >> birthday;
      SetBirthday(birthday);
    }
    ar >> m_iMaxCureDisease;
    ar >> unarmedDieS;
    ar >> unarmedNbrDieS;
    ar >> unarmedBonus;
    ar >> unarmedDieL;
    ar >> unarmedNbrDieL;

#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
    //ar >> ItemMask;
      BYTE itemMask;
      ar >> itemMask;
    }
#endif
    ar >> maxMovement;
    ar >> readyToTrain;
    if (version >= _VERSION_0695_)
      ar >> canTradeItems;

    if (version < 0.999702)
    {
      BYTE tmp;;
      /*
      ar >> tmp; SetStr(tmp);
      ar >> tmp; SetStrMod(tmp);
      ar >> tmp; SetInt(tmp);
      ar >> tmp; SetWis(tmp);
      ar >> tmp; SetDex(tmp);
      ar >> tmp; SetCon(tmp);
      ar >> tmp; SetCha(tmp);
      */
      ar >> tmp; SetPermStr(tmp);
      ar >> tmp; SetPermStrMod(tmp);
      ar >> tmp; SetPermInt(tmp);
      ar >> tmp; SetPermWis(tmp);
      ar >> tmp; SetPermDex(tmp);
      ar >> tmp; SetPermCon(tmp);
      ar >> tmp; SetPermCha(tmp);
      strength_adjustment = 0x7fffffff;
      strengthMod_adjustment = 0x7fffffff;
      intelligence_adjustment = 0x7fffffff;
      wisdom_adjustment = 0x7fffffff;
      dexterity_adjustment = 0x7fffffff;
      constitution_adjustment = 0x7fffffff;
      charisma_adjustment = 0x7fffffff;
    }
    else
    {
      die("Not Needed?"); //Not Implemented(0x442a, false);  // Cannot happen
    };
    ar >> openDoors;
    ar >> openMagicDoors;
    ar >> BB_LG;
    ar >> hitBonus;
    ar >> dmgBonus;
    ar >> magicResistance;
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      int trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
      ar >> trash;
    };
#endif

#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      BASECLASS_STATS bcl;
      
      ar >> FighterLevel;
      ar >> ClericLevel;
      ar >> RangerLevel;
      ar >> PaladinLevel;
      ar >> MULevel;
      ar >> ThiefLevel;
      ar >> DruidLevel;
      ar >> prevFighterLevel;
      ar >> prevClericLevel;
      ar >> prevRangerLevel;
      ar >> prevPaladinLevel;
      ar >> prevMULevel;
      ar >> prevThiefLevel;
      ar >> prevDruidLevel;
      ar >> pdFighterLevel;
      ar >> pdClericLevel;
      ar >> pdRangerLevel;
      ar >> pdPaladinLevel;
      ar >> pdMULevel;
      ar >> pdThiefLevel;
      ar >> pdDruidLevel;

      if (FighterLevel > 0)
      {
        bcl.baseclassID = "fighter";
        bcl.currentLevel = FighterLevel;
        bcl.previousLevel = prevFighterLevel;
        bcl.preDrainLevel = pdFighterLevel;
        bcl.x_experience = FighterExperience;
        baseclassStats.Add(bcl);
      };
      if (ClericLevel > 0)
      {
        bcl.baseclassID = "cleric";
        bcl.currentLevel = ClericLevel;
        bcl.previousLevel = prevClericLevel;
        bcl.preDrainLevel = pdClericLevel;
        bcl.x_experience = ClericExperience;
        baseclassStats.Add(bcl);
      };
      if (RangerLevel > 0)
      {
        bcl.baseclassID = "ranger";
        bcl.currentLevel = RangerLevel;
        bcl.previousLevel = prevRangerLevel;
        bcl.preDrainLevel = pdRangerLevel;
        bcl.x_experience = RangerExperience;
        baseclassStats.Add(bcl);
      };
      if (PaladinLevel > 0)
      {
        bcl.baseclassID = "paladin";
        bcl.currentLevel = PaladinLevel;
        bcl.previousLevel = prevPaladinLevel;
        bcl.preDrainLevel = pdPaladinLevel;
        bcl.x_experience = PaladinExperience;
        baseclassStats.Add(bcl);
      };
      if (MULevel > 0)
      {
        bcl.baseclassID = "magicUser";
        bcl.currentLevel = MULevel;
        bcl.previousLevel = prevMULevel;
        bcl.preDrainLevel = pdMULevel;
        bcl.x_experience = MUExperience;
        baseclassStats.Add(bcl);
      };
      if (ThiefLevel > 0)
      {
        bcl.baseclassID = "thief";
        bcl.currentLevel = ThiefLevel;
        bcl.previousLevel = prevThiefLevel;
        bcl.preDrainLevel = pdThiefLevel;
        bcl.x_experience = ThiefExperience;
        baseclassStats.Add(bcl);
      };
      if (DruidLevel > 0)
      {
        bcl.baseclassID = "druid";
        bcl.currentLevel = DruidLevel;
        bcl.previousLevel = prevDruidLevel;
        bcl.preDrainLevel = pdDruidLevel;
        bcl.x_experience = DruidExperience;
        baseclassStats.Add(bcl);
      };
    }
    else
#endif
    {
      BASECLASS_STATS bcl;
      int i, count;
      ar >> count;
      for (i=0; i<count; i++)
      {
        bcl.Serialize(ar);
        baseclassStats.Add(bcl);
      };
    };
    spellAbility.valid = FALSE;
    UpdateSpellAbility();




    ar >> IsPreGen;
    
    if (version >= _VERSION_0698_)
      ar >> CanBeSaved;

    if (version >= _VERSION_0900_)
      ar >> HasLayedOnHandsToday;

    if (version < _VERSION_0661_)
    {
      ar >> temp; money.Coins[PlatinumType] = temp;
      ar >> temp; money.Coins[GoldType] = temp;
      ar >> temp; money.Coins[ElectrumType] = temp;
      ar >> temp; money.Coins[SilverType] = temp;
      ar >> temp; money.Coins[CopperType] = temp;
      ar >> temp; { for (int g=0;g<temp;g++) money.AddGem(); }
      ar >> temp; { for (int j=0;j<temp;j++) money.AddJewelry(); }
      if (version >= _VERSION_0660_)
      {
        ar >> temp; money.Coins[CoinType6] = temp;
        ar >> temp; money.Coins[CoinType7] = temp;
        ar >> temp; money.Coins[CoinType8] = temp;
        ar >> temp; money.Coins[CoinType9] = temp;
        ar >> temp; money.Coins[CoinType10] = temp;
      }
    }
    else
    {
      money.Serialize(ar, version);
    }
    ar >> NbrAttacks;
    
    if (version < _VERSION_0640_)
    {
      CString tmp;
      long notused;
      DAS(ar,tmp);
      ar >> notused;
      StripFilenamePath(tmp);
      //tmp = rte.IconArtDir() + tmp;
      //xAddArtFolderToPath(tmp);
      icon.filename = tmp;
      icon.picType = IconDib;
      icon.SetDefaults();
    }
    else
    {
      icon.Serialize(ar, version, rte.IconArtDir());
      if (version <= _VERSION_0840_)
        icon.SetPre0840NonLoopDefaults();
      StripFilenamePath(icon.filename);  // Some old saves have a path built in
    }    
    if (version >= _VERSION_0640_)
      ar >> iconIndex;
    ar >> origIndex;
    ar >> uniquePartyID;

    if (version >= _VERSION_0870_)
      ar >> DisableTalkIfDead;
    if (version >= _VERSION_0662_)
      ar >> TalkEvent;
    if (version >= _VERSION_0710_)
      DAS(ar,TalkLabel);
    if (version >= _VERSION_0800_)
    {
      ar >> ExamineEvent;
      DAS(ar,ExamineLabel);
    }

    m_spellbook.Serialize(ar, version);

    if (version >= _VERSION_06991_)
    {
      if (version < _VERSION_0850_)
      {
        BOOL unused1;
        ar >> unused1;
      }
      ar >> detectingInvisible;
      ar >> detectingTraps;
    }
    if (version >= _VERSION_0630_)
    {
      if (version < _VERSION_06991_)
      {
        m_spellEffects.Serialize(ar);
      }
      else
      {
        m_spellEffects.RemoveAll();
        int count;
        ar >> count;
        for (int i=0;i<count;i++)
        {
          SPELL_EFFECTS_DATA data;
          data.Serialize(ar,version);
          m_spellEffects.AddTail(data);
          WriteDebugString("Loading spell effect for %s: %s\n",
                           GetName(), data.AffectedAttr());
        }
      }
    }
    if (version >= _VERSION_0696_)
      blockageData.Serialize(ar);

    UpdateSpellAbility();
    //m_spellbook.SortSpells();
  }
  smallPic.Serialize(ar, version, rte.PicArtDir());
  if (!ar.IsStoring())
  {
    if (version <= _VERSION_0840_)
      smallPic.SetPre0840LoopForeverDefaults();
  }

  myItems.Serialize(ar,version);
  if (ar.IsLoading() && (characterVersion == 0))
  {
    // Old versions included armor and such in m_AC.
    // We need to subtract it out for newer versions.
    m_AC -= myItems.GetProtectModForRdyItems();
  };
  specAbs.Serialize(ar, version, this->name, "char");
  //specAbQueue: no need to save this data
  char_asl.Serialize(ar,"CHARACTER_ATTRIBUTES");
/*
  {
    POSITION pos = m_spellbook.GetHeadPosition();
    while (pos!=NULL)
    {
      //const SPELL_DATA *pData = spellData.GetSpellData(m_spellbook.PeekAtPos(pos).spellName);
      const SPELL_DATA *pData = spellData.GetSpellData(m_spellbook.PeekAtPos(pos).spellID);
      if (pData!=NULL)
      {
        //WriteDebugString("%s Spell \'%s\',  sb key %u\n", 
        //                 GetName(),
        //                 pData->Name,
        //                 m_spellbook.PeekAtPos(pos).spell);
      }
      else
        WriteDebugString("%s Spell missing, sb key %s\n", 
                        GetName(),
                        m_spellbook.PeekAtPos(pos).spellID.UniqueName);
  
      m_spellbook.GetNext(pos);
    };
  };
*/
  {
    // Make a note of any spells that the character has but which are
    // missing from the spell database.
    int i, n;
    n = m_spellbook.GetCount();
    for (i=0; i<n; i++)
    {
      const CHARACTER_SPELL *pCharSp;
      pCharSp = m_spellbook.PeekCharacterSpell(i);
      if (!pCharSp->spellID.IsValidSpell())
      {
        WriteDebugString("Character %s is missing spell %s\n",
                          GetName(),
                          pCharSp->spellID.UniqueName());
      };
    };
  };


  //m_Properties.Serialize(ar, version);

  // just to make sure we catch this for some older
  // character's that might come from saved games
  //strengthKey     = abilityData.GetKeyByIndex(0);
  //intelligenceKey = abilityData.GetKeyByIndex(1);
  //wisdomKey       = abilityData.GetKeyByIndex(2);
  //dexterityKey    = abilityData.GetKeyByIndex(3);
  //constitutionKey = abilityData.GetKeyByIndex(4);
  //charismaKey     = abilityData.GetKeyByIndex(5);

  // design settings that affect encumbrance might
  // have changed since character was last saved
  encumbrance = determineEffectiveEncumbrance();
  
  DisplayCurrSpellEffects("Serialize");
}


//*****************************************************************************
//    NAME: CHARACTER::Serialize
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::Serialize(CAR &car, double version)
{
  // This data can be serialized along with the globalData, and
  // also separately as a 'CharName.CHR' file.
  // The version specified distinguishes between saved char file 
  // version and game.dat file version
  //
  int characterVersion=0;
  if (car.IsStoring())
  {
    int temp;
    car << CHARACTER_VERSION;
    //ar << uniqueKey;
    car << preSpellNamesKey;
    car << type;
    //temp = m_race;
    //ar << temp;
    car << race;
    temp = gender;
    //ar << temp;
    car << temp;
    //temp = charClass;
    car << classID;
    //car << temp;
    temp = alignment;
    car << temp;
    car << allowInCombat;
    temp = status;
    car << temp;
    //temp = undead;
    //temp = undeadLevel;
    //car << temp;
    car << undeadType;
    temp = creatureSize;
    car << temp;

    AS(car,name);
    car << characterID;
/*
    ar << FighterExperience;
    ar << ClericExperience;
    ar << RangerExperience;
    ar << PaladinExperience;
    ar << MUExperience;
    ar << ThiefExperience;
    ar << DruidExperience;
*/
    car << THAC0;
    car << morale;
    car << encumbrance;
    car << maxEncumbrance;
    car << m_AC;
    car << hitPoints;
    car << maxHitPoints;
    car << nbrHitDice;
    car << age;
    car << maxAge;
    car << birthday;
    car << m_iMaxCureDisease;
    car << unarmedDieS;
    car << unarmedNbrDieS;
    car << unarmedBonus;
    car << unarmedDieL;
    car << unarmedNbrDieL;
    //ar << ItemMask;
    car << maxMovement;
    car << readyToTrain;
    car << canTradeItems;

    car << GetPermStr();
    car << GetPermStrMod();
    car << GetPermInt();
    car << GetPermWis();
    car << GetPermDex();
    car << GetPermCon();
    car << GetPermCha();

    car << openDoors;
    car << openMagicDoors;
    car << BB_LG;
    car << hitBonus;
    car << dmgBonus;
    car << magicResistance;
    {
      CString BSVersion = "BS0";
      int i, count;
      car << BSVersion;
      count = GetBaseclassStatsCount();
      car << count;
      for (i=0; i<count; i++)
      {
        GetBaseclassStats(i)->Serialize(car, BSVersion);
        //baseclassStats[i].Serialize(car, BSVersion);
      };
    }
    {
      CString SAVersion = "SA0";
      int i, count;
      count = GetSkillAdjCount();
      car << SAVersion;
      car << count;
      for (i=0; i<count; i++)
      {
        GetSkillAdj(i)->Serialize(car, SAVersion);
        //skillAdjustments[i].Serialize(car, SAVersion);
      };
    };
    {
      CString SAVersion = "SA0";
      int i, count;
      count = GetSpellAdjCount();
      car << SAVersion;
      car << count;
      for (i=0; i<count; i++)
      {
        GetSpellAdj(i)->Serialize(car, SAVersion);
        //spellAdjustments[i].Serialize(car, SAVersion);
      };
    };
    car << IsPreGen;
    car << CanBeSaved;
    car << HasLayedOnHandsToday;
    money.Serialize(car, version);
    car << NbrAttacks;
    icon.Serialize(car, version, rte.IconArtDir());
    car << iconIndex;
    car << origIndex;
    car << uniquePartyID;
    car << DisableTalkIfDead;
    car << TalkEvent;
    AS(car,TalkLabel);
    car << ExamineEvent;
    AS(car,ExamineLabel);

    m_spellbook.Serialize(car, version);

    //ar << detectingMagic;
    car << detectingInvisible;
    car << detectingTraps;
    car << m_spellEffects.GetCount();
    POSITION pos = m_spellEffects.GetHeadPosition();
    while (pos!=NULL)
    {
      m_spellEffects.GetNext(pos)->Serialize(car,version);
    };
    blockageData.Serialize(car);
  }
  else
  {
    int temp;
    car >> temp; // Possible character version
    if ( (temp & 0x80000000) != 0)
    {
      // This is a character version, not an index.
      characterVersion = temp;
      if (version >= 0.998917)
      {
      //ar >> uniqueKey;
        car >> preSpellNamesKey;
      };
    }
    else
    {
      characterVersion = 0; // This is a key, not a version.
      //uniqueKey = temp;
    };
    car >> type;
    //ar >> temp;
    //race((raceType)temp);
    car >> race;
    car >> temp;
    gender = (genderType)temp;   
    //ar >> temp;
    //charClass = (classType)temp;   
    car >> classID;
    car >> temp;
    alignment = (alignmentType)temp;   
    
    if (version >= _VERSION_0912_)
      car >> allowInCombat;

    car >> temp;
    status = (charStatusType)temp;   
#ifdef UAFEDITOR
    if (globalData.version <= 0.998115)
    {
      car >> temp;
      undeadType.Empty();
      if ( (temp>0) && (temp<14) )
      {
        undeadType = UndeadTypeText[temp];
      };
    }
    else
#endif
    {
      //undead = (undeadType)temp;
      //undeadLevel = temp;
      car >> undeadType;
    };
    car >> temp;
    creatureSize = (creatureSizeType)temp;   
    DAS(car,name);
#ifdef UAFEDITOR
    if (version >= VersionSpellNames)
#endif
    {
      car >> characterID;
    }
/*
    ar >> FighterExperience;
    ar >> ClericExperience;
    ar >> RangerExperience;
    ar >> PaladinExperience;
    ar >> MUExperience;
    ar >> ThiefExperience;
    ar >> DruidExperience;
*/
    car >> THAC0;
    car >> morale;
    car >> encumbrance;
    car >> maxEncumbrance;
    car >> m_AC;
    car >> hitPoints;
    car >> maxHitPoints;
    car >> nbrHitDice;
    car >> age;
    car >> maxAge;
    if (version >= _VERSION_0830_)
    {
      car >> birthday;
      SetBirthday(birthday); // force a range check
    }
    car >> m_iMaxCureDisease;
    car >> unarmedDieS;
    car >> unarmedNbrDieS;
    car >> unarmedBonus;
    car >> unarmedDieL;
    car >> unarmedNbrDieL;
    //ar >> ItemMask;
    car >> maxMovement;
    car >> readyToTrain;
    if (version >= _VERSION_0695_)
      car >> canTradeItems;

    if (version < 0.999702)
    {
      BYTE tmp;
      car >> tmp; strength = tmp;
      car >> tmp; strengthMod = tmp;
      car >> tmp; intelligence = tmp;
      car >> tmp; wisdom = tmp;
      car >> tmp; dexterity = tmp;
      car >> tmp; constitution= tmp;
      car >> tmp; charisma = tmp;
    }
    else
    {
      int tmp;
      car >> tmp; strength = tmp;
      car >> tmp; strengthMod = tmp;
      car >> tmp; intelligence = tmp;
      car >> tmp; wisdom = tmp;
      car >> tmp; dexterity = tmp;
      car >> tmp; constitution= tmp;
      car >> tmp; charisma = tmp;
    };
    strength_adjustment = 0x7fffffff;
    strengthMod_adjustment = 0x7fffffff;
    intelligence_adjustment = 0x7fffffff;
    wisdom_adjustment = 0x7fffffff;
    dexterity_adjustment = 0x7fffffff;
    constitution_adjustment = 0x7fffffff;
    charisma_adjustment = 0x7fffffff;
    car >> openDoors;
    car >> openMagicDoors;
    car >> BB_LG;
    car >> hitBonus;
    car >> dmgBonus;
    car >> magicResistance;
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      int trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
      car >> trash;
    };
#endif

    {
      int i, count;
      CString BSVersion;
      car >> BSVersion;
      car >> count;
      for (i=0; i<count; i++)
      {
        BASECLASS_STATS bcl;
        bcl.Serialize(car, BSVersion);
        baseclassStats.Add(bcl);
      };
    };
    {
      int i, count;
      CString SAVersion;
      car >> SAVersion;
      car >> count;
      for (i=0; i<count; i++)
      {
        SKILL_ADJ sa;
        int isk;
        sa.Serialize(car, SAVersion);
        isk = LocateSkillAdj(sa.skillID, sa.adjID);
        if (isk >= 0)
        {
          DeleteSkillAdj(isk);
        };
        skillAdjustments.Add(sa);
      };
    };
#ifdef UAFEDITOR
    if (version >= 0.9984)
#endif
    {
      int i, count;
      CString SAVersion;
      car >> SAVersion;
      car >> count;
      for (i=0; i<count; i++)
      {
        SPELL_ADJ sa;
        sa.Serialize(car, SAVersion);
        spellAdjustments.Add(sa);
      };
    };

    car >> IsPreGen;

    if (version >= _VERSION_0698_)
      car >> CanBeSaved;
    
    if (version >= _VERSION_0900_)
      car >> HasLayedOnHandsToday;

    if (version < _VERSION_0661_)
    {
      car >> temp; money.Coins[PlatinumType] = temp;
      car >> temp; money.Coins[GoldType] = temp;
      car >> temp; money.Coins[ElectrumType] = temp;
      car >> temp; money.Coins[SilverType] = temp;
      car >> temp; money.Coins[CopperType] = temp;
      car >> temp; { for (int g=0;g<temp;g++) money.AddGem(); }
      car >> temp; { for (int j=0;j<temp;j++) money.AddJewelry(); }
      if (version >= _VERSION_0660_)
      {
        car >> temp; money.Coins[CoinType6] = temp;
        car >> temp; money.Coins[CoinType7] = temp;
        car >> temp; money.Coins[CoinType8] = temp;
        car >> temp; money.Coins[CoinType9] = temp;
        car >> temp; money.Coins[CoinType10] = temp;
      }
    }
    else
    {
      money.Serialize(car, version);
    }
    car >> NbrAttacks;
    if (version < _VERSION_0640_)
    {
      CString tmp;
      long notused;
      DAS(car,tmp);
      car >> notused;
      //AddFolderToPath(tmp, rte.IconArtDir());
      StripFilenamePath(tmp);
      icon.filename = tmp;
      icon.picType = IconDib;
      icon.SetDefaults();
    }
    else
    {
      icon.Serialize(car, version, rte.IconArtDir());
      if (version <= _VERSION_0840_)
        icon.SetPre0840NonLoopDefaults();
      StripFilenamePath(icon.filename);
    }
    if (version >= _VERSION_0640_)
      car >> iconIndex;
    car >> origIndex;
    if (version < 0.998917)
    {
      preSpellNamesKey = origIndex;
    };
    car >> uniquePartyID;

    if (version >= _VERSION_0870_)
      car >> DisableTalkIfDead;
    if (version >= _VERSION_0662_)
      car >> TalkEvent;
    if (version >= _VERSION_0710_)
      DAS(car,TalkLabel);
    if (version >= _VERSION_0800_)
    {
      car >> ExamineEvent;
      DAS(car,ExamineLabel);
    }

    //ar >> spells;
    m_spellbook.Serialize(car, version);


    if (version >= _VERSION_06991_)
    { 
      if (version < _VERSION_0850_)
      {
        BOOL unused1;
        car >> unused1;
      }
      car >> detectingInvisible;
      car >> detectingTraps;
    }
    if (version >= _VERSION_0630_)
    {
      if (version < _VERSION_06991_)
      {
        //m_spellEffects.Serialize(ar);
        // 20121208 PRS
        int i, n;
        SPELL_EFFECTS_DATA sed;
        car >> n;
        for (i=0; i<n; i++)
        {
          sed.Serialize(car, version);
          m_spellEffects.AddTail(sed);
        };
      }
      else
      {
        m_spellEffects.RemoveAll();
        int count;
        car >> count;
        for (int i=0;i<count;i++)
        {
          SPELL_EFFECTS_DATA data;
          data.Serialize(car,version);
          m_spellEffects.AddTail(data);
          WriteDebugString("Loading spell effect for %s: %s\n",
                           GetName(), data.AffectedAttr());
        }
      }
    }
    if (version >= _VERSION_0696_)
      blockageData.Serialize(car);

    //UpdateSpellAbility();
    // 20101217TEMP m_spellbook.SortSpells();
  }
  smallPic.Serialize(car, version, rte.PicArtDir());
  if (!car.IsStoring())
  {
    if (version <= _VERSION_0840_)
      smallPic.SetPre0840LoopForeverDefaults();
  }
  myItems.Serialize(car,version);
  if (car.IsLoading() && (characterVersion == 0))
  {
    // Old versions included armor and such in m_AC.
    // We need to subtract it out for newer versions.
    m_AC -= myItems.GetProtectModForRdyItems();
  };
  specAbs.Serialize(car, version, this->name, "char");
  // specAbQueue: no need to save this data
  char_asl.Serialize(car, "CHARACTER_ATTRIBUTES");


  /* 20101217TEMP
  POSITION pos = m_spellbook.GetHeadPosition();
  while (pos!=NULL)
  {
    SPELL_DATA *pData = spellData.GetSpellData(m_spellbook.GetAt(pos).spell);
    if (pData!=NULL)
    {
      WriteDebugString("%s Spell \'%s\', sb key %u\n", 
                       GetName(),
                       pData->Name,
                       m_spellbook.GetAt(pos).spell);
    }
    m_spellbook.GetNext(pos);
  }
*/
  //m_Properties.Serialize(ar, version);
  
  // just to make sure we catch this for some older
  // character's that might come from saved games
  //strengthKey     = abilityData.GetKeyByIndex(0);
  //intelligenceKey = abilityData.GetKeyByIndex(1);
  //wisdomKey       = abilityData.GetKeyByIndex(2);
  //dexterityKey    = abilityData.GetKeyByIndex(3);
  //constitutionKey = abilityData.GetKeyByIndex(4);
  //charismaKey     = abilityData.GetKeyByIndex(5);

  // design settings that affect encumbrance might
  // have changed since character was last saved
  encumbrance = determineEffectiveEncumbrance();

  DisplayCurrSpellEffects("Serialize");
  spellAbility.Clear();  // force computation.
}

#ifdef UAFEDITOR

const char *JKEY_CHARVERSION = "charVersion";
extern const char *JKEY_TYPE;
extern const char *JKEY_RACE;
extern const char *JKEY_GENDER;
extern const char *JKEY_CLASS;
const char *JKEY_ALIGNMENT = "alignment";
const char *JKEY_ALLOWINCOMBAT = "allowInCombat";
const char *JKEY_UNDEADTYPE = "undeadType";
const char *JKEY_SIZE = "size";
extern const char *JKEY_NAME;
extern const char *JKEY_CHARACTERID;
const char *JKEY_THAC0 = "thac0";
const char *JKEY_MORALE = "morale";
const char *JKEY_ENCUMBRANCE = "encumbrance";
const char *JKEY_MAXENCUMBRANCE = "maxencumbrance";
const char *JKEY_AC = "ac";
extern const char *JKEY_HP;
const char *JKEY_MAXHP = "maxHP";
const char *JKEY_NBRHITDICE = "nbrHitDice";
const char *JKEY_AGE = "age";
const char *JKEY_MAXAGE = "maxAge";
const char *JKEY_BIRTHDAY = "birthday";
const char *JKEY_MAXCUREDISEASE = "maxCureDisease";
const char *JKEY_UNARMEDDICESIDES = "unarmedDiceS";
const char *JKEY_UNARMEDNBRDICES = "unarmedNbrDiceS";
const char *JKEY_UNARMEDDICEBONUS = "unarmedDiceBonus";
const char *JKEY_UNARMEDDICEL = "unarmedDiceL";
const char *JKEY_UNARMEDNBRDICEL = "unarmedNbrDiceL";
const char *JKEY_MAXMOVEMENT = "maxMovement";
const char *JKEY_READYTOTRAIN = "readyToTrain";
const char *JKEY_CANTRADEITEMS = "canTradeItems";
const char *JKEY_STR = "str";
const char *JKEY_STRMOD = "strMod";
const char *JKEY_INT = "int";
const char *JKEY_WIS = "wis";
const char *JKEY_DEX = "dex";
const char *JKEY_CON = "con";
const char *JKEY_CHA = "cha";
const char *JKEY_OPENDOORS = "openDoors";
const char *JKEY_OPENMAGICDOORS = "openMagicDoors";
const char *JKEY_BBLG = "bblg";
const char *JKEY_HITBONUS = "hitBonus";
const char *JKEY_DMGBONUS = "dmgBonus";
const char *JKEY_MAGICRESISTANCE = "magicResistance";
const char *JKEY_ISPREGEN = "isPregen";
const char *JKEY_CANBESAVED = "canBeSaved";
const char *JKEY_HASLAYEDONHANDSTODAY = "hasLayedOnHandsToday";
const char *JKEY_NBRATTACKS = "nbrAttacks";
const char *JKEY_ICONINDEX = "iconIndex";
const char *JKEY_ORIGINDEX = "origIndex";
const char *JKEY_UNIQUEPARTYID = "uniquePartyID";
const char *JKEY_DISABLETALKIFDEAD = "disableTalkIfDead";
const char *JKEY_TALKEVENT = "talkEvent";
const char *JKEY_TALKLABEL = "talkLabel";
const char *JKEY_EXAMINEEVENT = "examineEvent";
const char *JKEY_EXAMINELABEL = "examineLabel";
const char *JKEY_DETECTINGINVISIBLE = "detectingInvisible";
const char *JKEY_DETECTINGTRAPS = "detectingTraps";
const char *JKEY_BASECLASSSTATS = "baseclassStats";
const char *JKEY_SKILLADJUSTMENTS = "skillAdjustments";
const char *JKEY_SPELLADJUSTMENTS = "spellAdjustments";
const char *JKEY_ICON = "icon"; 
const char *JKEY_SPELLEFFECTS = "spellEffects";
const char *JKEY_STATUS = "status";
extern const char *JKEY_SMALLPIC;
extern const char *JKEY_POSSESSIONS;

void CHARACTER::Export(JWriter& jw)
{
  // This data can be serialized along with the globalData, and
  // also separately as a 'CharName.CHR' file.
  // The version specified distinguishes between saved char file 
  // version and game.dat file version
  //
  jw.StartList();
  jw.NameAndValue(JKEY_CHARVERSION, CHARACTER_VERSION);
    //ar << uniqueKey;
  jw.NameAndValue(JKEY_TYPE, type);
    //temp = m_race;
    //ar << temp;
  jw.NameAndValue(JKEY_RACE, race);
  jw.NameAndEnum(JKEY_GENDER, gender);
    //ar << temp;
    //temp = charClass;
  jw.NameAndValue(JKEY_CLASS, classID);
    //car << temp;
  jw.NameAndEnum(JKEY_ALIGNMENT, alignment);
  jw.NameAndValue(JKEY_ALLOWINCOMBAT, allowInCombat);
    //temp = undead;
    //temp = undeadLevel;
    //car << temp;
  jw.NameAndValue(JKEY_UNDEADTYPE, undeadType);
  jw.NameAndEnum(JKEY_SIZE, creatureSize);

  jw.NameAndValue(JKEY_NAME, name);
  jw.NameAndValue(JKEY_CHARACTERID, characterID);
  jw.NameAndValue(JKEY_THAC0, THAC0);
  jw.NameAndValue(JKEY_MORALE, morale);
  jw.NameAndValue(JKEY_ENCUMBRANCE, encumbrance);
  jw.NameAndValue(JKEY_MAXENCUMBRANCE, maxEncumbrance);
  jw.NameAndValue(JKEY_AC, m_AC);
  jw.NameAndValue(JKEY_HP, hitPoints);
  jw.NameAndEnum (JKEY_STATUS, status);
  jw.NameAndValue(JKEY_MAXHP, maxHitPoints);
  jw.NameAndValue(JKEY_NBRHITDICE, nbrHitDice);
  jw.NameAndValue(JKEY_AGE, age);
  jw.NameAndValue(JKEY_MAXAGE, maxAge);
  jw.NameAndValue(JKEY_BIRTHDAY, birthday);
  jw.NameAndValue(JKEY_MAXCUREDISEASE, m_iMaxCureDisease);
  jw.NameAndValue(JKEY_UNARMEDDICESIDES, unarmedDieS);
  jw.NameAndValue(JKEY_UNARMEDNBRDICES, unarmedNbrDieS);
  jw.NameAndValue(JKEY_UNARMEDDICEBONUS, unarmedBonus);
  jw.NameAndValue(JKEY_UNARMEDDICEL, unarmedDieL);
  jw.NameAndValue(JKEY_UNARMEDNBRDICEL, unarmedNbrDieL);
    //ar << ItemMask;
  jw.NameAndValue(JKEY_MAXMOVEMENT, maxMovement);
  jw.NameAndValue(JKEY_READYTOTRAIN, readyToTrain);
  jw.NameAndValue(JKEY_CANTRADEITEMS, canTradeItems);

  jw.NameAndValue(JKEY_STR, GetPermStr());
  jw.NameAndValue(JKEY_STRMOD, GetPermStrMod());
  jw.NameAndValue(JKEY_INT, GetPermInt());
  jw.NameAndValue(JKEY_WIS, GetPermWis());
  jw.NameAndValue(JKEY_DEX, GetPermDex());
  jw.NameAndValue(JKEY_CON, GetPermCon());
  jw.NameAndValue(JKEY_CHA, GetPermCha());

  jw.NameAndValue(JKEY_OPENDOORS, openDoors);
  jw.NameAndValue(JKEY_OPENMAGICDOORS, openMagicDoors);
  jw.NameAndValue(JKEY_BBLG, BB_LG);
  jw.NameAndValue(JKEY_HITBONUS, hitBonus);
  jw.NameAndValue(JKEY_DMGBONUS, dmgBonus);
  jw.NameAndValue(JKEY_MAGICRESISTANCE, magicResistance);
  {
    //CString version = "BS0";
    int i, count;
    //car << version;
    count = GetBaseclassStatsCount();
    //car << count;
    jw.StartArray(JKEY_BASECLASSSTATS);
    for (i=0; i<count; i++)
    {
      jw.NextEntry();
      GetBaseclassStats(i)->Export(jw);
      //baseclassStats[i].Export(jw);
    };
    jw.EndArray();
  }
  {
    //CString version = "SA0";
    int i, count;
    count = GetSkillAdjCount();
    //car << version;
    //car << count;
    jw.StartArray(JKEY_SKILLADJUSTMENTS);
    for (i=0; i<count; i++)
    {
      jw.NextEntry();
      GetSkillAdj(i)->Export(jw);
      //skillAdjustments[i].Export(jw);
    };
    jw.EndArray();
  };
  {
    //CString version = "SA0";
    int i, count;
    count = GetSpellAdjCount();
    //car << version;
    //car << count;
    jw.StartArray(JKEY_SPELLADJUSTMENTS);
    for (i=0; i<count; i++)
    {
      jw.NextEntry();
      GetSpellAdj(i)->Export(jw);
      //spellAdjustments[i].Export(jw);
    };
    jw.EndArray();
  };
  jw.NameAndValue(JKEY_ISPREGEN, IsPreGen);
  jw.NameAndValue(JKEY_CANBESAVED, CanBeSaved);
  jw.NameAndValue(JKEY_HASLAYEDONHANDSTODAY, HasLayedOnHandsToday);
  money.Export(jw);
  jw.NameAndValue(JKEY_NBRATTACKS, NbrAttacks);
  icon.Export(jw, JKEY_ICON);
  jw.NameAndValue(JKEY_ICONINDEX, iconIndex);
  jw.NameAndValue(JKEY_ORIGINDEX, origIndex);
  jw.NameAndValue(JKEY_UNIQUEPARTYID, uniquePartyID);
  jw.NameAndValue(JKEY_DISABLETALKIFDEAD, DisableTalkIfDead);
  jw.NameAndValue(JKEY_TALKEVENT, TalkEvent);
  jw.NameAndValue(JKEY_TALKLABEL, TalkLabel);
  jw.NameAndValue(JKEY_EXAMINEEVENT, ExamineEvent);
  jw.NameAndValue(JKEY_EXAMINELABEL, ExamineLabel);

  m_spellbook.Export(jw);

    //ar << detectingMagic;
  jw.NameAndValue(JKEY_DETECTINGINVISIBLE, detectingInvisible);
  jw.NameAndValue(JKEY_DETECTINGTRAPS, detectingTraps);

    //car << m_spellEffects.GetCount();
  {
    POSITION pos = m_spellEffects.GetHeadPosition();
    jw.StartArray(JKEY_SPELLEFFECTS);
    while (pos!=NULL)
    {
      jw.NextEntry();
      m_spellEffects.GetNext(pos)->Export(jw);
    };
    jw.EndArray();
  };


  blockageData.Export(jw);

  smallPic.Export(jw, JKEY_SMALLPIC);
  myItems.Export(jw, JKEY_POSSESSIONS);
  specAbs.Export(jw);
  // specAbQueue: no need to save this data
  char_asl.Export(jw);


  //m_Properties.Serialize(ar, version);
  
  // just to make sure we catch this for some older
  // character's that might come from saved games
  //strengthKey     = abilityData.GetKeyByIndex(0);
  //intelligenceKey = abilityData.GetKeyByIndex(1);
  //wisdomKey       = abilityData.GetKeyByIndex(2);
  //dexterityKey    = abilityData.GetKeyByIndex(3);
  //constitutionKey = abilityData.GetKeyByIndex(4);
  //charismaKey     = abilityData.GetKeyByIndex(5);

  // design settings that affect encumbrance might
  // have changed since character was last saved

  DisplayCurrSpellEffects("Serialize");
  jw.EndList();
}

void CHARACTER::Import(JReader& jr)
{
  // This data can be serialized along with the globalData, and
  // also separately as a 'CharName.CHR' file.
  // The version specified distinguishes between saved char file 
  // version and game.dat file version
  //
  int characterVersion;
  jr.StartList();
  jr.NameAndValue(JKEY_CHARVERSION, characterVersion);
    //ar << uniqueKey;
  jr.NameAndValue(JKEY_TYPE, type);
    //temp = m_race;
    //ar << temp;
  jr.NameAndValue(JKEY_RACE, race);
  jr.NameAndEnum(JKEY_GENDER, gender);
    //ar << temp;
    //temp = charClass;
  jr.NameAndValue(JKEY_CLASS, classID);
    //car << temp;
  jr.NameAndEnum(JKEY_ALIGNMENT, alignment);
  jr.NameAndValue(JKEY_ALLOWINCOMBAT, allowInCombat);
    //temp = undead;
    //temp = undeadLevel;
    //car << temp;
  jr.NameAndValue(JKEY_UNDEADTYPE, undeadType);
  jr.NameAndEnum(JKEY_SIZE, creatureSize);

  jr.NameAndValue(JKEY_NAME, name);
  jr.NameAndValue(JKEY_CHARACTERID, characterID);
  jr.NameAndValue(JKEY_THAC0, THAC0);
  jr.NameAndValue(JKEY_MORALE, morale);
  jr.NameAndValue(JKEY_MAXENCUMBRANCE, maxEncumbrance);
  jr.NameAndValue(JKEY_AC, m_AC);
  jr.NameAndValue(JKEY_HP, hitPoints);
  jr.NameAndValue(JKEY_MAXHP, maxHitPoints);
  jr.NameAndValue(JKEY_NBRHITDICE, nbrHitDice);
  jr.NameAndValue(JKEY_AGE, age);
  jr.NameAndValue(JKEY_MAXAGE, maxAge);
  jr.NameAndValue(JKEY_BIRTHDAY, birthday);
  jr.NameAndValue(JKEY_MAXCUREDISEASE, m_iMaxCureDisease);
  jr.NameAndValue(JKEY_UNARMEDDICESIDES, unarmedDieS);
  jr.NameAndValue(JKEY_UNARMEDNBRDICES, unarmedNbrDieS);
  jr.NameAndValue(JKEY_UNARMEDDICEBONUS, unarmedBonus);
  jr.NameAndValue(JKEY_UNARMEDDICEL, unarmedDieL);
  jr.NameAndValue(JKEY_UNARMEDNBRDICEL, unarmedNbrDieL);
    //ar << ItemMask;
  jr.NameAndValue(JKEY_MAXMOVEMENT, maxMovement);
  jr.NameAndValue(JKEY_READYTOTRAIN, readyToTrain);
  jr.NameAndValue(JKEY_CANTRADEITEMS, canTradeItems);

  {
    int Str, Strmod, Wis, Dex, Con, Cha, Int;
    jr.NameAndValue(JKEY_STR, Str);
    SetPermStr(Str);
    jr.NameAndValue(JKEY_STRMOD, Strmod);
    SetPermStrMod(Strmod);
    jr.NameAndValue(JKEY_INT, Int);
    SetPermInt(Int);
    jr.NameAndValue(JKEY_WIS, Wis);
    SetPermWis(Wis);
    jr.NameAndValue(JKEY_DEX, Dex);
    SetPermDex(Dex);
    jr.NameAndValue(JKEY_CON, Con);
    SetPermCon(Con);
    jr.NameAndValue(JKEY_CHA, Cha);
    SetPermCha(Cha);
  };
  jr.NameAndValue(JKEY_ENCUMBRANCE, encumbrance);
  jr.NameAndValue(JKEY_OPENDOORS, openDoors);
  jr.NameAndValue(JKEY_OPENMAGICDOORS, openMagicDoors);
  jr.NameAndValue(JKEY_BBLG, BB_LG);
  jr.NameAndValue(JKEY_HITBONUS, hitBonus);
  jr.NameAndValue(JKEY_DMGBONUS, dmgBonus);
  jr.NameAndValue(JKEY_MAGICRESISTANCE, magicResistance);
  {
    //CString version = "BS0";
    //car << version;
    //car << count;
    jr.StartArray(JKEY_BASECLASSSTATS);
    while (jr.Optional(), jr.NextEntry())
    {
      BASECLASS_STATS bcs;
      bcs.Import(jr);
      baseclassStats.Add(bcs);
    };
    jr.EndArray();
  }
  {
    //CString version = "SA0";
    //car << version;
    //car << count;
    jr.StartArray(JKEY_SKILLADJUSTMENTS);
    while (jr.Optional(), jr.NextEntry())
    {
      SKILL_ADJ sa;
      int ski;
      sa.Import(jr);
      ski = LocateSkillAdj(sa.skillID, sa.adjID);
      if (ski >= 0)
      {
        DeleteSkillAdj(ski);
      };
      skillAdjustments.Add(sa);
      //skillAdjustments[i].Import(jr);
    };
    jr.EndArray();
  };
  {
    //CString version = "SA0";
    int i, count;
    count = GetSpellAdjCount();
    //car << version;
    //car << count;
    jr.StartArray(JKEY_SPELLADJUSTMENTS);
    for (i=0; i<count; i++)
    {
      jr.NextEntry();
      GetSpellAdj(i)->Import(jr);
      //spellAdjustments[i].Import(jr);
    };
    jr.EndArray();
  };
  jr.NameAndValue(JKEY_ISPREGEN, IsPreGen);
  jr.NameAndValue(JKEY_CANBESAVED, CanBeSaved);
  jr.NameAndValue(JKEY_HASLAYEDONHANDSTODAY, HasLayedOnHandsToday);
  money.Import(jr);
  jr.NameAndValue(JKEY_NBRATTACKS, NbrAttacks);
  icon.Import(jr, JKEY_ICON);
  jr.NameAndValue(JKEY_ICONINDEX, iconIndex);
  jr.NameAndValue(JKEY_ORIGINDEX, origIndex);
  jr.NameAndValue(JKEY_UNIQUEPARTYID, uniquePartyID);
  jr.NameAndValue(JKEY_DISABLETALKIFDEAD, DisableTalkIfDead);
  jr.NameAndValue(JKEY_TALKEVENT, TalkEvent);
  jr.NameAndValue(JKEY_TALKLABEL, TalkLabel);
  jr.NameAndValue(JKEY_EXAMINEEVENT, ExamineEvent);
  jr.NameAndValue(JKEY_EXAMINELABEL, ExamineLabel);

  m_spellbook.Import(jr);

    //ar << detectingMagic;
  jr.NameAndValue(JKEY_DETECTINGINVISIBLE, detectingInvisible);
  jr.NameAndValue(JKEY_DETECTINGTRAPS, detectingTraps);

    //car << m_spellEffects.GetCount();
  {
    POSITION pos = m_spellEffects.GetHeadPosition();
    jr.StartArray(JKEY_SPELLEFFECTS);
    while (pos!=NULL)
    {
      jr.NextEntry();
      m_spellEffects.GetNext(pos)->Import(jr);
    };
    jr.EndArray();
  };


  blockageData.Import(jr);

  smallPic.Import(jr, JKEY_SMALLPIC);
  myItems.Import(jr, JKEY_POSSESSIONS);
  specAbs.Import(jr);
  // specAbQueue: no need to save this data
  char_asl.Import(jr);


  //m_Properties.Serialize(ar, version);
  
  // just to make sure we catch this for some older
  // character's that might come from saved games
  //strengthKey     = abilityData.GetKeyByIndex(0);
  //intelligenceKey = abilityData.GetKeyByIndex(1);
  //wisdomKey       = abilityData.GetKeyByIndex(2);
  //dexterityKey    = abilityData.GetKeyByIndex(3);
  //constitutionKey = abilityData.GetKeyByIndex(4);
  //charismaKey     = abilityData.GetKeyByIndex(5);

  // design settings that affect encumbrance might
  // have changed since character was last saved
  jr.NameAndEnum (JKEY_STATUS, status);

  DisplayCurrSpellEffects("Serialize");
  jr.EndList();
}


// event descriptive text for editor
//*****************************************************************************
//    NAME: CHARACTER::GetNPCEventText
//
// PURPOSE: 
//
//*****************************************************************************
CString CHARACTER::GetNPCEventText() 
{ 
  CString tmp;
  tmp.Format("NPC Event: %s", GetName());
  return tmp;
}
#endif

//*****************************************************************************
// NAME:    CHARACTER::IsReadyToTrain
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::IsReadyToTrain(const BASECLASS_STATS *pBaseclassStats) const
{
  const BASECLASS_ID *pbci;
  //const BASECLASS_STATS *pbcs;
  int allowedLevel, currentLevel;
  pbci = &pBaseclassStats->baseclassID;
  // Now check the characer's experience in this particular baseclass
  //pbcs = PeekBaseclassStats(*pbci);
  allowedLevel = GetAllowedLevel(*pbci);
  currentLevel = pBaseclassStats->currentLevel;
  if (allowedLevel > currentLevel) return TRUE;
  return FALSE;
}

BOOL CHARACTER::IsReadyToTrain() const
{
  if (GetType() == MONSTER_TYPE) return FALSE;

  //classType ctype = GetAdjClass();
  //CLASS_ID AdjClassID = GetAdjClass();
/*
  //if (isFighter(ctype))
  if (isFighter(AdjClassID))
  {
    //if (getFighterLevel(GetAdjFighterExp()) > FighterLevel)
    if (getFighterLevel(GetAdjFighterExp()) > GetCurrLevel())
      readyToTrain = TRUE;
  }
  //if (isCleric(ctype))
  if (isCleric(AdjClassID))
  {
    if (getClericLevel(ClericExperience) > ClericLevel)
      readyToTrain = TRUE;
  }
  //if (isMagicUser(ctype))
  if (isMagicUser(AdjClassID))
  {
    if (getMagicUserLevel(MUExperience) > MULevel)
      readyToTrain = TRUE;
  }
  //if (isThief(ctype))
  if (isThief(AdjClassID))
  {
    if (getThiefLevel(ThiefExperience) > ThiefLevel)
      readyToTrain = TRUE;
  }
  //if (isRanger(ctype))
  if (isRanger(AdjClassID))
  {
    if (getRangerLevel(RangerExperience) > RangerLevel)
      readyToTrain = TRUE;
  }
  //if (isPaladin(ctype))
  if (isPaladin(AdjClassID))
  {
    if (getPaladinLevel(PaladinExperience) > PaladinLevel)
      readyToTrain = TRUE;
  }
  //if (isDruid(ctype))
  if (isDruid(AdjClassID))
  {
    if (getDruidLevel(DruidExperience) > DruidLevel)
      readyToTrain = TRUE;
  }
*/
  {
    int i, n;
    n = GetBaseclassStatsCount();
    for (i=0; i<n; i++)
    {
      const BASECLASS_STATS *pBaseclassStats = PeekBaseclassStats(i);
      // Examine all of the character's baseclasses.
      // See if he has enough experience to train in this baseclasses
      if (IsReadyToTrain(pBaseclassStats))
      {
        return TRUE;
      };
    };
  };
  return FALSE;
}

//*****************************************************************************
// NAME: CHARACTER::GetHealingPointsNeeded
//
// PURPOSE:  
//*****************************************************************************
int CHARACTER::GetHealingPointsNeeded()
{
  return (GetMaxHitPoints() - GetAdjHitPoints());
}

//*****************************************************************************
// NAME: CHARACTER::CanBeModified
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanBeModified()
{
  const BASECLASS_STATS *pbcs;
  int i, n, total;
  if (GetType() == MONSTER_TYPE) return FALSE;
  if (GetStatus()!=Okay) return FALSE;
  n = GetBaseclassStatsCount();
  total = 0;
  if (globalData.startExpType==START_EXP_VALUE)  
  {
/*
    int total = FighterExperience+
                ClericExperience+
                RangerExperience+
                PaladinExperience+
                MUExperience+
                ThiefExperience+
                DruidExperience;
*/
   for (i=0; i<n; i++)
   {
      pbcs = PeekBaseclassStats(i);
      total += pbcs->CurExperience();
   };
   if (total > 0)
      return (total == globalData.startExp);
  }
  else
  {
/*
    int total = FighterLevel+
                ClericLevel+
                RangerLevel+
                PaladinLevel+
                MULevel+
                ThiefLevel+
                DruidLevel;
*/
   for (i=0; i<n; i++)
   {
      pbcs = PeekBaseclassStats(i);
      total += pbcs->currentLevel;
   };
   if (total > 0)
      return (total == globalData.startExp);
  }
  return TRUE;
}

// ****************************************************************************
// NAME: CHARACTER::ComputeAIBaseclass
//
// PURPOSE:  Search baseclasses for Special Ability named "AIBaseclass"
// ****************************************************************************
void CHARACTER::ComputeAIBaseclass(void)
{
  if (AIBaseclass != -1) return;  // 'Tis already been done!
  int i, n;
  const CLASS_DATA *pClass;
  AIBaseclass = 0;  // Assume none is found.
  pClass = classData.PeekClass(classID);
  if (pClass == NULL) return;
  n = pClass->BaseclassList()->GetCount();
  for (i=0; i<n; i++)
  {
    BASECLASS_ID baseclassID;
    const BASE_CLASS_DATA *pBaseclass;
    CStringPAIR *pSpecAb;
    int value;
    baseclassID = *pClass->PeekBaseclassID(i);
    pBaseclass = baseclassData.PeekBaseclass(baseclassID);
    if (pBaseclass == NULL) continue;
    pSpecAb = pBaseclass->m_specAbs.FindAbility("$AIBaseclass");
    if (pSpecAb == NULL) continue;
    value = atoi(pSpecAb->Value());
    AIBaseclass = (AIBaseclass | value) & 0xffff;
  };
}


//*****************************************************************************
// NAME: CHARACTER::GetClassLevel
//
// PURPOSE:  lookup current level based on class flag
//*****************************************************************************
// dont pass type charClass to this function!
//BYTE CHARACTER::GetClassLevel(WORD ClassFlag) const
BYTE CHARACTER::GetBaseclassLevel(const BASECLASS_ID& baseclassID) const
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  int i;
  const BASECLASS_STATS *pBaseclassStats;
  i = LocateBaseclassStats(baseclassID);
  if (i < 0) return 0;
  pBaseclassStats = PeekBaseclassStats(i);

#ifdef OldDualClass20180126
  if (    (IsDualClass())
       && (CanUsePrevClass())
       && (pBaseclassStats->currentLevel <= 0))
  {
    return pBaseclassStats->previousLevel;
  }
#else
  if (!CanUseBaseclass(pBaseclassStats)) return 0;
  if (pBaseclassStats->currentLevel > 0)
  {
    return pBaseclassStats->currentLevel;
  };
  return pBaseclassStats->previousLevel;
#endif
}


// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::GetClassPreDrainLevel
//
// PURPOSE: 
//
//*****************************************************************************
//BYTE CHARACTER::GetClassPreDrainLevel(WORD ClassFlag) const
BYTE CHARACTER::GetBaseclassPreDrainLevel(const BASECLASS_ID& baseclassID) const
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  die ("Not Needed?"); //Not Implemented(0xbe213, false);
  return 0;
}

// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::GetCurrentLevel
//
// PURPOSE: 
//
//*****************************************************************************
//BYTE CHARACTER::GetCurrentLevel(WORD ClassFlag) const
BYTE CHARACTER::GetCurrentLevel(const BASECLASS_ID& baseclassID) const
{
  //ASSERT(ClassFlag!=0);
  if (GetType() == MONSTER_TYPE) 
  {
    //MONSTER_DATA *pData = monsterData.GetMonsterData(origIndex);
    const MONSTER_DATA *pMonster;
    pMonster = monsterData.PeekMonster(monsterID);
    //if (pData == NULL) return 1;
    if (pMonster == NULL) return 1;

    //double hd = pData->Hit_Dice; 
    double hd = pMonster->Hit_Dice; 

    //if (!pData->UseHitDice) // represents hit points instead of hit dice
    if (!pMonster->UseHitDice) // represents hit points instead of hit dice
      hd = (hd/FIGHTER_HIT_DIE)+1;

    if (hd < 1.0) hd = 1.0;    
    if (hd > 40.0) hd = 40.0;  // PRS 20120705  Else crash when level > 40
    return hd;    
  }

  BYTE val=0;
  //if (ClassFlag==0)
    //val = GetClassLevel(ClassFlag);
  val = GetBaseclassLevel(baseclassID);
  return val;
}



//*****************************************************************************
//    NAME: CHARACTER::GetCurrentLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::CurrentBaseclassLevel(unsigned int baseClass) const
int CHARACTER::CurrentBaseclassLevel(const BASECLASS_ID& baseclassID) const
{
  ASSERT(GetType() != MONSTER_TYPE);
  //switch (baseClass)
  //{
/*
  case Fighter:  return GetClassLevel(FighterFlag);
  case Cleric:   return GetClassLevel(ClericFlag);
  case Ranger:   return GetClassLevel(RangerFlag);
  case Paladin:  return GetClassLevel(PaladinFlag);
  case MagicUser:return GetClassLevel(MagicUserFlag);
  case Thief:    return GetClassLevel(ThiefFlag);
  case Druid:    return GetClassLevel(DruidFlag);
*/
  die ("Not Needed?"); // Not Implemented(0x43ab97, false);
  
  //};
  return 0;
}



/*
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::GetPrevLevel
//
// PURPOSE: 
//
//*****************************************************************************
//BYTE CHARACTER::GetPrevLevel(WORD ClassFlag) const
BYTE CHARACTER::GetPrevLevel(const BASECLASS_ID& baseclassID) const
{
  //ASSERT(ClassFlag!=0);
  //BYTE val;
  //val = GetClassPrevLevel(ClassFlag);
  //return val;
  return GetBaseclassPrevLevel(baseclassID);
}
*/
/*
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::GetPreDrainLevel
//
// PURPOSE: 
//
//*****************************************************************************
BYTE CHARACTER::GetPreDrainLevel(const BASECLASS_ID& baseclassID) const
{
  //ASSERT(ClassFlag!=0);
  //BYTE val=0;
  //val = GetClassPreDrainLevel(ClassFlag);
  //return val;
  return GetBaseclassPreDrainLevel(baseclassID);
}
*/

// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::SetCurrentLevel
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::SetCurrentLevel(WORD ClassFlag, int lvl)
void CHARACTER::SetCurrentLevel(const BASECLASS_ID& baseclassID, int lvl)
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  int i;
  i = LocateBaseclassStats(baseclassID);
  if (i < 0)
  {
    MsgBoxInfo("Cannot locate baseclass stats");
  }
  else
  {
    BASECLASS_STATS *pBaseclassStats;
    pBaseclassStats = GetBaseclassStats(i);
#ifdef OldDualClass20180126
    if (pBaseclassStats->currentLevel != lvl)
    {
      temp__canUsePrevClass = -1;
    };
#endif
    pBaseclassStats->currentLevel = lvl;
  };
}

/*
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::SetPrevLevel
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::SetPrevLevel(WORD ClassFlag, int lvl)
void CHARACTER::SetPrevLevel(const BASECLASS_ID& baseclassID, int lvl)
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  die("Not Needed?"); //Not Implemented(0x43aa1b, false);
}
*/
/*
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::SetPreDrainLevel
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::SetPreDrainLevel(WORD ClassFlag, int lvl)
void CHARACTER::SetPreDrainLevel(const BASECLASS_ID& baseclassID, int lvl)
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  die("Not Needed?"); //Not Implemented(0x031ab8, false);
}
*/
/*
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::IncCurrentLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::IncCurrentLevel(WORD ClassFlag, int inc)
int CHARACTER::IncCurrentLevel(const BASECLASS_ID& baseclassID, int inc)
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  die("Not Needed?"); //Not Implemented(0xee9223, false);
  return 0;
}
*/
/*
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::IncPrevLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::IncPrevLevel(WORD ClassFlag, int inc)
int CHARACTER::IncPrevLevel(const BASECLASS_ID& baseclassID, int inc)
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  die("Not Needed?"); //Not Implemented(0x1743bb, false);
  return 0;
}
*/
/*
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::IncPreDrainLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::IncPreDrainLevel(WORD ClassFlag, int inc)
int CHARACTER::IncPreDrainLevel(const BASECLASS_ID& baseclassID, int inc)
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
  die("Not Needed?"); //Not Implemented(0x2cda87, false);
  return 0;
}
*/
// dont pass type charClass to this function!
//*****************************************************************************
//    NAME: CHARACTER::IsUsingBaselass
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL CHARACTER::IsUsingClass(WORD ClassFlag)
BOOL CHARACTER::IsUsingBaseclass(const BASECLASS_ID& baseclassID) const
{
/*
  ASSERT( NumClassBits(ClassFlag) == 1 );
  switch (ClassFlag)
  {
    case MagicUserFlag:      
      return isMagicUser(GetAdjClass());
    case ClericFlag:
      return isCleric(GetAdjClass());
    case ThiefFlag:
      return isThief(GetAdjClass());
    case FighterFlag:
      return isFighter(GetAdjClass());
    case PaladinFlag:
      return isPaladin(GetAdjClass());
    case RangerFlag:
      return isRanger(GetAdjClass());
    case DruidFlag:
      return isDruid(GetAdjClass());
    default:
      ASS ERT(FALSE);
      return FALSE;
  }
*/
  const CLASS_DATA *pClass;
  pClass = classData.PeekClass(classID);
  return pClass->LocateBaseclass(baseclassID) >= 0;
}

//*****************************************************************************
// NAME: CHARACTER::GetBestMemorizedHealingSpell
//
// PURPOSE:  
//*****************************************************************************
//DWORD CHARACTER::GetBestMemorizedHealingSpell(GLOBAL_SPELL_ID &index)
DWORD CHARACTER::GetBestMemorizedHealingSpell(SPELL_ID *pSpellID) const
{
  double total;
  double best = 0.0;
  //GLOBAL_SPELL_ID bestIndex;  // Initialized to NO_SPELL
  SPELL_ID bestSpell;  // Initialized to NO_SPELL

  //index.Clear();
  pSpellID->Clear();
  const SPELL_DATA *pSpell;
/*
  {
    POSITION pos = m_spellbook.GetHeadPosition();

    while (pos != NULL)
    {
      if (m_spellbook.PeekAtPos(pos).memorized)
      {
        //if ((data = spellData.GetSpellData(m_spellbook.PeekAtPos(pos).spell)) != NULL)
        if ((data = spellData.GetSpellData(m_spellbook.PeekAtPos(pos).spellID)) != NULL)
        {
          POSITION epos = data->m_EffectedAttributes.GetHeadPosition();
          while (epos != NULL)
          {
            SPELL_EFFECTS_DATA effect = data->m_EffectedAttributes.GetAt(epos);
            if (effect.affectedAttr=="$CHAR_HITPOINTS")
            {
              if (effect.changeData.MaxRoll(total))
              {
                if (total > best)
                {
                  best = total;
                  //bestIndex = m_spellbook.PeekAtPos(pos).spell;
                  bestIndex = m_spellbook.PeekAtPos(pos).spellID;
                }
              }
            }
            data->m_EffectedAttributes.GetNext(epos);
          }
          //total = (data->Heal_Dice * data->Heal_Dice_Nbr) + data->Heal_Bonus;
          //total += (GetCurrentLevel(data->BaseClassMask) * data->Heal_Bonus_Level);
          //if (total > (DWORD)data->Heal_Max) total = data->Heal_Max;
        }
      }
      m_spellbook.GetNext(pos);
    }
  };
*/
  
  {
    int i, n;
    n = m_spellbook.GetCount();

    for (i=0; i<n; i++)
    {
      if (m_spellbook.PeekCharacterSpell(i)->IsMemorized())
      {
        const CHARACTER_SPELL *pCharSp;
        pCharSp = m_spellbook.PeekCharacterSpell(i);
        pSpell = spellData.PeekSpell(pCharSp->spellID);
        if (pSpell != NULL)
        {
          POSITION epos = pSpell->m_EffectedAttributes.GetHeadPosition();
          while (epos != NULL)
          {
            SPELL_EFFECTS_DATA effect = *pSpell->m_EffectedAttributes.PeekAt(epos);
            //if (effect.affectedAttr=="$CHAR_HITPOINTS")
            if (effect.affectedAttr_IFKEY==CHAR_HITPOINTS)
            {
              if (effect.changeData.MaxRoll(total))
              {
                if (total > best)
                {
                  best = total;
                  //bestIndex = m_spellbook.PeekAtPos(pos).spell;
                  bestSpell = m_spellbook.PeekCharacterSpell(i)->spellID;
                }
              }
            }
            pSpell->m_EffectedAttributes.PeekNext(epos);
          }
          //total = (data->Heal_Dice * data->Heal_Dice_Nbr) + data->Heal_Bonus;
          //total += (GetCurrentLevel(data->BaseClassMask) * data->Heal_Bonus_Level);
          //if (total > (DWORD)data->Heal_Max) total = data->Heal_Max;
        };
      };
    };
  };



  if (best > 0.0)
    *pSpellID = bestSpell;

  return (DWORD)best;
}

//*****************************************************************************
// NAME:    CHARACTER::DetermineHitDiceBonus
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int CHARACTER::DetermineHitDiceBonus(const BASECLASS_ID& baseclassID)
{
   if (GetType() == MONSTER_TYPE) return 0;


/*
   int bonus = determineHitDiceBonus(GetAdjCon(), isFighter(GetAdjClass()));
*/
  int prime = 0;
  int result = 0;
  int i, n;
  CLASS_ID AdjClassID;
  const CLASS_DATA *pClass;
  AdjClassID = GetAdjClass();
  pClass = classData.PeekClass(AdjClassID);
  if (pClass == NULL) return 0;

  n = pClass->GetHitDiceLevelBonusCount();
  for (i=0; i<n; i++)
  {
    const HIT_DICE_LEVEL_BONUS *pHitDiceLevelBonus;
    CString ability;
    pHitDiceLevelBonus = pClass->PeekHitDiceLevelBonus(i);
    if (pHitDiceLevelBonus->baseclassID != baseclassID) continue;
    ability = pHitDiceLevelBonus->ability;
    if      (ability == Ability_Constitution) prime = GetAdjCon();
    else if (ability == Ability_Strength)     prime = GetAdjStr();
    else if (ability == Ability_Intelligence) prime = GetAdjInt();
    else if (ability == Ability_Wisdom)       prime = GetAdjWis();
    else if (ability == Ability_Dexterity)    prime = GetAdjDex();
    else if (ability == Ability_Charisma)     prime = GetAdjCha();
    //pBaseclassStats = PeekBaseclassStats(pHitDiceLevelBonus->baseclassID);
    //if (pBaseclassStats == NULL) continue;
    //level = pBaseclassStats->currentLevel;
    result += pHitDiceLevelBonus->bonusValues[prime-1];
    
    
//    int CLASS_DATA::GetHitDiceBonus(int prime) const
//{
//  if (prime < 1) prime = 1;
//  if (prime > HIGHEST_CHARACTER_PRIME) prime = HIGHEST_CHARACTER_PRIME;
//  return hitDiceBonusValues[prime-1];
//};



  };

  return result;
}

//*****************************************************************************
// NAME:    CHARACTER::getCharExpWorth
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int CHARACTER::getCharExpWorth()
{
  int totexp = 0;
/*
  totexp += FighterExperience;
  totexp += PaladinExperience;
  totexp += RangerExperience;
  totexp += ClericExperience;
  totexp += DruidExperience;
  totexp += MUExperience;
  totexp += ThiefExperience;
*/
  if (type == NPC_TYPE)
  {
    CString parameter;
    parameter = specAbs.GetString(SA_XP_Value);
    sscanf(parameter,"%d",&totexp);
  }
  else
  {
    int i, n;
    n = GetBaseclassStatsCount();
    for (i=0; i<n; i++)
    {
      const BASECLASS_STATS *pBaseclassStats;
      pBaseclassStats = PeekBaseclassStats(i);
      // 20170926  totexp += pBaseclassStats->experience;
      totexp += pBaseclassStats->CurExperience();
    };
  };
  //totexp += (int)money.ConvertToDefault(money.Total(), money.GetBaseType());
  //totexp += money.TotalGemValue();
  //totexp += money.TotalJewelryValue();

  POSITION pos = myItems.GetHeadPosition();
  while (pos != NULL)
    //totexp += itemData.GetItemExpWorth(myItems.GetNext(pos).m_giID);
    totexp += itemData.GetItemExpWorth(myItems.GetNext(pos).itemID);
  
  double mod = GetAllExpMod();
  if (mod != 0.0) totexp += ((mod/100.0) * (double)totexp);
  if (totexp < 0) totexp=0;
  
  return totexp;
}

/*
int CreateSpellAvailabilityList( CHARACTER *pChar,
                                  CArray<AVAILABLE_SPELL, AVAILABLE_SPELL&> *pSpellAvailabilityList);
*/

//*****************************************************************************
// NAME: CHARACTER::TrainCharacter
//
// PURPOSE:  
//*****************************************************************************
void CHARACTER::TrainCharacter(const CArray<TRAINABLE_BASECLASS, TRAINABLE_BASECLASS&> *pTrainableBaseclasses)
{
  if (GetType() == MONSTER_TYPE) return;

  // need to train only eligible classes 
  // if a multi-class character
  //
  getNewCharLevel(pTrainableBaseclasses, 1);

  DetermineCharHitDice();
  DetermineCharMaxHitPoints();
  hitPoints = maxHitPoints;  
  readyToTrain = FALSE;
  UpdateLevelBasedStats();
  //SetThiefSkillDexAdjustments();
  SetThiefSkills();
  spellAbility.valid = FALSE;
  UpdateSpellAbility();


/*
  {
    int i, n;
    CArray<AVAILABLE_SPELL, AVAILABLE_SPELL&> spellAvailabilityList;
    CreateSpellAvailabilityList(this, &spellAvailabilityList);

    n = spellAvailabilityList.GetSize();
    for (i=0; i<n; i++)
    {
      const SPELL_DATA *pSpell;
      pSpell = spellAvailabilityList[i].pSpellData;
      //if (!pSpell->AutoScribeAtLevelChange) continue;

      // From Manikus 20171115
      // It is correct that they are not auto-scribed. Magic User school
      // spells are added to the $KnowableSpells$ ASL at character creation,
      // and then at time of attaining a new level or a scribeable item
      // being scribed, they can move a spell from the $KnowableSpells$ ASL
      // to their spellbook.
      //
      // So we removed the test for autoscribe.
      //if (!pSpell->AutoScribe) continue;


      if (!KnowSpell(pSpell, true))        
      {
        CString msg;
        msg.Format("% was unable to learn spell %s",GetName(), pSpell->Name);
        MsgBoxInfo(msg);
      };
    };

  };

*/


}

DWORD randomMT(void);

//*****************************************************************************
// NAME:    CHARACTER::generateNewCharacter
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::generateNewCharacter(DWORD StartExperience, int StartExpType)
{  



  ActorType actor;

  //strengthKey     = abilityData.GetKeyByIndex(0);
  //intelligenceKey = abilityData.GetKeyByIndex(1);
  //wisdomKey       = abilityData.GetKeyByIndex(2);
  //dexterityKey    = abilityData.GetKeyByIndex(3);
  //constitutionKey = abilityData.GetKeyByIndex(4);
  //charismaKey     = abilityData.GetKeyByIndex(5);

  GetContext(&actor);
  SetCharContext(&actor);
  
  rollStats();

/*
  FighterExperience = 0;
  ClericExperience = 0;
  RangerExperience =  0;
  PaladinExperience = 0;
  MUExperience = 0;
  ThiefExperience = 0;
  DruidExperience = 0;
*/
  baseclassStats.RemoveAll();

  // Here we need to add baseclassStats for each
  // baseclass of his current class.
  {
    const CLASS_DATA *pClass;
    BASECLASS_STATS bcs;
    int i, n;
    pClass = classData.PeekClass(classID);
    if (pClass == NULL)
    {
      n = classData.GetCount();
      if (n == 0)
      {
        WriteDebugString("Unable to assign any Class to character\n");
        return;
      };
      pClass = classData.PeekClass(0);
      classID = pClass->ClassID();
    }
    else
    {
      bcs.currentLevel = 1;
      bcs.x_experience = 0;
      bcs.preDrainLevel = 0;
      bcs.previousLevel = 0;
      n = pClass->GetCount();
      for (i=0; i<n; i++)
      {
        BASECLASS_ID baseclassID;
        baseclassID = *pClass->PeekBaseclassID(i);
        bcs.baseclassID = baseclassID;
        baseclassStats.Add(bcs);
      };
#ifdef OldDualClass20180126
      temp__canUsePrevClass = -1;
#endif
    }
  };
  {
    int i;
    i = raceData.LocateRace(RaceID());
    if (i < 0)
    {
      int n;
      n = raceData.GetCount();
      if (n == 0)
      {
        WriteDebugString("Cannot assign any Race to character");
        return;
      };
      race = raceData.PeekRace(0)->RaceID();
    };
  };

  if ((GetType()==MONSTER_TYPE) || (StartExpType == START_EXP_VALUE))
  {
    ASSERT(StartExpType == START_EXP_VALUE);
    giveCharacterExperience(StartExperience, FALSE);
  }
  else // startexp is minimum level
  {
    ASSERT(GetType()!=MONSTER_TYPE);    

    

/*
    int destlvl = StartExperience;
    //if (numClasses(charClass) > 1)
    if (NumClasses(classID) > 1)
      //destlvl = destlvl / numClasses(charClass);
      destlvl = destlvl / classData.PeekClass(classID)->GetCount();
    if (isFighter(charClass))
      FighterExperience = GetMinFighterExpForLevel(destlvl);
    if (isCleric(charClass))
      ClericExperience = GetMinClericExpForLevel(destlvl);
    if (isRanger(charClass))
      RangerExperience = GetMinRangerExpForLevel(destlvl);
    if (isPaladin(charClass))
      PaladinExperience = GetMinPaladinExpForLevel(destlvl);
    if (isMagicUser(charClass))
      MUExperience = GetMinMagicUserExpForLevel(destlvl);
    if (isThief(charClass))
      ThiefExperience = GetMinThiefExpForLevel(destlvl);
    if (isDruid(charClass))
      DruidExperience = GetMinDruidExpForLevel(destlvl);
*/  die("Not Needed?"); //Not Implemented(0x47abc, false);




  }

  TRACE("%d Before getNewCharLevel\n", timeGetTime());


  getNewCharLevel(NULL, 999);

  TRACE("%d After getNewCharLevel\n", timeGetTime());

  DetermineCharMaxAge();  
  DetermineCharStartAge(); //(START_AGE + (rand() % (AGE_RANGE+1)));
  age = min( maxAge,  age);
  SetBirthday(RollDice(365,1));

  status = Okay;
  myItems.Clear();

  getNewCharStartingMoney();
  getNewCharStartingEquip();
  determineNbrAttacks(); 
  DetermineUnarmedHitDice();

  maxHitPoints = 0;
  hitpointSeed = randomMT();
  DetermineNewCharMaxHitPoints(hitpointSeed);
  if (maxHitPoints > 0)
  {
    hitPoints = maxHitPoints;  
    UpdateStats(TRUE);
  }  // else we have a race/class definition error

  ClearCharContext();


}

void CHARACTER::UpdateBaseclassLevels()
{
  getNewCharLevel(NULL, 999);
}


//*****************************************************************************
//    NAME: CHARACTER::UpdateStats
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::UpdateStats(BOOL IsNewChar)
{
  ActorType actor;
  GetContext(&actor);
  SetCharContext(&actor);

  checkNewCharRaceScores(IsNewChar);  // Check Ability limits for race
  checkNewCharClassScores();          // Check Ability limits for class
  //ItemMask = DetermineCharItemMask();

  UpdateBaseclassLevels();  // They may be based on ability scores.
    
  UpdateSkillBasedStats();            // HitBonus, etc
  UpdateLevelBasedStats();            // THAC0, hit dice, etc

  ClearCharContext();
}

//*****************************************************************************
//    NAME: CHARACTER::UpdateLevelBasedStats
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::UpdateLevelBasedStats(void)
{
  ActorType actor;
  GetContext(&actor);
  SetCharContext(&actor);

  getCharTHAC0();
  //ComputeCharSavingThrows();
  SetThiefSkills();
  //SetThiefSkillRaceAdjustments();
  DetermineCharHitDice();
  DetermineMaxCureDisease();

  ClearCharContext();
}

//*****************************************************************************
//    NAME: CHARACTER::UpdateSkillBasedStats
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::UpdateSkillBasedStats(void)
{
  ActorType actor;
  GetContext(&actor);
  SetCharContext(&actor);

	maxEncumbrance = determineMaxEncumbrance();
	encumbrance = determineEffectiveEncumbrance();
	determineMaxMovement();
		
  int hb,db;
  BYTE od,omd,bl;
  determineStrengthProperties(GetLimitedStr(), GetLimitedStrMod(),
                              hb, db, 
                              od, omd, bl); 
  SetHitBonus(hb);
  SetDmgBonus(db);
  //SetOpenDoors(od);
  //SetOpenMagicDoors(omd);
  //SetBendBarsLiftGates(bl);  
  //SetThiefSkillDexAdjustments();
  SetThiefSkills();
  UpdateSpellAbility();
  SetCharBaseAC();
/*  PRS 20110215
  if (isPaladin(GetClass()))
  {
    DWORD src=0;
    DWORD result = GetAdjSpecAb(SA_ProtFromEvil, &src);
    if ((result==0) || (src != SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB))
    {
      SetSpecAb(SA_ProtFromEvil, TRUE, SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB);
    }
  }
*/
  ClearCharContext();
}

//*****************************************************************************
// NAME:    CHARACTER::rollStats
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::rollStats()
{
  ActorType actor;
  GetContext(&actor);
  SetCharContext(&actor);

  /*
	strength = rollSkillDie(strengthKey);
	intelligence = rollSkillDie(intelligenceKey);
	wisdom = rollSkillDie(wisdomKey);
	dexterity = rollSkillDie(dexterityKey);
	constitution = rollSkillDie(constitutionKey);
	charisma = rollSkillDie(charismaKey);
*/
	strength     = rollSkillDie(Ability_Strength);
	intelligence = rollSkillDie(Ability_Intelligence);
	wisdom       = rollSkillDie(Ability_Wisdom);
	dexterity    = rollSkillDie(Ability_Dexterity);
	constitution = rollSkillDie(Ability_Constitution);
	charisma     = rollSkillDie(Ability_Charisma);

  
  strengthMod = 0;
  if (GetAdjStr()==18)
  {
    //if ((isFighter(GetClass())) || (isRanger(GetClass())) || (isPaladin(GetClass())))
		//  strengthMod = (BYTE)RollDice(100,1);
    const CLASS_DATA *pClass;
    pClass = classData.PeekClass(classID);
    if (pClass != NULL)
    {
      double roll;
      pClass->strengthBonusDice.Roll(roll);
      strengthMod = roll;
    };
  }

  spellAbility.valid = FALSE;
  ClearCharContext();
}

int DAT_Race_Weight(CHARACTER *pChar)
{
  RACE_DATA *pRace;
  double weight;
  ActorType actor;
  pRace = raceData.GetRace(pChar->RaceID());
  if (pRace == NULL) return 0;
  pChar->GetContext(&actor);
  SetCharContext(&actor);
  pRace->m_weight.Roll(weight);
  ClearCharContext();
  return weight;
}

int DAT_Race_Height(CHARACTER *pChar)
{
  RACE_DATA *pRace;
  double height;
  ActorType actor;
  pRace = raceData.GetRace(pChar->RaceID());
  if (pRace == NULL) return 0;
  pChar->GetContext(&actor);
  SetCharContext(&actor);
  pRace->m_height.Roll(height);
  ClearCharContext();
  return height;
}

/*  Instead, we look for a character baseclass that the item says is legal
//*****************************************************************************
// NAME:    CHARACTER::DetermineCharItemMask
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BYTE CHARACTER::DetermineCharItemMask()
{
  if (GetType() == MONSTER_TYPE)
    //return monsterData.GetMonsterItemMask(origIndex);
    return monsterData.GetMonsterItemMask(monsterID);

  BYTE mask = 0;
  classType prevClass = BogusClass;

  if (CanUsePrevClass())
    prevClass = GetPreviousClass();
  classType ctype = GetAdjClass();

  if (isFighter(ctype))
     mask |= FighterFlag;

  if (isCleric(ctype))
     mask |= ClericFlag;

  if (isRanger(ctype))
     mask |= RangerFlag;

  if (isPaladin(ctype))
     mask |= PaladinFlag;

  if (isMagicUser(ctype))
     mask |= MagicUserFlag;

  if (isThief(ctype))
     mask |= ThiefFlag;

  if (isDruid(ctype))
     mask |= DruidFlag;

  if (isFighter(prevClass))
     mask |= FighterFlag;

  if (isCleric(prevClass))
     mask |= ClericFlag;

  if (isRanger(prevClass))
     mask |= RangerFlag;

  if (isPaladin(prevClass))
     mask |= PaladinFlag;

  if (isMagicUser(prevClass))
     mask |= MagicUserFlag;

  if (isThief(prevClass))
     mask |= ThiefFlag;

  if (isDruid(prevClass))
     mask |= DruidFlag;

  return mask;
}
*/
//*****************************************************************************
// NAME:    CHARACTER::getCharBaseAC
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::SetCharBaseAC()
{
  if (GetType() == MONSTER_TYPE)
  {
    //m_AC = monsterData.GetMonsterAC(origIndex);
    m_AC = monsterData.GetMonsterAC(monsterID);
  }
  else
  {
    BYTE dex = GetAdjDex();
	  if (dex > 14)
		  m_AC = 10 - (dex - 14);
	  else
		  m_AC = 10;
    if (m_AC >  MAX_AC) m_AC =  MAX_AC;
    if (m_AC <  MIN_AC) m_AC =  MIN_AC;
  }
}


//*****************************************************************************
// NAME: CHARACTER::SetCharAC
//
// PURPOSE:  
//*****************************************************************************
int CHARACTER::SetCharAC()
{
  SetCharBaseAC();
  // 28 Feb 2020 PRS  See definition of CHARACTER.m_AC
  //AC += myItems.GetProtectModForRdyItems();
  //if (AC >  10) AC =  10;
  //if (AC < -10) AC = -10;
  return GetEffectiveAC(); //return AC;
}

//*****************************************************************************
// NAME:    CHARACTER::DetermineUnarmedHitDice
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::DetermineUnarmedHitDice()
{
  if (GetType() == MONSTER_TYPE)
  {
    // Mostly a safety net. In combat the actual
    // attackData array should be used to determine
    // values for natural attacks (no weapons)
    //monsterData.GetMonsterDamageDice(origIndex,
    monsterData.GetMonsterDamageDice(monsterID,
                                     0, // first attack
                                     unarmedNbrDieS,
                                     unarmedDieS,
                                     unarmedBonus);
	  unarmedDieL = unarmedDieS;
	  unarmedNbrDieL = unarmedNbrDieS;
  }
  else
  {
	  unarmedDieS = 2;
	  unarmedNbrDieS = 1;
	  unarmedBonus = 0;
	  unarmedDieL = 2;
	  unarmedNbrDieL = 1;
  }
}

//*****************************************************************************
// NAME:    CHARACTER::DetermineCharHitDice
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::DetermineCharHitDice()
{
  if (GetType() == MONSTER_TYPE)
  {
    //MONSTER_DATA *pData = monsterData.GetMonsterData(origIndex);
    const MONSTER_DATA *pMonster;
    pMonster = monsterData.PeekMonster(monsterID);
    //if (pData == NULL) return;    
    if (pMonster == NULL) return;    
    //double hd = pData->Hit_Dice;     
    double hd = pMonster->Hit_Dice;     
    //if (!pData->UseHitDice) // represents hit points instead of hit dice
    if (!pMonster->UseHitDice) // represents hit points instead of hit dice
      hd = (hd/FIGHTER_HIT_DIE)+1;    
    if (hd < 1.0) hd = 1.0;    
    SetNbrHD(hd);       
    return;
  }

  //int nbrDice;
  //int constant;
  //int level;

  //classType prevClass = BogusClass;
  //classType ctype = GetAdjClass();
  CLASS_ID prevClass;
  CLASS_ID currClass = GetAdjClass();
#ifdef OldDualClass20180126
  if (CanUsePrevClass())
    prevClass = GetPreviousClass();
#endif
/*  
  if (isFighter(ctype))
  {
    for (level=1; level <= FighterLevel; level++)
    {
      getFighterHitDice(FighterLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isCleric(ctype))
  {
    for (level=1; level <= ClericLevel; level++)
    {
      getFighterHitDice(ClericLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isRanger(ctype))
  {
    for (level=1; level <= RangerLevel; level++)
    {
      getFighterHitDice(RangerLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isPaladin(ctype))
  {
    for (level=1; level <= PaladinLevel; level++)
    {
      getFighterHitDice(PaladinLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isMagicUser(ctype))
  {
    for (level=1; level <= MULevel; level++)
    {
      getFighterHitDice(MULevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isThief(ctype))
  {
    for (level=1; level <= ThiefLevel; level++)
    {
      getFighterHitDice(ThiefLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isDruid(ctype))
  {
    for (level=1; level <= DruidLevel; level++)
    {
      getFighterHitDice(DruidLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isFighter(prevClass))
  {

    for (level=1; level <= FighterLevel; level++)
    {
      getFighterHitDice(FighterLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isCleric(prevClass))
  {
    for (level=1; level <= ClericLevel; level++)
    {
      getFighterHitDice(ClericLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isRanger(prevClass))
  {
    for (level=1; level <= RangerLevel; level++)
    {
      getFighterHitDice(RangerLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isPaladin(prevClass))
  {
    for (level=1; level <= PaladinLevel; level++)
    {
      getFighterHitDice(PaladinLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isMagicUser(prevClass))
  {
    for (level=1; level <= MULevel; level++)
    {
      getFighterHitDice(MULevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isThief(prevClass))
  {
    for (level=1; level <= ThiefLevel; level++)
    {
      getFighterHitDice(ThiefLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }

  if (isDruid(prevClass))
  {
    for (level=1; level <= DruidLevel; level++)
    {
      getFighterHitDice(DruidLevel, dice,nbrDice,constant);
      totalDice += nbrDice;
    }
  }
*/
  //int totalClasses = numClasses(ctype) + numClasses(prevClass);
  const CLASS_DATA *pClass;
  int currDice=0, prevDice=0;
  pClass = classData.PeekClass(currClass);
  if (pClass != NULL)
  {
    currDice = pClass->DetermineCharHitDice(this);
  };
  pClass = classData.PeekClass(prevClass);
  if (pClass != NULL)
  {
    prevDice = pClass->DetermineCharHitDice(this);
  };

  SetNbrHD((currDice > prevDice) ? currDice : prevDice);
}

//*****************************************************************************
// NAME:    CHARACTER::DetermineCharMaxHitPoints
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::DetermineCharMaxHitPoints()
{
#ifdef OldDualClass20180126
  BOOL canUsePrevClass;
#endif
  if (GetType() == MONSTER_TYPE)
  {
    //MONSTER_DATA *pData = monsterData.GetMonsterData(origIndex);
    const MONSTER_DATA *pMonster;
    pMonster = monsterData.GetMonster(monsterID);
    //if (pData == NULL) return;    
    if (pMonster == NULL) return;    
    //double hd = pData->Hit_Dice;     
    double hd = pMonster->Hit_Dice;     
    //int bonus = pData->Hit_Dice_Bonus;
    int bonus = pMonster->Hit_Dice_Bonus;
    //if (!pData->UseHitDice) // represents hit points instead of hit dice
    if (!pMonster->UseHitDice) // represents hit points instead of hit dice
    {
      maxHitPoints = hd;
    }
    else
    {    
      if (hd < 1.0)
        maxHitPoints = RollDice(int(8.0*hd), 1, bonus);
      else 
        maxHitPoints = RollDice(8, (int)hd, bonus);
    }
    maxHitPoints = max(1.0, maxHitPoints);
    return;
  }

  int dice;
  int nbrDice;
  int constant;

 // classType prevClass = BogusClass;
  //classType ctype = GetAdjClass();
  CLASS_ID prevClass;
  CLASS_ID currClass = GetAdjClass();

  //if (CanUsePrevClass())
  //  prevClass = GetPreviousClass();

#ifdef OldDualClass20180126
  canUsePrevClass = CanUsePrevClass();
#endif

  int totalHP = 0;
  int baseclassCount = 0;
  //int totalClasses = numClasses(ctype) + numClasses(prevClass);
/*
  if (isFighter(ctype))
  {
     getFighterHitDice(FighterLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isCleric(ctype))
  
  {
     getClericHitDice(ClericLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isRanger(ctype))
  
  {
     getRangerHitDice(RangerLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isPaladin(ctype))
  {
     getPaladinHitDice(PaladinLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isMagicUser(ctype))
  {
     getMagicUserHitDice(MULevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isThief(ctype))
  {
     getThiefHitDice(ThiefLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isDruid(ctype))
  {
     getDruidHitDice(DruidLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isFighter(prevClass))
  {
     getFighterHitDice(FighterLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isCleric(prevClass))
  {
     getClericHitDice(ClericLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;  }

  if (isRanger(prevClass))
  {
     getRangerHitDice(RangerLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isPaladin(prevClass))
  {
     getPaladinHitDice(PaladinLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isMagicUser(prevClass))
  {
     getMagicUserHitDice(MULevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isThief(prevClass))
  {
     getThiefHitDice(ThiefLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }

  if (isDruid(prevClass))
  {
     getDruidHitDice(DruidLevel, dice,nbrDice,constant);
     totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
  }
*/
  {
    int i, n;
    int numLevel, sumOfLevels;  // So we can compute average level.
    numLevel = 0;
    sumOfLevels = 0;
    n = GetBaseclassStatsCount();
    for (i=0; i<n; i++)
    {
      // For each baseclass that has advanced one or more levels
      // we will add some hitpoints.
      int j;
      const BASECLASS_STATS *pBaseclassStats;
      baseclassCount++;
      pBaseclassStats = PeekBaseclassStats(i);
      if (pBaseclassStats->currentLevel == 0) continue;
      for (j=pBaseclassStats->temp__origLevel+1; j<=pBaseclassStats->currentLevel; j++)
      {
        const BASECLASS_ID& baseclassID = pBaseclassStats->baseclassID;
        GetBaseclassHitDice(baseclassID, j, dice, nbrDice, constant);
        totalHP += RollDice(dice, nbrDice, DetermineHitDiceBonus(baseclassID)) + constant;
      };
      //totalHP += averageLevel * DetermineHitDiceBonus("*Average*");
    };
  };
  //maxHitPoints += (totalHP / baseclassCount);
  maxHitPoints += totalHP;
  maxHitPoints = max( 1, maxHitPoints );
};


class LITTLE_RAN
{
  DWORD w, z;
public:
  void Init(DWORD seed);
  int Roll(int sides, int num, int bonus);
  DWORD Random(void);
};

DWORD LITTLE_RAN::Random()
{
  //z = 36969 * (z & 65535) + (z >> 16);
  //w = 18000 * (w & 65535) + (w >> 16);
  //return (z << 16) + w;  /* 32-bit result */
  w = 69069 * w + 1;
  return w + ((w>>16)&65535);  /* 32-bit result */
}

int LITTLE_RAN::Roll(int sides, int num, int bonus)
{
  if (sides > 0)
  {
	  for (; num>0; num--)
    {
      bonus += this->Random()%sides + 1;
    };
  };
	return bonus;
}

void LITTLE_RAN::Init(DWORD seed)
{
  w = seed & 0xffff;
  z = seed >> 16;
  if (w == 0) w = 0xc451;
  if (z == 0) z = 0x541b;
}


//*****************************************************************************
//    NAME: CHARACTER::DetermineNewCharMaxHitPoints
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::DetermineNewCharMaxHitPoints(DWORD randomSeed)
{
/*
 * A little problem....We want to 're-roll' the HP computation, changing
 * ONLY the bonus for ability scores for Strength, etc.  The computation can be
 * very complex and non-linear because of multiple baseclasses, averages,
 * and such.  So we have our own little random number generator
 * and we seed it before doing anything random.  That way, we can
 * repeat the computation for new values of strength, etc and only
 * changes in ability scores will affect the outcome.
 */
  LITTLE_RAN ran;
  ran.Init(randomSeed);
  if (GetType() == MONSTER_TYPE)
  {
    //MONSTER_DATA *pData = monsterData.GetMonsterData(origIndex);
    const MONSTER_DATA *pMonster;
    pMonster = monsterData.PeekMonster(monsterID);
    //if (pData == NULL) return;    
    if (pMonster == NULL) return;    
    //double hd = pData->Hit_Dice;     
    double hd = pMonster->Hit_Dice;     
    //int bonus = pData->Hit_Dice_Bonus;
    int bonus = pMonster->Hit_Dice_Bonus;
    //if (!pData->UseHitDice) // represents hit points instead of hit dice
    if (!pMonster->UseHitDice) // represents hit points instead of hit dice
    {
      maxHitPoints = hd;
    }
    else
    {    
      if (hd < 1.0)
        maxHitPoints = RollDice(int(8.0*hd), 1, bonus);
      else 
        maxHitPoints = RollDice(8, (int)hd, bonus);
    }
    maxHitPoints = max(1.0, maxHitPoints);
    return;
  }

  //int dice;
  //int nbrDice;
  //int constant;

  //int totalHP = 0;
  //classType ctype = GetAdjClass();
  CLASS_ID currClass = GetAdjClass();
  //int totalClasses = numClasses(ctype);
  int totalBaseClasses;
  const CLASS_DATA *pClass;
  pClass = classData.PeekClass(currClass);
  if (pClass != NULL)
  {
    totalBaseClasses = pClass->GetCount();

    if (totalBaseClasses == 0)
    {
      // we have a race/class configuration error
      WriteDebugString("Invalid race/class definition - totalClasses = 0 in DetermineNewCharMaxHitPoints()\n");
      return;
    }
  }
  else
  {
    WriteDebugString("invlaid class definition = \"%s\"\n", currClass);
    return;
  };

// I think what we wnat to do is:
//
// For each Baseclass
//    Get Hit Dice for that baseclass
//    HP = Roll those dice
//
// Add up all the HP for all Baseclasses and take the average.

  int numBaseclass;
  int totalHP;
  int maxHP;
  int specificHP;
  //bool specificBaseclass = false;
  numBaseclass = 0;
  totalHP = 0;
  maxHP = 0;
  specificHP = 0;
  pClass = classData.PeekClass(currClass);
  if (pClass != NULL)
  {
    int i, n;
    n = pClass->GetCount();
    for (i=0; i<n; i++)
    {
      const BASE_CLASS_DATA *pBaseclass;
      BASECLASS_ID baseclassID;
      baseclassID = *pClass->PeekBaseclassID(i);
      pBaseclass = baseclassData.PeekBaseclass(baseclassID);
      if (pBaseclass != NULL)
      {
        const BASECLASS_STATS *pBaseclassStats;
        pBaseclassStats = PeekBaseclassStats(baseclassID);
        if (pBaseclassStats != NULL)
        {
          int j, level, numDice, sides, constant, bonus, HP;
          numBaseclass++;
          level = pBaseclassStats->currentLevel;
          bonus = DetermineHitDiceBonus(pBaseclassStats->baseclassID);
          HP = 0;
          for (j=1; j<=level; j++)
          {
            pBaseclass->GetHitDice(j, &sides, &numDice, &constant);
            HP += (numDice>0)?ran.Roll(sides, numDice, bonus):0 + constant;
          };
          totalHP += HP;
          //if (HP > maxHP) maxHP = HP;
          //if (pClass->hitDiceBaseclassID == baseclassID) 
          //{
          //  specificHP = HP;
          //  specificBaseclass = true;
          //};
        };
      };
    };
  }
/*
  if (isFighter(ctype))
  {
    for (int j=1;j<=FighterLevel;j++)
    {
      getFighterHitDice(j, dice,nbrDice,constant);
      totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
    }
  }

  if (isCleric(ctype))
  {
    for (int j=1;j<=ClericLevel;j++)
    {
      getClericHitDice(j, dice,nbrDice,constant);
      totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
    }
  }

  if (isRanger(ctype))
  {
    for (int j=1;j<=RangerLevel;j++)
    {
      getRangerHitDice(j, dice,nbrDice,constant);
      totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
    }
  }

  if (isPaladin(ctype))
  {
    for (int j=1;j<=PaladinLevel;j++)
    {
      getPaladinHitDice(j, dice,nbrDice,constant);
      totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
    }
  }

  if (isMagicUser(ctype))
  {
    for (int j=1;j<=MULevel;j++)
    {
      getMagicUserHitDice(j, dice,nbrDice,constant);
      totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
    }
  }

  if (isThief(ctype))
  {
    for (int j=1;j<=ThiefLevel;j++)
    {
      getThiefHitDice(j, dice,nbrDice,constant);
      totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
    }
  }

  if (isDruid(ctype))
  {
    for (int j=1;j<=DruidLevel;j++)
    {
      getDruidHitDice(j, dice,nbrDice,constant);
      totalHP += 
        RollDice(dice, nbrDice, DetermineHitDiceBonus()) + constant;
    }
  }
*/
  maxHitPoints = max( 1, totalHP );
}

//*****************************************************************************
// NAME:    CHARACTER::getNewCharLevel
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::getNewCharLevel(const CArray<TRAINABLE_BASECLASS,TRAINABLE_BASECLASS&> *pTrainableBaseclasses,
                                int maxLevelGain)
{
  if (GetType() == MONSTER_TYPE)
  {
    //MONSTER_DATA *pData = monsterData.GetMonsterData(origIndex);
    const MONSTER_DATA *pMonster;
    pMonster = monsterData.PeekMonster(monsterID);
    //if (pData == NULL) return;    
    if (pMonster == NULL) return;    
    //double hd = pData->Hit_Dice;     
    double hd = pMonster->Hit_Dice;     
    //int bonus = pData->Hit_Dice_Bonus;
    int bonus = pMonster->Hit_Dice_Bonus;
    //if (!pData->UseHitDice) // represents hit points instead of hit dice
    if (!pMonster->UseHitDice) // represents hit points instead of hit dice
      hd = (hd/FIGHTER_HIT_DIE)+1;

    //FighterLevel = (BYTE)hd;
    //FighterLevel = max(FighterLevel, 1);  
    SetLevel(max(hd, 1));
    int hdBonus = bonus;
    if (hdBonus > 0)
    {
     int adjBonus = (hdBonus / 4) + 1;
     if ((hdBonus % 4) == 0)
       adjBonus--;
     //FighterLevel += bonus;
      SetLevel(max(hd, 1)+ adjBonus);
    }
  }
/*
  if (isFighter(charClass))
    FighterLevel = getFighterLevel(GetFighterExp());

  if (isCleric(charClass))
     ClericLevel = getClericLevel(GetClericExp());

  if (isRanger(charClass))
     RangerLevel = getRangerLevel(GetRangerExp());

  if (isPaladin(charClass))
     PaladinLevel = getPaladinLevel(GetPaladinExp());

  if (isMagicUser(charClass))
     MULevel = getMagicUserLevel(GetMagicUserExp());

  if (isThief(charClass))
     ThiefLevel = getThiefLevel(GetThiefExp());

  if (isDruid(charClass))
     DruidLevel = getDruidLevel(GetDruidExp());
*/
  {
    int i, n, maxLevel, limitLevel;
    maxLevel = 0;
    BASECLASS_ID baseclassID;
    n = GetBaseclassStatsCount();
    for (i=0; i<n; i++)
    {
      int j, m;
      DWORD exp;
      BASECLASS_STATS *pstats;
      const TRAINABLE_BASECLASS *pTrainableBaseclass;
      const BASE_CLASS_DATA *pBaseclass;
      pstats = GetBaseclassStats(i);
      if (pstats->currentLevel == 0) 
      {
        continue;  // To next baseclass
      };
      // 20170926 exp = pstats->experience;
      exp = pstats->CurExperience();
      baseclassID = pstats->baseclassID;
      pBaseclass = baseclassData.PeekBaseclass(baseclassID);
      if (pBaseclass == NULL)
      {
        WriteDebugString("Character %s has unknown baseclass '%s'\n",this->GetName(), baseclassID);
        continue;
      };
      m = pBaseclass->GetExpLevelsCount();
      for (j=m-1; j>=0; j--)
      {
        if (exp >= *pBaseclass->PeekExpLevels(j)) break;
      };
      j++; // j is the new level for this baseclass
      pTrainableBaseclass = NULL;
      if (pTrainableBaseclasses != NULL)
      {
        int k, num;
        num = pTrainableBaseclasses->GetSize();
        for (k=0; k<num; k++)
        {
          const TRAINABLE_BASECLASS *pTB;
          pTB = &(*(const_cast<CArray<TRAINABLE_BASECLASS,TRAINABLE_BASECLASS&> *>(pTrainableBaseclasses)))[k];
          if (pTB->baseclassID == baseclassID)
          {
            pTrainableBaseclass = pTB;
            break;
          };
        };
        if (pTrainableBaseclass==NULL)
        {
          continue;  // To next baseclass
        };
      };
      limitLevel = pstats->currentLevel + maxLevelGain;
      if (j > pstats->currentLevel+maxLevelGain)
      {
        j = pstats->currentLevel+maxLevelGain;
      };
      while (pstats->currentLevel < j)
      {
        if (pTrainableBaseclass != NULL)
        {
          if (pstats->currentLevel < pTrainableBaseclass->minLevel) break;
          if (pstats->currentLevel > pTrainableBaseclass->maxLevel) break;
        };
        pstats->currentLevel++;
#ifdef OldDualClass20180126
        temp__canUsePrevClass = -1;
#endif
      };
      {
        // At the last possible moment we will check for
        // level capping caused by skill "MaxLevel$SYS$" defined
        // in baseclass database and race database.
        int levelCap;
        levelCap = GetLevelCap(pBaseclass);
        if (levelCap != NoSkill)
        {
          if (pstats->currentLevel > levelCap)
          {
            pstats->currentLevel = levelCap;
          };
        };
      };
      {
        // Manikus says we should steal experience points
        // so that this fellow does not qualify for any
        // level higher than limitLevel;
        int maxExp;
        if (limitLevel < HIGHEST_CHARACTER_LEVEL)
        {
          maxExp = *pBaseclass->PeekExpLevels(limitLevel)-1;
          if (pstats->previousLevel <= 0)
          {
            if (pstats->x_experience > maxExp)
            {
              pstats->x_experience = maxExp;
            };
          };
        };
      };
    };
  };
}

//*****************************************************************************
// NAME:    CHARACTER::checkNewCharClassScores
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::checkNewCharClassScores()
{
  if (GetType() == MONSTER_TYPE) return;

  CHARSKILLDATA data;
  data.m_str = GetPermStr();
  data.m_int = GetPermInt();
  data.m_wis = GetPermWis();
  data.m_dex = GetPermDex();
  data.m_con = GetPermCon();
  data.m_cha = GetPermCha();
  //data.strKey = strengthKey;
  //data.intelKey = intelligenceKey;
  //data.wisKey = wisdomKey;
  //data.dexKey = dexterityKey;
  //data.conKey = constitutionKey;
  //data.chaKey = charismaKey;

  CheckNewCharClassScores(GetClass(), data);

  SetPermStr(data.m_str);
  SetPermInt(data.m_int);
  SetPermWis(data.m_wis);
  SetPermDex(data.m_dex);
  SetPermCon(data.m_con);
  SetPermCha(data.m_cha);
}

//*****************************************************************************
// NAME:    CHARACTER::checkNewCharRaceScores
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::checkNewCharRaceScores(BOOL ApplyModifiers)
{
  if (GetType() == MONSTER_TYPE) return;

  CHARSKILLDATA data;
  data.m_str = GetPermStr();
  data.m_int = GetPermInt();
  data.m_wis = GetPermWis();
  data.m_dex = GetPermDex();
  data.m_con = GetPermCon();
  data.m_cha = GetPermCha();
  //data.strKey = strengthKey;
  //data.intelKey = intelligenceKey;
  //data.wisKey = wisdomKey;
  //data.dexKey = dexterityKey;
  //data.conKey = constitutionKey;
  //data.chaKey = charismaKey;

  //if (ApplyModifiers)
  //  ApplyNewCharRaceScoreModifiers(race(), data);
  //CheckNewCharRaceScores(race(), data);
  CheckNewCharRaceScores(race, data);

  SetPermStr(data.m_str);
  SetPermInt(data.m_int);
  SetPermWis(data.m_wis);
  SetPermDex(data.m_dex);
  SetPermCon(data.m_con);
  SetPermCha(data.m_cha);
}

//*****************************************************************************
//    NAME: CHARACTER::DetermineCharStartAge
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::DetermineCharStartAge()
{
  if (GetType() == MONSTER_TYPE)
    age = START_AGE;
  else
    //age = determineCharStartAge(race());
    age = determineCharStartAge(race);
}

//*****************************************************************************
// NAME:    CHARACTER::DetermineCharMaxAge
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::DetermineCharMaxAge()
{
  if (GetType() == MONSTER_TYPE)
  {
    maxAge = 1500;
    return;
  }
  //maxAge = determineCharMaxAge(race());
  maxAge = determineCharMaxAge(race);
}

//*****************************************************************************
// NAME:    CHARACTER::getNewCharStartingMoney
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::getNewCharStartingMoney()
{
  if (GetType() == MONSTER_TYPE) return;

  money.Clear();
  money.Add(money.GetDefaultType(), GetDesignStartPlatinum()); 
  for (int g=0;g<GetDesignStartGems();g++) money.AddGem();
  for (int j=0;j<GetDesignStartJewelry();j++) money.AddJewelry();
}

//*****************************************************************************
//    NAME: CHARACTER::getNewCharStartingEquip
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::getNewCharStartingEquip()
{
  if (GetType() == MONSTER_TYPE) return;
  myItems.Clear();

  {
    const CLASS_DATA *pClass;
    pClass = classData.PeekClass(classID);
    if (pClass != NULL)
    {
      myItems = pClass->m_startingEquipment;
    };
  };

/*
  if (isFighter(charClass))
  {
    pos = globalData.FighterEquip.GetHeadPosition();
    while (pos != NULL)
      myItems.AddItem(globalData.FighterEquip.GetNext(pos));    
  }
  if (isCleric(charClass))
  {
    pos = globalData.ClericEquip.GetHeadPosition();
    while (pos != NULL)
      myItems.AddItem(globalData.ClericEquip.GetNext(pos));    
  }
  if (isRanger(charClass))
  {
    pos = globalData.RangerEquip.GetHeadPosition();
    while (pos != NULL)
      myItems.AddItem(globalData.RangerEquip.GetNext(pos));    
  }
  if (isPaladin(charClass))
  {
    pos = globalData.PaladinEquip.GetHeadPosition();
    while (pos != NULL)
      myItems.AddItem(globalData.PaladinEquip.GetNext(pos));    
  }
  if (isMagicUser(charClass))
  {
    pos = globalData.MagicUserEquip.GetHeadPosition();
    while (pos != NULL)
      myItems.AddItem(globalData.MagicUserEquip.GetNext(pos));    
  }
  if (isThief(charClass))
  {
    pos = globalData.ThiefEquip.GetHeadPosition();
    while (pos != NULL)
      myItems.AddItem(globalData.ThiefEquip.GetNext(pos));    
  }
  if (isDruid(charClass))
  {
    pos = globalData.DruidEquip.GetHeadPosition();
    while (pos != NULL)
      myItems.AddItem(globalData.DruidEquip.GetNext(pos));    
  }
  */
}

//*****************************************************************************
// NAME:    CHARACTER::determineMaxMovement
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int CHARACTER::determineMaxMovement()
{
  if (GetType() == MONSTER_TYPE)
  {
    //int val = monsterData.GetMonsterMovement(origIndex);
    int val = monsterData.GetMonsterMovement(monsterID);
    SetMaxMovement(val);
    return GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, 
                             "Determine monster max movement");
  }

	int ewt = determineEffectiveEncumbrance();	
  int normalEncumbrance = determineNormalEncumbrance();
  int val = 12;

  if (ewt <= normalEncumbrance) val = 12;
  else if (ewt <= (normalEncumbrance*2)) val = 9;
  else if (ewt <= (normalEncumbrance*3)) val = 6;
  else if (ewt <= (normalEncumbrance*4)) val = 3;
  else val = 1;

  SetMaxMovement(val);
  return GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS,
                           "Determine non-monster max movement");
}

//*****************************************************************************
// NAME:    CHARACTER::determineMaxEncumbrance
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int CHARACTER::determineMaxEncumbrance()
{
  if (GetType() == MONSTER_TYPE)
    return 100000; // monsters are not encumbered in combat?

  return (determineNormalEncumbrance()*5);
}

//*****************************************************************************
// NAME:    CHARACTER::getCharSavingThrows
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//void CHARACTER::ComputeCharSavingThrows()
//{
   //int level=1;
//   int ppd=20;int rsw=20; int pp=20; int br=20; int sp=20;
  //classType prevClass = BogusClass;
//  CLASS_ID adjClass, prevClass;
  //classType ctype = GetAdjClass();
//  adjClass = GetAdjClass();

  //if (CanUsePrevClass())
  //  prevClass = GetPreviousClass();
//  prevClass = GetPreviousClass();

//*****************************************************************************
// NAME:    CHARACTER::giveCharacterExperience
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::giveCharacterExperience(int exppts, BOOL UseLimits)
{
  //if (GetType() == MONSTER_TYPE) return;
  if (exppts == 0) return;
  const CLASS_DATA *pClass;
  pClass = classData.PeekClass(classID);
  if (pClass == NULL) return;
  {
    int i, n;
    int curExp;
    n = pClass->GetCount();
    curExp = (exppts+n-1)/n;
    for (i=0; i<n; i++)
    {
      BASECLASS_ID baseclassID;
      baseclassID = *pClass->PeekBaseclassID(i);
      const BASE_CLASS_DATA *pBaseclass;
      pBaseclass = baseclassData.PeekBaseclass(baseclassID);
      if (pBaseclass != NULL)
      {
        IncCurrExp(baseclassID, curExp);
      };
    };
  };
}



#ifdef OldDualClass20180126
//*****************************************************************************
// NAME:    CHARACTER::getCharTHAC0
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::getCharTHAC0()
{
  if (GetType() == MONSTER_TYPE)
  {
    //THAC0 = monsterData.GetMonsterTHAC0(origIndex);
    THAC0 = monsterData.GetMonsterTHAC0(monsterID);
    return;
  }

   //int thac0 = 20;
  int thac0;

   //classType prevClass = BogusClass;
#ifdef OldDualClass20180126
  CLASS_ID prevClass;
#endif
   //classType ctype = GetAdjClass();
  CLASS_ID adjClass;
  adjClass = GetAdjClass();
#ifdef OldDualClas20180126
  if (CanUsePrevClass())
     prevClass = GetPreviousClass();
#endif
/*
   if (isFighter(ctype))
//      thac0 = min(thac0, (21 - FighterLevel)); // manikus 7-21-09
   {									//
		if (FighterLevel > 2)			// 
			thac0 = min( thac0, (20-(((FighterLevel - 1)/2)*2)) ); //
		if (FighterLevel > 17)			//
			thac0 = min( thac0, 4 );	//
   }									//

   if (isPaladin(ctype))
//      thac0 = min(thac0, (21 - PaladinLevel)); // manikus 7-21-09
   {									//
		if (PaladinLevel > 2)			//
			thac0 = min( thac0, (20-(((PaladinLevel - 1)/2)*2)) ); //
		if (PaladinLevel > 17)			//
			thac0 = min( thac0, 4);		//
   }									//

   if (isRanger(ctype))
//      thac0 = min(thac0, (21 - RangerLevel)); // manikus 7-21-09
   {									//
		if (RangerLevel > 2)			//
			thac0 = min( thac0, (20-(((RangerLevel - 1)/2)*2)) ); //
		if (RangerLevel > 17)			//
			thac0 = min( thac0, 4);		//
   }									//
  
   if (isCleric(ctype))
   {
		if (ClericLevel > 3)
			thac0 = min( thac0, (20-(((ClericLevel-1)/3)*2)) );
		if (ClericLevel > 18)			// manikus 7-21-09
			thac0 = min( thac0, 9 );	//
   }
   
   if (isDruid(ctype))
   {
		if (DruidLevel > 3)
			thac0 = min( thac0, (20-(((DruidLevel-1)/3)*2)) );
		if (DruidLevel > 18)			// manikus 7-21-09
			thac0 = min( thac0, 9 );	//
   }
   
   if (isMagicUser(ctype))
   {
		if (MULevel > 5)				// manikus 7-21-09
			thac0 = min( thac0, 19 );	//
		if (MULevel > 10)				//
			thac0 = min( thac0, 16 );	//
		if (MULevel > 15)				//
			thac0 = min( thac0, 13 );	//
		if (MULevel > 20)				//
			thac0 = min( thac0, 11 );	//
   }

   if (isThief(ctype))
   {
		if (ThiefLevel > 4)				// manikus 7-21-09
			thac0 = min( thac0, 19 );	//
		if (ThiefLevel > 8)				//
			thac0 = min( thac0, (20 - (((ThiefLevel-1)/4)*2)) ); //
		if (ThiefLevel > 20)			//
			thac0 = min( thac0, 10 );	//
   }

   if (isFighter(prevClass))
//      thac0 = min(thac0, (21 - prevFighterLevel));	// manikus 7-21-09
   {									//
		if (prevFighterLevel > 2)		// 
			thac0 = min( thac0, (20-(((prevFighterLevel - 1)/2)*2)) ); //
		if (prevFighterLevel > 17)		//
			thac0 = min( thac0, 4);		//
   }									//

   if (isPaladin(prevClass))
//      thac0 = min(thac0, (21 - prevPaladinLevel));	// manikus 7-21-09
   {									//
		if (prevPaladinLevel > 2)		// 
			thac0 = min( thac0, (20-(((prevPaladinLevel - 1)/2)*2)) ); //
		if (prevPaladinLevel > 17)		//
			thac0 = min( thac0, 4);		//
   }									//

   if (isRanger(prevClass))
//      thac0 = min(thac0, (21 - prevRangerLevel));		// manikus 7-21-09
   {									//
		if (prevRangerLevel > 2)		// 
			thac0 = min( thac0, (20-(((prevRangerLevel - 1)/2)*2)) ); //
		if (prevRangerLevel > 17)		//
			thac0 = min( thac0, 4);		//
   }									//
  
   if (isCleric(prevClass))
   {
		if (prevClericLevel > 3)
			thac0 = min( thac0, (20-(((prevClericLevel-1)/3)*2)) );
		if (prevClericLevel > 18)		// manikus 7-21-09
			thac0 = min( thac0, 9 );	//
   }
   
   if (isDruid(prevClass))
   {
		if (prevDruidLevel > 3)
			thac0 = min( thac0, (20-(((prevDruidLevel-1)/3)*2)) );
		if (prevDruidLevel > 18)		// manikus 7-21-09
			thac0 = min( thac0, 9 );	//
   }
   
   if (isMagicUser(prevClass))
   {
		if (prevMULevel > 5)			// manikus 7-21-09
			thac0 = min( thac0, 19 );	//
		if (prevMULevel > 10)			//
			thac0 = min( thac0, 16 );	//
		if (prevMULevel > 15)			//
			thac0 = min( thac0, 13 );	//
		if (prevMULevel > 20)			//
			thac0 = min( thac0, 11 );	//
   }

   if (isThief(prevClass))
   {
		if (prevThiefLevel > 4)			// manikus 7-21-09
			thac0 = min( thac0, 19 );	//
		if (prevThiefLevel > 8)			//
			thac0 = min( thac0, (20 - (((prevThiefLevel-1)/4)*2)) ); //
		if (prevThiefLevel > 20)		//
			thac0 = min( thac0, 10 );	//
   }


   THAC0 = thac0;
*/

  // For each baseclass, compute THAC0;
  // Take the minumum
  const CLASS_DATA *pClass;
  pClass = classData.PeekClass(adjClass);
  if (pClass == NULL) 
  {
    thac0 = 20;
  }
  else
  {
    thac0 = pClass->GetCharTHAC0(this);
  };
  THAC0 = thac0;
#ifdef OldDualClass20180126
  pClass = classData.PeekClass(prevClass);
  if (pClass == NULL) 
  {
    thac0 = 20;
  }
  else
#else
  NotImplemented(0x20180126,false);
#endif
  {
    thac0 = pClass->GetCharTHAC0(this);
  };
  if (thac0 < THAC0) THAC0 = thac0;

	if (THAC0 < MIN_THAC0)		// manikus 7-21-09
		THAC0 = MIN_THAC0;	//
	if (THAC0 > MAX_THAC0)
		THAC0 = MAX_THAC0;
}
#else OldDualClass20180126

//*****************************************************************************
// NAME:    CHARACTER::getCharTHAC0
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::getCharTHAC0()
{
  int i, n, thac0;
  const BASECLASS_STATS *pBaseclassStats;
  if (GetType() == MONSTER_TYPE)
  {
    //THAC0 = monsterData.GetMonsterTHAC0(origIndex);
    THAC0 = monsterData.GetMonsterTHAC0(monsterID);
    return;
  }
  n = baseclassStats.GetCount();
  THAC0 = 20;
  for (i=0; i<n; i++)
  {
    int level;
    //pBaseclassID = PeekBaseclassID(i);
    //if (pBaseclassID == NULL) continue;
    //pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
    //if (pBaseclass == NULL) continue;
    pBaseclassStats = PeekBaseclassStats(i);
    if (pBaseclassStats == NULL) continue;
    if (!CanUseBaseclass(pBaseclassStats)) continue;
    if (pBaseclassStats->currentLevel > 0)
    {
      level = pBaseclassStats->currentLevel;
    }
    else
    {
      level = pBaseclassStats->previousLevel;
    };
    if (level < 1) continue;
    if (level >HIGHEST_CHARACTER_LEVEL) level = HIGHEST_CHARACTER_LEVEL;
    if (      (pBaseclassStats->pBaseclassData == NULL)
          ||  (pBaseclassStats->baseclassID != pBaseclassStats->pBaseclassData->BaseclassID()))
    {
      pBaseclassStats->pBaseclassData = baseclassData.PeekBaseclass(pBaseclassStats->baseclassID);
    };
    if (pBaseclassStats->pBaseclassData == NULL) continue;
    thac0 = pBaseclassStats->pBaseclassData->THAC0[level-1];;
    if (thac0 < THAC0) THAC0 = thac0;
  };
}
#endif OldDualClass20180126


//*****************************************************************************
//    NAME: CHARACTER::GetTHAC0
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetTHAC0(void) const
{
  return THAC0;
}

//*****************************************************************************
//    NAME: CHARACTER::determineNormalEncumbrance
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::determineNormalEncumbrance()
{
  return (DetermineNormalEncumbrance(GetAdjStr(), GetAdjStrMod()));
}

// ignore magical items
//*****************************************************************************
// NAME:    CHARACTER::determineEffectiveEncumbrance
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int CHARACTER::determineEffectiveEncumbrance()
{
  if (GetType() == MONSTER_TYPE) return 0;

	int total = money.GetTotalWeight(); 

  POSITION pos = myItems.GetHeadPosition();
	while (pos != NULL)
  {
    //if (!isMagical(myItems.GetAt(pos).item))
    {
      //total += getItemEncumbrance(myItems.PeekAtPos(pos).m_giID, 
      total += getItemEncumbrance(myItems.PeekAtPos(pos).itemID, 
                                  myItems.PeekAtPos(pos).qty);
    }
    myItems.GetNext(pos);
  }
		
	return total;
}

// include all items, even magical
//*****************************************************************************
// NAME:    CHARACTER::determineEncumbrance
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int CHARACTER::determineEncumbrance()
{
  if (GetType() == MONSTER_TYPE) return 0;

	int total = money.GetTotalWeight(); 

  POSITION pos = myItems.GetHeadPosition();
	while (pos != NULL)
  {
    //total += getItemEncumbrance(myItems.PeekAtPos(pos).m_giID, 
    total += getItemEncumbrance(myItems.PeekAtPos(pos).itemID, 
                                myItems.PeekAtPos(pos).qty);
    myItems.GetNext(pos);
  }

	return total;
}

//*****************************************************************************
// NAME:    CHARACTER::poolCharacterGold
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int CHARACTER::poolCharacterGold()
{	
  return (int)money.ConvertToDefault(money.Total(), money.GetBaseType());
}

//*****************************************************************************
// NAME:    CHARACTER::getThiefSkills
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::SetThiefSkills()
{

//  int pp=0;
//  int ol=0;
  //int rt=0;
//  int ft=0;
//  int ms=0;
//  int hs=0;
//  int hn=0;
//  int cw=0;
//  int rl=0;

  //classType ctype = GetAdjClass();
//  CLASS_ID adjClass;
//  adjClass = GetAdjClass();
  //classType prevClass = BogusClass;
//  CLASS_ID prevClass;
//  if (CanUsePrevClass())
//    prevClass = GetPreviousClass();
/*  
  if (isThief(ctype) || isThief(prevClass))
  {
    int level = max(ThiefLevel, prevThiefLevel);

    switch (level)
    {
    case 1:
      pp=30;ol=25;rt=20;ms=15;hs=10;hn=10;cw=85;rl=0;
      break;
    case 2:
      pp=35;ol=29;rt=25;ms=21;hs=15;hn=10;cw=86;rl=0;
      break;
    case 3:
      pp=40;ol=33;rt=30;ms=27;hs=20;hn=15;cw=87;rl=0;
      break;
    case 4:
      pp=45;ol=37;rt=35;ms=33;hs=25;hn=15;cw=88;rl=20;
      break;
    case 5:
      pp=50;ol=42;rt=40;ms=40;hs=31;hn=20;cw=90;rl=25;
      break;
    case 6:
      pp=55;ol=47;rt=45;ms=47;hs=37;hn=20;cw=92;rl=30;
      break;
    case 7:
      pp=60;ol=52;rt=50;ms=55;hs=43;hn=25;cw=94;rl=35;
      break;
    case 8:
      pp=65;ol=57;rt=55;ms=62;hs=49;hn=25;cw=96;rl=40;
      break;
    case 9:
      pp=70;ol=62;rt=60;ms=70;hs=56;hn=30;cw=98;rl=45;
      break;
    case 10:
      pp=80;ol=67;rt=65;ms=78;hs=63;hn=30;cw=99;rl=50;
      break;
    case 11:
      pp=90;ol=72;rt=70;ms=86;hs=70;hn=35;cw=99;rl=55;
      break;
    case 12:
      pp=95;ol=77;rt=75;ms=94;hs=77;hn=35;cw=99;rl=60;
      break;
    case 13:
      pp=99;ol=82;rt=80;ms=99;hs=85;hn=40;cw=99;rl=65;
      break;
    case 14:
      pp=99;ol=87;rt=85;ms=99;hs=93;hn=40;cw=99;rl=70;
      break;
    case 15:
      pp=99;ol=92;rt=90;ms=99;hs=99;hn=50;cw=99;rl=75;
      break;
    case 16:
      pp=99;ol=97;rt=95;ms=99;hs=99;hn=50;cw=99;rl=80;
      break;
    default:
      pp=99;ol=99;rt=99;ms=99;hs=99;hn=55;cw=99;rl=80;
      break;  
    }
  }
*/

  // Go through all the character's baseclasses.
  // Look for skills adjustments.
//  const CLASS_DATA *pClass;
//  pClass = classData.PeekClass(adjClass);
//  if (pClass != NULL)
//  {
//    pClass->GetThiefSkills( this, 
//                            &pp,
//                            &ol,
//                            &ft,
//                            &ms,
//                            &hs,
//                            &hn,
//                            &cw,
//                            &rl);
//  };
//  pClass = classData.PeekClass(prevClass);
//  if (pClass != NULL)
//  {
//    pClass->GetThiefSkills( this, 
//                            &pp,
//                            &ol,
//                            &ft,
//                            &ms,
//                            &hs,
//                            &hn,
//                            &cw,
//                            &rl);
//  };
  
  
  
//  SetPickPockets(pp);
//	SetOpenLocks(ol);
//	SetFindTraps(ft);
//	SetMoveSilent(ms);
//	SetHideInShadows(hs);
//	SetHearNoise(hn);
//	SetClimbWalls(cw);
//	SetReadLanguages(rl);

}
/*
//*****************************************************************************
// NAME:    CHARACTER::getThiefSkillRaceAdjustments
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::SetThiefSkillRaceAdjustments()
{
  classType prevClass = BogusClass;
  if (CanUsePrevClass())
    prevClass = GetPreviousClass();

  if ((!isThief(GetAdjClass())) && (!isThief(prevClass)))
    return;

  double pp=0.0;
  double ol=0.0;
  double rt=0.0;
  double ms=0.0;
  double hs=0.0;
  double hn=0.0;
  double cw=0.0;
  double rl=0.0;

  switch (race())
  {
  case Dwarf:
    pp=0;ol=1.10;rt=1.15;ms=0;hs=0;hn=0;cw=0.90;rl=0.95;		// manikus 7-21-09
    break;
  case Elf:
    pp=1.05;ol=0.95;rt=0;ms=1.05;hs=1.10;hn=1.05;cw=0;rl=0;		//
    break;
  case Gnome:
    pp=0;ol=1.05;rt=1.10;ms=1.05;hs=1.05;hn=1.10;cw=0.85;rl=0;		//
    break;
  case HalfElf:
    pp=1.10;ol=0;rt=0;ms=0;hs=1.05;hn=0;cw=0;rl=0;			//
    break;
  case Halfling:
    pp=1.05;ol=1.05;rt=1.05;ms=1.10;hs=1.15;hn=1.05;cw=0.85;rl=0.95;	//
    break;
  default:
    break;
  }

	if (pp != 0) SetPickPockets((int)((double)GetPickPockets() * pp));
	if (ol != 0) SetOpenLocks((int)((double)GetOpenLocks() * ol));
	if (rt != 0) SetFindTraps((int)((double)GetFindTraps() * rt));
	if (ms != 0) SetMoveSilent((int)((double)GetMoveSilent() * ms));
	if (hs != 0) SetHideInShadows((int)((double)GetHideInShadows() * hs));
	if (hn != 0) SetHearNoise((int)((double)GetHearNoise() * hn));
	if (cw != 0) SetClimbWalls((int)((double)GetClimbWalls() * cw));
	if (rl != 0) SetReadLanguages((int)((double)GetReadLanguages() * rl));

  die("Not Needed?"); //Not Implemented(0xd41cba, false);
}
*/


#ifdef UAFEDITOR

void CHARACTER::UpdateSkill(int *skillModifier, 
                                const CString& skillName)
{
  die("Not Needed?"); //Not Implemented(0xb1ea9, false);
  // Apply adjustments for Race, baseclass level, and ability.
/*  
  const ABILITY_SKILL_ADJ *pSkillAdj;
  int i, n;
  n = pBaseclass->GetSkillAdjCount();
  for (i=0; i<n; i++)
  {
    pSkillAdj = pBaseclass->PeekSkillAdj(i);
    if (skillName == pSkillAdj->skillID)
    {
      int prime;
      const ABILITY_ID *pAbility;
      pAbility = &pSkillAdj->abilityID;
      if      (*pAbility == Ability_Dexterity)    prime = GetDex();
      else if (*pAbility == Ability_Constitution) prime = GetCon();
      else if (*pAbility == Ability_Strength)     prime = GetStr();
      else if (*pAbility == Ability_Intelligence) prime = GetInt();
      else if (*pAbility == Ability_Charisma)     prime = GetCha();
      else if (*pAbility == Ability_Wisdom)       prime = GetWis();
      else prime = 0;
      if (pSkillAdj->adjustments[prime-1] > *skillModifier)
      {
        *skillModifier = pSkillAdj->adjustments[prime-1];
      };
    };
  };
*/
}

#endif


//*****************************************************************************
// NAME:    CHARACTER::getThiefSkillArmorAdustments
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::GetThiefSkillArmorAdustments() const
{

}


//*****************************************************************************
// NAME:    CHARACTER::removeCharMoney
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::removeCharMoney(itemClassType itype, int qty)
{
  if (!itemIsMoney(itype))
  {
    WriteDebugString("Wrong item type passed to removeCharMoney()\n");
    return;
  }

  switch (itype) 
  {
  case GemType:
    money.RemoveMultGems(qty);
    break;
  case JewelryType:
    money.RemoveMultJewelry(qty);
    break;
  default:
    money.Subtract(itype, qty);
    break;
  }
 	encumbrance = determineEffectiveEncumbrance();
	determineMaxMovement();
}


//*****************************************************************************
// NAME:    CHARACTER::addCharMoneyToItemList
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
#ifdef UAFEngine
void CHARACTER::addCharMoneyToItemList(ITEM_TEXT_LIST &list)
{
  for (int i=0;i<money.NumCoinTypes();i++)
  {
    if (money.IsActive(globalData.moneyData.GetItemClass(i)))
    {
      itemClassType itype = MONEY_DATA_TYPE::GetItemClass(i);
      list.Add(money.Coins[i], itype, money.Name(itype));
    }
  }

  list.Add(money.NumGems(),GemType,globalData.moneyData.GetName(GemType));    
  list.Add(money.NumJewelry(), JewelryType,globalData.moneyData.GetName(JewelryType));

  ItemsOnPage = list.GetCount();
	ItemsOnPage = min(ItemsOnPage, Items_Per_Page);
}
#endif



//*****************************************************************************
// NAME:    CHARACTER::getCharWeaponText
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::getCharWeaponText(CString &wpn, CString &dmg)
{
  wpn = "";
  dmg = "";

	CString temp;
	CString temp2;

  temp = "";
  temp2 = "";
  int dmg_bonus=0;

//  int wpnHand = myItems.GetReady(WeaponHand);
  int wpnHand = myItems.GetReadiedItem(WeaponHand, 0);

  if (wpnHand != NO_READY_ITEM)
  {
	  //GLOBAL_ITEM_ID giID = myItems.GetItem(wpnHand);
	  ITEM_ID itemID;
    itemID= myItems.GetItem(wpnHand);

    //ITEM_DATA *data = itemData.GetItemData(giID);
    ITEM_DATA *data = itemData.GetItem(itemID);

	  if (data != NULL)
	  {
      // PRS 20110107
      // PRS 20130227
      //if ((isMagical(itemID)) && (myItems.GetId(wpnHand)))
		  //  wpn = data->QualifiedName();
      //else
		  //  wpn = data->Name; // The undecorated name.
		  //wpn = data->CommonName(); // The undecorated name.
      if ((isMagical(itemID)) && (myItems.GetId(wpnHand)))
      {
        wpn = data->IdName();
      }
      else
      {
        if (data->Wpn_Type == SpellLikeAbility)
        {
          int n;
          n = data->IdName().Find('|');
          if (n < 0)
          {
            wpn.Empty();
          }
          else
          {
            wpn = data->IdName().Right(data->IdName().GetLength() - n - 1);
          };
        }
        else
        {
          wpn = data->CommonName();
        };
      };

      BOOL isMissile=FALSE;
      // if using bow or crossbow, use ammo damage
      if ((data->Wpn_Type == Bow) || (data->Wpn_Type == Crossbow))
      {
      //int ammoHand = myItems.GetReady(AmmoQuiver);
        int ammoHand = myItems.GetReadiedItem(AmmoQuiver, 0);
        if (ammoHand != NO_READY_ITEM)
        {
          CString itemName;
          //giID = myItems.GetItem(ammoHand);
          itemName = myItems.GetItem(ammoHand);
          //data = itemData.GetItemData(giID);
          data = itemData.GetItem(itemID);
          isMissile=TRUE;
        }
      }

      if ((data->Dmg_Dice_Sm > 0) && (data->Nbr_Dice_Sm > 0))
      {
        dmg_bonus = data->Dmg_Bonus_Sm;
	      temp.Format("%iD%i", data->Nbr_Dice_Sm, data->Dmg_Dice_Sm);
  
        //if ((isMagical(giID)) && (myItems.GetId(wpnHand)))
        if ((isMagical(itemID)) && (myItems.GetId(wpnHand)))
          dmg_bonus += max(data->Attack_Bonus, data->Dmg_Bonus_Sm);
    
        if (!isMissile)
          dmg_bonus += GetAdjDmgBonus();

        if (dmg_bonus > 0)
	      {
  	      temp2.Format("+%i",dmg_bonus);
		      temp += temp2;
		    }
      }
	  
		  dmg = temp;
		}
	}
	else 
	{
		temp.Format("%iD%i", unarmedNbrDieS, unarmedDieS);

    dmg_bonus = unarmedBonus;
    dmg_bonus += GetAdjDmgBonus();

    if (dmg_bonus > 0)
	  {
		  temp2.Format("+%i",dmg_bonus);
		  temp += temp2;
	  }
	
	  dmg = temp;
	  wpn = "NONE";
	}
}

//*****************************************************************************
// NAME:    CHARACTER::getCharArmorText
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::getCharArmorText(CString &armor)
{
//		int index = myItems.GetReady(BodyArmor); // dude's ready armor, if any
		int index = myItems.GetReadiedItem(BodyArmor, 0); // dude's ready armor, if any

		if (index != NO_READY_ITEM)
    {
      if ((isMagical(myItems.GetItem(index))) && (myItems.GetId(index)))
			  armor = itemData.GetItemIdName(myItems.GetItem(index));
      else
			  armor = itemData.GetItemCommonName(myItems.GetItem(index));
    }
		else
			armor = "";
}

//*****************************************************************************
// NAME:    CHARACTER::addCharacterItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//BOOL CHARACTER::addCharacterItem(GLOBAL_ITEM_ID giID, int qty, int numCharges, BOOL id, int cost)
BOOL CHARACTER::addCharacterItem(const ITEM_ID& itemID, int qty, int numCharges, BOOL id, int cost)
{
  //if ((getItemEncumbrance(giID, qty) + GetEncumbrance()) > GetAdjMaxEncumbrance())
  if ((getItemEncumbrance(itemID, qty) + GetEncumbrance()) > GetAdjMaxEncumbrance())
  {
		SetMiscError(TooMuchWeight);
    return FALSE;
  }

  //if (!myItems.addItem(giID, qty, numCharges, id, cost))
  if (!myItems.addItem(itemID, qty, numCharges, id, cost))
  {
    WriteDebugString("Failed to addItem to character\n");
    return FALSE;
  }

	encumbrance = determineEffectiveEncumbrance();
	determineMaxMovement();

	return TRUE;
}

//*****************************************************************************
// NAME:    CHARACTER::delCharacterItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::delCharacterItem(int index, int qty)
{
//#ifdef newCombatant
//  if (m_pCombatant != NULL) return m_pCombatant->delCharacterItem(index, qty);
//#endif

  if (!myItems.deleteItem(index, qty))
  {
    WriteDebugString("Failed to deleteItem in delCharacterItem()\n");
    return FALSE;
  }

  encumbrance = determineEffectiveEncumbrance();
	determineMaxMovement();
  return TRUE;
}



//*****************************************************************************
// NAME:    CHARACTER::buyItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//void CHARACTER::buyItem(GLOBAL_ITEM_ID giID, costFactorType cf)
void CHARACTER::buyItem(const ITEM_ID& itemID, costFactorType cf)
{
	if (GetAdjStatus() != Okay)
	{
		switch (GetAdjStatus()) 
		{
		case Unconscious: SetMiscError(UnconsciousChar);break;
		case Dead: SetMiscError(DeadChar);break;
		case Fled: SetMiscError(FledChar);break;
		case Petrified: SetMiscError(PetrifiedChar);break;
		case Gone: SetMiscError(GoneChar);break;
		case Animated: SetMiscError(AnimatedChar);break;
		case TempGone: SetMiscError(TempGoneChar);break;
		case Running: SetMiscError(RunningChar);break;
		case Dying: SetMiscError(DyingChar);break;
		default: SetMiscError(UnknownError);break;
		}
		return;
	}

  //ITEM_DATA *data = itemData.GetItemData(giID);
  ITEM_DATA *data = itemData.GetItem(itemID);
  if (data == NULL)
    return;

  DWORD cost = ApplyCostFactor(cf, data->Cost);

  if (!enoughMoney(cost,0,0,globalData.moneyData.GetDefaultType()))
  {
    SetMiscError(NotEnoughMoney);
    return;
  }

	//if ((getItemEncumbrance(giID, 1) + GetEncumbrance()) > GetAdjMaxEncumbrance())
	if ((getItemEncumbrance(itemID, 1) + GetEncumbrance()) > GetAdjMaxEncumbrance())
		SetMiscError(TooMuchWeight);
	else 
  {
    int bundle = max(data->Bundle_Qty,1);
    // giID is identified if purchased from shop
		//if (!addCharacterItem(giID, bundle, data->Num_Charges, TRUE, cost))
		if (!addCharacterItem(itemID, bundle, data->Num_Charges, TRUE, cost))
			SetMiscError(MaxItemsReached);
		else
			payForItem(cost, globalData.moneyData.GetDefaultType());
	}
}


//*****************************************************************************
// NAME:    CHARACTER::enoughMoney
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::enoughMoney(int moneyCost, int gemCost, int jewelryCost, itemClassType moneyType)
{ 
  bool enough = true;

  if (moneyCost > 0)
  {
    enough=false;

    if (party.moneyPooled)
      enough = (party.poolSack.HaveEnough(moneyType, moneyCost)?true:false);    

    if (!enough) // character's money
      enough = (money.HaveEnough(moneyType, moneyCost)?true:false);
  }

  if ((enough) && (gemCost > 0))
    enough = (money.NumGems() >= gemCost);

  if ((enough) && (jewelryCost > 0))
    enough = (money.NumJewelry() >= jewelryCost);

  return enough; 
}


//*****************************************************************************
// NAME:    CHARACTER::payForItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::payForItem(int moneyCost, itemClassType moneyType, int gemCost, int jewelryCost)
{
  if (moneyCost > 0)
  {
    if ((party.moneyPooled) && (party.poolSack.HaveEnough(moneyType, moneyCost))) {
      party.poolSack.Subtract(moneyType, moneyCost);
      party.moneyPooled = !(party.poolSack.IsEmpty());
    }
    else
      money.Subtract(moneyType, moneyCost);
  }

  if (gemCost > 0)
    money.RemoveMultGems(gemCost);

  if (jewelryCost > 0)
    money.RemoveMultJewelry(jewelryCost);

 	encumbrance = determineEffectiveEncumbrance();
	determineMaxMovement();
}

void CHARACTER::TakeDamage(int dmg, 
                           BOOL IsNonLethal, 
                           InvokeSpellOptions *invokeOptions, 
                           bool canFinishCasting,
                           int *pDeathIndex) 
{ 
#ifdef UAFEngine
  if (m_pCombatant != NULL)
  {
    m_pCombatant-> TakeDamage(dmg, IsNonLethal, invokeOptions, canFinishCasting, pDeathIndex);
  }
  else
#endif
  { 
    giveCharacterDamage(dmg);
  };
};


//*****************************************************************************
// NAME:    CHARACTER::toggleReadyItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::toggleReadyItem(int item)
{
//#ifdef newCombatant
//  if (m_pCombatant != NULL) return m_pCombatant->toggleReadyItem(item);
//#endif

  BOOL success = FALSE;
  DWORD rdyLoc;
  ITEM_DATA *data = itemData.GetItem(myItems.GetItem(item));
  if (data == NULL) return FALSE;
  rdyLoc = data->Location_Readied;
  if (myItems.IsReady(item))
  {
    myItems.UnReady(item);
    success = !myItems.IsReady(item);
    if (success)
    {
  		if      (rdyLoc == WeaponHand) UnReadyWeaponScript(item);
      else if (rdyLoc == ShieldHand) UnReadyShieldScript(item);
		  else if (rdyLoc == BodyArmor)  UnReadyArmorScript(item);
		  else if (rdyLoc == Hands)      UnReadyGauntletsScript(item);
		  else if (rdyLoc == Head)       UnReadyHelmScript(item);
		  else if (rdyLoc == Waist)      UnReadyBeltScript(item);
		  else if (rdyLoc == BodyRobe)   UnReadyRobeScript(item);
		  else if (rdyLoc == Back)       UnReadyCloakScript(item);
		  else if (rdyLoc == Feet)       UnReadyBootsScript(item);
		  else if (rdyLoc == Fingers)    UnReadyRingScript(item);
                                       // should separate this into ring1 and ring2?
		  else if (rdyLoc == AmmoQuiver) UnReadyAmmoScript(item);
      else                           UnReadyXXXScript(ON_UNREADY, item);
    };
#ifdef UAFEngine
    //Not Implemented(0xadf4b, FALSE);
    //data->specAbs.DisableAllFor(this);
#endif
  }
  else
  {
		     if (rdyLoc == WeaponHand) ReadyWeaponScript(item);
		else if (rdyLoc == ShieldHand) ReadyShieldScript(item);
		else if (rdyLoc == BodyArmor)  ReadyArmorScript(item);
		else if (rdyLoc == Hands)      ReadyGauntletsScript(item);
		else if (rdyLoc == Head)       ReadyHelmScript(item);
		else if (rdyLoc == Waist)      ReadyBeltScript(item);
		else if (rdyLoc == BodyRobe)   ReadyRobeScript(item);
		else if (rdyLoc == Back)       ReadyCloakScript(item);
		else if (rdyLoc == Feet)       ReadyBootsScript(item);
		else if (rdyLoc == Fingers)    ReadyRingScript(item);
                                      // should separate this into ring1 and ring2?
		else if (rdyLoc == AmmoQuiver) ReadyAmmoScript(item);
    else ReadyXXXScript(data->Location_Readied, ON_READY, item);
    success = myItems.IsReady(item);
	}

  if (success)
    SetCharAC();

  return success;
}

//*****************************************************************************
// NAME:    CHARACTER::serializeCharacter
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::serializeCharacter(BOOL save, LPCSTR fname)
{
  // cannot load/store NPC or Monster
  if (GetType() == MONSTER_TYPE) return FALSE; 
  //if ((GetType() == NPC_TYPE) && (IsPreGen)) return FALSE;
  if (!CanBeSaved) return FALSE;

  BOOL success = TRUE;
  CFile myFile;
  CreateSaveDirectory(); 

  if (!save) // load character
  {     
    CString fullPath;

    // always load npc from design save folder
    if (GetType()==NPC_TYPE)
    {
      fullPath = GetSavePath();
      fullPath += "DCNPC_";
    }
    else // not an NPC
    {
      // first look in design save folder
      if (fname[1] == ':')
      {
        fullPath = fname;
      }
      else
      {
        fullPath = GetSavePath();
        fullPath += fname;
        fullPath += ".chr";
      };

      // if char doesn't exist in design save folder, 
      // try common save folder
      if (!FileExists(fullPath)) 
      {
        fullPath = GetCommonCharPath();
        if (fullPath.IsEmpty()) return FALSE; // common folder not defined
        fullPath += fname;
        fullPath += ".chr";
        if (!FileExists(fullPath)) return FALSE;
      }
    }    

    if (!myFile.Open(fullPath, CFile::modeRead))
    {
      success = FALSE;
      WriteDebugString("Unable to open the character file (read) %s\n", fullPath);
    }
    else
    {      
      // save original data
      CHARACTER tempCHARACTER = *this;

      BOOL VersionError = FALSE;
      
      try 
      {
        // check for save game version
        __int64 hdr=0;
        double ver = ENGINE_VER;

        myFile.Read(&hdr, sizeof(hdr));
        if (hdr == 0xFABCDEFABCDEFABF)
        {
          myFile.Read(&ver, sizeof(ver));
        }
        else
        {
          // special hdr not present, so reset archive to beginning
          myFile.SeekToBegin();
          // set version to last build that didn't save CHR version
          ver = _VERSION_0563_;
        }

        if ((ver > ENGINE_VER) || (ver < _SPECIAL_ABILITIES_VERSION_))
        {
          VersionError = TRUE;
          WriteDebugString("Possible saved character version error: file %s, file ver %4.7f, game ver %4.7f",
                           fullPath, ver, ENGINE_VER);
          throw 0;
        }

        CAR ar(&myFile, CArchive::load);
        Serialize(ar, ver);
        ar.Close();
        //m_spellbook.SortSpells();
      }
      catch (...)
      {
        success = FALSE;
        WriteDebugString("Unable to load the character file %s\n", fullPath);
      }
      
      if (!success)        
      {
        if (VersionError)
          SetMiscError(SaveCharVersionError);
        else
          SetMiscError(FileLoadError);
        // restore original contents
        *this = tempCHARACTER;
      }
      else
      {
        // after loading saved character, make player choose
        // to put player on automatic combat AI
        automatic = FALSE;
      }
    }
  }
  else 
  { 
    // saved char/npc's always go in design save folder
    CString fullPath;
    fullPath = GetSavePath();
    if (GetType()==NPC_TYPE) fullPath += "DCNPC_";
    fullPath += fname;
    fullPath += ".chr";

    // remove read-only flags
    SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
    // save character    
    if (!myFile.Open(fullPath, CFile::modeWrite | CFile::modeCreate))
    {
      success = FALSE;
      WriteDebugString("Unable to open the save character file (write) %s\n", fullPath);
    }
    else
    {
      if (icon.filename != "")
      {
        //icon.filename = StripFilenamePath(icon.filename, 0);
      }
      if (smallPic.filename != "")
      {
        //smallPic.filename = StripFilenamePath(smallPic.filename, 0);
      }
      CAR ar(&myFile, CArchive::store);         
      
      try 
      {
        // put signal value and version into saved character file
        __int64 hdr = 0xFABCDEFABCDEFABF;
        ar.Serialize((char *)&hdr, sizeof(hdr));
        double ver = ENGINE_VER;
        ar.Serialize((char *)&ver, sizeof(ver));

        Serialize(ar, ver);
      }
      catch (...)
      {
        success = FALSE;
        WriteDebugString("Unable to save the character file %s\n", fullPath);
      }
      
      ar.Close();
      if (!success)
        SetMiscError(FileSaveError);
    }
  }
  
  return success;
}

#ifdef UAFEDITOR

void CHARACTER::CrossReference(CR_LIST *pCRList) const
{
  CR_REFERENCE crReference;
  crReference.m_referenceName.Format("NPC(%s)",name);
  crReference.m_referenceType = CR_TYPE_NPC;
  pCRList->CR_AddEventReference(TalkEvent, &crReference);
  pCRList->CR_AddEventReference(ExamineEvent, &crReference);
  smallPic.CrossReference(pCRList, &crReference);
  icon.CrossReference(pCRList, &crReference);
  icon.CrossReference(pCRList, &crReference);
  //spellEffects.CrossReference(pCRList, &crReference);
  m_spellbook.CrossReference(pCRList); // &crReference);
  myItems.CrossReference(pCRList, &crReference);
  //MsgBoxInfo("CHARACTER::CrossReference not implemented");
}

#endif



/*
//*****************************************************************************
// NAME: CHARACTER::CanDualClassCleric
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanDualClassCleric() const
{




  alignmentType align;
  if (GetType() == MONSTER_TYPE) return FALSE;

  if (isCleric(GetClass()))
    return FALSE;

  if (GetWis() < 17)
    return FALSE;

  align = GetAlignment();
  {
  //return baseclassData.IsAllowedAlignment(align, "Cleric");
    BASECLASS_ID baseclassID;
    baseclassID = "Cleric";
    //return baseclassData.IsAllowedAlignment(align, "Cleric");
    return baseclassData.IsAllowedAlignment(baseclassID, align);
  };

}


//*****************************************************************************
// NAME: CHARACTER::CanDualClassFighter
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanDualClassFighter() const
{



  alignmentType align;
  if (GetType() == MONSTER_TYPE) return FALSE;

  if (isFighter(GetClass()))
    return FALSE;

  if (GetStr() < 17)
    return FALSE;

  align = GetAlignment();
  {
    BASECLASS_ID baseclassID;
    baseclassID = "Fighter";
    //return baseclassData.IsAllowedAlignment(align, "Fighter");
    return baseclassData.IsAllowedAlignment(baseclassID, align);
  };



}


//*****************************************************************************
// NAME: CHARACTER::CanDualClassMagicUser
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanDualClassMagicUser() const
{

  
  alignmentType align;
  if (GetType() == MONSTER_TYPE) return FALSE;

  if (isMagicUser(GetClass()))
    return FALSE;

  if (GetInt() < 17)
    return FALSE;

  align = GetAlignment();
  {
    BASECLASS_ID baseclassID;
    baseclassID = "MagicUser";
    //return baseclassData.IsAllowedAlignment(align, "MagicUser");
    return baseclassData.IsAllowedAlignment(baseclassID, align);
  };


}


//*****************************************************************************
// NAME: CHARACTER::CanDualClassThief
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanDualClassThief() const
{



  alignmentType align;
  if (GetType() == MONSTER_TYPE) return FALSE;

  if (isThief(GetClass()))
    return FALSE;

  if (GetDex() < 17)
    return FALSE;

  align = GetAlignment();
  {
    BASECLASS_ID baseclassID;
    baseclassID = "Thief";
    //return baseclassData.IsAllowedAlignment(align, "Thief");
    return baseclassData.IsAllowedAlignment(baseclassID, align);
  };



}

//*****************************************************************************
// NAME: CHARACTER::CanDualClassDruid
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanDualClassDruid() const
{



  alignmentType align;
  if (GetType() == MONSTER_TYPE) return FALSE;

  if (isDruid(GetClass()))
    return FALSE;

  if (GetCha() < 17) return FALSE;
  if (GetWis()   < 17) return FALSE;

  align = GetAlignment();
  {
    BASECLASS_ID baseclassID;
    baseclassID = "Druid";
    //return baseclassData.IsAllowedAlignment(align, "Druid");  
    return baseclassData.IsAllowedAlignment(baseclassID, align);  
  };



}


//*****************************************************************************
// NAME: CHARACTER::CanDualClassRanger
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanDualClassRanger() const
{



  alignmentType align;
  if (GetType() == MONSTER_TYPE) return FALSE;

  if (isRanger(GetClass()))
    return FALSE;

  if (GetStr()     < 17) return FALSE;
  if (GetWis()       < 17) return FALSE;
  if (GetInt() < 17) return FALSE;


  align = GetAlignment();
  {
    BASECLASS_ID baseclassID;
    baseclassID = "Ranger";
    //return baseclassData.IsAllowedAlignment(align, "Ranger");
    return baseclassData.IsAllowedAlignment(baseclassID, align);
  };




}


//*****************************************************************************
// NAME: CHARACTER::CanDualClassPaladin
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanDualClassPaladin() const
{




  alignmentType align;
  if (GetType() == MONSTER_TYPE) return FALSE;

  if (isPaladin(GetClass()))
    return FALSE;

  if (GetStr() < 17) return FALSE;
  if (GetWis() < 17) return FALSE;

  align = GetAlignment();
  {
    BASECLASS_ID baseclassID;
    baseclassID = "Paladin";
    //return baseclassData.IsAllowedAlignment(align, "Paladin");
    return baseclassData.IsAllowedAlignment(baseclassID, align);
  };




}
*/





#ifdef OldDualClass20180126

//*****************************************************************************
// NAME: CHARACTER::CanUsePrevClass
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::CanUsePrevClass() const
{
  if (!IsDualClass())
    return FALSE;
  if (temp__canUsePrevClass < 0)
  // I guess we must go through the computations.
  //
  // According to Manikus email of 20130402
  // [quote]
/*
I would like to treat each baseclass separately if 
it occurs in the original and new classes, but as
part of a group if it doesn't, and I'll try to be
clear in what I mean...

PC starts off as baker/lawyer/grocer of
levels 5/6/7 respectively

  scenario 1
  PC changes to plumber/salesman/soldier
-> when the lowest new baseclass reaches one higher
than the highest original baseclass (in this case
an '8') then the PC gets the use of the original
baseclasses back.

  
scenario 2
PC changes to baker/gardener/dancer
->baker stays at 5, but other two new baseclasses 
are at 1, and when the lowest of them is 1 higher 
than the highest of the two remaning original baseclasses 
(again an '8'), then the two original baseclasses can be 
used by the PC at their previous levels of 6/7.
*/
  // [/quote]
  { 
    int i,n, lowest, highest;
    lowest = 999;  // Lowest of current class
    highest = 0;   // Highest of previous class
    n = GetBaseclassStatsCount();
    for (i=0; i<n; i++)
    {
      const BASECLASS_STATS *pBaseclassStats;
      pBaseclassStats = PeekBaseclassStats(i);
      if ((pBaseclassStats->currentLevel != 0) && (pBaseclassStats->previousLevel != 0))
      {
        // Ignore baseclasses that are present in both
        // the current class and the previous class.
        continue;
      };
      if (pBaseclassStats->currentLevel < lowest)
      {
        lowest = pBaseclassStats->currentLevel;
      };
      if (pBaseclassStats->previousLevel > highest)
      {
        highest = pBaseclassStats->previousLevel;
      };
    };
    temp__canUsePrevClass = 0;
    if (lowest > highest) temp__canUsePrevClass = 1;
  };
  return temp__canUsePrevClass == 1;

/*
  int currLevel = 0;
  switch (GetClass())
  {
  case Fighter:
    currLevel = FighterLevel;
    break;
  case Cleric:
    currLevel = ClericLevel;
    break;
  case Ranger:
    currLevel = RangerLevel;
    break;
  case Paladin:
    currLevel = PaladinLevel;
    break;
  case MagicUser:
    currLevel = MULevel;
    break;
  case Thief:
    currLevel = ThiefLevel;
    break;
  case Druid:
    currLevel = DruidLevel;
    break;
  default: return FALSE;
  }

  int prevLevel = 0;
  switch (GetPreviousClass())
  {
  case Fighter:
    prevLevel = prevFighterLevel;
    break;
  case Cleric:
    prevLevel = prevClericLevel;
    break;
  case Ranger:
    prevLevel = prevRangerLevel;
    break;
  case Paladin:
    prevLevel = prevPaladinLevel;
    break;
  case MagicUser:
    prevLevel = prevMULevel;
    break;
  case Thief:
    prevLevel = prevThiefLevel;
    break;
  case Druid:
    prevLevel = prevDruidLevel;
    break;
  default: return FALSE;
  }

  if ((currLevel >= prevLevel) && (prevLevel > 0))
    return TRUE;
  return FALSE;
*/
}
#else  //OldDualClass20180126

BOOL CHARACTER:: CanUseBaseclass(const BASECLASS_STATS *pBaseclassStats) const
{
  // He can use this baseclass if it is a current baseclass.
  //
  // He can use this baseclass if it is a previous baseclass
  // and a current baseclass level is greater than this previous baseclass level.
  //
  if (pBaseclassStats->currentLevel > 0) return TRUE;
  if (pBaseclassStats->previousLevel <= 0) return FALSE;
  {
    int i, n;
    n = baseclassStats.GetCount();
    for (i=0; i<n; i++)
    {
      if (PeekBaseclassStats(i)->previousLevel > 0) continue;
      if (PeekBaseclassStats(i)->currentLevel > pBaseclassStats->previousLevel) return TRUE;
    };
  };
  return FALSE;
}

#endif //OldDualClass20180126
//*****************************************************************************
// NAME: CHARACTER::IsDualClass
//
// PURPOSE:  
//*****************************************************************************
BOOL CHARACTER::IsDualClass() const
{
/*
  if (prevFighterLevel > 0)
    return TRUE;
  if (prevClericLevel > 0)
    return TRUE;
  if (prevRangerLevel > 0)
    return TRUE;
  if (prevPaladinLevel > 0)
    return TRUE;
  if (prevMULevel > 0)
    return TRUE;
  if (prevThiefLevel > 0)
    return TRUE;
  if (prevDruidLevel > 0)
    return TRUE;
*/



// I guess we can go through all the character's
// baseclasses to see if any has a non-zero 'prevLevel'.

  int i, n;
  n = baseclassStats.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekBaseclassStats(i)->previousLevel > 0) return TRUE;
    //if (baseclassStats[i].previousLevel > 0) return TRUE;
  }
  return FALSE;
}

#ifdef OldDualClass201800126

//*****************************************************************************
// NAME: CHARACTER::GetPreviousClass
//
// PURPOSE:  
//*****************************************************************************
//classType CHARACTER::GetPreviousClass() const
CLASS_ID CHARACTER::GetPreviousClass() const
{
/*
  if (prevFighterLevel > 0)
    return Fighter;
  if (prevClericLevel > 0)
    return Cleric;
  if (prevRangerLevel > 0)
    return Ranger;
  if (prevPaladinLevel > 0)
    return Paladin;
  if (prevMULevel > 0)
    return MagicUser;
  if (prevThiefLevel > 0)
    return Thief;
  if (prevDruidLevel > 0)
    return Druid;
*/
  int i, j, m, n1, n2;
  n1 = baseclassStats.GetSize();
  for (i=0; i<n1; i++)
  {
    if (PeekBaseclassStats(i)->previousLevel > 0)
    //if (baseclassStats[i].previousLevel > 0)
    {
      // Now we need to find a class that matches this baseclass!
      n2 = classData.GetCount();
      for (j=0; j<n2; j++)
      {
        m = classData.PeekClass(j)->GetCount();
        if (m != 1) continue;
        if (*classData.PeekClass(j)->PeekBaseclassID(0) == PeekBaseclassStats(i)->baseclassID)
        //if (*classData.PeekClass(i)->PeekBaseclassID(0) == baseclassStats[i].baseclassID)
        {
          return classData.PeekClass(j)->ClassID();
        };
      };
    };
  };
  return CLASS_ID();
}
#endif // OldDualClass20180126 


/*
//*****************************************************************************
// NAME: CHARACTER::GetPreviousClassLevel
//
// PURPOSE:  
//*****************************************************************************
BYTE CHARACTER::GetPreviousClassLevel() const
{
  die("Not Needed?"); //Not Implemented(0x5d9365, false);
  return 0;
}
*/

BOOL CHARACTER::CanChangeToClass(const CLASS_DATA *pFromClass,
                                 const CLASS_DATA *pToClass)
{
/*
 *
 *
 * Manikus instructed me via email that he wanted the
 * engine to have no part in this decision.  He wanted
 * his script to do all the work:
 *
 I would prefer the new Hook to do all the checks. I 
 would like to verify the possible classes one at a 
 time. Character and Class contexts, as suggested, 
 sound like the right place to handle this.

-Eric

 * But I will leave this code lying around in case
 * he changes his mind.
 * PRS 20170916

  const BASECLASS_LIST *pBaseclassList;
  int numBaseclass, baseclassIndex;
  pBaseclassList = pClass->BaseclassList();
  numBaseclass = pBaseclassList->GetBaseclassIDCount();
  for (baseclassIndex=0; baseclassIndex < numBaseclass; baseclassIndex++)
  {
    const BASECLASS_ID *pBaseclassID;
    const BASE_CLASS_DATA *pBaseclass;
    pBaseclassID = pBaseclassList->PeekBaseclassID(baseclassIndex);
    pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
    if (pBaseclass == NULL) continue;  // or should we 'break'?
    { // check character's alignment fits new baseclass.
      if ((pBaseclass->m_allowedAlignments & alignment) == 0)
      {
        return FALSE;
      };
    };
    {
      //check character's ability scores match new baseclass
      const ABILITY_REQ *pAbilityReq;
      int numAbilityReq, abilityReqIndex;
      numAbilityReq = pBaseclass->GetAbilityReqCount();
      for (abilityReqIndex=0; abilityReqIndex<numAbilityReq; abilityReqIndex++)
      {
        pAbilityReq = pBaseclass->PeekAbilityReq(abilityReqIndex);
        if (GetAbilityScore(pAbilityReq->m_abilityID) < pAbilityReq->GetMin())
        {
          return FALSE;
        };
      };
    };
  };
  return TRUE;
  *
  *
  */
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetClassContext(pFromClass);
    scriptContext.SetCharacterContext(this);
    hookParameters[5] = pToClass->ClassID();
    pFromClass->RunClassScripts("CanChangeFromClass", 
                                ScriptCallback_RunAllScripts,
                                NULL,
                                "Test if this character can change class"
                                );
    if (hookParameters[0] != 'Y') return FALSE;
    hookParameters[0].Empty();
    scriptContext.SetClassContext(pToClass);
    scriptContext.SetCharacterContext(this);
    hookParameters[5] = pFromClass->ClassID();
    pToClass->RunClassScripts("CanChangeToClass", 
                               ScriptCallback_RunAllScripts,
                               NULL,
                               "Test if this character can change to this class"
                              );
    if (hookParameters[0] != 'Y') return FALSE;
    return TRUE;
  };
}      



//*****************************************************************************
// NAME: CHARACTER::CanChangeClassHuman
// NAME: CHARACTER::CanChangeClass
//
// PURPOSE:  
//*****************************************************************************
//BOOL CHARACTER::CanChangeClassHuman()
//BOOL CHARACTER::CanChangeClass()
int CHARACTER::CreateChangeClassList(CArray<CLASS_ID, CLASS_ID&> *pClassArray)
{ // Return number of possible classes.
  int result = 0;
  if (this == &FakeCharacter) return 0;
  if (pClassArray != NULL)
  {
    pClassArray->RemoveAll();
  };
  if (GetType() == MONSTER_TYPE)
  {
    return 0;
  };
  //// must be human
  //if (race() != Human)
  //  return FALSE;
  {
    const RACE_DATA *pRace;
    //pRace = raceData.PeekRace(race);
    pRace = PeekRaceData();
    if (pRace == NULL) return FALSE;
    if (!pRace->m_canChangeClass)
    {
      return 0;
    };
  };
  // if already changed classes
  if (IsDualClass())
  {
    return 0;
  };
  // Search for a possible new class
  const CLASS_DATA *pMyClass;
  pMyClass = classData.PeekClass(this->classID);
  {
    int classIndex, numClass;
    numClass = classData.GetClassCount();
    for (classIndex=0; classIndex < numClass; classIndex++)
    {
      const CLASS_DATA *pClass;
      pClass = classData.PeekClass(classIndex);
      if (classID == pClass->ClassID()) continue;
      //
      // Check each of the new class' baseclasses.
      //
      {
        if (CanChangeToClass(pMyClass, pClass))
        {
          result++;
          if (pClassArray != NULL)
          {
            CLASS_ID classID2Add;
            classID2Add = pClass->ClassID();
            pClassArray->Add(classID2Add);
          };
        };
      };
    };  
  }
  // must have min of 15 in primary attribute(s) of
  // current class.


/*  
  if ((isFighter(GetClass())) && (GetStr() < 15))
   return FALSE;

  if (isPaladin(GetClass()))
  {
   if (GetStr() < 15) return FALSE;
   if (GetWis()   < 15) return FALSE;
  }

  if (isRanger(GetClass()))
  {
   if (GetStr()     < 15) return FALSE;
   if (GetWis()       < 15) return FALSE;
   if (GetInt() < 15) return FALSE;
  }

  if ((isCleric(GetClass())) && (GetWis() < 15))
   return FALSE;

  if (isDruid(GetClass()))
  {
   if (GetCha() < 15) return FALSE;
   if (GetWis()   < 15) return FALSE;
  }

  if ((isMagicUser(GetClass())) && (GetInt() < 15))
   return FALSE;

  if ((isThief(GetClass())) && (GetDex() < 15))
   return FALSE;

  // must have min of 17 in primary attribute(s) of
  // new class
  //
  // don't know new class yet, so just check each
  // possibility and verify that at least one is
  // available
  //


  if (CanDualClassCleric()) return TRUE;
  if (CanDualClassRanger()) return TRUE;
  if (CanDualClassDruid()) return TRUE;
  if (CanDualClassPaladin()) return TRUE;
  if (CanDualClassThief()) return TRUE;
  if (CanDualClassFighter()) return TRUE;
  if (CanDualClassMagicUser()) return TRUE;


  return FALSE;
  */
  return result;
}

//*****************************************************************************
//    NAME: CHARACTER::HumanChangeClass
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::HumanChangeClass(classType newClass)
void CHARACTER::HumanChangeClass(const CLASS_ID& newClassID)
{
/*
  switch (GetClass())
  {
  case Cleric:
    prevClericLevel = ClericLevel;
    break;
  case MagicUser:
    prevMULevel = MULevel;
    break;
  case Druid:
    prevDruidLevel = DruidLevel;
    break;
  case Ranger:
    prevRangerLevel = RangerLevel;
    break;
  case Thief:
    prevThiefLevel = ThiefLevel;
    break;
  case Paladin:
    prevPaladinLevel = PaladinLevel;
    break;
  case Fighter:
    prevFighterLevel = FighterLevel;
    break;
  default: return;
  }
*/
  /*
  * Email from Manikus 20171101
  * When a PC changes class from a non-spellcasting class to a spellcasting class,
  * or to a different spellcasting, no spells are available of the new class.
  *
  * I think this is all about special abilities. I believe all that I need to make
  * this right is a new hook. We have a hook called TrainingCharacters that is
  * called at the time immediately after the "Train"button is selected. What I
  * would like is something that does the same thing when the "Change Class" button
  * is selected and the change is applied. Does that make sense? Fighter can change
  * to Cleric, so player selects "Cleric" from the list and presses teh OK button
  * and the changes are applied and we go back to the Training Hall menu - I would
  * like the Hook to fire after OK and before the menu - changes are applied, scripts,
  * if present, are run and we go back to the menu.
  */
  {
    CLASS_DATA *pClass;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CString result;
    pClass = classData.GetClass(classID);
    scriptContext.SetClassContext(pClass);
    scriptContext.SetCharacterContext(this);
    result = pClass->m_specialAbilities.RunScripts
                (CHANGE_CLASS_FROM,
                ScriptCallback_RunAllScripts,
                NULL,
                "Character just changed from this class",
                ScriptSourceType_Class,
                pClass->ClassID()
                );
  }; 
  {
    int i, n;
    n = GetBaseclassStatsCount();
    for (i=0; i<n; i++)
    {
      BASECLASS_STATS *pBaseclassStats;
      pBaseclassStats = GetBaseclassStats(i);
      pBaseclassStats->previousLevel = pBaseclassStats->currentLevel;
      pBaseclassStats->currentLevel = 0;
    };
  };

  //SetClass(newClass);
  {
    int i, n;
    const CLASS_DATA *pNewClass;
    classID = newClassID;
    pNewClass = classData.PeekClass(classID);
    // Here we need to ensure that baseclass stats exist
    // for all baseclasses of the new class.  We will add
    // new baseclass stats as necessary.
    n = pNewClass->GetCount(); // baseclass count.
    for (i=0; i<n; i++)
    {
      const BASE_CLASS_DATA *pBaseclass;
      const BASECLASS_ID *pBaseclassID;
      pBaseclassID = pNewClass->PeekBaseclassID(i);
      pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
      // New check to see if we a baseclass stats exists
      // for this new baseclass.
      {
        int j, m;
        m = GetBaseclassStatsCount();
        for (j=0; j<m; j++)
        {
          if (PeekBaseclassStats(i)->baseclassID 
                        == pBaseclass->BaseclassID())
          {
            break;
          };
        };
        if (j != m) continue;  //baseclass stats entry already exists
        {
          BASECLASS_STATS bcs;
          bcs.baseclassID = *pBaseclassID;
          bcs.currentLevel = 1;
          bcs.x_experience = 0;
          bcs.preDrainLevel = 0;
          bcs.previousLevel = 0;
          baseclassStats.Add(bcs);
        };
      };
    };
  };
  /* Unready all items when character changes class.   */
  {
    POSITION pos;
    for (pos = myItems.GetHeadPosition(); pos!=NULL;)
    {
      ITEM *pItem;
      pItem = &myItems.GetNext(pos);
      pItem->ClearReadyLocation();
    };
  };
  /*
  * Email from Manikus 20171101
  * When a PC changes class from a non-spellcasting class to a spellcasting class,
  * or to a different spellcasting, no spells are available of the new class.
  *
  * I think this is all about special abilities. I believe all that I need to make
  * this right is a new hook. We have a hook called TrainingCharacters that is
  * called at the time immediately after the "Train"button is selected. What I
  * would like is something that does the same thing when the "Change Class" button
  * is selected and the change is applied. Does that make sense? Fighter can change
  * to Cleric, so player selects "Cleric" from the list and presses teh OK button
  * and the changes are applied and we go back to the Training Hall menu - I would
  * like the Hook to fire after OK and before the menu - changes are applied, scripts,
  * if present, are run and we go back to the menu.
  */
  {
    CLASS_DATA *pClass;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CString result;
    pClass = classData.GetClass(classID);
    scriptContext.SetClassContext(pClass);
    scriptContext.SetCharacterContext(this);
    result = pClass->m_specialAbilities.RunScripts
                (CHANGE_CLASS_TO,
                ScriptCallback_RunAllScripts,
                NULL,
                "Character just changed to this class",
                ScriptSourceType_Class,
                pClass->ClassID()
                );
  }; 
}


//void CHARACTER::SetClass(classType newClass)
void CHARACTER::SetClass(const CLASS_ID& newClassID)
{
  /*
  charClass = newClass;
  switch (charClass)
  {
  case Cleric:
    ClericLevel = 1;
    break;
  case MagicUser:
    MULevel = 1;
    break;
  case Druid:
    DruidLevel = 1;
    break;
  case Ranger:
    RangerLevel = 1;
    break;
  case Thief:
    ThiefLevel = 1;
    break;
  case Paladin:
    PaladinLevel = 1;
    break;
  case Fighter:
    FighterLevel = 1;
    break;
  default: return;
  }
*/
  const CLASS_DATA *pNewClass;
  const BASECLASS_ID *pBaseclassID;
  BASECLASS_STATS newBcStats;
  int i, n;
  this->classID = newClassID;
  // Set all baseclass levels to 1;
  pNewClass = classData.PeekClass(newClassID);
  if (pNewClass == NULL) return;
  n = pNewClass->GetCount();
  baseclassStats.RemoveAll();
  for (i=0; i<n; i++)
  {
    pBaseclassID = pNewClass->PeekBaseclassID(i);
    newBcStats.baseclassID = *pBaseclassID;
    baseclassStats.Add(newBcStats);
  };
}


//*****************************************************************************
// NAME:    CHARACTER::canCastSpells
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::CanCastSpells()
{
  if (GetType() == MONSTER_TYPE) return FALSE;

#ifdef UAFEngine

  // Search through the character's special abilities
  // to see if we can find a CanCastSpells script.
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    COMBATANT *pCombatant;
    const CLASS_DATA *pClass;
    //pClass = &classData[GetClass()];
    pClass = classData.PeekClass(GetClass());
    scriptContext.SetCharacterContext(this);
#ifdef newCombatant
    pCombatant = m_pCombatant;
#else
    pCombatant = this->GetCombatant();
#endif
    if (pCombatant != NULL)
    {
#ifdef newCombatant
      scriptContext.SetCombatantContext(pCombatant);
#else
      scriptContext.SetCombatantContext(this->GetCombatant());
#endif
    };
    scriptContext.SetClassContext(pClass);
    if (pCombatant != NULL)
    {
      if (!pCombatant->RunCombatantScripts(
                     CAN_CAST_SPELLS, 
                     ScriptCallback_LookForChar,
                     "N",
                     "Test if combatant can cast spells"
                             ).IsEmpty()) return FALSE;
    };

    if (!RunCharacterScripts(
                    CAN_CAST_SPELLS, 
                    ScriptCallback_LookForChar,
                    "N",
                    "Test if this combatant can cast spells"
                            ).IsEmpty()) return FALSE;

    if (pClass!=NULL)
    {
      if (!pClass->RunClassScripts(CAN_CAST_SPELLS, 
                    ScriptCallback_LookForChar,
                    "N",
                    "Test if this combatant can cast spells"))
      {
        return FALSE;
      };
    };
  };
  /*
  if (GetAdjStatus() != Okay) return FALSE;

  DWORD src;
  GLOBAL_SPELL_ID skey;
  
  if (GetAdjSpecAb(SA_HoldPerson,&src,&skey))
  {
    SetNbrAttacks(0.0);
    Not Implemented(0x5f4d0, FALSE); //QueueUsedSpecAb(SA_HoldPerson,src,skey);
    return FALSE;
  }
  else if (GetAdjSpecAb(SA_Sleep,&src,&skey))
  {
    SetNbrAttacks(0.0);
    Not Implemented(0xcca2, FALSE);//QueueUsedSpecAb(SA_Sleep,src,skey);
    return FALSE;
  }
  else if (GetAdjSpecAb(SA_Silence,&src,&skey))
  {
    SetNbrAttacks(0.0);
    Not Implemented(0xd32a, FALSE); //QueueUsedSpecAb(SA_Silence,src,skey);
    return FALSE;
  } 
  */
#endif
  
/*
  if (CanUsePrevClass())
  {
    switch (GetPreviousClass())
    {
    case Cleric:
    case MagicUser:
    case Druid:   return TRUE;
    case Ranger:  if (prevRangerLevel >= 8) return TRUE;  break;
    case Paladin: if (prevPaladinLevel >= 9) return TRUE; break;
    default: break;
    }
  }

  switch (GetClass())
  {
   case Cleric:
   case MagicUser:
   case Druid:
   case C_MU:
   case C_F:
   case C_R:
   case C_T:
   case F_D:
   case C_F_MU:
   case D_MU:
   case F_MU_D:
   case F_MU:
   case F_MU_T:
   case MU_T:
     return TRUE;

   case Ranger: if (RangerLevel >= 8) return TRUE;  break;
   case Paladin:if (PaladinLevel >= 9)return TRUE;  break;
   default: return FALSE;
  }
  return FALSE;
  */
  return TRUE;
}


//*****************************************************************************
// NAME:    CHARACTER::canMemorizeSpells
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::CanMemorizeSpells(int circumstance)
{
  // circumstance = 0 Magic Menu; should we display "MEMORIZE"
  //                1 Resting; should character memorize spells.
  if (GetType() == MONSTER_TYPE) return FALSE;

#ifdef UAFEngine
  char result[5];
  memcpy(result, "YYYYY", 5); // Innitial assumption

  // Search through the character's special abilities
  // to see if we can find a CanCastSpells script.
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CString scriptResult;
    const CLASS_DATA *pClass;
    //pClass = &classData[GetClass()];
    pClass = classData.PeekClass(GetClass());
    scriptContext.SetCharacterContext(this);
    scriptContext.SetClassContext(pClass);

    scriptResult = RunGlobalScript("Global_Memorize", "CanMemorizeSpells", true);
    ScriptCallback_YessesAndNoes(CBF_EXAMINESCRIPT, 
                                 &scriptResult,
                                 result);

    pClass->RunClassScripts
                 (CAN_MEMORIZE_SPELLS, 
                  ScriptCallback_YessesAndNoes,
                  result,
                  "Test if this character can memorize spells"
                  );
    this->RunCharacterScripts(CAN_MEMORIZE_SPELLS, 
                              ScriptCallback_YessesAndNoes,
                              result,
                              "Test if this character can memorize spells"
                              );

  };
  return result[circumstance] == 'Y';
#else // editor
  return TRUE;
#endif
}

//
// this is used for the GiveDamage event. Normally,
// damage is given during combat in combatant.cpp
//
//*****************************************************************************
//    NAME: CHARACTER::giveCharacterDamage
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::giveCharacterDamage(
                    spellSaveEffectType eventSave, 
                    int attackTHAC0, 
                    int dmgDice, 
                    int dmgDiceQty, 
                    int dmgBon, 
                    spellSaveVersusType spellSave, 
                    int saveBonus,
                    CHARACTER *pAttacker)
{
  int result = 0;
  int need = 0;
  BOOL saved = FALSE;
  
  if (eventSave == UseTHAC0)
  {
    int catt = attackTHAC0 - GetEffectiveAC();
    need = max(catt, 1);
    need = min(need, 20);
    
    if (RollDice(20,1) >= need)
      result = RollDice(dmgDice, dmgDiceQty, dmgBon);      
  }
  else
  {
    result = RollDice(dmgDice, dmgDiceQty, dmgBon);          
    saved = DidSaveVersus(spellSave, saveBonus, NULL, NULL);
    
    if (saved)
    {
      switch (eventSave)
      {
      case NoSave:// full affect
        break;
      case SaveNegates:
        result = 0;
        break;
      case SaveForHalf:
        result /= 2;
        result = max(1,result); 
        break;
      default: result=0; break;
      }
    }
  }
  
  if (result > 0)
  {
    giveCharacterDamage(result);
#ifdef UAFEngine
    PlayCharHit();
    CString msg;
    msg.Format("%s takes %i damage",
               GetName(), result);
    
    FormatPausedText(pausedTextData, msg);
    DisplayPausedText(pausedTextData, whiteColor, 0);    
#endif
  }
}

//*****************************************************************************
// NAME:    CHARACTER::giveCharacterDamage
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::giveCharacterDamage(int damage)
{   
   int HP;
   charStatusType stype = GetAdjStatus();

   if (   (stype == Okay)
       || (stype == Running)
       || (stype == Unconscious)
       || (stype == Animated)
       || (stype == Dying))
   {
      hitPoints -= damage;

      if (hitPoints < -10)
         hitPoints = -10;

      if (hitPoints > maxHitPoints)
        hitPoints = maxHitPoints;

      HP = GetAdjHitPoints();
      if (globalData.GetDeadAtZeroHP())
      {
        if (HP <= 0)
          SetStatus(Dead);
      }
      else
      {
        if      (HP <= -10)  SetStatus(Dead);
        else if (HP < 0)     SetStatus(Dying);
        else if (HP == 0)    SetStatus(Unconscious);
      };
   }
}

/*
CBRESULT ScriptCallback_AddToHookParameter_1(CBFUNC func, CString *scriptResult, void *pkt)
{
  HOOK_PARAMETERS *pHookParameters = (HOOK_PARAMETERS *)pkt;
  switch(func)
  {
  case CBF_EXAMINESCRIPT:
    if (scriptResult->IsEmpty()) return CBR_CONTINUE;
    {
      int delta;
      sscanf(*scriptResult, "%d", &delta);
      if (delta != 0)
      {
        int par1 = 0;  // In case hookParameter[1] is empty
        sscanf((*pHookParameters)[1], "%d", &par1);
        par1 += delta;
        (*pHookParameters)[1].Format("%d", par1);
      };
      return CBR_CONTINUE;
    };
  case CBF_ENDOFSCRIPTS:
    scriptResult->Empty();
    return CBR_STOP;
  default: 
    return CBR_CONTINUE;
  };
}
*/


//*****************************************************************************
// NAME:    CHARACTER::DidSaveVersus
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::DidSaveVersus(spellSaveVersusType stype, 
                          int bonus, 
                          SPELL_DATA *pSpell,
                          CHARACTER *pAttacker)
{
   CString scriptName;
   SKILL_ID skillID;
   const CString *pSaveVersus;
   switch (stype)
   {
   case ParPoiDM:     pSaveVersus = &Save_Vs_PPDM; break;
   case PetPoly:      pSaveVersus = &Save_Vs_PP;   break;
   case RodStaffWand: pSaveVersus = &Save_Vs_RSW;  break;
   case Sp:           pSaveVersus = &Save_Vs_SP;   break;
   case BreathWeapon: pSaveVersus = &Save_Vs_BR;   break;
   default:
     return FALSE;
   };
   skillID = *pSaveVersus;


   int mr = GetAdjMagicResistance();
   if (mr > 0)
   {
     int mr_roll = RollDice(100,1);
     if (mr_roll <= mr)
       return TRUE;
   }

   BOOL success = FALSE; 
   int score=0;
   score = GetAdjSkillValue(skillID, false, true);

   score = min(score, 20);
   int roll = RollDice(20,1);

#ifdef UAFEngine
   if (pAttacker != NULL)
   {
     ModifySaveRollAsTarget(pAttacker, &roll);

     if (pAttacker!=this)
     {
       pAttacker->ModifySaveRoll(this, &roll);
     };
   };
#endif

#ifdef UAFEngine
/*
 * All of the Save_Vs scripts are deprecated.
 * Saving throws depend on skills (See GetAdjSkillValue() a few line up).
  {
    // We are going to allow scripts to modify the score.
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    int delta;
    if (pAttacker != NULL) 
    {
      scriptContext.SetAttackerContext(pAttacker);
    };
    scriptContext.SetTargetContext(this);
    if (pSpell != NULL)
    {
      scriptContext.SetSpellContext(pSpell);
    };
    hookParameters[5].Format("%d",score);
    hookParameters[6].Format("%d",roll);

    if (pAttacker != NULL)
    {
      pAttacker->specAbs.RunScripts(
                     scriptName + "_Cast",
                     ScriptCallback_RunAllScripts,
                     &hookParameters,
                     pAttacker->name, 
                     (pSpell == NULL) ? "" : pSpell->Name
                     );
    };
    this->specAbs.RunScripts(
                   scriptName + "_Save",
                   ScriptCallback_RunAllScripts,
                   &hookParameters,
                   (pAttacker == NULL) ? "" : pAttacker->name, 
                   (pSpell == NULL) ? "" : pSpell->Name
                   );
    if (pSpell != NULL)
    {
      pSpell->specAbs.RunScripts(
                     scriptName + "_Spell",
                     ScriptCallback_RunAllScripts,
                     &hookParameters,
                     (pAttacker == NULL) ? "" : pAttacker->name, 
                     pSpell->Name
                     );
    };
    if ((pAttacker != NULL) && (pAttacker->GetType() == MONSTER_TYPE))
    {
      //int monsterIndex;
      //monsterIndex = pAttacker->m_pCombatant->origKey;
      {
        //MONSTER_DATA *pMonsterData;
        const MONSTER_DATA *pMonsterData;
        //pMonsterData = monsterData.GetMonsterData(monsterIndex);
        pMonsterData = monsterData.PeekMonster(monsterID);
        if (pMonsterData != NULL)
        {
          pMonsterData->specAbs.RunScripts(
                   scriptName + "_MonCast",
                   ScriptCallback_RunAllScripts,
                   &hookParameters,
                   pMonsterData->Name,
                   (pSpell == NULL) ? "" : pSpell->Name);
        };
      };

    };
    if(this->GetType() == MONSTER_TYPE)
    {
      //int monsterIndex;
      //monsterIndex = this->m_pCombatant->origKey;
      {
        //MONSTER_DATA *pMonsterData;
        const MONSTER_DATA *pMonsterData;
        //pMonsterData = monsterData.GetMonsterData(monsterIndex);
        pMonsterData = monsterData.PeekMonster(monsterID);
        if (pMonsterData != NULL)
        {
          pMonsterData->specAbs.RunScripts(
                   scriptName + "_MonSave",
                   ScriptCallback_RunAllScripts,
                   &hookParameters,
                   pMonsterData->Name,
                   (pSpell == NULL) ? "" : pSpell->Name);
        };
      };

    };
    score = 0;
    delta = 0;
    sscanf(hookParameters[5], "%d", &score);
    sscanf(hookParameters[0], "%d", &delta);
    sscanf(hookParameters[6], "%d", &roll);
    score += delta;
    score = min(score, 20);
    score = max(score, 1);
    roll  = min(roll, 20);
    roll  = max(roll,  1);
  };
  */
#endif


   if (roll < score)
      success = FALSE;
   else
      success = TRUE;

   return success;
}


//*****************************************************************************
// NAME:    CHARACTER::HasItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//BOOL CHARACTER::HasItem(GLOBAL_ITEM_ID giID)
BOOL CHARACTER::HasItem(const ITEM_ID& itemID)
{
  //return myItems.HaveItem(giID);
  return myItems.HaveItem(itemID);
}


//*****************************************************************************
// NAME: CHARACTER::SaveArt
//
// PURPOSE:  
//*****************************************************************************
void CHARACTER::SaveArt()
{
  icon.SavePicData(GLOBAL_ART);
  smallPic.SavePicData(GLOBAL_ART);
}

//*****************************************************************************
//    NAME: CHARACTER::GetPrimeSpellCastingScore
//
// PURPOSE: 
//
//*****************************************************************************
//BYTE CHARACTER::GetPrimeSpellCastingScore(WORD ClassFlag)
int CHARACTER::GetPrimeSpellCastingScore(const BASE_CLASS_DATA *pBaseclass, const SCHOOL_ID& schoolID) const
{
  //ASSERT( NumClassBits(ClassFlag) == 1 );
/*
  switch (ClassFlag)
  {
  case MagicUserFlag: return GetAdjInt();
  case ClericFlag:    return GetAdjWis();
  case ThiefFlag:     return GetAdjWis();
  case FighterFlag:   return GetAdjWis();
  case PaladinFlag:   return GetAdjWis();
  case RangerFlag:    return GetAdjWis();
  case DruidFlag:     return GetAdjWis();
  }
  ASS ERT(FALSE);
  return 0;
*/
  //if (pBaseclass->m_primeCasting == Ability_Wisdom)       return GetAdjWis();
  //if (pBaseclass->m_primeCasting == Ability_Intelligence) return GetAdjInt();
  
  const CASTING_INFO  *pCastingInfo;
  pCastingInfo = pBaseclass->PeekCastingInfo(schoolID);
  if (pCastingInfo == NULL)
  {
    return 0;
  };
  return GetAbilityScore(pCastingInfo->primeAbility);
}
/*
//*****************************************************************************
//    NAME: CHARACTER::PrimeSpellCastingScore
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::PrimeSpellCastingScore(int spellSchool)
{
  switch (spellSchool)
  {
  case MagicUserSchool: return GetAdjInt();
  case ClericSchool:    return GetAdjWis();
  }
  ASS ERT(FALSE);
  return 0;
}
*/
void CLASS_DATA::CreateAllowedSchoolList(SCHOOL_LIST *pSchoolList) const
{
  int i, n;
  SCHOOL_ID schoolID;
  pSchoolList->Clear();
  n = GetCount(); // number of baseclasses
  for (i=0; i<n; i++)
  {
    const BASE_CLASS_DATA *pBaseclass;
    const BASECLASS_ID *pBaseclassID;
    pBaseclassID = PeekBaseclassID(i);         // Next baseclass ID
    pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID); // Next baseclass data
    if (pBaseclass == NULL) continue;
    //pSchoolList->Add(pBaseclass->spellSchool);
    int j, m;
    m = pBaseclass->GetCastingInfoCount();
    for (j=0; j<m; j++)
    {
      schoolID = pBaseclass->PeekCastingInfo(j)->schoolID;
      pSchoolList->Add(schoolID);
    };
  };
}

BOOL CLASS_DATA::MatchSpellBaseclass(const SPELL_DATA *pSpell) const
{ // Does any spell baseclass match a character's baseclass?
  int i, n;
  n = pSpell->allowedBaseclasses.GetCount();
  for (i=0; i<n; i++)   // for each spell-<allowed baseclass
  {
    const BASECLASS_ID *pSpellBaseclass;
    pSpellBaseclass = pSpell->allowedBaseclasses.PeekBaseclassID(i);
    if (LocateBaseclass(*pSpellBaseclass) >= 0)
    {
      return TRUE;
    };
  };
  return FALSE;
} 

BOOL CLASS_DATA::MatchSpellSchool(const SCHOOL_LIST *pSchoolList) const
{
  int i, n;
  n = GetCount();   // Number of baseclasses in this class.
  for (i=0; i<n; i++)
  {
    const BASECLASS_ID *pBaseclassID;
    const BASE_CLASS_DATA *pBaseclassData;
    pBaseclassID = PeekBaseclassID(i);
    pBaseclassData = baseclassData.PeekBaseclass(*pBaseclassID);
    if (pBaseclassData == NULL) continue;
    int j, m;
    m = pBaseclassData->GetCastingInfoCount();
    for (j=0; j<m; j++)
    {
      if (pSchoolList->LocateSchool(pBaseclassData->PeekCastingInfo(j)->schoolID)) return TRUE;
    };
  };
  return FALSE;
}

int CLASS_DATA::LocateBaseclass(const BASECLASS_ID& baseclassID) const
{
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    if (*this->m_baseclasses.PeekBaseclassID(i) == baseclassID) return i;
  };
  return -1;
}

//*****************************************************************************
// NAME:    CHARACTER::KnowSpellyyy   ('yyy' to differentiate from the spellbook function)
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CHARACTER::KnowSpellyyy(const SPELL_DATA *pSpell, BOOL know)
{
  BOOL result;
  result = m_spellbook.KnowSpellxxx(pSpell, know);
  if (!result) return FALSE;
  if (know)
  { // Remove from ASL entry $KnowableSpells$
    DelKnowableSpell(pSpell->SpellID());
  }
  else
  { // Add to ASL entry $KnowableSpells
  };
  return TRUE;
};

#ifdef OldDualClass20180126
void CHARACTER::UpdateSpellAbilityForBaseclass(const BASE_CLASS_DATA *pBaseclass, int effectiveBaseclassLevel)
{
  int m, j;
  m = pBaseclass->GetCastingInfoCount();
  for (j=0; j<m; j++)
  {
    int k, spellLevel;
    int primeScore, bonusScore, maxSpells, maxSpellLevel, baseclassLevel;
    const BYTE *pMaxSpellsByPrime;
    const BYTE *pMaxSpellLevelsByPrime;
    const BYTE *pSpellLimits;
    const CASTING_INFO *pCastingInfo;
    const SCHOOL_ID *pSchoolID;
    const CString *pPrimeAbility;
    const CString *pBonusAbility;
    SCHOOL_ABILITY *pSchoolAbility;
    pCastingInfo = pBaseclass->PeekCastingInfo(j);
    pSchoolID = &pCastingInfo->schoolID;
    // Do we already have a spell ability for this school?
    k = spellAbility.LocateSchoolAbility(*pSchoolID);
    if (k < 0)
    {
      SCHOOL_ABILITY schoolAbility;
      schoolAbility.schoolID = *pSchoolID;
      k = spellAbility.GetSchoolAbilityCount();
      // We need to add a school to the spellAbility
      spellAbility.Add(schoolAbility);
    };
    pSchoolAbility             = spellAbility.GetSchoolAbility(k);
    pPrimeAbility              = &pCastingInfo->primeAbility;
    pBonusAbility              = &pBaseclass->m_spellBonusAbility;
    //baseclassLevel             = GetCurrentLevel(*pBaseclassID);
    baseclassLevel             = effectiveBaseclassLevel;
    pMaxSpellsByPrime          = pCastingInfo->m_maxSpellsByPrime;
    pMaxSpellLevelsByPrime     = pCastingInfo->m_maxSpellLevelsByPrime;
    primeScore                 = GetAbilityScore(*pPrimeAbility);
    bonusScore                 = GetAbilityScore(*pBonusAbility);
    pSpellLimits               = pCastingInfo->m_spellLimits[baseclassLevel-1];
    {
      int idx;
      maxSpellLevel            = 0;
      for (idx=0; idx<MAX_SPELL_LEVEL; idx++)
      {
        if (pSpellLimits[idx] != 0) maxSpellLevel = idx+1;
      };
    };
    maxSpells                  = pMaxSpellsByPrime[primeScore-1];
    if (maxSpells > pSchoolAbility->maxSpells)
    {
      pSchoolAbility->maxSpells = maxSpells;
    };
    // Replace 20171116 PRS
    //if (maxSpellLevel > pSchoolAbility->maxAbilitySpellLevel)
    //{  
    //  pSchoolAbility->maxAbilitySpellLevel = maxSpellLevel;
    //};
    pSchoolAbility->maxAbilitySpellLevel = pMaxSpellLevelsByPrime[primeScore-1];
    for (spellLevel=1; spellLevel<=MAX_SPELL_LEVEL; spellLevel++)
    {
      if (pSpellLimits[spellLevel-1] >= pSchoolAbility->base[spellLevel-1])
      {
        pSchoolAbility->base[spellLevel-1] = pSpellLimits[spellLevel-1];
        if (baseclassLevel > pSchoolAbility->baseclassLevel[spellLevel-1])
        {
          pSchoolAbility->baseclassLevel[spellLevel-1] = baseclassLevel;
          // The highest baseclass level that contributed the largest 
          // number of base spells at this spell level.
        };
      };
    };
    {  // Add the bonus spells for high baseclass levels.
      int idx, num;
      num = pBaseclass->m_bonusSpells.GetSize()/3;
      for (idx =0; idx<num; idx++)
      {
        if (bonusScore >= pBaseclass->m_bonusSpells[3* idx])
        {
          int level, bonus;
          level = pBaseclass->m_bonusSpells[3* idx +2];
          if (level > pSchoolAbility->maxAbilitySpellLevel) continue;
          bonus = pBaseclass->m_bonusSpells[3* idx +1];
          pSchoolAbility->bonus[level-1] += bonus;
        };
      };
    };
  };
}
#else
void CHARACTER::UpdateSpellAbilityForBaseclass(const BASECLASS_STATS *pBaseclassStats)
{
  int m, j, effectiveBaseclassLevel;
  const BASE_CLASS_DATA *pBaseclass;
  if (!CanUseBaseclass(pBaseclassStats)) return;
  if (pBaseclassStats->currentLevel > 0)
  {
    effectiveBaseclassLevel = pBaseclassStats->currentLevel;
  }
  else
  {
    effectiveBaseclassLevel = pBaseclassStats->previousLevel;
  };
  if (     (pBaseclassStats->pBaseclassData == NULL)
       ||  (pBaseclassStats->pBaseclassData->BaseclassID() != pBaseclassStats->baseclassID))
  {
    pBaseclassStats->pBaseclassData = baseclassData.PeekBaseclass(pBaseclassStats->baseclassID);
  };
  pBaseclass = pBaseclassStats->pBaseclassData;
  if (pBaseclass == NULL) return;
  m = pBaseclass->GetCastingInfoCount();
  for (j=0; j<m; j++)
  {
    int k, spellLevel;
    int primeScore, bonusScore, maxSpells, maxSpellLevel, baseclassLevel;
    const BYTE *pMaxSpellsByPrime;
    const BYTE *pMaxSpellLevelsByPrime;
    const BYTE *pSpellLimits;
    const CASTING_INFO *pCastingInfo;
    const SCHOOL_ID *pSchoolID;
    const CString *pPrimeAbility;
    const CString *pBonusAbility;
    SCHOOL_ABILITY *pSchoolAbility;
    pCastingInfo = pBaseclass->PeekCastingInfo(j);
    pSchoolID = &pCastingInfo->schoolID;
    // Do we already have a spell ability for this school?
    k = spellAbility.LocateSchoolAbility(*pSchoolID);
    if (k < 0)
    {
      SCHOOL_ABILITY schoolAbility;
      schoolAbility.schoolID = *pSchoolID;
      k = spellAbility.GetSchoolAbilityCount();
      // We need to add a school to the spellAbility
      spellAbility.Add(schoolAbility);
    };
    pSchoolAbility             = spellAbility.GetSchoolAbility(k);
    pPrimeAbility              = &pCastingInfo->primeAbility;
    pBonusAbility              = &pBaseclass->m_spellBonusAbility;
    //baseclassLevel             = GetCurrentLevel(*pBaseclassID);
    baseclassLevel             = effectiveBaseclassLevel;
    pMaxSpellsByPrime          = pCastingInfo->m_maxSpellsByPrime;
    pMaxSpellLevelsByPrime     = pCastingInfo->m_maxSpellLevelsByPrime;
    primeScore                 = GetAbilityScore(*pPrimeAbility);
    bonusScore                 = GetAbilityScore(*pBonusAbility);
    pSpellLimits               = pCastingInfo->m_spellLimits[baseclassLevel-1];
    {
      int idx;
      maxSpellLevel            = 0;
      for (idx=0; idx<MAX_SPELL_LEVEL; idx++)
      {
        if (pSpellLimits[idx] != 0) maxSpellLevel = idx+1;
      };
    };
    maxSpells                  = pMaxSpellsByPrime[primeScore-1];
    if (maxSpells > pSchoolAbility->maxSpells)
    {
      pSchoolAbility->maxSpells = maxSpells;
    };
    // Replace 20171116 PRS
    //if (maxSpellLevel > pSchoolAbility->maxAbilitySpellLevel)
    //{  
    //  pSchoolAbility->maxAbilitySpellLevel = maxSpellLevel;
    //};
    pSchoolAbility->maxAbilitySpellLevel = pMaxSpellLevelsByPrime[primeScore-1];
//    for (spellLevel=1; spellLevel<=MAX_SPELL_LEVEL; spellLevel++)
    for (spellLevel=1; spellLevel<=maxSpellLevel; spellLevel++)
    {
      if (pSpellLimits[spellLevel-1] >= pSchoolAbility->base[spellLevel-1])
      {
        pSchoolAbility->base[spellLevel-1] = pSpellLimits[spellLevel-1];
        if (baseclassLevel > pSchoolAbility->baseclassLevel[spellLevel-1])
        {
          pSchoolAbility->baseclassLevel[spellLevel-1] = baseclassLevel;
          // The highest baseclass level that contributed the largest 
          // number of base spells at this spell level.
        };
      };
    };
    {  // Add the bonus spells for high baseclass levels.
      int idx, num;
      num = pBaseclass->m_bonusSpells.GetSize()/3;
      for (idx =0; idx<num; idx++)
      {
        if (bonusScore >= pBaseclass->m_bonusSpells[3* idx])
        {
          int level, bonus;
          level = pBaseclass->m_bonusSpells[3* idx +2];
          if (level > maxSpellLevel) continue;
          if (level > pSchoolAbility->maxAbilitySpellLevel) continue;
          bonus = pBaseclass->m_bonusSpells[3* idx +1];
          pSchoolAbility->bonus[level-1] += bonus;
        };
      };
    };
  };
};
#endif

//*****************************************************************************
// NAME:    CHARACTER::UpdateSpellAbility
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CHARACTER::UpdateSpellAbility()
{
  if (GetType() == MONSTER_TYPE) return;

/*
  // Magic=01 cleric=02 thief=04
  //WORD BaseClassMask = SetClassBitsFromCharClass(GetClass());
  const CLASS_DATA *pClassData;
  SCHOOL_LIST charAllowedSchools;
  //WriteDebugString("Spell book summary for %s, character class %s:\n", GetName(), ClassFlagToText(BaseClassMask));  

  
  // Examine the character's baseclasses to determine which spell schools he is allowed.
  WORD spellmask = 0;  // The school this character can use.
  for (i=0;i<NumBaseClass;i++)
  {
    WORD flag = (BaseClassMask & (0x0001 << i));
    if (flag) 
    {
      //SetSpellLimits(flag, GetPrimeSpellCastingScore(flag), m_spellbook.spellLimits);
      m_spellbook.spellLimits.SetSpellLimits(flag, GetPrimeSpellCastingScore(flag));
      spellmask |= GetSpellCastingSchoolForCharBaseclass(flag);
    }
  }

  // Create the charAllowedSchools array.
  // this is a list of all the spell schools the character is allowed
  pClassData = classData.PeekClass(GetClass());
  CreateAllowedSchoolList(&charAllowedSchools);
  {
    // For each possible baseclass/school we will set up a
    // SPELL_LIMITS with:
    //   * the current applicable prime score 
    //   * the maximum spell memorize count for each spell level
    int i, n;
    n = pClassData->GetCount();
    for (i=0; i<n; i++)
    {
      int j, m;
      const BASE_CLASS_DATA *pBaseclass;
      const CASTING_INFO *pCastingInfo;
      const BASECLASS_ID *pBaseclassID;
      pBaseclassID = pClassData->PeekBaseclassID(i);
      pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
      if (pBaseclass == NULL) continue;
      m = pBaseclass->GetCastingInfoCount();
      for (j=0; j<m; j++)
      {
        SCHOOL_ID schoolID;
        CString   abilityName;
        int primeScore;
        pCastingInfo = pBaseclass->PeekCastingInfo(j);
        abilityName = pCastingInfo->primeAbility;
        schoolID    = pCastingInfo->schoolID;
        primeScore  = GetAbilityScore(abilityName);
//        m_spellbook.spellLimits.SetSpellLimits(pBaseclass, primeScore, schoolID);
      };
    };
  };

  for (i=0;i<NumBaseClass;i++)
  {
    WORD flag = (BaseClassMask & (0x0001 << i));
    if (flag) 
    {
      ////int maxlevel = CurrSpellLevel(flag, GetClassLevel(flag), GetPrimeSpellCastingScore(flag), SpellClass);      
      //int maxlevel = MaxSpellLevelForClassAndSchool(flag, GetClassLevel(flag), GetPrimeSpellCastingScore(flag), SpellClass);      
      //WriteDebugString("\tCurrent level for class %s = %u\n", ClassFlagToText(flag), GetClassLevel(flag));
      //WriteDebugString("\tMax spell level as class %s = %u\n", ClassFlagToText(flag), maxlevel);
    }
  }
  for (i=0;i<NumBaseClass;i++)
  {
    WORD flag = (BaseClassMask & (0x0001 << i));
    if (flag) 
    {
      //int maxlevel = MaxSpellLevelForClaxxAndSchool(flag, GetClassLevel(flag), GetPrimeSpellCastingScore(flag), MagicUserFlag);      
      int maxlevel = CurrSpellLevel(flag, GetClassLevel(flag), GetPrimeSpellCastingScore(flag), MagicUserFlag);      
      //maxlevel =     CurrSpellLevel(flag, GetClassLevel(flag), GetPrimeSpellCastingScore(flag), ClericFlag);      
      maxlevel =     MaxSpellLevelForClassAndSchool(flag, GetClassLevel(flag), GetPrimeSpellCastingScore(flag), ClericFlag);      
    }
  }
 



//  int spellsabovelevel = 0;
//  int spellsadded = 0;
  //int spellsforclass = 0;
  //int spellsforschool = 0;
  //int spellsineligible = 0;
  int spellsnotscribeable = 0;
//  int spelladdfailed = 0;

  // cleric spells automatically get added to spell book if
  // character is eligible to use the spell
  //
  // Magic Users will automatically get spells that have
  // AutoScribeAtLevelChange=TRUE and for which the
  // character is eligible
  //
  //POSITION pos = spellData.GetHeadPosition();
  //while (pos != NULL)
  {
    int numSpell = spellData.GetCount();
    for (int i=0; i<numSpell; i++)
    {
      //const SPELL_DATA *pSpell = &spellData.PeekAtPos(pos);
      const SPELL_DATA *pSpell = spellData.PeekSpell(i);
      //GLOBAL_SPELL_ID skey = spellData.GetKeyAt(pos);
      SPELL_ID spellID = pSpell->SpellID();
      //WORD SpellSchoolMask = pSpell->schoolMask; 
      SCHOOL_ID SpellSchool = pSpell->schoolID;      



      BOOL know = (   pSpell->UsableByParty               // party can memorize/cast this one
                  && (    (SpellSchoolMask & spellmask)  // right spell casting school
                        || (CastClass & BaseClassMask))); // class allowed to cast spells in this school

      BOOL know;
      // **** Is one of the character's baseclasses listed in the spell's allowed baseclasses?
      know = pClassData->MatchSpellBaseclass(pSpell);
      // ****** Is the spell's school one of the character's allowed schools
      know = know || pClassData->MatchSpellSchool(&charAllowedSchools);
      know  = know && pSpell->UsableByParty;

      // *****
      //if ((know) && (SpellSchoolMask == MagicUserFlag) && (!pSpell->AutoScribeAtLevelChange))
      // ***** Spell is disabled only for school 'MagicUser'.  But if a user
      // wants it AutoScribed of Cleric school, then he should check the appropriate box!
      // The engine should not be making such decisions based on the spell's school.
      // If schools are to be treated differently, then we need to make an
      // additional 'school' database, look for a Special Ability, or call
      // a hook.  Right now, we assume the Autoscribe will be checked if the
      // user wants Autoscribing.

      if (!pSpell->AutoScribeAtLevelChange)
      {
        know = FALSE;
        spellsnotscribeable++;
      }
      if (know)
      {
        int i, n;
//        int spelllevel = pSpell->Level; // spellData.GetSpellLevel(skey);
        //for (i=0;i<NumBaseClass;i++)
        //{
          // BaseClassMask = character's baseclasses.
        //  WORD flag = (BaseClassMask & (0x0001 << i));
        n = pClassData->GetCount();
        for (i=0; i<n; i++)
        {
          int primeSpellCastingScore;
          int baseclassLevel;
          const BASECLASS_ID *pBaseclassID;
          const BASE_CLASS_DATA *pBaseclassData;
          const SCHOOL_ID *pSpellSchoolID;
          pSpellSchoolID = &pSpell->schoolID;
          pBaseclassID = pClassData->PeekBaseclassID(i);
          pBaseclassData = baseclassData.PeekBaseclass(*pBaseclassID);
          //
          // Check to see what ability is needed for this spell.
          // And get the value of that ability.
          // For example:  Wisdom = 12.
          primeSpellCastingScore = GetPrimeSpellCastingScore(pBaseclassData, *pSpellSchoolID);
          baseclassLevel = GetBaseclassLevel(*pBaseclassID);

          //if (flag) 
          {
//            int maxlevel;
            //int maxlevel = CurrSpellLevel(flag, GetClassLevel(flag), GetPrimeSpellCastingScore(flag), SpellSchoolMask);
//            maxlevel = MaxSpellLevelForBaseclassAndSchool(pBaseclassData, 
//                                                          baseclassLevel,
//                                                          primeSpellCastingScore,
//                                                         *pSpellSchoolID);
            Not Implemented(0xc319,false);
//            if (spelllevel <= maxlevel)
//            {     
//              //if (!m_spellbook.KnowSpell(flag, SpellSchoolMask, spellID, spelllevel, TRUE))
//              if (!m_spellbook.fiKnowSpell(*pBaseclassID, *pSpellSchoolID, spellID, spelllevel, TRUE))
//              {
//                spelladdfailed++;
//              }
//              else
//              {
//                spellsadded++;
//              }
//            }
//            else
//              spellsabovelevel++;
          }
        }
      }
      //spellData.GetNext(pos);
    }
  };

  WriteDebugString("\tSpells Avail by school = %u\n", spellsforschool);
  WriteDebugString("\tSpells Avail for class = %u\n", spellsforclass);
  WriteDebugString("\tSpells Added = %u\n", spellsadded);
  WriteDebugString("\tSpells Not Added (not usable by party) = %u\n", spellsineligible);
  WriteDebugString("\tSpells Not Added (too high level) = %u\n", spellsabovelevel);
  WriteDebugString("\tSpells Not Added (not auto given to MU) = %u\n", spellsnotscribeable);  
  WriteDebugString("\tSpells Not Added (failed call to KnowSpell) = %u\n", spelladdfailed);  
  */
//#define UpdateSpellOld
#ifdef UpdateSpellOld
  const CLASS_DATA *pClass;
  if (spellAbility.valid) return;
  spellAbility.Clear();
  spellAbility.valid = TRUE;
  {
    // Add the spell schools this character has access to.
    int i, n;
    pClass = classData.PeekClass(classID);
    if (pClass == NULL) return;
    n = pClass->GetCount();
    for (i=0; i<n; i++)
    {
      int j,m;
      int baseclassIndex;
      const BASE_CLASS_DATA *pBaseclass;
      const BASECLASS_ID *pBaseclassID;
      //const BASE_CLASS_DATA *pBaseclass;
      pBaseclassID = pClass->PeekBaseclassID(i);
      baseclassIndex = baseclassData.LocateBaseclass(*pBaseclassID);
      if (baseclassIndex < 0) continue;
      pBaseclass = baseclassData.PeekBaseclass(baseclassIndex);
      m = pBaseclass->GetCastingInfoCount();
      for (j=0; j<m; j++)
      {
        int k, spellLevel;
        int primeScore, bonusScore, maxSpells, maxSpellLevel, baseclassLevel;
        const BYTE *pMaxSpellsByPrime;
        const BYTE *pMaxSpellLevelsByPrime;
        const BYTE *pSpellLimits;
        const CASTING_INFO *pCastingInfo;
        const SCHOOL_ID *pSchoolID;
        const CString *pPrimeAbility;
        const CString *pBonusAbility;
        SCHOOL_ABILITY *pSchoolAbility;
        pCastingInfo = pBaseclass->PeekCastingInfo(j);
        pSchoolID = &pCastingInfo->schoolID;
        // Do we already have a spell ability for this school?
        k = spellAbility.LocateSchoolAbility(*pSchoolID);
        if (k < 0)
        {
          SCHOOL_ABILITY schoolAbility;
          schoolAbility.schoolID = *pSchoolID;
          k = spellAbility.GetSchoolAbilityCount();
          // We need to add a school to the spellAbility
          spellAbility.Add(schoolAbility);
        };
        pSchoolAbility             = spellAbility.GetSchoolAbility(k);
        pPrimeAbility              = &pCastingInfo->primeAbility;
        pBonusAbility              = &pBaseclass->m_spellBonusAbility;
        baseclassLevel             = GetCurrentLevel(*pBaseclassID);
        pMaxSpellsByPrime          = pCastingInfo->m_maxSpellsByPrime;
        pMaxSpellLevelsByPrime     = pCastingInfo->m_maxSpellLevelsByPrime;
        primeScore                 = GetAbilityScore(*pPrimeAbility);
        bonusScore                 = GetAbilityScore(*pBonusAbility);
        pSpellLimits               = pCastingInfo->m_spellLimits[baseclassLevel-1];
        {
          int idx;
          maxSpellLevel            = 0;
          for (idx=0; idx<MAX_SPELL_LEVEL; idx++)
          {
            if (pSpellLimits[idx] != 0) maxSpellLevel = idx+1;
          };
        };
        maxSpells                  = pMaxSpellsByPrime[primeScore-1];
        if (maxSpells > pSchoolAbility->maxSpells)
        {
          pSchoolAbility->maxSpells = maxSpells;
        };

        // Replace 20171116 PRS
        //if (maxSpellLevel > pSchoolAbility->maxAbilitySpellLevel)
        //{  
        //  pSchoolAbility->maxAbilitySpellLevel = maxSpellLevel;
        //};
        pSchoolAbility->maxAbilitySpellLevel = pMaxSpellLevelsByPrime[primeScore-1];


        for (spellLevel=1; spellLevel<=MAX_SPELL_LEVEL; spellLevel++)
        {
          if (pSpellLimits[spellLevel-1] >= pSchoolAbility->base[spellLevel-1])
          {
            pSchoolAbility->base[spellLevel-1] = pSpellLimits[spellLevel-1];
            if (baseclassLevel > pSchoolAbility->baseclassLevel[spellLevel-1])
            {
              pSchoolAbility->baseclassLevel[spellLevel-1] = baseclassLevel;
              // The highest baseclass level that contributed the largest 
              // number of base spells at this spell level.
            };
          };
        };
        {  // Add the bonus spells for high baseclass levels.
          int idx, num;
          num = pBaseclass->m_bonusSpells.GetSize()/3;
          for (idx =0; idx<num; idx++)
          {
            if (bonusScore >= pBaseclass->m_bonusSpells[3* idx])
            {
              int level, bonus;
              level = pBaseclass->m_bonusSpells[3* idx +2];
              if (level > pSchoolAbility->maxAbilitySpellLevel) continue;
              bonus = pBaseclass->m_bonusSpells[3* idx +1];
              pSchoolAbility->bonus[level-1] += bonus;
            };
          };
        };
      };
    };
  };
#else
  if (spellAbility.valid) return;
  spellAbility.Clear();
  spellAbility.valid = TRUE;
#ifdef OldDualClass20180126
  const CLASS_DATA *pClass;
  {
    // Add the spell schools this character has access to.
    int i, n;
    pClass = classData.PeekClass(classID);
    if (pClass == NULL) return;
    n = pClass->GetCount();
    for (i=0; i<n; i++)
    {
      int baseclassIndex;
      const BASE_CLASS_DATA *pBaseclass;
      const BASECLASS_ID *pBaseclassID;
      //const BASE_CLASS_DATA *pBaseclass;
      pBaseclassID = pClass->PeekBaseclassID(i);
      baseclassIndex = baseclassData.LocateBaseclass(*pBaseclassID);
      if (baseclassIndex < 0) continue;
      pBaseclass = baseclassData.PeekBaseclass(baseclassIndex);
      UpdateSpellAbilityForBaseclass(pBaseclass, GetCurrentLevel(*pBaseclassID));
    };
  };
#else
  {
    int i, n;
    n = baseclassStats.GetCount();
    for (i=0; i<n; i++)
    {
      const BASECLASS_STATS *pBaseclassStats;
      pBaseclassStats = PeekBaseclassStats(i);
      UpdateSpellAbilityForBaseclass(pBaseclassStats);
    };
  };
#endif
#endif
  {
/*
 *   // Now go through the list of spells and add all those
 *   // that can be known to the character's spellbook.
 *   int i, n;
 *   n = spellData.GetCount();
 *   for (i=0; i<n; i++)
 *   {
 *     const SPELL_DATA *pSpell;
 *     const SPELL_ABILITY *pSpellAbility;
 *     const SCHOOL_ABILITY *pSchoolAbility;
 *     pSpell = spellData.PeekSpell(i);
 *     if (pSpell->AutoScribe == FALSE) continue;
 *     if (pSpell->Level <= 0) continue;
 *     pSpellAbility = &spellAbility;
 *     pSchoolAbility = pSpellAbility->PeekSchoolAbility(pSpell->schoolID);
 *     if (pSchoolAbility == NULL) continue;
 *     if (pSpell->Level > pSchoolAbility->maxSpellLevel) continue;
 *     //m_spellbook.KnowSpell(pSpell, TRUE);
 *   };
 */
  };
}
/*
void CHARACTER::GetMaxMemorizePerSpellLevel(const SCHOOL_ID& schoolID,
                                                  int       *maxMemorizeArray
                                           ) const // [MAX_SPELL_LEVEL]
{
  const SPELL_LIMITS *pSpellLimits;
  int i;
  pSpellLimits = m_spellbook.spellLimits.PeekSpellLimits(schoolID);
  if (pSpellLimits == NULL)
  {
    for(i=1; i<MAX_SPELL_LEVEL; i++)
    {
      maxMemorizeArray[i-1] = 0;
    };
    return;
  };
  for (i=1; i<=MAX_SPELL_LEVEL; i++)
  {
    maxMemorizeArray[i-1] = pSpellLimits->maxMemorizeCount[i-1];
  };
}
*/
/*


//*****************************************************************************
//    NAME: CHARACTER::race
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::race(raceType newrace)
{
  m_race=newrace;
  // m_traits.Update(CM_race);
}
*/
//*****************************************************************************
//    NAME: CHARACTER::GetAbility
//
// PURPOSE: 
//
//*****************************************************************************
//BYTE CHARACTER::GetAbility(abilityType atype)
BYTE CHARACTER::GetAbilityScore(const CString& abilityName) const
{
/*
  if (strengthKey==atype)
    return GetAdjStr();
  else if (intelligenceKey==atype)
    return GetAdjInt();
  else if (wisdomKey==atype)
    return GetAdjWis();
  else if (dexterityKey==atype)
    return GetAdjDex();
  else if (constitutionKey==atype)
    return GetAdjCon();
  else if (charismaKey==atype)
    return GetAdjCha();
*/

  if (abilityName == Ability_Strength)
    return GetAdjStr();
  else if (abilityName == Ability_Intelligence)
    return GetAdjInt();
  else if (abilityName == Ability_Wisdom)
    return GetAdjWis();
  else if (abilityName == Ability_Dexterity)
    return GetAdjDex();
  else if (abilityName == Ability_Constitution)
    return GetAdjCon();
  else if (abilityName == Ability_Charisma)
    return GetAdjCha();
  return 0;
}

//*****************************************************************************
//    NAME: CHARACTER::GetAbilityLimits
//
// PURPOSE: 
//
//*****************************************************************************
//ABILITYLIMITS CHARACTER::GetAbilityLimits(abilityType abType) const // Min/Max allowed for character
ABILITYLIMITS CHARACTER::GetAbilityLimits(const ABILITY_ID& abilityID) const // Min/Max allowed for character
{
  //return classData.GetAbilityLimits(GetClass(), abType);
  return classData.GetAbilityLimits(GetClass(), abilityID);
}

//*****************************************************************************
//    NAME: CHARACTER::getAllowedAlignments
//
// PURPOSE: 
//
//*****************************************************************************
WORD CHARACTER::getAllowedAlignments(void) const
{
  return classData.GetAllowedAlignments(GetClass());
}


int CHAR_LIST::LocatePreSpellNamesKey(int key)
{
  int i, n;
  n = chars.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekCharacter(i)->preSpellNamesKey == key) return i;
  };
  MsgBoxInfo("Searching for a Character that was defined by its 'key'.  We lost that information "
             "at about version 0.998101 of the editor.  If this is a problem for you, please "
             "contact the DC development team for help.");
  return -1;
}
#ifdef UAFEDITOR
int CHAR_LIST::LocateOrigIndex(int idx)
{
  int i, n;
  n = chars.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekCharacter(i)->origIndex == idx) return i;
  };
  return -1;
}
#endif

/*
int CHAR_LIST::GetNextKey()
{
  int key;
  if (GetCount() > 0)
  {
    key = chars.GetTail().uniqueKey;
    if ( (key >= INT_MAX) || (key < MAX_PARTY_MEMBERS) )
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is greater than
      // MAX_CHARACTERS.
      // Gaps occur when deletions are made to the list
      POSITION p = chars.GetHeadPosition();
      BOOL found = FALSE;        
      int prevKey = MAX_PARTY_MEMBERS-1;
      int currKey;
      while ((p != NULL) && (!found))
      {
        currKey = chars.GetNext(p).uniqueKey;
        if (currKey > prevKey+1)
        {
          key = prevKey+1;
          found = TRUE;
        }
        else
          if (currKey > prevKey) prevKey = currKey; 
      }
      return prevKey+1; //There must have been som old
                        // entries less than MAX_PARTY+MEMBERS
      //VERIFY(found); // should never happen
    }
    else
      key++;
  }
  else
    key = MAX_PARTY_MEMBERS;
  return key;
}
*/

BOOL CHARACTER_ID::IsValidNPC(void) const
{
  return globalData.charData.HaveNPC(*this);
}



CHAR_LIST::CHAR_LIST() 
{ 
  Clear(); 
}

CHAR_LIST::CHAR_LIST(const CHAR_LIST& src) 
{ 
  *this = src; 
}

CHAR_LIST::~CHAR_LIST() 
{ 
  Clear(); 
}  

void CHAR_LIST::Clear() 
{ 
  //CSingleLock sLock(&m_CS, TRUE); 
  chars.RemoveAll(); 
  m_IsTempCharList=FALSE;
}
/*
int CHAR_LIST::GetCount() 
{ 
  //CSingleLock sLock(&m_CS, TRUE); 
  //return chars.GetCount(); 
  return chars.GetSize(); 
}
*/
/*
int CHAR_LIST::numChars() 
{ 
  return GetCount(); 
}
*/
/*
POSITION CHAR_LIST::GetHeadPosition() const
{ 
  return chars.GetHeadPosition(); 
}
*/
/*
CHARACTER& CHAR_LIST::GetAt(POSITION pos) 
{ 
  return chars.GetAtPos(pos); 
}
*/
/*
CHARACTER& CHAR_LIST::GetNext(POSITION &pos) 
{ 
  return chars.GetNext(pos); 
}
*/
CHAR_LIST& CHAR_LIST::operator=(const CHAR_LIST& src)
{
  int i, n;
  if (&src == this) return *this;
  Clear();
  //POSITION pos = const_cast<CHAR_LIST&>(src).chars.GetHeadPosition();
  //while (pos != NULL)
  n = src.GetCount();
  for (i=0; i<n; i++)
  {
    //AddCharacterWithCurrKey(const_cast<CHAR_LIST&>(src).GetNext(pos));
    AddCharacter(src.PeekCharacter(i));
  };
  return *this;
}

BOOL CHAR_LIST::HaveCharacter(const CHARACTER_ID& characterID) const
{
  return LocateCharacter(characterID) >= 0;
}

int CHAR_LIST::LocateCharacter(const CHARACTER_ID& characterID) const
{
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    if (characterID == PeekCharacter(i)->CharacterID())
    {
      return i;
    };
  };
  return -1;
}


void CHAR_LIST::Serialize(CArchive &ar, double version)
{
//  CObject::Serialize(ar);
  int i;    
  if (ar.IsStoring())
  {
    int n;
    ar << GetCount();
    //POSITION pos = chars.GetHeadPosition();
    //while (pos != NULL)
    n = GetCount();
    for (i=0; i<n; i++)
    {
      //chars.GetNext(pos).Serialize(ar, version);
      GetCharacter(i)->Serialize(ar, version);
      //chars[i].Serialize(ar, version);
    };
  }
  else
  {
    Clear();
    CHARACTER data;
    int temp;
    ar >> temp;
    for (i=0; i<temp; i++)
    {
      data.Clear();
      data.Serialize(ar, version);
      //AddCharacterWithCurrKey(data);
      AddCharacter(&data);
    }
  }
}

void CHAR_LIST::Serialize(CAR &ar, double version)
{
  int i;    
  if (ar.IsStoring())
  {
    int n;
    ar << GetCount();
    //POSITION pos = chars.GetHeadPosition();
    //while (pos != NULL)
    n = GetCount();
    for (i=0; i<n; i++)
    {
      GetCharacter(i)->Serialize(ar, version);
      //chars[i].Serialize(ar, version);
    };
  }
  else
  {
    Clear();
    CHARACTER data;
    int temp;
    ar >> temp;
    for (i=0; i<temp; i++)
    {
      int n;
      data.Clear();
      data.Serialize(ar, version);
      data.UpdateSpellAbility();
      //AddCharacterWithCurrKey(data);
      n = AddCharacter(&data);
    }
  }
}

#ifdef UAFEDITOR

const char *JKEY_CHARLIST = "characterList";
void CHAR_LIST::Export(void)
{
  int i, n;  
  n = GetCount();
//  jw.StartArray(JKEY_CHARLIST);
  for (i=0; i<n; i++)
  {
    CString fname;
    CFile cf;
    CHARACTER *pCharacter;
    pCharacter = GetCharacter(i);
    fname.Format("%s%s.CHAR", rte.DataDir(), pCharacter->GetName());
//    jw.NextEntry();
    if (cf.Open(fname, CFile::modeCreate|CFile::modeWrite) == 0)
    {
      MsgBoxInfo("Cannot open file %s", fname);     
      continue;
    };
    JWriter jw(&cf);
    pCharacter->Export(jw);
    //chars[i].Export(jw);
  };
  {
    CString msg;
    msg.Format("%d characters exported", n);
    MsgBoxInfo(msg);
  };
//  jw.EndArray();
}

#include <io.h>

bool CHAR_LIST::Import(BOOL quiet)
{
  int newc, oldc;
  long result, handle;
  _finddata_t fdata;
  CString f, extensions;
  newc = oldc = 0;
  f = rte.DataDir();  // For now....the \Resources folder
  f += "*.CHAR";
  for (handle=result=_findfirst(f, &fdata); result!=-1; result=_findnext(handle,&fdata))
  {
    int i, n;
    bool success;
    CString fname;
    CHARACTER newCharacter;
    CFile cf;
    fname = rte.DataDir() + fdata.name;
    if (!cf.Open(fname, CFile::modeRead)) continue;
    JReader jr(&cf);
    success = true;
    try
    {
      jr.Initialize();
      newCharacter.Import(jr);
    }
    catch (int)
    {
      MsgBoxInfo ("Error importing character");
      success = false;
    }
    if (success)
    {
      n = GetCount();
      for (i=0; i<n; i++)
      {
        if (GetCharacter(i)->GetName() == newCharacter.GetName())
        {
          chars.SetAt(i, newCharacter);
          oldc++;
          break;
        };
      };
      if (i == n) 
      {
        chars.Add(newCharacter);
        newc++;
      };
    };
  };
  if (!quiet)
  {
    CString msg;
    msg.Format("%d new and %d old characters successfully imported", newc, oldc);
    MsgBoxInfo(msg);
  };
  return true;
}


BOOL CHAR_LIST::Validate(FILE *pFile, WORD flags) const
{
  BOOL success = TRUE;
  //POSITION pos = chars.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = chars.GetSize();
  for (i=0; i<n; i++)
  {
    if (flags & VAL_ICON)
    {
      //if (chars.PeekAtPos(pos).icon.filename != "")
      if (PeekCharacter(i)->icon.filename != "")
      {
        //if (!FileExists(chars.PeekAtPos(pos).icon.filename))
        if (!FileExists(PeekCharacter(i)->icon.filename))
        {
          WriteValidateLog(pFile, "NPC %s missing Icon file %s\n",
                           //chars.PeekAtPos(pos).GetName(), chars.PeekAtPos(pos).icon.filename);
                           PeekCharacter(i)->GetName(), PeekCharacter(i)->icon.filename);
          success = FALSE;
        }
      }
    }
    if (flags & VAL_SPIC)
    {
      //if (chars.PeekAtPos(pos).smallPic.filename != "")
      if (PeekCharacter(i)->smallPic.filename != "")
      {
        //if (!FileExists(chars.PeekAtPos(pos).smallPic.filename))
        if (!FileExists(PeekCharacter(i)->smallPic.filename))
        {
          WriteValidateLog(pFile, "NPC %s missing Small Pic file %s\n",
                           //chars.PeekAtPos(pos).GetName(), chars.PeekAtPos(pos).smallPic.filename);
                           PeekCharacter(i)->GetName(), PeekCharacter(i)->smallPic.filename);
          success = FALSE;
        }
      }
    }
    //chars.PeekNext(pos);
  }
  return success;
}

//BOOL CHAR_LIST::Validate(int index, FILE *pFile, WORD flags) const
BOOL CHAR_LIST::Validate(const CHARACTER_ID& characterID, FILE *pFile, WORD flags) const
{
  BOOL success = TRUE;
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
  int index;
  index = LocateCharacter(characterID);
  if (index >= 0)
  {
    if (flags & VAL_ICON)
    {
      //if (chars.PeekAtPos(pos).icon.filename != "")
      if (PeekCharacter(index)->icon.filename != "")
      {
        //if (!FileExists(chars.PeekAtPos(pos).icon.filename))
        if (!FileExists(PeekCharacter(index)->icon.filename))
        {
          WriteValidateLog(pFile, "NPC %s missing Icon file %s\n",
                           //chars.PeekAtPos(pos).GetName(), chars.PeekAtPos(pos).icon.filename);
                           PeekCharacter(index)->GetName(), PeekCharacter(index)->icon.filename);
          success = FALSE;
        }
      }
    }
    if (flags & VAL_SPIC)
    {
      //if (chars.PeekAtPos(pos).smallPic.filename != "")
      if (PeekCharacter(index)->smallPic.filename != "")
      {
        //if (!FileExists(chars.PeekAtPos(pos).smallPic.filename))
        if (!FileExists(PeekCharacter(index)->smallPic.filename))
        {
          WriteValidateLog(pFile, "NPC %s missing Small Pic file %s\n",
                           //chars.PeekAtPos(pos).GetName(), chars.PeekAtPos(pos).smallPic.filename);
                           PeekCharacter(index)->GetName(), PeekCharacter(index)->smallPic.filename);
          success = FALSE;
        }
      }
    }
  }
  else
  {
    WriteValidateLog(pFile, "Bogus NPC key %u\n", index);
    success = FALSE;
  }
  return success;
}


void CHAR_LIST::CrossReference(CR_LIST *pCRList) const
{
  //POSITION pos = chars.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //chars.PeekAtPos(pos).CrossReference(pCRList);
    PeekCharacter(i)->CrossReference(pCRList);
    //chars.PeekNext(pos);
  };
}



#endif
/*
BOOL CHAR_LIST::HaveKey(int index)
{ 
  return (chars.FindKeyPos(index) != NULL); 
}
*/
BOOL CHAR_LIST::HaveNPC(const CHARACTER_ID& characterID) const
{
  int i;
  i = LocateCharacter(characterID);
  if (i < 0) return FALSE;
  return PeekCharacter(i)->GetType() == NPC_TYPE;
}

// CHAR_LIST is used for global NPCs,
// and also for temp characters during
// char generation and party building.
//
// called when loading from scripts, or
// when adding from editor GUI
//int CHAR_LIST::AddCharacter(CHARACTER &data)
int CHAR_LIST::AddCharacter(const CHARACTER *pChar)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //if (chars.GetCount() < MAX_CHARACTERS)
  if (GetCount() < MAX_CHARACTERS)
  {
    //data.uniqueKey = GetNextKey();
    //data.SetType(NPC_TYPE);
    //data.SetPartyMember(FALSE);
    
    //if (!IsTempCharList()) // don't update origIndex for temp char list
    //  data.origIndex = data.uniqueKey;

    //chars.Insert(data, data.uniqueKey);

    chars.Add(*(const_cast<CHARACTER *>(pChar)));
    //return data.uniqueKey;
    GetCharacter(GetCount()-1)->SetPartyMember(FALSE);
    //chars[GetCount()-1].SetPartyMember(FALSE);
    return GetCount()-1;
  }
  else
    return -1;
}
/*
// called when loading binary monster data that
// was saved with key info
int CHAR_LIST::AddCharacterWithCurrKey(CHARACTER &data)
{
  CSingleLock sLock(&m_CS, TRUE);
  if (chars.GetCount() < MAX_CHARACTERS)
  {
    data.SetPartyMember(FALSE);

    if (!IsTempCharList()) // don't update origIndex for temp char list
      data.origIndex = data.uniqueKey;

    chars.Insert(data, data.uniqueKey);
    return data.uniqueKey;
  }
  else
    return 0;
}
*/
//CString CHAR_LIST::GetCharacterName(int index) const
CString CHAR_LIST::GetCharacterName(const CHARACTER_ID& characterID) const
{
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
  //if ((pos = LocateCharacter(npcID)) != NULL)
    //return chars.PeekAtPos(pos).GetName();
  //  return PeekCharacter(pos)->GetName();
  //else
  //  return "Unknown";
  return characterID.UniqueName();
}
/*
//CHARACTER *CHAR_LIST::GetCharacterData(int arrayIndex)
CHARACTER *CHAR_LIST::GetCharacter(int arrayIndex)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
  //  return (&chars.GetAtPos(pos));
  return &chars[arrayIndex];
  //else
  //  return NULL;
}
*/
//BOOL CHAR_LIST::SetCharacter(int index, CHARACTER &data)
BOOL CHAR_LIST::SetCharacter(int arrayIndex, const CHARACTER *pChar)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
  //{
  //  chars.GetAtPos(pos) = data;
  //  return TRUE;
  //}
  *GetCharacter(arrayIndex) = *pChar;
  //chars[arrayIndex] = *pChar;
  return TRUE;
  //else
  //  return FALSE;
}

//BOOL CHAR_LIST::DeleteCharacterEvents(int index)
BOOL CHAR_LIST::DeleteCharacterEvents(int arrayIndex)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
/*
  {
    globalData.eventData.DeleteEvents(chars.PeekAtPos(pos).TalkEvent);
    chars.GetAtPos(pos).TalkLabel="TALK";
    globalData.eventData.DeleteEvents(chars.PeekAtPos(pos).ExamineEvent);
    chars.GetAtPos(pos).ExamineLabel="EXAMINE";
    return TRUE;
  }
*/
  globalData.eventData.DeleteEvents(GetCharacter(arrayIndex)->TalkEvent);
  //globalData.eventData.DeleteEvents(chars[arrayIndex].TalkEvent);
  GetCharacter(arrayIndex)->TalkLabel="TALK";
  //chars[arrayIndex].TalkLabel="TALK";
  globalData.eventData.DeleteEvents(GetCharacter(arrayIndex)->ExamineEvent);
  //globalData.eventData.DeleteEvents(chars[arrayIndex].ExamineEvent);
  GetCharacter(arrayIndex)->ExamineLabel="EXAMINE";
  //chars[arrayIndex].ExamineLabel="EXAMINE";
  return TRUE;
  //else
  //  return FALSE;
}

void CHAR_LIST::RemoveCharacter(int arrayIndex)
{
  chars.RemoveAt(arrayIndex);
}


//BOOL CHAR_LIST::RemoveCharacter(int index)
BOOL CHAR_LIST::RemoveCharacter(const CHARACTER_ID& characterID)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
  int arrayIndex;
  if ((arrayIndex = LocateCharacter(characterID)) >= 0)
  {
    //globalData.eventData.DeleteEvents(chars.GetAt(pos).TalkEvent);
    //chars.RemoveAt(pos);
    RemoveCharacter(arrayIndex);
    return TRUE;
  }
  else
    return FALSE;
}

BOOL CHAR_LIST::FetchCharacter(int arrayIndex, CHARACTER *pChar) const
{
  const CHARACTER *p;
  p = PeekCharacter(arrayIndex);
  if (p == NULL) return FALSE;
  *pChar = *p;
  return TRUE;
}

//BOOL CHAR_LIST::FetchCharacterData(int index, CHARACTER &data)
BOOL CHAR_LIST::FetchCharacter(const CHARACTER_ID& characterID, CHARACTER *pChar) const
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
  int arrayIndex;
  if ((arrayIndex = LocateCharacter(characterID)) >= 0)
  {
    //data = chars.GetAtPos(pos);
    //*pChar = chars.GetAtPos(pos);
    FetchCharacter(arrayIndex, pChar);
    return TRUE;
  }
  else
    return FALSE;
}  
/*
// No locking... for internal use
int CHAR_LIST::m_FindNamei (const CString& name)
// Return key associated with the named object, or zero
{
  POSITION pos;
  pos=chars.GetHeadPosition(); 
  while (pos!=NULL)
  {
    if (chars.PeekAtPos(pos).GetName().CompareNoCase(name)) 
          return chars.PeekAtPos(pos).uniqueKey;
    chars.GetNext(pos);
  };
  return 0;
}
// With locking...for public use
int CHAR_LIST::FindNamei(const CString& name)
// Return key associated with the named object, or zero
{
  CSingleLock sLock(&m_CS,TRUE);
  return m_FindNamei(name);
}
*/
#ifdef UAFEngine
//void CHAR_LIST::CastSpell(int index, GLOBAL_SPELL_ID spell)
void CHAR_LIST::CastSpell(int arrayIndex, const SPELL_ID& spellID)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
    //chars.GetAtPos(pos).CastSpell(spellName, false);
  GetCharacter(arrayIndex)->CastSpell(spellID, false, false);
  //chars[arrayIndex].CastSpell(spellID, false, false);
}

//void CHAR_LIST::SpellActivate(int index, const PENDING_SPELL &data)
void CHAR_LIST::SpellActivate(int arrayIndex, 
                              const PENDING_SPELL &data,
                              ToHitComputation *pToHitComputation)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //POSITION pos;
  //if ((pos = chars.FindKeyPos(index)) != NULL)
  GetCharacter(arrayIndex)->SpellActivate(data, NULL, false, pToHitComputation);
  //chars[arrayIndex].SpellActivate(data, NULL, false);
}
#endif

//*****************************************************************************
// NAME: CHARACTER::getInfo
//
// PURPOSE: Provide string with lots of information about this character
//*****************************************************************************
CString CHARACTER::getInfo(void)
{
  CString result;
  result.Format("name=%s;"
                "pos=%d;"
                "THAC0=%d;"
                "AC=%d;"
                "age=%d;"
                "maxage=%d;"
                "HP=%d;"
                "maxHP=%d;"
                "enc=%d;"
                "maxenc=%d;"
                "maxmove=%d;"
                "STR=%d;"
                "INT=%d;"
                "WIS=%d;"
                "DEX=%d;"
                "CON=%d;"
                "CHA=%d;"
                "race=%s;"
                "gender=%s;"
                "class=%s;"
                "align=%s;",

                GetName(),
                uniquePartyID,
                GetAdjTHAC0(),
                GetAdjAC(),
                GetAdjAge(),
                GetAdjMaxAge(),
                GetAdjHitPoints(),
                GetMaxHitPoints(),
                GetEncumbrance(),
                maxEncumbrance,
                GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, "Retrieve character stats - max movement"),
                GetAdjStr(),
                GetAdjInt(),
                GetAdjWis(),
                GetAdjDex(),
                GetAdjCon(),
                GetAdjCha(),
                //raceData.GetRaceName(m_race),
                raceData.GetRaceName(race),
                GetAdjGender()==Male ? "male" : "female",
                //classData[GetClass()].m_name,
                classData.PeekClass(GetClass())->m_name,
                GetAlignmentName(GetAlignment()));
  return result;
}
/*
int CHARACTER::GetUniqueId()
{ 
#ifdef UAFEngine
  return (m_pCombatant==NULL)?uniquePartyID:m_pCombatant->GetUniqueId(); 
#else
  return uniquePartyID;
#endif
}
*/

//*****************************************************************************
//    NAME: CHARACTER::getItemList
//
// PURPOSE: 
//
//*****************************************************************************
CString CHARACTER::getItemList(int id)
{ //id is an identification number that the caller would like
  //associated with each of the items in this character's possession
  CString result;
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;
  CString temp;
  POSITION pos = myItems.GetHeadPosition();
	while (pos != NULL)
  {
    //giID = myItems.PeekAtPos(pos).m_giID;
    itemID = myItems.PeekAtPos(pos).itemID;
    //temp.Format( "{%s[%d]}", itemData.GetItemUniqueName(giID), id);
    temp.Format( "{%s[%d]}", itemData.GetItemUniqueName(itemID), id);
    result += temp;
    myItems.GetNext(pos);
  }
  return result;
}
//*****************************************************************************
// NAME: CHARACTER::ReadyBestWpn
//
// PURPOSE: Select best weapon for auto dudes
//*****************************************************************************
void CHARACTER::ReadyBestWpn(int dist, BOOL isLargeTarget)
{
  ReadyWeaponScript(NO_READY_ITEM);

  // need to check for usable items
  // need to check for targets requiring magical weapons

  int dmgIdx = NO_READY_ITEM;
  int dmg = 0;
  int dmgSum = 0;
  int defIdx = NO_READY_ITEM;
  int def = 0;
  int attIdx = NO_READY_ITEM;
  int att = 0;
  //int minDist = 0;
  ITEM_DATA *data;

  // determine which avail item has best damage, 
  // attack, and protection values for given distance
  // to target
  //
  POSITION pos = myItems.GetHeadPosition();
  while (pos != NULL)
  {
    //if ((data = itemData.GetItemData(myItems.PeekAtPos(pos).m_giID)) != NULL)
    if ((data = itemData.GetItem(myItems.PeekAtPos(pos).itemID)) != NULL)
    {

      
      
      if (data->Location_Readied == WeaponHand)
      {
        miscErrorType err;
          //&& (myItems.CanReady(myItems.GetKeyAt(pos), GetItemMask(), this))
        //err = myItems.CanReady(myItems.GetKeyAt(pos), this);
        err = myItems.CanReadyItem(&myItems.PeekAtPos(pos), this);
        if (err == NoError)
        {
          //&& (WpnCanAttackAtRange(myItems.PeekAtPos(pos).m_giID, dist)))
          //&& (WpnCanAttackAtRange(myItems.PeekAtPos(pos).itemID, dist)))
          if (WpnCanAttackAtRange(myItems.PeekAtPos(pos).itemID, dist))
          {
            if (data->Protection_Base + data->Protection_Bonus <= def)
            {
              def = data->Protection_Base + data->Protection_Bonus;
              defIdx = myItems.GetKeyAt(pos);
            }
        
            if (isLargeTarget)
              dmgSum = (data->Dmg_Dice_Lg * data->Nbr_Dice_Lg) + data->Dmg_Bonus_Lg;
            else
              dmgSum = (data->Dmg_Dice_Sm * data->Nbr_Dice_Sm) + data->Dmg_Bonus_Sm;
  
            // check for fighter multiple attacks (swords/daggers only?)
 
            if (data->ROF_Per_Round > 0)
              dmgSum *= data->ROF_Per_Round;
      
            if (dmgSum >= dmg)
            {
              dmg = dmgSum;
              dmgIdx = myItems.GetKeyAt(pos);
            }
      
            if (data->Attack_Bonus >= att)
            {
              att = data->Attack_Bonus;
              attIdx = myItems.GetKeyAt(pos);
            };
          };
        }
        else
        {
          SetMiscError(err);
        };
      };
    };
    myItems.GetNext(pos);
  };
  // clear miscError that may have been set by CanReady()
  SetMiscError(NoError);

  // we now have best attack, protection, and damage items, if any,
  // for given distance to target

  // total damage potential per round
  int wpn_sum = dmgSum; // already includes rate of fire for weapon
  DetermineUnarmedHitDice();

  // ignore potential for natural attacks if
  // too far away to use them (distance to targ > 1)
  //
  if (dist <= 1)
  {
    int nonwpn_sum = 0;
    if (!isLargeTarget)
	    nonwpn_sum = (unarmedDieS * unarmedNbrDieS) + unarmedBonus;
    else
      nonwpn_sum = (unarmedDieL * unarmedNbrDieL) + unarmedBonus;

    nonwpn_sum *= GetNbrAttacks();

    // better off using natural attacks
    if (wpn_sum < nonwpn_sum) return;
  }

  // for now, choose item with most damage first
  int IdxToUse = dmgIdx;
  if (IdxToUse==NO_READY_ITEM)
    IdxToUse = attIdx;
  if (IdxToUse==NO_READY_ITEM)
    IdxToUse = defIdx;
  if (IdxToUse==NO_READY_ITEM)
    return;
 
  data = itemData.GetItem(myItems.GetItem(IdxToUse));
  if (data != NULL)
  {
    if (data->Hands_to_Use > 1)
      //myItems.UnReady(myItems.GetReady(ShieldHand));    
      myItems.UnReady(myItems.GetReadiedItem(ShieldHand, 0));    
  }

  ReadyWeaponScript(IdxToUse);
  if (WpnUsesAmmo(myItems.GetItem(IdxToUse)))
    ReadyBestAmmo(isLargeTarget);
}

#ifdef UAFEngine
void CHARACTER::ListShields(COMBAT_SUMMARY_COMBATANT *pcsc)
{
  ITEM_DATA *data;
  POSITION pos = myItems.GetHeadPosition();
  while (pos != NULL)
  {
    if ((data = itemData.GetItem(myItems.PeekAtPos(pos).itemID)) != NULL)
    {
      if (data->Location_Readied == ShieldHand)
      {
        miscErrorType err;
        //err = myItems.CanReady(myItems.GetKeyAt(pos), this);
        err = myItems.CanReadyItem(&myItems.PeekAtPos(pos), this);
        if (err == NoError)
        {
          SHIELD_SUMMARY shieldSummary;
          shieldSummary.protection = data->Protection_Base + data->Protection_Bonus;
          shieldSummary.index = myItems.GetKeyAt(pos);
          pcsc->shieldSummary.Add(shieldSummary);
        }
        else
        {
          //SetMiscError(err);
        };
      }
    };
    myItems.GetNext(pos);
  }
}
#endif
//*****************************************************************************
// NAME: CHARACTER::ReadyBestShield
//
// PURPOSE:  
//*****************************************************************************
void CHARACTER::ReadyBestShield()
{
  // if 2-handed weapon being used, can't ready a shield
  //if (   (myItems.GetReady(ShieldHand) != NO_READY_ITEM)
  //    && (myItems.GetReady(ShieldHand) == myItems.GetReady(WeaponHand)))
  if (   (myItems.GetReadiedItem(ShieldHand, 0) != NO_READY_ITEM)
      && (myItems.GetReadiedItem(ShieldHand, 0) == myItems.GetReadiedItem(WeaponHand, 0)))
    return;

  ReadyShieldScript(NO_READY_ITEM);

  int defIdx = NO_READY_ITEM;
  int def = 0;
  ITEM_DATA *data;

  // determine which avail item has best
  // protection value
  //
  POSITION pos = myItems.GetHeadPosition();
  while (pos != NULL)
  {
    //if ((data = itemData.GetItem(myItems.PeekAtPos(pos).m_giID)) != NULL)
    if ((data = itemData.GetItem(myItems.PeekAtPos(pos).itemID)) != NULL)
    {
      if (data->Location_Readied == ShieldHand)
      {
        miscErrorType err;
          //&& (myItems.CanReady(myItems.GetKeyAt(pos), GetItemMask(), this)))
        //err = myItems.CanReady(myItems.GetKeyAt(pos), this);
        err = myItems.CanReadyItem(&myItems.PeekAtPos(pos), this);
        if (err == NoError)
        {
          if (data->Protection_Base + data->Protection_Bonus < def)
          {
            def = data->Protection_Base + data->Protection_Bonus;
            defIdx = myItems.GetKeyAt(pos);
          }        
        }
        else
        {
          SetMiscError(err);
        };
      }
    };
    myItems.GetNext(pos);
  }

  // clear miscError that may have been set by CanReady()
  SetMiscError(NoError);

  // else try for best defensive bonus item
  if (defIdx != NO_READY_ITEM)
  {
    ReadyShieldScript(defIdx);
    return;
  }
}


//*****************************************************************************
// NAME: CHARACTER::ReadyBestArmor
//
// PURPOSE:  
//*****************************************************************************
void CHARACTER::ReadyBestArmor()
{
  ReadyArmorScript(NO_READY_ITEM);  //Discard current Armor

  int defIdx = NO_READY_ITEM;
  int def = 0;
  ITEM_DATA *data;

  // determine which avail item has best
  // protection value
  //
  POSITION pos = myItems.GetHeadPosition();
  while (pos != NULL)
  {
    //if ((data = itemData.GetItem(myItems.PeekAtPos(pos).m_giID)) != NULL)
    if ((data = itemData.GetItem(myItems.PeekAtPos(pos).itemID)) != NULL)
    {

      
     
      //if (   (data->Location_Readied == BodyArmor)
      if (data->Location_Readied == BodyArmor)
      {
        miscErrorType err;
          //&& (myItems.CanReady(myItems.GetKeyAt(pos), GetItemMask(), this)))
        //err = myItems.CanReady(myItems.GetKeyAt(pos), this);
        err = myItems.CanReadyItem(&myItems.PeekAtPos(pos), this);
        if (err == NoError)
        {
          if (data->Protection_Base + data->Protection_Bonus < def)
          {
            def = data->Protection_Base + data->Protection_Bonus;
            defIdx = myItems.GetKeyAt(pos);
          }        
        }
        else
        {
          SetMiscError(err);
        };
      };
    };
    myItems.GetNext(pos);
  }

  // clear miscError that may have been set by CanReady()
  SetMiscError(NoError);

  // else try for best defensive bonus item
  if (defIdx != NO_READY_ITEM)
  {
    ReadyArmorScript(defIdx);
    return;
  }
}

//*****************************************************************************
// NAME: CHARACTER::ReadyBestAmmo
//
// PURPOSE:  
//*****************************************************************************
void CHARACTER::ReadyBestAmmo(BOOL isLargeTarget)
{
  ReadyAmmoScript(NO_READY_ITEM);

  // need to check for targets requiring magical weapons

  int dmgIdx = NO_READY_ITEM;
  int dmg = 0;
  int dmgSum = 0;
  int defIdx = NO_READY_ITEM;
  int def = 0;
  int attIdx = NO_READY_ITEM;
  int att = 0;
  ITEM_DATA *data=NULL;

  POSITION pos = myItems.GetHeadPosition();
  while (pos != NULL)
  {

    
    miscErrorType err;    
    //if (   (myItems.CanReady(myItems.GetKeyAt(pos), GetItemMask(), this))
    //err = myItems.CanReady(myItems.GetKeyAt(pos), this);
    err = myItems.CanReadyItem(&myItems.PeekAtPos(pos), this);
        //&& ((data = itemData.GetItem(myItems.PeekAtPos(pos).m_giID)) != NULL))
    if ((err==NoError) && ((data = itemData.GetItem(myItems.PeekAtPos(pos).itemID)) != NULL))



    {
      if ((data->Location_Readied==AmmoQuiver)&&(data->Wpn_Type==Ammo))
      {
        if (data->Protection_Base + data->Protection_Bonus <= def)
        {
          def = data->Protection_Base + data->Protection_Bonus;
          defIdx = myItems.GetKeyAt(pos);
        }
    
        if (isLargeTarget)
          dmgSum = (data->Dmg_Dice_Lg * data->Nbr_Dice_Lg) + data->Dmg_Bonus_Lg;
        else
          dmgSum = (data->Dmg_Dice_Sm * data->Nbr_Dice_Sm) + data->Dmg_Bonus_Sm;

        // check for fighter multiple attacks (swords/daggers only?)
        dmgSum *= data->ROF_Per_Round;

        if (dmgSum >= dmg)
        {
          dmg = dmgSum;
          dmgIdx = myItems.GetKeyAt(pos);
        }
    
        if (data->Attack_Bonus >= att)
        {
          att = data->Attack_Bonus;
          attIdx = myItems.GetKeyAt(pos);
        }
      }
    }
    myItems.GetNext(pos);
  }

  // clear miscError that may have been set by CanReady()
  SetMiscError(NoError);

  // for now, choose item with most damage
  if (dmgIdx != NO_READY_ITEM)
  {
    ReadyAmmoScript(dmgIdx);
    return;
  }

  // else try for best attack bonus item
  if (attIdx != NO_READY_ITEM)
  {
    ReadyAmmoScript(attIdx);
    return;
  }

  // else try for best defensive bonus item
  if (defIdx != NO_READY_ITEM)
  {
    ReadyAmmoScript(defIdx);
    return;
  }
}

//*****************************************************************************
//    NAME: CHARACTER::UnreadyItemByType
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::UnreadyItemByLocation(DWORD rdyLoc, bool specAbsOK)
{
  //int myitemidx = myItems.GetReady(loctype);
  int myitemidx = myItems.GetReadiedItem(rdyLoc, 0);
#ifdef UAFEngine
  ITEM_DATA *pData = itemData.GetItem(myItems.GetItem(myitemidx));
  if (pData!=NULL) 
  {
    if (!specAbsOK)
    {
      pData->specAbs.DisableAllFor(this);
    };
  };
#endif
  myItems.UnReady(myitemidx); 
}

//*****************************************************************************
//    NAME: CHARACTER::DisplayCurrSpellEffects
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::DisplayCurrSpellEffects(const CString &SrcFunc)
{
#ifdef DUMP_CHAR_SPELL_ATTS

  if (spellEffects.GetCount() == 0)
    return;
  
  CString src;
  
  if (GetName().IsEmpty())
    return; // sometimes gets called for empty CHARACTER slots in PARTY struct

  Sleep(10);
  
  WriteDebugString("*** BEGIN SPELL_EFFECT records for \'%s\' from %s\n", GetName(),SrcFunc);

  POSITION pos = spellEffects.GetHeadPosition();
  while (pos != NULL)
  {
    SPELL_EFFECTS_DATA &data = spellEffects.GetNext(pos);

    if (data.flags & SPELL_EFFECTS_DATA::EFFECT_SPELL)
      src = "SPELL";
    else if (data.flags & SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB)
      src = "CHARSPECAB";
    else if (data.flags & SPELL_EFFECTS_DATA::EFFECT_SPELLSPECAB)
      src = "SPELLSPECAB";
    else if (data.flags & SPELL_EFFECTS_DATA::EFFECT_ITEMSPECAB)
      src = "ITEMSPECAB";
    else
      src = "!!INVALID!!";


    WriteDebugString("   Attrib \'%s\', Parent %i, Src %s, Flags %u\n", 
            data.affectedAttr, data.parent, src, data.flags);
  }

  WriteDebugString("*** END SPELL_EFFECT records for \'%s\'\n", GetName());
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::ReadyItemByType
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ReadyItemByLocation(DWORD rdyLoc, int index, bool specAbsOK)
{
  if (index == NO_READY_ITEM)
  {
    // unready all items of this type
    UnreadyItemByLocation(rdyLoc, specAbsOK);
    return TRUE;
  }

  //if (myItems.CanReady(index, this) != NoError) return FALSE;
  if (myItems.CanReadyKey(index, this) != NoError) return FALSE;
  //Not Implemented(0x4f9044, false);
  // We see if the character has any of the item's Allowed Baseclasses.
#ifdef UAFEngine
  myItems.Ready(index, this, rdyLoc); 
  if (myItems.IsReady(index))
  {
    ITEM_DATA *pData = itemData.GetItem(myItems.GetItem(index));
    if (pData!=NULL) 
    {
      int PreSpecAbCount = m_spellEffects.GetCount();
      if (!specAbsOK)
      {
        pData->specAbs.EnableAllFor(this);
      };

      int PostSpecAbCount = m_spellEffects.GetCount();
      int Diff = PostSpecAbCount - PreSpecAbCount;
      if (Diff > 0)
      {
        //GLOBAL_SPELL_ID gsID;
        SPELL_ID spellID;
        POSITION pos = m_spellEffects.GetTailPosition();  
        while ((pos != NULL) && (Diff > 0))
        {        
          CString SpecAbText = m_spellEffects.PeekAt(pos)->AffectedAttr();
          m_spellEffects.GetAt(pos)->flags |= SPELL_EFFECTS_DATA::EFFECT_ITEMSPECAB;
          //m_spellEffects.GetAt(pos).SourceOfEffect(pData->m_giID, true);
          m_spellEffects.GetAt(pos)->SourceOfEffect(pData->ItemID(), true);
          m_spellEffects.GetPrev(pos);
          Diff--;
          specialAbilitiesType sa;
          sa = ConvertRuntimeIfTextToSpecAb(SpecAbText);
          //gsID.IllegalConversion(pData->m_giID.UniqueIdentifier());
          spellID = pData->SpellID();
          QueueUsedSpecAb(
                 sa, 
                 (WORD)SPELL_EFFECTS_DATA::EFFECT_ITEMSPECAB,
                 //gsID);
                 spellID);
        }

        CString msg = FormatSpecAbMsg(SPECAB_MSG_READY);
        while (msg.GetLength()>0)
        {
          FormatPausedText(pausedTextData,msg);
          DisplayPausedText(pausedTextData, whiteColor, 0);
          msg = FormatSpecAbMsg(SPECAB_MSG_READY);
        }
      }
      DisplayCurrSpellEffects("ReadyItemByType");
    }
    return TRUE;
  }
#endif
  return FALSE;
}


int CHARACTER::IsIdentified(int key, int num)
{
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;
  ITEM *pITEM, *pNextITEM;
  ITEM_DATA *pItem;
  BOOL newVal;
  int count;
  POSITION pos;
  pITEM = myItems.GetITEM(key);
  if (pITEM == NULL) return -1;
  //giID = pITEM->m_giID;
  itemID = pITEM->itemID;
  //if (!giID.IsValidItem()) return -1;
  //pItem = itemData.GetItemData(giID);
  pItem = itemData.GetItem(itemID);
  if (pItem == NULL) return -1;
  if (num == 0)
  {
    return (pITEM->identified)?1:0;
  };
  if (num > 0) { newVal = TRUE; }
  else          { newVal = FALSE; num = -num; };
  count = 0;
  if (pITEM->identified != newVal)
  {
    pITEM->identified = newVal;
    count++;
  };
  num--;
  pos = myItems.GetHeadPosition();
  while ( (pos != NULL) && (num > 0) )
  {
    pNextITEM = &myItems.GetAtPos(pos);
    if (pNextITEM != pITEM)
    {
      //if (pNextITEM->m_giID == giID)
      if (pNextITEM->itemID == itemID)
      {
        if (pNextITEM->identified != newVal)
        {
          pNextITEM->identified = newVal;
          count++;
          num--;
        };
      };
    };
    myItems.GetNext(pos);
  };
  return count;
}

bool CHARACTER::CanReady(int index)
{
  if (index != NO_READY_ITEM)
  {
    {
      ActorType actor;
      ITEM_DATA *pItem;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      CString answer;
      GetContext(&actor);
      SetCharContext(&actor);
      pItem = itemData.GetItem(myItems.GetItem(index));
      scriptContext.SetCharacterContext(this);
      scriptContext.SetItemContext(pItem);
      scriptContext.SetItemContextKey(index);
      pItem->RunItemScripts(CAN_READY, 
                            ScriptCallback_RunAllScripts, 
                            NULL, 
                            "Test if character can ready item");
      answer = RunCharacterScripts(CAN_READY, 
                                   ScriptCallback_RunAllScripts, 
                                   NULL,
                                   "Test if character can ready item");
      ClearCharContext();
      if ((answer.GetLength()>0) && (answer.GetAt(0) == 'N'))
      {
        return false;
      };
    };
  };
  return false;
}


void CHARACTER::ReadyXXXScript(DWORD rdyLoc, char *scriptName, int index)
{
  if (ReadyItemByLocation(rdyLoc, index, true))
  {
    if (index != NO_READY_ITEM)
    {
      ActorType actor;
      ITEM_DATA *pItem;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      GetContext(&actor);
      SetCharContext(&actor);
      pItem = itemData.GetItem(myItems.GetItem(index));
      scriptContext.SetCharacterContext(this);
      scriptContext.SetItemContext(pItem);
      scriptContext.SetItemContextKey(index);
      pItem->RunItemScripts(scriptName,
                            ScriptCallback_RunAllScripts, 
                            NULL,
                            "Character just readied an item");
      //specAbs.RunScripts(scriptName, ScriptCallback_RunAllScripts, NULL, name, "");
      ClearCharContext();
    };
  };
}

void CHARACTER::UnReadyXXXScript(char *scriptName, int index)
{
  ActorType actor;
  ITEM_DATA *pItem;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  GetContext(&actor);
  SetCharContext(&actor);
  pItem = itemData.GetItem(myItems.GetItem(index));
  scriptContext.SetCharacterContext(this);
  scriptContext.SetItemContext(pItem);
  scriptContext.SetItemContextKey(index);
  pItem->RunItemScripts(scriptName, 
                        ScriptCallback_RunAllScripts, 
                        NULL, 
                        "Character just un-readied an item");
  //specAbs.RunScripts(scriptName, ScriptCallback_RunAllScripts, NULL, name,"");
  ClearCharContext();
}



//*****************************************************************************
//    NAME: CHARACTER::ReadyWeapon
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyWeaponScript(int index) 
{ 
//  ReadyXXX(WeaponHand, ON_READY_WEAPON, index);
  ReadyXXXScript(WeaponHand, ON_READY, index);
}
void CHARACTER::UnReadyWeaponScript(int index) 
{ 
//  UnReadyXXX(ON_UNREADY_WEAPON, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyShield
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyShieldScript(int index) 
{ 
//  ReadyXXX(ShieldHand, ON_READY_SHIELD, index);
  ReadyXXXScript(ShieldHand, ON_READY, index);
}
void CHARACTER::UnReadyShieldScript(int index) 
{ 
//  UnReadyXXX(ON_UNREADY_SHIELD, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyArmor
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyArmorScript(int index) 
{ 
//  ReadyXXX(BodyArmor, ON_READY_ARMOR, index);
  ReadyXXXScript(BodyArmor, ON_READY, index);
}
void CHARACTER::UnReadyArmorScript(int index) 
{ 
  //ReadyXXX(ON_UNREADY_ARMOR, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyGauntlets
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyGauntletsScript(int index) 
{ 
//ReadyXXX(Hands, ON_READY_GAUNTLETS, index);
  ReadyXXXScript(Hands, ON_READY, index);
}
void CHARACTER::UnReadyGauntletsScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_GAUNTLETS, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyHelm
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyHelmScript(int index) 
{ 
//ReadyXXX(Head, ON_READY_HELM, index);
  ReadyXXXScript(Head, ON_READY, index);
}
void CHARACTER::UnReadyHelmScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_HELM, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyBelt
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyBeltScript(int index) 
{ 
//ReadyXXX(Waist, ON_READY_BELT, index);
  ReadyXXXScript(Waist, ON_READY, index);
}
void CHARACTER::UnReadyBeltScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_BELT, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyRobe
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyRobeScript(int index) 
{ 
//ReadyXXX(BodyRobe, ON_READY_ROBE, index);
  ReadyXXXScript(BodyRobe, ON_READY, index);
}
void CHARACTER::UnReadyRobeScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_ROBE, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyCloak
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyCloakScript(int index) 
{ 
//ReadyXXX(Back, ON_READY_CLOAK, index);
  ReadyXXXScript(Back, ON_READY, index);
}
void CHARACTER::UnReadyCloakScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_CLOAK, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyBoots
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyBootsScript(int index) 
{ 
//ReadyXXX(Feet, ON_READY_BOOTS, index);
  ReadyXXXScript(Feet, ON_READY, index);
}
void CHARACTER::UnReadyBootsScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_BOOTS, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyRing
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyRingScript(int index) 
{ 
//ReadyXXX(Fingers, ON_READY_RING, index);
  ReadyXXXScript(Fingers, ON_READY, index);
}
void CHARACTER::UnReadyRingScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_RING, index);
  UnReadyXXXScript(ON_UNREADY, index);
}
//*****************************************************************************
//    NAME: CHARACTER::ReadyAmmo
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ReadyAmmoScript(int index) 
{ 
//ReadyXXX(AmmoQuiver, ON_READY_AMMO, index);
  ReadyXXXScript(AmmoQuiver, ON_READY, index);
}
void CHARACTER::UnReadyAmmoScript(int index) 
{ 
//UnReadyXXX(ON_UNREADY_AMMO, index);
  UnReadyXXXScript(ON_UNREADY, index);
}

//*****************************************************************************
//    NAME: CHARACTER::determineNbrAttacks
//
// PURPOSE: 
//
//*****************************************************************************
float CHARACTER::determineNbrAttacks()
{
#ifdef UAFEngine
  //DWORD src;
  //GLOBAL_SPELL_ID skey;
  CString spellName;
  ActorType actor;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;

  int minmax[2] = {-1,9999999};
  GetContext(&actor);
  SetCharContext(&actor);

  COMBATANT *pCombatant;
  pCombatant = this->m_pCombatant;
  if (pCombatant != NULL)
  {
    ITEM_DATA *pWeapon;
    int wpn;
    //GLOBAL_ITEM_ID wpnID;
    ITEM_ID weaponID;
    //wpn = pCombatant->myItems.GetReady(WeaponHand);
    wpn = myItems.GetReadiedItem(WeaponHand, 0);
    if (wpn != NO_READY_ITEM)
    {
      //wpnID = myItems.GetItem(wpn);
      weaponID = myItems.GetItem(wpn);
      //pWeapon = itemData.GetItem(wpnID);
      pWeapon = itemData.GetItem(weaponID);
      scriptContext.SetItemContext(pWeapon);
      scriptContext.SetItemContextKey(wpn);
    };
    scriptContext.SetCombatantContext(pCombatant);
  };

  scriptContext.SetCharacterContext(this);
  if (pCombatant != NULL)
  {
    pCombatant->RunCombatantScripts(GET_NUMBER_OF_ATTACKS, 
                                    ScriptCallback_MinMax, 
                                    minmax, 
                                    "Determine number of attacks");
  };
  RunCharacterScripts(GET_NUMBER_OF_ATTACKS, 
                      ScriptCallback_MinMax, 
                      minmax, 
                      "Determine number of attacks");
  ClearCharContext();

  if (minmax[1] != 9999999)
  {
    SetNbrAttacks(minmax[1]);
    return GetNbrAttacks();
  };

  /*
  if (GetAdjSpecAb(SA_HoldPerson,&src,&skey))
  {
    SetNbrAttacks(0.0);
    QueueUsedSpecAb(SA_HoldPerson,src,skey);
    return GetNbrAttacks();
  }
  else if (GetAdjSpecAb(SA_Sleep,&src,&skey))
  {
    SetNbrAttacks(0.0);
    QueueUsedSpecAb(SA_Sleep,src,skey);
    return GetNbrAttacks();
  }
  */
#endif
  if (GetType() != MONSTER_TYPE)
    SetNbrAttacks(1.0);
  else
    //SetNbrAttacks(monsterData.GetMonsterNbrAttacks(origIndex));
    SetNbrAttacks(monsterData.GetMonsterNbrAttacks(monsterID));

  //if (myItems.GetReady(WeaponHand) != NO_READY_ITEM)
  if (myItems.GetReadiedItem(WeaponHand, 0) != NO_READY_ITEM)
  {
     //double wpnAttacks = itemData.GetItemROF(myItems.GetItem(myItems.GetReady(WeaponHand)));
     double wpnAttacks = itemData.GetItemROF(myItems.GetItem(myItems.GetReadiedItem(WeaponHand, 0)));
     if (wpnAttacks < 1.0) wpnAttacks = 1.0;
     SetNbrAttacks(wpnAttacks);
     // check for sweeps
  }
  return GetNbrAttacks();
}

//*****************************************************************************
//    NAME: CHARACTER::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::GetContext(ActorType *pActor, WORD ClassFlag) const
void CHARACTER::GetContext(ActorType *pActor, const BASECLASS_ID& baseclassID) const
{
  //  Avoid all these extra Clears!!!  ActorType data;
#ifdef UAFEngine
  if (m_pCombatant != NULL)
  {
    //m_pCombatant->GetContext(pActor, ClassFlag);
    m_pCombatant->GetContext(pActor, baseclassID);
    return;
  };
#endif
  GetContext(pActor);
  pActor->Level = this->GetCurrentLevel(baseclassID);
  ASSERT( (pActor->Level>=0) && (pActor->Level<=40) );
}


//*****************************************************************************
//    NAME: CHARACTER::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::GetContext(ActorType *pActor, WORD ClassFlag) const
void CHARACTER::GetContext(ActorType *pActor, const SCHOOL_ID& schoolID) const
{
  //  Avoid all these extra Clears!!!  ActorType data;
#ifdef UAFEngine
  if (m_pCombatant != NULL)
  {
    //m_pCombatant->GetContext(pActor, ClassFlag);
    m_pCombatant->GetContext(pActor, schoolID);
    return;
  };
#endif
  GetContext(pActor);
  pActor->m_schoolID = schoolID;
}



//*****************************************************************************
//    NAME: CHARACTER::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::GetContext(ActorType *pActor, WORD ClassFlag) const
void CHARACTER::GetContext(ActorType *pActor, const SPELL_DATA *pSpell) const
{
  //  Avoid all these extra Clears!!!  ActorType data;
#ifdef UAFEngine
  if (m_pCombatant != NULL)
  {
    //m_pCombatant->GetContext(pActor, ClassFlag);
    m_pCombatant->GetContext(pActor, pSpell);
    return;
  };
#endif
  GetContext(pActor);
  pActor->m_spellID = pSpell->SpellID();
}







//*****************************************************************************
//    NAME: CHARACTER::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::GetContext(ActorType *pActor, WORD ClassFlag) const
void CHARACTER::GetContext(ActorType *pActor) const
{
  //  Avoid all these extra Clears!!!  ActorType data;
#ifdef UAFEngine
  if (m_pCombatant != NULL)
  {
    //m_pCombatant->GetContext(pActor, ClassFlag);
    m_pCombatant->GetContext(pActor);
    return;
  };
#endif
  pActor->Clear();

  if (IsPartyMember())
  {
    pActor->SetPartySrc();
  }
  else
  {
    switch (GetType())
    {
    case MONSTER_TYPE:
      pActor->SetMonsterSrc();
      break;
    case NPC_TYPE:
      pActor->SetNPCSrc();
      if (gender == Bishop)
      {
        pActor->SetBishopSrc();
      };
      break;
    case CHAR_TYPE:
      // This will be used during character creation. The temp char
      // is displayed before it is added to the party.
      //return NULL_ACTOR;
      pActor->SetCreatedCharacterSrc();
      break;
    case FAKE_CHARACTER_TYPE:
      // Used when processing script $CASTSPELLONTARGET.
      pActor->SetFakeCharacterSrc();
      if (IsCombatActive())
      {
        pActor->Flags &= ~FLAG_NONCOMBAT;
        pActor->Flags |= FLAG_COMBAT;
      };
      break;
    }
  }

  // if ClassFlag=0, GetCurrentLevel will
  // return the highest level attained from
  // all classes this character has
  //pActor->Level = GetCurrentLevel(ClassFlag);
  pActor->Level = 0;  // Unknown level

  //pActor->Race = race();
  pActor->m_raceID = race;
  pActor->m_classID = GetClass();
  if (gender == Bishop)
  {
    pActor->Instance = origIndex;
    pActor->instanceType = InstanceType_GlobalDataIndex;
  }
  else if (IsPartyMember())
  {
    pActor->Instance = uniquePartyID; // assigned when added to party, never changes
    pActor->instanceType = InstanceType_UniquePartyID;
  }
  else
  {
    pActor->Instance = origIndex;
    pActor->instanceType = InstanceType_OrigIndex;
//#ifdef newCombatant
//    if (m_pCombatant != NULL)
//    {
//      pActor->Instance = 0;
//      pActor->instanceType = InstanceType_Combatant;
//    };
//#endif


  };
}














//*****************************************************************************
//    NAME: CHARACTER::EvalDiceAsClass
//
// PURPOSE:   Determine character's level and roll dice
//
//*****************************************************************************
//double CHARACTER::EvalDiceAsClass(DICEPLUS &dice, WORD ClassFlag, int *pRollerLevel)
double CHARACTER::EvalDiceAsClass(DICEPLUS &dice, const BASECLASS_ID& baseclassID, int *pRollerLevel) const
{
  ActorType actor;
  //GetContext(&actor, ClassFlag);
  GetContext(&actor, baseclassID);
  if (pRollerLevel != NULL)
  {
    if (*pRollerLevel >=0)
    {
      actor.Level = *pRollerLevel;
    }
    else
    {
      *pRollerLevel = actor.Level;
    };
  }
  SetCharContext(&actor);
  double result;
  dice.Roll(result);
  ClearCharContext();
  return result;
}

double CHARACTER::EvalDiceAsClass(DICEPLUS &dice, const SPELL_DATA *pSpell, int *pRollerLevel) const
{
  ActorType actor;
  //GetContext(&actor, ClassFlag);
  GetContext(&actor, pSpell);
  if (pRollerLevel != NULL)
  {
    if (*pRollerLevel >=0)
    {
      actor.Level = *pRollerLevel;
    }
    else
    {
      actor.DetermineLevel(pSpell->schoolID, pSpell->Level);
      *pRollerLevel = actor.Level;
    };
  }
  SetCharContext(&actor);
  double result;
  dice.Roll(result);
  ClearCharContext();
  return result;
}

double CHARACTER::EvalDiceAsClass(DICEPLUS &dice,
                                  const SCHOOL_ID& schoolID, 
                                  int spellLevel,
                                  int *pRollerLevel) const
{
  ActorType actor;
  //GetContext(&actor, ClassFlag);
  GetContext(&actor, schoolID);
  if (pRollerLevel != NULL)
  {
    if (*pRollerLevel >=0)
    {
      actor.Level = *pRollerLevel;
    }
    else
    {
      actor.DetermineLevel(schoolID, spellLevel);
      *pRollerLevel = actor.Level;
    };
  }
  SetCharContext(&actor);
  double result;
  dice.Roll(result);
  ClearCharContext();
  return result;
}

#ifdef UAFEngine

//*****************************************************************************
//    NAME: CHARACTER::CalcRestTime
//
// PURPOSE: 
//
//*****************************************************************************
DWORD CHARACTER::CalcRestTime(void)
{
  if (GetAdjStatus() != Okay) return 0;
  return (m_spellbook.CalcRestTime());
}

//*****************************************************************************
//    NAME: CHARACTER::HaveEffectAttr
//
// PURPOSE: 
//
//*****************************************************************************
bool CHARACTER::HaveEffectAttr(const SPELL_EFFECTS_DATA &src) const
{
  POSITION pos = m_spellEffects.GetHeadPosition();
  while (pos != NULL)
  {
    if (*m_spellEffects.PeekAt(pos) == src) 
    {
      return true;
    };
    m_spellEffects.PeekNext(pos);
  }
  return false;
}

//*****************************************************************************
//    NAME: CHARACTER::GetFirstEffectAttributeMod
//
// PURPOSE: 
//
//*****************************************************************************
POSITION CHARACTER::GetFirstEffectAttributeMod(IF_KEYWORD_INDEX attr) const
{
  POSITION pos = m_spellEffects.GetHeadPosition();
  while (pos != NULL)
  {
    if (m_spellEffects.PeekAt(pos)->affectedAttr_IFKEY == attr) 
      return pos;
    m_spellEffects.PeekNext(pos);
  }
  return NULL;
}
//*****************************************************************************
//    NAME: CHARACTER::GetNextEffectAttributeMod
//
// PURPOSE: 
//
//*****************************************************************************
POSITION CHARACTER::GetNextEffectAttributeMod(IF_KEYWORD_INDEX attr, POSITION start) const
{
  POSITION pos = start;
  m_spellEffects.PeekNext(pos);
  while (pos != NULL)
  {
    if (m_spellEffects.PeekAt(pos)->affectedAttr_IFKEY == attr) 
      return pos;
    m_spellEffects.PeekNext(pos);
  }
  return NULL;
}


#ifdef UAFEngine
//*****************************************************************************
//    NAME: CHARACTER::IsAffectedBySpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL CHARACTER::IsAffectedBySpell(const SPELL_ID& SpellUniqueName)
BOOL CHARACTER::IsAffectedBySpell(const SPELL_ID& spellID)
{
  // first, get spell db key
  //GLOBAL_SPELL_ID spellkey = spellData.FindUniqueName(SpellUniqueName);
  //if (!spellkey.IsValidSpell()) 
  if (!spellID.IsValidSpell()) 
  {
    WriteDebugString("IsAffectedBySpell(\'%s\') could not find spell name\n", spellID.UniqueName());
    return FALSE;
  }

  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {    
    //if (m_spellEffects.GetAt(pos).SourceSpell_ID() == spellkey)
    if (m_spellEffects.PeekAt(pos)->SourceSpell_ID() == spellID)
    {
      return TRUE;
    }
    m_spellEffects.PeekNext(pos);
  }
  return FALSE;
}
#endif
//*****************************************************************************
//    NAME: CHARACTER::IsAffectedBySpellAttribute
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsAffectedBySpellAttribute(const CString& SpellAttribName)
{ 
  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {    
    //GLOBAL_SPELL_ID spelldbkey = m_spellEffects.GetAt(pos).SourceSpell_ID();
    SPELL_ID spellID;
    spellID = m_spellEffects.PeekAt(pos)->SourceSpell_ID();
    //SPELL_DATA *pData = spellData.GetSpellData(spelldbkey);
    SPELL_DATA *pData = spellData.GetSpell(spellID);
    if (pData != NULL)
    {
      if (pData->spell_asl.Find(SpellAttribName) != NULL)
        return TRUE;
    }
    m_spellEffects.GetNext(pos);
  }

  return (char_asl.Find(SpellAttribName) != NULL);
}


// parent is the key from global list of active spells
//*****************************************************************************
//    NAME: CHARACTER::RemoveSpellEffect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::RemoveSpellEffect(DWORD parent, const SPELL_DATA *pSpell, bool endSpell)
{
  if (parent<0) return FALSE; // activeSpellList key
  bool found = false;
  
  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {
    POSITION tmp=pos;
    m_spellEffects.GetNext(tmp);

    if (m_spellEffects.PeekAt(pos)->parent == parent)
    {
      // A few spell effects can linger longer than
      // parent spell, but most effects will return
      // TRUE here. Those that return FALSE will be
      // removed eventually by function CheckForExpiredSpellEffects().
      //
      const SPELL_EFFECTS_DATA *pSE;
      pSE = m_spellEffects.PeekAt(pos);
      if (pSE->IsReadyToExpire())
      {
        found = true;
        ModifyByDouble(pSE->affectedAttr_IFKEY, -pSE->changeResult);
        m_spellEffects.RemoveAt(pos);
      }
    }
    pos=tmp;
  }
  

  /* 20141229  Version 1.0303  PRS.
   * I want the SpellEndScript to match up one-to-one with the SpellBeginScript.
   * As it was, we ran the Begin Script once for each target the spell attacked.
   * Then we ran the End Script for each spell effect that was removed.  The result
   * was that the Begin Script might run without any End Script or one Begin
   * Script might result in several End Scripts.  Moreover, the spell effects might
   * end at a much later time when the state of the caster could easily have
   * undergone dramtic changes.
   *
   * So.....Now we run the Begin Script, apply any effects to the target, and
   * immediately run the End Script.  One-for-one in the same instant of time.
   *if (found)
   *{
   *  if (pSpell!=NULL)
   *  {
   *    //Not Implemented(0xc4ea,FALSE);
   *    //pSpell->specAbs.DisableAllFor(this);
   *    ActorType actor;
   *    GetContext(&actor);
   *    SetCharContext(&actor);
   *    SetTargetContext(&actor);
   *    
   *    pSpell->ExecuteSpellEndScript(NULL, this);
   *    
   *    ClearCharContext();
   *    ClearTargetContext();    
   *    WriteDebugString("Removing spell effects from %s for %s\n",
   *                     name, pSpell->Name);
   *  }   
   *  
   *}
   */
  DisplayCurrSpellEffects("RemoveSpellEffect");
  return found;
}

// parent is the key from global list of active spells
//*****************************************************************************
//    NAME: CHARACTER::RemoveSpellEffect
//
// PURPOSE: 
//
//*****************************************************************************
CString CHARACTER::RemoveSpellEffect(const CString& SEIdentifier)
{
  SPELL_ID spellID;
  DWORD parent;
  int n;
  int ob;  // Location of open bracket.
  ob = SEIdentifier.Find('[');
  if (ob < 0) return "";
  spellID = SEIdentifier.Left(ob);
  n = sscanf(LPCSTR(SEIdentifier)+ob+1,"%d",&parent);
  if (n != 1) return "";
  
  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {

    if (m_spellEffects.PeekAt(pos)->parent == parent)
    {
      SPELL_EFFECTS_DATA *pSE;
      ACTIVE_SPELL *pActiveSpell;
      pSE = m_spellEffects.GetAt(pos);
      pActiveSpell = activeSpellList.Get(parent);
      //if (spellData.GetSpellData(pActiveSpell->spellKey)->Name != spellName)
      if (spellData.GetSpell(pActiveSpell->spellID)->SpellID() != spellID)
      {
        return "";
      };
      m_spellEffects.RemoveAt(pos);
      return "1";
    }
    m_spellEffects.GetNext(pos);
  }
  
  return "";
  //DisplayCurrSpellEffects("RemoveSpellEffect");
  //return found;
}

CString CHARACTER::RunSEScripts(const CString& scriptName, LPCSTR comment)
{
  bool scriptProcessed;
  CString result;
  POSITION pos = m_spellEffects.GetHeadPosition();  
  for ( ;pos != NULL; m_spellEffects.GetNext(pos))
  {
    m_spellEffects.GetAt(pos)->m_scriptProcessed = false;
    // Any spell effects that get added will have m_scriptProcessed = true;
  };
  for (scriptProcessed=true; scriptProcessed;)
  {
    scriptProcessed = false;
    for (pos = m_spellEffects.GetHeadPosition() ;pos != NULL; m_spellEffects.GetNext(pos))
    {
      SPELL_EFFECTS_DATA *pSE;
      pSE = m_spellEffects.GetAt(pos); 
      if (!pSE->m_scriptProcessed)
      {

        DWORD SESpellKey;
        SPELL_DATA *pSpell;
        ACTIVE_SPELL *pActiveSpell;
        CString SEIdentifier;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;

        pSE->m_scriptProcessed = true;
        SESpellKey = pSE->parent;
        // Search the ACTIVE_SPELL_LIST for this key
        pActiveSpell = activeSpellList.Get(SESpellKey);
        //pSpell= spellData.GetSpellData(pActiveSpell->spellKey);
        pSpell= spellData.GetSpell(pActiveSpell->spellID);


        scriptContext.SetSpellContext(pSpell);
        scriptContext.SetCharacterContext(this);
        SEIdentifier.Format("%s[%d]", pSpell->Name, SESpellKey);
        hookParameters[5] = SEIdentifier;
        result += pSpell->RunSpellScripts(scriptName, 
                                          ScriptCallback_RunAllScripts, 
                                          NULL, 
                                          comment);






        scriptProcessed = true;
        break;
      };
    };
  };
  return result;
}

CString CHARACTER::ForEachPossession(const CString& scriptName)
{
  bool scriptProcessed;
  CString result;
  POSITION pos = this->myItems.GetHeadPosition();  
  for ( ;pos != NULL; myItems.GetNext(pos)) //PRS 20120829 spellEffects.GetNext(pos))
  {
    myItems.GetAtPos(pos).m_scriptProcessed = false;
    // Any spell effects that get added will have m_scriptProcessed = true;
  };
  for (scriptProcessed=true; scriptProcessed;)
  {
    scriptProcessed = false;
    for (pos = myItems.GetHeadPosition() ;pos != NULL; myItems.GetNext(pos))
    {
      ITEM *pItem;
      pItem = &myItems.GetAtPos(pos); 
      if (!pItem->m_scriptProcessed)
      {
        ITEM_DATA *pItemData;
        //pItemData = itemData.GetItem(pItem->m_giID);
        pItemData = itemData.GetItem(pItem->itemID);
        HOOK_PARAMETERS hookParameters;
        pScriptContext->SetItemContext(pItemData);
        pScriptContext->SetItemContextKey(pItem->key);
        pItem->m_scriptProcessed = true;
        result += pItemData->RunItemScripts(scriptName, 
                                            ScriptCallback_RunAllScripts, 
                                            NULL,
                                            "Processing $ForEachPossession(actor, scriptName)");
        scriptProcessed = true;
        break;
      };
    };
  };
  return result;
}



//*****************************************************************************
//    NAME: CHARACTER::CheckForExpiredSpellEffects
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::CheckForExpiredSpellEffects()
{
  bool done = false;  

  while (!done)
  {
    BOOL removed = FALSE;
    POSITION pos = m_spellEffects.GetHeadPosition();  
    while (pos != NULL)
    {    
      // Look for spell effects in the list that have a duration different than
      // parent SPELL_DATA.
      // 
      if (m_spellEffects.PeekAt(pos)->StopTime > 0)
      {
        // remove spell effect (still checks expiration time)
        //SPELL_DATA *pSpell = spellData.GetSpellData(m_spellEffects.GetAt(pos).SourceSpell_ID());
        SPELL_DATA *pSpell = spellData.GetSpell(m_spellEffects.PeekAt(pos)->SourceSpell_ID());

        removed = RemoveSpellEffect(m_spellEffects.PeekAt(pos)->parent, pSpell, false);
      }  

      if (!removed)
        m_spellEffects.GetNext(pos);
      else
        break;
    }   
    
    if (!removed)
      done = true;
  }
}

//*****************************************************************************
//    NAME: CHARACTER::UpdateSpellForDamage
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::UpdateSpellForDamage(int DamageTaken)
{
  if (DamageTaken<=0) return;

  bool found = false;
  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {    
    //GLOBAL_SPELL_ID spelldbkey = m_spellEffects.GetAt(pos).SourceSpell_ID();
    SPELL_ID spellID;
    spellID = m_spellEffects.PeekAt(pos)->SourceSpell_ID();

    //SPELL_DATA *pSpell = spellData.GetSpellData(spelldbkey);
    SPELL_DATA *pSpell = spellData.GetSpell(spellID);
    if (pSpell != NULL)
    {
      if (pSpell->Duration_Rate == byDamageTaken)
      {
        int activespellkey = m_spellEffects.PeekAt(pos)->parent;
        ACTIVE_SPELL *pActive = activeSpellList.Get(activespellkey);
        if (pActive != NULL)
        {
          found = true;
          pActive->CountTime += DamageTaken;
          activeSpellList.Set(activespellkey, *pActive);
        }
      }
    }

    m_spellEffects.GetNext(pos);
  }

  if (found)
    activeSpellList.ProcessTimeSensitiveData(0);  
}

//*****************************************************************************
//    NAME: CHARACTER::UpdateSpellForAttacks
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::UpdateSpellForAttacks(int AttacksTaken)
{
  // this func is only called when a successful hit is 
  // made on this dude.

  if (AttacksTaken <= 0) return;

  bool found = false;
  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {    
    //GLOBAL_SPELL_ID spelldbkey = m_spellEffects.GetAt(pos).SourceSpell_ID();
    SPELL_ID spellID;
    spellID = m_spellEffects.PeekAt(pos)->SourceSpell_ID();
    
    //SPELL_DATA *pSpell = spellData.GetSpellData(spelldbkey);
    SPELL_DATA *pSpell = spellData.GetSpell(spellID);
    if (pSpell != NULL)
    {
      if (pSpell->Duration_Rate == byNbrAttacks)
      {
        int activespellkey = m_spellEffects.PeekAt(pos)->parent;
        ACTIVE_SPELL *pActive = activeSpellList.Get(activespellkey);
        if (pActive != NULL)
        {
          found = true;
          pActive->CountTime += AttacksTaken;
          activeSpellList.Set(activespellkey, *pActive);
        }
      }
    }
    
    m_spellEffects.GetNext(pos);
  }

  if (GetAdjSpecAb(SA_MirrorImage))
  {
    // decrement counter used to keep track of number of mirror images
    //AdjustEffectData("$CHAR_MIRRORIMAGE", -1);
    AdjustEffectData(CHAR_MIRRORIMAGE, -1);
  }

  if (found)
    activeSpellList.ProcessTimeSensitiveData(0);
}

//*****************************************************************************
//    NAME: CHARACTER::RemoveSpecAb
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::RemoveSpecAb(SPELL_EFFECTS_DATA &data)
{
  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {
    POSITION tmp=pos;
    m_spellEffects.GetNext(tmp);
    // Don't remove the ones associated with an
    // active spell (parent > 0)
    //
    // Those placed here as spec abs will have
    // parent = 0. It doesn't matter which one
    // is removed. That is, there may be more
    // than one instance of the same effect
    // in this list. But this list is also
    // used for spells and their spell effects,
    // so make sure only a spec ab effect is
    // removed.

    if (m_spellEffects.PeekAt(pos)->parent<=0)
    {
      if (m_spellEffects.PeekAt(pos)->AffectedAttr().CompareNoCase(data.AffectedAttr())==0)
      {
        WriteDebugString("Removing special ability from %s: %s\n", 
                         GetName(),
                         m_spellEffects.PeekAt(pos)->AffectedAttr());
        m_spellEffects.RemoveAt(pos);
        DisplayCurrSpellEffects("RemoveSpecAb");
        return;
      }
    }
    pos=tmp;
  }

  DisplayCurrSpellEffects("RemoveSpecAb");
}

//*****************************************************************************
//    NAME: CHARACTER::RemoveAllEffectsFor
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::RemoveAllEffectsFor(const CString &AttrName)
{
  POSITION pos = m_spellEffects.GetHeadPosition();  
  while (pos != NULL)
  {
    POSITION tmp=pos;
    m_spellEffects.GetNext(tmp);
    
    if (m_spellEffects.PeekAt(pos)->AffectedAttr().CompareNoCase(AttrName)==0)
    {
      WriteDebugString("Removing (all) spell effect from %s: %s\n", 
                        GetName(),
                        AttrName);

      m_spellEffects.RemoveAt(pos);
    }

    pos=tmp;
  }
  DisplayCurrSpellEffects("RemoveAllEffectsFor");
}

//*****************************************************************************
//    NAME: CHARACTER::AddSpecAb
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::AddSpecAb(SPELL_EFFECTS_DATA &data)
{
  WriteDebugString("Adding special ability to %s: %s\n", 
                   GetName(),
                   data.AffectedAttr());

  m_spellEffects.AddTail(data);
  DisplayCurrSpellEffects("AddSpecAb");
  return TRUE;
}


//*****************************************************************************
//    NAME: CHARACTER::DoesSavingThrowSucceed
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::DoesSavingThrowSucceed(SPELL_DATA *pSdata,
                                       SAVING_THROW_DATA &stData, 
                                       BOOL friendlyFire,
                                       CHARACTER *pCaster,
                                       CString *displayText)
{
  // Saving Throws:
  //
  //   It is assumed that no saving throw is needed when casting a spell
  //   on yourself, or on willing recipients. The party members are
  //   always assumed to be willing.
  //
  //   spellSaveVersusType { ParPoiDM, PetPoly, RodStaffWand, Sp, BreathWeapon };
  //   
  //   Retrieve needed save value for target based on SaveVersusType. Each type
  //   will have a single value that increases with target level. Roll 20-sided
  //   dice for target's saving throw. If roll is less than SaveVersus value, no
  //   save is made and full spell affect is applied. If roll is equal to or
  //   greater than SaveVersus, then SaveEffectType is used.
  //
  //   spellSaveEffectType { NoSave, SaveNegates, SaveForHalf, UseTHAC0 };
  //
  //   NoSave: full affect applied to target
  //   SaveNegates: no affect applied to target
  //   SaveForHalf: if numeric attribute affected, apply half, otherwise ignore
  //   UseTHAC0: if saving throw less than THAC0, full affect, else none

  // need to ignore this for self or party members!


  // Requested by Eric 20121017 PRS  
  //if (!friendlyFire || pCaster->targets.m_area)
  {
    int bonus = (int)pSdata->ExecuteSavingThrowScript(stData);
    stData.success = TRUE;
    stData.noEffectWhatsoever = FALSE;
    stData.changeResult = 1.0;
    if (pSdata->Save_Result == UseTHAC0)
    {
      int adjTHAC0, diceRoll, AC;
      diceRoll = RollDice(20, 1, bonus);
      adjTHAC0 = pCaster->GetAdjTHAC0();
      AC = GetAdjAC();
      if (diceRoll > AC-adjTHAC0)
      {
        stData.changeResult = 0.0;
        stData.noEffectWhatsoever = TRUE;
      }
      else
      {
        stData.success = FALSE;
      };
      if (logDebuggingInfo)
      {
        WriteDebugString("DoesSavingThrowSucceed(UseTHAC0); THAC0=%d DiceRoll=%d; %s\n",
          adjTHAC0, diceRoll, stData.success ? "Success" : "Failure");
      };
    }
    else
    {
      if (DidSaveVersus(pSdata->Save_Vs, bonus, pSdata, pCaster))
      {
        switch (pSdata->Save_Result)
        {
        case NoSave: // Full affect in spite of save.
          break;
        case SaveNegates: // no affect
          stData.noEffectWhatsoever = TRUE; ;
          stData.changeResult = 0.0;
          break;
        case SaveForHalf:
          stData.changeResult = stData.changeResult / 2.0;
          break;
        }
      }
      else
        stData.success = FALSE;
    }

    if (stData.success)
      pSdata->ExecuteSavingThrowSucceededScript(stData, displayText);
    else
      pSdata->ExecuteSavingThrowFailedScript(stData, displayText);

  }

  return stData.success;
}



//*****************************************************************************
//    NAME: CHARACTER::CalcSpellEffectChangeValue
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::CalcSpellEffectChangeValue(SPELL_DATA *pSdata,
                                           SPELL_EFFECTS_DATA &saData, 
                                           BOOL friendlyFire,
                                           CHARACTER *pCaster)
{
  //
  // assumes that current character and target context are already set up
  // Assumes that $AttackerContext() and $TargetContext() are already set up.
  //

  saData.m_pCaster = pCaster;
  saData.m_pTarget = this;
  saData.ClearChangeResult();
  saData.GetChange(); // rolls delta value and saves for future lookup by ApplyChange()

}

extern const unsigned int char_HITPOINTS;

//*****************************************************************************
//    NAME: CHARACTER::AddSpellEffect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::AddSpellEffect(SPELL_EFFECTS_DATA &data, 
                               const CHARACTER *pAttacker,
                               InvokeSpellOptions *pInvokeOptions,
                               LPCSTR comment)
{
  double delta=data.changeResult;
  bool isPerm;
  SPELL_DATA *pSdata;

  // ***************
  // 20141219 Manikus complained that spell effects were being applied in spite
  // of a successful saving throw.
  // if not cumulative with other effects having same name
  if (data.flags & SPELL_EFFECTS_DATA::EFFECT_NONE) return FALSE;
  // ***************


  if ((data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE) == 0)
  {
    // not cumulative, check for existing same spell effect in list
    if (GetFirstEffectAttributeMod(data.affectedAttr_IFKEY) != NULL)
    {
      WriteDebugString("NOT Applying spell effect (not cumulative): %s\n", 
        data.AffectedAttr());
      return FALSE;
    }
  }

  if (data.flags & SPELL_EFFECTS_DATA::EFFECT_REMOVEALL)
  {
    // remove all that match
    POSITION pos = GetFirstEffectAttributeMod(data.affectedAttr_IFKEY);
    while (pos != NULL)
    {
      POSITION tmp = pos;
      GetNextEffectAttributeMod(data.affectedAttr_IFKEY, pos);

      // can't remove character special abilities, since they are intrinsic
      if ((m_spellEffects.PeekAt(tmp)->flags & SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB) == 0)
      {        
        //SPELL_DATA *pSpell = spellData.GetSpellData(m_spellEffects.GetAt(tmp).SourceSpell_ID());
        //SPELL_DATA *pSpell = spellData.GetSpell(m_spellEffects.PeekAt(tmp)->SourceSpell_ID());
        m_spellEffects.RemoveAt(tmp);
        
      /* 20141229  Version 1.0303  PRS.
       * I want the SpellEndScript to match up one-to-one with the SpellBeginScript.
       * As it was, we ran the Begin Script once for each target the spell attacked.
       * Then we ran the End Script for each spell effect that was removed.  The result
       * was that the Begin Script might run without any End Script or one Begin
       * Script might result in several End Scripts.  Moreover, the spell effects might
       * end at a much later time when the state of the caster could easily have
       * undergone dramtic changes.
       *
       * So.....Now we run the Begin Script, apply any effects to the target, and
       * immediately run the End Script.  One-for-one in the same instant of time.
       *  if (pSpell!=NULL)
       *  {
       *    Not Implemented(0xd432c0, FALSE);//pSpell->specAbs.DisableAllFor(this);
       *
       *    ActorType actor;
       *    GetContext(&actor);
       *    SetCharContext(&actor);
       *    SetTargetContext(&actor);
       *
       *    pSpell->ExecuteSpellEndScript(NULL, this);
       *
       *    ClearCharContext();
       *    ClearTargetContext();    
       *  }   
       */
      }
    }
    DisplayCurrSpellEffects("AddSpellEffect");
    return TRUE;
  }  

  //SPELL_DATA *pSdata = spellData.GetSpellData(data.SourceSpell_ID());
  if (data.flags & SPELL_EFFECTS_DATA::EFFECT_SCRIPT)
  {
    isPerm = false;
    pSdata = NULL;
  }
  else
  {
    pSdata = spellData.GetSpell(data.SourceSpell_ID());
    if (pSdata==NULL) 
    {
      die(0xab502);
      WriteDebugString("NOT Applying spell effect (spell key bogus): %s, delta %f, cumulative %i\n", 
        data.AffectedAttr(), data.changeResult, (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
      return FALSE;
    }

    isPerm = (pSdata->Duration_Rate == Permanent);
  };
  int tmp = delta; // drop any fractional parts
  delta = tmp;
  data.changeResult = delta;

  if (!isPerm)
  {
    // if not permanent, store in spell effects list
    WriteDebugString("Adding non-perm spell effect to %s: %s, delta %f, cumulative %i\n", 
                     GetName(),
                     data.AffectedAttr(), 
                     data.changeResult, 
                     (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
    data.flags &= ~SPELL_EFFECTS_DATA::EFFECT_ONCEONLY;
    data.flags &= ~SPELL_EFFECTS_DATA::EFFECT_APPLIED;
    if (pSdata != NULL)
    {
      if (!pSdata->Lingers || pSdata->LingerOnceOnly)
      {
        data.flags |= SPELL_EFFECTS_DATA::EFFECT_ONCEONLY;
      };
    };
    if ((data.flags & SPELL_EFFECTS_DATA::EFFECT_SCRIPT) == 0)
    {
      data.affectedAttr_IFKEY = (IF_KEYWORD_INDEX)KeywordTextToNum(data.AffectedAttr());
    };
    ModifyByDouble(data.affectedAttr_IFKEY, data.changeResult);
    m_spellEffects.AddTail(data);
  }
  else // else apply effects to attribute now
  {   
    // invalid attribute string
    if (data.AffectedAttr().IsEmpty())
      return FALSE;

    ActorType actor;
    GetContext(&actor);

    // determine which attribute is modified and
    // get delta value
    //
    double val=0.0;
    DWORD attKey = KeywordTextToNum(data.AffectedAttr());
    BOOL IsVirtTrait = IsVirtualTrait(attKey);
    
    if (!IsVirtTrait)
    {
      BYTE attSize;
      // modify actual attribute value now
      {
        attSize = GetDataSize(attKey);
        switch (attSize)
        {
        case IF_TYPE_STRING:
          // no spell affects for string types
          return FALSE;
          break;
        case IF_TYPE_BYTE:   val = GetDataBYTE(attKey,&actor,comment);   break;
        case IF_TYPE_WORD:   val = GetDataWORD(attKey,&actor);   break;
        case IF_TYPE_DWORD:  val = GetDataDWORD(attKey,&actor);  break;
        case IF_TYPE_FLOAT:  val = GetDataFLOAT(attKey,&actor);  break;
        case IF_TYPE_UBYTE:  val = GetDataUBYTE(attKey,&actor);  break;
        case IF_TYPE_UWORD:  val = GetDataUWORD(attKey,&actor);  break;
        case IF_TYPE_UDWORD: val = GetDataUDWORD(attKey,&actor); break;
        default: die(0xab503); return FALSE; break;
        }
      };

      // apply delta value to affected attribute
      //val += delta;


// Call the ComputeSpellDamage hook to modify any changes to the targets attributes.
      {
        CString result, message;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        SPELL_DATA *pSpell=pSdata;
        hookParameters[5].Format("%d",int(data.changeResult));
        hookParameters[6]= data.AffectedAttr();
        scriptContext.SetAttackerContext(pAttacker);
        scriptContext.SetTargetContext(this);
        scriptContext.SetSpellContext(pSpell);

        result = pSpell->RunSpellScripts(
                COMPUTE_SPELL_DAMAGE,
                ScriptCallback_RunAllScripts,
                NULL,
                "Add spell effect - Computing damage");
        if (!result.IsEmpty())
        {
          int temp;
          if (sscanf(result,"%d",&temp) == 1) 
          {
            data.changeResult = temp;
          };
        };
        if (this->GetType() == MONSTER_TYPE)
        {
          //int monsterIndex;
          COMBATANT *pCombatant;
          pCombatant = this->m_pCombatant;
          if (pCombatant != NULL)
          {
            //monsterIndex = pCombatant->origKey;
            {
              //MONSTER_DATA *pMonsterData;
              const MONSTER_DATA *pMonsterData;
              pMonsterData = monsterData.PeekMonster(monsterID);
              if (pMonsterData != NULL)
              {
                result = pMonsterData->RunMonsterScripts(
                        COMPUTE_SPELL_DAMAGE,
                        ScriptCallback_RunAllScripts,
                        NULL,
                        "Adding spell effect - Computing damage");
                if (!result.IsEmpty())
                {
                  int temp;
                  if (sscanf(result,"%d",&temp) == 1) 
                  {
                    data.changeResult = temp;
                  };
                };
              };
            };
          };
        };
        if (this->m_pCombatant != NULL)
        {
          result = this->m_pCombatant->RunCombatantScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL,
                  "Adding spell effect - Computing damage");
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
        //message = hookParameters[7];
        {
          result = pAttacker->RunCharacterScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL,
                  "Adding spell effect - Computing damage");
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
        {
          result = this->RunCharacterScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL,
                  "Adding spell effect - Computing damage");
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
      };


      val = data.ApplyChange(val); // might be delta, percentage, or absolute change

      actor.m_canFinishCasting = pAttacker==this;

      {
        switch (attSize)
        {
        case IF_TYPE_BYTE:   SetDataBYTE(attKey, val,&actor);   break;
        case IF_TYPE_WORD:   SetDataWORD(attKey, val,&actor);   break;
        case IF_TYPE_DWORD:  SetDataDWORD(attKey, val,&actor);  break;
        case IF_TYPE_FLOAT:  SetDataFLOAT(attKey, val,&actor);  break;
        case IF_TYPE_UBYTE:  SetDataUBYTE(attKey, val,&actor);  break;
        case IF_TYPE_UWORD:  SetDataUWORD(attKey, val,&actor);  break;
        case IF_TYPE_UDWORD: SetDataUDWORD(attKey, val,&actor); break;
        default: die(0xab504); break;
        };
  
        actor.m_canFinishCasting = false;
        if (logDebuggingInfo)
        {
          WriteDebugString("Applying perm spell effect to %s: %s, delta %f, result %f, cumulative %i\n",
            GetName(),
            data.AffectedAttr(),
            delta,
            val,
            (data.flags&SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE) ? 1 : 0);
        };
      };
    }
    else
    {  // It must be a virtual Trait
      // there is no CHARACTER value that represents this attribute, so
      // we need to store it in the spellEffects list
        
      // first find out if same attribute exists in list of active
      // then either apply mods to existing or add this new one
      //
      // only one such attribute should ever exist in the list of actives
      //

      POSITION pos = GetFirstEffectAttributeMod(data.affectedAttr_IFKEY);
      if (pos != NULL)
      {
        // get old attribute data
        const SPELL_EFFECTS_DATA &olddata = *m_spellEffects.PeekAt(pos);
        // apply new modification to old mod value
        data.changeResult = data.ApplyChange(olddata.changeResult);
        // update old data
        *m_spellEffects.GetAt(pos) = data;

        WriteDebugString("Updating perm virtual spell effect for %s: %s, delta %f, cumulative %i\n", 
                         GetName(),
                         data.AffectedAttr(), 
                         data.changeResult, 
                         (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));        
      }
      else
      {
        WriteDebugString("Adding perm virtual spell effect for %s: %s, delta %f, cumulative %i\n", 
                         GetName(),
                         data.AffectedAttr(), 
                         data.changeResult, 
                         (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
        
        m_spellEffects.AddTail(data);
      }
    }

    // transfer spell attributes to character for permanent spell
    CString sakey;
    CString savalue;    
    POSITION pos = pSdata->spell_asl.GetStartPosition();
    while (pos != NULL)
    {      
      const ASLENTRY *pEntry;
      pSdata->spell_asl.GetNextAssoc(pos, &pEntry);
      // adds new value if doesn't exist, else updates existing
      char_asl.Insert(pEntry->Key(), pEntry->Value(), ASLF_MODIFIED);
    }
  }

  DisplayCurrSpellEffects("AddSpellEffect");
  return TRUE;
}


//*****************************************************************************
//    NAME: CHARACTER::AddTemporaryEffect
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::AddTemporaryEffect(const CString& attrName,
                                   const CString& timeUnits,
                                   int            duration,
                                   int            delta,
                                   const CString& text,
                                   const CString& source,
                                   LPCSTR comment)
{
  IF_KEYWORD_INDEX ifKey;
  SPELL_EFFECTS_DATA seData;
  SPELL_EFFECTS_DATA::TIME_UNITS units;
       if (attrName == "STR")    ifKey = CHAR_STR;
  else if (attrName == "STRMOD") ifKey = CHAR_STRMOD;
  else if (attrName == "INT")    ifKey = CHAR_INT;
  else if (attrName == "WIS")    ifKey = CHAR_WIS;
  else if (attrName == "DEX")    ifKey = CHAR_DEX;
  else if (attrName == "CON")    ifKey = CHAR_CON;
  else if (attrName == "CHA")    ifKey = CHAR_CHA;
  else
  {
    if (!debugStrings.AlreadyNoted(CString("MSIAN")))
    {
      writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
      WriteDebugString("MODIFY_CHAR_xxx specified illegal attribute name\n");
    };
    return FALSE;
  };
  if (timeUnits == "MINUTES") units = SPELL_EFFECTS_DATA::TU_MINUTES;
  else
  {
    if (!debugStrings.AlreadyNoted(CString("MSIUIT")))
    {
      writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
      WriteDebugString("MODIFY_CHAR_xxx specified illegal units of time\n");
    };
    return FALSE;
  };
  seData.changeResult = delta;
  seData.affectedAttr_IFKEY = ifKey;
  seData.ScriptEffectText(text);
  seData.TimedEffectSource(source);
  seData.flags =  SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE;
  seData.flags |= SPELL_EFFECTS_DATA::EFFECT_SCRIPT;
  seData.flags |= SPELL_EFFECTS_DATA::EFFECT_TIMEDSA;
  seData.data = units;
  seData.StopTime = party.getElapsedMinutes() + duration;

 /*

  //SPELL_DATA *pSdata = spellData.GetSpellData(data.SourceSpell_ID());
  SPELL_DATA *pSdata = spellData.GetSpell(data.SourceSpell_ID());
  if (pSdata==NULL) 
  {
    ASS ERT(FALSE);
    WriteDebugString("NOT Applying spell effect (spell key bogus): %s, delta %f, cumulative %i\n", 
      data.affectedAttr, data.changeResult, (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
    return FALSE;
  }

  bool isPerm = (pSdata->Duration_Rate == Permanent);

  int tmp = delta; // drop any fractional parts
  delta = tmp;
  data.changeResult = delta;

  if (!isPerm)
  {
    // if not permanent, store in spell effects list
    WriteDebugString("Adding non-perm spell effect to %s: %s, delta %f, cumulative %i\n", 
                     GetName(),
                     data.affectedAttr, 
                     data.changeResult, 
                     (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
    data.flags &= ~SPELL_EFFECTS_DATA::EFFECT_ONCEONLY;
    data.flags &= ~SPELL_EFFECTS_DATA::EFFECT_APPLIED;
    if (!pSdata->Lingers || pSdata->LingerOnceOnly)
    {
      data.flags |= SPELL_EFFECTS_DATA::EFFECT_ONCEONLY;
    };
    data.affectedAttr_IFKEY = (IF_KEYWORD_INDEX)KeywordTextToNum(data.affectedAttr);
    ModifyByDouble(data.affectedAttr_IFKEY, data.changeResult);
    m_spellEffects.AddTail(data);
  }
  else // else apply effects to attribute now
  {   
    // invalid attribute string
    if (data.affectedAttr.IsEmpty())
      return FALSE;

    ActorType actor;
    GetContext(&actor);

    // determine which attribute is modified and
    // get delta value
    //
    double val=0.0;
    DWORD attKey = KeywordTextToNum(data.affectedAttr);
    BOOL IsVirtTrait = IsVirtualTrait(attKey);
    
    if (!IsVirtTrait)
    {
      // modify actual attribute value now
      BYTE attSize = GetDataSize(attKey);
      switch (attSize)
      {
      case IF_TYPE_STRING:
        // no spell affects for string types
        return FALSE;
        break;
      case IF_TYPE_BYTE:   val = GetDataBYTE(attKey,&actor);   break;
      case IF_TYPE_WORD:   val = GetDataWORD(attKey,&actor);   break;
      case IF_TYPE_DWORD:  val = GetDataDWORD(attKey,&actor);  break;
      case IF_TYPE_FLOAT:  val = GetDataFLOAT(attKey,&actor);  break;
      case IF_TYPE_UBYTE:  val = GetDataUBYTE(attKey,&actor);  break;
      case IF_TYPE_UWORD:  val = GetDataUWORD(attKey,&actor);  break;
      case IF_TYPE_UDWORD: val = GetDataUDWORD(attKey,&actor); break;
      default: ASS ERT(FALSE); return FALSE; break;
      }

      // apply delta value to affected attribute
      //val += delta;


// Call the ComputeSpellDamage hook to modify any changes to the targets attributes.
      {
        CString result, message;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        SPELL_DATA *pSpell=pSdata;
        hookParameters[5].Format("%d",int(data.changeResult));
        hookParameters[6]= data.affectedAttr;
        scriptContext.SetAttackerContext(pAttacker);
        scriptContext.SetTargetContext(this);
        scriptContext.SetSpellContext(pSpell);

        result = pSpell->specAbs.RunScripts(
                COMPUTE_SPELL_DAMAGE,
                ScriptCallback_RunAllScripts,
                NULL,
                pSpell->Name,
                "");
        if (!result.IsEmpty())
        {
          int temp;
          if (sscanf(result,"%d",&temp) == 1) 
          {
            data.changeResult = temp;
          };
        };
        if (this->GetType() == MONSTER_TYPE)
        {
          //int monsterIndex;
          COMBATANT *pCombatant;
          pCombatant = this->m_pCombatant;
          if (pCombatant != NULL)
          {
            //monsterIndex = pCombatant->origKey;
            {
              //MONSTER_DATA *pMonsterData;
              const MONSTER_DATA *pMonsterData;
              pMonsterData = monsterData.PeekMonster(monsterID);
              if (pMonsterData != NULL)
              {
                result = pMonsterData->specAbs.RunScripts(
                        COMPUTE_SPELL_DAMAGE,
                        ScriptCallback_RunAllScripts,
                        NULL,
                        pMonsterData->Name,
                        "");
                if (!result.IsEmpty())
                {
                  int temp;
                  if (sscanf(result,"%d",&temp) == 1) 
                  {
                    data.changeResult = temp;
                  };
                };
              };
            };
          };
        };
        if (this->m_pCombatant != NULL)
        {
          result = this->m_pCombatant->combatantSA.RunScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL,
                  this->GetName(),
                  "");
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
        //message = hookParameters[7];
        {
          result = pAttacker->RunCharacterScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL);
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
        {
          result = this->RunCharacterScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL);
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
      };


      val = data.ApplyChange(val); // might be delta, percentage, or absolute change

      actor.m_canFinishCasting = pAttacker==this;

      switch (attSize)
      {
      case IF_TYPE_BYTE:   SetDataBYTE(attKey, val,&actor);   break;
      case IF_TYPE_WORD:   SetDataWORD(attKey, val,&actor);   break;
      case IF_TYPE_DWORD:  SetDataDWORD(attKey, val,&actor);  break;
      case IF_TYPE_FLOAT:  SetDataFLOAT(attKey, val,&actor);  break;
      case IF_TYPE_UBYTE:  SetDataUBYTE(attKey, val,&actor);  break;
      case IF_TYPE_UWORD:  SetDataUWORD(attKey, val,&actor);  break;
      case IF_TYPE_UDWORD: SetDataUDWORD(attKey, val,&actor); break;
      default: ASS ERT(FALSE); break;
      };

      actor.m_canFinishCasting = false;
  
      WriteDebugString("Applying perm spell effect to %s: %s, delta %f, result %f, cumulative %i\n", 
                       GetName(),
                       data.affectedAttr, 
                       delta, 
                       val, 
                       (data.flags&SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE)?1:0);
    }
    else
    {
      // there is no CHARACTER value that represents this attribute, so
      // we need to store it in the spellEffects list
      
      // first find out if same attribute exists in list of active
      // then either apply mods to existing or add this new one
      //
      // only one such attribute should ever exist in the list of actives
      //

      POSITION pos = GetFirstEffectAttributeMod(data.affectedAttr_IFKEY);
      if (pos != NULL)
      {
        // get old attribute data
        const SPELL_EFFECTS_DATA &olddata = *m_spellEffects.PeekAt(pos);
        // apply new modification to old mod value
        data.changeResult = data.ApplyChange(olddata.changeResult);
        // update old data
        *m_spellEffects.GetAt(pos) = data;

        WriteDebugString("Updating perm virtual spell effect for %s: %s, delta %f, cumulative %i\n", 
                         GetName(),
                         data.affectedAttr, 
                         data.changeResult, 
                         (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));        
      }
      else
      {
        WriteDebugString("Adding perm virtual spell effect for %s: %s, delta %f, cumulative %i\n", 
                         GetName(),
                         data.affectedAttr, 
                         data.changeResult, 
                         (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
        
        m_spellEffects.AddTail(data);
      }
    }

    // transfer spell attributes to character for permanent spell
    CString sakey;
    CString savalue;    
    POSITION pos = pSdata->spell_asl.GetStartPosition();
    while (pos != NULL)
    {      
      const ASLENTRY *pEntry;
      pSdata->spell_asl.GetNextAssoc(pos, &pEntry);
      // adds new value if doesn't exist, else updates existing
      char_asl.Insert(pEntry->Key(), pEntry->Value(), ASLF_MODIFIED);
    }
  }
*/
  AddSpellEffect(seData, NULL, NULL, comment);
  DisplayCurrSpellEffects("AddSpellEffect");
  return TRUE;
}

BOOL MatchMask(const char **pM, const char **pD)
{
  const char *pMask, *pData;
  pMask = *pM; pData = *pD;
  while (*pMask != 0)
  {
    while (*pMask != 0) {if(*pMask != ' ') break; pMask++;};
    if (*pMask == 0) return TRUE;  // No more mask... must be a match.
    while (*pData != 0) {if (*pData != ' ') break; else pData++;};
    if (*pMask == '*')
    {
      while (pData != 0) {if (*pData == ' ') break; pData++;};
      while (pMask != 0) {if (*pMask == ' ') break; pMask++;};
      continue;
    };
    while ((*pMask != 0) && (*pMask != ' '))
    {
      if (*pMask != *pData) return FALSE;
      pMask++; pData++;
    };
    if ((*pData != 0) && (*pData!= ' ')) return FALSE;
  };
  *pM = pMask; *pD = pData;
  return TRUE;
}

BOOL CHARACTER::RemoveTemporaryEffect(const CString& mask)
{
  POSITION pos;
  
  for (pos = m_spellEffects.GetHeadPosition();pos != NULL;m_spellEffects.PeekNext(pos))
  {
    const SPELL_EFFECTS_DATA *psa;
    psa = m_spellEffects.PeekAt(pos);
    if (psa->flags & SPELL_EFFECTS_DATA::EFFECT_TIMEDSA)
    {
      const char *pMask, *pData;
      pMask = (LPCSTR)mask;
      pData = (LPCSTR)(psa->TimedEffectSource());
      if (MatchMask(&pMask, &pData))
      {
        // We seem to have found a match.
        ModifyByDouble(psa->affectedAttr_IFKEY, -psa->changeResult);
        m_spellEffects.RemoveAt(pos);
        return TRUE;
      };
    };
  };
  return FALSE;
}



//*****************************************************************************
//    NAME: CHARACTER::AddLingeringSpellEffect
//
// PURPOSE: 
//
//*****************************************************************************

BOOL CHARACTER::AddLingeringSpellEffect(SPELL_EFFECTS_DATA &data,
                                        const CHARACTER *pAttacker, 
                                        LPCSTR comment)
{
  double delta=data.changeResult;

  // if not cumulative with other effects having same name
  if ((data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE) == 0)
  {
    // not cumulative, check for existing same spell effect in list
    if (GetFirstEffectAttributeMod(data.affectedAttr_IFKEY) != NULL)
    {
      WriteDebugString("NOT Applying spell effect (not cumulative): %s\n", 
        data.AffectedAttr());
      return FALSE;
    }
  }

  if (data.flags & SPELL_EFFECTS_DATA::EFFECT_REMOVEALL)
  {
    // remove all that match
    POSITION pos = GetFirstEffectAttributeMod(data.affectedAttr_IFKEY);
    while (pos != NULL)
    {
      POSITION tmp = pos;
      GetNextEffectAttributeMod(data.affectedAttr_IFKEY, pos);

      // can't remove character special abilities, since they are intrinsic
      if ((m_spellEffects.PeekAt(tmp)->flags & SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB) == 0)
      {        
        //SPELL_DATA *pSpell = spellData.GetSpellData(m_spellEffects.GetAt(tmp).SourceSpell_ID());
        //SPELL_DATA *pSpell = spellData.GetSpell(m_spellEffects.PeekAt(tmp)->SourceSpell_ID());
        m_spellEffects.RemoveAt(tmp);
        
      /* 20141229  Version 1.0303  PRS.
       * I want the SpellEndScript to match up one-to-one with the SpellBeginScript.
       * As it was, we ran the Begin Script once for each target the spell attacked.
       * Then we ran the End Script for each spell effect that was removed.  The result
       * was that the Begin Script might run without any End Script or one Begin
       * Script might result in several End Scripts.  Moreover, the spell effects might
       * end at a much later time when the state of the caster could easily have
       * undergone dramtic changes.
       *
       * So.....Now we run the Begin Script, apply any effects to the target, and
       * immediately run the End Script.  One-for-one in the same instant of time.
       *  if (pSpell!=NULL)
       *  {
       *    Not Implemented(0xd432c0, FALSE);//pSpell->specAbs.DisableAllFor(this);
       *
       *    ActorType actor;
       *    GetContext(&actor);
       *    SetCharContext(&actor);
       *    SetTargetContext(&actor);
       *
       *    pSpell->ExecuteSpellEndScript(NULL, this);
       *
       *    ClearCharContext();
       *    ClearTargetContext();    
       *  }   
       */
      }
    }
    DisplayCurrSpellEffects("AddSpellEffect");
    return TRUE;
  }  

  //SPELL_DATA *pSdata = spellData.GetSpellData(data.SourceSpell_ID());
 SPELL_DATA *pSdata = spellData.GetSpell(data.SourceSpell_ID());
  if (pSdata==NULL) 
  {
    die(0xab505);
    WriteDebugString("NOT Applying spell effect (spell key bogus): %s, delta %f, cumulative %i\n", 
      data.AffectedAttr(), data.changeResult, (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
    return FALSE;
  }

  // must be permanent...we cannot have lingering spell effects
  // add additional lingering spell effects.
  const bool isPerm = true;
  // bool isPerm = (pSdata->Duration_Rate == Permanent);

  int tmp = delta; // drop any fractional parts
  delta = tmp;
  data.changeResult = delta;

  if (!isPerm)
  {
    // if not permanent, store in spell effects list
    WriteDebugString("Adding non-perm spell effect to %s: %s, delta %f, cumulative %i\n", 
                     GetName(),
                     data.AffectedAttr(), 
                     data.changeResult, 
                     (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));

    m_spellEffects.AddTail(data);
  }
  else // else apply effects to attribute now
  {   
    // invalid attribute string
    if (data.AffectedAttr().IsEmpty())
      return FALSE;

    ActorType actor;
    GetContext(&actor);

    // determine which attribute is modified and
    // get delta value
    //
    double val=0.0;
    DWORD attKey = KeywordTextToNum(data.AffectedAttr());
    BOOL IsVirtTrait = IsVirtualTrait(attKey);
    
    if (!IsVirtTrait)
    {
      // modify actual attribute value now
      BYTE attSize = GetDataSize(attKey);
      switch (attSize)
      {
      case IF_TYPE_STRING:
        // no spell affects for string types
        return FALSE;
        break;
      case IF_TYPE_BYTE:   val = GetDataBYTE(attKey,&actor, comment);   break;
      case IF_TYPE_WORD:   val = GetDataWORD(attKey,&actor);   break;
      case IF_TYPE_DWORD:  val = GetDataDWORD(attKey,&actor);  break;
      case IF_TYPE_FLOAT:  val = GetDataFLOAT(attKey,&actor);  break;
      case IF_TYPE_UBYTE:  val = GetDataUBYTE(attKey,&actor);  break;
      case IF_TYPE_UWORD:  val = GetDataUWORD(attKey,&actor);  break;
      case IF_TYPE_UDWORD: val = GetDataUDWORD(attKey,&actor); break;
      default: die(0xab506); return FALSE; break;
      }

      // apply delta value to affected attribute
      //val += delta;


// Call the ComputeSpellDamage hook to modify any changes to the targets attributes.
      {
        CString result, message;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        SPELL_DATA *pSpell=pSdata;
        hookParameters[5].Format("%d",int(data.changeResult));
        hookParameters[6]= data.AffectedAttr();
        scriptContext.SetAttackerContext(pAttacker);
        scriptContext.SetTargetContext(this);
        scriptContext.SetSpellContext(pSpell);

        result = pSpell->RunSpellScripts(
                COMPUTE_SPELL_DAMAGE,
                ScriptCallback_RunAllScripts,
                NULL,
                "Lingering spell effect - Compute damage");
        if (!result.IsEmpty())
        {
          int temp;
          if (sscanf(result,"%d",&temp) == 1) 
          {
            data.changeResult = temp;
          };
        };
        if (this->GetType() == MONSTER_TYPE)
        {
          //int monsterIndex;
          COMBATANT *pCombatant;
          pCombatant = this->m_pCombatant;
          if (pCombatant != NULL)
          {
            //monsterIndex = pCombatant->origKey;
            {
              //MONSTER_DATA *pMonsterData;
              const MONSTER_DATA *pMonsterData;
              pMonsterData = monsterData.PeekMonster(monsterID);
              if (pMonsterData != NULL)
              {
                result = pMonsterData->RunMonsterScripts(
                        COMPUTE_SPELL_DAMAGE,
                        ScriptCallback_RunAllScripts,
                        NULL,
                        "Lingering spell effect - Compute damage");
                if (!result.IsEmpty())
                {
                  int temp;
                  if (sscanf(result,"%d",&temp) == 1) 
                  {
                    data.changeResult = temp;
                  };
                };
              };
            };
          };
        };
        if (this->m_pCombatant != NULL)
        {
          result = this->m_pCombatant->RunCombatantScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL,
                  "Lingering spell effect - Compute damage");
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
        //message = hookParameters[7];
        {
          result = pAttacker->RunCharacterScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL,
                  "Lingering spell effect - Compute damage");
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
        {
          result = this->RunCharacterScripts(
                  COMPUTE_SPELL_DAMAGE,
                  ScriptCallback_RunAllScripts,
                  NULL,
                  "Lingering spell effect - Compute damage");
          if (!result.IsEmpty())
          {
            int temp;
            if (sscanf(result,"%d",&temp) == 1) 
            {
              data.changeResult = temp;
            };
          };
        };
      };

      val = data.ApplyChange(val); // might be delta, percentage, or absolute change

      actor.m_canFinishCasting = false;

      switch (attSize)
      {
      case IF_TYPE_BYTE:   SetDataBYTE(attKey, val,&actor);   break;
      case IF_TYPE_WORD:   SetDataWORD(attKey, val,&actor);   break;
      case IF_TYPE_DWORD:  SetDataDWORD(attKey, val,&actor);  break;
      case IF_TYPE_FLOAT:  SetDataFLOAT(attKey, val,&actor);  break;
      case IF_TYPE_UBYTE:  SetDataUBYTE(attKey, val,&actor);  break;
      case IF_TYPE_UWORD:  SetDataUWORD(attKey, val,&actor);  break;
      case IF_TYPE_UDWORD: SetDataUDWORD(attKey, val,&actor); break;
      default: die(0xab507); break;
      }
      if (logDebuggingInfo)
      {
        WriteDebugString("Applying perm spell effect to %s: %s, delta %f, result %f, cumulative %i\n",
          GetName(),
          data.AffectedAttr(),
          delta,
          val,
          (data.flags&SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE) ? 1 : 0);
      };
    }
    else
    {
      // We no longer implement 'Virtual' traits.
      // This operation will become a 'No-Op'.
      /*
       * // there is no CHARACTER value that represents this attribute, so
       * // we need to store it in the spellEffects list
       *
       * // first find out if same attribute exists in list of active
       * // then either apply mods to existing or add this new one
       * //
       * // only one such attribute should ever exist in the list of actives
       * //
       * 
       * POSITION pos = GetFirstEffectAttributeMod(data.affectedAttr);
       * if (pos != NULL)
       * {
       *   // get old attribute data
       *  SPELL_EFFECTS_DATA &olddata = m_spellEffects.GetAt(pos);
       *  // apply new modification to old mod value
       *  data.changeResult = data.ApplyChange(olddata.changeResult);
       *  // update old data
       *  m_spellEffects.GetAt(pos) = data;
       *  
       *  WriteDebugString("Updating perm virtual spell effect for %s: %s, delta %f, cumulative %i\n", 
       *                   GetName(),
       *                   data.affectedAttr, 
       *                   data.changeResult, 
       *                   (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));        
       * }
       * else
       * {
       *  WriteDebugString("Adding perm virtual spell effect for %s: %s, delta %f, cumulative %i\n", 
       *                   GetName(),
       *                   data.affectedAttr, 
       *                   data.changeResult, 
       *                   (data.flags & SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE));
       *  
       *  m_spellEffects.AddTail(data);
       * }
       */
    }

    // transfer spell attributes to character for permanent spell
    CString sakey;
    CString savalue;    
    POSITION pos = pSdata->spell_asl.GetStartPosition();
    while (pos != NULL)
    {      
      const ASLENTRY *pEntry;
      pSdata->spell_asl.GetNextAssoc(pos, &pEntry);
      // adds new value if doesn't exist, else updates existing
      char_asl.Insert(pEntry->Key(), pEntry->Value(), ASLF_MODIFIED);
    }
  }

  DisplayCurrSpellEffects("AddSpellEffect");
  return TRUE;
}











//*****************************************************************************
//    NAME: CHARACTER::ApplySpellEffectAdjustments
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ApplySpellEffectAdjustments(DWORD /*flags*/, IF_KEYWORD_INDEX key, int &val) const
{
  //AutoSetActor ctx(*this);
  POSITION pos = GetFirstEffectAttributeMod(key);
  while (pos != NULL)
  {
    const SPELL_EFFECTS_DATA *pData = m_spellEffects.PeekAt(pos);
    double src = val;
    // GetChange() should have been called at least once before now
    //if ((pData->flags & SPELL_EFFECTS_DATA::EFFECT_ONCEONLY) == 0)
    {
      // Temporary effect (otherwise a permanent effect applied periodically)
      val = pData->ApplyChange(src); // return accumulated delta
    };
    //WriteDebugString("ApplySpellEffectAdj(%f) = %f for %s\n", src, val, akey);    
    pos = GetNextEffectAttributeMod(key, pos);
  }
}


//*****************************************************************************
//    NAME: CHARACTER::AdjustEffectData
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::AdjustEffectData(const char *akey, int val)
void CHARACTER::AdjustEffectData(const IF_KEYWORD_INDEX key, int val)
{
  // adjust only the first effect that matches
  POSITION pos = GetFirstEffectAttributeMod(key);
  if (pos == NULL)
    return;

  SPELL_EFFECTS_DATA &data = *m_spellEffects.GetAt(pos);
  WriteDebugString("Adjusting data for effect %s by %i for %a\n", data.AffectedAttr(), val, name);


  data.data += val;

  bool RemoveSpell = false;
  die("Not Needed?"); //Not Implemented(0xcccd54,FALSE);//specialAbilitiesType sa = ConvertRuntimeIfTextToSpecAb(akey);


  die("Not Needed?"); //Not Implemented(0x5c9bf6, FALSE);
  if (RemoveSpell)
  {    
    ACTIVE_SPELL *pSpell = activeSpellList.Get(data.parent);
    if (pSpell != NULL)
    {
      // if this is the only effect for this spell, might as well expire it
      // otherwise, wait for normal spell duration
      //SPELL_DATA *pSpellData = spellData.GetSpellData(pSpell->spellKey);
      SPELL_DATA *pSpellData = spellData.GetSpell(pSpell->spellID);
      if (pSpellData != NULL)
      {
        WriteDebugString("Marking spell %s from %s in AdjustEffectData() for removal\n", pSpellData->Name, name);
        if (pSpellData->m_EffectedAttributes.GetCount() == 1)
          pSpell->StopTime = 0; // force spell to expire
      }
      else
      {
        pSpell->StopTime = 0; // force spell to expire
        WriteDebugString("Marking spell from %s in AdjustEffectData() for removal\n", name);
      }
    }
  }
}
#endif // engine-only magic code

//*****************************************************************************
//    NAME: CHARACTER::GetAdjTHAC0
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjTHAC0(DWORD flags) const
{  
#ifdef UAFEDITOR
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;
  int tmp = GetTHAC0();
  //tmp -= GetAdjHitBonus(giID, 0); // subtract strength bonus from base THAC0
  tmp -= GetAdjHitBonus(itemID, 0); // subtract strength bonus from base THAC0

  int wpn = myItems.GetReadiedItem(WeaponHand, 0);
  if (wpn != NO_READY_ITEM)
  {  
    //GLOBAL_ITEM_ID giID = myItems.GetItem(wpn);
    ITEM_ID wpnID = myItems.GetItem(wpn);
    //ITEM_DATA *pData = itemData.GetItem(giID);
    ITEM_DATA *pData = itemData.GetItem(wpnID);
    if (pData != NULL)
      tmp -= pData->Attack_Bonus; // subtract weapon attack bonus
  }

  return tmp;

#else

  //const char akey[]="$CHAR_THAC0";
  const IF_KEYWORD_INDEX key=CHAR_THAC0;
  
  int val = GetTHAC0();
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;

  //int wpn = myItems.GetReady(WeaponHand);
  int wpn = myItems.GetReadiedItem(WeaponHand, 0);
  //giID = myItems.GetItem(wpn);
  itemID = myItems.GetItem(wpn);
  //val -= GetAdjHitBonus(giID, 0); // subtract strength bonus from base THAC0
  val -= GetAdjHitBonus(itemID, 0); // subtract strength bonus from base THAC0
  if (wpn != NO_READY_ITEM)
  {  
    //ITEM_DATA *pData = itemData.GetItem(giID);
    ITEM_DATA *pData = itemData.GetItem(itemID);
    if (pData != NULL)
      val -= pData->Attack_Bonus; // subtract weapon attack bonus
  }

  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = min(MAX_THAC0,val);
  val = max(MIN_THAC0,val);
  return val;

#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjAC
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjAC(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetBaseAC();
#else
  //const char akey[]="$CHAR_AC";
  const IF_KEYWORD_INDEX key=CHAR_AC;
  int val = GetBaseAC();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = min(MAX_AC,val);
  val = max(MIN_AC,val);
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetEffectiveAC
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetEffectiveAC(void) const
{
#ifdef UAFEDITOR
  return GetBaseAC();
#else
  int val = GetBaseAC();
  val += myItems.GetProtectModForRdyItems();
  val = min(MAX_AC,val);
  val = max(MIN_AC,val);
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjHitPoints
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjHitPoints(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetHitPoints();
#else
  //const char akey[]="$CHAR_HITPOINTS";
  const IF_KEYWORD_INDEX key=CHAR_HITPOINTS;
  int val = GetHitPoints();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(-10,val);
  val = min(val, GetMaxHitPoints());
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjReadyToTrain
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::GetAdjReadyToTrain(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetReadyToTrain();
#else
  //const char akey[]="$CHAR_RDYTOTRAIN";
  const IF_KEYWORD_INDEX key=CHAR_RDYTOTRAIN;
  int val = GetReadyToTrain();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(FALSE,val);
  val = min(val,TRUE);
  return val;
#endif
}
/*
//*****************************************************************************
//    NAME: CHARACTER::GetAdjClericExp
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjClericExp(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetClericExp();
#else
  const char akey[]="$CHAR_CLERICEXP";
  double val = GetClericExp();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0,val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjFighterExp
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjFighterExp(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetFighterExp();
#else
  const char akey[]="$CHAR_FIGHTEREXP";
  double val = GetFighterExp();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0,val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjMagicUserExp
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjMagicUserExp(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetMagicUserExp();
#else
  const char akey[]="$CHAR_MAGICUSEREXP";
  double val = GetMagicUserExp();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0,val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjThiefExp
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjThiefExp(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetThiefExp();
#else
  const char akey[]="$CHAR_THIEFEXP";
  double val = GetThiefExp();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0,val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjRangerExp
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjRangerExp(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetRangerExp();
#else
  const char akey[]="$CHAR_RANGEREXP";
  double val = GetRangerExp();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0,val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjPaladinExp
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjPaladinExp(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPaladinExp();
#else
  const char akey[]="$CHAR_PALADINEXP";
  double val = GetPaladinExp();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0,val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjDruidExp
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjDruidExp(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetDruidExp();
#else
  const char akey[]="$CHAR_DRUIDEXP";
  double val = GetDruidExp();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0,val);
  return val;
#endif
}
*/
//*****************************************************************************
//    NAME: CHARACTER::GetAdjAge
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjAge(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetAge();
#else
  //const char akey[]="$CHAR_AGE";
  const IF_KEYWORD_INDEX key=CHAR_AGE;
  int val = GetAge();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(1,val);
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjMaxAge
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjMaxAge(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetMaxAge();
#else
  //const char akey[]="$CHAR_MAXAGE";
  const IF_KEYWORD_INDEX key=CHAR_MAXAGE;
  int val = GetMaxAge();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(1,val);
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjMaxMovement
//
// PURPOSE: 
//
//*****************************************************************************
BYTE CHARACTER::GetAdjMaxMovement(DWORD flags, LPCTSTR comment) const
{
#ifdef UAFEDITOR
  return GetMaxMovement();
#else
  
  if (IsCombatActive())
  {
    {
      ActorType actor;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      int minmax[2];
      const COMBATANT *pCombatant;
      minmax[0] = -1;
      minmax[1] = 0x7fffffff;
      GetContext(&actor);
      SetCharContext(&actor);
      hookParameters[5].Format("%i", GetMaxMovement());
      scriptContext.SetCharacterContext(this);
      // Removed 20101228 PRS....sometimes called outside of combat!!! Like taking treasure.
      //pCombatant = this->GetCombatant();
      //scriptContext.SetCombatantContext(pCombatant);
      RunCharacterScripts(       GET_ADJ_MAX_MOVEMENT, 
                                 ScriptCallback_MinMax, 
                         (void *)minmax,
                                 comment);
#ifdef newCombatant
      pCombatant = m_pCombatant;
#else
      pCombatant = GetCombatant();
#endif
      if (pCombatant != NULL)
      {
        pCombatant->RunCombatantScripts(GET_ADJ_MAX_MOVEMENT, 
                                        ScriptCallback_MinMax, 
                                        (void *)minmax, 
                                        comment);
      };
      scriptContext.Clear();
      ClearCharContext();
      if ((minmax[0] == -1) && (minmax[1] == 0x7fffffff) ) return GetMaxMovement();
      else return minmax[1];
    }
/*
    if (GetAdjSpecAb(SA_Entangle,&src,&skey))
    {
      QueueUsedSpecAb(SA_Entangle,src,skey);
      return 0;
    }

    if (GetAdjSpecAb(SA_HoldPerson,&src,&skey))
    {
      QueueUsedSpecAb(SA_HoldPerson,src,skey);
      return 0;
    }

    if (GetAdjSpecAb(SA_Sleep,&src,&skey))
    {
      QueueUsedSpecAb(SA_Sleep,src,skey);
      return 0;
    }
    
    const char akey[]="$CHAR_MAXMOVE";
    double val = GetMaxMovement();
    ApplySpellEffectAdjustments(flags, akey, val);
    val = max(0,val);
    return val;    
    */
  }
  else
  {
    //const char akey[]="$CHAR_MAXMOVE";
    const IF_KEYWORD_INDEX key=CHAR_MAXMOVE;
    int val = GetMaxMovement();
    //ApplySpellEffectAdjustments(flags, akey, val);
    ApplySpellEffectAdjustments(flags, key, val);
    val = max(0,val);
    return val;
  }
#endif
}

#ifdef complexAbilities
void ABILITY_ADJUSTMENT::operator =(const ABILITY_ADJUSTMENT& src)
{
  Not Implemented(0xd41, false);
}

CHARACTER::ABILITY_SCORE::ABILITY_SCORE(void)
{
  totalAdjustments = 0;
  permanentValue = 0;
}


void CHARACTER::ABILITY_SCORE::operator =(const CHARACTER::ABILITY_SCORE& src)
{
  permanentValue = src.permanentValue;
  totalAdjustments = src.totalAdjustments;
  adjustments.RemoveAll();
  adjustments.Append(src.adjustments);
}


void CHARACTER::AbilityScores::operator =(const CHARACTER::AbilityScores& src)
{
  strength     = src.strength;
	strengthMod  = src.strengthMod;
	intelligence = src.intelligence;
	wisdom       = src.wisdom;
	dexterity    = src.dexterity;
	constitution = src.constitution;
	charisma     = src.charisma;
}

void CHARACTER::ABILITY_SCORE::Clear(void)
{
  int i, n;
  n = GetAbilityCount();
  for (i=0; i<n; i++)
  {
    Not Implemented(0x41cb, false);
  };
  totalAdjustments = 0;
}

int CHARACTER::ABILITY_SCORE::GetPermValue(void) const
{
  return permanentValue;
}
int CHARACTER::ABILITY_SCORE::GetAdjValue(void) const
{
  return permanentValue + totalAdjustments;
}
void CHARACTER::ABILITY_SCORE::SetPermValue(int v)
{
  permanentValue = v;
}

void CHARACTER::AbilityScores::Serialize(CAR& car, double version)
{
  Not Implemented(0xfa7, false);
}

#endif

static int LimitAb(int v, int min, int max)
{
  return min(max(v,min), max);
}


//*****************************************************************************
//    NAME: CHARACTER::GetAdjStr
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetPermStr(void) const
{
  return strength;
}
int CHARACTER::GetAdjStr(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPermStr();
#else
  //if (GetAdjSpecAb(SA_Enfeebled))
  //{
  //  return (min(GetStr(), 5));
  //}
  //else
  if (strength_adjustment == 0x7fffffff)
  {
    strength_adjustment = 0;
    //ApplySpellEffectAdjustments(flags, "$CHAR_STR", strength_adjustment);
    ApplySpellEffectAdjustments(flags, CHAR_ADJUSTED_STR, strength_adjustment);
  };
  return strength + strength_adjustment;
#endif
}
int CHARACTER::GetLimitedStr(void) const
{
  return LimitAb(GetAdjStr(), MIN_STRENGTH, MAX_STRENGTH);
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjStrMod
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetPermStrMod(void) const
{
  return strengthMod;
}
int CHARACTER::GetAdjStrMod(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPermStrMod();
#else
  if (strengthMod_adjustment == 0x7fffffff)
  {
    strengthMod_adjustment = 0;
    //ApplySpellEffectAdjustments(flags, "$CHAR_STRDOD", strengthMod_adjustment);
    ApplySpellEffectAdjustments(flags, CHAR_ADJUSTED_STRMOD, strengthMod_adjustment);
  };
  return strengthMod + strengthMod_adjustment;
#endif
}
int CHARACTER::GetLimitedStrMod(void) const
{
  return LimitAb(GetAdjStrMod(), MIN_STRENGTHMOD, MAX_STRENGTHMOD);
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjInt
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetPermInt(void) const
{
  return intelligence;
}
int CHARACTER::GetAdjInt(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPermInt();
#else
  if (intelligence_adjustment == 0x7fffffff)
  {
    intelligence_adjustment = 0;
    //ApplySpellEffectAdjustments(flags, "$CHAR_INT", intelligence_adjustment);
    ApplySpellEffectAdjustments(flags, CHAR_ADJUSTED_INT, intelligence_adjustment);
  };
  return intelligence + intelligence_adjustment;
#endif
}
int CHARACTER::GetLimitedInt(void) const
{
  return LimitAb(GetAdjInt(), MIN_INTELLIGENCE, MAX_INTELLIGENCE);
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjWis
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetPermWis(void) const
{
  return wisdom;
}
int CHARACTER::GetAdjWis(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPermWis();
#else
  if (wisdom_adjustment == 0x7fffffff)
  {
    wisdom_adjustment = 0;
    //ApplySpellEffectAdjustments(flags, "$CHAR_WIS", wisdom_adjustment);
    ApplySpellEffectAdjustments(flags, CHAR_ADJUSTED_WIS, wisdom_adjustment);
  };
  return wisdom + wisdom_adjustment;
#endif
}
int CHARACTER::GetLimitedWis(void) const
{
  return LimitAb(GetAdjWis(), MIN_WISDOM, MAX_WISDOM);
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjDex
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetPermDex(void) const
{
  return dexterity;
}
int CHARACTER::GetAdjDex(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPermDex();
#else
  if (dexterity_adjustment == 0x7fffffff)
  {
    dexterity_adjustment = 0;
    //ApplySpellEffectAdjustments(flags, "$CHAR_DEX", dexterity_adjustment);
    ApplySpellEffectAdjustments(flags, CHAR_ADJUSTED_DEX, dexterity_adjustment);
  };
  return dexterity + dexterity_adjustment;
#endif
}
int CHARACTER::GetLimitedDex(void) const
{
  return LimitAb(GetAdjDex(), MIN_DEXTERITY, MAX_DEXTERITY);
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjCon
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetPermCon(void) const
{
  return constitution;
}
int CHARACTER::GetAdjCon(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPermCon();
#else
  if (constitution_adjustment == 0x7fffffff)
  {
    constitution_adjustment = 0;
    //ApplySpellEffectAdjustments(flags, "$CHAR_CON", constitution_adjustment);
    ApplySpellEffectAdjustments(flags, CHAR_ADJUSTED_CON, constitution_adjustment);
  };
  return constitution + constitution_adjustment;
#endif
}
int CHARACTER::GetLimitedCon(void) const
{
  return LimitAb(GetAdjCon(), MIN_CONSTITUTION, MAX_CONSTITUTION);
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjCha
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetPermCha(void) const
{
  return charisma;
}
int CHARACTER::GetAdjCha(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPermCha();
#else
  if (charisma_adjustment == 0x7fffffff)
  {
    charisma_adjustment = 0;
    ApplySpellEffectAdjustments(flags, CHAR_ADJUSTED_CHA, charisma_adjustment);
  };
  return charisma + charisma_adjustment;
#endif
}
int CHARACTER::GetLimitedCha(void) const
{
  return LimitAb(GetAdjCha(), MIN_CHARISMA, MAX_CHARISMA);
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjMaxEncumbrance
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjMaxEncumbrance(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetMaxEncumbrance();
#else
  //const char akey[]="$CHAR_MAXENC";
  const IF_KEYWORD_INDEX key=CHAR_MAXENC;
  int val = GetMaxEncumbrance();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(1,val);
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjGender
//
// PURPOSE: 
//
//*****************************************************************************
genderType CHARACTER::GetAdjGender(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetGender();
#else
  //const char akey[]="$CHAR_GENDER";
  const IF_KEYWORD_INDEX key=CHAR_GENDER;
  int val = (double)((int)GetGender());
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  if ((val < 0)||(val >= NUM_GENDER_TYPES))
    val = (double)((int)GetGender());
  return ((genderType)((int)val));
#endif
}


//*****************************************************************************
//    NAME: CHARACTER::GetAdjClass
//
// PURPOSE: 
//
//*****************************************************************************
//classType CHARACTER::GetAdjClass(DWORD flags) const
CLASS_ID CHARACTER::GetAdjClass(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetClass();
#else
  //const char akey[]="$CHAR_CLASS";
  //double val = (double)((int)GetClass());
  //ApplySpellEffectAdjustments(flags, akey, val);
  //if ((val < 0)||(val >= NUM_CLASS_TYPES))
  //  val = (double)((int)GetClass());
  //return ((classType)((int)val));
  return GetClass();
#endif
}


//*****************************************************************************
//    NAME: CHARACTER::GetAdjAlignment
//
// PURPOSE: 
//
//*****************************************************************************
alignmentType CHARACTER::GetAdjAlignment(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetAlignment();
#else
  //const char akey[]="$CHAR_ALIGNMENT";
  const IF_KEYWORD_INDEX key=CHAR_ALIGNMENT;
  int val = (double)((int)GetAlignment());
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  if ((val < 0)||(val >= NUM_ALIGNMENT_TYPES))
    val = (double)((int)GetAlignment());
  return ((alignmentType)((int)val));
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::IsAlignmentEvil
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsAlignmentEvil() const
{
  switch (GetAdjAlignment())
  {
  case LawfulEvil:
  case NeutralEvil:
  case ChaoticEvil:
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: CHARACTER::IsAlignmentGood
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsAlignmentGood() const
{
  switch (GetAdjAlignment())
  {
  case LawfulGood:
  case NeutralGood:
  case ChaoticGood:
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjStatus
//
// PURPOSE: 
//
//*****************************************************************************
charStatusType CHARACTER::GetAdjStatus(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetStatus();
#else
  //const char akey[]="$CHAR_STATUS";
  const IF_KEYWORD_INDEX key=CHAR_STATUS;
  int val = (double)((int)GetStatus());
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  if ((val < 0)||(val >= NUM_CHAR_STATUS_TYPES))
    val = (double)((int)GetStatus());
  return ((charStatusType)((int)val));
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjUndeadType
//
// PURPOSE: 
//
//*****************************************************************************
//undeadType CHARACTER::GetAdjUndeadType(DWORD flags) const
//{
//#ifdef UAFEDITOR
//  return GetUndeadType();
//#else
//  const char akey[]="$CHAR_UNDEAD";
//  double val = (double)((int)GetUndeadType());
//  ApplySpellEffectAdjustments(flags, akey, val);
//  if ((val < 0)||(val >= NUM_UNDEAD_TYPES))
//    val = (double)((int)GetUndeadType());
//  return ((undeadType)((int)val));
//#endif
//}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjSize
//
// PURPOSE: 
//
//*****************************************************************************
creatureSizeType CHARACTER::GetAdjSize(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetSize();
#else
  //const char akey[]="$CHAR_SIZE";
  const IF_KEYWORD_INDEX key=CHAR_SIZE;
  int val = (double)((int)GetSize());
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  if ((val < 0)||(val >= NUM_SIZE_TYPES))
    val = (double)((int)GetSize());
  // lookup sa's Enlarge and Reduce
  //enum creatureSizeType { Small, Medium, Large };
  if (GetAdjSpecAb(SA_Enlarge))
    val++;
  if (GetAdjSpecAb(SA_Reduce))
    val--;

  if (val < Small) val=Small;
  if (val > Large) val=Large;

  return ((creatureSizeType)((int)val));
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjMagicResistance
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjMagicResistance(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetMagicResistance();
#else
  //const char akey[]="$CHAR_MAGICRESIST";
  const IF_KEYWORD_INDEX key=CHAR_MAGICRESIST;
  int val = GetMagicResistance();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(0,val);
  val = min(100,val);
  return val;
#endif
}


//*****************************************************************************
//    NAME: CHARACTER::GetAdjHitBonus
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::GetAdjHitBonus(GLOBAL_ITEM_ID weaponID, int distance, DWORD flags) const
int CHARACTER::GetAdjHitBonus(const ITEM_ID& weaponID, int distance, DWORD flags) const
{
#ifdef UAFEDITOR
  return GetHitBonus();
#else
  //const char akey[]="$CHAR_HITBONUS";
  const IF_KEYWORD_INDEX key=CHAR_HITBONUS;
  int val;

  weaponClassType weaponType;

  //weaponType = itemData.GetWpnType(weaponID);
  weaponType = itemData.GetWpnType(weaponID);
  switch (weaponType)
  {
    case NotWeapon:
    case HandBlunt:
    case HandCutting:
      val = GetHitBonus();
      break;
    case HandThrow:
      val = (distance == 0)?GetHitBonus():0;
      break;
    case SlingNoAmmo:
    case Bow:
    case Crossbow:
    case Throw:
      val = 0;
      break;
    case Ammo:
      val = 0;
      break;
    case SpellCaster:
    case SpellLikeAbility:
      val = 0;
      break;
  };

  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjDmgBonus
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjDmgBonus(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetDmgBonus();
#else
  //const char akey[]="$CHAR_DAMAGEBONUS";
  const IF_KEYWORD_INDEX key=CHAR_DAMAGEBONUS;
  int val = GetDmgBonus();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjMorale
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjMorale(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetMorale();
#else
  //const char akey[]="$CHAR_MORALE";
  const IF_KEYWORD_INDEX key=CHAR_MORALE;
  int val = GetMorale();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(0, val);
  val = min(100, val);
  return val;
#endif
}

/*
//*****************************************************************************
//    NAME: CHARACTER::GetAdjOpenDoors
//
// PURPOSE: 
//
//*****************************************************************************
BYTE CHARACTER::GetAdjOpenDoors(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetOpenDoors();
#else
  const char akey[]="$CHAR_OPENDOORS";
  double val = GetOpenDoors();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjOpenMagicDoors
//
// PURPOSE: 
//
//*****************************************************************************
BYTE CHARACTER::GetAdjOpenMagicDoors(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetOpenMagicDoors();
#else
  const char akey[]="$CHAR_OPENMAGICDOORS";
  double val = GetOpenMagicDoors();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjBendBarsLiftGates
//
// PURPOSE: 
//
//*****************************************************************************
BYTE CHARACTER::GetAdjBendBarsLiftGates(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetBendBarsLiftGates();
#else
  const char akey[]="$CHAR_BENDLIFT";
  double val = GetBendBarsLiftGates();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(0, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjPickPockets
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjPickPockets(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetPickPockets();
#else
  const char akey[]="$CHAR_PICKPOCKETS";
  double val = GetPickPockets();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjOpenLocks
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjOpenLocks(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetOpenLocks();
#else
  const char akey[]="$CHAR_OPENLOCKS";
  double val = GetOpenLocks();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjFindTraps
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjFindTraps(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetFindTraps();
#else
  const char akey[]="$CHAR_FINDTRAPS";
  double val = GetFindTraps();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjMoveSilent
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjMoveSilent(DWORD flags) const
0{
#ifdef UAFEDITOR
  return GetMoveSilent();
#else
  const char akey[]="$CHAR_MOVESILENT";
  double val = GetMoveSilent();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjHideInShadows
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjHideInShadows(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetHideInShadows();
#else
  const char akey[]="$CHAR_HIDESHADOWS";
  double val = GetHideInShadows();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjHearNoise
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjHearNoise(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetHearNoise();
#else
  const char akey[]="$CHAR_HEARNOISE";
  double val = GetHearNoise();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjClimbWalls
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjClimbWalls(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetClimbWalls();
#else
  const char akey[]="$CHAR_CLIMBWALLS";
  double val = GetClimbWalls();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjReadLanguages
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetAdjReadLanguages(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetReadLanguages();
#else
  const char akey[]="$CHAR_READLANG";
  double val = GetReadLanguages();
  ApplySpellEffectAdjustments(flags, akey, val);
  val = max(-100, val);
  val = min(100, val);
  return val;
#endif
}

*/
//*****************************************************************************
//    NAME: CHARACTER::GetAdjAllowPlayerControl
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::GetAdjAllowPlayerControl(DWORD flags)
{
#ifdef UAFEDITOR
  return GetAllowPlayerControl();
#else
  //const char akey[]="$CHAR_ALLOWPLAYERCONTROL";
  const IF_KEYWORD_INDEX key=CHAR_ALLOWPLAYERCONTROL;
  int val = GetAllowPlayerControl();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(0, val);
  val = min(1, val);

  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjAutomatic
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::GetAdjAutomatic(DWORD flags)
{
  // if player control not allowed, then automatic is TRUE
  if (!GetAdjAllowPlayerControl(flags)) return TRUE;
  return GetAutomatic();
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjDetectingInvisible
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::GetAdjDetectingInvisible(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetDetectingInvisible();
#else
  //const char akey[]="$CHAR_DETECTINVISIBLE";
  const IF_KEYWORD_INDEX key=CHAR_DETECTINVISIBLE;
  int val = GetDetectingInvisible();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(0, val);
  val = min(1, val);
  return val;
#endif
}
//*****************************************************************************
//    NAME: CHARACTER::GetAdjDetectingTraps
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::GetAdjDetectingTraps(DWORD flags) const
{
#ifdef UAFEDITOR
  return GetDetectingTraps();
#else
  //const char akey[]="$CHAR_DETECTTRAPS";
  const IF_KEYWORD_INDEX key=CHAR_DETECTTRAPS;
  int val = GetDetectingTraps();
  //ApplySpellEffectAdjustments(flags, akey, val);
  ApplySpellEffectAdjustments(flags, key, val);
  val = max(0, val);
  val = min(1, val);
  return val;
#endif
}

//*****************************************************************************
//    NAME: CHARACTER::GetAdjSpecAb
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::GetAdjSpecAb(specialAbilitiesType sa, DWORD *pSource, GLOBAL_SPELL_ID *pSrcKey) const
int CHARACTER::GetAdjSpecAb(specialAbilitiesType sa, DWORD *pSource, CString *pSpellName) const
{
  if (pSource!=NULL) *pSource=0;
  //if (pSrcKey!=NULL) pSrcKey->Clear();
  if (pSpellName!=NULL) pSpellName->Empty();

  if (specAbs.FindAbility(sa) == NULL)
  {
//    ASS ERT(FALSE);
    return 0;
  }

#ifdef UAFEDITOR
  return 0;
#else
  CString akey;
  akey = ConvertSpecAbToRuntimeIfText((specialAbilitiesType)sa);
  if (akey.GetLength()==0) return 0;

  double val = 0.0;
  die ("Not Needed?"); //Not Implemented(0x4f6da8, FALSE);
  return val;
#endif
}


//*****************************************************************************
//    NAME: CHARACTER::SetSpecAb
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetSpecAb(specialAbilitiesType sa, BYTE enable, DWORD flags)
{
  // flags should specify the source of this ability (item, char/monster, spell)

  if (specialAbilitiesData.FindAbility(sa) == NULL)
  {
    die(0xab508);
    return;
  }

  die("Not Needed?"); //Not Implemented(0x3aee,FALSE);
/*
  if (strcmp(sa, spellAbilitiesText[(int)SA_Poisoned]) == 0)
  {
    if (HasPoisonImmunity()) return;
  } 
  else if (   (strcmp(sa, spellAbilitiesText[(int)SA_HoldPerson]) == 0) 
           || (strcmp(sa, spellAbilitiesText[(int)SA_CharmPerson]) == 0))
  {
    if (!CanBeHeldOrCharmed()) return;
  }
*/

  // cannot stack special abilities
  // if already have this one, just return
  if (GetAdjSpecAb(sa))
    return;

#ifdef UAFEDITOR
  return;
#else
  CString akey=ConvertSpecAbToRuntimeIfText((specialAbilitiesType)sa);
  if (akey.GetLength()==0) return;

  SPELL_EFFECTS_DATA data;
  data.Clear();
  data.AffectedAttr(akey);
  data.changeData.DecodeText("1");
  data.GetChange();
  data.flags = flags | 
               SPELL_EFFECTS_DATA::EFFECT_CUMULATIVE |
               SPELL_EFFECTS_DATA::EFFECT_TARGET |
               SPELL_EFFECTS_DATA::EFFECT_ABS;

  die("Not Needed?"); //Not Implemented(0x4a2a1a,FALSE);

  if (enable)
    AddSpecAb(data);
  else
    RemoveSpecAb(data);
#endif
}



//*****************************************************************************
//    NAME: CHARACTER::SetHitBonus
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetHitBonus(int val)
{
  hitBonus=val;
}
//*****************************************************************************
//    NAME: CHARACTER::SetDmgBonus
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetDmgBonus(int val)
{
  dmgBonus=val;
}
//*****************************************************************************
//    NAME: CHARACTER::SetGender
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetGender(const CString& newGender) 
{     
  if (newGender.IsEmpty()) return;
  if ((newGender[0] == 'M') || (newGender[0] == 'm')) gender = Male;
  if ((newGender[0] == 'F') || (newGender[0] == 'f')) gender = Female;
}
//*****************************************************************************
//    NAME: CHARACTER::SetName
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetName(const char *val) 
{     
  name = val; 
  name.TrimLeft();
  name.TrimRight();
  if (name.GetLength() > MAX_CHAR_NAME)
    name = name.Left(MAX_CHAR_NAME);
}
//*****************************************************************************
//    NAME: CHARACTER::SetMagicResistance
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetMagicResistance(int val)
{
  if (GetType() != MONSTER_TYPE) val=0;
  val = max(0,val);
  val = min(100,val);
  magicResistance=val;
}

//*****************************************************************************
//    NAME: CHARACTER::SetStr
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetPermStr(int val)
{
  //val = min(25, val);
  //val = max(3, val);
  if (val != GetPermStr())
  {
    spellAbility.valid = FALSE;
    strength=val;
  };
  if (GetAdjStr() < 18) strengthMod = 0;
  UpdateSkillBasedStats(); // spells can change skill stats, so update this data
}
//*****************************************************************************
//    NAME: CHARACTER::SetStrMod
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetPermStrMod(int val)
{
  //if ((!isFighter(GetClass())) && (!isRanger(GetClass())) && (!isPaladin(GetClass())))
  //  val=0;
  //val = min(100, val);
  //val = max(0, val);
  if (val != strengthMod)
  {
    spellAbility.valid = FALSE;
    strengthMod=val;
    UpdateSkillBasedStats();
  };
}
//*****************************************************************************
//    NAME: CHARACTER::SetInt
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetPermInt(int val)
{
  //val = min(25, val);
  //val = max(3, val);
  if (val != intelligence)
  {
    spellAbility.valid = FALSE;
    intelligence=val;
    UpdateSkillBasedStats();
  };
}
//*****************************************************************************
//    NAME: CHARACTER::SetWis
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetPermWis(int val)
{
  //val = min(25, val);
  //val = max(3, val);
  if (val != wisdom)
  {
    spellAbility.valid = FALSE;
    wisdom=val;
    UpdateSkillBasedStats();
  };
}
//*****************************************************************************
//    NAME: CHARACTER::SetDex
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetPermDex(int val)
{
  //val = min(25, val);
  //val = max(3, val);
  if (val != dexterity)
  {
    spellAbility.valid = FALSE;
    dexterity=val;
    UpdateSkillBasedStats();
  };
}
//*****************************************************************************
//    NAME: CHARACTER::SetCon
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetPermCon(int val)
{
  //val = min(25, val);
  //val = max(3, val);
  if (val != constitution)
  {
    spellAbility.valid = FALSE;
    constitution=val;
    UpdateSkillBasedStats();
  };
}
//*****************************************************************************
//    NAME: CHARACTER::SetCha
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetPermCha(int val)
{
  //val = min(25, val);
  //val = max(3, val);
  if (val != charisma)
  {
    spellAbility.valid = FALSE;
    charisma=val;
    UpdateSkillBasedStats();
  };
}

//*****************************************************************************
//    NAME: CHARACTER::SetMaxMovement
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetMaxMovement(int val)
{
  if (GetType() != MONSTER_TYPE)
  {
    if (val > MAX_MOVE) val = MAX_MOVE;    
  }
  maxMovement = val;
}

//*****************************************************************************
//    NAME: CHARACTER::SetAge
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetAge(int val)
{
  int max = GetMaxAge();
  if (val > max) val=max;
  if (val < 1) val=1;
  age = val;
}


//*****************************************************************************
//    NAME: CHARACTER::SetTHAC0
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetTHAC0(int val)
{
  THAC0 = val;
  if (THAC0 < MIN_THAC0) THAC0 = MIN_THAC0;
  if (THAC0 > MAX_THAC0) THAC0 = MAX_THAC0;
}

//*****************************************************************************
//    NAME: CHARACTER::SetAC
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetAC(int val)
{
  m_AC=val;
  if (m_AC < MIN_AC) m_AC = MIN_AC;
  if (m_AC > MAX_AC) m_AC = MAX_AC;
}


int CHARACTER::SetHitPoints(int val, int /*fakearoo*/, bool canFinishCasting)
{
#ifdef UAFEngine
  if ((m_pCombatant != NULL) && m_pCombatant->m_useTempHP)
  {
    m_pCombatant->m_tempHP = val;
    return val;
  };
  if ( (val < hitPoints) && (IsCombatActive()) )
  {
    m_pCombatant->TakeDamage(hitPoints-val, FALSE, NULL, canFinishCasting, NULL);
  }
  else
#endif
  {
    SetHitPoints(val);
  };
  return val;
}


//*****************************************************************************
//    NAME: CHARACTER::GetHitPoints
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetHitPoints(void) const
{
#ifdef UAFEngine
  if ((m_pCombatant != NULL) && m_pCombatant->m_useTempHP)
  {
    return m_pCombatant->m_tempHP;
  };
#endif
  return hitPoints;
}




//*****************************************************************************
//    NAME: CHARACTER::SetHitPoints
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetHitPoints(int val)
{
#ifdef UAFEngine
  if ((m_pCombatant != NULL) && m_pCombatant->m_useTempHP)
  {
    m_pCombatant->m_tempHP = val;
    return;
  };
#endif
  // are we healing this character?
  if ((val > 0) && (val > hitPoints))
  {
    // disallow if diseased
    if (GetAdjSpecAb(SA_Diseased))
    {
      if (val > 1)
        val = 1;
    }
  }


  hitPoints=val;

  if (hitPoints > maxHitPoints)
    hitPoints = maxHitPoints;
  else if (hitPoints < -10)
    hitPoints = -10;

  if (hitPoints < 0)
  {
    if (!IsCombatActive()) // combat takes care of setting correct status
    {
      if (hitPoints == -10)
      {
        SetStatus(Dead);
      }
      else
      {
        // if okay, set to Dead, else leave status alone
        if (GetStatus() == Okay)
          SetStatus(Dead);
      }
    }
  }
}

//*****************************************************************************
//    NAME: CHARACTER::RestoreMaxHitPoints
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::RestoreMaxHitPoints()
{
  hitPoints=maxHitPoints;
}

//*****************************************************************************
//    NAME: CHARACTER::RestoreToBestCondition
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::RestoreToBestCondition()
{
  RestoreMaxHitPoints();
  SetStatus(Okay);
  m_spellEffects.RemoveAll();
}

//*****************************************************************************
//    NAME: CHARACTER::GetMaxHitPoints
//
// PURPOSE: 
//
//*****************************************************************************
int CHARACTER::GetMaxHitPoints() const
{
  return maxHitPoints;
}

//*****************************************************************************
//    NAME: CHARACTER::SetMaxHitPoints
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetMaxHitPoints(int val)
{
  maxHitPoints=val;
}

//*****************************************************************************
//    NAME: CHARACTER::SetBirthday
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetBirthday(int bday)
{
  if (bday <= 0) bday=1;
  if (bday > 365) bday = 365;
  birthday = bday;
}

//*****************************************************************************
//    NAME: CHARACTER::GetCurrExp
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::GetCurrExp(int classflag)
int CHARACTER::GetCurrExp(const BASECLASS_ID& baseclassID)
{
  //ASSERT( NumClassBits(classflag) == 1 );

/*
  switch (classflag)
  {
  case MagicUserFlag: return GetAdjMagicUserExp();
  case ClericFlag:    return GetAdjClericExp();
  case ThiefFlag:     return GetAdjThiefExp();
  case FighterFlag:   return GetAdjFighterExp();
  case PaladinFlag:   return GetAdjPaladinExp();
  case RangerFlag:    return GetAdjRangerExp();
  case DruidFlag:     return GetAdjDruidExp();
  }
  ASS ERT(FALSE);
*/
  int i;
  const BASECLASS_STATS *pStats;
  i = LocateBaseclassStats(baseclassID);
  if (i < 0) return 0;
  pStats = PeekBaseclassStats(i);
  if (pStats->previousLevel > 0) return 0;
  return pStats->x_experience;
}

#define excluded20170926
#ifdef excluded20170926
#ifdef UAFEDITOR
//*****************************************************************************
//    NAME: CHARACTER::ClearExperience
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ClearExperience(void)
{
/*
  MUExperience = 0;
  ClericExperience = 0;
  ThiefExperience = 0;
  FighterExperience = 0;
  PaladinExperience = 0;
  RangerExperience = 0;
  DruidExperience = 0;
*/
  int i, n;
  n = GetBaseclassStatsCount();
  for (i=0; i<n; i++)
  {
    BASECLASS_STATS *pbcs;
    pbcs = GetBaseclassStats(i);
    pbcs->x_experience = 0;
  };
}
#endif
#endif

//*****************************************************************************
//    NAME: CHARACTER::SetCurrExp
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::SetCurrExp(int classflag, int exp)
int CHARACTER::SetCurrExp(const BASECLASS_ID& baseclassID, int exp)
{
  //ASSERT( NumClassBits(classflag) == 1 );
/*
  switch (classflag)
  {
  case MagicUserFlag: MUExperience = exp; return;
  case ClericFlag:    ClericExperience = exp; return;
  case ThiefFlag:     ThiefExperience = exp; return;
  case FighterFlag:   FighterExperience = exp; return;
  case PaladinFlag:   PaladinExperience = exp; return;
  case RangerFlag:    RangerExperience = exp; return;
  case DruidFlag:     DruidExperience = exp; return;
  }
  ASSERT(FALSE);
*/
  BASECLASS_STATS *pbcs;
  pbcs = GetBaseclassStats(baseclassID);
  if (pbcs == NULL) return 0;
  if (pbcs->previousLevel > 0) return 0;
  pbcs->x_experience = exp;
  return exp;
}

//*****************************************************************************
//    NAME: CHARACTER::ClearLevels
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ClearLevels(void)
{
/*
  MULevel = 0; 
  ClericLevel = 0;
  ThiefLevel = 0; 
  FighterLevel = 0; 
  PaladinLevel = 0; 
  RangerLevel = 0;
  DruidLevel = 0; 
  */
  int i, n;
  n = GetBaseclassStatsCount();
  for (i=0; i<n; i++)
  {
    BASECLASS_STATS *pbcs;
    pbcs = GetBaseclassStats(i);
    pbcs->currentLevel = 0;
  };
#ifdef OldDualClass20180126
  temp__canUsePrevClass = -1;
#endif
}
/*
//*****************************************************************************
//    NAME: CHARACTER::SetLevel
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::SetLevel(int classflag, int lvl)
{
  ASSERT( NumClassBits(classflag) == 1 );
  switch (classflag)
  {
  case MagicUserFlag: MULevel = lvl; return;
  case ClericFlag:    ClericLevel = lvl; return;
  case ThiefFlag:     ThiefLevel = lvl; return;
  case FighterFlag:   FighterLevel = lvl; return;
  case PaladinFlag:   PaladinLevel = lvl; return;
  case RangerFlag:    RangerLevel = lvl; return;
  case DruidFlag:     DruidLevel = lvl; return;
  }
  ASSERT(FALSE);
}
*/
//*****************************************************************************
//    NAME: CHARACTER::SetLevel
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::SetLevel(const CString& baseclassName, int lvl)
void CHARACTER::SetLevel(const BASECLASS_ID& baseclassID, int lvl)
{
  //baseclassKey bct;
  //bct = baseclassData.FindName(baseclassName);
  int i;
  i = LocateBaseclassStats(baseclassID);
  if (i >= 0)
  {
    BASECLASS_STATS *pBaseclassStats;
    pBaseclassStats = GetBaseclassStats(i);
    //pBaseclassStats = &baseclassStats[i];
#ifdef OldDualClass20180126
    if (pBaseclassStats->currentLevel != lvl)
    {
      temp__canUsePrevClass = -1;
    };
#endif
    pBaseclassStats->currentLevel = lvl;
  };
  //switch (bct)
/*
  {
    case 0: FighterLevel   = lvl; break;
    case 1: ClericLevel    = lvl; break;
    case 2: RangerLevel    = lvl; break;
    case 3: PaladinLevel   = lvl; break;
    case 4: MULevel        = lvl; break;
    case 5: ThiefLevel     = lvl; break;
    case 6: DruidLevel     = lvl; break;
  }
*/
}

void CHARACTER::SetLevel(int lvl)
{
  int i, n;
  n = baseclassStats.GetSize();
  for (i=0; i<n; i++)
  {
    BASECLASS_STATS *pBaseclassStats;
    pBaseclassStats = GetBaseclassStats(i);
    //pBaseclassStats = &baseclassStats[i];
#ifdef OldDualClass20180126
    if (pBaseclassStats->currentLevel != lvl)
    {
      temp__canUsePrevClass = -1;
    };
#endif
    pBaseclassStats->currentLevel = lvl;    
  };
}


/*
//*****************************************************************************
//    NAME: CHARACTER::SetCurrExp
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::SetCurrExp(const CString& baseclassName, int exp)
void CHARACTER::SetCurrExp(const BASECLASS_ID& baseclassID, int exp)
{
  //baseclassKey bct;
  int arrayIndex;
  //bct = baseclassData.FindName(baseclassName);
  arrayIndex = baseclassData.LocateBaseclass(baseclassID);
  //switch (bct)
  {
    case 0: FighterExperience   = exp; break;
    case 1: ClericExperience    = exp; break;
    case 2: RangerExperience    = exp; break;
    case 3: PaladinExperience   = exp; break;
    case 4: MUExperience        = exp; break;
    case 5: ThiefExperience     = exp; break;
    case 6: DruidExperience     = exp; break;
  }

  baseclassStats[arrayIndex].experience = exp;
}
*/
//*****************************************************************************
//    NAME: CHARACTER::GetCurrLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::GetLevel(const CString& baseclassName)
int CHARACTER::GetCurrLevel(const BASECLASS_ID& baseclassID) const
{
  //baseclassKey bct;
  const BASECLASS_STATS *pStats;
  //bct = baseclassData.FindName(baseclassName);
  //arrayIndex = baseclassData.LocateBaseclass(baseclassID);
  pStats = this->PeekBaseclassStats(baseclassID);
  if (pStats == NULL) return -1;
  if (pStats->previousLevel > 0) return 0;
  return pStats->currentLevel;
  //return baseclassStats[arrayIndex].currentLevel;
}

//*****************************************************************************
//    NAME: CHARACTER::GetAllowedLevel
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::GetAllowedLevel(const CString& baseclassName)
int CHARACTER::GetAllowedLevel(const BASECLASS_ID& baseclassID) const
{
  //baseclassKey bct;
  int arrayIndex, maxLevel;
  int allowedLevel;
  const BASE_CLASS_DATA *pBaseclass;
  //bct = baseclassData.FindName(baseclassName);
  //arrayIndex = baseclassData.LocateBaseclass(baseclassID);
  arrayIndex = this->LocateBaseclassStats(baseclassID);
/*  {
    case 0: return FighterLevel;
    case 1: return ClericLevel;
    case 2: return RangerLevel;
    case 3: return PaladinLevel;
    case 4: return MULevel;
    case 5: return ThiefLevel;
    case 6: return DruidLevel;
  }
  return -1;
*/
  if (arrayIndex < 0) return -1;
  if (PeekBaseclassStats(arrayIndex)->previousLevel > 0) return 0;
  pBaseclass = baseclassData.PeekBaseclass(baseclassID);
  if (pBaseclass == NULL) return -1;
  allowedLevel =  pBaseclass->GetLevel(PeekBaseclassStats(arrayIndex)->x_experience);
  //allowedLevel =  pBaseclass->GetLevel(baseclassStats[arrayIndex].experience);
  maxLevel = GetLevelCap(pBaseclass);
  if (maxLevel != NoSkill)
  {
    if (maxLevel < allowedLevel)
    {
      allowedLevel = maxLevel;
    };
  };
  return allowedLevel;
}
/*
//*****************************************************************************
//    NAME: CHARACTER::GetCurrExp
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::GetCurrExp(const CString& baseclassName)
int CHARACTER::GetCurrExp(const BASECLASS_ID& baseclassID)
{
  //baseclassKey bct;
  int index;
  //bct = baseclassData.FindName(baseclassName);
  index = baseclassData.LocateBaseclass(baseclassID);
  //switch (bct)
  switch (index)
  {
    case 0: return FighterExperience;
    case 1: return ClericExperience;
    case 2: return RangerExperience;
    case 3: return PaladinExperience;
    case 4: return MUExperience;
    case 5: return ThiefExperience;
    case 6: return DruidExperience;
  }
  return -1;
}
*/
//*****************************************************************************
//    NAME: CHARACTER::IncCurrExp
//
// PURPOSE: 
//
//*****************************************************************************
//int CHARACTER::IncCurrExp(int classflag, int exp)
int CHARACTER::IncCurrExp(const BASECLASS_ID& baseclassID, int exp)
{
/*
  ASSERT( NumClassBits(classflag) == 1 );
  switch (classflag)
  {
  case MagicUserFlag: MUExperience += exp; MUExperience=max(MUExperience,0); return MUExperience;
  case ClericFlag:    ClericExperience += exp; ClericExperience=max(ClericExperience,0); return ClericExperience;
  case ThiefFlag:     ThiefExperience += exp; ThiefExperience=max(ThiefExperience,0); return ThiefExperience;
  case FighterFlag:   FighterExperience += exp; FighterExperience=max(FighterExperience,0); return FighterExperience;
  case PaladinFlag:   PaladinExperience += exp; PaladinExperience=max(PaladinExperience,0); return PaladinExperience;
  case RangerFlag:    RangerExperience += exp; RangerExperience=max(RangerExperience,0); return RangerExperience;
  case DruidFlag:     DruidExperience += exp; DruidExperience=max(DruidExperience,0); return DruidExperience;
  }
  ASSERT(FALSE);
  return 0;
*/
  //int arrayIndex;
  BASECLASS_STATS *pStats;
  pStats = GetBaseclassStats(baseclassID);
  //arrayIndex = LocateBaseclassStats(baseclassID);
  //if (arrayIndex < 0) return 0;
  if (pStats == NULL) return 0;
  return pStats->IncCurExperience(exp);
  //return (baseclassStats[arrayIndex].experience += exp);
}

#ifdef SpellDetectMagic
 
The original UAF decided whether the party could detect
magic by whether or not it had a memorized spell that
was capable of detecting magic.

Manikus wants this capability to be a character trait.
He asks "Can the active character detect magic?"
And he provides a hook to make this decision.
PRS 20130714

//*****************************************************************************
//    NAME: CHARACTER::CanCastDetectMagic
//
// PURPOSE: 
//
//*****************************************************************************
//GLOBAL_SPELL_ID CHARACTER::CanCastDetectMagic()
SPELL_ID CHARACTER::CanCastDetectMagic()
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  //CHARACTER_SPELL cspell;
  const CHARACTER_SPELL *pCharSp;
  const SPELL_DATA *pSdata;

  //POSITION pos = m_spellbook.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = m_spellbook.GetCount();
  for (i=0; i<n; i++)
  {
    //cspell = m_spellbook.GetNext(pos);
    pCharSp = m_spellbook.PeekCharacterSpell(i);
    //if (cspell.IsMemorized())
    if (pCharSp->IsMemorized())
    {
      //pSdata = spellData.GetSpellData(cspell.spell);
      pSdata = spellData.PeekSpell(pCharSp->spellID);
      if (pSdata != NULL)
      {
        CString answer;
        ActorType actor;
        // could be done using special abilities
        //if (pSdata->CanPerformSpecAb(SA_DetectMagic))
        GetContext(&actor);
        SetCharContext(&actor);
        ClearHookParameters();
        scriptContext.SetCharacterContext(this);
        scriptContext.SetSpellContext(pSdata);
        answer = pSdata->specAbs.RunScripts(CAN_SPELL_DETECT_MAGIC,
                                            ScriptCallback_LookForChar,
                                            "Y",
                                            name,
                                            pSdata->Name);
        scriptContext.Clear();
        ClearCharContext();
        if (!answer.IsEmpty())
        {
          //return cspell.spell;
          //return cspell.spellID;
          return pCharSp->spellID;
        };
        // or if user manually modifed this value with spell effect
        if (pSdata->CanEnableAffectAttr("$CHAR_DETECTMAGIC"))
          //return cspell.spell;
          //return cspell.spellID;
          return pCharSp->spellID;
      }
    }
  }
  //return GLOBAL_SPELL_ID();
  return SPELL_ID();
}
#endif

/*
//*****************************************************************************
//    NAME: CHARACTER::CanCastIdentify
//
// PURPOSE: 
//
//*****************************************************************************
GLOBAL_SPELL_ID CHARACTER::CanCastIdentify()
{
  CHARACTER_SPELL cspell;
  SPELL_DATA *pSdata;

  POSITION pos = m_spellbook.GetHeadPosition();
  while (pos != NULL)
  {
    cspell = m_spellbook.GetNext(pos);
    if (cspell.IsMemorized())
    {
      pSdata = spellData.GetSpellData(cspell.spell);
      if (pSdata!=NULL)
      {
        if (pSdata->CanEnableAffectAttr("$CHAR_IDENTIFY"))
          return cspell.spell;
      }
    }
  }
  return GLOBAL_SPELL_ID();
}
*/
#ifdef UAFEngine // engine-only magic code

//*****************************************************************************
//    NAME: CHARACTER::RemoveAllSpellEffectsByLevel
//
// PURPOSE: Removes only spells that have same level as lvl
//
//*****************************************************************************
int CHARACTER::RemoveAllSpellEffectsByLevel(int lvl)
{
  bool done = false; 
  int count=0;

  while (!done)
  {
    BOOL removed = FALSE;
    POSITION pos = m_spellEffects.GetHeadPosition();  

    while (pos != NULL)
    {    
      if (   ((m_spellEffects.PeekAt(pos)->flags & SPELL_EFFECTS_DATA::EFFECT_SPELL))
          || ((m_spellEffects.PeekAt(pos)->flags & SPELL_EFFECTS_DATA::EFFECT_SPELLSPECAB)) )
      {
        // Look for spell effects in the list that have a spell level == lvl
        //
        //SPELL_DATA *pSpell = spellData.GetSpellData(m_spellEffects.GetAt(pos).SourceSpell_ID());        
        SPELL_DATA *pSpell = spellData.GetSpell(m_spellEffects.PeekAt(pos)->SourceSpell_ID());        
        if ((pSpell!=NULL) && (pSpell->Level <= lvl))
        {
          count++;
          removed=true;
          m_spellEffects.RemoveAt(pos);
        }
      }
      else if ((m_spellEffects.PeekAt(pos)->flags & SPELL_EFFECTS_DATA::EFFECT_ITEMSPECAB))
      {
        // item special abilities require level 12 to remove in this manner
        if (lvl == 12)
        {
          count++;
          removed=true;
          m_spellEffects.RemoveAt(pos);
        }
      }
      // else it's a CHARSPECAB which cannot be removed

      if (!removed)
        m_spellEffects.GetNext(pos);
      else
        break;
    }   
    
    if (!removed)
      done = true;
  }

  DisplayCurrSpellEffects("RemoveAllSpellEffectsByLevel");
  return count;
}

//*****************************************************************************
//    NAME: CHARACTER::DispelAllSpellEffectsByLevel
//
// PURPOSE: Removes all spell effects that have spell level <= lvl, and
//          checks dispel flag for each spell
//
//*****************************************************************************
int CHARACTER::DispelSpellEffectsByLevel(int lvl)
{
  bool done = false; 
  int count=0;

  while (!done)
  {
    BOOL removed = FALSE;
    POSITION pos = m_spellEffects.GetHeadPosition();  

    while (pos != NULL)
    {    
      if (   ((m_spellEffects.PeekAt(pos)->flags & SPELL_EFFECTS_DATA::EFFECT_SPELL))
          || ((m_spellEffects.PeekAt(pos)->flags & SPELL_EFFECTS_DATA::EFFECT_SPELLSPECAB)) )
      {
        // Look for spell effects in the list that have a spell level == lvl
        //
        //SPELL_DATA *pSpell = spellData.GetSpellData(m_spellEffects.GetAt(pos).SourceSpell_ID());        
        SPELL_DATA *pSpell = spellData.GetSpell(m_spellEffects.PeekAt(pos)->SourceSpell_ID());
        if ((pSpell!=NULL) && (pSpell->Level <= lvl) && (pSpell->CanBeDispelled))
        {
          count++;
          removed=true;
          m_spellEffects.RemoveAt(pos);
        }
      }
      else if ((m_spellEffects.PeekAt(pos)->flags & SPELL_EFFECTS_DATA::EFFECT_ITEMSPECAB))
      {
        // item special abilities require level 12 to remove in this manner
        if (lvl >= 12)
        {
          count++;
          removed=true;
          m_spellEffects.RemoveAt(pos);
        }
      }
      // else it's a CHARSPECAB which cannot be removed

      if (!removed)
        m_spellEffects.GetNext(pos);
      else
        break;
    }   
    
    if (!removed)
      done = true;
  }
  DisplayCurrSpellEffects("DispelSpellEffectsByLevel");
  return count;
}

//*****************************************************************************
//    NAME: CHARACTER::RemoveSpellByName
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::RemoveSpellByName(const CString &sname)
{
  bool done = false; 
  int count=0;

  while (!done)
  {
    BOOL removed = FALSE;
    POSITION pos = m_spellEffects.GetHeadPosition();  

    while (pos != NULL)
    {    
      // Look for spell effects in the list that have a spell name == sname
      //
      // we have an effect that has a spell record in the spell database
      //SPELL_DATA *pSpell = spellData.GetSpellData(m_spellEffects.GetAt(pos).SourceSpell_ID());        
      SPELL_DATA *pSpell = spellData.GetSpell(m_spellEffects.PeekAt(pos)->SourceSpell_ID());
      if ((pSpell!=NULL) && (pSpell->Name.CompareNoCase(sname)==0) && (pSpell->CanBeDispelled))
      {
        count++;
        removed=true;
        m_spellEffects.RemoveAt(pos);
      }

      if (!removed)
        m_spellEffects.GetNext(pos);
      else
        break;
    }   
    
    if (!removed)
      done = true;
  }
  DisplayCurrSpellEffects("RemoveSpellByName");
  return count;
}

//*****************************************************************************
//    NAME: CHARACTER::ClearAllItemCursedFlags
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ClearAllItemCursedFlags()
{
  POSITION pos = myItems.GetHeadPosition();
  while (pos != NULL)
    myItems.GetNext(pos).cursed=FALSE;  
}

//*****************************************************************************
//    NAME: CHARACTER::RemoveAllTargets
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::RemoveAllTargets()
{
  targets.Clear();
}

//*****************************************************************************
//    NAME: CHARACTER::InitTargeting
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::InitTargeting(spellTargetingType ttype,
                              BOOL  canTargetFriend,
                              BOOL  canTargetEnemy,
                              BOOL  partyfriend,
                              DWORD targs,
                              DWORD range,
                              DWORD width,
                              DWORD height,
                              BOOL  lingering)
{
  //ASSERT( IsCombatActive() == FALSE ); // combat magic handled in combat.cpp

  RemoveAllTargets();
  targets.m_canTargetFriend = canTargetFriend;
  targets.m_canTargetEnemy  = canTargetEnemy;
  targets.m_IsPartyAligned = partyfriend;
  targets.m_area = FALSE;
  switch (ttype)
  {
  case Self:
    if (lingering)
    {
      targets.m_targType = LingerSelf;
    }
    else
    {
      targets.m_targType = NowSelf;
    };
    targets.m_SelectingUnits=TRUE;
    targets.m_MaxHitDice=0;
    //targets.m_MaxTargets=1;
    //targets.m_MaxRange=0;
    targets.MaxTargets(1);
    targets.MaxRange(0);
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;
  case SelectedByCount:
    if (lingering)
    {
      targets.m_targType = LingerSelectedByCount;
    }
    else
    {
      targets.m_targType = NowSelectedByCount;
    };
    targets.m_SelectingUnits=TRUE;
    targets.m_MaxHitDice=0;
    ASSERT( targs > 0);
    //targets.m_MaxTargets=targs;
    targets.MaxTargets(targs);
    if (range==0) range = 1000000;
    //targets.m_MaxRange=range;
    targets.MaxRange(range);
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;
  case WholeParty:
    if (lingering)
    {
      targets.m_targType = LingerWholeParty;
    }
    else
    {
      targets.m_targType = NowWholeParty;
    };
    targets.m_SelectingUnits=TRUE;
    targets.m_MaxHitDice=0;
    //targets.m_MaxTargets=party.numCharacters;
    //targets.m_MaxRange=0;
    targets.MaxTargets(party.numCharacters);
    targets.MaxRange(0);
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;
  case TouchedTargets:
    if (lingering)
    {
      targets.m_targType = LingerTouchedTargets;
    }
    else
    {
      targets.m_targType = NowTouchedTargets;
    };
    targets.m_SelectingUnits=TRUE;
    targets.m_MaxHitDice=0;
    //targets.m_MaxTargets=targs;
    //targets.m_MaxRange=1;
    targets.MaxTargets(targs);
    targets.MaxRange(9999);
    targets.m_maxRangeX = 1;
    targets.m_maxRangeY = 1;
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;
  case AreaLinePickStart:
    if (lingering)
    {
      targets.m_targType = LingerAreaLinePickStart;
    }
    else
    {
    targets.m_targType = NowAreaLinePickStart;
    };
    targets.m_area = TRUE;
    if (IsCombatActive())
    {
      targets.m_SelectingUnits=FALSE;
      //targets.m_MaxTargets=targs;
      targets.MaxTargets(targs);
      if (range==0) range = 1000000;
      //targets.m_MaxRange=range;
      targets.MaxRange(range);
    }
    else // acts like ttype=WholeParty
    {
      targets.m_SelectingUnits=TRUE; // will be whole party
      //targets.m_MaxTargets=party.numCharacters;
      //targets.m_MaxRange=0;
      targets.MaxTargets(party.numCharacters);
      targets.MaxRange(0);
    }
    targets.m_MaxHitDice=0;    
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;

  case AreaLinePickEnd:
    if (lingering)
    {
      targets.m_targType = LingerAreaLinePickEnd;
    }
    else
    {
      targets.m_targType = NowAreaLinePickEnd;
    };
    targets.m_area = TRUE;
    if (IsCombatActive())
    {
      targets.m_SelectingUnits=FALSE;
      //targets.m_MaxTargets=targs;
      targets.MaxTargets(targs);
      if (range==0) range = 1000000;
      //targets.m_MaxRange=range;
      targets.MaxRange(range);
    }
    else // acts like ttype=WholeParty
    {
      targets.m_SelectingUnits=TRUE; // will be whole party
      //targets.m_MaxTargets=party.numCharacters;
      //targets.m_MaxRange=0;
      targets.MaxTargets(party.numCharacters);
      targets.MaxRange(0);
    }
    targets.m_MaxHitDice=0;    
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;

  case AreaSquare:     
    if (lingering)
    {
      targets.m_targType = LingerAreaSquare;
    }
    else
    {
      targets.m_targType = NowAreaSquare;
    };
    targets.m_area = TRUE;
    if (IsCombatActive())
    {
      targets.m_SelectingUnits=FALSE;
      //targets.m_MaxTargets=targs;
      targets.MaxTargets(targs);
      if (range==0) range = 1000000;
      //targets.m_MaxRange=range;
      targets.MaxRange(range);
      targets.Width(width);
      targets.Height(height);
    }
    else // acts like ttype=WholeParty
    {
      targets.m_SelectingUnits=TRUE; // will be whole party
      //targets.m_MaxTargets=party.numCharacters;
      //targets.m_MaxRange=0;
      targets.MaxTargets(party.numCharacters);
      targets.MaxRange(0);
    }
    targets.m_MaxHitDice=0;    
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;

  
  case AreaCone:       
    if (lingering)
    {
      targets.m_targType = LingerAreaCone;
    }
    else
    {
      targets.m_targType = NowAreaCone;
    };
    targets.m_area = TRUE;
    if (IsCombatActive())
    {
      targets.m_SelectingUnits=FALSE;
      //targets.m_MaxTargets=targs;
      targets.MaxTargets(targs);
      if (range==0) range = 1000000;
      //targets.m_MaxRange=range;
      targets.MaxRange(range);
      targets.Width(width);
      targets.Height(height);
    }
    else // acts like ttype=WholeParty
    {
      targets.m_SelectingUnits=TRUE; // will be whole party
      //targets.m_MaxTargets=party.numCharacters;
      //targets.m_MaxRange=0;
      targets.MaxTargets(party.numCharacters);
      targets.MaxRange(0);
    }
    targets.m_MaxHitDice=0;    
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;

  
  case AreaCircle:
    if (lingering)
    {
      targets.m_targType = LingerAreaCircle;
    }
    else
    {
      targets.m_targType = NowAreaCircle;
    };
    targets.m_area = TRUE;
    if (IsCombatActive())
    {
      targets.m_SelectingUnits=FALSE;
      //targets.m_MaxTargets=targs;
      targets.MaxTargets(targs);
      if (range==0) range = 1000000;
      //targets.m_MaxRange=range;
      targets.MaxRange(range);
      targets.Height(height);
      targets.Width(width);
    }
    else // acts like ttype=WholeParty
    {
      targets.m_SelectingUnits=TRUE; // will be whole party
      //targets.m_MaxTargets=party.numCharacters;
      //targets.m_MaxRange=0;
      targets.MaxTargets(party.numCharacters);
      targets.MaxRange(0);
    }
    targets.m_MaxHitDice=0;    
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;
  case SelectByHitDice:
    if (lingering)
    {
      targets.m_targType = LingerSelectByHitDice;
    }
    else
    {
      targets.m_targType = NowSelectByHitDice;
    };
    targets.m_SelectingUnits=TRUE;
    targets.m_MaxHitDice=targs;
    //targets.m_MaxTargets=0;
    targets.MaxTargets(0);
    if (range==0) range = 1000000;
    //targets.m_MaxRange=range;
    targets.MaxRange(range);
    targets.m_MapTargetX=-1;
    targets.m_MapTargetY=-1;
    break;
  }
}

//*****************************************************************************
//    NAME: CHARACTER::AddTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::AddTarget(CHARACTER &targ, int range)
{
#ifdef newCombatant
  if (m_pCombatant != NULL)
  {
    return m_pCombatant->C_AddTarget(*targ.m_pCombatant, range);
  };
#endif
  ASSERT( IsCombatActive() == FALSE ); // combat magic handled in combat.cpp
  if (targets.m_SelectingUnits==FALSE) return FALSE;
  //if ((targets.m_FriendlyFireOk==FALSE) && (targ.IsPartyMember())) return FALSE;
  if ((targets.m_canTargetFriend==FALSE) && (targ.IsPartyMember())) return FALSE;
  if ((targets.m_canTargetEnemy==FALSE) && (!targ.IsPartyMember())) return FALSE;
  return targets.STD_AddTarget(targ.uniquePartyID, targ.GetNbrHD(), range, -1, -1);
}

#ifdef newCombatant
BOOL CHARACTER::GetIsFriendly(void) const
{
  return (m_pCombatant==NULL)?TRUE:m_pCombatant->GetIsFriendly();
}
#endif

//*****************************************************************************
//    NAME: CHARACTER::AddTargetSelf
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::AddTargetSelf()
{
#ifdef newCombatant
  if (m_pCombatant != NULL)
  {
    return m_pCombatant->AddTargetSelf();
  };
#endif
  return AddTarget(*this, 0);
}

//*****************************************************************************
//    NAME: CHARACTER::InvokeSpellOnTarget
//
// PURPOSE: 
//
//
// Check Combat_Only and Friendly_Fire_Ok flags.
//
// spellCastingTimeType { sctImmediate, sctInitiative, sctRounds, sctTurns }
// 
// long Casting_Time;
// spellCastingTimeType Casting_Time_Type;
//
// Casting Time (non-combat):
//   
//   not used, spells are activated immediately
//  
// Casting Time (combat):
//  
//   Each round has 10 initiatives
//   1 round = 1 minute
//   10 rounds = 1 Turn
//   Spells requiring 1 or more rounds/turns always get cast at the end of a round/turn
//   Any hit on caster during casting time voids the spell.
//
//   sctImmediate: spell activated immediately
//   sctInitiative: Casting_Time is added to current initiative. Spell is activated
//                  when initiative time is reached. Cannot go past current round.
//   sctRounds:     Casting_Time is added to current round. Spell is activated
//                  when result round is reached, at end of round.
//   sctTurns:      Casting_Time*10 is added to current round. Spell is activated
//                  when result turn is reached, at end of that round.
//   
// Spell targeting when not in combat will only
// involve party members.
//
// spellTargetingType { Self, SelectedByCount, WholeParty, TouchedTargets, AreaCircle, SelectByHitDice }
//
// Targeting (non-combat):
// 
//   Self: affects caster
//   WholeParty,AreaCircle: affects whole party 
//   SelectedByCount, TouchedTargets: affects x number of targeted party members
//   SelectByHitDice: affects targeted party members until HD limit reached
//
//   Self, WholeParty, and AreaCircle don't need target selection, cast immediately
//   SelectedByCount,TouchedTargets,SelectByHitDice require individual target selection
//
// Targeting (combat):
//
//   Self: affects caster
//   WholeParty: affects whole party 
//   AreaCircle: select target map square, all targets within range are affected
//   SelectedByCount: affects x number of targets
//   TouchedTargets: affects 1 target within range 1
//   SelectByHitDice: select targets until HD limit reached
//
//   Self and WholeParty don't need target selection, cast immediately
//   AreaCircle, SelectedByCount,TouchedTargets,SelectByHitDice require individual target selection
//
// After targets are selected (if needed), evaluate spell script (if present) in
// context of caster and each target. If script evaluates to TRUE=1 check target
// saving throw.
//
// Saving Throws:
//
//   spellSaveVersusType { ParPoiDM, PetPoly, RodStaffWand, Sp, BreathWeapon };
//   
//   Retrieve needed save value for target based on SaveVersusType. Each type
//   will have a single value that increases with target level. Roll 20-sided
//   dice for target's saving throw. If roll is less than SaveVersus value, no
//   save is made and full spell affect is applied. If roll is equal to or
//   greater than SaveVersus, then SaveEffectType is used.
//
//   spellSaveEffectType { NoSave, SaveNegates, SaveForHalf, UseTHAC0 };
//
//   NoSave: full affect applied to target
//   SaveNegates: no affect applied to target
//   SaveForHalf: if numeric attribute affected, apply half, otherwise ignore
//   UseTHAC0: if saving throw less than THAC0, full affect, else none
//
// Spell Duration (non-combat):
//
//   spellDurationType { inRounds, byDamageTaken, inHours, inDays, Instantaneous };
//   
//   Same duration applies to all affected targets, except for byDamageTaken.
//
//   inRounds: (rounds/10) minutes
//   byDamageTaken: track total hp damage done to target
//   inHours: (hours*60) minutes
//   inDays:  (days*24*60) minutes
//   Instantaneous: never expires, no need to track this one in ActiveSpell list
//
// Spell Duration (non-combat):
//
//   spellDurationType { inRounds, byDamageTaken, inHours, inDays, Instantaneous };
//   
//   Same duration applies to all affected targets, except for byDamageTaken.
//
//   The unit of time for duration is rounds. 1 round = 1 game minute
//
//   inRounds: rounds = minutes
//   byDamageTaken: track total hp damage done to target
//   inHours: (hours*60) = minutes 
//   inDays:  (days*24*60) = minutes
//   Instantaneous: never expires, no need to track this one in ActiveSpell list
//
//*****************************************************************************
int CHARACTER::InvokeSpellOnTarget(CHARACTER& targ, 
                                   int activeSpellKey, 
                                   //GLOBAL_SPELL_ID spellDbKey,
                                   const SPELL_ID& spellID,
                                   CString *displayText,
                                   BOOL *spellSucceeded,
                                   InvokeSpellOptions *pInvokeOptions,
                                   bool abortIfSetPartyXY,
                                   ToHitComputation *pToHitComputation)
{
  BOOL ss;
  SAVING_THROW_DATA stData;
  if (spellSucceeded == NULL) spellSucceeded = &ss;
  if (displayText != NULL)
  {
    displayText->Empty();
  };
  *spellSucceeded = FALSE;


  //SPELL_DATA *pSdata = spellData.GetSpellData(spellDbKey);
  SPELL_DATA *pSdata = spellData.GetSpell(spellID);
  if (pSdata==NULL) 
  {
    die(0x3c4c7);
  }


  // check to see if target is already affected by same spell effect
  // and it is not allowed to be cumulative
  if (!pSdata->IsCumulative)
  {
    bool dupFound=false;
    POSITION saPos = targ.m_spellEffects.GetHeadPosition();
    while ((saPos != NULL) && (!dupFound))
    {
      const SPELL_EFFECTS_DATA &saData = *targ.m_spellEffects.PeekNext(saPos);
      //if (saData.SourceSpell_ID() == pSdata->m_gsID)
      if (saData.SourceSpell_ID() == pSdata->SpellID())
        dupFound=true;
    }
    if (dupFound)
      return activeSpellKey;
  }


  
  {
    CString result;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    const CLASS_DATA *pClass;
    const RACE_DATA *pRace;
    CString spellName;
    //SetItemContext(giID);
    scriptContext.SetSpellContext(pSdata);  // Includes spell Context!
    scriptContext.SetTargetContext(&targ);
    scriptContext.SetAttackerContext(this);
    //pClass = classData.GetClassData(this->GetClass());
    pClass = classData.PeekClass(this->GetClass());
    //pRace = raceData.GetRaceData(targ.race());
    //pRace = raceData.PeekRace(targ.race);
    pRace = targ.PeekRaceData();
    scriptContext.SetClassContext(pClass);
    spellName = pSdata->Name;
  //  hookParameters[5].Format("%d",m_toHitDiceRoll);
  //  hookParameters[6].Format("%d",m_effectiveTHAC0);
    result = pSdata->RunSpellScripts
                 (DOES_SPELL_ATTACK_SUCCEED,
                  ScriptCallback_LookForChar,
                  "YN",
                  "Invoke spell on target");
    if (result.IsEmpty())
    {
      if (pRace != NULL)
      {
        result = pRace->RunRaceScripts
                    (DOES_SPELL_ATTACK_SUCCEED,
                     ScriptCallback_LookForChar,
                     "YN",
                    "Invoke spell on target");
      };
    }
    if (result.IsEmpty())
    {
      if (targ.GetType() == MONSTER_TYPE)
      {
        //int monsterIndex;
        //monsterIndex = targ.m_pCombatant->origKey;
        {
          //MONSTER_DATA *pMonsterData;
          const MONSTER_DATA *pMonsterData;
          pMonsterData = monsterData.PeekMonster(monsterID);
          if (pMonsterData != NULL)
          {
            result = pMonsterData->RunMonsterScripts
                      (DOES_SPELL_ATTACK_SUCCEED,
                       ScriptCallback_LookForChar,
                       "YN",
                       "Invoke spell on target");
          };
        };
      };
    };
    if (result.IsEmpty())
    {

      result = targ.RunCharacterScripts
                (DOES_SPELL_ATTACK_SUCCEED,
                 ScriptCallback_LookForChar,
                 "YN",
                 "Invoke spell on target");
    };

    *spellSucceeded = TRUE;
    if (!result.IsEmpty())
    {
      if (result[0] == 'N')
      {
        *spellSucceeded = FALSE;
      };
    };
    if (displayText != NULL)
    {
      *displayText = hookParameters[5];
    };
  };

  
  if (!(*spellSucceeded)) 
  {
    return activeSpellKey;
  };


  stData.changeResult = 1.0;
  stData.noEffectWhatsoever = FALSE;
  stData.pCaster = this;
  stData.pTarget = &targ;
  stData.pSpell = pSdata;
  stData.success = FALSE;
  if (pSdata->Save_Result != NoSave)
  {
    targ.DoesSavingThrowSucceed(pSdata,
                                stData,
                                targ.GetIsFriendly() == GetIsFriendly(), 
                                this,
                                displayText);
  };
  if (stData.noEffectWhatsoever)
  {
    return activeSpellKey;
  };


  ActorType cActor;  //Caster
  ActorType tActor;  //Target
  {

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
    result = pSdata->ExecuteSpellBeginScript(this, &targ, pToHitComputation);
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
    
  if (activeSpellKey < 0)
  {
    if (pSdata->Duration_Rate != Permanent)
      activeSpellKey=activeSpellList.GetNextKey();  //Global active Spell List
  }
  



  bool effectAdded= false;
  // run script for each effect
  POSITION pos = pSdata->m_EffectedAttributes.GetHeadPosition();
//  if (pos != NULL)
  {
    while (pos != NULL)
    {
      SPELL_EFFECTS_DATA saData = *pSdata->m_EffectedAttributes.PeekNext(pos);
      //saData.SourceOfEffect(pSdata->m_gsID, pSdata->LingerOnceOnly ); // spell database key
      saData.SourceOfEffect(pSdata->SpellID(), pSdata->LingerOnceOnly ); // spell database key
      saData.parent = activeSpellKey;
      saData.flags |= SPELL_EFFECTS_DATA::EFFECT_SPELL;
    
      if (saData.flags & SPELL_EFFECTS_DATA::EFFECT_TARGET)
      {
        SCRIPT_CONTEXT scriptContext;
        //scriptContext.SetAttackerContext(this);
        //scriptContext.SetTargetContext(&targ);
        saData.m_pCaster = this;
        saData.m_pTarget = &targ;
        BOOL activate = TRUE;
        if (saData.ActivationScript().GetLength() > 0)
        {
          activate = (pSdata->ExecuteActivationScript(saData)?TRUE:FALSE);    //true;// xxxxx          
        }

        if (activate)
        {
          // 20110201 PRS CalcSpellEffectChangeValue(pSdata,saData,TRUE);            
          targ.CalcSpellEffectChangeValue(pSdata,saData,targ.GetIsFriendly() == GetIsFriendly(), this); 
          if (targ.AddSpellEffect(saData, this, pInvokeOptions,"Invoking a spell on target"))
          {
            // force a scan of the current char status
            //Do this at FinalSpellStage  targ.TakeDamage(0, FALSE, pInvokeOptions, &targ==this, NULL);
            effectAdded = true;
          }      
        }          
      }
    }
  }
//  else
  //if (!effectAdded)
  /* There are several reasons that 'AddSpellEffect()' will return true even though
   * it has added no effect.  In fact, it may have deleted an effect!!!!!
   * So, if we want to guarantee that at least on effect has been added, then
   * must add a 'do-nothing' effect ourselves.
   */

  /* To tell the truth, I see no reason to add a NOP spell effect.
   * The 'activeSpellList' will contain an entry for any spell that 
   * will expire in a finite amount of time.  That entry will trigger
   * the 'Spell End Script'.  PRS 20150104
  {
    ASSERT(FALSE); // we should not do this for permanent spells
    if (activeSpellKey >= 0)
    {
      // no affected attributes, add placeholder
      //
      // I think this allows the spell end script to be executed
      // when the spell expires?
      //
      // no need to add unless this spell expires
      //
      SPELL_EFFECTS_DATA saData;
      saData.changeResult = 0.0;

      // *** PRS 20091019
      //saData.SourceSpell_ID() = pSdata->m_gsID; // spell database key
      //saData.SourceOfEffect(pSdata->m_gsID, pSdata->LingerOnceOnly); // spell database key
      saData.SourceOfEffect(pSdata->SpellID(), pSdata->LingerOnceOnly); // spell database key
      // *** PRS 20091019

      saData.parent = activeSpellKey;
      saData.flags |= SPELL_EFFECTS_DATA::EFFECT_SPELL; 
      saData.AffectedAttr("NOP_" + pSdata->Name);
      targ.AddSpellEffect(saData, this, pInvokeOptions);
    }
  }
   */

  // add special abilities
  {
    //int PreSpecAbCount = targ.spellEffects.GetCount();
    //pSdata->specAbs.EnableAllFor(&targ);
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetAttackerContext(this);
    scriptContext.SetSpellContext(pSdata);
    scriptContext.SetTargetContext(&targ);
    hookParameters[5].Format("%d", targets.m_MapTargetX);
    hookParameters[6].Format("%d", targets.m_MapTargetY);
    pSdata->RunSpellScripts(INVOKE_SPELL_ON_TARGET, 
                            ScriptCallback_RunAllScripts, 
                            NULL, 
                            "Invoke spell on target");
    //int PostSpecAbCount = targ.spellEffects.GetCount();
    //int Diff = PostSpecAbCount - PreSpecAbCount;
    //if (Diff > 0)
    //{
      //POSITION pos2 = targ.spellEffects.GetTailPosition();  
      //while ((pos2 != NULL) && (Diff > 0))
      //{        
        //targ.spellEffects.GetAt(pos2).flags |= SPELL_EFFECTS_DATA::EFFECT_SPELLSPECAB;
        //targ.spellEffects.GetAt(pos2).SourceSpell_ID() = pSdata->m_gsID;
        //if (activeSpellKey >= 0)
          //targ.spellEffects.GetAt(pos2).parent = activeSpellKey;
        //targ.spellEffects.GetPrev(pos2);
        //Diff--;
      //}
    //}
    if (pInvokeOptions != NULL)
    {
      if (targ.hitPoints <= -10)
      {
        pInvokeOptions->m_whatSpriteNeeded = DeathSprite;
      }
      else if (targ.hitPoints <= 0)
      {
        pInvokeOptions->m_whatSpriteNeeded = DyingSprite;
      };
    };
  };

  ClearTargetContext();
  ClearCharContext();      

  if (pSdata->Duration_Rate != Permanent) // Instantaneous = permanent = never expires
                                          // So there is no need for an 'activeSpellList' entry.
  {            
    ASSERT( activeSpellKey >= 0);

    // only need to add one active spell record for each
    // spell cast by caster.
    if (activeSpellList.Get(activeSpellKey) == NULL)
    {
      ACTIVE_SPELL aspell;
      //GetContext(&aspell.caster, pSdata->schoolMask);
      GetContext(&aspell.caster, pSdata->schoolID);
      aspell.caster = cActor;
      aspell.target = tActor;
      aspell.key = activeSpellKey;
      //aspell.spellKey = pSdata->m_gsID; // spell db key
      aspell.spellID = pSdata->SpellID(); // spell db key
      aspell.CountTime=0;
      aspell.StopTime = 0;
      aspell.Lingers = (IsCombatActive() ? pSdata->Lingers : FALSE); // can't linger if not in combat
      aspell.lingerData.OnceOnly = pSdata->LingerOnceOnly;

      if (aspell.Lingers)
      {
        // add affected map locations to active spell record
        aspell.lingerData.mapdata.RemoveAll();
        POSITION tpos = targets.m_maptarglist.GetHeadPosition();
        while (tpos != NULL)
          aspell.lingerData.mapdata.AddTail(targets.m_maptarglist.GetNext(tpos));

        // add targets to active spell record so that linger spell checks
        // won't re-target same dudes once they move (unless allowed by OnceOnly flag)
        for (int i=0;i<targets.m_targets.GetSize();i++)
          aspell.lingerData.affectedtargs.AddTail(targets.m_targets[i]);
      }

      WriteDebugString("Adding active spell cast by %i\n", aspell.caster.Instance);

      double val = 0.0;
      int casterLevel = -1;
      // determine duration value

      
      {
        CString result;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        scriptContext.SetSpellContext(pSdata);
        scriptContext.SetAttackerContext(this);
        if (m_pCombatant != NULL)
        {
          hookParameters[5] = m_pCombatant->GetSecondarySpellIDBeingCast();
        };
        result = pSdata->RunSpellScripts(SPELL_CASTER_LEVEL, 
                                          ScriptCallback_RunAllScripts, 
                                          NULL,
                                          "Invoke spell on target");
        if (result.GetLength()> 0)
        {
          sscanf(result,"%d",&casterLevel);
        };
      };
      
      {
        //val = EvalDiceAsClass(pSdata->Duration, pSdata->schoolMask, &casterLevel);
        val = EvalDiceAsClass(pSdata->Duration, pSdata, &casterLevel);
      };


      {
        CString result;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        scriptContext.SetSpellContext(pSdata);
        scriptContext.SetAttackerContext(this);
        hookParameters[5].Format("%d", int(val));
        result = pSdata->RunSpellScripts(SPELL_DURATION, 
                                         ScriptCallback_RunAllScripts, 
                                         NULL, 
                                         "Invoke spell on target");
        if (result.GetLength()> 0)
        {
          sscanf(result,"%lf",&val);
        };
      };
 




      // convert duration value into game minutes
      switch (pSdata->Duration_Rate)
      {
      case inRounds:
        val = max(1,val); // 1 minute min
        aspell.StopTime = val+party.getElapsedMinutes();        
        break;
      case byNbrAttacks:
      case byDamageTaken:      
        aspell.StopTime = val;
        break;
      case inHours:
        val = val * 60.0;
        val = max(1,val); // 1 round min
        aspell.StopTime = val+party.getElapsedMinutes();        
        break;
      case inDays:
        val = val * 24.0 * 60.0;
        val = max(1,val); // 1 round min
        aspell.StopTime = val+party.getElapsedMinutes();        
        break;
      default: 
        die(0xab509); 
        break;
      }
        
      activeSpellList.xAddWithCurrKey(aspell);
    }
  }

  DisplayCurrSpellEffects("InvokeSpellOnTarget");
  // 20161123 
  //  I am investigating when to run the Begin/End Scripts for a lingering spell.
  //  I found this following line removed.  I wonder why.  Where will the end script be run?
  //  We need to run it once for each beginscript.....and we executed the begin script above.
  //  Hmmmmm.....
  //
  // 10161206  The following comment copied from CHARACTER::addSpellEffect/
      /* 20141229  Version 1.0303  PRS.
       * I want the SpellEndScript to match up one-to-one with the SpellBeginScript.
       * As it was, we ran the Begin Script once for each target the spell attacked.
       * Then we ran the End Script for each spell effect that was removed.  The result
       * was that the Begin Script might run without any End Script or one Begin
       * Script might result in several End Scripts.  Moreover, the spell effects might
       * end at a much later time when the state of the caster could easily have
       * undergone dramtic changes.
       *
       * So.....Now we run the Begin Script, apply any effects to the target, and
       * immediately run the End Script.  One-for-one in the same instant of time.
       */
  //ASSERT(FALSE);  //Not now   pSdata->ExecuteSpellEndScript(this, &targ);
  {
    // Added 20161126.  We seem to have forgotten that every SpellBeginScript should
    // be matched with a SpellEndScript.  In the simple case that I tested, no end script
    // was ever run.  I wonder where we thought it might take place.
    // Oh, Well.  I'll put it here for now.  PRS
    //
    // 20161206 PRS
    // I see now that we ran the End script when the activeSpellList entry (if present)
    // was removed.  That is bad.  So I deleted the code at 
    // ACTIVE_SPELL_LIST::DeactivateActiveSpell which ran the End Script when the
    // expired entry was removed.

    pSdata->ExecuteSpellEndScript(this, &targ);
  };
  return activeSpellKey;
}



//*****************************************************************************
int CHARACTER::InvokeLingerSpellOnTarget(CHARACTER& targ, 
                                         int activeSpellKey, 
                                         //GLOBAL_SPELL_ID spellDbKey,
                                         const SPELL_ID& spellID,
                                         CString *displayText,
                                         BOOL *spellSucceeded,
                                         InvokeSpellOptions *pInvokeOptions,
                                         bool abortIfSetPartyXY,
                                         ToHitComputation *pToHitComputation)
{
  BOOL ss;
  SAVING_THROW_DATA stData;
  if (spellSucceeded == NULL) spellSucceeded = &ss;

  if (displayText != NULL)
  {
    displayText->Empty();
  };
  *spellSucceeded = FALSE;


  //SPELL_DATA *pSdata = spellData.GetSpellData(spellDbKey);
  SPELL_DATA *pSdata = spellData.GetSpell(spellID);
  if (pSdata==NULL) 
  {
    die(0x3c4c7);
  }


  // check to see if target is already affected by same spell effect
  // and it is not allowed to be cumulative
  if (!pSdata->IsCumulative)
  {
    bool dupFound=false;
    POSITION saPos = targ.m_spellEffects.GetHeadPosition();
    while ((saPos != NULL) && (!dupFound))
    {
      const SPELL_EFFECTS_DATA &saData = *targ.m_spellEffects.PeekNext(saPos);
      //if (saData.SourceSpell_ID() == pSdata->m_gsID)
      if (saData.SourceSpell_ID() == pSdata->SpellID())
        dupFound=true;
    }
    if (dupFound)
      return activeSpellKey;
  }

//  ActorType actor;
// The Spell Begin Script was executed when the spell was 
// originally cast.  We are now causing the effects of
// that spell.  Too late to stop the spell from being cast.
//  {
//    HOOK_PARAMETERS hookParameters;
//    SCRIPT_CONTEXT scriptContext;
//
//    bool result;
//    GetContext(&actor, pSdata->schoolMask);
//    SetCharContext(&actor);
//    targ.GetContext(&actor);
//    SetTargetContext(&actor);
//    scriptContext.SetSpellContext(pSdata);
//  
//    setPartyXY_x = -1;
//    result = pSdata->ExecuteSpellBeginScript();
//    if (abortIfSetPartyXY && (setPartyXY_x >= 0))
//    {
//      return activeSpellKey;
//    };
//    if (!result)
//      return activeSpellKey;
//  };
  
    

//  if (activeSpellKey < 0)
//  {
//    if (pSdata->Duration_Rate != Permanent)
//      activeSpellKey=activeSpellList.GetNextKey();
//  }
  
  // We should not be here if there is no Active Spell Key
  if (activeSpellKey < 0)
  {
    die(0x98cba);
  };

  
  {
    CString result;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    const CLASS_DATA *pClass;
    const RACE_DATA *pRace;
    CString spellName;
    //SetItemContext(giID);
    scriptContext.SetSpellContext(pSdata);  // Includes spell Context!
    scriptContext.SetTargetContext(&targ);
    scriptContext.SetAttackerContext(this);
    //pClass = classData.GetClassData(this->GetClass());
    pClass = classData.PeekClass(this->GetClass());
    //pRace = raceData.GetRaceData(targ.race());
    //pRace = raceData.PeekRace(targ.race);
    pRace = targ.PeekRaceData();
    scriptContext.SetClassContext(pClass);
    spellName = pSdata->Name;
  //  hookParameters[5].Format("%d",m_toHitDiceRoll);
  //  hookParameters[6].Format("%d",m_effectiveTHAC0);
    result = pSdata->RunSpellScripts
                (DOES_SPELL_ATTACK_SUCCEED,
                 ScriptCallback_LookForChar,
                 "YN",
                 "Invoke lingering spell on target");
    if (result.IsEmpty())
    {
      if (pRace != NULL)
      {
        result = pRace->RunRaceScripts
                    (DOES_SPELL_ATTACK_SUCCEED,
                     ScriptCallback_LookForChar,
                     "YN",
                     "Invoke lingering spell on target");
      };
    }
    if (result.IsEmpty())
    {
      if (targ.GetType() == MONSTER_TYPE)
      {
        //int monsterIndex;
        //monsterIndex = targ.m_pCombatant->origKey;
        {
          //MONSTER_DATA *pMonsterData;
          const MONSTER_DATA *pMonsterData;
          //pMonsterData = monsterData.GetMonsterData(monsterIndex);
          pMonsterData = monsterData.PeekMonster(monsterID);
          if (pMonsterData != NULL)
          {
            result = pMonsterData->RunMonsterScripts
                      (DOES_SPELL_ATTACK_SUCCEED,
                       ScriptCallback_LookForChar,
                       "YN",
                       "Invoke lingering spell on target");
          };
        };
      };
    };
    if (result.IsEmpty())
    {

      result = targ.RunCharacterScripts
                (DOES_SPELL_ATTACK_SUCCEED,
                 ScriptCallback_LookForChar,
                 "YN",
                 "Invoke lingering spell on target");
    };

    *spellSucceeded = TRUE;
    if (!result.IsEmpty())
    {
      if (result[0] == 'N')
      {
        *spellSucceeded = FALSE;
      };
    };
    if (displayText != NULL)
    {
      *displayText = hookParameters[5];
    };
  };

  
  if (!(*spellSucceeded)) return activeSpellKey;


  stData.changeResult = 1.0;
  stData.noEffectWhatsoever = FALSE;
  stData.pCaster = this;
  stData.pTarget = &targ;
  stData.pSpell = pSdata;
  stData.success = FALSE;
  if (pSdata->Save_Result != NoSave)
  {
    targ.DoesSavingThrowSucceed(pSdata,
                                stData,
                                targ.GetIsFriendly() == GetIsFriendly(), 
                                this,
                                displayText);
  };
  if (stData.noEffectWhatsoever)
  {
    return activeSpellKey;
  };




  
  
 
  // run script for each effect
  bool effectAdded = false;
  POSITION pos = pSdata->m_EffectedAttributes.GetHeadPosition();
  if (pos != NULL)
  {
    while (pos != NULL)
    {
      SPELL_EFFECTS_DATA saData = *pSdata->m_EffectedAttributes.PeekNext(pos);
      //saData.SourceOfEffect(pSdata->m_gsID, pSdata->LingerOnceOnly ); // spell database key
      saData.SourceOfEffect(pSdata->SpellID(), pSdata->LingerOnceOnly ); // spell database key
      saData.parent = activeSpellKey;
      saData.flags |= SPELL_EFFECTS_DATA::EFFECT_SPELL;
    
      saData.m_pCaster = this;
      saData.m_pTarget = &targ;

      if (saData.flags & SPELL_EFFECTS_DATA::EFFECT_TARGET)
      {
        SCRIPT_CONTEXT scriptContext;
        scriptContext.SetAttackerContext(this);
        scriptContext.SetTargetContext(&targ);
        BOOL activate = TRUE;
        if (saData.ActivationScript().GetLength() > 0)
        {
          activate = (pSdata->ExecuteActivationScript(saData)?TRUE:FALSE);    //true;// xxxxx          
        }
 
        if (activate)
        {
          // 20110201 PRS CalcSpellEffectChangeValue(pSdata,saData,TRUE);            
          targ.CalcSpellEffectChangeValue(pSdata,saData,targ.GetIsFriendly(), this);  
          // 20171205 PRS this was being done twice.  Why?    targ.AddLingeringSpellEffect(saData, this);
          if (targ.AddLingeringSpellEffect(saData, this, "Invoking lingering spell on target"))
          {
            effectAdded = true;
            // force a scan of the current char status
            // Do this at final spell stage  targ.TakeDamage(0, FALSE, pInvokeOptions, &targ==this, NULL);
          }      
        }          
      }
    }
  }
/*
 * else
 * {
 *   if (activeSpellKey >= 0)
 *   {
 *     // no affected attributes, add placeholder
 *     //
 *     // I think this allows the spell end script to be executed
 *     // when the spell expires?
 *     //
 *     // no need to add unless this spell expires
 *     //
 *     SPELL_EFFECTS_DATA saData;
 *     saData.changeResult = 0.0;
 *
 *     // *** PRS 20091019
 *     //saData.SourceSpell_ID() = pSdata->m_gsID; // spell database key
 *     saData.SourceOfEffect(pSdata->m_gsID, pSdata->LingerOnceOnly); // spell database key
 *     // *** PRS 20091019
 *
 *     saData.parent = activeSpellKey;
 *     saData.flags |= SPELL_EFFECTS_DATA::EFFECT_SPELL; 
 *     saData.affectedAttr = "NOP_" + pSdata->Name;
 *     targ.AddSpellEffect(saData, this);
 *   }
 * }
 */

  // add special abilities
  {
    //int PreSpecAbCount = targ.spellEffects.GetCount();
    //pSdata->specAbs.EnableAllFor(&targ);
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetAttackerContext(this);
    scriptContext.SetSpellContext(pSdata);
    scriptContext.SetTargetContext(&targ);
    hookParameters[5].Format("%d", targets.m_MapTargetX);
    hookParameters[6].Format("%d", targets.m_MapTargetY);
    pSdata->RunSpellScripts(INVOKE_SPELL_ON_TARGET, 
                            ScriptCallback_RunAllScripts, 
                            NULL, 
                            "Invoke lingering spell on target");
    //int PostSpecAbCount = targ.spellEffects.GetCount();
    //int Diff = PostSpecAbCount - PreSpecAbCount;
    //if (Diff > 0)
    //{
      //POSITION pos2 = targ.spellEffects.GetTailPosition();  
      //while ((pos2 != NULL) && (Diff > 0))
      //{        
        //targ.spellEffects.GetAt(pos2).flags |= SPELL_EFFECTS_DATA::EFFECT_SPELLSPECAB;
        //targ.spellEffects.GetAt(pos2).SourceSpell_ID() = pSdata->m_gsID;
        //if (activeSpellKey >= 0)
          //targ.spellEffects.GetAt(pos2).parent = activeSpellKey;
        //targ.spellEffects.GetPrev(pos2);
        //Diff--;
      //}
    //}
    if (pInvokeOptions != NULL)
    {
      if (targ.hitPoints <= -10)
      {
        pInvokeOptions->m_whatSpriteNeeded = DeathSprite;
      }
      else if (targ.hitPoints <= 0)
      {
        pInvokeOptions->m_whatSpriteNeeded = DyingSprite;
      };
    };
  };

  //ClearTargetContext();
  //ClearCharContext();      


  // Only permanent effects are allowed.
  // A lingering spell cannot create another lingering effect.
/*
 *  if (pSdata->Duration_Rate != Permanent) // Instantaneous = permanent = never expires
 * {            
 *   ASSERT( activeSpellKey >= 0);
 *
 *   // only need to add one active spell record for each
 *   // spell cast by caster.
 *   if (activeSpellList.Get(activeSpellKey) == NULL)
 *   {
 *     ACTIVE_SPELL aspell;
 *     GetContext(&aspell.caster, pSdata->schoolMask);
 *     aspell.caster = actor;
 *     aspell.key = activeSpellKey;
 *     aspell.spellKey = pSdata->m_gsID; // spell db key
 *     aspell.CountTime=0;
 *     aspell.StopTime = 0;
 *     aspell.Lingers = (IsCombatActive() ? pSdata->Lingers : FALSE); // can't linger if not in combat
 *     aspell.lingerData.OnceOnly = pSdata->LingerOnceOnly;
 *
 *     if (aspell.Lingers)
 *     {
 *       // add affected map locations to active spell record
 *       aspell.lingerData.mapdata.RemoveAll();
 *       POSITION pos = targets.m_maptarglist.GetHeadPosition();
 *       while (pos != NULL)
 *         aspell.lingerData.mapdata.AddTail(targets.m_maptarglist.GetNext(pos));
 *
 *       // add targets to active spell record so that linger spell checks
 *       // won't re-target same dudes once they move (unless allowed by OnceOnly flag)
 *       for (int i=0;i<targets.m_targets.GetSize();i++)
 *         aspell.lingerData.affectedtargs.AddTail(targets.m_targets[i]);
 *     }
 *
 *     WriteDebugString("Adding active spell cast by %i\n", aspell.caster.Instance);
 *
 *     double val = 0.0;
 *     int casterLevel = -1;
 *     // determine duration value
 *
 *     
 *     {
 *       CString result;
 *       HOOK_PARAMETERS hookParameters;
 *       SCRIPT_CONTEXT scriptContext;
 *       scriptContext.SetSpellContext(pSdata);
 *       scriptContext.SetAttackerContext(this);
 *       result = pSdata->specAbs.RunScripts(SPELL_CASTER_LEVEL, 
 *                                           ScriptCallback_RunAllScripts, 
 *                                           NULL, 
 *                                           "Spell", 
 *                                           pSdata->Name);
 *       if (result.GetLength()> 0)
 *       {
 *         sscanf(result,"%d",&casterLevel);
 *       };
 *     };
 *     
 *     
 *     val = EvalDiceAsClass(pSdata->Duration, pSdata->schoolMask, &casterLevel);*
 *
 *
 *     {
 *       CString result;
 *       HOOK_PARAMETERS hookParameters;
 *       SCRIPT_CONTEXT scriptContext;
 *       scriptContext.SetSpellContext(pSdata);
 *       scriptContext.SetAttackerContext(this);
 *       hookParameters[5].Format("%d", int(val));
 *       result = pSdata->specAbs.RunScripts(SPELL_DURATION, 
 *                                           ScriptCallback_RunAllScripts, 
 *                                           NULL, 
 *                                           "Spell", 
 *                                           pSdata->Name);
 *       if (result.GetLength()> 0)
 *       {
 *         sscanf(result,"%lf",&val);
 *       };
 *     };
 * 
 *
 *
 *     // convert duration value into game minutes
 *     switch (pSdata->Duration_Rate)
 *     {
 *     case inRounds:
 *       val = max(1,val); // 1 minute min
 *       aspell.StopTime = val+party.getElapsedMinutes();        
 *       break;
 *     case byNbrAttacks:
 *     case byDamageTaken:      
 *       aspell.StopTime = val;
 *       break;
 *     case inHours:
 *       val = val * 60.0;
 *       val = max(1,val); // 1 round min
 *       aspell.StopTime = val+party.getElapsedMinutes();        
 *       break;
 *     case inDays:
 *       val = val * 24.0 * 60.0;
 *       val = max(1,val); // 1 round min
 *       aspell.StopTime = val+party.getElapsedMinutes();        
 *       break;
 *     default: 
 *       ASSERT(FALSE); 
 *       break;
 *     }
 *       
 *     activeSpellList.AddWithCurrKey(aspell);
 *   }
 * }
 */

  DisplayCurrSpellEffects("InvokeSpellOnTarget");
  return activeSpellKey;
}









//*****************************************************************************
//    NAME: CHARACTER::SpellActivate
//
// PURPOSE: This function is invoked from the ::SpellActivate() in globals.cpp
//
//*****************************************************************************
void CHARACTER::SpellActivate(const PENDING_SPELL    &data, 
                              InvokeSpellOptions     *invokeOptions, 
                              bool                    abortIfSetPartyXY,
                              ToHitComputation       *pToHitComputation)
{
  // this is for non-combat only
  ASSERT( IsCombatActive() == FALSE );
  ASSERT( GetType() != MONSTER_TYPE );

  CHARACTER_SPELL cspell;
  //if (!m_spellbook.GetSpell(GetSpellBookKey(data.gsID, FALSE), cspell)) 
  //{
  //  ASSERT(FALSE);
  //  return;
  //}

  //SPELL_DATA *pSdata = spellData.GetSpellData(data.gsID);
  SPELL_DATA *pSdata = spellData.GetSpell(data.spellID);
  if (pSdata==NULL) 
  {
    die(0xab50a);
    return;
  }
#ifdef SpellInitiationScript
  pSdata->ExecuteSpellInitiationScript(this);
  int activeSpellKey = -2;  // Indicate that SpellInititionScript has been run
#endif



  
#ifndef SpellInitiationScript 
  // Let activeSpellKey be -1 unless something is actually added
  // to the activeSpellList.  In that way, we will know whether to
  // run the SpellTerminationScripts now or rely on the code that
  // removes the activeSpellEntry to run the SpellTerminationScript.
  if (pSdata->Duration_Rate != Permanent)
    activeSpellKey = activeSpellList.GetNextKey();
#endif
 
  // in non-combat mode, all targets are party members
  for (int i=0;i<targets.NumTargets();i++)
  {
    int targIndex=-1;
    for (int t=0;t<party.numCharacters;t++)
    {
      if (party.characters[t].uniquePartyID == targets.m_targets[i])
      {
        targIndex=t;
        break;
      }
    }            

    if (targIndex < 0)
      continue; // can't find matching target in party

    setPartyXY_x = -1;
    activeSpellKey = InvokeSpellOnTarget(party.characters[targIndex],
                                         activeSpellKey,
                                         //pSdata->m_gsID,
                                         pSdata->SpellID(),
                                         NULL,
                                         NULL,
                                         invokeOptions,
                                         abortIfSetPartyXY,
                                         pToHitComputation);
    if (setPartyXY_x >= 0)
    {
#ifdef SpellInitiationScript
      if (activeSpellKey < 0)
      { // Else removing the activeSpellEntry will run the Termination Script
        pSdata->ExecuteSpellTerminationScript(this);
      };
#endif
      return;
    };
  }

  // Spell Feedback:
  //
  //   Play the spell cast sound regardless of whether spell
  //   actually affected anybody. 
  //
  //   No graphical feedback needed when not in combat
  //
  WriteDebugString("Spell %s activated by %s\n", pSdata->SpellID(), GetName());
  pSdata->PlayCast();
#ifdef SpellInitiationScript
  if (activeSpellKey < 0)
  { // Else removing the activeSpellEntry will run the Termination Script
    if (activeSpellKey != -2)  // Was the SpellInitiationScript executed
    {
      die(0x552d);
    };
    pSdata->ExecuteSpellTerminationScript(this);
    activeSpellKey = -1;
  };
#endif

}

//*****************************************************************************
//    NAME: CHARACTER::CastSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL CHARACTER::CastSpell(GLOBAL_SPELL_ID spell, bool abortIfSetPartyXY)
BOOL CHARACTER::CastSpell(const SPELL_ID& spellID, bool abortIfSetPartyXY, bool LayOrCureOrWhatever)
{ 
  // 'spell' is spell book key
  ASSERT (IsCombatActive() == FALSE);
  if (IsCombatActive()) return FALSE;

  //if (!m_spellbook.GetSpell(GetSpellBookKey(spell, FALSE), cspell)) return FALSE;
  SPELL_DATA *pSdata = spellData.GetSpell(spellID);
  if (pSdata==NULL) return FALSE;

  //m_spellbook.DecMemorized(GetSpellBookID(spellID, FALSE), 1);
  if (!LayOrCureOrWhatever)
  {
    m_spellbook.DecMemorized(spellID, 1);
  };
  ClearCharContextStack();

  PENDING_SPELL data;
  //GetContext(&data.caster, pSdata->schoolMask);
  GetContext(&data.caster, pSdata->schoolID);
  //data.caster.SetFakeCharacterSrc(true);
  //data.gsID = spell;
  if (m_spellCastingLevel != -1)
  {
    data.caster.Level = m_spellCastingLevel;
  };
  data.spellID = spellID;
  data.waitUntil = 0;
  data.flags = pSdata->Casting_Time_Type;

  // combat not active, so cast the
  // spell immediately
  setPartyXY_x = -1;
  ::SpellActivate(data, abortIfSetPartyXY, NULL);
  if (setPartyXY_x >= 0)
  {
    return TRUE;
  };
  return TRUE;
}

//*****************************************************************************
//    NAME: CHARACTER::QueueUsedSpecAb
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::QueueUsedSpecAb(specialAbilitiesType sa, WORD src, GLOBAL_SPELL_ID skey) const
void CHARACTER::QueueUsedSpecAb(specialAbilitiesType sa, WORD src, const SPELL_ID& spellID) const
{
  die("Not Needed?"); //Not Implemented(0xdaf754,FALSE);
}

//*****************************************************************************
//    NAME: CHARACTER::ClearQueuedSpecAb
//
// PURPOSE: 
//
//*****************************************************************************
void CHARACTER::ClearQueuedSpecAb()
{
  specAbQueue.RemoveAll();
}

//*****************************************************************************
//    NAME: CHARACTER::FormatSpecAbMsg
//
// PURPOSE: 
//
//*****************************************************************************
CString CHARACTER::FormatSpecAbMsg(DWORD sa_state)
{
  if (sa_state == SPECAB_MSG_NONE) return "";
  if (specAbQueue.GetCount()==0) return "";

  CString msg("");

  BOOL found=FALSE;
  POSITION pos = specAbQueue.GetHeadPosition();
  while (pos != NULL)
  {
    SQueuedSpecAb data = *specAbQueue.PeekAt(pos);
    specialAbilitiesType stype = (specialAbilitiesType)(data.sa);

    const SPECIAL_ABILITIES *pSpecAbSrc=NULL;
    switch (data.src)
    {
    case SPELL_EFFECTS_DATA::EFFECT_ITEMSPECAB:
      {
        //ITEM_DATA *pItem = itemData.GetItemData(data.SourceItem_ID()); //Investigate someday
        ITEM_DATA *pItem = itemData.GetItem(data.SourceItem_ID()); //Investigate someday
        if (pItem!=NULL) pSpecAbSrc = &pItem->specAbs;
      }
      break;
    case SPELL_EFFECTS_DATA::EFFECT_SPELLSPECAB:
      {
        //SPELL_DATA *pSpell = spellData.GetSpellData(data.SourceSpell_ID());
        SPELL_DATA *pSpell = spellData.GetSpell(data.SourceSpell_ID());
        if (pSpell!=NULL) pSpecAbSrc = &pSpell->specAbs;
      }
      break;
    case SPELL_EFFECTS_DATA::EFFECT_CHARSPECAB:
      {
        pSpecAbSrc=&specAbs;
      }
      break;
    }

    if (pSpecAbSrc!=NULL)
    {
      if (pSpecAbSrc->GetMsgTypes(stype) & sa_state)
      {
        msg = pSpecAbSrc->GetMsgFormat(stype, sa_state);        
        if (msg.GetLength()>0)
        {
          specAbQueue.RemoveAt(pos);
          found=TRUE;
          break;
        }
      }
    }
    specAbQueue.GetNext(pos);
  }

  if (!found) return "";

  CString User("");
  CString Target("self");

  User = GetName(); 

  msg.Replace("/u", User);
  msg.Replace("/t", Target);
  return msg;
}

// give this character a chance to modify the attack dice roll used against
// a target during combat
//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackRollDice
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackRollDice(const CHARACTER *pTarget,const int num, const int sides, int* pBonus) const
{
  BOOL modify=FALSE;
  DWORD src;
  //GLOBAL_SPELL_ID skey;
  SPELL_ID spellID;
  //if (GetAdjSpecAb(SA_Bless,&src,&skey))
  if (GetAdjSpecAb(SA_Bless,&src,&spellID))
  {
    //QueueUsedSpecAb(SA_Bless, src,skey);
    QueueUsedSpecAb(SA_Bless, src,spellID);
    (*pBonus)++;
    modify=TRUE;
  }
  //if (GetAdjSpecAb(SA_Curse,&src,&skey))
  if (GetAdjSpecAb(SA_Curse,&src,&spellID))
  {
    //QueueUsedSpecAb(SA_Curse,src,skey);
    QueueUsedSpecAb(SA_Curse,src,spellID);
    (*pBonus)--;
    modify=TRUE;
  }

  // I'm invisible, so give +2 to dice roll
  if (!pTarget->GetAdjDetectingInvisible())
  {
    //if (GetAdjSpecAb(SA_Invisible,&src,&skey))
    if (GetAdjSpecAb(SA_Invisible,&src,&spellID))
    {
      (*pBonus) += 2;
      //QueueUsedSpecAb(SA_Invisible,src,skey);
      QueueUsedSpecAb(SA_Invisible,src,spellID);
      modify=TRUE;    
    }  
    //else if (GetAdjSpecAb(SA_InvisibleToAnimals,&src,&skey))
    else if (GetAdjSpecAb(SA_InvisibleToAnimals,&src,&spellID))
    {
      if (pTarget->IsAnimal())
      {
        (*pBonus) += 2;
        //QueueUsedSpecAb(SA_InvisibleToAnimals,src,skey);
        QueueUsedSpecAb(SA_InvisibleToAnimals,src,spellID);
        modify=TRUE;
      }
    }
    //else if (GetAdjSpecAb(SA_InvisibleToUndead,&src,&skey))
    else if (GetAdjSpecAb(SA_InvisibleToUndead,&src,&spellID))
    {
      //if (pTarget->GetUndeadType()!=NotUndead)
      if (!pTarget->GetUndeadType().IsEmpty())
      {
        (*pBonus) += 2;
        //QueueUsedSpecAb(SA_InvisibleToUndead,src,skey);
        QueueUsedSpecAb(SA_InvisibleToUndead,src,spellID);
        modify=TRUE;
      }
    }
  }
  
  // Target is invisible, so give -4 to dice roll
  if (!GetAdjDetectingInvisible())
  {
    //if (pTarget->GetAdjSpecAb(SA_Invisible,&src,&skey))
    if (pTarget->GetAdjSpecAb(SA_Invisible,&src,&spellID))
    {
      (*pBonus) -= 4;
      //pTarget->QueueUsedSpecAb(SA_Invisible,src,skey);
      pTarget->QueueUsedSpecAb(SA_Invisible,src,spellID);
      modify=TRUE;    
    }  
    //else if (pTarget->GetAdjSpecAb(SA_InvisibleToAnimals,&src,&skey))
    else if (pTarget->GetAdjSpecAb(SA_InvisibleToAnimals,&src,&spellID))
    {
      if (IsAnimal())
      {
        (*pBonus) -= 4;
        //pTarget->QueueUsedSpecAb(SA_InvisibleToAnimals,src,skey);
        pTarget->QueueUsedSpecAb(SA_InvisibleToAnimals,src,spellID);
        modify=TRUE;
      }
    }
    //else if (pTarget->GetAdjSpecAb(SA_InvisibleToUndead,&src,&skey))
    else if (pTarget->GetAdjSpecAb(SA_InvisibleToUndead,&src,&spellID))
    {
      //if (GetUndeadType()!=NotUndead)
      if (!GetUndeadType().IsEmpty())
      {
        (*pBonus) -= 4;
        //pTarget->QueueUsedSpecAb(SA_InvisibleToUndead,src,skey);
        pTarget->QueueUsedSpecAb(SA_InvisibleToUndead,src,spellID);
        modify=TRUE;
      }
    }
  }
  return modify;
}

// give this character a chance to modify the damage dice roll used against
// a target during combat
//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackDamageDice
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackDamageDice(const CHARACTER *pTarget,
                                       const int num, 
                                       const int sides, 
                                       int* pBonus, 
                                       BOOL* pNonLethal) const
{
  DWORD src;
  //GLOBAL_SPELL_ID skey;
  SPELL_ID spellID;
  BOOL modify=FALSE;
  //if (GetAdjSpecAb(SA_Enlarge,&src,&skey))
  if (GetAdjSpecAb(SA_Enlarge,&src,&spellID))
  {
    double maxdmg = num * sides;
    double bonusdmg = ceil(maxdmg*0.33);
    if (bonusdmg < 1.0) bonusdmg=1.0;
    (*pBonus) += bonusdmg;
    //QueueUsedSpecAb(SA_Enlarge,src,skey);
    QueueUsedSpecAb(SA_Enlarge,src,spellID);
    modify=TRUE;
  }
  //if (GetAdjSpecAb(SA_Reduce,&src,&skey))
  if (GetAdjSpecAb(SA_Reduce,&src,&spellID))
  {
    double maxdmg = num * sides;
    double bonusdmg = ceil(maxdmg*0.33);
    if (bonusdmg < 1.0) bonusdmg=1.0;
    (*pBonus) -= bonusdmg;
    //QueueUsedSpecAb(SA_Reduce,src,skey);
    QueueUsedSpecAb(SA_Reduce,src,spellID);
    modify=TRUE;
  }

  //if (GetAdjClass() == Ranger)
  if (pTarget->HasRangerDmgPenalty())
  {
    //SKILL_ID rangerBonusSkillID;
    const BASE_CLASS_DATA *pBaseclass;
    CLASS_ID adjClassID;
//    double maxRaceAdj, maxBaseclassAdj;
    adjClassID = GetAdjClass();
    //rangerBonusSkillID = Skill_RangerBonusLevel;
    const CLASS_DATA *pClass;

    int rangerBonus;
    pClass = classData.PeekClass(adjClassID);
    rangerBonus = GetAdjSkillValue(Skill_RangerBonusLevel, false, true);
    if ((rangerBonus != NoSkill) && (pClass != NULL))
    {
      int numBaseclass, i;
      SCRIPT_CONTEXT scriptContext;
      HOOK_PARAMETERS hookParameters;
      scriptContext.SetAttackerContext(this);
      scriptContext.SetTargetContext(pTarget);
      hookParameters[5].Format("%d", rangerBonus);
      numBaseclass = pClass->GetCount();
      for(i=0; i<numBaseclass; i++)
      {
        SKILL_ID skillID;
        skillID = Skill_RangerBonusLevel;
        pBaseclass = baseclassData.PeekBaseclass(*pClass->PeekBaseclassID(i));
        if ((pBaseclass != NULL) && (pBaseclass->PeekSkill(skillID) != NULL))
        {
          scriptContext.SetBaseClassContext(pBaseclass);
          pBaseclass->RunBaseClassScripts(RANGER_DAMAGE_BONUS, 
                                          ScriptCallback_RunAllScripts, 
                                          NULL, 
                                          "ModifyAttackDamageDice");
        };
      };      
        rangerBonus = atoi(hookParameters[5]);
    };
    if (rangerBonus != NoSkill)
    {
      *pBonus += rangerBonus;
    };


/*
    int maxLevel = 0;
//    maxRaceAdj = maxBaseclassAdj = NoSkillAdj;
    pClass = classData.PeekClass(adjClassID);
    // Search the attacker's baseclasses for the largest RangerBonusLevel
    if (pClass != NULL)
    {
      int i,n;
      n = pClass->GetCount();
      for (i=0; i<n; i++)
      {
        int index;
        pBaseclassID = pClass->PeekBaseclassID(i);
        index = baseclassData.LocateBaseclass(*pBaseclassID);
        if (index < 0) continue;
        pBaseclass = baseclassData.GetBaseclass(index);
        if (pBaseclass != NULL)
        {
          int currentLevel;
          int rangerBonusLevel;
          SKILL_COMPUTATION SC(this, Skill_RangerBonusLevel, false);
          const RACE_DATA *pRace;
          //rangerBonusLevel = pBaseclass->GetSkillValue(rangerBonusSkillID);
          //pBaseclass->GetSkillValue(rangerBonusSkillID);
          pBaseclass->GetSkillValue(SC);
          rangerBonusLevel = SC.baseVal + 0.5;
          if (rangerBonusLevel == NoSkill) continue;
          pBaseclass->UpdateSkillValue(SC);
          pRace = raceData.PeekRace(race);
          Not Implemented(0x4a08, false);
          if (pRace != NULL)
          {
            //rangerBonusLevel = pRace->UpdateSkillValue(this, rangerBonusSkillID, NULL, rangerBonusLevel, &maxRaceAdj);
            //rangerBonusLevel = pRace->UpdateSkillValue(this, 
            //pRace->UpdateSkillValue(this, 
            pRace->UpdateSkillValue(SC);
          };
          rangerBonusLevel = SC.finalAdjustedValue + 0.5;
          currentLevel = GetCurrentLevel(*pBaseclassID);
          if (currentLevel >= rangerBonusLevel)
          {
            if (currentLevel > maxLevel) maxLevel = currentLevel;
          };
        };
      };
      *pBonus += maxLevel;
    };
*/
  };


  //if (GetAdjSpecAb(SA_Enfeebled,&src,&skey))
  if (GetAdjSpecAb(SA_Enfeebled,&src,&spellID))
  {
    (*pBonus) -= 1;
    //QueueUsedSpecAb(SA_Enfeebled,src,skey);
    QueueUsedSpecAb(SA_Enfeebled,src,spellID);
    modify=TRUE;
  }
  
  return modify;
}

// give this character a chance to modify the attack THAC0 used against
// the target during combat
//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackThac0
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackThac0(const CHARACTER *pTarget,int *pVal) const
{  
#ifdef newCombatant
  if (m_pCombatant != NULL)
  {
    return m_pCombatant->ModifyAttackThac0(pTarget,pVal);
  };
#endif
  //DWORD src;
  //DWORD skey;  
  BOOL modify=FALSE;

  return modify;
}

// give this character a chance to modify the attack dice roll used against
// it during combat
//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackRollDiceAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackRollDiceAsTarget(const CHARACTER *pAttacker,const int num, const int sides, int *pBonus) const
{
  DWORD src;
  //GLOBAL_SPELL_ID skey;
  SPELL_ID spellID;
  BOOL modify=FALSE;
  if (pAttacker->IsAlignmentEvil())
  {
    //if (GetAdjSpecAb(SA_ProtFromEvil,&src,&skey))
    if (GetAdjSpecAb(SA_ProtFromEvil,&src,&spellID))
    {
      (*pBonus) -= 2;
      //QueueUsedSpecAb(SA_ProtFromEvil,src,skey);
      QueueUsedSpecAb(SA_ProtFromEvil,src,spellID);
      modify=TRUE;
    }
  }
  else if (pAttacker->IsAlignmentGood())
  {
    //if (GetAdjSpecAb(SA_ProtFromGood,&src,&skey))
    if (GetAdjSpecAb(SA_ProtFromGood,&src,&spellID))
    {
      (*pBonus) -= 2;
      //QueueUsedSpecAb(SA_ProtFromGood,src,skey);
      QueueUsedSpecAb(SA_ProtFromGood,src,spellID);
      modify=TRUE;
    }
  }

  //if (GetAdjSpecAb(SA_Enfeebled,&src,&skey))
  if (GetAdjSpecAb(SA_Enfeebled,&src,&spellID))
  {
    (*pBonus) -= 2;
    //QueueUsedSpecAb(SA_Enfeebled,src,skey);
    QueueUsedSpecAb(SA_Enfeebled,src,spellID);
    modify=TRUE;
  }
  return modify;
}

// give this character a chance to modify the damage dice roll used against
// it during combat
//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackDamageDiceAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackDamageDiceAsTarget(const CHARACTER *pAttacker,const int num, const int sides, int *pBonus, BOOL *pNonLethal) const
{
  return FALSE;
}

// give this character a chance to modify the attack THAC0 used against
// it during combat
//
// this function is the last one that is always called in an attack sequence,
// the damage roll functions are only called if the attack is successful.
//
//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackThac0AsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackThac0AsTarget(const CHARACTER *pAttacker,int *pVal) const
{
  //DWORD src;
  //DWORD skey;
  BOOL modify=FALSE;
  
  
  //if (   (pAttacker->GetAdjSpecAb(SA_Invisible))
  //    || (pAttacker->GetAdjSpecAb(SA_InvisibleToUndead))
  //    || (pAttacker->GetAdjSpecAb(SA_InvisibleToAnimals)))
  //{
  //  // attacking while invisible removes the invisibility
  //  pAttacker->RemoveAllEffectsFor("$CHAR_INVISIBLE");
  //  pAttacker->RemoveAllEffectsFor("$CHAR_INVISIBLETOANIMALS");
  //  pAttacker->RemoveAllEffectsFor("$CHAR_INVISIBLETOUNDEAD");
  //}  

  return modify;
}

//*****************************************************************************
//    NAME: CHARACTER::ModifyACAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL CHARACTER::ModifyACAsTarget(const CHARACTER *pAttacker, int *pAC, GLOBAL_ITEM_ID giID) const
BOOL CHARACTER::ModifyACAsTarget(const CHARACTER *pAttacker, int *pAC, const ITEM_ID& itemID) const
{
  DWORD src;
  //GLOBAL_SPELL_ID skey;
  SPELL_ID spellID;
  BOOL modify=FALSE;
  if (*pAC > 2)
  {
    //if (GetAdjSpecAb(SA_Shield,&src,&skey))
    if (GetAdjSpecAb(SA_Shield,&src,&spellID))
    {
      //if (skey.IsValidSpell())
      if (spellID.IsValidSpell())
      {
        //ITEM_DATA *pItem = itemData.GetItemData(giID);
        ITEM_DATA *pItem = itemData.GetItem(itemID);
        if (pItem!=NULL)
        {
          switch (pItem->Wpn_Type)
          {
          case NotWeapon: 
          case Bow:
          case Crossbow:
            break;
/*
          case HandBlunt:   *pAC=min(*pAC,4); QueueUsedSpecAb(SA_Shield,src,skey);modify=TRUE;break;
          case HandCutting: *pAC=min(*pAC,4); QueueUsedSpecAb(SA_Shield,src,skey);modify=TRUE;break;
          case HandThrow:   *pAC=min(*pAC,2); QueueUsedSpecAb(SA_Shield,src,skey);modify=TRUE;break;
          case SlingNoAmmo: *pAC=min(*pAC,3); QueueUsedSpecAb(SA_Shield,src,skey);modify=TRUE;break;
          case SpellCaster: *pAC=min(*pAC,3); QueueUsedSpecAb(SA_Shield,src,skey);modify=TRUE;break;
          case Throw:       *pAC=min(*pAC,2); QueueUsedSpecAb(SA_Shield,src,skey);modify=TRUE;break;
          case Ammo:        *pAC=min(*pAC,3); QueueUsedSpecAb(SA_Shield,src,skey);modify=TRUE;break;
*/
          case HandBlunt:       *pAC=min(*pAC,4); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          case HandCutting:     *pAC=min(*pAC,4); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          case HandThrow:       *pAC=min(*pAC,2); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          case SlingNoAmmo:     *pAC=min(*pAC,3); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          case SpellCaster:     *pAC=min(*pAC,3); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          case SpellLikeAbility:*pAC=min(*pAC,3); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          case Throw:           *pAC=min(*pAC,2); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          case Ammo:            *pAC=min(*pAC,3); QueueUsedSpecAb(SA_Shield,src,spellID);modify=TRUE;break;
          }
        }
        else
        {
          *pAC = min(*pAC,4);
          //QueueUsedSpecAb(SA_Shield,src,skey);
          QueueUsedSpecAb(SA_Shield,src,spellID);
          modify=TRUE;
        }
      }
      else
      {
        *pAC = min(*pAC,4);
        //QueueUsedSpecAb(SA_Shield,src,skey);
        QueueUsedSpecAb(SA_Shield,src,spellID);
        modify=TRUE;
      }
    }
  }
  return modify;
}

//*****************************************************************************
//    NAME: CHARACTER::ModifySaveRoll
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifySaveRoll(const CHARACTER *pTarget, int *pRoll) const
{
  return FALSE;
}

//*****************************************************************************
//    NAME: CHARACTER::ModifySaveRollAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifySaveRollAsTarget(const CHARACTER *pAttacker, int *pRoll) const
{
  DWORD src;
  //GLOBAL_SPELL_ID skey;
  SPELL_ID spellID;
  BOOL modify=FALSE;
  if (pAttacker->IsAlignmentEvil())
  {
    //if (GetAdjSpecAb(SA_ProtFromEvil,&src,&skey))
    if (GetAdjSpecAb(SA_ProtFromEvil,&src,&spellID))
    {
      (*pRoll) += 2;
      //QueueUsedSpecAb(SA_ProtFromEvil,src,skey);
      QueueUsedSpecAb(SA_ProtFromEvil,src,spellID);
      modify=TRUE;
    }
  }
  else if (pAttacker->IsAlignmentGood())
  {
    //if (GetAdjSpecAb(SA_ProtFromGood,&src,&skey))
    if (GetAdjSpecAb(SA_ProtFromGood,&src,&spellID))
    {
      (*pRoll) += 2;
      //QueueUsedSpecAb(SA_ProtFromGood,src,skey);
      QueueUsedSpecAb(SA_ProtFromGood,src,spellID);
      modify=TRUE;
    }
  }

  //if (GetAdjSpecAb(SA_Shield,&src,&skey))
  if (GetAdjSpecAb(SA_Shield,&src,&spellID))
  {
    (*pRoll) += 1;
    //QueueUsedSpecAb(SA_Shield,src,skey);
    QueueUsedSpecAb(SA_Shield,src,spellID);
    modify=TRUE;
  }

  //if (GetAdjSpecAb(SA_Displacement,&src,&skey))
  if (GetAdjSpecAb(SA_Displacement,&src,&spellID))
  {
    (*pRoll) += 2;
    //QueueUsedSpecAb(SA_Displacement,src,skey);
    QueueUsedSpecAb(SA_Displacement,src,spellID);
    modify=TRUE;
  }
  return modify;
}

//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackRollDiceForItem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackRollDiceForItem(
                    const CHARACTER *pTarget,
                    //GLOBAL_ITEM_ID giID,
                    const ITEM_ID& itemID,
                    const int num, 
                    const int sides, 
                    int *pBonus,
                    int distance) const
{
//  DWORD src;
//  GLOBAL_SPELL_ID skey;
  BOOL modify=FALSE;
  /*
  if (GetAdjSpecAb(SA_Shillelagh,&src,&skey))
  {
    ITEM_DATA *pItem = itemData.GetItemData(giID);
    if (pItem==NULL) return FALSE;
    if (pItem->Wpn_Type==HandBlunt)
    {
      (*pBonus)++;
      QueueUsedSpecAb(SA_Shillelagh,src,skey);
      modify=TRUE;
    }
  }
  */
  return modify;
}

//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackRollDiceForItemAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackRollDiceForItemAsTarget(const CHARACTER *pAttacker,
                                                    //GLOBAL_ITEM_ID giID,
                                                    const ITEM_ID& itemID,
                                                    const int num, 
                                                    const int sides, 
                                                    int *pBonus) const
{
  return FALSE;
}

//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackDamageDiceForItem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::ModifyAttackDamageDiceForItem(
                      const CHARACTER *pTarget,
                      //GLOBAL_ITEM_ID giID,
                      const ITEM_ID& itemID,
                      const int  num, 
                      const int  sides, 
                      int  *pBonus, 
                      BOOL *pNonLethal,
                      int distance) const
{
//  DWORD src;
//  GLOBAL_SPELL_ID skey;
  weaponClassType weaponType;
  BOOL modify=FALSE;
  //weaponType = itemData.GetWpnType(giID);
  weaponType = itemData.GetWpnType(itemID);
  switch (weaponType)
  {
    case NotWeapon:
    case HandBlunt:
    case HandCutting:
      break;
    case HandThrow:
      if (distance > 0) *pBonus = 0;
      break;
    case SlingNoAmmo:
    case Bow:
    case Crossbow:
    case Throw:
      *pBonus = 0;
      break;
    case Ammo:
      *pBonus = 0;
      break;
    case SpellCaster:
    case SpellLikeAbility:
      break;
  };
  /*
  if (GetAdjSpecAb(SA_Shillelagh,&src,&skey))
  {
    ITEM_DATA *pItem = itemData.GetItemData(giID);
    if (pItem==NULL) return FALSE;
    if (pItem->Wpn_Type==HandBlunt)
    {
      QueueUsedSpecAb(SA_Shillelagh,src,skey);
      modify=TRUE;
      if (pTarget->GetSize() > Medium)
      {
        *pNum = 1;
        *pSides = 4;
        (*pBonus)++;
      }
      else
      {
        *pNum=2;
        *pSides=4;
      }
    }
  }
  */
  return modify;
}

//*****************************************************************************
//    NAME: CHARACTER::ModifyAttackDamageDiceForItemAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
//void CHARACTER::ModifyAttackDamageDiceForItemAsTarget(const CHARACTER *pAttacker,GLOBAL_ITEM_ID giID,const int num, const int sides, int *pBonus, BOOL *pNonLethal) const
//void CHARACTER::ModifyAttackDamageDiceForItemAsTarget(const CHARACTER *pAttacker,GLOBAL_ITEM_ID giID,const int num, const int sides, int *pBonus, BOOL *pNonLethal) const
void CHARACTER::ModifyAttackDamageDiceForItemAsTarget(const CHARACTER *pAttacker,
                                                      //GLOBAL_ITEM_ID giID,
                                                      const ITEM_ID& itemID,
                                                      const int num, 
                                                      const int sides, 
                                                      int *pBonus, 
                                                      BOOL *pNonLethal) const
{
  //DWORD src;
  //GLOBAL_SPELL_ID skey;
  CString sName;

  int relAbs[2];
  relAbs[0] = 0;
  relAbs[1] = *pBonus;
  ITEM_DATA *pItem;
  //pItem = itemData.GetItemData(giID);
  pItem = itemData.GetItem(itemID);
  if (pItem == NULL) return;
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetAttackerContext(pAttacker);
    scriptContext.SetTargetContext(this);
    scriptContext.SetItemContext(pItem);
    RunCharacterScripts(GET_ITEM_TARGET_HIT_BONUS, 
                        ScriptCallback_RelOrAbs, 
                        relAbs, 
                        "Modify attack damage dice for item as target");
    pItem->RunItemScripts(GET_ITEM_TARGET_HIT_BONUS,
                          ScriptCallback_RelOrAbs,
                          relAbs,
                          "Modify attack damage dice for item as target");
  };
  *pBonus = relAbs[1] + relAbs[0];
  return;

  /*
  if (GetAdjSpecAb(SA_HoldPerson,&src,&skey))
  {
    *pBonus = GetHitPoints()+1; // make sure this attack kills target
    //QueueUsedSpecAb(SA_HoldPerson,src,skey); // doesn't really make sense here
    modify=TRUE;    
  }
  else if (GetAdjSpecAb(SA_Sleep,&src,&skey))
  {
    *pBonus = GetHitPoints()+1; // make sure this attack kills target
    //QueueUsedSpecAb(SA_Sleep,src,skey); // doesn't really make sense here
    modify=TRUE;    
  }
  */
    
}

#endif // UAFEngine code

//*****************************************************************************
//    NAME: CHARACTER::IsMammal
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsMammal() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterFormFlags(origIndex) & FormMammal) == FormMammal);
    return ((monsterData.GetMonsterFormFlags(monsterID) & FormMammal) == FormMammal);
  }

  return TRUE;
}

//*****************************************************************************
//    NAME: CHARACTER::IsAnimal
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsAnimal() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterFormFlags(origIndex) & FormAnimal) == FormAnimal);
    return ((monsterData.GetMonsterFormFlags(monsterID) & FormAnimal) == FormAnimal);
  }
  
  return FALSE; 
}

//*****************************************************************************
//    NAME: CHARACTER::IsSnake
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsSnake() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterFormFlags(origIndex) & FormSnake) == FormSnake);
    return ((monsterData.GetMonsterFormFlags(monsterID) & FormSnake) == FormSnake);
  }
  
  return FALSE;  
}

//*****************************************************************************
//    NAME: CHARACTER::IsGiant
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsGiant() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterFormFlags(origIndex) & FormGiant) == FormGiant);
    return ((monsterData.GetMonsterFormFlags(monsterID) & FormGiant) == FormGiant);
  }
  
  return FALSE;  
}

//*****************************************************************************
//    NAME: CHARACTER::IsAlwaysLarge
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::IsAlwaysLarge() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterFormFlags(origIndex) & FormLarge) == FormLarge);
    return ((monsterData.GetMonsterFormFlags(monsterID) & FormLarge) == FormLarge);
  }
  
  return FALSE;  
}

//*****************************************************************************
//    NAME: CHARACTER::HasDwarfACPenalty
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasDwarfACPenalty() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterPenaltyFlags(origIndex) & PenaltyDwarfAC) == PenaltyDwarfAC); 
    return ((monsterData.GetMonsterPenaltyFlags(monsterID) & PenaltyDwarfAC) == PenaltyDwarfAC); 
  }
  
  return FALSE;  
}

//*****************************************************************************
//    NAME: CHARACTER::HasGnomeACPenalty
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasGnomeACPenalty() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterPenaltyFlags(origIndex) & PenaltyGnomeAC) == PenaltyGnomeAC); 
    return ((monsterData.GetMonsterPenaltyFlags(monsterID) & PenaltyGnomeAC) == PenaltyGnomeAC); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::HasDwarfTHAC0Penalty
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasDwarfTHAC0Penalty() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterPenaltyFlags(origIndex) & PenaltyDwarfTHAC0) == PenaltyDwarfTHAC0); 
    return ((monsterData.GetMonsterPenaltyFlags(monsterID) & PenaltyDwarfTHAC0) == PenaltyDwarfTHAC0); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::HasGnomeTHAC0Penalty
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasGnomeTHAC0Penalty() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterPenaltyFlags(origIndex) & PenaltyGnomeTHAC0) == PenaltyGnomeTHAC0); 
    return ((monsterData.GetMonsterPenaltyFlags(monsterID) & PenaltyGnomeTHAC0) == PenaltyGnomeTHAC0); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::HasRangerDmgPenalty
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasRangerDmgPenalty() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterPenaltyFlags(origIndex) & PenaltyRangerDmg) == PenaltyRangerDmg); 
    return ((monsterData.GetMonsterPenaltyFlags(monsterID) & PenaltyRangerDmg) == PenaltyRangerDmg); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::HasPoisonImmunity
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasPoisonImmunity() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterImmunityFlags(origIndex) & ImmunePoison) == ImmunePoison); 
    return ((monsterData.GetMonsterImmunityFlags(monsterID) & ImmunePoison) == ImmunePoison); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::HasDeathImmunity
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasDeathImmunity() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterImmunityFlags(origIndex) & ImmuneDeath) == ImmuneDeath); 
    return ((monsterData.GetMonsterImmunityFlags(monsterID) & ImmuneDeath) == ImmuneDeath); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::HasConfusionImmunity
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasConfusionImmunity() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterImmunityFlags(origIndex) & ImmuneConfusion) == ImmuneConfusion); 
    return ((monsterData.GetMonsterImmunityFlags(monsterID) & ImmuneConfusion) == ImmuneConfusion); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::HasVorpalImmunity
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::HasVorpalImmunity() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterImmunityFlags(origIndex) & ImmuneVorpal) == ImmuneVorpal); 
    return ((monsterData.GetMonsterImmunityFlags(monsterID) & ImmuneVorpal) == ImmuneVorpal); 
  }
  
  return FALSE;    
}

//*****************************************************************************
//    NAME: CHARACTER::CanBeHeldOrCharmed
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::CanBeHeldOrCharmed() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterOptionFlags(origIndex) & OptionCanBeHeldCharmed) == OptionCanBeHeldCharmed); 
    return ((monsterData.GetMonsterOptionFlags(monsterID) & OptionCanBeHeldCharmed) == OptionCanBeHeldCharmed); 
  }
  
  return TRUE;    
}

//*****************************************************************************
//    NAME: CHARACTER::AffectedByDispelEvil
//
// PURPOSE: 
//
//*****************************************************************************
BOOL CHARACTER::AffectedByDispelEvil() const
{
  if (GetType() == MONSTER_TYPE)
  {
    //return ((monsterData.GetMonsterOptionFlags(origIndex) & OptionAffectedByDispelEvil) == OptionAffectedByDispelEvil); 
    return ((monsterData.GetMonsterOptionFlags(monsterID) & OptionAffectedByDispelEvil) == OptionAffectedByDispelEvil); 
  }
  
  return FALSE;    
}

#ifdef UAFEngine
CString CHARACTER::GetHighestLevelBaseclass(void)
{
  int i, n, maxLevel;
  CString result;
  n = baseclassStats.GetSize();
  maxLevel = -1;
  for (i=0; i<n; i++)
  {
    const BASECLASS_STATS *pBaseclassStats;
    pBaseclassStats = PeekBaseclassStats(i);
    if (pBaseclassStats->currentLevel > maxLevel)
    {
      maxLevel = pBaseclassStats->currentLevel;
      result = pBaseclassStats->baseclassID;
    };
  };
  return result;
}
#endif

void CHARACTER::ComputeCombatViewValues(void)
{
#ifdef UAFEngine
  die(0x100223); // Should have been overridden by derived class 'COMBATANT'
#else
  ASSERT(false);
#endif
}

void CHARACTER::ComputeCharacterViewValues(void)
{
  displayValues.AdjMaxMovement = GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, 
                                                   "Compute character view values");
  displayValues.MaxMovement    = GetMaxMovement();
}

#ifdef UAFEngine

void CHARACTER::ProcessLingeringSpellEffects(void)
{
  POSITION pos;
  bool needScan = false;
  pos = m_spellEffects.GetHeadPosition();
  while (pos != NULL)
  {

    SPELL_EFFECTS_DATA *pSE;
    pSE = m_spellEffects.GetAt(pos);
    if ((pSE->flags & SPELL_EFFECTS_DATA::EFFECT_ONCEONLY) == 0)
    {
      // A permanent effect is required.
      SPELL_DATA *pSpData;
      ACTIVE_SPELL *pActiveSpell;
      //pSpData = spellData.GetSpellData(pSE->SourceSpell_ID());
      pSpData = spellData.GetSpell(pSE->SourceSpell_ID());
      pActiveSpell = activeSpellList.Get(pSE->parent);
      if (pActiveSpell != NULL)
      {
        ActorType *pActor;
        pActor = &pActiveSpell->caster;
        if (pActor->instanceType == InstanceType_CombatantIndex)
        {
          CHARACTER *pCaster;
          pCaster = GetCombatant(pActor)->m_pCharacter;
          CalcSpellEffectChangeValue(pSpData, *pSE, GetIsFriendly(), pCaster);
          if (AddLingeringSpellEffect(*pSE, pCaster, "Processing Lingering Spell Effect"))
          {
            needScan = true;
          }      
        };
      };
    };
    m_spellEffects.GetNext(pos);
  };
  if (needScan)
  {
   // force a scan of the current char status
   TakeDamage(0, FALSE, NULL, false, NULL);
  };
}

#endif


#ifdef UAFEDITOR

#include "ConfigFile.h"

CHARACTER tempNPC; // Where we build item.
CHAR_LIST *pCharList=NULL;  // Where data will be put




// Handle clearing and storing of entire NPC object
static void NPCObject(CONFIG_PKT *pkt) {
  //static POSITION position;
  static int numNPC, curNPC;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempNPC.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (tempNPC.GetName().CompareNoCase("template")!=0)
    {
      CHARACTER_ID NPCId;
      NPCId = tempNPC.GetName();
      //if (pItemData->FindName(tempITEM.IdName()).IsValidItem())
      if (pCharList->LocateCharacter(NPCId) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in NPC.txt = %s\n",tempNPC.GetName());
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
        pCharList->AddCharacter(&tempNPC);
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pItemData->GetHeadPosition();
    curNPC = 0;
    numNPC = pCharList->GetCount();
    //tempITEM.Clear();
    //if (position == NULL)
    if (curNPC >= numNPC)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempITEM = pItemData->PeekAtPos(position);
    tempNPC = *pCharList->PeekCharacter(curNPC);
    //pItemData->GetNext(position);
    pkt->objectName=tempNPC.GetName();
    break;
  case CONFIG_FUNC_getNextObject:
    curNPC++;
    //if (position==NULL) 
    if (curNPC >= numNPC)
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempITEM=pItemData->PeekAtPos(position);
      //tempITEM=*pItemData->PeekItemData(position);
      tempNPC = *pCharList->PeekCharacter(curNPC);
      //pItemData->GetNext(position);
      pkt->objectName=tempNPC.GetName();
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}


void DNPCVersion(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->DWordValue = CHARACTER_VERSION;
  }
  else
  {
  };
}

static void DNPCName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempNPC.GetName();
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    pkt->objectName);
  tempNPC.SetName(pkt->objectName);
}

static void DNPCType(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->DWordValue = tempNPC.GetType();
  }
  else
  {
    tempNPC.SetType(pkt->DWordValue);
  };
}

static void DNPCUndeadType(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempNPC.GetUndeadType();
  }
  else
  {
    tempNPC.SetUndead(pkt->value);
  };
}


static void DNPCRace(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempNPC.RaceID();
  }
  else
  {
    RACE_ID raceID;
    raceID = pkt->value;
    tempNPC.RaceID(raceID);
  };
}


static void DNPCGender(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID genders[] =
  {
    CONFIGID( "male"   ,0),
    CONFIGID( "female" ,1),
    CONFIGID( ""       ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,genders,tempNPC.GetGender());
  }
  else
  {
    pkt->status=CONFIG_DECODE_enum (
      pkt->value,
      &pkt->errorMsg,
      genders,
      i);
    tempNPC.SetGender((genderType)i);
  };
}

static void DNPCClass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempNPC.GetClass();
  }
  else
  {
    CLASS_ID classID;
    classID = pkt->value;
    tempNPC.SetClass(classID);
  };
}

static void DNPCAlignment(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID alignments[] =
  {
    CONFIGID("LawfulGood"    ,LawfulGood),
    CONFIGID("NeutralGood"   ,NeutralGood),
    CONFIGID("ChaoticGood"   ,ChaoticGood),
    CONFIGID("LawfulNeutral" ,LawfulNeutral),
    CONFIGID("TrueNeutral"   ,TrueNeutral),
    CONFIGID("ChaoticNeutral",ChaoticNeutral), 
    CONFIGID("LawfulEvil"    ,LawfulEvil),  
    CONFIGID("NeutralEvil"   ,NeutralEvil),
    CONFIGID("ChaoticEvil"   ,ChaoticEvil),
    CONFIGID( ""             ,ChaoticEvil)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,alignments,tempNPC.GetAlignment());
  }
  else
  {
    pkt->status=CONFIG_DECODE_enum (
      pkt->value,
      &pkt->errorMsg,
      alignments,
      i);
    tempNPC.SetAlignment((alignmentType)i);
  };
}

static void DNPCStatus(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID stati[] =
  {
    CONFIGID("Okay"       ,Okay),       
    CONFIGID("Unconscious",Unconscious ),
    CONFIGID("Dead"       ,Dead),
    CONFIGID("Fled"       ,Fled),
    CONFIGID("Petrified"  ,Petrified),
    CONFIGID("Gone"       ,Gone),
    CONFIGID("Animated"   ,Animated),
    CONFIGID("Temp Gone"  ,TempGone),
	  CONFIGID("Running"    ,Running),
    CONFIGID("Dying"      ,Dying),
    CONFIGID( ""          ,Okay)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,stati,tempNPC.GetStatus());
  }
  else
  {
    pkt->status=CONFIG_DECODE_enum (
      pkt->value,
      &pkt->errorMsg,
      stati,
      i);
    tempNPC.SetStatus((charStatusType)i);
  };
}

static void DNPCAllowInCombat(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->boolValue = tempNPC.GetAllowInCombat();
  }
  else
  {
    tempNPC.SetAllowInCombat(pkt->boolValue);
  };
}




#undef df
#undef dh
#define df(a) DNPC##a
#define dh(a) IDS_HELP_NPC_##a




// default define
#define DEFID(a,b,c)    CONFIGID(a, CONFIG_DATA_##b, df(c), dh(c), 1, false, false)


CONFIGID NPCIDs[] =
{
CONFIGID( "" ,CONFIG_DATA_object ,NPCObject),
DEFID ( "name"                    ,string      ,Name),
DEFID ( "version"                 ,long        ,Version),



//    ar << CHARACTER_VERSION;
    //ar << uniqueKey;
    //ar << type;
DEFID ( "type"                    ,long        ,Type),
    //temp = m_race;
    //ar << temp;
    //ar << race;
DEFID ( "race"                    ,string      ,Race),
//    temp = gender;
DEFID ( "gender"                  ,enum        ,Gender),
//    ar << temp;
    //temp = charClass;
    //ar << temp;
    //ar << classID;
DEFID ( "class"                   ,string      ,Class),
//    temp = alignment;
//    ar << temp;
DEFID ( "alignment"               ,enum        ,Alignment),
    
    //ar << allowInCombat;
DEFID ( "allowInCombat"           ,BOOL        ,AllowInCombat),

//    temp = status;
//    ar << temp;
DEFID ( "status"                  ,enum        ,Status),
    //temp = undead;
    //temp = undeadLevel;
//    ar << undeadType;
#ifdef finished
DEFID ( "undeadType"              ,string      ,UndeadType),
//    temp = creatureSize;
//    ar << temp;
DEFID ( "size"                    ,enum        ,Size),

//    AS(ar,name);
//    ar << THAC0;
DEFID ( "THAC0"                   ,long        ,THAC0),
//    ar << morale;
DEFID ( "morale"                  ,long        ,Morale),
//    ar << encumbrance;
DEFID ( "encumbrance"             ,long        ,Encumbrance),
    //ar << maxEncumbrance;
DEFID("max encumbrance",long,MaxEncumbrance),
    ar << m_AC;
DEFID("armor class",long,ArmorClass),
    ar << hitpoints;
DEFID( "hitPoints",long,HitPoints),
    ar << maxHitPoints;
DEFID( "max hitpoints",long,MaxHitPoints),
    ar << nbrHitDice;
DEFID("nbr hit dice",long,NbrHitDice),
    ar << age;
DEFID("age",long,Age),
    ar << maxAge;
DEFID("max age",long,MaxAge),
    ar << birthday;
DEFID("birthday",long,Birthdat),
    ar << m_iMaxCureDisease;
DEFID("max cure disease",long,MaxCureDisease),
    ar << unarmedDieS;
DEFID("unarmed dice small",dice,UnarmedDiceSmall),
    ar << unarmedNbrDieS;
    ar << unarmedBonus;
    ar << unarmedDieL;
    ar << unarmedNbrDieL;
DEFID("unarmed dice large",dice,UnarmedDiceLarge),
    //ar << ItemMask;
    ar << maxMovement;
DEFID("max movement",long,MaxMovement),
    ar << readyToTrain;
DEFID("ready to train",BOOL,ReadyToTrain),
    ar << canTradeItems;   
DEFID("can trade items",BOOL,CanTradeItems),
    
    ar << GetStr();
DEFID("strength",long,Strength),
    ar << GetStrMod();
DEFID("strength mod",long,StrengthMod),
    ar << GetInt();
DEFID("intelligence",long,Intellignece),
    ar << GetWis();
DEFID("wisdom",long,Wisdom),
    ar << GetDex();
DEFID("dexterity",long,Dexterity),
    ar << GetCon();
DEFID("constitution",long,Constitution),
    ar << GetCha();
DEFID("charisma",long,Charisma),
    ar << openDoors;
DEFID("open doors",long,OpenDoors),
    ar << openMagicDoors;
DEFID("open magic doors",long,OpenMagicDoors),
    ar << BB_LG;
DEFID("bend bars lift gates",long,BendBarsLiftGates),
    ar << hitBonus;
DEFID("hit bonus",long,HitBonus),
    ar << dmgBonus;
DEFID("damage bonus",ling,DamageBonus),
    ar << magicResistance;
DEFID("baseclass",string,Baseclass),

//    ar << IsPreGen;
DEDFID("pregen",BOOL,IsPreGen),
    ar << CanBeSaved;
DEFID("can be saved",BOOL,CanBeSaved),
    ar << HasLayedOnHandsToday;
DEFID("has laid today",BOOL,HasLayedOnHandsToday),
    money.Serialize(ar, version);
DEFID("money",string,Money),
    ar << NbrAttacks;
DEFID("number of attacks",long,NuberOfAttacks),
    icon.Serialize(ar,version, rte.IconArtDir());
DEFID("icon",string,Icon),
    ar << iconIndex;
DEFID("icon index",long,IconIndex),
    ar << origIndex;
DEFID("original index",long,OrigIndex),
    ar << uniquePartyID;
DEFID("unique party id",long,UniquePartyID),
    ar << DisableTalkIfDead;
DEFID("disable talk if dead",BOOL,DisableTalkIfDead),
    ar << TalkEvent;
DEFID("talk event",long,TalkEvent),
    AS(ar, TalkLabel);
DEFID("talk label",string,TalkLabel),
    ar << ExamineEvent;
DEFID("examine event",long,ExamineEvent),
    AS(ar, ExamineLabel);
DEFID("examine label",string,ExamineLabel),
    
    //ar << spells;
    m_spellbook.Serialize(ar, version);
DEFID("spell",string,Spell),

    //ar << detectingMagic;
    ar << detectingInvisible;
DEFID("detecting invisible",BOOL,DetectingInvisible),
    ar << detectingTraps;
DEFID("detecting traps",BOOL,DetectingTraps),
    ar << m_spellEffects.GetCount();
    POSITION pos = m_spellEffects.GetHeadPosition();
    while (pos!=NULL)
      m_spellEffects.GetNext(pos).Serialize(ar,version);
DEFID("spell effect",string,SpellEffect),
    blockageData.Serialize(ar);
DEFID("blockage",string,BlockageData),

#endif //finished  - awaiting Rainbower
CONFIGID( "" ,CONFIG_DATA_end ,NULL)
};



#endif


