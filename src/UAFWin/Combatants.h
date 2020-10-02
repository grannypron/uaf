/******************************************************************************
* Filename: Combatants.h
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
#ifndef __COMBATANTS_H__
#define __COMBATANTS_H__

//#include "SharedQueue.h"
#include "GameEvent.h"
#include "Combatant.h"
#include "DrawTile.h"

enum combatVictorType { NoWinner, PartyWins, PartyRanAway, MonsterWins };

const int MAX_COMBATANTS = 100;
const int MAX_CHARS_IN_ROW = 3; // how many chars wide the battle line can be
const int MAX_COMBAT_IDLE_ROUNDS = 20;

enum overallCombatState 
{ 
  OCS_None,
  OCS_Casting,
  OCS_Attacking, 
  OCS_Guarding, 
  OCS_Bandaging, 
  OCS_Using,
  OCS_Moving,
  OCS_Turning,
  OCS_Fleeing,
  OCS_Fled,
  OCS_ContinueGuarding,
  OCS_Petrified, // Not used as an ICS_STATE...only for script
  OCS_Dying,     // Not used as an ICS_STATE...only for script
  OCS_Dead,      // Not used as an ICS_STATE...only for script
  OCS_Gone,      // Not used as an ICS_STATE...only for script

  // Up to this point we must match individualCombatState exactly

  OCS_NewCombatant,
  OCS_CombatOver,
  OCS_Delaying,
  OCS_EndingTurn,
  OCS_ForcingWin,
  OCS_StartNewRound,
  OCS_ActivateSpell,
  OCS_DisplayingAttacker,
};

const int NUM_COMBATANTS_STATES = 17;
extern char *CombatantsStateText[NUM_COMBATANTS_STATES];

#define newPartyArrangement
#define newMonsterArrangement

class MonsterPlacement
{
public:
  int directionFromParty; //0=N; 1=E; 2=S; 3=W
  int placeX;
  int placeY;
  int facing;
  void Clear(void)
  {
    directionFromParty = PathBAD;
    placeX = -1;
    placeY = -1;
  };
};

struct PARTY_POSITIONS
{
  int x;  // Party member elative to party position
  int y;  // Screen coordinates; 
};


void die(int errorNum);

struct MonsterArrangement
{ // 
public:
  bool      active;
  int       turtleX;          // Screen coordinates; Relative to partyX.
  int       turtleY;          // Screen coordinates; Relative to partyY.
  int       turtleWidth;      // Defaults to 1.
  int       turtleHeight;
  int       partyX;           // Screen coordinates; As seen by monsters.
  int       partyY;           // Screen coordinates; As seen by monsters.
  int       curDirFromParty;  // 0=N; 1=E; 2=S; 3=W
  bool      lineOfSight;      // Enforce the line-of-sight rule
  bool      partyAccessible;  // Enforce the accessible by party rule
  int       distance;         // Square of minimum distance from partyX/Y (or negative).
  int       totalNumMonster;
  int       currentMonster;   // combatData m_aCombatants[currentMonster] being placed next
  int       numMonster[4];    // In each direction
  int       numParty;
  int       limitMinX, limitMinY, limitMaxX, limitMaxY;//Screen coordinates; 
  int       partyMinX, partyMaxX, partyMinY, partyMaxY;//Screen coordinates; 
  int       turtleStack[4];   // Room for 2 pushes
  char      dx['R'-'A'+1];
  char      dy['R'-'A'+1];
  PARTY_POSITIONS partyPositions[MAX_COMBATANTS]; //Screen coordinates; 
  MonsterPlacement *monsterPlacement;
  unsigned char *distanceFromParty; // An entry for each cell on the map.
                                    // Index = MAX_TERRAIN*WIDTH * y + x
                                    // Walking distance.  0-->253   254=not computed  255=Not accessible.
                                    // In the final result 254 and 255 are not accessible.
  MonsterArrangement(void) {active = false; monsterPlacement = NULL;};
  void Activate(int numCombatants) 
  {
    int i;
    if (active)
    {
      die(0xc3111a5);
    };
    numMonster[0] = numMonster[1] = numMonster[2] = numMonster[3] = 0;
    numParty = 0;
    partyMinX = partyMinY = 0x7fffffff;
    partyMaxX = partyMaxY = 0x80000000;
    monsterPlacement = (MonsterPlacement *)realloc(monsterPlacement, numCombatants*sizeof(monsterPlacement[0]));
    for (i=0; i<numCombatants; i++) monsterPlacement[i].Clear();
    active = true;
    distanceFromParty = (unsigned char *)malloc(MAX_TERRAIN_WIDTH*MAX_TERRAIN_HEIGHT);
  };
  void deActivate(void) 
  {
    active = false;
    free(monsterPlacement);
    monsterPlacement = NULL;
    free(distanceFromParty);
    distanceFromParty = NULL;
  };
};

extern MonsterArrangement monsterArrangement;


class TURN_DATA
{
public:
  MONSTER_ID monsterID;     // Type of monster to be turned.
  int numMonster;
  CString monsterTurnType;
  int successDie;           // What needs to be rolled for successful turning
  int minAffected;
  int maxAffected;
  int whatToDo;             // 1='Turn'; 2='Destroy';
  int numToTurn;
  int numTurned;

  void Clear(void);

};

class UNDEAD_TYPE
{
public:
  CString monsterTurnType;
  int     maxToTurn;
  int     numTurned;
};

class TURNING_SUMMARY
{
  int          turnDataSize;  // -1 if invalid

  
  mCArray<TURN_DATA, TURN_DATA&> turnData;
  mCArray<UNDEAD_TYPE, UNDEAD_TYPE&> undeadTypes;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(TurnData,          /* array data name      */ \
                                 MONSTER_ID,        /* Search variable type */ \
                                 monsterID,         /* search variable name */ \
                                 turnData,          /* array name           */ \
                                 TURN_DATA,         /* array data type      */ \
                                 TURNING_SUMMARY)   /* type of *this        */
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(UndeadType,        /* array data name      */ \
                                 CString,           /* Search variable type */ \
                                 undeadType,        /* search variable name */ \
                                 undeadTypes,       /* array name           */ \
                                 UNDEAD_TYPE,       /* array data type      */ \
                                 TURNING_SUMMARY)   /* type of *this        */


  BASECLASS_ID baseclassID; // The one that needs the lowest roll to succeed.
  int          baseclassLevel;       // Associated with baseclassID.
  int          combatantIndex;
  int          rolled;        // result of rolling 1d20.  
  int          turnLevel;            // Associated with baseclassID;

  
  
  void Clear(void)             {turnDataSize = -1;};
  BOOL IsValid(void)           {return turnDataSize >= 0;};
  BOOL IsTurningPossible(void);
  int  GetSize(void)           {return turnDataSize;};
  void Validate(void)          {turnDataSize = 0;};
  void ClearUndeadTypes(void)  {undeadTypes.RemoveAll();};

  TURN_DATA *TurnData(const MONSTER_ID& monsterID) {return GetTurnData(monsterID);};
  TURN_DATA *Insert(void)
  {
    TURN_DATA *pTurnData;
    turnDataSize++;
    if (turnDataSize > turnData.GetSize()) turnData.SetSize(turnDataSize);
    pTurnData = GetTurnData(turnDataSize-1);
    //pTurnData = &turnData[turnDataSize-1];
    pTurnData->Clear();
    return pTurnData;
  };
};

enum AURA_SHAPE
{
  AURA_SHAPE_NULL,
  AURA_SHAPE_GLOBAL,
  AURA_SHAPE_ANNULARSECTOR,
};

enum AURA_WAVELENGTH
{
  AURA_WAVELENGTH_VISIBLE,
  AURA_WAVELENGTH_XRAY,
  AURA_WAVELENGTH_NEUTRINO,
};

enum AURA_ATTACHMENT
{
  AURA_ATTACH_NONE,
  AURA_ATTACH_COMBATANT,
  AURA_ATTACH_COMBATANT_FACING,
  AURA_ATTACH_XY,
};

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

class ToHitComputation
{
  int m_effectiveTargetAC;
  int m_effectiveTHAC0;
  int m_didHit;  //-1 Nothing known.  0 Miss  1 Hit
  int m_beginSpellScriptFailure;
  int m_successful; // HitDice and Spell Begin Script both succeeded.
  int m_toHitDiceRoll;
  int m_attDiceBonus;
  BOOL m_isBackStab;
  int m_backstabMultiplier;
public:
  ToHitComputation(void){m_didHit=-1; m_attDiceBonus=0;};
  int ToHitComputation::ComputeEffectiveTHAC0(const COMBATANT *pAttacker,
                                            const COMBATANT *pTarget,
                                            //GLOBAL_ITEM_ID weaponID,
                                            const ITEM_ID&  weaponID,
                                            int distance);
  void Compute(const COMBATANT *pAttacker,
               const int targetIndex, 
               const COMBATANT *pTarget, 
               const int wpn);
  bool DidHit(void){return m_didHit > 0;};

  int Rolled(void)const{return m_toHitDiceRoll;};
  int AttackDiceBonus(void)const{return m_attDiceBonus;};
  BOOL IsBackStab(void)const{return m_isBackStab;};
  int BackstabMultiplier(void)const{return m_backstabMultiplier;};
  int BeginSpellScriptFailure(void)const{return m_beginSpellScriptFailure;};
  void BeginSpellScriptFailure(int v){m_beginSpellScriptFailure = v;};
  int Successful(void)const {return m_successful;};
  void Successful(int v){m_successful = v;};
};




struct AURA
{
private:
  //COMBAT_SPRITE_DATA *pSprite;
public:
  AURA(void):specialAbilities(false)  {cells = NULL;/*pSprite=NULL;*/};
  ~AURA(void);
  void operator =(const AURA& src);
  void AddSprite(SPELL_DATA *pSpell);
  void RemoveSprite(void);
  CString RunAuraScripts(
                   LPCSTR     scriptName, 
                   CBRESULT (*fnc)(CBFUNC func, CString *scriptResult, void *pkt), 
                   void      *pkt,
                   LPCSTR     comment) const
  {
    CString msg;
    msg.Format("%Aura ID=%d", auraID);
    return specialAbilities.RunScripts(scriptName, 
                                       fnc, 
                                       pkt,
                                       comment,
                                       ScriptSourceType_Aura, 
                                       msg);
  };
  SPECIAL_ABILITIES specialAbilities;
  int   auraID;
  CString userData[10];
  unsigned char *cells;
  CArray<int, int> m_combatants;

  // The Aura definition properties.
  // [0] is the current.value
  // [1] is the new value
  AURA_WAVELENGTH wavelength[2];
  AURA_SHAPE shape[2];
  int x[2], y[2];
  int combatantIndex[2];
  int facing;
  int size1[2], size2[2], size3[2], size4[2];
  SPELL_ID spellID[2];
  AURA_ATTACHMENT attachment[2];
};

struct MOVE_DATA // Details a combatant's movement
{
  int oldX, oldY;
  int newX, newY;
  int combatantIndex;
};

struct COMBAT_DATA 
{
  COMBAT_DATA() { m_iNumCombatants=0; Clear(); }
  ~COMBAT_DATA() {  }
  void RemoveAllCombatants(void);
  void Clear();
  void InitCombatData(COMBAT_EVENT_DATA *event);
  void AddCombatants();
  void AddCharsToCombatants();
  void AddMonstersToCombatants();
  void AddMonsterToCombatants(const CString& monsterName, int friendly);
  //void AddMonsterToCombatants(int monsterDataIndex);
  void AddMonsterToCombatants(const MONSTER_DATA *pMonster);
  void GetWeaponRange(WEAPON_SUMMARY *pWeaponSummary, const ITEM_DATA *pItem, CHARACTER *pCharacter);
  bool CanReady(DWORD rdyLoc, CHARACTER *pChar, const ITEM *pItem, const ITEM_DATA *pItemData);
  bool CanReady(const ITEM *pCharItem, const ITEM_DATA *pItem, CHARACTER *pChar, CString *whyNot);
  void ListWeapons(COMBAT_SUMMARY_COMBATANT *pCombatant);
  void ListAmmo(COMBAT_SUMMARY_COMBATANT *pCSC);
  void ListReachableCells(COMBAT_SUMMARY *pCS, COMBAT_SUMMARY_COMBATANT *pCombatant);
  void ListAttacks(COMBAT_SUMMARY_COMBATANT *pCSC, int numAdditionalAttacks);
  void ListActionsByAmmo(COMBAT_SUMMARY  *pCombatSummary,
                         const WEAPON_SUMMARY *pWeapon,
                         COMBAT_SUMMARY_ACTION *pCSA);
  void ListActionsByWeapon(COMBAT_SUMMARY  *pCombatSummary,
                           int              activeCombatantIndex,
                           int              targetCombatantIndex,
                           int              weaponIndex,
                           bool             friendly,
                           int              distance22,
                           bool             judoMeleeOnly);
  void ListActionsByAttack(COMBAT_SUMMARY  *pCombatSummary,
                           int              activeCombatantIndex,
                           int              targetCombatantIndex,
                           int              attackIndex,
                           bool             friendly,
                           int              distance2);
  void ListMoveAction(COMBAT_SUMMARY  *pCombatSummary,
                      int              activeCombatantIndex,
                      int              targetCombatantIndex,
                      bool             friendly,
                      int              distance22);
  void ListActionsForTarget(COMBAT_SUMMARY *pCombatSummary,
                            int             activeCombatant,
                            int             targetIndex,
                            bool            friendly,
                            int             numAdditionalAttacks,
                            bool            judoMeleeOnly);
  void ListActions(COMBAT_SUMMARY *pCombatSummary, 
                   int activeCombatant, 
                   int numAdditioanlAttacks,
                   bool judoMeleeOnly);
  void ListCombatants(COMBAT_SUMMARY *pCombatSummary, COMBATANT *pActiveCombatant, COMBATANT *pTarget);

  void DetermineIfCombatOver();
  void DetermineVictoryExpPoints();
  void getNumCombatants(int* pNumParty, int* pNumMons);
  COMBATANT& getActiveCombatCharacter();  
  COMBATANT& getCombatPartyCharacter(int PartyIdx);
  COMBATANT *getCombatantPtr(int id);

  COMBATANT *getActiveCastingCharacter();
  void SetActiveCastingCharacter(int dude);

  int getNextCombatant();

  void ForcePartyVictory(); // force immediate party victory
  void DetermineCombatInitiative();

  individualCombatantState GetCombatantState();
  void SetCombatantState(individualCombatantState ICS);
  void determineInitCombatPos();
#ifdef newPartyArrangement
  void determineInitCombatPos(COMBATANT *pCombatant,
                              int   i, 
                              BOOL *reset, 
                              BOOL ignoreCurrentCombatant,
                              const char *partyArrangement
                              //int  *monsterPartyX,
                              //int  *monsterPartyY
                              );
  void getNextMonsterCombatDirection(int i, BOOL *reset);
#else
  void determineInitCombatPos(COMBATANT *pCombatant, int i, BOOL *reset, BOOL ignoreCurrentCombatant);
#endif
  int determinePartyOrientation(int x, int y);
  void GetNextDirection(eventDirType *pCurrDir, int *pFacing);
  int GetMonsterDistance(eventDirType dir);
#ifdef newMonsterArrangement
  void getNextMonsterCombatPos(COMBATANT &monster, int monsterPartyX, int monsterPartyY, BOOL *reset);
#else
  void getNextMonsterCombatPos(COMBATANT &monster);
#endif
#ifdef newMonsterArrangement
  void GetMonsterCoords(int &x, int &y, int partyX, int partyY, int dist, int dir);
#else
  void GetMonsterCoords(int &x, int &y, int dist, int dir);
#endif
#ifdef newPartyArrangement
  void getNextCharCombatPos(COMBATANT*   pDude,
                            BOOL*        pReset, 
                            BOOL         ignoreCurrentCombatant, 
                            const char*  partyArrangement);
#else
  void getNextCharCombatPos(COMBATANT* pDude, BOOL* pReset, BOOL ignoreCurrentCombatant);
#endif
  BOOL PlaceCharacter      (COMBATANT* pDude, int   x, int   y, BOOL ignoreCurrentCombatant);
  BOOL PlaceCharacterLeft  (COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant);
  BOOL PlaceCharacterRight (COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant);
  BOOL PlaceCharacterNorth (COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant);
  BOOL PlaceCharacterSouth (COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant);
  BOOL PlaceCharacterHorz  (COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant);
  BOOL PlaceCharacterVert  (COMBATANT* pDude, int* px, int* py, BOOL ignoreCurrentCombatant);

  BOOL IsCombatOver() { return m_bCombatOver; }

  BOOL IsStartOfTurn();
  BOOL IsFreeAttacker();
  BOOL IsGuardAttacker();
  void DecrementFreeAttacks(void);

  void /* PRS July 2009 BOOL */ UpdateCombat();
  void CheckForDeadGuys();
  void DisplayCombatants();
  void DisplayAURAs(void);
  void displayCursor(BOOL coverFullIcon);
  void HandleCombatRoundMsgExpired();
  void HandleTimeDelayMsgExpired(int iDeathIndex);
  int  HandleTimeDelayMsgBegin(int *pDeathIndex);
  int  HandleCurrState(BOOL zeroMoveAttackOK);

  void RestoreToParty();

  void CheckDyingCombatants();
  void CheckMorale();
  BOOL CheckIdleTime();
  void StartNewRound();

//20111024  int  Get_FreeAttack_Target(void);  // Get target from QComb
  void AddTarget(int target) ;
  int GetCurrTarget(bool updateTarget) ;
  int GetNextTarget() ;
  BOOL CanTarget(bool freeAttack) ;
  BOOL IsAttackPossible(void); 
  int GetNextAim();
  int GetPrevAim();
  int GetCurrAim() { return m_iAimPos; }
  void ResetAim(); 
  void RemoveCurrTarget();
  BOOL IsValidDude(int dude) const {return (dude>=0) && (dude<NumCombatants());};
  BOOL IsValidTarget(COMBATANT *pAttacker, COMBATANT *pTarget, int *targetValidity);
  bool IsAreaLingerSpell(COMBATANT *pCaster);
  
  BOOL CanSeeParty(int x, int y);
  BOOL IsAttacking();
  double AttacksRemaining();
  BOOL CanMove();  
  BOOL CanCast();  
  BOOL CanUse();   
  BOOL CanGuard(GUARDING_CASE guardCase); 
  BOOL CanAttack(int target) ;
  BOOL CanTurnUndead();  
  BOOL CanDelay(); 
  BOOL CanBandage();
  CString SpecialActionName();
  int  SpecialActionIndex();
  void Bandage();
  void EndTurn(individualCombatantState newState=ICS_None); 
  void StopMoving(); 
  void Quick(BOOL Enable); 
  void Guard(); 
  void TurnUndead();
  void StartTurnUndead();
  //void StartInitialSpellCasting(GLOBAL_SPELL_ID key, GLOBAL_SPELL_ID secondaryKey);
  void StartInitialSpellCasting(const SPELL_ID& spellName, const SPELL_ID& secondarySpellName);
  //void StartInitialItemSpellCasting(GLOBAL_SPELL_ID key);
  void StartInitialItemSpellCasting(const SPELL_ID& spellID);
  void EndInitialSpellCasting();
  void StopSpellCasting();
  void DelayAction(); 
  void StopAttack(void);
  void AttackOver(); 
  //void MakeAttack(); 
  BOOL StartAttack(); 

  int GetDistanceBetween_ByPartyIndex(int PartyMember1Idx, int PartyMember2Idx);
  CString ListAdjacentCombatants(int currCombatant, BOOL enemyOnly);

  int InvokePendingSpellOnCombatant(int caster, 
                                    int targ, 
                                    int activeSpellKey, 
                                    bool abortIfSetPartyXY,
                                    CString *displayText=NULL,
                                    BOOL *spellSucceeded=NULL,
                                    //GLOBAL_SPELL_ID spellDbKey=GLOBAL_SPELL_ID(),
                                    const SPELL_ID& spellID = SPELL_ID(),
                                    InvokeSpellOptions *invokeOptions=NULL,
                                    ToHitComputation *pToHitComputation=NULL
                                   );
  void InvokeAreaLingerSpellOnCombatant(int caster, 
                                        int targ, 
                                        int activeSpellKey, 
                                        bool abortIfSetPartyXY,
                                        CString *displayText=NULL,
                                        BOOL *spellSucceeded=NULL,
                                        //GLOBAL_SPELL_ID spellDbKey=GLOBAL_SPELL_ID(),
                                        const SPELL_ID& spellID = SPELL_ID(),
                                        InvokeSpellOptions *pInvokeOptions=NULL,
                                        ToHitComputation *pToHitComputation=NULL
                                       );
  int InvokePendingSpellOnMap(COMBATANT *pCCaster); //int caster, int activeSpellKey);    
  bool IsAreaLingerSpell(int caster);
  void RemoveSpellEffect(DWORD parent, const SPELL_DATA *pSpell, bool endSpell);

  BOOL NeedMissileAnimation();
  void InitMissileAnimation();
  BOOL NeedHitAnimation();
  void InitHitAnimation();
  BOOL NeedSpellCastAnimation(int targ);
  void InitSpellCastAnimation(int targ);
  BOOL NeedSpellInRouteAnimation(int x, int y, BOOL allowSelf);
  void InitSpellInRouteAnimation(int x, int y);
  void InitSpellInRouteAnimation(int srcx, int srcy, int dstx, int dsty);
  BOOL NeedSpellCoverageAnimation(int x, int y);
  void InitSpellCoverageAnimation(int x, int y);
  BOOL NeedSpellHitAnimation(int targ);
  void InitSpellHitAnimation(int targ);
  BOOL NeedSpellLingerAnimation();
  void InitSpellLingerAnimation(/*int mapx, int mapy*/);
  void InitSpellLingerAnimation(int targ);

  BOOL toggleReadyItem(int item);
  BOOL delCharacterItem(int index, int qty);
  void determineAvailAttacks();
  
  bool ProcessTimeSensitiveData(LONGLONG currTime);

  overallCombatState GetCombatState();

  void getCharWeaponText(int index, CString &wpn, CString &dmg);
  BOOL OnAuto(bool callAutoActionHook); 
  void SetUpdateFlag() { m_bNeedUpdate = TRUE; }

  CString FormatTimeDelayMsg(overallCombatState state=OCS_None);
  CString FormatSpecAbMsg(DWORD sa_state, int dude);

  bool CanMoveCursor(const COMBATANT *pCaster, int x, int y);
  bool CanMoveCursorRight(const COMBATANT *pCaster);
  bool CanMoveCursorLeft(const COMBATANT *pCaster);
  bool CanMoveCursorForward(const COMBATANT *pCaster);
  bool CanMoveCursorBackward(const COMBATANT *pCaster);
  bool CanMoveCursorNE(const COMBATANT *pCaster);
  bool CanMoveCursorSE(const COMBATANT *pCaster);
  bool CanMoveCursorSW(const COMBATANT *pCaster);
  bool CanMoveCursorNW(const COMBATANT *pCaster);
  void moveCursorLeft();
  void moveCursorRight();
  void moveCursorForward();
  void moveCursorBackward();
  void PlaceCursorOnCurrentDude(BOOL ForceCenter=FALSE); // on currently active dude
  void PlaceCursorOnDude(int dude,BOOL ForceCenter); // on specified dude

  void moveLeft();
  void moveRight();
  void moveForward();
  void moveBackward();
  void moveNW();
  void moveNE();
  void moveSW();
  void moveSE();
  BOOL CurrMoveWouldFleeMap();

  void PlayCombatDeath();
  void PlayCombatMove();
  void PlayCombatTurnUndead();

  void LoadSounds();
  void ClearSounds();

  int GetCurrCombatant();

  COMBAT_EVENT_DATA *m_pEvent;
  eventDistType monsterDistance; // Initially  pEvent->distance.
                                 // Reduced until some monsters are properly placed         

  int m_hCharDeathSound;
  int m_hCharMoveSound;
  int m_hTurnUndeadSound;

  BOOL m_bStartingNewCombatRound;
  BOOL m_bCombatActive;
  BOOL m_bLastRound;
  BOOL m_bCombatOver;
  BOOL m_bNeedUpdate;
  BOOL m_forceRoundDelay;
  int m_iExpPointTotal;
  int m_iCurrCaster;
  int m_iRedirectedSpellTarget;
  CString m_sRedirectedSpellMessage;
  CString m_sSpellEffectMessage;

//  BOOL m_bPlayerTypedSpace;

  eventTurnUndeadModType m_eUndeadTurnModifier;
 	combatVictorType       m_eCombatVictor;

  int m_iMoveToX      , m_iMoveToY;
	int m_iStartTerrainX, m_iStartTerrainY;
	int m_iCursorX      , m_iCursorY;
  int m_iPartyStartX  , m_iPartyStartY;
  int m_iPartyOriginX , m_iPartyOriginY;
//PRS 20110223  int m_iPrevRndCombatant;
  BOOL m_bNewCombatantSinceLastUpdateCombat;
	int m_iCurrInitiative;
	int m_iCurrRound;
  int m_iCurrTurn;
  int m_iAimPos;

  eventSurpriseType m_eSurprise;

  int m_iActiveScripter;
  void GetContext(ActorType *pActor, int i);

  // used for morale checks
  int m_iNumMonFlee;
  int m_iNumMonSlain;
  int m_iNumFrndFlee;
  int m_iNumFrndSlain;

  // flags that alter normal combat behavior
  BOOL m_bMonsterNoMove;
  BOOL m_bMonsterAlwaysMiss;
  BOOL m_bMonsterAlwaysHit;
  BOOL m_bMonsterNoCast;
  BOOL m_bMonsterPlyrControl;
  BOOL m_bMonsterNoFlee;
  BOOL m_bMonsterNoDeath;
  BOOL m_bPartyAlwaysMiss;
  BOOL m_bPartyAlwaysHit;
  BOOL m_bNoMagic;
  BOOL m_bPartyNeverDies; 
  BOOL m_bPartyNoExperience;
  
  // which combat map format is used
  BOOL m_bIsWilderness;

  QueuedCombatantData QComb;

  // used for clipping
  RECT m_combatRect;
  RECT m_fullRect;

  ITEM_LIST hurledWeapons;
  TURNING_SUMMARY  turningSummary;
  void SummarizeTurningCapability(void);

  private:
	  int m_iNumCombatants;
	  CArray<COMBATANT,  COMBATANT&> m_aCombatants;
	  CArray<COMBATANT,  COMBATANT&> m_aTempCOMBATANTS;
  public:
#ifdef newCombatant
    COMBATANT *GetCombatant(int dude)
    {
      if (!IsValidDude(dude)) return NULL;
      if (    (m_aCombatants[dude].m_pCharacter != NULL)
           && (m_aCombatants[dude].m_pCharacter->m_pCombatant != &m_aCombatants[dude])
         )
      {
        die(0x8ce2ab);
      };
      return &m_aCombatants[dude];
    };
#else
    COMBATANT *GetCombatant(int dude){return IsValidDude(dude)?&m_aCombatants[dude]:NULL;};
#endif
    int NumCombatants(void) const {return m_iNumCombatants;};
    //int NumCombatants(void) const {return m_aCombatants.GetSize();};
  private:
    int m_nextAuraID;
    mCList<AURA, AURA&>m_auras;
    CArray<int, int> m_auraReferenceStack;  // auraID
  public:
    int CreateAura(const CString& s1,
                   const CString& s2,
                   const CString& s3,
                   const CString& s4,
                   const CString& s5);
    void DeleteAura     (POSITION pos);
    void AddAuraSA      (POSITION pos, const CString& name, const CString& param);
    CString GetAuraSA   (POSITION pos, const CString& name);
    CString RemoveAuraSA(POSITION pos, const CString& name);
    AURA *GetAURA       (POSITION pos) {return m_auras.GetAt(pos);};
    POSITION GetAuraReference(void); // The top of the m_auraReferenceStack
    void PushAuraReference(int auraID);
    void PopAuraReference(void);
    bool CheckAuraPlacement(AURA *pAURA, MOVE_DATA *pMoveData);
    void CheckAllAuraPlacements(MOVE_DATA *pMoveData);
    void DetermineNULLCoverage(AURA *pAURA);
    void DetermineAnnularCoverage(AURA *pAURA);
    void DetermineGlobalCoverage(AURA *pAURA);
    void AnnularOctantX(AURA *pAURA,
                        int minRadius,
                        int maxRadius,
                        double minTangent,
                        double maxTangent,
                        int x0,
                        int y0,
                        int dirX,   // +-1
                        int dirY  // +-1
                        );
    void AnnularOctantY(AURA *pAURA,
                        int minRadius,
                        int maxRadius,
                        double minTangent,
                        double maxTangent,
                        int x0,
                        int y0,
                        int dirX,   // +-1
                        int dirY  // +-1
                        );
    void LocateAuraCenters(AURA *pAURA, CArray<POINT, POINT&> *c);
};

CString FormatCombatMoveText();
CString FormatSpellHitText(COMBATANT *pTarget, const CString *messageFormat);

//extern COMBAT_MSGS combatMsgs;
extern COMBAT_DATA combatData;


#endif //__COMBATANTS_H__