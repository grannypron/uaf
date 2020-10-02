/******************************************************************************
* Filename: GameRules.h
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
#ifndef __GAMERULES_H__
#define __GAMERULES_H__

//#include "char.h"
#include "class.h"
#include "gameevent.h"

/*
Valid Classes
  Humans cannot be multi-classed
     Cleric
     Fighter
     MU
     Thief
     Ranger
     Druid
     Paladin
  Dwarf	
    Cleric
    Fighter
    Thief
    Fighter/Thief	
    Fighter/Cleric
  Halfling
    Cleric
    Fighter
    Thief
    Fighter/Thief
  Elf	
    Cleric
    Fighter
    MU
    Thief
    Ranger
    Fighter/MU
    Fighter/Thief
    Fighter/MU/Thief
    MU/Thief
  Half-elf
    Cleric
    Druid
    Fighter
    Ranger
    MU
    Thief
    Cleric/Fighter
    Fighter/Druid
    Fighter/MU/Cleric
    Fighter/MU/Druid
    Cleric/Ranger
**->    Druid/Ranger ** not implemented anywhere!
    Cleric/MU
    Druid/MU	
    Fighter/MU   
    Fighter/Thief
    Fighter/MU/Thief   
    Thief/MU    
  Gnome	
    Fighter
    Thief
    Cleric
    MU
    Fighter/Thief
    Cleric/MU
*/

const int NUM_RACE_TYPES = 6;
const int NUM_GENDER_TYPES = 2;
const int NUM_ALIGNMENT_TYPES = 9;
const int NUM_CLASS_TYPES = 19;
//const int NUM_UNDEAD_TYPES = 14;
const int NUM_CHAR_STATUS_TYPES = 10;

#ifdef UAFEDITOR
enum raceType 
{ 
  Human=0, 
  Elf=1, 
  Dwarf=2, 
  Gnome=3, 
  HalfElf=4, 
  Halfling=5, 
  raceUnknown=6 
};
#endif
enum genderType 
{ 
  Male=0, 
  Female=1,
  Bishop=2,
};
const int NUM_GENTER_TYPES = 3;

enum alignmentType 
{ 
  LawfulGood=0,    
  NeutralGood=1, 
  ChaoticGood=2, 
  LawfulNeutral=3, 
  TrueNeutral=4, 
  ChaoticNeutral=5,  
  LawfulEvil=6,    
  NeutralEvil=7, 
  ChaoticEvil=8 
};

#ifdef UAFEDITOR
enum classType_Editor 
{ 
  Fighter=0, 
  Cleric=1, 
  Ranger=2, 
  Paladin=3, 
  MagicUser=4, 
  Thief=5, 
  Druid=6,
	C_F=7, 
  C_F_MU=8, 
  C_R=9, 
  C_MU=10, 
  C_T=11, 
  F_MU=12, 
  F_T=13, 
  F_MU_T=14, 
  MU_T=15, 
  F_D=16, 
  D_MU=17, 
  F_MU_D=18, 
  BogusClass=19, 
  classUnknown=20 
};
#endif
//enum undeadType { NotUndead, Skeleton, Zombie, Ghoul, Shadow, Wight, Ghast, Wraith, Mummy,
//						Spectre, Vampire, Ghost, Lich, Special };
enum charStatusType 
{ 
  Okay,        // 0
  Unconscious, // 1  less than one hp, but not losing hp (dying)
  Dead,        // 2  <= -10 hp
  Fled,        // 3 
  Petrified,   // 4
  Gone,        // 5
  Animated,    // 6
  TempGone,    // 7
	Running,     // 8
  Dying        // 9 less than one hp, and losing 1 hp per round (combat only)
};


enum creatureSizeType;

extern int START_AGE;
extern int AGE_RANGE;

const int MAX_MOVE = 12;

const int FIGHTER_HIT_DIE = 10;
const int CLERIC_HIT_DIE  = 8;
const int THIEF_HIT_DIE = 6;
const int MAGIC_USER_HIT_DIE = 4;
const int RANGER_HIT_DIE = 8;
const int DRUID_HIT_DIE = CLERIC_HIT_DIE;
const int PALADIN_HIT_DIE = FIGHTER_HIT_DIE;

const int SKILL_DIE = 6;

const int START_PLATINUM  = 200;
const int START_GOLD      =   0;
const int START_ELECTRUM  =   0;
const int START_SILVER    =   0;
const int START_COPPER    =   0;

inline int GetMaxCharMovement() { return MAX_MOVE; }
inline int GetCharStartingAge() { return START_AGE; }
inline int GetCharStartingAgeRanger() { return AGE_RANGE; }


void determineStrengthProperties(int strength, int strengthMod,
                                        int &hitBonus, int &dmgBonus, BYTE &openDoor, 
                                        BYTE &openMagicDoor, BYTE &BB_LG);

int getFighterLevel(int exp); // determine level for given exp pts
int getPaladinLevel(int exp);
int getRangerLevel(int exp);
int getClericLevel(int exp);
int getDruidLevel(int exp);
int getMagicUserLevel(int exp);
int getThiefLevel(int exp);

#ifdef UAFEDITOR

int GetMaxClericExpForLevel(int level);
int GetMaxDruidExpForLevel(int level);
int GetMaxFighterExpForLevel(int level);
int GetMaxRangerExpForLevel(int level);
int GetMaxPaladinExpForLevel(int level);
int GetMaxMagicUserExpForLevel(int level);
int GetMaxThiefExpForLevel(int level);

int GetMinClericExpForLevel(int level);
int GetMinDruidExpForLevel(int level);
int GetMinFighterExpForLevel(int level);
int GetMinRangerExpForLevel(int level);
int GetMinPaladinExpForLevel(int level);
int GetMinMagicUserExpForLevel(int level);
int GetMinThiefExpForLevel(int level);
#endif


//void getFighterHitDice(int level, int& dice, int& nbr, int& constant);
//void getPaladinHitDice(int level, int& dice, int& nbr, int& constant);
//void getRangerHitDice(int level, int& dice, int& nbr, int& constant);
//void getMagicUserHitDice(int level, int& dice, int& nbr, int& constant);
//void getThiefHitDice(int level, int& dice, int& nbr, int& constant);
//void getClericHitDice(int level, int& dice, int& nbr, int& constant);
//void getDruidHitDice(int level, int& dice, int& nbr, int& constant);
//void getCharClassHitDice(WORD ClassFlag, int level, int& dice, int& nbr, int& constant);
void GetBaseclassHitDice(const BASECLASS_ID &baseclassID, 
                         int level,
                         int& dice,
                         int& nbr,
                         int& constant);


#ifdef UAFEDITOR 
BOOL isDruid(classType_Editor type);
BOOL isCleric(classType_Editor type);
BOOL isFighter(classType_Editor type);
BOOL isRanger(classType_Editor type);
BOOL isPaladin(classType_Editor type);
BOOL isMagicUser(classType_Editor type);
BOOL isThief(classType_Editor type);
BOOL isMultiClass(classType_Editor type);
int  numClasses(classType_Editor type);
BOOL CanCastMagicUserSpells(classType_Editor CharClass);
BOOL CanCastClericSpells(classType_Editor CharClass);
#endif


//BOOL isDruid               (const CLASS_ID& classID);
//BOOL isCleric              (const CLASS_ID& classID);
//BOOL isFighter             (const CLASS_ID& classID);
//BOOL isRanger              (const CLASS_ID& classID);
//BOOL isPaladin             (const CLASS_ID& classID);
//BOOL isMagicUser           (const CLASS_ID& classID);
//BOOL isThief               (const CLASS_ID& classID);
//BOOL isMultiClass          (const CLASS_ID& classID);
//int  NumClasses            (const CLASS_ID& classID);
BOOL CanCastMagicUserSpells(const CLASS_ID& classID);
BOOL CanCastClericSpells   (const CLASS_ID& classID);

int determineHitDiceBonus(BYTE Score, BOOL IsFighter);
//BYTE rollSkillDie(abilityType type);
//BYTE rollSkillDie(const char *);
BYTE rollSkillDie(const CString& abilityName) ;
//void CheckNewCharClassScores(classType charClass, CHARSKILLDATA &scores);
void CheckNewCharClassScores(const CLASS_ID& classID, CHARSKILLDATA &scores);
//void CheckNewCharRaceScores(raceType race, CHARSKILLDATA &scores);
void CheckNewCharRaceScores(const RACE_ID& raceID, CHARSKILLDATA &scores);
//int determineCharStartAge(raceType race);
int determineCharStartAge(const RACE_ID& raceID);
//int determineCharMaxAge(raceType race);
int determineCharMaxAge(const RACE_ID& raceID);
int DetermineNormalEncumbrance(int str, int strMod);

CString GetGenderName(genderType type);
CString GetAlignmentName(alignmentType align);

#define UNDEAD_NOEFFECT (0)
#define UNDEAD_TURNED   (-1)
#define UNDEAD_DESTROY  (-2)
// See #defines above for return values.
// If value in range 1-20 then need that roll or higher
// on d20 to succeed in turning.
enum eventTurnUndeadModType;
int GetUndeadTurnValueByHD(int level, int undeadHD, eventTurnUndeadModType modifier);
//int GetUndeadTurnValue(int level, undeadType type, eventTurnUndeadModType modifier);
int GetUndeadTurnValue(int level, int undeadLevel, eventTurnUndeadModType modifier);


enum spellTargetingType { Self, SelectedByCount, WholeParty, TouchedTargets, AreaCircle, SelectByHitDice,
                          AreaLinePickStart, AreaLinePickEnd, AreaSquare, AreaCone };
const int NUM_SPELL_TARGETING_TYPES = 10;


enum TargetingType {  NowSelf, 
                      NowSelectedByCount, 
                      NowWholeParty, 
                      NowTouchedTargets, 
                      NowAreaCircle, 
                      NowSelectByHitDice,
                      NowAreaLinePickStart,
                      NowAreaLinePickEnd,
                      NowAreaSquare, 
                      NowAreaCone,
                      LingerSelf, 
                      LingerSelectedByCount, 
                      LingerWholeParty, 
                      LingerTouchedTargets, 
                      LingerAreaCircle, 
                      LingerSelectByHitDice,
                      LingerAreaLinePickStart,
                      LingerAreaLinePickEnd,
                      LingerAreaSquare,
                      LingerAreaCone };



enum spellSaveEffectType { NoSave, SaveNegates, SaveForHalf, UseTHAC0 };
const int NUM_SAVE_EFFECT_TYPES = 4;

enum spellSaveVersusType { ParPoiDM, PetPoly, RodStaffWand, Sp, BreathWeapon };
const int  NUM_SAVE_VERSUS_TYPES = 5;

enum spellDurationType { inRounds, byDamageTaken, inHours, inDays, Permanent, byNbrAttacks };
const int NUM_SPELL_DURATION_TYPES = 6;

enum spellCastingTimeType { sctImmediate, sctInitiative, sctRounds, sctTurns };
const int NUM_SPELL_CASTING_TIME_TYPES = 4;

#ifdef UAFEDITOR
enum MagicSchool
{
  MagicUserSchool  = 0,
  ClericSchool     = 1,
  MAX_SPELL_SCHOOL = 2

};
#endif

/*
struct SPELL_LIMITS
{
  SPELL_LIMITS(void) 
  {
    PrimeScore=0; 
    maxSpellCount = 0;
    maxSpellLevel = 0;
    memset(numSpellsKnown,0,sizeof(numSpellsKnown));
    memset(maxMemorizeCount,0,sizeof(maxMemorizeCount));
  };
  void Serialize(CAR& car);
  void Serialize(CArchive& ar);
  SCHOOL_ID    schoolID;
  // Replaces m_spellClasses.

  BYTE         PrimeScore;   
  // Replaces PrimeScore[NumBaseClasses]

  BYTE         numSpellsKnown[MAX_SPELL_LEVEL];
  BYTE         totalSpellsKnown; // Total over all spell levels;
  BYTE         maxSpellLevel;    // At current primeScore;
  BYTE         maxSpellCount;    // At current primeScore.
  BYTE         maxMemorizeCount[MAX_SPELL_LEVEL]; //current prime and baseclass levels
  //                                                     
  // replaces numSpellsKnown[NumBaseClass][MAX_SPELL_LEVEL];

  BOOL operator == (const SPELL_LIMITS *pSpellLimits) const;

};
*/

struct spellLimitsType
{
  //WORD m_spellClasses; // which spell class can this dude use? (only one!)
  BOOL UseLimits; // some NPC types get unlimited spells
  //BYTE PrimeScore[NumBaseClass];
  //BYTE  numSpellsKnown[NumBaseClass][MAX_SPELL_LEVEL];

  // The m_spellClasses had one bit for each possible baseclass.
  // The PrimeScore had a byte for each possible baseclass
  // The numSpellsKnown had 9 bytes (MAX_SPELL_LEVEL) for each possible baseclass.
  // We need to generalize this to additional baseclasses.
/*
  CArray<SPELL_LIMITS, SPELL_LIMITS&> m_spellLimits;  // One for each baseclass.

  DEFINE_CARRAY_ACCESS_FUNCTIONS(SpellLimits,     \
                                 SCHOOL_ID,       \
                                 schoolID,        \
                                 m_spellLimits,   \
                                 SPELL_LIMITS,    \
                                 spellLimitsType) 

  int GetSchoolCount() const {return m_spellLimits.GetSize();};
  int MaxSpellLevel(const SCHOOL_ID& schoolID);
  int MaxSpellCount(const SCHOOL_ID& schoolID);

  spellLimitsType() { Clear(); }
  ~spellLimitsType() { Clear(); }
*/  
  spellLimitsType &operator=(const spellLimitsType &src);
  BOOL operator==(const spellLimitsType &src) const;

  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
#ifdef UAFEDITOR
  void Export(JWriter& jw);
  bool Import(JReader& jr);
#endif
  void SetToMaxLimits();
/*
  //BOOL ValidSpellClass(WORD ClassFlag);
  BOOL ValidSpellClass(const SCHOOL_ID& schoolID);
  //BOOL AddSpell(WORD CasterClassFlag, WORD SpellClassFlag, WORD CastMask, int SpellLevel);
  BOOL KnowSpell(const BASECLASS_ID& baseclassID, const SPELL_DATA *pSpell);
                      //const BASECLASS_LIST& spellAllowedBaseclasses, 
                      //int                   SpellLevel);
  //void DecrementSpellCountSpell(WORD CasterClassFlag, int SpellLevel);
  void DecrementSpellCount(const SPELL_DATA *pSpell);
  void RemoveSpellLevel(int SpellLevel);
  // ie: how many cleric spells of any level?
  //BYTE (WORD ClassFlag, int SpellLevel);
  BYTE NumKnown(const SCHOOL_ID& schoolID, int spellLEvel);
  //BYTE MaxSpellsForLevel(WORD CharClass, int CharLevel, int SpellLevel, BYTE prime);
  BYTE MaxSpellsForLevel(const BASE_CLASS_DATA *pBaseclass, int CharLevel, int SpellLevel, BYTE prime);
  //BYTE MaxSpellsForLevelAndSchool(int charBaseClass, int charLevel, int spellLevel, int prime, int spellSchool);
  BYTE MaxSpellsForLevelAndSchool(const BASE_CLASS_DATA *pBaseclass, 
                                  int baseclassLevel, 
                                  int spellLevel, 
                                  int prime, 
                                  const SCHOOL_ID& schoolID);
  //BOOL CanKnowSpell(WORD CasterClassFlag, WORD SpellClassFlag, WORD CastMask, int SpellLevel);
  BOOL CanKnowSpell(const SCHOOL_ID& schoolID,
                    int   SpellLevel);
  //void SetSpellLimits(WORD CharClass, int prime);
  void SetSpellLimits(const BASE_CLASS_DATA *pBaseclass, int prime, const SCHOOL_ID& schoolID);

  //int  LocateSpellLimits(const BASECLASS_ID& baseclassID, const SCHOOL_ID& schoolID) const;
  //int  LocateSchool(const SCHOOL_ID& schoolID) const; // Search spell limits for the school (any baseclasss)
  //const SPELL_LIMITS *PeekSpellLimits(int index) const;  
  //      SPELL_LIMITS *GetSpellLimits(int index);  


void GetMaxMemorizePerSpellLevel(const BASE_CLASS_DATA *pBaseclass,
                                       int baseclassLevel, 
                                       int *maxMemorizeArray, //[MAX_SPELL_LEVEL] 
                                       int prime, 
                                 const SCHOOL_ID& schoolID) const;
*/
};

//void SetSpellLimits(WORD CharClass, int prime, spellLimitsType &limits);
//int CurrSpellLevel(WORD CharClass, int CharLevel, BYTE Prime, WORD CastingClass);
//int MaxSpellLevelForClassAndSchool(baseclassKey charClass, int charLevel, BYTE prime, int spellSchool);
//int MaxSpellLevelForBaseclassAndSchool(const BASE_CLASS_DATA *pBaseclass, int charLevel, BYTE prime, const SCHOOL_ID& schoolID);
//WORD GetSpellCastingClassForCharClass(WORD CharClassFlag);


void GetSpellClasses(WORD charClass, WORD &c1, WORD &c2, WORD &c3);
//WORD CharClassToClassFlag(classType ctype);
//WORD SetClassBitsFromCharClass(classType ctype);



//WORD CharClassToClassFlag(const CLASS_ID& classID);
//WORD SetClassBitsFromCharClass(const CLASS_ID& classID);
//classType ClassFlagToCharClass(WORD cflag);
//CLASS_ID ClassFlagToCharClass(WORD cflag);
//WORD GetSpellCastingSchoolForCharBaseclass(WORD CharClassFlag);
//BOOL NumClassBits(WORD ClassFlag);
int GetSpellMemorizeTime(int level);
int GetSpellPrepTime(int level);
BOOL doClassesOverlap(const CLASS_ID& classID1, const CLASS_ID& classID2);
BOOL doClassesOverlap(const BASECLASS_ID& classID1, const CLASS_ID& classID2);

#endif // __GAMERULES_H__
