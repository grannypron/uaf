/******************************************************************************
* Filename: class.cpp
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

#ifdef UAFEDITOR
//#include "..\UAFWinEd\UAFWinEd.h"
#else
//#include "externs.h"
#include "..\UAFWin\Dungeon.h"
#endif

#include "char.h"
#include "level.h"
#include "class.h"
#include "ConfigFile.h"
#ifdef UAFEngine
#include "..\UAFWIN\DrawTile.h"
#include "..\UAFWin\combatants.h"
#endif
#include "GlobalData.h"
#include "Specab.h"

#include "GPDLOpCodes.h"
#include "GPDLComp.h"
#include "GPDLExec.h"
#ifdef UAFEDITOR
#include "Monster.h"
#endif
//#include "RuntimeIf.h"

void *My_malloc(int size);
void My_free(void *addr);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef UAFEDITOR
BYTE GetClericBonusLevel(int prime);
BYTE GetClericBonusNum(int prime);

extern BYTE MaxFighterSpellsForEachSpellLvlByPrime[19];
extern BYTE MaxThiefSpellsForEachSpellLvlByPrime[19];
extern BYTE MaxClericSpellsForEachSpellLvlByPrime[19];
extern BYTE MaxDruidSpellsForEachSpellLvlByPrime[19];
extern BYTE MaxPaladinSpellsForEachSpellLvlByPrime[19];
extern BYTE MaxRangerSpellsForEachSpellLvlByPrime[19];
extern BYTE MaxMuSpellsForEachSpellLvlByPrime[19];

extern BYTE MaxSpellLevel[MAX_BASE_CLASSES][MAX_SPELL_SCHOOL][19];

void CopyClericSpellLevels(BYTE *limits, const char *school);
void CopyDruidSpellLevels(BYTE *limits, const char *school);
void CopyPaladinSpellLevels(BYTE *limits, const char *school);
void CopyRangerSpellLevels(BYTE *limits, const char *school);
void CopyMuSpellLevels(BYTE *limits, const char *school);

#endif

extern DICEDATA FighterHitDiceTable[];
extern DICEDATA ThiefHitDiceTable[];
extern DICEDATA ClericHitDiceTable[];
extern DICEDATA DruidHitDiceTable[];
extern DICEDATA PaladinHitDiceTable[];
extern DICEDATA RangerHitDiceTable[];
extern DICEDATA MagicUserHitDiceTable[];

extern const CString Skill_PickPockets;
extern const CString Skill_OpenLocks;
extern const CString Skill_FindTraps;
extern const CString Skill_MoveSilent;
extern const CString Skill_HideInShadows;
extern const CString Skill_HearNoise;
extern const CString Skill_ClimbWalls;
extern const CString Skill_ReadLanguages;
extern const CString Skill_Turn;
extern const CString Skill_BackstabMultiplier;
extern const CString Skill_RangerBonusLevel;

extern const CString Ability_Dexterity;
extern const CString Ability_Constitution;
extern const CString Ability_Strength;
extern const CString Ability_Intelligence;
extern const CString Ability_Charisma;
extern const CString Ability_Wisdom;

extern const CString Baseclass_magicUser;
extern const CString Baseclass_thief;
extern const CString Baseclass_paladin;
extern const CString Baseclass_cleric;
extern const CString Baseclass_fighter;
extern const CString Baseclass_ranger;
extern const CString Baseclass_druid;

extern const CString School_Magic_User;
extern const CString School_Cleric;

extern const CString Save_Vs_PPDM;
extern const CString Save_Vs_PP;
extern const CString Save_Vs_RSW;
extern const CString Save_Vs_BR;
extern const CString Save_Vs_SP;

extern const double VersionSpellNames;
extern const double VersionSpellIDs;


extern CString emptyString;
/*
RACE_DATA_TYPE       raceData;
BASE_CLASS_DATA_TYPE baseclassData;
CLASS_DATA_TYPE      classData;
ABILITY_DATA_TYPE    abilityData;
TRAIT_DATA_TYPE      traitData;
SPELLGROUP_DATA_TYPE spellgroupData;
*/
//STRINGINDEX stringIndex;

void loadUADefaults(RACE_DATA_TYPE& data)
{
  data.LoadUADefaults();
}

void clearData(RACE_DATA_TYPE& data)
{
  data.Clear();
}

int GetCount(RACE_DATA_TYPE& data)
{
  return data.GetCount();
}

BOOL saveData(RACE_DATA_TYPE& data)
{
//  BOOL success = FALSE;
  EditorStatusMsg("Saving race data...");

#ifdef SIMPLE_STRUCTURE
  CString fullPath;
  fullPath = rte.DataDir() + RACE_DB_NAME;
#else
  char fullPath[_MAX_PATH+1];
  GetDesignPath(fullPath);
  strcat(fullPath, RACE_DB_NAME);
#endif
  return saveData(data, fullPath);
}


int loadData(RACE_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  if (!myFile.Open(fullPath, CFile::modeRead, &e))
  {
    WriteDebugString("Unable to open race db file %s, error %i\n", fullPath, e.m_cause);
    return 0;
  }

  data.Clear();
  CAR car(&myFile, CArchive::load);
  //if (globalData.version >= _SPECIAL_ABILITIES_VERSION_)
  //{
  //  car.Compress(true);
  //};

  try
  {
    data.PreSerialize(car.IsStoring());
    if (data.Serialize(car)!=0)
    {
      data.Clear();
      car.Close();
      return 0;
    };
    data.PostSerialize(car.IsStoring());
  }
  catch (...)
  {
      WriteDebugString("Unable to load race data file %s\n", car.GetFile()->GetFileName());
      data.Clear();
  }

  car.Close();

  if (data.GetCount() == 0)
     WriteDebugString("No races in race db file\n");

  return data.GetCount();
}



BOOL saveData(RACE_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open race db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }

  CAR car(&myFile, CArchive::store);
  // Done later car.Compress(true);
  data.PreSerialize(car.IsStoring());
  data.Serialize(car);
  data.PostSerialize(car.IsStoring());
  car.Close();
  return TRUE;
}

int GetDataSize(RACE_DATA_TYPE& data)
{
  return data.GetDataSize();
}


int GetCount(ABILITY_DATA_TYPE& data)
{
  return data.GetCount();
}

int GetDataSize(ABILITY_DATA_TYPE& data)
{
  return data.GetDataSize();
}

BOOL saveData(ABILITY_DATA_TYPE& data)
{
//  BOOL success = FALSE;
  EditorStatusMsg("Saving ability data...");

#ifdef SIMPLE_STRUCTURE
  CString fullPath;
  fullPath = rte.DataDir() + ABILITY_DB_NAME;
#else
  char fullPath[_MAX_PATH+1];
  GetDesignPath(fullPath);
  strcat(fullPath, ABILITY_DB_NAME);
#endif
  return saveData(data, fullPath);
}

BOOL saveData(ABILITY_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open ability db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }

  CAR car(&myFile, CArchive::store);
  // Done later  car.Compress(true);
  data.PreSerialize(car.IsStoring());
  data.Serialize(car);
  data.PostSerialize(car.IsStoring());
  car.Close();
  return TRUE;
}


/****************************************
 *
 *            Special Abilities
 *
 *****************************************/

BOOL saveData(A_SPECABILITY_DEF_L& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open specialAbility db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }

  CAR car(&myFile, CArchive::store);
  // Done later  car.Compress(true);
  //data.PreSerialize(car.IsStoring());
  data.Serialize(car);
  //data.PostSerialize(car.IsStoring());
  car.Close();
  return TRUE;
}


BOOL saveData(A_SPECABILITY_DEF_L& data)
{
//  BOOL success = FALSE;
  EditorStatusMsg("Saving specialAbility data...");

  CString fullPath;
  fullPath = rte.DataDir() + SPECAB_DB_NAME;
  return saveData(data, fullPath);
}




/***************************************************
 ***************************************************/

void clearData(ABILITY_DATA_TYPE& data)
{
  data.Clear();
}

void loadUADefaults(ABILITY_DATA_TYPE& data)
{
  data.LoadUADefaults();
}

int loadData(ABILITY_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  if (!myFile.Open(fullPath, CFile::modeRead, &e))
  {
    WriteDebugString("Unable to open ability db file %s, error %i\n", fullPath, e.m_cause);
    return 0;
  }

  data.Clear();
  CAR car(&myFile, CArchive::load);

  try
  {
    data.PreSerialize(car.IsStoring());
    if (data.Serialize(car)!=0)
    {
      data.Clear();
      car.Close();
      return 0;
    };
    data.PostSerialize(car.IsStoring());
  }
  catch (...)
  {
      WriteDebugString("Unable to load ability data file %s\n", car.GetFile()->GetFileName());
      data.Clear();
  }

  car.Close();

  if (data.GetCount() == 0)
     WriteDebugString("No abilities in ability db file\n");

  return data.GetCount();
}

int GetCount(BASE_CLASS_DATA_TYPE& data)
{
  return data.GetCount();
}

#ifdef UAFEDITOR
void loadUADefaults(BASE_CLASS_DATA_TYPE& data)
{
  data.LoadUADefaults();
}
#endif


int GetDataSize(BASE_CLASS_DATA_TYPE& data) {
  return data.GetDataSize();
}


BOOL saveData(BASE_CLASS_DATA_TYPE& data)
{
//  BOOL success = FALSE;
  EditorStatusMsg("Saving baseclass data...");

  CString fullPath;
  fullPath = rte.DataDir() + BASE_CLASS_DB_NAME;
  return saveData(data, fullPath);
}

BOOL saveData(BASE_CLASS_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open baseclass db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }

  CAR car(&myFile, CArchive::store);
  // done later  car.Compress(true);
  data.PreSerialize(car.IsStoring());
  data.Serialize(car);
  data.PostSerialize(car.IsStoring());
  car.Close();
  return TRUE;
}

void clearData(BASE_CLASS_DATA_TYPE& data)
{
  data.Clear();
}

int loadData(BASE_CLASS_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  if (!myFile.Open(fullPath, CFile::modeRead, &e))
  {
    WriteDebugString("Unable to open baseclass db file %s, error %i\n", fullPath, e.m_cause);
    return 0;
  }

  data.Clear();
  CAR car(&myFile, CArchive::load);
  try
  {
    data.PreSerialize(car.IsStoring());
    if (data.Serialize(car)!=0)
    {
      data.Clear();
      car.Close();
      return 0;
    };
    data.PostSerialize(car.IsStoring());
  }
  catch (...)
  {
      WriteDebugString("Unable to load baseclass data file %s\n", car.GetFile()->GetFileName());
      data.Clear();
  }

  car.Close();

  if (data.GetCount() == 0)
     WriteDebugString("No base classes in baseclass db file\n");

  return data.GetCount();
}

int GetDataSize(CLASS_DATA_TYPE& data)
{
  return data.GetDataSize();
}

int GetCount(CLASS_DATA_TYPE& data)
{
  return data.GetCount();
}

void clearData(CLASS_DATA_TYPE& data)
{
  data.Clear();
}

#ifdef UAFEDITOR
void loadUADefaults(CLASS_DATA_TYPE& data)
{
  data.LoadUADefaults();
}
#endif

BOOL saveData(CLASS_DATA_TYPE& data)
{
//  BOOL success = FALSE;
  EditorStatusMsg("Saving class data...");

  CString fullPath;
  fullPath = rte.DataDir() + CLASS_DB_NAME;
  return saveData(data, fullPath);
}

BOOL saveData(CLASS_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open class db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }

  CAR car(&myFile, CArchive::store);
  // done later     car.Compress(true);
  data.PreSerialize(car.IsStoring());
  data.Serialize(car);
  data.PostSerialize(car.IsStoring());
  car.Close();
  return TRUE;
}


int loadData(CLASS_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  if (!myFile.Open(fullPath, CFile::modeRead, &e))
  {
    WriteDebugString("Unable to open class db file %s, error %i\n", fullPath, e.m_cause);
    return 0;
  }

  data.Clear();
  CAR car(&myFile, CArchive::load);

  try
  {
    data.PreSerialize(car.IsStoring());
    if (data.Serialize(car)!=0)
    {
      data.Clear();
      car.Close();
      return 0;
    };
    data.PostSerialize(car.IsStoring());
  }
  catch (...)
  {
      WriteDebugString("Unable to load class data file %s\n", car.GetFile()->GetFileName());
      data.Clear();
  }

  car.Close();

  if (data.GetCount() == 0)
     WriteDebugString("No classes in class db file\n");

  return data.GetCount();
}

#ifdef USE_TRAITS
int GetCount(TRAIT_DATA_TYPE& data)
{
  return data.GetCount();
}


BOOL saveData(TRAIT_DATA_TYPE& data)
{
//  BOOL success = FALSE;
  EditorStatusMsg("Saving trait data...");

#ifdef SIMPLE_STRUCTURE
  CString fullPath;
  fullPath = rte.DataDir() + TRAIT_DB_NAME;
#else
  char fullPath[_MAX_PATH+1];
  GetDesignPath(fullPath);
  strcat(fullPath, TRAIT_DB_NAME);
#endif
  return saveData(data, fullPath);
}


BOOL saveData(TRAIT_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open trait db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }

  CAR car(&myFile, CArchive::store);
  // done later   car.Compress(true);
  data.PreSerialize(car.IsStoring());
  data.Serialize(car);
  data.PostSerialize(car.IsStoring());
  car.Close();
  return TRUE;
}

void clearData(TRAIT_DATA_TYPE& data)
{
  data.Clear();
}

void loadUADefaults(TRAIT_DATA_TYPE& data)
{
  data.LoadUADefaults();
}

int loadData(TRAIT_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  if (!myFile.Open(fullPath, CFile::modeRead, &e))
  {
    WriteDebugString("Unable to open trait db file %s, error %i\n", fullPath, e.m_cause);
    return 0;
  }

  data.Clear();
  CAR car(&myFile, CArchive::load);

  try
  {
    data.PreSerialize(car.IsStoring());
    if (data.Serialize(car)!=0)
    {
      data.Clear();
      car.Close();
      return 0;
    };
    data.PostSerialize(car.IsStoring());
  }
  catch (...)
  {
      WriteDebugString("Unable to load trait data file %s\n", car.GetFile()->GetFileName());
      data.Clear();
  }

  car.Close();

  if (data.GetCount() == 0)
     WriteDebugString("No traits in trait db file\n");

  return data.GetCount();
}
#endif

int GetCount(SPELLGROUP_DATA_TYPE& data)
{
  return data.GetCount();
}


void loadUADefaults(SPELLGROUP_DATA_TYPE& data)
{
  data.LoadUADefaults();
}

int GetDataSize(SPELLGROUP_DATA_TYPE& data)
{
  return data.GetDataSize();
}



BOOL saveData(SPELLGROUP_DATA_TYPE& data)
{
//  BOOL success = FALSE;
  return TRUE;
/* No spellgroup database, please.  PRS 20130305
  EditorStatusMsg("Saving spellgroup data...");

  CString fullPath;
  fullPath = rte.DataDir() + SPELLGROUP_DB_NAME;
  return saveData(data, fullPath);
*/
}

BOOL saveData(SPELLGROUP_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  SetFileAttributes(fullPath, FILE_ATTRIBUTE_NORMAL);
  if (!myFile.Open(fullPath, CFile::modeCreate | CFile::modeWrite, &e))
  {
    WriteDebugString("Unable to open spellgroup db file %s, error %i\n", fullPath, e.m_cause);
    return FALSE;
  }

  CAR car(&myFile, CArchive::store);
  // done later   car.Compress(true);
  data.PreSerialize(car.IsStoring());
  data.Serialize(car);
  data.PostSerialize(car.IsStoring());
  car.Close();
  return TRUE;
}

void clearData(SPELLGROUP_DATA_TYPE& data)
{
  data.Clear();
}

int loadData(SPELLGROUP_DATA_TYPE& data, LPCSTR fullPath)
{
  CFileException e;
  CFile myFile;

  if (!myFile.Open(fullPath, CFile::modeRead, &e))
  {
    WriteDebugString("Unable to open spellgroup db file %s, error %i\n", fullPath, e.m_cause);
    return 0;
  }

  data.Clear();
  CAR car(&myFile, CArchive::load);

  try
  {
    data.PreSerialize(car.IsStoring());
    if (data.Serialize(car)!=0)
    {
      data.Clear();
      car.Close();
      return 0;
    };
    data.PostSerialize(car.IsStoring());
  }
  catch (...)
  {
      WriteDebugString("Unable to load spellgroup data file %s\n", car.GetFile()->GetFileName());
      data.Clear();
  }

  car.Close();

  if (data.GetCount() == 0)
     WriteDebugString("No spell groups in spellgroup db file\n");

  return data.GetCount();
}

//////////////////////////////////////////////////////////////////////////////////
//
//  Interface functions above.
//
//  Database member functions below.
//
///////////////////////////////////////////////////////////////////////////////////

void PerhapsPrintCompileError(CString& msg)
{
  static const CString err("**COMPILE ERROR** ");
  if (LogCompileErrors>0) WriteDebugString(err + msg);
  else if (LogCompileErrors==0) WriteDebugString (err + "More compile errors\n");
  LogCompileErrors--;
}


GENERIC_REFERENCE::GENERIC_REFERENCE(void)
{
  m_refKey=0;
  m_refType=UNKNOWN_DB;
}

GENERIC_REFERENCE::~GENERIC_REFERENCE(void)
{
}

void GENERIC_REFERENCE::Clear(void)
{
  m_refName="";
  m_refKey=0xffffffff;
  m_refType=UNKNOWN_DB;
}

GENERIC_REFERENCE& GENERIC_REFERENCE::operator =(const GENERIC_REFERENCE& src)
{
  if (this == &src) return *this;
  m_refKey=src.m_refKey;
  m_refName=src.m_refName;
  m_refType=src.m_refType;
  return *this;
}

GENERIC_REFERENCE& GENERIC_REFERENCE::operator = (const CString& name)
{
  Clear();
  m_refName=name;
  return *this;
}

BOOL GENERIC_REFERENCE::operator ==(const GENERIC_REFERENCE& src) const
{
  if (this == &src) return TRUE;
  if (m_refKey!=src.m_refKey) return FALSE;
  if (m_refName!=src.m_refName) return FALSE;
  if (m_refType!=src.m_refType) return FALSE;
  return TRUE;
}

int GENERIC_REFERENCE::GetDataSize(void)
{
  int size=sizeof (m_refType);
  size += sizeof (m_refName);
  size += sizeof (m_refKey);
  return size;
}

BOOL GENERIC_REFERENCE::LookupRefKey(void)
{
  int tempRefKey;
  int refNameLen;
  m_refType = 0;
  if (m_refName == "__const__")
  {
    m_refType=LITERAL_DB;
    return TRUE;
  }

  // Search the databases one at a time.
  if (m_refName.CompareNoCase("level")==0)
  {
    m_refType=LEVEL_DB;
    m_refKey=0xffffffff;
    return TRUE;
  };
  //if (m_refName.CompareNoCase("race")==0)

  //{
  //  m_refType=RACE_DB;
    //m_refKey=raceUnknown;
  //  m_raceID.Clear();
  //};
  //if (m_refName.CompareNoCase("class")==0)
  //{
  //  m_refType=BASECLASS_DB;
  //  //m_refKey=classUnknown;
  //  m_baseclassID.Clear();
  //};

  if (m_refName.CompareNoCase("male")==0)
  {
    m_refType=GENDER_DB;
    m_refKey=Male;
    return TRUE;
  };

  if (m_refName.CompareNoCase("female")==0)
  {
    m_refType=GENDER_DB;
    m_refKey=Female;
    return TRUE;
  };

  //if ((m_refKey=classData.FindName(m_refName))!=classUnknown)
  refNameLen = m_refName.GetLength();
  if (refNameLen > 6)
  {
    if (m_refName[5] == '_')
    {
      if (memcmp((LPCSTR)m_refName, "Class", 5) == 0)
      {
        m_refType = CLASSDB_ID;
        m_refName = m_refName.Mid(6);
        return TRUE;
      };
    };
  };
  //m_classID = m_refName;
  //if (classData.LocateClass(m_classID) >= 0)
  //{
  //  if (m_refType != 0)
  //  {
      //AmbiguousRefType(CLASS_DB);
    //}
    //else
    //{
    //  m_refType=CLASS_DB;
  //  };
  //};


  if (refNameLen > 5)
  {
    if (m_refName[4] == '_')
    {
      if (memcmp((LPCSTR)m_refName, "Race", 4) == 0)
      {
        m_refType = RACEDB_ID;
        m_refName = m_refName.Mid(5);
        return TRUE;
      };
    };
  };
  //if (raceData.LocateRace(m_raceID) >= 0)
  //{
  //  if (m_refType != 0)
  //  {
  //    //AmbiguousRefType(RACE_DB);
  //  //}
  //  //else
  //  //{
  //  //  m_refType=RACE_DB;
  //  };
  //};



  //m_baseclassID = m_refName;
  //if (baseclassData.LocateBaseclass(m_baseclassID) >= 0)
  //{
  //  if (m_refType != 0)
  //  {
  //    //AmbiguousRefType(BASECLASS_DB);
  //  }
  //  else
  //  {
  //    m_refType=BASECLASS_DB;
  //  };
  //};
  //if ((m_refKey=abilityData.FindName(m_refName))!=abilityUnknown)
  tempRefKey = abilityData.LocateAbilityData(m_refName);
  if (tempRefKey >= 0)
  {
    if (m_refType != 0)
    {
      //AmbiguousRefType(ABILITY_DB);
    }
    else
    {
      m_refType=ABILITY_DB;
      m_refKey = tempRefKey;
      return TRUE;
    };
  };

  SPELLGROUP_ID spellgroupID;
  //spellgroupID = m_refKey;
  spellgroupID = m_refName;
  //if ((m_refKey=spellgroupData.FindName(m_refName))!=spellgroupUnknown)
  //if (spellgroupData.LocateSpellgroup(spellgroupID) >= 0)
  tempRefKey = spellgroupData.LocateSpellgroup(spellgroupID);
  if (tempRefKey >= 0)
  {
    if (m_refType != 0)
    {
      //AmbiguousRefType(SPELLGROUP_DB);
    }
    else
    {
      m_refType=SPELLGROUP_DB;
      m_refKey = tempRefKey;
      return TRUE;
    };
  };

#ifdef USE_TRAITS
  if ((tempRefKey=traitData.FindName(m_refName))!=traitUnknown)
  {
    if (m_refType != 0)
    {
      //AmbiguousRefType(TRAIT_DB);
    }
    else
    {
      m_refType=TRAIT_DB;
      m_refKey = tempRefKey;
      return TRUE;
    };
  };
#endif

  if (m_refType == 0)
  {
    m_refType=UNKNOWN_DB;
    m_refKey=0xffffffff;
    return FALSE;
  };

  return FALSE;
}

// some of these db types assume that the char context
// has been set.
BOOL GENERIC_REFERENCE::LookupReferenceData(double &result)
{
  // This is only important if you need to retrieve
  // CHARACTER or COMBATANT information. Temp characters
  // used during character creation do not have a valid
  // source (neither NPC/Monster db or party member).
  //
  //ASSERT(ctx.IsValidSrc());

  switch (m_refType)
  {
  case LITERAL_DB:
    {
      int tmp = (int)m_refKey; // convert to integer
      result=tmp;
      return TRUE;
    }
  case LEVEL_DB:
    {
      ActorType ctx = GetCharContext();
      if (ctx.Level == LEVEL_UNKNOWN)
      {
        // Not Implemented(0x2abca, false);
        result = 0;
      }
      else
      {
        result=ctx.Level;
      };
      return TRUE;
    }
  case GENDER_DB:
    {
      ActorType ctx = GetCharContext();
      CHARACTER *pChar = GetCurrentlyActiveContext(&ctx, "GENERIC_REFERENCE::LookupReferenceData");
      if (pChar==NULL) { result=0.0; return FALSE; }
      result=(m_refKey==pChar->GetAdjGender());
      return TRUE;
    }

  //case RACE_DB:
  case RACEDB_ID:
    {
      RACE_ID raceID;
      ActorType ctx = GetCharContext();
      //if (m_refKey==raceUnknown)
      //if (m_raceID.IsNoRace())
      //{
      //  Not Implemented(0x3cec85, false);
        //result = ctx.Race;// evaluating 'race' keyword, return race key for char context
      //}
      //else
        //result = (m_refKey==ctx.Race);// return 1 if char race matches specified, else 0
      raceID = this->m_refName;
      result = (raceID == ctx.m_raceID) ? 1 : 0;// return 1 if char race matches specified, else 0
      return TRUE;
    }

  //case CLASS_DB:
  case CLASSDB_ID:
    {
      CLASS_ID classID;
      ActorType ctx = GetCharContext();
      //if (m_refKey==classUnknown)
      //if (m_classID.IsNoClass())
      //{
      //  Not Implemented(0x4c7a83, false);
        //result = ctx.Class;// evaluating 'class' keyword, return class key for char context
      //}
      //else
        //result = doClassesOverlap((classType)m_refKey,(classType)ctx.Class);// return 1 if char class matches specified, else 0
      classID = m_refName;
      result = (classID  == ctx.m_classID) ? 1 : 0;// return 1 if char class matches specified, else 0
      return TRUE;
    }

  //case BASECLASS_DB:
  case BASECLASSDB_ID:
    {
      ActorType ctx = GetCharContext();
      BASECLASS_ID baseclassID;
      //if (m_refKey==classUnknown)
      //if (m_baseclassID.IsNoBaseclass())
      //{
      //  Not Implemented(0x6c8ace, false);
        //result = ctx.Class;// evaluating 'class' keyword, return class key for char context
      //}
      //else
        //result = doClassesOverlap((classType)m_refKey,(classType)ctx.Class);// return 1 if char class matches specified, else 0
      //  result = doClassesOverlap(m_baseclassID,ctx.m_classID);// return 1 if char class matches specified, else 0
      baseclassID = this->m_refName;
      result = doClassesOverlap(baseclassID, ctx.m_classID) ? 1 : 0;// return 1 if char class matches specified, else 0
      return TRUE;
    }

  case ABILITY_DB:
    {
      ActorType ctx = GetCharContext();
      CHARACTER *pChar = GetCurrentlyActiveContext(&ctx, "GENERIC_REFERENCE::LookupReferenceData");
      if (pChar==NULL) { result=0.0; return FALSE; }
      //result = pChar->GetAbilityScore(m_refKey);
      result = pChar->GetAbilityScore(this->m_refName);
      return TRUE;
    }

  case TRAIT_DB:
    {
      //ActorType ctx = GetCharContext();
      //result = traitData.
      result = 0;
    }
    // fallthrough until implemented

  case SPELLGROUP_DB:
    {
      //ActorType ctx = GetCharContext();
      //result = spellgroupData.
      result = 0;
    }
    // fallthrough until implemented
  }
  return FALSE;
}
void GENERIC_REFERENCE::PreSerialize(BOOL IsStoring)
{
  if (IsStoring)
  {
    if (!LookupRefKey())
    {
      CString msg;
      msg.Format("Cannot locate GENERIC_REFERENCE database reference = \"%s\"\n",m_refName);
      PerhapsPrintCompileError(msg);
    }
  };
}

int GENERIC_REFERENCE::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    if (m_refName=="") m_refName="*";
    car << m_refName;
    if (m_refName=="*") m_refName="";
    car << m_refType;
    car << m_refKey;
  }
  else
  {
    car >> m_refName; if (m_refName=="*") m_refName="";
    car >> m_refType;
    car >> m_refKey;
  };
  return 0;
}
int GENERIC_REFERENCE::Serialize(CArchive& car)
{
  if (car.IsStoring())
  {
    if (m_refName=="") m_refName="*";
    car << m_refName;
    if (m_refName=="*") m_refName="";
    car << m_refType;
    car << m_refKey;
  }
  else
  {
    car >> m_refName; if (m_refName=="*") m_refName="";
    car >> m_refType;
    car >> m_refKey;
  };
  return 0;
}

BOOL GENERIC_REFERENCE::Evaluate(double &result)
{
  if (m_refType==UNKNOWN_DB)
  {
    LookupRefKey();
    if (m_refType==UNKNOWN_DB)
      return FALSE;
  }
  return (LookupReferenceData(result));
}

/*

BASECLASS_REFERENCE::BASECLASS_REFERENCE(void)
{
  //m_unusedBaseclassKey=baseclassUnknown;
}

BASECLASS_REFERENCE::~BASECLASS_REFERENCE(void)
{
}

BASECLASS_REFERENCE& BASECLASS_REFERENCE::operator = (const BASECLASS_REFERENCE& src)
{
  if (this == &src) return *this;
  //m_unusedBaseclassKey=src.m_unusedBaseclassKey;
  //m_baseclassName=src.m_baseclassName;
  m_baseclassID = src.m_baseclassID;
  return *this;
}

int BASECLASS_REFERENCE::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    //if (m_baseclassName=="") m_baseclassName="*";
    //car << m_baseclassName;
    //if (m_baseclassName=="*") m_baseclassName="";
    //car << m_unusedBaseclassKey;
    car << m_baseclassID;
  }
  else
  {
    //car >> m_baseclassName; if (m_baseclassName=="*") m_baseclassName="";
    //car >> m_unusedBaseclassKey;
    car >> m_baseclassID;
  };
  return 0;
}

void BASECLASS_REFERENCE::PreSerialize(BOOL IsStoring)
{
  if (!IsStoring) return;
//  stringIndex.AddString(m_baseclassName);
  // Convert the m_baseclassName to m_baseclassKey.  Print error message.
  //m_unusedBaseclassKey=baseclassData.FindName(m_baseclassName);
  //if (m_unusedBaseclassKey==baseclassUnknown)
  //{
  //  CString msg;
  //  msg.Format("Required BaseClass references unknown baseclass = \"%s\"\n",m_baseclassName);
  //  PerhapsPrintCompileError(msg);
  //};
}



int BASECLASS_REFERENCE::GetDataSize(void)
{
  int size=sizeof (*this);
  //size += m_baseclassName.GetLength();
  size += m_baseclassID.GetLength();
  return size;
}

*/
/*
ABILITY_REFERENCE::ABILITY_REFERENCE(void)
{
  //m_abilityKey=abilityUnknown;
  m_abilityName="";
}

ABILITY_REFERENCE::~ABILITY_REFERENCE(void)
{
}

void ABILITY_REFERENCE::Clear(void)
{
  m_abilityName="";
  //m_abilityKey=abilityUnknown;
}

int ABILITY_REFERENCE::GetDataSize(void)
{
  int size=sizeof (m_abilityKey);
  size += sizeof (m_abilityName);
  size += m_abilityName.GetLength();
  return size;
}

int ABILITY_REFERENCE::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    if (m_abilityName=="") m_abilityName="*";
    car << m_abilityName;
    if (m_abilityName=="*") m_abilityName="";
    car << m_abilityKey;
  }
  else
  {
    car >> m_abilityName; if (m_abilityName=="*") m_abilityName="";
    car >> m_abilityKey;
  };
  return 0;
}

ABILITY_REFERENCE& ABILITY_REFERENCE::operator = (const ABILITY_REFERENCE& src)
{
  if (this == &src) return *this;
  //m_abilityKey=src.m_abilityKey;
  m_abilityName=src.m_abilityName;
  return *this;
}

void ABILITY_REFERENCE::PreSerialize(BOOL IsStoring)
{
  if (!IsStoring) return;
//  stringIndex.AddString(m_abilityName);
  // Convert the m_abilityName to m_abilityKey.  Print error message.
  m_abilityKey=abilityData.FindName(m_abilityName);
  if (m_abilityKey==abilityUnknown)
  {
    CString msg;
    msg.Format("Required Ability references unknown ability = \"%s\"\n",m_abilityName);
    PerhapsPrintCompileError(msg);
  };
}
*/


SPELL_REFERENCE::SPELL_REFERENCE(void)
{
  //m_spellKey.Clear();
  m_spellID.Clear();
}

SPELL_REFERENCE::~SPELL_REFERENCE(void)
{
}

void SPELL_REFERENCE::Clear(void)
{
  m_spellID.Clear();
  //m_spellKey.Clear();
  //m_spellName.Empty();
}

int SPELL_REFERENCE::GetDataSize(void) const
{
  //int size=sizeof (m_spellKey);
  //size += sizeof (m_spellName);
  int size = sizeof (m_spellID);
  return size;
}


int SPELL_REFERENCE::Serialize(CAR& car, double version)
{
  if (car.IsStoring())
  {
    if (m_spellID=="") m_spellID="*";
    car << m_spellID;
    if (m_spellID=="*") m_spellID="";
    //m_spellKey.Serialize(car, version); //car << m_spellKey.m_key;
  }
  else
  {
    car >> m_spellID; if (m_spellID=="*") m_spellID="";
#ifdef UAFEDITOR
    if (version < VersionSpellNames)
    {
      {
        DWORD key;
        car >> key;
      };
    //m_spellKey.Serialize(car, version); //car >> m_spellKey.m_key;
    };
#endif
  };
  return 0;
}

SPELL_REFERENCE& SPELL_REFERENCE::operator = (const SPELL_REFERENCE& src)
{
  if (this == &src) return *this;
  //m_spellKey=src.m_spellKey;
  m_spellID=src.m_spellID;
  return *this;
}

void SPELL_REFERENCE::PreSerialize(BOOL IsStoring)
{
  if (!IsStoring) return;
//  stringIndex.AddString(m_spellName);
  // Convert the m_spellName to m_spellKey.  Print error message.
  //m_spellKey=spellData.FindName(m_spellName);
  //if (!m_spellKey.IsValidSpell())
  if (!m_spellID.IsValidSpell())
  {
    CString msg;
    msg.Format("Reference to unknown spell = \"%s\"\n",m_spellID.UniqueName());
    PerhapsPrintCompileError(msg);
  };
}

/*
SPELLGROUP_REFERENCE::SPELLGROUP_REFERENCE(void)
{
  m_spellgroupKey=spellgroupUnknown;
}

SPELLGROUP_REFERENCE::~SPELLGROUP_REFERENCE(void)
{
}

void SPELLGROUP_REFERENCE::Clear(void)
{
  m_spellgroupName="";
  m_spellgroupKey=spellgroupUnknown;
}
*/


int SPELLGROUP_REFERENCE::GetDataSize(void) const
{
  //int size=sizeof (m_spellgroupKey);
  //size += sizeof (m_spellgroupName);
  //size += m_spellgroupName.GetLength();
  //return size;
  return sizeof (m_spellgroupID);
}


void SPELLGROUP_REFERENCE::Serialize(CAR& car, const CString& version)
{
  if (car.IsStoring())
  {
    //if (m_spellgroupName=="") m_spellgroupName="*";
    //car << m_spellgroupName;
    //if (m_spellgroupName=="*") m_spellgroupName="";
    //car << m_spellgroupKey;
    car << m_spellgroupID;
  }
  else
  {
    //car >> m_spellgroupName; if (m_spellgroupName=="*") m_spellgroupName="";
    //car >> m_spellgroupKey;
    car >> m_spellgroupID;
#ifdef UAFEDITOR
    if (version < "SpGrpV3")
    {
      DWORD key;
      car >> key;
    };
#endif
  };
}

/*
SPELLGROUP_REFERENCE& SPELLGROUP_REFERENCE::operator = (const SPELLGROUP_REFERENCE& src)
{
  if (this == &src) return *this;
  m_spellgroupKey=src.m_spellgroupKey;
  m_spellgroupName=src.m_spellgroupName;
  return *this;
}

void SPELLGROUP_REFERENCE::PreSerialize(BOOL IsStoring)
{
  if (!IsStoring) return;
//  stringIndex.AddString(m_spellgroupName);
  // Convert the m_spellgroupName to m_spellgroupKey.  Print error message.
  m_spellgroupKey=spellgroupData.FindName(m_spellgroupName);
  if (m_spellgroupKey==spellUnknown)
  {
    CString msg;
    msg.Format("Reference to unknown Spell Group = \"%s\"\n",m_spellgroupName);
    PerhapsPrintCompileError(msg);
  };
}
*/
/*
RACE_REFERENCE::RACE_REFERENCE(void)
{
  //m_raceKey=raceUnknown;
  m_raceKey=raceUnknown;
}

RACE_REFERENCE::~RACE_REFERENCE(void)
{
}

void RACE_REFERENCE::Clear(void)
{
  m_raceName="";
  m_raceKey=raceUnknown;
}

int RACE_REFERENCE::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    if (m_raceName=="") m_raceName="*";
    car << m_raceName;
    if (m_raceName=="*") m_raceName="";
    car << m_raceKey;
  }
  else
  {
    car >> m_raceName; if (m_raceName=="*") m_raceName="";
    int temp;
    car >> temp;
    m_raceKey = (raceType)temp; // Cast until raceType conversion complete
  };
  return 0;
}
*/
/*
int RACE_REFERENCE::Serialize(CAR& car, int intVer)
{
  if (car.IsStoring())
  {
    car << m_raceID;
  }
  else
  {
    car >> m_raceID;
#ifdef UAFEDITOR
    if (intVer < 4)
    {
      int raceKey;
      car >> raceKey;
    };
#endif
  };
  return 0;
}

/*
int RACE_REFERENCE::GetDataSize(void)
{
  int size=sizeof (*this);
  size += m_raceName.GetLength();
  return size;
}



RACE_REFERENCE& RACE_REFERENCE::operator = (const RACE_REFERENCE& src)
{
  if (this == &src) return *this;
  m_raceKey=src.m_raceKey;
  m_raceName=src.m_raceName;
  return *this;
}

void RACE_REFERENCE::PreSerialize(BOOL IsStoring)
{
  if (!IsStoring) return;
//  stringIndex.AddString(m_raceName);
  // Convert the m_raceName to m_raceKey.  Print error message.
  m_raceKey=raceData.FindName(m_raceName);
  if (m_raceKey==raceUnknown)
  {
    CString msg;
    msg.Format("Race reference to unknown race = \"%s\"\n",m_raceName);
    PerhapsPrintCompileError(msg);
  };
}
*/

ADJUSTMENT::ADJUSTMENT(void)
{
  Clear();
}

ADJUSTMENT::ADJUSTMENT(const ADJUSTMENT& src)
{
  *this=src;
}

ADJUSTMENT::ADJUSTMENT(char value, CString name)
{
  Clear();
  m_parameter[0] = value;
  m_reference = name;
  m_operator[0]=AJO_gear;
}

ADJUSTMENT::~ADJUSTMENT(void)
{
}

BOOL ADJUSTMENT::Add(int param, char oper)
{
  int index=-1;
  for (int i=0;i<MAX_ADJ;i++)
  {
    if (m_operator[i]==AJO_none)
    {
      index=i;
      break;
    }
  }

  if (index >= 0)
  {
    m_parameter[index] = param;
    m_operator[index]  = oper;
    return TRUE;
  }
  return FALSE;
}

ADJUSTMENT& ADJUSTMENT::operator=(const ADJUSTMENT& src)
{
  if (this==&src) return *this;
  for (int i=0;i<MAX_ADJ;i++)
  {
    m_parameter[i] =src.m_parameter[i];
    m_operator[i]  =src.m_operator[i];
  }
  m_reference    =src.m_reference;
  return *this;
}

BOOL ADJUSTMENT::operator ==(const ADJUSTMENT& src) const
{
  if (this==&src) return TRUE;
  for (int i=0;i<MAX_ADJ;i++)
  {
    if (m_parameter[i] != src.m_parameter[i]) return FALSE;
    if (m_operator[i]  != src.m_operator[i]) return FALSE;
  }
  if ((m_reference   == src.m_reference)==FALSE) return FALSE;
  return TRUE;
}

void ADJUSTMENT::Clear(void)
{
  for (int i=0;i<MAX_ADJ;i++)
  {
    m_parameter[i]=0;
    m_operator[i]=AJO_none;
  }
  m_reference.Clear();
}

CString ADJUSTMENT::EncodeOperator(int op)
{
  switch (op)
  {
  case AJO_none: return " ";
  case AJO_gear:     return " * ";
  case AJO_plus:     return " + ";
  case AJO_equality: return " == ";
  case AJO_greater:  return " > ";
  case AJO_greaterEqual: return " >= ";
  case AJO_andBit:       return " & ";
  case AJO_slash:        return " / ";
  case AJO_andLogical:   return " && ";
  case AJO_continue:     return "";
  case AJO_minus:        return " - ";
  case AJO_less:         return " < ";
  case AJO_lessEqual:    return " <= ";
  case AJO_orBit:        return " | ";
  case AJO_orLogical:    return " || ";
  }
  return "";
}

static double ApplyAdjOperator(double p1, int op, double p2)
{
  switch (op)
  {
  case AJO_none: return p1;
  case AJO_gear:     return (p1*p2);
  case AJO_plus:     return (p1+p2);
  case AJO_equality: return (!(p1!=p2));
  case AJO_greater:  return (p1>p2);
  case AJO_greaterEqual: return (p1>=p2);
  case AJO_andBit:       return ((DWORD)p1&(DWORD)p2);
  case AJO_slash:        if (p2==0.0) return 0.0; else return (p1/p2);
  case AJO_andLogical:   return (p1&&p2);
  case AJO_continue:     return p1;
  case AJO_minus:        return (p1-p2);
  case AJO_less:         return (p1<p2);
  case AJO_lessEqual:    return (p1<=p2);
  case AJO_orBit:        return ((DWORD)p1|(DWORD)p2);
  case AJO_orLogical:    return ((DWORD)p1||(DWORD)p2);
  }
  return p1;
}


CString ADJUSTMENT::EncodeText()
{
  CString tmp("");
  CString otmp;

  tmp += m_reference.m_refName;

  for (int i=0;i<MAX_ADJ;i++)
  {
    if ((m_operator[i]!=AJO_none)&&(m_operator[i]!=AJO_continue))
    {
      tmp += EncodeOperator(m_operator[i]);
      otmp.Format("%i", abs(m_parameter[i]));
      tmp += otmp;
    }
  }
  return tmp;
}

double ADJUSTMENT::ApplyOperatorsToInput(double &input)
{
  for (int i=0;i<MAX_ADJ;i++)
    input = ApplyAdjOperator(input, m_operator[i], m_parameter[i]);
  return input;
}

BOOL ADJUSTMENT::HasTwoOperands()
{
  if (m_reference.m_refName.IsEmpty()) return FALSE;

  // look for at least one operator, and
  // make sure the AJO_continue operator isn't
  // present.
  char count=0;
  for (int i=0;i<MAX_ADJ;i++)
  {
    if (m_operator[i]!=AJO_none)
    {
      if (m_operator[i]==AJO_continue)
        return FALSE;

      count++;
    }
  }
  return (count>0);
}

// Unary evaluate. There will be no parameters,
// only a reference and one operator which is applied
// to ref and input.
BOOL ADJUSTMENT::Evaluate(double &result)
{
  double refValue=0.0;
  if (m_reference.Evaluate(refValue))
  {
    // reference has been evaluated,
    // now apply the operators to it.
    result = ApplyAdjOperator(refValue, m_operator[0], result);
    return TRUE;
  }
  return FALSE;
}

// Binary evaluate. There will be a reference and
// at least one parameter/operator pair.
BOOL ADJUSTMENT::EvaluateBinary(double &result)
{
  double refValue=0.0;
  if (m_reference.Evaluate(refValue))
  {
    // reference has been evaluated,
    // now apply the operators to it.
    ApplyOperatorsToInput(refValue);
    result = refValue;
    return TRUE;
  }
  return FALSE;
}

int ADJUSTMENT::GetDataSize(void)
{
  int size = sizeof (m_parameter);
  size += sizeof (m_operator);
  size += m_reference.GetDataSize();
  return size;
}

void ADJUSTMENT::PreSerialize(BOOL IsStoring)
{
  m_reference.PreSerialize(IsStoring);
}

int ADJUSTMENT::Serialize(CAR& car)
{
  int result=0;
  if (car.IsStoring())
  {
    car << m_parameter[0] << m_parameter[1] << m_parameter[2];
    car << m_operator[0] << m_operator[1] << m_operator[2];
    m_reference.Serialize(car);
  }
  else
  {
    car >> m_parameter[0] >> m_parameter[1] >> m_parameter[2];
    car >> m_operator[0] >> m_operator[1] >> m_operator[2];
    result=m_reference.Serialize(car);
  };
  return result;
}
int ADJUSTMENT::Serialize(CArchive& car)
{
  int result=0;
  if (car.IsStoring())
  {
    car << m_parameter[0] << m_parameter[1] << m_parameter[2];
    car << m_operator[0] << m_operator[1] << m_operator[2];
    m_reference.Serialize(car);
  }
  else
  {
    car >> m_parameter[0] >> m_parameter[1] >> m_parameter[2];
    car >> m_operator[0] >> m_operator[1] >> m_operator[2];
    result=m_reference.Serialize(car);
  };
  return result;
}


//***************************************************************************
inline bool whitespace(const char c)
{
  return ((c==' ') || (c=='\t'));
}

int GetInteger (int& column,
                const CString& value,
                int& result,
                CString& errorMsg)
{
  // Skips over leading and trailing whitespace
  // The string must not be all whitespace.
  int origColumn = column;
  int len;
  int sign=1;
  len=value.GetLength();
  result=0;
  while (column < len)
  {
    if (whitespace(value[column]))
    {
      column++;
      continue;
    };
    break;
  };

  // We have found first character, which
  // might be a +/- as in +3 or '- 4'
  if (column < len)
  {
    if (value[column]=='-')
    {
      sign=-1;
      // skip to next token
      column++;
      while ((column < len) && whitespace(value[column])) column++;
    }
    else if (value[column]=='+')
    {
      sign=1;
      // skip to next token
      column++;
      while ((column < len) && whitespace(value[column])) column++;
    }
  }

  if (column>=len)
  {
    errorMsg="Looking for integer.  Found only blanks";
    column=origColumn;
    return FALSE;
  };
  if ((value[column]<'0') || (value[column]>'9'))
    // There must be at least one digit!
  {
    errorMsg="Non-decimal digit in integer";
    column=origColumn;
    return FALSE;
  };
  while (column < len)
  {
    if (whitespace(value[column])) break;
    if ((value[column]<'0') || (value[column]>'9')) break;
    result=result*10+value[column]-'0';
    column++;
  };
  // We either found the end of line or a whitespace or a non-digit.
  // If it is whitespace, we promised to skip over it.
  while ((column < len) && whitespace(value[column])) column++;
  result*=sign;
  return TRUE;
}


static bool decodeNdM(const CString& value, int& n, int& m)
{
  // If it does not decode properly, leave incol as is.
  CString errorMsg; // Discard error message
  int col=0;
  int len=value.GetLength();
  while ( (col<len) && whitespace(value[col])) col++;
  if (!GetInteger(col,value,n,errorMsg)) return false;
  if ( (col>=len) || ( (value[col]!='D')&&(value[col]!='d') ) ) return false;
  col++;
  if (!GetInteger(col,value,m,errorMsg)) return false;
  return true;
}

int compileDicePlusRDR(CString& name, char *array, int arraySize)
{
  // int *array can hold up to 20 values
  //
  // array[0] should contain the number of int's
  // that follow.

  // some tokens may have quotes, "Half-Elf"
  name.Remove('\"');

  int qty=0,sides=0;
  if (decodeNdM(name, qty, sides))
  {
    array[0] = 3;
    array[1] = DICE_DB;
    array[2] = qty;
    array[3] = sides;
    return 3;
  }

  GENERIC_REFERENCE ref;
  ref.m_refName = name;
  if (!ref.LookupRefKey())
  {
    WriteDebugString("Unknown RDR value %s in compileDicePlusRDR\n", name);
    return 0; // error
  }
  {
    int i, n;
    n = ref.m_refName.GetLength();
    if (n+2 > arraySize)
    {
      n = arraySize-2;
    };
//  array[0] = 2;
//  array[1] = ref.m_refType;
//  array[2] = ref.m_refKey;
    array[0] = n + 1;
    array[1] = ref.m_refType;
    for (i=0; i<n; i++)
    {
      array[i+2] = ref.m_refName[i];
    };
  };

  return 3;
}

int interpretDicePlusRDR(int *array)
{
  switch (array[1])
  {
  case LEVEL_DB:
  case RACE_DB:
  case BASECLASS_DB:
  case ABILITY_DB:
  case CLASS_DB:
  case TRAIT_DB:
  case SPELLGROUP_DB:
  case GENDER_DB:
    // a known database type
    break;

  case DICE_DB:
      return (RollDice(array[3], array[2]));

  default:
    WriteDebugString("Illegal RDR code %i in interpretDicePlusRDR\n", array[1]);
    return 0;
  }

  GENERIC_REFERENCE ref;
  ref.m_refType = array[1];
  ref.m_refKey = array[2];
  double result=0.0;
  ref.LookupReferenceData(result);
  return result;
}

int interpretDicePlusRDRB(const char *array)
{
  GENERIC_REFERENCE ref;
  int i;
  ref.m_refKey = -1;
  ref.m_refType = array[1];
  ref.m_refName = CString(array+2, array[0]-1);
  switch (array[1])
  {
  case LEVEL_DB:
    break;
  case RACEDB_ID:
  case BASECLASSDB_ID:
  //case ABILITY_DB:
  case CLASSDB_ID:
  //case TRAIT_DB:
 // case SPELLGROUP_DB:
    break;
  case GENDER_DB:
    // a known database type
    for (i=0; i<sizeof(GenderNames)/sizeof(GenderNames[0]); i++)
    {
      if (ref.m_refName == GenderNames[i]) ref.m_refKey = i;
    };
    break;

  case DICE_DB:
      return (RollDice(array[3], array[2]));

  default:
    WriteDebugString("Illegal RDR code %i in interpretDicePlusRDR\n", array[1]);
    return 0;
  }

  double result=0.0;
  ref.LookupReferenceData(result);
  return result;
}

// this one maximizes the possible dice roll
// otherwise, same as other callback
int interpretMaxDicePlusRDR(int *array)
{
  switch (array[1])
  {
  case LEVEL_DB:
  case RACE_DB:
  case BASECLASS_DB:
  case ABILITY_DB:
  case CLASS_DB:
  case TRAIT_DB:
  case SPELLGROUP_DB:
  case GENDER_DB:
    // a known database type
    break;

  case DICE_DB:
      return (array[2]*array[1]);

  default:
    WriteDebugString("Illegal RDR code %i in interpretDicePlusRDR\n", array[1]);
    return 0;
  }

  GENERIC_REFERENCE ref;
  ref.m_refType = array[1];
  ref.m_refKey = array[2];
  double result=0.0;
  ref.LookupReferenceData(result);
  return result;
}

int interpretMaxDicePlusRDRB(const char *array)
{
  switch (array[1])
  {
  case LEVEL_DB:
  case RACE_DB:
  case BASECLASS_DB:
  case ABILITY_DB:
  case CLASS_DB:
  case TRAIT_DB:
  case SPELLGROUP_DB:
  case GENDER_DB:
    // a known database type
    break;

  case DICE_DB:
      return (array[2]*array[1]);

  default:
    WriteDebugString("Illegal RDR code %i in interpretDicePlusRDR\n", array[1]);
    return 0;
  }

  GENERIC_REFERENCE ref;
  ref.m_refType = array[1];
  ref.m_refKey = array[2];
  double result=0.0;
  ref.LookupReferenceData(result);
  return result;
}


DICEPLUS::DICEPLUS(void)
{
  Clear();
}

DICEPLUS::~DICEPLUS(void)
{
}

void DICEPLUS::Clear(void)
{
  /*
  m_sign=1;
  m_numDice=0;
  m_numSides=0;
  m_bonus=0;
  m_min=-999;
  m_max=999;
  m_adjust.RemoveAll();
  */
  m_Text="";
  m_Bin="";
}

DICEPLUS &DICEPLUS::operator = (const DICEPLUS& src)
{
  if (&src == this) return *this;
  //int count;
  m_Text=src.m_Text;
  m_Bin=src.m_Bin;
  /*
  m_sign=src.m_sign;
  m_numSides=src.m_numSides;
  m_numDice=src.m_numDice;
  m_bonus=src.m_bonus;
  m_min=src.m_min;
  m_max=src.m_max;
  m_adjust.RemoveAll();
  count=src.m_adjust.GetSize();
  m_adjust.SetSize(count);
  for (int i=0; i< count; i++)
  {
    m_adjust[i]=src.m_adjust[i];
    //ADJUSTMENT newAdj=src.m_adjust[i];
    //m_adjust.SetAtGrow(i,src.m_adjust[i]);
  };
  */
  return *this;
}

int DICEPLUS::GetDataSize(void) const
{
  //int i,n;
  int size = sizeof(DICEPLUS);
  size += m_Text.GetLength();
  size += m_Bin.GetLength();
  /*
  n=m_adjust.GetSize();
  for (i=0; i<n; i++)
    size += m_adjust[i].GetDataSize();
  */
  return size;
}
/*
void DICEPLUS::AddAdjust(int value, const CString& name)
{
  ADJUSTMENT newA(value, name);
  m_adjust.SetAtGrow(m_adjust.GetSize(),newA);
}

void DICEPLUS::AddAdjust(ADJUSTMENT& adjust)
{
  m_adjust.SetAtGrow(m_adjust.GetSize(), adjust);
}
*/

void DICEPLUS::AddAdjust(CString &name)
{
  // I don't think 'name' can even
  // have these characters if 'name'
  // is read from a text config file
  name.Remove('\n');
  name.Remove('\r');
  name.Remove('\t');

  CString tmp;
  if (m_Text.IsEmpty())
    tmp.Format("(%s)", name);
  else
    tmp.Format("+(%s)", name);
  m_Text += tmp;
}

BOOL DICEPLUS::Compile() const
{
    m_Bin="";
  if (m_Text.GetLength()==0)
    return FALSE;

  //WriteDebugString("COMPILING: \'%s\'\n", m_Text);

  RDRCOMP ccalc; //to compile
  m_Bin = ccalc.CompileExpression(m_Text, compileDicePlusRDR);
  if (m_Bin.GetLength()==0)
  {
    CString tmp = m_Text.Left(475); // to avoid text len error
    WriteDebugString("COMPILE FAILED: %s\n", tmp);
  }
   return (m_Bin.GetLength() > 0);
}

BOOL DICEPLUS::Roll(double &result) const
{
  result = 0.0;
  if (m_Bin.GetLength()==0)
  {
    if (!Compile())
      return FALSE;
  }

  RDREXEC icalc;      //to interpret
  result = icalc.InterpretExpression(m_Bin,interpretDicePlusRDR,interpretDicePlusRDRB);
  return TRUE;

  /*
  BOOL success=TRUE;
  if ((m_numSides==0)&&(m_numDice==0)&&(m_min==-999)&&(m_max==999))
    result = m_bonus;
  else
    result = RollDice(m_numSides,m_numDice,m_bonus);
  result*=m_sign;

  int num = m_adjust.GetSize();
  if (num > 0)
  {
    double tmp=result;
    char diceop = m_adjust[0].m_operator[0];

    if (num > 1)
    {
      double adj=0.0;
      for (int i=1;i<num;i++)
      {
        if (m_adjust[i].HasTwoOperands())
        {
          // There should be only one of these, if any.
          // It should also be the first adjustment listed.
          if (!m_adjust[i].EvaluateBinary(adj))
            success = FALSE;
        }
        else
        {
          if (!m_adjust[i].Evaluate(adj))
            success=FALSE;
        }
      }

      // update result if adjustments parsed ok,
      // else ignore them and just use the dice value
      if (success)
        result = ApplyAdjOperator(tmp,diceop,adj);
    }
  }

  if (success)
  {
    if (m_sign >= 0)
    {
      if (result < m_min) result=m_min;
      if ((m_max > 0) && (result > m_max)) result=m_max;
    }
    else
    {
      if ((m_max > 0) && (result < -m_max)) result=-m_max;
      if (result > -m_min) result=-m_min;
    }
  }
  return success;
  */
}

BOOL DICEPLUS::MaxRoll(double &result) const
{
  result = 1.0;
  if (m_Bin.GetLength()==0)
  {
    if (!Compile())
      return FALSE;
  }

  RDREXEC icalc;      //to interpret
  result = icalc.InterpretExpression(m_Bin,interpretMaxDicePlusRDR, interpretMaxDicePlusRDRB);
  return TRUE;
  /*
  BOOL success=TRUE;
  if ((m_numSides==0)&&(m_numDice==0)&&(m_min==-999)&&(m_max==999))
    result = m_bonus;
  else
    result = (m_numSides*m_numDice)+m_bonus;
  double tmp=result;
  for (int i=0;i<m_adjust.GetSize();i++)
  {
    if (!m_adjust[i].Evaluate(tmp))
      success=FALSE;
  }
  // update result if adjustments parsed ok,
  // else ignore them and just use the dice value
  if (success) result=tmp;
  result *= m_sign;
  if (result < m_min) result=m_min;
  if ((m_max > 0) && (result > m_max)) result=m_max;
  return TRUE;
  */
}

void DICEPLUS::PreSerialize(BOOL IsStoring)
{
  //int i,n;
  //n=m_adjust.GetSize();
  //for (i=0; i<n; i++) m_adjust[i].PreSerialize(IsStoring);
}


#define ISLETTER (((c>='A')&&(c<='Z'))  ||  ((c>='a')&&(c<='z')))
#define ISNUMBER ((c>='0')&&(c<='9'))
#define ISALPHANUM ((ISLETTER) || (ISNUMBER))

CString EncodeOldDicePlusText(const CString& txt)
{
  // I guess we must traverse the txt and find any identifiers.
  // We know of the six race names.
  int i, j, n;
  CString identifier, prefix;
  const char *ptxt;
  CArray<char, char> newtxt;
  ptxt = txt;
  n = txt.GetLength();
  newtxt.SetSize(2*n);
  j = 0;
  for (i=0; i<n;)
  {
    char c;
    // Look for an identifier.. Starts with a letter.
    c = ptxt[i];
    if (ISLETTER)  // Start of identifier
    {
      // Now find the end of the identifier.
      int k, m;
      k = i+1;
      for (c=ptxt[k]; ISALPHANUM; c=ptxt[++k]);
      identifier = txt.Mid(i, k-i);
      if (   (identifier == "Halfling")
          || (identifier == "Human")
          || (identifier == "Elf")
          || (identifier == "Dwarf")
	        || (identifier == "Gnome")
	        || (identifier == "Half Elf"))
      {
        prefix = "Race_";
      }
      else if (    (identifier.CompareNoCase(Baseclass_fighter)== 0)
	              || (identifier.CompareNoCase(Baseclass_cleric)== 0)
	              || (identifier.CompareNoCase(Baseclass_ranger)== 0)
	              || (identifier.CompareNoCase(Baseclass_paladin)== 0)
	              || (identifier.CompareNoCase(Baseclass_magicUser)== 0)
	              || (identifier.CompareNoCase(Baseclass_thief)== 0)
	              || (identifier.CompareNoCase(Baseclass_druid)== 0)
              )
      {
        prefix = "Baseclass_";
      }
      else if (    (identifier.CompareNoCase("level") == 0)
                || (identifier.CompareNoCase("male") == 0)
                || (identifier.CompareNoCase("female") == 0)
                || (identifier.CompareNoCase(Ability_Strength) == 0)
                || (identifier.CompareNoCase(Ability_Intelligence) == 0)
                || (identifier.CompareNoCase(Ability_Wisdom) == 0)
                || (identifier.CompareNoCase(Ability_Dexterity) == 0)
                || (identifier.CompareNoCase(Ability_Constitution) == 0)
                || (identifier.CompareNoCase(Ability_Charisma) == 0)
              )
      {
        prefix.Empty();
      }
      else
      {
        prefix.Empty();
      };
      prefix += identifier;
      m = prefix.GetLength();
      i = k;
      for (k=0; k<m; k++) newtxt.SetAtGrow(j++, prefix[k]);
    }
    else if (ISNUMBER)
    {
      // Copy all alphanumeric characters.
      newtxt.SetAtGrow(j++, c);
      for (c=ptxt[++i]; ISALPHANUM; c=ptxt[++i]) newtxt.SetAtGrow(j++, c);
    }
    else
    {
      newtxt.SetAtGrow(j, c);
      j++;
      i++;
    }
  };
  if (j > 0)
  {
    return CString(&newtxt.ElementAt(0), j);
  }
  else
  {
    return "";
  };
}


CString EncodeOldDicePlusText(char m_numDice,
                              BYTE m_numSides,
                              char m_bonus,
                              int m_min,
                              int m_max,
                              char m_sign,
                              DICEPLUSADJLIST &m_adjust)
{
  CString result;
  CString temp;
  if ((m_numSides==0)&&(m_numDice==0))
  {
    // literal only, such as '10'
    if (m_sign < 0)
      result.Format("-%i", m_bonus);
    else
      result.Format("%i", m_bonus);

    int num=m_adjust.GetSize();
    if (num > 0)
    {
      result += m_adjust[0].EncodeOperator(m_adjust[0].m_operator[0]);

      // for things like 3*level
      for (int i=1; i<num; i++)
      {
        ADJUSTMENT adj = m_adjust[i];
        result += adj.EncodeText();
      }

      // no need for min/max if just a literal used
      if ((m_bonus!=0)||(num>0))
      {
        // for upper/lower limits
        if (m_min != -999)
        {
          temp.Format(" |< %d",
                        m_min);
          result += temp;
        }
        if ((m_max != 999)&&(m_max!=0))
        {
          temp.Format(" >| %d",
                        m_max);
          result += temp;
        }
      }
    }
  }
  else
  {
    // 1d4
    if (m_sign < 0)
    {
      result.Format("-%dd%d",
                m_numDice,
                m_numSides);
    }
    else
    {
      result.Format("%dd%d",
                m_numDice,
                m_numSides);
    }

    int num=m_adjust.GetSize();
    if (num > 0)
    {
      result += m_adjust[0].EncodeOperator(m_adjust[0].m_operator[0]);

      // for things like 3*level
      for (int i=1; i<num; i++)
      {
        ADJUSTMENT adj = m_adjust[i];
        result += adj.EncodeText();
      }
    }

    // for upper/lower limit
    if (m_min != -999)
    {
      temp.Format(" |< %d",
                    m_min);
      result += temp;
    }
    if ((m_max != 999)&&(m_max!=0))
    {
      temp.Format(" >| %d",
                    m_max);
      result += temp;
    }
  }
  return result;
}

int DICEPLUS::Serialize(CAR& car)
{
  int result=0;
  CString dpVersion("DP2");
  unsigned int i, count;

  // temps used to load older DicePlus data
  char              m_numDice=0;
  BYTE              m_numSides=0;
  char              m_bonus=0;
  int               m_min=-999;
  int               m_max=999;
  char              m_sign=1;
  DICEPLUSADJLIST   m_adjust;

  if (car.IsStoring())
  {
    car << dpVersion;  // DICEPLUS version 2
    /*
    car << m_numDice;
    car << m_numSides;
    car << m_bonus;
    car << m_min;
    car << m_max;
    car << m_sign;
    count = m_adjust.GetSize();
    car.WriteCount(count);
    for (i=0; i<count; i++)
      m_adjust[i].Serialize(car);
    */
    AS(car, m_Text);
    AS(car, m_Bin);
  }
  else
  {
    car >> dpVersion;
    if (dpVersion=="DP0")
    {
      BYTE tmp;
      car >> m_numDice;
      car >> m_numSides;
      car >> m_bonus;
      car >> tmp; m_min=tmp;
      car >> tmp; m_max=tmp;
      count = car.ReadCount();
      m_adjust.SetSize(count);
      for (i=0; i<count; i++)
      {
        result=m_adjust[i].Serialize(car);
        if (result!=0) break;
      }

      if (m_numDice < 0)
      {
        m_sign = -1;
        m_numDice = abs(m_numDice);
      }
      m_Text = EncodeOldDicePlusText(m_numDice,m_numSides,m_bonus,m_min,m_max,m_sign,m_adjust);
    }
    else if (dpVersion=="DP1")
    {
      car >> m_numDice;
      car >> m_numSides;
      car >> m_bonus;
      car >> m_min;
      car >> m_max;
      car >> m_sign;
      count = car.ReadCount();
      m_adjust.SetSize(count);
      for (i=0; i<count; i++)
      {
        result=m_adjust[i].Serialize(car);
        if (result!=0) break;
      }

      if (m_numDice < 0)
      {
        m_sign = -1;
        m_numDice = abs(m_numDice);
      }
      m_Text = EncodeOldDicePlusText(m_numDice,m_numSides,m_bonus,m_min,m_max,m_sign,m_adjust);
    }
    else if (dpVersion=="DP2")
    {
      DAS(car, m_Text);
      DAS(car, m_Bin);
#ifdef UAFEDITOR
      // We need to rewrite the Text for "Runtime Data References" (RDR)
      // We used to say things like "Is the character a Halfling?" by simply
      // writing the word 'Halfling', which would evaluate to 0 or 1.
      // In the future, We need to specify what we mean by 'Halfling'; is it
      // a race or a class or a baseclass or what???
      // So the Text will be modified to say 'RACE_Halfling'.
      // The binary used to contain an integer indicating the
      // 'Race Type' (5 = Halfling, for example).  In the future, the
      // binary must contain a counted string.  The binary will have an
      // op-code for LITERAL_STRING.
      m_Text = EncodeOldDicePlusText(m_Text);
      m_Bin = ""; // Force a compile.
#endif
      m_Bin = ""; // Force a compile.
    }
    else
    {
      CString msg;
      msg.Format("Unknown DICEPLUS format = %s\n",dpVersion);
      WriteDebugString(msg);
      result=1;
    };
  }
  return result;
}

int DICEPLUS::Serialize(CArchive& car)
{
  int result=0;
  CString dpVersion("DP2");
  unsigned int i, count;

  char              m_numDice=0;
  BYTE              m_numSides=0;
  char              m_bonus=0;
  int               m_min=-999;
  int               m_max=999;
  char              m_sign=1;
  DICEPLUSADJLIST   m_adjust;

  if (car.IsStoring())
  {
    car << dpVersion;  // DICEPLUS version 2
    /*
    car << m_numDice;
    car << m_numSides;
    car << m_bonus;
    car << m_min;
    car << m_max;
    car << m_sign;
    count = m_adjust.GetSize();
    car.WriteCount(count);
    for (i=0; i<count; i++)
      m_adjust[i].Serialize(car);
    */
    AS(car, m_Text);
    AS(car, m_Bin);
  }
  else
  {
    car >> dpVersion;
    if (dpVersion=="DP0")
    {
      BYTE tmp;
      car >> m_numDice;
      car >> m_numSides;
      car >> m_bonus;
      car >> tmp; m_min=tmp;
      car >> tmp; m_max=tmp;
      count = car.ReadCount();
      m_adjust.SetSize(count);
      for (i=0; i<count; i++)
      {
        result=m_adjust[i].Serialize(car);
        if (result!=0) break;
      };

      if (m_numDice < 0)
      {
        m_sign = -1;
        m_numDice = abs(m_numDice);
      }
      m_Text = EncodeOldDicePlusText(m_numDice,m_numSides,m_bonus,m_min,m_max,m_sign,m_adjust);
    }
    else if (dpVersion=="DP1")
    {
      car >> m_numDice;
      car >> m_numSides;
      car >> m_bonus;
      car >> m_min;
      car >> m_max;
      car >> m_sign;
      count = car.ReadCount();
      m_adjust.SetSize(count);
      for (i=0; i<count; i++)
      {
        result=m_adjust[i].Serialize(car);
        if (result!=0) break;
      }

      if (m_numDice < 0)
      {
        m_sign = -1;
        m_numDice = abs(m_numDice);
      }
      m_Text = EncodeOldDicePlusText(m_numDice,m_numSides,m_bonus,m_min,m_max,m_sign,m_adjust);
    }
    else if (dpVersion=="DP2")
    {
      DAS(car, m_Text);
      DAS(car, m_Bin);
    }
    else
    {
      CString msg;
      msg.Format("Unknown DICEPLUS format = %s\n",dpVersion);
      WriteDebugString(msg);
      result=1;
    };
  }
  return result;
}
CString DICEPLUS::EncodeText()
{
  return m_Text;
  /*
  CString tmp("");
  if (m_numDice < 0)
  {
    m_sign = -1;
    m_numDice = abs(m_numDice);
  }

  CONFIG_ENCODE_dicePlus(tmp,*this);
  return tmp;
  */
}

BOOL DICEPLUS::DecodeText(CString data, CString& err)
{
  if (data.GetLength()==0) return FALSE;
  if (CONFIG_DECODE_dicePlus(data,err,this,0,0,0,-999,999)!=CONFIG_STAT_ok)
  {
    WriteDebugString("Decode DicePlus failed \'%s\': %s\n", data, err);
    return FALSE;
  }
  return TRUE;
}
BOOL DICEPLUS::DecodeText(CString data)
{
  CString msg;
  return (DecodeText(data,msg));
}

//*****************************************************************************


ABILITY_REQ::ABILITY_REQ(void)
{
  Clear();
}

ABILITY_REQ::~ABILITY_REQ(void)
{
}

int ABILITY_REQ::GetDataSize(void) const
{
  int size = sizeof (m_min);
  size += sizeof (m_minMod);
  size += sizeof (m_max);
  size += sizeof (m_maxMod);
  size += m_abilityID.GetDataSize();
  return size;
}

void ABILITY_REQ::Clear(void)
{
  m_abilityID.Clear();
  m_min=0;
  m_minMod=0;
  m_max=999;
  m_maxMod=100;
}

ABILITY_REQ& ABILITY_REQ::operator =(const ABILITY_REQ& src)
{
  if (this==&src) return *this;
  //m_ability    =src.m_ability;
  m_abilityID   =src.m_abilityID;
  m_min         =src.m_min;
  m_minMod      =src.m_minMod;
  m_max         =src.m_max;
  m_maxMod      =src.m_maxMod;
  return *this;
}

int ABILITY_REQ::Serialize(CAR& car)
{
  int result=0;
  CString ver("ABL1");

  if (car.IsStoring())
  {
    car << ver;  // version
    //m_ability.Serialize(car);  // Like strength
    car << m_abilityID;  // Like strength
    car << m_min; // Positive
    car << m_minMod;
    car << m_max; // Positive
    car << m_maxMod;
  }
  else
  {
    car >> ver;
    car >> m_abilityID;  // Like strength
#ifdef UAFEDITOR
      //result=m_ability.Serialize(car);  // Like strength
    if (ver=="ABL0")
    {
      DWORD key;
      car >> key;
    }
    else
#endif
    {
      if (ver == "ABL1")
      {
      }
      else
      {
        WriteDebugString("Unknown ABILITY_LIMITS version = %s \n",ver);
        return 1;
      };
    };
    car >> m_min; // Positive
    car >> m_minMod;
    car >> m_max; // Positive
    car >> m_maxMod;
  };
  return result;
}


void ABILITY_REQ::PreSerialize(BOOL IsStoring)
{
  //m_ability.PreSerialize(IsStoring);
}

CString ABILITY_REQ::Encode(void) const
{
  CString result, x;
  result = Name();
  result += " ";
  x.Format("%d",m_min);
  result += x;
  if (m_minMod != 0)
  {
    x.Format("(%d)", m_minMod);
    result += x;
  };
  x.Format("/%d", m_max);
  result += x;
  if (m_maxMod != 0)
  {
    x.Format("(%d)", m_maxMod);
    result += x;
  };
  return result;
}


RACE_DATA::RACE_DATA(void) :
      // ASL named "RACE_DATA_ATTRIBUTES"
	    m_race_asl (),
	    m_temp_asl (),
      m_specAbs(true)
{
  Clear();
}

RACE_DATA::~RACE_DATA(void)
{
  Clear();
}

void RACE_DATA::Clear(void)
{
  //m_raceKey=raceUnknown;
  preSpellNameKey = -1;
  m_name="";
  m_weight.Clear();
  m_height.Clear();
  m_age.Clear();
  m_maxAge.Clear();
  m_abilityRequired.RemoveAll();
  m_skills.RemoveAll();
  m_skillAdjustmentsAbility.RemoveAll();
  m_skillAdjustmentsBaseclass.RemoveAll();
  m_skillAdjustmentsScript.RemoveAll();
  m_skillAdjustmentsRace.RemoveAll();
  m_baseMovement.Clear();
  m_race_asl.Clear();
  m_temp_asl.Clear();
  m_specAbs.Clear();
  m_findSecretDoor = 1;
  m_findSecretDoorSearching = 2;
}


int RACE_DATA::LocateAbilityReq(const CString& abilityName) const
{
  int i, n;
  n = GetAbilityReqCount();
  for (i=0; i<n; i++)
  {
    if (PeekAbilityReq(i)->m_abilityID == abilityName) return i;
  };
  return -1;
}

int RACE_DATA::GetDataSize(void) const
{
  int i,n;
  int size=0;
  size += m_race_asl.GetDataSize();
  size += m_temp_asl.GetDataSize();
  size += m_name.GetLength();
  size += m_weight.GetDataSize();
  size += m_height.GetDataSize();
  size += m_age.GetDataSize();
  size += m_maxAge.GetDataSize();
  size += m_baseMovement.GetDataSize();
  size += m_specAbs.GetDataSize();
  n=m_abilityRequired.GetSize();
  for (i=0; i<n; i++) 
  {
    size += PeekAbilityReq(i)->GetDataSize();
    //size += m_abilityRequired[i].GetDataSize();
  };
  return size;
}

void RACE_DATA::PreSerialize(BOOL IsStoring)
{
  // We have to convert m_abilityRequired to keys in the abilities database
  int i;
  for (i=0; i<m_abilityRequired.GetSize(); i++)
  {
    GetAbilityReq(i)->PreSerialize(IsStoring);
    //m_abilityRequired[i].PreSerialize(IsStoring);
  };
}

void RACE_DATA::LogUndefinedMultiClassReferences(void)
{
  DELIMITED_STRING MCRef;
  CString ref;
  int i, n;
  MCRef = m_race_asl.Lookup(RACE_ALLOWED_CLASS_ATTRIBUTE);
  if (!MCRef.IsLegal())
  {
    CString msg;
    msg.Format("Race %s has an illegal AllowedMultiClass list\n",m_name);
    PerhapsPrintCompileError(msg);
    return;
  };
  n = MCRef.Count();
  for (i=0; i<n; i++)
  {
    ref = MCRef.GetAt(i);
    CLASS_ID classID;
    classID = ref;
    //if (classData.FindName(ref) == classUnknown)
    if (classData.LocateClass(classID) < 0)
    {
      CString msg;
      msg.Format("Race %s has undefined AllowedMultiClass %s\n",
                             m_name, ref);
      PerhapsPrintCompileError(msg);
    };
  };
}

int RACE_DATA::Serialize(CAR &car, const CString& version)
{
  //int i, n;
  int result=0;
  if (car.IsStoring())
  {
    {
      int i;
      //car << m_raceKey;
      car << preSpellNameKey;
      if (m_name == "") m_name = "*";
      car << m_name;
      if (m_name == "*") m_name = "";
      m_weight.Serialize(car);
      m_height.Serialize(car);
      m_age.Serialize(car);
      m_maxAge.Serialize(car);
      car.WriteCount(m_abilityRequired.GetSize());
      for (i = 0; i < m_abilityRequired.GetSize(); i++)
      {
        GetAbilityReq(i)->Serialize(car);
        //m_abilityRequired[i].Serialize(car);
      };
      m_baseMovement.Serialize(car);
      car << m_canChangeClass;
      car << m_dwarfResistance;
      car << m_gnomeResistance;
      car << m_findSecretDoor;
      car << m_findSecretDoorSearching;
      LogUndefinedMultiClassReferences();
      car.Serialize(m_race_asl, "RACE_DATA_ATTRIBUTES");
    };
    {
      int i, n;
      n = m_skills.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkill(i)->Serialize(car);
        //m_skills[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsAbility.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjAbility(i)->Serialize(car);
        //m_skillAdjustmentsAbility[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsBaseclass.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjBaseclass(i)->Serialize(car);
        //m_skillAdjustmentsBaseclass[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsRace.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjRace(i)->Serialize(car);
        //m_skillAdjustmentsRace[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsScript.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjScript(i)->Serialize(car);
        //m_skillAdjustmentsScript[i].Serialize(car);
      };
    };
    m_specAbs.Serialize(car, globalData.version, m_name, "races");
  }
  else
  {
    preSpellNameKey = -1;
#ifdef UAFEDITOR
    if (version < "RaceV2")
    {
      //int key;
      //car >> key;
      //m_raceKey = (raceType)key;
        //int key;
        //car >> key;
      car >> preSpellNameKey;
    };
#endif
    if (version > "RaceV2")
    {
      car >> preSpellNameKey;
    };
    {
      {
        int i, n;
        car >> m_name; if (m_name == "*") m_name = "";
#ifdef UAFEngine
        // In the runtime, Name means only the un-decorated
        // part of the name.  For example:  "Gnome|Stupid" will be
        // read as simply "Gnome".  There can be multiple objects with
        // the same name but once we are at the point where we have the binary
        // data then everything is identified uniquely and is referenced by the
        // binary key.  The name is only for printing.
        if (m_name.Find('|') >= 0) m_name = m_name.Left(m_name.Find('|')); // Remove qualifier.
#endif
        result = m_weight.Serialize(car);
        if (result != 0) return result;
        result = m_height.Serialize(car);
        if (result != 0) return result;
        result = m_age.Serialize(car);
        if (result != 0) return result;
        result = m_maxAge.Serialize(car);
        if (result != 0) return result;
        n = car.ReadCount();
        m_abilityRequired.SetSize(n);
        for (i = 0; i < n; i++)
        {
          result = GetAbilityReq(i)->Serialize(car);
          //result=m_abilityRequired[i].Serialize(car);
          if (result != 0) return result;
        };
        result = m_baseMovement.Serialize(car);
        if (result != 0) return result;
      };
#ifdef UAFEDITOR
      if (version < "RaceV2")
      {
        m_canChangeClass = (m_name.CompareNoCase("Human") == 0);
        m_dwarfResistance = (m_name.CompareNoCase("Dwarf") == 0);;
        m_gnomeResistance = (m_name.CompareNoCase("Gnome") == 0);;
        m_findSecretDoor = (m_name.CompareNoCase("Elf") == 0) ? 5 : 2;
        m_findSecretDoorSearching = (m_name.CompareNoCase("Elf") == 0) ? 2 : 1;
      }
      else
#endif
      {
        car >> m_canChangeClass;
        car >> m_dwarfResistance;
        car >> m_gnomeResistance;
        car >> m_findSecretDoor;
        car >> m_findSecretDoorSearching;
      };

      if (result != 0) return result;
      car.DeSerialize(m_race_asl, "RACE_DATA_ATTRIBUTES");
      if (globalData.version >= VersionSpellNames)
      {
        {
          int i, n;
          car >> n;
          m_skills.SetSize(n);
          for (i = 0; i < n; i++)
          {
            GetSkill(i)->Serialize(car);
            //m_skills[i].Serialize(car);
          };
        };
        {
          int i, n;
          car >> n;
          m_skillAdjustmentsAbility.SetSize(n);
          for (i = 0; i < n; i++)
          {
            GetSkillAdjAbility(i)->Serialize(car);
            //m_skillAdjustmentsAbility[i].Serialize(car);
          };
        };
        {
          int i, n;
          n = m_skillAdjustmentsBaseclass.GetSize();
          car >> n;
          m_skillAdjustmentsBaseclass.SetSize(n);
          for (i = 0; i < n; i++)
          {
            GetSkillAdjBaseclass(i)->Serialize(car);
            //m_skillAdjustmentsBaseclass[i].Serialize(car);
          };
        };
        {
          int i, n;
          car >> n;
          m_skillAdjustmentsRace.SetSize(n);
          for (i = 0; i < n; i++)
          {
            GetSkillAdjRace(i)->Serialize(car);
            //m_skillAdjustmentsRace[i].Serialize(car);
          };
        };
        {
          int i, n;
          car >> n;
          m_skillAdjustmentsScript.SetSize(n);
          for (i = 0; i < n; i++)
          {
            GetSkillAdjScript(i)->Serialize(car);
            //m_skillAdjustmentsScript[i].Serialize(car);
          };
        };
      };
    }
    if (globalData.version >= _SPECIAL_ABILITIES_VERSION_)
    {
      m_specAbs.Serialize(car, globalData.version, m_name, "races");
    };
  };
  return result;
}

void RACE_DATA::AddAbilityRequirement(ABILITY_REQ& abReq)
{
  int i, n;
  n=m_abilityRequired.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekAbilityReq(i)->Name()==abReq.Name()) break;
    //if (m_abilityRequired[i].Name()==abReq.Name()) break;
  };

  //if (abReq.m_ability.m_abilityKey==abilityUnknown)
  //  abReq.m_ability.m_abilityKey = abilityData.FindName(abReq.Name());

  m_abilityRequired.SetAtGrow(i,abReq);
}

void RACE_DATA::AddAllowedClass(const CString& className)
{
  CString key = RACE_ALLOWED_CLASS_ATTRIBUTE;
  CString newValue;
  DELIMITED_STRING ds;
  {
    const ASLENTRY *pEntry;
    pEntry = m_race_asl.Find(key);
    if (pEntry != NULL)
    {
      ds = *pEntry->pValue();
    };
  };
  if (ds.Contains(className)) return; //It is already there!
  ds.Insert(className);
  newValue = ds.Result();
  m_race_asl.Insert(key,newValue,ASLF_READONLY|ASLF_DESIGN);
}

//*********************************************************
// Is this particular Class allowed.
//*********************************************************
bool RACE_DATA::IsAllowedClass(const CString& className) const
{
  DELIMITED_STRING ds;
  CString key = RACE_ALLOWED_CLASS_ATTRIBUTE;
  {
    const ASLENTRY *pEntry;
    pEntry = m_race_asl.Find(key);
    if (pEntry == NULL) return true;
    ds = *pEntry->pValue();
  }
  if (!ds.IsLegal()) return true;
  return ds.Contains(className);
}

DWORD RACE_DATA::GetWeight(void) const
{
  // need character context before doing this
  double result;
  if (m_weight.Roll(result)) return result;
  return 0;
}

DWORD RACE_DATA::GetHeight(void) const
{
  // need character context before doing this
  double result;
  if (m_height.Roll(result)) return result;
  return 0;
}

DWORD RACE_DATA::GetStartAge(void) const
{
  // need character context before doing this
  double result;
  if (m_age.Roll(result)) return result;
  return 0;
}

DWORD RACE_DATA::GetMaxAge(void) const
{
  // need character context before doing this
  double result;
  if (m_maxAge.Roll(result)) return result;
  return 0;
}

DWORD RACE_DATA::GetBaseMovement(void) const
{
  // need character context before doing this
  double result;
  if (m_baseMovement.Roll(result)) return result;
  return 0;
}


//short RACE_DATA::GetMinAbility(abilityType type) const
short RACE_DATA::GetMinAbility(const CString& abilityName) const
{
/*
  CString keyText = abilityData.GetName(type);
  if (keyText.IsEmpty()) return DEFAULT_MIN_ABILITY_LIMIT;

  for (int i=0;i<m_abilityRequired.GetSize();i++)
  {
    if (m_abilityRequired[i].Name().CompareNoCase(keyText)==0)
      return (m_abilityRequired[i].GetMin());
  }
*/
  int indx;
  indx = LocateAbilityReq(abilityName);
  if (indx < 0)   return DEFAULT_MIN_ABILITY_LIMIT;
  const ABILITY_REQ *pAbilityReq;
  pAbilityReq = PeekAbilityReq(indx);
  return pAbilityReq->GetMin();
}


//short RACE_DATA::GetMaxAbility(abilityType type) const
short RACE_DATA::GetMaxAbility(const CString& abilityName) const
{
/*
  CString keyText = abilityData.GetName(type);
  if (keyText.IsEmpty()) return DEFAULT_MAX_ABILITY_LIMIT;

  for (int i=0;i<m_abilityRequired.GetSize();i++)
  {
    if (m_abilityRequired[i].Name().CompareNoCase(keyText)==0)
      return (m_abilityRequired[i].GetMax());
  }
*/
  int indx;
  indx = LocateAbilityReq(abilityName);
  if (indx < 0)   return DEFAULT_MAX_ABILITY_LIMIT;
  const ABILITY_REQ *pAbilityReq;
  pAbilityReq = PeekAbilityReq(indx);
  return pAbilityReq->GetMax();
}

/* Function moved to ALLOWED_CLASSES
void RACE_DATA::RestrictClasses(ALLOWED_CLASSES *pAllowedClasses)
{
  int i;
  for (i=pAllowedClasses->NumAllowedClasses(); i>=0; i--)
  {
    if (IsAllowedClass(pAllowedClasses->GetAt(i)->m_name)) continue;
    if (pAllowedClasses->GetAt(i)->IsRaceAllowed(m_name)) continue;
    pAllowedClasses->RemoveAt(i);
  }; //That was easy.
}
*/

RACE_DATA_TYPE::RACE_DATA_TYPE(void)
{
  Clear();
  LoadUADefaults();
}

void RACE_DATA_TYPE:: operator = (const RACE_DATA_TYPE& src)
{
  if (&src==this) return;
  m_RaceData.RemoveAll();
  m_RaceData.Append(src.m_RaceData);
}



void RACE_DATA_TYPE::LoadUADefaults(void)
{ // If you want to Clear() you have to do it yourself!
  RACE_DATA rd;
  rd.m_name="HUMAN";
  //rd.m_raceKey=Human;
  //AddRaceWithCurrKey(rd);
  AddRace(rd);
  rd.m_name="ELF";
  //rd.m_raceKey=Elf;
  //AddRaceWithCurrKey(rd);
  AddRace(rd);
  rd.m_name="DWARF";
  //rd.m_raceKey=Dwarf;
  //AddRaceWithCurrKey(rd);
  AddRace(rd);
  rd.m_name="GNOME";
  //rd.m_raceKey=Gnome;
  //AddRaceWithCurrKey(rd);
  AddRace(rd);
  rd.m_name="HALF-ELF";
  //rd.m_raceKey=HalfElf;
  //AddRaceWithCurrKey(rd);
  AddRace(rd);
  rd.m_name="HALFLING";
  //rd.m_raceKey=Halfling;
  //AddRaceWithCurrKey(rd);
  AddRace(rd);
}

RACE_DATA_TYPE::~RACE_DATA_TYPE(void)
{
}

/*
// called when loading binary race data that
// was saved with key info
raceType RACE_DATA_TYPE::AddRaceWithCurrKey(const RACE_DATA &data)
{
  CSingleLock sLock(&m_CS, TRUE);
  if (m_RaceData.GetCount() < MAX_RACES)
  {
    m_RaceData.Insert(data, data.m_raceKey);
    return (raceType)data.m_raceKey;
  }
  else
    return raceUnknown;
}
*/
int RACE_DATA_TYPE::GetDataSize(void) const
{
  int size=sizeof(RACE_DATA_TYPE);
  //POSITION pos;
  //for (pos=m_RaceData.GetHeadPosition();
  //     pos!=NULL;
  //     size+=m_RaceData.GetNext(pos).GetDataSize())
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    size += PeekRace(i)->GetDataSize();
  };     ;
  return size;
}

int RACE_DATA_TYPE::LocateRace(const RACE_ID& raceID) const
{
  int i, n;
  n = m_RaceData.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekRace(i)->m_name == raceID) return i;
  };
  return -1;
}

#ifdef UAFEDITOR
RACE_ID RACE_DATA_TYPE::FindPreVersionSpellNamesRaceID(DWORD id) const
{
  int i, n;
  n = GetRaceCount();
  for (i=0; i<n; i++)
  {
    const RACE_DATA *pRace;
    pRace = PeekRace(i);
    if (pRace->preSpellNameKey == (int)id)
    {
      return pRace->RaceID();
    };
  };
  {
    RACE_ID raceID;
    MsgBoxInfo("Searching for a Race that was defined by its 'key'.  We lost that information "
               "at about version 0.998101 of the editor.  If this is a problem for you, please "
               "contact the DC development team for help.");
    return raceID;
  };
}
#endif

void RACE_DATA_TYPE::PreSerialize(BOOL IsStoring)
{
  //POSITION pos;
  //pos=m_RaceData.GetHeadPosition();
  //while (pos!=NULL)
  //   m_RaceData.GetNext(pos).PreSerialize(IsStoring);
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    GetRace(i)->PreSerialize(IsStoring);
  };
}

void RACE_DATA_TYPE::PostSerialize(BOOL IsStoring)
{
}

int RACE_DATA_TYPE::Serialize(CAR& car)
{
  //POSITION p;
  CString version("RaceV3");
  int result=0;

  if (car.IsStoring())
  {
    //car << "RaceV0";
    car << version; // V1 is compressed; V2 is non-keyed databases
    car.Compress(true);
    car.WriteCount(GetCount());
    //p = m_RaceData.GetHeadPosition();
    //while (p != NULL)
    //  m_RaceData.GetNext(p).Serialize(car);
    {
      int i, n;
      n = GetCount();
      for (i=0; i<n; i++)
      {
        GetRace(i)->Serialize(car, version);
      };
    };
  }
  else
  {
    RACE_DATA data;
    int count;
    car >> version;
    if ((version =="RaceV0")||(version=="RaceV1")||(version=="RaceV2")||(version=="RaceV3"))
    {
      if (version > "RaceV0") car.Compress(true);
      Clear();
      count = car.ReadCount();
      for (int i=0; i<count; i++)
      {
        result=data.Serialize(car, version);
        if (result!=0) return result;
        //AddRaceWithCurrKey(data);
        AddRace(data);
      }
    }
    else
    {
      WriteDebugString("Unknown Race data version = %s",version);
      result=1;
    };
  };
  return result;
}
#ifdef UAFEDITOR
void RACE_DATA_TYPE::CrossReference(CR_LIST *pCRList)
{
  int i, n;
  n = GetCount();
  for (i = 0; i < n; i++)
  {
    GetRace(i)->CrossReference(pCRList);
  };
};
#endif

RACE_DATA *RACE_DATA::operator = (const RACE_DATA& src)
{
  if (this==&src)return this;
  //m_raceKey=src.m_raceKey;
  preSpellNameKey = src.preSpellNameKey;
	m_race_asl.Copy(src.m_race_asl);
	m_temp_asl.Copy(src.m_temp_asl);
  m_name=src.m_name;
  m_weight=src.m_weight;
  m_height=src.m_height;
  m_age          =src.m_age;
  m_maxAge       =src.m_maxAge;
  m_baseMovement =src.m_baseMovement;
  m_canChangeClass = src.m_canChangeClass;
  m_dwarfResistance = src.m_dwarfResistance;
  m_gnomeResistance = src.m_gnomeResistance;
  m_findSecretDoor = src.m_findSecretDoor;
  m_findSecretDoorSearching = src.m_findSecretDoorSearching;

  m_abilityRequired.SetSize(src.m_abilityRequired.GetSize());
    m_abilityRequired.Copy(src.m_abilityRequired);

  m_skills.SetSize(src.m_skills.GetSize());
    m_skills.Copy(src.m_skills);


  m_skillAdjustmentsBaseclass.SetSize(src.m_skillAdjustmentsBaseclass.GetSize());
    m_skillAdjustmentsBaseclass.Copy(src.m_skillAdjustmentsBaseclass);

  m_skillAdjustmentsAbility.SetSize(src.m_skillAdjustmentsAbility.GetSize());
    m_skillAdjustmentsAbility.Copy(src.m_skillAdjustmentsAbility);

  m_skillAdjustmentsRace.SetSize(src.m_skillAdjustmentsRace.GetSize());
    m_skillAdjustmentsRace.Copy(src.m_skillAdjustmentsRace);

  m_skillAdjustmentsScript.SetSize(src.m_skillAdjustmentsScript.GetSize());
    m_skillAdjustmentsScript.Copy(src.m_skillAdjustmentsScript);

  m_specAbs.Copy(src.m_specAbs);
  return this;
}

int RACE_DATA::LocateSkill(const SKILL_ID& skillID) const
{
  int i, n;
  n = m_skills.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekSkill(i)->skillID == skillID) return i;
  };
  return -1;
}

//int RACE_DATA::GetSkillValue(const SKILL_ID& skillID) const
void RACE_DATA::GetSkillValue(SKILL_COMPUTATION& SC) const
{
  const SKILL *pSkill;
  pSkill = PeekSkill(SC.skillID);
  if (pSkill == NULL) return;
  if (SC.raceValue == NoSkill) SC.raceValue = pSkill->value;
  else
  {
    if (SC.minimize) 
    {
      if (pSkill->value < SC.raceValue) SC.raceValue = pSkill->value;
    }
    else
    {
      if (pSkill->value > SC.raceValue) SC.raceValue = pSkill->value;
    };
  };
}

void RACE_DATA::UpdateSkillValue(SKILL_COMPUTATION& SC) const
{
  // Compute adjustments;
  // This includes adustments for Race, Baseclass, and Abilities.
  int i, n;
  //RACE_ID raceID;
  //raceID = SC.pChar->RaceID();
  n = GetSkillAdjBaseclassCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJUSTMENT_BASECLASS *pAdj;
    pAdj = PeekSkillAdjBaseclass(i);
    if (pAdj->skillID == SC.skillID)
    {
      if ((SC.baseclassID.IsEmpty()) || (SC.baseclassID == pAdj->baseclassID))
      {
        SC.baseclassLevel = SC.pChar->GetBaseclassLevel(pAdj->baseclassID);
        if (SC.baseclassLevel > 0)
        {
          //pAdj->UpdateSkillValue(baseclassLevel, baseVal, pBestBaseclassAdj, minimize);
          pAdj->UpdateSkillValue(SC);
        };
      };
    };
  };
  n = GetSkillAdjAbilityCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJUSTMENT_ABILITY *pAdj;
    pAdj = PeekSkillAdjAbility(i);
    if (pAdj->skillID == SC.skillID)
    {
      //pAdj->UpdateSkillValue(pChar, baseVal, minimize);
      pAdj->UpdateSkillValue(SC);
    };
  };
  n = GetSkillAdjRaceCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJUSTMENT_RACE *pAdj;
    pAdj = PeekSkillAdjRace(i);
    if (  (pAdj->skillID == SC.skillID) && (pAdj->raceID == RaceID()) )
    {
      //pAdj->UpdateSkillValue(baseVal, pBestRaceAdj);
      pAdj->UpdateSkillValue(SC);
    };
  };
}


// Called when loading from scripts, or
// when adding from editor GUI
//raceType RACE_DATA_TYPE::AddRace(const RACE_DATA &data)
void RACE_DATA_TYPE::AddRace(const RACE_DATA &data)
{
  RACE_ID raceID;
  //CSingleLock sLock(&m_CS, TRUE);
  //if (data.m_name.IsEmpty()) return raceUnknown;
  if (data.m_name.IsEmpty()) return;
  // Do not allow multiple items with same full name.
  raceID = data.m_name;
  //if (m_FindName(data.m_name)!=raceUnknown) return raceUnknown;
  if (LocateRace(raceID) >= 0) return;
  if (GetCount() < MAX_RACES)
  {
    //data.m_raceKey = GetNextRace();
    //m_RaceData.Insert(data, data.m_raceKey);
    m_RaceData.Add(const_cast<RACE_DATA&>(data));
    //return (raceType)data.m_raceKey;
    return;
  }
  else
    //return raceUnknown;
    return;
}

//DWORD RACE_DATA_TYPE::GetWeight(raceType race)
DWORD RACE_DATA_TYPE::GetWeight(const RACE_ID& raceID) const
{
  //ASSERT ( race < NUM_RACE_TYPES );
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.GetAtPos(pos).GetWeight();
  //}
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->GetWeight();
  }
  else
    return 0;
}

//DWORD RACE_DATA_TYPE::GetHeight(raceType race) const
DWORD RACE_DATA_TYPE::GetHeight(const RACE_ID& raceID) const
{
  //ASSERT ( race < NUM_RACE_TYPES );
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.GetAtPos(pos).GetHeight();
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->GetHeight();
  }
  else
    return 0;
}

//DWORD RACE_DATA_TYPE::GetStartAge(raceType race) const
DWORD RACE_DATA_TYPE::GetStartAge(const RACE_ID& raceID) const
{
//  ASSERT ( race < NUM_RACE_TYPES );
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.GetAtPos(pos).GetStartAge();
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->GetStartAge();
  }
  else
    return 0;
}

//DWORD RACE_DATA_TYPE::GetMaxAge(raceType race) const
DWORD RACE_DATA_TYPE::GetMaxAge(const RACE_ID& raceID) const
{
//  ASSERT ( race < NUM_RACE_TYPES );
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.GetAtPos(pos).GetMaxAge();
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->GetMaxAge();
  }
  else
    return 0;
}

//DWORD RACE_DATA_TYPE::GetBaseMovement(raceType race) const
DWORD RACE_DATA_TYPE::GetBaseMovement(const RACE_ID& raceID) const
{
  //ASSERT ( race < NUM_RACE_TYPES );
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.GetAtPos(pos).GetBaseMovement();
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->GetBaseMovement();
  }
  else
    return 0;
}

/*
//short RACE_DATA_TYPE::GetMinAbility(raceType race,abilityType type) const
short RACE_DATA_TYPE::GetMinAbility(const RACE_ID& raceID, abilityType type) const
{
//  ASSERT ( race < NUM_RACE_TYPES );
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.PeekAtPos(pos).GetMinAbility(type);
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->GetMinAbility(type);
  }
  else
    return 0;
}
*/

/*
//short RACE_DATA_TYPE::GetMaxAbility(raceType race,abilityType type) const
short RACE_DATA_TYPE::GetMaxAbility(const RACE_ID& raceID, abilityType type) const
{
//  ASSERT ( race < NUM_RACE_TYPES );
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.PeekAtPos(pos).GetMaxAbility(type);
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->GetMaxAbility(type);
  }
  else
    return 0;
}
*/


//CString RACE_DATA_TYPE::GetRaceName(raceType race)
CString RACE_DATA_TYPE::GetRaceName(const RACE_ID& raceID)
{
 // ASSERT ( race < NUM_RACE_TYPES );
  //CSingleLock sLock(&m_CS,TRUE);
  //POSITION pos;
  //if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  //{
  //  return m_RaceData.PeekAtPos(pos).Name();
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
    return PeekRace(index)->m_name;
  }
  else
    return CString("No Race");
}


/*
RACE_DATA *RACE_DATA_TYPE::GetRaceData(raceType race)
{
  ASSERT ( race < NUM_RACE_TYPES );
   CSingleLock sLock(&m_CS,TRUE);
  POSITION pos;
  if ((pos = m_RaceData.FindKeyPos(race)) != NULL)
  {
    return &m_RaceData.GetAtPos(pos);
  }
  else
    return NULL;
}
*/
/*
// No locking... for internal use
raceType RACE_DATA_TYPE::m_FindName (const CString& name)
// Return key associated with the named object, or zero
{
  POSITION pos;
  pos=m_RaceData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_RaceData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return (raceType)m_RaceData.PeekAtPos(pos).m_raceKey;
    m_RaceData.GetNext(pos);
  };
  return raceUnknown;
}
*/
/*
RACE_DATA *RACE_DATA_TYPE::GetRaceData(const CString& name)
{
  POSITION pos;
  pos=m_RaceData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_RaceData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return &m_RaceData.GetAtPos(pos);
    m_RaceData.GetNext(pos);
  };
  return NULL;
}
*/
/*
// With locking...for public use
raceType RACE_DATA_TYPE::FindName(const CString& name)
// Return key associated with the named object, or zero
{
  CSingleLock sLock(&m_CS,TRUE);
  return m_FindName(name);
}
*/
/*
raceType RACE_DATA_TYPE::GetNextRace()
{
  raceType key=raceUnknown;
  if (GetCount() > 0)
  {
    key = (raceType)m_RaceData.GetTail().m_raceKey;
    if (key >= INT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is much greater than
      // MAX_RACES.
      // Gaps occur when deletions are made to the races list
      POSITION pos = m_RaceData.GetHeadPosition();
      BOOL found = FALSE;
      DWORD prevKey = 0;
      DWORD currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = m_RaceData.GetNext(pos).m_raceKey;
        if (currKey != prevKey+1)
        {
          key = (raceType)(prevKey+1);
          found = TRUE;
        }
        else
          prevKey = currKey;
      }
      VERIFY(found); // should never happen
    }
    else
      key=(raceType)(key+1);
  }
  else
    key = (raceType)0;
  return key;
}
*/
//bool RACE_DATA_TYPE::IsAllowedClass(raceType race,const CString& className) const
bool RACE_DATA_TYPE::IsAllowedClass(const RACE_ID& raceID, const CString& className) const
{
  //POSITION p;
  //p = m_RaceData.FindKeyPos(race);
  //if (p == NULL) return false;
  int index;
  index = LocateRace(raceID);
  if (index >= 0)
  {
  //return m_RaceData.PeekAtPos(p).IsAllowedClass(className);
    CLASS_ID classID;
    classID = className;
    return PeekRace(index)->IsAllowedClass(classID);
  }
  else
  {
    return false;
  };
}

BOOL RACE_DATA_TYPE::HasGnomeResistance(const RACE_ID& raceID) const
{
  int i;
  i = LocateRace(raceID);
  if (i < 0) return false;
  return PeekRace(i)->m_gnomeResistance;
}

BOOL RACE_DATA_TYPE::HasDwarfResistance(const RACE_ID& raceID) const
{
  int i;
  i = LocateRace(raceID);
  if (i < 0) return false;
  return PeekRace(i)->m_dwarfResistance;
}

//*****************************************************************************

ABILITY_DATA::ABILITY_DATA (void):m_specAbs(true) 
{
  Clear();
}

ABILITY_DATA::~ABILITY_DATA (void)
{
}

bool ABILITY_DATA::operator == (const ABILITY_DATA& src) const
{
  //if (!(m_abilityKey==src.m_abilityKey)) return false;
  if (!(m_name==src.m_name)) return false;
  if (!(m_abbreviation==src.m_abbreviation)) return false;
  if (!(m_roll==src.m_roll)) return false;
  if (!(m_modify==src.m_modify)) return false;
  if (!(m_specAbs==src.m_specAbs)) return false;
  return true;
}


ABILITY_DATA& ABILITY_DATA::operator = (const ABILITY_DATA& src)
{
  //m_abilityKey=src.m_abilityKey;
  m_name=src.m_name;
  m_abbreviation=src.m_abbreviation;
  m_roll=src.m_roll;
  m_modify=src.m_modify;
  m_specAbs.Copy(src.m_specAbs);
  return *this;
}


void ABILITY_DATA::PreSerialize(BOOL IsStoring)
{
  m_roll.PreSerialize(IsStoring);
}

int ABILITY_DATA::Serialize(CAR& car, const CString& version)
{
  int result=0;
  CString tempStr("Abd0");
  if (car.IsStoring())
  {
    car << tempStr;  // Version
    //car << m_abilityKey;
    if (m_name=="") m_name="*";
    if (m_abbreviation=="") m_abbreviation="*";
    car << m_name;
    car << m_abbreviation;
    if (m_name=="*") m_name="";
    if (m_abbreviation=="*") m_abbreviation="";
    m_roll.Serialize(car);
  }
  else
  {
    Clear();
    car >> tempStr;
    if (tempStr=="Abd0")
    {
#ifdef UAFEDITOR
      if (globalData.version < VersionSpellNames)
      {
        DWORD junk;
        car >> junk;
      //car >> m_abilityKey;
      };
#endif
      car >> m_name; if (m_name=="*") m_name="";
      car >> m_abbreviation; if (m_abbreviation=="*") m_abbreviation="";
      result=m_roll.Serialize(car);
    }
    else
    {
      WriteDebugString("Unknown ABILITY_DATA serialize version = %s\n",tempStr);
      result=1;
    };
  };
  if (globalData.version >= _SPECIAL_ABILITIES_VERSION_)
  {
    m_specAbs.Serialize(car, globalData.version, this->m_name, "Ability");
  };
  return result;
}

void ABILITY_DATA::Clear(void)
{
  //m_abilityKey=0;
  m_name="";
  m_abbreviation="";
  m_modify=0;
  m_roll.Clear();
  m_specAbs.Clear();
}

int ABILITY_DATA::GetDataSize(void) const
{
  int size=sizeof(ABILITY_DATA);
  size += m_name.GetLength();
  size += m_abbreviation.GetLength();
  size += m_roll.GetDataSize();
  size += m_specAbs.GetDataSize();
  return size;
}
/*
void ABILITY_DATA::AddAdjust(ADJUSTMENT& adjust)
{
  m_roll.AddAdjust(adjust);
}
*/

void ABILITY_DATA::AddAdjust(CString &name)
{
  m_roll.AddAdjust(name);
}

BOOL ABILITY_DATA::RollAbility(double &result) const
{
  return (m_roll.Roll(result));

  /*
  BOOL success=TRUE;
  if ((m_roll.m_numSides==0)&&(m_roll.m_numDice==0)&&(m_roll.m_min==-999)&&(m_roll.m_max==999))
    result = m_roll.m_bonus;
  else
    result = RollDice(m_roll.m_numSides,m_roll.m_numDice,m_roll.m_bonus);

  result*=m_roll.m_sign;

  double tmp=result;

  int num = m_roll.m_adjust.GetSize();
  double adj=0.0;
  for (int i=0;i<num;i++)
  {
    if (m_roll.m_adjust[i].HasTwoOperands())
    {
      // There should be only one of these, if any.
      // It should also be the first adjustment listed.
      if (!m_roll.m_adjust[i].EvaluateBinary(adj))
        success = FALSE;
    }
    else
    {
      if (!m_roll.m_adjust[i].Evaluate(adj))
        success=FALSE;
    }
    tmp += adj;
  }

  if (success)
    result = tmp;

  if (success)
  {
    if (m_roll.m_sign >= 0)
    {
      if (result < m_roll.m_min) result=m_roll.m_min;
      if ((m_roll.m_max > 0) && (result > m_roll.m_max)) result=m_roll.m_max;
    }
    else
    {
      if ((m_roll.m_max > 0) && (result < -m_roll.m_max)) result=-m_roll.m_max;
      if (result > -m_roll.m_min) result=-m_roll.m_min;
    }
  }
  return success;
  */
}


//*****************************************************************************


ABILITY_DATA_TYPE::ABILITY_DATA_TYPE (void)
{
  LoadUADefaults();
}

void ABILITY_DATA_TYPE::LoadUADefaults(void)
{ // You have to do your own Clear() if that is what you want.
  ABILITY_DATA abdat;
  abdat.Clear();
    abdat.m_name=Ability_Strength;
    abdat.m_abbreviation="STR";
    AddAbility(abdat);
  abdat.Clear();
    abdat.m_name=Ability_Intelligence;
    abdat.m_abbreviation="INT";
    AddAbility(abdat);
  abdat.Clear();
    abdat.m_name=Ability_Wisdom;
    abdat.m_abbreviation="WIS";
    AddAbility(abdat);
  abdat.Clear();
    abdat.m_name=Ability_Dexterity;
    abdat.m_abbreviation="DEX";
    AddAbility(abdat);
  abdat.Clear();
    abdat.m_name=Ability_Constitution;
    abdat.m_abbreviation="CON";
    AddAbility(abdat);
  abdat.Clear();
    abdat.m_name=Ability_Charisma;
    abdat.m_abbreviation="CHA";
    AddAbility(abdat);
}

ABILITY_DATA_TYPE::~ABILITY_DATA_TYPE(void)
{
}

void ABILITY_DATA_TYPE::ClearAdjustments(double val)
{
  int i,n;
  n = GetCount();
  for(i=0; i<n; i++)
  {
    GetAbilityData(i)->bestSkillAdj = val;
  };
}

double ABILITY_DATA_TYPE::TotalAdjustments(double val)
{
  int i,n;
  double result;
  n = GetCount();
  result = val;
  for(i=0; i<n; i++)
  {
    ABILITY_DATA *pab;
    pab = GetAbilityData(i);
    if (pab->bestSkillAdj != val) 
    {
      if (result == val) result = pab->bestSkillAdj;
      else               result += pab->bestSkillAdj;
    };
  };
  return result;
}


void ABILITY_DATA_TYPE::Clear(void)
{
  //m_AbilityData.RemoveAll();
  m_abilityData.RemoveAll();
//  m_numAbilities=0;
}
/*
// With locking...for public use
abilityType ABILITY_DATA_TYPE::FindName(const CString& name)
// Return key associated with the named object, or zero
{
//  CSingleLock sLock(&m_CS,TRUE);
  return m_FindName(name);
}
*/
/*
int ABILITY_DATA_TYPE::GetDataSize(void)
{
  POSITION pos;
  int size= sizeof(ABILITY_DATA_TYPE);
  pos=m_AbilityData.GetHeadPosition();
  while (pos!= NULL) size += m_AbilityData.GetNext(pos).GetDataSize();
  return size;
}
*/
int ABILITY_DATA_TYPE::GetDataSize(void)
{
  int size=0, i,n;
  n = GetAbilityDataCount();
  for (i=0; i<n; i++)
  {
    size += PeekAbilityData(i)->GetDataSize();
  };
  return size;
}


/*
// called when loading binary ability data that
// was saved with key info
abilityType ABILITY_DATA_TYPE::AddAbilityWithCurrKey(const ABILITY_DATA &data)
{
//  CSingleLock sLock(&m_CS, TRUE);
  if (m_AbilityData.GetCount() < MAX_ABILITIES)
  {
    m_AbilityData.Insert(data, data.m_abilityKey);
    return (abilityType)data.m_abilityKey;
  }
  else
    return abilityUnknown;
}
*/

//abilityType ABILITY_DATA_TYPE::AddAbility(const ABILITY_DATA& newab)
void ABILITY_DATA_TYPE::AddAbility(const ABILITY_DATA& newab)
// Called when loading from scripts, or
// when adding from editor GUI
{
//  CSingleLock sLock(&m_CS, TRUE);
  //if (newab.m_name.IsEmpty()) return abilityUnknown;
  if (newab.m_name.IsEmpty()) return;
  // Do not allow multiple items with same full name.
  //if (m_FindName(newab.m_name)!=abilityUnknown) return abilityUnknown;
  if (LocateAbilityData(newab.m_name) >= 0) return;
  //if (m_AbilityData.GetCount() < MAX_ABILITIES)
  if (GetAbilityDataCount() < MAX_ABILITIES)
  {
    //newab.m_abilityKey = GetNextAbility();
    //m_AbilityData.Insert(newab, newab.m_abilityKey);
    m_abilityData.Add(const_cast<ABILITY_DATA&>(newab));
    //return (abilityType)newab.m_abilityKey;
  }
  //else
  //  return abilityUnknown;
}
/*
abilityType ABILITY_DATA_TYPE::GetNextAbility()
{
  abilityType key=abilityUnknown;
  if (GetCount() > 0)
  {
    key = (abilityType)m_AbilityData.GetTail().m_abilityKey;
    if (key >= UINT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is much greater than
      // MAX_ABILITIES.
      // Gaps occur when deletions are made to the abilities list
      POSITION pos = m_AbilityData.GetHeadPosition();
      BOOL found = FALSE;
      DWORD prevKey = 0;
      DWORD currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = m_AbilityData.GetNext(pos).m_abilityKey;
        if (currKey != prevKey+1)
        {
          key = (abilityType)(prevKey+1);
          found = TRUE;
        }
        else
          prevKey = currKey;
      }
      VERIFY(found); // should never happen
    }
    else
      key=(abilityType)(key+1);
  }
  else
    key = (abilityType)0;
  return key;
}
*/
/*
// No locking... for internal use
abilityType ABILITY_DATA_TYPE::m_FindName (const CString& name) const
// Return key associated with the named object, or abilityUnknown
{
  POSITION pos;
  pos=m_AbilityData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_AbilityData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return (abilityType)m_AbilityData.PeekAtPos(pos).m_abilityKey;
    m_AbilityData.PeekNext(pos);
  };
  return abilityUnknown;
}
*/

/*
ABILITY_DATA *ABILITY_DATA_TYPE::GetAbilityData(const CString& name)
{
  POSITION pos;
  pos=m_AbilityData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_AbilityData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return &m_AbilityData.GetAtPos(pos);
    m_AbilityData.GetNext(pos);
  };
  return NULL;
}
*/

/*
abilityType ABILITY_DATA_TYPE::GetKeyByIndex(int index) const
{
  if (index < 0) return abilityUnknown;
  POSITION pos;
  pos=m_AbilityData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (index <= 0)
      return ((abilityType)m_AbilityData.PeekAtPos(pos).m_abilityKey);
     index--;
    m_AbilityData.PeekNext(pos);
  };
  return abilityUnknown;
}
*/

void ABILITY_DATA_TYPE::PreSerialize(BOOL IsStoring)
{
  //POSITION pos;
  //pos=m_AbilityData.GetHeadPosition();
  int i, n;
  n = GetAbilityDataCount();
  //while (pos!=NULL)
  for (i=0; i<n; i++)
  {
     //m_AbilityData.GetNext(pos).PreSerialize(IsStoring);
     GetAbilityData(i)->PreSerialize(IsStoring);
  };
}

void ABILITY_DATA_TYPE::PostSerialize(BOOL IsStoring)
{
}

int ABILITY_DATA_TYPE::Serialize(CAR& car)
{
  //POSITION p;
  //CString version("AbilityV1");
  CString version("AbilityV2");  // V2 uses ABILITY_ID instead of key

  int i, n, result=0;
  if (car.IsStoring())
  {
//    car << "AbilityV0";
    car << version; // V1 is compressed
    car.Compress(true);
    car << GetCount();
    //p = m_AbilityData.GetHeadPosition();
    n = GetCount();
    //while (p != NULL)
    for (i=0; i<n; i++)
    {
      GetAbilityData(i)->Serialize(car, version);
    };
  }
  else
  {
    ABILITY_DATA data;
    int count;
    car >> version;
    if ((version =="AbilityV0")||(version=="AbilityV1")||(version=="AbilityV2"))
    {
      if (version > "AbilityV0") car.Compress(true);
      car >> count;
      for (i=0; i<count; i++)
      {
        result=data.Serialize(car, version);
        if (result!=0) break;
        //AddAbilityWithCurrKey(data);
        AddAbility(data);
      }
    }
    else
    {
      WriteDebugString("Unknown Ability data version = %s",version);
      result=1;
    };
  }
  return result;
}

int ABILITY_DATA_TYPE::LocateAbilityData(const ABILITY_ID& abilityID) const
{
  int i, n;
  n = m_abilityData.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekAbilityData(i)->m_name == abilityID) return i;
  };
  return -1;
}


int ABILITY_DATA_TYPE::LocateAbilityData(const CString& abilityName) const
{
  int i, n;
  n = m_abilityData.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekAbilityData(i)->m_name == abilityName) return i;
  };
  return -1;
}

/*
CString ABILITY_DATA_TYPE::GetName(abilityType type) const
{
  POSITION pos;
  if ((pos = m_AbilityData.FindKeyPos(type)) != NULL)
  {
    return (m_AbilityData.PeekAtPos(pos).m_name);
  }
  else
    return "";
}
*/

//CString ABILITY_DATA_TYPE::GetShortName(abilityType type) const
CString ABILITY_DATA_TYPE::GetShortName(const CString& abilityName) const
{
  int i;
  //POSITION pos;
  //if ((pos = m_AbilityData.FindKeyPos(type)) != NULL)
  if ((i = LocateAbilityData(abilityName)) >= 0)
  {
    //return (m_AbilityData.PeekAtPos(pos).m_abbreviation);
    return (PeekAbilityData(i)->m_abbreviation);
  }
  else
    return "";
}

//BOOL ABILITY_DATA_TYPE::RollAbility(abilityType ability, double &result)
BOOL ABILITY_DATA_TYPE::RollAbility(const CString& abilityName, double &result) const
{
  int i;
//  CSingleLock sLock(&m_CS,TRUE);
  //POSITION pos;
  //if ((pos = m_AbilityData.FindKeyPos(ability)) != NULL)
  if ((i=LocateAbilityData(abilityName)) >= 0)
  {
    //return (m_AbilityData.GetAtPos(pos).RollAbility(result));
    return (PeekAbilityData(i)->RollAbility(result));
  }
  else
    return FALSE;

  /*
  int count=0;
  POSITION pos = m_AbilityData.GetHeadPosition();
  while (pos != NULL)
  {
    if (count==index)
    {
      double result;
      if (m_AbilityData.GetAt(pos).RollAbility(result))
        return result;
      return 0.0;
    }
    m_AbilityData.GetNext(pos);
    count++;
  }
  return 0.0;
  */
}


//*****************************************************************************


BASE_CLASS_DATA::BASE_CLASS_DATA(void):m_specAbs(true) 
{
  Clear();
}

BASE_CLASS_DATA::~BASE_CLASS_DATA(void)
{
}

void BASE_CLASS_DATA::Clear(void)
{
  //m_baseclassKey=baseclassUnknown;
  int i;
  m_preSpellNameKey = -1;
  m_abilityReq.RemoveAll();
  m_allowedAlignments = 0x1ff; // any
  m_allowedRaces.RemoveAll();
  //m_backstabAbility.Empty();
  //m_backstabLevel = 999;
  m_bonusSpells.RemoveAll();
  m_castingInfo.RemoveAll();
  m_expLevels.RemoveAll();
  //m_maxSpellLevelByPrime.RemoveAll();
  m_name="";
  //m_primeReq.RemoveAll();
  //m_rangerBonusLevel = 999;
  m_skills.RemoveAll();
  m_skillAdjustmentsAbility.RemoveAll();
  m_skillAdjustmentsBaseclass.RemoveAll();
  m_skillAdjustmentsRace.RemoveAll();
  m_bonusXP.RemoveAll();
  m_specAbs.Clear();
  m_spellBonusAbility.Empty();
//  m_turnUndeadLevel = 999;
  for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++) hitDice[i].Clear();
  memset(THAC0,0,sizeof(THAC0));
}

int BASE_CLASS_DATA::GetDataSize(void) const
{
  int i,n;
  int size=sizeof (*this);
  size += m_name.GetLength();
  size += m_expLevels.GetSize() * sizeof(WORD);
  //n=m_primeReq.GetSize();
  //for (i=0; i<n; i++) size += m_primeReq[i].GetDataSize();
  //for (i=0; i<n; i++) size += sizeof(*PeekAbilityName(i))+PeekAbilityName(i)->GetLength();
  n=m_abilityReq.GetSize();
  for (i=0; i<n; i++)
  {
    size += PeekAbilityReq(i)->GetDataSize();
    //size += m_abilityReq[i].GetDataSize();
  };
  n=m_allowedRaces.GetSize();
  //for (i=0; i<n; i++) size += m_allowedRaces[i].GetDataSize();
  for (i=0; i<n; i++) size += PeekRaceID(i)->GetDataSize();
  return size;
}

BASE_CLASS_DATA& BASE_CLASS_DATA::operator =(const BASE_CLASS_DATA& src)
{
  int i, n;
  if (this==&src) return *this;
  Clear();
  m_preSpellNameKey = src.m_preSpellNameKey;
  //m_baseclassKey=src.m_baseclassKey;
  m_name        =src.m_name;
  //n=src.m_primeReq.GetSize();
  //  m_primeReq.SetSize(n);
  //  m_primeReq.Copy(src.m_primeReq);
  //m_primeCasting = src.m_primeCasting;
  //m_backstabAbility = src.m_backstabAbility;
  //m_backstabLevel = src.m_backstabLevel;
  //m_rangerBonusLevel = src.m_rangerBonusLevel;
  n=src.m_abilityReq.GetSize();
    m_abilityReq.SetSize(n);
    m_abilityReq.Copy(src.m_abilityReq);
  n=src.m_allowedRaces.GetSize();
    m_allowedRaces.SetSize(n);
    m_allowedRaces.Copy(src.m_allowedRaces);
  n=src.m_expLevels.GetSize();
    m_expLevels.SetSize(n);
    m_expLevels.Copy(src.m_expLevels);
  m_allowedAlignments=src.m_allowedAlignments;
  this->m_specAbs.Copy(src.m_specAbs);
  m_spellBonusAbility = src.m_spellBonusAbility;
  m_bonusSpells.RemoveAll();
  m_bonusSpells.Append(src.m_bonusSpells);
  n = src.m_skills.GetSize();
    m_skills.SetSize(n);
    m_skills.Copy(src.m_skills);
  n=src.m_skillAdjustmentsAbility.GetSize();
    m_skillAdjustmentsAbility.SetSize(n);
    m_skillAdjustmentsAbility.Copy(src.m_skillAdjustmentsAbility);
  n=src.m_skillAdjustmentsBaseclass.GetSize();
    m_skillAdjustmentsBaseclass.SetSize(n);
    m_skillAdjustmentsBaseclass.Copy(src.m_skillAdjustmentsBaseclass);
  n=src.m_skillAdjustmentsRace.GetSize();
    m_skillAdjustmentsRace.SetSize(n);
    m_skillAdjustmentsRace.Copy(src.m_skillAdjustmentsRace);
  n=src.m_bonusXP.GetSize();
    m_bonusXP.SetSize(n);
    m_bonusXP.Copy(src.m_bonusXP);
  n = src.GetCastingInfoCount();
    m_castingInfo.SetSize(n);
    m_castingInfo.Copy(src.m_castingInfo);
  for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
  {
    THAC0[i]   = src.THAC0[i];
    hitDice[i] = src.hitDice[i];
  };

  return *this;
}




BOOL BASE_CLASS_DATA::operator ==(const BASE_CLASS_DATA& src) const
{
  int i, n;
  if (this==&src) return TRUE;
  if (m_name != src.m_name) return FALSE;
  n = m_abilityReq.GetSize();
  if (n != src.m_abilityReq.GetSize()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekAbilityReq(i) == *src.PeekAbilityReq(i))) return FALSE;
  }
  n = m_allowedRaces.GetSize();
  if (n != src.m_allowedRaces.GetSize()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekRaceID(i) == *src.PeekRaceID(i))) return FALSE;
  };
  n = m_expLevels.GetSize();
  if (n != src.m_expLevels.GetSize()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekExpLevels(i) == *src.PeekExpLevels(i))) return FALSE;
  };
  if (m_allowedAlignments !=src.m_allowedAlignments) return FALSE;
  if (m_spellBonusAbility != src.m_spellBonusAbility) return FALSE;
  n = m_bonusSpells.GetSize();
  if (n != src.m_bonusSpells.GetSize())return FALSE;
  for (i=0; i<n; i++)
  {
    if (m_bonusSpells[i] != src.m_bonusSpells[i]) return FALSE;
  };
  n = m_skillAdjustmentsAbility.GetSize();
  if (n != src.m_skillAdjustmentsAbility.GetSize()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekSkillAdjAbility(i) == *src.PeekSkillAdjAbility(i))) return FALSE;
  };
  n = m_skillAdjustmentsBaseclass.GetSize();
  if (n != src.m_skillAdjustmentsBaseclass.GetSize()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekSkillAdjBaseclass(i) == *src.PeekSkillAdjBaseclass(i))) return FALSE;
  };
  n = m_skillAdjustmentsRace.GetSize();
  if (n != src.m_skillAdjustmentsRace.GetSize()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekSkillAdjRace(i) == *src.PeekSkillAdjRace(i))) return FALSE;
  };
  n = m_bonusXP.GetSize();
  if (n != src.m_bonusXP.GetSize()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekBonusXP(i) == *src.PeekBonusXP(i))) return FALSE;
  };
  for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
  {
    if (!(hitDice[i] == src.hitDice[i])) return FALSE;
    if (THAC0[i]     != src.THAC0[i])    return FALSE;
  };
  return TRUE;
}




void BASE_CLASS_DATA::GetHitDice(int level, int *sides, int *numDice, int *constant) const
{
  if (level > HIGHEST_CHARACTER_LEVEL) level = HIGHEST_CHARACTER_LEVEL;
  if (level < 1) level = 1;
  *sides    = hitDice[level-1].sides;
  *numDice  = hitDice[level-1].nbr;
  *constant = hitDice[level-1].bonus;
}

void BASE_CLASS_DATA::DetermineCharHitDice(int *totalDice, int *maxDice, int level) const
{
  int i, iDice;
  int sides, numDice, constant;
  float fDice = 0.0;
  for (i=1; i<=level; i++)
  {
    GetHitDice(level, &sides, &numDice, &constant);
    fDice += RollDice(sides, numDice, constant);
  };
  iDice = fDice + 0.5;
  *totalDice += iDice;
  if (iDice > *maxDice) *maxDice = iDice;
}

/*
//void BASE_CLASS_DATA::AddPrimeReq(const CString& reqName)
void BASE_CLASS_DATA::AddPrimeReq(const ABILITY_ID& abilityID)
{
//  int n, i;
//  n=m_primeReq.GetSize();
//  for (i=0; i<n; i++)
//  {
//    //if (m_primeReq[i].m_abilityName==reqName) return;
//    if (*PeekAbilityName(i) == reqName) return;
//  };
//  //ABILITY_REFERENCE ar;
//  //ar.m_abilityName=reqName;
//  //m_primeReq.SetAtGrow(n, ar);
//  m_primeReq.Add(const_cast<CString&>(reqName));
  m_primeReq.Add(const_cast<ABILITY_ID&>(abilityID));
}
*/

//void BASE_CLASS_DATA::AddAllowedRace(RACE_REFERENCE& race)
void BASE_CLASS_DATA::AddAllowedRace(const RACE_ID& raceID)
{
  //int n, i;
  //n=m_allowedRaces.GetSize();
  //for (i=0; i<n; i++)
  //{
  //  //if (m_allowedRaces[i].m_raceName==race.m_raceName) return;
  //  if (m_allowedRaces[i].m_raceID == race.m_raceID) return;
  //};
  if (LocateRaceID(raceID) >= 0) return;
  //m_allowedRaces.SetAtGrow(n, race);
  m_allowedRaces.Add(const_cast<RACE_ID&>(raceID));
}

int BASE_CLASS_DATA::LocateSkill(const SKILL_ID& skillID) const
{
  int i, n;
  n = GetSkillCount();
  for (i=0; i<n; i++)
  {
    if (PeekSkill(i)->skillID == skillID) return i;
  };
  return -1;
}

void BASE_CLASS_DATA::PreSerialize(BOOL IsStoring)
{
  int i,n;
  //n=m_primeReq.GetSize();
  //for (i=0; i<n; i++) m_primeReq[i].PreSerialize(IsStoring);
  n=m_abilityReq.GetSize();
  for (i=0; i<n; i++)
  {
    GetAbilityReq(i)->PreSerialize(IsStoring);
    //m_abilityReq[i].PreSerialize(IsStoring);
  };
  //n=m_allowedRaces.GetSize();
  //for (i=0; i<n; i++) m_allowedRaces[i].PreSerialize(IsStoring);
}

BASECLASS_STATS::BASECLASS_STATS(void)
{
  currentLevel = 0;
  previousLevel = 0;
  preDrainLevel = 0;
  x_experience = 0;
  pBaseclassData = NULL;
}


CArchive& BASECLASS_STATS::Serialize(CArchive& ar)
{
  die("BASECLASS_STATS should not serialize to CArchive"); // Not Implemented(0x2e4da65, false);
  return ar;
}

CAR& BASECLASS_STATS::Serialize(CAR& car, const CString& version)
{
  if (car.IsStoring())
  {
    car << baseclassID;
    car << currentLevel;
    car << previousLevel;
    car << preDrainLevel;
    car << x_experience;
  }
  else
  {
    car >> baseclassID;
    car >> currentLevel;
    car >> previousLevel;
    car >> preDrainLevel;
    car >> x_experience;
  };
  return car;
}

int BASECLASS_STATS::CurExperience(void) const
{
  if (previousLevel > 0) return 0;
  return x_experience;
}

int BASECLASS_STATS::IncCurExperience(int exp)
{
  if (previousLevel > 0) return 0;
  return x_experience += exp;
}
#ifdef UAFEDITOR

const char *JKEY_BASECLASSID = "baseclassID";
const char *JKEY_CURRENTLEVEL = "currentLevel";
const char *JKEY_PREVIOUSLEVEL = "previousLevel";
const char *JKEY_PREDRAINLEVEL = "preDrainLevel";
extern const char *JKEY_EXPERIENCE;

void BASECLASS_STATS::Export(JWriter& jw)
{
  jw.StartList();
  jw.NameAndValue(JKEY_BASECLASSID, baseclassID);
  jw.NameAndValue(JKEY_CURRENTLEVEL, currentLevel);
  jw.NameAndValue(JKEY_PREVIOUSLEVEL, previousLevel);
  jw.NameAndValue(JKEY_PREDRAINLEVEL, preDrainLevel);
  jw.NameAndValue(JKEY_EXPERIENCE, x_experience);
  jw.EndList();
}
void BASECLASS_STATS::Import(JReader& jr)
{
  jr.StartList();
  jr.NameAndValue(JKEY_BASECLASSID, baseclassID);
  jr.NameAndValue(JKEY_CURRENTLEVEL, currentLevel);
  jr.NameAndValue(JKEY_PREVIOUSLEVEL, previousLevel);
  jr.NameAndValue(JKEY_PREDRAINLEVEL, preDrainLevel);
  jr.NameAndValue(JKEY_EXPERIENCE, x_experience);
  jr.EndList();
}
#endif

/*
SAVE_VS::SAVE_VS(const CString& versus, const BYTE *s)
{
  int level;
  this->versusName = versus;
  for (level=1; level<21; level++)
  {
    this->save[level-1] = s[level];
  };
  for (level=21; level<=HIGHEST_CHARACTER_LEVEL; level++)
  {
    this->save[level-1] = s[21];
  };
}
*/

void SKILL::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << skillID;
    car << value;
    return;
  };
  car >> skillID;
  car >> value;
}

CAR& SKILL_ADJ::Serialize(CAR& car, const CString& version)
{
  if (car.IsStoring())
  {
    car << skillID;
    car << adjID;
    car << value;
    car << type;
  }
  else
  {
    car >> skillID;
    car >> adjID;
    car >> value;
    car >> type;
  };
  return car;
}


const char *JKEY_SKILLID          = "SkillID";
const char *JKEY_ADJUSTMENTID     = "AdjustmentID";
const char *JKEY_ADJUSTMENTAMOUNT = "AdjustmentAmount";
const char *JKEY_ADJUSTMENTTYPE   = "AdjustmentType";


#ifdef UAFEDITOR
void SKILL_ADJ::Export(JWriter& jw)
{
  CString temp;
  jw.StartList();
  jw.NameAndValue(JKEY_SKILLID, skillID);
  jw.NameAndQuotedValue(JKEY_ADJUSTMENTID, adjID);
  jw.NameAndValue(JKEY_ADJUSTMENTAMOUNT, value);
  temp = type;
  jw.NameAndValue(JKEY_ADJUSTMENTTYPE, temp);
  jw.EndList();
}
void SKILL_ADJ::Import(JReader& jr)
{
  CString temp;
  jr.StartList();
  jr.NameAndValue(JKEY_SKILLID, skillID);
  jr.NameAndQuotedValue(JKEY_ADJUSTMENTID, adjID);
  jr.NameAndValue(JKEY_ADJUSTMENTAMOUNT, value);
  jr.NameAndValue(JKEY_ADJUSTMENTTYPE, temp);
  if (temp.GetLength() > 0)
  {
    type = temp[0];
  }
  else
  {
    type = ' ';
  };
  jr.EndList();
}
#endif


//void SKILL_ADJUSTMENT_RACE::UpdateSkillValue(double baseVal, double *pBestAdjRace) const
void SKILL_ADJUSTMENT_RACE::UpdateSkillValue(SKILL_COMPUTATION& SC) const
{
  double adj;
  adj = 0.0;
  switch (adjType)
  {
  case '%':
    adj = (SC.baseVal * skillAdj) / 100.0 - SC.baseVal; break;
  case '+':
    adj = skillAdj;                                     break;
  case '-':
    adj = -skillAdj;                                     break;
  case '=':
    adj = skillAdj - SC.baseVal;                        break;
  default:
    if (!debugStrings.AlreadyNoted("SARIAT"))
    {
      CString msg;
      msg.Format("SKILL_ADJUSTMENT_RACE::UpdateSkillValue - Illegal adjType = '%c'", adjType);
      MsgBoxInfo(msg);
      WriteDebugString(msg);
    };
    return;
  };
  if (SC.bestRaceAdj == NoSkillAdj) SC.bestRaceAdj = adj;
  else
  {
    if (SC.minimize)  
    {
      if (adj < SC.bestRaceAdj) SC.bestRaceAdj = adj;
    }
    else
    {
      if (adj > SC.bestRaceAdj) SC.bestRaceAdj = adj;
    };
  };
}

//void SKILL_ADJUSTMENT_BASECLASS::UpdateSkillValue(int baseclassLevel, 
//                                                  double baseVal, 
//                                                  double *pBestBaseclassAdj,
//                                                  bool minimize) const
void SKILL_ADJUSTMENT_BASECLASS::UpdateSkillValue(SKILL_COMPUTATION& SC) const
{
  double adj;
  if (SC.baseclassLevel > HIGHEST_CHARACTER_LEVEL) SC.baseclassLevel = HIGHEST_CHARACTER_LEVEL;
  if (SC.baseclassLevel < 1) SC.baseclassLevel = 1;
  switch (adjType)
  {
  case '%':  adj = SC.baseVal * skillAdj[SC.baseclassLevel-1] / 100.0 - SC.baseVal; break;
  case '+':  adj = skillAdj[SC.baseclassLevel-1];                                   break;
  case '*':  adj = SC.baseVal * (skillAdj[SC.baseclassLevel-1] - 1);                break;
  case '-':  adj = -skillAdj[SC.baseclassLevel-1];                                  break;
  case '=':  adj = skillAdj[SC.baseclassLevel-1] - SC.baseVal;                      break;
  default: 
    if (!debugStrings.AlreadyNoted("SABIAT"))
    {
      CString msg;
      msg.Format("Skill_ADJUSTMENT_BASECLASS - Illegal adjustment type = '%d'", adjType);
      WriteDebugString(msg);
      MsgBoxInfo(msg);
    };
    return;
  };
  if (SC.bestBaseclassAdj == NoSkillAdj) SC.bestBaseclassAdj = adj;
  else
  {
    if (SC.minimize)
    {
      if (adj < SC.bestBaseclassAdj) SC.bestBaseclassAdj = adj;
    }
    else
    {
      if (adj > SC.bestBaseclassAdj) SC.bestBaseclassAdj = adj;
    };
  };
}

//void SKILL_ADJUSTMENT_ABILITY::UpdateSkillValue(const CHARACTER *pChar, 
//                                                double           baseVal,
//                                                bool             minimize) const
void SKILL_ADJUSTMENT_ABILITY::UpdateSkillValue(SKILL_COMPUTATION& SC) const
{
  // We update the field 'bestSkillAdj' in abilityData.
  int ability;
  double additiveAdj;
  ABILITY_DATA *pAbilityData;
  if      (abilityID == Ability_Strength)     ability = SC.pChar->GetLimitedStr();
  else if (abilityID == Ability_Wisdom)       ability = SC.pChar->GetLimitedWis();
  else if (abilityID == Ability_Charisma)     ability = SC.pChar->GetLimitedCha();
  else if (abilityID == Ability_Constitution) ability = SC.pChar->GetLimitedCon();
  else if (abilityID == Ability_Dexterity)    ability = SC.pChar->GetLimitedDex();
  else if (abilityID == Ability_Intelligence) ability = SC.pChar->GetLimitedInt();
  else return;
  pAbilityData = abilityData.GetAbilityData(abilityID);
  if (pAbilityData == NULL)
  {
    if (!debugStrings.AlreadyNoted("NARISA"))
    {
      writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
      WriteDebugString("Non-existent ability \"%s\" referenced in Skill Adjustment\n");
      MsgBoxInfo("Non-existent Ability referenced. See error log");
    };
    return;
  };
  if (ability > HIGHEST_CHARACTER_PRIME) ability = HIGHEST_CHARACTER_PRIME;
  if (ability < 1) ability = 1;
  switch (adjType)
  {
  case '%':
    additiveAdj = SC.baseVal * (skillAdj[ability-1]-100) / 100.0; break;
  case '+':
    additiveAdj = skillAdj[ability-1];                            break;
  default: 
    if (!debugStrings.AlreadyNoted("ISATIS"))
    {
      MsgBoxInfo("Illegal skill adjustment type in SkillAdjAbility");
    };
    return;
  };
  if (pAbilityData->bestSkillAdj == NoSkillAdj) pAbilityData->bestSkillAdj = additiveAdj;
  else
  {
    if (SC.minimize) 
    {
      if (pAbilityData->bestSkillAdj > additiveAdj) pAbilityData->bestSkillAdj = additiveAdj;
    }
    else
    {
      if (pAbilityData->bestSkillAdj < additiveAdj) pAbilityData->bestSkillAdj = additiveAdj;
    };
  };
}


void GetSkillValue(SKILL_COMPUTATION& SC)
{
  // First we will find the largest value of the skill
  // in the baseclass and race databases.
  BASECLASS_ID baseclassID;
  const BASE_CLASS_DATA *pBaseclass;
  if (SC.ppBestBaseclass != NULL) *SC.ppBestBaseclass = NULL;
  // Find the largest skill value among the baseclasses
  {
    int j, m;
    if (SC.pClass != NULL)
    {
      m = SC.pClass->GetCount();
      for (j=0; j<m; j++)
      {
        baseclassID = *SC.pClass->PeekBaseclassID(j);
        if (SC.baseclassID.IsEmpty() || (SC.baseclassID == baseclassID))
        {
          pBaseclass = baseclassData.PeekBaseclass(baseclassID);
          if (pBaseclass == NULL) continue;
          pBaseclass->GetSkillValue(SC);
        };
      };
    };
  };
  // Find the largest skill value in the race.
  if (SC.pRace != NULL)
  {
    SC.pRace->GetSkillValue(SC);
  };
  // Now find the best starting value (baseclass or race)  
  SC.baseVal = SC.baseclassValue;
  if (SC.raceValue != NoSkill)
  {
    if (SC.baseVal == NoSkill) SC.baseVal = SC.raceValue;
    else
    {
      if (SC.minimize) 
      {
        if (SC.raceValue < SC.baseVal) SC.baseVal = SC.raceValue;
      }
      else
      {
        if (SC.raceValue > SC.baseVal) SC.baseVal = SC.raceValue;
      };
    };
  };
}

void UpdateSkillValue(SKILL_COMPUTATION& SC)
{
  abilityData.ClearAdjustments(NoSkillAdj);
  if (SC.pRace != NULL)
  {
    //pRace->UpdateSkillValue (pChar, 
    //                         skillID, 
    //                         NULL, 
    //                         baseVal, 
    //                         &bestRaceAdj, 
    //                         &bestBaseclassAdj,
    //                         minimize);
    SC.pRace->UpdateSkillValue (SC);
  };
  {
    int i, n;
    n = SC.pClass->GetCount();
    for (i=0; i<n; i++)
    {
      BASECLASS_ID baseclassID;
      const BASE_CLASS_DATA *pBaseclass;
      baseclassID = *SC.pClass->PeekBaseclassID(i);
      if (SC.baseclassID.IsEmpty() || (SC.baseclassID == baseclassID))
      {
        pBaseclass = baseclassData.PeekBaseclass(baseclassID);
        if (pBaseclass == NULL) continue;
        //pBaseclass->UpdateSkillValue(pChar, 
        //                             skillID, 
        //                             baseVal, 
        //                             &bestRaceAdj, 
        //                             &bestBaseclassAdj,
        //                             minimize);
        if (SC.baseclassID.IsEmpty() || (SC.baseclassID == pBaseclass->BaseclassID()))
        {
          pBaseclass->UpdateSkillValue(SC);
        };
      };
    };
  };
}

//BOOL GetAdjSkillValue(const SKILL_ID&        skillID,
//                      const CHARACTER       *pChar,
//                      const BASE_CLASS_DATA **pBestBaseclass,
//                            int             *pBestBaseValue,
//                            int             *pAdjValue,
//                            bool             minimize)
void GetAdjSkillValue(SKILL_COMPUTATION& SC)
{
  // Determined the base value for the skill
  GetSkillValue(SC);
  // Now we compute the adjustments.
  UpdateSkillValue(SC);
  SC.abilityAdj = abilityData.TotalAdjustments(NoSkillAdj);
  if (SC.includeTempAdj)
  {
    SC.pChar->ApplyTempSkillAdjustments(SC);
  };
  if (       (SC.baseVal          == NoSkill)
        &&   (SC.bestRaceAdj      == NoSkillAdj)
        &&   (SC.bestBaseclassAdj == NoSkillAdj)
        &&   (SC.abilityAdj       == NoSkillAdj)
        &&   (SC.tempAdj          == NoSkillAdj) ) return;
  SC.finalAdjustedValue = 0.0;
  if (SC.baseVal          != NoSkill)    SC.finalAdjustedValue = SC.baseVal;
  if (SC.bestRaceAdj      != NoSkillAdj) SC.finalAdjustedValue += SC.bestRaceAdj;
  if (SC.bestBaseclassAdj != NoSkillAdj) SC.finalAdjustedValue += SC.bestBaseclassAdj;
  if (SC.abilityAdj       != NoSkillAdj) SC.finalAdjustedValue += SC.abilityAdj;
  if (SC.includeTempAdj)
  {
    if (SC.tempAdj          != NoSkillAdj) SC.finalAdjustedValue += SC.tempAdj;
  };
  //*pAdjValue = int(val+0.5);
}

/*
int LocateSkill(const CHARACTER *pChar, const SKILL_ID& skillID, const CString& adjName)
{
  int i, n;
  n = GetSkillAdjCount();
  for (i=0; i<n; i++)
  {
    const SKILL *pa;
    pa = PeekSkill(i);
    if (pa->skillID == skillID)
    {
      return i;
    };
  return NULL;
}
*/


SKILL_ADJ *FindSkillAdj(CHARACTER *pChar, const SKILL_ID& skillID, const CString& adjName)
{ // Create a new entry if none exists.
  int indx;
  indx = pChar->LocateSkillAdj(skillID, adjName);
  if (indx < 0)
  {
    indx = pChar->InsertSkillAdj(skillID, adjName, ' ', 0);
  };
  return pChar->GetSkillAdj(indx);
}


CString GPDL::m_SkillAdjustment(
                    CHARACTER *pChar,
              const CString&  skillName,
              const CString&  adjName,
              const CString&  adjType,
              int             adjValue)
{
  SKILL_ADJ *ps;
  SKILL_ID skillID;
  int indx;
  int skillValue = NoSkill;
  char adjChar = '+';
  static CString result;
  //BOOL skillExists = FALSE;
  skillID = skillName;
  if (adjType.GetLength() > 0)
  {
    adjChar = adjType[0];
  };
  switch (adjChar)
  {
  case '+':
  case '%':
  case '=':
  case '-':
  case '*':
    ps = FindSkillAdj(pChar, skillID, adjName);
    ps->type = adjChar;
    ps->value = adjValue;
    return emptyString;
    break;
  case 'D':
    indx = pChar->LocateSkillAdj(skillName, adjName);
    if (indx >= 0) pChar->DeleteSkillAdj(indx);
    return emptyString;
  case 'F':
    {
      SKILL_COMPUTATION SC(pChar, skillName, false, true);
      GetAdjSkillValue(SC);
      //skillExists = GetAdjSkillValue(skillID, 
      //                               pChar, 
      //                               NULL, 
      //                               NULL, 
      //                               &skillValue,
      //                               false);
      //pChar->ApplyTempSkillAdjustments(skillID, &skillValue);
      if (SC.finalAdjustedValue == NoSkillAdj) skillValue = NoSkill;
      else skillValue = SC.finalAdjustedValue + 0.5;
    };
    break;
  case 'f':
    {
      SKILL_COMPUTATION SC(pChar, skillName, true, true);
      GetAdjSkillValue(SC);
      //skillExists = GetAdjSkillValue(skillID, 
      //                               pChar, 
      //                               NULL, 
      //                               NULL, 
      //                               &skillValue,
      //                               true);
      //pChar->ApplyTempSkillAdjustments(skillID, &skillValue);
      if (SC.finalAdjustedValue == NoSkillAdj) skillValue = NoSkill;
      else skillValue = SC.finalAdjustedValue + 0.5;
    };
    break;
  case 'A':
    indx = pChar->LocateSkillAdj(skillName, adjName);
    if (indx < 0)
    {
      //skillExists = FALSE;
    }
    else
    {
      skillValue = pChar->PeekSkillAdj(indx)->value;
      //skillExists = TRUE;
    };
    break;
  case 'b':
    skillValue = pChar->GetAdjSkillValue(skillName, true, false);
    //skillExists = pChar->GetAdjSkillValue(skillName, true);
    //skillExists = GetAdjSkillValue(skillID, pChar, NULL, &skillValue, NULL, true);
    break;
  case 'B':
    skillValue = pChar->GetAdjSkillValue(skillName, false, false);
    break;
  default:
    //skillExists = GetAdjSkillValue(skillID, pChar, NULL, &skillValue, NULL, false);
    m_interpretError("Illegal 'type' specified in $SkillAdj()");
    break;
  };
  if (skillValue == NoSkill) return "NoSkill";
  result.Format("%d", skillValue);
  return result;
}


CAR& SKILL_ADJUSTMENT_ABILITY::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << skillID;
    car << abilityID;
    car << adjType;
    car.Serialize((char *)skillAdj, sizeof(skillAdj));
    return car;
  }
  car >> skillID;
  car >> abilityID;
  car >> adjType;
  car.Serialize((char *)skillAdj, sizeof(skillAdj));
  return car;
}


CAR& BONUS_XP::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << abilityID;
    car << bonusType;
    car.Serialize((char *)bonus, sizeof(bonus));
    return car;
  }
  car >> abilityID;
  car >> bonusType;
  car.Serialize((char *)bonus, sizeof(bonus));
  return car;
}



CAR& SKILL_ADJUSTMENT_BASECLASS::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << skillID;
    car << baseclassID;
    car << adjType;
    car.Serialize((char *)skillAdj, sizeof(skillAdj));
    return car;
  }
  car >> skillID;
  car >> baseclassID;
  car >> adjType;
  car.Serialize((char *)skillAdj, sizeof(skillAdj));
  return car;
}

CAR& SKILL_ADJUSTMENT_RACE::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << skillID;
    car << raceID;
    car << adjType;
    car << skillAdj;
    return car;
  }
  car >> skillID;
  car >> raceID;
  car >> adjType;
  car >> skillAdj;
  return car;
}

CAR& SKILL_ADJUSTMENT_SCRIPT::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << skillID;
    car << specialAbilityName;
    car << scriptName;
    return car;
  }
  car >> skillID;
  car >> specialAbilityName;
  car >> scriptName;
  return car;
}

/*
CAR& SAVE_VS::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << versusName;
    car.Serialize((char *)save, sizeof(save));
  }
  else
  {
    car >> versusName;
    car.Serialize((char *)save, sizeof(save));
  };
  return car;
}
*/


CAR& SPELL_ADJ::Serialize(CAR& car, const CString& version)
{
  if (car.IsStoring())
  {
    car << schoolID;
    car << adjID;
    car << firstLevel;
    car << lastLevel;
    car << p;
    car << b;
  }
  else
  {
    car >> schoolID;
    car >> adjID;
    car >> firstLevel;
    car >> lastLevel;
    car >> p;
    car >> b;
  };
  return car;
}


#ifdef UAFEDITOR
void SPELL_ADJ::Export(JWriter& jw)
{
  /* Really */ NotImplemented(0xdea7, false);
  /*
    car << schoolID;
    car << adjID;
    car << firstLevel;
    car << lastLevel;
    car << p;
    car << b;
    */
}
void SPELL_ADJ::Import(JReader& jr)
{
  /* Really */ NotImplemented(0xdea8, false);
  /*
    car << schoolID;
    car << adjID;
    car << firstLevel;
    car << lastLevel;
    car << p;
    car << b;
    */
}
#endif

CString CHARACTER::SpellAdjustment(
          const CString&  schoolID,
          const CString&  identifier,
          int             firstLevel,
          int             lastLevel,
          int             percent,
          int             bonus
          )
{
  if (percent == 999999)
  {
    int i, n;
    n = GetSpellAdjCount();
    for (i=0; i<n; i++)
    {
      const SPELL_ADJ *pSpellAdj;
      pSpellAdj = PeekSpellAdj(i);
      if (pSpellAdj->schoolID != schoolID) continue;
      if (pSpellAdj->adjID != identifier) continue;
      if (bonus != 0)
      {
        bonus--;
        continue;
      }
      spellAdjustments.RemoveAt(i);
      return CString();
    };
  }
  else
  {
    int i, n;
    SPELL_ADJ spellAdj;
    spellAdj.schoolID = schoolID;
    spellAdj.adjID = identifier;
    spellAdj.firstLevel = firstLevel;
    spellAdj.lastLevel = lastLevel;
    spellAdj.p = percent;
    spellAdj.b = bonus;
    n = GetSpellAdjCount();
    for (i=n; i>0; i--)
    {
      const SPELL_ADJ *pSpellAdj;
      pSpellAdj = PeekSpellAdj(i-1);
      if (spellAdj > *pSpellAdj) break;
    };
    spellAdjustments.SetAtGrow(i, spellAdj);
  };
  return CString();
}




#ifdef UAFEDITOR
void BASE_CLASS_DATA::AddSkillAdjAbility(const CString& skillName,
                                         const CString& abilityName,
                                         const char adjType,
                                         short      *skillModifier,
                                         int size)
{
  SKILL_ADJUSTMENT_ABILITY skillAdj;
  int i;
  skillAdj.abilityID = abilityName;
  skillAdj.skillID   = skillName;
  skillAdj.adjType   = adjType;
  for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
  {
    if (i < size)
    {
      skillAdj.skillAdj[i] = skillModifier[i];
    }
    else
    {
      skillAdj.skillAdj[i] = skillModifier[size-1];
    };
  };
  m_skillAdjustmentsAbility.Add(skillAdj);
}
#endif

#ifdef UAFEngine
int RollHitPointDice(const CString& baseclassName, int low, int high)
{
  const BASE_CLASS_DATA *pBaseclass;
  int i;
  float result;
  BASECLASS_ID baseclassID;
  baseclassID = baseclassName;
  pBaseclass = baseclassData.PeekBaseclass(baseclassID);
  if (pBaseclass == NULL) return 0;
  if (low < 1) low = 1;
  if (low > HIGHEST_CHARACTER_LEVEL) high = HIGHEST_CHARACTER_LEVEL;
  if (high < 1) low = 1;
  if (high > HIGHEST_CHARACTER_LEVEL) high = HIGHEST_CHARACTER_LEVEL;
  result = 0.0;
  for (i=low; i<=high; i++)
  {
    int sides, num, k;
    pBaseclass->GetHitDice(i, &sides, &num, &k);
    result += RollDice(sides, num, k);
  };
  return result;
}
#endif

int BASE_CLASS_DATA::Serialize(CAR& car)
{
  //int i,n;
  int result=0;
  CString ver("Bcd5");
  if (car.IsStoring())
  {
    {
      int i, n;
      car << ver; // version
      //car << m_baseclassKey;
      car << m_preSpellNameKey;
      if (m_name == "") m_name = "*";
      car << m_name;
      if (m_name == "*") m_name = "";
      //n=m_primeReq.GetSize();
      //  car.WriteCount(n);
      //  //for (i=0; i<n; i++) m_primeReq[i].Serialize(car);
      //  for (i=0; i<n; i++) car << *PeekAbilityName(i);
      n = m_abilityReq.GetSize();
      car.WriteCount(n);
      for (i = 0; i < n; i++)
      {
        GetAbilityReq(i)->Serialize(car);
        //m_abilityReq[i].Serialize(car);
      };
      n = m_allowedRaces.GetSize();
      car.WriteCount(n);
      //for (i=0; i<n; i++) m_allowedRaces[i].Serialize(car, 0);
      for (i = 0; i < n; i++) car << *PeekRaceID(i);
      //m_expLevels.Serialize(car);
      car << m_expLevels;
      car << m_allowedAlignments;
      car.Serialize(THAC0, sizeof(THAC0));
      //car << m_primeCasting;
      //car << m_backstabAbility;
      //car << m_backstabLevel;
      //car << m_turnUndeadLevel;
      car << m_spellBonusAbility;
      //car << m_rangerBonusLevel;
      n = m_bonusSpells.GetSize();
      car << n;
      for (i = 0; i < n; i++)
      {
        car << m_bonusSpells[i];
      };
    };
    {
      int i, n;
      n = m_castingInfo.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetCastingInfo(i)->Serialize(car);
      };
    };
    m_specAbs.Serialize(car, globalData.version, m_name, "baseclasses");
    {
      int i;
      for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
      {
        car << hitDice[i].sides;
        car << hitDice[i].nbr;
        car << hitDice[i].bonus;
      };
    };
    {
      int i, n;
      n = m_skills.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkill(i)->Serialize(car);
        //m_skills[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsAbility.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjAbility(i)->Serialize(car);
        //m_skillAdjustmentsAbility[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsBaseclass.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjBaseclass(i)->Serialize(car);
        //m_skillAdjustmentsBaseclass[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsRace.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjRace(i)->Serialize(car);
        //m_skillAdjustmentsRace[i].Serialize(car);
      };
    };
    {
      int i, n;
      n = m_skillAdjustmentsScript.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetSkillAdjScript(i)->Serialize(car);
        //m_skillAdjustmentsScript[i].Serialize(car);
      };
    };
    {
      int i,n;
      n = m_bonusXP.GetSize();
      car << n;
      for (i=0; i<n; i++)
      {
        GetBonusXP(i)->Serialize(car);
        //m_bonusXP[i].Serialize(car);
      };
    };
  }
  else
  {
    int intVer = -1;
    car >> ver;
    if (ver == "Bcd0") intVer = 0;
    else if (ver == "Bcd1") intVer = 1;
    else if (ver == "Bcd2") intVer = 2;
    else if (ver == "Bcd3") intVer = 3;
    else if (ver == "Bcd4") intVer = 4;
    else if (ver == "Bcd5") intVer = 5;
    if (intVer < 2)
    {
#ifdef UAFEngine
      MsgBoxInfo("This module contains an old 'baseclass.dat' file.  You must install a new one before we can proceed");
#else
      MsgBoxInfo("This module contains an old 'baseclass.dat' file.  You should import a new 'bassclass.txt' file.  "
                 "I will examine the classes.  If they reference a baseclass that starts with a lower-case "
                 "letter, and no such baseclass exists, but a baseclass with an upper-case letter does exist, "
                 "then I will duplicate that baseclass with a lower-case letter.  But the best thing for "
                 "you to do is import a new 'baseclass.txt'");
#endif
      SignalShutdown();
      return 1;
    };
    if (intVer>=0)
    {
      Clear();
#ifdef UAFEDITOR
      if (intVer < 4)
      {
        //car >> m_baseclassKey;
        //DWORD key;
        //car >> key;
        car >> m_preSpellNameKey;
      };
#endif
      if (intVer >= 5)
      {
        car >> m_preSpellNameKey;
      };
      car >> m_name; if (m_name=="*") m_name="";
#ifdef UAFEDITOR
      if (intVer < 4)
      {
        int i, n;
        n=car.ReadCount();
          //m_primeReq.SetSize(n);
        for (i=0; i<n; i++)
        {
          CString pr;
          //result=m_primeReq[i].Serialize(car);
          car >> pr;
          //if (result!=0) return result;
          DWORD key;
          car >> key;
        };
      };
#endif
      {
        int i, n;
        n = car.ReadCount();
        m_abilityReq.SetSize(n);
        for (i = 0; i < n; i++)
        {
          result = GetAbilityReq(i)->Serialize(car);
          //result=m_abilityReq[i].Serialize(car);
          if (result != 0) return result;
        };
        n = car.ReadCount();
        m_allowedRaces.SetSize(n);
        for (i = 0; i < n; i++)
        {
          //result=m_allowedRaces[i].Serialize(car, intVer);
          car >> *GetRaceID(i);
#ifdef UAFEDITOR
          if (intVer < 4)
          {
            DWORD key;
            car >> key;
          };
#endif
          //if (result!=0) return result;
        };
      };
      car >> m_expLevels;
      //m_expLevels.Serialize(car);
      if (result!=0) return result;

      if (ver>="Bcd1") car >> m_allowedAlignments;
      else m_allowedAlignments=0x1ff;
#ifdef UAFEDITOR
      if (intVer >= 4)
#endif
      {
        {
          int i, n;
          car.Serialize(THAC0, sizeof(THAC0));

          //car >> m_primeCasting;
//#ifdef UAFEDITOR
//          if (intVer < 4)
//          {
//            //car >> m_backstabAbility;
//            //car >> m_backstabLevel;
//            CString backstabAbility;
//            int backstabLevel;
//            car >> backstabAbility;
//            car >> backstabLevel;
//          };
//#endif
          car >> m_spellBonusAbility;
          //#ifdef UAFEDITOR
          //          if (intVer < 4)
          //          {
          //            int rangerBonusLevel;
          //          //car >> m_rangerBonusLevel;
          //            car >> rangerBonusLevel;
          //          };
          //#endif
          car >> n;
          m_bonusSpells.SetSize(n);
          for (i = 0; i < n; i++)
          {
            BYTE x;
            car >> x;
            m_bonusSpells.SetAt(i, x);
          };
        };
        {
          int i, n;
          car >> n;
          m_castingInfo.SetSize(n);
          for (i=0; i<n; i++)
          {
            GetCastingInfo(i)->Serialize(car);
            //m_castingInfo[i].Serialize(car);
          };
        };
      }
#ifdef UAFEDITOR
      else
      {
        m_skills.RemoveAll();
        m_skillAdjustmentsBaseclass.RemoveAll();
        m_skillAdjustmentsAbility.RemoveAll();
        m_skillAdjustmentsRace.RemoveAll();
        m_skillAdjustmentsScript.RemoveAll();
        // Set variables for old designs.
        if (m_name == Baseclass_ranger)
        {
          //m_rangerBonusLevel = 1;
          AddSkill(Skill_RangerBonusLevel, 1);
        //}
        //else
        //{
          //m_rangerBonusLevel = 999;
        };
        //if (m_name == Baseclass_magicUser)
        //{
        //  m_primeCasting = Ability_Intelligence;
        //}
        //else
        //{
        //  m_primeCasting = Ability_Wisdom;
        //};
        if (m_name == Baseclass_thief)
        {
          int i;
          //m_backstabAbility = Ability_Dexterity;
          //m_backstabLevel = 0;
          BYTE backstabMultipliers[40];
          for (i=0; i<40; i++)
          {
            backstabMultipliers[i] = 2 + i/4;
          };
          AddSkill(Skill_BackstabMultiplier,0);
          AddSkillAdjBaseclass(Skill_BackstabMultiplier,"thief",'+',backstabMultipliers,40);
        //}
        //else
        //{
          //m_backstabAbility = "";
          //m_backstabLevel = 999;
        };
        {
          SKILL turnUndeadSkill;
          turnUndeadSkill.skillID = Skill_Turn;
          //if (m_name == Baseclass_paladin) m_turnUndeadLevel = 3;
          //else if (m_name == Baseclass_cleric) m_turnUndeadLevel = 0;
          //else m_turnUndeadLevel = 999;
          if (m_name == Baseclass_paladin) turnUndeadSkill.value = 2;
          else if (m_name == Baseclass_cleric) turnUndeadSkill.value = 0;
          else turnUndeadSkill.value = 999;
          m_skills.Add(turnUndeadSkill);
        };
        if (m_name == Baseclass_cleric)
        {
          int i;
          m_spellBonusAbility = Ability_Wisdom;
          for (i=1; i<=19; i++)
          {
            int lvl, qty;
            lvl = GetClericBonusLevel(i);
            qty = GetClericBonusNum(i);
            if (qty != 0)
            {
              m_bonusSpells.Add(i);
              m_bonusSpells.Add(lvl);
              m_bonusSpells.Add(qty);
            };
          };
        }
        else
        {
          m_spellBonusAbility = "";
          m_bonusSpells.RemoveAll();
        };

        { //  m_castingInfo
          CASTING_INFO castingInfo;
          // There were seven possible baseclasses
          //schoolID
          //prime attribute name
          //BYTE m_spellLimits[HIGHEST_CHARACTER_LEVEL][MAX_SPELL_LEVEL];
          //BYTE m_maxSpellLevels[HIGHEST_CHARACTER_PRIME];
          //BYTE m_maxSpells[HIGHEST_CHARACTER_PRIME];

          m_castingInfo.RemoveAll();
          castingInfo.Clear();
          castingInfo.primeAbility = Ability_Wisdom;
          castingInfo.schoolID = School_Cleric;
          if (m_name == Baseclass_fighter)
          {
            int i, thac0;
            //No spells.
            for (i=1; i<=HIGHEST_CHARACTER_LEVEL; i++)
            {
              thac0 = 21 - i;
          		if (i > 2) thac0 = min(thac0, (20-(((i - 1)/2)*2)) );
		          if (i > 17) thac0 = min( thac0, 4 );
              THAC0[i-1] = thac0;
            };
          }
          else if (m_name == Baseclass_thief)
          {
            int i, thac0;
            //No Spells
            for (i=1; i<=HIGHEST_CHARACTER_LEVEL; i++)
            {
              thac0 = 20;
		          if (i > 4)  thac0 = min( thac0, 19 );
		          if (i > 8)	thac0 = min( thac0, (20 - (((i-1)/4)*2)) );
		          if (i > 20) thac0 = min( thac0, 10 );
              THAC0[i-1] = thac0;
            };
          }
          else if (m_name == Baseclass_cleric)
          {
            int i;
            // Cleric Spells only
            memcpy(castingInfo.m_maxSpellsByPrime, MaxClericSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[1][1], 19);
            CopyClericSpellLevels(castingInfo.m_spellLimits[0],School_Cleric);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);

            int thac0=0;
            for (i=1; i<=HIGHEST_CHARACTER_LEVEL; i++)
            {
              thac0 = 20;
          		if (i > 3)  thac0 = min( thac0, (20-(((i-1)/3)*2)) );
		          if (i > 18) thac0 = min( thac0, 9 );
              THAC0[i-1] = thac0;
            }
          }
          else if (m_name == Baseclass_druid)
          {
            int i;
            // Magic User and Cleric Spells
            memcpy(castingInfo.m_maxSpellsByPrime, MaxDruidSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[6][1], 19);
            CopyClericSpellLevels(castingInfo.m_spellLimits[0],School_Cleric);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);

            castingInfo.schoolID = School_Magic_User;
            castingInfo.primeAbility = Ability_Intelligence;
            memcpy(castingInfo.m_maxSpellsByPrime, MaxDruidSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[6][0], 19);
            CopyDruidSpellLevels(castingInfo.m_spellLimits[0],School_Magic_User);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);

            int thac0=0;
            for (i=1; i<=HIGHEST_CHARACTER_LEVEL; i++)
            {
              thac0 = 20;
		          if (i > 3)  thac0 = min( thac0, (20-(((i-1)/3)*2)) );
		          if (i > 18) thac0 = min( thac0, 9 );	//
              THAC0[i-1] = thac0;
            }
          }
          else if (m_name == Baseclass_paladin)
          {
            int i;
            // Magic User and Cleric Spells
            memcpy(castingInfo.m_maxSpellsByPrime, MaxPaladinSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[3][1], 19);
            CopyClericSpellLevels(castingInfo.m_spellLimits[0],School_Cleric);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);

            castingInfo.schoolID = School_Magic_User;
            castingInfo.primeAbility = Ability_Intelligence;
            memcpy(castingInfo.m_maxSpellsByPrime, MaxPaladinSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[3][0], 19);
            CopyPaladinSpellLevels(castingInfo.m_spellLimits[0],School_Magic_User);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);


            int thac0=0;
            for (i=1; i<=HIGHEST_CHARACTER_LEVEL; i++)
            {
              thac0 = 20;
		          if (i > 2)	 thac0 = min( thac0, (20-(((i - 1)/2)*2)) );
		          if (i > 17) thac0 = min( thac0, 4);
              THAC0[i-1] = thac0;
            }
          }
          else if (m_name == Baseclass_ranger)
          {
            int i;
            // Magic User and Cleric spells
            memcpy(castingInfo.m_maxSpellsByPrime, MaxRangerSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[2][1], 19);
            CopyClericSpellLevels(castingInfo.m_spellLimits[0],School_Cleric);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);

            castingInfo.schoolID = School_Magic_User;
            castingInfo.primeAbility = "Intelligence";
            memcpy(castingInfo.m_maxSpellsByPrime, MaxRangerSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[2][0], 19);
            CopyRangerSpellLevels(castingInfo.m_spellLimits[0],School_Magic_User);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);

            int thac0=0;
            for (i=1; i<=HIGHEST_CHARACTER_LEVEL; i++)
            {
              thac0 = 20;
		          if (i > 2)  thac0 = min( thac0, (20-(((i - 1)/2)*2)) );
		          if (i > 17) thac0 = min( thac0, 4);
              THAC0[i-1] = thac0;
            };

          }
          else if (m_name == Baseclass_magicUser)
          {
            int i;
            // Magic User spells only
            castingInfo.schoolID = School_Magic_User;
            castingInfo.primeAbility = Ability_Intelligence;
            memcpy(castingInfo.m_maxSpellsByPrime, MaxMuSpellsForEachSpellLvlByPrime, 19);
            memcpy(castingInfo.m_maxSpellLevelsByPrime, MaxSpellLevel[4][0], 19);
            CopyMuSpellLevels(castingInfo.m_spellLimits[0],School_Magic_User);
            for (i=19; i<HIGHEST_CHARACTER_PRIME; i++)
            {
              castingInfo.m_maxSpellsByPrime[i] =  castingInfo.m_maxSpellsByPrime[18];
              castingInfo.m_maxSpellLevelsByPrime[i] = castingInfo.m_maxSpellLevelsByPrime[18];
            };
            m_castingInfo.Add(castingInfo);

            int thac0=0;
            for (i=1; i<=HIGHEST_CHARACTER_LEVEL; i++)
            {
              thac0 = 20;
	  	        if (i > 5)   thac0 = min( thac0, 19 );
		          if (i > 10)  thac0 = min( thac0, 16 );
		          if (i > 15)  thac0 = min( thac0, 13 );
		          if (i > 20)  thac0 = min( thac0, 11 );
              THAC0[i-1] = thac0;
            }
          };
        };
      };
#endif
      if (ver > "Bcd2")
      {
        double version;
        version = globalData.version;
        // We have a problem here.
        // People package their old designs (Pre 0.930, eg) with new baseclass.dat.
        // The serialize code examines the design version and makes a mistake
        // about the baseclass.dat version.
        // So we will fake the version for the purposes of loading the baseclass.dat.
        if (intVer >= 5)
        {
          version = 0.930;
        };
        m_specAbs.Serialize(car, version, m_name, "baseclasses");
      };
#ifdef UAFEDITOR
      if (intVer < 4)
      {
        int i;
        // We need to establish the hitDice tables from the hard-coded tables.
        DICEDATA *pDiceTable = NULL;
        if      (m_name == Baseclass_fighter)    pDiceTable = FighterHitDiceTable;
        else if (m_name == Baseclass_thief)      pDiceTable = ThiefHitDiceTable;
        else if (m_name == Baseclass_cleric)     pDiceTable = ClericHitDiceTable;
        else if (m_name == Baseclass_druid)      pDiceTable = DruidHitDiceTable;
        else if (m_name == Baseclass_paladin)    pDiceTable = PaladinHitDiceTable;
        else if (m_name == Baseclass_ranger)     pDiceTable = RangerHitDiceTable;
        else if (m_name == Baseclass_magicUser)  pDiceTable = MagicUserHitDiceTable;
        if (pDiceTable == NULL)
        {
          WriteDebugString("Unknown Baseclass '%s'\n", m_name);
          pDiceTable = FighterHitDiceTable;
        };
        for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
        {
          hitDice[i] = pDiceTable[i];
        };
      }
      else
#endif
      {
        int i;
        for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
        {
          car >> hitDice[i].sides;
          car >> hitDice[i].nbr;
          car >> hitDice[i].bonus;
        };
      };
#ifdef UAFEDITOR
      if (intVer < 4)
      {
        // We need to create thiefAbilities entries for
        // the 'thief' baseclass.
        if (m_name == Baseclass_thief)
        {
          {  // From char.cpp  SetThiefSkills()
            const BYTE pp[17]={30,35,40,45,50,55,60,65,70,80,90,95,99,99,99,99,99};
            const BYTE ol[17]={25,29,33,37,42,47,52,57,62,67,72,77,82,87,92,97,99};
            const BYTE ft[17]={20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,99};
            const BYTE ms[17]={15,21,27,33,40,47,55,62,70,78,86,94,99,99,99,99,99};
            const BYTE hs[17]={10,15,20,25,31,37,43,49,56,63,70,77,85,93,99,99,99};
            const BYTE hn[17]={10,10,15,15,20,20,25,25,30,30,35,35,40,40,50,50,55};
            const BYTE cw[17]={85,86,87,88,90,92,94,96,98,99,99,99,99,99,99,99,99};
            const BYTE rl[17]={0, 0, 0, 20,25,30,35,40,45,50,55,60,65,70,75,80,80};
            AddSkill(Skill_PickPockets,   100);
            AddSkill(Skill_OpenLocks,     100);
            AddSkill(Skill_FindTraps,     100);
            AddSkill(Skill_MoveSilent,    100);
            AddSkill(Skill_HideInShadows, 100);
            AddSkill(Skill_HearNoise,     100);
            AddSkill(Skill_ClimbWalls,    100);
            AddSkill(Skill_ReadLanguages, 100);

            AddSkillAdjBaseclass(Skill_PickPockets,   Baseclass_thief, '%', pp, 17);
            AddSkillAdjBaseclass(Skill_OpenLocks,     Baseclass_thief, '%', ol, 17);
            AddSkillAdjBaseclass(Skill_FindTraps,     Baseclass_thief, '%', ft, 17);
            AddSkillAdjBaseclass(Skill_MoveSilent,    Baseclass_thief, '%', ms, 17);
            AddSkillAdjBaseclass(Skill_HideInShadows, Baseclass_thief, '%', hs, 17);
            AddSkillAdjBaseclass(Skill_HearNoise,     Baseclass_thief, '%', hn, 17);
            AddSkillAdjBaseclass(Skill_ClimbWalls,    Baseclass_thief, '%', cw, 17);
            AddSkillAdjBaseclass(Skill_ReadLanguages, Baseclass_thief, '%', rl, 17);
          };
          {  // From Char.cpp  SetThiefSkillDexAdjustments
            static short pp[19] =
            {0,0,0,0,0,0,0,0,85,90,95,0,0,0,0,0,105,110,115};

            static short ol[19] =
            {0,0,0,0,0,0,0,0,90,95,0,0,0,0,0,0,105,110,115};

            static short ft[19] =
            {0,0,0,0,0,0,0,0,90,90,95,0,0,0,0,0,0,105,110};

            static short ms[19] =
            {0,0,0,0,0,0,0,0,80,85,90,95,0,0,0,0,105,110,115};

            static short hs[19] =
            {0,0,0,0,0,0,0,0,90,95,0,0,0,0,0,0,105,110,115};


            AddSkillAdjAbility(Skill_PickPockets,   Ability_Dexterity, '%', pp, 19);
            AddSkillAdjAbility(Skill_OpenLocks,     Ability_Dexterity, '%', ol, 19);
            AddSkillAdjAbility(Skill_FindTraps,     Ability_Dexterity, '%', ft, 19);
            AddSkillAdjAbility(Skill_MoveSilent,    Ability_Dexterity, '%', ms, 19);
            AddSkillAdjAbility(Skill_HideInShadows, Ability_Dexterity, '%', hs, 19);
          };
        };
        {
          // We need to generate bonus experience entries for the
          // seven standard baseclasses;
          m_bonusXP.RemoveAll();
          if (m_name == Baseclass_fighter)
          {
            AddDefaultBonusXP(Ability_Strength, 16, 110);
          }
          if (m_name == Baseclass_cleric)
          {
            AddDefaultBonusXP(Ability_Wisdom, 16, 110);
          }
          if (m_name == Baseclass_druid)
          {
            AddDefaultBonusXP(Ability_Wisdom, 16, 110);
            AddDefaultBonusXP(Ability_Charisma, 16, 110);
          }
          if (m_name == Baseclass_paladin)
          {
            AddDefaultBonusXP(Ability_Strength, 16, 110);
            AddDefaultBonusXP(Ability_Wisdom, 16, 110);
          }
          if (m_name == Baseclass_ranger)
          {
            AddDefaultBonusXP(Ability_Intelligence, 16, 110);
            AddDefaultBonusXP(Ability_Strength, 16, 110);
            AddDefaultBonusXP(Ability_Wisdom, 16, 110);
          }
          if (m_name == Baseclass_magicUser)
          {
            AddDefaultBonusXP(Ability_Intelligence, 16, 110);
          }
          if (m_name == Baseclass_thief)
          {
            AddDefaultBonusXP(Ability_Dexterity, 16, 110);
          }
        };
      }
      else
#endif
      {
        {
          int i, n;
          car >> n;
          m_skills.SetSize(n);
          for (i=0; i<n; i++)
          {
            GetSkill(i)->Serialize(car);
            //m_skills[i].Serialize(car);
          };
        };
        {
          int i, n;
          car >> n;
          m_skillAdjustmentsAbility.SetSize(n);
          for (i=0; i<n; i++)
          {
            GetSkillAdjAbility(i)->Serialize(car);
            //m_skillAdjustmentsAbility[i].Serialize(car);
          };
        };
        {
          int i, n;
          n = m_skillAdjustmentsBaseclass.GetSize();
          car >> n;
          m_skillAdjustmentsBaseclass.SetSize(n);
          for (i=0; i<n; i++)
          {
            GetSkillAdjBaseclass(i)->Serialize(car);
            //m_skillAdjustmentsBaseclass[i].Serialize(car);
          };
        };
        {
          int i, n;
          car >> n;
          m_skillAdjustmentsRace.SetSize(n);
          for (i=0; i<n; i++)
          {
            GetSkillAdjRace(i)->Serialize(car);
            //m_skillAdjustmentsRace[i].Serialize(car);
          };
        };
        {
          int i, n;
          car >> n;
          m_skillAdjustmentsScript.SetSize(n);
          for (i=0; i<n; i++)
          {
            GetSkillAdjScript(i)->Serialize(car);
            //m_skillAdjustmentsScript[i].Serialize(car);
          };
        };
        {
          int i, n;
          car >> n;
          m_bonusXP.SetSize(n);
          for (i=0; i<n; i++)
          {
            GetBonusXP(i)->Serialize(car);
            //m_bonusXP[i].Serialize(car);
          };
        };
      };
#ifdef UAFEDITOR
      if (intVer < 4)
      {
        // We must supply the old hard-coded values.
        // Array values are for level 0 through 21.
        static const BYTE FPR_ppdm[22] = {16,14,14,13,13,11,11,10,10,8 ,8 ,7 ,7 , 5, 5, 4, 4, 3, 3, 3, 3, 3};
        static const BYTE FPR_rsw [22] = {18,16,16,15,15,13,13,12,12,10,10,9 ,9 , 7, 7, 6, 6, 5, 5, 5, 5, 5};
        static const BYTE FPR_pp  [22] = {17,15,15,14,14,12,12,11,11,9 ,9 ,8 ,8 , 6, 6, 5, 5, 4, 4, 4, 4, 4};
        static const BYTE FPR_br  [22] = {20,17,17,16,16,13,13,12,12,9 ,9 ,8 ,8 , 5, 5, 4, 4, 4, 4, 4, 4 ,4};
        static const BYTE FPR_sp  [22] = {19,17,17,16,16,14,14,13,13,11,11,10,10, 8, 8, 7, 7, 6, 6, 6, 6, 6};

        static const BYTE CD_ppdm [22] = {10,10,10,10, 9, 9, 9, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 2, 2, 2};
        static const BYTE CD_rsw  [22] = {14,14,14,14,13,13,13,11,11,11,10,10,10, 9, 9, 9, 8, 8, 8, 6, 6, 6};
        static const BYTE CD_pp   [22] = {13,13,13,13,12,12,12,10,10,10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 5, 5, 5};
        static const BYTE CD_br   [22] = {16,16,16,16,15,15,15,13,13,13,12,12,12,11,11,11,10,10,10, 8, 8, 8};
        static const BYTE CD_sp   [22] = {15,15,15,15,14,14,14,12,12,12,11,11,11,10,10,10, 9, 9, 9, 7, 7, 7};

        static const BYTE MU_ppdm [22] = {14,14,14,14,14,14,13,13,13,13,13,11,11,11,11,11,10,10,10,10,10, 8};
        static const BYTE MU_rsw  [22] = {11,11,11,11,11,11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 3};
        static const BYTE MU_pp   [22] = {13,13,13,13,13,13,11,11,11,11,11, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 5};
        static const BYTE MU_br   [22] = {15,15,15,15,15,15,13,13,13,13,13,11,11,11,11,11, 9, 9, 9, 9, 9, 7};
        static const BYTE MU_sp   [22] = {12,12,12,12,12,12,10,10,10,10,10, 8, 8, 8, 8, 8, 6, 6, 6, 6, 6, 4};

        static const BYTE T_ppdm  [22] = {13,13,13,13,13,12,12,12,12,11,11,11,11,10,10,10,10, 9, 9, 9, 9, 8};
        static const BYTE T_rsw   [22] = {14,14,14,14,14,12,12,12,12,10,10,10,10, 8, 8, 8, 8, 6, 6, 6, 6, 7};
        static const BYTE T_pp    [22] = {12,12,12,12,12,11,11,11,11,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 4};
        static const BYTE T_br    [22] = {16,16,16,16,16,15,15,15,15,14,14,14,14,13,13,13,13,12,12,12,12,11};
        static const BYTE T_sp    [22] = {15,15,15,15,15,13,13,13,13,11,11,11,11, 9, 9, 9, 9, 7, 7, 7, 7, 5};

        if ((m_name == Baseclass_fighter) || (m_name == Baseclass_paladin) || (m_name == Baseclass_ranger))
        {
          AddSkillAdjBaseclass(Save_Vs_PPDM, m_name, '%', FPR_ppdm, 22);
          AddSkillAdjBaseclass(Save_Vs_PPDM, m_name, '%', FPR_ppdm, 22);
          AddSkillAdjBaseclass(Save_Vs_RSW,  m_name, '%', FPR_rsw,  22);
          AddSkillAdjBaseclass(Save_Vs_PP,   m_name, '%', FPR_pp,   22);
          AddSkillAdjBaseclass(Save_Vs_BR,   m_name, '%', FPR_br,   22);
          AddSkillAdjBaseclass(Save_Vs_SP,   m_name, '%', FPR_sp,   22);
        };
        if ((m_name == Baseclass_cleric) || (m_name == Baseclass_druid))
        {
          AddSkillAdjBaseclass(Save_Vs_PPDM, m_name, '%', CD_ppdm, 22);
          AddSkillAdjBaseclass(Save_Vs_RSW,  m_name, '%', CD_rsw,  22);
          AddSkillAdjBaseclass(Save_Vs_PP,   m_name, '%', CD_pp,   22);
          AddSkillAdjBaseclass(Save_Vs_BR,   m_name, '%', CD_br,   22);
          AddSkillAdjBaseclass(Save_Vs_SP,   m_name, '%', CD_sp,   22);
        };
        if (m_name == Baseclass_magicUser)
        {
          AddSkillAdjBaseclass(Save_Vs_PPDM, m_name, '%', MU_ppdm, 22);
          AddSkillAdjBaseclass(Save_Vs_RSW,  m_name, '%', MU_rsw,  22);
          AddSkillAdjBaseclass(Save_Vs_PP,   m_name, '%', MU_pp,   22);
          AddSkillAdjBaseclass(Save_Vs_BR,   m_name, '%', MU_br,   22);
          AddSkillAdjBaseclass(Save_Vs_SP,   m_name, '%', MU_sp,   22);
        };
        if (m_name == Baseclass_thief)
        {
          AddSkillAdjBaseclass(Save_Vs_PPDM, m_name, '%', T_ppdm, 22);
          AddSkillAdjBaseclass(Save_Vs_RSW,  m_name, '%', T_rsw,  22);
          AddSkillAdjBaseclass(Save_Vs_PP,   m_name, '%', T_pp,   22);
          AddSkillAdjBaseclass(Save_Vs_BR,   m_name, '%', T_br,   22);
          AddSkillAdjBaseclass(Save_Vs_SP,   m_name, '%', T_sp,   22);
        };
      }
#endif
    }
    else
    {
      WriteDebugString("Unknown BASE_CLASS_DATA serialize version = %s\n",ver);
      result=1;
    };
  };
  return result;
}


void BASE_CLASS_DATA::AddAbilityRequirement(ABILITY_REQ& abReq)
{
  int i, n;
  n=m_abilityReq.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekAbilityReq(i)->Name()==abReq.Name()) break;
    //if (m_abilityReq[i].Name()==abReq.Name()) break;
  };
  //if (abReq.m_ability.m_abilityKey==abilityUnknown)
  //  abReq.m_ability.m_abilityKey = abilityData.FindName(abReq.Name());
  m_abilityReq.SetAtGrow(i,abReq);
}

void BASE_CLASS_DATA::AddExpLevel(DWORD exp)
{
  int n;
  n=m_expLevels.GetSize();
  m_expLevels.SetAtGrow(n,exp);
}

int BASE_CLASS_DATA::GetMaxLevel() const
{
  return m_expLevels.GetSize();
}



int BASE_CLASS_DATA::GetMinExpForLevel(int level) const
{
  if (level <= 1) return 0;
  return (GetMaxExpForLevel(level-1)+1);
}

int BASE_CLASS_DATA::GetMaxExpForLevel(int level) const
{
  static bool msg1=false;
  if (m_expLevels.GetSize()==0)
  {
    if (!msg1)
    {
      WriteDebugString("Baseclass '%s' has no experience levels defined\n", m_name);
      msg1=true;
    };
    return 0;
  }
  if (level < 1) level = 1;
  if (level > m_expLevels.GetUpperBound())
  {
    return 2*(*PeekExpLevels(m_expLevels.GetUpperBound()));
    //return 2*m_expLevels[m_expLevels.GetUpperBound()];
  }
  else
  {
    return *PeekExpLevels(level)-1;
    //return m_expLevels[level]-1;
  };
}


int BASE_CLASS_DATA::GetLevel(DWORD exp) const
{
  int count=0;
  int highestLevelAllowed;
  highestLevelAllowed = m_expLevels.GetSize();
  while (count<highestLevelAllowed)
  {
    if (exp < *PeekExpLevels(count))
    //if (exp < m_expLevels[count])
      return (count);
    count++;
  }
  return highestLevelAllowed;
}

struct {
  BYTE min;
  BYTE minMod;
  BYTE max;
  BYTE maxMod;
} UAabilityLimits[] =
{
  {9,0,18,100},  // Fighter
  {3,0,18,0},{3,0,18,0},{3,0,18,0},{3,0,18,0},{3,0,18,0},

  {3,0,18,0}, //Cleric
  {3,0,18,0},{9,0,18,0},{3,0,18,0},{3,0,18,0},{3,0,18,0},

  {13,0,18,0  },  // Ranger
  {3,0,18,0},{14,0,18,0},{13,0,18,0},{14,0,18,0},{3,0,18,0},

  {12,0,18,0  },  // Paladin
  {3,0,18,0},{13,0,18,0},{3,0,18,0},{9,0,18,0},{17,0,18,0},

  {3,0,18,0  },  // Magic User
  {9,0,18,0},{3,0,18,0},{3,0,18,0},{3,0,18,0},{3,0,18,0},

  {3,0,18,0  },  // Thief
  {3,0,18,0},{3,0,18,0},{9,0,18,0},{3,0,18,0},{3,0,18,0},

  {3,0,18,0  },  // Druid
  {3,0,18,0},{12,0,18,0},{3,0,18,0},{3,0,18,0},{15,0,18,0},

};

#ifdef UAFEDITOR
//void BASE_CLASS_DATA::SetUAAbilityLimits(void)
void BASE_CLASS_DATA::SetUAAbilityLimits(int baseclassArrayIndex)
{
  BYTE *limit;
  int i;
  //ASSERT(m_baseclassKey <= 6);
  ASSERT(baseclassArrayIndex <= 6);
  ABILITY_REQ ar;
  //if (m_baseclassKey > 6) return;
  if (baseclassArrayIndex > 6) return;
  //limit = ((BYTE *)UAabilityLimits+6*m_baseclassKey);
  limit = ((BYTE *)UAabilityLimits+6*baseclassArrayIndex);
  m_abilityReq.SetSize(ABREFKEY_COUNT);
  for (i=0; i<ABREFKEY_COUNT; i++)
  {
    //al.m_ability.m_abilityKey=i; // STR, INT, etc
    ar.m_min=*(limit++);
    ar.m_minMod=*(limit++);
    ar.m_max=*(limit++);
    ar.m_maxMod=*(limit++);
    *GetAbilityReq(i)=ar;
    //m_abilityReq[i]=ar;
  };
}

//void BASE_CLASS_DATA::SetUAAllowedAlignments(void)
void BASE_CLASS_DATA::SetUAAllowedAlignments(int baseclassArrayIndex)
{
  static WORD UAAlignments[7]=
  {
      0x1ff, //Fighter
      0x1ff, //Cleric
      0x007, //Ranger
      0x001, //Paladin
      0x1ff, //MagicUser
      0x1fe, //Thief
      0x1ff  //Druid
  };
  m_allowedAlignments=0x1ff;
  //if (m_baseclassKey > 6) return;
  if (baseclassArrayIndex > 6) return;
  //m_allowedAlignments=UAAlignments[m_baseclassKey];
  m_allowedAlignments=UAAlignments[baseclassArrayIndex];
}
#endif


//ABILITYLIMITS BASE_CLASS_DATA::GetAbilityLimits(abilityType abType) const
ABILITYLIMITS BASE_CLASS_DATA::GetAbilityReqs(const ABILITY_ID abilityID) const
{
  int i,n;
  n=m_abilityReq.GetSize();
  //for (i=0; i<n; i++)
  //{
    // The following was deleted by me, PRS, 20121118.
    // I don't understand what it is doing.
    // Why insert a new entry in the first empty entry?
    // Hmmmmm...seems strange.
    //if (m_abilityReq[i].m_ability.m_abilityKey==abilityUnknown)
    //{
    //  // make sure this abilityType exists in the database
    //  CString keytext = abilityData.GetName(abType);
    //  if (!keytext.IsEmpty())
    //    m_abilityReq[i].m_ability.m_abilityKey = abType;
    //}
    i = LocateAbilityReq(abilityID);
    if (i >= 0)
    //if (m_abilityReq[i].m_ability.m_abilityKey==abType)
    {
      //return ASSEMBLEABILITYLIMITS(m_abilityReq[i].m_min,
      //                             m_abilityReq[i].m_minMod,
      //                             m_abilityReq[i].m_max,
      //                             m_abilityReq[i].m_maxMod);
      const ABILITY_REQ *pAbilityReq;
      pAbilityReq = PeekAbilityReq(i);
      return ASSEMBLEABILITYLIMITS(pAbilityReq->m_min,
                                   pAbilityReq->m_minMod,
                                   pAbilityReq->m_max,
                                   pAbilityReq->m_maxMod);
    };
  //};
  return ASSEMBLEABILITYLIMITS(3,0,18,0);
}


//void BASE_CLASS_DATA::ComputeCharSavingThrow(const CString& versus, int *result, int level) const
//{
//  int val;
//  if (level > HIGHEST_CHARACTER_LEVEL) level = HIGHEST_CHARACTER_LEVEL;
//  if (level < 1) level = 1;
//  Not Implemented(0x74a12, false); val = 0;
//  if (val < *result) *result = val;
//}

SKILL_COMPUTATION::SKILL_COMPUTATION(const CHARACTER *pCH, const CString& skid, bool min, bool includeTemp)
{
  pChar              = pCH;
  skillID            = skid;
  minimize           = min;
  includeTempAdj     = includeTemp;
  baseclassID.Empty();
  //pRace              = raceData.PeekRace(pChar->RaceID());
  pRace              = pChar->PeekRaceData();
  pClass             = classData.PeekClass(pChar->GetClass());
  //pBaseclass         = NULL;
  ppBestBaseclass    = NULL;
  baseclassLevel     = 0;
  baseclassValue     = NoSkill;
  raceValue          = NoSkill;
  baseVal            = NoSkill;    // Best of baseclass and race values
  finalAdjustedValue = NoSkillAdj;
  bestRaceAdj        = NoSkillAdj;
  bestBaseclassAdj   = NoSkillAdj;
  abilityAdj         = NoSkillAdj;
  tempAdj            = NoSkillAdj;
}

void BASE_CLASS_DATA::GetSkillValue(SKILL_COMPUTATION& SC) const
{
  const SKILL *pSkill;
  int indx;
  indx = LocateSkill(SC.skillID);
  if (indx < 0) return;
  pSkill = PeekSkill(indx);
  if (SC.baseclassValue == NoSkill) 
  {
    SC.baseclassValue = pSkill->value;
    if (SC.ppBestBaseclass != NULL)
    {
      *SC.ppBestBaseclass = this;
    };
  }
  else
  {
    if (SC.minimize) 
    {
      if (pSkill->value < SC.baseclassValue) 
      {
        if (SC.ppBestBaseclass != NULL)
        {
          *SC.ppBestBaseclass = this;
        };
          SC.baseclassValue = pSkill->value;
      };
    }
    else
    {
      if (pSkill->value > SC.baseclassValue) 
      {
        if (SC.ppBestBaseclass != NULL)
        {
          *SC.ppBestBaseclass = this;
        };
        SC.baseclassValue = pSkill->value;
      };
    };
  };
}

//void BASE_CLASS_DATA::UpdateSkillValue(const  CHARACTER *pChar,
//                                       const  SKILL_ID&  skillID,
//                                       double            baseVal,
//                                       double           *pBestRaceAdj,
//                                       double           *pBestBaseclassAdj,
//                                       bool              minimize) const
void BASE_CLASS_DATA::UpdateSkillValue(SKILL_COMPUTATION& SC) const
{
  // Update adustments for Race, Baseclass, and Ability.
  int i, n;
//  RACE_ID raceID;
  BASECLASS_ID baseclassID;
  baseclassID = BaseclassID();
//  raceID = SC.pChar->RaceID();
  n = GetSkillAdjBaseclassCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJUSTMENT_BASECLASS *pAdj;
    pAdj = PeekSkillAdjBaseclass(i);
    if (pAdj->skillID == SC.skillID)
    {
      SC.baseclassLevel = SC.pChar->GetBaseclassLevel(pAdj->baseclassID);
      if (SC.baseclassLevel > 0)
      {
        //pAdj->UpdateSkillValue(baseclassLevel, baseVal, pBestBaseclassAdj, minimize);
        pAdj->UpdateSkillValue(SC);
      };
    };
  };
  n = GetSkillAdjAbilityCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJUSTMENT_ABILITY *pAdj;
    pAdj = PeekSkillAdjAbility(i);
    if (pAdj->skillID == SC.skillID)
    {
      //pAdj->UpdateSkillValue(pChar, baseVal, minimize);
      pAdj->UpdateSkillValue(SC);
    };
  };
  n = GetSkillAdjRaceCount();
  for (i=0; i<n; i++)
  {
    const SKILL_ADJUSTMENT_RACE *pAdj;
    pAdj = PeekSkillAdjRace(i);
    if (pAdj->skillID == SC.skillID)
    {
      if (pAdj->adjType == 'I')
      {
        if (SC.pRace != NULL)
        {
          SKILL_ID indirectSkillID;
          SKILL_ID savedSkillID;
          indirectSkillID = pAdj->raceID;
          savedSkillID = SC.skillID;
          SC.skillID = indirectSkillID;
          //pRace->UpdateSkillValue(pChar, 
          //                        indirectSkillID, 
          //                        &baseclassID, 
          //                        baseVal, 
          //                        pBestRaceAdj,
          //                        pBestBaseclassAdj,
          //                        minimize);
          SC.pRace->UpdateSkillValue(SC);
          SC.skillID = savedSkillID;
        };
      }
      else if ((SC.pRace != NULL) && (pAdj->raceID == SC.pRace->RaceID()))
      {
        //pAdj->UpdateSkillValue(baseVal, pBestRaceAdj);
        pAdj->UpdateSkillValue(SC);
      };
    };
  };
}

#ifdef UAFEDITOR
void BASE_CLASS_DATA::AddSkillAdjBaseclass(const CString& skillName,
                                           const CString& baseclassName,
                                           const char adjType,
                                           const BYTE *adjustments,
                                           int   size)
{
  int i;
  SKILL_ADJUSTMENT_BASECLASS skillAdjBaseclass;
  skillAdjBaseclass.baseclassID = m_name;
  skillAdjBaseclass.skillID = skillName;
  skillAdjBaseclass.adjType = adjType;
  skillAdjBaseclass.baseclassID = baseclassName;
  for (i=0; i<HIGHEST_CHARACTER_LEVEL; i++)
  {
    if (i < size)
    {
      skillAdjBaseclass.skillAdj[i] = adjustments[i];
    }
    else
    {
      skillAdjBaseclass.skillAdj[i] = adjustments[size-1];
    };
  };
  m_skillAdjustmentsBaseclass.Add(skillAdjBaseclass);
}


void BASE_CLASS_DATA::AddDefaultBonusXP(const CString& abilityName,
                                              int      abilityValue,
                                              int      bonusValue)
{
  int i;
  BONUS_XP bonusXP;
  bonusXP.abilityID = abilityName;
  bonusXP.bonusType = '^';
  for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
  {
    if (i < abilityValue)
    {
      bonusXP.bonus[i] = 0;
    }
    else
    {
      bonusXP.bonus[i] = bonusValue;
    };
  };
  m_bonusXP.Add(bonusXP);
}

void BASE_CLASS_DATA::AddSkill(const CString& skillName, int value)
{
  SKILL skill;
  skill.skillID = skillName;
  skill.value = value;
  m_skills.Add(skill);
}
#endif



//bool BASE_CLASS_DATA::IsAllowedRace(raceType race) const
bool BASE_CLASS_DATA::IsAllowedRace(const RACE_ID& raceID) const
{
  //int i,n;
  //n = m_allowedRaces.GetSize();
  //for (i=0; i<n; i++)
  //{
  //  //if (m_allowedRaces[i].m_raceKey == race) return true;
  //  if (m_allowedRaces[i].m_raceID == raceID) return true;
  //};
  //return false;
  return LocateRaceID(raceID) >= 0;
}


int BASE_CLASS_DATA::LocateRaceID(const RACE_ID& raceID) const
{
  int i, n;
  n = m_allowedRaces.GetSize();
  for (i=0; i<n; i++)
  {
    if (*PeekRaceID(i) == raceID) return i;
  };
  return -1;
}

int BASE_CLASS_DATA::LocateAbilityReq(const ABILITY_ID& abilityID) const
{
  int i, n;
  n = m_abilityReq.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekAbilityReq(i)->m_abilityID == abilityID) return i;
  };
  return -1;
}


int BASE_CLASS_DATA::LocateCastingInfo(const SCHOOL_ID& schoolID) const
{
  int i, n;
  n = m_castingInfo.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekCastingInfo(i)->schoolID == schoolID) return i;
  };
  return -1;
};


BASE_CLASS_DATA_TYPE::BASE_CLASS_DATA_TYPE(void)
{
#ifdef UAFEDITOR
  LoadUADefaults();
#endif
}


BOOL BASE_CLASS_DATA_TYPE::operator == (const BASE_CLASS_DATA_TYPE& src) const
{
  int i, n;
  n = src.GetCount();
  if (n != GetCount()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (!(*PeekBaseclass(i) == *src.PeekBaseclass(i))) return FALSE;
  };
  return TRUE;
}


#ifdef UAFEDITOR
void BASE_CLASS_DATA_TYPE::LoadUADefaults(void)
{ // You must do your own Clear() if you want it done.
  BASE_CLASS_DATA bc;
  //baseclassKey i;
  int arrayIndex;
  //POSITION pos;
  bc.m_name="Fighter";
    m_baseclassData.Add(bc);
  bc.m_name="Cleric";
    m_baseclassData.Add(bc);
  bc.m_name="Ranger";
    m_baseclassData.Add(bc);
  bc.m_name="Paladin";
    m_baseclassData.Add(bc);
  bc.m_name="Magic User";
    m_baseclassData.Add(bc);
  bc.m_name="Thief";
    m_baseclassData.Add(bc);
  bc.m_name="Druid";
    m_baseclassData.Add(bc);
  //for (i=0; i<=6; i++)
  for (arrayIndex=0; arrayIndex<=6; arrayIndex++)
  {
    //pos=m_BaseclassData.FindKeyPos(i);
    //ASSERT(pos!=NULL);
    //if (pos==NULL) continue;
    //m_BaseclassData.GetAtPos(pos).SetUAAbilityLimits();
    //m_BaseclassData.GetAtPos(pos).SetUAAllowedAlignments();
    GetBaseclass(arrayIndex)->SetUAAbilityLimits(arrayIndex);
    GetBaseclass(arrayIndex)->SetUAAllowedAlignments(arrayIndex);
  };
};
#endif


BASE_CLASS_DATA_TYPE::~BASE_CLASS_DATA_TYPE(void)
{
}

void BASE_CLASS_DATA_TYPE::Clear(void)
{
  m_baseclassData.RemoveAll();
}

int BASE_CLASS_DATA_TYPE::LocateBaseclass(const BASECLASS_ID& baseclassID) const
{
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    if (PeekBaseclass(i)->m_name == baseclassID) return i;
  };
  return -1;
}

int BASE_CLASS_DATA_TYPE::GetDataSize(void)
{
  //POSITION pos;
  int i, n;
  int size = sizeof (*this);
  //pos = m_BaseclassData.GetHeadPosition();
  //while (pos!=NULL)
  n = GetCount();
  for (i=0; i<n; i++)
  {
    //size += m_BaseclassData.GetNext(pos).GetDataSize();
    size += PeekBaseclass(i)->GetDataSize();
  };
  return size;
}


/*
baseclassKey BASE_CLASS_DATA_TYPE::GetNextBaseclass()
{
  baseclassKey key=baseclassUnknown;
  if (GetCount() > 0)
  {
    key = (baseclassKey)m_BaseclassData.GetTail().m_baseclassKey;
    if (key >= UINT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is much greater than
      // MAX_BASE_CLASSES
      // Gaps occur when deletions are made to the baseclass list
      POSITION pos = m_BaseclassData.GetHeadPosition();
      BOOL found = FALSE;
      DWORD prevKey = 0;
      DWORD currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = m_BaseclassData.GetNext(pos).m_baseclassKey;
        if (currKey != prevKey+1)
        {
          key = (baseclassKey)(prevKey+1);
          found = TRUE;
        }
        else
          prevKey = currKey;
      }
      VERIFY(found); // should never happen
    }
    else
      key=(baseclassKey)(key+1);
  }
  else
    key = (baseclassKey)0;
  return key;
}
*/

int BASECLASS_ID::Serialize(CAR& car)
{
  CString *pStr;
  pStr = this;
  if (car.IsStoring())
  {
    car << *pStr;
  }
  else
  {
    car >> *pStr;
  };
  return 0;
}

int CLASS_ID::Serialize(CAR& car)
{
  CString *pStr;
  pStr = this;
  if (car.IsStoring())
  {
    car << *pStr;
  }
  else
  {
    car >> *pStr;
  };
  return 0;
}

int BASECLASS_ID::Serialize(CArchive& ar)
{
  CString *pStr;
  pStr = this;
  if (ar.IsStoring())
  {
    ar << *pStr;
  }
  else
  {
    ar >> *pStr;
  };
  return 0;
}

int CLASS_ID::Serialize(CArchive& ar)
{
  CString *pStr;
  pStr = this;
  if (ar.IsStoring())
  {
    ar << *pStr;
  }
  else
  {
    ar >> *pStr;
  };
  return 0;
}


int BASECLASS_LIST::LocateBaseclassID(const BASECLASS_ID& baseclassID) const
{
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    if (*PeekBaseclassID(i) == baseclassID) return i;
  };
  return -1;
}

void BASECLASS_LIST::operator = (const BASECLASS_LIST& src)
{
  int i, n;
  baseclasses.RemoveAll();
  n = src.GetCount();
  for (i=0; i<n; i++)
  {
    baseclasses.Add(*const_cast<BASECLASS_ID *>(src.PeekBaseclassID(i)));
  };
}

void BASECLASS_LIST::Serialize(CAR& car)
{
  int i, n;
  n = GetCount();
  if (car.IsStoring())
  {
    car << n;
    for (i=0; i<n; i++)
    {
      car << *PeekBaseclassID(i);
    };
  }
  else
  {
    BASECLASS_ID baseclassID;
    car >> n;
    baseclasses.RemoveAll();
    for (i=0; i<n; i++)
    {
      car >> baseclassID;
      baseclasses.Add(baseclassID);
    };
  };
}

void BASECLASS_LIST::Serialize(CArchive& ar)
{
  die("BASECLASS_LIST Serialize(CArchive&)"); //Not Implemented(0x7feba4, false); // We should not be doing this!
}

BOOL BASECLASS_LIST::operator == (const BASECLASS_LIST& src) const
{
  int i, n;
  n = GetCount();
  if (n != src.GetCount()) return FALSE;
  for (i=0; i<n; i++)
  {
    if (*PeekBaseclassID(i) != *src.PeekBaseclassID(i)) return FALSE;
  };
  return TRUE;
}

BOOL BASECLASS_LIST::operator &(const BASECLASS_LIST& src) const
{  // Any overlap in the two lists?
  int i, j, m, n;
  m = GetCount();
  n = src.GetCount();
  for (i=0; i<m; i++)
  {
    for (j=0; j<n; j++)
    {
      if (*PeekBaseclassID(i) == *src.PeekBaseclassID(j)) return TRUE;
    };
  };
  return FALSE;
}

void BASECLASS_LIST::Remove(const BASECLASS_ID& baseclassID)
{
  int indx;
  indx = LocateBaseclassID(baseclassID);
  if (indx >= 0)
  {
    baseclasses.RemoveAt(indx);
  };
}


int CLASS_DATA_TYPE::LocateClass(const CLASS_ID& classID) const
{
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    if (PeekClass(i)->m_name == classID) return i;
  };
  return -1;
}

#ifdef UAFEDITOR
extern const int NumClassText;
extern const char *ClassText[];
CLASS_ID CLASS_DATA_TYPE::FindPreVersionSpellNamesClassID(DWORD classType) const
{
  int i, n;
  CLASS_ID classID;
  n = GetClassCount();
  for (i=0; i<n; i++)
  {
    const CLASS_DATA *pClass;
    pClass = PeekClass(i);
    if (pClass->preSpellNameKey == (int)classType)
    {
      return pClass->ClassID();
    };
  };
  {
    classID = ClassText[classType];
  };
  if (!debugStrings.AlreadyNoted("CDBK"))
  {
    MsgBoxInfo("Searching for a Class that was defined by its 'key'.  We lost that information "
               "at about version 0.998101 of the editor.  If this is a problem for you, please "
               "contact the DC development team for help.");
  };
  classID=ClassText[0];
  return classID;
}

#endif

// Called when loading from scripts, or
// when adding from editor GUI
//baseclassKey BASE_CLASS_DATA_TYPE::AddBaseclass(const BASE_CLASS_DATA &data)
BASECLASS_ID BASE_CLASS_DATA_TYPE::AddBaseclass(const BASE_CLASS_DATA &data)
{
  //CSingleLock sLock(&m_CS, TRUE);
  //if (data.m_name.IsEmpty()) return baseclassUnknown;
  //aaaif (data.m_name.IsEmpty()) return BASECLASS_ID();
  // Do not allow multiple items with same full name.
  //if (m_FindName(data.m_name, false)!=baseclassUnknown) return baseclassUnknown;
  //if (m_FindName(data.m_name, false)!=baseclassUnknown) return BASECLASS_ID();
  if (LocateBaseclass(data.BaseclassID()) >= 0) return BASECLASS_ID();
  if (GetCount() < MAX_BASE_CLASSES)
  {
    //data.m_baseclassKey = GetNextBaseclass();
    //m_BaseclassData.Insert(data, data.m_baseclassKey);
    m_baseclassData.Add(const_cast<BASE_CLASS_DATA&>(data));
    //return (baseclassKey)data.m_baseclassKey;
    return PeekBaseclass(GetCount()-1)->BaseclassID();
  }
  else
    //return baseclassUnknown;
    return BASECLASS_ID();
}

/*
// called when loading binary baseclass data that
// was saved with key info
baseclassKey BASE_CLASS_DATA_TYPE::AddBaseclassWithCurrKey(const BASE_CLASS_DATA &data)
{
  //CSingleLock sLock(&m_CS, TRUE);
  if (m_BaseclassData.GetCount() < MAX_BASE_CLASSES)
  {
    m_BaseclassData.Insert(data, data.m_baseclassKey);
    return (baseclassKey)data.m_baseclassKey;
  }
  else
    return baseclassUnknown;
}
*/
/*
// With locking...for public use
baseclassKey BASE_CLASS_DATA_TYPE::FindName(const char *name) const
// Return key associated with the named object, or zero
{
  return m_FindName(name);
}
*/


void BASE_CLASS_DATA_TYPE::PreSerialize(BOOL IsStoring)
{
  //POSITION pos;
  //pos=m_BaseclassData.GetHeadPosition();
  int i,n;
  n = GetCount();
  //while (pos!=NULL)
  for (i=0; i<n; i++)
     //m_BaseclassData.GetNext(pos).PreSerialize(IsStoring);
     GetBaseclass(i)->PreSerialize(IsStoring);
}

void BASE_CLASS_DATA_TYPE::PostSerialize(BOOL IsStoring)
{
}

int BASE_CLASS_DATA_TYPE::Serialize(CAR& car)
{
  //POSITION p;
  CString version("BaseclassV1");
  int result=0;

  if (car.IsStoring())
  {
//    car << "BaseclassV0";
    int i, n;
    car << version;  // V1 include compression
    car.Compress(true);
    car << GetCount();
    //p = m_BaseclassData.GetHeadPosition();
    //while (p != NULL)
    //  m_BaseclassData.GetNext(p).Serialize(car);
    n = GetCount();
    for (i=0; i<n; i++)
    {
      GetBaseclass(i)->Serialize(car);
    };
  }
  else
  {
    BASE_CLASS_DATA data;
    int count;
    car >> version;
    if ((version =="BaseclassV0")||(version =="BaseclassV1"))
    {
      if (version > "BaseclassV0") car.Compress(true);
      car >> count;
      for (int i=0; i<count; i++)
      {
        result=data.Serialize(car);
        if (result!=0) return result;
        //AddBaseclassWithCurrKey(data);
        AddBaseclass(data);
      }
    }
    else
    {
      WriteDebugString("Unknown Baseclass data version = %s\n",version);
      result=1;
    };
  };
  return result;
}
#ifdef UAFEDITOR
void BASE_CLASS_DATA_TYPE::CrossReference(CR_LIST *pCRList)
{
  int i, n;
  n = GetCount();
  for (i = 0; i < n; i++)
  {
    GetBaseclass(i)->CrossReference(pCRList);
  };
}
#endif


/*
// No locking... for internal use
baseclassKey BASE_CLASS_DATA_TYPE::m_FindName (const char *name, bool errorIfMissing) const
// Return key associated with the named object, or baseclassUnknown
{
  static bool msg1=false;
  POSITION pos;
  pos=m_BaseclassData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_BaseclassData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return (baseclassKey)m_BaseclassData.PeekAtPos(pos).m_baseclassKey;
    m_BaseclassData.PeekNext(pos);
  };
  if (errorIfMissing)
  {
    if (!msg1)
    {
      msg1 = true;
      WriteDebugString("Cannot find definition of baseclass '%s'\n", name);
    };
  };
  return baseclassUnknown;
}
*/

/*
BASE_CLASS_DATA *BASE_CLASS_DATA_TYPE::GetBaseclassData (baseclassKey base)
// Return key associated with the named object, or baseclassUnknown
{
  POSITION pos;
  pos=m_BaseclassData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_BaseclassData.PeekAtPos(pos).m_baseclassKey == base)
          return &m_BaseclassData.GetAtPos(pos);
    m_BaseclassData.PeekNext(pos);
  };
  return NULL;
}
*/



//ABILITYLIMITS BASE_CLASS_DATA_TYPE::GetAbilityLimits(baseclassKey bType, abilityType abType)
ABILITYLIMITS BASE_CLASS_DATA_TYPE::GetAbilityLimits(const BASECLASS_ID& baseclassID,
                                                     const ABILITY_ID& abilityID) const
{
  //POSITION pos;
  //pos=m_BaseclassData.FindKeyPos(bType);
  //ASSERT(pos!=NULL);
  //if (pos==NULL) return ASSEMBLEABILITYLIMITS(3,0,18,0);
  int arrayIndex;
  arrayIndex = LocateBaseclass(baseclassID);
  if (arrayIndex < 0) return ASSEMBLEABILITYLIMITS(3,0,18,0);
  //return PeekBaseclass(arrayIndex)->GetAbilityLimits(abType);
  return PeekBaseclass(arrayIndex)->GetAbilityReqs(abilityID);
}

//WORD BASE_CLASS_DATA_TYPE::GetAllowedAlignments(baseclassKey bType) const
WORD BASE_CLASS_DATA_TYPE::GetAllowedAlignments(const BASECLASS_ID& baseclassID) const
{
  //POSITION pos;
  //pos=m_BaseclassData.FindKeyPos(bType);
  //ASSERT(pos!=NULL);
  int arrayIndex;
  arrayIndex = LocateBaseclass(baseclassID);
  //if (pos==NULL) return 0x1ff;
  if (arrayIndex < 0) return 0x1ff;
  //return m_BaseclassData.PeekAtPos(pos).m_allowedAlignments;
  return GetAllowedAlignments(arrayIndex);
}

WORD BASE_CLASS_DATA_TYPE::GetAllowedAlignments(int arrayIndex) const
{
  return PeekBaseclass(arrayIndex)->m_allowedAlignments;
}

//bool BASE_CLASS_DATA_TYPE::IsAllowedAlignment(alignmentType align, const CString& baseclassName)
bool BASE_CLASS_DATA_TYPE::IsAllowedAlignment(const BASECLASS_ID& baseclassID, alignmentType align) const
{
  WORD allowedAlign;
  //baseclassKey baseclass;
  //baseclass = FindName(baseclassName);
  int arrayIndex;
  arrayIndex = LocateBaseclass(baseclassID);
  //if (baseclass == baseclassUnknown) return false;
  if (arrayIndex < 0) return false;
  //allowedAlign = this->GetAllowedAlignments(baseclass);
  allowedAlign = this->GetAllowedAlignments(arrayIndex);
  return ((1 << align) & allowedAlign) != 0;
}

//bool BASE_CLASS_DATA_TYPE::IsAllowedRace(baseclassKey baseclass, raceType race) const
bool BASE_CLASS_DATA_TYPE::IsAllowedRace(const BASECLASS_ID& baseclassID, const RACE_ID& raceID) const
{
  //POSITION pos;
  //pos = m_BaseclassData.FindKeyPos(baseclass);
  int arrayIndex;
  arrayIndex = LocateBaseclass(baseclassID);
  //if (pos == NULL) return false;
  if (arrayIndex < 0) return false;
  //return m_BaseclassData.PeekAtPos(pos).IsAllowedRace(race);
  return PeekBaseclass(arrayIndex)->IsAllowedRace(raceID);
}

//int BASE_CLASS_DATA_TYPE::GetMaxExpForLevel(baseclassKey bType, int level) const
int BASE_CLASS_DATA_TYPE::GetMaxExpForLevel(const BASECLASS_ID& baseclassID, int level) const
{
  static bool msg1=false;
  //POSITION pos=m_BaseclassData.FindKeyPos(bType);
  int arrayIndex;
  arrayIndex = LocateBaseclass(baseclassID);
  //if (pos==NULL)
  if (arrayIndex < 0)
  {
    if (!msg1)
    {
      msg1 = true;
      WriteDebugString("Unable to determine Max Experience for baseclass\n");
    };
    return 0;
  };
  //return (m_BaseclassData.PeekAtPos(pos).GetMaxExpForLevel(level));
  return PeekBaseclass(arrayIndex)->GetMaxExpForLevel(level);
}

//int BASE_CLASS_DATA_TYPE::GetMinExpForLevel(baseclassKey bType, int level) const
int BASE_CLASS_DATA_TYPE::GetMinExpForLevel(const BASECLASS_ID& baseclassID, int level) const
{
  //POSITION pos=m_BaseclassData.FindKeyPos(bType);
  int arrayIndex;
  arrayIndex = LocateBaseclass(baseclassID);
  //if (pos==NULL) return 0;
  if (arrayIndex < 0) return 0;
  //return (m_BaseclassData.PeekAtPos(pos).GetMinExpForLevel(level));
  return PeekBaseclass(arrayIndex)->GetMinExpForLevel(level);
}

/*
int BASE_CLASS_DATA_TYPE::GetMaxExpForLevel(const char *baseclassName, int level) const
{
  BASECLASS_ID baseclassID;
  baseclassID = baseclassName;
  //return GetMaxExpForLevel(FindName(baseclassName), level);
  return GetMaxExpForLevel(baseclassID, level);
}

int BASE_CLASS_DATA_TYPE::GetMinExpForLevel(const char *baseclassName, int level) const
{
  BASECLASS_ID baseclassID;
  baseclassID = baseclassName;
  //return GetMinExpForLevel(FindName(baseclassName), level);
  return GetMinExpForLevel(baseclassID, level);
}
*/
//int BASE_CLASS_DATA_TYPE::GetLevel(const char *baseclassName, int exp) const
int BASE_CLASS_DATA_TYPE::GetLevel(const BASECLASS_ID& baseclassID, int exp) const
{
  //baseclassKey bType;
  //POSITION pos;
  //const BASE_CLASS_DATA *pBaseClass;
  //if (exp==0) return 1;
  //bType = FindName(baseclassName);
  int i;
  i = LocateBaseclass(baseclassID);
  if (i <= 0) return 0;
  return PeekBaseclass(i)->GetLevel(exp);
  //if (bType == baseclassUnknown) return 1;

  //pos = m_BaseclassData.FindKeyPos(bType);
  //pBaseClass = &m_BaseclassData.PeekAtPos(pos);

  //return pBaseClass->GetLevel(exp);



  //return 1;
}

void HIT_DICE_LEVEL_BONUS::Serialize(CAR& car)
{
  int i;
  if (car.IsStoring())
  {
    car << baseclassID;
    car << ability;
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      car << bonusValues[i];
    };
  }
  else
  {
    car >> baseclassID;
    car >> ability;
    for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
    {
      car >> bonusValues[i];
    };
  };
}


bool HIT_DICE_LEVEL_BONUS::operator == (const HIT_DICE_LEVEL_BONUS& src) const
{
  int i;
  if (ability     != src.ability)     return false;
  if (baseclassID != src.baseclassID) return false;
  for (i=0; i<HIGHEST_CHARACTER_PRIME; i++)
  {
    if (bonusValues[i] != src.bonusValues[i]) return false;
  };
  return true;
}

CLASS_DATA::CLASS_DATA(void):m_specialAbilities(false) 
{
  Clear();
}

CLASS_DATA::~CLASS_DATA(void)
{
}

void CLASS_DATA::Clear(void)
{
  m_baseclasses.Clear();
  m_name.Empty();
  //m_classKey = classUnknown;
  preSpellNameKey = -1;
  m_specialAbilities.Clear();
  //hitDiceBonusAbility.Empty();
  //memset(hitDiceBonusValues,0, sizeof(hitDiceBonusValues));
  hitDiceLevelBonus.RemoveAll();
  strengthBonusDice.Clear();
  m_startingEquipment.Clear();
  hitDiceBaseclassID.Clear();
}

int CLASS_DATA::GetDataSize(void) const
{
  int i,n;
  int size = sizeof (*this);
  size += m_name.GetLength();
  size += m_specialAbilities.GetDataSize();
  n=m_baseclasses.GetCount();
  for (i=0; i<n; i++) size += m_baseclasses.PeekBaseclassID(i)->GetDataSize();
  return size;
}


//void CLASS_DATA::AddBaseclass(baseclassKey bclass)
void CLASS_DATA::AddBaseclass(const BASECLASS_ID& baseclassID)
{
  int n;
  //BASECLASS_REFERENCE br;
  //br.m_unusedBaseclassKey=bclass;
  n=m_baseclasses.GetCount();
  for (int i=0; i<n; i++)
  {
    //if (m_baseclasses[i].m_unusedBaseclassKey==bclass) return;
    if (*m_baseclasses.PeekBaseclassID(i) == baseclassID) return;
  };
  {
    //m_baseclasses.SetAtGrow(n,br);
    m_baseclasses.Add(baseclassID);
  };
}
/*
void CLASS_DATA::AddBaseclass(BASECLASS_REFERENCE& bclass)
{
  int n;
  n=m_baseclasses.GetSize();
  for (int i=0; i<n; i++)
  {
    //if (m_baseclasses[i].m_baseclassName==bclass.m_baseclassName) return;
    if (m_baseclasses[i].m_baseclassID==bclass.m_baseclassID) return;
  };
  //if (bclass.m_unusedBaseclassKey==baseclassUnknown)
  //{
  //  bclass.m_unusedBaseclassKey =
  //    baseclassData.FindName(bclass.m_baseclassName);
  //}
  //m_baseclasses.SetAtGrow(n,bclass);
  m_baseclasses.Add(bclass);
}
*/
/*
baseclassKey CLASS_DATA::GetNextBaseclass(baseclassKey baseclass)
{
  int index;
  if (baseclass == classUnknown)
  {
     index = 0;
  }
  else
  {
    for (index = 0; index <m_baseclasses.GetSize(); index++)
    {
      if (m_baseclasses[index].m_unusedBaseclassKey == (unsigned)baseclass)
      {
        index++;
        break;
      };
    };
  };
  if (index < m_baseclasses.GetSize())
  {
    return (classType)m_baseclasses[index].m_unusedBaseclassKey;
  }
  else
  {
    return classUnknown;
  };
}
*/

BOOL CLASS_DATA::operator ==(const CLASS_DATA& src) const
{
  if (!(hitDiceBaseclassID    == src.hitDiceBaseclassID))    return FALSE;
  if (!(m_baseclasses         == src.m_baseclasses))         return FALSE;
  //if (memcmp(hitDiceBonusValues, src.hitDiceBonusValues, sizeof(hitDiceBonusValues)) != 0)    return FALSE;
  {
    int i,n;
    n = GetHitDiceLevelBonusCount();
    if (n != src.GetHitDiceLevelBonusCount()) return FALSE;
    for (i=0; i<n; i++)
    {
      if (!(*PeekHitDiceLevelBonus(i) == *src.PeekHitDiceLevelBonus(i))) return FALSE;
      //if (!(hitDiceLevelBonus[i] == src.hitDiceLevelBonus[i])) return FALSE;
    };
  };
  //if (!(hitDiceBonusAbility   == src.hitDiceBonusAbility))   return FALSE;
  if (!(strengthBonusDice     == src.strengthBonusDice))     return FALSE;
  if (!(m_name                == src.m_name))                return FALSE;
  if (!(m_specialAbilities    == src.m_specialAbilities))    return FALSE;
  if (!(m_startingEquipment   == src.m_startingEquipment))   return FALSE;
  return TRUE;
}


CLASS_DATA& CLASS_DATA::operator =(const CLASS_DATA& src)
{
  int size;
  int i;
  if (this==&src) return *this;
  Clear();
  //m_classKey = src.m_classKey;
  preSpellNameKey = src.preSpellNameKey;
  m_name     = src.m_name;
  size=src.m_baseclasses.GetCount();
  //m_baseclasses.SetSize(size);
  for (i=0; i<size; i++)
  {
    //m_baseclasses[i]=src.m_baseclasses[i];
    m_baseclasses.Add(*src.PeekBaseclassID(i));
  };
  m_specialAbilities.Copy(src.m_specialAbilities);
  strengthBonusDice = src.strengthBonusDice;
  //hitDiceBonusAbility = src.hitDiceBonusAbility;
  hitDiceBaseclassID = src.hitDiceBaseclassID;
  //memcpy(hitDiceBonusValues, src.hitDiceBonusValues, sizeof(hitDiceBonusValues));
  hitDiceLevelBonus.RemoveAll();
  hitDiceLevelBonus.Append(src.hitDiceLevelBonus);
  m_startingEquipment = src.m_startingEquipment;
  return *this;
}


//ABILITYLIMITS CLASS_DATA::GetAbilityLimits(abilityType abType) const
ABILITYLIMITS CLASS_DATA::GetAbilityLimits(ABILITY_ID abilityID) const
{
  int i,n;
  int min=0,minmod=0,max=9999,maxmod=9999;
  int rmin,rminmod,rmax,rmaxmod;
  ABILITYLIMITS r;
  n=m_baseclasses.GetCount();
  for (i=0; i<n; i++)
  {
    //r=baseclassData.GetAbilityLimits(m_baseclasses[i].m_unusedBaseclassKey, abType);
    r=baseclassData.GetAbilityLimits(*m_baseclasses.PeekBaseclassID(i), abilityID);
    rmin=ABILITYMINBASE(r);
    rminmod=ABILITYMINMOD(r);
    rmax=ABILITYMAXBASE(r);
    rmaxmod=ABILITYMAXMOD(r);
    if (rmin>min) {
      min=rmin;
      minmod=rminmod;
    }
    else if (rmin==min)
    {
      if (rminmod>minmod) minmod=rminmod;
    };
    if (rmax<max) {
      max=rmax;
      maxmod=rmaxmod;
    }
    else if (rmax==max)
    {
      if (rmaxmod>maxmod) maxmod=rmaxmod;
    };
  };
  return ASSEMBLEABILITYLIMITS(min,minmod,max,maxmod);
}


#ifdef OldDualClass20180126
int CLASS_DATA::GetCharTHAC0(const CHARACTER *pChar) const
{
  int i, n, THAC0, thac0;
  const BASECLASS_ID *pBaseclassID;
  const BASE_CLASS_DATA *pBaseclass;
  const BASECLASS_STATS *pBaseclassStats;
  n = GetCount();
  THAC0 = 20;
  for (i=0; i<n; i++)
  {
    int level;
    pBaseclassID = PeekBaseclassID(i);
    if (pBaseclassID == NULL) continue;
    pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
    if (pBaseclass == NULL) continue;
    pBaseclassStats = pChar->PeekBaseclassStats(*pBaseclassID);
    if (pBaseclassStats == NULL) continue;
    level = pBaseclassStats->currentLevel;
    if (level < 1) level = 1;
    if (level >HIGHEST_CHARACTER_LEVEL) level = HIGHEST_CHARACTER_LEVEL;
    thac0 = pBaseclass->THAC0[level-1];;
    if (thac0 < THAC0) THAC0 = thac0;
  };
  return THAC0;
}
#endif

/*
void CLASS_DATA::ComputeCharSavingThrows(const CHARACTER *pChar,
                                         int *ppd,
                                         int *rsw,
                                         int *pp,
                                         int *br,
                                         int *sp) const
{
  int i, n;
  const BASECLASS_ID *pBaseclassID;
  const BASE_CLASS_DATA *pBaseclass;
  const BASECLASS_STATS *pBaseclassStats;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    int level;
    pBaseclassID = PeekBaseclassID(i);
    if (pBaseclassID == NULL) continue;
    pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
    if (pBaseclass == NULL) continue;
    pBaseclassStats = pChar->PeekBaseclassStats(*pBaseclassID);
    if (pBaseclassStats == NULL) continue;
    level = pBaseclassStats->currentLevel;
    if (level < 1) level = 1;
    if (level >HIGHEST_CHARACTER_LEVEL) level = HIGHEST_CHARACTER_LEVEL;

    pBaseclass->ComputeCharSavingThrow(Save_Vs_PPDM, ppd, level);
    pBaseclass->ComputeCharSavingThrow(Save_Vs_PP  , pp,  level);
    pBaseclass->ComputeCharSavingThrow(Save_Vs_RSW , rsw, level);
    pBaseclass->ComputeCharSavingThrow(Save_Vs_BR  , br,  level);
    pBaseclass->ComputeCharSavingThrow(Save_Vs_SP  , sp,  level);
  };
  return;
}
*/
/*
void CLASS_DATA::GetThiefSkills(const CHARACTER *pChar,
                      int *pp,
                      int *ol,
                      int *ft,
                      int *ms,
                      int *hs,
                      int *hn,
                      int *cw,
                      int *rl) const
{
  int i, n;
  const BASECLASS_ID *pBaseclassID;
  const BASE_CLASS_DATA *pBaseclass;
  const BASECLASS_STATS *pBaseclassStats;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    int level;
    pBaseclassID = PeekBaseclassID(i);
    if (pBaseclassID == NULL) continue;
    pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
    if (pBaseclass == NULL) continue;
    pBaseclassStats = pChar->PeekBaseclassStats(*pBaseclassID);
    if (pBaseclassStats == NULL) continue;
    level = pBaseclassStats->currentLevel;
    if (level < 1) level = 1;
    if (level >HIGHEST_CHARACTER_LEVEL) level = HIGHEST_CHARACTER_LEVEL;

    SKILL_ID skillID;
    pBaseclass->GetSkillValue((skillID=Skill_PickPockets, skillID),   pp, level);
    pBaseclass->GetSkillValue((skillID=Skill_OpenLocks, skillID),     ol, level);
    pBaseclass->GetSkillValue((skillID=Skill_FindTraps, skillID),     ft, level);
    pBaseclass->GetSkillValue((skillID=Skill_MoveSilent, skillID),    ms, level);
    pBaseclass->GetSkillValue((skillID=Skill_HideInShadows, skillID), hs, level);
    pBaseclass->GetSkillValue((skillID=Skill_HearNoise, skillID),     hn, level);
    pBaseclass->GetSkillValue((skillID=Skill_ClimbWalls, skillID),    cw, level);
    pBaseclass->GetSkillValue((skillID=Skill_ReadLanguages, skillID), rl, level);
  };
  return;
}
*/




WORD CLASS_DATA::GetAllowedAlignments(void) const
{
  int i,n;
  WORD result=0x1ff;
  n=m_baseclasses.GetCount();
  for (i=0; i<n; i++)
  {
    //result&=baseclassData.GetAllowedAlignments(m_baseclasses[i].m_unusedBaseclassKey);
    result&=baseclassData.GetAllowedAlignments(*m_baseclasses.PeekBaseclassID(i));
  };
  return result;
}

int CLASS_DATA::DetermineCharHitDice(const CHARACTER *pChar) const
{
  int i, n;
  bool average, max;
  int totalDice=0, maxDice=0, baseclassCount=0;
  average = hitDiceBaseclassID == "*Average*";
  max     = hitDiceBaseclassID == "*Maximum*";
  n = GetCount();
  for (i=0; i<n; i++)
  {
    const BASECLASS_ID *pBaseclassID;
    const BASE_CLASS_DATA *pBaseclass;
    pBaseclassID = PeekBaseclassID(i);
    pBaseclass = baseclassData.PeekBaseclass(*pBaseclassID);
    if (pBaseclass != NULL)
    {
      if (average || max || (hitDiceBaseclassID == pBaseclass->m_name))
      {
        const BASECLASS_STATS *pBaseclassStats;
        baseclassCount++;
        pBaseclassStats = pChar->PeekBaseclassStats(*pBaseclassID);
        if (pBaseclassStats != NULL)
        {
          pBaseclass->DetermineCharHitDice(&totalDice, &maxDice, pBaseclassStats->currentLevel);
        };
      };
    };
  };
  if (average)
  {
    if (baseclassCount > 0)
    {
      return totalDice /baseclassCount;
    }
    else
    {
      return 0;
    };
  }
  else if (max)
  {
    return maxDice;
  }
  else
  {
    return maxDice;
  };
}

//int CLASS_DATA::GetHitDiceBonus(int prime) const
//{
//  if (prime < 1) prime = 1;
//  if (prime > HIGHEST_CHARACTER_PRIME) prime = HIGHEST_CHARACTER_PRIME;
//  return hitDiceBonusValues[prime-1];
//}

//bool CLASS_DATA::IsRaceAllowed(raceType race) const
bool CLASS_DATA::IsRaceAllowed(const RACE_ID& raceID) const
{ //This race is allowed if we have a single Base Class
  // and the Base Class allows this race.
  // or the race explicitly allows this class.
  //
  //baseclassKey baseclass;
  BASECLASS_ID baseclassID;
  //if (raceData.IsAllowedClass(race, m_name)) return true;
  if (raceData.IsAllowedClass(raceID, ClassID())) return true;
  if (m_baseclasses.GetCount() != 1) return false;
  //baseclass =  m_baseclasses[0].m_unusedBaseclassKey;
  baseclassID =  *m_baseclasses.PeekBaseclassID(0);
  //return baseclassData.IsAllowedRace(baseclass, race);
  return baseclassData.IsAllowedRace(baseclassID, raceID);
}

void CLASS_DATA::PreSerialize(BOOL IsStoring)
{
  //int i,n;
  //n=m_baseclasses.GetSize();
  //for (i=0; i<n; i++) m_baseclasses[i].PreSerialize(IsStoring);
}

int CLASS_DATA::Serialize(CAR &car)
{
  int i,n;
  int result=0;
  CString ver("CL5");

  if (car.IsStoring())
  {
    car << ver; // version
    //car << m_classKey;
    car << preSpellNameKey;
    if (m_name=="") m_name="*";
    car << m_name;
    if (m_name=="*") m_name="";
    n=m_baseclasses.GetCount();
    car.WriteCount(n);
    //for (i=0; i<n; i++) m_baseclasses[i].Serialize(car);
    for (i=0; i<n; i++)
    {
      m_baseclasses.GetBaseclassID(i)->Serialize(car);
    };
    m_specialAbilities.Serialize(car, globalData.version,"","");
    //car << hitDiceBonusAbility;
    //car.Serialize((char *)hitDiceBonusValues, sizeof(hitDiceBonusValues));
    {
      int j, m;
      m = hitDiceLevelBonus.GetSize();
      car << m;
      for (j=0; j<m; j++)
      {
        GetHitDiceLevelBonus(j)->Serialize(car);
        //hitDiceLevelBonus[j].Serialize(car);
      };
    };
    strengthBonusDice.Serialize(car);
    m_startingEquipment.Serialize(car, globalData.version);
    hitDiceBaseclassID.Serialize(car);
  }
  else
  {
    //int key;
    int intVer = -1;
    Clear();
    car >> ver;
    if (ver=="Bc0") intVer = 0;
    else if (ver=="CL1") intVer = 1;
    else if (ver=="CL2") intVer = 2;
    else if (ver=="CL3") intVer = 3;  // ID replaced key
    else if (ver=="CL4") intVer = 4;  // Preserve preSpellNameKey
    else if (ver=="CL5") intVer = 5;  // hitDiceLevelBonus
    if (intVer >= 0)
    {
#ifdef UAFEDITOR
      if (intVer < 3)
      {
        //int key;
        //car >> key;  // Discard
        car >> preSpellNameKey;
      //car >> key;
      //m_classKey = (classType)key;
      };
#endif
      if (intVer >= 4)
      {
        car >> preSpellNameKey;
      };
      car >> m_name; if (m_name=="*") m_name="";
      if (intVer==0)
      {
        ABILITY_REQ ar;
        n=car.ReadCount();
//        m_abilityLimits.SetSize(n);
//        for (i=0; i<n; i++) m_abilityLimits[i].Serialize(ar);
          for (i=0; i<n; i++) ar.Serialize(car); // Discard
      };
      n=car.ReadCount();
      {
        BASECLASS_ID baseclassID;
        //m_baseclasses.SetSize(n);
        for (i=0; i<n; i++)
        {
          //result=m_baseclasses[i].Serialize(car);
          result = baseclassID.Serialize(car);
          if (result!=0) break;
          m_baseclasses.Add(baseclassID);
          if (baseclassData.LocateBaseclass(baseclassID) < 0)
          {
            if (baseclassID.GetLength() > 0)
            {
              if ((baseclassID[0]>='a') && (baseclassID <= 'z'))
              {
                CString ucbc;
                int indx;
                const BASE_CLASS_DATA *pOldBaseclass;
                ucbc = baseclassID;
                ucbc.SetAt(0, ucbc.GetAt(0)-0x20);
                BASECLASS_ID bcid;
                bcid = ucbc;
                if ((indx = baseclassData.LocateBaseclass(bcid)) >= 0)
                {
                  CString msg;
                  BASE_CLASS_DATA newBaseclass;
                  pOldBaseclass = baseclassData.PeekBaseclass(indx);
                  newBaseclass = *pOldBaseclass;
                  newBaseclass.m_name = baseclassID;
                  msg.Format("I am duplicating a baseclass named %s and naming it %s because class %s needs it.",
                              ucbc, baseclassID, ClassID());
                  MsgBoxInfo(msg);
                  baseclassData.AddBaseclass(newBaseclass);
                };
              };
            };
          };
#ifdef UAFEDITOR
          if (intVer < 3)
          {
            DWORD key;
            car >> key;
          };
#endif
        };
      };
      //if (ver == "CL2")
      if (intVer >= 2)
      {
        m_specialAbilities.Serialize(car, globalData.version,"","");
      };
      hitDiceLevelBonus.RemoveAll();
#ifdef UAFEDITOR
      if (intVer < 3)
      {
        int idx;
        //BASECLASS_ID baseclassID;
        HIT_DICE_LEVEL_BONUS hdlb;
        hdlb.baseclassID = Ability_Constitution;
        for (idx=0; idx<HIGHEST_CHARACTER_PRIME; idx++)
        {
          hdlb.bonusValues[idx] = determineHitDiceBonus(idx, m_name==Baseclass_fighter);
        };
        strengthBonusDice.Clear();
        if (    (LocateBaseclassID("fighter") >= 0)
             || (LocateBaseclassID("ranger") >= 0)
             || (LocateBaseclassID("paladin") >= 0)
           )
        {
          strengthBonusDice.DecodeText("1D100");
        };
      }
      else if (intVer < 5)
      {
        HIT_DICE_LEVEL_BONUS hdlb;
        //car >> hitDiceBonusAbility;
        car >> hdlb.ability;
        if (GetCount() > 0)
        {
          hdlb.baseclassID = *PeekBaseclassID(0);
        }
        else
        {
          hdlb.baseclassID = "";
        };
        car.Serialize((char *)hdlb.bonusValues, sizeof(hdlb.bonusValues));
        hitDiceLevelBonus.Add(hdlb);
        strengthBonusDice.Serialize(car);
      }
      else
#endif
      {
        HIT_DICE_LEVEL_BONUS hdlb;
        int ii, nn;
        car >> nn;
        for (ii=0; ii<nn; ii++)
        {
          hdlb.Serialize(car);
          hitDiceLevelBonus.Add(hdlb);
        };
        strengthBonusDice.Serialize(car);
      };
    }
    else
    {
      WriteDebugString("Unknown CLASS_DATA serialize version = %s\n",ver);
      result=1;
    };
#ifdef UAFEngine
    //We have decided not to allow 'Decorated' names for Classes.  prs

    //if (m_name.Find('|') >= 0) m_name = m_name.Left(m_name.Find('|')); // Remove qualifier.
#endif
#ifdef UAFEDITOR
    if (intVer < 3)
    {
      m_startingEquipment.Clear();  // Will be added when globalData is serialized.
    }
    else
#endif
    {
      m_startingEquipment.Serialize(car, globalData.version);
    };
    // Fix for empty starting equipment.  If equipment is specified, assume that there should be at least 1
    POSITION pos = m_startingEquipment.GetHeadPosition();
    while (pos != NULL) {
        ITEM i = m_startingEquipment.GetNext(pos);
        if (i.qty == 0)
            m_startingEquipment.SetQty(m_startingEquipment.GetListKeyByItemName(i.itemID), 1);
    }
#ifdef UAFEDITOR
    if (intVer < 3)
    {
      hitDiceBaseclassID = "*Average*";
    }
    else
#endif
    {
      hitDiceBaseclassID.Serialize(car);
    };
  };
  return result;
}

void CLASS_DATA_TYPE::Clear(void)
{
  m_ClassData.RemoveAll();
}
/*
CLASS_DATA *CLASS_DATA_TYPE::GetClassData(int key)
{
  //PRS 20120119 CSingleLock sLock(&m_CS,TRUE);
  POSITION pos;
  if ((pos = m_ClassData.FindKeyPos(key)) != NULL)
    return &(m_ClassData.GetAtPos(pos));
  else
    return NULL;
}
*/

int CLASS_DATA_TYPE::GetDataSize(void) const
{
  //POSITION pos;
  int size = sizeof (*this);
  //pos = m_ClassData.GetHeadPosition();
  //while (pos!=NULL) size += m_ClassData.GetNext(pos).GetDataSize();
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
    size += PeekClass(i)->GetDataSize();
  };
  return size;
}

CLASS_DATA_TYPE::CLASS_DATA_TYPE(void)
{
  Clear();
#ifdef UAFEDITOR
  LoadUADefaults();
#endif
}

#ifdef UAFEDITOR
void CLASS_DATA_TYPE::LoadUADefaults(void)
{ // You must do your own Clear() if you want it done.
  CLASS_DATA cd;
  BASECLASS_ID bci;
  cd.Clear();
  cd.m_name="Fighter";
  //cd.AddBaseclass(Fighter);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Cleric";
  //cd.AddBaseclass(Cleric);
  bci = "Cleric";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Ranger";
  //cd.AddBaseclass(Ranger);
  bci = "Ranger";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Paladin";
  //cd.AddBaseclass(Paladin);
  bci = "Paladin";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Magic User";
  //cd.AddBaseclass(MagicUser);
  bci = "Magic User";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Thief";
  //cd.AddBaseclass(Thief);
  bci = "Thief";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Druid";
  //cd.AddBaseclass(Druid);
  bci = "Druid";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Cleric/Fighter";
  //cd.AddBaseclass(Cleric);
  //cd.AddBaseclass(Fighter);
  bci = "Cleric";
  cd.AddBaseclass(bci);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Cleric/Fighter/Magic User";
  //cd.AddBaseclass(Cleric);
  //cd.AddBaseclass(Fighter);
  //cd.AddBaseclass(MagicUser);
  bci = "Cleric";
  cd.AddBaseclass(bci);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  bci = "Magic User";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Cleric/Ranger";
  //cd.AddBaseclass(Cleric);
  //cd.AddBaseclass(Ranger);
  bci = "Cleric";
  cd.AddBaseclass(bci);
  bci = "Ranger";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Cleric/Magic User";
  //cd.AddBaseclass(Cleric);
  //cd.AddBaseclass(MagicUser);
  bci = "Cleric";
  cd.AddBaseclass(bci);
  bci = "Magic User";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Cleric/Thief";
  //cd.AddBaseclass(Cleric);
  //cd.AddBaseclass(Thief);
  bci = "Cleric";
  cd.AddBaseclass(bci);
  bci = "Thief";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Fighter/Magic User";
  //cd.AddBaseclass(Fighter);
  //cd.AddBaseclass(MagicUser);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  bci = "Magic User";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Fighter/Thief";
  //cd.AddBaseclass(Fighter);
  //cd.AddBaseclass(Thief);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  bci = "Thief";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Fighter/Magic User/Thief";
  //cd.AddBaseclass(Fighter);
  //cd.AddBaseclass(MagicUser);
  //cd.AddBaseclass(Thief);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  bci = "Magic USer";
  cd.AddBaseclass(bci);
  bci = "Thief";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Magic User/Thief";
  //cd.AddBaseclass(MagicUser);
  //cd.AddBaseclass(Thief);
  bci = "Magic USer";
  cd.AddBaseclass(bci);
  bci = "Thief";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Fighter/Druid";
  //cd.AddBaseclass(Fighter);
  //cd.AddBaseclass(Druid);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  bci = "Druid";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Druid/Magic User";
  //cd.AddBaseclass(Druid);
  //cd.AddBaseclass(MagicUser);
  bci = "Druid";
  cd.AddBaseclass(bci);
  bci = "Magic User";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);

  cd.Clear();
  cd.m_name="Fighter/Magic User/Druid";
  //cd.AddBaseclass(Fighter);
  //cd.AddBaseclass(MagicUser);
  //cd.AddBaseclass(Druid);
  bci = "Fighter";
  cd.AddBaseclass(bci);
  bci = "Magic User";
  cd.AddBaseclass(bci);
  bci = "Druid";
  cd.AddBaseclass(bci);
  classData.AddClass(&cd);
}
#endif  //UAFEDITOR

CLASS_DATA_TYPE::~CLASS_DATA_TYPE(void)
{
}
/*
// No locking... for internal use
classType CLASS_DATA_TYPE::m_FindName (const CString& name) const
// Return key associated with the named object, or classUnknown
{
  POSITION pos;
  pos=m_ClassData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_ClassData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return (classType)m_ClassData.PeekAtPos(pos).m_classKey;
    m_ClassData.PeekNext(pos);
  };
  return classUnknown;
}
*/
/*
//CLASS_DATA *CLASS_DATA_TYPE::GetClassData(const CString& name)
CLASS_DATA *CLASS_DATA_TYPE::GetClass(const CLASS_ID& classID)
{
  //POSITION pos;
  //pos=m_ClassData.GetHeadPosition();
  //while (pos!=NULL)
    //if (m_ClassData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
    //if (m_ClassData[i].m_name.CompareNoCase(classID)==0)
    //      return &m_ClassData.GetAtPos(pos);
    //m_ClassData.GetNext(pos);
  int index;
  index = LocateClass(classID);
  if (index >= 0)
  {
    return &m_ClassData[index];
  };
  return NULL;
}
*/
/*
// With locking...for public use
classType CLASS_DATA_TYPE::FindName(const CString& name)
// Return key associated with the named object, or zero
{
  // PRS 20120119 CSingleLock sLock(&m_CS,TRUE);
  return m_FindName(name);
}
*/
/*
CString CLASS_DATA_TYPE::ClassName(classType type) const
{
  POSITION pos;
  pos = m_ClassData.FindKeyPos(type);
  if (pos!=NULL)
  {
    return m_ClassData.PeekAtPos(pos).m_name;
  }
  else
  {
    return emptyString;
  };
}
*/
/*

CLASS_DATA& CLASS_DATA_TYPE::operator [] (classType ctype)
{
  POSITION pos=m_ClassData.FindKeyPos(ctype);
  ASSERT(pos!=NULL);
  return m_ClassData.GetAtPos(pos);
}
*/
/*
CLASS_DATA& CLASS_DATA_TYPE::operator [] (int spellSchool)
{
  POSITION pos=m_ClassData.FindKeyPos(spellSchool);
  ASSERT(pos!=NULL);
  return m_ClassData.GetAtPos(pos);
}
*/

void CLASS_DATA_TYPE:: operator = (const CLASS_DATA_TYPE& src)
{
  if (&src==this) return;
  m_ClassData.RemoveAll();
  m_ClassData.Append(src.m_ClassData);
}




BOOL CLASS_DATA_TYPE::operator ==(const CLASS_DATA_TYPE& src) const
{
  int i, n;
  n = GetClassCount();
  if (n != src.GetClassCount())
  {
    return FALSE;
  };
  for (i=0; i<n; i++)
  {
    const CLASS_DATA  *pClass1, *pClass2;
    pClass1 = PeekClass(i);
    pClass2 = src.PeekClass(i);
    if (!(*pClass1 == *pClass2))
    {
      return FALSE;
    };
  };
  return TRUE;
}


//ABILITYLIMITS CLASS_DATA_TYPE::GetAbilityLimits(classType ctype, abilityType abtype)
//ABILITYLIMITS CLASS_DATA_TYPE::GetAbilityLimits(const CLASS_ID& classID, abilityType abtype) const
ABILITYLIMITS CLASS_DATA_TYPE::GetAbilityLimits(const CLASS_ID& classID,
                                                const ABILITY_ID& abilityID) const
{
  //POSITION pos;
  //pos=m_ClassData.FindKeyPos(ctype);
  //ASSERT(pos!=NULL);
  //if (pos==NULL) return 1;
  //return m_ClassData.GetAtPos(pos).GetAbilityLimits(abtype);
  const CLASS_DATA *pClass;
  pClass = PeekClass(classID);
  if (pClass == NULL) return 1;
  //return pClass->GetAbilityLimits(abtype);
  return pClass->GetAbilityLimits(abilityID);
}



//WORD CLASS_DATA_TYPE::GetAllowedAlignments(classType ctype) const
WORD CLASS_DATA_TYPE::GetAllowedAlignments(const CLASS_ID& classID) const
{
  //POSITION pos;
  //pos=m_ClassData.FindKeyPos(ctype);
  //ASSERT(pos!=NULL);
  //if (pos==NULL) return 0x1ff;
  //return m_ClassData.PeekAtPos(pos).GetAllowedAlignments();
  int index;
  index = LocateClass(classID);
  if (index < 0) return 0xff;
  return PeekClass(index)->GetAllowedAlignments();
}

/*
classType CLASS_DATA_TYPE::GetNextClass()
{
  classType key=classUnknown;
  if (GetCount() > 0)
  {
    key = (classType)m_ClassData.GetTail().m_classKey;
    if (key >= INT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is much greater than
      // MAX_CLASSES.
      // Gaps occur when deletions are made to the classes list
      POSITION pos = m_ClassData.GetHeadPosition();
      BOOL found = FALSE;
      DWORD prevKey = 0;
      DWORD currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = m_ClassData.GetNext(pos).m_classKey;
        if (currKey != prevKey+1)
        {
          key = (classType)(prevKey+1);
          found = TRUE;
        }
        else
          prevKey = currKey;
      }
      VERIFY(found); // should never happen
    }
    else
      key=(classType)(key+1);
  }
  else
    key = (classType)0;
  return key;
}
*/
/*
// called when loading binary class data that
// was saved with key info
classType CLASS_DATA_TYPE::AddClassWithCurrKey(const CLASS_DATA &data)
{
  // PRS 20120119 CSingleLock sLock(&m_CS, TRUE);
  if (m_ClassData.GetCount() < MAX_CLASSES)
  {
    m_ClassData.Insert(data, data.m_classKey);
    return (classType)data.m_classKey;
  }
  else
    return classUnknown;
}
*/

// Called when loading from scripts, or
// when adding from editor GUI
//classType CLASS_DATA_TYPE::AddClass(const CLASS_DATA &data)
void CLASS_DATA_TYPE::AddClass(const CLASS_DATA *data)
{
  // PRS 20120119 CSingleLock sLock(&m_CS, TRUE);
  //if (data.m_name.IsEmpty()) return classUnknown;
  //aaaif (data->m_name.IsEmpty()) return;
  // Do not allow multiple items with same full name.
  //if (m_FindName(data.m_name)!=classUnknown) return classUnknown;
  int index;
  index = LocateClass(data->ClassID());
  if (index >= 0) return;
  //if (m_ClassData.GetCount() < MAX_CLASSES)
  if (GetCount() < MAX_CLASSES)
  {
    //data.m_classKey = GetNextClass();
    //m_ClassData.Insert(data, data.m_classKey);
    //return (classType)data.m_classKey;
    m_ClassData.Add(*const_cast<CLASS_DATA*>(data));
    return;
  }
  else
    //return classUnknown;
    return;
}


//bool CLASS_DATA_TYPE::IsAllowedRace(classType Class, raceType race) const
bool CLASS_DATA_TYPE::IsAllowedRace(const CLASS_ID& classID, const RACE_ID& raceID) const
{
  //POSITION p;
  //p = GetHeadPosition();
  //while (p!=NULL)
  int index;
  index = LocateClass(classID);
  if (index < 0) return false;
  //{
  //  if (GetKeyAt(p) == Class)
  //  { //We now have found the class of interest.
  //    return PeekAtPos(p).IsRaceAllowed(race);
  //  };
  //  PeekNext(p);
  return PeekClass(index)->IsRaceAllowed(raceID);
  //};
  //return false; //Ain't no such class.  So There!
}



void CLASS_DATA_TYPE::PreSerialize(BOOL IsStoring)
{
  //POSITION pos;
  //pos=m_ClassData.GetHeadPosition();
  //while (pos!=NULL)
  int i, n;
  n = GetCount();
  for (i=0; i<n; i++)
  {
     //m_ClassData.GetNext(pos).PreSerialize(IsStoring);
     GetClass(i)->PreSerialize(IsStoring);
  };
}

void CLASS_DATA_TYPE::PostSerialize(BOOL IsStoring)
{
}

int CLASS_DATA_TYPE::Serialize(CAR& car)
{
  //POSITION p;
  int i, n;
  CString version("ClassV1");
  int result=0;

  if (car.IsStoring())
  {
//    car << "ClassV0";
    car << version; // V1 is compressed
    car.Compress(true);
    car << GetCount();
    //p = m_ClassData.GetHeadPosition();
    //while (p != NULL)
    n = GetCount();
    for (i=0; i<n; i++)
    {
      //m_ClassData.GetNext(p).Serialize(car);
      GetClass(i)->Serialize(car);
    };
  }
  else
  {
    CLASS_DATA data;
    int count;
    car >> version;
    if ((version =="ClassV0")||(version=="ClassV1"))
    {
      if (version > "ClassV0") car.Compress(true);
      car >> count;
      for (int ii=0; ii<count; ii++)
      {
        data.Clear();
        result=data.Serialize(car);
        if (result!=0) break;
        //AddClassWithCurrKey(data);
        AddClass(&data);
      }
    }
    else
    {
      WriteDebugString("Unknown Class data version = %s",version);
      result=1;
    };
  };
  return result;
}
#ifdef UAFEDITOR
void CLASS_DATA_TYPE::CrossReference(class CR_LIST *pCRList)
{
  //POSITION p;
  int i, n;
  n = GetCount();
  for (i = 0; i < n; i++)
  {
    GetClass(i)->CrossReference(pCRList);
  };
}
#endif

ALLOWED_CLASSES::ALLOWED_CLASSES(void)
{
  //m_allowedClasses = NULL;
  m_allowedClasses.RemoveAll();
  //m_numAllowedClasses = 0;
}

ALLOWED_CLASSES::~ALLOWED_CLASSES(void)
{
  //if (m_allowedClasses != NULL) My_free(m_allowedClasses);
  //m_allowedClasses = NULL;
  m_allowedClasses.RemoveAll();
}

void ALLOWED_CLASSES::Initialize(void)
{  // Add ALL classes to the allowed classes array.
  //POSITION p;
  int i, n;
  //Remove any old list we might have.
  //if (m_allowedClasses != NULL) My_free(m_allowedClasses);
  m_allowedClasses.RemoveAll();
  //How many classes are there?
  //m_numAllowedClasses = classData.GetCount();
  //m_allowedClasses =
  //  (classType*)My_malloc(m_numAllowedClasses*sizeof(*m_allowedClasses));
  //if (m_allowedClasses == NULL)
  //{
  //  m_numAllowedClasses = 0;
  //  return;
  //};
  //p = classData.GetHeadPosition();
  //i=0;
  n = classData.GetCount();
  //while (p != NULL)
  for (i=0; i<n; i++)
  {
    CLASS_ID classID;
    const CLASS_DATA *pClass;
    pClass = classData.PeekClass(i);
    classID = pClass->ClassID();
    //if (i >= m_numAllowedClasses) break; //Safety first.  Should not need this,
    //m_allowedClasses[i++] = classData.GetNext(p).WhatIsYourClassType();
    {
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetClassContext(pClass);
      if (!pClass->RunClassScripts(
                                  SELECT_CHARACTER_CLASS,
                                  ScriptCallback_RunAllScripts,
                                  NULL,
                                  "Character creation - is class allowed").IsEmpty()) continue;
    };
    m_allowedClasses.Add(classID);
  };
}

//void ALLOWED_CLASSES::RestrictByRace(raceType race)
void ALLOWED_CLASSES::Restrictions(const RACE_ID& raceID, const CString& gender)
{
  int i;
  //for (i=NumAllowedClasses()-1; i>=0; i--)
  for (i=GetCount()-1; i>=0; i--)
  {
    //if (!classData.IsAllowedRace(m_allowedClasses[i], race))
    if (!classData.IsAllowedRace(*PeekClassID(i), raceID))
    {
      RemoveAt(i);
      continue;
    };
    {
      ActorType actor;
      const CLASS_DATA *pClass;
      pClass = classData.PeekClass(*PeekClassID(i));
      if (pClass != NULL)
      {
        int j, n;
        CString result;
        n = pClass->GetCount();
        for (j=0; j<n; j++)
        {
          const BASE_CLASS_DATA *pBaseclass;
          pBaseclass = baseclassData.PeekBaseclass(*pClass->PeekBaseclassID(j));
          if (pBaseclass != NULL)
          {
            HOOK_PARAMETERS hookParameters;
            SCRIPT_CONTEXT scriptContext;

            scriptContext.SetClassContext(pClass);
            hookParameters[5] = gender;
            hookParameters[6] = raceID;
            result = pBaseclass->RunBaseClassScripts(
                                 IS_BASECLASS_ALLOWED,
                                 ScriptCallback_RunAllScripts,
                                 "",
                                 "Character creation - is baseclass allowed");
            if (result.IsEmpty()) continue;
            if (result[0] == 'N')
            {
              RemoveAt(i);
              break;
            }
          };
        };
      };
    };
  }; //That was easy.
}



void ALLOWED_CLASSES::RemoveAt(int i)
{
  //ASSERT(i < m_numAllowedClasses);
  //m_numAllowedClasses--;
  //for (; i<m_numAllowedClasses; i++)
  //      m_allowedClasses[i] = m_allowedClasses[i+1];
  m_allowedClasses.RemoveAt(i);
}
/*
classType ALLOWED_CLASSES::GetAt(int i) const
{
  if (i < m_numAllowedClasses) return m_allowedClasses[i];
  return classUnknown;
}
*/
/*
int ALLOWED_CLASSES::NumAllowedClasses(void) const
{
  return m_numAllowedClasses;
}
*/

#ifdef USE_TRAITS
TRAIT_DATA::TRAIT_DATA (void)
{
  Clear();
}

TRAIT_DATA::TRAIT_DATA(TRAIT_DATA& src)
{
  *this=src;
}

TRAIT_DATA::~TRAIT_DATA (void)
{
}

TRAIT_DATA& TRAIT_DATA::operator = (const TRAIT_DATA& src)
{
  if (&src==this) return *this;
  Clear();
  m_traitKey=src.m_traitKey;
  m_name=src.m_name;
  m_abbreviation=src.m_abbreviation;
  m_roll=src.m_roll;
  m_specAbs.Copy(src.m_specAbs);
  return *this;
}

void TRAIT_DATA::PreSerialize(BOOL IsStoring)
{
}

int TRAIT_DATA::Serialize(CAR& car)
{
  int i,n;
  int result=0;
  CString tempStr("Tr1");

  DICEPLUSADJLIST m_adjustments;

  if (car.IsStoring())
  {
    car << tempStr;  // Version
    car << m_traitKey;
    AS(car, m_name);
    AS(car, m_abbreviation);
    m_roll.Serialize(car);
  }
  else
  {
    Clear();
    car >> tempStr;
    if (tempStr=="Tr0")
    {
      car >> m_traitKey;
      DAS(car, m_name);
      DAS(car, m_abbreviation);

      n=car.ReadCount();
      m_adjustments.SetSize(n);
      for (i=0; i<n; i++)
      {
        result=m_adjustments[i].Serialize(car);
        if (result!=0) return result;
      };

      CString diceresult("");
      int num=m_adjustments.GetSize();
      if (num > 0)
      {
        diceresult += m_adjustments[0].EncodeOperator(m_adjustments[0].m_operator[0]);

        // for things like 3*level
        for (i=1; i<num; i++)
        {
          ADJUSTMENT adj = m_adjustments[i];
          diceresult += adj.EncodeText();
        }
      }
      m_roll.m_Text = diceresult;
    }
    else if (tempStr=="Tr1")
    {
      car >> m_traitKey;
      DAS(car, m_name);
      DAS(car, m_abbreviation);
      m_roll.Serialize(car);
    }
    else
    {
      WriteDebugString("Unknown TRAIT_DATA serialize version = %s\n",tempStr);
      result=1;
    };
  };
  if (globalData.version >= _SPECIAL_ABILITIES_VERSION_)
  {
    m_specAbs.Serialize(car, globalData.version, m_name, "traits");
  };
  return result;
}

void TRAIT_DATA::Clear(void)
{
  m_name="";
  m_abbreviation="";
  m_roll.Clear();
  m_specAbs.Clear();
}

void TRAIT_DATA::AddAdjust(CString &name)
{
  m_roll.AddAdjust(name);
}

BOOL TRAIT_DATA::EvaluateTrait(double &result)
{
  return (m_roll.Roll(result));
}


TRAIT_DATA_TYPE::TRAIT_DATA_TYPE (void)
{
  Clear();
}

TRAIT_DATA_TYPE::~TRAIT_DATA_TYPE(void)
{
}

void TRAIT_DATA_TYPE::Clear(void)
{
  m_TraitData.RemoveAll();
}

// With locking...for public use
traitType TRAIT_DATA_TYPE::FindName(const CString& name)
// Return key associated with the named object, or zero
{
//  CSingleLock sLock(&m_CS,TRUE);
  return m_FindName(name);
}

void TRAIT_DATA_TYPE::LoadUADefaults(void)
{
}

// called when loading binary trait data that
// was saved with key info
traitType TRAIT_DATA_TYPE::AddTraitWithCurrKey(const TRAIT_DATA &data)
{
//  CSingleLock sLock(&m_CS, TRUE);
  if (m_TraitData.GetCount() < MAX_TRAITS)
  {
    // over-ride if key already exists.
    //
    // A bug in versions < 0.900 didn't copy m_traitKey
    // in operator= so all inserts ended up using the
    // same m_traitKey in the binary file.
    if (m_TraitData.HaveKey(data.m_traitKey))
    {
      data.m_traitKey = GetNextTrait();
    }

    m_TraitData.Insert(data, data.m_traitKey);
    return (traitType)data.m_traitKey;
  }
  else
    return traitUnknown;
}


traitType TRAIT_DATA_TYPE::AddTrait(const TRAIT_DATA& newtr)
// Called when loading from scripts, or
// when adding from editor GUI
{
//  CSingleLock sLock(&m_CS, TRUE);
  if (newtr.m_name.IsEmpty()) return traitUnknown;
  // Do not allow multiple items with same full name.
  if (m_FindName(newtr.m_name)!=traitUnknown) return traitUnknown;
  if (m_TraitData.GetCount() < MAX_TRAITS)
  {
    newtr.m_traitKey = GetNextTrait();
    m_TraitData.Insert(newtr, newtr.m_traitKey);
    return (traitType)newtr.m_traitKey;
  }
  else
    return traitUnknown;
}

traitType TRAIT_DATA_TYPE::GetNextTrait()
{
  traitType key=traitUnknown;
  if (GetCount() > 0)
  {
    key = (traitType)m_TraitData.GetTail().m_traitKey;
    if (key >= UINT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is much greater than
      // MAX_TRAITS.
      // Gaps occur when deletions are made to the traits list
      POSITION pos = m_TraitData.GetHeadPosition();
      BOOL found = FALSE;
      DWORD prevKey = 0;
      DWORD currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = m_TraitData.GetNext(pos).m_traitKey;
        if (currKey != prevKey+1)
        {
          key = (traitType)(prevKey+1);
          found = TRUE;
        }
        else
          prevKey = currKey;
      }
      VERIFY(found); // should never happen
    }
    else
      key=(traitType)(key+1);
  }
  else
    key = (traitType)1;
  return key;
}

// No locking... for internal use
traitType TRAIT_DATA_TYPE::m_FindName (const CString& name) const
// Return key associated with the named object, or traitUnknown
{
  POSITION pos;
  pos=m_TraitData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_TraitData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return (traitType)m_TraitData.PeekAtPos(pos).m_traitKey;
    m_TraitData.PeekNext(pos);
  };
  return traitUnknown;
}

TRAIT_DATA *TRAIT_DATA_TYPE::GetTraitData(const CString& name)
{
  POSITION pos;
  pos=m_TraitData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_TraitData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return &m_TraitData.GetAtPos(pos);
    m_TraitData.PeekNext(pos);
  };
  return NULL;
}


void TRAIT_DATA_TYPE::PreSerialize(BOOL IsStoring)
{
  POSITION pos;
  pos=m_TraitData.GetHeadPosition();
  while (pos!=NULL)
     m_TraitData.GetNext(pos).PreSerialize(IsStoring);
}

void TRAIT_DATA_TYPE::PostSerialize(BOOL IsStoring)
{
}

int TRAIT_DATA_TYPE::Serialize(CAR& car)
{
  POSITION p;
  CString version("TraitV1");
  int result=0;

  if (car.IsStoring())
  {
    car << version; // V1 is compressed
//    car << "TraitV0";
    car.Compress(true);
    car << GetCount();
    p = m_TraitData.GetHeadPosition();
    while (p != NULL)
      m_TraitData.GetNext(p).Serialize(car);
  }
  else
  {
    TRAIT_DATA data;
    int count;
    car >> version;
    if ((version =="TraitV0")||(version=="TraitV1"))
    {
      if (version > "TraitV0") car.Compress(true);
      car >> count;
      for (int i=0; i<count; i++)
      {
        result=data.Serialize(car);
        if (result!=0) break;
        AddTraitWithCurrKey(data);
      }
    }
    else
    {
      WriteDebugString("Unknown Trait data version = %s",version);
      result=1;
    };
  };
  return result;
}

BOOL TRAIT_DATA_TYPE::EvaluteTrait(traitType trait, double &result)
{
  POSITION pos;
  if ((pos = m_TraitData.FindKeyPos(trait)) != NULL)
  {
    return (m_TraitData.GetAtPos(pos).EvaluateTrait(result));
  }
  else
    return FALSE;
}
#endif


SPELLGROUP_DATA::SPELLGROUP_DATA (void):m_specAbs(true) 
{
  Clear();
}

SPELLGROUP_DATA::~SPELLGROUP_DATA (void)
{
}

int SPELLGROUP_DATA::LocateSpellgroupReference(const SPELLGROUP_ID& spellgroupID) const
{
  int i, n;
  n = m_spellgroups.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekSpellgroupReference(i)->m_spellgroupID == spellgroupID) return i;
  };
  return -1;
}


SPELLGROUP_DATA& SPELLGROUP_DATA::operator = (const SPELLGROUP_DATA& src)
{
  int i, n;
  if (&src==this) return *this;
  Clear();
  m_name=src.m_name;
  //m_spellgroupKey=src.m_spellgroupKey;
  n=src.m_spells.GetSize();
  m_spells.SetSize(n);
  for (i=0; i<n; i++) 
  {
    *GetSpellReference(i)=*src.PeekSpellReference(i);
    //m_spells[i]=src.m_spells[i];
  };
  n=src.m_spellgroups.GetSize();
  m_spellgroups.SetSize(n);
  for (i=0; i<n; i++)
  {
    *GetSpellgroupReference(i)=*src.PeekSpellgroupReference(i);
    //m_spellgroups[i]=src.m_spellgroups[i];
  }
  m_specAbs.Copy(src.m_specAbs);
  return *this;
}

void SPELLGROUP_DATA::PreSerialize(BOOL IsStoring)
{
  int i, n;
  n=m_spells.GetSize();
  for (i=0; i<n; i++) 
  {
    GetSpellReference(i)->PreSerialize(IsStoring);
  };
  n=m_spellgroups.GetSize();
  for (i=0; i<n; i++)
  {
    GetSpellgroupReference(i)->PreSerialize(IsStoring);
  }
}

int SPELLGROUP_DATA::Serialize(CAR& car, CString& version)
{
  int i,n;
  int result=0;
  CString tempStr("SG0");
  if (car.IsStoring())
  {
    car << tempStr;  // Version
    //car << m_spellgroupKey;
    if (m_name=="") m_name="*";
    car << m_name;
    if (m_name=="*") m_name="";
    n=m_spells.GetSize();
    car.WriteCount(n);
    for (i=0; i<n; i++)
    {
      GetSpellReference(i)->Serialize(car, globalData.version);
      //m_spells[i].Serialize(car, globalData.version);
    };
    n=m_spellgroups.GetSize();
    car.WriteCount(n);
    for (i=0; i<n; i++)
    {
      GetSpellgroupReference(i)->Serialize(car, version);
      //m_spellgroups[i].Serialize(car, version);
    };
  }
  else
  {
    Clear();
    car >> tempStr;
    if (tempStr=="SG0")
    {
      //car >> m_spellgroupKey;
#ifdef UAFEDITOR
      if (version < "SpGrpV3")
      {
        DWORD key;
        car >> key;
      };
#endif
      car >> m_name; if (m_name=="*") m_name="";
      n=car.ReadCount();
      m_spells.SetSize(n);
      for (i=0; i<n; i++)
      {
        if (version < "SpGrpV2")
        {
          result=GetSpellReference(i)->Serialize(car, VersionSpellIDs);
          //result=m_spells[i].Serialize(car, VersionSpellIDs);
        }
        else
        {
          result=GetSpellReference(i)->Serialize(car, VersionSpellNames);
          //result=m_spells[i].Serialize(car, VersionSpellNames);
        };
        if (result!=0) return result;
      };
      n=car.ReadCount();
      m_spellgroups.SetSize(n);
      for (i=0; i<n; i++)
      {
        GetSpellgroupReference(i)->Serialize(car, version);
        //m_spellgroups[i].Serialize(car, version);
        if (result!=0) return result;
      };
    }
    else
    {
      WriteDebugString("Unknown SPELLGROUP_DATA serialize version = %s\n",tempStr);
      result=1;
    };
  };
  if (globalData.version >= _SPECIAL_ABILITIES_VERSION_)
  {
    m_specAbs.Serialize(car, globalData.version, m_name, "spellgroup");
  };
  return result;
}

void SPELLGROUP_DATA::Clear(void)
{
  m_name="";
  m_spells.RemoveAll();
  m_spellgroups.RemoveAll();
  m_specAbs.Clear();
}

int SPELLGROUP_DATA::GetDataSize(void) const
{
  int i, n;
  int size=sizeof *(this);
  n=m_spells.GetSize();
  size += m_specAbs.GetDataSize();
  for (i=0; i<n; i++) 
  {
    size+=PeekSpellReference(i)->GetDataSize();
  };
  n=m_spellgroups.GetSize();
  for (i=0; i<n; i++) 
  {
    size+=PeekSpellgroupReference(i)->GetDataSize();
    //size+=m_spellgroups[i].GetDataSize();
  };
  return size;
}


void SPELLGROUP_DATA::AddSpell(const SPELL_ID& spellID)
{
  int i,n;
  n=m_spells.GetSize();
  for (i=0; i<n; i++) 
  {
    if (PeekSpellReference(i)->m_spellID==spellID) return;
    //if (m_spells[i].m_spellID==spellID) return;
  };
  SPELL_REFERENCE temp;
  temp.m_spellID=spellID;
  m_spells.SetAtGrow(n,temp);
}

//void SPELLGROUP_DATA::AddSpellgroup(const CString& name)
void SPELLGROUP_DATA::AddSpellgroup(const SPELLGROUP_REFERENCE& spellgroupReference)
{
  int i,n;
  n=m_spellgroups.GetSize();
  for (i=0; i<n; i++)
  {
    if (LocateSpellgroupReference(spellgroupReference.m_spellgroupID) >= 0) return;
  };
  //temp.m_spellgroupName=name;
  //m_spellgroups.SetAtGrow(n,temp);
  //m_spellgroups.Add(temp);
  m_spellgroups.Add(spellgroupReference);
}



SPELLGROUP_DATA_TYPE::SPELLGROUP_DATA_TYPE (void)
{
  Clear();
}

SPELLGROUP_DATA_TYPE::~SPELLGROUP_DATA_TYPE(void)
{
}

void SPELLGROUP_DATA_TYPE::Clear(void)
{
  //m_SpellgroupData.RemoveAll();
  m_spellgroups.RemoveAll();
}

void SPELLGROUP_DATA_TYPE::LoadUADefaults(void)
{
}


int SPELLGROUP_DATA_TYPE::GetDataSize(void) const
{
  int size=sizeof (*this);
  //POSITION pos;
  //for (pos=m_SpellgroupData.GetHeadPosition();
  //     pos!=NULL;
  //     size+=m_SpellgroupData.GetNext(pos).GetDataSize())
  int i, n;
  n = GetSpellgroupCount();
  for (i=0; i<n; i++)
  {
    size += PeekSpellgroup(i)->GetDataSize();
  };
  return size;
}

int SPELLGROUP_DATA_TYPE::LocateSpellgroup(const SPELLGROUP_ID& spellgroupID) const
{
  int i, n;
  n = m_spellgroups.GetSize();
  for (i=0; i<n; i++)
  {
    if (spellgroupID == PeekSpellgroup(i)->m_name) return i;
  };
  return -1;
}

/*
// With locking...for public use
spellgroupType SPELLGROUP_DATA_TYPE::FindName(const CString& name)
// Return key associated with the named object, or zero
{
  CSingleLock sLock(&m_CS,TRUE);
  return m_FindName(name);
}

*/
/*
// called when loading binary spellgroup data that
// was saved with key info
spellgroupType SPELLGROUP_DATA_TYPE::AddSpellgroupWithCurrKey(const SPELLGROUP_DATA &data)
{
  CSingleLock sLock(&m_CS, TRUE);
  if (m_SpellgroupData.GetCount() < MAX_SPELLGROUPS)
  {
    m_SpellgroupData.Insert(data, data.m_spellgroupKey);
    return (spellgroupType)data.m_spellgroupKey;
  }
  else
    return spellgroupUnknown;
}
*/

//spellgroupType SPELLGROUP_DATA_TYPE::AddSpellgroup(const SPELLGROUP_DATA& newsg)
void SPELLGROUP_DATA_TYPE::AddSpellgroup(const SPELLGROUP_DATA& newsg)
// Called when loading from scripts, or
// when adding from editor GUI
{
  //CSingleLock sLock(&m_CS, TRUE);
  //if (newsg.m_name.IsEmpty()) return spellgroupUnknown;
  if (newsg.m_name.IsEmpty()) return;
  // Do not allow multiple items with same full name.
  //if (m_FindName(newsg.m_name)!=spellgroupUnknown) return spellgroupUnknown;
  if (LocateSpellgroup(newsg.SpellgroupID()) >= 0) return;
  //if (m_SpellgroupData.GetCount() < MAX_SPELLGROUPS)
  if (GetSpellgroupCount() < MAX_SPELLGROUPS)
  {
    //newsg.m_spellgroupKey = GetNextSpellgroup();
    //m_SpellgroupData.Insert(newsg, newsg.m_spellgroupKey);
    m_spellgroups.Add(const_cast<SPELLGROUP_DATA&>(newsg));
    //return (spellgroupType)newsg.m_spellgroupKey;
    return;
  }
  else
    //return spellgroupUnknown;
    return;
}


/*
spellgroupType SPELLGROUP_DATA_TYPE::GetNextSpellgroup()
{
  spellgroupType key=spellgroupUnknown;
  if (GetCount() > 0)
  {
    key = (spellgroupType)m_SpellgroupData.GetTail().m_spellgroupKey;
    if (key >= UINT_MAX)
    {
      // Need to wrap key.
      // Search for non-sequential gap between current keys.
      // A gap is guaranteed since the key range is much greater than
      // MAX_SPELLGROUPS.
      // Gaps occur when deletions are made to the traits list
      POSITION pos = m_SpellgroupData.GetHeadPosition();
      BOOL found = FALSE;
      DWORD prevKey = 0;
      DWORD currKey;
      while ((pos != NULL) && (!found))
      {
        currKey = m_SpellgroupData.GetNext(pos).m_spellgroupKey;
        if (currKey != prevKey+1)
        {
          key = (spellgroupType)(prevKey+1);
          found = TRUE;
        }
        else
          prevKey = currKey;
      }
      VERIFY(found); // should never happen
    }
    else
      key=(spellgroupType)(key+1);
  }
  else
    key = (spellgroupType)1;
  return key;
}
*/

/*
// No locking... for internal use
spellgroupType SPELLGROUP_DATA_TYPE::m_FindName (const CString& name) const
// Return key associated with the named object, or spellgroupUnknown
{
  POSITION pos;
  pos=m_SpellgroupData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_SpellgroupData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return (spellgroupType)m_SpellgroupData.PeekAtPos(pos).m_spellgroupKey;
    m_SpellgroupData.PeekNext(pos);
  };
  return spellgroupUnknown;
}
*/
/*
SPELLGROUP_DATA *SPELLGROUP_DATA_TYPE::GetSpellgroupData(const CString& name)
{
  POSITION pos;
  pos=m_SpellgroupData.GetHeadPosition();
  while (pos!=NULL)
  {
    if (m_SpellgroupData.PeekAtPos(pos).m_name.CompareNoCase(name)==0)
          return &m_SpellgroupData.GetAtPos(pos);
    m_SpellgroupData.GetNext(pos);
  };
  return NULL;
}
*/

void SPELLGROUP_DATA_TYPE::PreSerialize(BOOL IsStoring)
{
  //POSITION pos;
  //pos=m_SpellgroupData.GetHeadPosition();
  //while (pos!=NULL)
  int i, n;
  n = GetSpellgroupCount();
  for (i=0; i<n; i++)
  {
     //m_SpellgroupData.GetNext(pos).PreSerialize(IsStoring);
     GetSpellgroup(i)->PreSerialize(IsStoring);
     //m_spellgroups[i].PreSerialize(IsStoring);
  };
}

void SPELLGROUP_DATA_TYPE::PostSerialize(BOOL IsStoring)
{
}

int SPELLGROUP_DATA_TYPE::Serialize(CAR& car)
{
  //POSITION p;
  //CString version("SpGrpV1");
  //CString version("SpGrpV2");   // Version 2 has named references to spells
  CString version("SpGrpV3");     // Version 3 uses SPELLGROUP_ID
  int result=0;

  if (car.IsStoring())
  {
//    car << "SpGrpV0";
    car << version;  // V1 includes compression;
                     // V2 has named referenced to spells.
    car.Compress(true);
    car << GetCount();
    //p = m_SpellgroupData.GetHeadPosition();
    //while (p != NULL)
    int i, n;
    n = GetSpellgroupCount();
    for (i=0; i<n; i++)
    {
      //m_SpellgroupData.GetNext(p).Serialize(car, version);
      GetSpellgroup(i)->Serialize(car, version);
    };
  }
  else
  {
    SPELLGROUP_DATA data;
    int count;
    car >> version;
    if (version >= "SpGrpV0")
    {
      if (version > "SpGrpV0") car.Compress(true);
      car >> count;
      for (int i=0; i<count; i++)
      {
        result=data.Serialize(car, version);
        if (result!=0) break;
        //AddSpellgroupWithCurrKey(data);
        AddSpellgroup(data);
      }
    }
    else
    {
      WriteDebugString("Unknown Spell group data version = %s",version);
      result=1;
    };
  };
  return result;
}

#ifdef UAFEDITOR
void SPELLGROUP_DATA_TYPE::CrossReference(class CR_LIST *pCRList)
{
  int i, n;
  n = GetSpellgroupCount();
  for (i = 0; i < n; i++)
  {
    GetSpellgroup(i)->CrossReference(pCRList);
  };
}
#endif

#ifdef UAFEDITOR

// Divide a multi-line string and write it as an array of strings.
void JWriter::NameAndMultiLineString(const char *name, const CString& value)
{
  LPCSTR str, startStr;
  str = (LPCSTR)value;
  StartArray(name);
  while (*str != 0)
  {
    int n;
    startStr = str;
    for (;;)
    {
      if (*str == 0)
      {
        n = str - startStr;
        break;
      };
      if ((*str == '\r') && (*(str+1) == '\n'))
      {
        n = str - startStr;
        str += 2;
        break;
      };
      str++;
    };
    if (n > 0)
    {
      CString line(startStr, n);
      NextEntry();
      QuotedString(line);
    };
  };
  EndArray();
}


void JWriter::String(const char *str)
{
  if (strchr(str, '"') != NULL)
  {
    CString s(str);
    s.Replace("\"", "\\\"");
    // If a string supports linefeeds then you should use JWriter/JReader::NameAndMultilineString
    //s.Replace("\r\n", "\\n");
    //
    m_f->Write(s, s.GetLength());
  }
  else
  {
    m_f->Write(str, strlen(str));
  };
}

void JWriter::QuotedString(const char *str)
{
  m_f->Write("\"",1);
  String(str);
  m_f->Write("\"",1);
}

void JWriter::Tab(void)
{
  if (linefeed)
  {
    m_f->Write(tabs, JWTAB*indent);
  };
}

void JWriter::MaybeComma(void)
{
  if (lineCount[indent] > 0)
  {
    m_f->Write(",", 1);
  };
  lineCount[indent]++;
  if (linefeed)
  {
    m_f->Write("\n",1);
  };
}

void JWriter::Name(const char *name)
{
  if (name != NULL)
  {
    if (name[0] == 0)
    {
      die("Bad item name");
    };
    MaybeComma();
    Tab();
    QuotedString(name);
    Colon();
  };
}

JR_DATA::~JR_DATA(void)
{
  if (listValue != NULL)
  {
    delete listValue;
    listValue = NULL;
  };
  if (arrayValue != NULL) 
  {
    delete arrayValue;
    arrayValue = NULL;
  };
  if (next != NULL) 
  {
    die("Internal Destructor error");
  };
}


JR_LIST::~JR_LIST(void)
{
  JR_DATA *pData;
  while (first != NULL)
  {
    pData = first;
    first = pData->next;
    pData->next = NULL;
    delete pData;
  };
}

JReader::~JReader(void)
{
  Clear();
}



void JReader::Clear(void)
{
  if (m_root != NULL) delete(m_root);
  m_root = NULL;
}

bool JReader::NameAndDice(const char *name, int& n, int& s, int& b)
{
  CString v;
  if (!NameAndValue(name, v)) return false;
  const char *p = (LPCSTR)v;
  sscanf(p,"%dd%d+%d", &n, &s, &b);
  return true;
}


void JWriter::NameAndValue(const char *name, const CString& value)
{
  NameAndQuotedValue(name, value);
}

void JWriter::NameAndBinary(const char *name, const CString& value)
{
  int i, len, col;
  char result[81];
  len = value.GetLength();
  StartArray(name);
  i = 0;
  while (i < len)
  {
    int n;
    n = len-i;
    if (n >= 80) n = 40;
    else if (n > 40) n = n/2;
    NextEntry();
    col = 0;
    while (n > 0)
    {
      unsigned int c;
      c = value[i] & 0xff;
      sprintf(result+col, "%02x", c);
      i++;
      col +=2;
      n--;
    };
    QuotedString(result);
  }
  EndArray();
};

void JWriter::NameAndQuotedValue(const char *name, const CString& value)
{
  Name(name);
  QuotedString((LPCSTR)value);
}

void JWriter::NameAndQuotedNonBlank(const char *name, const CString& value)
{
  if (value.GetLength() == 0) return;
  Name(name);
  QuotedString((LPCSTR)value);
}

void JWriter::NameAndNonBlank(const char *name, const CString& value)
{
  if (value.GetLength() == 0) return;
  NameAndValue(name, value);
}

void JWriter::NameAndValue(const char *name, const int& value)
{
  char v[50];
  sprintf(v,"%d", value);
  NameAndValue(name,v);
}

void JWriter::NameAndValue(const char *name, const long& value)
{
  char v[50];
  sprintf(v,"%d", value);
  NameAndValue(name,v);
}

void JWriter::NameAndValue(const char *name, const unsigned char& value)
{
  char v[50];
  sprintf(v,"%d", value);
  NameAndValue(name,v);
}

void JWriter::NameAndValue(const char *name, const WORD& value)
{
  char v[50];
  sprintf(v,"%d", value);
  NameAndValue(name,v);
}

void JWriter::NameAndValue(const char *name, const DWORD& value)
{
  char v[50];
  sprintf(v,"%u", value);
  NameAndValue(name,v);
}

void JWriter::NameAndValue(const char *name, const LONGLONG& value)
{
  char v[50];
  sprintf(v,"%I64d", value);
  NameAndValue(name,v);
}


void JWriter::NameAndValue(const char *name, const double& value)
{
  char v[50];
  sprintf(v,"%f", value);
  NameAndValue(name, v);
}

void JWriter::NameAndValue(const char *name, const float& value)
{
  char v[50];
  sprintf(v,"%f", value);
  NameAndValue(name, v);
}

void JWriter::NameAndBool(const char *name, const bool& value)
{
  NameAndValue(name, value?"true":"false");
}

void JWriter::NameAndEnum(const char *name, const DWORD& value, DWORD numValue, const char *text[])
{
  if (value >= numValue)
  {
    errorStatus = ES_illegalValue;
    return;
  };
  NameAndValue(name, text[value]);
}



void JWriter::NameAndEnum(const char *name, const BYTE& value, DWORD numValue, const char *text[])
{
  if (value >= numValue)
  {
    errorStatus = ES_illegalValue;
    return;
  };
  NameAndValue(name, text[value]);
}

void JWriter::NameAndEnum(const char *name, const int& value, DWORD numValue, const char *text[])
{
  if ((unsigned)value >= numValue)
  {
    errorStatus = ES_illegalValue;
    return;
  };
  NameAndValue(name, text[value]);
}

void JWriter::NameAndEnum(const char *name, const chainTriggerType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_CHAINTRIG_TYPES, ChainTriggerText);
}

void JWriter::NameAndEnum(const char *name, const eventTriggerType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_EVENTTRIG_TYPES, EventTriggerText);
}

void JWriter::NameAndEnum(const char *name, const eventDistType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_DIST_TYPES, DistanceText);
}

void JWriter::NameAndEnum(const char *name, const eventDirType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_DIR_TYPES, DirectionText);
}

extern const char *MappingTypeText[];

void JWriter::NameAndEnum(const char *name, const MappingType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_MAPPING_TYPES, MappingTypeText);
}


extern const char *questAcceptText[];
void JWriter::NameAndEnum(const char *name, QuestAcceptType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_QUEST_ACCEPT_TYPE, questAcceptText);
}



extern const char *SurpriseText[];
void JWriter::NameAndEnum(const char *name, const eventSurpriseType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_SURPRISE_TYPES, SurpriseText);
}

void JWriter::NameAndEnum(const char *name, const eventTurnUndeadModType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_TURN_MOD_TYPES, TurnModText);
}

void JWriter::NameAndEnum(const char *name, const labelPostChainOptionsType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_LABEL_POST_CHAIN_OPTION_TYPES, ButtonPostChainOptionText);
}

void JWriter::NameAndEnum(const char *name, const passwordActionType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_PASSACTION_TYPES, PasswordActionText);
}

void JWriter::NameAndEnum(const char *name, const eventPartyAffectType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_AFFECT_TYPES, AffectsWhoText);
}

void JWriter::NameAndEnum(const char *name, const spellSaveEffectType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_SAVE_EFFECT_TYPES, SaveEffectText);
}

void JWriter::NameAndEnum(const char *name, const spellSaveVersusType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_SAVE_VERSUS_TYPES, SaveVersusText);
}

void JWriter::NameAndEnum(const char *name, const takeItemsAffectsType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_TAKEAFFECTS_TYPES, TakeItemsAffectsText);
}

void JWriter::NameAndEnum(const char *name, const takeItemQtyType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_TAKEITEMQTY_TYPES, TakeItemsQtyText);
}

void JWriter::NameAndEnum(const char *name, const taleOrderType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_TALEORDER_TYPES, TaleOrderTypeText);
}

void JWriter::NameAndEnum(const char *name, const costFactorType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_COST_FACTOR_TYPES, costFactorText);
}

void JWriter::NameAndEnum(const char *name, const MathOperationType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, MAX_MATH_OPERATION_TYPES, MathOperationText);
}

void JWriter::NameAndEnum(const char *name, const MultiItemCheckType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, MAX_MULTI_ITEM_CHECK_TYPES, MultiItemCheckText);
}

void JWriter::NameAndEnum(const char *name, const genderType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_GENDER_TYPES, genderText);
}


extern const char *SurfaceTypeTexts[];

void JWriter::NameAndEnum(const char *name, const SurfaceType& value)
{
  DWORD v;
  v = value;
  NameAndFlags(name, v, NUM_SURFACE_TYPES, SurfaceTypeTexts);
}


extern const char *QuestStateTypeText[];

void JWriter::NameAndEnum(const char *name, const QuestStateType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_QUEST_STATE_TYPE, QuestStateTypeText);
}

extern const char *CreatureSizeText[];
void JWriter::NameAndEnum(const char *name, const creatureSizeType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_SIZE_TYPES, CreatureSizeText);
}

void JWriter::NameAndEnum(const char *name, const alignmentType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_ALIGNMENT_TYPES, alignmentText);
}

void JWriter::NameAndEnum(const char *name, const charStatusType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_CHAR_STATUS_TYPES, CharStatusTypeText);
}

void JWriter::NameAndEnum(const char *name, const eventFacingType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_FACING_TYPES, FacingText);
}

void JWriter::NameAndEnum(const char *name, const eventStepType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_STEP_TYPES, StepText);
}

void JWriter::NameAndEnum(const char *name, const encounterButtonResultType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_ENCOUNTER_BUTTON_OPTION_TYPES, ButtonOptionText);
}

void JWriter::NameAndEnum(const char *name, const GlobalAreaViewStyle& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, MAX_GLOBAL_AVSTYLES, GlobalAVStyleText);
}

void JWriter::NameAndEnum(const char *name, const BlockageType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_BLOCKAGE_TYPES, BlockageText);
}

void JWriter::NameAndEnum(const char *name, const itemClassType& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_ITEMCLASS_TYPES, itemClassText);
}

void JWriter::NameAndEnum(const char *name, const VALUE_MODIFICATION& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_VALUE_MODIFICATION_TYPES, VALUE_MODIFICATIONText);
}

void JWriter::NameAndEnum(const char *name, const ACTION_CONDITION& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_ACTION_CONDITION_TYPES, ACTION_CONDITIONText);
}

void JWriter::NameAndEnum(const char *name, const ACTION& value)
{
  DWORD temp = value;
  NameAndEnum(name, temp, NUM_ACTION_TYPES, ACTIONText);
}

//void JWriter::NameAndEnum(const char *name, const takeItemQtyType& value)
//{
//  DWORD temp = value;
//  NameAndEnum(name, temp, NUM_TAKEITEMQTY_TYPES, TakeItemsQtyText);
//}




void JWriter::NameAndFlags(const char *name, const DWORD& value, int num, const char *text[])
{
  int i;
  DWORD v;
  v = value;
  StartArray(name);
  Linefeed(false);
  for (i=0; i<num; i++, v>>=1)
  {
    if (v & 1)
    {
      NextEntry();
      QuotedString(text[i]);
    };
  }
  EndArray();
  Linefeed(true);
};

void JWriter::NameAndFlags(const char *name, const BYTE& value, int num, const char *text[])
{
  DWORD v;
  v = value;
  NameAndFlags(name, v, num, text);
};

void JWriter::NameAndDice(const char *name, const int& n, const int& s, const int& b)
{
  char v[20];
  sprintf(v,"%dd%d+%d",n,s,b);
  NameAndValue(name, v);
}

void JWriter::NameAndValue(const char *name, const int *value, int num)
{
  int i;
  Name(name);
  String("[");
  for (i=0; i<num; i++)
  {
    char s[20];
    if (i != 0) String(",");
    sprintf(s,"%d",value[i]);
    String(s);
  };
  String("]");
}

void JWriter::NameAndValue(const char *name, const char *value, int num)
{
  int i;
  Name(name);
  String("[");
  for (i=0; i<num; i++)
  {
    char s[20];
    if (i != 0) String(",");
    sprintf(s,"%d",value[i]);
    String(s);
  };
  String("]");
}

void JWriter::OpenBrace(void)
{
  m_f->Write("{",1);
  indent++;
  lineCount.SetAtGrow(indent, 0);
}

void JWriter::OpenBracket(void)
{
  m_f->Write("[",1);
  indent++;
  lineCount.SetAtGrow(indent, 0);
}


void JWriter::Colon(void)
{
  String(":");
}

void JWriter::NewLine(void)
{
  m_f->Write("\n",1);
  Tab();
}

void JWriter::StartList(const char *name)
{
  if (name[0] != 0)
  {
    Name(name);
  };
  OpenBrace();
}

void JWriter::StartList(void)
{
  OpenBrace();
}

void JWriter::StartArray(const char *name)
{
  Name(name);
  OpenBracket();
}

void JWriter::StartArray()
{
  OpenBracket();
}

void JWriter::NextEntry(void)
{
  if (lineCount[indent] != 0)
  {
    m_f->Write(",",1);
  };
  lineCount[indent]++;
  if (linefeed)
  {
    m_f->Write("\n",1);
    Tab();
  };
}

void JWriter::EndList(void)
{
  indent--;
  if (linefeed && (lineCount[indent+1]>0))
  {
    m_f->Write("\n",1);
    Tab();
  };
  m_f->Write("}", 1);
}



void JWriter::EndArray(void)
{
  indent--;
  if (linefeed && (lineCount[indent+1]>0))
  {
    m_f->Write("\n",1);
    Tab();
  };
  m_f->Write("]", 1);
}


void JReader::SetError(ES e)
{
  errStat = e;
  errLine = lineNum;
  if (lineNum != 0) return;
  if (pCurList != NULL) errLine = pCurList->lineNum;
  if (pCurData != NULL) errLine = pCurData->lineNum;
}

ES JReader::GetError(void)
{
  return errStat;
}

CString JReader::GetErrorMessage(void)
{
  CString errorCode;
  CString result;
  switch (errStat)
  {
  case ES_ok: errorCode = "No Error"; break;
  case ES_EOF: errorCode = "Unexpected End of File"; break;
  case ES_syntax: errorCode = "Syntax Error"; break;
  case ES_missingColon: errorCode = "Missing Colon"; break;
  case ES_newlineInString: errorCode = "Newline in String"; break;
  case ES_missingList: errorCode = "Missing List"; break;
  case ES_missingArray: errorCode = "Missing an array"; break;
  case ES_illegalValue: errorCode = "Illegal Value"; break;
  case ES_missingData: errorCode = "Missing data"; break;
  default:
    errorCode = "Unknown Error Code";
  };
  result.Format("Error Code = %s\nError at line number %d\n%s", errorCode, errLine, errMessage);
  return result;
}

void JReader::JRError(const char *msg)
{
  CString message;
  errMessage = msg;
  message.Format("%s\nfile=%s\nline=%d", msg, m_f->GetFilePath(), errLine);
  MsgBoxError(message, "JSON Reader Error");
  throw (int)2;
}

BOOL JReader::Whitespace(void)
{
  return  (thisChar==' ')
        ||(thisChar=='\t')
        ||(thisChar=='\n')
        ||(thisChar=='\r');
}


void JReader::GetChar(void)
{
  if (nextCharValid)
  {
    thisChar = nextChar;
    nextCharValid = false;
  }
  else
  {
    if (m_f->Read(&thisChar,1) != 1)
    {
      SetError(ES_EOF);
    };
  };
  if (thisChar == '\n')
  {
    lineNum++;
  };
  return;
}

void JReader::EndList(void)
{
  listStackSize--;
  if (listStackSize < 0)
  {
    die("Import Internal Error");
  };
  if (pCurList != NULL) 
  {
    delete pCurList;
  };
  pCurList = listStack[listStackSize];
}

bool JReader::EndArray(void)
{
  listStackSize--;
  if (listStackSize < 0)
  {
    die("Import Internal Error");
  };
  if (pCurList != NULL)
  {
    delete pCurList;
  };
  pCurList = listStack[listStackSize];
  return true;
}

void JReader::QuotedString(char term)
{
  tokenSize = 0;
  for (;;)
  {
    GetChar();
    if (GetError() != ES_ok) return;
    if (thisChar == '\n')
    {
      SetError(ES_newlineInString);
      return;
    };
    if (thisChar == '\\')
    {
      GetChar();
      if (GetError() != ES_ok) return;
    }
    else
    {
      if (thisChar == term)
      {
        token = CString(tokenArray.GetData(), tokenSize);
        tokenType = TT_String;
        return;
      };
    };
    tokenArray.SetAtGrow(tokenSize, thisChar);
    tokenSize++;
  };
}

void JReader::GetToken(void)
{
  do
  {
    GetChar();
    if (GetError() != ES_ok) return;
  } while (Whitespace());
  if (thisChar == '"')
  {
    QuotedString('"');
    return;
  }
  if (thisChar == '[')
  {
    tokenType = TT_OpenBracket;
    return;
  }
  else if (thisChar == ']')
  {
    tokenType = TT_CloseBracket;
    return;
  }
  else if (thisChar == '{')
  {
    tokenType = TT_OpenBrace;
    return;
  }
  else if (thisChar == ':')
  {
    tokenType = TT_Colon;
    return;
  }
  else if (thisChar == '}')
  {
    tokenType = TT_CloseBrace;
    return;
  }
  else if (thisChar == ',')
  {
    tokenType = TT_Comma;
    return;
  }
  else
  {
    tokenSize = 0;
    do
    {
      if (thisChar == '\\')
      {
        GetChar();
        if (GetError() != ES_ok) return;
      };
      tokenArray.SetAtGrow(tokenSize++, thisChar);
      GetChar();
      if (GetError() != ES_ok) return;
      if (   (thisChar == ']')
          || (thisChar == '}')
          || (thisChar == ':')
          || (thisChar == ',')
         )
      {
        nextChar = thisChar;
        nextCharValid = TRUE;
        break;
      }
    } while (!Whitespace());
  };
  token = CString(tokenArray.GetData(), tokenSize);
  tokenType = TT_String;
  return;
}

void JReader::GetParseList(JR_LIST **pList)
{ // We gobble up the closeBrace.
  if (*pList != NULL) die("Internal Import Level error");
  // Loop while we get a name:value pair.  Stop at a closeBrace.
  *pList = new JR_LIST;
  (*pList)->lineNum = lineNum;
  for (;;)
  {
    GetToken();
    if (GetError() != ES_ok) return;
    if (tokenType == TT_CloseBrace)
    {
      return;
    };
    // Get the name.
    if (tokenType != TT_String)
    {
      SetError(ES_syntax);
      return;
    };
    // Name must be followed by a colon.
    {
      JR_DATA data; // Accumulate it locally utile it is complete.
      data.name = token;
      GetToken();
      if (GetError() != ES_ok) return;
      if (tokenType != TT_Colon)
      {
        SetError(ES_missingColon);
        JRError("File missing colon ':'");
        return; // GetError();
      };
      GetToken();
      if (GetError() != ES_ok) return;
      if (tokenType == TT_String)
      { // A simple node.
        data.stringValue = token;
      }
      else if (tokenType == TT_OpenBrace)
      {
        GetParseList(&data.listValue);
        if (GetError() != ES_ok) return;
      }
      else if (tokenType == TT_OpenBracket)
      {
        GetParseArray(&data.arrayValue);
        if (GetError() != ES_ok) return;
      };
      {
        JR_DATA *pData;
        pData = new JR_DATA;
        pData->lineNum = lineNum;
        pData->name = data.name;
        pData->stringValue = data.stringValue;
        pData->listValue = data.listValue;
        data.listValue = NULL;
        pData->arrayValue = data.arrayValue;
        data.arrayValue = NULL;
        if ((*pList)->first == NULL)
        {
          (*pList)->first = pData;
        }
        else
        {
          (*pList)->last->next = pData;
        };
        (*pList)->last = pData;
      }
      GetToken();
      if (GetError() != ES_ok) return;
      if (tokenType == TT_Comma) continue;
      else if (tokenType == TT_CloseBrace)
      {
        return;
      }
      else
      {
        SetError(ES_syntax);
        JRError("List not properly terminated");
        return;
      };
    };
  }
}

void JReader::GetParseArray(JR_LIST **pList)
{ // We gobble up the closeBracket.
  if (*pList != NULL) die("Internal Import Level error");
  // Loop while we get a new value.  Stop at a closeBracket.
  *pList = new JR_LIST;
  (*pList)->lineNum = lineNum;
  for (;;)
  {
    GetToken();
    if (GetError() != ES_ok) return;
    if (tokenType == TT_CloseBracket)
    {
      return;
    }
    {
      JR_DATA data; // Accumulate it locally until it is complete.
      // Name is empty.
      if (GetError() != ES_ok) return;
      if (tokenType == TT_String)
      { // A simple node.
        data.stringValue = token;
      }
      else if (tokenType == TT_OpenBrace)
      {
        GetParseList(&data.listValue);
        if (GetError() != ES_ok) return;
      }
      else if (tokenType == TT_OpenBracket)
      {
        GetParseArray(&data.arrayValue);
        if (GetError() != ES_ok) return;
      };
      {
        JR_DATA *pData;
        pData = new JR_DATA;
        pData->lineNum = lineNum;
        pData->name = data.name;
        pData->stringValue = data.stringValue;
        pData->listValue = data.listValue;
        data.listValue = NULL;
        pData->arrayValue = data.arrayValue;
        data.arrayValue = NULL;
        if ((*pList)->first == NULL)
        {
          (*pList)->first = pData;
        }
        else
        {
          (*pList)->last->next = pData;
        };
        (*pList)->last = pData;
      }
      GetToken();
      if (GetError() != ES_ok) 
      {
        return;
      };
      if (tokenType == TT_Comma) continue;
      else if (tokenType == TT_CloseBracket)
      {
        return;
      }
      else
      {
        SetError(ES_syntax);
        JRError("Array not properly terminated");
        return;
      };
    };
  }
}

int JReader::Initialize(void)
{
  // Parse the entire file into a tree.
  nextCharValid = FALSE;
  lineNum = 1;
  SetError(ES_ok);
  m_root = new JR_DATA;
  m_root->lineNum = lineNum;
  pCurData = m_root;
  pCurList = NULL;
  listStackSize = 0;
  GetToken();
  if (tokenType != TT_OpenBrace)
  {
    SetError(ES_syntax);
    JRError("File missing open '{'");
    return GetError();
  };
  GetParseList(&m_root->listValue);
  m_root = NULL;
  if (GetError()!=ES_ok) return GetError();
  lineNum = 0;
  return GetError();
};

bool JReader::StartList(const char *name)
{
  if ((name != NULL) && !FindAndRemoveName(name))
  {
    return false;
  }
  return StartList();
}

bool JReader::StartList()
{
  return OpenBrace();
}


bool JReader::FindAndRemoveOpenBracket(const char *name)
{
  if (FindAndRemoveName(name))
  {
    OpenBracket();
    return true;
  }
  else
  {
    return false;
  };
}

bool JReader::StartArray(const char *name)
{
  if (!FindAndRemoveName(name) || !OpenBracket())
  {
    return false;
  };
  return true;
}

bool JReader::StartArray()
{
  return OpenBracket();
}


bool JReader::NextEntry()
{
  USEOPTIONAL
  if (pCurList->first == NULL)
  {
    if (!ISOPTIONAL)
    {
      SetError(ES_missingData);
      JRError("Too few elements in array");
      throw 1;
    };
    return false;
  }
  if (pCurData != NULL)
  {
    die("Internal Error - pCurData != NULL");
  };
  pCurData = pCurList->first;
  pCurList->first = pCurData->next;
  pCurData->next = NULL;
  return true;
}

bool JReader::OpenBrace(void)
{
  listStack.SetAtGrow(listStackSize, pCurList);
  listStackSize++;
  pCurList = pCurData->listValue;
  pCurData->listValue = NULL;
  delete pCurData;
  pCurData = NULL;
  if (pCurList == NULL)
  {
    SetError(ES_missingList);
    JRError("Expected an open Brace");
    return false;
  };
  return true;
}

bool JReader::OpenBracket(void)
{
  listStack.SetAtGrow(listStackSize, pCurList);
  listStackSize++;
  if (pCurData->arrayValue == NULL)
  {
    SetError(ES_missingArray);
    JRError("Expected an open Bracket");
    return false;
  };
  pCurList = pCurData->arrayValue;
  pCurData->arrayValue = NULL;
  delete pCurData;
  pCurData = NULL;
  return true;
}

bool JReader::NameAndValue(const char *name, CString& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    value = pCurData->stringValue;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndBinary(const char *name, CString& value)
{
  /* Really */ NotImplemented(0x3abd, false);
  return false;
}

bool JReader::NameAndValue(const char *name, int& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    int temp;
    sscanf(pCurData->stringValue,"%d", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, WORD& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    int temp;
    sscanf(pCurData->stringValue,"%d", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, long& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    int temp;
    sscanf(pCurData->stringValue,"%d", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, DWORD& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    unsigned int temp;
    sscanf(pCurData->stringValue,"%d", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, char *value, int num)
{
  int i, *p;
  bool result;
  p = new int[num];
  result = NameAndValue(name, p, num);
  for (i=0; i<num; i++)
  {
    value[i] = p[i];
  };
  delete[] p;
  return result;
}


bool JReader::NameAndValue(const char *name, int *value, int num)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    int i;
    int sign;
    const char *c;
    const JR_LIST *pA;
    const JR_DATA *pD;
    pA = pCurData->arrayValue;
    if (pA != NULL)
    {
      pD = pA->first;
      for (i=0; i<num; i++)
      {
        sign = 1;
        if (pD == NULL)
        {
          SetError(ES_missingData);
          JRError("Too few emements in array");
          delete pCurData;
          pCurData = NULL;
          return false;
        };
        c = (LPCSTR)pD->stringValue;
        value[i] = 0;
        for (;;)
        {
          if (*c == 0) break;
          if (*c == ',')
          {
            c++;
            break;
          };
          if (*c == '-')
          {
            sign = -sign;
            c++;
            continue;
          };
          value[i] = 10*value[i] + *c - '0';
          c++;
        };
        value[i] = sign * value[i];
        pD = pD->next;
      };
    }
    else
    {
      SetError(ES_missingData);
      JRError("Missing Array");
      delete pCurData;
      pCurData = NULL;
      return false;
    };
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, double& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    double temp;
    sscanf(pCurData->stringValue,"%lf", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, LONGLONG& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    LONGLONG temp;
    sscanf(pCurData->stringValue,"%I64d", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, float& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    float temp;
    sscanf(pCurData->stringValue,"%f", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndBool(const char *name, BOOL& value)
{
  if (!FindAndRemoveName(name))
  {
    value = FALSE;
    return false;
  }
  else
  {
    value = pCurData->stringValue == "true";
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndBool(const char *name, bool& value)
{
  if (!FindAndRemoveName(name))
  {
    value = false;
    return false;
  }
  else
  {
    value = pCurData->stringValue == "true";
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndBool(const char *name, BYTE& value)
{
  if (!FindAndRemoveName(name))
  {
    value = false;
    return false;
  }
  else
  {
    value = pCurData->stringValue == "true";
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}


bool JReader::NameAndFlags(const char *name, DWORD& value, int num, const char *text[])
{
  int i;
  value = 0;
  StartArray(name);
  while (Optional(), NextEntry())
  {
    for (i=0; i<num; i++)
    {
      if (pCurData->stringValue == text[i])
      {
        value |= (1 << i);
        break;
      };
    };
    delete pCurData;
    pCurData = NULL;
  };
  EndArray();
  return true;
};

bool JReader::NameAndFlags(const char *name, BYTE& value, int num, const char *text[])
{
  DWORD v;
  bool result;
  result = NameAndFlags(name, v, num, text);
  value = v;
  return result;
};

bool JReader::NameAndMultiLineString(const char *name, CString& value)
{
  Optional(); 
  if (!StartArray(name)) return false;
  {
    CString line;
    while (Optional(), NextEntry())
    {
      if (!value.IsEmpty()) value += "\r\n";
      value += pCurData->stringValue;
      delete pCurData;
      pCurData = NULL;
    };
  };
  EndArray();
  return true;
}


void JReader::DeleteName(void)
{
  delete pCurData;
  pCurData = NULL;
}


bool JReader::FindAndRemoveName(const char *name)
{
  USEOPTIONAL
  JR_DATA *pData, *pPrevData;
  if (name == NULL)
  {
    if (pCurData == NULL)
    {
      die("Internal Error - pCurData == NULL");
    };
    return true;
  };
  pPrevData = NULL;
  // Check to see that old pCurData has been deleted.
  if (pCurData != NULL)
  {
    die("Internal Error - pCurData != NULL");
  };
  pData = pCurList->first;
  while (pData != NULL)
  {
    if (pData->name == name)
    {
      if (pPrevData == NULL)
      {
        pCurList->first = pData->next;
      }
      else
      {
        pPrevData->next = pData->next;
      };
      if (pCurList->last == pData)
      {
        pCurList->last = pPrevData;
      };
      pData->next = NULL;
      pCurData = pData;
      return true;
    };
    pPrevData = pData;
    pData = pData->next;
  };
  if (ISOPTIONAL) return false;
  SetError(ES_missingData);
  {
    CString msg;
    msg.Format("missing data entry named %s", name);
    JRError(msg);
  };
  return false;
}

bool JReader::FindAndRemoveOpenBrace(const char *name)
{
  if (FindAndRemoveName(name))
  {
    OpenBrace();
    return true;
  }
  else
  {
    return false;
  };
}

bool JReader::NameAndValue(const char *name, unsigned char& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    unsigned int temp;
    sscanf(pCurData->stringValue,"%d", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndValue(const char *name, char& value)
{
  if (!FindAndRemoveName(name))
  {
    return false;
  }
  else
  {
    unsigned int temp;
    sscanf(pCurData->stringValue,"%d", &temp);
    value = temp;
    delete pCurData;
    pCurData = NULL;
    return true;
  };
}

bool JReader::NameAndEnum(const char *name, DWORD& value, DWORD numValue, const char *text[])
{
  DWORD i;
  if (!FindAndRemoveName(name))
  {
    value = 0;
    return false;
  };
  for (i=0; i<numValue; i++)
  {
    if (pCurData->stringValue == text[i])
    {
      value = i;
      DeleteName();
      return true;
    };
  };
  value = 0;
  DeleteName();
  return false;
}

bool JReader::NameAndEnum(const char *name, eventDistType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_DIST_TYPES, DistanceText);
  value = (eventDistType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, genderType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_GENDER_TYPES, genderText);
  value = (genderType)temp;
  return status;
}

extern const char *SurfaceTypeTexts[];

bool JReader::NameAndEnum(const char *name, SurfaceType& value)
{
  DWORD v;
  NameAndFlags(name, v, NUM_SURFACE_TYPES, SurfaceTypeTexts);
  value = (SurfaceType)v;
  return true;
}

bool JReader::NameAndEnum(const char *name, QuestAcceptType& value)
{
  DWORD v;
  bool status;
  status = NameAndEnum(name, v, NUM_QUEST_ACCEPT_TYPE, questAcceptText);
  value = (QuestAcceptType)v;
  return status;
}


bool JReader::NameAndEnum(const char *name, QuestStateType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_QUEST_STATE_TYPE, QuestStateTypeText);
  value = (QuestStateType)temp;
  return status;
}

extern const char *CreatureSizeText[];
bool JReader::NameAndEnum(const char *name, creatureSizeType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_SIZE_TYPES, CreatureSizeText);
  value = (creatureSizeType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, alignmentType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_ALIGNMENT_TYPES, alignmentText);
  value = (alignmentType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, itemClassType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_ITEMCLASS_TYPES, itemClassText);
  value = (itemClassType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, chainTriggerType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_CHAINTRIG_TYPES, ChainTriggerText);
  value = (chainTriggerType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, eventTriggerType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_EVENTTRIG_TYPES, EventTriggerText);
  value = (eventTriggerType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, MappingType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_MAPPING_TYPES, MappingTypeText);
  value = (MappingType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, eventType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_EVENT_TYPES, EventListText);
  value = (eventType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, eventFacingType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_FACING_TYPES, FacingText);
  value = (eventFacingType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, charStatusType& value)
{
  DWORD temp = value;
  bool status;
  Optional();
  status = NameAndEnum(name, temp, NUM_CHAR_STATUS_TYPES, CharStatusTypeText);
  if (status)
  {
    value = (charStatusType)temp;
  };
  return status;
}

bool JReader::NameAndEnum(const char *name, VALUE_MODIFICATION& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_VALUE_MODIFICATION_TYPES, VALUE_MODIFICATIONText);
  value = (VALUE_MODIFICATION)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, ACTION_CONDITION& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_ACTION_CONDITION_TYPES, ACTION_CONDITIONText);
  value = (ACTION_CONDITION)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, ACTION& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_ACTION_TYPES, ACTIONText);
  value = (ACTION)temp;
  return status;
}



bool JReader::NameAndEnum(const char *name, eventStepType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_STEP_TYPES, StepText);
  value = (eventStepType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, encounterButtonResultType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_ENCOUNTER_BUTTON_OPTION_TYPES, ButtonOptionText);
  value = (encounterButtonResultType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, labelPostChainOptionsType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_LABEL_POST_CHAIN_OPTION_TYPES, ButtonPostChainOptionText);
  value = (labelPostChainOptionsType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, eventTurnUndeadModType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_TURN_MOD_TYPES, TurnModText);
  value = (eventTurnUndeadModType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, eventSurpriseType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_SURPRISE_TYPES, SurpriseText);
  value = (eventSurpriseType)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, eventDirType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_DIR_TYPES, DirectionText);
  value = (eventDirType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, passwordActionType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_PASSACTION_TYPES, PasswordActionText);
  value = (passwordActionType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, eventPartyAffectType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_AFFECT_TYPES, AffectsWhoText);
  value = (eventPartyAffectType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, spellSaveVersusType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_SAVE_VERSUS_TYPES, SaveVersusText);
  value = (spellSaveVersusType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, spellSaveEffectType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_SAVE_EFFECT_TYPES, SaveEffectText);
  value = (spellSaveEffectType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, takeItemQtyType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_TAKEITEMQTY_TYPES, TakeItemsQtyText);
  value = (takeItemQtyType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, taleOrderType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_TALEORDER_TYPES, TaleOrderTypeText);
  value = (taleOrderType)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, takeItemsAffectsType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_TAKEAFFECTS_TYPES, TakeItemsAffectsText);
  value = (takeItemsAffectsType)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, costFactorType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_COST_FACTOR_TYPES, costFactorText);
  value = (costFactorType)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, MultiItemCheckType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, MAX_MULTI_ITEM_CHECK_TYPES, MultiItemCheckText);
  value = (MultiItemCheckType)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, MathOperationType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, MAX_MATH_OPERATION_TYPES, MathOperationText);
  value = (MathOperationType)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, BlockageType& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, NUM_BLOCKAGE_TYPES, BlockageText);
  value = (BlockageType)temp;
  return status;
}
bool JReader::NameAndEnum(const char *name, GlobalAreaViewStyle& value)
{
  DWORD temp = value;
  bool status;
  status = NameAndEnum(name, temp, MAX_GLOBAL_AVSTYLES, GlobalAVStyleText);
  value = (GlobalAreaViewStyle)temp;
  return status;
}

bool JReader::NameAndEnum(const char *name, int& value, DWORD numValue, const char *text[])
{
  DWORD i;
  if (!FindAndRemoveName(name))
  {
    value = 0;
    return false;
  };
  for (i=0; i<numValue; i++)
  {
    if (pCurData->stringValue == text[i])
    {
      value = i;
      DeleteName();
      return true;
    };
  };
  value = 0;
  DeleteName();
  SetError(ES_missingData);
  JRError("Illegal enumeration value");
  throw 1;
  //return false;
}

bool JReader::NameAndEnum(const char *name, BYTE& value, DWORD numValue, const char *text[])
{
  DWORD i;
  if (!FindAndRemoveName(name))
  {
    value = 0;
    return false;
  };
  for (i=0; i<numValue; i++)
  {
    if (pCurData->stringValue == text[i])
    {
      value = i;
      DeleteName();
      return true;
    };
  };
  value = 0;
  DeleteName();
  return false;
}




#endif

//CAR::CAR(CFile *pFile, UINT nMode) :  CArchive(pFile, nMode)
CAR::CAR(CFile *pFile, UINT nMode) :  ar(pFile, nMode)
{
  m_compressType=0; // No compression yet.
  m_nextIndex=1;
  m_cData=NULL;
  m_dData=NULL;
  m_codes=NULL;
  m_w=0;
  m_bufferIndex=0;
  memset(m_buffer,0,sizeof(m_buffer));
  m_stkLen=0;
  m_OC=0;
  m_numCode=0;
  m_C=0;
}


CAR::~CAR(void)
{
  // Make sure we flush the output buffer
  Flush();
  if (m_cData!=NULL) delete m_cData;
  if (m_dData!=NULL) delete m_dData;
  if (m_codes!=NULL) delete m_codes;
}

void CAR::Flush(void)
{
  if (!IsStoring()) return;
  if (m_compressType==0) return;
  // And output the old string from w.
  *((unsigned int *)(((char *)m_buffer)+(m_bufferIndex>>3)))|=m_w<<(m_bufferIndex&7);
  m_bufferIndex=(m_bufferIndex+13)%416;
  if (m_bufferIndex==0)
  {
    Write(m_buffer,sizeof(m_buffer));
    memset(m_buffer,0,sizeof(m_buffer));
  };
// We need at least one EOF character.  if (m_bufferIndex==0) return;
  m_w=8191;
  do
  {
    *((unsigned int *)(((char *)m_buffer)+(m_bufferIndex>>3)))|=m_w<<(m_bufferIndex&7);
    m_bufferIndex=(m_bufferIndex+13)%416;
  } while  (m_bufferIndex!=0);
  Write(m_buffer,sizeof(m_buffer));
}

void CAR::Close(void)
{
  Flush();
  m_compressType=0;
  //CArchive::Close();
  ar.Close();  // 20121208
}


void CAR::Compress(bool compression)
{
  m_compressType=0;
  if (compression)
  {
    m_codes=new CODES;
    m_w=0xffff;
    m_bufferIndex=0;
    m_stkLen=0;
    m_OC=0xffff;
    m_numCode=256;
    if (IsStoring())
    {
      *this << (char) 2; // Compression type in clear
      memset(m_buffer,0,sizeof(m_buffer));
      m_compressType=2;
      m_cData=new CDATA; // Compression Data
    }
    else
    {
      m_dData=new DDATA; // Decompression Data
      *this >> m_compressType;
    };
  }
  else
  {
    if (m_cData!=NULL) delete m_cData;
    if (m_dData!=NULL) delete m_dData;
    if (m_codes!=NULL) delete m_codes;
    m_cData=NULL;
    m_dData=NULL;
    m_codes=NULL;
  };

}

void CAR::WriteCount(int n)
{
  //if (m_compressType==0) CArchive::WriteCount(n);
  if (m_compressType==0) ar.WriteCount(n); //20121208 PRS
  else operator << (n);
}

void CAR::WriteCount(unsigned int n)
{
  //if (m_compressType==0) CArchive::WriteCount(n);
  if (m_compressType==0) ar.WriteCount(n); //20121208 PRS
  else operator << (n);
}

DWORD CAR::ReadCount(void)
{
  if (m_compressType==0)
  {
    //return CArchive::ReadCount();
    return ar.ReadCount(); //20121208 PRS
  }
  else
  {
    DWORD temp;
    operator >> (temp);
    return temp;
  };
}

CAR& CAR::operator << (int Int)
{
  //if (m_compressType==0) CArchive::operator << (Int);
  if (m_compressType==0) ar << (Int); //20121208 PRS
  else compress((char *)(&Int),4);
  return *this;
}

CAR& CAR::operator >> (int& Int)
{
  //if (m_compressType==0) CArchive::operator >> (Int);
  if (m_compressType==0) ar >> (Int); //20121208 PRS
  else decompress((char *)(&Int),4);
  return *this;
}


CAR& CAR::operator << (unsigned int UInt)
{
  //if (m_compressType==0) CArchive::operator << (UInt);
  if (m_compressType==0) ar << (UInt); //20121208 PRS
  else compress((char *)(&UInt),4);
  return *this;
}

CAR& CAR::operator >> (unsigned int& UInt)
{
  //if (m_compressType==0) CArchive::operator >> (UInt);
  if (m_compressType==0) ar >> (UInt); //20121208 PRS
  else decompress((char *)(&UInt),4);
  return *this;
}

CAR& CAR::operator << (unsigned long ULong)
{
  return operator << ((unsigned int) ULong);
}

CAR& CAR::operator >> (unsigned long& ULong)
{
  return operator >> ((unsigned int&) ULong);
}

CAR& CAR::operator << (long Long)
{
  return operator << ((int) Long);
}

CAR& CAR::operator >> (long& Long)
{
  return operator >> ((int&) Long);
}

CAR& CAR::operator << (double Double)
{
  //if (m_compressType==0) CArchive::operator << (Double);
  if (m_compressType==0) ar << (Double); //20121208 PRS
  else compress ((char *)(&Double), 8);
  return *this;
}

CAR& CAR::operator >> (double& Double)
{
  //if (m_compressType==0) CArchive::operator >> (Double);
  if (m_compressType==0) ar >> (Double); //20121208 PRS
  else decompress ((char *)(&Double), 8);
  return *this;
}
CAR& CAR::operator << (float Float)
{
  //if (m_compressType==0) CArchive::operator << (Float);
  if (m_compressType==0) ar << (Float);//20121208 PRS
  else compress ((char *)(&Float), 4);
  return *this;
}

CAR& CAR::operator >> (float& Float)
{
  //if (m_compressType==0) CArchive::operator >> (Float);
  if (m_compressType==0) ar >> (Float); //20121208
  else decompress ((char *)(&Float), 4);
  return *this;
}
CAR& CAR::operator << (BYTE Byte)
{
  //if (m_compressType==0) CArchive::operator << (Byte);
  if (m_compressType==0) ar << (Byte); //20121208 PRS
  else compress ((char *)(&Byte), 1);
  return *this;
}

CAR& CAR::operator >> (BYTE& Byte)
{
  //if (m_compressType==0) CArchive::operator >> (Byte);
  if (m_compressType==0) ar >> (Byte); //20121208 PRS
  else decompress((char *)(&Byte), 1);
  return *this;
}

CAR& CAR::operator << (char Char)
{
  //if (m_compressType==0) CArchive::operator << (Char);
  if (m_compressType==0) ar << (Char); //20121208 PRS
  else compress (&Char, 1);
  return *this;
}

CAR& CAR::operator >> (char& Char)
{
  //if (m_compressType==0) CArchive::operator >> (Char);
  if (m_compressType==0) ar >> (Char); //20121208 PRS
  else decompress(&Char, 1);
  return *this;
}

CAR& CAR::operator << (short Short)
{
  //if (m_compressType==0) CArchive::operator << (Short);
  if (m_compressType==0) ar << (Short); //20121208 PRS
  else compress((char *)(&Short), 2);
  return *this;
}

CAR& CAR::operator >> (short& Short)
{
  //if (m_compressType==0) CArchive::operator >> (Short);
  if (m_compressType==0) ar >> (Short); //20121208 PRS
  else decompress((char *)(&Short), 2);
  return *this;
}

CAR& CAR::operator << (unsigned short UShort)
{
  //if (m_compressType==0) CArchive::operator << (UShort);
  if (m_compressType==0) ar << (UShort); //20121208 PRS
  else compress((char *)(&UShort), 2);
  return *this;
}

CAR& CAR::operator >> (unsigned short& UShort)
{
  //if (m_compressType==0) CArchive::operator >> (UShort);
  if (m_compressType==0) ar >> (UShort); //20121208 PRS
  else decompress((char *)(&UShort), 2);
  return *this;
}

CAR& CAR::operator << (const char *str)
{
  CString temp(str);
  *this << temp;
  return *this;
}

CAR& CAR::operator >> (char *str)
{
  CString temp;
  *this >> temp;
  strcpy(str, (LPCSTR)temp);
  return *this;
}


CString CAR::CanonicalString(CString& str)
{
  unsigned int index;
  if (stringIndex.Lookup(str,index))
  {
    return m_stringArray[index];
  };
  stringIndex.SetAt(str,m_nextIndex);
  m_stringArray.SetAtGrow(m_nextIndex, str);
  m_nextIndex++;
  return m_stringArray[m_nextIndex-1];
}


CAR& CAR::operator << (const CString& str)
{ // Keeping a list of strings not only shortens the output file but
  // perhaps more importantly it causes all identical strings to
  // share the same memory when the file is loaded.  It thereby makes
  // string loading faster, too.
  if (m_compressType==0)
  {
    //*(CArchive*)(this) << str;
    ar << str;
    return *this;
  };
  unsigned int index;
  index=0;
  if (str.GetLength() == (int)strlen(str))
  {
    if (stringIndex.Lookup(str,index))
    {
      operator << (index);
      return *this;
    };
    operator << (index);
//  *((CArchive *) this) << str;
    int len=str.GetLength();
    operator << (len);
    compress((LPCSTR)str, len);
    stringIndex.SetAt(str,m_nextIndex);
    m_nextIndex++;
    return *this;
  }
  else
  {
    int len=str.GetLength();
    operator << (index);
    operator << (len);
    compress((LPCSTR)str, len);
    return *this;
  };
}

CAR& CAR::operator >> (CString& str)
{
  if (m_compressType==0)
  {
    //*(CArchive *)this >> str;
    ar >> str;
    return *this;
  };
  unsigned int index;
  operator >> (index);
  if (index==0)
  { // A new string
//    *(CArchive *)this >> str;
    int len;
    char *temp=NULL;
    operator >> (len);
    if (len > 1000000)
    {
      throw(0x23);
    };
    if (len==0)
    {
      str="";
    }
    else
    {
      temp = (char *)My_malloc(len+1);
      if (temp==NULL)
      {
        MsgBoxError("Memory exhausted");
        str="";
      }
      else
      {
        decompress(temp,len);
        temp[len] = 0;
        str = CString(temp, len);
        if (m_compressType > 1)
        {
          if (len != (int)strlen(temp))
          {
            My_free(temp);
            return *this;
          };
        };
      };
    };
    m_stringArray.SetAtGrow(m_nextIndex,str);
    m_nextIndex++;
    if (temp!=NULL) My_free(temp);
    return *this;
  };
  if (index >= (unsigned)m_stringArray.GetSize())
  {
    throw 0x23;
  };
  str = m_stringArray[index];
  return *this;
}

CAR& CAR::operator << (CArray<WORD,WORD>& warray)
{
  if (m_compressType==0)
  {
    //warray.Serialize(*(CArchive *)this);
    warray.Serialize(ar);
    return *this;
  };
  *this << warray.GetSize();
  if (warray.GetSize() > 0)
  {
    compress((char *)(&warray[0]),warray.GetSize()*sizeof(WORD));
  };
  return *this;
}

CAR& CAR::operator >> (CArray<WORD,WORD>& warray)
{
  if (m_compressType==0)
  {
    //warray.Serialize(*(CArchive *)this);
    warray.Serialize(ar);
    return *this;
  };
  int size;
  *this >> size;
  warray.SetSize(size);
  decompress((char *)(&warray[0]),size*sizeof(WORD));
  return *this;
}


CAR& CAR::operator << (CArray<DWORD,DWORD>& warray)
{
  if (m_compressType==0)
  {
    //warray.Serialize(*(CArchive *)this);
    warray.Serialize(ar);
    return *this;
  };
  *this << warray.GetSize();
  if (warray.GetSize() > 0)
  {
    compress((char *)(&warray[0]),warray.GetSize()*sizeof(DWORD));
  };
  return *this;
}

CAR& CAR::operator >> (CArray<DWORD,DWORD>& warray)
{
  if (m_compressType==0)
  {
    //warray.Serialize(*(CArchive *)this);
    warray.Serialize(ar);
    return *this;
  };
  int size;
  *this >> size;
  warray.SetSize(size);
  if (size > 0)
  {
    decompress((char *)(&warray[0]),size*sizeof(DWORD));
  };
  return *this;
}


CAR& CAR::Serialize(char *buf, int n)
{
  if (m_compressType==0)
  {
    if (IsStoring())
    {
      Write(buf, n);
    }
    else
    {
      Read(buf,n);
    }
  }
  else
  {
    if (IsStoring())
    {
      compress(buf, n);
    }
    else
    {
      decompress(buf, n);
    };
  }
  return *this;
}




CAR& CAR::Serialize (A_ASLENTRY_L& asl, LPCSTR mapName)
{
  POSITION pos;
  const ASLENTRY *pEntry;
  if (m_compressType==0)
  {
    //asl.Serialize(*(CArchive *)this, mapName);
    asl.Serialize(ar, mapName);
    return *this;
  };
  *this << mapName;
  *this << asl.GetCount();
  pos=asl.GetStartPosition();
  while (pos!=NULL)
  {
    asl.GetNextAssoc(pos,&pEntry);
    pEntry->Serialize(this);
  };
  return *this;
}


CAR& CAR::DeSerialize (A_ASLENTRY_L& asl, LPCSTR mapName)
{
  if (m_compressType==0)
  {
    //asl.Serialize(*(CArchive *)this, mapName);
    asl.Serialize(ar, mapName);
    return *this;
  };
  CString key;
  int count;
  ASLENTRY entry("","",0);
  *this >> key;
  if (key!=mapName)
  {
	  CString msg;
	  msg.Format("Expected:\n%s\nEncountered:\n%s\nwhile reading file.",
		            mapName,key);
    WriteDebugString(msg);
	  MsgBoxError(msg);
        throw 7;
        // I replaced the AfsThrowArchiveExecption because I could not
        // figure out how tatch it properly.  It caused a memory leak.
		//AfxThrowArchiveException(CArchiveException::badIndex,
    //           GetFile()->GetFileName());
  };
  *this >> count;
  asl.Clear();
  for (;count>0; count--)
  {
    entry.DeSerialize(this);
    asl.Insert(&entry);
  };
  return *this;
}




void CAR::compress(const char *chars, int n)
{
//  static unsigned int buffer[13];
//  static unsigned int bufferIndex=0;
//  static unsigned _int16 w=0xffff;
  unsigned __int16 cd;
  unsigned char c;
  for (;n>0; n--, chars++)
  {
    if (m_codes->Full())
    {
      // And output the old code from w.
      *((unsigned int *)(((char *)m_buffer)+(m_bufferIndex>>3)))|=m_w<<(m_bufferIndex&7);
      m_bufferIndex=(m_bufferIndex+13)%416;
      if (m_bufferIndex==0)
      {
        Write(m_buffer,52);
        memset(m_buffer,0,52);
      };
      m_w=0xffff;
      m_codes->Clear();
      // And output the RESET code
      *((unsigned int *)(((char *)m_buffer)+(m_bufferIndex>>3)))|=8190<<(m_bufferIndex&7);
      m_bufferIndex=(m_bufferIndex+13)%416;
      if (m_bufferIndex==0)
      {
        Write(m_buffer,52);
        memset(m_buffer,0,52);
      };
    };
    c=*chars;
    if (m_w==0xffff)
    { // Single characters are in code list by definition.
      m_w=c;
      continue;
    };
    if (((cd=m_codes->FindCode((m_w<<8)|c))!=0) /*&& (c<256) What was this for????*/)
    {
      // We found the new, larger string in the table.
      m_w=cd;
      if (m_w > 8189) // 8191 reserved for EOF, 8190 for RESET
      {
        _exit(1);
      };
      continue;
    };
    // We want to add the new, larger string to the table.
    m_codes->AddCode((m_w<<8)|c);
    // And output the old string from m_w.
    *((unsigned int *)(((char *)m_buffer)+(m_bufferIndex>>3)))|=m_w<<(m_bufferIndex&7);
    m_bufferIndex=(m_bufferIndex+13)%416;
    if (m_bufferIndex==0)
    {
      Write(m_buffer,sizeof(m_buffer));
      memset(m_buffer,0,sizeof(m_buffer));
    };
    m_w=c;
  };
}

void CAR::decompress(char *chars, int n)
{
 // static unsigned _int16 OC=0xffff;
 // static unsigned _int16 prefix[8192];
 // static unsigned char   postfix[8192];
 // static unsigned int numCode=256;
    unsigned __int16 NC;
 // static unsigned char stack[1000];
 // static unsigned char C;
 // static unsigned int stkLen=0;
    unsigned int code;
 // static unsigned int buffer[13];
 // static unsigned int bufferIndex=0;
  unsigned __int16 theN;
  for (;n>0; n--, chars++)
  {
    if (m_stkLen!=0)
    {
      *chars = m_dData->m_stack[--m_stkLen];
      continue;
    };
    if (m_bufferIndex==0)
    {
      if (Read(m_buffer,sizeof(m_buffer))!=sizeof(m_buffer)) return;
    };
    theN = (unsigned __int16)(((*((unsigned int *)(((char *)m_buffer)+(m_bufferIndex>>3))))>>(m_bufferIndex&7))&0x1fff);
    m_bufferIndex=(m_bufferIndex+13)%416;
    while (theN==8190) // RESET
    {
      m_OC=0xffff;
      m_numCode=256;
      if (m_bufferIndex==0)
      {
        if (Read(m_buffer,52)!=52) return;
      };
      theN = (unsigned __int16)(((*((unsigned int *)(((char *)m_buffer)+(m_bufferIndex>>3))))>>(m_bufferIndex&7))&0x1fff);
      m_bufferIndex=(m_bufferIndex+13)%416;
    };
    if (m_OC == 0xffff)
    {
      m_OC=theN;
      *chars = theN;
      m_C = theN;
      continue;
    };
    NC=theN;
    if (NC > 255)
    {
      if (NC >= m_numCode)
      {
        if (NC == 8191) return;
        // Expand OC+C into stack;
        m_dData->m_stack[m_stkLen++] = m_C;
        code=m_OC;
      }
      else
      {
        // Expand NC onto stack.
        code = NC;
      };
      while (code > 255)
      {
        ASSERT (code < m_numCode);
        m_dData->m_stack[m_stkLen++] = m_dData->m_postfix[code];
        code = m_dData->m_prefix[code];
      };
      m_dData->m_stack[m_stkLen++] = (unsigned char)code;
      m_C = m_dData->m_stack[m_stkLen-1];
    }
    else
    {
      m_C = (unsigned char)NC;
      m_dData->m_stack[m_stkLen++]=m_C;
    };
    m_dData->m_prefix[m_numCode] = m_OC;
    m_dData->m_postfix[m_numCode++] = m_C;
    m_OC = NC;
    *chars = m_dData->m_stack[--m_stkLen];
  };
}



CAR::CODES::CODES(void)
{
  Clear();
  m_numMisses=0;
  m_numCodes=0;
}

CAR::CODES::~CODES(void)
{
}

void CAR::CODES::Clear(void)
{
  memset(m_hashTable,0,sizeof (m_hashTable));
  memset(m_codes,0xff,256*sizeof (unsigned int));
  m_nextCode=256;
}

unsigned __int16 CAR::CODES::AddCode(unsigned int code)
{
  unsigned int hashcode, incr;
  m_numCodes++;
  ASSERT(m_nextCode <= 8189);
  incr = hashcode = code % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
  if (incr==0) incr=1;
  while (m_hashTable[hashcode]!=0)
  {
    m_numMisses++;
    hashcode = (hashcode + incr) % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
  };
  m_hashTable[hashcode] = m_nextCode;
  m_codes[m_nextCode] = code;
  return m_nextCode++;
}

unsigned __int16 CAR::CODES::FindCode(unsigned int code)
{
  unsigned int hashcode, incr;
  incr = hashcode = code % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
  if (incr==0) incr=1;
  while (m_hashTable[hashcode]!=0)
  {
    if (m_codes[m_hashTable[hashcode]]==code) return m_hashTable[hashcode];
    hashcode = (hashcode + incr) % (sizeof(m_hashTable) / sizeof(m_hashTable[0]));
  };
  return 0;
}

void CAR::CODES::HashDump(FILE *f)
{
  int code, stklen;
  int stk[1000];
  for (int i=256; i<m_nextCode; i++)
  {
    code =i;
    fprintf(f,"%4d ",code);
    stklen=0;
    while (code > 255)
    {
      code = m_codes[code];
      stk[stklen++]=code & 0xff;
      code = code >> 8;
    };
    stk[stklen++]=code;
    while (stklen > 0)
    {
      fprintf(f,"%3d ",stk[--stklen]);
    };
    fprintf(f,"\n");
  };
}



void CASTING_INFO::Serialize(CAR& car)
{
  if (car.IsStoring())
  {
    car << schoolID;
    car << primeAbility;
  }
  else
  {
    car >> schoolID;
    car >> primeAbility;
  };
  car.Serialize((char *)m_spellLimits, sizeof(m_spellLimits));
  car.Serialize((char *)m_maxSpellLevelsByPrime, sizeof(m_maxSpellLevelsByPrime));
  car.Serialize((char *)m_maxSpellsByPrime, sizeof(m_maxSpellsByPrime));
}

#ifdef UAFEDITOR
void ABILITY_DATA_TYPE::CrossReference(CR_LIST *pCRList)
{
  int n, i;
  n = GetCount();
  for (i = 0; i < n; i++)
  {
    GetAbilityData(i)->CrossReference(pCRList);
  };
}
#endif

