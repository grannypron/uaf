/******************************************************************************
* Filename: Party.cpp
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

#include "externs.h"

#ifdef UAFEDITOR
#include "..\UAFWinEd\UAFWinEd.h"
#endif
// #include "externs.h"
#include <math.h>
#include "Char.h"
#include "party.h"
#include "GlobalData.h"
#include "level.h"

#ifdef UAFEngine
#include "..\UAFWin\dgngame.h"
#include "..\UAFWin\disptext.h"
#include "..\UAFWin\Path.h"
#include "..\UAFWin\FormattedText.h"
#endif


BOOL MyDeleteFile(LPCSTR name);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_SERIAL( BLOCKAGE_STATUS, CObject, 1 )
//IMPLEMENT_SERIAL( VISIT_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( TRIGGER_FLAGS, CObject, 1 )
//IMPLEMENT_SERIAL( LEVEL_FLAG_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( EVENT_TRIGGER_DATA, CObject, 1 )
//IMPLEMENT_SERIAL( PARTY, CObject, 1 )

extern const CString Skill_OpenLocks;
extern CHARACTER FakeCharacter;

PARTY party;


#ifdef UAFEngine
TEMP_CHARACTERS tempChars;

TEMP_CHARACTERS::TEMP_CHARACTERS() 
{ 
  Clear(); 
}

TEMP_CHARACTERS::~TEMP_CHARACTERS() 
{ 
  Clear(); 
}
int TEMP_CHARACTERS::numChars() 
{ 
  //return temps.numChars(); 
  return temps.GetCount(); 
}

void TEMP_CHARACTERS::Clear()
{
  inParty.RemoveAll();
  temps.Clear();
  temps.SetAsTempCharList();
}

CString TEMP_CHARACTERS::GetCharName(int index)
{
  if ((index >= 0) && (index < inParty.GetSize()))
    //return temps.GetCharacterName(inParty[index].key);
    return temps.GetCharacterName(inParty[index].characterID);
  else
    return "Unknown";
}

void TEMP_CHARACTERS::RemoveThoseInParty()
{
  BOOL done = FALSE;

  while (!done)
  {
    done = TRUE;
    for (int i=0;i<inParty.GetSize();i++)
    {
      if (inParty[i].isInParty)
      {
        done = FALSE;
        //temps.RemoveCharacter(inParty[i].key);
        temps.RemoveCharacter(inParty[i].characterID);
        inParty.RemoveAt(i);
        break; // start from begining;
      }
    }
  }
}

void TEMP_CHARACTERS::RemoveNPCIfInParty()
{
  BOOL done = FALSE;
  //CHARACTER dude;
  CHARACTER *pChar;
  while (!done)
  {
    done = TRUE;
    for (int inPartyIndex=0;inPartyIndex<inParty.GetSize();inPartyIndex++)
    {      
      //dude = GetCharacter(i);
      pChar = GetCharacter(inPartyIndex);
      if (pChar->GetType() == NPC_TYPE)
      {
        for (int p=0;p<party.numCharacters;p++)
        {
          if (   (party.characters[p].GetType() == NPC_TYPE) 
              //&& (party.characters[p].uniqueKey == dude.uniqueKey))
              && (party.characters[p].characterID == pChar->characterID))
          {
            done = FALSE;
            deleteTemp(inPartyIndex);
          };
        };
      };
      if (!done)
        break;
    }
  }
}

int TEMP_CHARACTERS::CountInPartyMembers()
{
  int i, j;
  int count = 0;
  int partyCount, tempCount;
  partyCount = party.numCharacters;
  tempCount = inParty.GetSize();
  for (i=0; i<partyCount; i++)
  {
    for (j=0; j<tempCount; j++)
    {
      if (party.GetCharacter(i, NULL).characterID == inParty[j].characterID)
      {
        break;
      };
    };
    if (j == tempCount)
    {
      count++;
    };
  };
  for (i=0;i<tempCount;i++)
  {
    if (inParty[i].isInParty)
      count++;
  }
  return count;
}

int TEMP_CHARACTERS::CountInPartyPCs()
{
  int i, j;
  int count = 0;
  int partyCount, tempCount;
  partyCount = party.numCharacters;
  tempCount = inParty.GetSize();
  for (i=0; i<partyCount; i++)
  {
    for (j=0; j<tempCount; j++)
    {
      if (party.GetCharacter(i, NULL).characterID == inParty[j].characterID)
      {
        break;
      };
    };
    if (j == tempCount)
    {
      if (party.GetCharacter(i, NULL).GetType() == CHAR_TYPE)
      {
        count++;
      };
    };
  };
  for (i=0;i<tempCount;i++)
  {
    if (inParty[i].isInParty)
    {
      if (inParty[i].isPC)
      {
        count++;
      };
    };
  }
  return count;
}

void TEMP_CHARACTERS::SetInParty(int index, BOOL status, BOOL isPC)
{
  if ((index >= 0) && (index < inParty.GetSize()))
  {
    inParty[index].isInParty = status;
    inParty[index].isPC = isPC;
  };
}

BOOL TEMP_CHARACTERS::InParty(int index) 
{
  if ((index >= 0) && (index < inParty.GetSize()))
      return inParty[index].isInParty;
  else
    return FALSE;
}
/*
int TEMP_CHARACTERS::GetKey(int index)
{
  if ((index >= 0) && (index < inParty.GetSize()))
    return inParty[index].key;
  else
    return -1;
}
*/
//CHARACTER TEMP_CHARACTERS::GetCharacter(int index)
CHARACTER *TEMP_CHARACTERS::GetCharacter(int inPartyIndex)
{
  //CHARACTER dude;
  CHARACTER *pChar;
  //temps.GetCharacterData(inParty[index].key, dude);
  pChar = temps.GetCharacter(inParty[inPartyIndex].characterID);
  //temps.FetchCharacterData(inParty[index], dude);
  return pChar;
  //return dude;
}

void TEMP_CHARACTERS::deleteTemp(int inPartyIndex)
{
  if ((inPartyIndex >= 0) && (inPartyIndex < inParty.GetSize()))
  {
    //temps.RemoveCharacter(inParty[inPartyIndex].key);
    temps.RemoveCharacter(inParty[inPartyIndex].characterID);
    inParty.RemoveAt(inPartyIndex);      
  }
}

BOOL TEMP_CHARACTERS::IsCharacterInTemps(const CHARACTER &dude) const
{
  //POSITION pos=temps.GetHeadPosition();
  //while (pos!=NULL)
  int i, n;
  n = temps.GetCount();
  for (i=0; i<n; i++)
  {
    //if (temps.GetAt(pos).IsSameCharacter(dude))
    if (temps.PeekCharacter(i)->IsSameCharacter(dude))
      return TRUE;
    //temps.PeekNext(pos);
  }
  return FALSE;
}

//int TEMP_CHARACTERS::addPreGenCharacter(CHARACTER &dude, DWORD key)
int TEMP_CHARACTERS::addPreGenCharacter(const CHARACTER &dude)
{
  if (numChars() < MAX_TEMP_CHARS)
  {
     BOOL partymember=FALSE;
     for (int i=0;i<party.numCharacters;i++)
     {
       // try to determine if the two characters match,
       // this doesn't check all CHARACTER members for
       // equality since some portions of a CHARACTER
       // will change after it is saved (exp, level, etc)
       if (party.characters[i].IsSameCharacter(dude))
       {
         partymember=TRUE;
         break;
       }
     }

     if (!IsCharacterInTemps(dude))
     {
      //dude.origIndex = key; // key for character in globalData.charData
      //dude.type = NPC_TYPE;
      TEMP_IN_PARTY_DATA data;
      //data.key = temps.AddCharacter(dude);

      // The temps characters do not get the ability scores.
      {
        temps.AddCharacter(&dude);
      };

      data.characterID = dude.characterID;
      data.isInParty = partymember;    
      data.isPC = dude.GetType() == CHAR_TYPE;
      return (inParty.Add(data));
     }
  }
  return -1;
}

int TEMP_CHARACTERS::addTempCharacter(CHARACTER &dude)
{    
  if (numChars() >= MAX_TEMP_CHARS) return -1;
  if (IsCharacterInTemps(dude)) return -1;

  BOOL partymember=FALSE;
   for (int i=0;i<party.numCharacters;i++)
   {
     // try to determine if the two characters match,
     // this doesn't check all CHARACTER members for
     // equality since some portions of a CHARACTER
     // will change after it is saved (exp, level, etc)
     if (party.characters[i].IsSameCharacter(dude))
     {
       partymember=TRUE;
       break;
     }
   }

  //dude.key = 0;
  //dude.type = CHAR_TYPE;
  TEMP_IN_PARTY_DATA data;
  //data.key = temps.AddCharacter(dude);
  data.characterID = dude.CharacterID();
  temps.AddCharacter(&dude);
  data.isInParty = partymember;
  data.isPC = dude.GetType() == CHAR_TYPE;
  return (inParty.Add(data));
}

void TEMP_CHARACTERS::AddSavedCharacters()
{
  if (numChars() >= MAX_TEMP_CHARS) return;

  // search save game folder and common char folder for characters
  // and add them to available temps,

  // save game folder first
  CString searchpath = GetSavePath();
  if (searchpath[searchpath.GetLength()-1] != '\\')
    searchpath += "\\";
  searchpath += "*.chr";
 
  CFileFind finder;
  BOOL found = finder.FindFile(searchpath);
  CHARACTER tempCHARACTER;

  while (found)
  {
     found = finder.FindNextFile();
     if (finder.IsDots())
        continue;
     if (finder.IsDirectory())
       continue;

     tempCHARACTER.Clear();

     if (finder.GetFilePath().Find("chr_bad") == -1)
     {
       if (tempCHARACTER.serializeCharacter(FALSE, finder.GetFileTitle()))
       {
         if ((tempCHARACTER.GetType() == NPC_TYPE) && (tempCHARACTER.IsPreGen))
           continue;

         // previously saved character might be dead, and when
         // starting a new game the original NPC must be restored.
         if (tempCHARACTER.GetType()==NPC_TYPE)
           tempCHARACTER.RestoreToBestCondition();

         // only CHAR_TYPE and non-pregen NPC's are stored
         if (!IsCharacterInTemps(tempCHARACTER))
          addTempCharacter(tempCHARACTER);
       }
       else
       {
         WriteDebugString("Failed to load saved char %s into temps\n",
                          finder.GetFilePath());
         CString tmp(finder.GetFilePath());
         tmp.MakeLower();
         if (tmp.Find(".chr_bad")==-1)
         {
           tmp+="_bad";
           SetFileAttributes(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
           rename(finder.GetFilePath(), tmp);
         }
       }
     }
  }

  finder.Close();

  // now the common char folder
  searchpath = GetCommonCharPath();
  if (searchpath.IsEmpty())
  {
    // no common char folder defined
    return;
  }

  if (searchpath[searchpath.GetLength()-1] != '\\')
    searchpath += "\\";
  searchpath += "*.chr";
 
  found = finder.FindFile(searchpath);

  while (found)
  {
     found = finder.FindNextFile();
     if (finder.IsDots())
        continue;
     if (finder.IsDirectory())
       continue;

     if (finder.GetFilePath().Find("chr_bad") == -1)
     {
       tempCHARACTER.Clear();
       if (tempCHARACTER.serializeCharacter(FALSE, finder.GetFileTitle()))
       {
         if ((tempCHARACTER.GetType() == NPC_TYPE) && (tempCHARACTER.IsPreGen))
           continue;

         // only CHAR_TYPE and non-pregen NPC's are stored
         if (!IsCharacterInTemps(tempCHARACTER))
          addTempCharacter(tempCHARACTER);
       }
       else
       {
         WriteDebugString("Failed to load common saved char %s into temps\n",
                          finder.GetFilePath());
         CString tmp(finder.GetFilePath());
         tmp.MakeLower();
         if (tmp.Find(".chr_bad")==-1)
         {
           tmp+="_bad";
           SetFileAttributes(finder.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
           rename(finder.GetFilePath(), tmp);
         }
       }
     }
  }
}

void TEMP_CHARACTERS::RemoveSavedCharacters()
{
  // remove temps if found in save game folder
  // so they don't get duplicated next time
  // the saved characters get added.

  if (numChars() <= 0) return;
  CString searchpath = GetSavePath();
  if (searchpath[searchpath.GetLength()-1] != '\\')
    searchpath += "\\";
  searchpath += "*.chr";
 
  CFileFind finder;
  BOOL found = finder.FindFile(searchpath);
  CHARACTER tempCHARACTER;

  while (found)
  {
     found = finder.FindNextFile();
     if (finder.IsDots())
        continue;
     if (finder.IsDirectory())
       continue;

     tempCHARACTER.Clear();
     if (tempCHARACTER.serializeCharacter(FALSE, finder.GetFileTitle()))
     {
        for (int i=0;i<inParty.GetSize();i++)
        {      
          //if (GetCharacter(i).IsSameCharacter(tempCHARACTER))
          if (GetCharacter(i)->IsSameCharacter(tempCHARACTER))
          {
            deleteTemp(i);
            break;
          }
        }
     }
     else
     {
       WriteDebugString("Failed to load saved char %s while clearing temps\n",
                        finder.GetFilePath());
     }
  }
}
#endif // UAFEngine

//*****************************************************************************
// NAME:    LEVEL_FLAG_DATA::Serialize
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void LEVEL_FLAG_DATA::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);

  if (ar.IsStoring())
  {
    ar.Write(&stepData, sizeof(stepData));

    ar << flagData.GetCount();  

    POSITION pos = flagData.GetHeadPosition();
    while (pos != NULL)
    {
      ar << flagData.GetKeyAt(pos);
      flagData.GetNext(pos).Serialize(ar);
    }
  }
  else
  {
    int count;  
    TRIGGER_FLAGS tmpFlags;
    DWORD key;

    ar.Read(&stepData, sizeof(stepData));

    ar >> count;
  
    for (int i=0; i<count; i++)
    {
      ar >> key;
      tmpFlags.Serialize(ar);
      flagData.Insert(tmpFlags, key);
    }  
  }
}

void LEVEL_FLAG_DATA::Serialize(CAR &car)
{
//  CObject::Serialize(ar);

  if (car.IsStoring())
  {
    //car.Write(&stepData, sizeof(stepData));
    car.Serialize((char *)&stepData, sizeof(stepData));

    car << flagData.GetCount();  

    POSITION pos = flagData.GetHeadPosition();
    while (pos != NULL)
    {
      car << flagData.GetKeyAt(pos);
      flagData.GetNext(pos).Serialize(car);
    }
  }
  else
  {
    int count;  
    TRIGGER_FLAGS tmpFlags;
    DWORD key;

    //car.Read(&stepData, sizeof(stepData));
    car.Serialize((char *)&stepData, sizeof(stepData));

    car >> count;
  
    for (int i=0; i<count; i++)
    {
      car >> key;
      tmpFlags.Serialize(car);
      flagData.Insert(tmpFlags, key);
    }  
  }
}


//*****************************************************************************
// NAME: EVENT_TRIGGER_DATA::Serialize
//
// PURPOSE:  
//*****************************************************************************
void EVENT_TRIGGER_DATA::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);
  
  int count,i;  

  if (ar.IsStoring())
  {
    ar << data.GetSize();
    for (i=0;i<data.GetSize(); i++)
      data[i].Serialize(ar);
  }
  else
  {
    ar >> count;
    data.SetSize(count);
    for (i=0;i<count;i++)
      data[i].Serialize(ar);
  }
}

void EVENT_TRIGGER_DATA::Serialize(CAR &car)
{
//  CObject::Serialize(ar);
  
  int count,i;  

  if (car.IsStoring())
  {
    car << data.GetSize();
    for (i=0;i<data.GetSize(); i++)
      data[i].Serialize(car);
  }
  else
  {
    car >> count;
    data.SetSize(count);
    for (i=0;i<count;i++)
      data[i].Serialize(car);
  }
}

//*****************************************************************************
// NAME:    =
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
PARTY& PARTY::operator =(const PARTY& src)
{
  if (&src == this) return *this;
   memcpy(&stateStack, &src.stateStack, sizeof(stateStack));
   days = src.days;
   hours = src.hours;
   minutes = src.minutes;
   drinkPoints = src.drinkPoints;
   if (!src.name.IsEmpty())
      name = src.name;
   else
      name="";
   adventuring = src.adventuring;
   areaView = src.areaView;
   searching = src.searching;
   looking = src.looking;
   resting = src.resting;
   //detectingTraps = src.detectingTraps;
   //detectingInvisible = src.detectingInvisible;
   //detectingMagic = src.detectingMagic;
   level = src.level;
   speed = src.speed;
   Posx = src.Posx;
   Posy = src.Posy;
   PrevPosx = src.PrevPosx;
   PrevPosy = src.PrevPosy;
   facing = src.facing;
   PrevRelFacing=src.PrevRelFacing;
   activeCharacter = src.activeCharacter;
   activeItem = src.activeItem;
   tradeItem = src.tradeItem;
   tradeGiver = src.tradeGiver;
   tradeQty = src.tradeQty;
   skillLevel = src.skillLevel;
   numCharacters = src.numCharacters;
   moneyPooled = src.moneyPooled;
   poolSack = src.poolSack;   
   ActiveScripter=src.ActiveScripter;
   eventFlags = src.eventFlags;
   visitData  = src.visitData;
   blockageData = src.blockageData;
   journal = src.journal;

   int i;
   for (i=0;i<MAX_PARTY_MEMBERS;i++)
      characters[i] = src.characters[i];
   party_asl.Copy(src.party_asl);
   return *this;
}

//*****************************************************************************
// NAME:    PARTY::PARTY
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
PARTY::PARTY(const PARTY& src)
   // ASL named "PARTY_ATTRIBUTES"
	:party_asl()
{
  *this = src;
}

//*****************************************************************************
// NAME: clearPartyFlags
//
// PURPOSE:  
//*****************************************************************************
void PARTY::clearPartyFlags()
{
   eventFlags.Clear();
   visitData.Clear();
   blockageData.Clear();
}

//*****************************************************************************
// NAME: Clear
//
// PURPOSE:  
//*****************************************************************************
void PARTY::Clear(BOOL IsConstructor)
{
   memset(&stateStack, 0, sizeof(stateStack));
   days = 0;
   hours = 0;
   minutes = 0;
   drinkPoints = 0;
   name="";
   adventuring = FALSE;
   areaView = FALSE;
   searching = FALSE;
   looking = FALSE;
   resting = FALSE;
   minutesRested = 0;
#ifdef OLD_AUTO_HEAL
   minutesUntilAutoHeal = 1440;
#endif
   //detectingTraps = FALSE;
   //detectingInvisible = FALSE;
   //detectingMagic = FALSE;
   level = 0;
   speed = 3;
   Posx = 0;
   Posy = 0;
   PrevPosx = -1;
   PrevPosy = -1;
   facing = 0;
   PrevRelFacing=-1;
   activeCharacter =  0;
   activeItem = 0;
   tradeItem = 0;
   tradeGiver = 0;
   tradeQty = 0;
   skillLevel = 2;
   numCharacters = 0;
   DayMusic=-1;
   moneyPooled = FALSE;
   poolSack.Clear();

   clearPartyFlags();

   int i;
   for (i=0;i<MAX_PARTY_MEMBERS;i++)
      characters[i].Clear(IsConstructor);
   party_asl.Clear();
}

//*****************************************************************************
// NAME:    PARTY::Serialize
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
#ifdef UAFEngine
void PARTY::Serialize(CArchive &ar)
{
  //CObject::Serialize(ar);
  int temp,i;

  if (ar.IsStoring())
  {
    ar << stateStack.count;
    for (i=0;i<(int)stateStack.count;i++)
    {
      ar << stateStack.tdata[i].id;
      ar << stateStack.tdata[i].flags;
      ASSERT( stateStack.tdata[i].datacount <= MAX_TASK_STATE_SAVE_BYTES );
      ar << stateStack.tdata[i].datacount;
      for (int d=0;d<stateStack.tdata[i].datacount;d++)
        ar << stateStack.tdata[i].data[d];
    }

    ar << days;
    ar << hours;
    ar << minutes;
    ar << drinkPoints;

    AS(ar,name);
    ar << adventuring;
    ar << areaView;
    ar << searching;
    ar << level;
    ar << speed;
    ar << Posx;
    ar << Posy;
    ar << PrevPosx;
    ar << PrevPosy;
    ar << facing;
    ar << activeCharacter;
    ar << activeItem;
    ar << tradeItem;
    ar << tradeGiver;
    ar << tradeQty;
    ar << skillLevel;
    ar << numCharacters;
    ar << moneyPooled;
  }
  else
  {
    ar >> temp;
    stateStack.count = temp;
    for (i=0;i<temp;i++)
    {
      ar >> stateStack.tdata[i].id;
      ar >> stateStack.tdata[i].flags;
      ar >> stateStack.tdata[i].datacount;
      ASSERT( stateStack.tdata[i].datacount <= MAX_TASK_STATE_SAVE_BYTES );
      for (int d=0;d<stateStack.tdata[i].datacount;d++)
        ar >> stateStack.tdata[i].data[d];
    }

    ar >> days;
    ar >> hours;
    ar >> minutes;
    ar >> drinkPoints;
    DAS(ar,name);
    ar >> adventuring;
    ar >> areaView;
    ar >> searching;
    if (globalData.SaveGameVersion < _VERSION_0850_)
    {
      BOOL unused;
      ar >> unused; //detectingTraps; // these three unused
      ar >> unused; //detectingInvisible;    
      ar >> unused; //detectingMagic;
    }
    ar >> level;
    ar >> speed;
    ar >> Posx;
    ar >> Posy;
    if (globalData.SaveGameVersion >= _VERSION_0575_)
    {
      ar >> PrevPosx;
      ar >> PrevPosy;
    }
    ar >> facing;
    ar >> activeCharacter;
    ar >> activeItem;
    ar >> tradeItem;
    ar >> tradeGiver;
    ar >> tradeQty;
    ar >> skillLevel;
    ar >> numCharacters;
    ar >> moneyPooled;

    poolSack.Clear();
    if (globalData.SaveGameVersion < _VERSION_0661_)
    {
      ar >> temp; poolSack.Coins[PlatinumType] = temp;
      ar >> temp; poolSack.Coins[ElectrumType] = temp;
      ar >> temp; poolSack.Coins[GoldType] = temp;
      ar >> temp; poolSack.Coins[SilverType] = temp;
      ar >> temp; poolSack.Coins[CopperType] = temp;
      ar >> temp; { for (int g=0;g<temp;g++) poolSack.AddGem(); }
      ar >> temp; { for (int j=0;j<temp;j++) poolSack.AddJewelry(); }
      if (globalData.SaveGameVersion >= _VERSION_0660_)
      {
        ar >> temp; poolSack.Coins[CoinType6] = temp;
        ar >> temp; poolSack.Coins[CoinType7] = temp;
        ar >> temp; poolSack.Coins[CoinType8] = temp;
        ar >> temp; poolSack.Coins[CoinType9] = temp;
        ar >> temp; poolSack.Coins[CoinType10] = temp;
      }
    }
  }

  eventFlags.Serialize(ar);
  visitData.Serialize(ar);
  blockageData.Serialize(ar);  

  // not part of party! //  globalData.questData.Serialize(ar);

  if (ar.IsStoring())
  {
    ar << MAX_PARTY_MEMBERS;
    for (i=0;i<MAX_PARTY_MEMBERS;i++)
      characters[i].Serialize(ar, globalData.SaveGameVersion);
  }
  else
  {
    PrevRelFacing=-1;
    ActiveScripter=-1;

    ar >> temp;
    
    if (temp > MAX_PARTY_MEMBERS)
      WriteDebugString("Bogus value for MAX_PARTY_MEMBERS read from save file: %u\n", temp);

    temp = min( temp, MAX_PARTY_MEMBERS );
    
    for (i=0;i<temp;i++)
    {
      characters[i].Serialize(ar, globalData.SaveGameVersion);

      if (characters[i].GetType()!=NPC_TYPE)
        characters[i].origIndex = -1;
    }
  }  

  if (ar.IsStoring())
  {
    poolSack.Serialize(ar, globalData.SaveGameVersion);
    journal.Serialize(ar);
  }
  else
  {
    if (globalData.SaveGameVersion >= _VERSION_0661_)
      poolSack.Serialize(ar, globalData.SaveGameVersion);
    if (globalData.SaveGameVersion >= _VERSION_0780_)
      journal.Serialize(ar);
  }
    
  party_asl.Serialize(ar, "PARTY_ATTRIBUTES");

  // reset internal time keeping flags
  ProcessTimeSensitiveData(0);
}
#endif



//*****************************************************************************
// NAME:    PARTY::Serialize
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
#ifdef UAFEngine
void PARTY::Serialize(CAR& car)
{
  //CObject::Serialize(ar);
  int temp,i;

  if (car.IsStoring())
  {
    car << stateStack.count;
    for (i=0;i<(int)stateStack.count;i++)
    {
      car << stateStack.tdata[i].id;
      car << stateStack.tdata[i].flags;
      ASSERT( stateStack.tdata[i].datacount <= MAX_TASK_STATE_SAVE_BYTES );
      car << stateStack.tdata[i].datacount;
      for (int d=0;d<stateStack.tdata[i].datacount;d++)
        car << stateStack.tdata[i].data[d];
    }

    car << days;
    car << hours;
    car << minutes;
    car << drinkPoints;

    AS(car,name);
    car << adventuring;
    car << areaView;
    car << searching;
    car << level;
    car << speed;
    car << Posx;
    car << Posy;
    car << PrevPosx;
    car << PrevPosy;
    car << facing;
    car << activeCharacter;
    car << activeItem;
    car << tradeItem;
    car << tradeGiver;
    car << tradeQty;
    car << skillLevel;
    car << numCharacters;
    car << moneyPooled;
  }
  else
  {
    DWORD filePos;
    car >> temp;
    stateStack.count = temp;
    for (i=0;i<temp;i++)
    {
      filePos = car.GetPosition();
      car >> stateStack.tdata[i].id;
      car >> stateStack.tdata[i].flags;
      car >> stateStack.tdata[i].datacount;
      ASSERT( stateStack.tdata[i].datacount <= MAX_TASK_STATE_SAVE_BYTES );
      for (int d=0;d<stateStack.tdata[i].datacount;d++)
      {
        car >> stateStack.tdata[i].data[d];
        filePos = car.GetPosition();
      };
    }
    filePos = car.GetPosition();

    car >> days;
    car >> hours;
    car >> minutes;
    car >> drinkPoints;
    DAS(car,name);
    car >> adventuring;
    car >> areaView;
    car >> searching;
    if (globalData.SaveGameVersion < _VERSION_0850_)
    {
      BOOL unused;
      car >> unused; //detectingTraps; // these three unused
      car >> unused; //detectingInvisible;    
      car >> unused; //detectingMagic;
    }
    car >> level;
    car >> speed;
    car >> Posx;
    car >> Posy;
    if (globalData.SaveGameVersion >= _VERSION_0575_)
    {
      car >> PrevPosx;
      car >> PrevPosy;
    }
    car >> facing;
    car >> activeCharacter;
    car >> activeItem;
    car >> tradeItem;
    car >> tradeGiver;
    car >> tradeQty;
    car >> skillLevel;
    car >> numCharacters;
    car >> moneyPooled;

    poolSack.Clear();
    if (globalData.SaveGameVersion < _VERSION_0661_)
    {
      car >> temp; poolSack.Coins[PlatinumType] = temp;
      car >> temp; poolSack.Coins[ElectrumType] = temp;
      car >> temp; poolSack.Coins[GoldType] = temp;
      car >> temp; poolSack.Coins[SilverType] = temp;
      car >> temp; poolSack.Coins[CopperType] = temp;
      car >> temp; { for (int g=0;g<temp;g++) poolSack.AddGem(); }
      car >> temp; { for (int j=0;j<temp;j++) poolSack.AddJewelry(); }
      if (globalData.SaveGameVersion >= _VERSION_0660_)
      {
        car >> temp; poolSack.Coins[CoinType6] = temp;
        car >> temp; poolSack.Coins[CoinType7] = temp;
        car >> temp; poolSack.Coins[CoinType8] = temp;
        car >> temp; poolSack.Coins[CoinType9] = temp;
        car >> temp; poolSack.Coins[CoinType10] = temp;
      }
    }
  }

  eventFlags.Serialize(car);
  visitData.Serialize(car);
  blockageData.Serialize(car);  

  // not part of party! //  globalData.questData.Serialize(ar);

  if (car.IsStoring())
  {
    car << MAX_PARTY_MEMBERS;
    for (i=0;i<MAX_PARTY_MEMBERS;i++)
    {
      characters[i].Serialize(car, globalData.SaveGameVersion);
    };
  }
  else
  {
    PrevRelFacing=-1;
    ActiveScripter=-1;

    car >> temp;
    
    if (temp > MAX_PARTY_MEMBERS)
      WriteDebugString("Bogus value for MAX_PARTY_MEMBERS read from save file: %u\n", temp);

    temp = min( temp, MAX_PARTY_MEMBERS );
    
    for (i=0;i<temp;i++)
    {
      characters[i].Serialize(car, globalData.SaveGameVersion);
      characters[i].UpdateSpellAbility();

      if (characters[i].GetType()!=NPC_TYPE)
        characters[i].origIndex = -1;
    }
  }  

  if (car.IsStoring())
  {
    poolSack.Serialize(car, globalData.SaveGameVersion);
    journal.Serialize(car);
  }
  else
  {
    if (globalData.SaveGameVersion >= _VERSION_0661_)
      poolSack.Serialize(car, globalData.SaveGameVersion);
    if (globalData.SaveGameVersion >= _VERSION_0780_)
      journal.Serialize(car);
  }
    
  party_asl.Serialize(car, "PARTY_ATTRIBUTES");

  // reset internal time keeping flags
  ProcessTimeSensitiveData(0);
}
#endif





//*****************************************************************************
// NAME:    PARTY::PartyHasItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//BOOL PARTY::PartyHasItem(GLOBAL_ITEM_ID giID)
BOOL PARTY::PartyHasItem(const ITEM_ID& itemID)
{
  BOOL found = FALSE;
  int i;

  for (i=0;i<numCharacters && !found;i++)
  {
    //if (characters[i].HasItem(giID))
    if (characters[i].HasItem(itemID))
       found = TRUE;
  }

  return found;
}

//*****************************************************************************
// NAME:    PARTY::PartyInDaytime
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PARTY::PartyInDaytime()
{ 
  return ((hours >= 6) && (hours <= 18));
}

//*****************************************************************************
// NAME:    PARTY::PartyIsSearching
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PARTY::PartyIsSearching()
{
  return (searching);
}

//*****************************************************************************
// NAME:    PARTY::GetPartyFacing
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int PARTY::GetPartyFacing()
{
  return (facing);
}

//*****************************************************************************
// NAME:    PARTY::PartyIsDetectingTraps
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PARTY::PartyIsDetectingTraps()
{
  for (int i=0;i<numCharacters;i++)
  {
    if (characters[i].GetAdjDetectingTraps()) return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
// NAME:    PARTY::PartyIsDetectingInvisible
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PARTY::PartyIsDetectingInvisible()
{
  for (int i=0;i<numCharacters;i++)
  {
    if (characters[i].GetAdjDetectingInvisible()) return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
// NAME:    PARTY::PartyIsDetectingMagic
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PARTY::PartyIsDetectingMagic()
{
/*  Replaced by a test for active character only at Manikus' request 20130714
  for (int i=0;i<numCharacters;i++)
  {
    if (characters[i].GetAdjSpecAb(SA_DetectMagic)) return TRUE;
  }
  return FALSE;
*/
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  CString result;
  const CHARACTER *pChar;
  pChar = &getActiveChar();
  result = pChar->RunCharacterScripts(CAN_CHARACTER_DETECT_MAGIC,
                                      ScriptCallback_RunAllScripts,
                                      NULL,
                                      "Test if party is detecting magic");
  return !result.IsEmpty() && (result[0] == 'Y');
}

//*****************************************************************************
// NAME:    PARTY::PartyHasGender
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL PARTY::PartyHasGender(genderType type)
{
  for (int i=0;i<numCharacters;i++)
  {
    if (characters[i].GetAdjGender()==type) return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: PARTY::PartyHasNPC
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL PARTY::PartyHasNPC(int npckey)
BOOL PARTY::PartyHasNPC(const CHARACTER_ID& characterID) const
{
  //return (isNPCinParty(npckey));
  return (isNPCinParty(characterID));
}

//*****************************************************************************
// NAME:    PARTY::PartyHasClass
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//BOOL PARTY::PartyHasClass(classType ctype)
BOOL PARTY::PartyHasClass(const CLASS_ID& classID) const
{
  int i;
  for (i=0; i<numCharacters; i++)
  {
     //if (characters[i].GetAdjClass() == ctype)
     if (characters[i].GetAdjClass() == classID) return TRUE;
  }
  return FALSE;
}

BOOL PARTY::PartyHasBaseclass(const BASECLASS_ID& baseclassID) const
{
  int i;
  for (i=0; i<numCharacters; i++)
  {
    const CLASS_DATA *pClass;
    pClass = classData.PeekClass(characters[i].GetAdjClass());
    if (pClass != NULL)
    {
      int j, n;
      n = pClass->GetCount();
      for (j=0; j<n; j++)
      {
        if (*pClass->PeekBaseclassID(j) == baseclassID) return TRUE;
      };
    };
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: PARTY::PartyHasSpellMemorized
//
// PURPOSE: Determine if at least one party member has a 
//          particular spell memorized.
//
//*****************************************************************************
//BOOL PARTY::PartyHasSpellMemorized(GLOBAL_SPELL_ID spellDbKey)
BOOL PARTY::PartyHasSpellMemorized(const SPELL_ID& spellID)
{
  for (int i=0;i<numCharacters;i++)
  {
    //if (characters[i].HaveSpell(spellDbKey,TRUE))
    if (characters[i].HaveSpell(spellID,TRUE))
      return TRUE;
  }

  return FALSE;
}

//*****************************************************************************
// NAME:    PARTY::PartyHasRace
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//BOOL PARTY::PartyHasRace(raceType rtype)
BOOL PARTY::PartyHasRace(const RACE_ID& raceID) const
{
  BOOL found = FALSE;
  int i;

  for (i=0;!found && i<numCharacters;i++)
  {
     //if (characters[i].race() == rtype)
     if (characters[i].RaceID() == raceID)
        found = TRUE;
  }

  return found;
}

//*****************************************************************************
// NAME:    PARTY::GetHealingPointsNeeded
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int PARTY::GetHealingPointsNeeded()
{
  int total = 0;
  int i;

  for (i=0;i<numCharacters;i++)
     total += characters[i].GetHealingPointsNeeded();

  return total;
}

// time in 24 hr format, does not affect current day
//*****************************************************************************
// NAME:    PARTY::setPartyTime
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::setPartyTime(int ptime)
{  
   int min = ptime % 100;
   int hr = ptime / 100;
   
   hours = hr;
   minutes = min;
}

//*****************************************************************************
// NAME:    PARTY::get24HourClock
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int PARTY::get24HourClock()
{
#ifdef UAFEngine
   return ((hours*100)+minutes);
#else
   return globalData.startTime;
#endif
}

DWORD PARTY::getElapsedMinutes()
{
  return  (days*24 + hours) * 60 + minutes ;
}

//*****************************************************************************
// NAME: incrementClock
//
// PURPOSE:  
//*****************************************************************************
void PARTY::incrementClock(int MinuteInc)
{  
   int mn, hr, day;
   
   if (MinuteInc <= 0)
      return;
   
   if (MinuteInc >= 60)
   {
      day = MinuteInc / (1440);
      MinuteInc %= (1440);
      hr  = MinuteInc / 60;
      MinuteInc %= 60;
      mn  = MinuteInc;
   }
   else 
   {
      day = 0;
      hr = 0;
      mn = MinuteInc;
   }
   
   minutes += mn;
   
   if (minutes >= 60)
   { 
      minutes -= 60;
      hr++;
   }
   
   hours += hr;
   
   if (hours >= 24)
   {
      hours -= 24;
      day++;
   }

   //days += day;

   for (int d=0; d < day; d++)
   {
     days += 1;

     int DayOfYear = days % 365;

     // then start checking character birthdates
     for (int i=0;i<numCharacters;i++)
     {
       int bday = characters[i].GetBirthday();
       if (bday <= 0) bday = 1;

       if (DayOfYear == bday)
       {
         characters[i].SetAge( characters[i].GetAge()+1 );
         WriteDebugString("%s has aged one year\n", characters[i].GetName());
       }
     }
   }
   {
     HOOK_PARAMETERS hookParameters;
     SCRIPT_CONTEXT scriptContext;
     char num[20];
     itoa((days*24+hours)*60+minutes, num, 10);
     hookParameters[5] = num;
     itoa(days, num, 10);
     hookParameters[6] = num;
     itoa(hours, num, 10);
     hookParameters[7] = num;
     itoa(minutes, num, 10);
     hookParameters[8] = num;
     itoa(MinuteInc, num, 10);
     hookParameters[9] = num;
     RunGlobalScript(GLOBAL_TIME, TIME_ELAPSED, true);
   };
}


//*****************************************************************************
// NAME:    PARTY::PartyBlockage
//
// PURPOSE: 
//
// RETURNS: enum PartyBlockageType
//*****************************************************************************
PartyBlockageType PARTY::PartyBlockage(BlockageType type, int currfacing)
{
  int key;
  switch (type)
  {
  case OpenBlk:
    return PartyBlock_none; // can always pass

  case OpenSecretBlk:
    return (blockageData.IsSecret(GetCurrentLevel(), Posx, Posy, currfacing))?
        PartyBlock_openSecret:PartyBlock_none;  // can pass if secret door already discovered

  case LockedBlk:             // can pass if lock was picked successfully
    return (blockageData.IsLocked(GetCurrentLevel(), Posx, Posy, currfacing))?
        PartyBlock_locked:PartyBlock_none;  // can pass if secret door already discovered

  case LockedSecretBlk:       // if found secret door, then picked lock
    return (blockageData.IsLockedSecret(GetCurrentLevel(), Posx, Posy, currfacing))?
        PartyBlock_lockedSecret:PartyBlock_none;  // can pass if secret door already discovered

  case LockedWizardBlk:       // if cast unlock spell
    return (blockageData.IsSpelled(GetCurrentLevel(), Posx, Posy, currfacing))?
        PartyBlock_lockedWizard:PartyBlock_none;  // can pass if secret door already discovered

  case LockedWizardSecretBlk: // if found secret door, then cast unlock spell
    return (blockageData.IsSpelledSecret(GetCurrentLevel(), Posx, Posy, currfacing))?
        PartyBlock_lockedWizardSecret:PartyBlock_none;  // can pass if secret door already discovered

  case BlockedBlk:
    return PartyBlock_blocked;  // can never pass
  case FalseDoorBlk:
    return PartyBlock_falseDoor;  // can never pass

  // Can pass if party has special key that allows passage.
  case LockedKey1:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(1);
    else
      key = levelData.blockageKeys.SpecialKeys[0];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey0;

  case LockedKey2:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(2);
    else
      key = levelData.blockageKeys.SpecialKeys[1];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey1;

  case LockedKey3:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(3);
    else
      key = levelData.blockageKeys.SpecialKeys[2];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey2;

  case LockedKey4:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(4);
    else
      key = levelData.blockageKeys.SpecialKeys[3];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey3;

  case LockedKey5:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(5);
    else
      key = levelData.blockageKeys.SpecialKeys[4];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey4;

  case LockedKey6:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(6);
    else
      key = levelData.blockageKeys.SpecialKeys[5];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey5;

  case LockedKey7:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(7);
    else
      key = levelData.blockageKeys.SpecialKeys[6];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey6;

  case LockedKey8:
    if (globalData.version < _VERSION_0842_)
      key = globalData.keyData.GetItemKeyUsingSequence(8);
    else
      key = levelData.blockageKeys.SpecialKeys[7];
    return (hasSpecialKey(key))?PartyBlock_none:PartyBlock_specialKey7;
  }
  return PartyBlock_unknown;
}

int PARTY::RelativeToActualFacing(int relativeDirection)
{
  // relative dir works according to:
  //
  // 4  0  5
  // 3     1
  // 6  2  7
  //
  // which follows facing directions in externs.h 

  if ((relativeDirection < 0)||(relativeDirection > 7)) return -1;
  if (!globalData.levelInfo.stats[globalData.currLevel].overland)
  {
    return ((facing+relativeDirection)&3); // no diagonal in dungeons\}
  }
  else
  {
    return (relativeDirection&7);
  };
}


PartyBlockageType PARTY::newForwardPosition(int relativeDirection, int &newX, int &newY)
{
   int oldX = Posx;
   int oldY = Posy;
   BlockageType type = BlockedBlk;
   PartyBlockageType partyBlock = PartyBlock_unknown;

//   char temp[30];
//   char dirChar = 'N';
   int ActualFacing = RelativeToActualFacing(relativeDirection);

   if (!globalData.levelInfo.stats[globalData.currLevel].overland)
   {
     switch (ActualFacing) 
     {      
     case FACE_NORTH:
        type = levelData.area[Posy][Posx].blockage[0];
//	      dirChar='N';
        break;      
     case FACE_EAST:
        type = levelData.area[Posy][Posx].blockage[2];
//	      dirChar='E';
        break;      
     case FACE_SOUTH:
         type = levelData.area[Posy][Posx].blockage[1];
//	       dirChar='S';
        break;      
     case FACE_WEST:
        type = levelData.area[Posy][Posx].blockage[3];
//	      dirChar='W';
        break;
     }     
	/*   
     sprintf(temp,"$Block,%d,%d,%c",Posx,Posy,dirChar);
     {
        const ASLENTRY *pEntry;
        pEntry = globalData.levelInfo.stats[globalData.currLevel].level_asl.Find(temp);
        if (pEntry != NULL)
	      {
		      if (*pEntry->pValue() == "OpenBlk") type=OpenBlk;
	      };
     };
  */
    {
      int tempType;
      tempType = globalData.levelInfo.stats[globalData.currLevel].GetMapOverride
                                                          (BLOCKAGE_OVERRIDE, Posx, Posy, ActualFacing);

      if (tempType >= 0)
      {
        tempType &= 0x0f;
        type = (BlockageType)tempType;
      };
    };
    partyBlock = PartyBlockage(type, ActualFacing);

   }
   else
   {
     int tempX = Posx;
     int tempY = Posy;
     // check blockage in dest square for overland
     switch (ActualFacing)
     {
     case FACE_NORTH: tempY--; break;
     case FACE_EAST:  tempX++; break;
     case FACE_SOUTH: tempY++; break;
     case FACE_WEST:  tempX--; break;
     case FACE_NW:    tempY--; tempX--; break;
     case FACE_NE:    tempY--; tempX++; break;
     case FACE_SW:    tempY++; tempX--; break;
     case FACE_SE:    tempY++; tempX++; break;
     }     
     if (tempX < 0) tempX=0;
     if (tempY < 0) tempY=0;
     if (tempX >= levelData.area_width)  tempX=levelData.area_width-1;         
     if (tempY >= levelData.area_height) tempY=levelData.area_height-1;
     // if able to move into dest square
     if ((oldX != tempX) || (oldY != tempY))
       type = (levelData.area[tempY][tempX].bkgrnd > 0) ? BlockedBlk : OpenBlk;
     else
       type = BlockedBlk;
     partyBlock = (type == OpenBlk)?PartyBlock_none:PartyBlock_blocked;
   }   

   //if (CanMove)
   {
     if (globalData.levelInfo.stats[globalData.currLevel].overland)
     {
       newX = Posx;
       newY = Posy;
       // don't wrap party to opposite side of map when using overland
       switch (ActualFacing)
       {      
       case FACE_NORTH: newY--; break;
       case FACE_EAST:  newX++; break;
       case FACE_SOUTH: newY++; break;
       case FACE_WEST:  newX--; break;
       case FACE_NW:    newY--; newX--; break;
       case FACE_NE:    newY--; newX++; break;
       case FACE_SW:    newY++; newX--; break;
       case FACE_SE:    newY++; newX++; break;
       }     
       if (newX < 0) newX=0;
       if (newY < 0) newY=0;
       if (newX >= levelData.area_width)  newX=levelData.area_width-1;         
       if (newY >= levelData.area_height) newY=levelData.area_height-1;
       if ((oldX == newX) && (oldY == newY)) partyBlock=PartyBlock_blocked;
     }
     else
     {
       ActualFacing &= 3;
       newY=(Posy+deltaY[ActualFacing]+levelData.area_height)%levelData.area_height;
       newX=(Posx+deltaX[ActualFacing]+levelData.area_width)%levelData.area_width;
     }
   }

   return partyBlock;
}

int PARTY::DetermineSpriteDistance(int distance)
{
  int result;
  int origX;
  int origY;
  int newX, newY;

  result = distance;
  origX = Posx;
  origY = Posy;
  
  if (levelData.area_width <= 0) return distance;

  // check one square away to see if it is obstructed
  if (newForwardPosition(0, newX, newY) != PartyBlock_none)
  {
    // move monsters closer since party is up against
    // an obstruction (a wall?)
    switch (distance)
    {
      case FarAway:
      case Nearby:
      case UpClose: 
      default:
        result = UpClose;
        break;
      case AutoUpClose:
      case AutoNearby:
      case AutoFarAway:
        result = AutoUpClose;
        break;
    };
  }
  else
  {
    Posx = newX;
    Posy = newY;
     // check next square
    if (newForwardPosition(0, newX, newY) != PartyBlock_none)
    {
      switch (distance)
      {
        case UpClose:
          break;
        case Nearby:
        case FarAway:
          result = Nearby;
          break;
        case AutoUpClose:
          break;
        case AutoNearby:
        case AutoFarAway:
          result = AutoNearby;
          break;
      };
    }

    Posx = origX;
    Posy = origY;
  };
  switch(result)
  {
    default:
    case UpClose:
    case Nearby:
    case FarAway:
      return result;
    case AutoUpClose:
      return UpClose;
    case AutoNearby:
      return Nearby;
    case AutoFarAway:
      return FarAway;
  };
}


BOOL PARTY::movePartyForward(int relativeDirection) // 0=facing, 1=right.etc
{
  int oldX = Posx;
  int oldY = Posy;
  int newX;
  int newY;
  BOOL CanMove;

  if (relativeDirection == -1)
  {
    newX = Posx;
    newY = Posy;
    PrevPosx=oldX;
    PrevPosy=oldY;
    PrevRelFacing = facing;
    CanMove = TRUE;
  }
  else
  {
    CanMove = newForwardPosition(relativeDirection, newX, newY) == PartyBlock_none;

    if (CanMove)
    {
      Posx = newX;
      Posy = newY;
      PrevPosx=oldX;
      PrevPosy=oldY;
      PrevRelFacing=relativeDirection;
    };
  };
  return CanMove;
}


//*****************************************************************************
// NAME: movePartyBackward
//
// PURPOSE:  
//*****************************************************************************
BOOL PARTY::movePartyBackward(void)
{
   int oldX = Posx;
   int oldY = Posy;
   int currFace = facing;

   BlockageType type = BlockedBlk;
   PartyBlockageType CanMove = PartyBlock_unknown;
   BOOL UsePrevPos = TRUE;

   if ( (abs(PrevPosx-Posx) > 1) || (abs(PrevPosy-Posy) > 1) )
     UsePrevPos=FALSE;
   if (!levelData.ValidAreaMapCoords(PrevPosx,PrevPosy))
     UsePrevPos=FALSE;

   if (UsePrevPos)
   {
#ifdef UAFEngine
     switch (GetDir(Posx,Posy, PrevPosx,PrevPosy))
     {
     case PathNorth: currFace=FACE_SOUTH; break;
     case PathSouth: currFace=FACE_NORTH; break;
     case PathEast:  currFace=FACE_WEST;  break;
     case PathWest:  currFace=FACE_EAST;  break;
     }
#endif
   }

   if (!globalData.levelInfo.stats[globalData.currLevel].overland)
   {
     switch (currFace) 
     {      
     case FACE_NORTH: type = levelData.area[Posy][Posx].blockage[1]; break;      
     case FACE_EAST:  type = levelData.area[Posy][Posx].blockage[3];  break;      
     case FACE_SOUTH: type = levelData.area[Posy][Posx].blockage[0]; break;      
     case FACE_WEST:  type = levelData.area[Posy][Posx].blockage[2];  break;
     }
     CanMove = PartyBlockage(type, currFace);
   }
   else
   {
     int newX = Posx;
     int newY = Posy;
     // check blockage in next square for overland
     switch (currFace) 
     {
     case FACE_NORTH: newY++; break;      
     case FACE_EAST:  newX--; break;      
     case FACE_SOUTH: newY--; break;      
     case FACE_WEST:  newX++; break;
     }     
     if (newX < 0) newX=0;
     if (newY < 0) newY=0;
     if (newX >= levelData.area_width)  newX=levelData.area_width-1;         
     if (newY >= levelData.area_height) newY=levelData.area_height-1;

     // if able to move into next square
     if ((oldX != newX) || (oldY != newY))
       type = (levelData.area[newY][newX].bkgrnd > 0) ? BlockedBlk : OpenBlk;
     else
       type = BlockedBlk;
     CanMove = (type == OpenBlk)?PartyBlock_none:PartyBlock_blocked;
   }

   if (CanMove == PartyBlock_none)
   {
     switch (currFace) 
     {      
     case FACE_NORTH: Posy++; break;      
     case FACE_EAST:  Posx--; break;      
     case FACE_SOUTH: Posy--; break;      
     case FACE_WEST:  Posx++; break;
     }

     if (globalData.levelInfo.stats[globalData.currLevel].overland)
     {
       // don't wrap party to opposite side of map when on overland map
       if (Posx < 0) Posx=0;
       if (Posy < 0) Posy=0;
       if (Posx >= levelData.area_width)  Posx=levelData.area_width-1;         
       if (Posy >= levelData.area_height) Posy=levelData.area_height-1;
     }
     else
     {
       if (Posx < 0) Posx=levelData.area_width-1;
       if (Posy < 0) Posy=levelData.area_height-1;
       if (Posx >= levelData.area_width) Posx=0;
       if (Posy >= levelData.area_height) Posy=0;       
     }

     if ((oldX == Posx) && (oldY == Posy)) CanMove=PartyBlock_blocked;
   }
   else
     PlayPartyBump();

   if (CanMove == PartyBlock_none)
   {
     PrevPosx=oldX;
     PrevPosy=oldY;
   }
   return CanMove = PartyBlock_none;
}


//*****************************************************************************
// NAME: turnPartyLeft
//
// PURPOSE:  
//*****************************************************************************
void PARTY::turnPartyLeft(void)
{
   if (facing == FACE_NORTH)
      facing = FACE_WEST;
   else if (facing == FACE_EAST)
      facing = FACE_NORTH;
   else if (facing == FACE_SOUTH)
      facing = FACE_EAST;
   else if (facing == FACE_WEST)
      facing = FACE_SOUTH;
}


//*****************************************************************************
// NAME: turnPartyRight
//
// PURPOSE:  
//*****************************************************************************
void PARTY::turnPartyRight(void)
{
   if (facing == FACE_NORTH)
      facing = FACE_EAST;
   else if (facing == FACE_EAST)
      facing = FACE_SOUTH;
   else if (facing == FACE_SOUTH)
      facing = FACE_WEST;
   else if (facing == FACE_WEST)
      facing = FACE_NORTH;
}


//*****************************************************************************
// NAME: turnParty180
//
// PURPOSE:  
//*****************************************************************************
void PARTY::turnParty180()
{
   switch (facing) 
   {
   case FACE_NORTH:
      facing = FACE_SOUTH;
      break;
   case FACE_EAST:
      facing = FACE_WEST;
      break;
   case FACE_SOUTH:
      facing = FACE_NORTH;
      break;
   case FACE_WEST:
      facing = FACE_EAST;
      break;
   }
}

CHARACTER *PARTY::FindCharacter(int uniquePartyID)
{
  int i;
  for (i=0;i<GetPartySize(party);i++)
  {
    if (characters[i].uniquePartyID == uniquePartyID)
    {
      return &characters[i];;
    };
  };
  return NULL;
}

//*****************************************************************************
// NAME: removeCharacter
//
// PURPOSE:  
//*****************************************************************************
void PARTY::removeCharacter(int loser, BOOL AddToTemps)
{  
   ASSERT( loser >= 0 );
   if (loser < 0) return;

   if ((loser >= 0) && (loser < numCharacters))
   {
      // not permanently deleted, add dude
      // back into the temp dude list
     CHARACTER tempCHARACTER;
     tempCHARACTER = characters[loser];

     if (tempCHARACTER.GetType() == CHAR_TYPE)
     {
       tempCHARACTER.serializeCharacter(TRUE, tempCHARACTER.GetName());
       AddToTemps=FALSE;
     }
     else
     {
       //if ((tempCHARACTER.GetType() == NPC_TYPE) && (!tempCHARACTER.IsPreGen))
       {
         tempCHARACTER.serializeCharacter(TRUE, tempCHARACTER.GetName());
         AddToTemps=FALSE;
       }
     }

      if (loser == numCharacters-1)
      {
         characters[loser].Clear();
      }
      else 
      {    
	     WORD i;
         for (i=loser;i<(numCharacters-1);i++)
         {
            characters[i] = characters[i+1];         
         };
         characters[i].Clear();
      }
      
      numCharacters--;
      if (activeCharacter>=numCharacters)
        activeCharacter=0;

#ifdef UAFEngine
      // must remove from party before adding back
      // into temps
      if (AddToTemps)
        tempChars.addTempCharacter(tempCHARACTER);
#endif
   }
}


//*****************************************************************************
// NAME: purgeCharacter
//
// PURPOSE:  
//*****************************************************************************
void PARTY::purgeCharacter(int loser)
{
   char fullPath[_MAX_PATH+1];     

   if ((loser >= 0) && (loser < numCharacters))
       //&& (characters[loser].GetType() != NPC_TYPE))
   {
     if (characters[loser].GetName() == "") return;

     strcpy(fullPath, GetSavePath());
     if (characters[loser].GetType()==NPC_TYPE) strcat(fullPath,"DCNPC_");
     strcat(fullPath, characters[loser].GetName());
     strcat(fullPath, ".chr");
     if (!MyDeleteFile(fullPath))
       WriteDebugString("Cannot delete character file %s\n", fullPath);

      if (characters[loser].icon.filename != "")
      {
        CString tmpPath;
        CString newName="";
        CString tmp;
        tmpPath = GetSavePath();
        if (characters[loser].GetType()==NPC_TYPE) newName += "DCNPC_";
        newName += characters[loser].GetName();
        newName += "_icon";
        tmp = characters[loser].icon.filename;
        StripFilenamePath(tmp);
        int index = tmp.Find('.', 0);
        if (index > 0) newName += tmp.Right(tmp.GetLength()-(index));
        tmpPath += newName;
        SetFileAttributes(tmpPath, FILE_ATTRIBUTE_NORMAL);
        if (!MyDeleteFile(tmpPath))
          WriteDebugString("Cannot delete character icon file %s\n", tmpPath);
      }
      if (characters[loser].smallPic.filename != "")
      {
        CString tmpPath;
        CString newName="";
        CString tmp;
        tmpPath = GetSavePath();
        if (characters[loser].GetType()==NPC_TYPE) newName += "DCNPC_";
        newName += characters[loser].GetName();
        newName += "_spic";
        tmp = characters[loser].smallPic.filename;
        StripFilenamePath(tmp);
        int index = tmp.Find('.', 0);
        if (index > 0) newName += tmp.Right(tmp.GetLength()-(index));
        tmpPath += newName;
        SetFileAttributes(tmpPath, FILE_ATTRIBUTE_NORMAL);
        if (!MyDeleteFile(tmpPath))
          WriteDebugString("Cannot delete character smallPic file %s\n", tmpPath);
      }

      if (loser == numCharacters-1)
      {
         characters[loser].Clear();
      }
      else 
      {    
	     WORD i;
         for (i=loser;i<(numCharacters-1);i++)
            characters[i] = characters[i+1]; 
         
         characters[i].Clear();
      }
      
      numCharacters--;

      if (activeCharacter>=numCharacters)
        activeCharacter=0;
   }
}

//*****************************************************************************
// NAME: TakePartyItems
//
// PURPOSE:  Remove party goods based on event data
//*****************************************************************************
void PARTY::TakePartyItems(TAKE_PARTY_ITEMS_DATA &data)
{
  int qty = 0;
  int dude=-1;

  switch (data.takeAffects)
  {
  case TakeAffectsParty:
    dude = -1;
    break;
  case TakeAffectsRndChar:
    dude = (RollDice(numCharacters, 1) - 1);
    break;
  case TakeAffectsActiveChar:
    dude = activeCharacter;
    break;
  }

  if (data.takeItems & TakeInventoryAction)
  {
    switch (data.itemSelectFlags)
    {
    case TakeSpecifiedQty:
      {
        POSITION pos = data.items.GetHeadPosition();
        while (pos != NULL)
        {
          ITEM &tempITEM = data.items.GetAtPos(pos);
          if (dude >= 0)
          {
            int count = 0;
            while (count < data.itemPcnt)
            {
              //int key = characters[dude].myItems.GetListKeyByItemKey(tempITEM.m_giID);
              int key = characters[dude].myItems.GetListKeyByItemName(tempITEM.itemID);
              if (key >= 0)
              {
                count++;
                if (characters[dude].delCharacterItem(key, 1))
                {
                  if (data.StoreItems)
                  {
                    tempITEM.qty=1;
                    globalData.vault[data.WhichVault].items.AddItem(tempITEM);
                  }
                }
              }
              else
                break;
            }
          }
          else // whole party
          {
            for (int i=0;i<numCharacters;i++)
            {
              int count = 0;
              while (count < data.itemPcnt)
              {
                //int key = characters[i].myItems.GetListKeyByItemKey(tempITEM.m_giID);
                int key = characters[i].myItems.GetListKeyByItemName(tempITEM.itemID);
                if (key >= 0)
                {
                  count++;
                  if (characters[i].delCharacterItem(key, 1))
                  {
                    if (data.StoreItems)
                    {
                      tempITEM.qty=1;
                      globalData.vault[data.WhichVault].items.AddItem(tempITEM);
                    }
                  }
                }
                else
                  break;
              }
            }
          }
          data.items.GetNext(pos);
        }
      }
      break;

    case TakeRndQty:    
    case TakePcntQty:
      // not used for items
      break;

    case TakeAll:     
      {
        if (dude >= 0)
        {
          POSITION pos = characters[dude].myItems.GetHeadPosition();
          while (pos != NULL)
          {
            ITEM &tempITEM = characters[dude].myItems.GetNext(pos);
            if (characters[dude].myItems.DeleteItem(tempITEM.key))
            {
              if (data.StoreItems)
                globalData.vault[data.WhichVault].items.AddItem(tempITEM);
            }
            // DeleteItem() removes the item from the list, so this
            // next call won't work.
            //characters[dude].myItems.GetNext(pos);
          }
        }
        else
        {
          for (int i=0;i<numCharacters;i++)
          {
            POSITION pos = characters[i].myItems.GetHeadPosition();
            while (pos != NULL)
            {
              ITEM &tempITEM = characters[i].myItems.GetNext(pos);
              if (characters[i].myItems.DeleteItem(tempITEM.key))
              {
                if (data.StoreItems)
                  globalData.vault[data.WhichVault].items.AddItem(tempITEM);
              }
              //characters[i].myItems.GetNext(pos);
            }
          }
        }
      }
      break;
    }
  }
  
  if (data.takeItems & TakeGemsAction)
  {
    if (dude >= 0)
    {
      int availgems = characters[dude].money.NumGems();
      qty = TakePartyItemQty(dude, data.gemsSelectFlags, data.gems, availgems);
      if ((data.StoreItems) && (qty > 0))
      {
        POSITION pos = characters[dude].money.Gems.GetHeadPosition();
        int i=0;
        while ((i < qty) && (pos != NULL))
        {
          GEM_TYPE gem = characters[dude].money.Gems.GetAt(pos);
          globalData.vault[data.WhichVault].money.AddGem(gem);
          characters[dude].money.Gems.GetNext(pos);
          i++;
        }
      }
      characters[dude].money.RemoveMultGems(qty);
    }
    else
    {
      for (int i=0;i<numCharacters;i++)
      {
        int availgems = characters[i].money.NumGems();
        qty = TakePartyItemQty(i, data.gemsSelectFlags, data.gems, availgems);
        if ((data.StoreItems) && (qty > 0))
        {
          POSITION pos = characters[i].money.Gems.GetHeadPosition();
          int j=0;
          while ((j < qty) && (pos != NULL))
          {
            GEM_TYPE gem = characters[j].money.Gems.GetAt(pos);
            globalData.vault[data.WhichVault].money.AddGem(gem);
            characters[j].money.Gems.GetNext(pos);
            j++;
          }
        }
        characters[i].money.RemoveMultGems(qty);
      }
    }
  }

  if (data.takeItems & TakeJewelryAction)
  {
    if (dude >= 0)
    {
      int availgems = characters[dude].money.NumJewelry();
      qty = TakePartyItemQty(dude, data.jewelrySelectFlags, data.jewelry, availgems);
      if ((data.StoreItems) && (qty > 0))
      {
        POSITION pos = characters[dude].money.Jewelry.GetHeadPosition();
        int i=0;
        while ((i < qty) && (pos != NULL))
        {
          GEM_TYPE gem = characters[dude].money.Jewelry.GetAt(pos);
          globalData.vault[data.WhichVault].money.AddJewelry(gem);
          characters[dude].money.Jewelry.GetNext(pos);
          i++;
        }
      }
      characters[dude].money.RemoveMultJewelry(qty);
    }
    else
    {
      for (int i=0;i<numCharacters;i++)
      {
        int availgems = characters[i].money.NumJewelry();
        qty = TakePartyItemQty(i, data.jewelrySelectFlags, data.jewelry, availgems);
        if ((data.StoreItems) && (qty > 0))
        {
          POSITION pos = characters[i].money.Jewelry.GetHeadPosition();
          int j=0;
          while ((j < qty) && (pos != NULL))
          {
            GEM_TYPE gem = characters[j].money.Jewelry.GetAt(pos);
            globalData.vault[data.WhichVault].money.AddJewelry(gem);
            characters[j].money.Jewelry.GetNext(pos);
            j++;
          }
        }
        characters[i].money.RemoveMultJewelry(qty);
      }
    }
  }

  if (data.takeItems & TakeMoneyAction)
  {
    // convert money type to base
    qty = globalData.moneyData.ConvertToBase(data.platinum, data.moneyType);

    if (dude >= 0)
    {
      int avail = characters[dude].money.Total();//globalData.moneyData.ConvertToBase(characters[dude].poolCharacterGold(), globalData.moneyData.GetDefaultType());
      int taken = TakePartyItemQty(dude, data.platinumSelectFlags, qty, avail);
      if (taken > 0)
      {
        if (data.StoreItems)
        {
          int even = 0;
          double remain = 0.0;

          even = globalData.moneyData.Convert(taken, globalData.moneyData.GetBaseType(), data.moneyType, &remain);
          globalData.UpdateMoneyInVault(data.WhichVault, data.moneyType, even);
          globalData.UpdateMoneyInVault(data.WhichVault,globalData.moneyData.GetBaseType(), remain);
        }
        // remove money from character
        characters[dude].payForItem(taken, globalData.moneyData.GetBaseType());
      }
    }
    else // dude = -1, so take equally from all party members
    {
      for (int i=0;i<numCharacters;i++)
      {
        int avail = characters[i].money.Total();//globalData.moneyData.ConvertToBase(characters[i].poolCharacterGold(), globalData.moneyData.GetDefaultType());
        int taken = TakePartyItemQty(i, data.platinumSelectFlags, qty, avail);
        if (taken > 0)
        {
          if (data.StoreItems)
          {
            int even = 0;
            double remain = 0.0;
            even = globalData.moneyData.Convert(taken, globalData.moneyData.GetBaseType(), data.moneyType, &remain);
            globalData.UpdateMoneyInVault(data.WhichVault,data.moneyType, even);
            globalData.UpdateMoneyInVault(data.WhichVault,globalData.moneyData.GetBaseType(), remain);
          }

          // remove money from character
          characters[i].payForItem(taken, globalData.moneyData.GetBaseType());
        }
      }
    }
  }

  if (dude >= 0)
  {
    characters[dude].SetEncumbrance(characters[dude].determineEffectiveEncumbrance());
  	characters[dude].determineMaxMovement();
  }
  else
  {
    for (int i=0;i<numCharacters;i++)
    {
      characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
  	  characters[i].determineMaxMovement();
    }
  }

}

int PARTY::TakePartyItemQty(int dude, takeItemQtyType type, int amount, int &data)
{
  int qty=0;
  if (data == 0)
    return 0;

  switch (type)
  {
  case TakeSpecifiedQty:
    qty = amount;
    qty = min( qty, data);
    data -= qty;
    break;

  case TakeRndQty:
    qty = RollDice(amount, 1);
    qty = min( qty, data);
    data -= qty;
    break;

  case TakePcntQty:      
    qty = (int) ((double)data * ((double)amount / 100.0) );
    qty = min( qty, data);
    data -= qty;
    break;

  case TakeAll:
    qty = data;
    data -= qty;
    break;
  } 

  return qty;
}

int PARTY::GetNextUniquePartyID()
{
  int i;
  int used[MAX_PARTY_MEMBERS];
  memset(used, MAX_PARTY_MEMBERS+1, sizeof(used)); // set to impossible value

  // collect current uniquePartyID values
  for (i=0;i<numCharacters;i++)
    used[i]=characters[i].uniquePartyID;

  // Search for value that isn't already used by party member.
  for (i=0;i<MAX_PARTY_MEMBERS;i++)
  {
    bool found=false;
    for (int u=0; (u<MAX_PARTY_MEMBERS) && (!found); u++)
    {
      if (used[u]==i)
        found = true;
    }
    if (!found) return i;
  }
  die(0xab529); // should not be possible to reach this
  return numCharacters; // old default behavior
}

#ifdef UAFEngine


void PARTY::RunJoinPartyMemorizeScripts(int charIndex)
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  scriptContext.SetCharacterContext(&characters[charIndex]);
  characters[charIndex].RunCharacterScripts(JOIN_PARTY, 
                                            ScriptCallback_RunAllScripts, 
                                            NULL,
                                            "Run join party memorize scripts");
}

CString PARTY::ForEachPartyMember(const CString& SAName, const CString& scriptName)
{
  CString result;
  int i;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  // For each member of the party:
  // 1 - Set Character Context
  // 2 - Run global script
  for (i=numCharacters-1; i>=0; i--)
  {
    scriptContext.SetCharacterContext(&characters[i]);
    scriptContext.SetSA_Source_Type(ScriptSourceType_ForEachPrtyMember);
    result = RunGlobalScript(SAName, scriptName, true);
  };
  return result;
}

//*****************************************************************************
// NAME: addNPCToParty
//
// PURPOSE:  
//*****************************************************************************
//BOOL PARTY::addNPCToParty(int npcIndex, int hitPointMod, BOOL useOrig)
BOOL PARTY::addNPCToParty(const CHARACTER_ID& characterID, int hitPointMod, BOOL useOrig)
{ 
   //if (!globalData.charData.HaveKey(npcIndex))
   //if (!globalData.charData.HaveKey(npcID))
   if (!globalData.charData.HaveNPC(characterID))
   {
     WriteDebugString("Invalid npcIndex %s in PARTY::addNPCToParty()\n", characterID.UniqueName());
     return FALSE;
   }

   if (CanAddNPC())
   {
      // adjust base NPC morale using highest charisma
      // score in the party
      int HiCharisma = 0;
      int MoraleMod = 0;
      for (int i=0;i<numCharacters;i++)
      {
        BYTE pc_cha = characters[i].GetAdjCha();
        if (pc_cha > HiCharisma) HiCharisma = pc_cha;
      }

      switch (HiCharisma)
      {
      case 3: MoraleMod = -30;break;
      case 4: MoraleMod = -25;break;
      case 5: MoraleMod = -20;break;
      case 6: MoraleMod = -15;break;
      case 7: MoraleMod = -10;break;
      case 8: MoraleMod = -5;break;
      case 14: MoraleMod = 5;break;
      case 15: MoraleMod = 15;break;
      case 16: MoraleMod = 20;break;
      case 17: MoraleMod = 30;break;
      case 18: MoraleMod = 40;break;
      default: break; // 9..13
      }

      double percent = ((double)hitPointMod / (double)100);

      CHARACTER data;      
      //if (globalData.charData.GetCharacterData(npcIndex, data))
      //if (globalData.charData.FetchCharacterData(characterID, &data))
      if (globalData.charData.FetchCharacter(characterID, &data))
      {
        if (!useOrig)
        {
          CHARACTER TEMPchar;
          // check for saved NPC first (will have more recent data)
          TEMPchar=data;
          if (TEMPchar.serializeCharacter(FALSE, TEMPchar.GetName()))
            data = TEMPchar;
        }

        characters[numCharacters] = data;
        characters[numCharacters].uniquePartyID = GetNextUniquePartyID();
        //characters[numCharacters].origIndex = npcIndex;
        characters[numCharacters].characterID = characterID;
        characters[numCharacters].SetType(NPC_TYPE);
        characters[numCharacters].SetPartyMember();        
        characters[numCharacters].SetHitPoints(
           (((double)characters[numCharacters].GetMaxHitPoints()) * percent));
        if (characters[numCharacters].GetAdjHitPoints() > 0)
          characters[numCharacters].SetStatus(Okay);
        else
          characters[numCharacters].SetStatus(Unconscious);
        characters[numCharacters].SetMorale(characters[numCharacters].GetAdjMorale() + MoraleMod);
        numCharacters++;

        RunJoinPartyMemorizeScripts(numCharacters-1);

        return TRUE;
      }
      else
      {
        //WriteDebugString("Failed GetCharacterData(%i) in PARTY::addNPCToParty()\n", npcIndex);
        WriteDebugString("Failed GetCharacterData(%s) in PARTY::addNPCToParty()\n", characterID.UniqueName());
        return FALSE;
      }
   }
   else
     WriteDebugString("Failed CanAddNPC() in PARTY::addNPCToParty()\n");

   return FALSE;
}
#endif
//*****************************************************************************
// NAME: removeNPCFromParty
//
// PURPOSE:  
//*****************************************************************************
//void PARTY::removeNPCFromParty(int npcIndex)
void PARTY::removeNPCFromParty(const CHARACTER_ID& characterID)
{
   int i=0;
   BOOL found = FALSE;

   while ((i<numCharacters) && (!found))
   {
      if (characters[i].GetType()==NPC_TYPE)      
      {
        //if (characters[i].origIndex == npcIndex)
        if (characters[i].characterID == characterID)
        {
          found = TRUE;
          removeCharacter(i, FALSE);
        }
      }  
      i++;
   }

   if (!found)
     //WriteDebugString("RemoveNPCFromParty for char %i, no matching npc found\n", npcIndex);
     WriteDebugString("RemoveNPCFromParty for char %s, no matching npc found\n", characterID.UniqueName());
}

//*****************************************************************************
// NAME: isNPCinParty
//
// PURPOSE:  
//*****************************************************************************
//BOOL PARTY::isNPCinParty(int npcIndex)
BOOL PARTY::isNPCinParty(const CHARACTER_ID& characterID) const
{
  int i=0;
  BOOL found = FALSE;

  while ((i<numCharacters) && (!found))
  {
    if (characters[i].GetType() == NPC_TYPE)
    {
      //if (characters[i].origIndex == npcIndex)
      if (characters[i].characterID == characterID)
         found = TRUE;
    }
    i++;
  }

  return found;
}


void PARTY::deductPoolMoney(itemClassType type, int qty)
{
  if (!itemIsMoney(type)) return;

  if (!moneyPooled)
    return;

  switch (type) 
  {
  case GemType:
    poolSack.RemoveMultGems(qty);
    break;
  case JewelryType:
    poolSack.RemoveMultJewelry(qty);
    break;
  default:
    poolSack.Subtract(type , qty);
    break;
  }
  moneyPooled = !(poolSack.IsEmpty());
}


BOOL PARTY::PartyHasMoney()
{
  if (moneyPooled)
    return (!poolSack.IsEmpty());

  for (int i=0; i<numCharacters;i++)
  {
    if (!characters[i].money.IsEmpty()) 
      return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
// NAME: PARTY::GetPoolGoldValue
//
// PURPOSE:  
//*****************************************************************************
int PARTY::GetPoolGoldValue()
{
  return poolSack.ConvertToDefault(poolSack.Total(), poolSack.GetBaseType());
}

//*****************************************************************************
// NAME: poolPartyGold
//
// PURPOSE:  
//*****************************************************************************
void PARTY::poolPartyGold()
{
   int i;   
   for (i=0; i<numCharacters;i++)
   {
     poolSack.Transfer(characters[i].money);
	   characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
	   characters[i].determineMaxMovement();
   }
   
   moneyPooled = !poolSack.IsEmpty();
}

//*****************************************************************************
//    NAME: PARTY::AutoUpConvertPoolMoney
//
// PURPOSE: 
//
//*****************************************************************************
void PARTY::AutoUpConvertPoolMoney()
{
  if (!moneyPooled) return;  
  poolSack.AutoUpConvert();
}

//*****************************************************************************
// NAME: sharePartyGold
//
// PURPOSE:  
//*****************************************************************************
void PARTY::sharePartyGold()
{
   int tot = 0;
   int temp = 0;
   int i;
   int avail;
   int shared = 0;
   
   if (!moneyPooled)
      return;

   if (numCharacters == 0)
   {
     poolSack.Clear();
     moneyPooled=FALSE;
     return;
   }
   
   int c;
   for (c=0; c<poolSack.NumCoinTypes();c++)
   {
     shared = 0;
     itemClassType type = globalData.moneyData.GetItemClass(c);
     avail = poolSack[type];


     if (avail > 0)
     {
        int moneyweight = globalData.moneyData.GetWeight();
        if (moneyweight <= 0) moneyweight = 1;
        temp = avail % (numCharacters);
        tot = avail / (numCharacters);
      
        for (i=0; i<numCharacters;i++)
        {
          int maxenc = characters[i].GetAdjMaxEncumbrance();
          int curenc = characters[i].GetEncumbrance();

          for (int t=0;t<tot;t++)
          {
            // make sure allowed encumbrance is not exceeded            
            if (curenc <= maxenc)
            {
              characters[i].money.Add(type, 1);              
              shared += 1;
            }
            else
              break;

            if (((t+1) % (moneyweight*10)) == 0) // every 10 units of encumbrance
            {
              characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
              curenc = characters[i].GetEncumbrance();
            }
          }
          characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
        }
      
        for (i=0; i<temp;i++, temp--)
        {
          if (characters[i].GetEncumbrance() <= characters[i].GetAdjMaxEncumbrance())
          {
            characters[i].money.Add(type, 1);
            characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
            shared += 1;
          }
        }
     }
     if (shared > 0)
       poolSack.Subtract(globalData.moneyData.GetItemClass(c), shared);
   }

   shared = 0;
   avail = poolSack.NumGems();
   if (avail > 0)
   {
      temp = avail % (numCharacters);
      tot = avail / (numCharacters);
    
      for (i=0; i<numCharacters;i++)
      {
        int maxenc = characters[i].GetAdjMaxEncumbrance();
        for (int g=0;g<tot;g++)
        {
          if (characters[i].GetEncumbrance() <= maxenc)
          {
            characters[i].money.AddGem();
            shared += 1;
          }
          else
            break;

          if (((g+1) % 10) == 0) // every 10
            characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
        }
        characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
      }    
      for (i=0; i<temp;i++, temp--)
      {
        if (characters[i].GetEncumbrance() <= characters[i].GetAdjMaxEncumbrance())
        {
          characters[i].money.AddGem();
          characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
          shared += 1;
        }
      }
   }
   if (shared > 0)
     poolSack.RemoveMultGems(shared);

   shared = 0;
   avail = poolSack.NumJewelry();
   if (avail > 0)
   {
      temp = avail % (numCharacters);
      tot = avail / (numCharacters);
    
      for (i=0; i<numCharacters;i++)
      {
        int maxenc = characters[i].GetAdjMaxEncumbrance();
        for (int g=0;g<tot;g++)
        {
          if (characters[i].GetEncumbrance() <= maxenc)
          {
            characters[i].money.AddJewelry();
            shared += 1;
          }
          else
            break;

          if (((g+1) % 10) == 0) // every 10
            characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
        }
        characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
      }    
      for (i=0; i<temp;i++, temp--)
      {
        if (characters[i].GetEncumbrance() <= characters[i].GetAdjMaxEncumbrance())
        {
          characters[i].money.AddJewelry();
          characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
          shared += 1;
        }
      }
   }
   if (shared > 0)
     poolSack.RemoveMultJewelry(shared);

   for (i=0; i<numCharacters;i++)
   {
	   //characters[i].SetEncumbrance(characters[i].determineEffectiveEncumbrance());
	   characters[i].determineMaxMovement();
   }

   moneyPooled = (poolSack.Total() > 0);
   moneyPooled = !(poolSack.IsEmpty());
   //poolSack.Clear();
}


//*****************************************************************************
// NAME: setPartyAdventureState
//
// PURPOSE:  
//*****************************************************************************
void PARTY::setPartyAdventureState()
{
   level = GetCurrentLevel();
   adventuring = TRUE;
   //sharePartyGold();
}


//*****************************************************************************
// NAME: setPartyLevelState
//
// PURPOSE:  
//*****************************************************************************
void PARTY::setPartyLevelState(int plevel, int x, int y, int f)
{
   level = (BYTE) plevel;
   searching = FALSE;
   looking = FALSE;
   resting = FALSE;
   areaView = FALSE;
   activeCharacter = 0;
   activeItem = 0;
   PrevPosx=Posx;
   PrevPosy=Posy;   
   Posx = x;
   Posy = y;
   facing = f;
   SetVisited(level,x,y);  
}

#ifdef UAFEngine

//*****************************************************************************
// NAME:    prevItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::prevItem()
{
  if (ItemsOnPage > 0)
  {
    if (activeItem > 0)
      activeItem--;
    else 
      activeItem = (BYTE) (ItemsOnPage - 1);
  }
}

//*****************************************************************************
// NAME:    nextItem
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::nextItem()
{
  if (ItemsOnPage > 0)
  {
    activeItem = 
      (BYTE) ((activeItem+1) % ItemsOnPage);        
  }
}

//*****************************************************************************
// NAME:    nextCharacter
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::nextCharacter()
{
  if (numCharacters > 0)
  {
    activeCharacter = 
      (BYTE) ((activeCharacter+1) % (numCharacters));
  }
}

//*****************************************************************************
// NAME:    prevCharacter
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::prevCharacter()
{
  if (numCharacters > 0)
  {
    if (activeCharacter > 0)
      activeCharacter--;
    else
      activeCharacter = 
      (BYTE) (numCharacters-1);
  }
}  
#endif

//*****************************************************************************
// NAME: distributeExpPoints
//
// PURPOSE:  
//*****************************************************************************
void PARTY::distributeExpPoints(int total)
{
  int numOK, i;
  if (total <= 0)
     return;
  if (numCharacters == 0)
    return;

  numOK = 0;
  for (i=0; i<numCharacters; i++)
  {
    if (characters[i].GetStatus() == Okay)
    {
      numOK++;
    };
  };
  if (numOK == 0) return;
  int remainder = total % (numOK);
  int bonus = total / (numOK);
  int lcv;
   
  for (lcv=0;lcv<numCharacters;lcv++)
  {
    if (characters[lcv].GetStatus() == Okay)
    {
      characters[lcv].giveCharacterExperience(bonus+remainder);
      remainder = 0;
    };
  }  
}


//*****************************************************************************
// NAME: getActiveChar
//
// PURPOSE:  
//*****************************************************************************
CHARACTER& PARTY::getActiveChar()
{
  if (numCharacters == 0) return FakeCharacter;
  return GetCharacter(activeCharacter, NULL);
}

//*****************************************************************************
// NAME:    GetCharacter
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//#ifdef UAFEngine
CHARACTER& PARTY::GetCharacter(int activeChar, const char *msg)
{
  ASSERT(    ((activeChar >= 0) && (activeChar <= numCharacters))
          || (activeChar == -2));
  if (activeChar == -2)
  {
    if (pScriptContext->GetCreatedCharacterContext(msg) != NULL)
    {
      return *pScriptContext->GetCreatedCharacterContext(msg);
    };
  };
  return characters[activeChar];
}
//#endif

//*****************************************************************************
// NAME:    GetStrongestCharacter
//
// PURPOSE: Find a character to bash a door
//
// RETURNS: Pointer to strongest character
//*****************************************************************************
CHARACTER& PARTY::GetStrongestCharacter(void)
{
  int i, strongest;
  for (strongest=0,i=0; i<numCharacters; i++)
  {
    if (characters[i].GetAdjStr() > characters[strongest].GetAdjStr())
    {
      strongest = i; 
      continue;
    };
    if (characters[i].GetAdjStr() < characters[strongest].GetAdjStr())
    {
      continue;
    };
    if (characters[i].GetAdjStrMod() > characters[strongest].GetAdjStr())
    {
      strongest = i; 
      continue;
    };
  };
  return characters[strongest];
}


//*****************************************************************************
// NAME:    GetBestLockpicker
//
// PURPOSE: Find a character to Pick a lock
//
// RETURNS: Pointer to best lockpicking character
//*****************************************************************************
CHARACTER& PARTY::GetBestLockpicker(int *bestSkillValue)
{
  SKILL_ID skillOpenLocks;
  int indx;
  skillOpenLocks = Skill_OpenLocks;
  indx = GetBestSkill(skillOpenLocks, bestSkillValue, false);
  if (indx < 0) indx = 0;
  return characters[indx];
}
  
//*****************************************************************************
// NAME:    GetBestSkill
//
// PURPOSE: Find a character with particular skill
//
// RETURNS: Party index of most skillful character
//          (or -1 if nobody has such a skill)
//*****************************************************************************
int PARTY::GetBestSkill(const SKILL_ID& skillID, int *bestSkillValue, bool minimize)
{  
  int i, result;
  result = -1;  
  *bestSkillValue = NoSkill;
  for (i=0; i<numCharacters; i++)
  {
    int skillValue;
    //if (!GetAdjSkillValue(skillID, &characters[i], NULL, NULL, &skillValue, minimize)) continue;
    skillValue = characters[i].GetAdjSkillValue(skillID, minimize, true);
    if (skillValue == NoSkill) continue;
    if (skillValue > *bestSkillValue)
    {
      *bestSkillValue = skillValue;
      result = i;
    };
  };
  return result;
}



//*****************************************************************************
// NAME: addSpecialItem
//
// PURPOSE:  
//*****************************************************************************
void PARTY::addSpecialItem(int item)
{
  if (!globalData.specialItemData.HaveItem(item))
  {
    WriteDebugString("Bogus special item index in addSpecialItem(%u)\n",
                     item);
    return;
  }

  globalData.specialItemData.SetStage(item, 1);
}


//*****************************************************************************
// NAME: addSpecialKey
//
// PURPOSE:  
//*****************************************************************************
void PARTY::addSpecialKey(int key)
{
  if (!globalData.keyData.HaveItem(key))
  {
    WriteDebugString("Bogus special key index in addSpecialKey(%u)\n",
                     key);
    return;
  }

  globalData.keyData.SetStage(key, 1);
}


//*****************************************************************************
// NAME: removeSpecialItem
//
// PURPOSE:  
//*****************************************************************************
void PARTY::removeSpecialItem(int item)
{
  if (!globalData.specialItemData.HaveItem(item))
  {
    WriteDebugString("Bogus special item index in removeSpecialItem(%u)\n",
                     item);
    return;
  }

  globalData.specialItemData.SetStage(item, 0);
}


//*****************************************************************************
// NAME: removeSpecialKey
//
// PURPOSE:  
//*****************************************************************************
void PARTY::removeSpecialKey(int key)
{
   if (!globalData.keyData.HaveItem(key))
  {
    WriteDebugString("Bogus special key int in removeSpecialKey(%u)\n",
                     key);
    return;
  }

  globalData.keyData.SetStage(key, 0);
}


//*****************************************************************************
// NAME: hasSpecialItem
//
// PURPOSE:  
//*****************************************************************************
BOOL PARTY::hasSpecialItem(int item)
{
  if (!globalData.specialItemData.HaveItem(item))
  {
    WriteDebugString("Bogus special item index in hasSpecialItem(%u)\n",
                     item);
    return FALSE;
  }

  return (globalData.specialItemData.GetStage(item) > 0);
}


//*****************************************************************************
// NAME: hasSpecialKey
//
// PURPOSE:  
//*****************************************************************************
BOOL PARTY::hasSpecialKey(int key)
{
  if (!globalData.keyData.HaveItem(key))
  {
    WriteDebugString("Bogus special key index in hasSpecialKey(%u)\n",
                     key);
    return FALSE;
  }

  return (globalData.keyData.GetStage(key) > 0);
}


//*****************************************************************************
// NAME: getPartySpeed
//
// PURPOSE:  
//*****************************************************************************
BYTE PARTY::getPartySpeed()
{
  BYTE pspeed = 12;
  int i;

  for (i=0;i<numCharacters;i++)
  {
    BYTE charmove = characters[i].GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS,
                                                    "Get party speed.  Find slowest character.");
    if (charmove < pspeed)
      pspeed = charmove;
  }

  return pspeed;
}

//*****************************************************************************
// NAME:    AddPooledMoneyToVault
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::AddPooledMoneyToVault(int WhichVault)
{
  // put all party funds into vault
  poolPartyGold();

  moneyPooled = FALSE;
  globalData.TransferMoney(WhichVault, poolSack);
}

//*****************************************************************************
// NAME:    RemovePooledMoneyFromVault
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::RemovePooledMoneyFromVault(int WhichVault)
{
  if ((WhichVault < 0) || (WhichVault >= MAX_GLOBAL_VAULTS))
  {
    TRACE("Bogus vault index in RemovePooledMoneyFromVault()\n");
    die(0xab52a);    
    return;
  }

  // share vault funds among party members
  
  // put vault funds into pool vars
  moneyPooled = TRUE;
  poolSack.Transfer(globalData.vault[WhichVault].money);
  AutoUpConvertPoolMoney();
  // now share funds
  sharePartyGold();
}


//*****************************************************************************
// NAME: IsPartyDrunk
//
// PURPOSE:  
//*****************************************************************************
BOOL PARTY::IsPartyDrunk()
{
  return (drinkPoints >= DRUNK_THRESHOLD);
}


//*****************************************************************************
// NAME: UpdatePartyDrinkPoints
//
// PURPOSE:  
//*****************************************************************************
void PARTY::UpdatePartyDrinkPoints()
{
  static int lastHour = -1;
  static int lastDay = -1;

  if (drinkPoints <= 0)
    return;

  if (lastHour == -1)
  {
    lastHour = hours;
    lastDay = days;
  }

  while ((lastDay != days) && (drinkPoints > 0))
  {
    lastDay++;
    drinkPoints -= (DRINK_POINT_DEC_PER_HOUR*24);
  }

  while ((lastHour != hours) && (drinkPoints > 0))
  {
    lastHour++;
    drinkPoints -= DRINK_POINT_DEC_PER_HOUR;
  }

  if (drinkPoints < 0)
    drinkPoints = 0;
}


//*****************************************************************************
// NAME:    getCharWeaponText
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void PARTY::getCharWeaponText(int index, CString &wpn, CString &dmg)
{
  characters[index].getCharWeaponText(wpn, dmg);
}

#ifdef UAFEngine


//*****************************************************************************
// NAME: addTempToParty
//
// PURPOSE:  
//*****************************************************************************
CHARACTER *PARTY::addTempToParty(CHARACTER &luckyDude)
{
   // To leave room for future NPC additions to party, 
   // only allow up to GetMaxPartyCharacters() of player
   // created characters into the party list.
   //
  for (int i=0;i<numCharacters;i++)
  {
    if (characters[i].IsSameCharacter(luckyDude))
      return &characters[i];
  }

  //if (numCharacters < GetMaxPartyCharacters())
  if (numCharacters < GetMaxPartyMembers())
  {
    characters[numCharacters] = luckyDude;
    characters[numCharacters].uniquePartyID = GetNextUniquePartyID();
    characters[numCharacters].SetPartyMember();
    if (characters[numCharacters].GetType()!=NPC_TYPE)
      characters[numCharacters].origIndex = -1;
    characters[numCharacters].UpdateSpellAbility();
    //characters[numCharacters].SortSpells();
    numCharacters++;
    return &characters[numCharacters-1];
  };
  return NULL;
}
#endif

//*****************************************************************************
// NAME: PARTY::TeleportParty
//
// PURPOSE:  
//*****************************************************************************
BOOL PARTY::TeleportParty(TRANSFER_DATA &dest)
{
  BOOL success = TRUE;

  int currLevel = GetCurrentLevel();

  int dx,dy,df;
  if (dest.destLevel != currLevel)
  {
    // this data gets wiped when the new level is loaded
    TRANSFER_DATA temp;
    temp = dest;

    // transfer to new level
    if (!LoadLevel(temp.destLevel))
    {
      success = FALSE;
      SetMiscError(LevelLoadError);
    }
    else
    {
      WriteDebugString("Teleporting: level %u data loaded successfully\n", temp.destLevel);

      // need to use entry point data from loaded level
      if (temp.destEP >= 0)
      {
        dx = GetEntryPtX(temp.destEP);
        dy = GetEntryPtY(temp.destEP);
        // entry point dialog doesn't allow facing to be given yet
        //df = globalData.levelInfo.GetEPFacing(temp.destEP);
        df = temp.m_facing;
      }
      else
      {
        dx = temp.destX;
        dy = temp.destY;
        df = temp.m_facing;
      }

      if (   (dx < 0) || (dx >= CurrLevelWidth())
          || (dy < 0) || (dy >= CurrLevelHeight()))
      {
        // that entry point is bogus
        WriteDebugString("Incorrect entry point data in TeleportParty()\n");
        success = FALSE;
        // reload old level?
      }
      else
      {
        if (df == 4)
        {
          df = facing;  // Facing unchanged
        };
        facing = df;
        setPartyAdventureState();
        setPartyLevelState(temp.destLevel, dx, dy, df);
      }
    }
  }
  else // transfer within same level
  {
    // use entry point data for current level
    if (dest.destEP >= 0)
    {
      dx = GetEntryPtX(dest.destEP);
      dy = GetEntryPtY(dest.destEP);
      // entry point dialog doesn't allow facing to be given yet
      //df = globalData.levelInfo.GetEPFacing(dest.destEP);
      df = dest.m_facing;
    }
    else if (dest.destEP == -2)
    {
      dx = Posx;
      dy = Posy;
      df = dest.m_facing;
    }
    else
    {
      dx = dest.destX;
      dy = dest.destY;
      df = dest.m_facing;
    }

    if (df == 4)
    {
      df = facing;  // Facing unchanged
    };
    facing = df;
    setPartyAdventureState();
    setPartyLevelState(dest.destLevel, dx, dy, df);
  }

  return success;
}

#ifdef UAFEngine


//*****************************************************************************
// NAME: PARTY::GetCharThatNeedsHealedFirst
//
// PURPOSE:  fill in CHARACTER with party member that needs the most
//           healing and return TRUE. If no party members need healing, 
//           return FALSE.
//*****************************************************************************
BOOL PARTY::GetCharThatNeedsHealedFirst(CHARACTER &dude)
{
  int i;

  // look for unconscious chars first
  for (i=0;i<numCharacters;i++)
  {
    if (   ((characters[i].GetAdjStatus() == Unconscious) || (characters[i].GetAdjStatus() == Dying))
        && (characters[i].GetAdjHitPoints() <= 0))
    {
      dude = characters[i];   
      return TRUE;
    }
  }

  DWORD worst = 0;
  DWORD total;
  int index = -1;

  // find char with biggest hp deficit
  for (i=0;i<numCharacters;i++)
  {
    if (characters[i].GetAdjStatus() == Okay)
    {
      total = characters[i].GetHealingPointsNeeded();
      if (total > worst)
      {
        worst = total;
        index = i;
      }
    }
  }

  if (index >= 0)
  {
    dude = characters[index];
    return TRUE;
  }

  return FALSE;

}

//*****************************************************************************
// NAME: PARTY::FixParty
//
// PURPOSE:  
//*****************************************************************************
/*
void PARTY::FixParty(BOOL cast)
{
  int i;
  globalData.fixSpellBook.SortSpells(); // If not already sorted.
  for (i=0;i<numCharacters;i++)
    FixParty(characters[i], cast);
}
*/
/*
//*****************************************************************************
// NAME: PARTY::FixParty
//
// PURPOSE:  
//*****************************************************************************
void PARTY::FixParty(CHARACTER &caster, BOOL cast)
{
  if (!caster.CanCastSpells())
    return;
  
  POSITION p1 = caster.GetFirstSpellPosition(); // access ordered list
  while (p1 != NULL)
  {
    // find memorized spells that match a spell in globalData.fixSpellBook;
    if (caster.GetSpellAt(p1).IsMemorized())
    {
      POSITION p2 = globalData.fixSpellBook.GetHeadPosition();
      while (p2 != NULL)
      {
        if (globalData.fixSpellBook.PeekAtPos(p2).spell == caster.GetSpellAt(p1).spell)
        {
          // we have a match, and the spell is memorized by this caster
          CHARACTER recipient;
          if (GetCharThatNeedsHealedFirst(recipient))
          {
            WriteDebugString("%s is fixing %s\n", caster.GetName(), recipient.GetName());
            // if there is a character that needs healing
            // now cast the spell
            caster.RemoveAllTargets(); // clear target data
            caster.AddTarget(recipient); // add recipient as target
            caster.CastSpell(caster.GetSpellKeyAt(p1)); // invoke spell on target
            //caster.CastSpell(caster.spells.list.GetKeyAt(p1)); // invoke spell on target
          }
        }

        globalData.fixSpellBook.GetNext(p2);
      }
    }

    caster.GetNextSpell(p1);
  }

  caster.RemoveAllTargets(); // make sure targ list is cleaned up
}
*/
DWORD randomMT(void);

class FIX_SPELL_ENTRY
{
  int m_numCasters;
  CHARACTER **m_casters;
  int m_numTarget;
  CHARACTER **m_targets;
public:
  SPELL_DATA *m_pSpell;
  CHARACTER *m_activeCaster;
  CHARACTER *m_activeTarget;
  //POSITION m_pos;
  FIX_SPELL_ENTRY(void)
  {
    m_numCasters = 0;
    m_casters = NULL;
    m_numTarget = 0;
    m_targets = NULL;
  };
  ~FIX_SPELL_ENTRY(void)
  {
    if (m_casters != NULL) delete m_casters;
    if (m_targets != NULL) delete m_targets;
  };
  void Swap(FIX_SPELL_ENTRY *pFSE);
  void RandomCaster(PARTY *pParty);
  void RandomTarget(PARTY *pParty, int environment); //0=encamp; 1=temple
};

void FIX_SPELL_ENTRY::Swap(FIX_SPELL_ENTRY *pFSE)
{
  FIX_SPELL_ENTRY temp;
  temp = *this;
  *this = *pFSE;
  *pFSE = temp;
  temp.m_casters = temp.m_targets = NULL;
}


void FIX_SPELL_ENTRY::RandomCaster(PARTY *pParty)
{
  CHARACTER *pChar;
  //POSITION p1;
  int i;
  if (m_casters == NULL)
  { // Must initialize to all party members able to cast spells.
    m_casters = new CHARACTER *[pParty->numCharacters];
    m_numCasters = 0;
    for (i=0; i<pParty->numCharacters; i++)
    {
      pChar = &pParty->characters[i];
      if (pChar->CanCastSpells())
      {
        m_casters[m_numCasters++] = pChar;
      };
    };
  };
  while (m_numCasters > 0)
  {
    i = randomMT() % m_numCasters;
    pChar = m_casters[i];




    

    {
      int j, n;
      n = pChar->GetSpellCount();
      for (j=0; j<n; j++)
      //for (p1 = pChar->GetFirstSpellPosition(); p1 != NULL; pChar->GetNextSpell(p1))
      {
        



        // find memorized spells that match a spell in globalData.fixSpellBook;
        if (pChar->PeekCharacterSpell(j)->IsMemorized())
        {
          //if (m_pSpell->m_gsID == pChar->GetSpellAt(p1).spell)
          if (m_pSpell->SpellID() == pChar->PeekCharacterSpell(j)->spellID)
          {
            m_activeCaster = pChar;
            return;
          };
        };
      };
    };
  // This fellow cannot cast this spell.  Remove him!
    m_casters[i] = m_casters[--m_numCasters];
  };
  m_activeCaster = NULL;
}

void FIX_SPELL_ENTRY::RandomTarget(PARTY *pParty, int environment)
{
  if (m_targets == NULL)
  {
    int i;
    m_targets = new CHARACTER *[pParty->numCharacters];
    for (i=0; i<pParty->numCharacters; i++)
    {
      m_targets[i] = &pParty->GetCharacter(i, NULL);
    };
    m_numTarget = pParty->numCharacters;
  };
  while (m_numTarget > 0)
  {
    int i;
    CHARACTER *pChar;
    i = randomMT() % m_numTarget;
    pChar = m_targets[i];
    {
      CString ans;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetSpellContext(m_pSpell);
      scriptContext.SetCharacterContext(pChar);
      hookParameters[5] = (environment==0)?"ENCAMP":"TEMPLE";
      hookParameters[0] = (pChar->GetHitPoints() < pChar->GetMaxHitPoints())?"1":"";
      ans = m_pSpell->RunSpellScripts(FIX_CHARACTER,
                                      ScriptCallback_RunAllScripts,
                                      "",
                                      "Looking for random target");
      if (!ans.IsEmpty() && (ans != "0"))
      {
        m_activeTarget = pChar;
        return;
      };
    };
    m_targets[i] = m_targets[--m_numTarget];
  };
  m_activeTarget = NULL;
}



class FIX_SPELL_LIST
{
  FIX_SPELL_ENTRY *m_fixSpells;
  int m_numFixSpells;
  spellBookType *m_spellBook;
  int m_environment;
public:
  //int m_tcKey;
  CHARACTER_ID m_characterID;
  FIX_SPELL_LIST(spellBookType *spellBook,int environment)
  {
    m_spellBook = spellBook; 
    m_fixSpells = NULL;
    m_environment = environment;
    //m_tcKey = -1;
    m_characterID = CHARACTER_ID();
  };
  ~FIX_SPELL_LIST(void)
  {
    if (m_fixSpells != NULL)
    {
      delete[] m_fixSpells;
      m_fixSpells = NULL;
    };
  };
  int GetCount();
  FIX_SPELL_ENTRY *GetRandom(PARTY *pParty);
};


int FIX_SPELL_LIST::GetCount(void)
{
  if (m_fixSpells == NULL)
  {
    int i;
    //m_spellBook->SortSpells();
    //m_numFixSpells = m_spellBook->GetSortedCount();
    m_numFixSpells = m_spellBook->GetCount();
    m_fixSpells = new FIX_SPELL_ENTRY[m_numFixSpells];
    {
      for (i=0; i<m_numFixSpells; i++)
      {
        //m_fixSpells[i].m_pSpell = spellData.GetSpellData(m_spellBook->GetAtPos(pos).spell);
        m_fixSpells[i].m_pSpell = spellData.GetSpell(m_spellBook->PeekCharacterSpell(i)->spellID);
      };
    };
  };
  return m_numFixSpells;
}


extern CHARACTER NULL_CHAR;

FIX_SPELL_ENTRY *FIX_SPELL_LIST::GetRandom(PARTY *pParty)
{
  GetCount(); // Force initialization
  while (m_numFixSpells > 0)
  {
    int i;
    FIX_SPELL_ENTRY *fixSpellEntry;
    i = randomMT() % m_numFixSpells;
    fixSpellEntry = &m_fixSpells[i];
    if (m_environment == 0)
    {
      fixSpellEntry->RandomCaster(pParty);
    }
    else
    {




      //if (!globalData.charData.HaveKey(m_tcKey))
      if (!globalData.charData.HaveNPC(m_characterID))
      {
        int globalDataIndex;
        // create max level Cleric/MagicUser so any
        // spell can be cast.
        //

        //DWORD StartExp;

/*
        StartExp =  1+GetMinClericExpForLevel(29);
        StartExp += 1+GetMinMagicUserExpForLevel(29);
*/ /* Really */ NotImplemented(0xae41bc, false); // StartExp = 0;



        CHARACTER TempleBishop;
        TempleBishop.SetAlignment(ChaoticNeutral);
        //TempleBishop.race(Human);
        RACE_ID raceID;
        raceID = "Human";
        TempleBishop.RaceID(raceID);
        TempleBishop.SetGender(Bishop);
        CLASS_ID classID;
        classID = "Cleric/Magic User";
        //TempleBishop.SetClass(C_MU);
        TempleBishop.SetClass(classID);
        TempleBishop.SetType(NPC_TYPE);
        TempleBishop.SetName("@TempleBishop");
        TempleBishop.generateNewCharacter(0,0);  
        TempleBishop.SetPermStr(18);
        TempleBishop.SetPermInt(18);
        TempleBishop.SetPermWis(18);
        TempleBishop.SetPermDex(18);
        TempleBishop.SetPermCon(18);
        TempleBishop.SetPermCha(18);
        TempleBishop.CopySpellbook(*m_spellBook);//templeSpells);

        if (TempleBishop.GetMaxHitPoints() == 0)
          WriteDebugString("Default temple cleric not created correctly, might have bogus race/class data files\n");

        //m_tcKey = globalData.charData.AddCharacter(TempleBishop);
        globalDataIndex = globalData.charData.AddCharacter(&TempleBishop);
        CHARACTER *pTempleBishop = globalData.charData.GetCharacter(globalDataIndex);
        pTempleBishop->origIndex = globalDataIndex;
        m_characterID = TempleBishop.CharacterID();
      }

      //CHARACTER *pTempleBishop = globalData.charData.GetCharacterData(m_tcKey);
      CHARACTER *pTempleBishop = globalData.charData.GetCharacter(m_characterID);



      fixSpellEntry->m_activeCaster = pTempleBishop;
    };
    if (fixSpellEntry->m_activeCaster != NULL)
    {
      fixSpellEntry->RandomTarget(pParty, m_environment);
      if (fixSpellEntry->m_activeTarget != NULL)
      {
        return fixSpellEntry;
      }
    };
    m_fixSpells[i].Swap(&m_fixSpells[--m_numFixSpells]);
  };
  return NULL;
}
  


void PARTY::FixParty(int environment)
{
  FIX_SPELL_ENTRY *fixSpellEntry;
  FIX_SPELL_LIST fixSpellList(&globalData.fixSpellBook, environment);
  while (fixSpellList.GetCount() > 0)
  {
    fixSpellEntry = fixSpellList.GetRandom(this);
    if (fixSpellEntry == NULL) continue;
    WriteDebugString("%s is fixing %s\n",
                     fixSpellEntry->m_activeCaster->GetName(),
                     fixSpellEntry->m_activeTarget->GetName());
            fixSpellEntry->m_activeCaster->RemoveAllTargets(); // clear target data
            fixSpellEntry->m_activeCaster->targets.m_canTargetEnemy = fixSpellEntry->m_pSpell->CanTargetEnemy;
            fixSpellEntry->m_activeCaster->targets.m_canTargetFriend = fixSpellEntry->m_pSpell->CanTargetFriend;
            fixSpellEntry->m_activeCaster->AddTarget(*fixSpellEntry->m_activeTarget); // add recipient as target
            //fixSpellEntry->m_activeCaster->CastSpell(fixSpellEntry->m_pSpell->m_gsID, false); // invoke spell on target
            fixSpellEntry->m_activeCaster->CastSpell(fixSpellEntry->m_pSpell->SpellID(), false, false); // invoke spell on target
  };
  //if (fixSpellList.m_tcKey >= 0)
  if (fixSpellList.m_characterID.IsValidNPC())
  {
    //globalData.charData.RemoveCharacter(fixSpellList.m_tcKey);
    globalData.charData.RemoveCharacter(fixSpellList.m_characterID);
  };
}

DWORD PARTY::CalcRestTime(void)
{
  // figure out which party member has the longest rest
  // period required for memorizing the selected spells
  DWORD max=0;
  for (int i=0;i<numCharacters;i++)
  {
    DWORD t = characters[i].CalcRestTime();
    if (t > max) 
      max = t;
  }
  return max;
}

void PARTY::RemoveSpellEffect(DWORD parent, const SPELL_DATA *pSpell, bool endSpell)
{
  for (int i=0;i<numCharacters;i++)
    characters[i].RemoveSpellEffect(parent, pSpell, endSpell);
}


// These four declarations removed from 'ProcessTimeSensitiveData so
// that they can be seen by the debugger.
  static int lastDay=0;
  static LONGLONG lastUpdateTime=-1;
//  static int minutesRested=0;
//  static int minutesUntilAutoHeal=1440; // 24 hours
  static DWORD minuteTotal=0;
  static LONGLONG currGameTime=0;


void PARTY::BeginResting(void)
{
  int i;
  for (i=0;i<numCharacters;i++)
  {
    // The very first time we get here when beginning to rest
    CString result;
    int iResult;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetCharacterContext(&characters[i]);
    //currRaceName =  raceData.GetRaceName(dude.race());
    result = characters[i].RunCharacterScripts(
                                BEGIN_RESTING,
                                ScriptCallback_RunAllScripts, 
                                NULL,
                                "Character begins resting");
  // If party is resting and characters are unconscious (from battle, traps, etc.)
  // wake that character up.
    iResult = atoi(result);
    if (iResult == 0)
    {
      if (characters[i].GetStatus() == Unconscious)
      {
		    characters[i].SetHitPoints(1);
	      characters[i].SetStatus(Okay);
      };
    };
  };
}

// this function is called each time GameEvent::OnCycle() is called,
// which basically amounts to all of the time, regardless of
// whether the player provides input.
bool PARTY::ProcessTimeSensitiveData(LONGLONG currTime)
{
  bool UpdateScreen = false;
  

  // special flag that tells us to reset these values,
  // such as when loading a saved game.
  if (currTime <= 0)
  {
    lastDay=0;
    lastUpdateTime=-1;
    minutesRested=0;
#ifdef OLD_AUTO_HEAL
    minutesUntilAutoHeal=1440; // 24 hours
#endif
    minuteTotal=0;
    currGameTime=0;
    DayMusic=-1;
    return false;
  }

  // update once per minute (game minutes, NOT computer clock minutes)
  currGameTime = getElapsedMinutes();//get24HourClock(); // hours:mins (no days)
  if (currGameTime == 0) currGameTime=2400; 
  if (lastUpdateTime != currGameTime)
  {     
    // NewDay does not mean that 24 hours have passed, it only
    // means that the party time clock has incremented to the
    // next day
    bool NewDay=false;
    if (lastDay != days) // days is member of PARTY
    {
      NewDay=true;
      lastDay=days;
    }

    if (lastUpdateTime != -1)
    {
      if (currGameTime < lastUpdateTime) 
      {
        die(0x661bc);  // What on earth is this about?
        currGameTime+=2400;
      };
      //int gmins = (currGameTime%100) + ((currGameTime/100)*60);
      //int lmins = (lastUpdateTime%100) + ((lastUpdateTime/100)*60);
      //int inc = gmins-lmins;      
      int inc = currGameTime-lastUpdateTime;
      inc = max(0,inc);

      if (inc > 0)
      {
        minuteTotal += inc;
#ifdef OLD_AUTO_HEAL
        minutesUntilAutoHeal -= inc;
#endif
        
        if (resting) // set to TRUE when 'Rest' menu option is used to pass time
          minutesRested += inc;
        else
          minutesRested=0;

        UpdatePartyDrinkPoints();
        activeSpellList.ProcessTimeSensitiveData(inc);
        //PendingSpellData.ProcessTimeSensitiveData(inc); // not needed?

        CheckAndPlayBackgroundMusic(DayMusic);

        int i;
        if (resting)
        {
          for (int mt=0; mt<inc; mt++)
          {
            for (i=0;i<numCharacters;i++)
            {
              if (characters[i].CanCastSpells())
              {
                if (characters[i].SpellsKnown() > 0)
                {
                  // add time delta to all spells being memorized
                  // returns TRUE if a spell was memorized
                  if (characters[i].IncAllMemorizedTime(1, false))
                  {
                    // find newly memorized spell and announce it
                    //POSITION p = characters[i].GetFirstSpellPosition(); // access ordered list
                    int j, n;
                    n = characters[i].GetSpellCount();
                    //while (p != NULL)
                    for (j=0; j<n; j++)
                    {
                      if (characters[i].PeekCharacterSpell(j)->JustMemorized)
                      {
                        characters[i].GetCharacterSpell(j)->JustMemorized=FALSE;
                        CString tmp;
                        tmp.Format("%s memorizes %s", 
                                    characters[i].GetName(),
                                    //spellData.GetSpellName(characters[i].GetSpellAt(p).spell));
                                    spellData.GetSpellName(characters[i].PeekCharacterSpell(j)->spellID));
                        FormatPausedText(pausedTextData, tmp);
                        DisplayPausedText(pausedTextData, whiteColor, 0);
                      }
                      //characters[i].GetNextSpell(p);
                    }
                  }
                  else
                  {
                    ClearFormattedText(pausedTextData);
                  }
                }
              }
            }
          }
        }

        for (i=0;i<numCharacters;i++)
        {
          characters[i].ClearQueuedSpecAb();
          characters[i].CheckForExpiredSpellEffects();

          if (NewDay)
          {
            characters[i].myItems.ResetItemCharges(TRUE);
            characters[i].HasLayedOnHandsToday=FALSE;
          };
          if (resting && (lastUpdateTime == -1))
          {
            // The very firt time we get here when beginning to rest
            CString result;
            int iResult;
            HOOK_PARAMETERS hookParameters;
            SCRIPT_CONTEXT scriptContext;
            scriptContext.SetCharacterContext(&characters[i]);
            //currRaceName =  raceData.GetRaceName(dude.race());
            result = characters[i].RunCharacterScripts(
                                        BEGIN_RESTING,
                                        ScriptCallback_RunAllScripts, 
                                        NULL,
                                        "Character begins resting");
		        // If party is resting and characters are unconscious (from battle, traps, etc.)
		        // wake that character up.
            iResult = atoi(result);
            if (iResult == 0)
            {
		          if (characters[i].GetStatus() == Unconscious)
              {
  			        characters[i].SetHitPoints(1);
			          characters[i].SetStatus(Okay);
		          };
            };
          };
/*
          if (characters[i].GetStatus() == Okay)
          {
            if (characters[i].GetAdjSpecAb(SA_Poisoned))
            {
              double rateofloss = 0.25;
              BOOL IsSlowed = characters[i].GetAdjSpecAb(SA_SlowPoison);
              if (IsSlowed) rateofloss = 0.12;
              
              double currhp = characters[i].GetHitPoints();
              double hploss = currhp * rateofloss;
              
              hploss += 0.5; // round up
              hploss = floor(hploss);              
              if (hploss < 1.0) hploss = 1.0;

              int newhptotal = currhp - hploss;
              
              if ((IsSlowed) && (newhptotal < 1))
                newhptotal = 1;

              characters[i].SetHitPoints( newhptotal );
              
              if (newhptotal <= 0)
                characters[i].SetStatus(Dead);

              if (newhptotal != currhp)
                PlayCharHit();

              UpdateScreen=true;
            };
          };
*/
        };
      };
    };

    //if ((minuteTotal>0) && (NewDay)) // 24 hours
    //if ((minuteTotal>0) && ((minuteTotal%1440)==0))


      //  According to Eric and Tom we should add a hit point for every
      //  1440 minutes of unbroken rest.  No need for this minutesUntilAutoHeal.
#ifdef OLD_AUTO_HEAL
    if (minutesUntilAutoHeal <= 0)
    {      
      // the subtraction above may have taken it below
      // zero, and the difference figures into the new
      // time remaining
      minutesUntilAutoHeal = 1440 - abs(minutesUntilAutoHeal);
      int hp = ((minutesRested>=1440)?3:1);
      minutesRested=0;
      for (int i=0;i<numCharacters;i++)
      {
        if (characters[i].IsAlive())
        {
          int currhp=characters[i].GetHitPoints();
          characters[i].SetHitPoints(currhp+hp);
          UpdateScreen=true;
        }
      }
    };
#else  //if !OLD_AUTO_HEAL
    {
      if (minutesRested >= 1440)
      {
        minutesRested -= 1440;
        for (int i=0;i<numCharacters;i++)
        {
          if (characters[i].IsAlive())
          {
            int hp;
            int currhp;
            if (characters[i].GetStatus() != Unconscious)
            {
              hp = 1;
              currhp = characters[i].GetHitPoints();
              characters[i].SetHitPoints(currhp+hp);
              UpdateScreen=true;
            };
          }
        }
      };
    };
#endif //OLD_AUTO_HEAL
    lastUpdateTime=getElapsedMinutes();//get24HourClock(); // hours:mins (no days)
  };
  return UpdateScreen;
}

#endif // ifdef UAFEngine

void InsertPartyASL(
       PARTY& data,
       const CString& key,
       const CString& value,
       int flags)
{
  data.party_asl.Insert(key, value, flags);
}

void InsertCharacterASL(
       PARTY& data,
       int memberNumber,
       const CString& key,
       const CString& value,
       int flags)
{
  data.characters[memberNumber].char_asl.Insert(key, value, flags);
}

void InsertCharacterASL(
       CHARACTER *pChar,
       const CString& key,
       const CString& value,
       int flags)
{
  pChar->char_asl.Insert(key, value, flags);
}

void DeletePartyASL(PARTY& data, const CString& key)
{
  data.party_asl.Delete(key);
}

CString LookupPartyASL(
           PARTY& data,
           const CString& key)
{
  return data.party_asl.Lookup(key);
}

CString LookupCharacterASL(
           PARTY& data,
           int memberNumber,
           const CString& key)
{
  return data.characters[memberNumber].char_asl.Lookup(key);
}

CString LookupCharacterASL(
           CHARACTER *pChar,
           const CString& key)
{
  return pChar->char_asl.Lookup(key);
}



BOOL ExistsPartyASL(
           PARTY& data,
           const CString& key)
{
  return data.party_asl.Find(key) != NULL;
}

genderType GetCharacterGender(PARTY& data, int memberNumber)
{
  return data.characters[memberNumber].GetAdjGender();
}

void SetCharacterGender(PARTY& data, int memberNumber, const CString& gender)
{
  data.characters[memberNumber].SetGender(gender);
}

CString GetCharacterName(PARTY& data, int memberNumber)
{
  return data.characters[memberNumber].GetName();
}

int GetPartySize(PARTY& data)
{
  return data.numCharacters;
}

//*****************************************************************************

//*****************************************************************************
//    NAME: VISIT_DATA::VISIT_DATA
//
// PURPOSE: 
//
//*****************************************************************************
VISIT_DATA::VISIT_DATA()
{ 
  for (int i=0;i<MAX_LEVELS;i++)
    visited[i] = NULL;

  Clear(); 
}

//*****************************************************************************
//    NAME: ~VISIT_DATA::VISIT_DATA
//
// PURPOSE: 
//
//*****************************************************************************
VISIT_DATA::~VISIT_DATA() 
{ 
  for (int i=0;i<MAX_LEVELS;i++)
  {
    if (visited[i] != NULL) 
      delete visited[i];
    visited[i]=NULL; 
  }
}

//*****************************************************************************
//    NAME: VISIT_DATA::operator=
//
// PURPOSE: 
//
//*****************************************************************************
VISIT_DATA& VISIT_DATA::operator=(const VISIT_DATA& src) 
{ 
  if (&src == this) return *this;
  
  Clear();

  for (int i=0;i<MAX_LEVELS;i++)
  {
    if (src.visited[i] != NULL)
    {
      visited[i] = new TAG_LIST_2D(src.visited[i]->GetWidth(), 
                                   src.visited[i]->GetHeight());

      *visited[i] = *src.visited[i];
    }
  }
  
  return *this; 
}

//*****************************************************************************
//    NAME: VISIT_DATA::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void VISIT_DATA::Clear()
{ 
  for (int i=0;i<MAX_LEVELS;i++)
  {
    if (visited[i] != NULL) 
      delete visited[i];

    visited[i] = NULL;
  }

  //visited = new TAG_LIST_2D(MAX_AREA_WIDTH, MAX_AREA_HEIGHT);
}
//*****************************************************************************
//    NAME: VISIT_DATA::IsVisited
//
// PURPOSE: 
//
//*****************************************************************************
BOOL VISIT_DATA::IsVisited(int level, int x, int y) 
{ 
  if ((level < 0) || (level >= MAX_LEVELS)) return FALSE;
  
  if (visited[level] == NULL)
    return FALSE;

  return (visited[level]->Get(x, y));
}
//*****************************************************************************
//    NAME: VISIT_DATA::SetVisited
//
// PURPOSE: 
//
//*****************************************************************************
void VISIT_DATA::SetVisited(int level, int x, int y) 
{ 
  if ((level < 0) || (level >= MAX_LEVELS)) return;
  
  if (visited[level] == NULL)
    visited[level] = new TAG_LIST_2D(MAX_AREA_WIDTH, MAX_AREA_HEIGHT);

  visited[level]->Set(x, y);
}

//*****************************************************************************
// NAME:    VISIT_DATA::Serialize
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void VISIT_DATA::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);

  if (ar.IsStoring())
  {
    CString DataBlockTag("VISIT_DATA"); 
    AS(ar, DataBlockTag);

    for (int i=0;i<MAX_LEVELS;i++)
    {
      ar << i; // which level this info is for      

      if (visited[i] != NULL)
      {
        LPBYTE bytes = visited[i]->GetBytes();  
        ASSERT( bytes != NULL );
        if (bytes == NULL) continue;

        ar << visited[i]->GetByteSize(); // how many bytes in data block
        ar.Write(bytes, visited[i]->GetByteSize()); // write the data block
      }
      else
      {
        ar << 0; // zero data bytes for this level
      }
    }
  }
  else
  {
    Clear();
    
    if (globalData.SaveGameVersion < _VERSION_0911_)
    {
      // In older versions only the information for a single level
      // was stored (bug). The data block was always allocated and had
      // a fixed size.       
      
      visited[0] = new TAG_LIST_2D(MAX_AREA_WIDTH, MAX_AREA_HEIGHT);
      
      // determine number of bytes in data block
      int count = visited[0]->GetByteSize();

      // get a pointer to the data block (only 1 in this case)
      LPBYTE bytes = visited[0]->GetBytes();
      ASSERT( bytes != NULL );
      if (bytes == NULL) return;      

      ar.Read(bytes, count);
    }
    else
    {      
      int level;
      int count;

      // sanity check, make sure we are located at the correct
      // offset in the data file
      CString DataBlockTag; 
      DAS(ar, DataBlockTag);
      ASSERT( DataBlockTag.Compare("VISIT_DATA") == 0 );

      for (int i=0;i<MAX_LEVELS;i++)
      {
        ar >> level;
        ar >> count;

        if (count > 0)
        {
          visited[level] = new TAG_LIST_2D(MAX_AREA_WIDTH, MAX_AREA_HEIGHT);
          ASSERT( visited[level]->GetByteSize() == count );

          LPBYTE pbytes = visited[level]->GetBytes();
          ASSERT( pbytes != NULL );
          if (pbytes == NULL) continue; 

          ar.Read(pbytes, count);
        }
      }
    }
  }
}

void VISIT_DATA::Serialize(CAR &car)
{
//  CObject::Serialize(ar);

  if (car.IsStoring())
  {
    CString DataBlockTag("VISIT_DATA"); 
    AS(car, DataBlockTag);

    for (int i=0;i<MAX_LEVELS;i++)
    {
      car << i; // which level this info is for      

      if (visited[i] != NULL)
      {
        LPBYTE bytes = visited[i]->GetBytes();  
        ASSERT( bytes != NULL );
        if (bytes == NULL) continue;

        car << visited[i]->GetByteSize(); // how many bytes in data block
        //car.Write(bytes, visited[i]->GetByteSize()); // write the data block
        car.Serialize((char *)bytes, visited[i]->GetByteSize()); // write the data block
      }
      else
      {
        car << 0; // zero data bytes for this level
      }
    }
  }
  else
  {
    Clear();
    
    if (globalData.SaveGameVersion < _VERSION_0911_)
    {
      // In older versions only the information for a single level
      // was stored (bug). The data block was always allocated and had
      // a fixed size.       
      
      visited[0] = new TAG_LIST_2D(MAX_AREA_WIDTH, MAX_AREA_HEIGHT);
      
      // determine number of bytes in data block
      int count = visited[0]->GetByteSize();

      // get a pointer to the data block (only 1 in this case)
      LPBYTE bytes = visited[0]->GetBytes();
      ASSERT( bytes != NULL );
      if (bytes == NULL) return;      

      //car.Read(bytes, count);
      car.Serialize((char *)bytes, count);
    }
    else
    {      
      int level;
      int count;

      // sanity check, make sure we are located at the correct
      // offset in the data file
      CString DataBlockTag; 
      DAS(car, DataBlockTag);
      ASSERT( DataBlockTag.Compare("VISIT_DATA") == 0 );

      for (int i=0;i<MAX_LEVELS;i++)
      {
        car >> level;
        car >> count;

        if (count > 0)
        {
          visited[level] = new TAG_LIST_2D(MAX_AREA_WIDTH, MAX_AREA_HEIGHT);
          ASSERT( visited[level]->GetByteSize() == count );

          LPBYTE pbytes = visited[level]->GetBytes();
          ASSERT( pbytes != NULL );
          if (pbytes == NULL) continue; 

          //car.Read(pbytes, count);
          car.Serialize((char *)pbytes, count);
        }
      }
    }
  }
}

//*****************************************************************************

void STEP_COUNTER::Clear() 
{ 
  for (int i=0;i<MAX_ZONES;i++) 
    stepCount[i]=0; 
}
STEP_COUNTER& STEP_COUNTER::operator =(const STEP_COUNTER& src) 
{ 
  if (&src == this)
    return *this;
  for (int i=0;i<MAX_ZONES; i++)
    stepCount[i] = src.stepCount[i];
  return *this;
}

STEP_COUNTER::STEP_COUNTER(const STEP_COUNTER& src)
{
  for (int i=0;i<MAX_ZONES; i++)
    stepCount[i] = src.stepCount[i];
}

//*****************************************************************************

TRIGGER_FLAGS& TRIGGER_FLAGS::operator =(const TRIGGER_FLAGS& src) 
{ 
  if (&src == this)
    return *this;
  eventResult=src.eventResult; 
  return *this; 
}
TRIGGER_FLAGS::TRIGGER_FLAGS(const TRIGGER_FLAGS& src)
{ 
  *this = src; 
}
void TRIGGER_FLAGS::Serialize(CArchive &ar)
{
//  CObject::Serialize(ar);
  if (ar.IsStoring())
  {
    ar << (int)eventStatusUnused;
    ar << (int)eventResult;
  }
  else
  {
    int temp;
    ar >> temp;
    eventStatusUnused = temp;
    ar >> temp;
    eventResult = (eventResultType)temp;
  }
}

void TRIGGER_FLAGS::Serialize(CAR &car)
{
//  CObject::Serialize(ar);
  if (car.IsStoring())
  {
    car << (int)eventStatusUnused;
    car << (int)eventResult;
  }
  else
  {
    int temp;
    car >> temp;
    eventStatusUnused = temp;
    car >> temp;
    eventResult = (eventResultType)temp;
  }
}

//*****************************************************************************

LEVEL_FLAG_DATA& LEVEL_FLAG_DATA::operator =(const LEVEL_FLAG_DATA& src) 
{  
  if (&src == this)
    return *this;
  stepData = src.stepData;
  flagData = src.flagData;
  flags=src.flags;
  return *this; 
}
LEVEL_FLAG_DATA::LEVEL_FLAG_DATA(const LEVEL_FLAG_DATA& src) 
{ 
  *this = src; 
}

void LEVEL_FLAG_DATA::IncZoneStepCount(int zone) 
{ 
  (stepData.stepCount[zone])++; 
}
unsigned long LEVEL_FLAG_DATA::GetZoneStepCount(int zone) 
{ 
  return (stepData.stepCount[zone]); 
}
BOOL LEVEL_FLAG_DATA::HasEventHappened(DWORD event)
{
  if (flagData.Find(flags, event)) return flags.HasEventHappened();
  else return FALSE;
}

void LEVEL_FLAG_DATA::markEventHappened(DWORD event)
{
  if (flagData.Find(flags, event))
  {
    flags.markEventHappened();
    flagData.Update(flags, event);
  }
  else
  {
    flags.markEventHappened();
    flagData.Insert(flags, event);
  }
}

//*****************************************************************************

EVENT_TRIGGER_DATA& EVENT_TRIGGER_DATA::operator =(const EVENT_TRIGGER_DATA& src)
{
  if (&src == this)
    return *this;
  Clear();
  data.SetSize(src.data.GetSize());
  for (int i=0; i<src.data.GetSize(); i++)
    data[i] = src.data[i];
  return *this;
}
EVENT_TRIGGER_DATA::EVENT_TRIGGER_DATA(const EVENT_TRIGGER_DATA& src) 
{ 
  *this = src; 
}

void EVENT_TRIGGER_DATA::Clear()
{
  for (int i=0;i<data.GetSize(); i++)
    data[i].Clear();
}

BOOL EVENT_TRIGGER_DATA::CheckLevel(int level)
{
  if (level < 0)
    return FALSE;

  if (level >= data.GetSize())
  {
    data.SetSize(level+1);
    LEVEL_FLAG_DATA temp;
    data[level]=temp;
  }
  return TRUE;
}

void EVENT_TRIGGER_DATA::IncZoneStepCount(int level, int zone) 
{ 
  if (CheckLevel(level))
    data[level].IncZoneStepCount(zone);
}

unsigned long EVENT_TRIGGER_DATA::GetZoneStepCount(int level, int zone) 
{ 
  if (CheckLevel(level))
    return data[level].GetZoneStepCount(zone);
  else
    return 0;
}
void EVENT_TRIGGER_DATA::MarkEventHappened(int level, DWORD event) 
{
  if (CheckLevel(level))
    data[level].markEventHappened(event);
}
BOOL EVENT_TRIGGER_DATA::HasEventHappened(int level, DWORD event)
{
  if (CheckLevel(level))
    return data[level].HasEventHappened(event);
  else
    return FALSE;
}

//*****************************************************************************

int PARTY::NumNPCs()
{
  int numNPC = 0;
  for (int i=0;i<numCharacters; i++)
  {
    if (characters[i].GetType() == NPC_TYPE)
      numNPC++;
  }
  return numNPC;
}

#ifdef UAFEngine

BOOL PARTY::CanAddNPC()
{
  return (numCharacters < GetMaxPartyMembers());
}
#endif

void PARTY::DecCharacterOrder(void)
{
  if (numCharacters <= 1) return;

  CHARACTER tempCHARACTER;
  int src = activeCharacter;
  int dst;
  
  if (activeCharacter > 0)
  {
    // if not already in first slot
    dst = activeCharacter-1;

    // swap active with active-1
    tempCHARACTER = characters[dst];
    characters[dst] = characters[src];
    characters[src] = tempCHARACTER;
  }
  else
  {
    // already in first slot.
    // move all characters up 1 and
    // put active into slot numCharacters-1
    dst = numCharacters-1;
    // save current
    tempCHARACTER = characters[src];
    // now move everybody up 1
    for (int i=0;i<numCharacters-1;i++)
      characters[i] = characters[i+1];
    characters[dst] = tempCHARACTER;
  }
  activeCharacter = dst;
}

void PARTY::IncCharacterOrder(void)
{
  if (numCharacters <= 1) return;
  CHARACTER tempCHARACTER;
  int src = activeCharacter;
  int dst;
  
  if (activeCharacter >= numCharacters-1)
  {
    // already in last slot.
    // move all characters down 1 and
    // put active into slot 0
    dst = 0;
    // save current
    tempCHARACTER = characters[src];
    // now move everybody down 1
    // start from end and move backwards
    for (int i=numCharacters-1;i>0;i--)
      characters[i] = characters[i-1];
    characters[dst] = tempCHARACTER;
  }
  else
  {
    // swap active with active+1
    dst = activeCharacter+1;    
    tempCHARACTER = characters[dst];
    characters[dst] = characters[src];
    characters[src] = tempCHARACTER;
  }
  activeCharacter = dst;
}

//   NoPartyMember, EntireParty, ActiveChar,
//   OneAtRandom, ChanceOnEach 
void PARTY::HealParty(HEAL_PARTY_DATA &data)
{
  int rndDude = RollDice(numCharacters, 1) - 1;
  switch (data.who)
  {
  case NoPartyMember:
    return;
  case EntireParty:
    if (data.HealHP)
    {
      for (int i=0;i<numCharacters;i++)
      {
        int currHp = characters[i].GetHitPoints();
        int totalHp;

        if (data.LiteralOrPercent==0) // add literal
        {
          totalHp = currHp+data.HowMuchHP;
        }
        else if (data.LiteralOrPercent==1)// add percent of max
        {
          double maxHp = characters[i].GetMaxHitPoints();
          maxHp *= ((double)data.HowMuchHP * 0.01);
          totalHp = currHp+maxHp;
        }
        else if (data.LiteralOrPercent==2)// set to percent of max
        {
          double maxHp = characters[i].GetMaxHitPoints();
          maxHp *= ((double)data.HowMuchHP * 0.01);
          totalHp = maxHp;
          if (totalHp < currHp) totalHp=currHp; // no change
        }
        else
          return;

        characters[i].SetHitPoints(totalHp);
        if (characters[i].GetHitPoints() > 0)
          characters[i].SetStatus(Okay);
      }
    }

    if (data.HealCurse)
    {
      for (int i=0;i<numCharacters;i++)
      {        
        POSITION pos = characters[i].myItems.GetHeadPosition();
        while (pos != NULL)
        {
          if (characters[i].myItems.PeekAtPos(pos).cursed)
            characters[i].myItems.GetAtPos(pos).cursed = FALSE;
          characters[i].myItems.PeekNext(pos);
        }
      }
    }
    if (data.HealDrain)
    {
      WriteDebugString("Heal Drain not coded yet\n");
    }
    break;
  case ActiveChar:
    if (data.HealHP)
    {
      int currHp = characters[activeCharacter].GetHitPoints();
      int totalHp;

      if (data.LiteralOrPercent==0) // literal
      {
        totalHp = currHp+data.HowMuchHP;
      }
      else if (data.LiteralOrPercent==1)// percent of max
      {
        double maxHp = characters[activeCharacter].GetMaxHitPoints();
        maxHp *= ((double)data.HowMuchHP * 0.01);
        totalHp = currHp+maxHp;
      }
      else if (data.LiteralOrPercent==2)// set to percent of max
      {
        double maxHp = characters[activeCharacter].GetMaxHitPoints();
        maxHp *= ((double)data.HowMuchHP * 0.01);
        totalHp = maxHp;
        if (totalHp < currHp) totalHp=currHp; // no change
      }
      else
        return;
      
      characters[activeCharacter].SetHitPoints(totalHp);
      if (characters[activeCharacter].GetHitPoints() > 0)
        characters[activeCharacter].SetStatus(Okay);
    }

    if (data.HealCurse)
    {
      POSITION pos = characters[activeCharacter].myItems.GetHeadPosition();
      while (pos != NULL)
      {
        if (characters[activeCharacter].myItems.PeekAtPos(pos).cursed)
          characters[activeCharacter].myItems.GetAtPos(pos).cursed = FALSE;
        characters[activeCharacter].myItems.PeekNext(pos);
      }
    }
    if (data.HealDrain)
    {
      WriteDebugString("Heal Drain not coded yet\n");
    }
    break;
  case OneAtRandom:
    if (data.HealHP)
    {
      int currHp = characters[rndDude].GetHitPoints();
      int totalHp;

      if (data.LiteralOrPercent==0) // literal
      {
        totalHp = currHp+data.HowMuchHP;
      }
      else if (data.LiteralOrPercent==1)// percent of max
      {
        double maxHp = characters[rndDude].GetMaxHitPoints();
        maxHp *= ((double)data.HowMuchHP * 0.01);
        totalHp = currHp+maxHp;
      }
      else if (data.LiteralOrPercent==2)// set to percent of max
      {
        double maxHp = characters[rndDude].GetMaxHitPoints();
        maxHp *= ((double)data.HowMuchHP * 0.01);
        totalHp = maxHp;
        if (totalHp < currHp) totalHp=currHp; // no change
      }
      else
        return;

      characters[rndDude].SetHitPoints(totalHp);
      if (characters[rndDude].GetHitPoints() > 0)
        characters[rndDude].SetStatus(Okay);
    }

    if (data.HealCurse)
    {
      POSITION pos = characters[rndDude].myItems.GetHeadPosition();
      while (pos != NULL)
      {
        if (characters[rndDude].myItems.PeekAtPos(pos).cursed)
          characters[rndDude].myItems.GetAtPos(pos).cursed = FALSE;
        characters[rndDude].myItems.PeekNext(pos);
      }
    }
    if (data.HealDrain)
    {
      WriteDebugString("Heal Drain not coded yet\n");
    }
    break;
  case ChanceOnEach:
    if (data.HealHP)
    {
      for (int i=0;i<numCharacters;i++)
      {
        if (RollDice(100, 1, 0) <= data.chance)
        {
          int currHp = characters[i].GetHitPoints();
          int totalHp;

          if (data.LiteralOrPercent==0) // add literal
          {
            totalHp = currHp+data.HowMuchHP;
          }
          else if (data.LiteralOrPercent==1)// add percent of max
          {
            double maxHp = characters[i].GetMaxHitPoints();
            maxHp *= ((double)data.HowMuchHP * 0.01);
            totalHp = currHp+maxHp;
          }
          else if (data.LiteralOrPercent==2)// set to percent of max
          {
            double maxHp = characters[i].GetMaxHitPoints();
            maxHp *= ((double)data.HowMuchHP * 0.01);
            totalHp = maxHp;
            if (totalHp < currHp) totalHp=currHp; // no change
          }
          else
            return;

          characters[i].SetHitPoints(totalHp);
          if (characters[i].GetHitPoints() > 0)
            characters[i].SetStatus(Okay);
        }
      }
    }

    if (data.HealCurse)
    {
      for (int i=0;i<numCharacters;i++)
      {        
        if (RollDice(100, 1, 0) <= data.chance)
        {
          POSITION pos = characters[i].myItems.GetHeadPosition();
          while (pos != NULL)
          {
            if (characters[i].myItems.PeekAtPos(pos).cursed)
              characters[i].myItems.GetAtPos(pos).cursed = FALSE;
            characters[i].myItems.PeekNext(pos);
          }
        }
      }
    }
    if (data.HealDrain)
    {
      WriteDebugString("Heal Drain not coded yet\n");
    }
    break;
  }
}
