/******************************************************************************
* Filename: Monster.h
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
#ifndef __MONSTER_H__
#define __MONSTER_H__

//#include "externs.h"
#include "class.h"
#include "items.h"
#include "money.h"
//#include "ProjectVersion.h"
//#include "SoundMgr.h"


const int MAX_MONSTERS = 0x00FFFFFF;
const int MAX_MONSTER_NAME = 30; // should match MAX_CHAR_NAME!
const int NUM_SIZE_TYPES = 3;
enum creatureSizeType { Small, Medium, Large };

//*** Monster Special Stuff ***

/*
Form
* MAMMAL
Indicates whether the monster is vulnerable to the second level Druid
Spell "Charm Person or Mammal."

* ANIMAL
Determines whether the monster is susceptible to the first level Druid
spell "Invisibility to Animals."

* SNAKE
Determines whether the monster is vulnerable to the second level
Clerical spell "Snake Charm."

* TRUE GIANT
Indicates whether the monster takes extra damage from a Long Sword vs
Giants.

* LARGE EVEN IF ICON 1 BY 1
Monster receives weapon damage based on the "Large" column on the
damage table.  
*/

enum MonsterFormType { FormMammal=1, FormAnimal=2, FormSnake=4, FormGiant=8, FormLarge=16 /*,FormUndead=32*/ };

/*
Penalties

* DWARF AC
Indicates that creatures with the effect "Dwarf AC bonus" (e.g.
dwarves) get an AC bonus of +4 when attacked by the monster.

* GNOME AC
Indicates that creatures with the effect "Gnome AC bonus" (e.g.
gnomes) get an AC bonus of +4 when attacked by the monster.

* DWARF THAC0
Indicates that creatures with the effect "Dwarf THAC0 +1" (e.g.
dwarves) get a THAC0 bonus of +1 when attacking the monster.

* GNOME THAC0
Indicates that creatures with the effect "Gnome THAC0 +1" (e.g.
gnomes) get a THAC0 bonus of +1 when attacking the monster.

* RANGER DAMAGE
Indicates that creatures with the effect "Ranger Gnt Bonus" (e.g.
rangers) get a damage bonus equal to their ranger levels whenever they
hit the monster.
*/

enum MonsterPenaltyType { PenaltyDwarfAC=1, PenaltyGnomeAC=2, 
                          PenaltyDwarfTHAC0=4, PenaltyGnomeTHAC0=8,
                          PenaltyRangerDmg=16 };

/*
IMMUNE TO

* POISON
Indicates that the monster is immune to poison, including "stinking
cloud".

* DEATH
Indicates that the monster is immune to all death magic, including
"Cloudkill."

* CONFUSION
Indicates that confusion wands, spells, and gazes have no effect on
the monster.

* VORPAL SWORDS
Indicates that the monster cannot be decapitated by a vorpal blade.
*/

enum MonsterImmunityType { ImmunePoison=1, ImmuneDeath=2, ImmuneConfusion=4, ImmuneVorpal=8 };

/*
MISC OPTIONS

* MAY BE HELD OR CHARMED
Indicates that the monster is vulnerable to charm or hold spells. 
Note that the monster is also invulnerable to "Hold Person" spells if
it is of race "monster"; only humans and similar races are susceptible
to "Hold Person."

* AFFECTED BY DISPEL EVIL
Indicates that the monster is summoned from another plane of existence
and may be affected by the fifth level clerical spell "Dispel Evil".
*/

enum MonsterMiscOptionsType { OptionCanBeHeldCharmed=1, OptionAffectedByDispelEvil=2 };

#define MAX_MONSTER_ATTACK_MSG_LEN 20

//struct MONSTER_ATTACK_DETAILS
struct ATTACK_DETAILS
{
  //MONSTER_ATTACK_DETAILS() { Clear(); }
  ATTACK_DETAILS() { Clear(); }
  //~MONSTER_ATTACK_DETAILS() { }
  ~ATTACK_DETAILS() { }
  void Clear()
  { 
    sides=0;
    nbr=0;
    bonus=0;
    attackMsg="";
    //spelldbkey.Clear();
    spellID.Clear();
#ifdef UAFEDITOR
    preVersionSpellNames_gsID = -1;
#endif
    spellclass=0;
    spelllevel=0; 
  };
  //MONSTER_ATTACK_DETAILS& operator =(const MONSTER_ATTACK_DETAILS& src)
  ATTACK_DETAILS& operator =(const ATTACK_DETAILS& src)
  {
    if (this==&src) return *this;
    Clear();
    sides=src.sides;
    nbr=src.nbr;
    bonus=src.bonus;
    attackMsg=src.attackMsg;
    //spelldbkey=src.spelldbkey;
    spellID=src.spellID;
    spellclass=src.spellclass;
    spelllevel=src.spelllevel;
    return *this;
  }
  //bool operator == (const MONSTER_ATTACK_DETAILS& src) const
  bool operator == (const ATTACK_DETAILS& src) const
  {
    if (this==&src) return true;
    if (sides!=src.sides) return false;
    if (nbr!=src.nbr) return false;
    if (bonus!=src.bonus) return false;
    if (attackMsg!=src.attackMsg) return false;
    //if (spelldbkey!=src.spelldbkey) return false;
    if (spellID!=src.spellID) return false;
    if (spellclass!=src.spellclass) return false;
    if (spelllevel!=src.spelllevel) return false;
    return true;
  }
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);

  void CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference) const;
  int sides; // nbr sides on each dice
  int nbr;   // nbr of dice to roll
  int bonus; // bonus per roll
  //GLOBAL_SPELL_ID spelldbkey;
  SPELL_ID spellID;
  int spellclass; // class/level needed to satisfy using
  int spelllevel; // spellbook class in editor
  CString attackMsg; // attack msg

#ifdef UAFEDITOR
  // These are not serialized and are only to allow the
  // editor to convert old designs.
  int preVersionSpellNames_gsID;  // Old GLOBAL_ITEM_ID
#endif
};

//struct MONSTER_ATTACK_DATA
struct ATTACK_DATA
{
  //MONSTER_ATTACK_DATA() { Clear(); }
  ATTACK_DATA() { Clear(); }
  //~MONSTER_ATTACK_DATA() { }
  ~ATTACK_DATA() { }
  void Clear() { monsterAttackDetails.RemoveAll(); }
  //int GetCount() const { return data.GetSize(); }
  //MONSTER_ATTACK_DATA& operator =(const MONSTER_ATTACK_DATA& src)
  ATTACK_DATA& operator =(const ATTACK_DATA& src)
  {
    if (this==&src) return *this;
    Clear();
    for (int i=0;i<src.GetMonsterAttackDetailsCount();i++)
      Add(*src.PeekMonsterAttackDetails(i));
    return *this;
  }
//  bool operator == (const MONSTER_ATTACK_DATA& src) const
  bool operator == (const ATTACK_DATA& src) const
  {
    if (this==&src) return true;
    if (GetMonsterAttackDetailsCount()!=src.GetMonsterAttackDetailsCount()) return false;
    // order is also important
    for (int i=0;i<GetMonsterAttackDetailsCount();i++)
    {
      if ( !(*PeekMonsterAttackDetails(i)== *src.PeekMonsterAttackDetails(i)) )
        return false;
    }
    return true;
  }

  //int Add(const MONSTER_ATTACK_DETAILS& src)
  //{
  //  int index = GetCount();
  //  data.SetAtGrow(index, src);
  //  return index;
  //}

  void Delete(int index)
  {
    if ((index>=0)&&(index<GetMonsterAttackDetailsCount()))
      monsterAttackDetails.RemoveAt(index);
  }
  void Serialize(CArchive &ar, double version)
  {
    if (ar.IsStoring())
    {
      int cnt=GetMonsterAttackDetailsCount();
      ar << cnt;
      for (int i=0;i<cnt;i++)
        GetMonsterAttackDetails(i)->Serialize(ar,version);
    }
    else
    {
      //MONSTER_ATTACK_DETAILS tmp;
      ATTACK_DETAILS tmp;
      Clear();
      int cnt;
      ar >> cnt;
      for (int i=0;i<cnt;i++)
      {
        tmp.Serialize(ar,version);
        Add(tmp);
      }
    }
  }
  void Serialize(CAR &ar, double version)
  {
    if (ar.IsStoring())
    {
      int cnt=GetMonsterAttackDetailsCount();
      ar << cnt;
      for (int i=0;i<cnt;i++)
        GetMonsterAttackDetails(i)->Serialize(ar,version);
    }
    else
    {
      //MONSTER_ATTACK_DETAILS tmp;
      ATTACK_DETAILS tmp;
      Clear();
      int cnt;
      ar >> cnt;
      for (int i=0;i<cnt;i++)
      {
        tmp.Serialize(ar,version);
        Add(tmp);
      }
    }
  }

  void CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference) const
  {
    int i;
    for (i=0; i<GetMonsterAttackDetailsCount(); i++)
    {
      PeekMonsterAttackDetails(i)->CrossReference(pCRList, pCRReference);
    };
  };

//        MONSTER_ATTACK_DETAILS &GetAtIndex(int index)  { return data[index]; }
//  const MONSTER_ATTACK_DETAILS &PeekAtIndex(int index) const
//  { 
//    return const_cast<MONSTER_ATTACK_DATA *>(this)->data[index]; 
//  };
private:
  //mCArray<MONSTER_ATTACK_DETAILS, MONSTER_ATTACK_DETAILS&> monsterAttackDetails;
  mCArray<ATTACK_DETAILS, ATTACK_DETAILS&> monsterAttackDetails;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(MonsterAttackDetails,   /* array data name      */ \
                                  MONSTER_ID,             /* Search variable type */ \
                                  monsterID,              /* search variable name */ \
                                  monsterAttackDetails,   /* array name           */ \
                                  //MONSTER_ATTACK_DETAILS,  array data type       
                                  ATTACK_DETAILS, /* array data type      */ \
                                  ATTACK_DATA)    /* type of *this        */
  void SetMonsterAttackDetailsSize(int n)
  {
    monsterAttackDetails.SetSize(n);
  };
//  void SetMonsterAttackDetailsAt(int i, MONSTER_ATTACK_DETAILS& ad)
  void SetMonsterAttackDetailsAt(int i, ATTACK_DETAILS& ad)
  {
    monsterAttackDetails.SetAt(i, ad);
  };
};

class MONSTER_DATA // 20121212 PRS : public CObject
{
public:
  // 20121212 PRSDECLARE_SERIAL( MONSTER_DATA )
  MONSTER_DATA();
  MONSTER_DATA(const MONSTER_DATA &src) : 
        // ASLs are named "MONSTER_DATA_ATTRIBUTES"
      	mon_asl(),
	      temp_asl(),
	      //hHitSound(-1), hMissSound(-1), hMoveSound(-1), hDeathSound(-1) { Clear(); *this = src; }
	      //HitSound(-1), MissSound(-1), MoveSound(-1), DeathSound(-1) { Clear(); *this = src; }
	      HitSound(), 
        MissSound(), 
        MoveSound(), 
        DeathSound(),
        specAbs(true)
        { Clear(); *this = src; }
  ~MONSTER_DATA() { ClearSounds(); Clear(TRUE); }
  MONSTER_DATA& operator =(const MONSTER_DATA& src);
#ifdef UAFEDITOR
  bool operator == (const MONSTER_DATA& src) const;
#endif
  int  GetDataSize(void) const;
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring);
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  void Save(CAR& car) const;
  void PrepareForRestore(void);
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);
  void CrossReference(CR_LIST *pCRList) const;
  void Clear(BOOL ctor=FALSE);

  void PlayHit();
  void PlayMiss();
  void PlayMove();
  void PlayDeath();
  void ClearSounds();
  ActorType GetContext() const;
  void GuessUndeadStatus();
  MONSTER_ID MonsterID(void) const 
  {
    MONSTER_ID mID; 
#ifdef UAFEngine
    mID = monsterID;
#else
    mID = Name; 
#endif
    return mID;
  };

  //int key;
  int preSpellNameKey;  //See comment at defintion of VersionSaveIDs 
	CString Name;  // In the engine, the printable name.
                 // In the editor, the unique name.
#ifdef UAFEngine
  CString monsterID;  // Not serialized.  Used as unique ID in the engine.
#endif
  PIC_DATA Icon;
  CLASS_ID classID;
  RACE_ID  raceID; // Serialized via ASL


  //CString HitSound;
  //CString MissSound;
  //CString MoveSound;
  //CString DeathSound;
  //int hHitSound;
  //int hMissSound;
  //int hMoveSound;
  //int hDeathSound;

  SOUND_BYTE HitSound;
  SOUND_BYTE MissSound;
  SOUND_BYTE MoveSound;
  SOUND_BYTE DeathSound;


	long Intelligence;
	long Armor_Class;
	long Movement;
	float Hit_Dice;
  BOOL UseHitDice;
	long Hit_Dice_Bonus;
	long THAC0;
	long Magic_Resistance;
	creatureSizeType Size;
	long Morale;
	long XP_Value;
  
	SPECIAL_ABILITIES specAbs;
  CString RunMonsterScripts(LPCSTR     scriptName, 
                            CBRESULT (*fnc)(CBFUNC func, CString *scriptResult, void *pkt), 
                            void      *pkt,
                            LPCSTR     comment
                            ) const
  {
    return specAbs.RunScripts(scriptName, 
                              fnc, 
                              pkt,
                              comment,
                              ScriptSourceType_Monster, 
                              Name);
  };

  //BYTE  ItemMask;
  DWORD Form_Type;
  DWORD Penalty_Type;
  DWORD Immunity_Type;
  DWORD MiscOptions_Type;
  ITEM_LIST myItems;
  MONEY_SACK money;

  //undeadType undeadStatus;
  CString undeadType;

  //MONSTER_ATTACK_DATA attackData;
  ATTACK_DATA attackData;
  A_ASLENTRY_L mon_asl;
  A_ASLENTRY_L temp_asl;
};



class MONSTER_DATA_TYPE // 20121212 PRS: public CObject
{
  //POSITION pos;
  //CCriticalSection m_CS;
  //OrderedQueue<MONSTER_DATA, MAX_MONSTERS> MonsterData;
  mCArray<MONSTER_DATA, MONSTER_DATA&> MonsterData;
  //int GetNextKey();

public:

  //20121212 PRSDECLARE_SERIAL( MONSTER_DATA_TYPE )
  MONSTER_DATA_TYPE() { Clear(); }
  ~MONSTER_DATA_TYPE() { Clear(); }

  MONSTER_DATA_TYPE& operator = (MONSTER_DATA_TYPE& src);
  bool operator ==(MONSTER_DATA_TYPE& src) const ;
  //void Clear() { CSingleLock sLock(&m_CS, TRUE); MonsterData.RemoveAll();version=0.0; }
  void Clear() { MonsterData.RemoveAll();version=0.0; }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar, double version);
  void Save(CAR& car) const;
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);
  //int  GetCount() { CSingleLock sLock(&m_CS, TRUE); return MonsterData.GetCount(); }
  //int  GetCount() { return MonsterData.GetCount(); }
  int  GetCount() const { return MonsterData.GetSize(); }
  int  GetDataSize(void) const;
  void SaveSounds();
  void SaveArt() ;
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring);

  //POSITION GetHeadPosition() { return MonsterData.GetHeadPosition(); }
  //      MONSTER_DATA& GetAtPos(POSITION pos)        { return MonsterData.GetAtPos(pos); }
  //const MONSTER_DATA& PeekAtPos(POSITION pos) const { return MonsterData.PeekAtPos(pos); }
  //MONSTER_DATA& GetNext(POSITION &pos) { return MonsterData.GetNext(pos); }

  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Monster,MONSTER_ID,monsterID,MonsterData,MONSTER_DATA,MONSTER_DATA_TYPE)
#ifdef UAFEDITOR
  int LocatePreSpellNamesKey(int key);
#endif

  //int LocateMonsterData(const MONSTER_ID& monsterID) const;
  //const MONSTER_DATA *PeekMonsterData(int index) const {return &(const_cast<MONSTER_DATA_TYPE*>(this)->MonsterData[index]);};
  //MONSTER_DATA *GetMonsterData(int index);
  //const MONSTER_DATA *PeekMonsterData(const MONSTER_ID& monsterID) const;
  //MONSTER_DATA *GetMonsterData(const MONSTER_ID& monsterID);


  //int GetKeyAt(POSITION pos) { return MonsterData.GetKeyAt(pos); }

  //MONSTER_DATA *GetMonsterData(const CString& monsterName);
  void ExpandDefaultFilenames();
#ifdef UAFEDITOR
  void CollapseDefaultFilenames();
#endif

  //ActorType GetContext(int index) const;
  ActorType GetContext(const MONSTER_ID& monsterID) const;

  //BOOL Validate(MONSTER_DATA data, FILE *pFile) const;
  //BOOL Validate(int index, FILE *pFile) const; // specific monster
  BOOL Validate(const MONSTER_DATA *pMonster, FILE *pFile) const;
  BOOL Validate(const MONSTER_ID& monsterID, FILE *pFile) const; // specific monster
  BOOL Validate(FILE *pFile); // all monsters
  void CrossReference(CR_LIST *pCRList) const;
  //void PlayHit(int index);
  //void PlayMiss(int index);
  //void PlayMove(int index);
  //void PlayDeath(int index);
  void PlayHit  (const MONSTER_ID& monsterID);
  void PlayMiss (const MONSTER_ID& monsterID);
  void PlayMove (const MONSTER_ID& monsterID);
  void PlayDeath(const MONSTER_ID& monsterID);
  void ClearSounds();
  //CString GetMonsterName(int index) const;

  //BOOL HaveMonster(int index);
  BOOL HaveMonster(const MONSTER_ID& monsterID) const;
  // called when loading from scripts, or
  // when adding from editor GUI
  //int AddMonster(MONSTER_DATA &data);
  MONSTER_ID AddMonster(MONSTER_DATA *pMonster);
  // called when loading binary monster data that
  // was saved with key info
  //int AddMonsterWithCurrKey(MONSTER_DATA &data);
  //BOOL SetMonster(int index, MONSTER_DATA *pMonster);
  //BOOL RemoveMonster(int index);
  //MONSTER_DATA *GetMonsterData(int index);
  //creatureSizeType GetMonsterSize(int index) const;
  //int GetMonsterNbrAttacks(int index) const;
  //int GetMonsterExpValue(int index) const;
  //int GetMonsterTHAC0(int index) const;
  //int GetMonsterAC(int index) const;
  //void GetMonsterDamageDice(int index, int attackNum, int &num, int &sides, int &bonus) const;
  //void GetMonsterAttackMsg(int index, int attackNum, CString &msg) const;
  //BYTE GetMonsterItemMask(int index) const;
  //double GetMonsterHitDice(int index) const;
  //int GetMonsterHitDiceBonus(int index) const;
  //int GetMonsterMovement(int index) const;
  //DWORD GetMonsterFormFlags(int index) const;
  //DWORD GetMonsterOptionFlags(int index) const;
  //DWORD GetMonsterImmunityFlags(int index) const;
  //DWORD GetMonsterPenaltyFlags(int index) const;


  BOOL RemoveMonster              (const MONSTER_ID& monsterID);
  creatureSizeType GetMonsterSize (const MONSTER_ID& monsterID) const;
  int GetMonsterNbrAttacks        (const MONSTER_ID& monsterID) const;
  int GetMonsterExpValue          (const MONSTER_ID& monsterID) const;
  int GetMonsterTHAC0             (const MONSTER_ID& monsterID) const;
  int GetMonsterAC                (const MONSTER_ID& monsterID) const;
  BYTE GetMonsterItemMask         (const MONSTER_ID& monsterID) const;
  double GetMonsterHitDice        (const MONSTER_ID& monsterID) const;
  int GetMonsterHitDiceBonus      (const MONSTER_ID& monsterID) const;
  int GetMonsterMovement          (const MONSTER_ID& monsterID) const;
  DWORD GetMonsterFormFlags       (const MONSTER_ID& monsterID) const;
  DWORD GetMonsterOptionFlags     (const MONSTER_ID& monsterID) const;
  DWORD GetMonsterImmunityFlags   (const MONSTER_ID& monsterID) const;
  DWORD GetMonsterPenaltyFlags    (const MONSTER_ID& monsterID) const;
  BOOL SetMonster                 (const MONSTER_ID& monsterID, MONSTER_DATA *pMonster);
  void GetMonsterDamageDice       (const MONSTER_ID& monsterID, int attackNum, int &num, int &sides, int &bonus) const;
  void GetMonsterAttackMsg        (const MONSTER_ID& monsterID, int attackNum, CString &msg) const;

  
  
  // No locking... for internal use
  //int m_FindName (const CString& name) const;
  //int m_FindNamei (const CString& name) const; // case-insensitive
  double version;

public:
  // With locking...for public use
  //int FindName(const CString& name);
  //int FindNamei(const CString& name); // case-insensitive
};


#endif