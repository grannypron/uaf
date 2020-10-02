/******************************************************************************
* Filename: Spell.h
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
#ifndef __SPELL_H__
#define __SPELL_H__

//#include "externs.h"
#include "SpecAb.h"
#include "SharedQueue.h"
#include "PicData.h"
#include "class.h"
#include "gamerules.h"
//#include "ProjectVersion.h"


const int COST_FLAG                   = 0x0001;
const int LEVEL_FLAG                  = 0x0002;
const int QTY_FLAG                    = 0x0004;
const int MEMORIZE_FORMAT_FLAG        = 0x0008;
const int SHOW_SELECTED_FLAG          = 0x0010;
const int SHOW_MEMORIZED_FLAG         = 0x0020;
const int SHOW_NOT_MEMORIZED_FLAG     = 0x0040;
const int SHOW_NOT_SELECTED_FLAG      = 0x0080;
const int CAST_FORMAT_FLAG            = 0x0100;
const int SHOW_DESIGN_FLAG            = 0x0200;
const int NEW_SPELL_LEVEL_FORMAT_FLAG = 0x0400;
const int TEMPLE_CAST_FORMAT_FLAG     = 0x0800;
const int CHAR_VIEW_DISPLAY           = 0x1000;
const int ARBITRARY_TEXT_FORMAT_FLAG  = 0x2000;
const int RACE_FORMAT_FLAG            = 0x4000;
const int CLASS_FORMAT_FLAG           = 0x8000;


const int MAX_ACTIVE_SPELLS = INT_MAX;
const int MAX_PENDING_SPELLS = INT_MAX;

struct CHARACTER_SPELL 
{
  CHARACTER_SPELL() { Clear(); }
  ~CHARACTER_SPELL() { Clear(); }
  void Clear();
  CHARACTER_SPELL &operator=(const CHARACTER_SPELL &cSpell);
  BOOL operator==(const CHARACTER_SPELL &src);
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  bool Import(JReader& jr);
#endif

  BOOL IsSelected() const { return (selected > 0); }
  BOOL IsMemorized() const { return (memorized > 0); }
  void ResetMemorizedTime() { memTime=0; }
  BOOL IncMemorizedTime(int minuteInc, bool all);
  BOOL MemTimeSufficient();
  void SetMemorized(bool all);
  void SetUnMemorized();
  BOOL HaveUnmemorized() const;

  int selected;  // TRUE if dude will memorize this spell again
	int memorized; // 
	//GLOBAL_SPELL_ID spell;      // SpellDbKey to retrieve data from spellData table
	SPELL_ID spellID;      // SpellDbKey to retrieve data from spellData table
  int level;      // spell level, here for convenience
  int memTime;    // total time spent on memorizing this spell
  BOOL JustMemorized;

#ifdef UAFEDITOR
  // A non-serialized spell id to allow editor to convert
  // older designs.
  int preVersionSpellNames_gsID;  // The old GLOBAL_SPELL_ID
#endif

};

struct SPELL_LIST 
{
  SPELL_LIST() { Clear(); }
  ~SPELL_LIST() { Clear(); }

private:
  mCArray <CHARACTER_SPELL, CHARACTER_SPELL&> spells;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(CharacterSpell,  /* array data name      */ \
                                  SPELL_ID,        /* Search variable type */ \
                                  spellID,         /* search variable name */ \
                                  spells,          /* array name           */ \
                                  CHARACTER_SPELL, /* array data type      */ \
                                  SPELL_LIST)      /* type of *this        */

  
  
  void Clear();
  int GetCount() const;
  //int GetSortedCount() const {return sortedAccess.GetCount();};

  SPELL_LIST &operator=(const SPELL_LIST &list);
  BOOL operator==(const SPELL_LIST &src);
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  bool Import(JReader& jr);
#endif
  void CrossReference(CR_LIST *pCRList, const CR_REFERENCE *pCRReference) const;

  // Functions to reference CHARACTER_SPELLS in various ways.
  //int                    LocateCharacterSpell(const SPELL_ID& spellID) const;
  //CHARACTER_SPELL       *GetCharacterSpell   (int index) {return &spells[index];};
  //const CHARACTER_SPELL *PeekCharacterSpell  (const SPELL_ID& spellID) const;
  //const CHARACTER_SPELL *PeekCharacterSpell  (int index) const;
  BOOL                   FetchCharacterSpell (const SPELL_ID& spellID, CHARACTER_SPELL *pCharSp) const;


  // these access the sorted list and will return
  // spells is sorted order regardless of spell book
  // ordering.
  //POSITION               GetHeadPosition() const;
  //const CHARACTER_SPELL& PeekHead(void)    const {return PeekAtPos(GetHeadPosition());};
  //POSITION               FindIDPos(const SPELL_ID& spellID) const;


  //      CHARACTER_SPELL&  GetAtPos (POSITION pos);
  //const CHARACTER_SPELL&  PeekAtPos(POSITION pos) const;
  //      CHARACTER_SPELL&  GetNext  (POSITION &pos);
  //const CHARACTER_SPELL&  PeekNext (POSITION &pos) const;
  //GLOBAL_SPELL_ID         GetKeyAt(POSITION pos);
  //SPELL_ID                GetIDAt(POSITION pos) const;

  //
  // These words provide unsorted access to the spell list
  //POSITION                GetUnsortedHeadPosition () const        {return spells.GetHeadPosition();};
  //const CHARACTER_SPELL&  GetUnsortedAt           (POSITION pos)  {return spells.GetAtPos(pos);};
  //const CHARACTER_SPELL&  GetUnsortedAt           (POSITION pos)  {return spells.GetAt(pos);};

  //const CHARACTER_SPELL&  PeekUnsortedNext        (POSITION &pos) const 
  //{
  //  return const_cast<SPELL_LIST *>(this)->spells.GetNext(pos);
  //};

  //GLOBAL_SPELL_ID         GetUnsortedKeyAt        (POSITION pos)  {return spells.GetKeyAt(pos);};
  //SPELL_ID                  GetUnsortedIDAt         (POSITION pos)  {return spells.GetAt(pos).spellID;};
  
/*
  BOOL  IsMemorized       (GLOBAL_SPELL_ID index) const;
  BOOL  DecMemorized      (GLOBAL_SPELL_ID index, int memorized_count);
  BOOL  SetSelected       (GLOBAL_SPELL_ID index, int selected);
  BOOL  IsSelected        (GLOBAL_SPELL_ID index) const;
  BOOL  HaveSpell         (GLOBAL_SPELL_ID index) const;
  BOOL  AddSpell          (CHARACTER_SPELL &data);
  void  SortSpells        (void);
  BOOL  RemoveSpell       (GLOBAL_SPELL_ID index);  
  void  RemoveSpellLevel  (int level);
  int   SpellsMemorized   () const;
  BOOL  GetSpell          (GLOBAL_SPELL_ID gsID, CHARACTER_SPELL &spell);
  BOOL  xSetSpell         (GLOBAL_SPELL_ID gsID, CHARACTER_SPELL &spell);
  DWORD GetRestTime       (GLOBAL_SPELL_ID gsID) const;
  int   GetSelectedCount  (GLOBAL_SPELL_ID gsID) const;
  int   GetMemorizedCount (GLOBAL_SPELL_ID gsID) const;
  int   GetMemorizedTime  (GLOBAL_SPELL_ID gsID) const;
  int   ResetMemorizedTime(GLOBAL_SPELL_ID gsID);
  int   IncMemorizedTime  (GLOBAL_SPELL_ID gsID, int minuteInc);
*/
  BOOL             IsMemorized        (const SPELL_ID& spellID) const;
  BOOL             DecMemorized       (const SPELL_ID& spellID, int memorized_count);
  BOOL             SetSelected        (const SPELL_ID& spellID, int selected);
  BOOL             IsSelected         (const SPELL_ID& spellID) const;
  BOOL             HaveSpell          (const SPELL_ID& spellID) const;
  BOOL             AddSpell           (CHARACTER_SPELL &data);
//  void             SortSpells         (void);
  BOOL             RemoveSpell        (const SPELL_ID& spellID);  
  void             RemoveSpell        (int i);  
  void             RemoveSpellLevel   (int level);
  int              SpellsMemorized    (void) const;
  //BOOL  xSetSpell          (const SPELL_ID& spellID, CHARACTER_SPELL &spell);
  DWORD            GetRestTime        (const SPELL_ID& spellID) const;
  int              GetSelectedCount   (const SPELL_ID& spellID) const;
  int              GetMemorizedCount  (const SPELL_ID& spellID) const;
  DWORD            GetRestTime        (int index) const;
  int              GetSelectedCount   (int index) const;
  int              GetMemorizedCount  (int index) const;
  int              GetMemorizedTime   (const SPELL_ID& spellID) const;
  int              ResetMemorizedTime (const SPELL_ID& spellID);
  int              IncMemorizedTime   (const SPELL_ID& spellID, int minuteInc);
  int              GetLevel           (int index) const;
  int              GetLevel           (const SPELL_ID& spellID) const;

  
  //void  Selected(POSITION pos, int m){spells.GetAtPos(pos).selected = m;};
  void  Selected(int index, int m){GetCharacterSpell(index)->selected = m;};
  //void  Selected(int index, int m){spells[index].selected = m;};
  //void  ResetMemorizedTime(POSITION p){spells.GetAtPos(p).ResetMemorizedTime();};
  void  ResetMemorizedTime(int index){GetCharacterSpell(index)->ResetMemorizedTime();};
  //void  ResetMemorizedTime(int index){spells[index].ResetMemorizedTime();};

private:
  //OrderedQueue <CHARACTER_SPELL, MAX_SPELLS> spells;
  void CopySpells(const SPELL_LIST& spellLIst);
  //CList<int, int> sortedAccess;
  //BOOL m_isSorted; // AddSpell, RemoveSpell set this to false; SortSpells sets it true.
};


// The keys used to access spell data are the
// spell db keys. Some code appears to be using
// an actual spell book key instead, but in
// reality the lookup of such a key only
// returns the spell db key. 
//
// When added, each new spell uses
// the spell db key as the SPELL_LIST key.
//
struct spellBookType
{
private:
	SPELL_LIST list;
public:

  //POSITION GetHeadPosition(void)  const {return list.GetHeadPosition();};
  //
  //
  //      CHARACTER_SPELL& GetAtPos (POSITION pos)        {return list.GetAtPos(pos);};
  //const CHARACTER_SPELL& PeekAtPos(POSITION pos) const  {return list.PeekAtPos(pos);};
  //      CHARACTER_SPELL& GetNext  (POSITION& pos)       {return list.GetNext(pos);};
  //const CHARACTER_SPELL& PeekNext (POSITION& pos) const {return list.PeekNext(pos);};


  //GLOBAL_SPELL_ID GetSpellKeyAt(POSITION pos){return list.GetKeyAt(pos);};
  //SPELL_ID GetSpellIDAt(POSITION pos){return list.GetIDAt(pos);};
  //CHARACTER_SPELL& GetSpellAt(POSITION pos){return list.GetAtPos(pos);};
  //void SortSpells(void){list.SortSpells();};
        int              LocateCharacterSpell(const SPELL_ID& spellID) const;
  const CHARACTER_SPELL *PeekCharacterSpell  (const SPELL_ID& spellID) const;
        CHARACTER_SPELL *GetCharacterSpell   (const SPELL_ID& spellID);
  const CHARACTER_SPELL *PeekCharacterSpell  (int index) const;
        CHARACTER_SPELL *GetCharacterSpell   (int index);


  spellLimitsType spellLimits;
  DWORD spellPrepTimeNeeded;
  DWORD spellPrepTimeUsed;

  spellBookType() { Clear(); }
  ~spellBookType() { Clear(); }

  int SpellsKnown();
  int SpellsMemorized();

  void Clear();
  spellBookType &operator=(const spellBookType &book);
  BOOL operator==(const spellBookType &book);

  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  bool Import(JReader& jr);
#endif
  void CrossReference(CR_LIST *pCRList) const;
  // index is spell book key, not spell database key
  //
  // Spellbook key is no longer used.  The spellbook
  // data contains a GLOBAL_SPELL_ID and is indexed by
  // GLOBAL_SPELL_ID.

  /*
  BOOL GetSpell           (GLOBAL_SPELL_ID gsID, CHARACTER_SPELL &spell);
  BOOL IsKnown            (GLOBAL_SPELL_ID gsID) const;
  BOOL IsMemorized        (GLOBAL_SPELL_ID gsID) const;
  BOOL isSelected         (GLOBAL_SPELL_ID gsID) const;
  int  GetSelectedCount   (GLOBAL_SPELL_ID gsID) const;
  int  GetMemorizedCount  (GLOBAL_SPELL_ID gsID) const;
  int  GetMemorizedTime   (GLOBAL_SPELL_ID gsID) const;
  int  ResetMemorizedTime (GLOBAL_SPELL_ID gsID);
  int  IncMemorizedTime   (GLOBAL_SPELL_ID gsID, int minuteInc);
  BOOL DecMemorized       (GLOBAL_SPELL_ID gsID, int memorized_count)
  {
    return list.DecMemorized(gsID, memorized_count);
  };
*/
  BOOL FetchCharacterSpell (const SPELL_ID& spellID, CHARACTER_SPELL *pCS) const;
  BOOL IsKnown             (const SPELL_ID& spellID) const;
  BOOL IsMemorized         (const SPELL_ID& spellID) const;
  BOOL isSelected          (const SPELL_ID& spellID) const;
  int  GetSelectedCount    (const SPELL_ID& spellID) const;
  int  GetMemorizedCount   (const SPELL_ID& spellID) const;
  int  GetMemorizedTime    (const SPELL_ID& spellID) const;
  int  ResetMemorizedTime  (const SPELL_ID& spellID);
  int  IncMemorizedTime    (const SPELL_ID& spellID, int minuteInc);
  BOOL DecMemorized        (const SPELL_ID& spellID, int memorized_count)
  {
    return list.DecMemorized(spellID, memorized_count);
  };

  
  void  ResetAllMemorizedTime();
  BOOL  IncAllMemorizedTime(int minuteInc, bool all);
  void  RemoveSpellLevel(int level); 
  //BOOL  CanKnowSpell(WORD CasterClassFlag, WORD SpellClassFlag, WORD CastMask, int SpellLevel);
  //BOOL  CanKnowSpell(//const CLASS_DATA *pClass, 
  //                   const SCHOOL_ID& schoolID,  //Yes
  //                   //const BASECLASS_LIST& spellAllowedBaseclasses, 
  //                   int   SpellLevel);
  //BOOL  KnowSpell(WORD CasterClassFlag, WORD SpellClassFlag, GLOBAL_SPELL_ID SpellDbKey, int level, BOOL known);
  BOOL  KnowSpell(const BASECLASS_ID& baselassID, const SCHOOL_ID& spellSchoolID, const SPELL_ID& spellID, int level, BOOL known);
  BOOL  KnowSpellxxx(const SPELL_DATA *pSpell, BOOL known);
  //BOOL  SelectSpell(GLOBAL_SPELL_ID index, int selected=1);
  BOOL  SelectSpell(const SPELL_ID& spellID, int selected=1);
  void  UnselectIfNotMemorized(void);
  void  UnselectAll(void);
  DWORD CalcRestTime(void);
  DWORD CalcSpellPrepTime(void);
  void  RemoveSpell(int i){list.RemoveSpell(i);};

  //int   GetSortedCount(void){return list.GetSortedCount();};
  int   GetCount(void) const {return list.GetCount();};
};

class SPELL_TARGETING_DATA
{
  int m_MaxTargets;
  int m_MaxRange;
  int m_Width;
  int m_Height;
public:
  int  m_maxRangeX;
  int  m_maxRangeY;
  int  MaxTargets(void){return m_MaxTargets;};
  int  MaxRange(void){return m_MaxRange;};
  int  Width(void){return m_Width;};
  int  Height(void){return m_Height;};
  void MaxTargets(int v){m_MaxTargets=v;};
  void MaxRange(int v){m_MaxRange=v;};
  void Width(int v){m_Width=v;};
  void Height(int v){m_Height=v;};

public:
  SPELL_TARGETING_DATA();
  SPELL_TARGETING_DATA(const SPELL_TARGETING_DATA &src);
  SPELL_TARGETING_DATA &operator=(const SPELL_TARGETING_DATA &src);
  bool operator==(const SPELL_TARGETING_DATA& src) const;
  void Clear();

  int NumTargets() const;
  int NumMapTargets(void) const;
  BOOL STD_AddTarget(int target, double hd, int range, int mapX, int mapY);
  BOOL IsAlreadyTargeted(int target) const;
  BOOL AddMapTarget(int mapx, int mapy);

  BOOL STD_CanAddTarget(int hd=0, int range=0); 
  BOOL AllTargetsChosen();
  BOOL ValidNumTargets();
  BOOL HDLimitReached();
  CString FormatRemainingTargsText();

  TargetingType m_targType;
  BOOL m_SelectingUnits; // vs selecting map location
  BOOL m_canTargetFriend; // can party member be targeted?
  BOOL m_canTargetEnemy;
  BOOL m_IsPartyAligned;
  BOOL m_area;
  double m_MaxHitDice;
  double m_HDTotal;
  int m_MapTargetX;
  int m_MapTargetY;
  CArray<int, int> m_targets;  // In combat, this is a COMBAT_ID;  else a Unique party ID
  MAPLOCATIONLIST m_maptarglist;
};

class ToHitComputation;

struct SAVING_THROW_DATA
{
  SPELL_DATA *pSpell;
  BOOL success;
  double changeResult;
  BOOL   noEffectWhatsoever;
  CHARACTER *pCaster;
  CHARACTER *pTarget;
};


class SPELL_DATA //: public CObject
{
  enum
  {
    //NOT_IN_CAMP     = 0x01,  // flag
    //NOT_IN_COMBAT   = 0x02,  // flag
    IN_CAMP     = 0x01,  // flag
    IN_COMBAT   = 0x02,  // flag
  };
public:
//  DECLARE_SERIAL( SPELL_DATA )
  SPELL_DATA();
  SPELL_DATA(const SPELL_DATA &src) : 
    // ASLs are named "SPELL_DATA_ATTRIBUTES"
	  spell_asl (),
	  temp_asl  (),
    hCastSound(-1),
    hMissileSound(-1),
    hCoverageSound(-1),
    hHitSound(-1),
    hLingerSound(-1),
    specAbs(true)
      { Clear(); *this = src; }
  ~SPELL_DATA() { ClearSounds(); Clear(TRUE); }
  SPELL_DATA &operator=(const SPELL_DATA &src);
  bool operator ==(const SPELL_DATA& src) const;
#ifdef UAFEDITOR
  SPELL_ID SpellID(void) const {SPELL_ID x; x=Name; return x;};
#else
  SPELL_ID SpellID(void) const {SPELL_ID x; x=uniqueName; return x;};
#endif
  void Clear(BOOL ctor=FALSE);
  int GetDataSize(void) const;

  void SetSpellAnimDefaults(PIC_DATA &art, const char *file);
  
  void Serialize(CArchive &ar, double version);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double version);
  void Serialize(CAR &ar, double version);
  void PreCARSerialize(BOOL IsStoring);
  void PostCARSerialize(BOOL IsStoring, double version);
  void CrossReference(CR_LIST *pCRList) const;

  void Save(CAR& car) const;
  void PrepareForRestore(void);
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);

  void PlayCast();
  void PlayMissile();
  void PlayCoverage();
  void PlayHit();
  void PlayLinger();

  void ClearSounds();
  void LoadArt();
  void ClearArt();
  void FreeArt();

  //GLOBAL_SPELL_ID m_gsID;
	CString Name;
#ifdef UAFEngine
  CString uniqueName;  // The fully qualified name for scripts "Bless|Ranger"
#endif
  CString CastSound;
  int hCastSound;
  int hMissileSound;
  int hCoverageSound;
  int hHitSound;
  int hLingerSound;

  // what school this spell belongs to
  // Must not indicate more than one school!
  //     ??Then why is it a mask rather than an int??  PRS 20101210
	//WORD schoolMask; 
  SCHOOL_ID schoolID;
  //WORD CastMask; // which classes are allowed to cast this spell
  BASECLASS_LIST allowedBaseclasses;
#ifdef UAFEDITOR
  void  AddBaseclass(int castMask, int UserFlag, char *name);
#endif


  BOOL IsCumulative; // can cast multiple times?
	BOOL CanTargetFriend;
  BOOL CanTargetEnemy;
private:
  // We know that a BOOL is an int.
  BOOL restrictions; //Combat_Only;
public:
  //BOOL NotInCamp(void) const {return (restrictions & NOT_IN_CAMP) != 0;};
  //BOOL NotInCombat(void) const {return (restrictions & NOT_IN_COMBAT) != 0;};
  //void NotInCamp(BOOL b) {if(b)restrictions |= NOT_IN_CAMP;else restrictions &= ~NOT_IN_CAMP;};
  //void NotInCombat(BOOL b)   {if(b)restrictions |= NOT_IN_COMBAT;  else restrictions &= ~NOT_IN_COMBAT;};
  BOOL InCamp(void) const {return (restrictions & IN_CAMP) != 0;};
  BOOL InCombat(void) const {return (restrictions & IN_COMBAT) != 0;};
  void InCamp(BOOL b) {if(b)restrictions |= IN_CAMP;else restrictions &= ~IN_CAMP;};
  void InCombat(BOOL b)   {if(b)restrictions |= IN_COMBAT;  else restrictions &= ~IN_COMBAT;};
  //BOOL UsableByParty;
  BOOL CanMemorize;
  //BOOL CanScribeAtLevelChange; // always true for clerics, affects MU spells only
  BOOL AllowScribe; 
  //BOOL AutoScribeAtLevelChange; // always true for clerics, affects MU spells only
  BOOL AutoScribe; 
  BOOL CanBeDispelled;
  BOOL Lingers;
  BOOL LingerOnceOnly; // affects target once only, else once per round
	spellSaveVersusType Save_Vs;
	spellSaveEffectType Save_Result;
	spellTargetingType Targeting;
	spellDurationType Duration_Rate;
  spellCastingTimeType Casting_Time_Type;
	long Level;
	long Casting_Time;
  long Cast_Cost;
  long Cast_Priority;
	SPECIAL_ABILITIES specAbs;
  CString RunSpellScripts(LPCSTR scriptName, 
                                 CBRESULT (*fnc)(CBFUNC func, CString *scriptResult, void *pkt), 
                                 void      *pkt,
                                 LPCSTR     comment) const
  {
    return specAbs.RunScripts(scriptName, 
                              fnc, 
                              pkt,
                              comment,
                              ScriptSourceType_Spell,
#ifdef UAFengine
                              uniqueName);
#else
                              Name);
#endif
  };

  
  
  
  
  DICEPLUS Duration; // duration for spell
  DICEPLUS EffectDuration; // duration for spell's effects on target (if different than spell's duration)

  DICEPLUS P1; //Was NumTargets;
  DICEPLUS P2; //Was TargetRange;
  DICEPLUS P3;
  DICEPLUS P4;
  DICEPLUS P5;
  DICEPLUS P6;
  DICEPLUS& TargetRange(void);
  DICEPLUS& TargetQuantity(void);
  DICEPLUS& TargetWidth(void);
  DICEPLUS& TargetHeight(void);
  DICEPLUS& TargetRadius(void);
  DICEPLUS& TargetLength(void);



#ifdef UAFEDITOR
  // These are used temporarily when reading a text file.
  // Until we know the targeting type, we don't know where to 
  // store results of something like:  "Range = 4".
  DICEPLUS  temp_range;
  DICEPLUS  temp_quantity;
  DICEPLUS  temp_height;
  DICEPLUS  temp_width;
  DICEPLUS  temp_length;
  DICEPLUS  temp_number;
  DICEPLUS  temp_radius;
#endif

  mCList< SPELL_EFFECTS_DATA, SPELL_EFFECTS_DATA& > m_EffectedAttributes;
  CString MissileSound;
  CString CoverageSound;
  CString HitSound;
  CString LingerSound;
  CString CastMsg; // what msg is displayed as spell affects the target?
  PIC_DATA CastArt;          // used when spell is cast, along with CastSound
  PIC_DATA MissileArt;       // used in-route to target
  PIC_DATA TargCoverageArt;  // displays over target area when target reached
  PIC_DATA TargHitArt;       // used on each succesfully hit target (each in turn)
  PIC_DATA TargLingerArt;    // used if graphic is displayed for spell duration

  bool CompileScripts();
  bool CompileScript(SPELL_EFFECTS_DATA &src);
  bool ExecuteSpellBeginScript(CHARACTER *pAttacker, 
                               CHARACTER *pTarget,
                               ToHitComputation *pToHitComutation);
  void ExecuteSpellEndScript(CHARACTER *pAttacker, CHARACTER *pTarget) const;
#ifdef SpellInitiationScript
  bool ExecuteSpellInitiationScript(CHARACTER *pAttacker) const;
  void ExecuteSpellTerminationScript(CHARACTER *pAttacker) const;
#endif
  bool CompileBeginScript(void);
  bool CompileEndScript(void) const;
  bool CompileInitiationScript(void) const;
  bool CompileTerminationScript(void) const;
  bool CompileSavingThrowScript(void);
  bool CompileSavingThrowFailedScript(void);
  bool CompileSavingThrowSucceededScript(void);
    
  CString SpellBeginSource;
  CString SpellBeginBinary;
  CString SpellEndSource;
  CString SpellEndBinary;
  
  //Introduced in version 1.0303
  CString SavingThrowSource;           // 1.0303
  CString SavingThrowBinary;           // 1.0303
  CString SavingThrowSucceededSource;  // 1.0303
  CString SavingThrowSucceededBinary;  // 1.0303
  CString SavingThrowFailedSource;     // 1.0303
  CString SavingThrowFailedBinary;     // 1.0303
  //
  CString SpellInitiationSource;       // 2.60
  CString SpellInitiationBinary;       // 2.60
  CString SpellTerminationSource;      // 2.60
  CString SpellTerminationBinary;      // 2.60

  
  POSITION AddEffect(SPELL_EFFECTS_DATA &src);
  void DeleteEffect(SPELL_EFFECTS_DATA &src);
  bool HaveEffectAttr(const SPELL_EFFECTS_DATA &src) const;
  //bool CompileScript(SPELL_EFFECTS_DATA &src);
  CString CompileScript(const CString& script, const char *namePostfix) const;
  bool ExecuteActivationScript(SPELL_EFFECTS_DATA &src);
  int ExecuteModificationScript(SPELL_EFFECTS_DATA &src);
  int ExecuteSavingThrowScript(SAVING_THROW_DATA &src);    
  void ExecuteSavingThrowFailedScript(SAVING_THROW_DATA &src, CString *displayText);    
  void ExecuteSavingThrowSucceededScript(SAVING_THROW_DATA &src, CString *displayText);    
  BOOL CanPerformSpecAb(specialAbilitiesType sa);
  bool CanEnableAffectAttr(const CString &attr) const;

	A_ASLENTRY_L spell_asl;
	A_ASLENTRY_L temp_asl;

//#ifdef UAFEDITOR
//  // Non-serialized member variable to allow editor to 
//  // convert old designms.
//  int preVersionSpellNames_gsID;  //The old GLOBAL_SPELL_ID
//#endif
  int  preSpellNameKey; //See comment at defintion of VersionSaveIDs 
};

class SPELL_DATA_TYPE //: public CObject
{
  //CCriticalSection m_CS;
  mCArray<SPELL_DATA, SPELL_DATA&> SpellData;

public:
  double version;
//  DECLARE_SERIAL( SPELL_DATA_TYPE )
  SPELL_DATA_TYPE() { Clear(); }
  ~SPELL_DATA_TYPE() { Clear(); }
  SPELL_DATA_TYPE& operator =(SPELL_DATA_TYPE& src);
  bool operator ==(SPELL_DATA_TYPE& src);

  void Clear();
  void Serialize(CArchive &ar, double version);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double version);

  void Serialize(CAR &ar, double version);
  void PreCARSerialize(BOOL IsStoring);
  void PostCARSerialize(BOOL IsStoring, double version);

  void Save(CAR& car);
  void Restore(CArchive& ar); // Restore to temporary location
  void Restore(CAR& car); // Restore to temporary location
  void CommitRestore(void);

  //int  GetCount() { CSingleLock sLock(&m_CS, TRUE); return SpellData.GetCount(); }
  int  GetCount() const { return SpellData.GetSize(); }
  int  GetDataSize(void);
  void SaveSounds();
  void SaveArt();


  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Spell,SPELL_ID,spellID,SpellData,SPELL_DATA,SPELL_DATA_TYPE)

  //POSITION GetHeadPosition() { CSingleLock sLock(&m_CS, TRUE); return SpellData.GetHeadPosition(); }

  //      SPELL_DATA *GetSpellData(POSITION pos)        { return &SpellData.GetAtPos(pos); }
  //const SPELL_DATA *PeekSpellData(POSITION pos) const { return &SpellData.PeekAtPos(pos); }
  //const SPELL_DATA *PeekSpell(int i) const { return &(const_cast <SPELL_DATA_TYPE *>(this)->SpellData[i]); }
  //      SPELL_DATA *GetSpell(int i)        { return &SpellData[i]; }
  //const SPELL_DATA *PeekNext (POSITION& pos) const { return &(const_cast <SPELL_DATA_TYPE *>(this)->SpellData.GetNext(pos));};

  //SPELL_DATA& GetNext(POSITION &pos) { CSingleLock sLock(&m_CS, TRUE); return SpellData.GetNext(pos); }
  //GLOBAL_SPELL_ID GetKeyAt(POSITION pos) { CSingleLock sLock(&m_CS, TRUE); return GLOBAL_SPELL_ID(SpellData.GetKeyAt(pos)); }

  //SPELL_ID GetIDAt(POSITION pos) 
  //{ 
  //  CSingleLock sLock(&m_CS, TRUE); 
  //  return SpellData.PeekAtPos(pos).SpellID(); 
  //};

  void ExpandDefaultFilenames();
#ifdef UAFEDITOR
  void CollapseDefaultFilenames();
#endif

  //BOOL  IsValidSpell(const SPELL_ID& spellID);
  DWORD GetNextKey();
  BOOL Validate(const SPELL_DATA &data, FILE *pFile) const;
  BOOL Validate(int index, FILE *pFile) const; // specific spell
  BOOL Validate(FILE *pFile) const; // all spells
  void CrossReference(CR_LIST *pCRList) const;

  void PlayCast(DWORD index);
  void ClearSounds();
  void FreeArt();
  /*
  WORD GetSpellSchool   (GLOBAL_SPELL_ID index) const;
  WORD GetSpellCastMask (GLOBAL_SPELL_ID index) const;
  int GetSpellLevel     (GLOBAL_SPELL_ID index) const;
  int GetSpellCastCost  (GLOBAL_SPELL_ID index) const;
  */
  //WORD GetSpellSchool   (const SPELL_ID& spellID) const;
  SCHOOL_ID GetSpellSchool (const SPELL_ID& spellID) const;
  //WORD GetSpellCastMask    (const SPELL_ID& spellID) const;
  const BASECLASS_LIST *GetSpellAllowedBaseclasses    (const SPELL_ID& spellID) const;
  int GetSpellLevel        (const SPELL_ID& spellID) const;
  int GetSpellCastCost     (const SPELL_ID& spellID) const;
  //CString GetSpellName   (GLOBAL_SPELL_ID gsID) const;
  CString GetSpellName     (const SPELL_ID& spellID) const;
  //BOOL HaveSpell         (GLOBAL_SPELL_ID gsID) const;
  BOOL HaveSpell           (const SPELL_ID& spellID) const;
  // called when loading from scripts, or
  // when adding from editor GUI
  //GLOBAL_SPELL_ID AddSpell(SPELL_DATA &data);
  int AddSpell(SPELL_DATA *pSpell);
  // called when loading binary monster data that
  // was saved with key info
  //GLOBAL_SPELL_ID AddSpellWithCurrKey (SPELL_DATA &data);
  //SPELL_ID AddSpellWithCurrKey (SPELL_DATA &data);
  /*
  BOOL SetSpell                       (GLOBAL_SPELL_ID gsID, SPELL_DATA &spell);
  BOOL RemoveSpell                    (GLOBAL_SPELL_ID gsID);
  SPELL_DATA *GetSpellData            (GLOBAL_SPELL_ID gsID);
  */
  BOOL SetSpell               (const SPELL_ID& spellID, SPELL_DATA &spell);
  BOOL RemoveSpell            (const SPELL_ID& spellID);
  //SPELL_DATA *GetSpell        (const SPELL_ID& spellID);
  //const SPELL_DATA *PeekSpell (const SPELL_ID& spellID) const;
  //int LocateSpell             (const SPELL_ID& spellID) const;

private:
  // No locking... for internal use
  //GLOBAL_SPELL_ID m_FindName (const CString& name) const;
  //SPELL_ID m_FindName (const CString& name) const;
#ifdef UAFEngine
  //GLOBAL_SPELL_ID m_FindUniqueName (const CString& uniqueName) const;
  //SPELL_ID m_FindUniqueName (const CString& uniqueName) const;
#endif

public:
  // With locking...for public use
  //GLOBAL_SPELL_ID FindName(const CString& name) const;
  //SPELL_ID FindName(const CString& name) const;
#ifdef UAFEngine
  //GLOBAL_SPELL_ID FindUniqueName (const CString& uniqueName) const;
  //SPELL_ID FindUniqueName (const CString& uniqueName) const;
#endif


#ifdef UAFEDITOR
  // Special goodies to convert old designs
  SPELL_ID FindPreVersionSpellNamesSpellID(int preVersionSpellNames_gsID) const;
#endif


};

extern SPELL_DATA_TYPE spellData;

struct CHARACTER_SCROLL 
{
	BYTE qty;
	int scroll;
};

#ifdef UAFEngine

#define newMEMORIZE

struct MAX_BY_BASECLASS
{
  BYTE m_maxSelect[MAX_SPELL_LEVEL];
  BYTE m_maxLevel;     // [school][charBaseClass]
};

struct SELECT_COUNTS
{
  SCHOOL_ID m_schoolID;
  mCArray<MAX_BY_BASECLASS, MAX_BY_BASECLASS&> m_maxByBaseclass; 
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(MaxByBaseclass,    /* array data name      */ \
                                  BASECLASS_ID,      /* Search variable type */ \
                                  baseclassID,       /* search variable name */ \
                                  m_maxByBaseclass,  /* array name           */ \
                                  MAX_BY_BASECLASS,  /* array data type      */ \
                                  SELECT_COUNTS)     /* type of *this        */

  BYTE m_currSelect[MAX_SPELL_LEVEL];


  SELECT_COUNTS(void) {Clear();};
  void Clear(void)
  {
    m_maxByBaseclass.RemoveAll();
    memset(m_currSelect,0, sizeof(m_currSelect));
  };
  SELECT_COUNTS& operator =(const SELECT_COUNTS& selectCounts);
  int CurrSelect(int spellLevel) const {return m_currSelect[spellLevel-1];};
  void IncCurrSelectCount(int spellLevel) {m_currSelect[spellLevel-1]++;};
};



struct SCHOOL_DATA
{
  SCHOOL_ID schoolID;
  int maxMemorizePerSpellLevel[MAX_SPELL_LEVEL];

  SCHOOL_DATA(void) {};
  SCHOOL_DATA(const SCHOOL_ID& sID) 
  { 
    schoolID = sID; 
    memset(maxMemorizePerSpellLevel, 0, sizeof(maxMemorizePerSpellLevel));
  };
};


class MEMORIZE_ITEM
{
public:
  int charSpellIndex;
  int spellDataIndex;
  int numMemorized;
  int numSelected;
  int available;    // Number of this spell that can be memorized
};

class MemorizeSummary
{
public:
  mCArray<MEMORIZE_ITEM, MEMORIZE_ITEM&> spells;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(MemorizeItem,    /* array data name      */ \
                                  INTEGER,         /* Search variable type */ \
                                  charSpellIndex,  /* search variable name */ \
                                  spells,          /* array name           */ \
                                  MEMORIZE_ITEM,   /* array data type      */ \
                                  MemorizeSummary) /* type of *this        */

  void Clear(void);
};



struct SPELL_TEXT_LIST 
{
  SPELL_TEXT_LIST() { Clear(); }
  ~SPELL_TEXT_LIST() { Clear(); }

  void Clear();
  int GetCount() const;
  int GetMemorizeCount() const { return memSum.GetMemorizeItemCount();};
  int GetRaceCount(void) const {return races.GetSize();};
  int GetGenderCount(void) const {return genders.GetSize();};
  int GetClassCount(void) const {return classes.GetSize();};
  int GetAlignmentCount(void) const {return alignments.GetSize();};
  int GetEffectCount() const {return spellEffects.GetSize()/2;};
  //int GetSpellIndex(int index) const { return spells[index]; };
  SPELL_ID GetSpellID(int index) const { return spellIDs[index]; };
  int GetSpellIndex(const SPELL_ID& spellID);

  int             GetSpellMemorizeCount       (int index);
  int             GetSpellSelectedCount       (int index);
  //GLOBAL_SPELL_ID GetSpellDatabaseKey         (int index);
  SPELL_ID        GetSpellDatabaseID          (int index);
  //GLOBAL_SPELL_ID GetSpellBookKey             (int index);
  SPELL_ID        GetSpellBookID              (int index);
  SCHOOL_ID       GetSpellSchool              (int index);
  int             GetSpellLevel               (int index);
  int             MaxSelectByLevel            (int level);
  int             MaxSelectBySchoolAndLevel   (int school, int Level);
  void            GetSpellLevelAndSchoolIndex (int index, int *lvl, int *sindex);
  //int             IncSpellSelectedCount       (int index);
  //void            DecSpellSelectedCount       (int index);
  int             IncCurrSelectCount          (const SCHOOL_ID& schoolID, int spellLevel);
  int             DecCurrSelectCount          (const SCHOOL_ID& schoolID, int spellLEvel);

  //void            DecSpellMemorizedCount      (int index);
  void            ToggleSelected              (int index);
  BOOL            IsSelected                  (int index);
  //int  NumSelected();
  BOOL            CanSelect                   (int index);
  BOOL            CanSelectSpell              (int index);
  BOOL            CanSelectAnything(); // return TRUE if MaxSelect[] is non-zero in any slot

  void FillTempleCastSpellListText  (CHARACTER &data, costFactorType type);
  void FillCastSpellListText        (CHARACTER &data, CASTING_ENVIRONMENT ce);
  void FillNewSpellLevelListText    (CHARACTER &data);
  void FillMemorizeSpellListText    (CHARACTER *pChar);
  void FillSpellEffectsListText     (CHARACTER &data, int flags);
  // PRS July 2009void FillSpellListText(CHARACTER &cdata, spellBookType *list, UINT flags, costFactorType type=Normal);
  void FillSpellListText            (CHARACTER &cdata, UINT flags, costFactorType type=Normal);

  //void Add(GLOBAL_SPELL_ID index) { spells.Add(index.m_key); isSelected.Add(FALSE); };

  void Add(const SPELL_ID& spellID) { spellIDs.Add(const_cast <SPELL_ID&>(spellID)); isSelected.Add(FALSE); };
  //void Add(raceType race) {races.Add(race);};
  void Add(const RACE_ID& raceID) {races.Add(const_cast<RACE_ID&>(raceID));};
  void Add(genderType gender) {genders.Add(gender);};
  void Add(alignmentType alignment) {alignments.Add(alignment);};
  //void Add(classType Class) { classes.Add(Class);};
  void Add(const CLASS_ID& classID) { classes.Add(const_cast<CLASS_ID&>(classID));};
  //void Add(DWORD pos)             { spells.Add(pos), isSelected.Add(FALSE); };
  //BOOL Have(DWORD index);
  BOOL Have(const SPELL_ID& spellID);

  char *GetSpellLevelText(int index);
  char *GetSpellSelectedText(int index);
  char *GetSpellMemorizedText(int index);
  char *GetSpellAvailableText(int index);
  char *GetSpellSchoolText(int index);
  char *GetSpellCostText(int index);
  char *GetSpellNameText(int index);
  char *GetSpellEffectText(int index);
  char *GetSpellEffectLevelText(int index);
  char *GetSpellText(int index);
  //raceType      GetRace(int index){return races[index];};
  RACE_ID       GetRace(int index){return races[index];};
  genderType    GetGender(int index){return genders[index];};
  alignmentType GetAlignment(int index) {return alignments[index];};
  //classType     GetClass(int index) { return classes[index];};
  CLASS_ID      GetClass(int index) { return classes[index];};

  void FormatSpellListTitle(const char *EmptySpellTitle=NULL);

  BOOL UseSpellEffectList()     const { return m_flags & CHAR_VIEW_DISPLAY;};
  BOOL UseTempleCastLayout()    const { return (m_flags & TEMPLE_CAST_FORMAT_FLAG); }
  BOOL UseCastLayout()          const { return (m_flags & CAST_FORMAT_FLAG); }
  BOOL UseMemorizeLayout()      const { return (m_flags & MEMORIZE_FORMAT_FLAG); }
  BOOL UseRaceLayout()          const { return (m_flags & RACE_FORMAT_FLAG);};
  BOOL UseClassLayout()         const { return (m_flags & CLASS_FORMAT_FLAG);};
  BOOL UseNewSpellLevelLayout() const { return (m_flags & NEW_SPELL_LEVEL_FORMAT_FLAG); }
  BOOL UseArbitraryTextLayout() const { return (m_flags & ARBITRARY_TEXT_FORMAT_FLAG); }
  
  BOOL UseDesignLayout() const { return (m_flags & SHOW_DESIGN_FLAG); }
  BOOL UseLevel() const { return ((m_flags & LEVEL_FLAG) && (!UseDesignLayout()) && (!UseMemorizeLayout()) && (!UseNewSpellLevelLayout()) ); }
  BOOL UseCost()  const { return ((m_flags & COST_FLAG) && (!UseDesignLayout()) && (!UseMemorizeLayout()) && (!UseNewSpellLevelLayout()) ); }
  
  BOOL UsesSpellBookPos() const { return ((UseTempleCastLayout())||(UseCastLayout())||(UseMemorizeLayout())); }
  BOOL UsesSpellDbKey() const { return ((!UsesSpellBookPos())||(UseNewSpellLevelLayout())); }
  BOOL UsesSelectionArrays() const { return ((UseNewSpellLevelLayout())||(UseMemorizeLayout())); }

  costFactorType CostFactor;
  DWORD m_flags;

	char temp[MAX_SPELL_NAME+50];
  char text[300];

  CHARACTER *m_CharData;
  //int m_SpellLevelFilter;
  //int m_school;
  //WORD spellClasses;
  BASECLASS_LIST baseclassList;
  CString memorizedText;
  CString title;
  CString arbitraryText[5];
  void ClearArbitraryText(void);

  // this will keep track of how many spells are
  // currently selected and how many can be selected,
  // and is used by:
  //
  // FillNewSpellLevelListText:  SpellDbKey
  // FillMemorizeSpellListText:  character's spell book POS
  //

#ifdef newMEMORIZE
  mCArray<SELECT_COUNTS, SELECT_COUNTS&> selectCounts;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(SelectCounts,    /* array data name      */ \
                                  SCHOOL_ID,       /* Search variable type */ \
                                  schoolID,        /* search variable name */ \
                                  selectCounts,    /* array name           */ \
                                  SELECT_COUNTS,   /* array data type      */ \
                                  SPELL_TEXT_LIST) /* type of *this        */



  int MaxSelect (const SCHOOL_ID& schoolID, int level) const;  // For all baseclasses
  int MaxSelect (const SCHOOL_ID& schoolID, int level, const BASECLASS_ID& baseclassID) const;
  int CurrSelect(const SCHOOL_ID& schoolID, int level) const;
  int IncreaseSpellSelectedCount(int index, int num=1);
  int IncreaseSpellMemorizedCount(int index, int num=1);
#else
  BYTE MaxLevel[NumBaseClass];     // [characterClass];  Maximum spell level for each baseclass
  BYTE CurrSelect[MAX_SPELL_LEVEL][NumBaseClass];
  BYTE MaxSelect[MAX_SPELL_LEVEL][NumBaseClass];
#endif
  CArray<BOOL, BOOL> isSelected;

  // this will contain either the SpellDbKey,
  // or a POSITION for access to character's spell book
  //
  // FillNewSpellLevelListText:  SpellDbKey
  // FillCastSpellListText:      character's spell book POS
  // FillTempleCastSpellListText character's spell book POS
  // FillMemorizeSpellListText:  character's spell book POS
  // FillSpellEffectsListText:   SpellDbKey
  // FillSpellListText:          SpellDbKey
  //CArray<DWORD, DWORD> spells;
  CArray<SPELL_ID, SPELL_ID&> spellIDs;
  CArray<CString, CString&> spellEffects;
  //CArray<raceType,raceType> races;
  CArray<RACE_ID, RACE_ID&> races;
  CArray<genderType, genderType> genders;
  CArray<alignmentType, alignmentType> alignments;
  //CArray<classType, classType> classes;
  CArray<CLASS_ID, CLASS_ID&> classes;
  private:
  mCArray<int, int> integers;
  public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Integer,         /* array data name      */ \
                                  INTEGER,         /* Search variable type */ \
                                  integer,         /* search variable name */ \
                                  integers,        /* array name           */ \
                                  int,             /* array data type      */ \
                                  SPELL_TEXT_LIST) /* type of *this        */

  CArray<POSITION, POSITION> POSITIONs;
  MemorizeSummary memSum;
};

struct PENDING_SPELL
{
  PENDING_SPELL() { Clear(); }
  ~PENDING_SPELL() { Clear(); }
  void Clear() 
  { 
    //gsID.Clear();
    //gsIDsecondary.Clear();
    spellID.Clear();
    spellIDSecondary.Clear();
    waitUntil=0;caster.Clear(); 
  };
  PENDING_SPELL &operator=(const PENDING_SPELL &src)
  {
    key = src.key; // What sense does this make? PRS
    //gsID=src.gsID;
    //gsIDsecondary = src.gsIDsecondary;
    spellID = src.spellID;
    spellIDSecondary = src.spellIDSecondary;
    caster=src.caster;
    waitUntil=src.waitUntil;
    flags=src.flags;
    return *this;
  }

  int             key;        // key that id's this spell
  //GLOBAL_SPELL_ID gsID; 
  SPELL_ID spellID;
  //GLOBAL_SPELL_ID gsIDsecondary;
  SPELL_ID spellIDSecondary;
  ActorType       caster; // context info to id caster
  DWORD           waitUntil;  // clock time when spell is activated
  DWORD           flags;
};

struct SPELL_LINGER_DATA
{
  SPELL_LINGER_DATA() { Clear(); }
  
  void Clear()
  {
    OnceOnly=FALSE;
    mapdata.RemoveAll();
    affectedtargs.RemoveAll();
  }

  void Serialize(CAR &ar, double ver)
  {
    if (ar.IsStoring())
    {
      ar << OnceOnly;
      ar << mapdata.GetCount();
      POSITION pos = mapdata.GetHeadPosition();
      while (pos != NULL)
      {
        POINT pt = mapdata.GetAt(pos);
        
        ar << pt.x;
        ar << pt.y;
        
        mapdata.GetNext(pos);
      }
      
      ar << affectedtargs.GetCount();
      pos = affectedtargs.GetHeadPosition();
      while (pos != NULL)
      {
        int dude = affectedtargs.GetAt(pos);        
        ar << dude;        
        affectedtargs.GetNext(pos);
      }
    }
    else
    {
      ar >> OnceOnly;
      int count;
      int i;
      int dude;
      POINT pt;
      ar >> count;
      for (i=0;i<count;i++)
      {
        ar >> pt.x;
        ar >> pt.y;
        mapdata.AddTail(pt);
      }      
      ar >> count;
      for (i=0;i<count;i++)
      {
        ar >> dude;
        affectedtargs.AddTail(dude);
      }      
    }
  }
  void Serialize(CArchive &ar, double ver)
  {
    if (ar.IsStoring())
    {
      ar << OnceOnly;
      ar << mapdata.GetCount();
      POSITION pos = mapdata.GetHeadPosition();
      while (pos != NULL)
      {
        POINT pt = mapdata.GetAt(pos);
        
        ar << pt.x;
        ar << pt.y;

        mapdata.GetNext(pos);
      }

      ar << affectedtargs.GetCount();
      pos = affectedtargs.GetHeadPosition();
      while (pos != NULL)
      {
        int dude = affectedtargs.GetAt(pos);        
        ar << dude;        
        affectedtargs.GetNext(pos);
      }
    }
    else
    {
      ar >> OnceOnly;
      int count;
      int i;
      int dude;
      POINT pt;
      ar >> count;
      for (i=0;i<count;i++)
      {
        ar >> pt.x;
        ar >> pt.y;
        mapdata.AddTail(pt);
      }      
      ar >> count;
      for (i=0;i<count;i++)
      {
        ar >> dude;
        affectedtargs.AddTail(dude);
      }      
    }
  }
  SPELL_LINGER_DATA &operator=(const SPELL_LINGER_DATA &src)
  {
    if (this==&src) return *this;
    
    OnceOnly=src.OnceOnly;
    
    mapdata.RemoveAll();
    POSITION pos = src.mapdata.GetHeadPosition();
    while (pos!=NULL)
    {
      POINT pt = src.mapdata.GetNext(pos);
      mapdata.AddTail( pt );
    }

    affectedtargs.RemoveAll();
    pos = src.affectedtargs.GetHeadPosition();
    while (pos!=NULL)
    {
      int dude = src.affectedtargs.GetNext(pos);
      affectedtargs.AddTail( dude ); 
    }
    
    return *this;
  }

  void AddTarget(int targ)
  {
    POSITION pos = affectedtargs.GetHeadPosition();
    while (pos != NULL)
    {
      if (affectedtargs.GetNext(pos) == targ)
        return;
    }
    affectedtargs.AddTail(targ);
    return;    
  }

  BOOL EligibleTarget(int targ) const
  {
    POSITION pos = affectedtargs.GetHeadPosition();
    while (pos != NULL)
    {
      if (affectedtargs.GetAt(pos) == targ)
        return (!OnceOnly);
      affectedtargs.GetNext(pos);
    }
    //affectedtargs.AddTail(targ);
    return TRUE;
  }

  BOOL AffectsTarget(int targ, int mapx, int mapy, int width, int height) const
  {
    BOOL found=FALSE;
    POSITION pos = mapdata.GetHeadPosition();
    while ((pos != NULL) && (!found))
    {
      POINT pt = mapdata.GetAt(pos);
      if (    (pt.x >= mapx)
           && (pt.y >= mapy)
           && (pt.x < mapx + width)
           && (pt.y < mapy + height)
         )
      {
        found = TRUE;
      };
      mapdata.GetNext(pos);
    }

    // spell not affecting specified target
    if (!found)
      return FALSE;

    // spell is affecting specified target,
    // so check for target eligibility.
    return (EligibleTarget(targ));
  }

  BOOL BlocksCombatant(int mapx, int mapy, COMBATANT *pCombatantLingers, SPELL_DATA *pSpell) const;

  BOOL OnceOnly;
  MAPLOCATIONLIST mapdata;
  MAPTARGETLIST affectedtargs;
};

struct ACTIVE_SPELL
{
  ACTIVE_SPELL() { Clear(); }
  ~ACTIVE_SPELL() { Clear(); }
  void Clear() 
  { 
    key=-1;
    //spellKey.Clear();
    spellID.Clear();
    caster.Clear();
    target.Clear();
    StopTime=0;
    CountTime=0;
    Lingers=FALSE;
    m_XscriptProcessed = true;
    casterLevel = -1;
    lingerData.Clear(); 
  }
  void Serialize(CAR &ar, double ver)
  {
    if (ar.IsStoring())
    {
      ar << key;
      caster.Serialize(ar, ver);
      target.Serialize(ar, ver);
      //spellKey.Serialize(ar, ver); //ar << spellKey;
      ar << spellID; //ar << spellKey;
      ar << StopTime;
      ar << CountTime;
      ar << Lingers;
      ar << casterLevel;

      lingerData.Serialize(ar, ver);
    }
    else
    {
      ar >> key;
      caster.Serialize(ar, ver);
      if (ver >= 1.0303)
      {
        target.Serialize(ar, ver);
      }
      else
      {
        target.Clear();
      };
      //spellKey.Serialize(ar, ver); //bar >> spellKey;
      ar >> spellID; //bar >> spellKey;
      ar >> StopTime;
      ar >> CountTime;

      if (ver >= _VERSION_0906_)
      {
        ar >> Lingers;
        lingerData.Serialize(ar, ver);      
      }
      if (ver >= 0.975000)
      {
        ar >> casterLevel;
      }
    }
  }
  void Serialize(CArchive &ar, double ver)
  {
    if (ar.IsStoring())
    {
      die(0x41b12);
      //ar << key;
      //caster.Serialize(ar, ver);
      ////spellKey.Serialize(ar, ver); //ar << spellKey;
      //ar << spellID; //ar << spellKey;
      //ar << StopTime;
      //ar << CountTime;
      //ar << Lingers;
      //ar << casterLevel;
      //
      //lingerData.Serialize(ar, ver);
    }
    else
    {
      ar >> key;
      caster.Serialize(ar, ver);
      target.Clear();
      //spellKey.Serialize(ar, ver); //ar >> spellKey;
      ar >> spellID; //ar >> spellKey;
      ar >> StopTime;
      ar >> CountTime;

      if (ver >= _VERSION_0906_)
      {
        ar >> Lingers;
        if (ver >= 0.975000)
        {
          ar >> casterLevel;
        }
        lingerData.Serialize(ar, ver);
      }
    }    
  }
  ACTIVE_SPELL &operator=(const ACTIVE_SPELL &src)
  {
    if (this==&src) return *this;
    key=src.key;
    caster=src.caster;
    target=src.target;
    //spellKey=src.spellKey;
    spellID=src.spellID;
    StopTime = src.StopTime;
    CountTime = src.CountTime;
    lingerData = src.lingerData;
    Lingers=src.Lingers;
    casterLevel = src.casterLevel;
    return *this;
  }
  BOOL LingerSpellAffectsTarget(int targ, int mapx, int mapy, int width, int height) const
  {
    if (!Lingers) return FALSE;
    return (lingerData.AffectsTarget(targ, mapx, mapy, width, height));
  }

  BOOL LingerSpellBlocksCombatant(int mapx, int mapy, COMBATANT *pCombatant) const;
    
  int key; // key that id's this active spell in activeSpellList
  //GLOBAL_SPELL_ID spellKey; // spell db key
  SPELL_ID spellID;
  ActorType caster; // context info that id's caster
  ActorType target;
  DWORD StopTime;
  DWORD CountTime;
  BOOL Lingers;
  SPELL_LINGER_DATA lingerData;
  bool m_XscriptProcessed;
  int  casterLevel;  // At the time the spell was cast
};

class ACTIVE_SPELL_LIST
{
public:
  ACTIVE_SPELL_LIST() { xClear(); }
  ~ACTIVE_SPELL_LIST() { xClear(); }

  void xClear();
  //int GetCount() { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetCount(); }
  int GetCount() {/* CSingleLock sLock(&m_CS, TRUE);*/ return m_spellList.GetCount(); }
  POSITION GetHeadPosition() const {return m_spellList.GetHeadPosition(); }
        //ACTIVE_SPELL& GetAtPos(POSITION pos)  { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetAtPos(pos); }
        ACTIVE_SPELL& GetAtPos(POSITION pos)  { /*CSingleLock sLock(&m_CS, TRUE);*/ return m_spellList.GetAtPos(pos); }
  const ACTIVE_SPELL& PeekAtPos(POSITION pos) const {return m_spellList.PeekAtPos(pos); }
  //ACTIVE_SPELL& GetNext(POSITION &pos) { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetNext(pos); }
  ACTIVE_SPELL& GetNext(POSITION &pos) { return m_spellList.GetNext(pos); }
  const ACTIVE_SPELL& PeekNext(POSITION &pos) const {return *m_spellList.PeekNext(pos); }
  //int GetKeyAt(POSITION pos) { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetKeyAt(pos); }
  int GetKeyAt(POSITION pos) { return m_spellList.GetKeyAt(pos); }
  ACTIVE_SPELL_LIST&operator=(ACTIVE_SPELL_LIST&src);

  // called when loading from scripts, or
  // when adding from editor GUI
  int xAdd(ACTIVE_SPELL &data);
  // called when loading binary data that
  // was saved with key info
  int xAddWithCurrKey(ACTIVE_SPELL &data);
  BOOL Set(int index, ACTIVE_SPELL &spell);
  BOOL Remove(int index, const SPELL_DATA *pSpell);
  ACTIVE_SPELL *Get(int index);
  int GetNextKey();

  void Serialize(CAR &ar, double ver);
  void Serialize(CArchive &ar, double ver);
  void PreSerialize(BOOL IsStoring) { }
  void PostSerialize(BOOL IsStoring) { }

  void ProcessTimeSensitiveData(int roundInc);
  void DeactivateActiveSpell(int index, const SPELL_DATA *pSpell);

  BOOL LingerSpellAffectsTarget(int targ, int mapx, int mapy, int width, int height)
  {
//    CSingleLock sLock(&m_CS, TRUE);
    POSITION pos = GetHeadPosition();
    while (pos != NULL)
    {
      if (PeekAtPos(pos).LingerSpellAffectsTarget(targ,mapx,mapy,width,height))
        return TRUE;
      GetNext(pos);
    }
    return FALSE;
  }    

  BOOL LingerSpellBlocksCombatant(int mapx, int mapy, COMBATANT *pCombatant) const;

  BOOL ActivateLingerSpellsOnTarget(int targ, int mapx, int mapy, int width, int height);
  void RemoveLingerSpells();
  CString RunSEScripts(int x, int y, const CString& scriptName, LPCSTR comment);
#ifdef SpellInitiationScript
  void ExitCombat(void);  // Convert Actor references from combat to non-combat
#endif

private:
  CCriticalSection m_CS;
  OrderedQueue<ACTIVE_SPELL, MAX_ACTIVE_SPELLS> m_spellList;

  ACTIVE_SPELL_LIST(ACTIVE_SPELL_LIST &src); // not enabled
};

struct PENDING_SPELL_LIST
{
public:
  PENDING_SPELL_LIST() { Clear(); }
  ~PENDING_SPELL_LIST() { Clear(); }

  //void Clear() { CSingleLock sLock(&m_CS, TRUE); m_spells.RemoveAll(); }
  void Clear() { m_spells.RemoveAll(); }
  //int GetCount() { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetCount(); }
  int GetCount() { return m_spells.GetCount(); }


  //POSITION GetHeadPosition() { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetHeadPosition(); }
  POSITION GetHeadPosition() { return m_spells.GetHeadPosition(); }
        PENDING_SPELL&  GetAtPos (POSITION pos)       { return m_spells.GetAtPos(pos); }
  const PENDING_SPELL&  PeekAtPos(POSITION pos) const { return m_spells.PeekAtPos(pos); }


  //void            RemoveAt(POSITION pos) { CSingleLock sLock(&m_CS, TRUE); m_spells.RemoveAt(pos); }
  void            RemoveAt(POSITION pos) {m_spells.RemoveAt(pos); }
  //PENDING_SPELL&  GetNext(POSITION &pos) { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetNext(pos); }
  PENDING_SPELL&  GetNext(POSITION &pos) { return m_spells.GetNext(pos); }
  //int             GetKeyAt(POSITION pos) { CSingleLock sLock(&m_CS, TRUE); return m_spells.GetKeyAt(pos); }
  int             GetKeyAt(POSITION pos) { return m_spells.GetKeyAt(pos); }

  int Add(PENDING_SPELL &data);
  BOOL Set(int index, PENDING_SPELL &spell);
  BOOL Remove(int index);
  PENDING_SPELL *Get(int index);

  // no need to save pending spells
  //void Serialize(CAR &ar);
  //void Serialize(CArchive &ar);
  //void PreSerialize(BOOL IsStoring) { }
  //void PostSerialize(BOOL IsStoring) { }

  // initiative is for combat only
  bool ProcessTimeSensitiveData(int roundInc, int currInitiative=-1);

private:
  CCriticalSection m_CS;
  OrderedQueue<PENDING_SPELL, MAX_PENDING_SPELLS> m_spells;

  int GetNextKey();

  PENDING_SPELL_LIST(PENDING_SPELL_LIST &src); // not enabled
  PENDING_SPELL_LIST &operator=(PENDING_SPELL_LIST &src); // not enabled
};

extern PENDING_SPELL_LIST pendingSpellList;
extern ACTIVE_SPELL_LIST activeSpellList;

#endif // UAFEngine

#endif