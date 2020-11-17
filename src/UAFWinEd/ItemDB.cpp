/******************************************************************************
* Filename: ItemDB.cpp
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
//#include "..\Shared\Version.h"
#include "resource.h"

//#include "UAFWinEd.h"

//#include "externs.h"
#include "SoundMgr.h"
#include "items.h"
#include "spell.h"
#include "char.h"
#include "monster.h"
//#include "picslot.h"

//#include <math.h>
#include "class.h"
#include "itemdb.h"
#include "ConfigFile.h"
#include "level.h"
#include "GlobalData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CStringFromBase38(unsigned int v);
unsigned int CString2Base38(const CString& str);

LEVEL tempLEVEL;
LEVEL *pLevelData = NULL;


int long2log(int n)
{ // convert to integer whose decimal form is:
  //   Exxx  where xxx is an integer and E is a power of 10.
  int E;
  for (E=0; n>999; E+=1000, n/=10)
    ;
  return E+n;
}

int EncodeBYTE(int k, char *r)
{
  int i = 0;
  if (k >= 100)
  {
    r[i++] = k/100 + '0';
    k = k % 100;
    r[i++] = k/10 + '0';
    k = k % 10;
    r[i++] = k + '0';
    return i;
  };
  if (k >= 10)
  {
    r[i++] = k/10 + '0';
    k = k % 10;
  };
  r[i++] = k + '0';
  return i;
}

void ReadWriteSpecialAbilities(CONFIG_PKT *pkt, SPECIAL_ABILITIES *pSpecAb)
{
  if (pkt->IsStoring)
  {
    const CStringPAIR *pEntry;
    if (pkt->listNum == 0) pkt->listPos = pSpecAb->GetHeadPosition();

    if (pkt->listPos == NULL)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    pEntry = pSpecAb->GetNextData(pkt->listPos);
    pkt->value = pEntry->Key();;
    if (!pEntry->Value().IsEmpty())
    {
      pkt->value += ',';
      pkt->value += pEntry->Value();
    };
    if (pkt->listPos != NULL)
    {
      pkt->status = CONFIG_STAT_more;
    };
  }
  else
  {
    int index;
    CString name, param;
    index = pkt->value.Find(',');
    if (index < 0)
    {
      name = pkt->value;
      param = "";
    }
    else
    {
      name = pkt->value.Left(index);
      param = pkt->value.Right(pkt->value.GetLength()-index-1);
    };
    pSpecAb->InsertAbility(name, param,"","");
  };
}


void ReadWriteASL(CONFIG_PKT *pkt, A_ASLENTRY_L *pASL)
{
  if (pkt->IsStoring)
  {
    const ASLENTRY *pEntry;
    if (pkt->listNum == 0) pkt->listPos = pASL->GetStartPosition();

    if (pkt->listPos == NULL)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    pASL->GetNextAssoc(pkt->listPos, &pEntry);
    pkt->value.Format("%s,%s,0x%02x",pEntry->Key(), pEntry->Value(), pEntry->Flags());
    if (pkt->listPos != NULL)
    {
      pkt->status = CONFIG_STAT_more;
    };
  }
  else
  {
    int index, flags;
    CString name, param;
    index = pkt->value.Find(',');
    if (index < 0)
    {
      pkt->status = CONFIG_STAT_value;
      return;
    }
    else
    {
      CString tail;
      name = pkt->value.Left(index);
      tail = pkt->value.Right(pkt->value.GetLength()-index-1);
      index = tail.Find(',');
      if (index < 0)
      {
        pkt->status = CONFIG_STAT_value;
        return;
      }
      else
      {

        param = tail.Left(index);
        tail = tail.Right(tail.GetLength()-index-1);
        tail.TrimLeft();
        if (sscanf(tail, "0x%x", &flags) != 1)
        {
          pkt->status = CONFIG_STAT_value;
          return;
        }
      };
    };
    pASL->Insert(name, param, flags);
  };
}


void CONFIG_PKT::AddLongValue(int n)
{
  CString v;
  if (value.GetLength() == 0)
  {
    v.Format("%d", n);
  }
  else
  {
    v.Format(",%d",n);
  };
    value += v;
}

char *EncodeArray(int *values, int numValue, char *dst, int *sum)
{
  int s;
  char sign;
  s = *sum;
  while (numValue > 0)
  {
    int v;   // value
    v = values[--numValue];
    if (v < 0)
    {
      sign = '-';
      v = -v;
    }
    else
    {
      sign = 0;
    };
    s += v;
    if (s)
    {
      do
      {
        *(--dst) = v%10+'0';
        v /= 10;
      } while (v);
      if (sign) *(--dst) = sign;
      *(--dst) = ',';
    };
  };
  *sum = s;
  return dst;
}




char *PrependString(const CString& str, char *dst)
{
  int n;
  n = str.GetLength();
  dst -= n;
  memcpy(dst, (LPCTSTR)str, n);
  *(--dst) = ',';
  return dst;
}


const char *GetPrefix(CONFIG_PKT *pkt, const char *src, CString *str)
{
  int n = 0;
  while ( (src[n] != 0) && (src[n] != ',') ) { n++;};
  *str = CString(src, n);
  src += n;
  return (src[0]==0) ? src : src+1;
}

const char *DecodeArray(CONFIG_PKT *pkt, const char *src, int *values, int numValue)
{
  bool validValue;
  int  *lastValue;
  lastValue = NULL;
  while (numValue > 0)
  {
    int sign = 1;
    validValue = false;
    *values = 0;
    while (*src == ' '){src++;};
    while ( (*src != 0) && (*src != ',') )
    {
      if ( (*src < '0') || (*src > '9') )
      {
        if (*src == '-') 
        {
          sign = -1;
          src++;
          continue;
        };
        pkt->status = CONFIG_STAT_value;
        return src;
      };
      validValue = true;
      *values = *values * 10 + *src - '0';
      src++;
    };
    if (validValue)
    {
      *values *= sign;
      lastValue = values;
    }
    else
    {
      if (lastValue != NULL)
      {
        *values = *lastValue;
      };
    };
    numValue--;
    values++;
    if (*src != 0) src++;
  };
  return src;
};




void SKILL::Encode(CONFIG_PKT *pkt) const
{
  pkt->value.Format("%s,%d", skillID, value);
  pkt->status = CONFIG_STAT_more;
}

void SKILL::Decode(CONFIG_PKT *pkt)
{
  const char *src;
  CString type;
  int v;
  src = (LPCSTR)pkt->value;
  src = GetPrefix(pkt, src, &skillID);
  src = DecodeArray(pkt, src, &v, 1);
  value = v;
}


void SKILL_ADJUSTMENT_BASECLASS::Encode(CONFIG_PKT *pkt) const
{
  int i, sum;
  char *dst;
  char line[800];
  int values[HIGHEST_CHARACTER_LEVEL];
  dst = line + 800;
  *(--dst) = 0;
  for (i = HIGHEST_CHARACTER_LEVEL-1; i>=0; i--)
  {
    values[i] = skillAdj[i];
  };
  sum = 0;
  dst = EncodeArray(values, HIGHEST_CHARACTER_LEVEL, dst, &sum);
  if (sum == 0)
  {
    pkt->status = CONFIG_STAT_skip;
    return;
  };
  *(--dst) = adjType;
  *(--dst) = ',';
  dst = PrependString(baseclassID, dst);
  dst = PrependString(skillID, dst);
  pkt->value = dst+1;  
  pkt->status = CONFIG_STAT_more;
  return;
}

void SKILL_ADJUSTMENT_BASECLASS::Decode(CONFIG_PKT *pkt)
{
  const char *src;
  CString type;
  src = (LPCSTR)pkt->value;
  src = GetPrefix(pkt, src, &skillID);
  src = GetPrefix(pkt, src, &baseclassID);
  int i;
  baseclassID = baseclassID;
  skillID = skillID;
  int values[HIGHEST_CHARACTER_LEVEL];
  src = GetPrefix(pkt, src, &type);
  if (!type.IsEmpty())
  {
    adjType = type[0];
  }
  else
  {
    adjType = ' ';
  };
  src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_LEVEL);
  for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
  {
    skillAdj[i] = values[i];
  };
}

void SKILL_ADJUSTMENT_ABILITY::Encode(CONFIG_PKT *pkt) const
{
  NotImplemented(0x14a45, false);
}

void SKILL_ADJUSTMENT_ABILITY::Decode(CONFIG_PKT *pkt)
{
  NotImplemented(0x24a65, false);
}

void SKILL_ADJUSTMENT_RACE::Encode(CONFIG_PKT *pkt) const
{
  pkt->value.Format("%s,%s,%c,%d",skillID, raceID, adjType, skillAdj);
}

void SKILL_ADJUSTMENT_RACE::Decode(CONFIG_PKT *pkt)
{
  CString skillName, raceName;
  const char *src;
  int value;
  CString type;
  src = (LPCSTR)pkt->value;
  src = GetPrefix(pkt, src, &skillName);
  src = GetPrefix(pkt, src, &raceName);
  src = GetPrefix(pkt, src, &type);
  if (type.GetLength() == 0) type = "%";
  if (type.GetLength() > 1)
  {
    pkt->status = CONFIG_STAT_semantic;
    return;
  };
  skillID = skillName;
  raceID = raceName;
  src = DecodeArray(pkt, src, &value, 1);
  skillAdj = value;
  if (type.GetLength() > 0)
  {
    adjType = type[0];
  }
  else
  {
    adjType = ' ';
  };
}

void SKILL_ADJUSTMENT_SCRIPT::Encode(CONFIG_PKT *pkt) const
{
  NotImplemented(0x34a66, false);
}

void SKILL_ADJUSTMENT_SCRIPT::Decode(CONFIG_PKT *pkt)
{
  NotImplemented(0x44a67, false);
}



ITEM_DATA tempITEM; // Where we build item.
ITEM_DATA_TYPE *pItemData=NULL;  // Where data will be put

// Handle clearing and storing of entire ITEM_DATA object
static void itemObject(CONFIG_PKT *pkt) {
  //static POSITION position;
  static int numItem, curItem;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempITEM.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (tempITEM.IdName().CompareNoCase("template")!=0)
    {
      ITEM_ID itemID;
      itemID = tempITEM.ItemID();
      //if (pItemData->FindName(tempITEM.IdName()).IsValidItem())
      if (pItemData->LocateItem(itemID) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in items.txt = %s\n",itemID);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
        pItemData->AddItem(tempITEM);
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pItemData->GetHeadPosition();
    curItem = 0;
    numItem = pItemData->GetCount();
    //tempITEM.Clear();
    //if (position == NULL)
    if (curItem >= numItem)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempITEM = pItemData->PeekAtPos(position);
    tempITEM = *pItemData->PeekItem(curItem);
    //pItemData->GetNext(position);
    pkt->objectName=tempITEM.UniqueName();
    break;
  case CONFIG_FUNC_getNextObject:
    curItem++;
    //if (position==NULL) 
    if (curItem >= numItem)
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempITEM=pItemData->PeekAtPos(position);
      //tempITEM=*pItemData->PeekItemData(position);
      tempITEM = *pItemData->PeekItem(curItem);
      //pItemData->GetNext(position);
      pkt->objectName=tempITEM.UniqueName();
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}


static void DitemName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempITEM.UniqueName();
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    pkt->objectName);
  tempITEM.SetUniqueName(pkt->objectName);
}

static void DitemExperience(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_integer(pkt->value,tempITEM.Experience);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000000,
    tempITEM.Experience);
}

static void DitemCost(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Cost);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000000,
    tempITEM.Cost);
}

static void DitemBundleQuantity(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Bundle_Qty);
    return;
  };
	pkt->status=CONFIG_DECODE_long(
		pkt->value,
		pkt->errorMsg,
		1,1000,
		tempITEM.Bundle_Qty, 1);

  if (tempITEM.Bundle_Qty < 1)
    tempITEM.Bundle_Qty = 1;   
}

static void DitemEncumbrance(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Encumbrance);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000000,
    tempITEM.Encumbrance);
}

static void DitemAttackBonus(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Attack_Bonus);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    -2000000000,2000000000,
    tempITEM.Attack_Bonus);
}

static void DitemACbase(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Protection_Base);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    -2000000000,2000000000,
    tempITEM.Protection_Base);
}

static void DitemACbonus(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Protection_Bonus);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    -2000000000,2000000000,
    tempITEM.Protection_Bonus);
}

static void DitemKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_long(pkt->value,tempITEM.m_giID.IllegalConversion());
    //CONFIG_ENCODE_long(pkt->value, -1);
    pkt->status = CONFIG_STAT_end;
    return;
  };
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  -2000000000,2000000000,
  //  tempITEM.Protection_Bonus);
}


static void DitemSmallDamageDice(CONFIG_PKT *pkt)
{
  unsigned int dice=1, sides=1;
  int bonus=0;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dice(
        pkt->value,
        tempITEM.Nbr_Dice_Sm,
        tempITEM.Dmg_Dice_Sm,
        tempITEM.Dmg_Bonus_Sm);
    return;
  };
  pkt->status=CONFIG_DECODE_dice(
    pkt->value,
    pkt->errorMsg,
    dice, sides, bonus);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempITEM.Nbr_Dice_Sm=dice;
    tempITEM.Dmg_Dice_Sm=sides;
    tempITEM.Dmg_Bonus_Sm=bonus;
  };
}

static void DitemLargeDamageDice(CONFIG_PKT *pkt)
{
  unsigned int dice=1, sides=1;
  int bonus=0;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dice(
          pkt->value,
          tempITEM.Nbr_Dice_Lg,
          tempITEM.Dmg_Dice_Lg,
          tempITEM.Dmg_Bonus_Lg);
    return;
  };
  pkt->status=CONFIG_DECODE_dice(
    pkt->value,
    pkt->errorMsg,
    dice, sides, bonus);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempITEM.Nbr_Dice_Lg=dice;
    tempITEM.Dmg_Dice_Lg=sides;
    tempITEM.Dmg_Bonus_Lg=bonus;
  };
}

static void DitemHandsToUse(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Hands_to_Use);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2,
    tempITEM.Hands_to_Use,1);
}


/*
static void DitemSpellLevel(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Spell_Level);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    1,9,
    tempITEM.Spell_Level,0);
}
*/


static void DitemNumberCharges(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.Num_Charges);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000000,
    tempITEM.Num_Charges,0);
}

static void DitemExamineEvent(CONFIG_PKT *pkt)
{
  long temp;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.ExamineEvent);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    -1,2000000000,
    temp,-1);
    tempITEM.ExamineEvent = temp;
}

static void DitemExamineLabel(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempITEM.ExamineLabel;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
	  pkt->value,
	  pkt->errorMsg,
	  tempITEM.ExamineLabel);
}

static void DitemRange(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempITEM.RangeMax);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000,
    tempITEM.RangeMax,1);
}

static void DitemCursed(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempITEM.Cursed) ;
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    tempITEM.Cursed, FALSE);
}
static void DitemNonLethal(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempITEM.IsNonLethal);
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    tempITEM.IsNonLethal, TRUE);
}

static void DitemUsable(CONFIG_PKT *pkt)
{
  BOOL usable;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempITEM.IsUsable());
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    usable,FALSE);
  tempITEM.IsUsable(usable);
}

static void DitemNotMagical(CONFIG_PKT *pkt)
{
  BOOL notMagical;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempITEM.IsNotMagical());
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    notMagical,FALSE);
  tempITEM.IsNotMagical(notMagical);
}


static void DitemScribable(CONFIG_PKT *pkt)
{
  BOOL scribable;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempITEM.IsScribable());
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    scribable,FALSE);
  tempITEM.IsScribable(scribable);
}

static void DitemDroppable(CONFIG_PKT *pkt)
{
  BOOL droppable;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempITEM.CanBeTradeDropSoldDep);
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    droppable,FALSE);
  tempITEM.CanBeTradeDropSoldDep = droppable;
}

static void DitemCanHalveJoin(CONFIG_PKT *pkt)
{
  BOOL halveJoinable;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempITEM.CanBeHalvedJoined);
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    halveJoinable,FALSE);
  tempITEM.CanBeHalvedJoined = halveJoinable;
}


static void DitemReadiedLocation(CONFIG_PKT *pkt)
{
#ifdef OldLocation20180129
  itemReadiedLocation L;
#else
  DWORD L;
#endif
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_enum(pkt->value,carriedIDs,tempITEM.Location_Readied);
    //return;
#ifdef OldLocation20180129
    pkt->value = CStringFromBase38(tempITEM.Location_Readied.Location());
#else
    pkt->value = CStringFromBase38(tempITEM.Location_Readied);
#endif
    return;
  };
  L = CString2Base38(pkt->value);
  tempITEM.Location_Readied=L;
}

static void DitemLocationCarried(CONFIG_PKT *pkt)
{
#ifdef OldLocation20180129
  itemReadiedLocation L;
#else
  DWORD L;
#endif
#ifdef OldLocation20180129
  int i;
  static CONFIGID carriedIDs[] =
  {
    CONFIGID( "weapon hand" ,WeaponHand),
    CONFIGID( "shield"      ,ShieldHand),
    CONFIGID( "armor"       ,BodyArmor),
    CONFIGID( "hands"       ,Hands),
    CONFIGID( "head"        ,Head),
    CONFIGID( "waist"       ,Waist),
    CONFIGID( "body"        ,BodyRobe),
    CONFIGID( "back"        ,Back),
    CONFIGID( "feet"        ,Feet),
    CONFIGID( "fingers"     ,Fingers),
    CONFIGID( "quiver"      ,AmmoQuiver),
    CONFIGID( "none"        ,Cannot),
    CONFIGID( ""            ,0)
  };
#endif
  if (!pkt->IsStoring)
  {
#ifdef OldLocation20180129
    pkt->status=CONFIG_DECODE_enum (
      pkt->value,
      NULL, //pkt->errorMsg,
      carriedIDs,
      i);
    if (pkt->status==CONFIG_STAT_ok)
    {
      L=(itemReadiedLocation)i;
      tempITEM.Location_Readied=L;
    };
#else
    L = CString2Base38(pkt->value);
    tempITEM.Location_Readied = L;
#endif
  }
  else
  {
#ifdef OldLocation20180129
    pkt->status = CONFIG_STAT_end;
#else
    // Not used.  'readied location' is a synonym that is now used.
    //pkt->value = CStringFromBase38(tempITEM.Location_Readied);
    pkt->status = CONFIG_STAT_end;
#endif
  };
}

static void DitemRechargeRate(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID rechargeIDs[] =
  {
    CONFIGID( "never" ,irrNever),
    CONFIGID( "daily" ,irrDaily),
    CONFIGID( ""          ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,rechargeIDs,tempITEM.Recharge_Rate);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    rechargeIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempITEM.Recharge_Rate=(itemRechargeRate)i;
  };
}

static void DitemWeaponType(CONFIG_PKT *pkt)
{
  weaponClassType W;
  int i;
  static CONFIGID typeIDs[] =
  {
    CONFIGID( "not weapon"            ,NotWeapon),
    CONFIGID( "hand held blunt"       ,HandBlunt),
    CONFIGID( "hand held cutting"     ,HandCutting),
    CONFIGID( "hand held or thrown"   ,HandThrow),
    CONFIGID( "sling"                 ,SlingNoAmmo),
    CONFIGID( "bow"                   ,Bow),
    CONFIGID( "crossbow"              ,Crossbow),
    CONFIGID( "thrown only"           ,Throw),
    CONFIGID( "ammo"                  ,Ammo),
    CONFIGID( "SpellCaster"           ,SpellCaster),
    CONFIGID( "Spell-like Ability"    ,SpellLikeAbility),
    CONFIGID( ""          ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,typeIDs,tempITEM.Wpn_Type);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    typeIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    W=(weaponClassType)i;
    tempITEM.Wpn_Type=W;
  };
}


/*
static void DitemSpellClass(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID classIDs[] =
  {
    CONFIGID( "MU"      ,1),
    CONFIGID( "Cleric"    ,2),
    CONFIGID( ""          ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,classIDs,tempITEM.Spell_Class);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    classIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempITEM.Spell_Class=i;
  };
}
*/


static void DitemBaseclass(CONFIG_PKT *pkt)
{
  //unsigned int i;
/*
  static CONFIGID classIDs[] =
  {
    CONFIGID( "MU"      ,MagicUserFlag),
    CONFIGID( "Cleric"  ,ClericFlag),
    CONFIGID( "thief"   ,ThiefFlag),
    CONFIGID( "fighter" ,FighterFlag),
    CONFIGID( "Paladin" ,PaladinFlag),
    CONFIGID( "ranger"  ,RangerFlag),
    CONFIGID( "Druid"   ,DruidFlag),
    CONFIGID( "bit7"    ,0x80),
    CONFIGID( "bit8"    ,0x100),
    CONFIGID( "bit9"    ,0x200),
    CONFIGID( "any"     ,AllClasses),
    CONFIGID( ""    ,0)
  };
*/


  if (pkt->IsStoring)
  {
    //if (tempITEM.Usable_by_Class==AllClasses) 
    if (tempITEM.usableByBaseclass.GetSize() == 0)
    {
      pkt->value="any";
    }
    else
    {
    //CONFIG_ENCODE_flags(pkt->value,
    //                    classIDs,
    //                    tempITEM.Usable_by_Class);
      if (pkt->listNum < 0)
      {
        pkt->listNum = 0;
      };
      if (pkt->listNum >= tempITEM.usableByBaseclass.GetSize())
      {
        pkt->status = CONFIG_STAT_end;
        return;
      };
      pkt->value = tempITEM.PeekBaseclass(pkt->listNum)->UniqueName();
      //pkt->value = tempITEM.usableByBaseclass[pkt->listNum];
      pkt->status = CONFIG_STAT_more;
      return;
    };
    return;
  };
  if ((pkt->value=="") || (pkt->value.CompareNoCase("any") == 0)) {
    //tempITEM.Usable_by_Class=AllClasses;
    tempITEM.usableByBaseclass.RemoveAll();
  }
  else
  {
    //pkt->status=CONFIG_DECODE_flags (
    //  pkt->value,
    //  pkt->errorMsg,
    //  classIDs,
    //  i);
    //if (pkt->status==CONFIG_STAT_ok)
    //{
    //  tempITEM.Usable_by_Class=i;
    //};
    BASECLASS_ID baseclassID;
    baseclassID = pkt->value;
    tempITEM.usableByBaseclass.Add(baseclassID);
  };
}


#ifdef OLD_SPECIAL_ABILITIES
static void DitemSpecialAbility(CONFIG_PKT *pkt)
{
  CList<int, int> *list;
  static CONFIGID abilityIDs[] =
  {
    CONFIGID("none"                 ,SA_None),
    CONFIGID("bless"                ,SA_Bless),
    CONFIGID("Curse"                ,SA_Curse),
    CONFIGID("Undead Fear"          ,SA_UndeadFear),
    CONFIGID("Enlarge"              ,SA_Enlarge),
    CONFIGID("Reduce"               ,SA_Reduce),
    CONFIGID("Charm Person"         ,SA_CharmPerson),
    CONFIGID("Detect Magic"         ,SA_DetectMagic),
    CONFIGID("Reflect Gaze Attack"  ,SA_ReflectGazeAttack),
    CONFIGID("Prot from Evil"       ,SA_ProtFromEvil),
    CONFIGID("Prot from Good"       ,SA_ProtFromGood),
    CONFIGID("Shield"               ,SA_Shield),
    CONFIGID("Sleep"                ,SA_Sleep),
    CONFIGID("Fog"                  ,SA_Fog),
    CONFIGID("Entangle"             ,SA_Entangle),
    CONFIGID("Invisible to Animals" ,SA_InvisibleToAnimals),
    CONFIGID("Invisible to Undead"  ,SA_InvisibleToUndead),
    CONFIGID("Non-Undead Fear"      ,SA_NonUndeadFear),
    CONFIGID("Sanctuary"            ,SA_Sanctuary),
    CONFIGID("Shillelagh"           ,SA_Shillelagh),
    CONFIGID("Displacement"         ,SA_Displacement),
    CONFIGID("Wizardry"             ,SA_Wizardry),
    CONFIGID("Hold Person"          ,SA_HoldPerson),
    CONFIGID("Vorpal Attack"        ,SA_VorpalAttack),
    CONFIGID("Silenced"             ,SA_Silence),
    CONFIGID("Poisoned"             ,SA_Poisoned),
    CONFIGID("Slow Poison"          ,SA_SlowPoison),
    CONFIGID("Mirror Image"         ,SA_MirrorImage),
    CONFIGID("Invisible"            ,SA_Invisible),
    CONFIGID("Enfeebled"            ,SA_Enfeebled),
    CONFIGID("Blindness"            ,SA_Blindness),
    CONFIGID("Diseased"             ,SA_Diseased),
    CONFIGID(""          ,0)
  };
  if (pkt->IsStoring)
  {
    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->value = "";
      pkt->listPos = tempITEM.specAbs.GetHeadPosition();
    }

    if ((pkt->listNum>NUM_SPECIAL_ABILITIES)||(pkt->listPos==NULL))
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    specialAbilitiesType sa = tempITEM.specAbs.GetAt(pkt->listPos);
    CONFIG_ENCODE_enum(
          pkt->value,
          abilityIDs,
          sa);

    tempITEM.specAbs.GetNext(pkt->listPos);
    pkt->status=CONFIG_STAT_more;
    return;

    /*
    if (pkt->listNum<tempITEM.specAbs.specAbs.GetSize())
    {
      CONFIG_ENCODE_enum(
            pkt->value,
            abilityIDs,
            tempITEM.specAbs.specAbs[pkt->listNum]);
      pkt->status=CONFIG_STAT_more;
    } 
    else 
    {
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
    }
    return;
    */
  };
  pkt->status=CONFIG_DECODE_enumList (
    pkt->value,
    pkt->errorMsg,
    abilityIDs,
    &list);
  if ((list!=NULL) && (pkt->status==CONFIG_STAT_ok))
  {
    POSITION pos;
    tempITEM.specAbs.Clear();
    pos=list->GetHeadPosition();
    while (pos!=NULL)
    {
      tempITEM.specAbs.EnableSpecAb((specialAbilitiesType)list->GetAt(pos));
    };
  };
}

#else
static void DitemSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempITEM.specAbs);
}
#endif

static void DitemASL(CONFIG_PKT *pkt)
{
  ReadWriteASL(pkt, &tempITEM.item_asl);
}



static void DitemIDname(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempITEM.IdName();
    return;
  };
  if (pkt->value.CompareNoCase("NA") == 0)
    tempITEM.SetIdName("");
  else
  {
    CString qname;
    qname = tempITEM.IdName();
	  pkt->status=CONFIG_DECODE_string(
		  pkt->value,
		  pkt->errorMsg,
		  qname);
    tempITEM.SetIdName(qname);
  };
}
static void DitemAmmoType(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempITEM.AmmoType;
    return;
  };
  if (   (pkt->value.CompareNoCase("NA")==0)
      || (pkt->value.CompareNoCase("None")==0)
      || (pkt->value==""))
    tempITEM.AmmoType = "";
  else
	  pkt->status=CONFIG_DECODE_string(
		  pkt->value,
		  pkt->errorMsg,
		  tempITEM.AmmoType);
}


static void DitemSpellEffect(CONFIG_PKT *pkt)
{
  CString effect;
  if (pkt->IsStoring)
  {
    SPELL_ID spellID;
    spellID = tempITEM.SpellID();
    //if ((!tempITEM.m_gsID.IsValidSpell()))
    if ((!spellID.IsValidSpell()))
    {
      pkt->value="none";
      return;
    };
/*
    // We need to find the tempITEM.Spell'th entry in spellData.
    POSITION pos=spellData.GetHeadPosition();
    DWORD index = 0;
    while (pos!=NULL)
    {
      if (tempITEM.m_gsID == spellData.PeekAtPos(pos).m_gsID) 
      {
        pkt->value=spellData.PeekAtPos(pos).Name;
        return;
      };
      // PRS 20July2009  What is this?  Should we NextPos(pos) here somewhere???
      spellData.GetNext(pos);
      index++;
    };

    // There was no such spell!!!
    pkt->value.Format("Unknown spell index = 0x%08x",tempITEM.m_gsID);
*/
    pkt->value = spellID.UniqueName();
    return;
  };

  if ((pkt->value=="")||(pkt->value.CompareNoCase("none")==0))
  {
    //tempITEM.m_gsID.Clear();
    tempITEM.spellID.Clear();
  }
  else
  {
    pkt->status=CONFIG_DECODE_string(
      pkt->value,
      pkt->errorMsg,
      effect);
    if (pkt->status==CONFIG_STAT_ok)
    {
      //POSITION pos=spellData.GetHeadPosition();
      //int index=0;
      //while (pos!=NULL) {
        //if (spellData.PeekAtPos(pos).Name==effect) break;
      //  if (spellData.PeekSpellData(pos)->Name==effect) break;
      //  index++;
      //  spellData.GetNext(pos);
      //};
      SPELL_ID spellID;
      spellID = effect;
      if (spellData.LocateSpell(spellID) < 0)
      //if (pos==NULL)
      {
        pkt->errorMsg.Format(
          "No such spell as %s is defined",effect);
        pkt->status=CONFIG_STAT_value;
      }
      else
      {
        // PRS July 2009 tempITEM.Spell=index;
        //tempITEM.m_gsID=spellData.PeekAtPos(pos).m_gsID;
        //tempITEM.spellID=spellData.PeekAtPos(pos).SpellID();
        //tempITEM.spellID=spellData.PeekSpellData(pos)->SpellID();
        tempITEM.spellID = spellID;
      };
    };
  };
}

static void DitemROFperRound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_float(pkt->value,tempITEM.ROF_Per_Round);
    return;
  };
  pkt->status=CONFIG_DECODE_float(
    pkt->value,
    pkt->errorMsg,
    tempITEM.ROF_Per_Round);
}

static void DitemHitSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString path;
    pkt->value = tempITEM.HitSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";          
    return;
  };

	if ((pkt->value == "") || (pkt->value.CompareNoCase("DEFAULT")==0))
	{
		tempITEM.HitSound = "DEFAULT";
	}
	else if (pkt->value.CompareNoCase("none")==0)
	{
		tempITEM.HitSound="";
	}
	else
	{
		pkt->status=CONFIG_DECODE_file(
			pkt->value,
			pkt->errorMsg,
			tempITEM.HitSound);
	};
}

static void DitemMissSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString path;
    pkt->value = tempITEM.MissSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  };
	if ((pkt->value=="") || (pkt->value.CompareNoCase("DEFAULT")==0))
	{
		tempITEM.MissSound = "DEFAULT";
	}
	else if (pkt->value.CompareNoCase("none")==0)
	{
		tempITEM.MissSound="";
	}
	else
	{
		pkt->status=CONFIG_DECODE_file(
			pkt->value,
			pkt->errorMsg,
			tempITEM.MissSound);
	};
}
static void DitemLaunchSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString path;
    pkt->value = tempITEM.LaunchSound;
    if (pkt->value == "") pkt->value = "none";          
    return;
  };

	if ((pkt->value == "") || (pkt->value.CompareNoCase("DEFAULT")==0))
	{
		tempITEM.LaunchSound = "DEFAULT";
	}
	else if (pkt->value.CompareNoCase("none")==0)
	{
		tempITEM.LaunchSound="";
	}
	else
	{
		pkt->status=CONFIG_DECODE_file(
			pkt->value,
			pkt->errorMsg,
			tempITEM.LaunchSound);
	};
}

static void DitemSpriteData(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempITEM.MissileArt.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempITEM.MissileArt.filename;
      return;
    };
    if (tempITEM.MissileArt.filename=="")
    {
      pkt->value="none";
      return;
    };
    CONFIG_ENCODE_SpriteData(
      pkt->value,
			tempITEM.MissileArt.filename,
      tempITEM.MissileArt.timeDelay,
      tempITEM.MissileArt.NumFrames,
      tempITEM.MissileArt.FrameWidth,
      tempITEM.MissileArt.FrameHeight,
      tempITEM.MissileArt.style,
      tempITEM.MissileArt.MaxLoops,
      tempITEM.MissileArt.flags,
      tempITEM.MissileArt.AllowCentering,
      tempITEM.MissileArt.UseAlpha,
      tempITEM.MissileArt.AlphaValue);
    return;
  };
  tempITEM.MissileArt.picType = SpriteDib;
	if (pkt->value.CompareNoCase("DEFAULT")==0)
	{
    tempITEM.MissileArt.filename = "DEFAULT";
	}
	else if ((pkt->value == "") ||
           (pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempITEM.MissileArt.Clear();
	}
	else
	{
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempITEM.MissileArt.filename,
      tempITEM.MissileArt.timeDelay,
      tempITEM.MissileArt.NumFrames,
      tempITEM.MissileArt.FrameWidth,
      tempITEM.MissileArt.FrameHeight,
      tempITEM.MissileArt.style,
      tempITEM.MissileArt.MaxLoops,
      tempITEM.MissileArt.flags,
      tempITEM.MissileArt.AllowCentering,
      tempITEM.MissileArt.UseAlpha,
      tempITEM.MissileArt.AlphaValue);
  };
}
static void DitemHitSpriteData(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempITEM.HitArt.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempITEM.HitArt.filename;
      return;
    };
    if (tempITEM.HitArt.filename=="")
    {
      pkt->value="none";
      return;
    };
    CONFIG_ENCODE_SpriteData(
      pkt->value,
			tempITEM.HitArt.filename,
      tempITEM.HitArt.timeDelay,
      tempITEM.HitArt.NumFrames,
      tempITEM.HitArt.FrameWidth,
      tempITEM.HitArt.FrameHeight,
      tempITEM.HitArt.style,
      tempITEM.HitArt.MaxLoops,
      tempITEM.HitArt.flags,
      tempITEM.HitArt.AllowCentering,
      tempITEM.HitArt.UseAlpha,
      tempITEM.HitArt.AlphaValue);
    return;
  };
  tempITEM.HitArt.picType = SpriteDib;
	if (pkt->value.CompareNoCase("DEFAULT")==0)
	{
    tempITEM.HitArt.filename = "DEFAULT";
	}
	else if ((pkt->value == "") ||
           (pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempITEM.HitArt.Clear();
	}
	else
	{
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempITEM.HitArt.filename,
      tempITEM.HitArt.timeDelay,
      tempITEM.HitArt.NumFrames,
      tempITEM.HitArt.FrameWidth,
      tempITEM.HitArt.FrameHeight,
      tempITEM.HitArt.style,
      tempITEM.HitArt.MaxLoops,
      tempITEM.HitArt.flags,
      tempITEM.HitArt.AllowCentering,
      tempITEM.HitArt.UseAlpha,
      tempITEM.HitArt.AlphaValue);
  };
}

#undef df
#undef dh
#define df(a) Ditem##a
#define dh(a) IDS_HELP_item_##a

// default define
#define DEFID(a,b,c)    CONFIGID(a, CONFIG_DATA_##b, df(c), dh(c), 1, false, false)
// used to specify max number
#define DEFIDn(a,b,c,d) CONFIGID(a, CONFIG_DATA_##b, df(c), dh(c), d, false, false)
// used to disable writes to text file
#define DEFIDz(a,b,c,d) CONFIGID(a, CONFIG_DATA_##b, df(c), dh(c), 1, d, false)
// multi-line items are associated by sequential numbering
#define DEFIDg(a,b,c) CONFIGID(a, CONFIG_DATA_##b, df(c), dh(c), 1, false, true)

CONFIGID itemIDs[] =
{
CONFIGID( "" ,CONFIG_DATA_object ,itemObject),
DEFID ( "name"                    ,string      ,Name),
DEFID ( "Key"                     ,long        ,Key),
DEFID ( "id name"                 ,string      ,IDname),
DEFID ( "launch sound"            ,file        ,LaunchSound),
DEFID ( "hit sound"               ,file        ,HitSound),
DEFID ( "miss sound"              ,file        ,MissSound),
DEFID ( "missile sprite"          ,spritedata  ,SpriteData),
DEFID ( "hit sprite"              ,spritedata  ,HitSpriteData),
DEFID ( "ROF per round"           ,float       ,ROFperRound),
DEFID ( "combat range"            ,long        ,Range),
DEFID ( "cost"                    ,long        ,Cost),
DEFID ( "hands to carry"          ,long        ,HandsToUse),
DEFID ( "baseclass"               ,flags       ,Baseclass),
DEFID ( "small/medium damage dice",dice        ,SmallDamageDice),
DEFID ( "large damage dice"       ,dice        ,LargeDamageDice),
DEFID ( "AC base"                 ,long        ,ACbase),
DEFID ( "AC bonus"                ,long        ,ACbonus),
DEFID ( "attack bonus"            ,long        ,AttackBonus),
DEFID ( "experience"              ,long        ,Experience),
DEFID ( "encumbrance"             ,long        ,Encumbrance),
DEFID ( "cursed"                  ,BOOL        ,Cursed),
DEFID ( "non lethal"              ,BOOL        ,NonLethal),
DEFID ( "readied location"        ,DWORD       ,ReadiedLocation),
DEFID ( "location carried"        ,DWORD       ,LocationCarried), // For backward compatibility
DEFID ( "bundle quantity"         ,long        ,BundleQuantity),
DEFID ( "Special Ability"         ,string      ,SpecialAbility),
DEFID ( "ASL"                     ,string      ,ASL),
DEFID ( "weapon type"             ,enum        ,WeaponType),
DEFID ( "ammo type"               ,string      ,AmmoType),
DEFID ( "usable"                  ,BOOL        ,Usable),
DEFID ( "scribtable"              ,BOOL        ,Scribable),
DEFID ( "not magical"             ,BOOL        ,NotMagical),
DEFID ( "can drop"                ,BOOL        ,Droppable),
DEFID ( "CanHalveJoin"            ,BOOL        ,CanHalveJoin),
DEFID ( "number of charges"       ,long        ,NumberCharges),
DEFID ( "Examine Event ID"        ,long        ,ExamineEvent),
DEFID ( "Examine Label"           ,string      ,ExamineLabel),
DEFID ( "recharge rate"           ,enum        ,RechargeRate),
DEFID ( "spell effect"            ,string      ,SpellEffect),
//DEFID ( "spell level"             ,long        ,SpellLevel),
//DEFID ( "spell class"             ,enum        ,SpellClass),
CONFIGID( "" ,CONFIG_DATA_end ,NULL)
};


int ReadItemDB(CString &file, 
               ITEM_DATA_TYPE *itemsPointer,
               CONFIG_FILE *pConfigFile)
{
  return ReadItemDB((LPCSTR)file, itemsPointer, pConfigFile);
}


int ReadItemDB(LPCSTR file, 
               ITEM_DATA_TYPE *itemsPointer,
               CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CONFIG_FILE myConfigFile;  // In case none is provided.
  CFile dataFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  ITEM_DATA_TYPE oldItems;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  CArchive ar( &dataFile, CArchive::load);
  ar.ReadString(string);
  if (string != "// Item database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
//    AfxGetApp()->WinHelp(IDS_HELP_Monster_Name);
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  oldItems = *itemsPointer;  // Save old preSpellNameKey values
  
  // Now we begin the decode process
  // Get rid of old itemData
  itemsPointer->Clear();
  // Read and store new itemData
  ASSERT (pItemData==NULL); // Nobody should be using it
  pItemData=itemsPointer;
  status=pConfigFile->DecodeFile(itemIDs, true);

  // 
  // Now we will transfer all possible preSpellNameKey values from the
  // old items to the new items.
  {
    int i, n;
    n = oldItems.GetItemCount();
    for (i=0; i<n; i++)
    {
      const ITEM_DATA *pOldItem;
      ITEM_DATA *pNewItem;
      pOldItem = oldItems.PeekItem(i);
      if (pOldItem->preSpellNameKey >= 0)
      {
        pNewItem = itemsPointer->GetItem(pOldItem->ItemID());
        if (pNewItem != NULL)
        {
          if (pNewItem->preSpellNameKey < 0)
          {
            pNewItem->preSpellNameKey = pOldItem->preSpellNameKey;
          };
        };
      };
    };
  };
  
  
  pItemData=NULL; // We are done.
  // substitute default filenames for resources using "DEFAULT"
  itemsPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}

int WriteItemDB(LPCSTR file, ITEM_DATA_TYPE *itemsPointer, CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pItemData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
  itemsPointer->CollapseDefaultFilenames();
  
  pItemData=itemsPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Item database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the items file!!!\r\n");
  status=pConfigFile->WriteFile(ar, itemIDs);


  // substitute default filenames for resources using "DEFAULT"
  itemsPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pItemData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}



//*****************************************************************************
// NAME: ReadMonsterDB
//
// PURPOSE:  
//*****************************************************************************
MONSTER_DATA tempMONSTER;  // Where we build monster
MONSTER_DATA_TYPE *pMonsterData=NULL;

// Handle clearing and storing of entire MONSTER_DATA object
static void monsterObject(CONFIG_PKT *pkt) {
  //static POSITION position=NULL;
  static int indx;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempMONSTER.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    ASSERT(pMonsterData!=NULL);
    // don't add templates to database
    if (tempMONSTER.Name.CompareNoCase("template")!=0)
    {
      //if (pMonsterData->FindName(tempMONSTER.Name)!=0)
      if (pMonsterData->LocateMonster(tempMONSTER.MonsterID()) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in monsters.txt = %s\n",tempMONSTER.Name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
      {
        tempMONSTER.GuessUndeadStatus();
        pMonsterData->AddMonster(&tempMONSTER);
      }
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pMonsterData->GetHeadPosition();
    indx = 0;
    //if (position == NULL)
    if (indx >= pMonsterData->GetCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
   //tempMONSTER.Clear();
    //tempMONSTER.Name="template";
    //tempMONSTER = pMonsterData->PeekAtPos(position);
    tempMONSTER = *pMonsterData->PeekMonster(indx);
    //pMonsterData->GetNext(position);
    pkt->objectName=tempMONSTER.Name;
    break;
  case CONFIG_FUNC_getNextObject:
    indx++;
    //if (position==NULL) {
    if (indx >= pMonsterData->GetCount()) {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempMONSTER=pMonsterData->PeekAtPos(position);
      tempMONSTER = *pMonsterData->PeekMonster(indx);
      //pMonsterData->GetNext(position);
      pkt->objectName=tempMONSTER.Name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}

static void DmonsterName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempMONSTER.Name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempMONSTER.Name);
  pkt->objectName=tempMONSTER.Name;
}

static void DmonsterKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_long(pkt->value,tempMONSTER.key);
    pkt->status = CONFIG_STAT_end;
    return;
  };
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  -2000000000,2000000000,
  //  tempITEM.Protection_Bonus);
}



static void DmonsterIconFile(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString iconFilename;
    if (tempMONSTER.Icon.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempMONSTER.Icon.filename;
      return;
    };
    if (tempMONSTER.Icon.filename=="")
    {
      pkt->value="none";
      return;
    };
    iconFilename = tempMONSTER.Icon.filename;
    StripFilenamePath(iconFilename);
    CONFIG_ENCODE_SpriteData(
                  pkt->value,
			            iconFilename,
      tempMONSTER.Icon.timeDelay,
      tempMONSTER.Icon.NumFrames,
      tempMONSTER.Icon.FrameWidth,
      tempMONSTER.Icon.FrameHeight,
      tempMONSTER.Icon.style,
      tempMONSTER.Icon.MaxLoops,
      tempMONSTER.Icon.flags,
      tempMONSTER.Icon.AllowCentering,
      tempMONSTER.Icon.UseAlpha,
      tempMONSTER.Icon.AlphaValue);
    return;
  };

	if ((pkt->value == "") || (pkt->value.CompareNoCase("DEFAULT")==0))
	{
    tempMONSTER.Icon.filename="DEFAULT";
    tempMONSTER.Icon.picType = IconDib;
    tempMONSTER.Icon.SetDefaults();
	}
	else if ((pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempMONSTER.Icon.Clear();
	}
	else
	{
    // cm_a_ogre.pcx,0,2,48,48,0
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempMONSTER.Icon.filename,
      tempMONSTER.Icon.timeDelay,
      tempMONSTER.Icon.NumFrames,
      tempMONSTER.Icon.FrameWidth,
      tempMONSTER.Icon.FrameHeight,
      tempMONSTER.Icon.style,
      tempMONSTER.Icon.MaxLoops,
      tempMONSTER.Icon.flags,
      tempMONSTER.Icon.AllowCentering,
      tempMONSTER.Icon.UseAlpha,
      tempMONSTER.Icon.AlphaValue);

    if (pkt->status!=CONFIG_STAT_ok)
    {
      tempMONSTER.Icon.picType = IconDib;
      tempMONSTER.Icon.SetDefaults();
      pkt->status=CONFIG_STAT_ok;
    }
  };
  tempMONSTER.Icon.picType = IconDib;
}

static void DmonsterHitSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString path;
    pkt->value = tempMONSTER.HitSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  };
		pkt->status=CONFIG_DECODE_file(
				pkt->value,
				pkt->errorMsg,
				tempMONSTER.HitSound);
		if (tempMONSTER.HitSound.CompareNoCase("none")==0) 
								tempMONSTER.HitSound="";
		else if (tempMONSTER.HitSound=="")
								tempMONSTER.HitSound="DEFAULT";
}

static void DmonsterMissSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString path;
    pkt->value = tempMONSTER.MissSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  };
		pkt->status=CONFIG_DECODE_file(
				pkt->value,
				pkt->errorMsg,
				tempMONSTER.MissSound);
		if (tempMONSTER.MissSound.CompareNoCase("none")==0) 
								tempMONSTER.MissSound="";
		else if (tempMONSTER.MissSound=="")
								tempMONSTER.MissSound="DEFAULT";
}
static void DmonsterMoveSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString path;
    pkt->value = tempMONSTER.MoveSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  };
		pkt->status=CONFIG_DECODE_file(
				pkt->value,
				pkt->errorMsg,
				tempMONSTER.MoveSound);
		if (tempMONSTER.MoveSound.CompareNoCase("none")==0) 
								tempMONSTER.MoveSound="";
		else if (tempMONSTER.MoveSound=="")
								tempMONSTER.MoveSound="DEFAULT";
}
static void DmonsterDeathSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString path;
    pkt->value = tempMONSTER.DeathSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  };
		pkt->status=CONFIG_DECODE_file(
				pkt->value,
				pkt->errorMsg,
				tempMONSTER.DeathSound);
		if (tempMONSTER.DeathSound.CompareNoCase("none")==0) 
								tempMONSTER.DeathSound="";
		else if (tempMONSTER.DeathSound=="")
								tempMONSTER.DeathSound="DEFAULT";
}

static void DmonsterIntelligence(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempMONSTER.Intelligence);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000,
    tempMONSTER.Intelligence,0);
}

static void DmonsterArmorClass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempMONSTER.Armor_Class);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    -200000,2000000,
    tempMONSTER.Armor_Class,0);
}

static void DmonsterMovementRate(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value, tempMONSTER.Movement);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000,
    tempMONSTER.Movement,1);
}

static void DmonsterHitDice(CONFIG_PKT *pkt)
{
  double temp;
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_float(pkt->value, double(tempMONSTER.Hit_Dice));
    return;
  };
  pkt->status=CONFIG_DECODE_float(
    pkt->value,
    pkt->errorMsg,
    temp,2.71828);
  tempMONSTER.Hit_Dice=(float)temp;
}
static void DmonsterUseHitDice(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID ffIDs[] =
  {
    CONFIGID( "no"    ,0),
    CONFIGID( "yes"   ,1),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,ffIDs,tempMONSTER.UseHitDice);
    return;
  }
  pkt->status=CONFIG_DECODE_enum (pkt->value,
                                  &pkt->errorMsg,
                                  ffIDs,
                                  i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempMONSTER.UseHitDice=i;
  };
}
static void DmonsterHitDiceBonus(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempMONSTER.Hit_Dice_Bonus);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000,
    tempMONSTER.Hit_Dice_Bonus,0);
}

static void DmonsterTHAC0(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempMONSTER.THAC0);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    -100,100,
    tempMONSTER.THAC0,10);
}

static void DmonsterNbrAttacks(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring) 
  {
    pkt->value = "";
    return;
  }

  //pkt->errorMsg = "Nbr Attacks is no longer supported, use Attack records instead";
  //pkt->status = CONFIG_STAT_value;

  long Nbr_Attacks=0;
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,1000,
    Nbr_Attacks,1);
  // No certainty about the order in which Nbr_Attacks
  // and AttackDice will be read from the file.
  //
  // if Nbr_Attacks is first, add the attack records
  //
  // if AttackDice is first, there will be just one
  // attack record in the array, but it will need
  // to be duplicated for Nbr_Attacks.
  //
  // if Nbr_Attacks second, set the proper array size
  // and duplicate the first attack record to all
  // others.
  //

  // need at least one attack
  if (Nbr_Attacks <= 0) Nbr_Attacks=1;
  // set the array size to proper value
  if (Nbr_Attacks != tempMONSTER.attackData.GetMonsterAttackDetailsCount())
    tempMONSTER.attackData.SetMonsterAttackDetailsSize(Nbr_Attacks);
  // dup first record to all others of size > 1
  for (int i=1;i<tempMONSTER.attackData.GetMonsterAttackDetailsCount();i++)
    tempMONSTER.attackData.SetMonsterAttackDetailsAt(i,*tempMONSTER.attackData.GetMonsterAttackDetails(0));
}

static void DmonsterAttackDice(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring) 
  {
    pkt->value = "";
    return;
  }
  //pkt->errorMsg = "Attack Dice is no longer supported, use Attack records instead";
  //pkt->status = CONFIG_STAT_value;

  unsigned int dice=1, sides=6;
  int bonus=0;
  pkt->status=CONFIG_DECODE_dice(
    pkt->value,
    pkt->errorMsg,
    dice, sides, bonus);

  // No certainty about the order in which Nbr_Attacks
  // and AttackDice will be read from the file.
  //
  // if Nbr_Attacks is first, add the attack records
  //
  // if AttackDice is first, there will be just one
  // attack record in the array, but it will need
  // to be duplicated for Nbr_Attacks.
  //
  // if Nbr_Attacks second, set the proper array size
  // and duplicate the first attack record to all
  // others.
  //
  if (pkt->status==CONFIG_STAT_ok)
  {
    //MONSTER_ATTACK_DETAILS tmp;
    ATTACK_DETAILS tmp;
    tmp.sides = sides;
    tmp.nbr = dice;
    tmp.bonus = bonus;
    tmp.attackMsg = "attacks";

    if (tempMONSTER.attackData.GetMonsterAttackDetailsCount()==0)
    {
      // read AttackDice first, just add this
      // record and it will be duplicated
      // Nbr_Attack times when that value is
      // determined later.
      tempMONSTER.attackData.Add(tmp);
    }
    else
    {
      // Nbr_Attacks was read first, so update
      // all existing records with the current
      // AttackDice data.
      for (int i=0;i<tempMONSTER.attackData.GetMonsterAttackDetailsCount();i++)
        tempMONSTER.attackData.SetMonsterAttackDetailsAt(i,tmp);
    }
  }
}

static void DmonsterMagicResistance(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempMONSTER.Magic_Resistance);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,1000,
    tempMONSTER.Magic_Resistance,0);
}

static void DmonsterSize(CONFIG_PKT *pkt)
{
  creatureSizeType S;
  int i;
  static CONFIGID sizeIDs[] =
  {
    CONFIGID( "small"     ,Small),
    CONFIGID( "medium"    ,Medium),
    CONFIGID( "large"     ,Large),
    CONFIGID( ""          ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,sizeIDs,tempMONSTER.Size);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    sizeIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    S=(creatureSizeType)i;
    tempMONSTER.Size=S;
  };
}

static void DmonsterMorale(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempMONSTER.Morale);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,1000,
    tempMONSTER.Morale,0);
}

static void DmonsterXPValue(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempMONSTER.XP_Value);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,10000000,
    tempMONSTER.XP_Value,0);
}

#ifdef OLD_SPECIAL_ABILITIES

static void DmonsterSpecialAbility(CONFIG_PKT *pkt)
{
  specialAbilitiesType S;
  int i;
  static CONFIGID abilityIDs[] =
  {
    CONFIGID("none"                 ,SA_None),
    CONFIGID("bless"                ,SA_Bless),
    CONFIGID("Curse"                ,SA_Curse),
    CONFIGID("Undead Fear"          ,SA_UndeadFear),
    CONFIGID("Enlarge"              ,SA_Enlarge),
    CONFIGID("Reduce"               ,SA_Reduce),
    CONFIGID("Charm Person"         ,SA_CharmPerson),
    CONFIGID("Detect Magic"         ,SA_DetectMagic),
    CONFIGID("Reflect Gaze Attack"  ,SA_ReflectGazeAttack),
    CONFIGID("Prot from Evil"       ,SA_ProtFromEvil),
    CONFIGID("Prot from Good"       ,SA_ProtFromGood),
    CONFIGID("Shield"               ,SA_Shield),
    CONFIGID("Sleep"                ,SA_Sleep),
    CONFIGID("Fog"                  ,SA_Fog),
    CONFIGID("Entangle"             ,SA_Entangle),
    CONFIGID("Invisible to Animals" ,SA_InvisibleToAnimals),
    CONFIGID("Invisible to Undead"  ,SA_InvisibleToUndead),
    CONFIGID("Non-Undead Fear"      ,SA_NonUndeadFear),
    CONFIGID("Sanctuary"            ,SA_Sanctuary),
    CONFIGID("Shillelagh"           ,SA_Shillelagh),
    CONFIGID("Displacement"         ,SA_Displacement),
    CONFIGID("Wizardry"             ,SA_Wizardry),
    CONFIGID("Hold Person"          ,SA_HoldPerson),
    CONFIGID("Vorpal Attack"        ,SA_VorpalAttack),    
    CONFIGID("Silenced"             ,SA_Silence),
    CONFIGID("Poisoned"             ,SA_Poisoned),
    CONFIGID("Slow Poison"          ,SA_SlowPoison),
    CONFIGID("Mirror Image"         ,SA_MirrorImage),
    CONFIGID("Invisible"            ,SA_Invisible),
    CONFIGID("Enfeebled"            ,SA_Enfeebled),
    CONFIGID("Blindness"            ,SA_Blindness),
    CONFIGID("Diseased"             ,SA_Diseased),
    CONFIGID(""          ,0)
  };
  
  if (pkt->IsStoring)
  {
    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->value = "";
      pkt->listPos = tempMONSTER.specAbs.GetHeadPosition();
    }

    if ((pkt->listNum>NUM_SPECIAL_ABILITIES)||(pkt->listPos==NULL))
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    specialAbilitiesType sa = tempMONSTER.specAbs.GetAt(pkt->listPos);
    CONFIG_ENCODE_enum(
          pkt->value,
          abilityIDs,
          sa);

    tempMONSTER.specAbs.GetNext(pkt->listPos);
    pkt->status=CONFIG_STAT_more;
    return;

    /*
    if (pkt->listNum >= tempMONSTER.specAbs.specAbs.GetSize())
    {
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    };
    CONFIG_ENCODE_enum(
              pkt->value,
              abilityIDs,
              tempMONSTER.specAbs.specAbs[pkt->listNum]);
    pkt->status=CONFIG_STAT_more;
    return;
    */
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    pkt->errorMsg,
    abilityIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    S=(specialAbilitiesType)i;
    tempMONSTER.specAbs.EnableSpecAb(S);
  };
}
#else
static void DmonsterSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempMONSTER.specAbs);
}
#endif


static void DmonsterUndeadType(CONFIG_PKT *pkt)
{
  //int i;
  //enum undeadType { NotUndead, Skeleton, Zombie, Ghoul, Shadow, Wight, Ghast, Wraith, Mummy,
  //						Spectre, Vampire, Ghost, Lich, Special };
/*
  static CONFIGID abilityIDs[] =
  {
        CONFIGID("none"     ,NotUndead),
        CONFIGID("Skeleton" ,Skeleton),
        CONFIGID("Zombie"   ,Zombie),
        CONFIGID("Ghoul"    ,Ghoul),
        CONFIGID("Shadow"   ,Shadow),
        CONFIGID("Wight"    ,Wight),
        CONFIGID("Ghast"    ,Ghast),
        CONFIGID("Wraith"   ,Wraith),
        CONFIGID("Mummy"    ,Mummy),
        CONFIGID("Spectre"  ,Spectre),
        CONFIGID("Vampire"  ,Vampire),
        CONFIGID("Ghost"    ,Ghost),
        CONFIGID("Lich"     ,Lich),
        CONFIGID("Special"  ,Special),
        CONFIGID( ""        ,0)
  };
*/
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_long(
    //          pkt->value,
    //          tempMONSTER.undeadLevel);
    pkt->value = tempMONSTER.undeadType;
     return;
  }
  
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  0,999, tempMONSTER.undeadLevel,
  //  999);
  tempMONSTER.undeadType = pkt->value;
}


static void DmonsterClass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempMONSTER.classID;
    return;
  };
  {
    tempMONSTER.classID = pkt->value;
  };
}

static void DmonsterRace(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempMONSTER.raceID;
    return;
  };
  {
    tempMONSTER.raceID = pkt->value;
  };
}

/*
static void DmonsterClasses(CONFIG_PKT *pkt)
{
  unsigned int i;
  static CONFIGID classIDs[] =
  {
    CONFIGID( "MU"      ,MagicUserFlag),
    CONFIGID( "Cleric"  ,ClericFlag),
    CONFIGID( "thief"   ,ThiefFlag),
    CONFIGID( "fighter" ,FighterFlag),
    CONFIGID( "Paladin" ,PaladinFlag),
    CONFIGID( "ranger"  ,RangerFlag),
    CONFIGID( "Druid"   ,DruidFlag),
    CONFIGID( "any"     ,AllClasses),
    CONFIGID( ""    ,0)
  };
  if (pkt->IsStoring)
  {
    if (tempMONSTER.ItemMask==AllClasses)
    {
      pkt->value="any";
    }
    else
    {
      CONFIG_ENCODE_flags(pkt->value,classIDs,tempMONSTER.ItemMask);
    };
    return;
  };
  if (pkt->value=="") {
    tempMONSTER.ItemMask=0;
  }
  else
  {
    pkt->status=CONFIG_DECODE_flags (
      pkt->value,
      pkt->errorMsg,
      classIDs,
      i);
    if (pkt->status==CONFIG_STAT_ok)
    {
      tempMONSTER.ItemMask=i;
    } else tempMONSTER.ItemMask=0;
  };
}
*/


static void DmonsterForm(CONFIG_PKT *pkt)
{
  unsigned int i;
  static CONFIGID formIDs[] =
  {
    CONFIGID( "mammal"      ,1),
    CONFIGID( "animal"      ,2),
    CONFIGID( "snake"       ,4),
    CONFIGID( "giant"       ,8),
    CONFIGID( "large"       ,16),
    //CONFIGID( "undead"      ,32),
    CONFIGID( "none"        ,0),
    CONFIGID( ""            ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_flags(pkt->value,formIDs,tempMONSTER.Form_Type);
    return;
  };
  if (pkt->value=="") tempMONSTER.Form_Type=1;
  else
  {
    pkt->status=CONFIG_DECODE_flags (
      pkt->value,
      pkt->errorMsg,
      formIDs,
      i);
    if (pkt->status==CONFIG_STAT_ok)
    {
      tempMONSTER.Form_Type=i;
    };
  };
}

static void DmonsterImmunity(CONFIG_PKT *pkt)
{
  unsigned int i;
  static CONFIGID immunityIDs[] =
  {
    CONFIGID( "poison"        ,1),
    CONFIGID( "death magic"   ,2),
    CONFIGID( "confusion"     ,4),
    CONFIGID( "vorpal weapon" ,8),
    CONFIGID( "none"          ,0),
    CONFIGID( ""              ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_flags(pkt->value,immunityIDs,tempMONSTER.Immunity_Type);
    return;
  };
  if (pkt->value=="") tempMONSTER.Form_Type=1;
  else
  {
    pkt->status=CONFIG_DECODE_flags (
      pkt->value,
      pkt->errorMsg,
      immunityIDs,
      i);
    if (pkt->status==CONFIG_STAT_ok)
    {
      tempMONSTER.Immunity_Type=i;
    };
  };
}


static void DmonsterPenalty(CONFIG_PKT *pkt)
{
  unsigned int i;
  static CONFIGID penaltyIDs[] =
  {
    CONFIGID( "DwarfAC"     ,1),
    CONFIGID( "GnomeAC"     ,2),
    CONFIGID( "DwarfTHAC0"  ,4),
    CONFIGID( "GnomeTHAC0"  ,8),
    CONFIGID( "RangerDmg"   ,16),
    CONFIGID( "none"        ,0),
    CONFIGID( ""            ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_flags(pkt->value,penaltyIDs,tempMONSTER.Penalty_Type);
    return;
  };
  pkt->status=CONFIG_DECODE_flags (
    pkt->value,
    pkt->errorMsg,
    penaltyIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempMONSTER.Penalty_Type=i;
  };
}

static void DmonsterMiscOptions(CONFIG_PKT *pkt)
{
  unsigned int i;
  static CONFIGID miscIDs[] =
  {
    CONFIGID( "can be held/charmed"     ,1),
    CONFIGID( "affected by dispel evil" ,2),
    CONFIGID( "none"                    ,0),
    CONFIGID( ""                        ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_flags(pkt->value,miscIDs,tempMONSTER.MiscOptions_Type);
    return;
  };
  pkt->status=CONFIG_DECODE_flags (
    pkt->value,
    pkt->errorMsg,
    miscIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempMONSTER.MiscOptions_Type=i;
  };
}

static void DmonsterItem(CONFIG_PKT *pkt)
{
  CString iname;
  int quantity;
  if (pkt->IsStoring)
  {
    if (tempMONSTER.myItems.GetCount()==0)
    {
      pkt->value="none";
      return;
    }

    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->listPos = tempMONSTER.myItems.GetHeadPosition();
      pkt->value = "";
    }

    //if (pkt->listPos==NULL)
    if (pkt->listNum >= tempMONSTER.myItems.GetCount())
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    //GLOBAL_ITEM_ID giID = tempMONSTER.myItems.PeekAtPos(pkt->listPos).m_giID;
    //pkt->value = itemData.GetItemUniqueName(giID);
    //ITEM_ID itemID = tempMONSTER.myItems.PeekAtPos(pkt->listPos).itemID;
    ITEM_ID itemID = tempMONSTER.myItems.PeekAtPos(pkt->listPos).itemID;
    quantity = tempMONSTER.myItems.PeekAtPos(pkt->listPos).qty;
    //pkt->value = itemData.GetItemUniqueName(itemID);
    pkt->value.Format("%s,%d", itemData.GetItemUniqueName(itemID), quantity);

    tempMONSTER.myItems.GetNext(pkt->listPos);
    pkt->status=CONFIG_STAT_more;
    return;
  };

  if ((pkt->value=="")||(pkt->value.CompareNoCase("none")==0))
  {
    return;
  }
  else
  {
    int col;
    if ((col = pkt->value.Find(',')) > 0)
    {
      iname = pkt->value.Left(col);
      if (sscanf(pkt->value.GetBuffer(0)+col+1, "%d", &quantity) != 1)
      {
        quantity = 1;
      };
      pkt->status = CONFIG_STAT_ok;
    }
    else
    {
      pkt->status=CONFIG_DECODE_string(
          pkt->value,
          pkt->errorMsg,
          iname);
      quantity = 1;
    };
    
    if (pkt->status==CONFIG_STAT_ok)
    {
      //POSITION pos=itemData.GetHeadPosition();
      //bool found=false;
      //while ((pos!=NULL)&&(!found)) 
      //{
        //const ITEM_DATA *pItem;
      ITEM_ID itemID;
      int idx;
      itemID = iname;
        //pItem = &itemData.PeekAtPos(pos);
        //pItem = itemData.PeekItemData(pos);
      if ((idx=itemData.LocateItem(itemID)) >= 0)
        //if (pItem->UniqueName().CompareNoCase(iname)==0)
      {
          //const ITEM_DATA& idata = itemData.PeekAtPos(pos);
        //const ITEM_DATA& idata = *itemData.PeekItemData(pos);
        //tempMONSTER.myItems.addItem(idata.m_giID,1,idata.Num_Charges,FALSE,idata.Cost);
        //tempMONSTER.myItems.addItem(idata.ItemID(),1,idata.Num_Charges,FALSE,idata.Cost);
        const ITEM_DATA *pItem;
        pItem = itemData.PeekItem(idx);
        tempMONSTER.myItems.addItem(itemID,quantity,pItem->Num_Charges,FALSE,pItem->Cost);
        //found=true;
        return;
      }
        //itemData.GetNext(pos);
      //}

      //if (!found)
      {
        pkt->errorMsg.Format("No such item called %s is defined", iname);
        pkt->status=CONFIG_STAT_value;
      }
    }
  }
}

static void DmonsterAttack(CONFIG_PKT *pkt)
{
  CString iname;
  if (pkt->IsStoring)
  {
    if (tempMONSTER.attackData.GetMonsterAttackDetailsCount()==0)
    {
      pkt->value = "none";
      pkt->status=CONFIG_STAT_ok;
      return;
    }

    if (pkt->listNum>=tempMONSTER.attackData.GetMonsterAttackDetailsCount())
    {
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    CONFIG_ENCODE_MonsterAttack(
            pkt->value,
            tempMONSTER.attackData.PeekMonsterAttackDetails(pkt->listNum));
    pkt->status=CONFIG_STAT_more;
    return;
  }

  if ((pkt->value=="")||(pkt->value.CompareNoCase("none")==0))
  {
    return;
  }
  else
  {
    //MONSTER_ATTACK_DETAILS tmp;
    ATTACK_DETAILS tmp;
    pkt->status=CONFIG_DECODE_MonsterAttack (
                      pkt->value,
                      pkt->errorMsg,
                      &tmp);
    if (pkt->status==CONFIG_STAT_ok)
      tempMONSTER.attackData.Add(tmp);
  }
}

#undef df
#undef dh
#define df(a) Dmonster##a
#define dh(a) IDS_HELP_monster_##a
// defined above // #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))
CONFIGID monsterIDs[] =
{
CONFIGID( "" ,CONFIG_DATA_object ,monsterObject),
DEFID( "name"                    ,string ,Name),
DEFID( "Key"                     ,long   ,Key),
DEFID( "icon file"               ,spritedata   ,IconFile),
DEFID( "miss sound"              ,file   ,MissSound),
DEFID( "hit sound"               ,file   ,HitSound),
DEFID( "move sound"              ,file   ,MoveSound),
DEFID( "death sound"             ,file   ,DeathSound),
DEFID( "intelligence"            ,long   ,Intelligence),
DEFID( "armor class"             ,long   ,ArmorClass),
DEFID( "movement rate"           ,long   ,MovementRate),
DEFID( "hit dice"                ,float  ,HitDice),
DEFID( "treat HD as hit points"  ,enum   ,UseHitDice),
DEFID( "hit dice bonus"          ,long   ,HitDiceBonus),
DEFID( "THAC0"                   ,long   ,THAC0),
DEFIDz( "Number of Attacks"      ,long   ,NbrAttacks, true), // read-only
DEFIDz( "Attack dice"            ,dice   ,AttackDice, true), // read-only
DEFID( "magic resistance"        ,long   ,MagicResistance),
DEFID( "size"                    ,enum   ,Size),
DEFID( "morale"                  ,long   ,Morale),
DEFID( "experience point value"  ,long   ,XPValue),
DEFID( "Special Ability"         ,string ,SpecialAbility),
//DEFID( "classes"                 ,flags  ,Classes),
DEFID( "Class"                   ,string ,Class),
DEFID( "Race"                    ,string ,Race),
DEFID( "form"                    ,flags  ,Form),
DEFID( "penalty"                 ,flags  ,Penalty),
DEFID( "immunity"                ,flags  ,Immunity),
DEFID( "Misc Options"            ,flags  ,MiscOptions),
DEFID( "item"                    ,string ,Item),
DEFID( "attack"                  ,string ,Attack),
DEFID( "Undead"                  ,string ,UndeadType),
CONFIGID( "" ,CONFIG_DATA_end ,NULL)
};



int ReadMonsterDB(CString& file, 
               MONSTER_DATA_TYPE *monstersPointer,
               CONFIG_FILE *pConfigFile)
{
  return ReadMonsterDB((LPCSTR)file,monstersPointer,pConfigFile);
}


int ReadMonsterDB(LPCSTR file, 
               MONSTER_DATA_TYPE *monstersPointer,
               CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CONFIG_FILE myConfigFile;  // In case none is provided.
  CFile dataFile;
  MONSTER_DATA_TYPE oldMonsters;
  CONFIG_ITEM_STATUS status;
  CString string;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  CArchive ar( &dataFile, CArchive::load);
  ar.ReadString(string);
  if (string != "// Monster database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
//    AfxGetApp()->WinHelp(IDS_HELP_Monster_Name);
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old monsterData
  oldMonsters = *monstersPointer;  // Save old data temporarily
  monstersPointer->Clear();
  // Read and store new monsterData
  ASSERT (pMonsterData==NULL); // Nobody should be using it
  pMonsterData=monstersPointer;
  status=pConfigFile->DecodeFile(monsterIDs, true);
  // 
  // Now we will transfer all possible preSpellNameKey values from the
  // old monsters to the new monsters.
  {
    int i, n;
    n = oldMonsters.GetMonsterCount();
    for (i=0; i<n; i++)
    {
      const MONSTER_DATA *pOldMon;
      MONSTER_DATA *pNewMon;
      pOldMon = oldMonsters.PeekMonster(i);
      if (pOldMon->preSpellNameKey >= 0)
      {
        pNewMon = monstersPointer->GetMonster(pOldMon->MonsterID());
        if (pNewMon != NULL)
        {
          if (pNewMon->preSpellNameKey < 0)
          {
            pNewMon->preSpellNameKey = pOldMon->preSpellNameKey;
          };
        };
      };
    };
  };
  pMonsterData=NULL; // We are done.
  // substitute default filenames for resources using "DEFAULT"
  monstersPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


SPECABILITY_DEF tempSPECABILTIY; // Where we build special Ability.
A_SPECABILITY_DEF_L *pSpecAbData=NULL;  // Where data will be put

/*
int ReadSpecialAbilitiesDB(CString &file, 
               ITEM_DATA_TYPE *itemsPointer,
               CONFIG_FILE *pConfigFile)
{
  return ReadSpecialAbilitiesDB((LPCSTR)file, itemsPointer, pConfigFile);
}
*/

bool IsComment(const CString& str)
{
  if (str.GetLength() < 2) return false;
  if (str[0] != '\\') return false;
  if (str[1] != '\\') return false;
  return true;
}

bool IsContinuation(const CString& str)
{
  if (str.GetLength() < 1) return false;
  return str[0] == '-';
}


CONFIG_ITEM_STATUS GetCompleteLine(CONFIG_FILE *pConfig, int objNum, int *lineNum, CString& line)
{
  int numLine;
  CString tLine, error;
  numLine = pConfig->NumberOfItems(objNum);
  while (*lineNum < numLine)
  {
    // Find start-of-line
    tLine = pConfig->GetLineText(objNum, *lineNum);
    (*lineNum)++;
    if (IsComment(tLine)) continue;
    line = tLine;
    while (*lineNum < numLine)
    {
      // Add all continuation lines
      tLine = pConfig->GetLineText(objNum, *lineNum);
      if (!IsContinuation(tLine)) return CONFIG_STAT_ok;
      line += "\r\n" + tLine.Right(tLine.GetLength()-1);
      (*lineNum)++;
    };
  };
  return CONFIG_STAT_ok;
}

int DecodeSpecAbObject(CONFIG_FILE *pConfig, int objNum, SPECABILITY_DEF *pSpecDef, CAR& car)
{
  CString line, error, name, value;
  CONFIG_ITEM_STATUS stat;
  int lineNum;
  pConfig->GetLineInfo(objNum, 0, line, stat, NULL, NULL);
  if (line != "\\(BEGIN)") 
  {
    pConfig->PerhapsLogError(objNum,1,"Expected \"\\(BEGIN)\"");
    return CONFIG_STAT_syntax;
  };
  lineNum = 1;
  while (lineNum < pConfig->NumberOfItems(objNum))
  {
    int col;
    int type;
    int errorLine;
    errorLine = lineNum;
    stat = GetCompleteLine(pConfig, objNum, &lineNum, line);
    if (line.IsEmpty()) continue;
    if (line.Left(6) == "\\(END)") return CONFIG_STAT_ok;
    col = line.Find('=');
    if (col < 0) return CONFIG_STAT_syntax;
    name = line.Left(col);
    name.TrimLeft();
    name.TrimRight();
    value = line.Right(line.GetLength() - col - 1);
    value.TrimLeft();
    value.TrimRight();
    if (name.GetLength() < 1) return CONFIG_STAT_syntax;
    if (name[0] == '[')
    {
      type = SPECAB_SCRIPT;
      if (name.GetLength() < 3) return CONFIG_STAT_syntax;
      if (name[name.GetLength()-1] != ']')
      {
        pConfig->PerhapsLogError(objNum, errorLine, "Expected \"]\" at end of script name");
        return CONFIG_STAT_syntax;
      };
      name = name.Mid(1, name.GetLength()-2);
    }
    else
    if (name[0] == '(')
    {
      type = SPECAB_VARIABLE;
      if (name.GetLength() < 3) return CONFIG_STAT_syntax;
      if (name[name.GetLength()-1] != ')') return CONFIG_STAT_syntax;
      name = name.Mid(1, name.GetLength()-2);
    }
    else if (name[0] == '<')
    {
      type = SPECAB_INTEGERTABLE;
      if (name.GetLength() < 3) return CONFIG_STAT_syntax;
      if (name[name.GetLength()-1] != '>') return CONFIG_STAT_syntax;
      name = name.Mid(1, name.GetLength()-2);
    }
    else
    {
      type = SPECAB_CONSTANT;
    };
    name = car.CanonicalString(name);
    value = car.CanonicalString(value);
    pSpecDef->Insert(name, value, type);
  };
  return CONFIG_STAT_ok;
}
  

int ReadSpecialAbilitiesDB(LPCSTR file, 
               A_SPECABILITY_DEF_L *specAbPointer,
               CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CONFIG_FILE myConfigFile;  // In case none is provided.
  CFile dataFile;
  SPECABILITY_DEF specDef;
  int status = CONFIG_STAT_ok;
  CString string;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  CAR car( &dataFile, CArchive::load);
  car.ReadString(string);
  if (string != "// Special Abilities database file")
  {
    car.Close();
    dataFile.Close();
    return -2;
  };
//    AfxGetApp()->WinHelp(IDS_HELP_Monster_Name);


  pConfigFile->ReadFile(car);
  // Now we begin the decode process
  // Get rid of old itemData
  specAbPointer->Clear();
  // Read and store new itemData
  ASSERT (pSpecAbData==NULL); // Nobody should be using it
  pSpecAbData=specAbPointer;
  //status=pConfigFile->DecodeFile(specialAbilitiesIDs, true);
  {
    int objNum;
    int s=0;
    for (objNum=1; pConfigFile->Exists(objNum); objNum++)
    {
      CString name;
      s = DecodeSpecAbObject(pConfigFile, objNum, &specDef, car);
      name = specDef.Lookup("name");
      if (s > status)
      {
        status = s;
        WriteDebugString("Error loading SpecialAbility %s\n",name);
      };
      if (name.IsEmpty())
      {
        s = CONFIG_STAT_semantic;
        if (s > status) 
        {
          status = s;
        };
      }
      else
      {
        SPECABILITY_DEF *pEntry;
        pEntry = specAbPointer->InsertAbility(name);
        specDef.Delete("name");
        pEntry->Swap(specDef);
        specDef.Clear();
      };
    };
  };
  pSpecAbData=NULL; // We are done.
  car.Close();
  dataFile.Close();
  // substitute default filenames for resources using "DEFAULT"
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}





#ifdef OLD_SPECIAL_ABILITIES
#else
int WriteSpecAbDB(LPCSTR file, 
                   A_SPECABILITY_DEF_L *specAbDef, 
                   CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status = CONFIG_STAT_ok;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  //ASSERT (pSpecAbData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
  //monstersPointer->CollapseDefaultFilenames();
  
  //pSpecAbData=specAbDef;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Special Abilities database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the special abilities file!!!\r\n");
  ar.WriteString("// Sample:\r\n");
  ar.WriteString("// \\(BEGIN)\r\n");
  ar.WriteString("// name = Bless\r\n");
  ar.WriteString("// [Activation Script] = $PUBLIC $FUNC SA_Bless  // Script names in []\r\n"); 
  ar.WriteString("// -{   //  Continuations start with hyphen\r\n");
  ar.WriteString("// -  $RETURN $SET_CHAR_BLESS($IndexOf($Target()),true)\r\n");
  ar.WriteString("// -} SA_Bless;\r\n");
  ar.WriteString("// (parameterA) = 5    // parameter names in ()\r\n");
  ar.WriteString("// \\(END)\r\n\r\n");

  //status=pConfigFile->WriteFile(ar, monsterIDs);

  // substitute default filenames for resources using "DEFAULT"
  // monstersPointer->ExpandDefaultFilenames();

  POSITION pos;
  const SPECABILITY_DEF *pSpecAbDef;
  pos = specAbDef->GetStartPosition();
  while (pos != NULL)
  {
    pSpecAbDef = specAbDef->GetNext(pos);
    ar.WriteString("\r\n\\(BEGIN)\r\n");
    ar.WriteString("name = ");
    ar.WriteString(pSpecAbDef->Name());
    ar.WriteString("\r\n");
    {
      POSITION position;
      position = pSpecAbDef->GetStartPosition();
      while (position != NULL)
      {
        {
          CString indentedValue;
          const ASLENTRY *pASL;
          pSpecAbDef->GetNextEntry(position, &pASL);
          switch(pASL->Flags())
          {
          case SPECAB_SCRIPT: ar.WriteString("["); break;
          case SPECAB_VARIABLE: ar.WriteString("("); break;
          case SPECAB_INTEGERTABLE: ar.WriteString("<"); break;
          };
          ar.WriteString(pASL->Key());
          switch(pASL->Flags())
          {
          case SPECAB_SCRIPT: ar.WriteString("]"); break;
          case SPECAB_VARIABLE: ar.WriteString(")"); break;
          case SPECAB_INTEGERTABLE: ar.WriteString(">"); break;
          };
          ar.WriteString(" = ");
          indentedValue = pASL->Value();
          indentedValue.Replace("\n", "\n-");
          ar.WriteString(indentedValue);
          ar.WriteString("\r\n");
        };
      };
    };
    ar.WriteString("\\(END)\r\n");
  };

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  // 20110519 PRS pMonsterData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}
#endif


int WriteMonsterDB(LPCSTR file, 
                   MONSTER_DATA_TYPE *monstersPointer, 
                   CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pMonsterData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
  monstersPointer->CollapseDefaultFilenames();
  
  pMonsterData=monstersPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Monster database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the monsters file!!!\r\n");

  status=pConfigFile->WriteFile(ar, monsterIDs);

  // substitute default filenames for resources using "DEFAULT"
  monstersPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pMonsterData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}



int ExportLevelDB(LPCSTR file,
                  int level,
                  LEVEL *levelPointer,
                  CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pLevelData==NULL); // Nobody should be using it
  
  
  pLevelData = levelPointer;

  {
    JWriter jw(&dataFile);
    jw.StartList();
    status = globalData.levelInfo.stats[level].Export(jw);
    if (status == CONFIG_STAT_ok)
    {
      status=pLevelData->Export(jw);
    };
    jw.EndList();
};

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pLevelData=NULL; // We are done.
  dataFile.Close();
  return 0;
}

int ExportGlobalDB(LPCSTR file,
                   GLOBAL_STATS *pGlobalPointer,
                   CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  

  {
    JWriter jw(&dataFile);
    jw.StartList();
    status = pGlobalPointer->Export(jw);
    jw.EndList();
  };

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  dataFile.Close();
  return 0;
}

int ImportGlobalDB(LPCSTR file,
                   GLOBAL_STATS *pGlobalPointer,
                   CONFIG_FILE *pConfigFile,
                   BOOL quiet)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (dataFile.Open(file, CFile::modeRead))
  {
    JReader jr(&dataFile);
    try
    {
      if (jr.Initialize() == ES_ok)
      {
        jr.StartList();
        status = pGlobalPointer->Import(jr, quiet);
        jr.EndList();
        SET_MODIFIED;
      }
      else
      {
        status = CONFIG_STAT_syntax;
        MsgBoxError(jr.GetErrorMessage());
      };
    }
    catch (int )
    {
      int i;
      CString msg;
      i = jr.GetError();
      msg = jr.GetErrorMessage();
      MsgBoxError(msg);
      status = CONFIG_STAT_syntax;
    };
  }
  else
  {
    MsgBoxError(CString("Cannot open ") + file);
    return -1;
  };

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error reading and decoding file");
    return -1;
  };
  return (status==CONFIG_STAT_ok)?0:-1;
}



int ExportCharacter(LPCSTR file,
                    CHARACTER *pCharacter)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  

  {
    JWriter jw(&dataFile);
    jw.StartList();
    pCharacter->Export(jw);
    jw.EndList();
  };
  dataFile.Close();
  return 0;
}

int ImportCharacter(LPCSTR file,
                    CHARACTER *pCharacter)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeRead)) return -1;
  

  {
    JReader jr(&dataFile);
    try
    {
      if (jr.Initialize())
      {
        jr.StartList();
        pCharacter->Import(jr);
        status = CONFIG_STAT_ok;
        jr.EndList();
      }
      else
      {
        status = CONFIG_STAT_syntax;
      };
    }
    catch (int )
    {
      int i;
      CString msg;
      i = jr.GetError();
      msg = jr.GetErrorMessage();
      status = CONFIG_STAT_syntax;
    };
  };

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error reading and decoding file");
    return -1;
  };
  dataFile.Close();
  return 0;
}

void ExportCharacters(void)
{
  globalData.charData.Export();
}

bool ImportCharacters(void)
{
  bool status;
  status = globalData.charData.Import(FALSE);
  SET_MODIFIED;
  return status;
}


//*****************************************************************************
// NAME: ReadSpellDB
//
// PURPOSE:  
//*****************************************************************************
SPELL_DATA tempSPELL;
SPELL_DATA_TYPE *pSpellData=NULL;


int ReadSpellDB(CString &file,
                SPELL_DATA_TYPE *spellsPointer,
                CONFIG_FILE *pConfigFile)
{
  return ReadSpellDB((LPCSTR)file, spellsPointer, pConfigFile);
}

// Handle clearing and storing of entire SPELL_DATA object
static void spellObject(CONFIG_PKT *pkt) {
  //static POSITION position;
  static int spellIdx;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempSPELL.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (tempSPELL.Name.CompareNoCase("template")!=0)
    {
      switch (tempSPELL.Targeting)
      {
        case Self:
          // No Parameters
          break;
        case SelectedByCount:
          tempSPELL.P1 = tempSPELL.temp_quantity;
          tempSPELL.P3 = tempSPELL.temp_range;
          break;
        case WholeParty:
          // No parameters
          break;
        case TouchedTargets:
          tempSPELL.P1 = tempSPELL.temp_quantity;
          break;
          case AreaCircle:
          tempSPELL.P1 = tempSPELL.temp_quantity;
          tempSPELL.P2 = tempSPELL.temp_radius;
          tempSPELL.P3 = tempSPELL.temp_range;
          break;
        case AreaLinePickStart:
          tempSPELL.P1 = tempSPELL.temp_width;
          tempSPELL.P2 = tempSPELL.temp_length;
          tempSPELL.P3 = tempSPELL.temp_range;
          break;
        case AreaLinePickEnd:
          tempSPELL.P1 = tempSPELL.temp_width;
          tempSPELL.P2 = tempSPELL.temp_length;
          tempSPELL.P3 = tempSPELL.temp_range;
          break;
        case AreaSquare:
          tempSPELL.P1 = tempSPELL.temp_width;
          tempSPELL.P2 = tempSPELL.temp_height;
          tempSPELL.P3 = tempSPELL.temp_range;
          break;
        case AreaCone:
          tempSPELL.P1 = tempSPELL.temp_width;
          tempSPELL.P2 = tempSPELL.temp_length;
          tempSPELL.P3 = tempSPELL.temp_range;
          break;
        case SelectByHitDice:
          tempSPELL.P1 = tempSPELL.temp_number;
          tempSPELL.P3 = tempSPELL.temp_range;
          break;
        default:
          NotImplemented(0x551b, false);
      };
      //if (pSpellData->FindName(tempSPELL.Name).IsValidSpell())
      if (pSpellData->LocateSpell(tempSPELL.SpellID()) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in spells.txt = %s\n",tempSPELL.Name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
        pSpellData->AddSpell(&tempSPELL);
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pSpellData->GetHeadPosition();
    spellIdx = 0;
    //if (position == NULL)
    if (spellIdx >= spellData.GetCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempSPELL.Clear();
    //tempSPELL.Name="template";
    //tempSPELL = pSpellData->PeekAtPos(position);
    //tempSPELL = *pSpellData->PeekSpellData(position);
    tempSPELL = *pSpellData->PeekSpell(spellIdx);
    //pSpellData->GetNext(position);
    pkt->objectName=tempSPELL.Name;
    break;
  case CONFIG_FUNC_getNextObject:
    spellIdx++;
    //if (position==NULL) 
    if (spellIdx >= pSpellData->GetCount())
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempSPELL=pSpellData->PeekAtPos(position);
      tempSPELL = *pSpellData->PeekSpell(spellIdx);
      //pSpellData->GetNext(position);
      pkt->objectName=tempSPELL.Name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}


static void DspellDescription(CONFIG_PKT* pkt)
{
    if (pkt->IsStoring)
    {
        pkt->value = tempSPELL.Description;
        return;
    };
    pkt->status = CONFIG_DECODE_string(
        pkt->value,
        pkt->errorMsg,
        tempSPELL.Description);
    tempSPELL.Description.Replace("/n", "\r\n");
    pkt->objectName = tempSPELL.Description;
}
static void DspellName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempSPELL.Name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempSPELL.Name);
  pkt->objectName=tempSPELL.Name;
}
static void DspellKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_long(pkt->value,tempSPELL.m_gsID.IllegalConversion());
    //CONFIG_ENCODE_long(pkt->value, 0);
    pkt->status = CONFIG_STAT_end;
    return;
  };
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  -2000000000,2000000000,
  //  tempITEM.Protection_Bonus);
}




static void DspellCastMsg(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempSPELL.CastMsg;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempSPELL.CastMsg);
  pkt->objectName=tempSPELL.CastMsg;
}
static void DspellCastingSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempSPELL.CastSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  };
    pkt->status=CONFIG_DECODE_file(
        pkt->value,
        pkt->errorMsg,
        tempSPELL.CastSound,
        "DEFAULT");
    if (tempSPELL.CastSound.CompareNoCase("none")==0)
      tempSPELL.CastSound="";
    else if (tempSPELL.CastSound=="")
      tempSPELL.CastSound="DEFAULT";
}

static void DspellMissileSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempSPELL.MissileSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  }

  pkt->status=CONFIG_DECODE_file( pkt->value,
                                  pkt->errorMsg,
                                  tempSPELL.MissileSound,
                                  "DEFAULT");

  if (tempSPELL.MissileSound.CompareNoCase("none")==0)
    tempSPELL.MissileSound="";
  else if (tempSPELL.MissileSound=="")
    tempSPELL.MissileSound="DEFAULT";
}

static void DspellCoverageSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempSPELL.CoverageSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  }

  pkt->status=CONFIG_DECODE_file( pkt->value,
                                  pkt->errorMsg,
                                  tempSPELL.CoverageSound,
                                  "DEFAULT");

  if (tempSPELL.CoverageSound.CompareNoCase("none")==0)
    tempSPELL.CoverageSound="";
  else if (tempSPELL.CoverageSound=="")
    tempSPELL.CoverageSound="DEFAULT";
}
static void DspellHitSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempSPELL.HitSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  }

  pkt->status=CONFIG_DECODE_file( pkt->value,
                                  pkt->errorMsg,
                                  tempSPELL.HitSound,
                                  "DEFAULT");

  if (tempSPELL.HitSound.CompareNoCase("none")==0)
    tempSPELL.HitSound="";
  else if (tempSPELL.HitSound=="")
    tempSPELL.HitSound="DEFAULT";
}
static void DspellLingerSound(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempSPELL.LingerSound;
    StripFilenamePath(pkt->value);
    if (pkt->value == "") pkt->value = "none";
    return;
  }

  pkt->status=CONFIG_DECODE_file( pkt->value,
                                  pkt->errorMsg,
                                  tempSPELL.LingerSound,
                                  "DEFAULT");

  if (tempSPELL.LingerSound.CompareNoCase("none")==0)
    tempSPELL.LingerSound="";
  else if (tempSPELL.LingerSound=="")
    tempSPELL.LingerSound="DEFAULT";
}

static void DspellSchool(CONFIG_PKT *pkt)
{
  //unsigned int i;

/*
  static CONFIGID classIDs[] =
  {
    CONFIGID( "MU"      ,MagicUserFlag),
    CONFIGID( "Cleric"  ,ClericFlag),
    //CONFIGID( "Paladin" ,PaladinFlag),
    //CONFIGID( "Ranger"  ,RangerFlag),
    //CONFIGID( "Druid"   ,DruidFlag),
    CONFIGID( ""        ,0)
  };
*/


  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_flags(pkt->value,classIDs,tempSPELL.schoolMask);
    pkt->value = tempSPELL.schoolID;
    return;
  };
  //if (pkt->value=="")
  //{
  //  tempSPELL.schoolMask=MagicUserFlag;
  //  tempSPELL.CastMask = MagicUserFlag; 
  //  return;
  //};
  //pkt->status=CONFIG_DECODE_flags (
  //  pkt->value,
  //  pkt->errorMsg,
  //  classIDs,
  //  i);
  tempSPELL.schoolID = pkt->value;
  //if (pkt->status==CONFIG_STAT_ok)
  //{
  //  tempSPELL.schoolMask=i;

    // as a default in case 'CastMask' is not specified in the file
  //  tempSPELL.CastMask = i; 
  //};
}

static void DspellCastByClass(CONFIG_PKT *pkt)
{
  //unsigned int i;

/*
  static CONFIGID classIDs[] =
  {
    CONFIGID( "MU"      ,MagicUserFlag),
    CONFIGID( "Cleric"  ,ClericFlag),
    CONFIGID( "Paladin" ,PaladinFlag),
    CONFIGID( "Ranger"  ,RangerFlag),
    CONFIGID( "Druid"   ,DruidFlag),
    CONFIGID( "Thief"   ,ThiefFlag),
    CONFIGID( "Fighter" ,FighterFlag),
    CONFIGID( ""        ,0)
  };
*/


  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_flags(pkt->value,classIDs,tempSPELL.CastMask);
    if (pkt->listNum >= tempSPELL.allowedBaseclasses.GetCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    pkt->value = *tempSPELL.allowedBaseclasses.PeekBaseclassID(pkt->listNum);
    pkt->status = CONFIG_STAT_more;
    return;
  };

  //if (pkt->value=="")
  //{
  //  if (tempSPELL.schoolMask != 0)
  //    tempSPELL.CastMask=tempSPELL.schoolMask;
  //  else
  //    tempSPELL.CastMask = MagicUserFlag;
  //
  //  return;
  //}


  //pkt->status=CONFIG_DECODE_flags (
  //  pkt->value,
  //  pkt->errorMsg,
  //  classIDs,
  //  i);

  //if (pkt->status==CONFIG_STAT_ok)
  {
    //tempSPELL.CastMask=i;

    //if (tempSPELL.schoolMask != 0)
    //{
    //  if (tempSPELL.schoolMask == MagicUserFlag)
    //  {
    //    if (tempSPELL.CastMask == 0)
    //    {
    //      tempSPELL.CastMask = MagicUserFlag; // force MU flag
    //    };
    //  }
    //  else
    //  {
    //    tempSPELL.CastMask &= ~MagicUserFlag; // remove MU flag
    //    if (tempSPELL.CastMask == 0)
    //      tempSPELL.CastMask = tempSPELL.schoolMask;
    //  }
    //}      
    BASECLASS_ID baseclassID;
    baseclassID = pkt->value;
    tempSPELL.allowedBaseclasses.Add(baseclassID);
    if (baseclassData.LocateBaseclass(baseclassID) < 0)
    {
      WriteDebugString("Spell %s 'cast by %s' is non-existent baseclass\n",
                       tempSPELL.Name,
                       baseclassID);
      pkt->status = CONFIG_STAT_semantic;
    };
  }
}

static void DspellTargetFriend(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID ffIDs[] =
  {
    CONFIGID( "no"    ,0),
    CONFIGID( "yes"   ,1),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,ffIDs,tempSPELL.CanTargetFriend);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    ffIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.CanTargetFriend = (i!=0);
  };
}
static void DspellTargetEnemy(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID ffIDs[] =
  {
    CONFIGID( "no"    ,0),
    CONFIGID( "yes"   ,1),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,ffIDs,tempSPELL.CanTargetEnemy);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    ffIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.CanTargetEnemy = (i!=0);
  };
}
static void DspellCumulative(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID cuIDs[] =
  {
    CONFIGID( "no"    ,0),
    CONFIGID( "yes"   ,1),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,cuIDs,tempSPELL.IsCumulative);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    cuIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.IsCumulative=i;
  };
}
//static void DspellUsableByParty(CONFIG_PKT *pkt)
static void DspellCanMemorize(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID ubpIDs[] =
  {
    CONFIGID( "no"    ,0),
    CONFIGID( "yes"   ,1),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_enum(pkt->value,ubpIDs,tempSPELL.UsableByParty);
    CONFIG_ENCODE_enum(pkt->value,ubpIDs,tempSPELL.CanMemorize);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    ubpIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    //tempSPELL.UsableByParty=i;
    tempSPELL.CanMemorize=i;
  };
}
//static void DspellCanScribe(CONFIG_PKT *pkt)
static void DspellAllowScribe(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID scribeIDs[] =
  {
    CONFIGID( "no"    ,0),
    CONFIGID( "yes"   ,1),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_enum(pkt->value,scribeIDs,tempSPELL.CanScribeAtLevelChange);
    CONFIG_ENCODE_enum(pkt->value,scribeIDs,tempSPELL.AllowScribe);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    scribeIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    //tempSPELL.CanScribeAtLevelChange=i;
    tempSPELL.AllowScribe = i;
  };
}

static void DspellAutoScribe(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID scribeIDs[] =
  {
    CONFIGID( "no"    ,0),
    CONFIGID( "yes"   ,1),
    CONFIGID( ""      ,0)
  };

  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_enum(pkt->value,scribeIDs,tempSPELL.AutoScribeAtLevelChange);
    CONFIG_ENCODE_enum(pkt->value,scribeIDs,tempSPELL.AutoScribe);
    return;
  }

  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    scribeIDs,
    i);

  if (pkt->status==CONFIG_STAT_ok)
  {
    //tempSPELL.AutoScribeAtLevelChange=i;
    tempSPELL.AutoScribe = i;
  }
}

//static void DspellCombatOnly(CONFIG_PKT *pkt)
static void DspellInCamp(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID combatIDs[] =
  {
    CONFIGID( "yes"   ,1),
    CONFIGID( "no"    ,0),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_enum(pkt->value,combatIDs,tempSPELL.Combat_Only);
    CONFIG_ENCODE_enum(pkt->value,combatIDs,tempSPELL.InCamp()?1:0);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    combatIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    //tempSPELL.Combat_Only=i;
    //tempSPELL.NotInCamp(i!=0);
    tempSPELL.InCamp(i!=0);
  };
}

//static void DspellCampOnly(CONFIG_PKT *pkt)
static void DspellInCombat(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID campIDs[] =
  {
    CONFIGID( "yes"   ,1),
    CONFIGID( "no"    ,0),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_enum(pkt->value,combatIDs,tempSPELL.Combat_Only);
    CONFIG_ENCODE_enum(pkt->value,campIDs,tempSPELL.InCombat()?1:0);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    campIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    //tempSPELL.Combat_Only=i;
    tempSPELL.InCombat(i!=0);
  };
}

static void DspellCanBeDispelled(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID combatIDs[] =
  {
    CONFIGID( "yes"   ,1),
    CONFIGID( "no"    ,0),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,combatIDs,tempSPELL.CanBeDispelled);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    combatIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.CanBeDispelled=i;
  };
}

static void DspellLingers(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID lingerIDs[] =
  {
    CONFIGID( "yes"   ,1),
    CONFIGID( "no"    ,0),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,lingerIDs,tempSPELL.Lingers);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    lingerIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.Lingers=i;
  };
}

static void DspellLingerAffectsOnce(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID lingerIDs[] =
  {
    CONFIGID( "yes"   ,1),
      CONFIGID( "no"    ,0),
      CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,lingerIDs,tempSPELL.LingerOnceOnly);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    lingerIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.LingerOnceOnly=i;
  };
}

static void DspellCastingTimeUnits(CONFIG_PKT *pkt)
{
  int i;
  //enum spellCastingTimeType { sctImmediate, sctInitiative, sctRounds, sctTurns };
  static CONFIGID casttimeunitIDs[] = {
    CONFIGID( "immediate"  ,sctImmediate),
    CONFIGID( "initiative" ,sctInitiative),
    CONFIGID( "rounds"     ,sctRounds),
    CONFIGID( "turns"      ,sctTurns),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,casttimeunitIDs,tempSPELL.Casting_Time_Type);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    casttimeunitIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.Casting_Time_Type=(spellCastingTimeType)i;
  };
}

static void DspellSaveVersus(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID saveIDs[] = {
    CONFIGID( "spell"           ,Sp),
    CONFIGID( "Par/Pois/DM"     ,ParPoiDM),
    CONFIGID( "Pet/Poly"        ,PetPoly),
    CONFIGID( "Rod/Staff/Wand"  ,RodStaffWand),
    CONFIGID( "Breath Weapon"   ,BreathWeapon),
    CONFIGID( ""      ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,saveIDs,tempSPELL.Save_Vs);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    saveIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.Save_Vs=(spellSaveVersusType)i;
  };
}

static void DspellSaveResult(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID saveIDs[] = {
    CONFIGID( "none"        ,NoSave),
    CONFIGID( "negates"     ,SaveNegates),
    CONFIGID( "half damage" ,SaveForHalf),
    CONFIGID( "use THAC0"   ,UseTHAC0),
    CONFIGID( ""            ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,saveIDs,tempSPELL.Save_Result);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    saveIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.Save_Result=(spellSaveEffectType)i;
  };
}

static void DspellSpellAttribute(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->value = "";
      pkt->listPos = tempSPELL.spell_asl.GetStartPosition();
    }
    
    if (pkt->listPos==NULL)
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }    
    
    //CString sakey;
    //CString savalue;
    {
      const ASLENTRY *pEntry;
      tempSPELL.spell_asl.GetNextAssoc(pkt->listPos, &pEntry);

      pkt->value = *pEntry->pKey();
      if (!(*pEntry->pValue()).IsEmpty())
      { 
        pkt->value += ',';
        pkt->value += *pEntry->pValue();
      }

      pkt->status = CONFIG_STAT_more;
      return;   
    };
  };

  CString temp;
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    temp);

  tempSPELL.spell_asl.Update(temp, "1", ASLF_EDITOR);
  pkt->objectName=temp;  
}


#ifdef OLD_SPECIAL_ABILITIES
static void DspellSpecialAbility(CONFIG_PKT *pkt)
{
  specialAbilitiesType S;
  int i;
  static CONFIGID abilityIDs[] =
  {
    CONFIGID("none"                 ,SA_None),
    CONFIGID("bless"                ,SA_Bless),
    CONFIGID("Curse"                ,SA_Curse),
    CONFIGID("Undead Fear"          ,SA_UndeadFear),
    CONFIGID("Enlarge"              ,SA_Enlarge),
    CONFIGID("Reduce"               ,SA_Reduce),
    CONFIGID("Charm Person"         ,SA_CharmPerson),
    CONFIGID("Detect Magic"         ,SA_DetectMagic),
    CONFIGID("Reflect Gaze Attack"  ,SA_ReflectGazeAttack),
    CONFIGID("Prot from Evil"       ,SA_ProtFromEvil),
    CONFIGID("Prot from Good"       ,SA_ProtFromGood),
    CONFIGID("Shield"               ,SA_Shield),
    CONFIGID("Sleep"                ,SA_Sleep),
    CONFIGID("Fog"                  ,SA_Fog),
    CONFIGID("Entangle"             ,SA_Entangle),
    CONFIGID("Invisible to Animals" ,SA_InvisibleToAnimals),
    CONFIGID("Invisible to Undead"  ,SA_InvisibleToUndead),
    CONFIGID("Non-Undead Fear"      ,SA_NonUndeadFear),
    CONFIGID("Sanctuary"            ,SA_Sanctuary),
    CONFIGID("Shillelagh"           ,SA_Shillelagh),
    CONFIGID("Displacement"         ,SA_Displacement),
    CONFIGID("Wizardry"             ,SA_Wizardry),
    CONFIGID("Hold Person"          ,SA_HoldPerson),
    CONFIGID("Vorpal Attack"        ,SA_VorpalAttack),    
    CONFIGID("Silenced"             ,SA_Silence),
    CONFIGID("Poisoned"             ,SA_Poisoned),
    CONFIGID("Slow Poison"          ,SA_SlowPoison),
    CONFIGID("Mirror Image"         ,SA_MirrorImage),
    CONFIGID("Invisible"            ,SA_Invisible),
    CONFIGID("Enfeebled"            ,SA_Enfeebled),
    CONFIGID("Blindness"            ,SA_Blindness),
    CONFIGID("Diseased"             ,SA_Diseased),
    CONFIGID(""          ,0)
  };
  if (pkt->IsStoring)
  {
    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->value = "";
      pkt->listPos = tempSPELL.specAbs.GetHeadPosition();
    }

    if ((pkt->listNum>NUM_SPECIAL_ABILITIES)||(pkt->listPos==NULL))
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    specialAbilitiesType sa = tempSPELL.specAbs.GetAt(pkt->listPos);
    CONFIG_ENCODE_enum(
          pkt->value,
          abilityIDs,
          sa);

    tempSPELL.specAbs.GetNext(pkt->listPos);
    pkt->status=CONFIG_STAT_more;
    return;
  }

  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    pkt->errorMsg,
    abilityIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    S=(specialAbilitiesType)i;
    tempSPELL.specAbs.EnableSpecAb(S);
  };
}

#else
static void DspellSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempSPELL.specAbs);
}
#endif

static void DspellTargetingType(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID targetIDs[] = 
  {
    CONFIGID( "self"              ,Self),
    CONFIGID( "selectByCount"     ,SelectedByCount),
    CONFIGID( "party"             ,WholeParty),
    CONFIGID( "touch"             ,TouchedTargets),
    CONFIGID( "areacircle"        ,AreaCircle),
    CONFIGID( "arealinepickstart" ,AreaLinePickStart),
    CONFIGID( "arealinepickend"   ,AreaLinePickEnd),
    CONFIGID( "areasquare"        ,AreaSquare),
    CONFIGID( "areacone"          ,AreaCone),
    CONFIGID( "SelectByHitDice"   ,SelectByHitDice),
    CONFIGID( ""                  ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,targetIDs,tempSPELL.Targeting);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    targetIDs,
    i);
  if (pkt->status==CONFIG_STAT_ok)
  {
    tempSPELL.Targeting=(spellTargetingType)i;
  };
}

static void DspellDurationUnits(CONFIG_PKT *pkt)
{
  int i;
  static CONFIGID unitIDs[] = {
    CONFIGID( "instantaneous" ,Permanent),
    CONFIGID( "rounds"        ,inRounds),
    CONFIGID( "damage taken"  ,byDamageTaken),
    CONFIGID( "hours"         ,inHours),
    CONFIGID( "days"          ,inDays),
    CONFIGID( "nbr attacks"   ,byNbrAttacks),
    CONFIGID( ""              ,0)
  };
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_enum(pkt->value,unitIDs,tempSPELL.Duration_Rate);
    return;
  };
  pkt->status=CONFIG_DECODE_enum (
    pkt->value,
    &pkt->errorMsg,
    unitIDs,
    i);

  if (pkt->status==CONFIG_STAT_ok)
    tempSPELL.Duration_Rate=(spellDurationType)i;
  else
    tempSPELL.Duration_Rate = Permanent;
}

static void DspellLevel(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempSPELL.Level);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,9,
    tempSPELL.Level,1);
}

static void DspellCastingTime(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempSPELL.Casting_Time);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,1000,
    tempSPELL.Casting_Time,1);
}

//static void DspellNumberOfTargets(CONFIG_PKT *pkt)
static void DspellTargetQuantity(CONFIG_PKT *pkt)
{

  if (pkt->IsStoring)
  {
    if (    (tempSPELL.Targeting != SelectedByCount)
         && (tempSPELL.Targeting != TouchedTargets)
         && (tempSPELL.Targeting != AreaCircle)
       ) 
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.NumTargets);
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.P1);
    return;
  };
  if ((pkt->value=="")||(pkt->value.CompareNoCase("none")==0))
  {
    //tempSPELL.NumTargets.Clear();
    tempSPELL.P1.Clear();
  }
  else
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      //&tempSPELL.NumTargets,
      &tempSPELL.temp_quantity,
          0,0,0,0,0);
  }
}

static void DspellTargetLength(CONFIG_PKT *pkt)
{

  if (pkt->IsStoring)
  {
    if (    (tempSPELL.Targeting != AreaLinePickEnd)
         && (tempSPELL.Targeting != AreaLinePickStart)
         && (tempSPELL.Targeting != AreaCone)
       ) 
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.NumTargets);
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.P2);
    return;
  };
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      //&tempSPELL.NumTargets,
      &tempSPELL.temp_length,
          0,0,0,0,0);
  }
}

static void DspellTargetRadius(CONFIG_PKT *pkt)
{

  if (pkt->IsStoring)
  {
    if (    (tempSPELL.Targeting != AreaCircle)
       ) 
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.NumTargets);
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.P2);
    return;
  };
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      //&tempSPELL.NumTargets,
      &tempSPELL.temp_radius,
          0,0,0,0,0);
  }
}

static void DspellTargetNumber(CONFIG_PKT *pkt)
{

  if (pkt->IsStoring)
  {
    if (    (tempSPELL.Targeting != SelectByHitDice)
       ) 
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.NumTargets);
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.P1);
    return;
  };
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      //&tempSPELL.NumTargets,
      &tempSPELL.temp_number,
          0,0,0,0,0);
  }
}



static void DspellTargetWidth(CONFIG_PKT *pkt)
{

  if (pkt->IsStoring)
  {
    if (    (tempSPELL.Targeting != AreaSquare)
         && (tempSPELL.Targeting != AreaLinePickStart)
         && (tempSPELL.Targeting != AreaLinePickEnd)
         && (tempSPELL.Targeting != AreaCone)
       ) 
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.NumTargets);
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.P1);
    return;
  };
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      //&tempSPELL.NumTargets,
      &tempSPELL.temp_width,
          0,0,0,0,0);
  }
}

static void DspellTargetHeight(CONFIG_PKT *pkt)
{

  if (pkt->IsStoring)
  {
    if (    (tempSPELL.Targeting != AreaSquare)
       ) 
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.NumTargets);
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.P2);
    return;
  };
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      //&tempSPELL.NumTargets,
      &tempSPELL.temp_height,
          0,0,0,0,0);
  }
}

static void DspellDuration(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.Duration);
    return;
  };
  if ((pkt->value=="")||(pkt->value.CompareNoCase("none")==0))
  {
    tempSPELL.Duration.Clear();
  }
  else
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      &tempSPELL.Duration,
          0,0,0,0,0);
  }
}

static void DspellTargetDuration(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.EffectDuration);
    return;
  };
  if ((pkt->value=="")||(pkt->value.CompareNoCase("none")==0))
  {
    tempSPELL.EffectDuration.Clear();
  }
  else
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      &tempSPELL.EffectDuration,
      0,0,0,0,0);
  }
}

static void DspellTargetRange(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (     (tempSPELL.Targeting != SelectedByCount)
         &&  (tempSPELL.Targeting != AreaCircle)
         &&  (tempSPELL.Targeting != SelectByHitDice)
         &&  (tempSPELL.Targeting != AreaLinePickStart)
         &&  (tempSPELL.Targeting != AreaLinePickEnd)
         &&  (tempSPELL.Targeting != AreaSquare)
         &&  (tempSPELL.Targeting != AreaCone)
       )
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.TargetRange);
    CONFIG_ENCODE_dicePlus(pkt->value,tempSPELL.P3);
    return;
  };
  if ((pkt->value=="")||(pkt->value.CompareNoCase("none")==0))
  {
    //tempSPELL.TargetRange.Clear();
    tempSPELL.temp_range.Clear();
  }
  else
  {
    pkt->status=CONFIG_DECODE_dicePlus(
      pkt->value,
      pkt->errorMsg,
      //&tempSPELL.TargetRange,
      &tempSPELL.temp_range,
          0,0,0,0,0);
  }
}

static void DspellAttackBonus(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    // not supported now
    pkt->value = "";
    return;
  };
  DICEPLUS dp;
  pkt->status=CONFIG_DECODE_dicePlus(
                  pkt->value,
                  pkt->errorMsg,
                  &dp,
                  0,0,0,0,0);
  if (pkt->status==CONFIG_STAT_ok)
  {
    //if (dp.m_numDice>0)
    if (!dp.m_Text.IsEmpty())
    {
      SPELL_EFFECTS_DATA seData;
      seData.AffectedAttr("$CHAR_THAC0");      
      seData.changeData=dp;
      seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
      tempSPELL.AddEffect(seData);
    }
  };
}

static void DspellDamage(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    // not supported now
    pkt->value = "";
    return;
  };
  DICEPLUS dp;
  pkt->status=CONFIG_DECODE_dicePlus(
                  pkt->value,
                  pkt->errorMsg,
                  &dp,
                  0,0,0,0,0);
  if (pkt->status==CONFIG_STAT_ok)
  {
    if (!dp.m_Text.IsEmpty())
    {
      CString diceformat;
      SPELL_EFFECTS_DATA seData;
      seData.AffectedAttr("$CHAR_DAMAGEBONUS");
      seData.changeData=dp;
      seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
      tempSPELL.AddEffect(seData);
    }
  };
}

static void DspellProtectionBonus(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    // not supported now
    pkt->value = "";
    return;
  };
  DICEPLUS dp;
  pkt->status=CONFIG_DECODE_dicePlus(
                  pkt->value,
                  pkt->errorMsg,
                  &dp,
                  0,0,0,0,0);
  if (pkt->status==CONFIG_STAT_ok)
  {
    //if (dp.m_numDice>0)
    if (!dp.m_Text.IsEmpty())
    {
      SPELL_EFFECTS_DATA seData;
      seData.AffectedAttr("$CHAR_AC");
      seData.changeData=dp;
      seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
      tempSPELL.AddEffect(seData);
    }
  };
}

static void DspellHealing(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    // not supported now
    pkt->value = "";
    return;
  };
  DICEPLUS dp;
  pkt->status=CONFIG_DECODE_dicePlus(
                  pkt->value,
                  pkt->errorMsg,
                  &dp,
                  0,0,0,0,0);
  if (pkt->status==CONFIG_STAT_ok)
  {
    //if (dp.m_numDice>0)
    if (!dp.m_Text.IsEmpty())
    {
      SPELL_EFFECTS_DATA seData;
      seData.AffectedAttr("$CHAR_HITPOINTS");
      seData.changeData=dp;
      seData.flags = SPELL_EFFECTS_DATA::EFFECT_TARGET;
      tempSPELL.AddEffect(seData);
    }
  };
}

static void DspellPrice(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempSPELL.Cast_Cost);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,2000000000,
    tempSPELL.Cast_Cost,0);
}

static void DspellPriority(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempSPELL.Cast_Priority);
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    -0x7ffffff,0x7fffffff,
    tempSPELL.Cast_Priority,0);
}

static void DspellCastArt(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.CastArt.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempSPELL.CastArt.filename;
      return;
    };
    if (tempSPELL.CastArt.filename=="")
    {
      pkt->value="none";
      return;
    };
    CONFIG_ENCODE_SpriteData(
      pkt->value,
			tempSPELL.CastArt.filename,
      tempSPELL.CastArt.timeDelay,
      tempSPELL.CastArt.NumFrames,
      tempSPELL.CastArt.FrameWidth,
      tempSPELL.CastArt.FrameHeight,
      tempSPELL.CastArt.style,
      tempSPELL.CastArt.MaxLoops,
      tempSPELL.CastArt.flags,
      tempSPELL.CastArt.AllowCentering,
      tempSPELL.CastArt.UseAlpha,
      tempSPELL.CastArt.AlphaValue);
    return;
  };
  tempSPELL.CastArt.picType = SpriteDib;
	if (pkt->value.CompareNoCase("DEFAULT")==0)
	{
    tempSPELL.CastArt.filename = "DEFAULT";
	}
	else if ((pkt->value == "") ||
           (pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempSPELL.CastArt.Clear();
	}
	else
	{
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempSPELL.CastArt.filename,
      tempSPELL.CastArt.timeDelay,
      tempSPELL.CastArt.NumFrames,
      tempSPELL.CastArt.FrameWidth,
      tempSPELL.CastArt.FrameHeight,
      tempSPELL.CastArt.style,
      tempSPELL.CastArt.MaxLoops,
      tempSPELL.CastArt.flags,
      tempSPELL.CastArt.AllowCentering,
      tempSPELL.CastArt.UseAlpha,
      tempSPELL.CastArt.AlphaValue);
  };
}

static void DspellMissileArt(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.MissileArt.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempSPELL.MissileArt.filename;
      return;
    };
    if (tempSPELL.MissileArt.filename=="")
    {
      pkt->value="none";
      return;
    };
    CONFIG_ENCODE_SpriteData(
      pkt->value,
			tempSPELL.MissileArt.filename,
      tempSPELL.MissileArt.timeDelay,
      tempSPELL.MissileArt.NumFrames,
      tempSPELL.MissileArt.FrameWidth,
      tempSPELL.MissileArt.FrameHeight,
      tempSPELL.MissileArt.style,
      tempSPELL.MissileArt.MaxLoops,
      tempSPELL.MissileArt.flags,
      tempSPELL.MissileArt.AllowCentering,
      tempSPELL.MissileArt.UseAlpha,
      tempSPELL.MissileArt.AlphaValue);
    return;
  };
  tempSPELL.MissileArt.picType = SpriteDib;
	if (pkt->value.CompareNoCase("DEFAULT")==0)
	{
    tempSPELL.MissileArt.filename = "DEFAULT";
	}
	else if ((pkt->value == "") ||
           (pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempSPELL.MissileArt.Clear();
	}
	else
	{
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempSPELL.MissileArt.filename,
      tempSPELL.MissileArt.timeDelay,
      tempSPELL.MissileArt.NumFrames,
      tempSPELL.MissileArt.FrameWidth,
      tempSPELL.MissileArt.FrameHeight,
      tempSPELL.MissileArt.style,
      tempSPELL.MissileArt.MaxLoops,
      tempSPELL.MissileArt.flags,
      tempSPELL.MissileArt.AllowCentering,
      tempSPELL.MissileArt.UseAlpha,
      tempSPELL.MissileArt.AlphaValue);
  };
}
static void DspellTargCoverArt(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.TargCoverageArt.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempSPELL.TargCoverageArt.filename;
      return;
    };
    if (tempSPELL.TargCoverageArt.filename=="")
    {
      pkt->value="none";
      return;
    };
    CONFIG_ENCODE_SpriteData(
      pkt->value,
			tempSPELL.TargCoverageArt.filename,
      tempSPELL.TargCoverageArt.timeDelay,
      tempSPELL.TargCoverageArt.NumFrames,
      tempSPELL.TargCoverageArt.FrameWidth,
      tempSPELL.TargCoverageArt.FrameHeight,
      tempSPELL.TargCoverageArt.style,
      tempSPELL.TargCoverageArt.MaxLoops,
      tempSPELL.TargCoverageArt.flags,
      tempSPELL.TargCoverageArt.AllowCentering,
      tempSPELL.TargCoverageArt.UseAlpha,
      tempSPELL.TargCoverageArt.AlphaValue);
    return;
  };
  tempSPELL.TargCoverageArt.picType = SpriteDib;
	if (pkt->value.CompareNoCase("DEFAULT")==0)
	{
    tempSPELL.TargCoverageArt.filename = "DEFAULT";
	}
	else if ((pkt->value == "") ||
           (pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempSPELL.TargCoverageArt.Clear();
	}
	else
	{
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempSPELL.TargCoverageArt.filename,
      tempSPELL.TargCoverageArt.timeDelay,
      tempSPELL.TargCoverageArt.NumFrames,
      tempSPELL.TargCoverageArt.FrameWidth,
      tempSPELL.TargCoverageArt.FrameHeight,
      tempSPELL.TargCoverageArt.style,
      tempSPELL.TargCoverageArt.MaxLoops,
      tempSPELL.TargCoverageArt.flags,
      tempSPELL.TargCoverageArt.AllowCentering,
      tempSPELL.TargCoverageArt.UseAlpha,
      tempSPELL.TargCoverageArt.AlphaValue);
  };
}
static void DspellTargHitArt(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.TargHitArt.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempSPELL.TargHitArt.filename;
      return;
    };
    if (tempSPELL.TargHitArt.filename=="")
    {
      pkt->value="none";
      return;
    };
    CONFIG_ENCODE_SpriteData(
      pkt->value,
			tempSPELL.TargHitArt.filename,
      tempSPELL.TargHitArt.timeDelay,
      tempSPELL.TargHitArt.NumFrames,
      tempSPELL.TargHitArt.FrameWidth,
      tempSPELL.TargHitArt.FrameHeight,
      tempSPELL.TargHitArt.style,
      tempSPELL.TargHitArt.MaxLoops,
      tempSPELL.TargHitArt.flags,
      tempSPELL.TargHitArt.AllowCentering,
      tempSPELL.TargHitArt.UseAlpha,
      tempSPELL.TargHitArt.AlphaValue);
    return;
  };
  tempSPELL.TargHitArt.picType = SpriteDib;
	if (pkt->value.CompareNoCase("DEFAULT")==0)
	{
    tempSPELL.TargHitArt.filename = "DEFAULT";
	}
	else if ((pkt->value == "") ||
           (pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempSPELL.TargHitArt.Clear();
	}
	else
	{
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempSPELL.TargHitArt.filename,
      tempSPELL.TargHitArt.timeDelay,
      tempSPELL.TargHitArt.NumFrames,
      tempSPELL.TargHitArt.FrameWidth,
      tempSPELL.TargHitArt.FrameHeight,
      tempSPELL.TargHitArt.style,
      tempSPELL.TargHitArt.MaxLoops,
      tempSPELL.TargHitArt.flags,
      tempSPELL.TargHitArt.AllowCentering,
      tempSPELL.TargHitArt.UseAlpha,
      tempSPELL.TargHitArt.AlphaValue);
  };
}
static void DspellTargLingerArt(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.TargLingerArt.filename.CompareNoCase("DEFAULT")==0)
    {
      pkt->value=tempSPELL.TargLingerArt.filename;
      return;
    };
    if (tempSPELL.TargLingerArt.filename=="")
    {
      pkt->value="none";
      return;
    };
    CONFIG_ENCODE_SpriteData(
      pkt->value,
			tempSPELL.TargLingerArt.filename,
      tempSPELL.TargLingerArt.timeDelay,
      tempSPELL.TargLingerArt.NumFrames,
      tempSPELL.TargLingerArt.FrameWidth,
      tempSPELL.TargLingerArt.FrameHeight,
      tempSPELL.TargLingerArt.style,
      tempSPELL.TargLingerArt.MaxLoops,
      tempSPELL.TargLingerArt.flags,
      tempSPELL.TargLingerArt.AllowCentering,
      tempSPELL.TargLingerArt.UseAlpha,
      tempSPELL.TargLingerArt.AlphaValue);
    //AllowCentering
    //UseAlpha
    //AlphaValue
    
    return;
  };
  tempSPELL.TargLingerArt.picType = SpriteDib;
	if (pkt->value.CompareNoCase("DEFAULT")==0)
	{
    tempSPELL.TargLingerArt.filename = "DEFAULT";
	}
	else if ((pkt->value == "") ||
           (pkt->value.CompareNoCase("NA") == 0) ||
           (pkt->value.CompareNoCase("none") == 0))
	{
		tempSPELL.TargLingerArt.Clear();
	}
	else
	{
		pkt->status=CONFIG_DECODE_SpriteData(
			pkt->value,
			pkt->errorMsg,
			tempSPELL.TargLingerArt.filename,
      tempSPELL.TargLingerArt.timeDelay,
      tempSPELL.TargLingerArt.NumFrames,
      tempSPELL.TargLingerArt.FrameWidth,
      tempSPELL.TargLingerArt.FrameHeight,
      tempSPELL.TargLingerArt.style,
      tempSPELL.TargLingerArt.MaxLoops,
      tempSPELL.TargLingerArt.flags,
      tempSPELL.TargLingerArt.AllowCentering,
      tempSPELL.TargLingerArt.UseAlpha,
      tempSPELL.TargLingerArt.AlphaValue);
  };
}

static void DspellSpellActivateScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.m_EffectedAttributes.GetCount()==0)
    {
      pkt->value="";
      return;
    }

    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->value = "";
      pkt->listPos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
    }

    if (pkt->listPos==NULL)
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    pkt->value = tempSPELL.m_EffectedAttributes.GetAt(pkt->listPos)->ActivationScript();
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');

    tempSPELL.m_EffectedAttributes.GetNext(pkt->listPos);
    pkt->status=CONFIG_STAT_more;
    return;
  }

  // loading
	if ((pkt->value == "") ||
      (pkt->value.CompareNoCase("NA") == 0) ||
      (pkt->value.CompareNoCase("none") == 0) ||
      (pkt->value.CompareNoCase("DEFAULT")==0))
	{
		return;
	}
	else
	{
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
                           pkt->errorMsg,
                           temp);
    pkt->objectName=temp;

    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");

    int count = 1;
    POSITION pos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
    while (pos != NULL)
    {
      if (count == pkt->keyNum)
      {
        tempSPELL.m_EffectedAttributes.GetAt(pos)->ActivationScript(temp);
        return;
      }
      count++;
      tempSPELL.m_EffectedAttributes.GetNext(pos);
    }
    // a spell script was given before the associated spell effect
    ASSERT(FALSE);
  }  
}

static void DspellSpellModifyScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.m_EffectedAttributes.GetCount()==0)
    {
      pkt->value="";
      return;
    }

    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->value = "";
      pkt->listPos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
    }

    if (pkt->listPos==NULL)
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    pkt->value = tempSPELL.m_EffectedAttributes.GetAt(pkt->listPos)->ModificationScript();
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');

    tempSPELL.m_EffectedAttributes.GetNext(pkt->listPos);
    pkt->status=CONFIG_STAT_more;
    return;
  }

  // loading
	if ((pkt->value == "") ||
      (pkt->value.CompareNoCase("NA") == 0) ||
      (pkt->value.CompareNoCase("none") == 0) ||
      (pkt->value.CompareNoCase("DEFAULT")==0))
	{
		return;
	}
	else
	{
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
                           pkt->errorMsg,
                           temp);
    pkt->objectName=temp;

    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");


    int count = 1;
    POSITION pos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
    while (pos != NULL)
    {
      if (count == pkt->keyNum)
      {
        tempSPELL.m_EffectedAttributes.GetAt(pos)->ModificationScript(temp);
        return;
      }
      count++;
      tempSPELL.m_EffectedAttributes.GetNext(pos);
    }
    // a spell script was given before the associated spell effect
    ASSERT(FALSE);
  }  
}

static void DspellSpellSavingThrowScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  // Removed in version 1.0303
  {
  /*  if (tempSPELL.m_EffectedAttributes.GetCount()==0)
  /*  {
  /*    pkt->value="";
  /*    return;
  /*  }
  /*
  /*  if (pkt->listNum==0)
  /*  {
  /*    // only happens on first call,
  /*    pkt->value = "";
  /*    pkt->listPos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
  /*  }
  /*
  /*  if (pkt->listPos==NULL)
  /*  { 
  /*    pkt->value = "";
  /*    pkt->status=CONFIG_STAT_end;
  /*    return;
  /*  }
  /*
  /*  pkt->value = tempSPELL.m_EffectedAttributes.GetAt(pkt->listPos)->SavingThrowScript();
  /*  pkt->value.Remove('\n');
  /*  pkt->value.Remove('\r');
  /*  pkt->value.Remove('\t');
  /*  pkt->value.Remove('\v');
  /*
  /*  tempSPELL.m_EffectedAttributes.GetNext(pkt->listPos);
  /*  pkt->status=CONFIG_STAT_more;
   */
    return;
  }

  // loading
	if ((pkt->value == "") ||
      (pkt->value.CompareNoCase("NA") == 0) ||
      (pkt->value.CompareNoCase("none") == 0) ||
      (pkt->value.CompareNoCase("DEFAULT")==0))
    {
      return;
    }
	else
	{
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
                           pkt->errorMsg,
                           temp);
    pkt->objectName=temp;

    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");

    /*  Version 1.0303 moved these scripts from the effects to the spell.
     *int count = 1;
     *POSITION pos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
     *while (pos != NULL)
     *{
     *  if (count == pkt->keyNum)
     *  {
     *    tempSPELL.m_EffectedAttributes.GetAt(pos)->SavingThrowScript(temp);
     *    return;
     *  }
     *  count++;
     *  tempSPELL.m_EffectedAttributes.GetNext(pos);
     *}
     * // a spell script was given before the associated spell effect
     *ASSERT(FALSE);
     */
    if (tempSPELL.SavingThrowSource.IsEmpty())
    {
      tempSPELL.SavingThrowSource = temp;
    };
    }
}

static void DspellSavingThrowScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  // Added in version 1.0303
  {
    if (tempSPELL.SavingThrowSource.IsEmpty())
    { 
      pkt->value="";
      return;
    }
    pkt->value = tempSPELL.SavingThrowSource;
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');
    return;
  }

  // loading
	if ((pkt->value == "") ||
      (pkt->value.CompareNoCase("NA") == 0) ||
      (pkt->value.CompareNoCase("none") == 0) ||
      (pkt->value.CompareNoCase("DEFAULT")==0))
	{
		return;
	}
	else
	{
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
                           pkt->errorMsg,
                           temp);
    pkt->objectName=temp;

    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");
    tempSPELL.SavingThrowSource = temp;
  }  
}


static void DspellSpellSavingThrowFailedScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
  /* Moved from effects to spell proer at version 1.0303
   *  if (tempSPELL.m_EffectedAttributes.GetCount()==0)
   *  {
   *    pkt->value="";
   *    return;
   *  }
   *  
   *  if (pkt->listNum==0)
   *  {
   *    // only happens on first call,
   *    pkt->value = "";
   *    pkt->listPos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
   *  }
   *  
   *  if (pkt->listPos==NULL)
   *  { 
   *    pkt->value = "";
   *    pkt->status=CONFIG_STAT_end;
   *    return;
   *  }
   *  
   *  pkt->value = tempSPELL.m_EffectedAttributes.GetAt(pkt->listPos)->SavingThrowFailedScript();
   *  pkt->value.Remove('\n');
   *  pkt->value.Remove('\r');
   *  pkt->value.Remove('\t');
   *  pkt->value.Remove('\v');
   *  
   *  tempSPELL.m_EffectedAttributes.GetNext(pkt->listPos);
   *  pkt->status=CONFIG_STAT_more;
   */
    return;
  }
  
  // loading
  if ((pkt->value == "") ||
    (pkt->value.CompareNoCase("NA") == 0) ||
    (pkt->value.CompareNoCase("none") == 0) ||
    (pkt->value.CompareNoCase("DEFAULT")==0))
    {
      return;
    }
  else
  {
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
      pkt->errorMsg,
      temp);
    pkt->objectName=temp;
    
    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");
   /* Moved from e ffects to spell proper in versio 1.0303 
    * int count = 1;
    * POSITION pos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
    * while (pos != NULL)
    * {
    *   if (count == pkt->keyNum)
    *   {
    *     tempSPELL.m_EffectedAttributes.GetAt(pos)->SavingThrowFailedScript(temp);
    *     return;
    *   }
    *   count++;
    *   tempSPELL.m_EffectedAttributes.GetNext(pos);
    * }
    * // a spell script was given before the associated spell effect
    * ASSERT(FALSE);
    */
    tempSPELL.SavingThrowFailedSource = temp;
    }
}
    
static void DspellSavingThrowFailedScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.SavingThrowFailedSource.IsEmpty())
    { 
      pkt->value="";
      return;
    }
    pkt->value = tempSPELL.SavingThrowFailedSource;
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');
    return;
  }
  
  // loading
  if ((pkt->value == "") ||
    (pkt->value.CompareNoCase("NA") == 0) ||
    (pkt->value.CompareNoCase("none") == 0) ||
    (pkt->value.CompareNoCase("DEFAULT")==0))
  {
    return;
  }
  else
  {
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
      pkt->errorMsg,
      temp);
    pkt->objectName=temp;
    
    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");
    tempSPELL.SavingThrowFailedSource = temp;
  }  
}


static void DspellSpellSavingThrowSucceededScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
  /* Moved from effect to spell proper in version 1.0303
   *  if (tempSPELL.m_EffectedAttributes.GetCount()==0)
   *  {
   *    pkt->value="";
   *    return;
   *  }
   *  
   *  if (pkt->listNum==0)
   *  {
   *    // only happens on first call,
   *    pkt->value = "";
   *    pkt->listPos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
   *  }
   *  
   *  if (pkt->listPos==NULL)
   *  { 
   *    pkt->value = "";
   *    pkt->status=CONFIG_STAT_end;
   *    return;
   *  }
   *  
   *  pkt->value = tempSPELL.m_EffectedAttributes.GetAt(pkt->listPos)->SavingThrowSucceededScript();
   *  pkt->value.Remove('\n');
   *  pkt->value.Remove('\r');
   *  pkt->value.Remove('\t');
   *  pkt->value.Remove('\v');
   *  
   *  tempSPELL.m_EffectedAttributes.GetNext(pkt->listPos);
   *  pkt->status=CONFIG_STAT_more;
   */
    return;
  }
  
  // loading
  if ((pkt->value == "") ||
    (pkt->value.CompareNoCase("NA") == 0) ||
    (pkt->value.CompareNoCase("none") == 0) ||
    (pkt->value.CompareNoCase("DEFAULT")==0))
    {
      return;
    }
  else
  {
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
      pkt->errorMsg,
      temp);
    pkt->objectName=temp;
    
    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");
    
  /* Moved from effect to spell proper in version 1.0303
   *  int count = 1;
   *  POSITION pos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
   *  while (pos != NULL)
   *  {
   *    if (count == pkt->keyNum)
   *    {
   *      tempSPELL.m_EffectedAttributes.GetAt(pos)->SavingThrowSucceededScript(temp);
   *      return;
   *    }
   *    count++;
   *    tempSPELL.m_EffectedAttributes.GetNext(pos);
   *  }
   *  // a spell script was given before the associated spell effect
   *  ASSERT(FALSE);
   */
    if (tempSPELL.SavingThrowSucceededSource.IsEmpty())
    {
      tempSPELL.SavingThrowSucceededSource = temp;
    };
    }
}
    
static void DspellSavingThrowSucceededScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.SavingThrowSucceededSource.IsEmpty())
    { 
      pkt->value="";
      return;
    }
    pkt->value = tempSPELL.SavingThrowSucceededSource;
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');
    return;
  }
  
  // loading
  if ((pkt->value == "") ||
    (pkt->value.CompareNoCase("NA") == 0) ||
    (pkt->value.CompareNoCase("none") == 0) ||
    (pkt->value.CompareNoCase("DEFAULT")==0))
  {
    return;
  }
  else
  {
    CString temp;
    pkt->status = 
      CONFIG_DECODE_string(pkt->value,
      pkt->errorMsg,
      temp);
    pkt->objectName=temp;
    
    // an attempt to reformat the script for easier viewing
    temp.Replace("{", "\r\n{\r\n");
    temp.Replace("};", "\r\n};\r\n");
    tempSPELL.SavingThrowSucceededSource = temp;
  }  
}



static void DspellSpellEffect(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (tempSPELL.m_EffectedAttributes.GetCount()==0)
    {
      pkt->value="none";
      return;
    }

    if (pkt->listNum==0)
    {
      // only happens on first call,
      pkt->value = "";
      pkt->listPos = tempSPELL.m_EffectedAttributes.GetHeadPosition();
    }

    if (pkt->listPos==NULL)
    { 
      pkt->value = "";
      pkt->status=CONFIG_STAT_end;
      return;
    }

    if (tempSPELL.m_EffectedAttributes.GetAt(pkt->listPos)->AffectedAttr()!="")
      CONFIG_ENCODE_SpellEffect(pkt->value, tempSPELL.m_EffectedAttributes.GetAt(pkt->listPos));

    tempSPELL.m_EffectedAttributes.GetNext(pkt->listPos);
    pkt->status=CONFIG_STAT_more;
    return;
  }

  // loading
	if ((pkt->value == "") ||
      (pkt->value.CompareNoCase("NA") == 0) ||
      (pkt->value.CompareNoCase("none") == 0) ||
      (pkt->value.CompareNoCase("DEFAULT")==0))
	{
		return;
	}
	else
	{
    SPELL_EFFECTS_DATA data;
		pkt->status=CONFIG_DECODE_SpellEffect(
			            pkt->value,
			            pkt->errorMsg,
			            &data);
    if (pkt->status==CONFIG_STAT_ok)
      tempSPELL.AddEffect(data);
  }  
}

static void DspellSpellBeginScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempSPELL.SpellBeginSource;
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');

    return;
  };

  CString temp;
  pkt->status = 
    CONFIG_DECODE_string(pkt->value,
                         pkt->errorMsg,
                         temp);
  pkt->objectName=temp;

  // an attempt to reformat the script for easier viewing
  temp.Replace("{", "\r\n{\r\n");
  temp.Replace("};", "\r\n};\r\n");

  tempSPELL.SpellBeginSource = temp;
}

static void DspellSpellEndScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempSPELL.SpellEndSource;
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');

    return;
  };

  CString temp;
  pkt->status = 
    CONFIG_DECODE_string(pkt->value,
                         pkt->errorMsg,
                         temp);
  pkt->objectName=temp;

  // an attempt to reformat the script for easier viewing
  temp.Replace("{", "\r\n{\r\n");
  temp.Replace("};", "\r\n};\r\n");

  tempSPELL.SpellEndSource = temp;
}


static void DspellSpellInitiationScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempSPELL.SpellInitiationSource;
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');

    return;
  };

  CString temp;
  pkt->status = 
    CONFIG_DECODE_string(pkt->value,
                         pkt->errorMsg,
                         temp);
  pkt->objectName=temp;

  // an attempt to reformat the script for easier viewing
  temp.Replace("{", "\r\n{\r\n");
  temp.Replace("};", "\r\n};\r\n");

  tempSPELL.SpellInitiationSource = temp;
}

static void DspellSpellTerminationScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempSPELL.SpellTerminationSource;
    pkt->value.Remove('\n');
    pkt->value.Remove('\r');
    pkt->value.Remove('\t');
    pkt->value.Remove('\v');

    return;
  };

  CString temp;
  pkt->status = 
    CONFIG_DECODE_string(pkt->value,
                         pkt->errorMsg,
                         temp);
  pkt->objectName=temp;

  // an attempt to reformat the script for easier viewing
  temp.Replace("{", "\r\n{\r\n");
  temp.Replace("};", "\r\n};\r\n");

  tempSPELL.SpellTerminationSource = temp;
}




#undef df
#undef dh
#define df(a) Dspell##a
#define dh(a) IDS_HELP_spell_##a
// Defined above //  #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))

CONFIGID spellIDs[] =
{
CONFIGID ("",CONFIG_DATA_object,spellObject),
DEFID ("name"               ,string     ,Name),
DEFID ("description"        ,string     ,Description),
DEFID ("Key"                ,long       ,Key),
DEFID ("casting sound"      ,file       ,CastingSound),
DEFID ("cast msg"           ,string     ,CastMsg),
DEFID ("missile sound"      ,file       ,MissileSound),
DEFID ("coverage sound"     ,file       ,CoverageSound),
DEFID ("hit sound"          ,file       ,HitSound),
DEFID ("linger sound"       ,file       ,LingerSound),
DEFID ("cast art"           ,spritedata ,CastArt),
DEFID ("missile art"        ,spritedata ,MissileArt),
DEFID ("target cover art"   ,spritedata ,TargCoverArt),
DEFID ("target hit art"     ,spritedata ,TargHitArt),
DEFID ("target linger art"  ,spritedata ,TargLingerArt),
DEFID ("school"             ,string     ,School),
DEFID ("cast by"            ,flags      ,CastByClass),
DEFID ("can target friend"  ,enum       ,TargetFriend),
DEFID ("can target enemy"   ,enum       ,TargetEnemy),
DEFID ("cumulative"         ,enum       ,Cumulative),
//DEFID ("usable by party"    ,enum       ,UsableByParty),
DEFID ("can memorize"       ,enum       ,CanMemorize),
//DEFID ("can scribe"         ,enum       ,CanScribe),
DEFID ("allow scribe"       ,enum       ,AllowScribe),
DEFID ("auto scribe"        ,enum       ,AutoScribe),
//DEFID ("combat only"        ,enum       ,CombatOnly),      // If not in camp   // Sets NOT_IN_CAMP
//DEFID ("camp only"          ,enum       ,CampOnly),        // If not in combat // Sets NOT_IN_COMBAT
DEFID ("in camp"            ,enum       ,InCamp),    
DEFID ("in combat"          ,enum       ,InCombat),  
DEFID ("can be dispelled"   ,enum       ,CanBeDispelled),
DEFID ("lingers"            ,enum       ,Lingers),
DEFID ("linger affects once",enum       ,LingerAffectsOnce),
DEFID ("save versus"        ,enum       ,SaveVersus),
DEFID ("save result"        ,enum       ,SaveResult),
DEFID ("Special Ability"    ,string     ,SpecialAbility),

DEFID ("targeting type"     ,enum       ,TargetingType),
DEFID ("target width"       ,dicePlus   ,TargetWidth),
DEFID ("target height"      ,dicePlus   ,TargetHeight),
DEFID ("target length"      ,dicePlus   ,TargetLength),
DEFID ("target quantity"    ,dicePlus   ,TargetQuantity),
DEFID ("target range"       ,dicePlus   ,TargetRange),
DEFID ("target radius"      ,dicePlus   ,TargetRadius),
DEFID ("target number"      ,dicePlus   ,TargetNumber),

DEFID ("level"              ,long       ,Level),
DEFID ("casting time"       ,long       ,CastingTime),
DEFID ("casting time units" ,enum       ,CastingTimeUnits),
DEFID ("duration"           ,dicePlus   ,Duration),
DEFID ("duration units"     ,enum       ,DurationUnits),
DEFID ("target duration"    ,dicePlus   ,TargetDuration),
DEFIDz ("attack bonus"      ,exDice     ,AttackBonus, true), // read-only
DEFIDz ("damage"            ,exDice     ,Damage, true), // read-only
DEFIDz ("protection bonus"  ,exDice     ,ProtectionBonus, true), // read-only
DEFIDz ("healing"           ,exDice     ,Healing, true), // read-only
DEFID ("price"              ,long       ,Price),
DEFID ("priority"           ,long       ,Priority),
DEFIDg ("spell effect"      ,spellEffect,SpellEffect),
DEFIDg ("activate script"   ,string     ,SpellActivateScript),
DEFIDg ("modify script"     ,string     ,SpellModifyScript),
// At version 1.0303 the following three scripts were moved from effects to the spell proper.
DEFIDg ("saving throw script",string    ,SpellSavingThrowScript),
DEFIDg ("saving throw failed script",string    ,SpellSavingThrowFailedScript),
DEFIDg ("saving throw succeeded script",string    ,SpellSavingThrowSucceededScript),
// THe following three scripts were moved here in version 1.0303
DEFID  ("savingThrowScript" ,string     ,SavingThrowScript),
DEFID  ("savingThrowSucceededScript" ,string     ,SavingThrowSucceededScript),
DEFID  ("savingThrowFailedScript" ,string     ,SavingThrowFailedScript),
//
DEFID ("spell attribute"    ,string     ,SpellAttribute),
DEFID ("begin script"       ,string     ,SpellBeginScript),
DEFID ("end script"         ,string     ,SpellEndScript),
DEFID ("initiation script"  ,string     ,SpellInitiationScript),
DEFID ("termination script" ,string     ,SpellTerminationScript),
CONFIGID( "",CONFIG_DATA_end,NULL)
};

int ReadSpellDB(LPCSTR file,
                SPELL_DATA_TYPE *spellsPointer,
                CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CFile dataFile;
  CONFIG_FILE myConfigFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  SPELL_DATA_TYPE oldSpells;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  CArchive ar( &dataFile, CArchive::load);
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  ar.ReadString(string);
  if (string != "// Spell database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old spellData
  oldSpells = *spellsPointer;
  spellsPointer->Clear();
  // Read and store new spellData
  ASSERT(pSpellData==NULL); // Nobody should be using it.
  pSpellData=spellsPointer;
  status=pConfigFile->DecodeFile(spellIDs, true);

  // 
  // Now we will transfer all possible preSpellNameKey values from the
  // old items to the new items.
  {
    int i, n;
    n = oldSpells.GetSpellCount();
    for (i=0; i<n; i++)
    {
      const SPELL_DATA *pOldSpell;
      SPELL_DATA *pNewSpell;
      pOldSpell = oldSpells.PeekSpell(i);
      if (pOldSpell->preSpellNameKey >= 0)
      {
        pNewSpell = spellsPointer->GetSpell(pOldSpell->SpellID());
        if (pNewSpell != NULL)
        {
          if (pNewSpell->preSpellNameKey < 0)
          {
            pNewSpell->preSpellNameKey = pOldSpell->preSpellNameKey;
          };
        };
      };
    };
  };
  
  
  
  pSpellData=NULL; // We are done with it.
  // substitute default filenames for resources using "DEFAULT"
  spellsPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


int WriteSpellDB(LPCSTR file, 
                 SPELL_DATA_TYPE *spellsPointer, 
                 CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pSpellData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
  spellsPointer->CollapseDefaultFilenames();
  
  pSpellData=spellsPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Spell database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the spells file!!!\r\n");
  status=pConfigFile->WriteFile(ar, spellIDs);

  // substitute default filenames for resources using "DEFAULT"
  spellsPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pSpellData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}




//*****************************************************************************
// NAME: ReadRaceDB
//
// PURPOSE:  
//*****************************************************************************

RACE_DATA tempRACE; // Where we build race.
RACE_DATA_TYPE *pRaceData=NULL;  // Where data will be put

// Handle clearing and storing of entire RACE_DATA object
static void raceObject(CONFIG_PKT *pkt) {
  //static POSITION position;
  static int indx;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempRACE.Clear();
    tempRACE.m_race_asl.Insert(RACE_ALLOWED_CLASS_ATTRIBUTE,
                                     "",   //An empty list..else anything goes.
                                     ASLF_READONLY|ASLF_DESIGN);
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (tempRACE.m_name.CompareNoCase("template")!=0)
    {
      //if (pRaceData->FindName(tempRACE.m_name)!=raceUnknown)
      indx = pRaceData->LocateRace(tempRACE.RaceID());
      if (indx >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in races.txt = %s\n",tempRACE.m_name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
        pRaceData->AddRace(tempRACE);
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pRaceData->GetHeadPosition();
    indx = 0;
    //if (position == NULL)
    if (indx >= pRaceData->GetCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempRACE.Clear();
    //tempRACE.m_name="template";
    //tempRACE = pRaceData->PeekAtPos(position);
    tempRACE = *pRaceData->PeekRace(indx);
    //pRaceData->GetNext(position);
    pkt->objectName=tempRACE.m_name;
    break;
  case CONFIG_FUNC_getNextObject:
    indx++;
    //if (position==NULL) {
    if (indx >= pRaceData->GetCount()) 
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempRACE=pRaceData->PeekAtPos(position);
      tempRACE = *pRaceData->PeekRace(indx);
      //pRaceData->GetNext(position);
      pkt->objectName=tempRACE.m_name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}

static void DraceName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempRACE.m_name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempRACE.m_name);
  pkt->objectName=tempRACE.m_name;
}


static void DraceKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->status = CONFIG_STAT_skip;
    //CONFIG_ENCODE_long(pkt->value,tempRACE.m_raceKey);
    return;
  };
}




static void DraceWeight (CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(pkt->value,tempRACE.m_weight);
    return;
  };
  if (pkt->value.IsEmpty())
  {
    tempRACE.m_weight.Clear();
    return;
  };
  pkt->status=CONFIG_DECODE_dicePlus(
    pkt->value,
    pkt->errorMsg,
    &tempRACE.m_weight,
        0,0,0);
}

static void DraceHeight (CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(pkt->value,tempRACE.m_height);
    return;
  };
  if (pkt->value.IsEmpty())
  {
    tempRACE.m_height.Clear();
    return;
  };
  pkt->status=CONFIG_DECODE_dicePlus(
    pkt->value,
    pkt->errorMsg,
    &tempRACE.m_height,
    0,0,0);
}

static void DraceAge (CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(pkt->value,tempRACE.m_age);
    return;
  };
  if (pkt->value.IsEmpty())
  {
    tempRACE.m_age.Clear();
    return;
  };
  pkt->status=CONFIG_DECODE_dicePlus(
    pkt->value,
    pkt->errorMsg,
    &tempRACE.m_age,
    0,0,0);
}

static void DraceMaxAge (CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(pkt->value,tempRACE.m_maxAge);
    return;
  };
  if (pkt->value.IsEmpty())
  {
    tempRACE.m_maxAge.Clear();
    return;
  };
  pkt->status=CONFIG_DECODE_dicePlus(
    pkt->value,
    pkt->errorMsg,
    &tempRACE.m_maxAge,
    0,0,0);
}

static void DraceBaseMovement (CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(pkt->value, tempRACE.m_baseMovement);
    return;
  };
  if (pkt->value.IsEmpty())
  {
    tempRACE.m_baseMovement.Clear();
    return;
  };
  pkt->status=CONFIG_DECODE_dicePlus(
              pkt->value,
              pkt->errorMsg,
              &tempRACE.m_baseMovement,
              0,0,0);
}

static void DraceAbilityReq (CONFIG_PKT *pkt)
{
  ABILITY_REQ temp;
  if (pkt->IsStoring)
  {
    //int num = tempRACE.m_abilityRequired.GetSize();
    int n;
    n = tempRACE.GetAbilityReqCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //pkt->value = tempRACE.m_abilityRequired[pkt->listNum].Encode();
    pkt->value = tempRACE.PeekAbilityReq(pkt->listNum)->Encode();
    if (pkt->listNum+1 < n) pkt->status = CONFIG_STAT_more;
    return;
  };
  pkt->status=CONFIG_DECODE_AbilityRequirement(
    pkt->value,
    pkt->errorMsg,
    &temp);
  if (pkt->status==CONFIG_STAT_ok)
        tempRACE.AddAbilityRequirement(temp);
}

static void DraceMultiClass (CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CString key = RACE_ALLOWED_CLASS_ATTRIBUTE;
    CString newValue;
    DELIMITED_STRING ds;
    {
      const ASLENTRY *pEntry;
      pEntry = tempRACE.m_race_asl.Find(key);
      if (pEntry != NULL)
      {
        ds = *pEntry->pValue();
        if (ds.IsLegal())
        {
          int num;
          num = ds.Count();
          if (num > pkt->listNum)
          {
            CString x;
            x = ds.GetAt(pkt->listNum);
            pkt->value = x;
            if (num > pkt->listNum + 1)
            {
              pkt->status = CONFIG_STAT_more;
            };
            return;
          };
        };
      };
    };
    pkt->status = CONFIG_STAT_end;
    return;
  };
  tempRACE.AddAllowedClass(pkt->value);
  pkt->status=CONFIG_STAT_ok;
}

static void DraceChangeClass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempRACE.m_canChangeClass) ;
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    tempRACE.m_canChangeClass, FALSE);
}

static void DraceDwarfResistance(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempRACE.m_dwarfResistance) ;
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    tempRACE.m_dwarfResistance, FALSE);
}

static void DraceGnomeResistance(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_BOOL(pkt->value,tempRACE.m_gnomeResistance) ;
    return;
  };
  pkt->status=CONFIG_DECODE_BOOL(
    pkt->value,
    pkt->errorMsg,
    tempRACE.m_gnomeResistance, FALSE);
}

static void DraceFindSecretDoor(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempRACE.m_findSecretDoor) ;
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0, // min
    7, // max
    tempRACE.m_findSecretDoor, 
    0);
}

static void DraceFindSecretDoorSearching(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempRACE.m_findSecretDoorSearching) ;
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,  // min
    7,  // max
    tempRACE.m_findSecretDoorSearching, 
    0);
}



static void DraceSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempRACE.m_specAbs);
}



static void DraceSkill(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempRACE.GetSkillCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const SKILL *pSkill;
    pSkill = tempRACE.PeekSkill(pkt->listNum);
    pSkill->Encode(pkt);
    return;
  };
  {
    SKILL skill;
    skill.Decode(pkt);
    tempRACE.Add(skill);
  };
};

static void DraceSkillAdjAbility(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int i, n;
    n = tempRACE.GetSkillAdjAbilityCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    char line[1000];
    int values[HIGHEST_CHARACTER_PRIME];
    int sum;
    char *dst;
    const SKILL_ADJUSTMENT_ABILITY *pSkillAdj;
    pSkillAdj = tempRACE.PeekSkillAdjAbility(pkt->listNum);
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      values[i] = pSkillAdj->skillAdj[i];
    };
    dst = line+999;
    *dst = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_PRIME, dst, &sum);
    *(--dst) = pSkillAdj->adjType;
    *(--dst) = ',';
    dst = PrependString(pSkillAdj->abilityID, dst);
    dst = PrependString(pSkillAdj->skillID, dst);
    pkt->value = dst+1;
    pkt->status = CONFIG_STAT_more;
    return;
  };
  const char *src;
  char adjType;
  SKILL_ADJUSTMENT_ABILITY saa;
  CString skillName;
  CString abilityName;
  CString type;
  src = (LPCSTR)pkt->value;
  src = GetPrefix(pkt, src, &skillName);
  src = GetPrefix(pkt, src, &abilityName);
  saa.abilityID = abilityName;
  saa.skillID = skillName;
  int i;
  int values[HIGHEST_CHARACTER_PRIME];
  src = GetPrefix(pkt, src, &type);
  if (!type.IsEmpty())
  {
    adjType = type[0];
  }
  else
  {
    adjType = ' ';
  };
  saa.adjType = adjType;
  src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_PRIME);
  for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
  {
    saa.skillAdj[i] = values[i];
  };
  tempRACE.Add(saa);
};

static void DraceSkillAdjBaseclass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempRACE.GetSkillAdjBaseclassCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const SKILL_ADJUSTMENT_BASECLASS *psab;
    psab = tempRACE.PeekSkillAdjBaseclass(pkt->listNum);
    psab->Encode(pkt);
    return;
  };
  SKILL_ADJUSTMENT_BASECLASS sab;
  sab.Decode(pkt);
  tempRACE.Add(sab);
};

static void DraceSkillAdjRace(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempRACE.GetSkillAdjRaceCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const SKILL_ADJUSTMENT_RACE *pSkillAdjRace;
    pSkillAdjRace = tempRACE.PeekSkillAdjRace(pkt->listNum);
    pSkillAdjRace->Encode(pkt);
    pkt->status = CONFIG_STAT_more;
    return;
  };
  SKILL_ADJUSTMENT_RACE skillAdjRace;
  skillAdjRace.Decode(pkt);
  if (pkt->status != CONFIG_STAT_ok) return;
  tempRACE.Add(skillAdjRace);
};

static void DraceSkillAdjScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempRACE.GetSkillAdjScriptCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    NotImplemented(0x1e76caa, false);
    return;
  };
  SKILL_ADJUSTMENT_SCRIPT skillAdjScript;
  skillAdjScript.Decode(pkt);
  tempRACE.Add(skillAdjScript);
};





#undef df
#undef dh
#define df(a) Drace##a
#define dh(a) IDS_HELP_race_##a
// Defined above //  #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))

CONFIGID DraceIDs[] =
{
CONFIGID ("",CONFIG_DATA_object,raceObject),
DEFID ("name"                   ,string     ,Name),
DEFID ("Key"                    ,long       ,Key),
DEFID ("weight"                 ,dicePlus   ,Weight),
DEFID ("height"                 ,dicePlus   ,Height),
DEFID ("age"                    ,dicePlus   ,Age),
DEFID ("maxage"                 ,dicePlus   ,MaxAge),
DEFID ("BaseMovement"           ,dicePlus   ,BaseMovement),
DEFID ("AbilityRequirement"     ,AbilityReq ,AbilityReq),
DEFID ("AllowedMultiClass"      ,string     ,MultiClass),
DEFID ("Special Ability"        ,string     ,SpecialAbility),
DEFID ("ChangeClass"            ,BOOL       ,ChangeClass),
DEFID ("DwarfResistance"        ,BOOL       ,DwarfResistance),
DEFID ("GnomeResistance"        ,BOOL       ,GnomeResistance),
DEFID ("FindSecretDoor"         ,long       ,FindSecretDoor),
DEFID ("FindSecretDoorSearching",long       ,FindSecretDoorSearching),
DEFID ("Skill"                  ,string     ,Skill),
DEFID ("SkillAdjAbility"        ,string     ,SkillAdjAbility),
DEFID ("SkillAdjBaseclass"      ,string     ,SkillAdjBaseclass),
DEFID ("SkillAdjRace"           ,string     ,SkillAdjRace),
DEFID ("SkillAdjScript"         ,string     ,SkillAdjScript),
CONFIGID( "",CONFIG_DATA_end,NULL)
};


int ReadRaceDB(CString &file, 
               RACE_DATA_TYPE *racePointer,
               CONFIG_FILE *pConfigFile)
{
  return ReadRaceDB((LPCSTR)file, racePointer, pConfigFile);
}


int ReadRaceDB(LPCSTR file,
                RACE_DATA_TYPE *racePointer,
                CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CFile dataFile;
  CONFIG_FILE myConfigFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  RACE_DATA_TYPE oldRaces;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  CArchive ar( &dataFile, CArchive::load);
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  ar.ReadString(string);
  if (string != "// Race database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old raceData
  oldRaces = *racePointer;
  racePointer->Clear();
  // Read and store new raceData
  ASSERT(pRaceData==NULL); // Nobody should be using it.
  pRaceData=racePointer;
  status=pConfigFile->DecodeFile(DraceIDs, true);

  // 
  // Now we will transfer all possible preSpellNameKey values from the
  // old races to the new races.
  {
    int i, n;
    n = oldRaces.GetRaceCount();
    for (i=0; i<n; i++)
    {
      const RACE_DATA *pOldRace;
      RACE_DATA *pNewRace;
      pOldRace = oldRaces.PeekRace(i);
      if (pOldRace->preSpellNameKey >= 0)
      {
        pNewRace = racePointer->GetRace(pOldRace->RaceID());
        if (pNewRace != NULL)
        {
          if (pNewRace->preSpellNameKey < 0)
          {
            pNewRace->preSpellNameKey = pOldRace->preSpellNameKey;
          };
        };
      };
    };
  };
  
  
  
  
  pRaceData=NULL; // We are done with it.
  // substitute default filenames for resources using "DEFAULT"
//  racePointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


int WriteRaceDB(LPCSTR file, 
                RACE_DATA_TYPE *racePointer, 
                CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pRaceData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
//  racePointer->CollapseDefaultFilenames();
  
  pRaceData=racePointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Race database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the races file!!!\r\n");
  status=pConfigFile->WriteFile(ar, DraceIDs);

  // substitute default filenames for resources using "DEFAULT"
//  racePointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pRaceData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}



// ====================================================================
// 
//  Manipulate ability database
//
// ====================================================================



ABILITY_DATA tempABILITY; // Where we build ability
ABILITY_DATA_TYPE *pAbilityData=NULL;  // Where data will be put

// Handle clearing and storing of entire ABILITY_DATA object
static void abilityObject(CONFIG_PKT *pkt) {
  //static POSITION position;
  static int indx;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempABILITY.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (tempABILITY.m_name.CompareNoCase("template")!=0)
    {
      //if (pAbilityData->FindName(tempABILITY.m_name)!=abilityUnknown)
      if (pAbilityData->LocateAbilityData(tempABILITY.m_name) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in ability.txt = %s\n",tempABILITY.m_name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
      {
        tempABILITY.m_roll.Compile();
        pAbilityData->AddAbility(tempABILITY);
      }
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pAbilityData->GetHeadPosition();
    indx = 0;
    //if (position == NULL)
    if (indx >= pAbilityData->GetAbilityDataCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempABILITY.Clear();
    //tempABILITY.m_name="template";
    //tempABILITY = pAbilityData->PeekAtPos(position);
    tempABILITY = *pAbilityData->PeekAbilityData(indx);
    //pAbilityData->GetNext(position);
    pkt->objectName=tempABILITY.m_name;
    break;
  case CONFIG_FUNC_getNextObject:
    indx++;
    //if (position==NULL) {
    if (indx >= pAbilityData->GetAbilityDataCount())
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempABILITY=pAbilityData->PeekAtPos(position);
      tempABILITY = *pAbilityData->PeekAbilityData(indx);
      //pAbilityData->GetNext(position);
      pkt->objectName=tempABILITY.m_name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}


static void DabilityName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempABILITY.m_name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempABILITY.m_name);
  pkt->objectName=tempABILITY.m_name;
}


static void DabilityKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
  //  CONFIG_ENCODE_long(pkt->value,tempABILITY.m_abilityKey);
    pkt->status = CONFIG_STAT_end;
    return;
  };
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  -2000000000,2000000000,
  //  tempITEM.Protection_Bonus);
  return;
}


static void DabilityAbbrev(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempABILITY.m_abbreviation;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempABILITY.m_abbreviation);
}

static void DabilityRoll(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_dicePlus(
      pkt->value,
      tempABILITY.m_roll);
    return;
  };
  pkt->status=CONFIG_DECODE_dicePlus(
    pkt->value,
    pkt->errorMsg,
    &tempABILITY.m_roll);
}
/*
static void DabilityModify(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_integer(
      pkt->value,
      tempABILITY.m_modify);
    return;
  };
  int temp;
  pkt->status=CONFIG_DECODE_integer(
    pkt->value,
    pkt->errorMsg,
    0,255,
    temp);
  tempABILITY.m_modify=(BYTE)temp;
}
*/
/*
static void DabilityAdjust(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = "";
//    CONFIG_ENCODE_adjust(
//      pkt->value,
//      tempABILITY.m_adjust);
    return;
  };

  pkt->value.TrimLeft();
  pkt->value.TrimRight();
  pkt->value.Remove(' ');

  tempABILITY.AddAdjust(pkt->value);

  //ADJUSTMENTListType list;
  //pkt->status=CONFIG_DECODE_adjust(
  //  pkt->value,
  //  pkt->errorMsg,
  //  list);
  //if (pkt->status==CONFIG_STAT_ok)
  //{
  //  for (int i=0;i<list.GetSize();i++)
  //    tempABILITY.AddAdjust(list[i]);
  //};

}
*/

static void DabilitySpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempABILITY.m_specAbs);
}


#undef df
#undef dh
#define df(a) Dability##a
#define dh(a) IDS_HELP_ability_##a
// Defined above //  #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))

CONFIGID abilityIDs[] =
{
CONFIGID ("",CONFIG_DATA_object,abilityObject),
DEFID ("name"               ,string     ,Name),
DEFID ("Key"                ,long       ,Key),
DEFID ("Abbreviation"       ,string     ,Abbrev),
DEFID ("Roll"               ,dicePlus   ,Roll),
//DEFID ("Modify"             ,long       ,Modify),
//DEFID ("Adjust"             ,adjust     ,Adjust),
DEFID ("Special Ability"    ,string     ,SpecialAbility),
CONFIGID( "",CONFIG_DATA_end,NULL)
};

int ReadAbilityDB(CString &file, 
                  ABILITY_DATA_TYPE *abilityPointer,
                  CONFIG_FILE *pConfigFile)
{
  return ReadAbilityDB((LPCSTR)file, abilityPointer, pConfigFile);
}



int ReadAbilityDB(LPCSTR file,
                 ABILITY_DATA_TYPE *abilityPointer,
                 CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CFile dataFile;
  CONFIG_FILE myConfigFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  CArchive ar( &dataFile, CArchive::load);
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  ar.ReadString(string);
  if (string != "// Ability database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old abilityData
  abilityPointer->Clear();
  // Read and store new abilityData
  ASSERT(pAbilityData==NULL); // Nobody should be using it.
  pAbilityData=abilityPointer;
  status=pConfigFile->DecodeFile(abilityIDs, true);
  pAbilityData=NULL; // We are done with it.
  // substitute default filenames for resources using "DEFAULT"
//  abilityPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


int WriteAbilityDB(LPCSTR file, 
                ABILITY_DATA_TYPE *abilityPointer, 
                CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pAbilityData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
//  abilityPointer->CollapseDefaultFilenames();
  
  pAbilityData=abilityPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Ability database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the abilities file!!!\r\n");
  status=pConfigFile->WriteFile(ar, abilityIDs);

  // substitute default filenames for resources using "DEFAULT"
//  abilityPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pAbilityData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}

// ====================================================================
// 
//  Manipulate BaseClass database
//
// ====================================================================


BASE_CLASS_DATA tempBASECLASS; // Where we build baseClass
BASE_CLASS_DATA_TYPE *pBaseclassData=NULL;  // Where data will be put

// Handle clearing and storing of entire BASE_CLASS_DATA object
static void baseclassObject(CONFIG_PKT *pkt) {
  //static POSITION position;
  static int indx;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempBASECLASS.Clear();
    tempBASECLASS.m_allowedAlignments |= 0x8000; // Not set yet.
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    tempBASECLASS.m_allowedAlignments&=0x7fff;
    if (tempBASECLASS.m_name.IsEmpty()) break;
    if (tempBASECLASS.m_name.CompareNoCase("template")!=0)
    {
      //if (pBaseclassData->FindName(tempBASECLASS.m_name)!=baseclassUnknown)
      if (pBaseclassData->LocateBaseclass(tempBASECLASS.BaseclassID()) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in baseclass.txt = %s\n",tempBASECLASS.m_name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
        pBaseclassData->AddBaseclass(tempBASECLASS);
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    {
//      ABILITY_REFERENCE abref;
//      ABILITY_LIMITS abreq;
//      RACE_REFERENCE raceref;
//      abref.m_abilityName = "Strength";
//      abref.m_abilityKey = 0;
//      abreq.m_ability.m_abilityName = "Strength";
//      abreq.m_ability.m_abilityKey = 0;
 //     raceref.m_raceName = "Human";
 //     raceref.m_raceKey = Human;
      //position=pBaseclassData->GetHeadPosition();
      indx = 0;
      //if (position == NULL)
      if (indx >= pBaseclassData->GetCount())
      {
        pkt->status = CONFIG_STAT_end;
        return;
      };
     //tempBASECLASS.Clear();
//      tempBASECLASS.m_name="template";
//      tempBASECLASS.m_primeReq.Add(abref);
//      tempBASECLASS.m_abilityReq.Add(abreq);
//      tempBASECLASS.m_allowedRaces.Add(raceref);
//      tempBASECLASS.m_expLevels.Add(100);
//      tempBASECLASS.m_expLevels.Add(200);
//      tempBASECLASS.m_expLevels.Add(400);
//      pkt->objectName=tempBASECLASS.m_name;
      //tempBASECLASS = pBaseclassData->PeekAtPos(position);
      tempBASECLASS = *pBaseclassData->PeekBaseclass(indx);
      pkt->objectName=tempBASECLASS.m_name;
      //pBaseclassData->GetNext(position);
    };
    break;
  case CONFIG_FUNC_getNextObject:
    indx++;
    //if (position==NULL) {
    if (indx >= pBaseclassData->GetCount())
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempBASECLASS=pBaseclassData->PeekAtPos(position);
      tempBASECLASS = *pBaseclassData->PeekBaseclass(indx);
      //pBaseclassData->GetNext(position);
      pkt->objectName=tempBASECLASS.m_name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}


static void DbaseclassName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempBASECLASS.m_name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempBASECLASS.m_name);
  pkt->objectName=tempBASECLASS.m_name;
}



static void DbaseclassKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->status = CONFIG_STAT_skip;
    return;
  };
}

static void DbaseclassPrimeRequisite(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->status = CONFIG_STAT_skip;
    return;
  };
}


static void DbaseclassSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempBASECLASS.m_specAbs);
}




static void DbaseclassPrimeAbility(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    //if (tempBASECLASS.GetPrimeReqSize() > 0)
    //if (tempBASECLASS.GetPrimeReqCount() > 0)
    //{
      //pkt->value=tempBASECLASS.m_primeReq[0].m_abilityName;
    //  pkt->value=*tempBASECLASS.PeekPrimeReq(0);
    //};
    NotImplemented(0x94591,false);
    return;
  };
  //ABILITY_ID temp;
  //pkt->status=CONFIG_DECODE_string(
  //  pkt->value,
  //  pkt->errorMsg,
  //  temp);
  //if (pkt->status==CONFIG_STAT_ok)
  //{
  //  tempBASECLASS.Add(temp);
  //};
}

static void DbaseclassAbilityReq (CONFIG_PKT *pkt)
{
  ABILITY_REQ temp;
  if (pkt->IsStoring)
  {
    int i, num;
    //num = tempBASECLASS.m_abilityReq.GetSize();
    num = tempBASECLASS.GetAbilityReqCount();
    if (num > 0)
    {
      i = pkt->listNum;
      CString x;
      //pkt->value = tempBASECLASS.PeekAbilityReq(i)->Name();
      //pkt->value += "  ";
      x = tempBASECLASS.PeekAbilityReq(i)->Encode();
      pkt->value = x;
      if (i+1 < num) pkt->status = CONFIG_STAT_more;
    }
    else
    {
      pkt->status = CONFIG_STAT_end;
    };
    return;
  };
  pkt->status=CONFIG_DECODE_AbilityRequirement(
    pkt->value,
    pkt->errorMsg,
    &temp);
  if (pkt->status==CONFIG_STAT_ok)
        tempBASECLASS.AddAbilityRequirement(temp);
}

static void DbaseclassAllowRace (CONFIG_PKT *pkt)
{
  RACE_ID temp;
  if (pkt->IsStoring)
  {
    int num;
    num = tempBASECLASS.GetRaceIDCount();
    if (num > 0)
    {
      //pkt->value = tempBASECLASS.m_allowedRaces[pkt->listNum].m_raceName;
      pkt->value = *tempBASECLASS.PeekRaceID(pkt->listNum);
      if (pkt->listNum + 1 < num) pkt->status = CONFIG_STAT_more;
    }
    else
    {
      pkt->status = CONFIG_STAT_end;
    };
    return;
  };
  {
    CString name;
    pkt->status=CONFIG_DECODE_string(
      pkt->value,
      pkt->errorMsg,
      //temp.m_raceName);
      name);
    if (pkt->status==CONFIG_STAT_ok)
    {
      temp = name;
      tempBASECLASS.AddAllowedRace(temp);
    };
  };
}

static void DbaseclassExpLevel (CONFIG_PKT *pkt)
{
  long temp;
  if (pkt->IsStoring)
  {
    int num;
    CString x;
    num = tempBASECLASS.m_expLevels.GetSize();
    if (num > 0)
    {
      x.Format("%d", *tempBASECLASS.PeekExpLevels(pkt->listNum));
      pkt->value = x;
      if (pkt->listNum + 1 < num) pkt->status = CONFIG_STAT_more;
    }
    else
    {
      pkt->status = CONFIG_STAT_end;
    };
    return;
  };
  pkt->status=CONFIG_DECODE_long(
    pkt->value,
    pkt->errorMsg,
    0,999999999,
    temp);
  if (pkt->status==CONFIG_STAT_ok)
    tempBASECLASS.AddExpLevel(temp);
        //tempBASECLASS.AddExpLevel(long2log(temp));
}


static void DbaseclassAllowAlign(CONFIG_PKT *pkt)
{
  static char alignments[9][16] =
  {
    "Lawful Good",
    "Neutral Good",
    "Chaotic Good",
    "Lawful Neutral",
    "True Neutral",
    "Chaotic Neutral",
    "Lawful Evil",
    "Neutral Evil",
    "Chaotic Evil"
  };
    
  if (pkt->IsStoring)
  {
    int i;
    if (tempBASECLASS.m_allowedAlignments == 0)
    {
      pkt->value = "none";
      return;
    };
    if (tempBASECLASS.m_allowedAlignments == 0x1ff)
    {
      pkt->value = "any";
      return;
    };
    {
      int j, m, k = tempBASECLASS.m_allowedAlignments;
      for (j=-1,i=0,m=1; (i<=pkt->listNum) && (k!=0); j++,m<<=1)
      {
        if (k & m)
        {
          k = k^m;
          i++;
        };
      };
      pkt->value = alignments[j];
      if (k != 0) pkt->status = CONFIG_STAT_more;
      return;
    };
  };
  CString temp;
  pkt->status = CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    temp);
  if (pkt->status == CONFIG_STAT_ok)
  {
    for (int i=0; i<9; i++)
    {
      if (temp.CompareNoCase(alignments[i])==0)
      {
        if (tempBASECLASS.m_allowedAlignments&0x8000)
          tempBASECLASS.m_allowedAlignments=0;
        tempBASECLASS.m_allowedAlignments |= 1<<i;
        return;
      };
    };
    if (temp.CompareNoCase("any")==0)
    {
      tempBASECLASS.m_allowedAlignments=0x1ff;
    }
    else
    {
      pkt->status=CONFIG_STAT_value;
    };
  };
}

static void DbaseclassHitDice(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (pkt->listNum >= HIGHEST_CHARACTER_LEVEL)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const DICEDATA *pDice;
    pDice = &tempBASECLASS.hitDice[pkt->listNum];
    pkt->value.Format("%d,%d,%d,%d", pkt->listNum+1,
                                     pDice->nbr,
                                     pDice->sides,
                                     pDice->bonus);
    pkt->status = CONFIG_STAT_more;
    return;
  };
  int values[4];
  const char *src;
  src = (LPCTSTR)pkt->value;
  DecodeArray(pkt, src, values, 4);
  if ( (values[0] <= HIGHEST_CHARACTER_LEVEL) && (values[0] > 0) )
  {
    DICEDATA hitDice;
    hitDice.nbr = values[1];
    hitDice.sides = values[2];
    hitDice.bonus = values[3];
    tempBASECLASS.hitDice[values[0]-1] = hitDice;
  }
  else
  {
    pkt->status = CONFIG_STAT_value;
  };
};

static void DbaseclassSkill(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempBASECLASS.GetSkillCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const SKILL *pSkill;
    pSkill = tempBASECLASS.PeekSkill(pkt->listNum);
    pSkill->Encode(pkt);
    return;
  };
  {
    SKILL skill;
    skill.Decode(pkt);
    tempBASECLASS.Add(skill);
  };
};

static void DbaseclassSkillAdjAbility(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int i, n;
    n = tempBASECLASS.GetSkillAdjAbilityCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    char line[1000];
    int values[HIGHEST_CHARACTER_PRIME];
    int sum;
    char *dst;
    const SKILL_ADJUSTMENT_ABILITY *pSkillAdj;
    pSkillAdj = tempBASECLASS.PeekSkillAdjAbility(pkt->listNum);
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      values[i] = pSkillAdj->skillAdj[i];
    };
    dst = line+999;
    *dst = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_PRIME, dst, &sum);
    *(--dst) = pSkillAdj->adjType;
    *(--dst) = ',';
    dst = PrependString(pSkillAdj->abilityID, dst);
    dst = PrependString(pSkillAdj->skillID, dst);
    pkt->value = dst+1;
    pkt->status = CONFIG_STAT_more;
    return;
  };
  const char *src;
  char adjType;
  SKILL_ADJUSTMENT_ABILITY saa;
  CString skillName;
  CString abilityName;
  CString type;
  src = (LPCSTR)pkt->value;
  src = GetPrefix(pkt, src, &skillName);
  src = GetPrefix(pkt, src, &abilityName);
  saa.abilityID = abilityName;
  saa.skillID = skillName;
  int i;
  int values[HIGHEST_CHARACTER_PRIME];
  src = GetPrefix(pkt, src, &type);
  if (!type.IsEmpty())
  {
    adjType = type[0];
  }
  else
  {
    adjType = ' ';
  };
  saa.adjType = adjType;
  src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_PRIME);
  for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
  {
    saa.skillAdj[i] = values[i];
  };
  tempBASECLASS.AddSkillAdjAbility(saa.skillID,
                                   saa.abilityID,
                                   saa.adjType,
                                   saa.skillAdj,
                                   sizeof(saa.skillAdj));
};

static void DbaseclassBonusXP(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int i, n;
    n = tempBASECLASS.GetBonusXPCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    char line[1000];
    int values[HIGHEST_CHARACTER_PRIME];
    int sum;
    char *dst;
    const BONUS_XP *pBonusXP;
    pBonusXP = tempBASECLASS.PeekBonusXP(pkt->listNum);
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      values[i] = pBonusXP->bonus[i];
    };
    dst = line+999;
    *dst = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_PRIME, dst, &sum);
    *(--dst) = pBonusXP->bonusType;
    *(--dst) = ',';
    dst = PrependString(pBonusXP->abilityID, dst);
    pkt->value = dst+1;
    pkt->status = CONFIG_STAT_more;
    return;
  };
  int i;
  int values[HIGHEST_CHARACTER_PRIME];
  BONUS_XP bonusXP;
  CString type;
  const char *src;
  src = (LPCTSTR)pkt->value;
  src = GetPrefix(pkt, src, &bonusXP.abilityID);
  src = GetPrefix(pkt, src, &type);
  bonusXP.bonusType = ' ';
  if (type.GetLength() > 0)
  {
    bonusXP.bonusType = type[0];
  };
  DecodeArray(pkt, src, values, HIGHEST_CHARACTER_PRIME);
  for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
  {
    bonusXP.bonus[i] = values[i];
  };
  tempBASECLASS.Add(bonusXP);
};

static void DbaseclassSkillAdjBaseclass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempBASECLASS.GetSkillAdjBaseclassCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const SKILL_ADJUSTMENT_BASECLASS *psab;
    psab = tempBASECLASS.PeekSkillAdjBaseclass(pkt->listNum);
    psab->Encode(pkt);
    return;
  };
  SKILL_ADJUSTMENT_BASECLASS sab;
  sab.Decode(pkt);
  tempBASECLASS.Add(sab);
};

static void DbaseclassSkillAdjRace(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempBASECLASS.GetSkillAdjRaceCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const SKILL_ADJUSTMENT_RACE *pSkillAdjRace;
    pSkillAdjRace = tempBASECLASS.PeekSkillAdjRace(pkt->listNum);
    pSkillAdjRace->Encode(pkt);
    pkt->status = CONFIG_STAT_more;
    return;
  };
  SKILL_ADJUSTMENT_RACE skillAdjRace;
  skillAdjRace.Decode(pkt);
  if (pkt->status != CONFIG_STAT_ok) return;
  tempBASECLASS.Add(skillAdjRace);
};

static void DbaseclassSkillAdjScript(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    n = tempBASECLASS.GetSkillAdjScriptCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    NotImplemented(0x1e76cac, false);
    return;
  };
  NotImplemented(0x5e776ca9, false);
};

static void DbaseclassSpellBonusAbility(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempBASECLASS.m_spellBonusAbility;
    return;
  };
  tempBASECLASS.m_spellBonusAbility = pkt->value;
};

static void DbaseclassBonusSpells(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int i, n;
    CString temp;
    if (pkt->listNum > HIGHEST_CHARACTER_PRIME)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    n = tempBASECLASS.m_bonusSpells.GetSize();
    pkt->value.Empty();
    for (i=0; i<n; i+=3)
    {
      if (tempBASECLASS.m_bonusSpells[i] == pkt->listNum)
      {
        if (pkt->value.IsEmpty())
        {
          pkt->value.Format("%d", tempBASECLASS.m_bonusSpells[i]);
        };
        temp.Format(",%d,%d",tempBASECLASS.m_bonusSpells[i+1],tempBASECLASS.m_bonusSpells[i+2]);
        pkt->value += temp;
      }
    };
    if (pkt->value.IsEmpty())
    {
      pkt->status = CONFIG_STAT_next;
      return;
    };
    pkt->status = CONFIG_STAT_more;
    return;
  };
  {
    int prime, n, values[2];
    const char *src;
    src = (LPCSTR)pkt->value;
    src = DecodeArray(pkt, src, values, 1);
    if (pkt->status != CONFIG_STAT_ok) return;
    if (   (values[0] > HIGHEST_CHARACTER_PRIME) 
        || (values[0] < 1))
    {
      pkt->status = CONFIG_STAT_value;
      return;
    };
    prime = values[0];
    while (*src != 0)
    {
      src = DecodeArray(pkt, src, values, 2);
      if (pkt->status != CONFIG_STAT_ok) return;
      n = tempBASECLASS.m_bonusSpells.GetSize();
      tempBASECLASS.m_bonusSpells.SetSize(n+3);
      tempBASECLASS.m_bonusSpells[n] = prime;
      tempBASECLASS.m_bonusSpells[n+1] = values[0];
      tempBASECLASS.m_bonusSpells[n+2] = values[1];
    };
  };
};

static void DbaseclassMaxSpellLevelByPrime(CONFIG_PKT *pkt)
{
  int col, sum, i;
  if (pkt->IsStoring)
  {
    int school;
    school = pkt->listNum;
    if (school >= tempBASECLASS.GetCastingInfoCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const CASTING_INFO *pCastingInfo;
    char *dst;
    pCastingInfo = tempBASECLASS.PeekCastingInfo(school);
    char line[800];
    col=800;
    int values[HIGHEST_CHARACTER_PRIME];
    dst = line + 800;
    *(--dst) = 0;
    for (i = HIGHEST_CHARACTER_PRIME-1; i>=0; i--)
    {
      values[i] = pCastingInfo->m_maxSpellLevelsByPrime[i];
    };
    sum = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_PRIME, dst, &sum);
    if (sum == 0)
    {
      pkt->status = CONFIG_STAT_skip;
      return;
    };
    dst = PrependString(pCastingInfo->primeAbility, dst);
    dst = PrependString(pCastingInfo->schoolID, dst);
    pkt->value = dst+1;  
    pkt->status = CONFIG_STAT_more;
    return;
  };
  {
    SCHOOL_ID schoolID;
    const char *src;
    src = (LPCTSTR)pkt->value;
    int values[HIGHEST_CHARACTER_PRIME];
    CASTING_INFO *pCastingInfo;
    src = GetPrefix(pkt, src, &schoolID);
    pCastingInfo = tempBASECLASS.GetCastingInfo(schoolID);
    if (pCastingInfo == NULL)
    {
      CASTING_INFO castingInfo;
      castingInfo.schoolID = schoolID;
      tempBASECLASS.Add(castingInfo);
      pCastingInfo = tempBASECLASS.GetCastingInfo(schoolID);
    };
    src = GetPrefix(pkt, src, &pCastingInfo->primeAbility);
    src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_PRIME);
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      pCastingInfo->m_maxSpellLevelsByPrime[i] = values[i];
    };
  };
};

static void DbaseclassMaxNumSpellsByPrime(CONFIG_PKT *pkt)
{
  int col, sum, i;
  if (pkt->IsStoring)
  {
    int school;
    school = pkt->listNum;
    if (school >= tempBASECLASS.GetCastingInfoCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const CASTING_INFO *pCastingInfo;
    char *dst;
    pCastingInfo = tempBASECLASS.PeekCastingInfo(school);
    char line[800];
    col=800;
    int values[HIGHEST_CHARACTER_PRIME];
    dst = line + 800;
    *(--dst) = 0;
    for (i = HIGHEST_CHARACTER_PRIME-1; i>=0; i--)
    {
      values[i] = pCastingInfo->m_maxSpellsByPrime[i];
    };
    sum = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_PRIME, dst, &sum);
    if (sum == 0)
    {
      pkt->status = CONFIG_STAT_skip;
      return;
    };
    dst = PrependString(pCastingInfo->primeAbility, dst);
    dst = PrependString(pCastingInfo->schoolID, dst);
    pkt->value = dst+1;  
    pkt->status = CONFIG_STAT_more;
    return;
  };
  {
    SCHOOL_ID schoolID;
    ABILITY_ID abilityID;
    const char *src;
    src = (LPCTSTR)pkt->value;
    int values[HIGHEST_CHARACTER_PRIME];
    CASTING_INFO *pCastingInfo;
    src = GetPrefix(pkt, src, &schoolID);
    pCastingInfo = tempBASECLASS.GetCastingInfo(schoolID);
    if (pCastingInfo == NULL)
    {
      CASTING_INFO castingInfo;
      castingInfo.schoolID = schoolID;
      tempBASECLASS.Add(castingInfo);
      pCastingInfo = tempBASECLASS.GetCastingInfo(schoolID);
    };
    src = GetPrefix(pkt, src, &abilityID);
    pCastingInfo->primeAbility = abilityID;
    src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_PRIME);
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      pCastingInfo->m_maxSpellsByPrime[i] = values[i];
    };
  };
};



static void DbaseclassBaseSpellLevels(CONFIG_PKT *pkt)
{
  // Lines of form:
  // Base Spell Level = <baseclassLevel>,<spellSchool>,<#level 1 spells>,<#level 2 spells>, etc
  // So there are:
  //    MAX_SPELL_LEVEL entries on each line.
  //    HIGHEST_CHARACTER_LEVEL lines for each Spell School.
  int col, sum, i, val;
  if (pkt->IsStoring)
  {
    int school, level;
    school = pkt->listNum / HIGHEST_CHARACTER_LEVEL;
    level  = pkt->listNum % HIGHEST_CHARACTER_LEVEL;
    if (school >= tempBASECLASS.GetCastingInfoCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    const CASTING_INFO *pCastingInfo;
    char *dst;
    pCastingInfo = tempBASECLASS.PeekCastingInfo(school);
    char line[800];
    col=800;
    int values[MAX_SPELL_LEVEL];
    dst = line + 800;
    *(--dst) = 0;
    for (i = MAX_SPELL_LEVEL-1; i>=0; i--)
    {
      values[i] = pCastingInfo->m_spellLimits[level][i];
    };
    sum = 0;
    dst = EncodeArray(values, MAX_SPELL_LEVEL, dst, &sum);
    if (sum == 0)
    {
      pkt->status = CONFIG_STAT_skip;
      return;
    };
    dst = PrependString(pCastingInfo->schoolID, dst);
    pkt->value = dst+1;  
    val = level+1;
    do
    {
      *(--dst) = val % 10 + '0';
      val /= 10;
    } while (val);
    pkt->value = dst;
    pkt->status = CONFIG_STAT_more;
    return;
  };
  {
    int values[HIGHEST_CHARACTER_LEVEL];
    int level;
    SCHOOL_ID schoolID;
    if (pkt->value.IsEmpty()) return;
    const char *src = (LPCSTR)pkt->value;
    src = DecodeArray(pkt, src, &level, 1);
    if ( (level < 1) || (level > HIGHEST_CHARACTER_LEVEL) )
    {
      pkt->status = CONFIG_STAT_value;
      return;
    };
    src = GetPrefix(pkt, src, &schoolID);
    CASTING_INFO *pCastingInfo;
    pCastingInfo = tempBASECLASS.GetCastingInfo(schoolID);
    if (pCastingInfo == NULL)
    {
      CASTING_INFO castingInfo;
      castingInfo.schoolID = schoolID;
      tempBASECLASS.Add(castingInfo);
      pCastingInfo = tempBASECLASS.GetCastingInfo(schoolID);
    };
    src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_LEVEL);
    for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
    {
      pCastingInfo->m_spellLimits[level-1][i] = values[i];
    };
  };
};

static void DbaseclassTHAC0(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int i, sum = 0;
    char *dst;
    char line[800];
    int values[HIGHEST_CHARACTER_LEVEL];
    dst = line + 800;
    *(--dst) = 0;
    for (i = HIGHEST_CHARACTER_LEVEL-1; i>=0; i--)
    {
      values[i] = tempBASECLASS.THAC0[i];
    };
    sum = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_LEVEL, dst, &sum);
    if (sum != 0) pkt->value = dst + 1;
    else pkt->value.Empty();
    return;
  }
  else
  {
    int i;
    const char *src;
    src = (LPCTSTR)pkt->value;
    int values[HIGHEST_CHARACTER_LEVEL];
    src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_LEVEL);
    for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
    {
      tempBASECLASS.THAC0[i] = values[i];
    };
  };
}


#undef df
#undef dh
#define df(a) Dbaseclass##a
#define dh(a) IDS_HELP_baseclass_##a
// Defined above //  #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))

CONFIGID baseclassIDs[] =
{
CONFIGID ("",CONFIG_DATA_object,baseclassObject),
DEFID ("name"                ,string     ,Name),
DEFID ("Key"                 ,long       ,Key),
//DEFIDn("Prime Ability"       ,string     ,PrimeAbility ,9),
DEFID ("Prime Requisite"     ,string     ,PrimeRequisite),
DEFID ("BonusXP"             ,string     ,BonusXP),
DEFID ("THAC0"               ,string     ,THAC0),
DEFID ("AbilityRequirement"  ,AbilityReq ,AbilityReq),
DEFID ("Allowed Race"        ,string     ,AllowRace),
DEFID ("Allowed Alignment"   ,string     ,AllowAlign),
DEFID ("ExpLevel"            ,integer    ,ExpLevel),
DEFID ("Base Spell Levels"   ,string     ,BaseSpellLevels),
DEFID ("MaxSpellLevelByPrime",string     ,MaxSpellLevelByPrime),
DEFID ("MaxNumSpellsByPrime" ,string     ,MaxNumSpellsByPrime),
DEFID ("HitDice"             ,string     ,HitDice),
DEFID ("Skill"               ,string     ,Skill),
DEFID ("SkillAdjAbility"     ,string     ,SkillAdjAbility),
DEFID ("SkillAdjBaseclass"   ,string     ,SkillAdjBaseclass),
DEFID ("SkillAdjRace"        ,string     ,SkillAdjRace),
DEFID ("SkillAdjScript"      ,string     ,SkillAdjScript),
DEFID ("SpellBonusAbility"   ,string     ,SpellBonusAbility),
DEFID ("BonusSpells"         ,string     ,BonusSpells),
DEFID("Special Ability"     ,string     ,SpecialAbility),
CONFIGID( "",CONFIG_DATA_end,NULL)
};

int ReadBaseclassDB(CString &file, 
                    BASE_CLASS_DATA_TYPE *baseclassPointer,
                    CONFIG_FILE *pConfigFile)
{
  return ReadBaseclassDB((LPCSTR)file, baseclassPointer, pConfigFile);
}



int ReadBaseclassDB(LPCSTR file,
                    BASE_CLASS_DATA_TYPE *baseclassPointer,
                    CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CFile dataFile;
  CONFIG_FILE myConfigFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  CArchive ar( &dataFile, CArchive::load);
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  ar.ReadString(string);
  if (string != "// BaseClass database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old baseclassData
  baseclassPointer->Clear();
  // Read and store new baseclassData
  ASSERT(pBaseclassData==NULL); // Nobody should be using it.
  pBaseclassData=baseclassPointer;
  status=pConfigFile->DecodeFile(baseclassIDs, true);
  pBaseclassData=NULL; // We are done with it.
  // substitute default filenames for resources using "DEFAULT"
//  baseclassPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


int WriteBaseclassDB(LPCSTR file, 
                     BASE_CLASS_DATA_TYPE *baseclassPointer, 
                     CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pBaseclassData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
//  baseclassPointer->CollapseDefaultFilenames();
  
  pBaseclassData=baseclassPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// BaseClass database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the baseclass file!!!\r\n");
  status=pConfigFile->WriteFile(ar, baseclassIDs);

  // substitute default filenames for resources using "DEFAULT"
//  baseclassPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pBaseclassData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}


// ====================================================================
// 
//  Manipulater class database
//
// ====================================================================


CLASS_DATA tempCLASS; // Where we build class
CLASS_DATA_TYPE *pClassData=NULL;  // Where data will be put

// Handle clearing and storing of entire CLASS_DATA object
static void classObject(CONFIG_PKT *pkt) {
  //static POSITION position;
  static int indx;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempCLASS.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (   (tempCLASS.m_name.CompareNoCase("template")!=0)
         && !tempCLASS.m_name.IsEmpty())
    {
      //if (pClassData->FindName(tempCLASS.m_name)!=classUnknown)
      if (pClassData->LocateClass(tempCLASS.ClassID()) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in class.txt = %s\n",tempCLASS.m_name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
        //pClassData->AddClass(tempCLASS);
        pClassData->AddClass(&tempCLASS);
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pClassData->GetHeadPosition();
    indx = 0;
    //if (position == NULL)
    if (indx >= pClassData->GetCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempCLASS.Clear();
    //tempCLASS.m_name="template";
    //tempCLASS = pClassData->PeekAtPos(position);
    tempCLASS = *pClassData->PeekClass(indx);
    //pClassData->GetNext(position);
    pkt->objectName=tempCLASS.m_name;
    break;
  case CONFIG_FUNC_getNextObject:
    indx++;
    //if (position==NULL) {
    if (indx >= pClassData->GetCount()) 
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempCLASS=pClassData->PeekAtPos(position);
      tempCLASS = *pClassData->PeekClass(indx);
      //pClassData->GetNext(position);
      pkt->objectName=tempCLASS.m_name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}

static void DclassName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempCLASS.m_name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempCLASS.m_name);
  pkt->objectName=tempCLASS.m_name;
}

static void DclassHitDiceLevelBonus(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    if (pkt->listNum >= tempCLASS.GetHitDiceLevelBonusCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    char r[3000];
    const HIT_DICE_LEVEL_BONUS *pHitDiceLevelBonus;
    pHitDiceLevelBonus = tempCLASS.PeekHitDiceLevelBonus(pkt->listNum);
    char *dst = r+3000;
    int values[HIGHEST_CHARACTER_PRIME];
    int i, sum=0;
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      values[i] = pHitDiceLevelBonus->bonusValues[i];
    };
    *(--dst) = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_PRIME, dst, &sum);
    dst = PrependString(pHitDiceLevelBonus->ability, dst);
    dst = PrependString(pHitDiceLevelBonus->baseclassID, dst);
    pkt->value = dst+1;
    pkt->status = CONFIG_STAT_more;
    return;
  };
  {
    const char *src;
    HIT_DICE_LEVEL_BONUS hdlb;
    int values[HIGHEST_CHARACTER_PRIME];
    src = (LPCTSTR)pkt->value;
    int k;
    CString temp;
    //tempCLASS.hitDiceBonusAbility = r;
    //src = GetPrefix(pkt, src, &tempCLASS.hitDiceBonusAbility);
    src = GetPrefix(pkt, src, &temp);
    hdlb.baseclassID = temp;
    src = GetPrefix(pkt, src, &temp);
    hdlb.ability = temp;
    src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_PRIME);
    for (k=0; k<HIGHEST_CHARACTER_PRIME; k++)
    {
      //tempCLASS.hitDiceBonusValues[k] = values[k];
      hdlb.bonusValues[k] = values[k];
    };
    tempCLASS.hitDiceLevelBonus.Add(hdlb);
  };
}
static void DclassHitDiceBonus(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    /*
    char r[3000];
    char *dst = r+3000;
    int values[HIGHEST_CHARACTER_PRIME];
    int i, sum=0;
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      values[i] = tempCLASS.hitDiceBonusValues[i];
    };
    *(--dst) = 0;
    dst = EncodeArray(values, HIGHEST_CHARACTER_PRIME, dst, &sum);
    dst = PrependString(tempCLASS.hitDiceBonusAbility, dst);
    pkt->value = dst+1;
    */
    return;
  };
  {
    const char *src;
    HIT_DICE_LEVEL_BONUS hdlb;
    int values[HIGHEST_CHARACTER_PRIME];
    src = (LPCTSTR)pkt->value;
    int k;
    CString temp;
    //tempCLASS.hitDiceBonusAbility = r;
    //src = GetPrefix(pkt, src, &tempCLASS.hitDiceBonusAbility);
    src = GetPrefix(pkt, src, &temp);
    hdlb.ability = temp;
    src = DecodeArray(pkt, src, values, HIGHEST_CHARACTER_PRIME);
    for (k=0; k<HIGHEST_CHARACTER_PRIME; k++)
    {
      //tempCLASS.hitDiceBonusValues[k] = values[k];
      hdlb.bonusValues[k] = values[k];
    };
    if (tempCLASS.GetCount() == 0)
    {
      hdlb.baseclassID = "";  // Filled in when baseclasses are defined
    }
    else
    {
      hdlb.baseclassID = *tempCLASS.PeekBaseclassID(0);
    };

    tempCLASS.hitDiceLevelBonus.Add(hdlb);
  };
}
/*
static void DclassHitDiceBonusAttr(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    NotImplemented(0xb123a8, false);
  };
  NotImplemented(0xc1209a, false);
}
*/
static void DclassHitDiceBaseclass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempCLASS.hitDiceBaseclassID;
    return;
  };
  tempCLASS.hitDiceBaseclassID = pkt->value;
}
/*
static void DclassHitDiceBonusDice(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    NotImplemented(0xf123a8, false);
  };
  NotImplemented(0x12209a, false);
}
*/
static void DclassStartingEquipment(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    int n;
    pkt->value.Empty();
    n = tempCLASS.m_startingEquipment.GetCount();
    if (pkt->listNum >= n)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    {
      POSITION pos;
      int i;
      ITEM *pItem = NULL;
      pos = tempCLASS.m_startingEquipment.GetHeadPosition();
      for (i=0; i<=pkt->listNum; i++)
      {
        pItem = &tempCLASS.m_startingEquipment.GetNext(pos);
      };
      pkt->value.Format("%s,%d",(LPCSTR)pItem->itemID, pItem->qty);
      pkt->status = CONFIG_STAT_more;
    };
    return;
  };
  if (pkt->value.IsEmpty()) return;
  {
    ITEM item;
    const char *src;
    int values[1];
    src = GetPrefix(pkt, pkt->value,&item.itemID);
    DecodeArray(pkt, src, values, 1);
    //item.itemID = pkt->value;
    if (pkt->status == CONFIG_STAT_ok)
    {
      item.qty = values[0];
    }
    else
    {
      item.qty = 1;
      pkt->status = CONFIG_STAT_ok;
    };
    tempCLASS.m_startingEquipment.AddItem(item);
  }
}

static void DclassStrengthBonusDice(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = tempCLASS.strengthBonusDice.EncodeText();
    return;
  };
  if (pkt->value.IsEmpty()) return;
  if (!tempCLASS.strengthBonusDice.DecodeText(pkt->value))
  {
    pkt->status = CONFIG_STAT_value;
  };
  return;
}



static void DclassKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    //CONFIG_ENCODE_long(pkt->value,tempCLASS.m_classKey);
    pkt->status = CONFIG_STAT_end;
    return;
  };
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  -2000000000,2000000000,
  //  tempITEM.Protection_Bonus);
}



static void DclassBaseclass(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    //if (pkt->listNum >= tempCLASS.m_baseclasses.GetSize())
    if (pkt->listNum >= tempCLASS.m_baseclasses.GetCount())
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    pkt->status = CONFIG_STAT_more;
    //pkt->value = tempCLASS.m_baseclasses[pkt->listNum].m_baseclassName;
    pkt->value = *tempCLASS.m_baseclasses.PeekBaseclassID(pkt->listNum);

    return;
  };
  BASECLASS_ID temp;
  CString str;
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    str);
  if (pkt->status==CONFIG_STAT_ok)
  {
    int i, n;
    temp = str;
    tempCLASS.AddBaseclass(temp);
    // Fill in any HitPointLevelBonus that did not have a baseclass defined.
    n = tempCLASS.GetHitDiceLevelBonusCount();
    for (i=0; i<n; i++)
    {
      if (tempCLASS.PeekHitDiceLevelBonus(i)->baseclassID.IsEmpty())
      {
        tempCLASS.GetHitDiceLevelBonus(i)->baseclassID = temp;
      };
    };
  };
}

static void DclassSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempCLASS.m_specialAbilities);
}




#undef df
#undef dh
#define df(a) Dclass##a
#define dh(a) IDS_HELP_class_##a
// Defined above //  #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))

CONFIGID classIDs[] =
{
CONFIGID ("",CONFIG_DATA_object,classObject),
DEFID ("name"               ,string     ,Name),
DEFID ("Key"                ,long       ,Key),
DEFID ("HitDiceBonus"       ,string     ,HitDiceBonus),
DEFID ("HitDiceLevelBonus"  ,string     ,HitDiceLevelBonus),
DEFID ("HitDiceBaseclass"   ,string     ,HitDiceBaseclass),
DEFID ("StrengthBonusDice"  ,dicePlus   ,StrengthBonusDice),
DEFID ("Starting Equipment" ,string     ,StartingEquipment),
DEFID ("Baseclass"          ,string     ,Baseclass),
DEFID ("Special Ability"    ,string     ,SpecialAbility),
CONFIGID( "",CONFIG_DATA_end,NULL)
};

int ReadClassDB(CString &file, 
                CLASS_DATA_TYPE *classPointer,
                CONFIG_FILE *pConfigFile)
{
  return ReadClassDB((LPCSTR)file, classPointer, pConfigFile);
}


int ReadClassDB(LPCSTR file,
                CLASS_DATA_TYPE *classPointer,
                CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CFile dataFile;
  CONFIG_FILE myConfigFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  CLASS_DATA_TYPE oldClasses;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  CArchive ar( &dataFile, CArchive::load);
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  ar.ReadString(string);
  if (string != "// Class database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old classData
  oldClasses = *classPointer;
  classPointer->Clear();
  // Read and store new classData
  ASSERT(pClassData==NULL); // Nobody should be using it.
  pClassData=classPointer;
  status=pConfigFile->DecodeFile(classIDs, true);

  
  // 
  // Now we will transfer all possible preSpellNameKey values from the
  // old classes to the new classes.
  {
    int i, n;
    n = oldClasses.GetClassCount();
    for (i=0; i<n; i++)
    {
      const CLASS_DATA *pOldClass;
      CLASS_DATA *pNewClass;
      pOldClass = oldClasses.PeekClass(i);
      if (pOldClass->preSpellNameKey >= 0)
      {
        pNewClass = classPointer->GetClass(pOldClass->ClassID());
        if (pNewClass != NULL)
        {
          if (pNewClass->preSpellNameKey < 0)
          {
            pNewClass->preSpellNameKey = pOldClass->preSpellNameKey;
          };
        };
      };
    };
  };
  
  
  
  
  pClassData=NULL; // We are done with it.
  // substitute default filenames for resources using "DEFAULT"
//  classPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


int WriteClassDB(LPCSTR file, 
                 CLASS_DATA_TYPE *classPointer, 
                 CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pClassData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
//  classPointer->CollapseDefaultFilenames();
  
  pClassData=classPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Class database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the classes file!!!\r\n");
  status=pConfigFile->WriteFile(ar, classIDs);

  // substitute default filenames for resources using "DEFAULT"
//  classPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pClassData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}

// ====================================================================
// 
//  Manipulate trait database
//
// ====================================================================


#ifdef USE_TRAITS
TRAIT_DATA tempTRAIT; // Where we build trait
TRAIT_DATA_TYPE *pTraitData=NULL;  // Where data will be put

// Handle clearing and storing of entire TRAIT_DATA object
static void traitObject(CONFIG_PKT *pkt) {
  static POSITION position;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempTRAIT.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (tempTRAIT.m_name.CompareNoCase("template")!=0)
    {
      if (pTraitData->FindName(tempTRAIT.m_name)!=traitUnknown)
      {
        CString msg;
        msg.Format("Duplicate name in trait.txt = %s\n",tempTRAIT.m_name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
      {
        tempTRAIT.m_roll.Compile();
        pTraitData->AddTrait(tempTRAIT);
      }
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    position=pTraitData->GetHeadPosition();
    if (position == NULL)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempTRAIT.Clear();
    //tempTRAIT.m_name="template";
    tempTRAIT = pTraitData->PeekAtPos(position);
    pTraitData->GetNext(position);
    pkt->objectName=tempTRAIT.m_name;
    break;
  case CONFIG_FUNC_getNextObject:
    if (position==NULL) {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      tempTRAIT=pTraitData->PeekAtPos(position);
      pTraitData->GetNext(position);
      pkt->objectName=tempTRAIT.m_name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}


static void DtraitName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempTRAIT.m_name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempTRAIT.m_name);
  pkt->objectName=tempTRAIT.m_name;
}

static void DtraitKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempTRAIT.m_traitKey);
    return;
  };
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  -2000000000,2000000000,
  //  tempITEM.Protection_Bonus);
}



static void DtraitAdjust(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = "";
    WriteDebugString("DtraitAdjust(IsStoring) not implemented\n");
    return;
  };

  pkt->value.TrimLeft();
  pkt->value.TrimRight();
  pkt->value.Remove(' ');
  tempTRAIT.AddAdjust(pkt->value);

  /*
  ADJUSTMENTListType list;
  pkt->status=CONFIG_DECODE_adjust(
    pkt->value,
    pkt->errorMsg,
    list);
  if (pkt->status==CONFIG_STAT_ok) 
  {
    for (int i=0;i<list.GetSize();i++)
      tempTRAIT.AddAdjust(list[i]);
  }
  */
}

static void DtraitSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempTRAIT.m_specAbs);
}



#undef df
#undef dh
#define df(a) Dtrait##a
#define dh(a) IDS_HELP_trait_##a
// Defined above //  #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))

CONFIGID traitIDs[] =
{
CONFIGID ("",CONFIG_DATA_object,traitObject),
DEFID ("name"               ,string     ,Name),
DEFID ("Key"                ,long       ,Key),
DEFID ("adjust"             ,adjust     ,Adjust),
DEFID ("Special Ability"    ,string     ,SpecialAbility),
CONFIGID( "",CONFIG_DATA_end,NULL)
};



int ReadTraitDB(LPCSTR file,
                TRAIT_DATA_TYPE *traitPointer,
                CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CFile dataFile;
  CONFIG_FILE myConfigFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  CArchive ar( &dataFile, CArchive::load);
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  ar.ReadString(string);
  if (string != "// Trait database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old traitData
  traitPointer->Clear();
  // Read and store new traitData
  ASSERT(pTraitData==NULL); // Nobody should be using it.
  pTraitData=traitPointer;
  status=pConfigFile->DecodeFile(traitIDs, true);
  pTraitData=NULL; // We are done with it.
  // substitute default filenames for resources using "DEFAULT"
//  traitPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


int WriteTraitDB(LPCSTR file, 
                 TRAIT_DATA_TYPE *traitPointer, 
                 CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pTraitData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
//  traitPointer->CollapseDefaultFilenames();
  
  pTraitData=traitPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Trait database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the trait file!!!\r\n");
  status=pConfigFile->WriteFile(ar, traitIDs);

  // substitute default filenames for resources using "DEFAULT"
//  traitPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internl error encoding and writing file");
    return -1;
  };
  pTraitData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}
#endif
// ====================================================================
// 
//  Manipulate spellgroup database
//
// ====================================================================



SPELLGROUP_DATA tempSPELLGROUP; // Where we build Spell Group
SPELLGROUP_DATA_TYPE *pSpellgroupData=NULL;  // Where data will be put

// Handle clearing and storing of entire SPELLGROUP_DATA object
static void spellgroupObject(CONFIG_PKT *pkt) {
  static POSITION position;
  switch (pkt->func)
  {
  case CONFIG_FUNC_beginObject:
    tempSPELLGROUP.Clear();
    break;
  case CONFIG_FUNC_storeObject:
    // don't add templates to database
    if (tempSPELLGROUP.m_name.CompareNoCase("template")!=0)
    {
      //if (pSpellgroupData->FindName(tempSPELLGROUP.m_name)!=spellgroupUnknown)
      if (pSpellgroupData->LocateSpellgroup(tempSPELLGROUP.SpellgroupID()) >= 0)
      {
        CString msg;
        msg.Format("Duplicate name in spellgroups.txt = %s\n",tempSPELLGROUP.m_name);
        WriteDebugString(msg);
        pkt->status=CONFIG_STAT_key;
      }
      else
        pSpellgroupData->AddSpellgroup(tempSPELLGROUP);
    }
    break;
  case CONFIG_FUNC_getFirstObject:
    //position=pSpellgroupData->GetHeadPosition();
    //if (position == NULL)
    if (pSpellgroupData->GetSpellgroupCount() == 0)
    {
      pkt->status = CONFIG_STAT_end;
      return;
    };
    //tempSPELLGROUP.Clear();
    //tempSPELLGROUP.m_name="template";
    //tempSPELLGROUP = pSpellgroupData->PeekAtPos(position);
    tempSPELLGROUP = *pSpellgroupData->PeekSpellgroup(0);
    //pSpellgroupData->GetNext(position);
    pkt->objectName=tempSPELLGROUP.m_name;
    pkt->listNum = 1;
    break;
  case CONFIG_FUNC_getNextObject:
    //if (position==NULL) 
    if (pkt->listNum >= pSpellgroupData->GetSpellgroupCount())
    {
      pkt->status=CONFIG_STAT_end;
    }
    else
    {
      //tempSPELLGROUP=pSpellgroupData->PeekAtPos(position);
      //pSpellgroupData->GetNext(position);
      tempSPELLGROUP = *pSpellgroupData->PeekSpellgroup(pkt->listNum);
      pkt->listNum++;
      pkt->objectName=tempSPELLGROUP.m_name;
    };
    break;
  default:
    pkt->status=CONFIG_STAT_intError;
  };
}


static void DspellgroupName(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value=tempSPELLGROUP.m_name;
    return;
  };
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    tempSPELLGROUP.m_name);
  pkt->objectName=tempSPELLGROUP.m_name;
}

/*
static void DspellgroupKey(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    CONFIG_ENCODE_long(pkt->value,tempSPELLGROUP.m_spellgroupKey);
    return;
  };
  //pkt->status=CONFIG_DECODE_long(
  //  pkt->value,
  //  pkt->errorMsg,
  //  -2000000000,2000000000,
  //  tempITEM.Protection_Bonus);
}
*/


static void DspellgroupSpell(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = "";
    WriteDebugString("DspellgroupSpell(IsStoring) not implemented\n");
    return;
  };
  CString temp;
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    temp);
  //if (pkt->status==CONFIG_STAT_ok) tempSPELLGROUP.AddSpell(temp);
  if (pkt->status==CONFIG_STAT_ok)
  {
    SPELL_ID spellID;
    spellID = temp;
    tempSPELLGROUP.AddSpell(spellID);
  };
}

static void DspellgroupSpellgroup(CONFIG_PKT *pkt)
{
  if (pkt->IsStoring)
  {
    pkt->value = "";
    WriteDebugString("DspellgroupSpellgroup(IsStoring) not implemented\n");
    return;
  };
  CString temp;
  pkt->status=CONFIG_DECODE_string(
    pkt->value,
    pkt->errorMsg,
    temp);
  //if (pkt->status==CONFIG_STAT_ok) tempSPELLGROUP.AddSpellgroup(temp);
  if (pkt->status==CONFIG_STAT_ok) 
  {
    SPELLGROUP_REFERENCE sgr;
    sgr.m_spellgroupID = temp;
    tempSPELLGROUP.AddSpellgroup(sgr);
  };
}

static void DspellgroupSpecialAbility(CONFIG_PKT *pkt)
{
  ReadWriteSpecialAbilities(pkt, &tempSPELLGROUP.m_specAbs);
}



#undef df
#undef dh
#define df(a) Dspellgroup##a
#define dh(a) IDS_HELP_spellgroup_##a
// Defined above //  #define DEFID(a,b,c) CONFIGID(a,CONFIG_DATA_##b,df(c),dh(c))

CONFIGID spellgroupIDs[] =
{
CONFIGID ("",CONFIG_DATA_object,spellgroupObject),
DEFID ("name"               ,string     ,Name),
//DEFID ("Key"                ,long       ,Key),
DEFID ("spell"              ,string     ,Spell),
DEFID ("spellgroup"         ,string     ,Spellgroup),
DEFID ("Special Ability"    ,string     ,SpecialAbility),
CONFIGID( "",CONFIG_DATA_end,NULL)
};



int ReadSpellgroupDB(LPCSTR file,
                     SPELLGROUP_DATA_TYPE *spellgroupPointer,
                     CONFIG_FILE *pConfigFile)
{
  // Returns  1 if minor errors.
  // Returns  0 if everything is perfect
  // Returns -1 if cannot read file.
  // Returns -2 if file appears to be wrong kind of file.
  // Returns -3 if internal program error occurs.
  CFile dataFile;
  CONFIG_FILE myConfigFile;
  CONFIG_ITEM_STATUS status;
  CString string;
  if (pConfigFile==NULL) pConfigFile = &myConfigFile;
  CArchive ar( &dataFile, CArchive::load);
  if (!dataFile.Open(file, CFile::modeRead)) return -1;
  ar.ReadString(string);
  if (string != "// Spell Group database file")
  {
    ar.Close();
    dataFile.Close();
    return -2;
  };
  pConfigFile->ReadFile(ar);
  ar.Close();
  dataFile.Close();
  // Now we begin the decode process
  // Get rid of old spellgroupData
  spellgroupPointer->Clear();
  // Read and store new spellgroupData
  ASSERT(pSpellgroupData==NULL); // Nobody should be using it.
  pSpellgroupData=spellgroupPointer;
  status=pConfigFile->DecodeFile(spellgroupIDs, true);
  pSpellgroupData=NULL; // We are done with it.
  // substitute default filenames for resources using "DEFAULT"
//  traitPointer->ExpandDefaultFilenames();
  if (status>=CONFIG_STAT_intError) return -3;
  if (status>CONFIG_STAT_ok) return 1;
  return 0;
}


int WriteSpellgroupDB(LPCSTR file, 
                      SPELLGROUP_DATA_TYPE *spellgroupPointer, 
                      CONFIG_FILE *pConfigFile)
{
  // Returns  0 if everything is perfect
  // Returns -1 if cannot write file.
  CFile dataFile;
  CString string;
  CONFIG_ITEM_STATUS status;
  SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL);
  if (!dataFile.Open(file, CFile::modeCreate|CFile::modeWrite)) return -1;
  ASSERT (pSpellgroupData==NULL); // Nobody should be using it
  
  // substitute "DEFAULT" for all resources using default filenames
//  traitPointer->CollapseDefaultFilenames();
  
  pSpellgroupData=spellgroupPointer;
  CArchive ar( &dataFile, CArchive::store);
  ar.WriteString("// Spell Group database file\r\n");
  ar.WriteString("// The line above **must** be the first line of the Spell Group file!!!\r\n");
  status=pConfigFile->WriteFile(ar, spellgroupIDs);

  // substitute default filenames for resources using "DEFAULT"
//  traitPointer->ExpandDefaultFilenames();

  if (status==CONFIG_STAT_intError)
  {
    MsgBoxError("Internal error encoding and writing file");
    return -1;
  };
  pSpellgroupData=NULL; // We are done.
  ar.Close();
  dataFile.Close();
  return 0;
}



