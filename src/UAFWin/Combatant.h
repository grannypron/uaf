/******************************************************************************
* Filename: Combatant.h
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
#ifndef __COMBATANT_H__
#define __COMBATANT_H__

#include "SharedQueue.h"
#include "monster.h"
#include "spell.h"
#include "Path.h"
#include "CombatSummary.h"

// make sure this enum has the same order as CombatStateType (overallCombatState?)
//
enum individualCombatantState
{ 
  ICS_None, 
  ICS_Casting, 
  ICS_Attacking, 
  ICS_Guarding, 
  ICS_Bandaging, 
  ICS_Using,
  ICS_Moving,
  ICS_Turning,
  ICS_Fleeing,
  ICS_Fled,
  ICS_ContinueGuarding, // ContinueGuarding means that a combatant who
                        // was guarding at the end of a round continues
                        // to guard until his initiative arrives.in
                        // the new round.
  ICS_Petrified,  // Not used as an ICS_STATE...only for script
  ICS_Dying,      // Not used as an ICS_STATE...only for script
  ICS_Unconscious,// Not used as an ICS_STATE...only for script
  ICS_Dead,       // Not used as an ICS_STATE...only for script
  ICS_Gone        // Not used as an ICS_STATE...only for script
  // Don't forget to update overallCombatState!

};

enum LASTACTION
{
  LA_None = 0,
  LA_Attack,
  LA_Defend
};

const int NUM_COMBATANT_STATES = 12;
extern char *CombatantStateText[NUM_COMBATANT_STATES];

enum FLEEING_FLAGS
{
  fleeBecauseOffMap     = 1,
  fleeAutoActionScript  = 2,  // The "AutoAction" script says combatant should flee
  fleeBecauseImpossible = 4,  // Never used!
};

enum AUTO_FLAGS
{
  forcePlayerScript     = 1,
  forceAutoScript       = 2,  // The "AutoAction" script says use Auto
};

enum WhatSprite
{  // Bit Mask
  DeathSprite = 1,
  DyingSprite = 2
};

enum GUARDING_CASE
{
  GUARD_AutoMaxMovement = 1,
  GUARD_CombatMenu
};

struct InvokeSpellOptions
{ // A packet to be passed into (and out of) the depths of 'InvokePendingSpell'.
  // Provided as a method of getting miscellaneous information to/from the 
  // functions involved with applying spell damage.
  // The first use is to supress the blitting of 'Death' and 'Dying' sprites.
  // We want to do this after displaying the damage text - so we supress having
  // it done in 'InvokeSpellOnTarget' and instead receive back an indication
  // that it needs to be done.
  int m_supressBlitSprite;  // bit mask of enum WhatSprite
  int m_whatSpriteNeeded;   // bit mask of enum WhatSprite
};


struct COMBATANT //: public CHARACTER
{
  COMBATANT():combatantSA(false) /*: CHARACTER() */{ Clear(); }
  ~COMBATANT(void);
  COMBATANT& operator=(COMBATANT& src);




  /* Here we put functions that are needed to access the
   * underlying character.  We used to derive COMBATANT 
   * from character and so the character variables were
   * directly exposed.  Now we need to access them via
   * the m_pCharacter pointer.
   */
  CHARACTER *m_pCharacter;
  BOOL  deleteChar;  // combatData.Clear will delete this character at end-of-combat
  inline const CString& GetName(void) const { return m_pCharacter->GetName();};
  inline BOOL IsPartyMember(void)  const {return m_pCharacter->IsPartyMember();};
  inline BYTE GetType()            const { return m_pCharacter->GetType(); }
  //inline BYTE GetCurrentLevel(WORD ClassFlag) const {return m_pCharacter->GetCurrentLevel(ClassFlag);};
  inline BYTE GetCurrentLevel(const BASECLASS_ID& baseclassID) const {return m_pCharacter->GetCurrentLevel(baseclassID);};
  //inline raceType race(void)       const {return m_pCharacter->race();};
  inline RACE_ID race(void)          const {return m_pCharacter->race;};
  //inline classType GetClass(void)  const { return m_pCharacter->GetClass(); };
  inline CLASS_ID GetClass(void)   const { return m_pCharacter->GetClass(); };
  inline BOOL CanCastSpells(void)  const { return m_pCharacter->CanCastSpells();};
  //inline GLOBAL_SPELL_ID GetSpellBookKey(GLOBAL_SPELL_ID gsID, BOOL checkmemorized) const 
  //                { return m_pCharacter->GetSpellBookKey(gsID, checkmemorized);};
  //inline SPELL_ID GetSpellBookID(const SPELL_ID& spellID, BOOL checkmemorized) const 
  //{ 
  //  return m_pCharacter->GetSpellBookID(spellID, checkmemorized);
  //};
  //inline BOOL GetSpell(GLOBAL_SPELL_ID gsID, CHARACTER_SPELL &spell) const
  //                { return m_pCharacter->GetSpell(gsID, spell);};
  //inline BOOL GetSpell(const SPELL_ID& spellID, CHARACTER_SPELL &spell) const
  //{ 
  //  return m_pCharacter->GetSpell(spellID, spell);
  //};
  inline BOOL FetchCharacterSpell(const SPELL_ID& spellID, CHARACTER_SPELL *pCharSp) const
  { 
    return m_pCharacter->FetchCharacterSpell(spellID, pCharSp);
  };
  inline void SetStatus(charStatusType val) { m_pCharacter->SetStatus(val); };
  inline void ReadyBestArmor(void) { m_pCharacter->ReadyBestArmor();};
  inline void ListShields(COMBAT_SUMMARY_COMBATANT *pcsc){m_pCharacter->ListShields(pcsc);};
  inline void ReadyBestShield(void) { m_pCharacter->ReadyBestShield();};
  //inline double EvalDiceAsClass(DICEPLUS &dice, WORD ClassFlag, int *pRollerLevel)
  //      { return m_pCharacter->EvalDiceAsClass(dice, ClassFlag, pRollerLevel);};
  inline double EvalDiceAsClass(DICEPLUS &dice, const BASECLASS_ID& baseclassID, int *pRollerLevel) const
        { return m_pCharacter->EvalDiceAsClass(dice, baseclassID, pRollerLevel);};
  inline double EvalDiceAsClass(DICEPLUS &dice, const SCHOOL_ID& schoolID, int spellLevel, int *pRollerLevel) const
        { return m_pCharacter->EvalDiceAsClass(dice, schoolID, spellLevel, pRollerLevel);};
  inline int GetMorale() const { return m_pCharacter->GetMorale(); };
  inline float GetNbrAttacks() const { return m_pCharacter->GetNbrAttacks(); };
  inline charStatusType GetStatus() const { return m_pCharacter->GetStatus(); };
  inline void UpdateSpellForAttacks(int AttacksTaken) {m_pCharacter->UpdateSpellForAttacks(AttacksTaken);};
  inline void SetEncumbrance(int val) { m_pCharacter->SetEncumbrance(val); };
  inline int  determineEffectiveEncumbrance() { return m_pCharacter->determineEffectiveEncumbrance();};
  inline int  determineMaxMovement() { return m_pCharacter->determineMaxMovement();};
  inline BOOL GetAutomatic() const { return m_pCharacter->GetAutomatic(); };
  inline void SetAutomatic(BOOL flag) { m_pCharacter->SetAutomatic(flag); };
  inline BYTE GetMaxMovement() const { return m_pCharacter->GetMaxMovement(); };
  inline double GetNbrHD() const { return m_pCharacter->GetNbrHD(); };
  inline void SetPartyMember(BOOL flag=TRUE) { m_pCharacter->SetPartyMember(flag);};
  inline void SetAllowPlayerControl(BOOL flag) { m_pCharacter->SetAllowPlayerControl(flag); };
  inline float determineNbrAttacks() {return m_pCharacter->determineNbrAttacks();};
  inline void SetType(BYTE flag) {m_pCharacter->SetType(flag) ;};
  inline void SetMaxMovement(BYTE val) {m_pCharacter->SetMaxMovement(val);};
  inline void ClearQueuedSpecAb() {m_pCharacter->ClearQueuedSpecAb();};
  inline int  getCharExpWorth() {return m_pCharacter->getCharExpWorth();};
  inline int  GetMaxHitPoints() const {return m_pCharacter->GetMaxHitPoints();};
  inline int  GetEffectiveAC(void) const {return m_pCharacter->GetEffectiveAC();};
  //inline void SortSpells(void){m_pCharacter->SortSpells();};
  inline BOOL IsAlive() {return m_pCharacter->IsAlive();};
  inline genderType GetGender() const { return m_pCharacter->GetGender(); };
  inline void SetGender(genderType val) { m_pCharacter->SetGender(val); };
  inline void SetGender(const CString& gender){m_pCharacter->SetGender(gender);};
  inline void InitTargeting(spellTargetingType ttype, 
                            BOOL canTargetFriend,
                            BOOL canTargetEnemy,
                            BOOL partyfriend,
                            DWORD targs,
                            DWORD range,
                            int   xSize,
                            int   ySize,
                            BOOL lingering)
              {
                m_pCharacter->InitTargeting(ttype, 
                                            canTargetFriend,
                                            canTargetEnemy,
                                            partyfriend,
                                            targs,
                                            range,
                                            xSize,
                                            ySize,
                                            lingering);
              };









  inline int  GetThiefBackstabDamageMultiplier() const { return m_pCharacter->GetThiefBackstabDamageMultiplier();};
  inline void SetMorale(int val) { m_pCharacter->SetMorale(val);};
  inline void SetHitPoints(int val) {m_pCharacter->SetHitPoints(val);};
  inline int  GetHitPoints() const { return m_pCharacter->GetHitPoints(); };
  inline BOOL HasDeathImmunity() const {return m_pCharacter->HasDeathImmunity(); };
  inline void UpdateSpellForDamage(int DamageTaken) {m_pCharacter->UpdateSpellForDamage(DamageTaken);};
  //inline int  GetAdjSpecAb(specialAbilitiesType sa, DWORD *pSource=NULL, GLOBAL_SPELL_ID *pSrcKey=NULL) const {return m_pCharacter->GetAdjSpecAb(sa, pSource, pSrcKey);};
  inline int  GetAdjSpecAb(specialAbilitiesType sa, DWORD *pSource=NULL, CString *pSpellName=NULL) const {return m_pCharacter->GetAdjSpecAb(sa, pSource, pSpellName);};
  //inline undeadType GetUndeadType() const { return m_pCharacter->GetUndeadType(); };
  inline CString  GetUndeadType() const { return m_pCharacter->GetUndeadType(); };
  inline BOOL IsAnimal() const {return m_pCharacter->IsAnimal();};
  inline BOOL IsAlwaysLarge() const {return m_pCharacter->IsAlwaysLarge();};
  inline BOOL HasVorpalImmunity() const {return m_pCharacter->HasVorpalImmunity();};
  inline void ComputeCharacterViewValues(void) {m_pCharacter->ComputeCharacterViewValues();};




  //inline int GetAdjMorale(DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const;
  inline BYTE GetAdjInt               (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjInt(flags);};
  inline charStatusType GetAdjStatus  (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjStatus(flags);};
  inline int GetAdjTHAC0              (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjTHAC0(flags);};
  inline int GetAdjAC                 (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjAC(flags);};
  inline BOOL GetAdjAllowPlayerControl(DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) {return m_pCharacter->GetAdjAllowPlayerControl(flags);};
  inline BOOL GetAdjAutomatic         (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) {return m_pCharacter->GetAdjAutomatic(flags);};
  inline BYTE GetAdjMaxMovement       (DWORD flags, LPCTSTR comment) const {return m_pCharacter->GetAdjMaxMovement(flags, comment);}; 
  inline int  GetAdjDmgBonus          (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjDmgBonus(flags); };
  //inline int  GetAdjHitBonus(GLOBAL_ITEM_ID weaponID, int distance, DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const 
  //{
  //  return m_pCharacter->GetAdjHitBonus(weaponID, distance, flags);
  //};
  inline int  GetAdjHitBonus(const ITEM_ID& weaponID, int distance, DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const 
  {
    return m_pCharacter->GetAdjHitBonus(weaponID, distance, flags);
  };
  inline int  GetAdjHitPoints         (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjHitPoints(flags);};
  inline BOOL GetAdjDetectingInvisible(DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjDetectingInvisible(flags);};
  inline creatureSizeType GetAdjSize  (DWORD flags=DEFAULT_SPELL_EFFECT_FLAGS) const {return m_pCharacter->GetAdjSize(flags);};



/*  Questionable */
  inline void generateNewCharacter(DWORD StartExperience, int StartExpType) {m_pCharacter->generateNewCharacter(StartExperience, StartExpType);};


  int m_preCombatMorale;





  enum { MAX_COMBAT_TARGETS=100 };

  //individualCombatantState GetState() { return state; }
  int GetUniqueId() { return self; }

  void Clear();
  void InitFromCharData(int dude);
  //void InitFromMonsterData(int dude, BOOL IsFriendly, ITEM_LIST &items, MONEY_SACK &msack);
  void InitFromMonsterData(const MONSTER_DATA *pMonster, BOOL IsFriendly, const ITEM_LIST &items, const MONEY_SACK &msack);
  void InitFromMonsterData(const MONSTER_ID& monsterID,  BOOL IsFriendly, const ITEM_LIST &items, const MONEY_SACK &msack);
  //void InitFromNPCData(int dude, BOOL IsFriendly, ITEM_LIST &items, MONEY_SACK &msack);
  void InitFromNPCData    (const CHARACTER_ID& characterID, BOOL IsFriendly, const ITEM_LIST &items, const MONEY_SACK &msack);
  void determineAvailAttacks(int numberOfAttacks);
  void LoadCombatIcon();
  void determineIconSize();
  BOOL isLargeDude() const;
  void RollInitiative(eventSurpriseType surprise);
  BOOL charOnCombatMap(bool unconsciousOK, bool petrifiedOK) const;
  BOOL charUnconscious() const;
  BOOL charPetrified() const;
  BOOL charCanTakeAction();
  //void GetDamageDice(int wpn, int *pNum, int *pSides, int *pBonus, BOOL *pNonLethal, BOOL IsLarge, GLOBAL_SPELL_ID *pSpellDbKey) const;
  void GetDamageDice(int wpn, 
                     int *pNum, 
                     int *pSides, 
                     int *pBonus, 
                     BOOL *pNonLethal, 
                     BOOL IsLarge, 
                     CString *pSpellName) const;
  //int GetAttackBonus(GLOBAL_ITEM_ID weaponID, int distance) const;
  int GetAttackBonus(const ITEM_ID& itemID, int distance) const;
  //void GetContext(ActorType *pActor, WORD ClassFlag=0) const;
  void GetContext(ActorType *pActor, const BASECLASS_ID& baseclassID) const;
  void GetContext(ActorType *pActor, const SCHOOL_ID& schoolID) const;
  void GetContext(ActorType *pActor, const SPELL_ID& spellID) const;
  void GetContext(ActorType *pActor, const SPELL_DATA *pSpell) const;
  void GetContext(ActorType *pActor) const; // Unknown class; therefore unknown Level.

  //BOOL CastSpell(GLOBAL_SPELL_ID spell, GLOBAL_SPELL_ID secondarySpell);
  BOOL CastSpell(const SPELL_ID& spellID, const SPELL_ID& secondarySpellID);
  //BOOL CastItemSpell(GLOBAL_SPELL_ID spell);
  BOOL CastItemSpell(const SPELL_ID& spellID);
  void SpellActivate(const PENDING_SPELL &data);
  //void InstantSpellActivate(GLOBAL_SPELL_ID spelldbkey, int target);    
  void InstantSpellActivate(const SPELL_ID& attackSpellID,
                            const SPELL_ID& itemSpellID,
                            int target, 
                            ToHitComputation *pToHitComputation);    
  BOOL IsSpellPending();

  // one time only, used for temp bonuses on such attacks as backstabbing
  //void SetTempAttackBonus(int att, int dmg) { m_iTmpAttDiceBonus=att;m_iTmpDmgDiceBonus=dmg; }

  void FaceOpponent(int opponent);
  void FaceLocation(int x, int y);
  void FaceDirection(PATH_DIR dir);
  BOOL CheckForGuardingEnemy();
  BOOL CheckOpponentFreeAttack(int oldX, int oldY, int newX, int newY);
  void FillTargeterList(PATH_DIR dir);
  
  void PlayHit() const;
  void PlayMiss() const;
  void PlayLaunch() const;
  void PlayCombatDeath() const;
  void PlayCombatMove() const;
  void PlayCombatTurnUndead() const;

  void TakeDamage(int dmg, 
                  BOOL IsNonLethal, 
                  InvokeSpellOptions *invokeOptions, 
                  bool canFinishCasting,
                  int *pDeathIndex);
  void RestoreToParty();  
  
  void /* PRS July 2009 BOOL */ UpdateCombatant();
  BOOL Think();
  BOOL FreeThink(void);
  void HandleCombatRoundMsgExpired();
  void HandleTimeDelayMsgExpired(int iDeathIndex);
  int  HandleTimeDelayMsgBegin(int extraAttacksAvailable, int *pDeathIndex);
  void PostCombatScreenUpdate();
  int  HandleCurrState(BOOL zeroMoveAttackOK);

  void displayCombatSprite();
  void blitDeadSprite();
  void blitDyingSprite();
  void blitPetrifiedSprite();

  BOOL CanAddTarget(int target);
  void AddTarget(int newTarget, bool freeAttack);
  BOOL C_AddTarget(COMBATANT &dude, int range=0);
  BOOL AddMapTarget(int mapx, int mapy, PATH_DIR dir, int dirX, int dirY);
  BOOL AddTargetSelf();
  void AutoChooseSpellTargets();
  void RemoveAllTargets();
  void SetCurrTarget();
  void RemoveCurrTarget();
  int  GetCurrTarget(bool updateTarget, bool unconsciousOK, bool petrifiedOK);
  int  GetNextTarget();
  int  GetMaxTargets();
  int  GetNumTargets() const { return combattargets.GetCount(); }
  BOOL HaveTarget(int target);
  void RemoveTarget(int target);

  BOOL canAttack(int target, 
                 int targetX, int targetY, 
                 int additionalAttacks, 
                 int (*Distance)(int,int,int,int,int,int),
                 bool canAttackSelf);
  BOOL IsAttackPossible(void);
  BOOL StartAttack(int target, int additionalAttacks);
  int  makeAttack(int target, int extraAttacksAvailable, int *pDeathIndex);
  BOOL DetermineIfBackStab(int wpn, int targ) const;

  void AttackOver();
  void StopAttack();
  double AttacksRemaining() const { return availAttacks; }
  BOOL UseAttackPose();

  BOOL NeedMissileAnimation();
  void InitMissileAnimation();
  BOOL NeedHitAnimation();
  void InitHitAnimation();
  BOOL NeedSpellCastAnimation();
  void InitSpellCastAnimation(int targ);
  BOOL NeedSpellInRouteAnimation(int x, int y, BOOL allowSelf);
  void InitSpellInRouteAnimation(int x, int y);
  void InitSpellInRouteAnimation(int srcx, int srcy, int dstx, int dsty);
  BOOL NeedSpellCoverageAnimation();
  void InitSpellCoverageAnimation(int x, int y);
  BOOL NeedSpellHitAnimation();
  void InitSpellHitAnimation(int targ);
  BOOL NeedSpellLingerAnimation();
  void InitSpellLingerAnimation(/*int mapx, int mapy*/);
  void InitSpellLingerAnimation(int targ);

  BOOL CanTarget(bool freeAttack);
  BOOL CanMove(BOOL allowZeroMove);
  BOOL CanCast();
  BOOL CanUse();
  BOOL CanTurnUndead();
  BOOL CanDelay();
  BOOL CanBandage();
  CString SpecialActionName();
  int SpecialActionIndex();
  BOOL CanGuard(GUARDING_CASE guardCase);

  void Bandage();
  void EndTurn(individualCombatantState newState=ICS_None);
  void Quick(BOOL Enable);
  void Guard();
  void DelayAction(); 
  void TurnUndead();
  //void StartInitialSpellCasting(GLOBAL_SPELL_ID key, GLOBAL_SPELL_ID secondaryKey);
  void StartInitialSpellCasting(const SPELL_ID& spellName, const SPELL_ID& secondarySpellName);
  //void StartInitialItemSpellCasting(GLOBAL_SPELL_ID key);
  void StartInitialItemSpellCasting(const SPELL_ID& spellID);
  void EndInitialSpellCasting();

  BOOL IsDone(bool freeAttack, LPCSTR comment); 
  BOOL IsAttacking() const { return (State() == ICS_Attacking); }
  BOOL IsGuarding() const { return (State() == ICS_Guarding); }

  BOOL IsCasting();
  void StopCasting(BOOL EndTurn, bool canFinishCasting);
 
  //GLOBAL_SPELL_ID  GetSpellKeyBeingCast() const {return m_spellKeyBeingCast;};
  SPELL_ID  GetSpellIDBeingCast() const {return m_spellIDBeingCast;};
  //GLOBAL_SPELL_ID  GetSecondarySpellKeyBeingCast() const {return m_secondarySpellKeyBeingCast;};
  SPELL_ID  GetSecondarySpellIDBeingCast() const {return m_secondarySpellIDBeingCast;};
  //GLOBAL_SPELL_ID  GetEffectiveSpellKeyBeingCast() const
  SPELL_ID GetEffectiveSpellIDBeingCast() const
  {
    //if (m_secondarySpellKeyBeingCast.IsValidSpell())
    if (m_secondarySpellIDBeingCast.IsValidSpell())
    {
      //return m_secondarySpellKeyBeingCast;
      return m_secondarySpellIDBeingCast;
    };
    //return m_spellKeyBeingCast;
    return m_spellIDBeingCast;
  };
  //GLOBAL_SPELL_ID  GetItemSpellKeyBeingCast() const {return m_itemSpellKeyBeingCast;};;
  SPELL_ID  GetItemSpellIDBeingCast() const {return m_itemSpellIDBeingCast;};;
  //void SetSpellKeyBeingCast(GLOBAL_SPELL_ID gsID, GLOBAL_SPELL_ID secondaryKey)
  void SetSpellIDBeingCast(const SPELL_ID& spellID, const SPELL_ID& secondarySpellID)
  {
    //m_spellKeyBeingCast = gsID;
    //m_secondarySpellKeyBeingCast = secondaryKey;
    m_spellIDBeingCast = spellID;
    m_secondarySpellIDBeingCast = secondarySpellID;
  };
  //void SetItemSpellKeyBeingCast(GLOBAL_SPELL_ID gsID){m_itemSpellKeyBeingCast = gsID;};
  void SetItemSpellIDBeingCast(const SPELL_ID& spellID){m_itemSpellIDBeingCast = spellID;};
  BOOL FetchSpellBeingCast(CHARACTER_SPELL *pCharSp=NULL);
  CString RunPSScripts(const CString& scriptName, LPCSTR comment);
 

  BOOL OnAuto(bool callAutoActionHook);
  void StopMoving() { State(ICS_None); moveX=-1; moveY=-1;}  

  void moveLeft();
  void moveRight();
  void moveForward();
  void moveBackward();
  void moveNW();
  void moveNE();
  void moveSW();
  void moveSE();
  BOOL CurrMoveWouldFleeMap();

  BOOL MoveCombatant(int newX, int newY, BOOL allowZeroMoveAttack);
  BOOL TeleportCombatant(int newX, int newY);
  BOOL FindPathTo(int destLeft,  int destTop, 
                  int destRight, int destBottom, 
                  BOOL CheckOccupants, 
                  BOOL allowZeroMove,
                  BOOL moveOriginPoint);
  BOOL FindPathAwayFrom(int fleeFrom);
  int FindPathToMapNorthEdge(void);
  int FindPathToMapEastEdge(void);
  int FindPathToMapSouthEdge(void);
  int FindPathToMapWestEdge(void);
  BOOL FindPathToMapEdge();
  BOOL FindPathAlongLine(PATH_DIR dir, int dist);
  void ClearPath();
  BOOL TakeNextStep(BOOL allowZeroMoveAttack);
 
  void ReadyBestWpn(int targ);
  BOOL toggleReadyItem(int item);
  BOOL delCharacterItem(int index, int qty);

  int DetermineInitialFacing(int PartyFacing);
  void CheckMorale();

  DWORD MakeTargetKey(int dude, int dist) { return ((((DWORD)dist) << 16) | ((DWORD)dude)); }
  int   GetTargetKeyDude(DWORD k) const { return (k & 0x0000FFFF); }
  int   GetTargetKeyDist(DWORD k) const { return ((k >> 16) & 0x0000FFFF); }

  int GetCenterX();
  int GetCenterY();

  BOOL GetIsFriendly() const;
  
  void OnStartCombatantAction();
  void OnEndTurn();
  void OnEnsureVisible();

  CString FormatSpecAbMsg(DWORD sa_state);


  // These ModifyXXX functions dynamically alter character
  // values based on spell effects or special abilities.
  BOOL ModifyAttackRollDice                 
        (const CHARACTER *pTarget,                        const int num, const int sides, int *pBonus) const;
  BOOL ModifyAttackRollDiceForItem          
        //(const CHARACTER *pTarget,   GLOBAL_ITEM_ID giID, const int num, const int sides, int *pBonus, int distance) const;
        (const CHARACTER *pTarget,   const ITEM_ID& itemID, const int num, const int sides, int *pBonus, int distance) const;
  BOOL ModifyAttackRollDiceAsTarget         
        (const CHARACTER *pAttacker,                      const int num, const int sides, int *pBonus) const;
  BOOL ModifyAttackRollDiceForItemAsTarget  
        //(const CHARACTER *pAttacker, GLOBAL_ITEM_ID giID, const int num, const int sides, int *pBonus) const;
        (const CHARACTER *pAttacker, const ITEM_ID& itemID, const int num, const int sides, int *pBonus) const;
  BOOL ModifyAttackDamageDice               
        (const CHARACTER *pTarget,                        const int num, const int sides, int *pBonus, BOOL *pNonLethal) const;
  BOOL ModifyAttackDamageDiceForItem        
        //(const CHARACTER *pTarget,   GLOBAL_ITEM_ID giID, const int num, const int sides, int *pBonus, BOOL *pNonLethal, int distance) const;
        (const CHARACTER *pTarget,   const ITEM_ID& itemID, const int num, const int sides, int *pBonus, BOOL *pNonLethal, int distance) const;
  BOOL ModifyAttackDamageDiceAsTarget       
        (const CHARACTER *pAttacker,                      const int num, const int sides, int *pBonus, BOOL *pNonLethal) const;
  void ModifyAttackDamageDiceForItemAsTarget
        //(const CHARACTER *pAttacker, GLOBAL_ITEM_ID giID, const int num, const int sides, int *pBonus, BOOL *pNonLethal, int toHitRolled) const;
        (const CHARACTER *pAttacker, const ITEM_ID& itemID, const int num, const int sides, int *pBonus, BOOL *pNonLethal, int toHitRolled) const;
  BOOL ModifyAttackThac0                    
        (const CHARACTER *pTarget,                  int *pVal) const;
  BOOL ModifyAttackThac0AsTarget            
        (const CHARACTER *pAttacker,                int *pVal) const;
  BOOL ModifySaveRoll                       
        (const CHARACTER *pTarget,                        int *pRoll) const;
  BOOL ModifySaveRollAsTarget               
        (CHARACTER *pAttacker,                      int *pRoll) const;
  BOOL ModifyACAsTarget                     
        //(const CHARACTER *pAttacker,                int *pAC, GLOBAL_ITEM_ID giID) const;
        (const CHARACTER *pAttacker,                int *pAC, const ITEM_ID& itemID) const;

  enum SPECIAL_SELF {
    SELF_NewCombatant = -3,
  };
  int self;
	int x,y;  // During creation x is the newMonsters index!
  int hPath;
  enum INITIATIVE
  {
    INITIATIVE_AlwaysFirst = 1,
    INITIATIVE_FirstDefault = 9,
    INITIATIVE_LastDefault = 18,
    INITIATIVE_Never = 23,
  };
	int m_iInitiative; //
  int scriptPriority;
	int m_target; // index of target
  int targetValidity; //-1=unknown; 0=notValid; 1=valid
  void SetTarget(int t){m_target=t; targetValidity=-1;}; //Not yet checked
	int m_iFacing;
  int m_iMovement;
  int m_iMoveDir;
  int m_iNumDiagonalMoves;  // In this round.
  int moveX;
  int moveY;
  int lastAttackRound;
  double availAttacks;
  BOOL continueAttack; // interrupt a series of multiple attacks?
  int  iFleeingFlags;  // See FLEEING_FLAGS
  int  iAutoFlags;     // See AUTO_FLAGS
  BOOL isTurned;  // this guy has been turned and is fleeing from the cleric that did it
  BOOL hasTurnedUndead; // only get to turn undead once per combat
  int  m_iLastAttacker; // who tried to hit us most recently?
  int  m_iLastAttacked; // who did we try to hit most recently?
  LASTACTION m_eLastAction; //
	BOOL turnIsDone;
  BOOL isBandaged;
  int  bandageWho;
  BOOL didMove;
  private:
  individualCombatantState m_ICS;
  public:
    inline individualCombatantState State(void) const {return m_ICS;};
    void State(individualCombatantState ICS);
	BOOL friendly;
  int  m_adjFriendly; // 0=leave alone; 1=friendly; 2=not friendly; 3=toggle friendly
  BOOL blitDeathTile;
  //GLOBAL_SPELL_ID m_spellKeyBeingCast;
  //GLOBAL_SPELL_ID m_secondarySpellKeyBeingCast; 
  SPELL_ID m_spellIDBeingCast;
  SPELL_ID m_secondarySpellIDBeingCast; 
  // Moved to CHARACTER  int             m_spellCastingLevel;  // level of combatant casting m_spellKeyBeingCast
  //GLOBAL_SPELL_ID m_itemSpellKeyBeingCast;
  SPELL_ID m_itemSpellIDBeingCast;
  weaponClassType Wpn_Type; // Not serializedUsed during item spell casting process.
  int width;
  int height;
  POSITION targetPos;
  //int m_iTmpAttDiceBonus;  // Valid for one attack then set to 0.
  //int m_iTmpDmgDiceBonus;  // Valid for one attack then set to 0.
  int origKey;
  int combatant_pendingSpellKey;
  int combatant_activeSpellKey; // -1 in idle state
#ifdef SpellInitiationScript
                                // -2 means the SpellInitiationScript has been executed
                                // positive value is index into activeSpellList
#endif
  int combatant_spellCastingLevel; //level at which combatant_activeSpellKey was cast
  BOOL forceAttackPose;
  // Only referenced in one function 20101017 PRS   int m_iAttDiceSides;
  // Only referenced in one function 20101017 PRS   int m_iAttDiceNum;
  // Only computed and used in one function 20101017 PRS   int m_iAttDiceRoll;
  int m_isCombatReady;
  int m_spellDamageHP;
  // *******************************
  bool m_useTempHP; // During combat spell processing adjustments are made here
  int m_tempHP;     // rather than to the character's HP.  At the end of spell
                    // processing, we transfer the temp value to the character.
  // *******************************
  CString m_specialActionName;
  int m_specialActionIndex;

  SPECIAL_ABILITIES combatantSA;
  CString       RunCombatantScripts(
                     LPCSTR          scriptName, 
                     enum CBRESULT (*fnc)(enum CBFUNC func, CString *scriptResult, void *pkt),
                     void           *pkt,
                     LPCSTR          comment
                     ) const
  {
    return combatantSA.RunScripts(scriptName, 
                                  fnc,
                                  pkt,
                                  comment,
                                  ScriptSourceType_Combatant,
#ifdef newCombatant
                                  GetName()
#else
                                  name
#endif
                                 );
  };
  void EnterGuardingState(void);


  OrderedQueue<int, MAX_COMBAT_TARGETS> combattargets; // who am I targeting?
  // temp list to check perimeter of self for attackers.
  CList<int, int> targeters; 


  void ComputeCombatViewValues(void);

};

struct QueuedCombatant
{
  QueuedCombatant() { Dude=NO_DUDE; AffectStats=TRUE; }
  QueuedCombatant(int dude, BOOL stats, int numFreeAttacks, int numGuardAttacks) 
  { 
    Dude=dude; 
    AffectStats=stats; 
    m_numFreeAttacks = numFreeAttacks;
    m_numGuardAttacks = numGuardAttacks;
    m_delayedX = -1;
    m_delayedY = -1;
    m_bStartOfTurn = false;
    m_bRestartInterruptedTurn = false;
    //20111024m_target = NO_DUDE;
  }

  BOOL AffectStats;
  int  m_numFreeAttacks;
  int  m_numGuardAttacks;
  int  m_delayedX;
  int  m_delayedY;
  int  Dude;
  bool m_bStartOfTurn;  // For the combatant at top-of-queue.  Set
                        // true when combatant first reaches top-of-queue,
                        // Set false when UpdateCombat is called again
                        // for the same combatant.
  bool m_bRestartInterruptedTurn;
                        // Like m_bStartOfTurn except this is a chaacter
                        // whose turn was interrupted and he has just returned
                        // to the top-of-queue.  Cleared when UpdateCombat is called
  //20111024int m_target;
};

struct QueuedCombatantData
{
private:
  CList<QueuedCombatant, QueuedCombatant&> m_q;
public:
  QueuedCombatantData() { Clear(); }
  void NotStartOfTurn(void)
  {
    POSITION pos;
    pos = m_q.GetHeadPosition();
    if (pos == NULL) return;
    m_q.GetAt(pos).m_bStartOfTurn = false;
    m_q.GetAt(pos).m_bRestartInterruptedTurn = false;
  };
  bool StartOfTurn(void)
  {
    POSITION pos;
    pos = m_q.GetHeadPosition();
    if (pos == NULL) return false;
    return m_q.GetAt(pos).m_bStartOfTurn;
  };
  int DelayedX(void)
  {
    if (m_q.GetCount() > 0) 
      return m_q.GetHead().m_delayedX;
    return -1;
  };
  int DelayedY(void)
  {
    if (m_q.GetCount() > 0) 
      return m_q.GetHead().m_delayedY;
    return -1;
  };
  bool RestartInterruptedTurn(void)
  {
    POSITION pos;
    pos = m_q.GetHeadPosition();
    if (pos == NULL) return false;
    return m_q.GetAt(pos).m_bRestartInterruptedTurn;
  };
  void Clear() { m_q.RemoveAll(); }
  void Push(int dude, BOOL stats, int numFreeAttacks, int numGuardAttacks)
  {
    //ASSERT(m_q.GetCount() == 0);
    POSITION pos;
    pos = m_q.GetHeadPosition();
    if (pos != NULL)
    {
      QueuedCombatant& queuedCombatant = m_q.GetAt(pos);
      queuedCombatant.m_bRestartInterruptedTurn = !queuedCombatant.m_bStartOfTurn;
    };
    QueuedCombatant temp(dude, stats, numFreeAttacks, numGuardAttacks);
    temp.m_bStartOfTurn = true;
    m_q.AddHead(temp);

  }
  void PushTail(int dude, BOOL stats)
  {
    //ASSERT(m_q.GetCount() == 0);
    QueuedCombatant temp(dude,stats,0,0);
    m_q.AddTail(temp);
  }
  void Pop() { m_q.RemoveHead();}
  
  void Remove(int dude)
  {
    POSITION pos = m_q.GetHeadPosition();
    while (pos != NULL)
    {
      if (m_q.GetAt(pos).Dude == dude)
      {
        m_q.RemoveAt(pos);
        return;
      }
      m_q.GetNext(pos);
    }
  }

  int Top() 
  { 
    if (m_q.GetCount() > 0) 
      return m_q.GetHead().Dude; 
    else 
      return NO_DUDE; 
  }

  BOOL ChangeStats()
  {
    if (m_q.GetCount() > 0) 
      return m_q.GetHead().AffectStats; 
    else 
      return FALSE; 
  }

  int NumFreeAttacks(void)
  {
    if (m_q.GetCount() > 0) 
      return m_q.GetHead().m_numFreeAttacks;
    else
      return 0;
  };
  int NumGuardAttacks(void)
  {
    if (m_q.GetCount() > 0) 
      return m_q.GetHead().m_numGuardAttacks;
    else
      return 0;
  };
  void NumFreeAttacks(int n)
  {
    if (m_q.GetCount() > 0) 
      m_q.GetHead().m_numFreeAttacks = n;
  };
  void NumGuardAttacks(int n)
  {
    if (m_q.GetCount() > 0) 
      m_q.GetHead().m_numGuardAttacks = n;
  };
  void SetXY(int x, int y)
  {
    if (m_q.GetCount() > 0) 
    {
      m_q.GetHead().m_delayedX = x;
      m_q.GetHead().m_delayedY = y;
    };
  }

/* 20111024
  int Target(void)
  {
    if (m_q.GetCount() > 0) 
      return m_q.GetHead().m_target;
    else
      return 0;
  };
*/


//  BOOL SingleFreeAttack()
//  {
//    if (m_q.GetCount() > 0) 
//      return m_q.GetHead().m_singleFreeAttack; 
//    else 
//      return FALSE; 
//  }

  int DecrementFreeAttacks()
  {
    if (m_q.GetCount() > 0) 
      return --m_q.GetHead().m_numFreeAttacks; 
    return 0;
  }

  int DecrementGuardAttacks()
  {
    if (m_q.GetCount() > 0) 
      return --m_q.GetHead().m_numGuardAttacks; 
    return 0;
  }

};


struct COMBAT_MSGS
{
  enum { MAX_COMBAT_MSGS=1 };

  ~COMBAT_MSGS() { msgs.RemoveAll(); }
  void Add(CString &msg) 
  { 
    if (msgs.GetCount() > MAX_COMBAT_MSGS) msgs.RemoveHead();
    msgs.AddTail(msg);
  }
     
  CList<CString, CString> msgs;
};

#endif //__COMBATANT_H__