/******************************************************************************
* Filename: Combatant.cpp
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

#include "char.h"
#include "GlobalData.h"
#include "party.h"
#include "DispText.h"
#include "Combatant.h"
#include "DrawTile.h"
#include "Combatants.h"
#include "DrawTile.h"
#include "Viewport.h"
#include "Screen.h"
#include "GameMenu.h"
#include "FormattedText.h"
#include "CombatSummary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int RunTHINK(unsigned char *program, COMBAT_SUMMARY *pcs);
int RunSpellCasterFilter(COMBAT_SUMMARY *pcs,  COMBAT_SUMMARY_ACTION *pcsa);
int RunSpellLikeAbilityFilter(COMBAT_SUMMARY *pcs,  COMBAT_SUMMARY_ACTION *pcsa);
int RunAdvanceFilter(COMBAT_SUMMARY *pcs,  COMBAT_SUMMARY_ACTION *pcsa);
int RunJudoFilter(COMBAT_SUMMARY *pcs,  COMBAT_SUMMARY_ACTION *pcsa);
int RunMeleeWeaponFilter(COMBAT_SUMMARY *pcs,  COMBAT_SUMMARY_ACTION *pcsa);
int RunRangedWeaponFilter(COMBAT_SUMMARY *pcs,  COMBAT_SUMMARY_ACTION *pcsa);
bool ExpandKernel(void);
void FetchKernel(unsigned char *program);

extern const char *WeaponClassText[];
extern BOOL logDebuggingInfo;
extern BOOL LogCombatActions;
extern const CString Skill_BackstabMultiplier;
extern ITEM_DATA bogusItem;


char *CombatantStateText[NUM_COMBATANT_STATES] = 
{
  "ICS_None",
  "ICS_Casting",
  "ICS_Attacking",
  "ICS_Guarding",
  "ICS_Bandaging",
  "ICS_Using",
  "ICS_Moving",
  "ICS_Turning",
  "ICS_Fleeing",
  "ICS_Fled",
  "ICS_ContinueGuarding",
  "ICS_Petrified"
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
//  COMBATANT functions
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifdef newCombatant
//*****************************************************************************
//    NAME: COMBATANT::~COMBATANT
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT::~COMBATANT(void)
{
  if (m_pCharacter != NULL)
  {
    die(0xffd21c);
  };
  combatantSA.Clear();
}
#endif

//*****************************************************************************
//    NAME: COMBATANT::Clear
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::Clear()
{
  m_pCharacter = NULL;
  m_useTempHP = false;
  deleteChar = FALSE;
  self=-1;
  x=-1; y=-1;
  hPath=-1;
  m_iInitiative=0;
  m_target=-1;
  targetValidity=-1;
  m_iFacing=0;
  m_iMoveDir=0;
  m_iMovement=0;
  m_iNumDiagonalMoves = 0;
  moveX=-1;moveY=-1;
  lastAttackRound=0;
  availAttacks=0.0;
  continueAttack=FALSE;
  turnIsDone=FALSE;
  isBandaged=FALSE;
  bandageWho=-1;
  origKey=-1;
  didMove=FALSE;
  State(ICS_None);
  friendly=TRUE;
  m_adjFriendly = 0;  // No adjustment
  blitDeathTile=FALSE;
  width=0;height=0;
  targetPos=NULL;  
  //m_iTmpAttDiceBonus=0;
  //m_iTmpDmgDiceBonus=0;
  //partyMember=FALSE;
  iFleeingFlags = 0;
  isTurned=FALSE;
  hasTurnedUndead=FALSE;
  m_iLastAttacker=-1;
  m_iLastAttacked=-1;
  m_eLastAction = LA_None;
  RemoveAllTargets();
  targeters.RemoveAll();
  //m_spellKeyBeingCast.Clear();
  m_spellIDBeingCast.Clear();
  // moved to CHARACTER;   m_spellCastingLevel = -1;
  //m_itemSpellKeyBeingCast.Clear();
  m_itemSpellIDBeingCast.Clear();
  Wpn_Type = NotWeapon;
  combatant_pendingSpellKey=-1;
  combatant_activeSpellKey=-1;
  combatant_spellCastingLevel=-1;
  forceAttackPose=FALSE;
  //m_iAttDiceSides=0;
  //m_iAttDiceNum=0;
  //m_iAttDiceRoll=0;
  m_isCombatReady = -1;
  m_specialActionName = "";
  m_specialActionIndex = -1;
  combatantSA.Clear();
  //combatantSA.InsertAbility("Competing","", "Combatant Constructor ",m_pCharacter->GetName());
  combatantSA.InsertAbility("Competing","", "Combatant Constructor ","");
}

//*****************************************************************************
//    NAME: COMBATANT::operator=
//
// PURPOSE: 
//
//*****************************************************************************
COMBATANT& COMBATANT::operator=(COMBATANT& src)
{
  if (&src == this)
    return *this;

  if ((m_pCharacter != NULL) || (src.m_pCharacter != NULL))
  {
    // You need to take care of this yourself.  If one of these pointers
    // is valid, then you need to set them to NULL before the copy and
    // relink the character to the proper combatant via its m_pCombatant pointer.
    // There should always be a one-to-one link between a combatant and
    // a character.
    die (0xfefe22);
  };
  m_useTempHP = src.m_useTempHP;
  self = src.self;
  deleteChar = src.deleteChar;
	x = src.x;
  y = src.y;
  hPath = src.hPath;
	m_iInitiative = src.m_iInitiative; //
	m_target = src.m_target; // index of target
  targetValidity = src.targetValidity;
	m_iFacing = src.m_iFacing;
	m_iMoveDir = src.m_iMoveDir;
  m_iMovement = src.m_iMovement;
  m_iNumDiagonalMoves = src.m_iNumDiagonalMoves;
  moveX = src.moveX;
  moveY = src.moveY;
  lastAttackRound = src.lastAttackRound;
  availAttacks = src.availAttacks;
  iFleeingFlags = src.iFleeingFlags;
  isTurned=src.isTurned;
  hasTurnedUndead=src.hasTurnedUndead;
  m_iLastAttacker=src.m_iLastAttacker;
  m_iLastAttacked=src.m_iLastAttacked;
  m_eLastAction = src.m_eLastAction;
	turnIsDone = src.turnIsDone;
  isBandaged = src.isBandaged;
  bandageWho = src.bandageWho;
  origKey=src.origKey;
  didMove = src.didMove;
  m_ICS = src.m_ICS;
  State(src.State());
	friendly = src.friendly;
	m_adjFriendly = src.m_adjFriendly;
  blitDeathTile = src.blitDeathTile;
  width = src.width;
  height = src.height;
  targetPos = src.targetPos;
  //partyMember = src.partyMember;
  //m_iTmpAttDiceBonus = src.m_iTmpAttDiceBonus;
  //m_iTmpDmgDiceBonus = src.m_iTmpDmgDiceBonus;
  continueAttack=src.continueAttack;
  //m_spellKeyBeingCast=src.m_spellKeyBeingCast;
  m_spellIDBeingCast=src.m_spellIDBeingCast;
  // moved to CHARACTER;  m_spellCastingLevel=src.m_spellCastingLevel;
  //m_itemSpellKeyBeingCast=src.m_itemSpellKeyBeingCast;
  m_itemSpellIDBeingCast=src.m_itemSpellIDBeingCast;
  Wpn_Type=src.Wpn_Type;
  combatant_pendingSpellKey=src.combatant_pendingSpellKey;
  combatant_activeSpellKey=src.combatant_activeSpellKey;
  combatant_spellCastingLevel=src.combatant_spellCastingLevel;
  forceAttackPose=src.forceAttackPose;
  //m_iAttDiceSides=src.m_iAttDiceSides;
  //m_iAttDiceNum=src.m_iAttDiceNum;
  //m_iAttDiceRoll=src.m_iAttDiceRoll;
  m_isCombatReady=src.m_isCombatReady;
  m_specialActionName = src.m_specialActionName;
  m_specialActionIndex = src.m_specialActionIndex;

  combattargets.RemoveAll();
  POSITION pos = src.combattargets.GetHeadPosition();
  while (pos != NULL)
  {
    DWORD data = src.combattargets.GetNext(pos);
    int dude = GetTargetKeyDude(data);
    combattargets.Insert(dude, data);
  }

  combatantSA.Copy(src.combatantSA);
  //targeters is a temp list that should not need to be copied

  return *this;
}





//*****************************************************************************
//    NAME: COMBATANT::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::GetContext(ActorType *pActor, WORD ClassFlag) const
void COMBATANT::GetContext(ActorType *pActor) const
{
  //ActorType data;
  pActor->Clear();

  if (IsPartyMember())
  {
    pActor->SetCombatPartySrc(friendly);
  }
  else
  {
    if (GetType() == MONSTER_TYPE)
      pActor->SetCombatMonsterSrc(friendly);
    else if (GetType() == NPC_TYPE)
      pActor->SetCombatNPCSrc(friendly);
#ifdef newCombatantx
    else
      die(0xea667);
#endif
  }  

  // This character will not always have a
  // level value for ClassFlag. For instance,
  // a cleric casts a spell on a fighter. The
  // ClassFlag will be ClericFlag, and the fighter's
  // target context will have a zero cleric
  // level.
  //pActor->Level = GetCurrentLevel(ClassFlag);
  pActor->Level = 0; //Level is unknown.


  pActor->m_raceID = race();
  pActor->m_classID = GetClass(); 
  if (self == SELF_NewCombatant)
  { 
    pActor->instanceType = InstanceType_NewCombatant;
    pActor->Instance = x;
  }
  else
  {
    pActor->Instance = self;
    pActor->instanceType = InstanceType_CombatantIndex;
  };
  //pActor->Level = LEVEL_UNKNOWN;;
  return;// data;
}



//*****************************************************************************
//    NAME: COMBATANT::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::GetContext(ActorType *pActor, WORD ClassFlag) const
void COMBATANT::GetContext(ActorType *pActor, const BASECLASS_ID& baseclassID) const
{
  GetContext(pActor);
  pActor->Level = GetCurrentLevel(baseclassID);
  ASSERT( (pActor->Level>=0) && (pActor->Level<=40) );
}


//*****************************************************************************
//    NAME: COMBATANT::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::GetContext(ActorType *pActor, WORD ClassFlag) const
void COMBATANT::GetContext(ActorType *pActor, const SPELL_DATA *pSpell) const
{
  GetContext(pActor);
  pActor->m_spellID = pSpell->SpellID();
}

//*****************************************************************************
//    NAME: COMBATANT::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::GetContext(ActorType *pActor, WORD ClassFlag) const
void COMBATANT::GetContext(ActorType *pActor, const SPELL_ID& spellID) const
{
  GetContext(pActor);
  pActor->m_spellID = spellID;
}


//*****************************************************************************
//    NAME: COMBATANT::GetContext
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::GetContext(ActorType *pActor, WORD ClassFlag) const
void COMBATANT::GetContext(ActorType *pActor, const SCHOOL_ID& schoolID) const
{
  GetContext(pActor);
  pActor->m_schoolID = schoolID;
}




void COMBATANT::EnterGuardingState(void)
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  scriptContext.SetCombatantContext(this);
  RunCombatantScripts(GUARDING_GUARD, 
                      ScriptCallback_RunAllScripts, 
                      NULL,
                      "Combatant entering guarding state");
}

void COMBATANT::State(individualCombatantState ICS)
{
  if (m_pCharacter != NULL)
  {
    //WriteDebugString("DEBUG - COMBATANT(%s)::State(%d)\n", m_pCharacter->GetName(),ICS);
  };
  if ((m_ICS != ICS_Guarding) && (ICS == ICS_Guarding))
  {
    EnterGuardingState();
  };
#ifdef TraceFreeAttacks
  if (   (   (m_ICS == ICS_Guarding) 
          || (m_ICS == ICS_ContinueGuarding)
          || (ICS == ICS_Guarding) 
          || (ICS == ICS_ContinueGuarding)
         )
      && (m_ICS != ICS)
     )
  {
    WriteDebugString("TFA - %s enters state %s\n", GetName(), CombatantStateText[ICS]);
  };
#endif
  m_ICS = ICS;
}

//*****************************************************************************
// NAME: COMBATANT::CanCast
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanCast()
{
  if (GetConfigNoMagic()) return FALSE;
  if ((!IsPartyMember()) && (GetConfigMonsterNoCast())) return FALSE;
  if (IsDone(false, "Can combatant cast spell")) return FALSE;
  return CanCastSpells();
}

//*****************************************************************************
//    NAME: COMBATANT::IsCasting
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::IsCasting()
{
  return (State()==ICS_Casting);
}
//*****************************************************************************
//    NAME: COMBATANT::IsSpellPending
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::IsSpellPending() 
{ 
  return ((IsCasting()) && (combatant_pendingSpellKey>=0)); 
}

//*****************************************************************************
//    NAME: COMBATANT::StartInitialSpellCasting
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::StartInitialSpellCasting(GLOBAL_SPELL_ID skey, GLOBAL_SPELL_ID secondaryKey)
void COMBATANT::StartInitialSpellCasting(const SPELL_ID& spellID, const SPELL_ID& secondarySpellID)
{
  if (logDebuggingInfo)
  {
    WriteDebugString("StartInitialSpellCasting for char %i, sbkey %s, turn %i, round %i, initiative %i\n",
      self, spellID.UniqueName(), combatData.m_iCurrTurn, combatData.m_iCurrRound, combatData.m_iCurrInitiative);
  };
  forceAttackPose=FALSE;
  combatant_activeSpellKey=-1;
  m_pCharacter->targets.Clear();
  //SetSpellKeyBeingCast(skey, secondaryKey);
  //SetSpellKeyBeingCast(spellName, secondarySpellName);
  SetSpellIDBeingCast(spellID, secondarySpellID);
  //m_itemSpellKeyBeingCast.Clear();
  m_itemSpellIDBeingCast.Clear();
  Wpn_Type = NotWeapon;
  State(ICS_Casting);  
  //if (!CastSpell(GetSpellBookKey(skey, FALSE), secondaryKey))
  //if (!CastSpell(GetSpellBookKey(spellNAme, FALSE), secondarySpellName))
  if (!CastSpell(spellID, secondarySpellID))
  {
    //WriteDebugString("StartInitialSpellCasting: CastSpell() FAILED for %i, sbkey %i\n", self,skey);
    if (logDebuggingInfo)
    {
      WriteDebugString("StartInitialSpellCasting: CastSpell() FAILED for %i, sbkey %s\n", self, spellID.UniqueName());
    };
    //m_spellKeyBeingCast.Clear();
    m_spellIDBeingCast.Clear();
    //m_secondarySpellKeyBeingCast.Clear();
    m_secondarySpellIDBeingCast.Clear();
    EndTurn();
  }
}


//*****************************************************************************
//    NAME: COMBATANT::StartInitialUseSpellCasting
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::StartInitialItemSpellCasting(GLOBAL_SPELL_ID skey)
void COMBATANT::StartInitialItemSpellCasting(const SPELL_ID& spellID)
{
  //WriteDebugString("StartInitialItemSpellCasting for char %i, sbkey %i, turn %i, round %i, initiative %i\n", 
  //                  self,skey, combatData.m_iCurrTurn, combatData.m_iCurrRound, combatData.m_iCurrInitiative);
  if (logDebuggingInfo)
  {
    WriteDebugString("StartInitialItemSpellCasting for char %i, sbkey %s, turn %i, round %i, initiative %i\n",
      self, spellID.UniqueName(), combatData.m_iCurrTurn, combatData.m_iCurrRound, combatData.m_iCurrInitiative);
  };
  forceAttackPose=FALSE;
  combatant_activeSpellKey=-1;
  if (!OnAuto(false))
  {
    m_pCharacter->targets.Clear();
  };
  //SetSpellKeyBeingCast(skey, GLOBAL_SPELL_ID());
  //SetItemSpellKeyBeingCast(skey);
  SetSpellIDBeingCast(spellID, SPELL_ID());
  SetItemSpellIDBeingCast(spellID);
  State(ICS_Casting);  
  //if (!CastItemSpell(skey))
  if (!CastItemSpell(spellID))
  {
    //WriteDebugString("StartInitialItemSpellCasting: CastItemSpell() FAILED for %i, sbkey %i\n", self,skey);
    if (logDebuggingInfo)
    {
      WriteDebugString("StartInitialItemSpellCasting: CastItemSpell() FAILED for %i, sbkey %s\n", self, spellID.UniqueName());
    };
    //m_spellKeyBeingCast.Clear();
    m_spellIDBeingCast.Clear();
    EndTurn();
  }
}

//*****************************************************************************
//    NAME: COMBATANT::EndInitialSpellCasting
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::EndInitialSpellCasting()
{
  if (logDebuggingInfo)
  {
    WriteDebugString("EndInitialSpellCasting() for %i\n", self);
  };
  EndTurn(ICS_Casting);
}


BOOL COMBATANT::FetchSpellBeingCast(CHARACTER_SPELL *pCharSp)
{
  //if (GetSpell(m_spellKeyBeingCast, pcSpell))
  if (FetchCharacterSpell(m_spellIDBeingCast, pCharSp))
    return true;
  // PRS 20111013
  //if (m_itemSpellKeyBeingCast != m_spellKeyBeingCast)
  //{
    //if (m_itemSpellKeyBeingCast.IsValidSpell())
    if (m_itemSpellIDBeingCast.IsValidSpell())
    {
      //pcSpell.spell = m_itemSpellKeyBeingCast;
      if (pCharSp != NULL)
      {
        pCharSp->spellID = m_itemSpellIDBeingCast;
      };
      return true;
    };
  //};
  return false;
}

//*****************************************************************************
//    NAME: COMBATANT::StopCasting
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::StopCasting(BOOL EndYourTurn, bool canFinishCasting)
{
  if (canFinishCasting) return;
  //WriteDebugString("StopCasting() for %i, EndYourTurn %i\n", self,EndYourTurn);
  m_pCharacter->targets.Clear();  
  //m_spellKeyBeingCast.Clear();
  m_spellIDBeingCast.Clear();
  //m_itemSpellKeyBeingCast.Clear();
  m_itemSpellIDBeingCast.Clear();
  Wpn_Type = NotWeapon;
  
  if (State() == ICS_Casting)
    State(ICS_None);

  if (combatant_pendingSpellKey >= 0)
  {
    pendingSpellList.Remove(combatant_pendingSpellKey);
    combatant_pendingSpellKey=-1;
  }
  combatant_activeSpellKey=-1;
  forceAttackPose=FALSE;
  if (EndYourTurn)
    EndTurn(State());
}



// queue up this spell on the pending list
//*****************************************************************************
//    NAME: COMBATANT::CastSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL COMBATANT::CastSpell(GLOBAL_SPELL_ID spell, GLOBAL_SPELL_ID secondarySpell)
BOOL COMBATANT::CastSpell(const SPELL_ID& spellID, const SPELL_ID& secondarySpellID)
{ 
  //WriteDebugString("CastSpell() for %i, sbkey %i\n", self,spell);
  if (logDebuggingInfo)
  {
    WriteDebugString("CastSpell() for %i, sbkey %s\n", self, spellID.UniqueName());
  };
  ASSERT (IsCombatActive() == TRUE);
  SPELL_DATA *pSdata;
  CHARACTER_SPELL charSp;
  //if (!m_pCharacter->GetSpell(spell, cspell)) return FALSE;
  if (!m_pCharacter->FetchCharacterSpell(spellID, &charSp)) return FALSE;
  //if (secondarySpell.IsValidSpell())
  if (secondarySpellID.IsValidSpell())
  {
    //pSdata = spellData.GetSpellData(secondarySpell);
    pSdata = spellData.GetSpell(secondarySpellID);
  }
  else
  {
    //pSdata = spellData.GetSpellData(cspell.spell);
    pSdata = spellData.GetSpell(charSp.spellID);
  };
  if (pSdata==NULL) return FALSE;

  //m_pCharacter->DecMemorized(spell, 1);
  m_pCharacter->DecMemorized(spellID, 1);

  int rnd = GetCurrentRound();//CurrCombatRound();

  PENDING_SPELL data;
  GetContext(&data.caster, spellID);
  //data.gsID = spell;
  data.spellID = spellID;
  //data.gsIDsecondary = secondarySpell;
  data.spellIDSecondary = secondarySpellID;
  data.waitUntil = max(rnd-1, 0);
  data.flags = pSdata->Casting_Time_Type;

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
  //   sctImmediate:  Spell activated immediately
  //   sctInitiative: Casting_Time is added to current initiative. Spell is activated
  //                  when initiative time is reached. Cannot go past current round.
  //   sctRounds:     Casting_Time is added to current round. Spell is activated
  //                  when result round is reached, at end of round.
  //   sctTurns:      Casting_Time*10 is added to current round. Spell is activated
  //                  when result turn is reached, at end of that round.
  //

  pSdata->Casting_Time = max(0, pSdata->Casting_Time);
  switch (pSdata->Casting_Time_Type)
  {
  case sctImmediate:
    data.waitUntil = m_iInitiative; // immediate activation
    break;
  case sctInitiative:
    data.waitUntil = m_iInitiative+pSdata->Casting_Time;
    if (data.waitUntil > INITIATIVE_Never) 
    {
      data.flags = sctRounds;
      // PRS 20120318
      // We could do many things here.
      // But we certainly don't want to wait many rounds.
      // That is what the following line accomplished.
      // We will activate the spell at the end of this round.

      // data.waitUntil += (rnd+1); // wait until next round
      data.waitUntil = rnd;
    }
    if (data.waitUntil == (DWORD)m_iInitiative)
    {
      data.flags = sctImmediate;
      // What was this for?   data.waitUntil = m_iInitiative;
    }
    break;
  case sctRounds:
    data.waitUntil = (rnd+pSdata->Casting_Time);
    if (data.waitUntil == (DWORD)rnd)
    {
      data.flags = sctImmediate;
      data.waitUntil = m_iInitiative;
    }
    break;
  case sctTurns:
    data.waitUntil = (rnd+(pSdata->Casting_Time*10));
    if (data.waitUntil == (DWORD)rnd)
    {
      data.flags = sctImmediate;
      data.waitUntil = m_iInitiative;
    }
    break;
  default:
    die(0xab500);
    data.flags = sctImmediate;
    data.waitUntil = m_iInitiative;
    break;
  }

  if (pSdata->Casting_Time_Type!=sctImmediate)
  {
    //WriteDebugString("CastSpell() add pending spell for %i, sdbkey %i\n", self,data.gsID);
    if (logDebuggingInfo)
    {
      WriteDebugString("CastSpell() add pending spell for %i, sdbkey %s\n", self, data.spellID.UniqueName());
    };
    combatant_pendingSpellKey = pendingSpellList.Add(data);
  }
  else
    combatant_pendingSpellKey=-1;
  return TRUE;
}


// queue up this spell on the pending list
//*****************************************************************************
//    NAME: COMBATANT::CastItemSpell
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL COMBATANT::CastItemSpell(GLOBAL_SPELL_ID spell)
//BOOL COMBATANT::CastItemSpell(GLOBAL_SPELL_ID spell)
BOOL COMBATANT::CastItemSpell(const SPELL_ID& spellID)
{ 
  //WriteDebugString("CastItemSpell() for %i, sbkey %i\n", self,spell);
  WriteDebugString("CastItemSpell() for %i, sbkey %s\n", self,spellID.UniqueName());
  ASSERT (IsCombatActive() == TRUE);

  //SPELL_DATA *pSdata = spellData.GetSpellData(spell);
  SPELL_DATA *pSdata = spellData.GetSpell(spellID);
  if (pSdata==NULL) return FALSE;

  int rnd = GetCurrentRound();//CurrCombatRound();

  PENDING_SPELL data;
  GetContext(&data.caster, spellID);
  //data.gsID = spell;
  data.spellID = spellID;
  data.waitUntil = max(rnd-1, 0);
  data.flags = pSdata->Casting_Time_Type;

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
  //   sctImmediate:  Spell activated immediately
  //   sctInitiative: Casting_Time is added to current initiative. Spell is activated
  //                  when initiative time is reached. Cannot go past current round.
  //   sctRounds:     Casting_Time is added to current round. Spell is activated
  //                  when result round is reached, at end of round.
  //   sctTurns:      Casting_Time*10 is added to current round. Spell is activated
  //                  when result turn is reached, at end of that round.
  //

  pSdata->Casting_Time = max(0, pSdata->Casting_Time);
  switch (pSdata->Casting_Time_Type)
  {
  case sctImmediate:
    data.waitUntil = m_iInitiative; // immediate activation
    break;
  case sctInitiative:
    data.waitUntil = m_iInitiative+pSdata->Casting_Time;
    if (data.waitUntil > INITIATIVE_Never) 
    {
      data.flags = sctRounds;
      // PRS 20120318
      // We could do many things here.
      // But we certainly don't want to wait many rounds.
      // That is what the following line accomplished.
      // We will activate the spell at the end of this round.

      // data.waitUntil += (rnd+1); // wait until next round
      data.waitUntil = rnd+1;
    }
    if (data.waitUntil == (DWORD)m_iInitiative)
    {
      data.flags = sctImmediate;
      // What was this for?   data.waitUntil = m_iInitiative;
    }
    break;
  case sctRounds:
    data.waitUntil = (rnd+pSdata->Casting_Time);
    if (data.waitUntil == (DWORD)rnd)
    {
      data.flags = sctImmediate;
      data.waitUntil = m_iInitiative;
    }
    break;
  case sctTurns:
    data.waitUntil = (rnd+(pSdata->Casting_Time*10));
    if (data.waitUntil == (DWORD)rnd)
    {
      data.flags = sctImmediate;
      data.waitUntil = m_iInitiative;
    }
    break;
  default:
    die(0xab501);
    data.flags = sctImmediate;
    data.waitUntil = m_iInitiative;
    break;
  }

  if (pSdata->Casting_Time_Type!=sctImmediate)
  {
    //WriteDebugString("CastItemSpell() add pending spell for %i, sdbkey %i\n", self,data.gsID);
    WriteDebugString("CastItemSpell() add pending spell for %i, sdbkey %s\n", self,data.spellID.UniqueName());
    combatant_pendingSpellKey = pendingSpellList.Add(data);
  }
  else
    combatant_pendingSpellKey=-1;
  return TRUE;
}



// pending spell is ready for activation
//*****************************************************************************
//    NAME: COMBATANT::SpellActivate
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::SpellActivate(const PENDING_SPELL &data)
{
  //WriteDebugString("SpellActivate() for %i, sdbkey %i\n", self,data.gsID);
  if (logDebuggingInfo)
  {
    WriteDebugString("SpellActivate() for %i, sdbkey %s\n", self, data.spellID.UniqueName());
  };
  ASSERT(State()==ICS_Casting);
  turnIsDone=false;
  QueuedCombatantData &qcomb = GetQueuedCombatants();
  qcomb.PushTail(self, TRUE);
  // no longer valid
  combatant_pendingSpellKey=-1;
}

// doesn't come from spell book
//*****************************************************************************
//    NAME: COMBATANT::InstantSpellActivate
//
// PURPOSE: 
//
//*****************************************************************************
//void COMBATANT::InstantSpellActivate(GLOBAL_SPELL_ID spelldbkey, int target)
void COMBATANT::InstantSpellActivate(const SPELL_ID& attackSpellID, 
                                     const SPELL_ID& itemSpellID,
                                     int target, 
                                     ToHitComputation *pToHitComputation)
{
  //if (!spelldbkey.IsValidSpell()) return;
  if (attackSpellID.IsEmpty() && itemSpellID.IsEmpty()) return;
  if (!attackSpellID.IsEmpty() && !attackSpellID.IsValidSpell()) return;
  if (!itemSpellID.IsEmpty() && !itemSpellID.IsValidSpell()) return;
  if (target == NO_DUDE) return;

  /* Removed 20181109  When Manikus put a spell item in a PC's hands
  if (GetType() != MONSTER_TYPE)
  {
    die(0xab4c1);
    return;
  }
  */

  COMBATANT *pTarget = GetCombatantPtr(target);
  if (pTarget==NULL)
  {
    die (0xab4c2);
    return;
  }
    
  //SPELL_DATA *pSdata = spellData.GetSpellData(spelldbkey);
  SPELL_DATA *pSdata = NULL;
  if (!attackSpellID.IsEmpty())pSdata = spellData.GetSpell(attackSpellID);
  if (!itemSpellID.IsEmpty())  pSdata = spellData.GetSpell(itemSpellID);
  if (pSdata==NULL) 
  {
    die(0xab4c3);
    return;
  } 

  // PRS 19 Dec 2009
  //m_spellKeyBeingCast = GetSpellBookKey(spelldbkey, FALSE);
  //m_spellKeyBeingCast = spelldbkey;
  //m_itemSpellKeyBeingCast.Clear();
  m_spellIDBeingCast = attackSpellID;
  m_itemSpellIDBeingCast = itemSpellID;
  Wpn_Type = NotWeapon;
  {
    int result;
    result = InvokeSpellOnCombatant(self, target, -1, pToHitComputation);
//    if (result < 0)
//    {
//      pToHitComputation->BeginSpellScriptFailure(1);
//    }
  }
}
//*****************************************************************************
// NAME: COMBATANT::UpdateCombatant
//
// PURPOSE:  
//*****************************************************************************
void /* PRS July 2009 BOOL */ COMBATANT::UpdateCombatant()
{
  BOOL onAuto;
  /* this will fill up the log window if used
  WriteDebugString("updating %s combatant %u (%s)\n", 
                   OnAuto() ? "auto" : "manual",
                   self, 
                   CombatantStateText[state]);
  */

  individualCombatantState oldState = State();


  onAuto = OnAuto(true);

  if (onAuto)
  {
    if (m_ICS != ICS_Casting)  // No need to think if already casting
    {
      if (combatData.IsFreeAttacker())
      {
        return;  // No need to Think
      };
      if (!Think()) 
      return; // PRS July 2009 FALSE; 
    };
  };

  //HandleCurrState();

  if ((State() != ICS_None) && (State() != oldState))
  {
    TRACE("done updating %s combatant %u (%s)\n", 
                     onAuto ? "auto" : "manual",
                     self, 
                     CombatantStateText[State()]);
  }
  return; // PRS July 2009TRUE;
}

class THINK_PGM
{
public:
  unsigned char *pgm;
  THINK_PGM(void){pgm = (unsigned char *)malloc(65538);};
  ~THINK_PGM(void){free (pgm);};
};

THINK_PGM thinkProgram;
COMBAT_SUMMARY combatSummary;
void StartCombatSummary(void)
{
  combatSummary.m_thinkCount = 0;
  combatSummary.m_filenum = 0;
}


bool COMBAT_DATA::CanReady(DWORD               rdyLoc,
                           CHARACTER          *pChar, 
                           const ITEM         *pItem,
                           const ITEM_DATA    *pItemData)
{
  int count;
  ActorType actor;
  CString result;
  bool answer;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  pChar->GetContext(&actor);
  //pItemData = itemData.GetItemData(pItem->m_giID);
  scriptContext.SetCharacterContext(pChar);
  scriptContext.SetItemContext(pItemData);
  scriptContext.SetItemContextKey(pItem->key);
  count = 0; // GetReadiedCount(type);
  hookParameters[5].Format("%d", count);
  result = pItemData->RunItemScripts(CAN_READY, 
                                     ScriptCallback_RunAllScripts, 
                                     NULL,
                                     "Combat - can character ready this item");
  answer = TRUE;
  if (result.IsEmpty())
  {
    answer = count == 0;
  }
  else
  {
    if (result[0] != 'Y') answer = FALSE;
  };
  if (!hookParameters[6].IsEmpty())
  {
    CString errorText;
    errorText = hookParameters[6];
    SetMiscError(ErrorText);
  }
  return answer;
}


bool COMBAT_DATA::CanReady(const ITEM *pCharItem, const ITEM_DATA *pItem, CHARACTER *pChar, CString *whyNot)
{
  if (pCharItem->itemID.IsMoney()) 
  {
    if (whyNot != NULL) *whyNot = "Cannot Ready Money";
    return false;
  };
  if (pCharItem->qty <= 0)
  {
    if (whyNot != NULL) *whyNot = "Quantity is not positive";
    return false;
  };
  if (pItem == NULL)
  {
    if (whyNot != NULL) *whyNot = "Cannot find item in item database";
    return false;
  };
  if (pItem->Hands_to_Use > 2) 
  {
    if (whyNot != NULL) *whyNot = "Needs more than 2 hands";
    return false;
  };
#ifdef OldDualClass20180126
  if (!pItem->IsUsableByClass(pChar->GetClass())) 
#else
  if (!pItem->IsUsableByClass(pChar)) 
#endif
  {
    if (whyNot != NULL) *whyNot = "Item not usable by this class";
    return false;
  };
  if (!itemUsesRdySlot(pItem))                
  {
    if (whyNot != NULL) *whyNot = "Item does not use ready slot";
    return false;
  };
  if (!CanReady(pItem->Location_Readied, pChar, pCharItem, pItem))
  {
    if (whyNot != NULL) *whyNot = "Designer 'CanReady' script refuses to ready this item";
    return false;
  }
  return true;
}

void COMBAT_DATA::GetWeaponRange(WEAPON_SUMMARY *pWeaponSummary, const ITEM_DATA *pItem, CHARACTER *pCharacter)
{
  switch (pWeaponSummary->Wpn_Type) 
  {
    case NotWeapon:
    case Ammo:   
      pWeaponSummary->range22 = 0;
      break;
    case SlingNoAmmo:
    case Bow:
    case Crossbow:
    case Throw:
      pWeaponSummary->range22 = pItem->RangeMax;
      break;
    case HandBlunt:
    case HandCutting:
    case HandThrow:
      pWeaponSummary->range22 = pItem->RangeMax;
      break;
    case SpellLikeAbility:
    case SpellCaster:
      {
        if (   ((pItem->Wpn_Type == SpellCaster) || (pItem->Wpn_Type == SpellLikeAbility))
            && (pWeaponSummary->pSpell != NULL))
        {
          DICEPLUS dp;
          dp = const_cast<SPELL_DATA *>(pWeaponSummary->pSpell)->TargetRange();
          pWeaponSummary->range22 = pCharacter->EvalDiceAsClass(dp, 
                                                                pWeaponSummary->pSpell->schoolID, 
                                                                pWeaponSummary->pSpell->Level, 
                                                                NULL);
        }
        else
        {
          pWeaponSummary->range22 = pItem->RangeMax;
        };
      };
      break;
    default:
      WriteDebugString("Bogus item type in IsWeapon()\n");
      pWeaponSummary->range22 = 0;
  };
  if (pWeaponSummary->range22 > 90) pWeaponSummary->range22 = 32767;
  else 
  {
    pWeaponSummary->range22 = 2*pWeaponSummary->range22+1;
    pWeaponSummary->range22 = pWeaponSummary->range22*pWeaponSummary->range22;
  };
}




void COMBAT_DATA::ListWeapons(COMBAT_SUMMARY_COMBATANT *pCSC)
{
  WEAPON_SUMMARY weaponSummary;
  CHARACTER *pCharacter;
  pCSC->weaponSummary.SetSize(0);
  pCharacter = m_aCombatants[pCSC->index].m_pCharacter;

  // The following was inspired (copied, you might say) from CHARACTER::ReadyBestWpn()
  ITEM_DATA *pItem;
  const ITEM *pCharItem;
  POSITION pos = pCharacter->myItems.GetHeadPosition();
  while (pos != NULL)
  {
    pCharItem = &pCharacter->myItems.PeekAtPos(pos);
    if ((pItem = itemData.GetItem(pCharItem->itemID)) != NULL)
    {
      if (pItem->Location_Readied == WeaponHand)
      {
        CString whyNot;
        if (CanReady(pCharItem, pItem, pCharacter, &whyNot))
        {
          weaponSummary.pWeapon = pItem;
          weaponSummary.index = pCharItem->key;
          weaponSummary.Wpn_Type = pItem->Wpn_Type;
          weaponSummary.ammoType = pItem->AmmoType;
          weaponSummary.protection = pItem->Protection_Base + pItem->Protection_Bonus;
          weaponSummary.largeDamageBonus = 10*pItem->Dmg_Bonus_Lg;
          weaponSummary.largeDamageDice  = 
            5*((1 + pItem->Dmg_Dice_Lg) * pItem->Nbr_Dice_Lg);                    
          weaponSummary.smallDamageBonus = 10*pItem->Dmg_Bonus_Sm;
          weaponSummary.smallDamageDice  = 
            5*((1 + pItem->Dmg_Dice_Sm) * pItem->Nbr_Dice_Sm);
          weaponSummary.ROF_Per_Round = pItem->ROF_Per_Round;
          weaponSummary.attackBonus = 10*(pItem->Attack_Bonus);
          weaponSummary.priority = 0;
          if (  (weaponSummary.Wpn_Type == SpellCaster) || (weaponSummary.Wpn_Type == SpellLikeAbility)  )
          {
            weaponSummary.pSpell = spellData.PeekSpell(pItem->SpellID());
            if (weaponSummary.pSpell != NULL)
            {
              weaponSummary.priority = weaponSummary.pSpell->Cast_Priority;
            };
          }
          else if (weaponSummary.Wpn_Type == SpellLikeAbility)
          {
            weaponSummary.pSpell = spellData.PeekSpell(pItem->SpellID());
            if (weaponSummary.pSpell != NULL)
            {
              weaponSummary.priority = weaponSummary.pSpell->Cast_Priority;
            };
          }
          else
          {
            weaponSummary.pSpell = NULL;
          };
          GetWeaponRange(&weaponSummary, pItem, pCharacter);
          pCSC->weaponSummary.Add(weaponSummary);
        }
        else
        {
          CString msg;
          msg.Format("Item name: %s  not considered as a weapon because:\n    %s\n\n",
                      pItem->IdName(), whyNot);
          pCSC->debugSummary.Add(msg);
        };
      };
    };
    pCharacter->myItems.GetNext(pos);
  };
}

void COMBAT_DATA::ListAmmo(COMBAT_SUMMARY_COMBATANT *pCSC)
{
  AMMO_SUMMARY ammoSummary;
  CHARACTER *pCharacter;
  pCSC->ammoSummary.SetSize(0);
  pCharacter = m_aCombatants[pCSC->index].m_pCharacter;

  // The following was inspired (copied, you might say) from COMBAT_DATA::ListWeapons()
  ITEM_DATA *pItem;
  const ITEM *pCharItem;
  POSITION pos = pCharacter->myItems.GetHeadPosition();
  while (pos != NULL)
  {
    pCharItem = &pCharacter->myItems.PeekAtPos(pos);
    if ((pItem = itemData.GetItem(pCharItem->itemID)) != NULL)
    {
      if (pItem->Wpn_Type == Ammo)
      {
        if (CanReady(pCharItem, pItem, pCharacter, NULL))
        {
          ammoSummary.pAmmo = pItem;
          ammoSummary.index = pCharItem->key;
          ammoSummary.ammoType = pItem->AmmoType;
          ammoSummary.qty   = pCharItem->qty;
          ammoSummary.protection = pItem->Protection_Base + pItem->Protection_Bonus;
          ammoSummary.largeDamageBonus  =  10*pItem->Dmg_Bonus_Lg;
          ammoSummary.smallDamageBonus  =  10*pItem->Dmg_Bonus_Sm;
          ammoSummary.largeDamageDice  = 
            5*((1 + pItem->Dmg_Dice_Lg) * pItem->Nbr_Dice_Lg);                    
          ammoSummary.smallDamageDice  = 
            5*((1 + pItem->Dmg_Dice_Sm) * pItem->Nbr_Dice_Sm);                    
          ammoSummary.ROF_Per_Round = pItem->ROF_Per_Round;
          ammoSummary.attackBonus = 10*(pItem->Attack_Bonus);
          ammoSummary.priority = 0;
          //ammoSummary.range22 = pItem->RangeMax;
          //ammoSummary.range22 = 2*ammoSummary.range22+1;
          //ammoSummary.range22 = ammoSummary.range22*ammoSummary.range22;
          pCSC->ammoSummary.Add(ammoSummary);
        };
      };
    };
    pCharacter->myItems.GetNext(pos);
  };
}


struct CELLQUEUE
{
  int x, y;
};

void COMBAT_DATA::ListReachableCells(COMBAT_SUMMARY *pCS, COMBAT_SUMMARY_COMBATANT *pCSC)
{
  // Starting at the combatant's x and y locations we will find
  // every possible cell the combatant can walk to and how many
  // steps it will take to get there.
  // pCS->reachableCells was cleared to 0xff at the start of this
  // combatant's turn.
  CELLQUEUE *pQueue;
  int queueSize;
  int processed;
  int width, height;
  pQueue = (CELLQUEUE *)malloc(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT*sizeof(CELLQUEUE));
  pQueue[0].x = pCSC->x;
  pQueue[0].y = pCSC->y;
  width = pCSC->width;
  height = pCSC->height;
  queueSize = 1;
  processed = 0;
  pCS->reachableCells[MAX_TERRAIN_WIDTH*pQueue[0].y + pQueue[0].x] = 0;
  for (processed=0; processed<queueSize; processed++)
  {
    int dx, dy;
    for (dx=-1; dx<2; dx++) for (dy=-1;dy<2; dy++)
    {
      // Search the 8 cells around this cell.
      int x, y, distance;
      x = pQueue[processed].x + dx;
      y = pQueue[processed].y + dy;
      if (x < 0) continue;
      if (y < 0) continue;
      if (x >= MAX_TERRAIN_WIDTH) continue;
      if (y >= MAX_TERRAIN_HEIGHT) continue;
      if (pCS->reachableCells[MAX_TERRAIN_WIDTH*y + x] >= 0) continue;
      if (OBSTICAL_none != ObsticalType(x, y, width, height, TRUE, FALSE, NULL)) continue;
      pQueue[queueSize].x = x;
      pQueue[queueSize].y = y;
      distance = pCS->reachableCells[MAX_TERRAIN_WIDTH*pQueue[processed].y+pQueue[processed].x];
      pCS->reachableCells[MAX_TERRAIN_WIDTH*y + x] = distance+1;
      queueSize++;
    };
  };
  free(pQueue);
}

void COMBAT_DATA::ListAttacks(COMBAT_SUMMARY_COMBATANT* pCSC, int numAdditionalAttacks)
{
  int atkIdx, numAtk;
  ATTACK_SUMMARY attackSummary;
  const CHARACTER* pCharacter;
  const COMBATANT* pCombatant;
  const MONSTER_DATA* pMonster;
  pCSC->attackSummary.SetSize(0);
  pCharacter = m_aCombatants[pCSC->index].m_pCharacter;


  /* 30 Jan 2020
   * It was discovered that an unfriendly NPC would not attack
   * a party member.  So the test for MONSTER_TYPE had to be
   * removed and somehow replaced with a test for 'not friendly'
   */
   //  if (pCharacter->GetType() != MONSTER_TYPE) return;

  switch (pCharacter->GetType())

  {
  case MONSTER_TYPE:
    pMonster = monsterData.PeekMonster(pCharacter->monsterID);
    if (pMonster == NULL) break;
    if (pCSC->availAttacks + numAdditionalAttacks == 0) break;

    numAtk = pMonster->attackData.GetMonsterAttackDetailsCount();
    for (atkIdx = 0; atkIdx < numAtk; atkIdx++)
    {
      //const MONSTER_ATTACK_DETAILS* pAttack;
      const ATTACK_DETAILS* pAttack;
      pAttack = pMonster->attackData.PeekMonsterAttackDetails(atkIdx);
      attackSummary.index = atkIdx;
      attackSummary.pSpell = NULL;
      if (!pAttack->spellID.IsEmpty())
      {
        attackSummary.pSpell = spellData.PeekSpell(pAttack->spellID);
      };
      attackSummary.damage = 5 * ((1 + pAttack->nbr) * pAttack->sides + 2 * pAttack->bonus); ;
      pCSC->attackSummary.Add(attackSummary);
    };
    break;
  case NPC_TYPE:
    {
      int numDice, numSides, bonus;
      BOOL nonLethal;
      CString spellName;
      if (pCSC->availAttacks + numAdditionalAttacks == 0) break;
      pCombatant = pCharacter->m_pCombatant;
      pCombatant->GetDamageDice(NO_READY_ITEM,
                                &numDice,     // number of dice
                                &numSides,    // sides on dice
                                &bonus,       // damage bonus
                                &nonLethal,   // nonLethal
                                FALSE,        // large target
                                &spellName);  // spell name

      attackSummary.index = -1;
      attackSummary.pSpell = NULL;
      //if (!pAttack->spellID.IsEmpty())
      //{
      //  attackSummary.pSpell = spellData.PeekSpell(pAttack->spellID);
      //};
      attackSummary.damage = 5 * ((1 + numDice) * numSides + 2 * bonus); ;
      pCSC->attackSummary.Add(attackSummary);
    };
    break;
  default:
    NotImplemented(0x551b8c, false);
    break;
  };
}




void COMBAT_DATA::ListCombatants(COMBAT_SUMMARY *pCombatSummary, 
                                 COMBATANT *pActiveCombatant, 
                                 COMBATANT *pTarget)
{
  int i, j, k;
  BOOL activeNonFriendly;
  COMBAT_SUMMARY_COMBATANT CSC;
  COMBAT_SUMMARY_COMBATANT *pCSC;
  pCombatSummary->m_combatants.SetSize(0);
  if (pCombatSummary->m_thinkCount == 2)
  {
    ASSERT(TRUE);
  };
  j = 1;
  activeNonFriendly = !pActiveCombatant->GetIsFriendly();
  for (i=0; i<NumCombatants(); i++)
  {
    COMBATANT *pCombatant;
    CHARACTER *pCharacter;
    charStatusType status;
    individualCombatantState ICS;
    pCombatant = &m_aCombatants[i];
    if (   (pCombatant != pActiveCombatant)
        && (pCombatant != pTarget)
        && (pTarget != NULL)) 
    {
      continue;
    };
    pCharacter = pCombatant->m_pCharacter;
    CSC.pCharacter = pCharacter;
    CSC.index = i;
    CSC.fleeing = (pCombatant->iFleeingFlags == 0)?0:1;
    CSC.isLarge = pCombatant->isLargeDude()?1:0;
    ICS = pCombatant->State();
    //WriteDebugString("DEBUG - %s is %d\n", pCharacter->GetName(), ICS);
    status = pCharacter->GetAdjStatus();
    switch(status)
    {
    case Dead:
      ICS = ICS_Dead;
      break;
    case Okay:
      break;
    case Gone:
      ICS = ICS_Gone;
      break;
    case Fled:
      ICS = ICS_Fled;
      break;
    case Running:
      ICS = ICS_Fleeing;
      break;
    case Dying:
      ICS = ICS_Dying;
      break;
    case Unconscious:
      ICS = ICS_Unconscious;
      break;
    case Petrified:
      ICS = ICS_Petrified;
      break;
    default:
      die(0x21ac);
    };
    CSC.state = ICS;
    CSC.availAttacks = pCombatant->availAttacks;
    if (pCombatant->charPetrified())
    {
      CSC.state = ICS_Petrified;
    };
    if (pCombatant->GetStatus() == Dying)
    {
      CSC.state = ICS_Dying;
    };
    if (pCombatant->GetStatus() == Dead)
    {
      CSC.state = ICS_Dead;
    };
    CSC.friendly = pCombatant->GetIsFriendly() ^ activeNonFriendly;
    if (pCombatant == pActiveCombatant) k = 0;
    else k = j++;
    pCombatSummary->m_combatants.SetAtGrow(k, CSC);
    pCSC = pCombatSummary->GetCombatant(k);
    pCombatant->ListShields(pCSC);
    pCSC->x = pCombatant->x;
    pCSC->y = pCombatant->y;
    pCSC->canMove = pCombatant->CanMove(FALSE);
    pCharacter->ComputeAIBaseclass();
    pCSC->AIBaseclass = pCharacter->AIBaseclass; 
  };
}

void COMBAT_DATA::ListActionsByAmmo(COMBAT_SUMMARY  *pCombatSummary,
                                    const WEAPON_SUMMARY *pWeapon,
                                    COMBAT_SUMMARY_ACTION *pCSA)
{
  int i, numAmmo;
  const COMBAT_SUMMARY_COMBATANT *pCSC;
  pCSC = pCombatSummary->PeekCombatant(0);
  if (pWeapon->ammoType.IsEmpty())
  {
    if (RunRangedWeaponFilter(pCombatSummary, pCSA) != 0) return;
    pCSA->damage =  pCSA->pHe->isLarge
                   ?(pWeapon->largeDamageDice + pWeapon->largeDamageBonus)
                   :(pWeapon->smallDamageDice + pWeapon->smallDamageBonus);
    pCombatSummary->m_availableActions.Add(*pCSA);
    return;
  };
  numAmmo = pCSC->GetAmmoCount();
  for (i=0; i<numAmmo; i++)
  {
    const AMMO_SUMMARY *pAmmo;
    int large;
    pAmmo = pCSC->PeekAmmo(i);
    if (pAmmo->ammoType != pWeapon->ammoType) continue;
    if (pAmmo->qty <= 0) continue;
    large = pCSA->pHe->isLarge;
    pCSA->ammoOrd = i+1;
    pCSA->damage =  pCSA->pHe->isLarge
                   ?(pAmmo->largeDamageDice + pAmmo->largeDamageBonus + pWeapon->largeDamageBonus) 
                   :(pAmmo->smallDamageDice + pAmmo->smallDamageBonus + pWeapon->smallDamageBonus);
    switch (pCSA->actionType)
    {
    case AT_RangedWeapon: 
      {
        if (RunRangedWeaponFilter(pCombatSummary, pCSA)) continue;
        break;
      };
    default:
      die(0x3cc10);
    };
    pCombatSummary->m_availableActions.Add(*pCSA);
  };
}



void COMBAT_DATA::ListActionsByWeapon(COMBAT_SUMMARY  *pCombatSummary,
                                      int              activeCombatantIndex,
                                      int              targetCombatantIndex,
                                      int              weaponIndex,
                                      bool             friendly,
                                      int              distance22,
                                      bool             judoMeleeOnly)
{
  COMBAT_SUMMARY_ACTION csa;
  const WEAPON_SUMMARY *pWeapon;
  const ITEM_DATA *pItemData;
  const SPELL_DATA *pSpellData;



  pWeapon = pCombatSummary->PeekCombatant(activeCombatantIndex)->PeekWeapon(weaponIndex);
  pItemData = pWeapon->pWeapon;
  pSpellData = pWeapon->pSpell;

  //if (distance2 > pWeapon->range2) return;
  csa.targetOrd = targetCombatantIndex+1;

  if ((pCombatSummary->m_thinkCount == 2) && (csa.targetOrd == 2))
  {
    ASSERT(TRUE);
  }

  csa.weaponOrd = weaponIndex+1;
  csa.ammoOrd = 0;
  csa.pMe = pCombatSummary->GetCombatant(activeCombatantIndex);
  csa.pHe = pCombatSummary->GetCombatant(targetCombatantIndex);
  csa.distance22 = distance22;

  if ((pItemData!=NULL) && (pItemData->Wpn_Type == SpellCaster))
  {
    if (judoMeleeOnly) return;
    if (pSpellData == NULL) return;
/*
    if (!pSpellData->CanTargetEnemy)
    {
      if (!friendly) return;
    }
    if (!pSpellData->CanTargetFriend)
    {
      if (friendly) return;
    };
*/
    csa.actionType = AT_SpellCaster;
    if (RunSpellCasterFilter(pCombatSummary, &csa) != 0) return;
  }
  else if ((pItemData!=NULL) && (pItemData->Wpn_Type == SpellLikeAbility))
  {
    if (pSpellData == NULL) return;
    if (judoMeleeOnly) return;
/*
    if (!pSpellData->CanTargetEnemy)
    {
      if (!friendly) return;
    }
    if (!pSpellData->CanTargetFriend)
    {
      if (friendly) return;
    };
*/
    csa.actionType = AT_SpellLikeAbility;
    if (RunSpellLikeAbilityFilter(pCombatSummary, &csa) != 0) return;
  }
  else
  {
    if (friendly) return;
    if (pWeapon->range22 > 9)
    {
      if (judoMeleeOnly) return;
      csa.actionType = AT_RangedWeapon;
      ListActionsByAmmo(pCombatSummary, pWeapon, &csa);
      return;
    }
    else
    {
      csa.actionType = AT_MeleeWeapon;
      if (RunMeleeWeaponFilter(pCombatSummary, &csa) != 0)
      {
        CString msg;
        msg.Format("Item name: %s\n"
                   "    Not considered as a Melee weapon because\n"
                   "    designer script 'Melee Weapon Filter' rejected it.", 
                   pItemData->ItemID());
        pCombatSummary->GetCombatant(activeCombatantIndex)->debugSummary.Add(msg);
        return;
      };
    };
  };
  pCombatSummary->m_availableActions.Add(csa);
}

void COMBAT_DATA::ListActionsByAttack(COMBAT_SUMMARY  *pCombatSummary,
                                      int              activeCombatantIndex,
                                      int              targetCombatantIndex,
                                      int              attackIndex,
                                      bool             friendly,
                                      int              distance22)
{
  COMBAT_SUMMARY_ACTION csa;
  const ATTACK_SUMMARY *pAttack;
  const SPELL_DATA *pSpellData;
  if (targetCombatantIndex == activeCombatantIndex) return;

  pAttack = pCombatSummary->PeekCombatant(activeCombatantIndex)->PeekAttack(attackIndex);
  pSpellData = pAttack->pSpell;

  if ( /*(pSpellData == NULL) && */ ( distance22 > 8) ) return;
  if (pCombatSummary->m_thinkCount == 2)
  if (targetCombatantIndex == 17)
    ASSERT(TRUE);
  csa.actionType = AT_Judo;
  csa.targetOrd = targetCombatantIndex+1;
  csa.attackOrd = attackIndex+1;
  csa.pMe = pCombatSummary->GetCombatant(activeCombatantIndex);
  csa.pHe = pCombatSummary->GetCombatant(targetCombatantIndex);
  csa.distance22 = distance22;
  if (RunJudoFilter(pCombatSummary, &csa)) return;
  pCombatSummary->m_availableActions.Add(csa);
}


void COMBAT_DATA::ListMoveAction(COMBAT_SUMMARY  *pCombatSummary,
                                 int              activeCombatantIndex,
                                 int              targetCombatantIndex,
                                 bool             friendly,
                                 int              distance22)
{
  COMBAT_SUMMARY_ACTION csa;
  if (pCombatSummary->m_thinkCount == 17)
  {
    ASSERT(TRUE);
  };
  csa.actionType = AT_Advance;
  csa.attackOrd  = 0;
  csa.distance22 = distance22;
  csa.targetOrd  = targetCombatantIndex+1;
  csa.weaponOrd  = 0;
  csa.advance    = 1;
  csa.pMe = pCombatSummary->GetCombatant(activeCombatantIndex);
  csa.pHe = pCombatSummary->GetCombatant(targetCombatantIndex);
  if (csa.pMe->canMove == 0) return;
  if (RunAdvanceFilter(pCombatSummary, &csa)) return;
  pCombatSummary->m_availableActions.Add(csa);
}

int Distance22(const COMBATANT *pCombatant1, const COMBATANT *pCombatant2)
{
  int  x1L, x2L, y1T, y2T, x1R, x2R, y1B, y2B, h1, h2, w1, w2;
  x1L = pCombatant1->x;
  x2L = pCombatant2->x;
  y1T = pCombatant1->y;
  y2T = pCombatant2->y;
  h1 = pCombatant1->height;
  h2 = pCombatant2->height;
  w1 = pCombatant1->width;
  w2 = pCombatant2->width;
  x1R = x1L + w1 -1;
  x2R = x2L + w2 -1;
  y1B = y1T + h1 -1;
  y2B = y2T + h2 -1;
  if (x1L < x2L)
  {
    if (x1R < x2L) x1L = x1R;
    else x1L = x2L;
  }
  else 
  {
    if (x1L > x2R) x2L = x2L;
    else x2L = x1L;
  };
  if (y1T < y2T)
  {
    if (y1B < y2T) y1T = y1B;
    else y1T = y2T;
  }
  else 
  {
    if (y1T > y2B) y2T = y2T;
    else y2T = y1T;
  };
  return 4*((x1L-x2L)*(x1L-x2L) + (y1T-y2T)*(y1T-y2T));
}


void COMBAT_DATA::ListActionsForTarget(COMBAT_SUMMARY *pCombatSummary,
                                       int             activeCombatant,
                                       int             targetIndex,
                                       bool            friendly,
                                       int             numAdditionalAttacks,
                                       bool            judoMeleeOnly)
{
  COMBATANT *pTarget, *pActive;
  int wpnIndex, atkIndex;
  int numWpn, numAtk, distance22;
  pTarget = &m_aCombatants[pCombatSummary->GetCombatant(targetIndex)->index];
  pActive = &m_aCombatants[pCombatSummary->PeekCombatant(activeCombatant)->index];
  distance22 = Distance22(pActive, pTarget);
  numWpn = pCombatSummary->GetCombatant(0)->GetWeaponCount();
  if ( pActive->availAttacks + numAdditionalAttacks > 0)
  {
    for (wpnIndex=0; wpnIndex<numWpn; wpnIndex++)
    {
      WEAPON_SUMMARY *pWeapon;
      pWeapon = pCombatSummary->GetCombatant(0)->GetWeapon(wpnIndex);
      ListActionsByWeapon(pCombatSummary, 
                          activeCombatant, 
                          targetIndex, 
                          wpnIndex, 
                          friendly, 
                          distance22,
                          judoMeleeOnly);
    };
  };
  numAtk = pCombatSummary->GetCombatant(0)->GetAttackCount();
  for (atkIndex=0; atkIndex<numAtk; atkIndex++)
  {
    ATTACK_SUMMARY *pAttack;
    pAttack = pCombatSummary->GetCombatant(0)->GetAttack(atkIndex);
    ListActionsByAttack(pCombatSummary, activeCombatant, targetIndex, atkIndex, friendly, distance22);
  }
  {
    // the following test was removed 20161003.
    //Here is the problem.  _FOUR has no attacks left.  Perhaps he used
    //them in a free attack on Warrior.  So he cannot attack.
    //
    //All that is left is for him to Advance.  But the engine won't let
    //him advance on Warrior because he is adjacent to Warrior.
    //So he advances on Knight.  Then he advances on Warrior who
    //is closer.  Etc.
    //
    //How to fix this?  The easiest way for me is to allow a combatant
    //to 'Advance' towards an adjacent combatant.  The script would
    //choose the closest of several and the engine would 'do nothing'
    //if told to advance on an adjacent combatant and turn it into 'Guard'.
    //if (distance22 > 8)
    {
      ListMoveAction(pCombatSummary, activeCombatant, targetIndex, friendly, distance22);
    };
  };
}

void COMBAT_DATA::ListActions(COMBAT_SUMMARY *pCombatSummary,
                              int activeCombatant, 
                              int numAdditionalAttacks,
                              bool judoMeleeOnly)
{
  // List everything this combatant can do.
  int numCombatants, curTarget;
  pCombatSummary->m_availableActions.SetSize(0);
  numCombatants = pCombatSummary->GetCombatantCount();
  for (curTarget=0; curTarget<numCombatants; curTarget++)
  {
    bool friendly;
    COMBATANT *pActive;
    COMBATANT *pTarget;
    pActive = &m_aCombatants[pCombatSummary->PeekCombatant(activeCombatant)->index];
    pTarget = &m_aCombatants[pCombatSummary->PeekCombatant(curTarget)->index];
    friendly = !(pActive->GetIsFriendly() ^ pTarget->GetIsFriendly());
    ListActionsForTarget(pCombatSummary, 
                         activeCombatant, 
                         curTarget, 
                         friendly, 
                         numAdditionalAttacks,
                         judoMeleeOnly);
  };
}



void DumpWeapon(FILE *f, int weaponOrdinal, int activeCombatant, int targetOrdinal, COMBAT_SUMMARY *pCombatSummary)
{
//  int range2;
  const WEAPON_SUMMARY *pWeapon;
  const COMBATANT *pTarget;
  weaponClassType wpnType;
  CString weaponTypeText, weaponName;
  pTarget = combatData.GetCombatant(pCombatSummary->PeekCombatant(targetOrdinal-1)->index);
  fprintf(f, "  Weapon = weapon  #%d\n", weaponOrdinal);
  if (weaponOrdinal == 0)
  {
    pWeapon = NULL;
    weaponName = "No Weapon";
    wpnType = NotWeapon;
  }
  else
  {
    pWeapon = pCombatSummary->PeekCombatant(activeCombatant)->PeekWeapon(weaponOrdinal-1);
    if (pWeapon->pWeapon == NULL) 
    {
      weaponName = "No Weapon";
      wpnType = NotWeapon;
    }
    else
    {
      weaponName = pWeapon->pWeapon->UniqueName();
      wpnType = pWeapon->Wpn_Type;
    };
  };
  switch (wpnType)
  {
    case NotWeapon:        weaponTypeText = "No Weapon"; break;
    case HandBlunt:        weaponTypeText = "Hand Blunt"; break;
    case HandCutting:      weaponTypeText = "Hand Cutting"; break;
    case HandThrow:        weaponTypeText = "Hand Throw"; break;
    case SlingNoAmmo:      weaponTypeText = "Sling No Ammo"; break;
    case Bow:              weaponTypeText = "Bow"; break;
    case Crossbow:         weaponTypeText = "Crossbow"; break;
    case Throw:            weaponTypeText = "Throw"; break;
    case Ammo:             weaponTypeText = "Ammo"; break;
    case SpellCaster:      weaponTypeText = "Spell Caster"; break;
    case SpellLikeAbility: weaponTypeText = "Spell-like Ability"; break;
  };
  if (pWeapon == NULL) return;
  fprintf(f, "    Weapon Type      = %s"   , (LPCSTR)weaponTypeText);
  fprintf(f,                  "  [%s]\n"   , (LPCSTR)weaponName);
  fprintf(f, "    2*range squared  = %5d\n", pWeapon->range22);
  fprintf(f, "    protection       = %5d\n", pWeapon->protection);
  fprintf(f, "    damage(x10)      = %5d\n",   pTarget->isLargeDude()
                                             ?(pWeapon->largeDamageDice+pWeapon->largeDamageBonus)
                                             :(pWeapon->smallDamageDice+pWeapon->smallDamageBonus));
  fprintf(f, "    ROF              = %5d\n", pWeapon->ROF_Per_Round);
  fprintf(f, "    attackBonus(x10) = %5d\n", pWeapon->attackBonus);
  fprintf(f, "    priority         = %5d\n", pWeapon->priority);
}


void DumpAmmo(FILE *f, int ammoOrdinal, int activeCombatant, int targetOrdinal, COMBAT_SUMMARY *pCombatSummary)
{
//  int range2;
  const AMMO_SUMMARY *pAmmo;
  const COMBATANT *pTarget;
  CString ammoName;
  pTarget = combatData.GetCombatant(pCombatSummary->PeekCombatant(targetOrdinal-1)->index);
  fprintf(f, "  Ammo = ammo  #%d  ", ammoOrdinal);
  if (ammoOrdinal == 0)
  {
    pAmmo = NULL;
    ammoName = "No Ammo";
  }
  else
  {
    pAmmo = pCombatSummary->PeekCombatant(activeCombatant)->PeekAmmo(ammoOrdinal-1);
    if (pAmmo->pAmmo == NULL) 
    {
      ammoName = "No Ammo";
    }
    else
    {
      ammoName = pAmmo->pAmmo->UniqueName();
    };
  };
  fprintf(f,                     "  [%s]\n", (LPCSTR)ammoName);
  if (pAmmo == NULL) return;
  fprintf(f, "    quantity         = %5d\n", pAmmo->qty);
  fprintf(f, "    protection       = %5d\n", pAmmo->protection);
  fprintf(f, "    damage(x10)      = %5d\n",  pTarget->isLargeDude()
                                             ?(pAmmo->largeDamageDice+pAmmo->largeDamageBonus)
                                             :(pAmmo->smallDamageDice+pAmmo->smallDamageBonus));
  fprintf(f, "    ROF              = %5d\n", pAmmo->ROF_Per_Round);
  fprintf(f, "    attackBonus(x10) = %5d\n", pAmmo->attackBonus);
  fprintf(f, "    priority         = %5d\n", pAmmo->priority);
}



void DumpAttack(FILE *f, int attackOrdinal, int activeCombatant, int targetOrdinal, COMBAT_SUMMARY *pCombatSummary)
{
  const COMBATANT *pTarget;
  const ATTACK_SUMMARY *pAttack;
  pTarget = combatData.GetCombatant(pCombatSummary->PeekCombatant(targetOrdinal-1)->index);
  if (attackOrdinal == 0)
  {
    die(0x44fa); //"Attack ordinal == ");
  };
  fprintf(f, "    Attack without weapon\n");
  pAttack = pCombatSummary->PeekCombatant(activeCombatant)->PeekAttack(attackOrdinal-1);
  if (pAttack->pSpell == NULL) fprintf(f, "    No Spell\n");
  else fprintf(f, "    Spell = %s\n", (LPCSTR)pAttack->pSpell->Name);
  fprintf(f, "    Damage = %d\n", pAttack->damage);

}


void DumpCombatant(FILE *f, int ordinal, COMBAT_SUMMARY *pCombatSummary)
{
  const COMBATANT *pCombatant;
  const COMBAT_SUMMARY_COMBATANT *pCSC;
  const char *asciiState;
  pCSC = pCombatSummary->PeekCombatant(ordinal-1);
  pCombatant = combatData.GetCombatant(pCSC->index);
  fprintf(f,"[%s]\n", (LPCSTR)pCombatant->m_pCharacter->GetName());
  fprintf(f,"    x = %d\n", pCombatant->x);
  fprintf(f,"    y = %d\n", pCombatant->y);
  fprintf(f,"    fleeing = %d\n", pCSC->fleeing);
  switch (pCSC->state)
  {
    case ICS_None: asciiState = "None"; break;
    case ICS_Casting: asciiState = "Casting"; break;
    case ICS_Attacking: asciiState = "Attacking"; break;
    case ICS_Guarding:  asciiState = "Guarding"; break;
    case ICS_Bandaging:  asciiState = "Bandaging"; break;
    case ICS_Using: asciiState = "Using"; break;
    case ICS_Moving: asciiState = "Moving"; break;
    case ICS_Turning: asciiState = "Turning"; break;
    case ICS_Fleeing: asciiState = "Fleeing"; break;
    case ICS_Fled: asciiState = "Fled"; break;
    case ICS_ContinueGuarding: asciiState = "ContinueGuarding"; break;
    case ICS_Petrified: asciiState = "Petrified"; break;
    case ICS_Dying: asciiState = "Dying"; break;
    case ICS_Unconscious: asciiState = "Unconscious"; break;
    case ICS_Dead: asciiState = "Dead"; break;
    case ICS_Gone: asciiState = "Gone"; break;
    default: asciiState = "???"; break;
  };
  fprintf(f,"    state = %s\n", asciiState);
  if (pCSC->friendly) fprintf(f,"    friendly\n"); else fprintf(f,"    unfriendly\n");
}

void DumpAdvance(FILE *f, const COMBAT_SUMMARY_ACTION *pAction)
{
  //fprintf(f, "Advance toward target\n");
}

const char *typeName[] = {
  "Unknown",
  "SpellCaster",
  "Advance",
  "Range Weapon",
  "Melee Weapon",
  "Judo",
  "Spell-like Ability"
};

void DumpAction(FILE *f, int i, int action, COMBAT_SUMMARY *pCombatSummary)
{
  const COMBAT_SUMMARY_ACTION *pAction;
  const COMBAT_SUMMARY_COMBATANT *pCOMBATANT;
  int   targetOrdinal;
  pAction = pCombatSummary->PeekAction(action);
  targetOrdinal = pAction->targetOrd;
  pCOMBATANT = pCombatSummary->PeekCombatant(targetOrdinal-1);
  fprintf(f, "Action number %d  ;  Action Type = %s\n", i, typeName[pAction->actionType]);
  fprintf(f, "  damage = %d\n", pAction->damage);
  fprintf(f, "  Target = combatant #%d  ", pCombatSummary->PeekCombatant(pAction->targetOrd-1)->index);
  DumpCombatant(f, pAction->targetOrd, pCombatSummary);
  fprintf(f, "    2*distance(squared) = %d\n", pAction->distance22);
  fprintf(f, "    advance = %d\n", pAction->advance);
  if (pAction->weaponOrd == 0)
  {
    if (pAction->attackOrd == 0)
    {
      if (pAction->advance != 0)
      {
        DumpAdvance(f, pAction);
      }
      else
      {
        /* Really */ NotImplemented(0x221fd, false);
      };
    }
    else
    {
      DumpAttack(f, pAction->attackOrd, 0, pAction->targetOrd, pCombatSummary);
    };
  }
  else
  {
    DumpWeapon(f, pAction->weaponOrd, 0, pAction->targetOrd, pCombatSummary);
    DumpAmmo(f, pAction->ammoOrd, 0, pAction->targetOrd, pCombatSummary);
  };
}

void DumpActions(CArray<int,int> *index, COMBAT_SUMMARY *pCombatSummary, bool evaluated)
{
  if (LogCombatActions)
  {
    int action;
    CString fname;
    COMBATANT *pCombatant;
    pCombatant = combatData.GetCombatant(pCombatSummary->PeekCombatant(0)->index);
    fname.Format("%sCombatActions%04d.txt",rte.DesignDir(),pCombatSummary->m_filenum);
    pCombatSummary->m_filenum++;
    FILE *f;
    f = fopen(fname, "w");
    if (f == NULL)
    {
      static bool once = false;
      if (once) return;
      once = true;
      WriteDebugString("Cannot open 'DumpActions.txt' for writing\n");
      return;
    };
    fprintf(f, "%d Possible Actions for combatant %d  ***[%s]*****************\n",
                   pCombatSummary->m_thinkCount,
                   pCombatant->self,
                   (LPCSTR)pCombatant->GetName());
    fprintf(f, "Time = 0x%08llx\n", virtualGameTime);
    if (!evaluated) fprintf(f, "Actions were not evaluated or sorted.  Probably a free/guard attack\n");
    fprintf(f, " x = %d\n", pCombatant->x);
    fprintf(f, " y = %d\n", pCombatant->y);
    for (action=0; action<index->GetSize(); action++)
    {
      DumpAction(f, action, (*index)[action], pCombatSummary);
    };
    {
      int i;
      for (i=0; i<pCombatSummary->GetCombatant(0)->GetDebugItemCount(); i++)
      {
        CString msg;
        msg = *pCombatSummary->GetCombatant(0)->PeekDebugItem(i);
        fprintf(f,"%s\n", (LPCSTR)msg);
      };
    };
    fclose(f);
  };
}

bool LoadAI_Script(void)
{
  static bool kernelError = false;
  if (!kernelError)
  {
    if (ExpandKernel())
    {
      FetchKernel(thinkProgram.pgm);
    }
    else
    {
      kernelError = true;
      debugSeverity = 5;
      WriteDebugString("***Error in combat THINKING script***\n");
    };
  };
  return !kernelError;
};



//*****************************************************************************
// NAME:    COMBATANT::Think
//
// PURPOSE: 
//*****************************************************************************
// 
// can return one of these states
/*
  CS_Moving
  CS_Guarding
  CS_Attacking
*/
BOOL COMBATANT::Think()
{//8a0c
  // no user input allowed while dude is on auto
  CArray<int, int> actionIndex;
  bool useScriptedAI;
  int bestWeaponIdx;
  if (IsPartyMember())
    EnableUserInput(TRUE);
  else
  {
    EnableUserInput(FALSE);
  };
  TRACE("thinking for auto combatant %u (%s)\n", self, CombatantStateText[State()]);

  // make sure current path has not been exhausted
  if (hPath >= 0)
  {
    if (pathMgr.GetStepsRemaining(hPath)==0)
      ClearPath();
  }  

  if (iFleeingFlags != 0)
  {
    State(ICS_Moving);
    TRACE("thinking result for auto combatant %u (%s=fleeing)\n", self, CombatantStateText[State()]);
    // find path to closest map edge

    BOOL fled=FALSE;
    // if already on map edge, flee off the map
    if (   (x==0) || (x==(MAX_TERRAIN_WIDTH-1))
        || (y==0) || (y==(MAX_TERRAIN_HEIGHT-1)) )
    {
      fled=TRUE;
    }

    if (fled) 
    {
      if (friendly)
        IncNumFriendFlee(); //combatData.numFrndFlee++;
      else
        IncNumMonsterFlee(); //combatData.numMonFlee++;

      menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
      menu.setHorzOrient();
      menu.setUseActive(FALSE); // no highlighting      

      CString msg("");
      msg.Format("%s has fled...", GetName());
      menu.changeMenuItem(1, msg);
      UpdateCombatTimeDelayMsg();
      theApp.AppSleep(GetMsgTimeDelay()); // give time to see normal icon      

      //BUGFIX EndTurn();
      SetStatus(Fled);
      placeCombatant(x, y, NO_DUDE, width, height);
      TRACE("combatant %i has fled the map\n", self);
      State(ICS_Fled); //BUGFIX
      EnableUserInput(false);
      return TRUE;
    }
    if (CanMove(FALSE))
    {
      if (hPath < 0)
      {
        // find path to farthest corner of room away from cleric that turned me
        if (!FindPathAwayFrom(m_iLastAttacker))    
        {
          WriteDebugString("Fleeing combatant %i failed to find exit path\n", self);
          State(ICS_Guarding);
        }
      }
    }
    else
      State(ICS_Guarding);    
    return TRUE;
  }

  if (isTurned)
  {
    State(ICS_Moving);
    TRACE("thinking result for auto combatant %u (%s=turned/fleeing %i)\n", 
                     self, CombatantStateText[State()], m_iLastAttacker);

    BOOL fled=FALSE;
    // if already on map edge, flee off the map
    if (   (x==0) || (x==(MAX_TERRAIN_WIDTH-1))
        || (y==0) || (y==(MAX_TERRAIN_HEIGHT-1)) )
    {
      fled=TRUE;
    }

    if (fled) 
    {
      if (friendly)
        IncNumFriendFlee(); //combatData.numFrndFlee++;
      else
        IncNumMonsterFlee(); //combatData.numMonFlee++;

      menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
      menu.setHorzOrient();
      menu.setUseActive(FALSE); // no highlighting      

      CString msg("");
      msg.Format("%s has fled...", GetName());
      menu.changeMenuItem(1, msg);
      UpdateCombatTimeDelayMsg();
      theApp.AppSleep(GetMsgTimeDelay()); // give time to see normal icon      

      // BUGFIX EndTurn(ICS_Fled);
      SetStatus(Fled);
      State(ICS_Fled); // BUGFIX
      placeCombatant(x, y, NO_DUDE, width, height);
      TRACE("Turned Undead %i has fled the map\n", self);
      return TRUE;
    }
    if (CanMove(FALSE))
    {
      if (hPath < 0)
      {
        // find path to farthest corner of room away from cleric that turned me
        if (!FindPathAwayFrom(m_iLastAttacker))    
        {
          WriteDebugString("Fleeing combatant %i failed to find exit path\n", self);
          State(ICS_Guarding);
        }
      }
    }
    else
      State(ICS_Guarding);
    return TRUE;
  }

  // make sure current target is still on the map
  useScriptedAI = false;
  m_pCharacter->ReadyWeaponScript(NO_READY_ITEM);
  m_pCharacter->ReadyShieldScript(NO_READY_ITEM);
  {
    if (LoadAI_Script())
    {
      combatSummary.m_thinkCount++;
      combatSummary.Clear();  // Does not clear thinkcount
      combatData.ListCombatants(&combatSummary, this, NULL);
      combatData.ListWeapons(combatSummary.GetCombatant(0)); // List weapons for attacker only.
      combatData.ListAmmo(combatSummary.GetCombatant(0));    // List ammo for attacker only.
      combatData.ListAttacks(combatSummary.GetCombatant(0), 0); // List attacks for attacker only.
      combatData.ListReachableCells(&combatSummary, combatSummary.GetCombatant(0)); // Number of steps to each cell.
      combatData.ListActions(&combatSummary, 0, 0, false);
      // Compare all the actions to find the very best one.
      // We could simply go down the list from one end to
      // the other, keeping track of the best one found and
      // comparing the current best to the next.
      // But let us instead place the actions in a binary 
      // tree such that at each node, the action at the top
      // is better than the two actions immediately below.
      // Then we can easily extract several actions if we
      // would like to choose randomly from the three best, 
      // for example.
      {
        int i, j, k;
        actionIndex.SetSize(combatSummary.GetActionCount());
        for (i=0; i<combatSummary.GetActionCount(); i++)
        {
          actionIndex.SetAt(i,i);
          // Move the new entry towards the top of the tree.
          j = i;
          while (j > 0)
          {
            int temp;
            k = (j-1)/2;
            combatSummary.pActionA = combatSummary.GetAction(actionIndex[j]);
            combatSummary.pActionB = combatSummary.GetAction(actionIndex[k]);
            if (RunTHINK(thinkProgram.pgm, &combatSummary) <= 0) break;
            temp = actionIndex[j];
            actionIndex[j] = actionIndex[k];
            actionIndex[k] = temp;
            j = k;
          };
        };
        DumpActions(&actionIndex, &combatSummary, true);
        if (combatSummary.GetActionCount() > 0)
        {
          useScriptedAI = true;
        }
        else
        {
          State(ICS_Guarding);
          TRACE("thinking result for auto combatant %u (%s) - no actions\n", self, CombatantStateText[State()]);
          return TRUE;
        };
      };
    };
  };

  int dude = NO_DUDE;

  if (useScriptedAI)
  {
    if (combatSummary.m_thinkCount == 20)
    {
      ASSERT(TRUE);
    }
    ACTION_TYPE actionType = combatSummary.PeekAction(actionIndex[0])->actionType;
    switch (actionType)
    {
    case AT_SpellLikeAbility:
    case AT_SpellCaster:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        int itemIdx;
        combattargets.RemoveAll();
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        itemIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd-1)->index;
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        m_pCharacter->ReadyWeaponScript(itemIdx);
        AddTarget(dude, false);
        ReadyBestArmor();
        {
          State(ICS_Attacking);
          //ITEM_ID itemID;
          itemIdx = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
          //if (itemIdx != NO_READY_ITEM)
          //{
            //itemID = m_pCharacter->myItems.GetItem(itemIdx);
            //weaponClassType weaponType;
            //weaponType = itemData.GetWpnType(itemID);
            //if (weaponType == SpellCaster)
            //{
              if (m_pCharacter->myItems.GetCharges(itemIdx) > 0)
              {
                const ITEM_DATA *pItem;
                //SPELL_ID spellID;
                //pItem = itemData.GetItem(itemID);
                pItem = combatSummary.PeekCombatant(0)->PeekWeapon(pCSA->weaponOrd-1)->pWeapon;
                //spellID = pItem->spellID;
                //if (spellID.IsValidSpell())
                //{
                  POSITION pos;
                  pos = combattargets.GetHeadPosition();
                  if (pos != NULL)
                  {
                    const SPELL_DATA *pSpell;
                    SPELL_DATA *pSpData;
                    COMBATANT *pTarget;
                    int rollerLevel=-1;
                    CHARACTER_SPELL newSpell;
                    PATH_DIR dir;
                    pSpell = combatSummary.PeekCombatant(0)->PeekWeapon(pCSA->weaponOrd-1)->pSpell;
                    pSpData = const_cast<SPELL_DATA *>(pSpell);
                    pTarget = GetCombatantPtr(dude);
                    combattargets.RemoveAll();
                    //pSpData = spellData.GetSpell(spellID);
                    { 
                      CString result;
                      HOOK_PARAMETERS hookParameters;
                      SCRIPT_CONTEXT scriptContext;
                      scriptContext.SetSpellContext(pSpData);
                      scriptContext.SetAttackerContext(this);
                      //hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
                      result = pSpData->RunSpellScripts(
                                        SPELL_CASTER_LEVEL,
                                        ScriptCallback_RunAllScripts, 
                                        NULL,
                                        "Combat AI - Can combatant use this spell");
                      if (result.GetLength()> 0)
                      {
                        sscanf(result,"%d",&rollerLevel);
                      };
                    };
                    
                    DWORD targs =  EvalDiceAsClass(pSpData->TargetQuantity(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    DWORD range =  EvalDiceAsClass(pSpData->TargetRange(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    DWORD xSize =  EvalDiceAsClass(pSpData->TargetWidth(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    DWORD ySize = EvalDiceAsClass(pSpData->TargetHeight(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    m_pCharacter->InitTargeting(pSpData->Targeting,
                                                pSpData->CanTargetFriend,
                                                pSpData->CanTargetEnemy,
                                                FALSE,
                                                targs,
                                                range,
                                                xSize,
                                                ySize,
                                                pSpData->Lingers);
                    m_itemSpellIDBeingCast = pSpData->SpellID();
                    Wpn_Type = pItem->Wpn_Type;
                    combatant_spellCastingLevel = rollerLevel;
                    m_spellIDBeingCast.Clear();
                    dir = GetSpellTargetingDir(GetCenterX(),GetCenterY(), pTarget->x,pTarget->y);
                    if (m_pCharacter->targets.m_SelectingUnits)
                    {
                      // 20200204 PRS  We set the target distance to 0.
                      // Why?  Because it is irrelevant.  This action wuould not
                      // have been listed (and chosen) if the target were 
                      // beyond reach of the spell.
                      //C_AddTarget(*pTarget, range);
                      C_AddTarget(*pTarget, 0);
                    }
                    else
                    {
                      int dirX, dirY;
                      dirX = pTarget->x - GetCenterX();
                      dirY = pTarget->y - GetCenterY();
                      AddMapTarget(pTarget->x, pTarget->y, dir, dirX, dirY);
                    };
                    State(ICS_Casting);
                    m_pCharacter->ClearSpellbook();
                    m_pCharacter->UseSpellLimits(FALSE);
                    // We may have to do some fiddling here.
                    // Setting the caster baseclass and spell school to
                    // illegal values for now......
                    {
                      SCHOOL_ID schoolID;
                      BASECLASS_ID baseclassID;
                      m_pCharacter->KnowSpell(baseclassID, schoolID, pSpData->SpellID(), 1, TRUE);
                    };
                    m_pCharacter->myItems.SetCharges(itemIdx, m_pCharacter->myItems.GetCharges(itemIdx) - 1);
                    if (m_pCharacter->myItems.GetCharges(itemIdx) <= 0)
                    {
                      m_pCharacter->myItems.DeleteItem(itemIdx);
                    };
                  };
                //};
              }
              else
              {
                m_pCharacter->myItems.DeleteItem(itemIdx);
                State(ICS_Guarding);
              };  
            //};
          //};
        }
      };
      return TRUE;
    case AT_Advance:
      {
        bool found = false;
        const COMBAT_SUMMARY_ACTION *pCSA;
        combattargets.RemoveAll();
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        AddTarget(dude, false);
        {    
          //BOOL repath = TRUE;

          // check to see if existing path to target needs to change
          //CPathFinder* pathPtr = pathMgr.GetPath(hPath);

          //if (pathPtr != NULL) 
          //{
          //  CPoint stepPtr = pathPtr->GetLastStep();    
    
          //  if (   (tempCOMBATANT->x == stepPtr.x)
          //      && (tempCOMBATANT->y == stepPtr.y))
          //  {
          //    // use same path to target
          //    repath = FALSE;
          //    found = TRUE;
          //    TRACE("Using same path for combatant %i to target %i\n", self, dude);
          //  }
          //  else
          //    TRACE("Existing path for combatant %i does not end at curr target, re-pathing\n", self);
          //}        

          // find path to closest enemy
          // need to change to closest enemy with shortest path!
          //if ((repath) && (CanMove()))
          {
            COMBATANT *tempCOMBATANT;
            ClearPath();
            SetCurrTarget(); // setup for iteration
            int targetCount = 0;
            dude = GetCurrTarget(true, false, true);

            while (   (!found) 
                   && (dude != NO_DUDE) 
                   && (targetCount < GetNumTargets()))
            {
              tempCOMBATANT = GetCombatantPtr(dude);
              ASSERT(tempCOMBATANT!=NULL);
              if (tempCOMBATANT==NULL) return FALSE;
              if (FindPathTo(tempCOMBATANT->x-1,
                             tempCOMBATANT->y-1, 
                             tempCOMBATANT->x + tempCOMBATANT->width,
                             tempCOMBATANT->y + tempCOMBATANT->height,
                             TRUE,
                             FALSE,
                             FALSE))
              {
                found = TRUE;
                RemoveAllTargets();
                AddTarget(dude, false);
              }
              else
              {
                dude = GetNextTarget();
              };
              targetCount++;
            };
          };
        };
        if (found)
        {
          State(ICS_Moving);
          return TRUE;
        };
      };
      break;
    case AT_RangedWeapon:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        int itemIdx;
        combattargets.RemoveAll();
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        itemIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd-1)->index;
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        m_pCharacter->ReadyWeaponScript(itemIdx);
        m_pCharacter->ReadyBestAmmo(m_pCharacter->m_pCombatant->isLargeDude());
        AddTarget(dude, false);
        ReadyBestArmor();
        State(ICS_Attacking);
      };
      return TRUE;
    case AT_Judo:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        combattargets.RemoveAll();
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        AddTarget(dude, false);
        ReadyBestArmor();
        State(ICS_Attacking);
      };
      return TRUE;
    case AT_MeleeWeapon:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        combattargets.RemoveAll();
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        if (pCSA->weaponOrd == 0) bestWeaponIdx = NO_READY_ITEM;
        else bestWeaponIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd-1)->index;
        m_pCharacter->ReadyWeaponScript(bestWeaponIdx);
        AddTarget(dude, false);
        ReadyBestArmor();
        State(ICS_Attacking);
      };
      return TRUE;
    case AT_Unknown:
      /* Really */ NotImplemented(0xa88, false);
      break;
    default:
      /* Really */ NotImplemented(0x55ab, false);
      break;
    };
  };


  if (useScriptedAI)
  {
    const COMBAT_SUMMARY_ACTION *pCSA;
    if (combatSummary.GetActionCount() > 0)
    {
      combattargets.RemoveAll();
      pCSA = combatSummary.PeekAction(actionIndex[0]);
      dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
      if (pCSA->weaponOrd == 0) bestWeaponIdx = 0;
      else bestWeaponIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd)->index;
      m_pCharacter->ReadyWeaponScript(bestWeaponIdx);
      AddTarget(dude, false);
    };
  };

  if (dude == NO_DUDE)
  {
    dude = GetCurrTarget(true, false, true);
  };
  COMBATANT *tempCOMBATANT=NULL;
  if (dude != NO_DUDE)
  {
    tempCOMBATANT = GetCombatantPtr(dude);
    ASSERT(tempCOMBATANT!=NULL);
    if (tempCOMBATANT==NULL) return FALSE;

    //here we must ready the item indicated by combatSunnary
    if (!useScriptedAI || (combatSummary.PeekAction(actionIndex[0])->advance == 0))
    {
      if (!canAttack(dude, -1, -1, 0, Distance, useScriptedAI))
      {
        // target is not attackable for some reason,
        // so force new target acquisition
        dude = NO_DUDE;
      }
    };
  }

  if (   (dude == NO_DUDE) 
      || ((tempCOMBATANT != NULL) && (!tempCOMBATANT->charOnCombatMap(false, true))))
  {
    // if no target or it's gone, acquire new target (if any)
    RemoveAllTargets();

    int i;
    // try for combat targets with line of sight first
    // combat targets are ordered by distance, but the closest
    // target in terms of distance may be on the other side
    // of a wall. Using line of sight helps to ensure we will attack
    // closest target that is also the shortest path distance.
    for (i=0; i < GetNumCombatants()/*combatData.numCombatants*/; i++)
    {
      tempCOMBATANT = GetCombatantPtr(i);
      ASSERT(tempCOMBATANT!=NULL);
      if (tempCOMBATANT==NULL) return FALSE;
      if (   (i != self) 
          && (tempCOMBATANT->GetIsFriendly() != GetIsFriendly())
          && (tempCOMBATANT->charOnCombatMap(false, true)))
      {
        if (HaveLineOfSight(GetCenterX(),GetCenterY(), tempCOMBATANT->GetCenterX(), tempCOMBATANT->GetCenterY(),NULL))
          AddTarget(i, false);
      }
    }

    if (GetCurrTarget(true, false, true) == NO_DUDE)
    {
      // no combat targets within line of sight
      // so use any avail target
      for (i=0; i < GetNumCombatants()/*combatData.numCombatants*/; i++)
      {
        tempCOMBATANT = GetCombatantPtr(i);
        ASSERT(tempCOMBATANT!=NULL);
        if (tempCOMBATANT==NULL) return FALSE;
        if (   (i != self) 
            && (tempCOMBATANT->friendly != GetIsFriendly())
            && (tempCOMBATANT->charOnCombatMap(false, true)))
          AddTarget(i, false);
      }
    }
  }  

  // might use current target when determining best
  // items to make ready.
  ReadyBestArmor();
  // Moved past ReadyBestWeapon 20140425 PRS
  // A monster with a shield refused to use a two-handed weapon at range.
  // So we remove the shield until we decide what weapon will be used.


  // Scripted "THINK" inagurated on 7 May 2014. PRS
  { // *********************************************************** SHIELD
    // ReadyBestShield();
    const COMBAT_SUMMARY_COMBATANT *pcsc;
    pcsc = combatSummary.PeekCombatant(0);
    if ((pcsc!=NULL) && (pcsc->shieldToReady != 0))
    {
      m_pCharacter->ReadyShieldScript(pcsc->PeekShield(pcsc->shieldToReady-1)->index);
    };
  };
  if (!useScriptedAI)
  { // *********************************************************** WEAPON
    ReadyBestWpn(GetCurrTarget(true, false, true));
  };
  //ReadyBestShield();
  if (!useScriptedAI)
  {
    dude = GetCurrTarget(true, false, true);
  };

  // no combattargets, sit tight and guard
  if (dude == NO_DUDE)
  {
    State(ICS_Guarding);
    TRACE("thinking result for auto combatant %u (%s) - no targets\n", self, CombatantStateText[State()]);
    return TRUE;
  }
  BOOL CanAttack;  
  if (useScriptedAI && (combatSummary.PeekAction(0)->advance != 0))
  {
    CanAttack = FALSE;
  }
  else
  {
    CanAttack = canAttack(dude, -1, -1, 0, Distance, useScriptedAI);
  };
  tempCOMBATANT = GetCombatantPtr(dude);
  ASSERT(tempCOMBATANT!=NULL);
  if (tempCOMBATANT==NULL) return FALSE;
  BOOL found = FALSE;

  // no need to find a path if we can attack from current spot
  if (!CanAttack)
  {    
    BOOL repath = TRUE;

    // check to see if existing path to target needs to change
    CPathFinder* pathPtr = pathMgr.GetPath(hPath);

    if (pathPtr != NULL) 
    {
      CPoint stepPtr = pathPtr->GetLastStep();    

      if (   (tempCOMBATANT->x == stepPtr.x)
          && (tempCOMBATANT->y == stepPtr.y))
      {
        // use same path to target
        repath = FALSE;
        found = TRUE;
        TRACE("Using same path for combatant %i to target %i\n", self, dude);
      }
      else
        TRACE("Existing path for combatant %i does not end at curr target, re-pathing\n", self);
    }        

    // find path to closest enemy
    // need to change to closest enemy with shortest path!
    if ((repath) && (CanMove(FALSE)))
    {
      ClearPath();
      SetCurrTarget(); // setup for iteration
      int targetCount = 0;
      dude = GetCurrTarget(true, false, true);

      while (   (!found) 
             && (dude != NO_DUDE) 
             && (targetCount < GetNumTargets()))
      {
        tempCOMBATANT = GetCombatantPtr(dude);
        ASSERT(tempCOMBATANT!=NULL);
        if (tempCOMBATANT==NULL) return FALSE;
        if (FindPathTo(tempCOMBATANT->x-1,
                       tempCOMBATANT->y-1, 
                       tempCOMBATANT->x + tempCOMBATANT->width,
                       tempCOMBATANT->y + tempCOMBATANT->height,
                       TRUE,
                       FALSE,
                       FALSE))
        {
          found = TRUE;
          RemoveAllTargets();
          AddTarget(dude, false);
        }
        else
        {
          dude = GetNextTarget();
        }
        targetCount++;
      }
    }
  }

  if (!found) // do we have a path to take?
  {
    if (!useScriptedAI && (GetCurrTarget(true, false, true) == NO_DUDE))
    {
      State(ICS_Guarding);
    }
    else
    {
      if (CanAttack)
      {
        State(ICS_Attacking);
        //GLOBAL_ITEM_ID giID;
        ITEM_ID itemID;
        int itemIdx;
        //itemIdx = myItems.GetReady(WeaponHand);
        itemIdx = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
        if (itemIdx != NO_READY_ITEM)
        {
          //giID = m_pCharacter->myItems.GetItem(itemIdx);
          itemID = m_pCharacter->myItems.GetItem(itemIdx);
          weaponClassType weaponType;
          //weaponType = itemData.GetWpnType(giID);
          weaponType = itemData.GetWpnType(itemID);
          if (     (weaponType == SpellCaster) 
               ||  (weaponType == SpellLikeAbility))
          {
            if (m_pCharacter->myItems.GetCharges(itemIdx) > 0)
            {
              ITEM_DATA *pItem;
              //GLOBAL_SPELL_ID gsID;
              SPELL_ID spellID;
              //pItem = itemData.GetItemData(giID);
              pItem = itemData.GetItem(itemID);
              //gsID = pItem->m_gsID;
              spellID = pItem->spellID;
              if (spellID.IsValidSpell())
              {
                POSITION pos;
                pos = combattargets.GetHeadPosition();
                if (pos != NULL)
                {
                  SPELL_DATA *pSpData;
                  COMBATANT *pTarget;
                  int rollerLevel=-1;
                  CHARACTER_SPELL newSpell;
                  //DWORD target;
                  PATH_DIR dir;
                  //target = combattargets.GetAtPos(pos);
                  pTarget = GetCombatantPtr(dude);
                  combattargets.RemoveAll();
                  //pSpData = spellData.GetSpellData(gsID);
                  pSpData = spellData.GetSpell(spellID);
                  //DWORD targs = EvalDiceAsClass(pSpData->NumTargets, pSpData->schoolMask, NULL);
                  //DWORD range = EvalDiceAsClass(pSpData->TargetRange, pSpData->schoolMask, NULL);
                  { 
                    CString result;
                    HOOK_PARAMETERS hookParameters;
                    SCRIPT_CONTEXT scriptContext;
                    scriptContext.SetSpellContext(pSpData);
                    scriptContext.SetAttackerContext(this);
                    //hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
                    result = pSpData->RunSpellScripts(
                                   SPELL_CASTER_LEVEL, 
                                   ScriptCallback_RunAllScripts,
                                   NULL,
                                   "Combat AI - Can combatant use this spell");
                    if (result.GetLength()> 0)
                    {
                      sscanf(result,"%d",&rollerLevel);
                    };
                  };
                  
                  DWORD targs =  EvalDiceAsClass(pSpData->TargetQuantity(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  DWORD range =  EvalDiceAsClass(pSpData->TargetRange(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  DWORD xSize =  EvalDiceAsClass(pSpData->TargetWidth(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  DWORD ySize = EvalDiceAsClass(pSpData->TargetHeight(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  m_pCharacter->InitTargeting(pSpData->Targeting,
                                              pSpData->CanTargetFriend,
                                              pSpData->CanTargetEnemy,
                                              FALSE,
                                              targs,
                                              range,
                                              xSize,
                                              ySize,
                                              pSpData->Lingers);
                  //this->m_itemSpellKeyBeingCast = gsID;
                  //this->m_spellKeyBeingCast.Clear();
                  m_itemSpellIDBeingCast = spellID;
                  Wpn_Type = pItem->Wpn_Type;
                  combatant_spellCastingLevel = rollerLevel;
                  m_spellIDBeingCast.Clear();
                  dir = GetSpellTargetingDir(GetCenterX(),GetCenterY(), pTarget->x,pTarget->y);
                  if (m_pCharacter->targets.m_SelectingUnits)
                  {
                    C_AddTarget(*pTarget, range);
                  }
                  else
                  {
                    int dirX, dirY;
                    dirX = pTarget->x - GetCenterX();
                    dirY = pTarget->y - GetCenterY();
                    AddMapTarget(pTarget->x, pTarget->y, dir, dirX, dirY);
                  };
                  State(ICS_Casting);
                  m_pCharacter->ClearSpellbook();
                  m_pCharacter->UseSpellLimits(FALSE);
                  //m_pCharacter->KnowSpell(MagicUserFlag,MagicUserFlag,gsID,1,TRUE);
                  //Not Implemented(0x7c5da86, false);
                  // We may have to do some fiddling here.
                  // Setting the caster baseclass and spell school to
                  // illegal values for now......
                  {
                    SCHOOL_ID schoolID;
                    BASECLASS_ID baseclassID;
                    //m_pCharacter->KnowSpell(NULL, schoolID, spellID, 1, TRUE);
                    m_pCharacter->KnowSpell(baseclassID, schoolID, spellID, 1, TRUE);
                  };
                  m_pCharacter->myItems.SetCharges(itemIdx, m_pCharacter->myItems.GetCharges(itemIdx) - 1);
                  if (m_pCharacter->myItems.GetCharges(itemIdx) <= 0)
                  {
                    m_pCharacter->myItems.DeleteItem(itemIdx);
                  };
                };
              };
            }
            else
            {
              m_pCharacter->myItems.DeleteItem(itemIdx);
              State(ICS_Guarding);
            };
          };
        };
      }
      else
        State(ICS_Guarding);
    }
  }
  else
    State(ICS_Moving);

  TRACE("thinking result for auto combatant %u (%s)\n", self, CombatantStateText[State()]);
  return TRUE;
}




//*****************************************************************************
// NAME:    COMBATANT::FreeThink
//
// PURPOSE: 
//*****************************************************************************
// 
// can return one of these states
/*
  CS_Moving
  CS_Guarding
  CS_Attacking
*/

/*   This started as a copy of Think() and will be used to
 *   select a weapon and either a Judo attack or Melee attack.
 *   This function is called whenever we have a Free Attack or Guarding Attack.
 *   As a by-product, it will produce an Actionsnnn.txt file for debugging.
 */
BOOL COMBATANT::FreeThink()
{//8a0c
  // no user input allowed while dude is on auto
  CArray<int, int> actionIndex;
  bool useScriptedAI;
  int bestWeaponIdx;
  //if (IsPartyMember())
  //  EnableUserInput(TRUE);
  //else
  //  EnableUserInput(FALSE);

  if (IsPartyMember())
  {
    return TRUE;
  }

  TRACE("thinking for auto combatant %u (%s)\n", self, CombatantStateText[State()]);

  // make sure current path has not been exhausted
//  if (hPath >= 0)
//  {
//    if (pathMgr.GetStepsRemaining(hPath)==0)
//      ClearPath();
//  }  

//  if (iFleeingFlags != 0)
//  {
//    State(ICS_Moving);
//    TRACE("thinking result for auto combatant %u (%s=fleeing)\n", self, CombatantStateText[State()]);
//    // find path to closest map edge

//    BOOL fled=FALSE;
//    // if already on map edge, flee off the map
//    if (   (x==0) || (x==(MAX_TERRAIN_WIDTH-1))
//        || (y==0) || (y==(MAX_TERRAIN_HEIGHT-1)) )
//    {
//      fled=TRUE;
//    }

//    if (fled) 
//    {
//      if (friendly)
//        IncNumFriendFlee(); //combatData.numFrndFlee++;
//      else
//        IncNumMonsterFlee(); //combatData.numMonFlee++;

//      menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
//      menu.setHorzOrient();
//      menu.setUseActive(FALSE); // no highlighting      

//      CString msg("");
//      msg.Format("%s has fled...", GetName());
//      menu.changeMenuItem(1, msg);
//      UpdateCombatTimeDelayMsg();
//      theApp.AppSleep(GetMsgTimeDelay()); // give time to see normal icon      

//      //BUGFIX EndTurn();
//      SetStatus(Fled);
//      placeCombatant(x, y, NO_DUDE, width, height);
//      TRACE("combatant %i has fled the map\n", self);
//      State(ICS_Fled); //BUGFIX
//      EnableUserInput(false);
//      return TRUE;
//    }
//    if (CanMove(FALSE))
//    {
//      if (hPath < 0)
//      {
//        // find path to farthest corner of room away from cleric that turned me
//        if (!FindPathAwayFrom(m_iLastAttacker))    
//        {
//          WriteDebugString("Fleeing combatant %i failed to find exit path\n", self);
//          State(ICS_Guarding);
//        }
//      }
//    }
//    else
//      State(ICS_Guarding);    
//    return TRUE;
//  }

//  if (isTurned)
//  {
//    State(ICS_Moving);
//    TRACE("thinking result for auto combatant %u (%s=turned/fleeing %i)\n", 
//                     self, CombatantStateText[State()], m_iLastAttacker);

//    BOOL fled=FALSE;
//    // if already on map edge, flee off the map
//    if (   (x==0) || (x==(MAX_TERRAIN_WIDTH-1))
//        || (y==0) || (y==(MAX_TERRAIN_HEIGHT-1)) )
//    {
//      fled=TRUE;
//    }

//    if (fled) 
//    {
//      if (friendly)
//        IncNumFriendFlee(); //combatData.numFrndFlee++;
//      else
//        IncNumMonsterFlee(); //combatData.numMonFlee++;

//      menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
//      menu.setHorzOrient();
//      menu.setUseActive(FALSE); // no highlighting      

//      CString msg("");
//      msg.Format("%s has fled...", GetName());
//      menu.changeMenuItem(1, msg);
//      UpdateCombatTimeDelayMsg();
//      theApp.AppSleep(GetMsgTimeDelay()); // give time to see normal icon      

//      // BUGFIX EndTurn(ICS_Fled);
//      SetStatus(Fled);
//      State(ICS_Fled); // BUGFIX
//      placeCombatant(x, y, NO_DUDE, width, height);
//      TRACE("Turned Undead %i has fled the map\n", self);
//      return TRUE;
//    }
//    if (CanMove(FALSE))
//    {
//      if (hPath < 0)
//      {
//        // find path to farthest corner of room away from cleric that turned me
//        if (!FindPathAwayFrom(m_iLastAttacker))    
//        {
//          WriteDebugString("Fleeing combatant %i failed to find exit path\n", self);
//          State(ICS_Guarding);
//        }
//      }
//    }
//    else
//      State(ICS_Guarding);
//    return TRUE;
//  }

//  // make sure current target is still on the map

  useScriptedAI = false;
  m_pCharacter->ReadyWeaponScript(NO_READY_ITEM);
  m_pCharacter->ReadyShieldScript(NO_READY_ITEM);
  {
    if (LoadAI_Script())
    {
      COMBATANT *pAttacker, *pTarget;
      int additionalAttacks;
      combatSummary.m_thinkCount++;
      combatSummary.Clear();
      pAttacker = combatData.GetCombatant(combatData.GetCurrCombatant());
      pTarget = combatData.GetCombatant(pAttacker->m_target);

      // Lists only the two combatants of interest.
      combatData.ListCombatants(&combatSummary, pAttacker, pTarget);
      combatData.ListWeapons(combatSummary.GetCombatant(0)); // List weapons for attacker only.
      combatData.ListAmmo(combatSummary.GetCombatant(0));    // List ammo for attacker only.
      additionalAttacks = combatData.QComb.NumFreeAttacks() + combatData.QComb.NumGuardAttacks();
      // List attacks for attacker only.
      combatData.ListAttacks(combatSummary.GetCombatant(0), additionalAttacks); 
      //combatData.ListReachableCells(&combatSummary, combatSummary.GetCombatant(0)); // Number of steps to each cell.
      combatData.ListActions(&combatSummary, 0, additionalAttacks, true);
            
      actionIndex.SetSize(combatSummary.GetActionCount());
      {
        int i, j, k;
        actionIndex.SetSize(combatSummary.GetActionCount());
        for (i=0; i<combatSummary.GetActionCount(); i++)
        {
          actionIndex.SetAt(i,i);
          // Move the new entry towards the top of the tree.
          j = i;
          while (j > 0)
          {
            int temp;
            k = (j-1)/2;
            combatSummary.pActionA = combatSummary.GetAction(actionIndex[j]);
            combatSummary.pActionB = combatSummary.GetAction(actionIndex[k]);
            if (RunTHINK(thinkProgram.pgm, &combatSummary) <= 0) break;
            temp = actionIndex[j];
            actionIndex[j] = actionIndex[k];
            actionIndex[k] = temp;
            j = k;
          };
        };
        DumpActions(&actionIndex, &combatSummary, true);
        if (combatSummary.GetActionCount() > 0)
        {
          useScriptedAI = true;
        };
      };
    };
  };

  int dude = NO_DUDE;

  if (useScriptedAI)
  {
    if (combatSummary.m_thinkCount == 20)
    {
      ASSERT(TRUE);
    }
    ACTION_TYPE actionType = combatSummary.PeekAction(actionIndex[0])->actionType;
    switch (actionType)
    {
    case AT_SpellLikeAbility:
    case AT_SpellCaster:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        int itemIdx;
        combattargets.RemoveAll();
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        itemIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd-1)->index;
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        m_pCharacter->ReadyWeaponScript(itemIdx);
        AddTarget(dude, false);
        ReadyBestArmor();
        {
          State(ICS_Attacking);
          //ITEM_ID itemID;
          itemIdx = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
          //if (itemIdx != NO_READY_ITEM)
          //{
            //itemID = m_pCharacter->myItems.GetItem(itemIdx);
            //weaponClassType weaponType;
            //weaponType = itemData.GetWpnType(itemID);
            //if (weaponType == SpellCaster)
            //{
              if (m_pCharacter->myItems.GetCharges(itemIdx) > 0)
              {
                const ITEM_DATA *pItem;
                //SPELL_ID spellID;
                //pItem = itemData.GetItem(itemID);
                pItem = combatSummary.PeekCombatant(0)->PeekWeapon(pCSA->weaponOrd-1)->pWeapon;
                //spellID = pItem->spellID;
                //if (spellID.IsValidSpell())
                //{
                  POSITION pos;
                  pos = combattargets.GetHeadPosition();
                  if (pos != NULL)
                  {
                    const SPELL_DATA *pSpell;
                    SPELL_DATA *pSpData;
                    COMBATANT *pTarget;
                    int rollerLevel=-1;
                    CHARACTER_SPELL newSpell;
                    PATH_DIR dir;
                    pSpell = combatSummary.PeekCombatant(0)->PeekWeapon(pCSA->weaponOrd-1)->pSpell;
                    pSpData = const_cast<SPELL_DATA *>(pSpell);
                    pTarget = GetCombatantPtr(dude);
                    combattargets.RemoveAll();
                    //pSpData = spellData.GetSpell(spellID);
                    { 
                      CString result;
                      HOOK_PARAMETERS hookParameters;
                      SCRIPT_CONTEXT scriptContext;
                      scriptContext.SetSpellContext(pSpData);
                      scriptContext.SetAttackerContext(this);
                      //hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
                      result = pSpData->RunSpellScripts(
                                           SPELL_CASTER_LEVEL, 
                                           ScriptCallback_RunAllScripts,
                                           NULL,
                                           "Combat AI Free Attack - can combatant use this spell");
                      if (result.GetLength()> 0)
                      {
                        sscanf(result,"%d",&rollerLevel);
                      };
                    };
                    
                    DWORD targs =  EvalDiceAsClass(pSpData->TargetQuantity(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    DWORD range =  EvalDiceAsClass(pSpData->TargetRange(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    DWORD xSize =  EvalDiceAsClass(pSpData->TargetWidth(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    DWORD ySize = EvalDiceAsClass(pSpData->TargetHeight(), 
                                                   pSpData->schoolID, 
                                                   pSpData->Level, 
                                                   &rollerLevel);
                    m_pCharacter->InitTargeting(pSpData->Targeting,
                                                pSpData->CanTargetFriend,
                                                pSpData->CanTargetEnemy,
                                                FALSE,
                                                targs,
                                                range,
                                                xSize,
                                                ySize,
                                                pSpData->Lingers);
                    m_itemSpellIDBeingCast = pSpData->SpellID();
                    Wpn_Type = pItem->Wpn_Type;
                    combatant_spellCastingLevel = rollerLevel;
                    m_spellIDBeingCast.Clear();
                    dir = GetSpellTargetingDir(GetCenterX(),GetCenterY(), pTarget->x,pTarget->y);
                    if (m_pCharacter->targets.m_SelectingUnits)
                    {
                      C_AddTarget(*pTarget, range);
                    }
                    else
                    {
                      int dirX, dirY;
                      dirX = pTarget->x - GetCenterX();
                      dirY = pTarget->y - GetCenterY();
                      AddMapTarget(pTarget->x, pTarget->y, dir, dirX, dirY);
                    };
                    State(ICS_Casting);
                    m_pCharacter->ClearSpellbook();
                    m_pCharacter->UseSpellLimits(FALSE);
                    // We may have to do some fiddling here.
                    // Setting the caster baseclass and spell school to
                    // illegal values for now......
                    {
                      SCHOOL_ID schoolID;
                      BASECLASS_ID baseclassID;
                      m_pCharacter->KnowSpell(baseclassID, schoolID, pSpData->SpellID(), 1, TRUE);
                    };
                    m_pCharacter->myItems.SetCharges(itemIdx, m_pCharacter->myItems.GetCharges(itemIdx) - 1);
                    if (m_pCharacter->myItems.GetCharges(itemIdx) <= 0)
                    {
                      m_pCharacter->myItems.DeleteItem(itemIdx);
                    };
                  };
                //};
              }
              else
              {
                m_pCharacter->myItems.DeleteItem(itemIdx);
                State(ICS_Guarding);
              };  
            //};
          //};
        }
      };
      return TRUE;
    case AT_Advance:
      {
        bool found = false;
        const COMBAT_SUMMARY_ACTION *pCSA;
        combattargets.RemoveAll();
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        AddTarget(dude, false);
        {    
          //BOOL repath = TRUE;

          // check to see if existing path to target needs to change
          //CPathFinder* pathPtr = pathMgr.GetPath(hPath);

          //if (pathPtr != NULL) 
          //{
          //  CPoint stepPtr = pathPtr->GetLastStep();    
    
          //  if (   (tempCOMBATANT->x == stepPtr.x)
          //      && (tempCOMBATANT->y == stepPtr.y))
          //  {
          //    // use same path to target
          //    repath = FALSE;
          //    found = TRUE;
          //    TRACE("Using same path for combatant %i to target %i\n", self, dude);
          //  }
          //  else
          //    TRACE("Existing path for combatant %i does not end at curr target, re-pathing\n", self);
          //}        

          // find path to closest enemy
          // need to change to closest enemy with shortest path!
          //if ((repath) && (CanMove()))
          {
            COMBATANT *tempCOMBATANT;
            ClearPath();
            SetCurrTarget(); // setup for iteration
            int targetCount = 0;
            dude = GetCurrTarget(true, false, true);

            while (   (!found) 
                   && (dude != NO_DUDE) 
                   && (targetCount < GetNumTargets()))
            {
              tempCOMBATANT = GetCombatantPtr(dude);
              ASSERT(tempCOMBATANT!=NULL);
              if (tempCOMBATANT==NULL) return FALSE;
              if (FindPathTo(tempCOMBATANT->x-1,
                             tempCOMBATANT->y-1, 
                             tempCOMBATANT->x + tempCOMBATANT->width,
                             tempCOMBATANT->y + tempCOMBATANT->height,
                             TRUE,
                             FALSE,
                             FALSE))
              {
                found = TRUE;
                RemoveAllTargets();
                AddTarget(dude, false);
              }
              else
              {
                dude = GetNextTarget();
              };
              targetCount++;
            };
          };
        };
        if (found)
        {
          State(ICS_Moving);
          return TRUE;
        };
      };
      break;
    case AT_RangedWeapon:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        int itemIdx;
        combattargets.RemoveAll();
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        itemIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd-1)->index;
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        m_pCharacter->ReadyWeaponScript(itemIdx);
        m_pCharacter->ReadyBestAmmo(m_pCharacter->m_pCombatant->isLargeDude());
        AddTarget(dude, false);
        ReadyBestArmor();
        State(ICS_Attacking);
      };
      return TRUE;
    case AT_Judo:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        combattargets.RemoveAll();
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        AddTarget(dude, false);
        ReadyBestArmor();
        State(ICS_Attacking);
      };
      return TRUE;
    case AT_MeleeWeapon:
      {
        const COMBAT_SUMMARY_ACTION *pCSA;
        pCSA = combatSummary.PeekAction(actionIndex[0]);
        combattargets.RemoveAll();
        dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
        if (pCSA->weaponOrd == 0) bestWeaponIdx = NO_READY_ITEM;
        else bestWeaponIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd-1)->index;
        m_pCharacter->ReadyWeaponScript(bestWeaponIdx);
        AddTarget(dude, false);
        ReadyBestArmor();
        State(ICS_Attacking);
      };
      return TRUE;
    case AT_Unknown:
      /* Really */ NotImplemented(0xa88, false);
      break;
    default:
      /* Really */ NotImplemented(0x55ab, false);
      break;
    };
  };


  if (useScriptedAI)
  {
    const COMBAT_SUMMARY_ACTION *pCSA;
    if (combatSummary.GetActionCount() > 0)
    {
      combattargets.RemoveAll();
      pCSA = combatSummary.PeekAction(actionIndex[0]);
      dude = combatSummary.PeekCombatant(pCSA->targetOrd-1)->index;
      if (pCSA->weaponOrd == 0) bestWeaponIdx = 0;
      else bestWeaponIdx = pCSA->pMe->PeekWeapon(pCSA->weaponOrd)->index;
      m_pCharacter->ReadyWeaponScript(bestWeaponIdx);
      AddTarget(dude, false);
    };
  };

  if (dude == NO_DUDE)
  {
    dude = GetCurrTarget(true, false, true);
  };
  COMBATANT *tempCOMBATANT=NULL;
  if (dude != NO_DUDE)
  {
    tempCOMBATANT = GetCombatantPtr(dude);
    ASSERT(tempCOMBATANT!=NULL);
    if (tempCOMBATANT==NULL) return FALSE;

    //here we must ready the item indicated by combatSunnary
    if (!useScriptedAI || (combatSummary.PeekAction(actionIndex[0])->advance == 0))
    {
      if (!canAttack(dude, -1, -1, 0, Distance, useScriptedAI))
      {
        // target is not attackable for some reason,
        // so force new target acquisition
        dude = NO_DUDE;
      }
    };
  }

  if (   (dude == NO_DUDE) 
      || ((tempCOMBATANT != NULL) && (!tempCOMBATANT->charOnCombatMap(false, true))))
  {
    // if no target or it's gone, acquire new target (if any)
    RemoveAllTargets();

    int i;
    // try for combat targets with line of sight first
    // combat targets are ordered by distance, but the closest
    // target in terms of distance may be on the other side
    // of a wall. Using line of sight helps to ensure we will attack
    // closest target that is also the shortest path distance.
    for (i=0; i < GetNumCombatants()/*combatData.numCombatants*/; i++)
    {
      tempCOMBATANT = GetCombatantPtr(i);
      ASSERT(tempCOMBATANT!=NULL);
      if (tempCOMBATANT==NULL) return FALSE;
      if (   (i != self) 
          && (tempCOMBATANT->GetIsFriendly() != GetIsFriendly())
          && (tempCOMBATANT->charOnCombatMap(false, true)))
      {
        if (HaveLineOfSight(GetCenterX(),GetCenterY(), tempCOMBATANT->GetCenterX(), tempCOMBATANT->GetCenterY(),NULL))
          AddTarget(i, false);
      }
    }

    if (GetCurrTarget(true, false, true) == NO_DUDE)
    {
      // no combat targets within line of sight
      // so use any avail target
      for (i=0; i < GetNumCombatants()/*combatData.numCombatants*/; i++)
      {
        tempCOMBATANT = GetCombatantPtr(i);
        ASSERT(tempCOMBATANT!=NULL);
        if (tempCOMBATANT==NULL) return FALSE;
        if (   (i != self) 
            && (tempCOMBATANT->friendly != GetIsFriendly())
            && (tempCOMBATANT->charOnCombatMap(false, true)))
          AddTarget(i, false);
      }
    }
  }  

  // might use current target when determining best
  // items to make ready.
  ReadyBestArmor();
  // Moved past ReadyBestWeapon 20140425 PRS
  // A monster with a shield refused to use a two-handed weapon at range.
  // So we remove the shield until we decide what weapon will be used.


  // Scripted "THINK" inagurated on 7 May 2014. PRS
  { // *********************************************************** SHIELD
    // ReadyBestShield();
    const COMBAT_SUMMARY_COMBATANT *pcsc;
    pcsc = combatSummary.PeekCombatant(0);
    if ((pcsc!=NULL) && (pcsc->shieldToReady != 0))
    {
      m_pCharacter->ReadyShieldScript(pcsc->PeekShield(pcsc->shieldToReady-1)->index);
    };
  };
  if (!useScriptedAI)
  { // *********************************************************** WEAPON
    ReadyBestWpn(GetCurrTarget(true, false, true));
  };
  //ReadyBestShield();
  if (!useScriptedAI)
  {
    dude = GetCurrTarget(true, false, true);
  };

  // no combattargets, sit tight and guard
  if (dude == NO_DUDE)
  {
    State(ICS_Guarding);
    TRACE("thinking result for auto combatant %u (%s) - no targets\n", self, CombatantStateText[State()]);
    return TRUE;
  }
  BOOL CanAttack;  
  if (useScriptedAI && (combatSummary.PeekAction(0)->advance != 0))
  {
    CanAttack = FALSE;
  }
  else
  {
    CanAttack = canAttack(dude, -1, -1, 0, Distance, useScriptedAI);
  };
  tempCOMBATANT = GetCombatantPtr(dude);
  ASSERT(tempCOMBATANT!=NULL);
  if (tempCOMBATANT==NULL) return FALSE;
  BOOL found = FALSE;

  // no need to find a path if we can attack from current spot
  if (!CanAttack)
  {    
    BOOL repath = TRUE;

    // check to see if existing path to target needs to change
    CPathFinder* pathPtr = pathMgr.GetPath(hPath);

    if (pathPtr != NULL) 
    {
      CPoint stepPtr = pathPtr->GetLastStep();    

      if (   (tempCOMBATANT->x == stepPtr.x)
          && (tempCOMBATANT->y == stepPtr.y))
      {
        // use same path to target
        repath = FALSE;
        found = TRUE;
        TRACE("Using same path for combatant %i to target %i\n", self, dude);
      }
      else
        TRACE("Existing path for combatant %i does not end at curr target, re-pathing\n", self);
    }        

    // find path to closest enemy
    // need to change to closest enemy with shortest path!
    if ((repath) && (CanMove(FALSE)))
    {
      ClearPath();
      SetCurrTarget(); // setup for iteration
      int targetCount = 0;
      dude = GetCurrTarget(true, false, true);

      while (   (!found) 
             && (dude != NO_DUDE) 
             && (targetCount < GetNumTargets()))
      {
        tempCOMBATANT = GetCombatantPtr(dude);
        ASSERT(tempCOMBATANT!=NULL);
        if (tempCOMBATANT==NULL) return FALSE;
        if (FindPathTo(tempCOMBATANT->x-1,
                       tempCOMBATANT->y-1, 
                       tempCOMBATANT->x + tempCOMBATANT->width,
                       tempCOMBATANT->y + tempCOMBATANT->height,
                       TRUE,
                       FALSE,
                       FALSE))
        {
          found = TRUE;
          RemoveAllTargets();
          AddTarget(dude, false);
        }
        else
        {
          dude = GetNextTarget();
        }
        targetCount++;
      }
    }
  }

  if (!found) // do we have a path to take?
  {
    if (!useScriptedAI && (GetCurrTarget(true, false, true) == NO_DUDE))
    {
      State(ICS_Guarding);
    }
    else
    {
      if (CanAttack)
      {
        State(ICS_Attacking);
        //GLOBAL_ITEM_ID giID;
        ITEM_ID itemID;
        int itemIdx;
        //itemIdx = myItems.GetReady(WeaponHand);
        itemIdx = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
        if (itemIdx != NO_READY_ITEM)
        {
          //giID = m_pCharacter->myItems.GetItem(itemIdx);
          itemID = m_pCharacter->myItems.GetItem(itemIdx);
          weaponClassType weaponType;
          //weaponType = itemData.GetWpnType(giID);
          weaponType = itemData.GetWpnType(itemID);
          if (     (weaponType == SpellCaster) 
               ||  (weaponType == SpellLikeAbility))
          {
            if (m_pCharacter->myItems.GetCharges(itemIdx) > 0)
            {
              ITEM_DATA *pItem;
              //GLOBAL_SPELL_ID gsID;
              SPELL_ID spellID;
              //pItem = itemData.GetItemData(giID);
              pItem = itemData.GetItem(itemID);
              //gsID = pItem->m_gsID;
              spellID = pItem->spellID;
              if (spellID.IsValidSpell())
              {
                POSITION pos;
                pos = combattargets.GetHeadPosition();
                if (pos != NULL)
                {
                  SPELL_DATA *pSpData;
                  COMBATANT *pTarget;
                  int rollerLevel=-1;
                  CHARACTER_SPELL newSpell;
                  //DWORD target;
                  PATH_DIR dir;
                  //target = combattargets.GetAtPos(pos);
                  pTarget = GetCombatantPtr(dude);
                  combattargets.RemoveAll();
                  //pSpData = spellData.GetSpellData(gsID);
                  pSpData = spellData.GetSpell(spellID);
                  //DWORD targs = EvalDiceAsClass(pSpData->NumTargets, pSpData->schoolMask, NULL);
                  //DWORD range = EvalDiceAsClass(pSpData->TargetRange, pSpData->schoolMask, NULL);
                  { 
                    CString result;
                    HOOK_PARAMETERS hookParameters;
                    SCRIPT_CONTEXT scriptContext;
                    scriptContext.SetSpellContext(pSpData);
                    scriptContext.SetAttackerContext(this);
                    //hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
                    result = pSpData->RunSpellScripts(
                                         SPELL_CASTER_LEVEL,
                                         ScriptCallback_RunAllScripts,
                                         NULL,
                                         "Combat AI Free Attack - Can combatant use this spell");
                    if (result.GetLength()> 0)
                    {
                      sscanf(result,"%d",&rollerLevel);
                    };
                  };
                  
                  DWORD targs =  EvalDiceAsClass(pSpData->TargetQuantity(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  DWORD range =  EvalDiceAsClass(pSpData->TargetRange(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  DWORD xSize =  EvalDiceAsClass(pSpData->TargetWidth(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  DWORD ySize = EvalDiceAsClass(pSpData->TargetHeight(), 
                                                 pSpData->schoolID, 
                                                 pSpData->Level, 
                                                 &rollerLevel);
                  m_pCharacter->InitTargeting(pSpData->Targeting,
                                              pSpData->CanTargetFriend,
                                              pSpData->CanTargetEnemy,
                                              FALSE,
                                              targs,
                                              range,
                                              xSize,
                                              ySize,
                                              pSpData->Lingers);
                  //this->m_itemSpellKeyBeingCast = gsID;
                  //this->m_spellKeyBeingCast.Clear();
                  m_itemSpellIDBeingCast = spellID;
                  Wpn_Type = pItem->Wpn_Type;
                  combatant_spellCastingLevel = rollerLevel;
                  m_spellIDBeingCast.Clear();
                  dir = GetSpellTargetingDir(GetCenterX(),GetCenterY(), pTarget->x,pTarget->y);
                  if (m_pCharacter->targets.m_SelectingUnits)
                  {
                    C_AddTarget(*pTarget, range);
                  }
                  else
                  {
                    int dirX, dirY;
                    dirX = pTarget->x - GetCenterX();
                    dirY = pTarget->y - GetCenterY();
                    AddMapTarget(pTarget->x, pTarget->y, dir, dirX, dirY);
                  };
                  State(ICS_Casting);
                  m_pCharacter->ClearSpellbook();
                  m_pCharacter->UseSpellLimits(FALSE);
                  //m_pCharacter->KnowSpell(MagicUserFlag,MagicUserFlag,gsID,1,TRUE);
                  //Not Implemented(0x7c5da86, false);
                  // We may have to do some fiddling here.
                  // Setting the caster baseclass and spell school to
                  // illegal values for now......
                  {
                    SCHOOL_ID schoolID;
                    BASECLASS_ID baseclassID;
                    //m_pCharacter->KnowSpell(NULL, schoolID, spellID, 1, TRUE);
                    m_pCharacter->KnowSpell(baseclassID, schoolID, spellID, 1, TRUE);
                  };
                  m_pCharacter->myItems.SetCharges(itemIdx, m_pCharacter->myItems.GetCharges(itemIdx) - 1);
                  if (m_pCharacter->myItems.GetCharges(itemIdx) <= 0)
                  {
                    m_pCharacter->myItems.DeleteItem(itemIdx);
                  };
                };
              };
            }
            else
            {
              m_pCharacter->myItems.DeleteItem(itemIdx);
              State(ICS_Guarding);
            };
          };
        };
      }
      else
        State(ICS_Guarding);
    }
  }
  else
    State(ICS_Moving);

  TRACE("thinking result for auto combatant %u (%s)\n", self, CombatantStateText[State()]);
  return TRUE;
}







//*****************************************************************************
// NAME: COMBATANT::CheckMorale
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::CheckMorale()
{
  if ((iFleeingFlags != 0)||isTurned)
    return;

  // monsters with no INT never flee
  if ((GetType() == MONSTER_TYPE) && (GetAdjInt() <= 1))
    return;

  static int fflee = 0;
  static int fslain =0;
  static int mflee = 0;
  static int mslain = 0;
  static BOOL SuperiorForceModMade = FALSE;

  int mod = 0;
  int numParty, numMons;
  BOOL Flee = FALSE;

  GetNumCombatants(&numParty, &numMons);
  //combatData.getNumCombatants(numParty, numMons);

  if (friendly)
  {
    if (fflee != GetNumFriendFlee() /*combatData.numFrndFlee*/)
      mod += ((GetNumFriendFlee()-fflee) * 5);
    if (fslain != GetNumFriendSlain())
      mod += ((GetNumFriendSlain()-fslain) * 10);
    if (((numParty * 3) <= numMons) && (!SuperiorForceModMade))
    {
      mod += 20;
      SuperiorForceModMade = TRUE;
    }
  }
  else
  {
    if (mflee != GetNumMonsterFlee()/*combatData.numMonFlee*/)
      mod += ((GetNumMonsterFlee()-mflee) * 5);
    if (mslain != GetNumMonsterSlain())
      mod += ((GetNumMonsterSlain()-mslain) * 10);
    if (((numMons * 3) <= numParty) && (!SuperiorForceModMade))
    {
      mod += 20;
      SuperiorForceModMade = TRUE;
    }
  }

  /* *********************************************************
    *   The following email from Manikus  20181102
    * Good afternoon.
    * I would like the Morale value to not autochange. 
    * if I can GET 1) total Morale and 2) Combat Morale, 
    * OR 1) Morale set in editor (monster or NPC) and 
    * 2) Combat Morale, I will be happy.
    * I also think I will want the function to match with the 
    * AdjustInitiative Hook parameters.
    * 
    * -Eric
    ********************************************************* */
   
       // SetMorale( GetMorale()-mod );

  //int cur_morale = GetAdjMorale();
  Flee = FALSE; //(RollDice(100, 1, 0) > cur_morale);

  if (Flee)
  { 
    // only auto combatants choose to flee
    if (OnAuto(false))
    {      
      if (!IsPartyMember())
      {
        if (!GetConfigMonsterNoFlee())
          iFleeingFlags |= fleeBecauseImpossible;
      }
      else
        iFleeingFlags |= fleeBecauseImpossible;
    }
  }

  if (iFleeingFlags != 0)
  {
    ClearPath();
    RemoveAllTargets();
    SetStatus(Running);
    State(ICS_Moving);
    // stop any casting in progress
    StopCasting(FALSE, false);
  }

  fflee = GetNumFriendFlee();
  fslain = GetNumFriendSlain();
  mflee = GetNumMonsterFlee();
  mslain = GetNumMonsterSlain();
}

//*****************************************************************************
// NAME: COMBATANT::toggleReadyItem
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::toggleReadyItem(int item)
{  
  // nbr attacks remaining
  double attacksRemaining = availAttacks;

  // nbr attacks with current weapon
  // determineAvailAttacks(GetNbrAttacks());
  double oldAttacks = availAttacks;

  // how many attacks made prior to changing to new item
  double attacksUsed = oldAttacks-attacksRemaining;

  if (m_pCharacter->toggleReadyItem(item))
  {
    determineNbrAttacks();
    determineAvailAttacks(GetNbrAttacks());
    availAttacks -= attacksUsed;
    if (availAttacks < 0.0) availAttacks = 0.0;
    return TRUE;
  }
  /* This else clause appears useless  PRS 20140926
  else // nothing changed, restore original data
    availAttacks = attacksRemaining;
  */

  return FALSE;
}

//*****************************************************************************
//    NAME: COMBATANT::delCharacterItem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::delCharacterItem(int index, int qty)
{
  if (m_pCharacter->delCharacterItem(index, qty))
  {
    determineNbrAttacks();
    determineAvailAttacks(GetNbrAttacks());
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
//    NAME: COMBATANT::CurrMoveWouldFleeMap
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::CurrMoveWouldFleeMap()
{
  if ((moveX>=0)&&(moveY>=0))
    return (coordsOnMap(moveX, moveY, width, height) == FALSE);
  return FALSE;
}

//*****************************************************************************
//    NAME: COMBATANT::HandleCurrState
//
// PURPOSE: 
//
// Returns non-zero if scrren needs to be redrawn
//
//*****************************************************************************
int COMBATANT::HandleCurrState(BOOL zeroMoveAttackOK)
{
  int dude;
  int updateScreen = 0;
  switch (State())
  {
  case ICS_None:
    break;
  case ICS_Fled:
    EndTurn();
    break;
  case ICS_Casting:
    break;   
  case ICS_Using:
    break;
  case ICS_Guarding:
    //if (!CanGuard())
    //  EndTurn();
    break;
  case ICS_Bandaging:    
    //if (!CanBandage())
    //  EndTurn();
    break;
  case ICS_Turning:
    //if (!CanTurnUndead())
    //  EndTurn();
    break;

  case ICS_Attacking:
    dude = GetCurrTarget(true, false, true);
    if (dude == NO_DUDE)
      EndTurn();
    else 
    {
      if (!canAttack(dude, -1, -1, 0, Distance, false))
      EndTurn();
    }
    break;

  case ICS_Moving:    
    if (hPath < 0) 
    {
      // if actually have a destination
      if (  (moveX>=0) && (moveY>=0) && coordsOnMap(moveX, moveY, width, height))
      {
        // find path to moveX, moveY
        FindPathTo(moveX, moveY, moveX, moveY, !zeroMoveAttackOK, TRUE, TRUE);
      }
      else 
      {
        // This guy is fleeing the map. Paths of length 1 result
        // in instant flee. Longer paths need to move dude to
        // edge of map before flee is performed.
        iFleeingFlags |= fleeBecauseOffMap;
        
        if (friendly)
          IncNumFriendFlee(); //combatData.numFrndFlee++;
        else
          IncNumMonsterFlee(); //combatData.numMonFlee++;
          
        menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
        menu.setHorzOrient();
        menu.setUseActive(FALSE); // no highlighting      
        
        CString msg("");
        msg.Format("%s has fled...", GetName());
        menu.changeMenuItem(1, msg);
        UpdateCombatTimeDelayMsg();
        theApp.AppSleep(GetMsgTimeDelay()); // give time to see flee message      
        
        //BUGFIX EndTurn();
        SetStatus(Fled);
        State(ICS_Fled); //BUGFIX
        placeCombatant(x, y, NO_DUDE, width, height);         TRACE("%i has fled the map\n", self);
      }
    }

    if (hPath >= 0)
    {
      if (!TakeNextStep(TRUE))
      {
        // unable to move/attack target
        if ((OnAuto(false)) && (State() == ICS_Moving))
        {
          // end your turn but continue
          // guarding if possible          
          if (CanGuard(GUARD_AutoMaxMovement))
          {
            TRACE("Auto combatant %i can't move, ending turn and guarding\n", self);
            EndTurn(ICS_Guarding);
          }
          else
          {
            TRACE("Auto combatant %i can't move, ending turn\n", self);
            EndTurn();
          }
        }
      }
      else
      {
        updateScreen = 1;
      };
    }
//    else // can't find a path
//      EndTurn();

    moveX = -1; moveY = -1;
    if (State() != ICS_Moving)
       ClearPath();
    break;

  default:
    die(0xab4c4);
    break;
  }
  return updateScreen;
}

//*****************************************************************************
// NAME: COMBATANT::TakeNextStep
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::TakeNextStep(BOOL allowZeroMoveAttack)
{
  BOOL isUpdate = FALSE;
  CPathFinder *pathPtr = pathMgr.GetPath(hPath);

  if (pathPtr != NULL) 
  {
    CPoint stepPtr = pathPtr->GetNextStep();

    if (stepPtr.x >= 0)
    {
      TRACE("%u taking next step of path %u to %u,%u\n", 
                  self, hPath, stepPtr.x, stepPtr.y);
      isUpdate = MoveCombatant(stepPtr.x, stepPtr.y, allowZeroMoveAttack);
    }
    else
    {
      WriteDebugString("Bogus step data for path %u in TakeNextStep()\n", hPath);
      ClearPath();
    }

    if ((pathPtr->GetStepCount() <= 0) || (!isUpdate))
      ClearPath();
  }
  else
  {
    WriteDebugString("Bogus hPath %u in TakeNextStep()\n", hPath);
    ClearPath();
  }

  return isUpdate;
}

//*****************************************************************************
// NAME: COMBATANT::ReadyBestWpn
//
// PURPOSE: Select best weapon for auto dudes
//*****************************************************************************
void COMBATANT::ReadyBestWpn(int targ)
{
  BOOL isLargeTarget=FALSE;
  int dist=1;

  // get distance to target
  if (targ == NO_DUDE)
  {
    // no combat targets, nobody to shoot at, so just ready 
    // a hand-to-hand weapon
    dist = 1;
    isLargeTarget = FALSE;
  }
  else
  {
    COMBATANT *targCOMBATANT;
    targCOMBATANT = GetCombatantPtr(targ);
    ASSERT(targCOMBATANT!=NULL);
    if (targCOMBATANT!=NULL)
    {
      isLargeTarget = targCOMBATANT->isLargeDude();
      dist = Distance(self, x, y, 
                      targCOMBATANT->self, targCOMBATANT->x, targCOMBATANT->y);  
    }
  }  
  m_pCharacter->ReadyBestWpn(dist, isLargeTarget);

  // if target is distant but don't have distance weapon
  // then ready hand-to-hand weapon
  //if ((dist > 1) && (myItems.GetReady(WeaponHand) == NO_READY_ITEM))
  if ((dist > 1) && (m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0) == NO_READY_ITEM))
    m_pCharacter->ReadyBestWpn(1, isLargeTarget);
}

//*****************************************************************************
// NAME:    COMBATANT::PostCombatScreenUpdate
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::PostCombatScreenUpdate()
{
  // this is called after primary screen has been
  // blitted
}

//*****************************************************************************
// NAME:    COMBATANT::PlayCombatMove
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::PlayCombatMove() const
{
  if (GetType() == MONSTER_TYPE)
    //monsterData.PlayMove(origKey);
    monsterData.PlayMove(m_pCharacter->monsterID);
  else
    ::PlayCombatMove(); //combatData.PlayCombatMove();
}

//*****************************************************************************
//    NAME: COMBATANT::PlayCombatTurnUndead
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::PlayCombatTurnUndead() const
{
  if (GetType() == MONSTER_TYPE) return;
  ::PlayCombatTurnUndead(); //combatData.PlayCombatTurnUndead();
}

//*****************************************************************************
// NAME:    COMBATANT::PlayCombatDeath
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::PlayCombatDeath() const
{
  if (GetType() == MONSTER_TYPE)
    //monsterData.PlayDeath(origKey);
    monsterData.PlayDeath(m_pCharacter->monsterID);
  else
    ::PlayCombatDeath();//combatData.PlayCombatDeath();
}

//*****************************************************************************
// NAME: COMBATANT::ClearPath
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::ClearPath()
{
  //TRACE("Combatant %i ClearPath(%i)\n", self, hPath);
  if (hPath >= 0) 
    pathMgr.FreePath(hPath); 
  hPath = -1; 
}


//*****************************************************************************
// NAME: COMBATANT::GetCurrTarget
//
// PURPOSE:  
//*****************************************************************************
int COMBATANT::GetCurrTarget(bool updateTarget, bool unconsciousOK, bool petrifiedOK)
{ //8a2a
  if (m_target != NO_DUDE)
  {    
    COMBATANT *targCOMBATANT;
    targCOMBATANT = GetCombatantPtr(m_target);

    if (targCOMBATANT==NULL) return NO_DUDE;
    if (!targCOMBATANT->charOnCombatMap(unconsciousOK, petrifiedOK))
    {
      if (!updateTarget) return m_target;
      RemoveCurrTarget();
      if (m_target == NO_DUDE) return m_target;
      targCOMBATANT = GetCombatantPtr(m_target);
    };
    if (!combatData.IsValidTarget(this, targCOMBATANT, &targetValidity)) return NO_DUDE;
  };

  return m_target; 
}

//*****************************************************************************
// NAME: StartAttack
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::StartAttack(int targ, int additionalAttacks)
{
   ASSERT(self != NO_DUDE);
   continueAttack = FALSE;
   if (targ == NO_DUDE)
   {
     die(0xab4c5);
     WriteDebugString("StartAttack() for %i called with invalid target\n", self);
     return FALSE;
   }
   if (availAttacks + additionalAttacks <= 0) 
   {
     die(0xab4c6);
     WriteDebugString("StartAttack() for %i called with availAttacks=0\n", self);
     return FALSE;
   }
   if (!charOnCombatMap(false, true))
   {
     die(0xab4c7);
     WriteDebugString("StartAttack() for %i called when combatant not on map\n", self);
     return FALSE;
   }
   {
     const COMBATANT *pTarget;
     pTarget = GetCombatantPtr(targ);

     if (!pTarget->charOnCombatMap(false, true))
     {
       return FALSE;
     };
   };
   State(ICS_Attacking);
   StopCasting(FALSE, false);
   //PlaceCursorOnCurrentDude();
   EnsureVisibleTarget(targ);
   continueAttack=TRUE;
   //WriteDebugString("DEBUG - PlayLaunch for %s\n",m_pCharacter->name);
   //PlayLaunch();
   return TRUE;
}

//*****************************************************************************
//    NAME: COMBATANT::NeedMissileAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::NeedMissileAnimation()
{
  COMBATANT *targCOMBATANT;
  targCOMBATANT = GetCombatantPtr(m_target);
  ASSERT(targCOMBATANT!=NULL);
  if (targCOMBATANT==NULL) return FALSE;
  BOOL result = (Distance(self, x, y, 
                   m_target, targCOMBATANT->x, targCOMBATANT->y) > 1);
  TRACE("NeedMissileAnimation() %i for combatant %i, target %i\n", result,self,m_target);
  return result;
}

//*****************************************************************************
//    NAME: COMBATANT::InitMissileAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitMissileAnimation()
{
  combatSprite.Clear();
  // lookup itemData key to determine missile animation image for
  // the weapon being used.
  //GLOBAL_ITEM_ID giID = myItems.GetItem(myItems.GetReady(WeaponHand));
  //GLOBAL_ITEM_ID giID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0));
  ITEM_ID itemID;
  itemID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0));

  //if (WpnUsesAmmo(giID))
  if (WpnUsesAmmo(itemID))
    //giID = myItems.GetItem(myItems.GetReady(AmmoQuiver));
    //giID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0));
    itemID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0));
  //ITEM_DATA *data = itemData.GetItemData(giID);
  ITEM_DATA *data = itemData.GetItem(itemID);
  if (data == NULL)
  {
    WriteDebugString("InitMissileAnimation() for combatant %i failed item data lookup\n", self);
    return;  
  }

  COMBATANT *targCOMBATANT;
  targCOMBATANT = GetCombatantPtr(m_target);
  ASSERT(targCOMBATANT!=NULL);
  if (targCOMBATANT==NULL) return;
  combatSprite.Init(data->MissileArt, 
                    TRUE, // use path
                    "it_",
                    GetCenterX(), GetCenterY(), 
                    targCOMBATANT->GetCenterX(), targCOMBATANT->GetCenterY(), FALSE); 
  EnsureVisibleTarget(self);
}

//*****************************************************************************
//    NAME: COMBATANT::NeedHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::NeedHitAnimation()
{
  //int wpn = myItems.GetReady(WeaponHand);
  int wpn = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
  //WriteDebugString("DEBUG - wpn = %d\n", wpn);
  if (wpn == NO_READY_ITEM) return FALSE;
  //GLOBAL_ITEM_ID giID = m_pCharacter->myItems.GetItem(wpn);
  ITEM_ID itemID;
  itemID = m_pCharacter->myItems.GetItem(wpn);
  //if (WpnUsesAmmo(giID))
  if (WpnUsesAmmo(itemID))
    //giID = myItems.GetItem(myItems.GetReady(AmmoQuiver));
    //giID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0));
    itemID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0));
  //ITEM_DATA *data = itemData.GetItemData(giID);
  ITEM_DATA *data = itemData.GetItem(itemID);
  if (data == NULL)
  {
    WriteDebugString("NeedHitAnimation() for combatant %i failed item data lookup\n", self);
    return FALSE;  
  }
  return (data->HitArt.filename.IsEmpty() == FALSE);
}
//*****************************************************************************
//    NAME: COMBATANT::InitHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitHitAnimation()
{
  combatSprite.Clear();
  // lookup itemData key to determine hit animation image for
  // the weapon being used.
  //GLOBAL_ITEM_ID giID = myItems.GetItem(myItems.GetReady(WeaponHand));
  //GLOBAL_ITEM_ID giID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0));
  ITEM_ID itemID;
  itemID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0));

  //if (WpnUsesAmmo(giID))
  if (WpnUsesAmmo(itemID))
    //giID = myItems.GetItem(myItems.GetReady(AmmoQuiver));
    //giID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0));
    itemID = m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0));

  //ITEM_DATA *data = itemData.GetItemData(giID);
  ITEM_DATA *data = itemData.GetItem(itemID);
  if (data == NULL)
  {
    WriteDebugString("InitHitAnimation() for combatant %i failed item data lookup\n", self);
    return;  
  }

  COMBATANT *targCOMBATANT;
  targCOMBATANT = GetCombatantPtr(m_target);
  ASSERT(targCOMBATANT!=NULL);
  if (targCOMBATANT==NULL) return;
  combatSprite.Init(data->HitArt, 
                    FALSE, // is stationary
                    "it_",
                    targCOMBATANT->GetCenterX(), targCOMBATANT->GetCenterY(),
                    0,0, FALSE);

  EnsureVisibleTarget(m_target);
}

//*****************************************************************************
//    NAME: COMBATANT::NeedSpellCastAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::NeedSpellCastAnimation()
{
  CHARACTER_SPELL charSp;
  SPELL_DATA *pSdata;
  if (!FetchSpellBeingCast(&charSp))
  {
      return FALSE;
  }
  else
  {
    //pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
    pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  };
  if (pSdata==NULL) return FALSE;
  return (pSdata->CastArt.filename.IsEmpty() == FALSE);
}

//*****************************************************************************
//    NAME: COMBATANT::InitSpellCastAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitSpellCastAnimation(int targ)
{
  combatSprite.Clear();

  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return;

  combatSprite.Init(pSdata->CastArt, 
                    FALSE, // is stationary
                    "spl_",
                    GetCenterX(), GetCenterY(),
                    0,0, FALSE);

  EnsureVisibleTarget(self, TRUE);
}

//*****************************************************************************
//    NAME: COMBATANT::NeedSpellInRouteAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::NeedSpellInRouteAnimation(int mapx, int mapy, BOOL allowSelf)
{
  CHARACTER_SPELL charSp;
  SPELL_DATA *pSdata;
  if (!FetchSpellBeingCast(&charSp)) 
  {
      return FALSE;
  }
  else
  {
    //pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
    pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  };
  if (pSdata==NULL) return FALSE;
  if (!allowSelf && (x==mapx) && (y==mapy)) return FALSE;
  return (pSdata->MissileArt.filename.IsEmpty() == FALSE);  
}

//*****************************************************************************
//    NAME: COMBATANT::InitSpellInRouteAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitSpellInRouteAnimation(int mapx, int mapy)
{
  combatSprite.Clear();
  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return;

  FaceLocation(mapx,mapy);

  if (     (m_pCharacter->targets.m_targType != NowAreaLinePickStart)
        && (m_pCharacter->targets.m_targType != LingerAreaLinePickStart)
     )
  {
    // animation starts at caster
    combatSprite.Init(pSdata->MissileArt, 
                      TRUE, // use path
                      "spl_",
                      GetCenterX(), GetCenterY(), 
                      mapx, mapy, FALSE); 

    EnsureVisibleTarget(self);
  }
  else
  {
    // animation starts at mapx,mapy
    int sx = mapx;
    int sy = mapy;
    //int length = m_pCharacter->targets.m_MaxRange;
    int length = m_pCharacter->targets.MaxRange();
    PATH_DIR dir = GetSpellTargetingDir(GetCenterX(),GetCenterY(), mapx,mapy);
    int dx=sx,dy=sy;

    switch (dir)
    {
    case PathNorth:
      dx = sx;
      dy = sy-length;
      break;
    case PathNE:
      dx = sx+length;
      dy = sy-length;
      break;
    case PathEast:
      dx = sx+length;
      dy = sy;
      break;
    case PathSE:
      dx = sx+length;
      dy = sy+length;
      break;
    case PathSouth:
      dx = sx;
      dy = sy+length;
      break;
    case PathSW:
      dx = sx-length;
      dy = sy+length;
      break;
    case PathWest:
      dx = sx-length;
      dy = sy;
      break;
    case PathNW:
      dx = sx-length;
      dy = sy-length;
      break;
    }

    combatSprite.Init(pSdata->MissileArt, 
                      TRUE, // use path
                      "spl_",
                      sx, sy, 
                      dx, dy, TRUE); 

    EnsureVisible(mapx, mapy, FALSE);
  }  
}

//*****************************************************************************
//    NAME: COMBATANT::InitSpellInRouteAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitSpellInRouteAnimation(int srcx, int srcy, int dstx, int dsty)
{
  combatSprite.Clear();
  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return;

  FaceLocation(dstx,dsty);

  if (   (m_pCharacter->targets.m_targType != NowAreaLinePickStart)
      && (m_pCharacter->targets.m_targType != LingerAreaLinePickStart)
     )
  {
    // animation starts at caster
    combatSprite.Init(pSdata->MissileArt, 
                      TRUE, // use path
                      "spl_",
                      srcx, srcy, 
                      dstx, dsty, FALSE); 

    EnsureVisibleTarget(self);
  }
  else
  {
    // animation starts at mapx,mapy
    /* Really */ NotImplemented(0xc51adf, false);
    /*
    int sx = mapx;
    int sy = mapy;
    int length = targets.m_MaxRange;
    PATH_DIR dir = GetSpellTargetingDir(GetCenterX(),GetCenterY(), mapx,mapy);
    int dx=sx,dy=sy;

    switch (dir)
    {
    case PathNorth:
      dx = sx;
      dy = sy-length;
      break;
    case PathNE:
      dx = sx+length;
      dy = sy-length;
      break;
    case PathEast:
      dx = sx+length;
      dy = sy;
      break;
    case PathSE:
      dx = sx+length;
      dy = sy+length;
      break;
    case PathSouth:
      dx = sx;
      dy = sy+length;
      break;
    case PathSW:
      dx = sx-length;
      dy = sy+length;
      break;
    case PathWest:
      dx = sx-length;
      dy = sy;
      break;
    case PathNW:
      dx = sx-length;
      dy = sy-length;
      break;
    }

    combatSprite.Init(pSdata->MissileArt, 
                      TRUE, // use path
                      "spl_",
                      sx, sy, 
                      dx, dy); 

    EnsureVisible(mapx, mapy, FALSE);
    */
  }  
}

//*****************************************************************************
//    NAME: COMBATANT::NeedSpellCoverageAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::NeedSpellCoverageAnimation()
{
  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return FALSE;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return FALSE;
  return (pSdata->TargCoverageArt.filename.IsEmpty() == FALSE);
}

//*****************************************************************************
//    NAME: COMBATANT::InitSpellCoverageAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitSpellCoverageAnimation(int mapx, int mapy)
{
  combatSprite.Clear();

  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return;

  if (pSdata->TargCoverageArt.style == PIC_DATA::AS_Directional)
  {
    combatSprite.Init(pSdata->TargCoverageArt, 
                      "spl_",
                      mapx, mapy,
                      GetSpellTargetingDir(GetCenterX(), GetCenterY(), mapx,mapy));
    combatSprite.picData.AllowCentering=FALSE;
  }
  else
  {
    combatSprite.Init(pSdata->TargCoverageArt, 
                      FALSE, // is stationary
                      "spl_",
                      mapx, mapy,
                      0,0, FALSE);
  }

  EnsureVisible(mapx, mapy, TRUE);
}

//*****************************************************************************
//    NAME: COMBATANT::NeedSpellHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::NeedSpellHitAnimation()
{
  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return FALSE;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return FALSE;
  return (pSdata->TargHitArt.filename.IsEmpty() == FALSE);
}

//*****************************************************************************
//    NAME: COMBATANT::InitSpellHitAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitSpellHitAnimation(int targ)
{
  combatSprite.Clear();

  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return;

  COMBATANT *targCOMBATANT;
  targCOMBATANT = GetCombatantPtr(targ);
  ASSERT(targCOMBATANT!=NULL);
  if (targCOMBATANT==NULL) return;
  combatSprite.Init(pSdata->TargHitArt, 
                    FALSE, // is stationary
                    "spl_",
                    targCOMBATANT->GetCenterX(), targCOMBATANT->GetCenterY(),
                    0,0, FALSE);

  EnsureVisibleTarget(targ);
}

//*****************************************************************************
//    NAME: COMBATANT::NeedSpellLingerAnimation
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::NeedSpellLingerAnimation()
{
  CHARACTER_SPELL charSp;
  SPELL_DATA *pSdata;
  if (!FetchSpellBeingCast(&charSp)) 
  {
      return FALSE;
  };
  //pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return FALSE;
  if (!pSdata->Lingers) return FALSE;
  return (pSdata->TargLingerArt.filename.IsEmpty() == FALSE);
}

//*****************************************************************************
//    NAME: COMBATANT::InitSpellLingerAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitSpellLingerAnimation(/*int mapx, int mapy*/)
{
  //combatSprite.Clear();
  
  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return;
  {
    if (m_pCharacter->targets.m_maptarglist.GetCount() == 0)
    {
      // Why might there be no 'map targets'?
      ASSERT(   (m_pCharacter->targets.m_targType == LingerAreaSquare)
             && ((m_pCharacter->targets.Width()==0) || (m_pCharacter->targets.Height()==0))
            );
      return;
    }
    POSITION pos = m_pCharacter->targets.m_maptarglist.GetHeadPosition();
    while (pos != NULL)
    {
      COMBAT_SPRITE_DATA *pSprite = new COMBAT_SPRITE_DATA;
      pSprite->Init(pSdata->TargLingerArt, 
                    "spl_",
                    m_pCharacter->targets.m_maptarglist.GetAt(pos).x, m_pCharacter->targets.m_maptarglist.GetAt(pos).y);

      WriteDebugString("Adding spell animation with active spell key %i\n", combatant_activeSpellKey);
      pSprite->activeSpellKey = combatant_activeSpellKey;
      AnimateList.AddTail(pSprite);
      m_pCharacter->targets.m_maptarglist.GetNext(pos);
    };
  };
  EnsureVisible(m_pCharacter->targets.m_MapTargetX,
                m_pCharacter->targets.m_MapTargetY, 
                TRUE);
}

//*****************************************************************************
//    NAME: COMBATANT::InitSpellLingerAnimation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::InitSpellLingerAnimation(int targ)
{
  //combatSprite.Clear();
  
  CHARACTER_SPELL charSp;
  if (!FetchSpellBeingCast(&charSp)) return;
  //SPELL_DATA *pSdata = spellData.GetSpellData(GetEffectiveSpellKeyBeingCast());
  SPELL_DATA *pSdata = spellData.GetSpell(GetEffectiveSpellIDBeingCast());
  if (pSdata==NULL) return;
  switch (m_pCharacter->targets.m_targType)
  {
  case    NowSelectedByCount:
  case LingerSelectedByCount:
  case    NowSelf:
  case LingerSelf:
  case    NowWholeParty:
  case LingerWholeParty:
  case    NowTouchedTargets:
  case LingerTouchedTargets:
  case    NowSelectByHitDice:
  case LingerSelectByHitDice:
    {
      COMBAT_SPRITE_DATA *pSprite = new COMBAT_SPRITE_DATA;
      pSprite->Init(pSdata->TargLingerArt, 
                    "spl_",
                    targ);

      WriteDebugString("Adding spell animation with active spell key %i\n", combatant_activeSpellKey);
      pSprite->activeSpellKey = combatant_activeSpellKey;
      AnimateList.AddTail(pSprite);
  
    };
    break;
  default:
    return;
  }; //switch

  EnsureVisible(m_pCharacter->targets.m_MapTargetX,
                m_pCharacter->targets.m_MapTargetY, 
                TRUE);
}
//*****************************************************************************
// NAME  DetermineIfBackStab
//
// PURPOSE: Determine if 'self' has the proper weapon and
// proper position to attempt a backstab on an opponent
// who is unaware of our presence.
//*****************************************************************************
BOOL COMBATANT::DetermineIfBackStab(int wpn, int targ) const
{
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;
  int dx, dy;
  int distraction;
  COMBATANT *pTarget;
  COMBATANT *pDistraction;
  weaponClassType weaponType;
  //if (!isThief(GetClass())) return FALSE;




/*
  const CLASS_DATA *pClass;
  pClass = classData.PeekClass(m_pCharacter->classID);
  if (pClass == NULL) return FALSE;
  {
    int i,n;
    SKILL_ID backstabSkillID;
    backstabSkillID = Skill_BackstabMultiplier;
    n = pClass->GetCount();
    for (i=0; i<n; i++)
    {
      const BASE_CLASS_DATA *pBaseclass;
      pBaseclass = baseclassData.PeekBaseclass(*pClass->PeekBaseclassID(i));
      //if (!pBaseclass->m_backstabAttribute.IsEmpty()) break;
      if (pBaseclass->GetSkillValue(backstabSkillID) != NoSkill) break;
    };
    if (i == n) return FALSE;
  };
*/
  {
    int backstabSkill;
    SKILL_ID skillID;
    skillID = Skill_BackstabMultiplier;
    backstabSkill = this->m_pCharacter->GetAdjSkillValue(skillID, false, true);
    if (backstabSkill == NoSkill) return FALSE;
  };




  if (wpn == NO_READY_ITEM) return FALSE;
  //giID = m_pCharacter->myItems.GetItem(wpn);
  itemID = m_pCharacter->myItems.GetItem(wpn);
  //if (giID.IsNoItem()) return FALSE;
  if (itemID.IsNoItem()) return FALSE;
  //weaponType = itemData.GetWpnType(giID);
  weaponType = itemData.GetWpnType(itemID);
  if (    (weaponType != HandBlunt) 
       && (weaponType != HandCutting)
       && (weaponType != HandThrow)
     ) return FALSE;
  //
  // Now decide if we are standing adjacent to our enemy.
  pTarget = GetCombatantPtr(targ);
  dx = abs(x - pTarget->x);
  dy = abs(y - pTarget->y);
  if (dx > 1) return FALSE;
  if (dy > 1) return FALSE;
  //
  // OK.  Now let us determine if our target is quite busy
  // fighting off one of our friends.
  // We do this by checking that he has just attacked
  // with a combatant that is within 1 square of himself.
  if (pTarget->m_eLastAction == LA_None) return FALSE;
  //
  distraction = NO_DUDE;
  if (pTarget->m_eLastAction == LA_Attack) 
  {
      distraction = pTarget->m_iLastAttacked;
  }
  else
  {
    if (pTarget->m_iLastAttacked == pTarget->m_iLastAttacker)
    {
      distraction = pTarget->m_iLastAttacked;
    };
  }
  //
  if (distraction == NO_DUDE) return FALSE;
  pDistraction = GetCombatantPtr(distraction);
  if (pDistraction->GetAdjStatus() != Okay) return FALSE;
  //
  // Now see if we are on the opposite side of the target
  // from the distraction at the same distance.
  if ( (x - pTarget->x != pTarget->x - pDistraction->x) ) return FALSE;
  if ( (y - pTarget->y != pTarget->y - pDistraction->y) ) return FALSE;
  return TRUE;
}

extern COMBAT_DATA combatData;

class DamageComputation
{
  int m_damage;
  //GLOBAL_SPELL_ID m_spellDbKey;
  SPELL_ID m_spellID;
  BOOL m_isNonLethal;
  CString m_message;
public:
  DamageComputation(void){};
  int Damage(void){return m_damage;};
  BOOL IsNonLethal(void){return m_isNonLethal;};
  void Compute(const COMBATANT *pAttacker,
               const COMBATANT *pTarget,
               int wpn,
               int toHitRoll,
               BOOL isBackStab,
               int  backstabMultiplier);
  //GLOBAL_SPELL_ID SpellDbKey(void) {return m_spellDbKey;};
  SPELL_ID SpellID(void) {return m_spellID;};
  CString Message(void){return m_message;};
};


int GetAttackerTHAC0(const COMBATANT *pAttacker)
{
  return pAttacker->m_pCharacter->GetTHAC0();
}

int GetWeaponToHitBonus(ITEM_DATA *pWeapon)
{  
  if (pWeapon != NULL)
  {      
    return pWeapon->Attack_Bonus;
  }
  else
  {
    return 0;
  };
}

int GetEnvironmentalBonusTHAC0(const COMBATANT *pAttacker, ITEM_DATA *pWeapon, int distance)
{
  int environmentalBonusTHAC0;
  if (pWeapon != NULL)
  {
    weaponClassType weaponType;
    weaponType = pWeapon->Wpn_Type;
    switch (weaponType)
    {
      case NotWeapon:
      case HandBlunt:
      case HandCutting:
        environmentalBonusTHAC0 = pAttacker->m_pCharacter->GetHitBonus();
        break;
      case HandThrow:
        environmentalBonusTHAC0 = (distance == 0)?pAttacker->m_pCharacter->GetHitBonus():0;
        break;
      case SlingNoAmmo:
      case Bow:
      case Crossbow:
      case Throw:
        environmentalBonusTHAC0 = 0;
        break;
      case Ammo:
        environmentalBonusTHAC0 = 0;
        break;
      case SpellCaster:
      case SpellLikeAbility:
        environmentalBonusTHAC0 = 0;
        break;
      default:
        environmentalBonusTHAC0 = 0;
        break;
    };  
  }
  else
  {
    environmentalBonusTHAC0 = 0;
  };
  return environmentalBonusTHAC0;
}

//int GetEffectiveTargetAC(const COMBATANT *pTarget, const COMBATANT *pAttacker, GLOBAL_ITEM_ID giID)
int GetEffectiveTargetAC(const COMBATANT *pTarget, const COMBATANT *pAttacker, const ITEM_ID& itemID)
{
  int effectiveTargetAC;
  effectiveTargetAC = pTarget->GetAdjAC();
  //pTarget->ModifyACAsTarget(pAttacker->m_pCharacter, &effectiveTargetAC, giID);
  pTarget->ModifyACAsTarget(pAttacker->m_pCharacter, &effectiveTargetAC, itemID);
  effectiveTargetAC += pTarget->m_pCharacter->myItems.GetProtectModForRdyItems();
  return effectiveTargetAC;
}

int ToHitComputation::ComputeEffectiveTHAC0(const COMBATANT *pAttacker,
                                            const COMBATANT *pTarget,
                                            //GLOBAL_ITEM_ID weaponID,
                                            const ITEM_ID&   weaponID,
                                            int distance)
{
/*
********************* Revised xxxxx Algorithm *******************
********************* (name to be determined)

0)AttackerTHAC0 = THAC0 from character's stats (14)
1)something001 = some function(weaponType-HandCutting, etc) (0)
2)something002 = AttackerTHAC0 - something001 (14 - 0 = 14)
3)WeaponToHitBonus = WeaponBonus from weapon description (5)
4)something003 = something002 - WeaponToHitBonus (14 - 5 = 9)
5)EffectiveTargetAC = some function (target and shields and etc) (9)
6)EffectiveTHAC0 = something003 - EffectiveTargetAC (9 - 9 = 0)


Re-writing gives:
6) EffectiveTHAC0 = (something002 - WeaponToHitBonus) - EffectiveTargetAC
6) EffectiveTHAC0 = something002 - WeaponToHitBonus - EffectiveTargetAC
6) EffectiveTHAC0 = (AttackerTHAC0 - somethin001) - WeaponToHitBonus - EffectiveTargetAC
6) EffectiveTHAC0 = AttackerTHAC0 - somethin001 - WeaponToHitBonus - EffectiveTargetAC
6) EffectiveTHAC0 = AttackerTHAC0 - environmentalBonusTHAC0 - WeaponToHitBonus - EffectiveTargetAC


Then use EffectiveTHAC0 as parameter[6]
**************** End of xxxxx Algorithm **************** 

Eric Cone (manikus) wrote to me:

I am officially signing off on this. :)
ComputerEffectiveTHAC0 seems like a good name to me.


something001 -> environmentalBonusTHAC0
something002 -> adjustedAttackerTHAC0
something003 -> RealizedTHAC0
 

*/  
  int effectiveTHAC0;  
  int attackerTHAC0;
  int environmentalBonusTHAC0;
  int weaponToHitBonus;
  int effectiveTargetAC;

  ITEM_DATA *pWeapon = NULL;
  //if (weaponID.IsValidItem())
  if (weaponID.IsValidItem())
  {
    //pWeapon = itemData.GetItemData(weaponID);
    pWeapon = itemData.GetItem(weaponID);
  };

  //////////////////////// attackerTHAC0
  attackerTHAC0 = GetAttackerTHAC0(pAttacker);

  /////////////////////// weaponToHitBonus
  weaponToHitBonus = GetWeaponToHitBonus(pWeapon);

  /////////////////////// environmentalBonusTHAC0
  environmentalBonusTHAC0 = GetEnvironmentalBonusTHAC0(pAttacker, pWeapon, distance);

  ///////////////////////// effectiveTargtAC
  //effectiveTargetAC = GetEffectiveTargetAC(pTarget, pAttacker, weaponID);
  effectiveTargetAC = GetEffectiveTargetAC(pTarget, pAttacker, weaponID);

  ///////////////////////// effectiveTHAC0
  effectiveTHAC0 = attackerTHAC0 - environmentalBonusTHAC0 - weaponToHitBonus - effectiveTargetAC;

  return effectiveTHAC0;
}


void ToHitComputation::Compute(const COMBATANT *pAttacker,
                               const int targetIndex,
                               const COMBATANT *pTarget,
                               int wpn)
{
  int attDiceSides;
  int attDiceNum;
  int distance;
  int bsm;
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;
  SKILL_ID backstabSkillID;


  distance = Distance(pAttacker->self, pAttacker->x, pAttacker->y, 
                      pTarget->self, pTarget->x, pTarget->y);  

  //attDiceSides=20;
  //attDiceNum=1;


  attDiceSides=20;
  attDiceNum=1;

  m_attDiceBonus = 0;

  //giID.Clear();
  itemID.Clear();
  if (wpn != NO_READY_ITEM)
  {
    //giID = pAttacker->m_pCharacter->myItems.GetItem(wpn);
    itemID = pAttacker->m_pCharacter->myItems.GetItem(wpn);
  };

  pAttacker->ModifyAttackRollDice (
                            pTarget->m_pCharacter,
                            attDiceNum,
                            attDiceSides,
                          & m_attDiceBonus);

  pAttacker->ModifyAttackRollDiceForItem(  
                            pTarget->m_pCharacter, 
                            //*(GLOBAL_ITEM_ID *)&wpn,  // Investigate this!!
                            itemID,  // Investigate this!!
                            attDiceNum,
                            attDiceSides,
                          & m_attDiceBonus,
                            distance);

  pTarget->ModifyAttackRollDiceAsTarget(  
                            pAttacker->m_pCharacter,
                            attDiceNum,
                            attDiceSides,
                          & m_attDiceBonus);

  pTarget->ModifyAttackRollDiceForItemAsTarget(  
                            pAttacker->m_pCharacter, 
                            //*(GLOBAL_ITEM_ID *)&wpn,  //Investigate this!!
                            itemID,  //Investigate this!!
                            attDiceNum,
                            attDiceSides,
                          & m_attDiceBonus);


  m_toHitDiceRoll = RollDice(attDiceSides,
                             attDiceNum, 
                             m_attDiceBonus);


  // backstabSkillID = Skill_BackstabMultiplier;
//  if (!GetAdjSkillValue(backstabSkillID,
//                        pAttacker->m_pCharacter,
//                        NULL,
//                        NULL,
//                       &m_backstabMultiplier,
//                        false))

  bsm = pAttacker->m_pCharacter->GetAdjSkillValue(Skill_BackstabMultiplier, false, true);
  if (bsm == NoSkill)
  {
    m_isBackStab = FALSE;
  }
  else
  {
    m_isBackStab = pAttacker->DetermineIfBackStab(wpn, targetIndex);
  };
  //if (m_isBackStab) m_attDiceBonus += pAttacker->m_pCharacter->GetThiefBackstabAttackAdjustment();


  //m_attDiceBonus += pAttacker->GetAttackBonus(giID, distance);
  m_attDiceBonus += pAttacker->GetAttackBonus(itemID, distance);
  if (m_toHitDiceRoll < 1)  m_toHitDiceRoll = 1;
  if (m_toHitDiceRoll > 20) m_toHitDiceRoll = 20;
   



  //m_effectiveTHAC0 = ComputeEffectiveTHAC0(pAttacker, pTarget, giID, distance);
  m_effectiveTHAC0 = ComputeEffectiveTHAC0(pAttacker, pTarget, itemID, distance);
/*
  m_effectiveTHAC0 = pAttacker->GetAdjTHAC0() - m_attDiceBonus;
  
  
  m_effectiveTargetAC = pTarget->GetAdjAC();
  //pTarget is combatant
  pTarget->ModifyACAsTarget(pAttacker->m_pCharacter, &m_effectiveTargetAC, giID);
  m_effectiveTargetAC += pTarget->m_pCharacter->myItems.GetProtectModForRdyItems();




  m_effectiveTHAC0 -= m_effectiveTargetAC;
  pAttacker->ModifyAttackThac0(pTarget->m_pCharacter, &m_effectiveTHAC0);
  pTarget->ModifyAttackThac0AsTarget(pAttacker->m_pCharacter, &m_effectiveTHAC0);

  if (m_effectiveTHAC0 < MIN_THAC0)
    m_effectiveTHAC0 = 0;


  TRACE("%i attacks %i: rolls %i, needs %i\n", 
                   pAttacker->self, pTarget->self, m_toHitDiceRoll, m_effectiveTHAC0);

*/

  if (pAttacker->friendly)
  {
    if      (GetConfigPartyAlwaysMiss()) m_toHitDiceRoll = m_effectiveTHAC0 - 1;
    else if (GetConfigPartyAlwaysHit())  m_toHitDiceRoll = m_effectiveTHAC0;
  }
  else
  {
    if      (GetConfigMonsterAlwaysMiss()) m_toHitDiceRoll = m_effectiveTHAC0 - 1;
    else if (GetConfigMonsterAlwaysHit())  m_toHitDiceRoll = m_effectiveTHAC0;
  };
  {
    CString result, finalResult, itemName;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    ITEM_DATA *pItem;
    //SetItemContext(giID);
    //scriptContext.SetItemContext(giID);  // Includes spell Context!
    scriptContext.SetItemContext(itemID);  // Includes spell Context!
    scriptContext.SetItemContextKey(wpn);
    scriptContext.SetTargetContext(pTarget);
    scriptContext.SetAttackerContext(pAttacker);
    if (itemID.IsValidItem())
    {
      itemName = scriptContext.GetItemContext("Bogus Item Context")->UniqueName();
    }
    else
    {
      itemName = "Bogus Item";
    };
    hookParameters[5].Format("%d",m_toHitDiceRoll);
    hookParameters[6].Format("%d",m_effectiveTHAC0);
    result = pAttacker->RunCombatantScripts
                (DOES_ATTACK_SUCCEED,
                 ScriptCallback_RunAllScripts,
                 "YN",
                 "To-Hit versus THAC0");
    if (!result.IsEmpty()) finalResult = result;
    result = pAttacker->m_pCharacter->RunCharacterScripts
                (DOES_ATTACK_SUCCEED,
                 ScriptCallback_RunAllScripts,
                 "YN",
                 "To-Hit versus THAC0");
    if (!result.IsEmpty()) finalResult = result;
    result = pTarget->RunCombatantScripts
                (DOES_ATTACK_SUCCEED,
                 ScriptCallback_RunAllScripts,
                 "YN",
                 "To-Hit versus THAC0");
    if (!result.IsEmpty()) finalResult = result;
    result = pTarget->m_pCharacter->RunCharacterScripts
                (DOES_ATTACK_SUCCEED,
                 ScriptCallback_RunAllScripts,
                 "YN",
                 "To-Hit versus THAC0");
    if (!result.IsEmpty()) finalResult = result;
    //if (giID.IsValidItem())
    if (itemID.IsValidItem())
    {
      //pItem = itemData.GetItemData(giID);
      pItem = itemData.GetItem(itemID);
      if (pItem != NULL)
      {
        result = pItem->RunItemScripts
                    (DOES_ATTACK_SUCCEED,
                     ScriptCallback_RunAllScripts,
                     "YN",
                     "To-Hit versus THAC0");
        if (!result.IsEmpty()) finalResult = result;
        if (scriptContext.pSpellContext != NULL)
        {
          result = scriptContext.GetSpellContext(NULL)->RunSpellScripts
                      (DOES_ATTACK_SUCCEED,
                       ScriptCallback_RunAllScripts,
                       "YN",
                       "To-Hit versus THAC0");
        if (!result.IsEmpty()) finalResult = result;
        };
      };
    };
    const CLASS_DATA *pClass;
    //pClass = classData.GetClassData(pAttacker->GetClass());
    pClass = classData.PeekClass(pAttacker->GetClass());
    if (pClass != NULL)
    {
      result = pClass->RunClassScripts
                  (DOES_ATTACK_SUCCEED,
                   ScriptCallback_RunAllScripts,
                   "YN",
                   "To-Hit versus THAC0");
    }
    else
    {
      result.Empty();
    };
    if (!result.IsEmpty()) finalResult = result;
    {
      const RACE_DATA *pRace;
      //pRace = raceData.GetRaceData(pAttacker->race());
      //pRace = raceData.PeekRace(pAttacker->race());
      pRace = pAttacker->m_pCharacter->PeekRaceData();
      result.Empty();
      if (pRace != NULL)
      {
        result = pRace->RunRaceScripts
                    (DOES_ATTACK_SUCCEED,
                     ScriptCallback_RunAllScripts,
                     "YN",
                     "To-Hit versus THAC0");
        if (!result.IsEmpty()) finalResult = result;
      };
    };
    {
      const CLASS_DATA *pTargClass;
      //pTargClass = classData.GetClassData(pTarget->GetClass());
      pTargClass = classData.PeekClass(pTarget->GetClass());
      result.Empty();
      if (pTargClass != NULL)
      {
        result = pTargClass->RunClassScripts
                    (DOES_ATTACK_SUCCEED,
                     ScriptCallback_RunAllScripts,
                     "YN",
                     "To-Hit versus THAC0");
        if (!result.IsEmpty()) finalResult = result;
      };
    }
    {
      const RACE_DATA *pRace;
      //pRace = raceData.GetRaceData(pTarget->race());
      //pRace = raceData.PeekRace(pTarget->race());
      pRace = pTarget->m_pCharacter->PeekRaceData();
      result.Empty();
      if (pRace != NULL)
      {
        result = pRace->RunRaceScripts
                    (DOES_ATTACK_SUCCEED,
                     ScriptCallback_RunAllScripts,
                     "YN",
                     "To-Hit versus THAC0");
        if (!result.IsEmpty()) finalResult = result;
      };
    };
    {
      if (pTarget->GetType() == MONSTER_TYPE)
      {
        //int monsterIndex;
        //monsterIndex = pTarget->origKey;
        MONSTER_ID monsterID;
        monsterID = pTarget->m_pCharacter->monsterID;
        {
          //MONSTER_DATA *pMonsterData;
          const MONSTER_DATA *pMonsterData;
          //pMonsterData = monsterData.GetMonsterData(monsterIndex);
          pMonsterData = monsterData.PeekMonster(monsterID);
          result.Empty();
          if (pMonsterData != NULL)
          {
            result = pMonsterData->RunMonsterScripts(
                     DOES_ATTACK_SUCCEED,
                     ScriptCallback_RunAllScripts,
                     NULL,
                     "To-Hit versus THAC0");
           if (!result.IsEmpty()) finalResult = result;
          };
        };
      };

    };
    //ClearItemContext();
    {
      if (finalResult.Find('N') >= 0) m_didHit = 0;
      else if (finalResult.Find('Y') >= 0) m_didHit = 1;
    };
  };

  if (m_didHit < 0) m_didHit = (m_toHitDiceRoll >= m_effectiveTHAC0);
}

void DamageComputation::Compute(const COMBATANT *pAttacker,
                                const COMBATANT *pTarget,
                                int   wpn,
                                int   toHitRoll,
                                BOOL  isBackStab,
                                int   backstabMultiplier)

  //GLOBAL_ITEM_ID giID;
{
  ITEM_ID itemID;
  int distance;

  int dmgDiceQty, dmgDiceSides, dmgDiceBonus;

  distance = Distance(pAttacker->self, pAttacker->x, pAttacker->y,
                      pTarget->self,   pTarget->x,   pTarget->y);

  //giID.Clear();
  itemID.Clear();
  if (wpn != NO_READY_ITEM)
  {
    //giID = pAttacker->m_pCharacter->myItems.GetItem(wpn);
    itemID = pAttacker->m_pCharacter->myItems.GetItem(wpn);
  };

  
  
  m_damage = 0;
  pAttacker->GetDamageDice(
              wpn, 
            & dmgDiceQty, 
            & dmgDiceSides, 
            & dmgDiceBonus, 
            & m_isNonLethal, 
              pTarget->isLargeDude(),
            //& m_spellDbKey);
            & m_spellID);

  //dmgDiceBonus += m_iTmpDmgDiceBonus; // for backstabbing
    
  pAttacker->ModifyAttackDamageDice(
#ifdef newCombatant
                    pTarget->m_pCharacter,   
#else
                    pTarget,   
#endif
                    dmgDiceQty, 
                    dmgDiceSides, 
                  & dmgDiceBonus, 
                  & m_isNonLethal);

  pAttacker->ModifyAttackDamageDiceForItem(
#ifdef newCombatant
                    pTarget->m_pCharacter,
#else
                    pTarget,
#endif
                    //giID, // 20110525 PRS // //*(GLOBAL_ITEM_ID *)&wpn,  // Investigate this!!
                    itemID,
                    dmgDiceQty,  
                    dmgDiceSides,  
                  & dmgDiceBonus,  
                  & m_isNonLethal,
                    distance);

  pTarget->ModifyAttackDamageDiceAsTarget(
#ifdef newCombatant
                    pAttacker->m_pCharacter,
#else
                    pAttacker,
#endif
                    dmgDiceQty,
                    dmgDiceSides,
                  & dmgDiceBonus,
                  & m_isNonLethal);

  pTarget->ModifyAttackDamageDiceForItemAsTarget(
#ifdef newCombatant
                    pAttacker->m_pCharacter,
#else
                    pAttacker,
#endif
                    //giID, // 20110525 PRS //*(GLOBAL_ITEM_ID *)&wpn,  // Investigate this!!
                    itemID,
                    dmgDiceQty,
                    dmgDiceSides,
                  & dmgDiceBonus,
                  & m_isNonLethal,
                    toHitRoll  //toHitComputation.Rolled()
                  );

  m_damage = RollDice(dmgDiceSides, dmgDiceQty, dmgDiceBonus);
     
  if (m_damage <= 0) 
    m_damage = 1;

  TRACE("%i rolls %i damage using %i sides, %i qty, %i bonus (%s)\n",
                     pAttacker->self, m_damage, dmgDiceSides, dmgDiceQty, dmgDiceBonus,
                     (m_isNonLethal?"NonLethal":"Lethal"));


  if (m_damage > 0)
  {        
    if (isBackStab)
    {
      m_damage = m_damage*backstabMultiplier / 100;
    };
    {
      CString result;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      ITEM_DATA *pWeapon=NULL;
      //scriptContext.SetItemContext(giID);
      scriptContext.SetItemContext(itemID);
      //if (!giID.IsNoItem())
      if (!itemID.IsNoItem())
      {
        //pWeapon = itemData.GetItemData(giID);
        pWeapon = itemData.GetItem(itemID);
      };
      hookParameters[2].Format("%d", m_damage);
      if (pWeapon != NULL)
      {
        if (pWeapon->Wpn_Type == Ammo)
        {      
          // We need to find the launching weapon.
          int launchWpnIndex;
          launchWpnIndex = pAttacker->m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
          if (launchWpnIndex != NO_READY_ITEM)
          {
            //GLOBAL_ITEM_ID launch_giID;
            ITEM_ID launchID;
            ITEM_DATA *pLauncher;
            //launch_giID = pAttacker->m_pCharacter->myItems.GetItem(launchWpnIndex);
            launchID = pAttacker->m_pCharacter->myItems.GetItem(launchWpnIndex);
            //pLauncher = itemData.GetItemData(launch_giID);
            pLauncher = itemData.GetItem(launchID);
            hookParameters[3].Format("%d",pLauncher->Attack_Bonus);
            m_damage += pLauncher->Attack_Bonus;
          };
        }
        hookParameters[4].Format("%d",pWeapon->Attack_Bonus);
        m_damage += pWeapon->Attack_Bonus;
      };
      hookParameters[5].Format("%d",m_damage);
      hookParameters[6].Format("%d",m_isNonLethal);
      scriptContext.SetAttackerContext(pAttacker);
      scriptContext.SetTargetContext(pTarget);
      if (pWeapon != NULL)
      {
        scriptContext.SetItemContext(pWeapon);
      }
      else
      {
        scriptContext.SetItemContext(&bogusItem);
      };
      scriptContext.SetItemContextKey(wpn);
      if (pWeapon != NULL)
      {
        result = pWeapon->RunItemScripts(
                 COMPUTE_DAMAGE,
                 ScriptCallback_RunAllScripts,
                 NULL,
                 "Compute damage of attack");
      };
      if (pTarget->GetType() == MONSTER_TYPE)
      {
        //int monsterIndex;
        //monsterIndex = pTarget->origKey;
        MONSTER_ID monsterID;
        monsterID = pTarget->m_pCharacter->monsterID;
        {
          //MONSTER_DATA *pMonsterData;
          const MONSTER_DATA *pMonsterData;
          //pMonsterData = monsterData.GetMonsterData(monsterIndex);
          pMonsterData = monsterData.PeekMonster(monsterID);
          if (pMonsterData != NULL)
          {
            result = pMonsterData->RunMonsterScripts(
                     COMPUTE_DAMAGE,
                     ScriptCallback_RunAllScripts,
                     NULL,
                     "Compute damage of attack");
          };
        };
      };
      result = pTarget->RunCombatantScripts(
               COMPUTE_DAMAGE,
               ScriptCallback_RunAllScripts,
               NULL,
               "Compute damage of attack");
      m_message = hookParameters[7];
      //if (!hookParameters[5].IsEmpty())
      {
        int temp;
        if (sscanf(hookParameters[5],"%d",&temp) == 1) m_damage = temp;
      };
    };
  };

}

int ComputeAttackDamage(const COMBATANT *pAttacker, const COMBATANT *pTarget)
{
  ToHitComputation toHitComputation;
  DamageComputation damageComputation;
  int wpn;
  //wpn = pAttacker->myItems.GetReady(WeaponHand);
#ifdef newCombatant
  wpn = pAttacker->m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
#else
  wpn = pAttacker->myItems.GetReadiedItem(WeaponHand, 0);
#endif

  toHitComputation.BeginSpellScriptFailure(0);
  toHitComputation.Compute(pAttacker, pTarget->self, pTarget, wpn);
  if (!toHitComputation.DidHit()) return 0;
  damageComputation.Compute(pAttacker, 
                            pTarget, 
                            wpn, 
                            toHitComputation.Rolled(),
                            toHitComputation.IsBackStab(),
                            toHitComputation.BackstabMultiplier());
  return damageComputation.Damage();
}


//*****************************************************************************
// NAME: makeAttack
//
// PURPOSE:  
//*****************************************************************************
int COMBATANT::makeAttack(int targ, int extraAttacksAvailable, int *pDeathIndex)
{
  BOOL wpnConsumesSelfAsAmmo = FALSE; // Assign value to make compiler happy!
  if (targ == 0)
  {
    ASSERT(TRUE);
  };
  ToHitComputation toHitComputation;
  ASSERT(self != NO_DUDE);
  if (IsDone(false, "Can combatant make attack")) return 1;
  toHitComputation.BeginSpellScriptFailure(0);
  CombatMsg = "";
  ClearFormattedText(combatTextData);

  if (targ == NO_DUDE) return 1;
  if (availAttacks + extraAttacksAvailable <= 0) return 1;

  //int thac0;//, ac;
  //int dmgDiceQty, dmgDiceSides, dmgDiceBonus;
  //int attDiceBonus;
  //BOOL IsNonLethal;
  //BOOL isBackStab;

  {
    CString msg;
    int nbrattacks = GetNbrAttacks();
    int availattacks = availAttacks;
    int currAttack = nbrattacks - availattacks;

    //20111023 PRSASSERT( (currAttack>=0) && (currAttack<nbrattacks) );
    if ((currAttack < 0) || (currAttack >= nbrattacks)) currAttack = 0;
    //monsterData.GetMonsterAttackMsg(origKey, currAttack, msg);
    monsterData.GetMonsterAttackMsg(m_pCharacter->monsterID, currAttack, msg);
    if (msg == "*NoAttack*") return 1;
  };

  EnsureVisibleTarget(targ);

  lastAttackRound = GetCurrentRound();
  State(ICS_Attacking);
  StopCasting(FALSE, false);

  COMBATANT *targCOMBATANT;
  targCOMBATANT = GetCombatantPtr(targ);
  ASSERT(targCOMBATANT != NULL);
  if (targCOMBATANT == NULL) return 1;
  if (targCOMBATANT->GetStatus() == Dead) return 1; // not on the map anymore


  BOOL decQty = FALSE;
  int wpn;
  //int ammo;
  //wpn = myItems.GetReady(WeaponHand);
  wpn = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;
  //giID.Clear();
  itemID.Clear();

  if (wpn != NO_READY_ITEM)
  {
    //giID = m_pCharacter->myItems.GetItem(wpn);
    itemID = m_pCharacter->myItems.GetItem(wpn);
    int dist = Distance(self, x, y,
      targCOMBATANT->self, targCOMBATANT->x, targCOMBATANT->y);
    //decQty = WpnConsumesAmmoAtRange(giID, dist);
    decQty = WpnConsumesAmmoAtRange(itemID, dist);

    // do we decrement weapon qty or ammo qty?
    //wpnConsumesSelfAsAmmo = WpnConsumesSelfAsAmmo(giID);
    wpnConsumesSelfAsAmmo = WpnConsumesSelfAsAmmo(itemID);
    if (decQty && !wpnConsumesSelfAsAmmo)
    {
      // ammo is readied and must be decremented
      //wpn = myItems.GetReady(AmmoQuiver);
      wpn = m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0);
      //giID = m_pCharacter->myItems.GetItem(wpn);
      itemID = m_pCharacter->myItems.GetItem(wpn);
    }
  }

  //isBackStab = DetermineIfBackStab(wpn, targ);

  //attDiceBonus = GetAttackBonus() + m_iTmpAttDiceBonus;
  //if (isBackStab) attDiceBonus += getThiefBackstabAttackAdjustment();
  {
    ActorType actor;
    this->GetContext(&actor);
    SetCharContext(&actor);
    targCOMBATANT->GetContext(&actor);
    SetTargetContext(&actor);
  };
  ITEM_DATA *pWeapon = NULL;
  if (!itemID.IsNoItem())
  {
    //pWeapon = itemData.GetItemData(giID);
    pWeapon = itemData.GetItem(itemID);
  };

  //m_iAttDiceSides=20;
  //m_iAttDiceNum=1;

  //ModifyAttackRollDice ( 
  //                          targCOMBATANT,
  //                        & m_iAttDiceNum,
  //                        & m_iAttDiceSides,
  //                        & attDiceBonus);

  //ModifyAttackRollDiceForItem(  
  //                          targCOMBATANT, 
  //                          *(GLOBAL_ITEM_ID *)&wpn,  // Investigate this!!
  //                        & m_iAttDiceNum,
  //                        & m_iAttDiceSides,
  //                        & attDiceBonus);

  //targCOMBATANT->ModifyAttackRollDiceAsTarget(  
  //                          this,
  //                        & m_iAttDiceNum,
  //                        & m_iAttDiceSides,
  //                        & attDiceBonus);

  //targCOMBATANT->ModifyAttackRollDiceForItemAsTarget(  
  //                          this, 
  //                          *(GLOBAL_ITEM_ID *)&wpn,  //Investigate this!!
  //                        & m_iAttDiceNum,
  //                        & m_iAttDiceSides,
  //                        & attDiceBonus);

  toHitComputation.Compute(this, targ, targCOMBATANT, wpn);

  //thac0 = GetAdjTHAC0() - toHitComputation.AttackDiceBonus();
   
  //int targAC = targCOMBATANT->GetAdjAC();
  //targCOMBATANT->ModifyACAsTarget(this, &targAC, giID);
  //targAC += targCOMBATANT->myItems.GetProtectModForRdyItems();
  //thac0 -= targAC;
  //ModifyAttackThac0(targCOMBATANT, &thac0);
  //targCOMBATANT->ModifyAttackThac0AsTarget(this, &thac0);

  //if (thac0 < 0)
  //  thac0 = 0;



  //m_iAttDiceRoll = RollDice(m_iAttDiceSides, m_iAttDiceNum, attDiceBonus);

  //if (m_iAttDiceRoll < 1)  m_iAttDiceRoll = 1;
  //if (m_iAttDiceRoll > 20) m_iAttDiceRoll = 20;
   
  //TRACE("%i attacks %i: rolls %i, needs %i\n", 
  //                 self, targ, m_iAttDiceRoll, thac0);

  //if (friendly)
  //{
  //  if      (GetConfigPartyAlwaysMiss()) m_iAttDiceRoll = thac0 - 1;
  //  else if (GetConfigPartyAlwaysHit())  m_iAttDiceRoll = thac0;
  //}
  //else
  //{
  //  if      (GetConfigMonsterAlwaysMiss()) m_iAttDiceRoll = thac0 - 1;
  //  else if (GetConfigMonsterAlwaysHit())  m_iAttDiceRoll = thac0;
  //};
  //{
  //  CString result, itemName;
  //  ITEM_DATA *pItem;
  //  //SetItemContext(giID);
  //  scriptContext.SetItemContext(giID);  // Includes spell Context!
  //  scriptContext.SetTargetContext(targCOMBATANT);
  //  scriptContext.SetAttackerContext(this);
  //  if (giID.IsValidItem())
  //  {
  //    itemName = scriptContext.GetItemContext()->Name;
  //  }
  //  else
  //  {
  //    itemName = "Bogus Item";
  //  };
  //  PrepareHooks();
  //  hookParameters[5].Format("%d",m_iAttDiceRoll);
  //  hookParameters[6].Format("%d",thac0);
  //  result = this->combatantSA.RunScripts
  //              (DOES_ATTACK_SUCCEED,
  //               ScriptCallback_LookForChar,
  //               "YN",
  //               this->GetName(),
  //               itemName);
  //  if (result.IsEmpty())
  //  {
  //    result = this->specAbs.RunScripts
  //                (DOES_ATTACK_SUCCEED,
  //                 ScriptCallback_LookForChar,
  //                 "YN",
  //                 this->GetName(),
  //                 itemName);
  //  };
  //  if (result.IsEmpty())
  //  {
  //    result = targCOMBATANT->combatantSA.RunScripts
  //                (DOES_ATTACK_SUCCEED,
  //                 ScriptCallback_LookForChar,
  //                 "YN",
  //                 targCOMBATANT->GetName(),
  //                 itemName);
  //  };
  //  if (result.IsEmpty())
  //  {
  //    result = targCOMBATANT->specAbs.RunScripts
  //                (DOES_ATTACK_SUCCEED,
  //                 ScriptCallback_LookForChar,
  //                 "YN",
  //                 targCOMBATANT->GetName(),
  //                 itemName);
  //  };
  //  if (result.IsEmpty() && giID.IsValidItem())
  //  {
  //    pItem = itemData.GetItemData(giID);
  //    if (pItem != NULL)
  //    {
  //      result = pItem->specAbs.RunScripts
  //                  (DOES_ATTACK_SUCCEED,
  //                   ScriptCallback_LookForChar,
  //                   "YN",
  //                   targCOMBATANT->GetName(),
  //                   itemName);
  //      if (result.IsEmpty())
  //      {
  //        if (scriptContext.GetSpellContext() != NULL)
  //        {
  //          result = scriptContext.GetSpellSAContext()->RunScripts
  //                      (DOES_ATTACK_SUCCEED,
  //                       ScriptCallback_LookForChar,
  //                       "YN",
  //                       targCOMBATANT->GetName(),
  //                       itemName);
  //        };
  //      };
  //    };
  //  };
  //  scriptContext.Clear();
  //  //ClearItemContext();
  //  if (!result.IsEmpty())
  //  {
  //    if (result[0] == 'Y') DidHit = 1;
  //    else if (result[0] == 'N') DidHit = 0;
  //  };
  //};

  //if (DidHit < 0) DidHit = (m_iAttDiceRoll >= thac0);
  //GLOBAL_SPELL_ID SpellDbKey; //=-1;



  if (toHitComputation.DidHit()) //if (DidHit)
  { 
    toHitComputation.Successful(1);
#ifdef TraceFreeAttacks
    {
      WriteDebugString("TFA - %s succeeds in hitting %s; numberAttacks=%d; availAttacks=%d\n", 
                   GetName(), targCOMBATANT->GetName(), int(GetNbrAttacks()), int(availAttacks));
    };
#endif

    DamageComputation damageComputation;
    //GetDamageDice(
    //            wpn, 
    //          & dmgDiceQty, 
    //          & dmgDiceSides, 
    //          & dmgDiceBonus, 
    //          & IsNonLethal, 
    //            targCOMBATANT->isLargeDude(),
    //          & SpellDbKey);

    //dmgDiceBonus += m_iTmpDmgDiceBonus; // for backstabbing
    
    //ModifyAttackDamageDice(
    //                targCOMBATANT,   
    //                dmgDiceQty, 
    //                dmgDiceSides, 
    //              & dmgDiceBonus, 
    //              & IsNonLethal);

    //ModifyAttackDamageDiceForItem(
    //                targCOMBATANT,
    //                *(GLOBAL_ITEM_ID *)&wpn,  // Investigate this!!
    //                dmgDiceQty,  
    //                dmgDiceSides,  
    //              & dmgDiceBonus,  
    //              & IsNonLethal);

    //targCOMBATANT->ModifyAttackDamageDiceAsTarget(
    //                this,
    //                dmgDiceQty,
    //                dmgDiceSides,
    //              & dmgDiceBonus,
    //              & IsNonLethal);

    //targCOMBATANT->ModifyAttackDamageDiceForItemAsTarget(
    //                this,
    //                *(GLOBAL_ITEM_ID *)&wpn,  // Investigate this!!
    //                dmgDiceQty,
    //                dmgDiceSides,
    //              & dmgDiceBonus,
    //              & IsNonLethal,
    //              toHitComputation.Rolled()
    //              );

    //dmg = RollDice(dmgDiceSides, dmgDiceQty, dmgDiceBonus);
     
    //if (dmg <= 0) 
    //  dmg = 1;

    //TRACE("%i rolls %i damage using %i sides, %i qty, %i bonus (%s)\n",
    //                 self, dmg, dmgDiceSides, dmgDiceQty, dmgDiceBonus,
    //                 (IsNonLethal?"NonLethal":"Lethal"));

    //InstantSpellActivate(damageComputation.SpellDbKey(), targ);

    
 
    



    SPELL_ID attackSpellID, itemSpellID;
    if (wpn != NO_READY_ITEM)
    {  
      const SPELL_DATA *pSpell;
      itemSpellID = pWeapon->SpellID();
      if (!itemSpellID.IsEmpty())
      {
        pSpell = spellData.PeekSpell(itemSpellID);
        if (itemSpellID.IsValidSpell())
        {
          //Not Implemented(0x551c, false);
        }
        else
        {
          CString msg;
          if (!debugStrings.AlreadyNoted("WNHUSN"))
          {
            msg.Format("A Weapon named:\n%s\nhas an undefined spell named\n%s",
              pWeapon->UniqueName(),
              pWeapon->SpellID());
            MsgBoxInfo(msg, "Warning");
          };
        };
      };
    }
    else 
    {
      if (GetType()==MONSTER_TYPE)
      {
        int nbrAttacks = GetNbrAttacks();
        int currAttack = nbrAttacks - availAttacks;
        //20130829 PRS  ASSERT( (currAttack>=0) && (currAttack<nbrAttacks) );
        if ((currAttack < 0)||(currAttack>=nbrAttacks)) currAttack=0;
        
        //MONSTER_DATA *pData = monsterData.GetMonsterData(origKey);
        const MONSTER_DATA *pMonster = monsterData.PeekMonster(m_pCharacter->monsterID);
        //ASSERT( pData!=NULL );
        ASSERT( pMonster!=NULL );
  
        attackSpellID = pMonster->attackData.PeekMonsterAttackDetails(currAttack)->spellID;
      };  
    };  
    
    
    
    
    //InstantSpellActivate(damageComputation.SpellID(), targ, &toHitComputation);
    InstantSpellActivate(attackSpellID, itemSpellID, targ, &toHitComputation);
    if (    (attackSpellID.IsEmpty() && itemSpellID.IsEmpty())
         || (toHitComputation.BeginSpellScriptFailure() == 0))
    {
      damageComputation.Compute(this,
                                targCOMBATANT, 
                                wpn, 
                                toHitComputation.Rolled(),
                                toHitComputation.IsBackStab(),
                                toHitComputation.BackstabMultiplier());
      {
        //if (!giID.IsNoItem())
        if (!itemID.IsNoItem())
        {
          //pWeapon = itemData.GetItemData(giID);
          // done above....pWeapon = itemData.GetItem(itemID);
          SPELL_ID noSpell;
          if (pWeapon->IsUsable() && (pWeapon->Wpn_Type == SpellCaster))
          {
            //InstantSpellActivate(pWeapon->m_gsID, targ);
            InstantSpellActivate(pWeapon->spellID, noSpell, targ, &toHitComputation);
          };
          if (pWeapon->IsUsable() && (pWeapon->Wpn_Type == SpellLikeAbility))
          {
            //InstantSpellActivate(pWeapon->m_gsID, targ);
            //Not Implemented(0x553, false);
            InstantSpellActivate(pWeapon->spellID, noSpell, targ, &toHitComputation);
          };
        };
      };

      targCOMBATANT->UpdateSpellForAttacks(1);

      PlayHit();

  //    if (dmg > 0)
  //    {        
  //      if (toHitComputation.IsBackStab())
  //      {
  //        dmg *= GetThiefBackstabDamageMultiplier();
  //      };

  //      {
  //        CString result;
  //        ITEM_DATA *pWeapon=NULL;
  //        scriptContext.SetItemContext(giID);
  //        if (!giID.IsNoItem())
  //        {
  //          pWeapon = itemData.GetItemData(giID);
  //        };
  //        PrepareHooks();
  //        hookParameters[5].Format("%d",dmg);
  //        hookParameters[6].Format("%d",IsNonLethal);
  //        scriptContext.SetAttackerContext(this);
  //        scriptContext.SetTargetContext(targCOMBATANT);
  //        scriptContext.SetItemContext(pWeapon);
  //        result = targCOMBATANT->combatantSA.RunScripts(
  //                COMPUTE_DAMAGE,
  //                ScriptCallback_RunAllScripts,
  //                NULL,
  //                targCOMBATANT->GetName(),
  //                scriptContext.GetItemContext()->Name);
  //        if (!result.IsEmpty())
  //        {
  //          int temp;
  //          if (sscanf(result,"%d",&temp) == 1) dmg = temp;
  //        };
  //        scriptContext.Clear();
  //      };
      {
        targCOMBATANT->TakeDamage(damageComputation.Damage(),
                                  damageComputation.IsNonLethal(),
                                  NULL,
                                  targCOMBATANT==this,
                                  pDeathIndex);
       
        if (targCOMBATANT->GetStatus()==Dead)
        {
          // no longer a valid target
          RemoveCurrTarget();
        }
      };
      {
        //if (damageComputation.SpellDbKey().IsValidSpell())
        if (damageComputation.SpellID().IsValidSpell())
        {
          //SPELL_DATA *pSpell = spellData.GetSpellData(damageComputation.SpellDbKey());
          CString samsg;
          SPELL_DATA *pSpell = spellData.GetSpell(damageComputation.SpellID());
          ASSERT(pSpell!=NULL);  
          if (pSpell!=NULL)
          {       
            samsg = pSpell->CastMsg;
            samsg.Replace("/s", pSpell->Name);
            samsg.Replace("/c", GetName());
            if (targCOMBATANT!=NULL)
              samsg.Replace("/t", targCOMBATANT->GetName());
            else
              samsg.Replace("/t", ""); 
          
            CombatMsg = samsg;
          }
        };
        if (CombatMsg.IsEmpty())
        {
          if (damageComputation.Message().IsEmpty())
          {
            if (toHitComputation.IsBackStab())
            {
              CombatMsg.Format("Rolls %i and BackStabs for %i dmg",
                            toHitComputation.Rolled(), damageComputation.Damage());
            }
            else
            {
              CombatMsg.Format("Rolls %i and hits for %i dmg",
                            toHitComputation.Rolled(), damageComputation.Damage());
            }
          }
          else
          {
            CombatMsg.Format("%s", damageComputation.Message());
          };
        }
      };
      // Condition added 20110413 PRS......
      // Manikus said that auto combatants should get multiple attacks even after a successful hit.
      //continueAttack=FALSE;
    
      // Changed again 20110522.
      // Manikus tells me that multiple attacks should ALWAYS be possible
      // if (!OnAuto(false)) continueAttack=FALSE; else continueAttack=TRUE;
    }
    else
    {
      toHitComputation.Successful(0);
    };
  } 
  else
  {
    toHitComputation.Successful(0);
  };
  if (!toHitComputation.Successful())
  {
#ifdef TraceFreeAttacks
    {
      WriteDebugString("TFA - %s failed to hit %s; numberAttacks=%d; availAttacks=%d\n", 
                   GetName(), targCOMBATANT->GetName(), int(GetNbrAttacks()), int(availAttacks));
    };
#endif
    PlayMiss();
    if (toHitComputation.IsBackStab())
    {
      CombatMsg.Format("Rolls %i amd fails BackStab", toHitComputation.Rolled());
    }
    else
    {
      CombatMsg.Format("Rolls %i and misses", toHitComputation.Rolled());
    };
    continueAttack=TRUE;
  }


 

  if (toHitComputation.Successful())
  {
  }
  else
  {
  };
  FormatCombatText(combatTextData, CombatMsg);
  //combatMsgs.msgs.RemoveAll();
  //combatMsgs.Add(CombatMsg);

  if (decQty)  // wpn and giID refer to either the weapon or the ammo consumed by the weapon
  {
    // dec item qty by 1, item won't show up in post-combat treasure list
    if (wpn != NO_READY_ITEM)
    {
      if (wpnConsumesSelfAsAmmo)
      {
        ITEM myItem;
        m_pCharacter->myItems.GetItem(wpn, myItem);
        myItem.qty = 1;
        combatData.hurledWeapons.AddItem(myItem);
      };
      m_pCharacter->myItems.AdjustQty(wpn, -1);
      //if (!m_pCharacter->myItems.HaveItem(giID)) // if deleted because of zero quantity
      if (!m_pCharacter->myItems.HaveItem(itemID)) // if deleted because of zero quantity
      // 20110519 PRS  if (!myItems.HaveItem(*(GLOBAL_ITEM_ID*)&wpn)) // if deleted because of zero quantity
      {
        // item removed, disable special abilities granted by it (if any)
        //ITEM_DATA *pItem = itemData.GetItemData(giID);
        ITEM_DATA *pItem = itemData.GetItem(itemID);
        if (pItem!=NULL) 
        {
          ActorType actor;
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          GetContext(&actor);
          SetCharContext(&actor);
          scriptContext.SetCharacterContext(this->m_pCharacter);
          m_pCharacter->RunCharacterScripts(
                  ON_USE_LAST_WEAPON, 
                  ScriptCallback_RunAllScripts, 
                  NULL,
                  "Decrement weapon or ammo count");
          ClearCharContext();
          //pItem->specAbs.DisableAllFor(this);
        };
      };

      // re-calculate movement/encumbrance
      SetEncumbrance(determineEffectiveEncumbrance());
      determineMaxMovement();    
    }
  }


  // make sure combatants are facing towards each other
  FaceOpponent(targ);
  if (!toHitComputation.IsBackStab()) targCOMBATANT->FaceOpponent(self);   


  targCOMBATANT->m_iLastAttacker = self;
  targCOMBATANT->m_eLastAction = LA_Defend;
  m_iLastAttacked=targCOMBATANT->self;
  m_eLastAction = LA_Attack;


  //m_iTmpAttDiceBonus = 0;
  //m_iTmpDmgDiceBonus = 0;

  ClearTargetContext();
  ClearCharContext();
  return -1;
}

//*****************************************************************************
// NAME:    COMBATANT::AttackOver
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::AttackOver()
{
  QueuedCombatantData &qcomb = GetQueuedCombatants();
  if (qcomb.NumFreeAttacks())
  {
#ifdef TraceFreeAttacks
    WriteDebugString("TFA - COMBATANT::AttackOver;  availAttacks=%d;  numFreeAttacks=%d\n",
               (int)availAttacks, qcomb.NumFreeAttacks());
#endif
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CString result;
    scriptContext.SetCombatantContext(this);
    hookParameters[5].Format("%d", int(availAttacks));
    hookParameters[6].Format("%d", qcomb.NumFreeAttacks());
    hookParameters[7] = "-1";
    hookParameters[8] = "-1";
    result = RunCombatantScripts(
                    FREEATTACK_ATTACKOVER,
                    ScriptCallback_RunAllScripts,
                    NULL,
                    "Free Attack Complete");
    scriptContext.Clear();
    {
      int n;
      if (sscanf(hookParameters[7],"%d", &n) == 1)
      {
        availAttacks += n;
      };
      if (sscanf(hookParameters[8],"%d", &n) == 1)
      {
        qcomb.NumFreeAttacks(qcomb.NumFreeAttacks() + n);
      };
    };
    if(qcomb.NumFreeAttacks() <= 0)
    {
      qcomb.NumFreeAttacks(0);
      continueAttack = false;
    };
    if (availAttacks <= 0)
    {
      m_iInitiative = INITIATIVE_Never;  // After a free attack that exhausts
                                         // available attacks, the combatant is
                                         // finished for this round.
    };
#ifdef TraceFreeAttacks
    WriteDebugString("TFA - COMBATANT::AttackOver;  availAttacks=%d;  numFreeAttacks=%d\n",
               (int)availAttacks, qcomb.NumFreeAttacks());
#endif
  }
  else 
  {
    if (qcomb.NumGuardAttacks())
    {
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      CString result;
      scriptContext.SetCombatantContext(this);
      hookParameters[5].Format("%d", int(availAttacks));
      hookParameters[6].Format("%d", qcomb.NumFreeAttacks());
      hookParameters[7] = "-1";
      hookParameters[8] = "-1";
      result = RunCombatantScripts(
                      GUARDING_ATTACKOVER,
                      ScriptCallback_RunAllScripts,
                      NULL,
                      "Guarding Attack Completed");
      scriptContext.Clear();
      {
        int n;
        if (sscanf(hookParameters[7],"%d", &n) == 1)
        {
          availAttacks += n;
        };
        if (sscanf(hookParameters[8],"%d", &n) == 1)
        {
          qcomb.NumGuardAttacks(qcomb.NumGuardAttacks() + n);
        };
      };
      if(qcomb.NumGuardAttacks()<=0)
      {
        if (qcomb.NumGuardAttacks() < 0) qcomb.NumGuardAttacks(0);
        continueAttack = false;
      };
    }
    else
    {
      availAttacks -= 1.0;   // Default Behaviour for normal attacks.
      if (availAttacks <= 0) continueAttack = false;
    }
  };
  if ( availAttacks <= 0)
  {
    availAttacks = 0;
  };

  if (!continueAttack)
  {
    int dude;
    qcomb.NumFreeAttacks(0);
    qcomb.NumGuardAttacks(0);
    EndTurn();
    dude = qcomb.Top();
    if (dude != NO_DUDE)
    {
      if (qcomb.DelayedX() >= 0)
      {
        COMBATANT *pDelayedCombatant;
        if (terrain[qcomb.DelayedY()][qcomb.DelayedX()].tileIndex != NO_DUDE)
        {
          /* Really */ NotImplemented(0x34c1, false);
        };
        pDelayedCombatant = combatData.GetCombatant(dude);
        placeCombatant(pDelayedCombatant->x, 
                       pDelayedCombatant->y,
                       NO_DUDE, 
                       pDelayedCombatant->width, 
                       pDelayedCombatant->height);
        pDelayedCombatant->x = qcomb.DelayedX();
        pDelayedCombatant->y = qcomb.DelayedY();
        qcomb.SetXY(-1,-1);
        placeCombatant(pDelayedCombatant->x, 
                       pDelayedCombatant->y,
                       dude, 
                       pDelayedCombatant->width, 
                       pDelayedCombatant->height);
      };
    };
    return;
  };
  float temp = availAttacks;

  ClearFormattedText(combatTextData);
  TRACE("Attack over for %u, attacks remaining %f\n",
                   self, temp);


  //if (combatData.GetCurrCombatant() == self)
  //{
  //  if (combatData.IsFreeAttacker())
  //  {
  //    continueAttack=FALSE;
  //    //availAttacks = 0;
  //   EndTurn();
  //    return;
  //  };
  //};


  PlaceCursorOnCurrentDude();
  // if manual control, and first attack hits, then stop
  // attacks if multiple allowed
  if (    (!continueAttack && !OnAuto(false))
     )
  {
    StopAttack();
    return;
  }
}

//*****************************************************************************
// NAME:    COMBATANT::AttackOver
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::StopAttack()
{
  // stop casting spells if in progress
  StopCasting(FALSE, false);
  State(ICS_None);
}

//*****************************************************************************
// NAME:    COMBATANT::HandleTimeDelayMsgExpired
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::HandleTimeDelayMsgExpired(int iDeathIndex)
{  
  switch (State())
  {
  case ICS_Attacking:
    TRACE("Handle TimeDelayMsgExpired: AttackOver\n");
    AttackOver();
    break;
  case ICS_Bandaging:
    TRACE("Handle TimeDelayMsgExpired: Bandage Over\n");    
    break;
  case ICS_Guarding:
    TRACE("Handle TimeDelayMsgExpired: Guarding continues\n");
    EndTurn(ICS_Guarding);
    break;
  case ICS_Turning:
    TRACE("Handle TimeDelayMsgExpired: Turn Undead Over\n");
    // message preceeds action in this case
    //EndTurn();
    break;
  }
}

//*****************************************************************************
//    NAME: COMBATANT::HandleCombatRoundMsgExpired
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::HandleCombatRoundMsgExpired()
{
  TRACE("Handle HandleCombatRoundMsgExpired for %i - state %s\n", 
         self,CombatantStateText[State()]);
  PlaceCursorOnCurrentDude();  
  // do nothing yet
}

//*****************************************************************************
// NAME:    COMBATANT::HandleTimeDelayMsgBegin
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
int COMBATANT::HandleTimeDelayMsgBegin(int extraAttacksAvailable, int *pDeathIndex)
{
  switch (State())
  {
#ifdef D20140707
  case ICS_Guarding:
    if (extraAttacksAvailable > 0)
    {
      return makeAttack(GetCurrTarget(true, false), 
                        extraAttacksAvailable,
                        pDeathIndex);
    };
    break;
#endif
  case ICS_Attacking:
    return makeAttack(GetCurrTarget(true, false, true), 
                      extraAttacksAvailable,
                      pDeathIndex);

  default:
    //PlaceCursorOnCurrentDude();
    break;
  }
  return -1;
}

//*****************************************************************************
//    NAME: COMBATANT::UseAttackPose
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::UseAttackPose()
{
  if (State()==ICS_Attacking) return TRUE;
  if (State()==ICS_Using) return TRUE;
  if (State()==ICS_Casting) return forceAttackPose;
  return FALSE;
}

//*****************************************************************************
// NAME:    displayCombatSprite
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::displayCombatSprite()
{  
//   charStatusType cstat = GetAdjStatus();
   if (charUnconscious())
   {
      // blit dying combat tile
      blitDyingSprite();
   }
   else if (charPetrified())
   {
     blitPetrifiedSprite();
   }
   else
   {
     if (!charOnCombatMap(false, true)) return;

     if (!GraphicsMgr.ValidSprite(m_pCharacter->icon.key))
     {
        LoadCombatIcon();
        if (!GraphicsMgr.ValidSprite(m_pCharacter->icon.key))
        {
          if (!debugStrings.AlreadyNoted(CString("NISTUID")))
          {
            writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
            WriteDebugString("No icon surface to use in displayCombatSprite\n");
          };
          return;
        }
     }
   
     int startTerX, startTerY;
     GetStartTerrain(&startTerX,  &startTerY);
     if (   (x < startTerX + TILES_HORZ)
         && (y < startTerY + TILES_VERT))
     {
        int tx = x;
        int ty = y;

        // Check for partial icon display
        //
        // For icons larger than 1x1, the starting x,y may
        // be off of the left,top but still the partially visible
        // part of the icon should be displayed.
        //
        // This isn't a problem for the right,bottom since
        // if the x,y isn't within those bounds then none
        // of the icon will be either.
        if (tx < startTerX)
        {
          int ex = (tx+width)-1;
          // none of the icon is visible
          if (ex < startTerX)
            return;
          BOOL visible = FALSE;
          while ((tx <= ex) && (!visible))
          {
            if (tx >= startTerX)
              visible = TRUE;
            else
              tx++;
          }
          if (!visible)
            return;
        }
           
        if (ty < startTerY)
        {
          int ey = (ty+height)-1;
          // none of the icon is visible
          if (ey < startTerY)
            return;
          BOOL visible = FALSE;
          while ((ty <= ey) && (!visible))
          {
            if (ty >= startTerY)
              visible = TRUE;
            else
              ty++;
          }
          if (!visible)
            return;
        }

        RECT srcRect;  // clipping rect for source bitmap
        RECT dstRect;

        // width and height in pixels of a single tile
        int iw = width * COMBAT_TILE_WIDTH;
        int ih = height * COMBAT_TILE_HEIGHT;

        // offset in pixels to icon pair indicated by iconIndex
        int offset = (m_pCharacter->iconIndex-1)*(iw*2);
        if (offset < 0) offset=0;
                  
        if (IsCurrentCombatant(self))
        {
          if (UseAttackPose())
          {
            // display attack pose
            srcRect.left = offset+iw;
          }
          else
          {
            // display ready pose
            srcRect.left = offset;
          }
        }
        else
          srcRect.left = offset;
    
        srcRect.right  = srcRect.left + iw;
        srcRect.top    = 0;
        srcRect.bottom = ih;
        
        BOOL flip = (m_iFacing == FACE_WEST);

        // take into account any clipping done against
        // top, left for partially visible icons
        if (tx > x)
        {
          if (flip)
            srcRect.right -= ((tx-x)*COMBAT_TILE_WIDTH);
          else
            srcRect.left += ((tx-x)*COMBAT_TILE_WIDTH);
        }
        if (ty > y)
          srcRect.top += ((ty-y)*COMBAT_TILE_HEIGHT);

        dstRect.left=TerrainToScreenCoordX(tx);
        dstRect.top=TerrainToScreenCoordY(ty);
        dstRect.right  = dstRect.left+(srcRect.right-srcRect.left);
        dstRect.bottom = dstRect.top+(srcRect.bottom-srcRect.top);

        /*
        if (dude.GetType() == CHAR_TYPE)
          TRACE("Blitting pc icon\n");
        else if (dude.GetType() == NPC_TYPE)
          TRACE("Blitting npc icon\n");
        else if (dude.GetType() == monster)
          TRACE("Blitting monster icon\n");
        */

        GraphicsMgr.SetClipRect(&GetCombatRect());  

        // blit the source tile from the art bitmap to the back buffer
        GraphicsMgr.BlitSpriteImage(dstRect.left, dstRect.top,
                              m_pCharacter->icon.key,
                              &srcRect, 
                              flip,
                              BACKBUFFERNUM,
                              &dstRect); 

        GraphicsMgr.SetClipRect(&GetFullRect());
     }
   }
}

//*****************************************************************************
// NAME:    blitDyingSprite
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::blitDyingSprite()
{
  if (!blitDeathTile)
    blitDeadSprite();
  
  if (!IsPartyMember()) return;    
  //UpdateCombatTimeDelayMsg();

  if (!ValidTerrainViewportCoords(x, y)) 
  {
    return;
  }
  
  //EnsureVisible(GetCenterX(),GetCenterY());

  int sx = TerrainToScreenCoordX(x);
  int sy = TerrainToScreenCoordY(y);
 
  GraphicsMgr.SetClipRect(&GetCombatRect());  
  GraphicsMgr.BlitImage(sx,sy,CombatDeathIconArt.surface,&DeadCharSrcA);
  GraphicsMgr.SetClipRect(&GetFullRect());
}

//*****************************************************************************
// NAME:    blitPetrifiedSprite
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::blitPetrifiedSprite()
{
  if (!blitDeathTile)
    blitDeadSprite();
  
  if (!IsPartyMember()) return;    
  //UpdateCombatTimeDelayMsg();

  if (!ValidTerrainViewportCoords(x, y)) 
  {
    return;
  }
  
  //EnsureVisible(GetCenterX(),GetCenterY());

  int sx = TerrainToScreenCoordX(x);
  int sy = TerrainToScreenCoordY(y);
 
  GraphicsMgr.SetClipRect(&GetCombatRect());  
  GraphicsMgr.BlitImage(sx,sy,CombatPetrifiedIconArt.surface,&PetrifiedCharSrcA);
  GraphicsMgr.SetClipRect(&GetFullRect());
}


void DisplayCombatData(BOOL ShowCursor=TRUE, BOOL FullIcon=TRUE);
void DisplayCombatDataWithSprite(BOOL ShowCursor=TRUE, BOOL FullIcon=TRUE);


//*****************************************************************************
// NAME:    blitDeadSprite
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::blitDeadSprite()
{
  int dude;
  if (blitDeathTile) return; // already done
  blitDeathTile = TRUE;

  if (getCombatantInCell(x, y, 1, 1, NO_DUDE) == self)
  {
    placeCombatant(x, y, NO_DUDE, width, height);
  };
  placeDyingCombatant(x,y,self,width,height);
   
  //UpdateCombatTimeDelayMsg();
  DisplayCombatDataWithSprite();
  menu.DisplayMenu(0);
  dude = combatData.GetCurrCombatant();
  DisplayCombatData();
  displayCombatStats(dude, 0);
  DisplayTextAndFlip(0);

  
  PlayCombatDeath();

   EnsureVisible(GetCenterX(),GetCenterY());
   GraphicsMgr.SetClipRect(&GetCombatRect());

   int i,j;
   for (i=0;i<height;i++)
   {
     for (j=0;j<width;j++)
     {
  
    
       //20140421 PRS added this check (moved from above!)
       if (!ValidTerrainViewportCoords(x+j, y+i)) 
       {
         continue;
       };

       GraphicsMgr.BlitImage(TerrainToScreenCoordX(x+j),
                             TerrainToScreenCoordY(y+i),
                             CombatDeathArt.surface,
                             &DeadCharSrcA,
                             FALSE,FRONTBUFFERNUM);
     }
   }      

   theApp.AppSleep(200);

   for (i=0;i<height;i++)
   {
     for (j=0;j<width;j++)
     {
       //20140421 PRS added this check (moved from above!)
       if (!ValidTerrainViewportCoords(x+j, y+i)) 
       {
         continue;
       };
       GraphicsMgr.BlitImage(TerrainToScreenCoordX(x+j),
                             TerrainToScreenCoordY(y+i),
                             CombatDeathArt.surface,
                             &DeadCharSrcB,
                             FALSE,FRONTBUFFERNUM);
     }
   } 
   
   theApp.AppSleep(200);

   for (i=0;i<height;i++)
   {
     for (j=0;j<width;j++)
     {
       //20140421 PRS added this check (moved from above!)
       if (!ValidTerrainViewportCoords(x+j, y+i)) 
       {
         continue;
       };
       GraphicsMgr.BlitImage(TerrainToScreenCoordX(x+j),
                             TerrainToScreenCoordY(y+i),
                             CombatDeathArt.surface,
                             &DeadCharSrcA,
                             FALSE,FRONTBUFFERNUM);
     }
   }      

   theApp.AppSleep(200);

   for (i=0;i<height;i++)
   {
     for (j=0;j<width;j++)
     {
       //20140421 PRS added this check (moved from above!)
       if (!ValidTerrainViewportCoords(x+j, y+i)) 
       {
         continue;
       };
       GraphicsMgr.BlitImage(TerrainToScreenCoordX(x+j),
                             TerrainToScreenCoordY(y+i),
                             CombatDeathArt.surface,
                             &DeadCharSrcB,
                             FALSE,FRONTBUFFERNUM);
     }
   }      

   theApp.AppSleep(200);

   for (i=0;i<height;i++)
   {
     for (j=0;j<width;j++)
     {
       //20140421 PRS added this check (moved from above!)
       if (!ValidTerrainViewportCoords(x+j, y+i)) 
       {
         continue;
       };
       GraphicsMgr.BlitImage(TerrainToScreenCoordX(x+j),
                             TerrainToScreenCoordY(y+i),
                             CombatDeathArt.surface,
                             &DeadCharSrcA,
                             FALSE,FRONTBUFFERNUM);
     }
   }      

   GraphicsMgr.SetClipRect(&GetFullRect());
   theApp.AppSleep(500);
   DisplayCombatData();
}

//*****************************************************************************
// NAME: COMBATANT::EndTurn
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::EndTurn(individualCombatantState newState)
{   
  State(newState);
  QueuedCombatantData &qcomb = GetQueuedCombatants();

  if (qcomb.Top() == self)
  {
    // PRS 20110219  if (qcomb.ChangeStats())
    if (qcomb.ChangeStats() || qcomb.NumFreeAttacks() || qcomb.NumGuardAttacks())
    {
      TRACE("EndTurn(%s), %i done\n", CombatantStateText[State()], self);
      turnIsDone = TRUE;
    }
    else
      TRACE("EndTurn(%s), %i (no change to done)\n", CombatantStateText[State()], self);

    // input was disabled if auto combatant
    EnableUserInput(TRUE);
    //  PRS  20110219                         //*
    if (qcomb.NumFreeAttacks() || qcomb.NumGuardAttacks())             //*
    {                                         //*
      //combatData.m_iPrevRndCombatant = self;//*
      combatData.m_forceRoundDelay = TRUE;    //*
    };                                        //*
    qcomb.Pop();
    if (qcomb.Top() != NO_DUDE)
    {
      if (qcomb.DelayedX() >= 0)
      {
        int delayedDude;
        COMBATANT *pDelayedCombatant;
        if (terrain[qcomb.DelayedY()][qcomb.DelayedX()].tileIndex != NO_DUDE)
        {
          /* Really */ NotImplemented(0x34c1, false);
        };
        delayedDude = qcomb.Top();
        pDelayedCombatant = combatData.GetCombatant(delayedDude);
        placeCombatant(pDelayedCombatant->x, 
                       pDelayedCombatant->y,
                       NO_DUDE, 
                       pDelayedCombatant->width, 
                       pDelayedCombatant->height);
        pDelayedCombatant->x = qcomb.DelayedX();
        pDelayedCombatant->y = qcomb.DelayedY();
        qcomb.SetXY(-1,-1);
        placeCombatant(pDelayedCombatant->x, 
                       pDelayedCombatant->y,
                       delayedDude, 
                       pDelayedCombatant->width, 
                       pDelayedCombatant->height);
      };
    };
  }
  else
  {
    qcomb.Remove(self);
    if (qcomb.DelayedX() >= 0)
    {
      /* Really */NotImplemented(0x43c6, false);
    };
    TRACE("Forced EndTurn(%s), %i done\n", CombatantStateText[State()], self);
    turnIsDone = TRUE;
  }
  forceAttackPose=FALSE;
  OnEndTurn();
  //combatMsgs.msgs.RemoveAll(); // not set up for scrolling msgs yet
}


//*****************************************************************************
// NAME: COMBATANT::IsDone
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::IsDone(bool freeAttack, LPCSTR comment)
{ 
  //QueuedCombatantData &qcomb = GetQueuedCombatants();
  //if (   (qcomb.Top() == self)
  //    && (!qcomb.ChangeStats()))
  //{
  //  return FALSE; // queued up for free attack
  //}

  switch(GetStatus())
  {
  case Petrified:
    return TRUE;
  default: 
    break;
  };


  if (m_isCombatReady < 0)
  {
    ActorType actor;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    if (m_isCombatReady == -1) hookParameters[5] = "Round";
    if (m_isCombatReady == -2) hookParameters[5] = "Action";
    m_isCombatReady = 1;
    GetContext(&actor);
    SetCharContext(&actor);
    scriptContext.SetCharacterContext(this->m_pCharacter);
    scriptContext.SetCombatantContext(this);
    if (!m_pCharacter->RunCharacterScripts(
                       IS_COMBAT_READY, 
                       ScriptCallback_LookForChar, 
                       "N", 
                       comment).IsEmpty())
    {
      m_isCombatReady = 0;
    }
    else
    {
      if (!RunCombatantScripts(
                IS_COMBAT_READY, 
                ScriptCallback_LookForChar, 
                "N", 
                comment).IsEmpty())
      {
        m_isCombatReady = 0;
      }
    };
    ClearCharContext();
    /*
    if (GetAdjSpecAb(SA_Sleep))
    {
      m_incapacitated = 1;
    }
  
    if (GetAdjSpecAb(SA_HoldPerson))
    {
      m_incapacitated = 1;
    }
    */
  };

  if (m_isCombatReady == 0) return TRUE;

  if (!charOnCombatMap(false, false))
    turnIsDone = TRUE;
  if (freeAttack)
  {
    if (m_target == NO_DUDE)
    {
      turnIsDone = TRUE;
    };
  };

  return turnIsDone; 
}

//*****************************************************************************
// NAME: COMBATANT::Quick
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::Quick(BOOL Enable)
{
  // can't change quick status if not allowed to control
  if (GetAdjAllowPlayerControl())
  {
    // if turning off auto, stop doing whatever
    // the AI had you doing.
    if (!Enable)
    {
      ClearPath();
      combattargets.RemoveAll();
      State(ICS_None);
      StopCasting(FALSE, false);
    }
    SetAutomatic(Enable);
    if (!IsPartyMember())
      EnableUserInput(!GetAdjAutomatic());
    else
      EnableUserInput(TRUE);
  }
}

//*****************************************************************************
// NAME: COMBATANT::TurnUndead
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::TurnUndead()
{
  // end any spell casting in progress
  StopCasting(FALSE, false);
  hasTurnedUndead=TRUE;
  State(ICS_Turning);
}

//*****************************************************************************
// NAME: COMBATANT::CanBandage
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanBandage()
{
  return (!IsDone(false, "Can Combatant Bandage"));
}

//*****************************************************************************
// NAME: COMBATANT::SpecialActionName
//
// PURPOSE:  
//*****************************************************************************
CString COMBATANT::SpecialActionName()
{
  return IsDone(false, "Retrieve special action name") ? "" : m_specialActionName;
}

//*****************************************************************************
// NAME: COMBATANT::SpecialActionIndex
//
// PURPOSE:  
//*****************************************************************************
int COMBATANT::SpecialActionIndex()
{
  return IsDone(false, "Retrieve special action index") ? -1 : m_specialActionIndex;
}

//*****************************************************************************
// NAME:    COMBATANT::Bandage
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::Bandage()
{ 
  // end any spell casting in progress
  StopCasting(FALSE, false);
  State(ICS_Bandaging);
}

//*****************************************************************************
// NAME:    COMBATANT::CanGuard
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBATANT::CanGuard(GUARDING_CASE guardCase)
{
  // See email at CheckOpponentFreeAttack()



  if (IsDone(false, "Can combatant guard")) return FALSE;
/*
  // can guard only when melee weapon is readied, if is a
  // party member. Monsters can guard always
  if (!IsPartyMember())


    // PRS 20111018  See email posted at CheckOpponentFreeAttack
    return FALSE; //TRUE;

  int mywpnidx = myItems.GetReady(WeaponHand);
  if (mywpnidx != NO_READY_ITEM)
  {  
    // using a weapon rather than natural attack types (ie claws/jaws/fists)
     if (myItems.GetQty(mywpnidx) <= 0)
        return FALSE;    
 
     return (WpnCanAttackAtRange(myItems.GetItem(mywpnidx), 1));
  }
*/
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CString result;

      hookParameters[5].Format("%d", guardCase);

      scriptContext.SetCombatantContext(this);
      result = RunCombatantScripts(
                      GUARDING_CAN_GUARD,
                      ScriptCallback_RunAllScripts,
                      NULL,
                      "Test if combatant can guard");
      scriptContext.Clear();
      if (!result.IsEmpty())
      {
        if (result[0] == 'Y') return TRUE;
      };
  }


  return FALSE;
}

//*****************************************************************************
// NAME: COMBATANT::Guard
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::Guard()
{
  // end any spell casting in progress
  StopCasting(FALSE, false);
  State(ICS_Guarding);
}

//*****************************************************************************
// NAME: COMBATANT::CanMove
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanMove(BOOL allowZeroMove)
{
  if (GetType() == MONSTER_TYPE)
  {
    if (GetConfigMonsterNoMove())
      return FALSE;
  }

  if (IsDone(false, "Can combatant move")) 
    return FALSE;

  if (allowZeroMove) 
  {
    return true;
  };
  if (m_iMovement >= GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, "Can combatant move"))
  {
    return FALSE;
  };
  return TRUE;
}

//*****************************************************************************
// NAME:    COMBATANT::FindPathTo
//
// PURPOSE: 
//
// RETURNS: FALSE if already there
//*****************************************************************************
BOOL COMBATANT::FindPathTo(int destLeft,  int destTop, 
                           int destRight, int destBottom, 
                           BOOL CheckOccupants, 
                           BOOL allowZeroMove,
                           BOOL moveOriginPoint)
{  
  if (!OnAuto(false))
  {
    if ((abs(destLeft-x) > 1) || (abs(destTop-y) > 1))
      TRACE("Moving more than 1 square\n");
  }

  //WriteDebugString("FindPathTo(%u,%u)\n", dx, dy);
  ClearPath();

  if (!CanMove(allowZeroMove)) return FALSE;

  pathMgr.SetPathSize((BYTE)width, (BYTE)height);
  hPath = pathMgr.GetPath(x, y, destLeft, destTop, destRight, destBottom, CheckOccupants, this, moveOriginPoint);
  return (hPath >= 0);
}

//*****************************************************************************
//    NAME: COMBATANT::FindPathAwayFrom
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::FindPathAwayFrom(int fleeFrom)
{  
  ClearPath();
  if (!CanMove(FALSE)) return FALSE;
  if (fleeFrom < 0) return FALSE;

//  /*
  pathMgr.SetPathSize((BYTE)width, (BYTE)height);

  // locate path to position in room that is farthest
  // from indicated fleeFrom character

  //int fx = combatData.combatants[fleeFrom].x;
  //int fy = combatData.combatants[fleeFrom].y;
  int fx = GetCombatantX(fleeFrom);
  int fy = GetCombatantY(fleeFrom);

  // get direction for moving away from 'fleeFrom'
  PATH_DIR dir = GetDir(x, y, fx, fy);
  switch (dir)
  {
  case PathNorth: dir = PathSouth; break;
  case PathNE: dir = PathSW; break;
  case PathEast: dir = PathWest; break;
  case PathSE: dir = PathNW; break;
  case PathSouth: dir = PathNorth; break;
  case PathSW: dir = PathNE; break;
  case PathWest: dir = PathEast; break;
  case PathNW: dir = PathSE; break;
  default: dir = PathNorth; break;
  }

  PATH_DIR pleft = PathDirLeft(dir);
  PATH_DIR pleft2 = PathDirLeft(pleft);
  PATH_DIR pleft3 = PathDirLeft(pleft2);
  PATH_DIR pright = PathDirRight(dir);
  PATH_DIR pright2 = PathDirRight(pright);
  PATH_DIR pright3 = PathDirRight(pright2);

  // max movement distance may not allow straight-line pathing
  // but go with it for now.
  int maxmove = GetMaxMovement();
  if (FindPathAlongLine(dir, maxmove))
    return TRUE;
  if (FindPathAlongLine(pleft, maxmove))
    return TRUE;
  if (FindPathAlongLine(pright, maxmove))
    return TRUE;
  if (FindPathAlongLine(pleft2, maxmove))
    return TRUE;
  if (FindPathAlongLine(pright2, maxmove))
    return TRUE;
  if (FindPathAlongLine(pleft3, maxmove))
    return TRUE;
  if (FindPathAlongLine(pright3, maxmove))
    return TRUE;
//  */
  // no corner in room to flee to, just flee the map if possible
  if (FindPathToMapEdge())
    return TRUE;

  return FALSE;
}

//*****************************************************************************
//    NAME: COMBATANT::FindPathAlongLine
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::FindPathAlongLine(PATH_DIR dir, int dist)
{
  BOOL found = FALSE;
  int maxdist = dist;
  int halfdist = maxdist/2;
  int dx=0;
  int dy=0;

  switch (dir)
  {
    case PathNorth: dx=x;dy=y-maxdist; break;
    case PathNE: dx=x+halfdist;dy=y-halfdist; break;
    case PathEast: dx=x+maxdist;dy=y; break;
    case PathSE: dx=x+halfdist;dy=y+halfdist; break;
    case PathSouth: dx=x;dy=y+maxdist; break;
    case PathSW: dx=x-halfdist;dy=y+halfdist; break;
    case PathWest: dx=x-maxdist;dy=y; break;
    case PathNW: dx=x-halfdist;dy=y-halfdist; break;
    default: return FALSE;
  }

  if (dx < 0) dx=0;
  if (dx >= MAX_TERRAIN_WIDTH) dx = MAX_TERRAIN_WIDTH-1;
  if (dy < 0) dy=0;
  if (dy >= MAX_TERRAIN_HEIGHT) dy = MAX_TERRAIN_HEIGHT-1;

  if (HaveLineOfSight(GetCenterX(),GetCenterY(),dx,dy, NULL))
  {

    // 20160612 PRS
    //hPath = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this);
    hPath = pathMgr.GetPath(x, y, dx, dy, dx, dy, TRUE, this, FALSE);


    found = (hPath>=0);
  }

  return found;
}

//*****************************************************************************
//    NAME: COMBATANT::FindPathToMapNorthEdge
//
// PURPOSE: 
//
//*****************************************************************************
int COMBATANT::FindPathToMapNorthEdge(void)
{
  int path=-1;
  BOOL found = FALSE;

  // find a path to one of the four map sides
  int dx=0;
  int dy=0;
  pathMgr.SetPathSize((BYTE)width, (BYTE)height);

  // north side, find empty, non-blocked, 
  // square along north map edge.
  // start with center and move towards edges
  int count = 0;
  int max = MAX_TERRAIN_WIDTH/2;
  int center = MAX_TERRAIN_WIDTH/2;

  dy=0;
  while ((!found) && (count<=max))
  {
    if (OBSTICAL_none == ObsticalType(center+count, dy, 1, 1, TRUE, TRUE, this))
    {
      dx=center+count;

      // 20160612 PRS
      //path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this);
      path = pathMgr.GetPath(x, y, dx, dy, dx, dy, TRUE, this, FALSE);


      found = (path >= 0);      
    }
    else if (OBSTICAL_none == ObsticalType(center-count, dy, 1, 1, TRUE, TRUE, this))
    {
      dx=center-count;

      // 20160612 PRS
      //path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this);
      path = pathMgr.GetPath(x, y, dx, dy, dx, dy, TRUE, this, FALSE);


      found = (path >= 0);      
    }
    count++;
  }
  return path;
}

//*****************************************************************************
//    NAME: COMBATANT::FindPathToMapEastEdge
//
// PURPOSE: 
//
//*****************************************************************************
int COMBATANT::FindPathToMapEastEdge(void)
{
  int path=-1;
  BOOL found = FALSE;

  // find a path to one of the four map sides
  int dx=0;
  int dy=0;
  pathMgr.SetPathSize((BYTE)width, (BYTE)height);

  // north side, find empty, non-blocked, 
  // square along north map edge.
  // start with center and move towards edges
  int count = 0;
  int max = MAX_TERRAIN_HEIGHT/2;
  int center = MAX_TERRAIN_HEIGHT/2;

  dx=MAX_TERRAIN_WIDTH-1;
  while ((!found) && (count<=max))
  {
    if (OBSTICAL_none == ObsticalType(dx, center+count, 1, 1, TRUE, TRUE, this))
    {
      dy=center+count;
      path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this, FALSE);
      found = (path >= 0);      
    }
    else if (OBSTICAL_none == ObsticalType(dx, center-count, 1, 1, TRUE, TRUE, this))
    {
      dy=center-count;

      // 20160612 PRS
      //path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this);
      path = pathMgr.GetPath(x, y, dx, dy, dx, dy, TRUE, this, FALSE);


      found = (path >= 0);      
    }
    count++;
  }
  return path;
}

//*****************************************************************************
//    NAME: COMBATANT::FindPathToMapSouthEdge
//
// PURPOSE: 
//
//*****************************************************************************
int COMBATANT::FindPathToMapSouthEdge(void)
{
  int path=-1;
  BOOL found = FALSE;

  // find a path to one of the four map sides
  int dx=0;
  int dy=0;
  pathMgr.SetPathSize((BYTE)width, (BYTE)height);

  // north side, find empty, non-blocked, 
  // square along north map edge.
  // start with center and move towards edges
  int count = 0;
  int max = MAX_TERRAIN_WIDTH/2;
  int center = MAX_TERRAIN_WIDTH/2;

  dy=MAX_TERRAIN_HEIGHT-1;
  while ((!found) && (count<=max))
  {
    if (OBSTICAL_none == ObsticalType(center+count, dy, 1, 1, TRUE, TRUE, this))
    {
      dx=center+count;
      path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this, FALSE);
      found = (path >= 0);      
    }
    else if (OBSTICAL_none == ObsticalType(center-count,dy, 1, 1, TRUE, TRUE, this))
    {
      dx=center-count;

      // 20160612
      //path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this);
      path = pathMgr.GetPath(x, y, dx, dy, dx, dy, TRUE, this, FALSE);


      found = (path >= 0);      
    }
    count++;
  }
  return path;
}

//*****************************************************************************
//    NAME: COMBATANT::FindPathToMapWestEdge
//
// PURPOSE: 
//
//*****************************************************************************
int COMBATANT::FindPathToMapWestEdge(void)
{
  int path=-1;
  BOOL found = FALSE;

  // find a path to one of the four map sides
  int dx=0;
  int dy=0;
  pathMgr.SetPathSize((BYTE)width, (BYTE)height);

  // north side, find empty, non-blocked, 
  // square along north map edge.
  // start with center and move towards edges
  int count = 0;
  int max = MAX_TERRAIN_HEIGHT/2;
  int center = MAX_TERRAIN_HEIGHT/2;

  dx=0;
  while ((!found) && (count<=max))
  {
    if (OBSTICAL_none == ObsticalType(dx, center+count, 1, 1, TRUE, TRUE, this))
    {
      dy=center+count;
      path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this, FALSE);
      found = (path >= 0);      
    }
    else if (OBSTICAL_none == ObsticalType(dx,center-count,1, 1, TRUE, TRUE, this))
    {
      dy=center-count;

      // 20160612 PRS
      //path = pathMgr.GetPath(x, y, dx, dy, 1, 1, TRUE, this);
      path = pathMgr.GetPath(x, y, dx, dy, dx, dy, TRUE, this, FALSE);
      found = (path >= 0);      
    }
    count++;
  }
  return path;
}

//*****************************************************************************
//    NAME: COMBATANT::FindPathToMapEdge
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::FindPathToMapEdge(void)
{    
  ClearPath();
  if (!CanMove(FALSE)) return FALSE;
  int shortestpath=-1;
  int shortestdist=INT_MAX;
  int ndist=INT_MAX;
  int edist=INT_MAX;
  int sdist=INT_MAX;
  int wdist=INT_MAX;

  int npath = FindPathToMapNorthEdge();
  if (npath >= 0) ndist = pathMgr.GetStepsRemaining(npath); 
  if (ndist <= 10)
  {
    // close enough, stop searching
    hPath = npath;
    return TRUE;
  }

  int epath = FindPathToMapEastEdge();
  if (epath >= 0) edist = pathMgr.GetStepsRemaining(epath); 
  if (edist <= 10)
  {
    // close enough, stop searching
    hPath = epath;
    return TRUE;
  }
  
  int spath = FindPathToMapSouthEdge();
  if (spath >= 0) sdist = pathMgr.GetStepsRemaining(spath); 
  if (sdist <= 10)
  {
    // close enough, stop searching
    hPath = spath;
    return TRUE;
  }

  int wpath = FindPathToMapWestEdge(); 
  if (wpath >= 0) wdist = pathMgr.GetStepsRemaining(wpath); 
  if (wdist <= 10)
  {
    // close enough, stop searching
    hPath = wpath;
    return TRUE;
  }

  // all are more than 10 squares away, pick the shortest
  if (ndist < shortestdist)
  {
    shortestdist = ndist;
    shortestpath = npath;
  }
  if (edist < shortestdist)
  {
    shortestdist = edist;
    shortestpath = epath;
  }
  if (sdist < shortestdist)
  {
    shortestdist = sdist;
    shortestpath = spath;
  }
  if (wdist < shortestdist)
  {
    shortestdist = wdist;
    shortestpath = wpath;
  }

  if (shortestpath != npath) pathMgr.FreePath(npath);
  if (shortestpath != epath) pathMgr.FreePath(epath);
  if (shortestpath != spath) pathMgr.FreePath(spath);
  if (shortestpath != wpath) pathMgr.FreePath(wpath);

  if (shortestpath >= 0)
  {
    hPath = shortestpath;
    return TRUE;
  }
  return FALSE;
}

//*****************************************************************************
// NAME: COMBATANT::CanUse
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanUse()
{
  if (IsDone(false, "Can combatant use")) return FALSE;
  return TRUE;
}

/*
//*****************************************************************************
// NAME: COMBATANT::CanTurnUndead
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanTurnUndead()
{
  if (hasTurnedUndead) return FALSE;
  if (IsDone(false)) return FALSE;

  //if (isCleric(GetClass()))
  //  return TRUE;

  //if ((isPaladin(GetClass())) && (GetCurrentLevel(PaladinFlag) >= 3))
  //  return TRUE;
  return m_pCharacter->GetTurnUndeadLevel() < 99;
}
*/

//*****************************************************************************
// NAME: COMBATANT::CanDelay
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanDelay()
{
  if (IsDone(false, "Can combatant delay")) return FALSE;
  return (m_iInitiative+1 < INITIATIVE_Never);
}
  
 
//*****************************************************************************
// NAME: COMBATANT::DelayAction
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::DelayAction()
{
  if (CanDelay())
  {
    if (m_iInitiative+1 < INITIATIVE_Never) m_iInitiative++;
    State(ICS_None);
    QueuedCombatantData &qcomb = GetQueuedCombatants();

    if (qcomb.Top() == self)
    {
      TRACE("DelayAction() for %i\n", self);
      // input was disabled if auto combatant
      EnableUserInput(TRUE);
      qcomb.Pop();
      if (qcomb.Top() != NO_DUDE)
      {
        if (qcomb.DelayedX() >= 0)
        {
          /* Really */ NotImplemented(0x34c2, false);
        };
      };
    }
  }
}

//*****************************************************************************
// NAME: COMBATANT::GetMaxTargets
//
// PURPOSE:  
//*****************************************************************************
int COMBATANT::GetMaxTargets()
{
  int max = 0;
  int mywpnidx;
  switch (State())
  {
  case ICS_Casting:
    die(0xab4c8); // handled elsewhere
    break;

  default:
    //mywpnidx = myItems.GetReady(WeaponHand);
    mywpnidx = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
    if (mywpnidx != NO_READY_ITEM)
    {
      // item is readied    
     // check for spell abilities
      if (   (State() == ICS_Using)
          && (m_pCharacter->myItems.GetCharges(mywpnidx) > 0))
      {
        ITEM_DATA *data = itemData.GetItem(m_pCharacter->myItems.GetItem(mywpnidx));
        if (data != NULL)
        {
          //if (data->m_gsID.IsValidSpell())
          if (data->spellID.IsValidSpell())
          {
            // get spell combattargets
          }
        }
      }
      else
      {
        // make sure it is a weapon
        if (isWeapon(m_pCharacter->myItems.GetItem(mywpnidx)))
        {
          if (GetType() != MONSTER_TYPE)
          {
            // check for sweeps
          }
          else
            max = 1;
        }
      }
    }
    else // using fists, claws, etc.
      max = 1;
    break;
  }

  return max;
}


//*****************************************************************************
// NAME: COMBATANT::CanTarget
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanTarget(bool freeAttack)
{
  if (!freeAttack && IsDone(freeAttack, "Can combatant choose target")) return FALSE;

  if ((State() == ICS_Casting) || (State() == ICS_Using))
  {
    die(0xab4c9); // spell targeting uses different data structures
    return FALSE;
  }
  else
    return TRUE;
}

//*****************************************************************************
// NAME: COMBATANT::HaveTarget
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::HaveTarget(int targ)
{
  if (combattargets.GetCount() == 0)
    return FALSE;

  POSITION pos = combattargets.GetHeadPosition();
  while (pos != NULL)
  {
    if (combattargets.GetNext(pos) == targ)
      return TRUE;
  }
  return FALSE;
}


//*****************************************************************************
// NAME: COMBATANT::RemoveTarget
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::RemoveTarget(int targ)
{
  if (combattargets.GetCount() == 0)
    return;

  POSITION pos = combattargets.GetHeadPosition();
  while (pos != NULL)
  {
    if (combattargets.PeekAtPos(pos) == targ)
    {
      combattargets.RemoveAt(pos);
      return;
    }
    else
      combattargets.GetNext(pos);
  }
}

//*****************************************************************************
// NAME: COMBATANT::AddTarget
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::CanAddTarget(int target)
{

  // Probably should add Special Abilities scripts at the end of this function.
  COMBATANT *pTarget = combatData.getCombatantPtr(target);
  if (   (pTarget->GetIsFriendly() && !m_pCharacter->targets.m_canTargetFriend)
      || (!pTarget->GetIsFriendly() && !m_pCharacter->targets.m_canTargetEnemy))
  {
    return FALSE;
  }
  else
  {
    int targrange = Distance(self, x, y, 
                             pTarget->self, pTarget->x,pTarget->y);
    {
      int dx, dy;
      dx = abs(x-pTarget->x);
      dy = abs(y-pTarget->y);
      if (dx > m_pCharacter->targets.m_maxRangeX)
      {
        menu.setItemInactive(4); // target
      };
      if (dy > m_pCharacter->targets.m_maxRangeY)
      {
        menu.setItemInactive(4); // target
      };
    };
    if (!m_pCharacter->targets.STD_CanAddTarget(pTarget->GetNbrHD(), targrange))
    {
      return FALSE;
    }
    else
    {
      if (m_pCharacter->targets.IsAlreadyTargeted(target))
      {
        return FALSE;
      };
    };
  };
  return TRUE;
}



void COMBATANT::AddTarget(int newTarget, bool freeAttack) 
{ 
  if (newTarget == NO_DUDE)
    return;

  RemoveTarget(newTarget);

  if (CanTarget(freeAttack))
  {
    // if using weapon, change to new target (only one allowed)
    //if (!combattargets.IsEmpty())
    //  combattargets.RemoveTail();

    COMBATANT *targCOMBATANT;
    targCOMBATANT = GetCombatantPtr(newTarget);
    ASSERT(targCOMBATANT!=NULL);
    if (targCOMBATANT==NULL) return;

    int dist = Distance(self, x, y, 
                        targCOMBATANT->self, targCOMBATANT->x, targCOMBATANT->y);  

    combattargets.Insert(newTarget, 
                   MakeTargetKey(newTarget, dist));
  }

  SetCurrTarget(); 
}

//*****************************************************************************
//    NAME: COMBATANT::AddTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::C_AddTarget(COMBATANT &targ, int range)
{
  int targX, targY;
  ASSERT( IsCombatActive() == TRUE ); // non-combat magic handled in char.cpp
  if (m_pCharacter->targets.m_SelectingUnits==FALSE) 
  {
    WriteDebugString("AddTarget() fails, SelectingUnits is FALSE\n");
    return FALSE;
  }
  if (m_pCharacter->targets.m_canTargetFriend==FALSE)
  {
    if (targ.GetIsFriendly() == this->GetIsFriendly())
    {
      WriteDebugString("AddTarget() fails, IsFriendly && FriendlyFireNotAllowed\n");
      return FALSE;
    };
  }
  if (m_pCharacter->targets.m_canTargetEnemy==FALSE)
  {
    if (targ.GetIsFriendly() != this->GetIsFriendly())
    {
      WriteDebugString("AddTarget() fails, IsEnemu && EnemyFireNotAllowed\n");
      return FALSE;
    };
  }
  targX = targ.GetCenterX();
  targY = targ.GetCenterY();
  if (!HaveLineOfSight(GetCenterX(), GetCenterY(), targX, targY, NULL))
  {
    WriteDebugString("AddTarget() fails, no line of sight from caster to target\n");
    return FALSE;    
  }
  return (m_pCharacter->targets.STD_AddTarget(targ.self, targ.GetNbrHD(), range, targX, targY));
}

//*****************************************************************************
//    NAME: COMBATANT::AddMapTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::AddMapTarget(int mapx, int mapy, PATH_DIR dir, int dirX, int dirY)
{
  ASSERT( IsCombatActive() == TRUE ); // non-combat magic handled in char.cpp
  if (m_pCharacter->targets.m_SelectingUnits==TRUE) return FALSE;

  if (!HaveLineOfSight(GetCenterX(), GetCenterY(), mapx, mapy, NULL))
  {
    WriteDebugString("AddMapTarget() fails, no line of sight from caster to mapx,mapy\n");
    return FALSE;    
  }
  m_pCharacter->targets.m_maptarglist.RemoveAll();

  //int range = targets.m_MaxRange;
  if (m_pCharacter->targets.AddMapTarget(mapx, mapy))
  {    
    // figure out which targets are in range
    MAPTARGETLIST targlist;
    MAPLOCATIONLIST maplist;
    int mapWidth, mapHeight;
    int maxtargs=100000;
    int VisCheckX=0,VisCheckY=0;
    bool screenself=false;
    mapWidth = m_pCharacter->targets.Width();
    mapHeight = m_pCharacter->targets.Height();
    switch (m_pCharacter->targets.m_targType)
    {
    case NowAreaCircle: // circle is centered at x,y
    case LingerAreaCircle: // circle is centered at x,y
      //maxtargs = m_pCharacter->targets.m_MaxTargets;
      maxtargs = m_pCharacter->targets.MaxTargets();
      GetCombatantsInCircle(targlist, 
                            mapx,mapy, 
                            //m_pCharacter->targets.m_MaxRange);
                            mapWidth);
      GetMapTilesInCircle(maplist,//targets.m_maptarglist, 
                          mapx,mapy, 
                          //m_pCharacter->targets.m_MaxRange);      
                          mapWidth);      
      VisCheckX=mapx;
      VisCheckY=mapy;
      break;

    case NowAreaLinePickStart: // line starts at mapx,mapy, dir is from caster to mapx,mapy
    case LingerAreaLinePickStart: // line starts at mapx,mapy, dir is from caster to mapx,mapy
      screenself=true;
      GetCombatantsInLine(targlist,  
                          mapx,mapy,            // start
                          //m_pCharacter->targets.m_MaxRange,   // length
                          m_pCharacter->targets.MaxRange(),   // length
                          //m_pCharacter->targets.m_MaxTargets, // width
                          m_pCharacter->targets.MaxTargets(), // width
                          dir);
      GetMapTilesInLine(maplist,  
                        mapx,mapy,            // start
                        //m_pCharacter->targets.m_MaxRange,   // length
                        m_pCharacter->targets.MaxRange(),   // length
                        //m_pCharacter->targets.m_MaxTargets, // width
                        m_pCharacter->targets.MaxTargets(), // width
                        dir);      
      VisCheckX=mapx;
      VisCheckY=mapy;
      break;

    case NowAreaLinePickEnd: // line starts at caster, towards selected map location
    case LingerAreaLinePickEnd: // line starts at caster, towards selected map location
      screenself=true;
      GetCombatantsInLine(targlist, 
                          this->x,this->y,       // start
                          mapx,mapy,             // end
                          //m_pCharacter->targets.m_MaxTargets); // width
                          m_pCharacter->targets.MaxTargets()); // width
      GetMapTilesInLine(maplist, 
                        this->x,this->y,       // start
                        mapx,mapy,             // end
                        //m_pCharacter->targets.m_MaxTargets); // width      
                        m_pCharacter->targets.MaxTargets()); // width      
      VisCheckX=this->x;
      VisCheckY=this->y;
      break;

    case NowAreaSquare: // square is centered at x,y
      {
        GetMapTilesInRectangle(maplist, 
                               mapx,mapy,dirX,dirY,
                               //m_pCharacter->targets.m_MaxTargets,  // width
                               mapWidth,  // width
                               //m_pCharacter->targets.m_MaxRange);   // height                                
                               mapHeight,   // height                                
                               true);
//      GetCombatantsInRectangle(targlist, 
//                               mapx,mapy,dirX,dirY,
//                               //m_pCharacter->targets.m_MaxTargets,  // width
//                               m_pCharacter->targets.Width(),  // width
//                               //m_pCharacter->targets.m_MaxRange);   // height                          
//                               m_pCharacter->targets.Height());   // height
        GetCombatantsInRectangle(targlist, maplist);
      };
      break;
    case LingerAreaSquare: // square is centered at x,y
      GetMapTilesInRectangle(maplist, 
                             mapx,mapy,dirX,dirY,
                             //m_pCharacter->targets.m_MaxTargets,  // width
                             mapWidth,  // width
                             //m_pCharacter->targets.m_MaxRange);   // height                                
                             mapHeight,   // height                                
                             true);
      VisCheckX=mapx;
      VisCheckY=mapy;
      break;

    case NowAreaCone: // cone starts at x,y, extends in 'dir' direction
    case LingerAreaCone: // cone starts at x,y, extends in 'dir' direction
      screenself=true;
      /*
       * These originals worked approximately only for the 8 major directions.
       * I have attempted to make it work exactly for arbitrary directions.
       * GetCombatantsInCone(targlist, 
       *                    mapx,mapy,            // start
       *                    targets.m_MaxRange,   // length
       *                    targets.m_MaxTargets, // width at end of cone
       *                    dir);      
       *GetMapTilesInCone(maplist, 
       *                  mapx,mapy,            // start
       *                  targets.m_MaxRange,   // length
       *                  targets.m_MaxTargets, // width at end of cone
       *                  dir);            
       */
      GetCombatantsAndTilesInCone(targlist, maplist,
                          x, y,                 // Caster
                          mapx,mapy,            // target
                          //m_pCharacter->targets.m_MaxRange,   // length
                          mapHeight,   // length
                          //m_pCharacter->targets.m_MaxTargets  // width at end of cone
                          mapWidth,  // width at end of cone
                          true);
      //GetMapTilesInCone(maplist, 
      //                  mapx,mapy,            // start
      //                  targets.m_MaxRange,   // length
      //                  targets.m_MaxTargets, // width at end of cone
      //                  dir);            
      VisCheckX=mapx;
      VisCheckY=mapy;
      break;

    default: die(0xab4ca); break;
    }

    // now add them to the target list
    POSITION pos=targlist.GetHeadPosition();
    int targcount=0;
    while ((pos!=NULL)&&(targcount < maxtargs))
    {
      int index = m_pCharacter->targets.m_targets.GetSize();
      int dude = targlist.GetAt(pos);

      // target selection code does not care about walls or other map
      // obstructions, so weed out the targets that are not visible from
      // the source location of the spell.
      if (HaveLineOfSight(VisCheckX,VisCheckY, GetCombatantPtr(dude)->x,GetCombatantPtr(dude)->y,NULL))
      {
        if (screenself)
        {
          if (dude!=self)
          {
            m_pCharacter->targets.m_targets.SetAtGrow(index, dude);
            targcount++;
          }
        }
        else
        {
          m_pCharacter->targets.m_targets.SetAtGrow(index, dude);
          targcount++;
        }
      }

      targlist.GetNext(pos);      
    }

    // now add them to the map target list
    pos=maplist.GetHeadPosition();
    while (pos!=NULL)
    {
      POINT pt = maplist.GetNext(pos);
      
      // target selection code does not care about walls or other map
      // obstructions, so weed out the targets that are not visible from
      // the source location of the spell.
      if (HaveLineOfSight(VisCheckX,VisCheckY, pt.x, pt.y,NULL))
      {
        if (HaveMovability(pt.y, pt.x))
        {
          m_pCharacter->targets.m_maptarglist.AddTail(pt);     
        };
      };
    }

    if (targcount > 0)
    {
      ASSERT( m_pCharacter->targets.m_maptarglist.GetCount() > 0 );
    }
  }
  return (m_pCharacter->targets.m_targets.GetSize()>0);
}

//*****************************************************************************
//    NAME: COMBATANT::AddTargetSelf
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::AddTargetSelf()
{
  return (C_AddTarget(*this, 0));
}

//*****************************************************************************
//    NAME: COMBATANT::AutoChooseSpellTargets
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::AutoChooseSpellTargets()
{

}

//*****************************************************************************
// NAME: COMBATANT::RemoveAllTargets
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::RemoveAllTargets() 
{ 
  combattargets.RemoveAll();
  SetCurrTarget();
}

//*****************************************************************************
// NAME: COMBATANT::SetCurrTarget
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::SetCurrTarget()
{  
  if (!combattargets.IsEmpty())
  {
    targetPos = combattargets.GetHeadPosition();
    SetTarget(combattargets.GetAtPos(targetPos));
  }
  else
  {
    SetTarget(NO_DUDE);
    targetPos = NULL;
  }
}

//*****************************************************************************
// NAME: COMBATANT::RemoveCurrTarget
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::RemoveCurrTarget()
{
  if (!combattargets.IsEmpty())
    combattargets.RemoveHead();

  SetCurrTarget();
}


//*****************************************************************************
// NAME: COMBATANT::GetNextTarget
//
// PURPOSE:  
//*****************************************************************************
int COMBATANT::GetNextTarget()
{
  int dude = NO_DUDE;

  if (combattargets.IsEmpty())
    return NO_DUDE;

  if (targetPos == NULL)
  {
    targetPos = combattargets.GetHeadPosition();
  }
  else
  {
    combattargets.GetNext(targetPos);
    if (targetPos == NULL)
      targetPos = combattargets.GetHeadPosition();
  }

  dude = combattargets.PeekAtPos(targetPos);
  return dude;
}

//*****************************************************************************
// NAME:    COMBATANT::FaceOpponent
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::FaceOpponent(int opponent)
{
  COMBATANT *pOpponent;
  pOpponent = GetCombatantPtr(opponent);
  ASSERT(pOpponent != NULL);
  if (pOpponent == NULL) return;
  int ax = pOpponent->x;
  int ay = pOpponent->y;
  PATH_DIR dir;

  // get direction to attacker
  if (x > ax) // nw, w, sw
  {
    if (y > ay) dir = PathNW;
    else if (y < ay) dir =  PathSW;
    else dir = PathWest;
  }
  else if (x < ax) // ne, e, se
  {
    if (y > ay) dir = PathNE;
    else if (y < ay) dir = PathSE;
    else dir = PathEast;
  }
  else // x == ax
  {
    if (y > ay) dir = PathNorth;
    else if (y < ay) dir = PathSouth;
    else  dir = PathBAD; // curr == dest
  }

  // if backstabbing, don't face attacker

  FaceDirection(dir);
}

//*****************************************************************************
//    NAME: COMBATANT::FaceLocation
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::FaceLocation(int fx, int fy)
{
  int ax = fx;
  int ay = fy;
  PATH_DIR dir;

  // get direction to x,y
  if (x > ax) // nw, w, sw
  {
    if (y > ay) dir = PathNW;
    else if (y < ay) dir =  PathSW;
    else dir = PathWest;
  }
  else if (x < ax) // ne, e, se
  {
    if (y > ay) dir = PathNE;
    else if (y < ay) dir = PathSE;
    else dir = PathEast;
  }
  else // x == ax
  {
    if (y > ay) dir = PathNorth;
    else if (y < ay) dir = PathSouth;
    else  dir = PathBAD; // curr == dest
  }

  FaceDirection(dir);
}

//*****************************************************************************
//    NAME: COMBATANT::FaceDirection
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::FaceDirection(PATH_DIR dir)
{
  // translate attacker direction into
  // east/west icon facing
  switch (dir)
  {
  case PathNW:
    m_iMoveDir = FACE_NW;
    m_iFacing = FACE_WEST;
    break;
  case PathWest:
    m_iMoveDir = FACE_WEST;
    m_iFacing = FACE_WEST;
    break;
  case PathSW:
    m_iMoveDir = FACE_SW;
    m_iFacing = FACE_WEST;
    break;
  case PathNE:
    m_iMoveDir = FACE_NE;
    m_iFacing = FACE_EAST;
    break;
  case PathEast:
    m_iMoveDir = FACE_EAST;
    m_iFacing = FACE_EAST;
    break;
  case PathSE:
    m_iMoveDir = FACE_SE;
    m_iFacing = FACE_EAST;
    break;

  default:
    // if north/south attacker, no need to change facing.
    break;
  }
}

//*****************************************************************************
// NAME:    COMBATANT::GetDamageDice
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::GetDamageDice(int wpn, 
                              int *pNum, 
                              int *pSides, 
                              int *pBonus, 
                              BOOL *pNonLethal, 
                              BOOL IsLarge, 
                              //GLOBAL_SPELL_ID *pSpellDbKey) const
                              CString *pSpellName) const
{
  *pNonLethal=FALSE;
  //pSpellDbKey->Clear();
  pSpellName->Empty();
  
  if (wpn != NO_READY_ITEM)
  {  
     ITEM_DATA *idata = itemData.GetItem(m_pCharacter->myItems.GetItem(wpn));

     if (idata != NULL)
     {      
        *pNonLethal = idata->IsNonLethal;

        if (IsLarge)
        {
           *pNum = idata->Nbr_Dice_Lg;
           *pSides = idata->Dmg_Dice_Lg;
           *pBonus = idata->Dmg_Bonus_Lg + GetAdjDmgBonus() + idata->Attack_Bonus;
        }
        else 
        {
           *pNum = idata->Nbr_Dice_Sm;
           *pSides = idata->Dmg_Dice_Sm;
           *pBonus = idata->Dmg_Bonus_Sm + GetAdjDmgBonus() + idata->Attack_Bonus;
        }
     }
     else 
     {
        WriteDebugString("Bogus item num in GetDamageDice()\n");
     }
  }
  else 
  {
    if (GetType()==MONSTER_TYPE)
    {
      int nbrAttacks = GetNbrAttacks();
      int currAttack = nbrAttacks - availAttacks;
      //20130829 PRS  ASSERT( (currAttack>=0) && (currAttack<nbrAttacks) );
      if ((currAttack < 0)||(currAttack>=nbrAttacks)) currAttack=0;
      
      //MONSTER_DATA *pData = monsterData.GetMonsterData(origKey);
      const MONSTER_DATA *pMonster = monsterData.PeekMonster(m_pCharacter->monsterID);
      //ASSERT( pData!=NULL );
      ASSERT( pMonster!=NULL );

      //if (pData == NULL) 
      if (pMonster == NULL) 
      {
        *pNum   = m_pCharacter->unarmedNbrDieL;
        *pSides = m_pCharacter->unarmedDieL;
        *pBonus = GetAdjDmgBonus();
        return;
      }

      //if (pData->attackData.GetCount() == 0)
      if (pMonster->attackData.GetMonsterAttackDetailsCount() == 0)
      {
        //WriteDebugString("Monster %s has zero attacks defined\n", pData->Name);
        WriteDebugString("Monster %s has zero attacks defined\n", pMonster->Name);
        *pNum   = m_pCharacter->unarmedNbrDieL;
        *pSides = m_pCharacter->unarmedDieL;
        *pBonus = GetAdjDmgBonus();
        return;
      }

      //if ((currAttack < 0)||(currAttack>=pData->attackData.GetCount()))
      if ((currAttack < 0)||(currAttack>=pMonster->attackData.GetMonsterAttackDetailsCount()))
        currAttack=0;

      //*pNum = pData->attackData.GetAtIndex(currAttack).nbr;
      //*pSides = pData->attackData.GetAtIndex(currAttack).sides;
      //*pBonus = pData->attackData.GetAtIndex(currAttack).bonus;
      ////*pSpellDbKey = pData->attackData.GetAtIndex(currAttack).spelldbkey;
      //*pSpellName = pData->attackData.GetAtIndex(currAttack).spellID;
      *pNum = pMonster->attackData.PeekMonsterAttackDetails(currAttack)->nbr;
      *pSides = pMonster->attackData.PeekMonsterAttackDetails(currAttack)->sides;
      *pBonus = pMonster->attackData.PeekMonsterAttackDetails(currAttack)->bonus;
      //*pSpellDbKey = pData->attackData.GetAtIndex(currAttack).spelldbkey;
      *pSpellName = pMonster->attackData.PeekMonsterAttackDetails(currAttack)->spellID;
    }
    else
    {
      if (IsLarge)
      {
        *pNum   = m_pCharacter->unarmedNbrDieL;
        *pSides = m_pCharacter->unarmedDieL;
        *pBonus = GetAdjDmgBonus();
      }
      else 
      {
        *pNum   = m_pCharacter->unarmedNbrDieS;
        *pSides = m_pCharacter->unarmedDieS;
        *pBonus = m_pCharacter->unarmedBonus + GetAdjDmgBonus();
      }
    }
  }
}

//*****************************************************************************
// NAME:    COMBATANT::LoadCombatIcon
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::LoadCombatIcon()
{
#ifdef newCombatant
  if (m_pCharacter->icon.filename.IsEmpty()) return;

  m_pCharacter->icon.FreePic();
#else
  if (icon.filename.IsEmpty()) return;

  icon.FreePic();
#endif

  CString prefix;
  switch (GetType())
  {
  case MONSTER_TYPE: prefix="cm_"; break;
  default: prefix="cn_";break;
  }

  // load icon data as a sprite
  //WriteDebugString("112 Calling LoadPicSurfaces(%s) for character(%s)\n", "", GetName());
#ifdef newCombatant
  if (!m_pCharacter->icon.LoadPicSurfaces(""))
  {
    if (!debugStrings.AlreadyNoted(CString("FTLIF01") + m_pCharacter->icon.filename))
    {
      writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
      debugSeverity = 5;
      WriteDebugString("Failed to load icon file %s in LoadCombatIcon\n", m_pCharacter->icon.filename);
    };
    // one last try
    m_pCharacter->icon.filename = DEFAULT_MONSTER_ICON;
    //AddFolderToPath(icon.filename, rte.PicArtDir());
    m_pCharacter->icon.picType=IconDib;
    m_pCharacter->icon.SetDefaults();
    //WriteDebugString("113 Calling LoadPicSurfaces(%s)\n", rte.IconArtDir());
    if (!m_pCharacter->icon.LoadPicSurfaces(prefix))
    {
      SetMiscError(MissingMonsterCombatIcons);
      if (!debugStrings.AlreadyNoted(CString("FTLDCI01") + m_pCharacter->icon.filename))
      {
        writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
        debugSeverity = 5;
        WriteDebugString("Failed to load default combat icon %s in LoadCombatIcon()\n",
                          m_pCharacter->icon.filename);
      };
      m_pCharacter->icon.FreePic();
    }
  }
#else
  if (!icon.LoadPicSurfaces(""))
  {
    if (!debugStrings.AlreadyNoted(CString("FTLIF01") + icon.filename))
    {
      writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
      debugSeverity = 5;
      WriteDebugString("Failed to load icon file %s in LoadCombatIcon\n", icon.filename);
    };
    // one last try
    icon.filename = DEFAULT_MONSTER_ICON;
    //AddFolderToPath(icon.filename, rte.PicArtDir());
    icon.picType=IconDib;
    icon.SetDefaults();
    //WriteDebugString("113 Calling LoadPicSurfaces(%s)\n", rte.IconArtDir());
    if (!icon.LoadPicSurfaces(prefix))
    {
      SetMiscError(MissingMonsterCombatIcons);
      if (!debugStrings.AlreadyNoted(CString("FTLDCI01") + icon.filename))
      {
        writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
        debugSeverity = 5;
        WriteDebugString("Failed to load default combat icon %s in LoadCombatIcon()\n",
                          icon.filename);
      };
      icon.FreePic();
    }
  }
#endif
}

//*****************************************************************************
// NAME:    COMBATANT::determineIconSize
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::determineIconSize()
{
  int w,h,bits;
#ifdef newCombatant
  if (GraphicsMgr.GetSpriteSurfaceStats(m_pCharacter->icon.key, w, h, bits))
#else
  if (GraphicsMgr.GetSpriteSurfaceStats(icon.key, w, h, bits))
#endif
  {
    int imagewidth=w; // pixel width

    // (width / frame_width) / 2_frames_per_icon
    w = (w / COMBAT_TILE_WIDTH) / 2;

    // 2 frame minimum (one of each pose, rdy/attack)
    // frame count is multiple of 2
    // the icon frame pair used is indicated by iconIndex
    // adjust for number of frames in icon
#ifdef newCombatant
    w /= (m_pCharacter->icon.NumFrames/2);
#else
    w /= (icon.NumFrames/2);
#endif
    
    h = h / COMBAT_TILE_HEIGHT;
    
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    width = w;
    height= h;
#ifdef newCombatant
    int offset = (m_pCharacter->iconIndex-1)*((width*COMBAT_TILE_WIDTH)*2);
    if ((offset+(width*COMBAT_TILE_WIDTH)) >= (imagewidth-width))
      m_pCharacter->iconIndex=1;
#else
    int offset = (iconIndex-1)*((width*COMBAT_TILE_WIDTH)*2);
    if ((offset+(width*COMBAT_TILE_WIDTH)) >= (imagewidth-width))
      iconIndex=1;
#endif
  }
  else
#ifdef newCombatant
    m_pCharacter->icon.FreePic();
#else
    icon.FreePic();
#endif
}

//*****************************************************************************
// NAME:    COMBATANT::GetAttackBonus
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
//int COMBATANT::GetAttackBonus(GLOBAL_ITEM_ID weaponID, int distance) const
int COMBATANT::GetAttackBonus(const ITEM_ID& weaponID, int distance) const
{
  int bonus = 0;

  //if (myItems.GetReady(WeaponHand) != NO_READY_ITEM)
  if (m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0) != NO_READY_ITEM)
  {  
     //ITEM_DATA *idata = itemData.GetItemData(myItems.GetItem(myItems.GetReady(WeaponHand)));
     ITEM_DATA *idata = itemData.GetItem(m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0)));
     if (idata != NULL)
     {      
       bonus += idata->Attack_Bonus;
     }
     else 
     {
        WriteDebugString("Bogus item num in GetAttackBonus()\n");
     }
  }

  //bonus += GetAdjHitBonus(weaponID, distance);
  bonus += GetAdjHitBonus(weaponID, distance);

  return bonus;
}

//*e****************************************************************************
// NAME:    COMBATANT::TakeDamage
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::TakeDamage(int dmg,
                           BOOL IsNonLethal,
                           InvokeSpellOptions *invokeOptions,
                           bool canFinishCasting,
                           int *pDeathIndex)
{  
  // don't beat a dead horse!
  // *but!* allow dying to take 1 hp damage per turn if not yet bandaged
  //
  charStatusType stype = GetAdjStatus();
  if (   (stype == Fled)
      || (stype == Gone)
      || (stype == TempGone)
      || (stype == Dead))
    return;

  if (IsNonLethal) dmg=0;

  // adjust raw hitpoints
  SetHitPoints(GetHitPoints()-dmg);

  int trueHp = GetHitPoints();
  int startTrueHp = trueHp;

  // can't go below -10
  // and 10 = dead
  if (trueHp < -10)
    trueHp = -10;

  if (!IsPartyMember())
  {
    // monster die when hp reaches zero
    if (trueHp <= 0)
      trueHp = -10;
    // unless configured to not die
    if ((trueHp < 1) && (GetConfigMonsterNoDeath())) // debug flag
      trueHp = 1;
    if ((trueHp < 1) && (HasDeathImmunity())) // monster immunity flag
      trueHp = -10; 
  }

  // set to modified value if needed
  if (trueHp != startTrueHp)
    SetHitPoints(trueHp);

  // now get adjusted hp value, which can
  // magically change the current hp
  //
  int adjHp = GetAdjHitPoints();

  if (!IsPartyMember())
  {
    // 20120605  PRS I changed this to avoid the ASSERT about 25 lines below.
    // It also agress with the code about 21 lines above.
    if (adjHp <= 0)
    //if (adjHp < 0)
      adjHp = -10;

    // still can't let monsters die, even if hp is 
    // magically below 0
    if ((adjHp < 1) && (GetConfigMonsterNoDeath()))
      adjHp = 1;
    if ((adjHp < 1) && (HasDeathImmunity())) // monster immunity flag
      adjHp = -10;     
  }

  if (adjHp <= -10)
  {
    SetStatus(Dead);
    if (getCombatantInCell(x, y, 1, 1, NO_DUDE) == self)
    {
      placeCombatant(x, y, NO_DUDE, width, height);
    };
    placeDyingCombatant(x,y,NO_DUDE,width,height);
    TRACE("%i takes %i hp damage, -10 left, and is dead\n", self, dmg);
    if ( (invokeOptions != NULL) && (invokeOptions->m_supressBlitSprite & DeathSprite))
    {
      invokeOptions->m_whatSpriteNeeded |= DeathSprite;
    }
    else
    {
      if (pDeathIndex != NULL)
      {
        *pDeathIndex = self;
      }
      else
      {
        blitDeadSprite();
      };
    };
  }
  else if (adjHp <= 0)
  {
    ASSERT( IsPartyMember() );
    if (getCombatantInCell(x, y, 1, 1, NO_DUDE) == self)
    {
      placeCombatant(x, y, NO_DUDE, width, height);
    };
    placeDyingCombatant(x,y,self,width,height);
    SetStatus(Dying);
    isBandaged = FALSE;
    bandageWho=-1;
    TRACE("%i takes %i hp damage, %i left, and is dying\n", self, dmg, adjHp);
    if ( (invokeOptions != NULL) && (invokeOptions->m_supressBlitSprite & DyingSprite))
    {
      invokeOptions->m_whatSpriteNeeded |= DyingSprite;
    }
    else
    {
      blitDyingSprite();
    };
  }

  if (adjHp <= 0)
  {
    // update stats used for monster morale
    if (friendly)
     IncNumFriendSlain();
    else
     IncNumMonsterSlain();

    // if dead, this dude can't do anything else    
    StopCasting(TRUE, false); // also calls EndTurn()
    m_iMovement = GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, "Combatant HP < 0");          
  }
  else if (dmg > 0) // cancel any pending spell casting if damage taken
  {
    StopCasting(FALSE, canFinishCasting); // does not call EndTurn()
  };

  UpdateSpellForDamage(dmg);
}

//*****************************************************************************
// NAME:    COMBATANT::PlayHit
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::PlayHit() const
{
  //if (myItems.GetReady(WeaponHand) != NO_READY_ITEM)
  if (m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0) != NO_READY_ITEM)
  {  
     //itemData.PlayHit(myItems.GetItem(myItems.GetReady(WeaponHand)));
     itemData.PlayHit(m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0)));
  }
  else
  { 
    if (GetType() == MONSTER_TYPE)
      //monsterData.PlayHit(origKey);
      monsterData.PlayHit(m_pCharacter->monsterID);
    else
      PlayCharHit();
  }
}

//*****************************************************************************
// NAME:    COMBATANT::PlayMiss
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::PlayMiss() const
{
  //if (myItems.GetReady(WeaponHand) != NO_READY_ITEM)
#ifdef newCombatant
  if (m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0) != NO_READY_ITEM)
  {  
     //itemData.PlayMiss(myItems.GetItem(myItems.GetReady(WeaponHand)));
     itemData.PlayMiss(m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0)));
  }
#else
  if (myItems.GetReadiedItem(WeaponHand, 0) != NO_READY_ITEM)
  {  
     //itemData.PlayMiss(myItems.GetItem(myItems.GetReady(WeaponHand)));
     itemData.PlayMiss(myItems.GetItem(myItems.GetReadiedItem(WeaponHand, 0)));
  }
#endif
  else
  { 
    if (GetType() == MONSTER_TYPE)
      //monsterData.PlayMiss(origKey);
      monsterData.PlayMiss(m_pCharacter->monsterID);
    else
      PlayCharMiss();
  }
}

//*****************************************************************************
//    NAME: COMBATANT::PlayLaunch
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::PlayLaunch() const
{
  //if (myItems.GetReady(WeaponHand) != NO_READY_ITEM)
#ifdef newCombatant
  if (m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0) != NO_READY_ITEM)
  {  
     //itemData.PlayLaunch(myItems.GetItem(myItems.GetReady(WeaponHand)));
     itemData.PlayLaunch(m_pCharacter->myItems.GetItem(m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0)));
  }
#else
  if (myItems.GetReadiedItem(WeaponHand, 0) != NO_READY_ITEM)
  {  
     //itemData.PlayLaunch(myItems.GetItem(myItems.GetReady(WeaponHand)));
     itemData.PlayLaunch(myItems.GetItem(myItems.GetReadiedItem(WeaponHand, 0)));
  }
#endif
}



//*****************************************************************************
// NAME: IsAttackPossible
//
// PURPOSE:  
// Can this combatant attack in any way?
// For example, he cannot attack if he has ranged weapon and opponent is adjacent.
//*****************************************************************************
BOOL COMBATANT::IsAttackPossible(void)
{
  //int mywpnitemidx = myItems.GetReady(WeaponHand);
  int mywpnitemidx = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
  if (mywpnitemidx == NO_READY_ITEM)
  {
    return TRUE;
  }
  else
  {       
    // using a weapon rather than natural attack types (ie claws/jaws/fists)
    //GLOBAL_ITEM_ID wpn_giID;
    ITEM_ID wpn_ID;
    CString result;
    ITEM_DATA *pWeapon;
    //wpn_giID = m_pCharacter->myItems.GetItem(mywpnitemidx);
    wpn_ID = m_pCharacter->myItems.GetItem(mywpnitemidx);
    //pWeapon = itemData.GetItemData(wpn_giID);
    pWeapon = itemData.GetItem(wpn_ID);
    {
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      weaponClassType wpnClass;

      wpnClass = pWeapon->Wpn_Type;
      scriptContext.SetCombatantContext(this);
      scriptContext.SetCharacterContext(this->m_pCharacter);
      scriptContext.SetItemContext(pWeapon);
      scriptContext.SetItemContextKey(mywpnitemidx);
      if (wpnClass >= MAX_WEAPON_CLASS) wpnClass = NotWeapon;
      hookParameters[5] = WeaponClassText[wpnClass];
      hookParameters[6] = combatData.ListAdjacentCombatants(this->self, TRUE);
      result = pWeapon->RunItemScripts(
                   CAN_AIM, 
                   ScriptCallback_RunAllScripts, 
                   NULL,
                   "Test if attack possible");
      if (result.IsEmpty())
      {
        result = m_pCharacter->RunCharacterScripts(
                       CAN_AIM, 
                       ScriptCallback_RunAllScripts, 
                       NULL, 
                       "Test if attack possible");
      };
    };
    if (!result.IsEmpty() && (result[0] == 'N')) return FALSE;
    return TRUE;
  };
}


//*****************************************************************************
// NAME: canAttack
//
// PURPOSE:  
// check non-magical attack capabilities
//*****************************************************************************
BOOL COMBATANT::canAttack(int targ, 
                          int targetX, int targetY, 
                          int additionalAttacks, 
                          int (*DistanceFunction)(int,int,int,int,int,int),
                          bool canAttackSelf)
{  
  if (availAttacks + additionalAttacks <= 0)
    return FALSE;

  if (targ == NO_DUDE)
    return FALSE;

  if (   (availAttacks + additionalAttacks < 1.0)
     && (GetCurrentRound() - lastAttackRound <= 1))
    return FALSE;

  // cannot attack yourself
  if ((targ == self) && !canAttackSelf)
    return FALSE;  

  COMBATANT *targCOMBATANT;
  targCOMBATANT = GetCombatantPtr(targ);
  ASSERT(targCOMBATANT!=NULL);
  if (targCOMBATANT==NULL) return FALSE;
  if (targetX < 0) targetX = targCOMBATANT->x;
  if (targetY < 0) targetY = targCOMBATANT->y;

  // if attacking dude on same side of battle
  if (targCOMBATANT->GetIsFriendly() == GetIsFriendly())
  {
    // AI will never attack dudes on same side
    if (OnAuto(false) && !canAttackSelf) // includes all monsters (always auto), and auto dudes.
    {
      return FALSE;
    }
    else if ((targCOMBATANT->GetType() == CHAR_TYPE) && !canAttackSelf)
    {
      // cannot attack pc party members with weapon
      return FALSE;
    }
    else if ((targCOMBATANT->GetType() == NPC_TYPE)&&(!targCOMBATANT->m_pCharacter->IsPreGen))
    {      
      // if player chooses to attack friendly npc, the npc changes sides,
      // even if unable to perform attack due to following checks.
      //
      // This does not belong here.  We are only attempting to determine
      // if we CAN attack targCOMBATANT.  We have not actually performed
      // any attack yet.  Perhaps this line of code belongs elsewhere.
      // PRS 20140916
      //targCOMBATANT->friendly = !targCOMBATANT->friendly;
      //
      //
      //
      // npc will leave the battle (and the party after combat)?
      //targCOMBATANT.SetMorale(0);
    }
  }

  int dis = DistanceFunction(self, x, y, targ, targetX, targetY);  
  //int mywpnitemidx = myItems.GetReady(WeaponHand);
  int mywpnitemidx = m_pCharacter->myItems.GetReadiedItem(WeaponHand, 0);
  if (mywpnitemidx != NO_READY_ITEM)
  {       
     // using a weapon rather than natural attack types (ie claws/jaws/fists)
     //GLOBAL_ITEM_ID wpn_giID = m_pCharacter->myItems.GetItem(mywpnitemidx);
     ITEM_ID wpn_ID;
     wpn_ID = m_pCharacter->myItems.GetItem(mywpnitemidx);
 
     if (m_pCharacter->myItems.GetQty(mywpnitemidx) <= 0)
        return FALSE;
 
     //if (!WpnCanAttackAtRange(wpn_giID, dis))
     if (!WpnCanAttackAtRange(wpn_ID, dis))
       return FALSE;

     //switch (itemData.GetWpnType(wpn_giID)) 
     switch (itemData.GetWpnType(wpn_ID)) 
     {
     case NotWeapon: // not weapon, must be used, invokes special abilities
        //if (isMagical(wpn_giID))
        if (isMagical(wpn_ID))
        {
           if (m_pCharacter->myItems.GetCharges(mywpnitemidx) <= 0)
              return FALSE;
        }
        else
           return FALSE;
        break;

     case HandBlunt: // hand, no throw, blunt
     case HandCutting: // hand, no throw, cutting
     case HandThrow: // hand or throw
     case SlingNoAmmo: // sling
     case SpellCaster:
     case SpellLikeAbility:
       //
       // handled by WpnCanAttackAtRange()
       //
       break;

     case Bow: // bow
     case Crossbow: // crossbow
       {
         //int myammoitemidx = myItems.GetReady(AmmoQuiver);         
         int myammoitemidx = m_pCharacter->myItems.GetReadiedItem(AmmoQuiver, 0);         

         // arrows or bolts must be readied in AmmoQuiver
         if (myammoitemidx == NO_READY_ITEM)
           return FALSE;

         //GLOBAL_ITEM_ID ammo_giID = m_pCharacter->myItems.GetItem(myammoitemidx);
         ITEM_ID ammo_ID = m_pCharacter->myItems.GetItem(myammoitemidx);

         //if (itemData.GetWpnType(ammo_giID) != Ammo)
         if (itemData.GetWpnType(ammo_ID) != Ammo)
           return FALSE;

         // ammo class must match between weapon and ammo
         CString myammoclass(m_pCharacter->myItems.GetAmmoClass(myammoitemidx));
         if (myammoclass.IsEmpty()) return FALSE; // might be wand,potion,amulet,etc

         if (myammoclass != m_pCharacter->myItems.GetAmmoClass(mywpnitemidx))
           return FALSE;

         // must have non-zero qty of ammo
         if (m_pCharacter->myItems.GetQty(myammoitemidx) <= 0)
           return FALSE;
       }
       break;

     case Throw:
       //
       // handled by WpnCanAttackAtRange()
       //
       /*
        if (dis < 2)
           return FALSE;
        */
        break;

     case Ammo: // ammo - arrows or bolts
        return FALSE;

     default:
        WriteDebugString("Unhandled item Wpn_Type in canAttack()\n");
        return FALSE;
     }
  }
  else
  {
     // use natural attack weapons (ie claws, jaws, paws, fists)
     if (dis > 1)
        return FALSE;
  }
 
  // passed all tests so far, now check for line of sight
  if (!HaveLineOfSight(GetCenterX(), GetCenterY(), targetX, targetY,NULL))
    return FALSE;

  if (!combatData.IsValidTarget(this, targCOMBATANT, NULL)) return FALSE;

  if (dis > 1)
  {
    if (!GetAdjDetectingInvisible())
    {
      // cannot attack invisible targets with ranged weapons
      if (targCOMBATANT->GetAdjSpecAb(SA_Invisible))
        return FALSE;

      //if (GetUndeadType() != NotUndead)
      if (!GetUndeadType().IsEmpty())
      {
        if (targCOMBATANT->GetAdjSpecAb(SA_InvisibleToUndead))
          return FALSE;
      }

      if (IsAnimal())
      {
        if (targCOMBATANT->GetAdjSpecAb(SA_InvisibleToAnimals))
          return FALSE;
      }
    }
  }

  return TRUE;
}

//*****************************************************************************
// NAME:    COMBATANT::moveNW
//
// PURPOSE: Change the facing of the icon to the direction of the keypress
//			and move the icon in the stated direction
//*****************************************************************************
void COMBATANT::moveNW()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();

   m_iFacing = FACE_WEST;
   m_iMoveDir = FACE_NW;
   moveX = x-1;
   moveY = y-1;
}

//*****************************************************************************
// NAME:    COMBATANT::moveNE
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::moveNE()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();

   m_iFacing = FACE_EAST;
   m_iMoveDir = FACE_NE;
   moveX = x+1;
   moveY = y-1;
}

//*****************************************************************************
// NAME:    COMBATANT::moveSE
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::moveSE()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();

   m_iFacing = FACE_EAST;
   m_iMoveDir = FACE_SE;
   moveX = x+1;
   moveY = y+1;
}

//*****************************************************************************
// NAME:    COMBATANT::moveSW
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::moveSW()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();

   m_iFacing = FACE_WEST;
   m_iMoveDir = FACE_SW;
   moveX = x-1;
   moveY = y+1;
}

//*****************************************************************************
// NAME:    COMBATANT::moveForward
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::moveForward()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();
   m_iFacing = FACE_NORTH;
   m_iMoveDir = FACE_NORTH;
   moveX = x;
   moveY = y-1;
}

//*****************************************************************************
// NAME:    COMBATANT::moveBackward
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::moveBackward()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();
   m_iFacing = FACE_SOUTH;
   m_iMoveDir = FACE_SOUTH;
   moveX = x;
   moveY = y+1;
}

//*****************************************************************************
// NAME:    COMBATANT::moveLeft
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::moveLeft()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();

   m_iFacing = FACE_WEST;
   m_iMoveDir = FACE_WEST;
   moveX = x-1;
   moveY = y;
}

//*****************************************************************************
// NAME:    COMBATANT::moveRight
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::moveRight()
{
   StopCasting(FALSE, false);
   State(ICS_Moving);
   //combatData.SetUpdateFlag();

   m_iFacing = FACE_EAST;
   m_iMoveDir = FACE_EAST;
   moveX = x+1;
   moveY = y;
}

//*****************************************************************************
// NAME:    COMBATANT::MoveCombatant
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBATANT::MoveCombatant(int newX, int newY, BOOL allowZeroMoveAttack)
{
  int dude;
  BOOL isUpdate = FALSE;
  MOVE_DATA moveData;
  // if already at destination
  if ((newX == x) && (newY == y))
    return FALSE;

  if (GetType() == MONSTER_TYPE)
  {
    if (GetConfigMonsterNoMove())
      return FALSE;
  }

  moveData.combatantIndex = self;
  moveData.oldX = x;
  moveData.oldY = y;
  moveData.newX = newX;
  moveData.newY = newY;

  PATH_DIR dir = GetDir(x, y, newX, newY);
  int movePoints = GetDist(dir);  // Returns 1 (orthogonal) or 2 (diagonal)
  if (movePoints == 2)
  {
    m_iNumDiagonalMoves++;
    if ((m_iNumDiagonalMoves & 1) == 0)movePoints = 1;
  };
  FaceDirection(dir);

  dude = NO_DUDE;
  if (coordsOnMap(newX, newY, width, height))
  {     
    dude = getCombatantInCell(newX, newY, width, height, self);      
  };

  if (   ( (dude != NO_DUDE) && (allowZeroMoveAttack) )
      || ( m_iMovement < GetAdjMaxMovement(DEFAULT_SPELL_EFFECT_FLAGS, "Move Combatant")-(movePoints-1) )
     )
  {     
    if (coordsOnMap(newX, newY, width, height))
    {     
      if (dude != NO_DUDE)
      {        
        if (canAttack(dude, -1, -1, 0, Distance, false))
        {
          RemoveAllTargets();
          AddTarget(dude, false);
          StartAttack(dude, 0);
//          WriteDebugString("DEBUG - Return to MoveCombatant");
          isUpdate = TRUE;
        }
        /*
        else
        {
          // unable to take step, or to attack
          // dude blocking path, or no attacks remaining
          if (OnAuto())
          {
            // maybe should call Think() here?
            TRACE("Auto combatant %i cannot attack targ in path, end turn + guard\n", self);
            EndTurn(CS_Guarding); // sets state to none, done=true
          }
        }
        */
      }
      // check for walls and such
      else if (OBSTICAL_none == ObsticalType(newX, newY, width, height, FALSE, TRUE, this))
      {        
        // if moving away from an enemy that has no target,
        // the enemy gets to make an attack at this dude's
        // retreating backside.

        int oldX, oldY;

        PlayCombatMove();        
        m_iMovement += movePoints;

        // remove dude from old location
        placeCombatant(x, y, NO_DUDE, width, height);

        TRACE("MoveCombatant() from %u,%u to %u,%u\n",
                    x,y,newX,newY);
      
        if ((abs(newX-x) > 1) || (abs(newY-y) > 1))
          TRACE("Moving more than 1 square\n");

        // place dude at new location
        placeCombatant(newX, newY, self, width, height);

        oldX = x;
        oldY = y;
        x = newX;
        y = newY;

        if (CheckOpponentFreeAttack(oldX, oldY, newX, newY))
        {
          // This guys turn isn't over, but once the free
          // attacks are over, he will not start out in CS_Move.
#ifdef D20140707
          State(ICS_None);  // Should we skip this if OnAuto()?
#else
          State(ICS_None);
#endif
        }

        {
          ActorType actor;
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          GetContext(&actor);
          SetCharContext(&actor);

          //hookParameters[5].Format("%d",dmg);
          //hookParameters[6].Format("%d",IsNonLethal);
          scriptContext.SetCombatantContext(this);


          RunCombatantScripts(
                        ON_STEP,
                        ScriptCallback_LookForChar,
                        "F",
                        "Move Combatant");
          ClearCharContext();
        };

        if (activeSpellList.LingerSpellAffectsTarget(self, x, y, width, height))
        {
          // A lingering spell is active at the location 
          // just moved to. 
          TRACE("%i moved into at least one active linger spell at location %i,%i\n",
                self, x,y);

          activeSpellList.ActivateLingerSpellsOnTarget(self, x, y, width, height);
        }

        // if just moved into square with adjacent guarding
        // enemy, the enemy gets to make an attack
        //if (CheckForGuardingEnemy())
        //{
        //  // This guys turn isn't over, but once the free
        //  // attacks are over, he will not start out in CS_Move.
        //  state = ICS_None;
        //}
        isUpdate = TRUE;
      }      
    }
    else 
    { 
       isUpdate = TRUE;
       // dude fled map
       SetStatus(Fled);
       if (friendly)
         IncNumFriendFlee(); //combatData.numFrndFlee++;
       else
         IncNumMonsterFlee(); //combatData.numMonFlee++;
       EndTurn();
    }
  }

  if ((isUpdate) && (charOnCombatMap(false, true)))
  {    
    ForceMapToMoveAlong(x, y, dir, max(width, height));
    PlaceCursorOnCurrentDude();  
  }
  combatData.CheckAllAuraPlacements(&moveData);
  return isUpdate;
}



//*****************************************************************************
// NAME:    COMBATANT::TeleportCombatant
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL COMBATANT::TeleportCombatant(int newX, int newY)
{
  int dude;
  if (this == NULL) return FALSE;
  BOOL isUpdate = FALSE;

  // if already at destination
  if ((newX == x) && (newY == y))
    return FALSE;


  PATH_DIR dir = GetDir(x, y, newX, newY);
  //int movePoints = GetDist(dir);
  FaceDirection(dir);

//  if (movement < GetAdjMaxMovement()-(movePoints-1))
  {     
    if (coordsOnMap(newX, newY, width, height))
    {     
      dude = getCombatantInCell(newX, newY, width, height, self);      

      if (dude != NO_DUDE)
      { 
        return FALSE;
      //  if (canAttack(dude))
      //  {
      //    AddTarget(dude);
      //    StartAttack(dude);
      //    isUpdate = TRUE;
      //  }
        /*
        else
        {
          // unable to take step, or to attack
          // dude blocking path, or no attacks remaining
          if (OnAuto())
          {
            // maybe should call Think() here?
            TRACE("Auto combatant %i cannot attack targ in path, end turn + guard\n", self);
            EndTurn(CS_Guarding); // sets state to none, done=true
          }
        }
        */
      }
      // check for walls and such
      else if (OBSTICAL_none == ObsticalType(newX, newY, width, height, FALSE, TRUE, this))
      {        
        // if moving away from an enemy that has no target,
        // the enemy gets to make an attack at this dude's
        // retreating backside.

      //  PlayCombatMove();        
      //  movement += movePoints;

        // remove dude from old location
      //  placeCombatant(x, y, NO_DUDE, width, height);

        TRACE("TeleportCombatant() from %u,%u to %u,%u\n",
                    x,y,newX,newY);
      
      //  if ((abs(newX-x) > 1) || (abs(newY-y) > 1))
      //    TRACE("Moving more than 1 square\n");

        x = newX;
        y = newY;

        // place dude at new location
        placeCombatant(x, y, self, width, height);
        //{

        //  SetCharContext(GetContext());

        //  PrepareHooks();
          //hookParameters[5].Format("%d",dmg);
          //hookParameters[6].Format("%d",IsNonLethal);
        //  scriptContext.SetCombatantContext(this);


        //  combatantSA.RunScripts(
        //                ON_STEP,
        //                ScriptCallback_LookForChar,
        //                "F",
        //                "Combatant Move",
        //                "");
        //  scriptContext.Clear();
        //  ClearCharContext();
        //};

        if (activeSpellList.LingerSpellAffectsTarget(self, x, y, width, height))
        {
          // A lingering spell is active at the location 
          // just moved to. 
          TRACE("%i moved into at least one active linger spell at location %i,%i\n",
                self, x,y);

          activeSpellList.ActivateLingerSpellsOnTarget(self, x, y, width, height);
        }

        // if just moved into square with adjacent guarding
        // enemy, the enemy gets to make an attack
        //if (CheckForGuardingEnemy())
        //{
        //  // This guys turn isn't over, but once the free
        //  // attacks are over, he will not start out in CS_Move.
        //  state = ICS_None;
        //}
        isUpdate = TRUE;
      }      
    }
    //else 
    //{ 
    //   isUpdate = TRUE;
    //   // dude fled map
    //   SetStatus(Fled);
    //   if (friendly)
    //     IncNumFriendFlee(); //combatData.numFrndFlee++;
    //   else
    //     IncNumMonsterFlee(); //combatData.numMonFlee++;
    //   EndTurn();
    //}
  }

  //if ((isUpdate) && (charOnCombatMap()))
  //{    
  //  ForceMapToMoveAlong(x, y, dir, max(width, height));
  //  PlaceCursorOnCurrentDude();  
  //}

  return isUpdate;
}



//*****************************************************************************
//    NAME: COMBATANT::CheckForGuardingEnemy
//
// PURPOSE: 
//
//*****************************************************************************
/*
BOOL COMBATANT::CheckForGuardingEnemy()
{
  // check clockwise around current location
  // looking for enemy in guarding state that
  // can make an attack.
  BOOL found = FALSE;
  int dude;
  POSITION pos;
  int d = PathNorth;

  COMBATANT *tempCOMBATANT;
  QueuedCombatantData &qcomb = GetQueuedCombatants();

  while (d <= PathNW)
  {
    FillTargeterList((PATH_DIR)d);
    pos = targeters.GetHeadPosition();
    while (pos != NULL)
    {
      dude = targeters.GetAt(pos);
      tempCOMBATANT = GetCombatantPtr(dude);
      ASSERT(tempCOMBATANT!=NULL);
      if (tempCOMBATANT==NULL) return FALSE;

      if (tempCOMBATANT!=NULL)
      {
        tempCOMBATANT->availAttacks++;
        tempCOMBATANT->NbrAttacks++;
        if (   (tempCOMBATANT->GetIsFriendly() != GetIsFriendly())          
            && (tempCOMBATANT->state == ICS_Guarding)
            && (tempCOMBATANT->canAttack(self)))
        {
          // queue up the attackers
          qcomb.Push(dude, FALSE, TRUE);
          // give self as target to each attacker
          tempCOMBATANT->RemoveAllTargets();
          tempCOMBATANT->AddTarget(self, true);   // Free Attack is true
          // set each attacker in attack mode (if possible)
          tempCOMBATANT->StartAttack(self);
          found = TRUE;
        }
        else
        {
          tempCOMBATANT->availAttacks--;
          tempCOMBATANT->NbrAttacks--;
        };
      }
      targeters.GetNext(pos);
    }
    d++;
  }
  return found;
}

*/
/*
 * Good Morning,
 * 
 * OK.  Here is what I plan.  This email message
 * will be copied verbatim into the C++ code as
 * comments as a reference.
 * Let me know if it seems wrong.
 *
 * What follows here is a kind of "Pseudo-code"
 * (as if it were being done by hook scripts)
 * and is applied to each combatants; PC, NPC,
 * or monster.  I will attempt to code this
 * algorithm in such a way that hooks could
 * be inserted at any of these points in the
 * hard-coded algorithm.  This may take a couple
 * of days and obviously will require more
 * extensive testing because there are so
 * many possibilities.
 * 
 * $VAR AttacksRemaining;
 * $VAR GuardingAttacksRemaining;
 * $VAR IsGuarding;
 * 
 ** [Guarding-CanGuard]
 * $IF (is monster) $RETURN "N";
 * $IF (rangeWeapon) $RETURN "N";
 * $RETURN "Y";
 * 
 * [Guarding-BeginCombat]
 * GuardingAttacksRemaining = 0;
 * IsGuarding = false;
 * 
 * [Guarding-StartOfRound]
 * AttacksRemaining = <number of attacks allowed>;
 * 
 * [Guarding-Guard]  // combatant enters 'guard' mode
 * GuardingAttacksRemaining = AttacksRemaining
 * IsGuarding = true;
 * 
 * [Guarding-StartOfTurn]  // menu presented for combatant
 * IsGuarding = false;
 * GuardingAttacksRemaining = 0;
 * 
 * [Guarding-CanGuardAttack]  // enemy approaches
 * $IF( !IsGuarding) $RETURN "N";
 * $IF (rangeWeapon) $RETURN "N";
 * $IF(GuardAttacksRemaining <=# 0) $RETURN "N";
 * $IF(AttacksRemaining <=#0) $RETURN "N";
 * $RETURN "Y";
 * 
 * [Guarding-MakeAttack]
 * Decrement GuardAttacksRemaining;
 * $IF(GuardAttacksRemaining <=#0) IsGuarding = false;
 * Decrement AttacksRemaining
 * 
 * [FreeAttack-CanFreeAttack]  // enemy departs
 * $IF(rangeWeapon) $RETURN "N";
 * $RETURN "Y";
 * 
 * [FreeAttack-MakeAttack]
 * Decrement AttacksRemaining;
 * Decrement GuardAttacksRemaining;
 * $IF(GuardAttacksRemaining <=#0) IsGuarding = false;
 * 
 * 
 * [Guarding-MakeAttack]
 * Decrement GuardAttacksRemaining;
 * $IF(GuardAttacksRemaining <=#0) IsGuarding = false;
 * Decrement AttacksRemaining
 * 
 * [CanAttack]
 * $IF (IsGuarding) $RETURN "N";
 * $RETURN "Y";
 * 
 * Now I will attempt to apply this algorithm to your
 * examples.
 * 
 * On 10/17/2011 7:30 PM, Eric Cone wrote:
 * ***> Good afternoon.
 * ***>
 * ***> First things first - terminology.
 * ***>
 * ***> Free attack: an attack that follows all the rules of regular melee
 * ***> physical attacks (with or without weapon weapon, if weapon only range of
 * ***> 1 counts as melee)
 * ***> Guarding attack: a type of free attack that occurs when a combatant
 * ***> moves adjacent to a combatant that is GUARDING. As of this time, only
 * ***> combatants under player control may GUARD. The number of GUARD attacks
 * 
 * Handled by $IF (is monster) in [Guarding-CanGuardAttack]
 * 
 * ***> which may occur between a combatants consecutive turns is equal to it's
 * ***> number of attacks per turn.
 * 
 * Handled by:
 * [Guarding-Guard]
 * [Guarding-StartOfTurn]
 * {Guarding-MakeAttack]
 * 
 * ***>
 * ***> The other kind of free attack, occurs when a combatant moves away from
 * ***> an adjacent square, once per monster moving away.
 * ***>
 * ***> With that in mind, let's use Tom's answers to the questions - he was
 * ***> very thorough, and once the vocabulary is straightened out, I completely
 * ***> agree. It is immediately below-
 * ***>
 * ***> -Eric
 * ***>
 * ***> On Sun, Oct 16, 2011 at 7:40 AM, <Nologgie@aol.com
 * ***> <mailto:Nologgie@aol.com>> wrote:
 * ***>
 * ***>     Good Morning Eric,
 * ***>
 * ***>     Here are the answers I came up with for Paul's questions.
 * ***>
 * ***>     If an enemy is immediately adjacent and moves away then he is
 * ***>     subject to a
 * ***>     free attack.
 * ***>
 * ***>     If an enemy is not immediately adjacent and moves so as to become
 * ***>     immediately adjacent he is subject to a 'guarding' attack, but not a
 * ***>     free attack.
 * ***>
 * ***>     The Exact Rules:
 * ***>
 * ***>     Free Attacks
 * ***>
 * ***>     1. The 'free attack' is actually comprised of the attacker's entire
 * ***>     'attack
 * ***>     routine' for a combat round. This is normally 1 or 2 attacks for
 * ***>     characters. For 'monsters', it may be 8 or more.
 * ***>
 * ***>     2. Free attacks will always occur if the target is valid
 * ***>     (IsValidTarget !=
 * ***>     "N") and the attacker is able to execute a melee attack.
 * ***>     (IsCombatReady !=
 * ***>     "N", and a missile weapon is not readied.)
 * 
 * Handled by [FreeAttack-CanFreeAttack]
 * 
 * 
 * ***>
 * ***>     3. If a free attack occurs before the attacker's normal action for the
 * ***>     round, any attacks executed are deducted from the attacks available
 * ***>     for that
 * ***>     round. If the attacks are depleted it constitutes the combatant's
 * ***>     action for
 * ***>     the round, and the combatant gets no 'turn'.
 * 
 * Handled by [FreeAttack-MakeFreeAttack]
 * You did not specify this so I made a guess
 * that a free attack should also reduce the
 * number of guard attacks remaining.
 * 
 * ***>
 * ***>     4. Free attacks are allowed against fleeing enemies regardless of
 * ***>     whether
 * ***>     the attacker has any 'normal' attacks remaining, so in that sense
 * ***>     they are
 * ***>     unlimited.
 * ***>
 * 
 * Handled by [FreeAttack-CanFreeAttack]
 * 
 * 
 * ***>     Guarding Attacks
 * ***>
 * ***>     Combatants who do not have missile weapons readied may select
 * ***>     'Guard' as a
 * ***>     combat action. If a non-adjacent enemy moves adjacent to a guarding
 * 
 * Handled by [Guarding-CanGuard]
 * 
 * ***>     combatant, the guarding combatant will execute its attack routine.
 * ***>     Any attacks made
 * ***>     will be deducted from those available to the attacker in the combat
 * ***>     round
 * ***>     when guard was set. If the attacks are depleted, guarding status is
 * ***>     removed.
 * 
 * Handled by:
 * [MakeAttack]
 * [FreeAttack-MakeFreeAttack]
 * [Guarding-MakeAttack]
 * 
 * ***>
 * ***>     Example:
 * 
 * [StartOfCombat]
 * GuardAttacksRemaining=0;
 * 
 * 
 * ***>     A combatant selects Guard in round three. If an enemy moves adjacent
 * ***>     prior
 * [Guarding-StartOfRound] Round 3
 * AttacksRemaining = 3; // for example
 * [Guarding-Guard] round 3
 * GuardAttacksRemaining = AttacksRemaining = 3;  //for example
 * [Guarding-GuardAttack] round 3
 * Decrement GuardAttacksRemaining (now = 2)
 * ***>     to the guarding combatant's turn in round four, the guarding
 * ***>     combatant will
 * 
 * [GuardIng-StartOfRound] Round 4
 * AttacksRemaining = 3;
 * 
 * ***>     execute its attack routine, and the attacks will be counted against the
 * ***>     attacks available in round three (when Guard was set). The
 * ***>     combatant's round
 * ***>     four turn remains available.
 * 
 * [Guarding-GuardAttack]
 * Decrement AttacksAvailable (now = 3)
 * Decrement GuardAttacksRemaining (now = 1)
 * 
 * The rest of your explanation assumed that
 * 'free attacks' are completely separate from
 * 'guard attacks' whereas my questions assumed
 * they were two different aspects of the same
 * thing.  So the answers did not apply to the
 * intent of my questions.
 * 
 * 
 * PAul
 */


/*  Email added 20120627
*****************************************************************************************
*****************************************************************************************
*****************************************************************************************
*****************************************************************************************
  
The Exact Rules:

Free Attacks

1. The 'free attack' is actually comprised of the attacker's entire 'attack
routine' for a combat round. This is normally 1 or 2 attacks for
characters. For 'monsters', it may be 8 or more.

2. Free attacks will always occur if the target is valid (IsValidTarget !=
"N") and the attacker is able to execute a melee attack. (IsCombatReady !=
"N", and a missile weapon is not readied.)

3. If a free attack occurs before the attacker's normal action for the
round, any attacks executed are deducted from the attacks available for that
round. If the attacks are depleted it constitutes the combatant's action for
the round, and the combatant gets no 'turn'.

4. Free attacks are allowed against fleeing enemies regardless of whether
the attacker has any 'normal' attacks remaining, so in that sense they are
unlimited.

-----

**Who can execute a free attack?
Any combat-ready combatant without a readied missile weapon.

**When can he execute a free attack?
When an adjacent enemy who is a valid target moves away.

**Against whom can he execute a free attack?
Any adjacent enemy who is a valid target and moves away.

**How often can he execute a free attack?
Once each time any adjacent enemy who is a valid target moves away.

**How many free attacks?
A combatant uses all of his available attacks, until either the adjacent enemy is no longer adjacent, the adjacent enemy is dead, or the number of available attacks reaches zero.

**Is there more than one kind of free attack?
No. It only applies to physical melee attacks as described.

**Do the same rules apply to monsters?
**How are they different?
**Do we need a hook to allow more complicated logic.
The same rules apply to monsters.

*************************************************************

Guarding Attacks

Combatants who do not have missile weapons readied may select 'Guard' as a
combat action. If a non-adjacent enemy moves adjacent to a guarding
combatant, the guarding combatant will execute its attack routine. Any attacks made
will be deducted from those available to the attacker in the combat round
when guard was set. If the attacks are depleted, guarding status is removed.
*Note: Fleeing combatants may not guard, this applies to all - PC, NPC and monster; friendly and enemy.

Example:
A combatant selects Guard in round three. If an enemy moves adjacent prior
to the guarding combatant's turn in round four, the guarding combatant will
execute its attack routine, and the attacks will be counted against the
attacks available in round three (when Guard was set). The combatant's round
four turn remains available.

-----

**Who can execture a guarding attack?
Any combatant who matches the "Guarding Attack" criteria above.

**When can he execute a guarding attack?
When a non-adjacent enemy combatant moves adjacent.

**How often can he execute a guarding attack?
Each time a non-adjacent enemy combatant moves to become adjacent, and the combatant has more than zero available guarding attacks.

**How many guarding attacks?
A combatant uses all of his available attacks, until either the adjacent enemy is no longer adjacent, the adjacent enemy is dead, or the number of available attacks reaches zero.

**Is there more than one kind of guarding attack?
No. It only applies to physical melee attacks as described.

**Do the same rules apply to monsters?
**How are they different?
**Do we need a hook to allow more complicated logic.
Yes, monsters follow the same rules.

************************************************************************

$VAR AttacksRemaining;
$VAR GuardingAttacksRemaining;
$VAR IsGuarding;

[Guarding-CanGuard]
$IF (is monster) $RETURN "N";
$IF (rangeWeapon) $RETURN "N";
$RETURN "Y";

[Guarding-BeginCombat]
GuardingAttacksRemaining = 0;
IsGuarding = false;

[Guarding-StartOfRound]
AttacksRemaining = <number of attacks allowed>;

[Guarding-Guard]  // combatant enters 'guard' mode
GuardingAttacksRemaining = AttacksRemaining
IsGuarding = true;

[Guarding-StartOfTurn]  // menu presented for combatant
IsGuarding = false;
GuardingAttacksRemaining = 0;

[Guarding-CanGuardAttack]  // enemy approaches
$IF( !IsGuarding) $RETURN "N";
$IF (rangeWeapon) $RETURN "N";
$IF(GuardAttacksRemaining <=# 0) $RETURN "N";
$IF(AttacksRemaining <=#0) $RETURN "N";
$RETURN "Y";

[Guarding-MakeAttack]
Decrement GuardAttacksRemaining;
$IF(GuardAttacksRemaining <=#0) IsGuarding = false;
Decrement AttacksRemaining

[FreeAttack-CanFreeAttack]  // enemy departs
$IF(rangeWeapon) $RETURN "N";
$RETURN "Y";

[FreeAttack-MakeFreeAttack]
Decrement AttacksRemaining;
Decrement GuardAttacksRemaining;
$IF(GuardAttacksRemaining <=#0) IsGuarding = false;


[MakeAttack]
Decrement GuardAttacksRemaining;
$IF(GuardAttacksRemaining <=#0) IsGuarding = false;
Decrement AttacksRemaining

[CanAttack]
$IF (IsGuarding) $RETURN "N";
$RETURN "Y";

******

*/


int FreeAttackDistance(int,int,int,int,int,int)
{
  return 1;
}


//*****************************************************************************
//    NAME: COMBATANT::CheckOpponentFreeAttack
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::CheckOpponentFreeAttack(int oldX, int oldY, int newX, int newY)
{
  // See the email in the comments immediately above this function

  // More email adde 20120627
  // check clockwise around current location
  // looking for enemy that
  // can make an attack.
  BOOL found = FALSE;
  int maxDude = -1;
  int dude;

  unsigned char wasAdjacent[MAX_COMBATANTS];
  unsigned char willBeAdjacent[MAX_COMBATANTS];
  COMBATANT *tempCOMBATANT;
  QueuedCombatantData *qcomb;
  int dx, dy;
  memset(wasAdjacent, 0, MAX_COMBATANTS);
  memset(willBeAdjacent, 0, MAX_COMBATANTS);


  for (dx=-1; dx<=width; dx++)
  {
    for (dy=-1; dy<=height; dy++)
    {
      if (coordsOnMap(newX+dx, newY+dy, 1, 1))
      {      
        dude = getCombatantInCell(newX+dx, newY+dy, 1, 1, self);
        if (dude != NO_DUDE)
        {
          if (combatData.getCombatantPtr(dude)->charOnCombatMap(false, false))
          {
          if (dude > maxDude) maxDude = dude;
          willBeAdjacent[dude] = 1;  // opponent will be adjacent
        };
      };
    };
  };
  };
  for (dx=-1; dx<=width; dx++)
  {
    for (dy=-1; dy<=height; dy++)
    {
      if (coordsOnMap(oldX+dx, oldY+dy, 1, 1))
      {      
        dude = getCombatantInCell(oldX+dx, oldY+dy, 1, 1, self);
        if (dude != NO_DUDE)
        {
          if (combatData.getCombatantPtr(dude)->charOnCombatMap(false, false))
          {
            if (dude > maxDude) maxDude = dude;
            wasAdjacent[dude] = 1;  // opponent was adjacent
          };
        };
      };
    };
  };



  qcomb = &GetQueuedCombatants();

  // Perform Guard Attacks first.  They will get
  // pushed onto QComb and then the Free Attacks will
  // get pushed on top of them/
  // In that way, the Free Attacks will be performed first.
  for (dude=0; dude<=maxDude; dude++)
  {
    if (willBeAdjacent[dude] && !wasAdjacent[dude])
    {
#ifdef TraceFreeAttacks
      {
        COMBATANT *pOtherCombatant;
        CString direction;
        direction = "Approaches";
        pOtherCombatant = GetCombatantPtr(dude);
        WriteDebugString("TFA - %s %s %s\n", GetName(), direction, pOtherCombatant->GetName());
      };
#endif
      tempCOMBATANT = GetCombatantPtr(dude);
      ASSERT(tempCOMBATANT!=NULL);
      if (tempCOMBATANT!=NULL)
      {
        if (tempCOMBATANT->m_ICS  == ICS_Casting)
        {
          continue;
        };
        if (tempCOMBATANT->GetIsFriendly() != GetIsFriendly())
        {
          if (!combatData.IsValidTarget(tempCOMBATANT, this, NULL))
          {
#ifdef TraceFreeAttacks
            WriteDebugString("'IsValidTarget' script says that this is not a valid target\n");
#endif
            continue;
          };
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          CString result;
          hookParameters[5].Format("%d", int(tempCOMBATANT->AttacksRemaining()));
          hookParameters[6] = tempCOMBATANT->IsPartyMember()?"Y":"N";
          hookParameters[7] = tempCOMBATANT->OnAuto(false)?"Y":"N";
          scriptContext.SetAttackerContext(tempCOMBATANT);
          scriptContext.SetTargetContext(this);

          {
            bool performGuardAttack;
            performGuardAttack = FALSE;
            if (tempCOMBATANT->canAttack(self,
                                         newX, newY, 
                                         tempCOMBATANT->GetNbrAttacks(), 
                                         FreeAttackDistance,
                                         false))
            {
#ifdef TraceFreeAttacks
              WriteDebugString("TFA - Call script 'Guarding-CanGuardAttack(%s,%s,%s)'\n",
                                hookParameters[5],
                                hookParameters[6],
                                hookParameters[7]);
#endif
              result = tempCOMBATANT->RunCombatantScripts(
                              GUARDING_CAN_GUARD_ATTACK,
                              ScriptCallback_RunAllScripts,
                              NULL,
                              "Check opponent free attack");
#ifdef TraceFreeAttacks
              WriteDebugString("TFA - script 'Guarding-CanGuardAttack' returned \"%s\"\n", result);
#endif
              scriptContext.Clear();
              if (!result.IsEmpty())
              {
                if (result[0] == 'Y')
                {
                  performGuardAttack = true;
                };
              };
  
            };
            if (performGuardAttack)
            {
#ifdef TraceFreeAttacks
              WriteDebugString("TFA - queueing %s to make guard attack\n", GetCombatantPtr(dude)->GetName());
#endif
              // Restore my original position until after the opponent has made
              // his free attack.
              // First, check that we are still at the top of the queue.  There may
              // have been other Free Attackers.
              if (qcomb->Top() == self)  // There may be attackers befor this one.
                                         // So we only do this once.
             {
                qcomb->SetXY(newX, newY);
                // remove dude from new location
                placeCombatant(x, y, NO_DUDE, width, height);
                x = oldX;
                y = oldY;
                placeCombatant(x, y, self, width, height); // added 20160927 PRS
              };
              // queue up the attackers
              qcomb->Push(dude, FALSE, 0, 1);
              // give self as target to each attacker
              tempCOMBATANT->turnIsDone = FALSE;
              tempCOMBATANT->RemoveAllTargets();
              tempCOMBATANT->AddTarget(self, true);        
              // set each attacker in attack mode (if possible)
              //tempCOMBATANT->StartAttack(self, qcomb->NumGuardAttacks());
              found = TRUE;
#ifdef TraceFreeAttacks
            }
            else
            {
              WriteDebugString("TFA - No Guard Attack is possible\n");
#endif
            };
          };
#ifdef TraceFreeAttacks
        }
        else
        {
          WriteDebugString("TFA - But they are friends\n");
#endif
        };
      };
    };
  }
  for (dude=0; dude<=maxDude; dude++)
  {
    if (wasAdjacent[dude] && !willBeAdjacent[dude])
    {
#ifdef TraceFreeAttacks
      {
        COMBATANT *pOtherCombatant;
        CString direction;
        direction = "Retreats From";
        pOtherCombatant = GetCombatantPtr(dude);
        WriteDebugString("TFA - %s(%d) %s %s(%d)\n", GetName(), self, direction, pOtherCombatant->GetName(),dude);
      };
#endif
      tempCOMBATANT = GetCombatantPtr(dude);
      ASSERT(tempCOMBATANT!=NULL);
      if (tempCOMBATANT!=NULL)
      {
        if (tempCOMBATANT->m_ICS  == ICS_Casting)
        {
          continue;
        };
        if (tempCOMBATANT->GetIsFriendly() != GetIsFriendly())
        {
          if (!combatData.IsValidTarget(tempCOMBATANT, this, NULL))
          {
#ifdef TraceFreeAttacks
            WriteDebugString("'IsValidTarget' script says that this is not a valid target\n");
#endif
            continue;
          };
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          CString result;
          hookParameters[5].Format("%d", int(tempCOMBATANT->AttacksRemaining()));
          hookParameters[6] = tempCOMBATANT->IsPartyMember()?"Y":"N";
          hookParameters[7] = tempCOMBATANT->OnAuto(false)?"Y":"N";
          hookParameters[8].Format("%d", int(tempCOMBATANT->GetNbrAttacks()));
          scriptContext.SetAttackerContext(tempCOMBATANT);
          scriptContext.SetTargetContext(this);

          {
  
            int freeAttackCount;
            freeAttackCount = 0;
            if (tempCOMBATANT->canAttack(self, 
                                         oldX, oldY, 
                                         tempCOMBATANT->m_pCharacter->GetNbrAttacks(), 
                                         FreeAttackDistance, 
                                         false))
            {
#ifdef TraceFreeAttacks
              WriteDebugString("TFA - Call script 'FreeAttack-CanFreeAttack(%s,%s,%s,%s)\n", 
                                hookParameters[5],
                                hookParameters[6],
                                hookParameters[7],
                                hookParameters[8]);
#endif
              result = tempCOMBATANT->RunCombatantScripts(
                              FREEATTACK_CAN_FREE_ATTACK,
                              ScriptCallback_RunAllScripts,
                              NULL,
                              "Can opponent free attack");
#ifdef TraceFreeAttacks
              WriteDebugString("TFA - Script 'FreeAttack-CanFreeAttack' returned \"%s\"\n", 
                                result);
#endif
              scriptContext.Clear();
              if (!result.IsEmpty())
              {
                if (result[0] == 'Y')
                {
                  freeAttackCount = 1 + tempCOMBATANT->AttacksRemaining();
                }
                else
                {
                  int n;
                  n = atoi(result);
                  if (n > 0)
                  {
                    freeAttackCount = n;
                  };
                };
              };
            };
            if (freeAttackCount)
            {               
#ifdef TraceFreeAttacks
              WriteDebugString("TFA - queueing %s(%d) to make free attack\n", GetCombatantPtr(dude)->GetName(), dude);
#endif
              // queue up the attackers
              if (qcomb->Top() == self) // There may be several Free Attackers before this one!
                                        // So we only do this once.
              {
                qcomb->SetXY(newX, newY);
                // remove dude from new location
                placeCombatant(x, y, NO_DUDE, width, height);
                x = oldX;
                y = oldY;
                // put dude into original location
                placeCombatant(x, y, self, width, height); // added 20160927 PRS
              };
              qcomb->Push(dude, FALSE, freeAttackCount, 0);
              // give self as target to each attacker
              tempCOMBATANT->turnIsDone = FALSE;
              tempCOMBATANT->RemoveAllTargets();
              tempCOMBATANT->AddTarget(self, true);        
              // set each attacker in attack mode (if possible)
              //tempCOMBATANT->StartAttack(self, qcomb->NumFreeAttacks());
              found = TRUE;
#ifdef TraceFreeAttacks
            }
            else
            {
              WriteDebugString("TFA - No Free Attack is possible\n");
#endif
            };
          };
        };
      };
    };
  }
  return found;
}




//*****************************************************************************
//    NAME: COMBATANT::FillTargeterList
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::FillTargeterList(PATH_DIR dir)
{
  targeters.RemoveAll();
  int i;
  int dude;

  switch (dir)
  {
  case PathNorth:
    for (i=x; i < x+width; i++)
    {
      if (coordsOnMap(i, y-1, 1, 1))
      {      
        if ((dude = getCombatantInCell(i, y-1, 1, 1, self)) != NO_DUDE)
          targeters.AddTail(dude);
      }
    }
    break;
  case PathNE:
    if (coordsOnMap(x+width, y-1, 1, 1))
    {      
      if ((dude = getCombatantInCell(x+width, y-1, 1, 1, self)) != NO_DUDE)
        targeters.AddTail(dude);
    }
    break;

  case PathEast:
    for (i=y; i < y+height; i++)
    {
      if (coordsOnMap(x+width, i, 1, 1))
      {      
        if ((dude = getCombatantInCell(x+width, i, 1, 1, self)) != NO_DUDE)
          targeters.AddTail(dude);
      }
    }
    break;
  case PathSE:
    if (coordsOnMap(x+width, y+height, 1, 1))
    {      
      if ((dude = getCombatantInCell(x+width, y+height, 1, 1, self)) != NO_DUDE)
        targeters.AddTail(dude);
    }
    break;
  case PathSouth:
    for (i=x; i < x+width; i++)
    {
      if (coordsOnMap(i, y+height, 1, 1))
      {      
        if ((dude = getCombatantInCell(i, y+height, 1, 1, self)) != NO_DUDE)
          targeters.AddTail(dude);
      }
    }
    break;
  case PathSW:
    if (coordsOnMap(x-1, y+height, 1, 1))
    {      
      if ((dude = getCombatantInCell(x-1, y+height, 1, 1, self)) != NO_DUDE)
        targeters.AddTail(dude);
    }
    break;
  case PathWest:
    for (i=y; i < y+height; i++)
    {
      if (coordsOnMap(x-1, i, 1, 1))
      {      
        if ((dude = getCombatantInCell(x-1, i, 1, 1, self)) != NO_DUDE)
          targeters.AddTail(dude);
      }
    }
    break;
  case PathNW:
    if (coordsOnMap(x-1, y-1, 1, 1))
    {      
      if ((dude = getCombatantInCell(x-1, y-1, 1, 1, self)) != NO_DUDE)
        targeters.AddTail(dude);
    }
    break;
  default:
    break;
  }
}
//*****************************************************************************
// NAME: charOnCombatMap
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::charOnCombatMap(bool unconsciousOK, bool petrifiedOK) const
{
  charStatusType stype = GetAdjStatus();
   if (   (charUnconscious() && !unconsciousOK)
       || (charPetrified() && !petrifiedOK)
       || (stype == Fled)
       || (stype == Gone)
       || (stype == TempGone)
       || (stype == Dead))
     return FALSE;

  return TRUE;
}

//*****************************************************************************
//    NAME: COMBATANT::charUnconscious
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::charUnconscious() const
{
  charStatusType stype = GetAdjStatus();
  if ((stype == Dying) || (stype == Unconscious)) return TRUE;
  return FALSE;
}

//*****************************************************************************
//    NAME: COMBATANT::charPetrified
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::charPetrified() const
{
  charStatusType stype = GetAdjStatus();
  if (stype == Petrified) return TRUE;
  return FALSE;
}

//*****************************************************************************
//    NAME: COMBATANT::charCanTakeAction
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::charCanTakeAction()
{
  charStatusType stype = GetAdjStatus();
   if ((stype == Okay) || (stype == Running) || (stype == Animated))
     return TRUE;
  return FALSE;  
}

//*****************************************************************************
// NAME:    COMBATANT::RestoreToParty
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::RestoreToParty()
{
  if (!IsPartyMember())
#ifdef newCombatant
  {
    return;
  };
#else
    return;
#endif

  if (   ((GetType() == CHAR_TYPE) || (GetType() == NPC_TYPE))
      && (origKey >= 0))
  {     
    // need to restore original morale to characters
    // so that they begin each battle with normal morale.
#ifdef newCombatant
    if (&party.characters[origKey] != m_pCharacter)
    {
      die(0xdabcd);
    };
#endif

#ifdef newCombatant
    int origMorale = m_preCombatMorale;
#else
    int origMorale = party.characters[origKey].GetMorale();
#endif

    // unless of course if the NPC has decided to leave
    // the party
#ifdef newCombatant
    if ((GetType() == NPC_TYPE) && (!m_pCharacter->IsPreGen) && (GetMorale() <= 0)) 
#else
    if ((GetType() == NPC_TYPE) && (!IsPreGen) && (GetMorale() <= 0)) 
#endif
      origMorale = 0;

    // if PartyNeverDies flag is set and dude has 0 or less hp,
    // restore to original hp with Okay status
    if (GetConfigPartyNeverDies())
    {
      SetStatus(party.characters[origKey].GetStatus());
      if (GetHitPoints() <= 0) SetHitPoints(1);
    }
    //party.characters[origKey] = *((CHARACTER*)(this));
    party.characters[origKey].SetMorale(origMorale);
    
    // if dying when combat ends (between -10 and 0 hitpoints)
    // set to 0 hp and unconscious status
    if (   (party.characters[origKey].GetStatus() == Dying)
        || (party.characters[origKey].GetStatus() == Unconscious))
    {
      party.characters[origKey].SetStatus(Unconscious);
      if (GetHitPoints() < 0) 
        SetHitPoints(0);
    }

    // spells may have altered control status
    //20140915 PRS party.characters[origKey].SetAllowPlayerControl((GetType()==CHAR_TYPE) || (m_pCharacter->IsPreGen)); 
    party.characters[origKey].SetAllowPlayerControl(true); 
  }
}


//*****************************************************************************
// NAME: COMBATANT::DetermineInitialFacing
//
// PURPOSE:  
//*****************************************************************************
int COMBATANT::DetermineInitialFacing(int PartyFacing)
{   
  switch (PartyFacing)
  {
  case FACE_NORTH:
  case FACE_SOUTH:
    if (RollDice(100, 1, 0) <= 50) 
      return FACE_EAST;
    else 
      return FACE_WEST;

  case FACE_EAST:
    return FACE_EAST;
  case FACE_WEST:
    return FACE_WEST;
  }
  return FACE_EAST;
}

//*****************************************************************************
// NAME:    COMBATANT::InitFromCharData
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::InitFromCharData(int dude)
{
  if (m_pCharacter!= NULL)
  {
    die(0x6659c);
  };
  m_pCharacter = &party.characters[dude];
  if (m_pCharacter->m_pCombatant != NULL)
  {
    die(0x5cbff);
  };
  m_pCharacter->m_pCombatant = this;
  m_preCombatMorale = m_pCharacter->GetMorale();

  SetPartyMember(TRUE);
  State(ICS_None);
  origKey = dude;
  //WriteDebugString("add 1: origIndex %i, origKey %i\n", origIndex, origKey);
  m_iMovement = 0;
  m_iNumDiagonalMoves = 0;

  friendly = TRUE;
  m_adjFriendly = 0;
  // NPC in party cannot be controlled in combat unless it is a 
  // pre-generated character
  //20140915 PRS SetAllowPlayerControl( (GetType()==CHAR_TYPE) || (m_pCharacter->IsPreGen) );
  SetAllowPlayerControl(true);
  m_iFacing = DetermineInitialFacing(party.facing);
  m_iMoveDir = m_iFacing;

  determineNbrAttacks(); // lookup max possible defaults
  if (GetAdjAutomatic())
  {
    ReadyBestArmor();
    ReadyBestWpn(NO_DUDE);
    ReadyBestShield();  
  }
  determineNbrAttacks();
  determineAvailAttacks(GetNbrAttacks()); // now take ready wpn into account
  //WriteDebugString("add 2: origIndex %i, origKey %i\n", origIndex, origKey);
}

//*****************************************************************************
// NAME:    COMBATANT::InitFromMonsterData
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::InitFromMonsterData(const MONSTER_ID& monsterID,
                                    BOOL IsFriendly, 
                                    const ITEM_LIST &items, 
                                    const MONEY_SACK &msack)
{
  const MONSTER_DATA *pMonster;
  pMonster = monsterData.PeekMonster(monsterID);
  TRACE("%d Afet PeekMonster\n", timeGetTime());
  if (pMonster == NULL)
  {
    WriteDebugString("Cannot find data for monster %s\n", monsterID.UniqueName());
    pMonster = monsterData.PeekMonster(0);
    if (pMonster == NULL)
    {
      WriteDebugString("Combat with no monsters defined");
      die(0x45acb);
    };
  }
  InitFromMonsterData(pMonster, IsFriendly, items, msack);
}


//void COMBATANT::InitFromMonsterData(int dude, BOOL IsFriendly, ITEM_LIST &items, MONEY_SACK &msack)
void COMBATANT::InitFromMonsterData(const MONSTER_DATA *pMonster, BOOL IsFriendly, const ITEM_LIST &items, const MONEY_SACK &msack)
{
  //MONSTER_DATA *data;
  //const MONSTER_DATA *pMonster;
  if (m_pCharacter != NULL)
  {
    die(0xc33bd);
  };
  m_pCharacter = new CHARACTER;
  m_pCharacter->m_pCombatant = this;  // Link the character and combatant one to another.
  deleteChar = TRUE;

  State(ICS_None);
  //if ((data = monsterData.GetMonsterData(dude)) == NULL)
  if (pMonster == NULL)
  {
    //WriteDebugString("Bogus monster index %u in InitFromMonsterData()\n", dude);    
    WriteDebugString("Bogus monster index in InitFromMonsterData()\n");    
    Clear();
    origKey = NO_DUDE;
    return;
  }

  //partyMember = FALSE;
  SetPartyMember(FALSE);
  SetType(MONSTER_TYPE);
  //origKey =  dude;
  //m_pCharacter->origIndex = dude;
  m_pCharacter->monsterID = pMonster->MonsterID();
  origKey = monsterData.LocateMonster(m_pCharacter->monsterID);
  m_pCharacter->classID = pMonster->classID;
  m_pCharacter->race = pMonster->raceID;
  friendly = IsFriendly; 
  m_adjFriendly = 0;

  //generateNewCharacter(data->XP_Value, START_EXP_VALUE); // determines monster AC,THAC0,Hit Points,etc
  generateNewCharacter(pMonster->XP_Value, START_EXP_VALUE); // determines monster AC,THAC0,Hit Points,etc

  TRACE("%d After generateNewCharacter\n", timeGetTime());
  SetStatus(Okay);
  //SetAutomatic(!combatData.MonsterPlyrControl);
  SetAllowPlayerControl(GetConfigMonsterPlyrControl());
  //m_pCharacter->SetName(data->Name);
  //m_pCharacter->SetInt(data->Intelligence);
  //m_pCharacter->SetMagicResistance(data->Magic_Resistance);
  //m_pCharacter->SetSize(data->Size);
  //SetMorale(data->Morale);  
  //m_pCharacter->ItemMask = data->ItemMask;
  //specAbs.Copy(data->specAbs);  
  //m_pCharacter->SetUndead(data->undeadStatus);
  m_pCharacter->SetName(pMonster->Name);
  m_pCharacter->SetPermInt(pMonster->Intelligence);
  m_pCharacter->SetMagicResistance(pMonster->Magic_Resistance);
  m_pCharacter->SetSize(pMonster->Size);
  SetMorale(pMonster->Morale);  
  //m_pCharacter->ItemMask = pMonster->ItemMask;
  //specAbs.Copy(data->specAbs);  
  //m_pCharacter->SetUndead(pMonster->undeadStatus);
  m_pCharacter->SetUndead(pMonster->undeadType);

  // items specified in combat event for this monster
  m_pCharacter->myItems = items;

  // default monster items in monster database
  //POSITION pos = data->myItems.GetHeadPosition();
  POSITION pos = pMonster->myItems.GetHeadPosition();
  while (pos!=NULL)
  {
    //m_pCharacter->myItems.AddItem(data->myItems.GetNext(pos));
    m_pCharacter->myItems.AddItem(const_cast<MONSTER_DATA *>(pMonster)->myItems.GetNext(pos));
  }

  m_pCharacter->money = msack; // combat additional money
  //m_pCharacter->money.Add(data->money); // monster default money
  m_pCharacter->money.Add(pMonster->money); // monster default money

  //m_pCharacter->icon = data->Icon;
  m_pCharacter->icon = pMonster->Icon;

  if (false)  // This was done in generateNewCharacter
  {
    determineNbrAttacks();
  };

  ReadyBestArmor();
  ReadyBestWpn(NO_DUDE);
  ReadyBestShield();  
  determineNbrAttacks(); // Take armor and such into account.
  determineAvailAttacks(GetNbrAttacks());
  determineMaxMovement();

  if (GetConfigMonsterNoMove())
  {
    SetMaxMovement(0);
    m_iMovement = 0;
    m_iNumDiagonalMoves = 0;
  }

  ASSERT(GetAdjHitPoints() > 0);


  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetCharacterContext(this->m_pCharacter);
    //scriptContext.SetMonsterTypeContext(data);
    scriptContext.SetMonsterTypeContext(pMonster);
    RunGlobalScript("Global_CreateMonsterCombatant",CREATE_MONSTER_COMBATANT, true);
    //data->specAbs.RunScripts(CREATE_MONSTER_COMBATANT, ScriptCallback_RunAllScripts, NULL, data->Name, "");
    pMonster->RunMonsterScripts(
          CREATE_MONSTER_COMBATANT, 
          ScriptCallback_RunAllScripts,
          NULL,
          "Initializing monster for combat");
  }
}

//*****************************************************************************
// NAME: COMBATANT::InitFromNPCData
//
// PURPOSE:  
//*****************************************************************************
//void COMBATANT::InitFromNPCData(int dude, BOOL IsFriendly, ITEM_LIST &items, MONEY_SACK &msack)
void COMBATANT::InitFromNPCData(const CHARACTER_ID& characterID, BOOL IsFriendly, const ITEM_LIST &items, const MONEY_SACK &msack)
{

  if (m_pCharacter != NULL)
  {
    die (0x3ca1fe);
  };
  {
    CHARACTER *pNewCharacter;
    pNewCharacter = new CHARACTER;
    //if (!globalData.charData.GetCharacterData(dude, *pNewCharacter))
    //if (!globalData.charData.FetchCharacterData(characterID, pNewCharacter))
    if (!globalData.charData.FetchCharacter(characterID, pNewCharacter))
    {
      //WriteDebugString("invalid NPC combatant  %i in InitFromNPCData()\n", dude);
      WriteDebugString("invalid NPC combatant  %s in InitFromNPCData()\n", characterID.UniqueName());
      Clear();
      origKey = NO_DUDE;
      delete pNewCharacter;
      return;
    };
    m_pCharacter = pNewCharacter;
    origKey = globalData.charData.LocateCharacter(characterID);
    m_pCharacter->m_pCombatant = this;
  };


  POSITION pos = items.GetHeadPosition();
  while (pos != NULL)
    //m_pCharacter->myItems.AddItem(items.GetNext(pos));
    //m_pCharacter->myItems.AddItem(*items.PeekNext(pos));
    m_pCharacter->myItems.AddItem(*const_cast<ITEM *>(items.PeekNext(pos)));

  m_pCharacter->money = msack;
  //partyMember = FALSE;
  SetPartyMember(FALSE);
  State(ICS_None);
  //origKey = dude;
  m_pCharacter->characterID = characterID;
  SetType(NPC_TYPE);
  friendly = IsFriendly;
  m_adjFriendly = 0;
  m_iMovement = 0;
  m_iNumDiagonalMoves = 0;
  SetStatus(Okay);
  //SetAutomatic(!combatData.MonsterPlyrControl);
  SetAllowPlayerControl(GetConfigMonsterPlyrControl());
  determineNbrAttacks();
  ReadyBestArmor();
  ReadyBestWpn(NO_DUDE);
  ReadyBestShield();  
  determineNbrAttacks(); // Take Armor and such into account.
  determineAvailAttacks(GetNbrAttacks());
  determineMaxMovement();

  if (GetConfigMonsterNoMove())
  {
    SetMaxMovement(0);
    m_iMovement = 0;
    m_iNumDiagonalMoves = 0;
  };
  int kkk=1;
  m_pCharacter->monsterID = m_pCharacter->characterID;
  kkk=1;
}

//*****************************************************************************
// NAME: determineAvailAttacks
//
// PURPOSE:  
//*****************************************************************************
void COMBATANT::determineAvailAttacks(int numberOfAttacks)
{
  //determineNbrAttacks(); // lookup max attacks
  //availAttacks = GetNbrAttacks();
  availAttacks = numberOfAttacks;

  float temp = availAttacks;
  DWORD t = timeGetTime();
  TRACE("%d determineAvailAttacks\n", t);
  TRACE("Setting availAttacks for %i to %f\n", self, temp);     
}

//*****************************************************************************
// NAME: isLargeDude
//
// PURPOSE:  
//*****************************************************************************
BOOL COMBATANT::isLargeDude() const
{
  creatureSizeType ctype = GetAdjSize();

  if (ctype == Large) return TRUE;
  if ((width > 1) || (height > 1)) return TRUE;
  if (IsAlwaysLarge()) return TRUE;
  
  return FALSE;
}

//*****************************************************************************
// NAME:    COMBATANT::RollInitiative
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void COMBATANT::RollInitiative(eventSurpriseType eSurprise)
{
  BOOL partymember = (IsPartyMember() || friendly);
  switch (eSurprise)
  {
  case Neither:
    m_iInitiative = RollDice(INITIATIVE_LastDefault-INITIATIVE_FirstDefault+1,1,INITIATIVE_FirstDefault-1);
    //if (GetAdjSpecAb(SA_Blindness))
    //  initiative += 2;
    break;
  case PartySurprised: // monsters go first
    m_iInitiative = (partymember ? INITIATIVE_LastDefault : INITIATIVE_FirstDefault);
    break;
  case MonsterSurprised: // party members go first
    m_iInitiative = (partymember ? INITIATIVE_FirstDefault : INITIATIVE_LastDefault);
    break;
  }

  if (m_iInitiative < INITIATIVE_FirstDefault) m_iInitiative = INITIATIVE_FirstDefault;
  if (m_iInitiative > INITIATIVE_LastDefault) m_iInitiative = INITIATIVE_LastDefault;
}

//*****************************************************************************
//    NAME: COMBATANT::GetCenterX
//
// PURPOSE: 
//
//*****************************************************************************
int COMBATANT::GetCenterX()
{
  if (width<=1) return x;
  if (m_iFacing == FACE_WEST)
    return (x+(width/2)-1);
  else
    return (x+(width/2));
}

//*****************************************************************************
//    NAME: COMBATANT::GetCenterY
//
// PURPOSE: 
//
//*****************************************************************************
int COMBATANT::GetCenterY()
{
  if (height<=1) return y;
  return (y+(height/2)-1);
}

//*****************************************************************************
//    NAME: COMBATANT::OnStartCombatantAction
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::OnStartCombatantAction()
{
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  ActorType actor;
  TRACE("OnStartCombatantAction for %i\n", self);
  ClearQueuedSpecAb();
  PlaceCursorOnCurrentDude();
  
  CombatMsg="";
  ClearFormattedText(combatTextData);
  CString tmp;


  GetContext(&actor);
  SetCharContext(&actor);
  scriptContext.SetCharacterContext(this->m_pCharacter);
  scriptContext.SetCombatantContext(this);
  /*  We moved this to 'DisplayCombatStats' so that targets
   *  would have their stats displayed.   PRS 20110111
  specAbs.RunScripts(CHAR_COMBAT_STATUS, ScriptCallback_CharCombatStatus, &CombatMsg, name, "");
  combatantSA.RunScripts(CHAR_COMBAT_STATUS, ScriptCallback_CharCombatStatus, &CombatMsg, name, "");
  ClearHookParameters();
  */
  RunCombatantScripts(
        ON_START_COMBATANT_ACTION, 
        ScriptCallback_RunAllScripts, 
        NULL, 
        "Start Combatant Action");
  ClearCharContext();


  /*
  
  if (GetAdjSpecAb(SA_Curse))
  {
    tmp.Format(" \nCursed");
    CombatMsg += tmp;
  }
  if (GetAdjSpecAb(SA_CharmPerson))
  {
    tmp.Format(" \nCharmed");
    CombatMsg += tmp;
  }
  if (GetAdjSpecAb(SA_Sleep))
  {
    tmp.Format(" \nSleeping");
    CombatMsg += tmp;
  }
  if (GetAdjSpecAb(SA_Entangle))
  {
    tmp.Format(" \nEntangled");
    CombatMsg += tmp;
  }
  if (GetAdjSpecAb(SA_HoldPerson))
  {
    tmp.Format(" \nHeld");
    CombatMsg += tmp;
  }
  if (GetAdjSpecAb(SA_Silence))
  {
    tmp.Format(" \nSilenced");
    CombatMsg += tmp;
  }
  if (GetAdjSpecAb(SA_Poisoned))
  {
    tmp.Format(" \nPoisoned");
    CombatMsg += tmp;
  }
  if (GetAdjSpecAb(SA_Blindness))
  {
    tmp.Format(" \nBlinded");
    CombatMsg += tmp;
  }
  */

  /*  We moved this to 'DisplayCombatStats' so that targets
   *  would have their stats displayed.   PRS 20110111
   *if (!CombatMsg.IsEmpty())
   *{
   *  FormatCombatText(combatTextData, CombatMsg);
   *  UpdateCombatTimeDelayMsg();
   *  theApp.AppSleep(GetMsgTimeDelay());
   *  //ClearFormattedText(combatTextData);
   *}
   */
}


//*****************************************************************************
//    NAME: COMBATANT::OnAuto
//
// PURPOSE: Should the Engine control the combatant?
//
//*****************************************************************************
BOOL COMBATANT::OnAuto(bool callAutoActionHook)
{ // We want to call the hook only at those points in
  // the combat where it makes sense for 'auto action' to have changed.
  // Otherwise we call the hook hundreds of times (for example from OnIdle).

  if (callAutoActionHook)
  {
    ActorType actor;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    GetContext(&actor);
    SetCharContext(&actor);
    scriptContext.SetCombatantContext(this);

#ifdef newCombatant
    RunCombatantScripts(AUTO_ACTION,
                           ScriptCallback_RunAllScripts, 
                           NULL,
                           "Combatant Auto-action may have changed");
#else
    combatantSA.RunScripts(AUTO_ACTION, ScriptCallback_RunAllScripts, NULL, "OnAuto", name);
#endif
    ClearCharContext();
    //if (hookParameters[0].IsEmpty()) return FALSE;
  
  
   

    while (!hookParameters[0].IsEmpty())
    {
      int col;
      iFleeingFlags &= ~fleeAutoActionScript;
      iAutoFlags &= ~forceAutoScript;
      iAutoFlags &= ~forcePlayerScript;
      switch(hookParameters[0][0])
      {
      case 'F': 
        // We probably need to clear 'fleeAutoActionScript' when we 'GetNextCombatant'
        // so that the fleeing can be terminated when a spell effect ends.
        iFleeingFlags |= fleeAutoActionScript;
        if (hookParameters[0].GetLength() > 1)
        {
          int attacker;
          if (sscanf((LPCSTR)hookParameters[0] + 1, "%d", &attacker) == 1)
          {
            if ((attacker >= 0) && (attacker < combatData.NumCombatants()) )
            {
              if (attacker != this->self)
              {
                this->m_iLastAttacker = attacker;
              };
            };
          };
        };
        break;
      case 'C':
        if (hookParameters[0].GetLength() > 1)
        {
          if (hookParameters[0][1] == 'P') iAutoFlags |= forcePlayerScript;
          if (hookParameters[0][1] == 'A') iAutoFlags |= forceAutoScript;
        };
        break;
      };
      col = hookParameters[0].Find(',');
      if (col < 0) hookParameters[0] = "";
      else hookParameters[0] = hookParameters[0].Right(hookParameters[0].GetLength()-col);
    };
  };
  
  
  if (iFleeingFlags != 0) return TRUE;
  if (iAutoFlags & forceAutoScript) return TRUE;
  if (iAutoFlags & forcePlayerScript) return FALSE;
  
  if (    (friendly && (m_adjFriendly==2))
      ||  (m_adjFriendly == 3)
     )
  {
    return TRUE;
  };
  

  if (GetAdjAutomatic())
  {
    return TRUE; 
  };

  return FALSE;

}

//*****************************************************************************
//    NAME: COMBATANT::OnEndTurn
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::OnEndTurn()
{
  TRACE("OnEndTurn for %i\n", self);
}

//*****************************************************************************
//    NAME: COMBATANT::OnEnsureVisible
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::OnEnsureVisible()
{
  TRACE("OnEnsureVisible for %i\n", self);
}

//*****************************************************************************
//    NAME: COMBATANT::FormatSpecAbMsg
//
// PURPOSE: 
//
//*****************************************************************************
CString COMBATANT::FormatSpecAbMsg(DWORD sa_state)
{
  if (sa_state == SPECAB_MSG_NONE) return "";
  if (m_pCharacter->GetSpecAbQueueCount()==0) return "";

  CString msg("");

  BOOL found=FALSE;
  POSITION pos = m_pCharacter->GetSpecAbQueueHeadPosition();
  while (pos != NULL)
  {
    SQueuedSpecAb data = m_pCharacter->GetSpecAbQueueAt(pos);
    specialAbilitiesType stype = (specialAbilitiesType)(data.sa);

    const SPECIAL_ABILITIES *pSpecAbSrc=NULL;
    switch (data.src)
    {
    case SPELL_EFFECTS_DATA::EFFECT_ITEMSPECAB:
      {
        //GLOBAL_ITEM_ID giID;
        ITEM_ID itemID;
        //giID = data.SourceItem_ID();
        itemID = data.SourceItem_ID();
        //ITEM_DATA *pItem = itemData.GetItemData(giID);
        ITEM_DATA *pItem = itemData.GetItem(itemID);
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
        //if (data.SourceChar_ID()==self)
        //  pSpecAbSrc=&m_pCharacter->specAbs;
        /* Really */ NotImplemented(0x4cbbc, false);
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
          m_pCharacter->RemoveSpecAbQueueAt(pos);
          found=TRUE;
          break;
        }
      }
    }
    m_pCharacter->GetSpecAbQueueNext(pos);
  }

  if (!found) return "";

  CString User("");
  CString Target("");

  User = GetName();
  
  int targ = m_iLastAttacked;//GetCurrTarget(); ** targ may be dead by this time
  if (targ != NO_DUDE)
  {
    COMBATANT *tempCOMBATANT=GetCombatantPtr(targ);
    if (tempCOMBATANT!=NULL)
      Target = tempCOMBATANT->GetName();
  }
  else
  {
    Target="the enemy";
  }

  msg.Replace("/u", User);
  msg.Replace("/t", Target);
  return msg;
}

//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackRollDice
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackRollDice(const CHARACTER *pTarget,const int num, const int sides, int *pBonus) const
{
  return m_pCharacter->ModifyAttackRollDice(pTarget, num, sides, pBonus);
}
//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackDamageDice
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackDamageDice(const CHARACTER *pTarget,const int num, const int sides, int *pBonus, BOOL *pNonLethal) const
{
  return m_pCharacter->ModifyAttackDamageDice(pTarget, num, sides, pBonus, pNonLethal);
}
//*****************************************************************************
//    NAME: COMBATANT::ModifyACAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL COMBATANT::ModifyACAsTarget(const CHARACTER *pAttacker, int *pAC, GLOBAL_ITEM_ID giID) const
BOOL COMBATANT::ModifyACAsTarget(const CHARACTER *pAttacker, int *pAC, const ITEM_ID& itemID) const
{
  DWORD src;
  //GLOBAL_SPELL_ID skey;
  SPELL_ID spellID;
  //BOOL modify = m_pCharacter->ModifyACAsTarget(pAttacker, pAC, giID);
  BOOL modify = m_pCharacter->ModifyACAsTarget(pAttacker, pAC, itemID);

  //if (GetAdjSpecAb(SA_Displacement,&src, &skey))
  if (GetAdjSpecAb(SA_Displacement,&src, &spellID))
  {
    COMBATANT *pa = pAttacker->m_pCombatant;
    //m_pCharacter->QueueUsedSpecAb(SA_Displacement,src,skey);
    m_pCharacter->QueueUsedSpecAb(SA_Displacement,src,spellID);
    modify=TRUE;
    if (pa->self != m_iLastAttacker) // if first attack on me
      *pAC = 21; // force a miss
    else
      (*pAC) -= 2; // +2 bonus to armor class
  }
  
  //if (GetAdjSpecAb(SA_MirrorImage,&src, &skey))
  if (GetAdjSpecAb(SA_MirrorImage,&src, &spellID))
  {
    //m_pCharacter->QueueUsedSpecAb(SA_MirrorImage,src,skey);
    m_pCharacter->QueueUsedSpecAb(SA_MirrorImage,src,spellID);
    // decrement counter used to keep track of number of mirror images
    //AdjustEffectData("$CHAR_MIRRORIMAGE", -1);
    modify=TRUE;
    *pAC = 21; // force a miss
  }
/*
  if (GetAdjSpecAb(SA_Blindness,&src, &skey))
  {
    QueueUsedSpecAb(SA_Blindness,src,skey);
    modify=TRUE;
    (*pAC) += 4;
  }
*/  

  if (pAttacker->HasDwarfACPenalty())
  {
    //if (race() == Dwarf)
    if (raceData.HasDwarfResistance(race()))
    {
      (*pAC) -= 4; // +4 bonus to armor class
      modify=TRUE;
    }
  };
  if (pAttacker->HasGnomeACPenalty())
  {
    //if (race() == Gnome)
    if (raceData.HasGnomeResistance(race()))
    {
      (*pAC) -= 4; // +4 bonus to armor class    
      modify=TRUE;
    }
  };
  return modify;
}
//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackThac0
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackThac0(const CHARACTER *pTarget,int *pVal) const
{
  CString spellName;
  BOOL modify = m_pCharacter->ModifyAttackThac0AsTarget(pTarget, pVal);

  if (pTarget->HasDwarfTHAC0Penalty())
  {
    if (raceData.HasDwarfResistance(race()))
    {
      (*pVal) -= 1; // +1 bonus to THAC0
      modify=TRUE;
    }
  };
  if (pTarget->HasGnomeTHAC0Penalty())
  {
    if (raceData.HasGnomeResistance(race()))
    {
      (*pVal) -= 1; // +1 bonus to THAC0
      modify=TRUE;
    }
  };
  return modify;
}
//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackRollDiceAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackRollDiceAsTarget(const CHARACTER *pAttacker,const int num, const int sides, int *pBonus) const
{
#ifdef newCombatant
  return m_pCharacter->ModifyAttackRollDiceAsTarget(pAttacker, num, sides, pBonus);
#else
  return CHARACTER::ModifyAttackRollDiceAsTarget(pAttacker, num, sides, pBonus);
#endif
}
//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackDamageDiceAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackDamageDiceAsTarget(const CHARACTER *pAttacker, const int num, const int sides, int *pBonus, BOOL *pNonLethal) const
{
#ifdef newCombatant
  return m_pCharacter->ModifyAttackDamageDiceAsTarget(pAttacker, num, sides, pBonus, pNonLethal);
#else
  return CHARACTER::ModifyAttackDamageDiceAsTarget(pAttacker, num, sides, pBonus, pNonLethal);
#endif
}
//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackThac0AsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackThac0AsTarget(const CHARACTER *pAttacker,int *pVal) const
{
#ifdef newCombatant
  return m_pCharacter->ModifyAttackThac0AsTarget(pAttacker, pVal);
#else
  return CHARACTER::ModifyAttackThac0AsTarget(pAttacker, pVal);
#endif
}
//*****************************************************************************
//    NAME: COMBATANT::ModifySaveRoll
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifySaveRoll(const CHARACTER *pTarget, int *pRoll) const
{
  return m_pCharacter->ModifySaveRoll(pTarget, pRoll);
}
//*****************************************************************************
//    NAME: COMBATANT::ModifySaveRollAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifySaveRollAsTarget(CHARACTER *pAttacker, int *pRoll) const
{
  return m_pCharacter->ModifySaveRollAsTarget(pAttacker, pRoll);
}
//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackRollDiceForItem
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackRollDiceForItem(const CHARACTER *pTarget, 
                                            //GLOBAL_ITEM_ID giID, 
                                            const ITEM_ID& itemID, 
                                            const int num, 
                                            const int sides, 
                                            int *pBonus, 
                                            int distance) const
{
  //return m_pCharacter->ModifyAttackRollDiceForItem(pTarget, giID, num, sides, pBonus, distance);
  return m_pCharacter->ModifyAttackRollDiceForItem(pTarget, itemID, num, sides, pBonus, distance);
}

//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackRollDiceForItemAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::ModifyAttackRollDiceForItemAsTarget(const CHARACTER *pAttacker, 
                                                    //GLOBAL_ITEM_ID giID, 
                                                    const ITEM_ID& itemID, 
                                                    const int num, 
                                                    const int sides, 
                                                    int *pBonus) const
{
  //return m_pCharacter->ModifyAttackRollDiceForItemAsTarget(pAttacker, giID, num, sides, pBonus);
  return m_pCharacter->ModifyAttackRollDiceForItemAsTarget(pAttacker, itemID, num, sides, pBonus);
}

//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackDamageDiceForItem
//
// PURPOSE: 
//
//*****************************************************************************
//BOOL COMBATANT::ModifyAttackDamageDiceForItem(const CHARACTER *pTarget, GLOBAL_ITEM_ID giID,const int num, const int sides, int *pBonus, BOOL *pNonLethal, int distance) const
BOOL COMBATANT::ModifyAttackDamageDiceForItem(const CHARACTER *pTarget, const ITEM_ID& itemID,const int num, const int sides, int *pBonus, BOOL *pNonLethal, int distance) const
{
  //return m_pCharacter->ModifyAttackDamageDiceForItem(pTarget, giID, num, sides, pBonus, pNonLethal, distance);
  return m_pCharacter->ModifyAttackDamageDiceForItem(pTarget, itemID, num, sides, pBonus, pNonLethal, distance);
}

//*****************************************************************************
//    NAME: COMBATANT::ModifyAttackDamageDiceForItemAsTarget
//
// PURPOSE: 
//
//*****************************************************************************
void COMBATANT::ModifyAttackDamageDiceForItemAsTarget(const CHARACTER *pAttacker,
                                                      //GLOBAL_ITEM_ID giID, 
                                                      const ITEM_ID& itemID, 
                                                      const int num, 
                                                      const int sides, 
                                                      int *pBonus, 
                                                      BOOL *pNonLethal,
                                                      int toHitRolled) const
{
  //ITEM_DATA *pItem = itemData.GetItemData(giID);
  ITEM_DATA *pItem = itemData.GetItem(itemID);
  if (pItem==NULL) return;

  DWORD src;
  //GLOBAL_SPELL_ID skey;
  SPELL_ID spellID;
  //m_pCharacter->ModifyAttackDamageDiceForItemAsTarget(pAttacker, giID, num, sides, pBonus, pNonLethal);
  m_pCharacter->ModifyAttackDamageDiceForItemAsTarget(pAttacker, itemID, num, sides, pBonus, pNonLethal);
  COMBATANT *pa = pAttacker->m_pCombatant;
  // if vorpal item readied (attacker has ability enabled), and using item that confers ability
  if (   (toHitRolled==20)
      //&& (pa->GetAdjSpecAb(SA_VorpalAttack,&src, &skey))
      && (pa->GetAdjSpecAb(SA_VorpalAttack,&src, &spellID))
      && (pItem->specAbs.HaveSpecAb(SA_VorpalAttack)))
  {    
    if (!HasVorpalImmunity())
    {
      *pBonus = GetHitPoints()+1; // make sure this attack kills target
      //pa->m_pCharacter->QueueUsedSpecAb(SA_VorpalAttack,src,skey);
      pa->m_pCharacter->QueueUsedSpecAb(SA_VorpalAttack,src,spellID);
//      modify=TRUE;
    }
  }
  return;
}


//*****************************************************************************
//    NAME: COMBATANT::GetIsFriendly
//
// PURPOSE: 
//
//*****************************************************************************
BOOL COMBATANT::GetIsFriendly() const
{
  if (m_adjFriendly == 0) return friendly;
  if (m_adjFriendly == 1) return true;
  if (m_adjFriendly == 2) return false;
  return !friendly;
}

void die(int);

void COMBATANT::ComputeCombatViewValues(void)
{
  ComputeCharacterViewValues();
}


CString COMBATANT::RunPSScripts(const CString& scriptName, LPCSTR comment)
{
  CString result;
  SPELL_DATA *pSpell;
  PENDING_SPELL *pPendingSpell;
  if (combatant_pendingSpellKey < 0) return "";
  pPendingSpell = pendingSpellList.Get(combatant_pendingSpellKey);
  //pSpell = spellData.GetSpellData(pPendingSpell->gsID);
  pSpell = spellData.GetSpell(pPendingSpell->spellID);
  if (pSpell == NULL) return "";
  {
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetCombatantContext(this);
    scriptContext.SetSpellContext(pSpell);
    result = pSpell->RunSpellScripts(scriptName, 
                                     ScriptCallback_RunAllScripts, 
                                     NULL, 
                                     comment); 
    if (result.Find('R') >= 0)
    {
      pendingSpellList.Remove(combatant_pendingSpellKey);
      this->StopCasting(true, false);
    };
  };
  /*
  POSITION pos = spellEffects.GetHeadPosition();  
  for ( ;pos != NULL; spellEffects.GetNext(pos))
  {
    spellEffects.GetAt(pos).m_scriptProcessed = false;
    // Any spell effects that get added will have m_scriptProcessed = true;
  };
  for (scriptProcessed=true; scriptProcessed;)
  {
    scriptProcessed = false;
    for (pos = spellEffects.GetHeadPosition() ;pos != NULL; spellEffects.GetNext(pos))
    {
      SPELL_EFFECTS_DATA *pSE;
      pSE = &spellEffects.GetAt(pos); 
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
        pSpell= spellData.GetSpellData(pActiveSpell->spellKey);


        scriptContext.SetSpellContext(pSpell);
        scriptContext.SetCharacterContext(this);
        SEIdentifier.Format("%s[%d]", pSpell->Name, SESpellKey);
        hookParameters[5] = SEIdentifier;
        result += pSpell->specAbs.RunScripts(scriptName, 
                                            ScriptCallback_RunAllScripts, 
                                            NULL, 
                                            "Run SE Scripts", 
                                            pSpell->Name);






        scriptProcessed = true;
        break;
      };
    };
  };
  */
  return result;
}

#ifdef UAFEngine
CString GetHighestLevelBaseclass(int charIndex)
{
  CHARACTER *pChar;
  if (IsCombatActive())
  {
    if (charIndex >= combatData.NumCombatants())
    {
      return "";
    };
    pChar = combatData.GetCombatant(charIndex)->m_pCharacter;
  }
  else
  {
    if (charIndex >= party.numCharacters)
    {
      return "";
    };
    pChar = &party.GetCharacter(charIndex, NULL);
  };
  return pChar->GetHighestLevelBaseclass();
};

int GetBaseclassLevel(int charIndex, const CString& baseclassName)
{
  CHARACTER *pChar;
  BASECLASS_ID baseclassID;
  if (IsCombatActive())
  {
    if (charIndex >= combatData.NumCombatants())
    {
      return 0;
    };
    pChar = combatData.GetCombatant(charIndex)->m_pCharacter;
  }
  else
  {
    if (charIndex >= party.numCharacters)
    {
      return 0;
    };
    pChar = &party.GetCharacter(charIndex, NULL);
  };
  baseclassID = baseclassName;
  return pChar->GetBaseclassLevel(baseclassID);
};

#endif



