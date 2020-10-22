/******************************************************************************
* Filename: GameEvent.h
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
#ifndef __GAMEEVENT_H__
#define __GAMEEVENT_H__

//lint -e1411

#include "items.h" 
#include "Money.h"
#include "SoundMgr.h"

#define EARLY_WALL_SOUNDS

//#include "spell.h"
// Replaced spell.h with the following:

struct spellBookType;
enum spellSaveVersusType;
enum spellSaveEffectType;
enum key_code;
enum TASKSTATE;
enum EventDisposition;
class BACKGROUND_SOUND_DATA;


//class CAR;

const int NO_EVENT = 0;

const int MAX_TOUR_STEPS = 24;
const int MAX_BUTTON_TEXT = 15; // horizontal buttons
const int MAX_LABEL_TEXT  = 36; // vertical buttons
const int MAX_PSWD_TEXT = MAX_BUTTON_TEXT;
const int MAX_BUTTONS = 5; // both horiz or vertical


const BYTE SPECIAL_OBJECT_NOTHING = 0x00;
const BYTE SPECIAL_OBJECT_TAKE    = 0x01;
const BYTE SPECIAL_OBJECT_GIVE    = 0x02;
const BYTE SPECIAL_OBJECT_CHECK   = 0x04;

enum EventSourceType { LevelEventSrc=0, GlobalEventSrc=1, CopyEventSrc=2 };

const int NoUserResult  = 0;
enum eventLocationRestriction { AllowAnywhere, AllowInWilderness, AllowInCities };

enum eventDistType { UpClose, 
                     Nearby, 
                     FarAway,
                     AutoFarAway,    
                     AutoUpClose,  // These last two are only used internally
                     AutoNearby,   // by the TEXT_EVENT to provide AutoApproach
                     };
const int NUM_DIST_TYPES = 4;

enum eventFacingType { FaceNorth, FaceEast, FaceSouth, FaceWest, FaceUnchanged };
const int NUM_FACING_TYPES = 5;

enum eventDirType 
{ 
   Any, North, South, East, West,
   N_S, N_E, N_W, S_E, S_W, E_W,
   N_S_E, N_S_W, N_W_E, W_S_E, InFront 
};

const int NUM_DIR_TYPES = 16;

enum eventSurpriseType { Neither, PartySurprised, MonsterSurprised };
const int NUM_SURPRISE_TYPES = 3;

enum eventPartyAffectType 
{ 
   NoPartyMember, EntireParty, ActiveChar,
   OneAtRandom, ChanceOnEach 
};
const int NUM_AFFECT_TYPES = 5;

enum eventStepType { TStep_NoMove, TStep_Pause, TStep_Forward, TStep_Left, TStep_Right };
const int NUM_STEP_TYPES = 5;

enum eventType        // implementation status
{                     //
   // Status:
   //        Dlg: Editor dialog has been created to allow user to configure this event
   //       Game: Engine has the code to process this event
   //   ChainIds: Indicates an event which stores references to other events
   //             using event keys, not including the chainEventHappen/chainEventNotHappen
   //             events which all events have. These events need special handling
   //             for situations such as event deletion, since the referenced events
   //             must also be deleted.
   //      
   //             Events with ChainIds must over-ride the following virtual functions
   //             and provide proper handling for each chain event.
   //
   //               IsChained
   //               ChainToEventId
   //               UnchainToEventId
   //               GetEVChainText
   //               GetEVChain
   //               GetChainCount
   //
   //             Special handling must also be added to these functions which
   //             are shared by all events.
   //
   //               DeleteEvent
   //               CopyEvent
   //
   //             And of course each chain event must be processed in 
   //             functions such as Serialize and operator=.
   //
   //             The list of events with ChainIds currently includes:
   //
   //               ChainEventType,
   //               EncounterEvent,
   //               EnterPassword,
   //               QuestStage,
   //               QuestionButton,
   //               QuestionList,
   //               QuestionYesNo,
   //               SmallTown,
   //               TavernEvent,
   //               TempleEvent,
   //               WhoPays,
   //               WhoTries,
   //               LogicBlock,
   //               RandomEvent
   //
                      // Dlg? Game? ChainIds
   NoEvent,           //  -     -     -
   AddNpc,            //  x     x     -
   Camp,              //  x     x     -
   ChainEventType,    //  x     x     x
   Combat,            //  x     x     -
   CombatTreasure,    //  x     x     - <--- CombatTreasure=GiveTreasure
   Damage,            //  x     x     -
   EncounterEvent,    //  x     x     x
   EnterPassword,     //  x     x     x
   GainExperience,    //  x     x     -
   GiveTreasure,      //  x     x     - <--- CombatTreasure=GiveTreasure
   GuidedTour,        //  x     x     -
   InnEvent,          //  -     -     - <--- obsoleted, WhoPays+CampEvent
   NPCSays,           //  x     x     -
   PassTime,          //  x     x     -
   PickOneCombat,     //  -     -     - <--- obsoleted, use option in Combat event
   QuestStage,        //  x     x     x
   QuestionButton,    //  x     x     x
   QuestionList,      //  x     x     x
   QuestionYesNo,     //  x     x     x
   RemoveNPCEvent,    //  x     x     - 
   ShopEvent,         //  x     x     -
   SmallTown,         //  x     x     x
   Sounds,            //  x     x     -
   SpecialItem,       //  x     x     -
   Stairs,            //  x     x     - <--- Stairs=Teleporter=TransferModule
   TavernEvent,       //  x     x     x
   TavernTales,       //  -     -     - <--- obsoleted, included in TavernEvent
   Teleporter,        //  x     x     - <--- Stairs=Teleporter=TransferModule
   TempleEvent,       //  x     x     x
   TextStatement,     //  x     x     - 
   TrainingHallEvent, //  x     x     -
   TransferModule,    //  x     x     - <--- Stairs=Teleporter=TransferModule
   Utilities,         //  x     x     -
   Vault,             //  x     x     -
   WhoPays,           //  x     x     x
   WhoTries,          //  x     x     x
   TakePartyItems,    //  x     x     -
   HealParty,         //  x     x     -
   LogicBlock,        //  x     x     x
   GPDLEvent,         //        x     -
   RandomEvent,       //  x     x     x
   PlayMovieEvent,    //  x     x     -
   JournalEvent,      //  x     x     -
   FlowControl,       //  x     x     -

   // The rest are runtime-generated 'fake' events for control purposes
   // These values are in save files and must not change.
   
   CONTROL_Splash=1000, // Must remain as first CONTROL_ event
   CONTROL_DoNothing=1001,
   CONTROL_Exit=1002,
   CONTROL_Start=1003,
   CONTROL_Main=1004,
   CONTROL_CreateCharacter=1005,
   CONTROL_Race=1006,
   CONTROL_Gender=1007,
   CONTROL_Class=1008,
   CONTROL_Alignment=1009,
   CONTROL_ChooseStats=1010,
   CONTROL_GetCharName=1011,
   CONTROL_GetCharIcon=1012,
   CONTROL_GetCharSmallPic=1013,
   CONTROL_AddCharacter=1014,
   CONTROL_Adventure=1015,
   CONTROL_Move=1016,
   CONTROL_UNUSED=1017,
   CONTROL_Save=1018,
   CONTROL_Load=1019,
   CONTROL_BeginXYEvent=1020,
   CONTROL_BeginStepEvent=1021,
   CONTROL_PlayStepsound=1022,
   CONTROL_TakeTreasure=1023,
   CONTROL_ViewCharacter=1024,
   CONTROL_ItemsMenu=1025,
   CONTROL_GetMoneyQty=1026,
   CONTROL_Encamp=1027,
   CONTROL_PassingTime=1028,
   CONTROL_BuyItems=1029,
   CONTROL_TrainCharacters=1030,
   CONTROL_BeginRestEvent=1031,
   CONTROL_BeginZoneEvent=1032,
   CONTROL_CombatAimAuto=1033,
   CONTROL_CombatAimManual=1034,
   CONTROL_CombatMove=1035,
   CONTROL_CombatSpriteAnimate=1036,
   CONTROL_TimeDelayMsg=1037,
   CONTROL_CombatRoundDelay=1038,
   CONTROL_GameSpeed=1039,
   CONTROL_HumanChangeClass=1040,
   CONTROL_TradeTakerSelect=1041,
   CONTROL_AlterGame=1042,
   CONTROL_AlterGameLevel=1043,
   CONTROL_AlterPartyOrder=1044,
   CONTROL_PartyDied=1045,
   CONTROL_CombatResults=1046,
   CONTROL_MagicMenu=1047,
   CONTROL_CastMenu=1048,
   CONTROL_CastTargetMenu=1049,
   CONTROL_MemorizeMenu=1050,
   CONTROL_MemorizeVerifyMenu=1051,
   CONTROL_ScribeMenu=1052,
   CONTROL_DisplaySpellAffectsMenu=1053,
   CONTROL_RestTimeMenu=1054,
   CONTROL_AppraiseSelect=1055,
   CONTROL_AppraiseEvaluate=1056,
   CONTROL_AskYesNo=1057,
   CONTROL_DesignSelect=1058,
   CONTROL_TargetSelectNonCombat=1059,
   CONTROL_NewMuSpellLevelMenu=1069,
   CONTROL_BashDoorMenu=1070,
   CONTROL_AlterGameSound=1071,
   CONTROL_CastNonCombatSpell=1072,
   CONTROL_DisplayPartyJournal=1073,
   CONTROL_CastCombatSpell=1074,
   CONTROL_TargetSelectCombat=1075,
   CONTROL_CombatSpellSpriteAnimate=1076,
   CONTROL_CombatSpellAimAuto=1077,
   CONTROL_CombatSpellAimManual=1078,
   CONTROL_CombatUtilityDelay=1079,
   CONTROL_InitialMUSpellsMenu=1080,
   CONTROL_LevelUpLearnSpellsMenu=1081,
   CONTROL_AskCustom,
   CONTROL_BeginTimeEvent,
   CONTROL_ScriptChoice,
   CONTROL_CombatSpecialAction,
   CONTROL_ChooseFromList,
};
const int NUM_EVENT_TYPES = 45; // doesn't include CONTROL_* types

enum eventTurnUndeadModType { None, Hard, Difficult, Impossible };
const int NUM_TURN_MOD_TYPES = 4;

enum eventTriggerType 
{  
   AlwaysTrigger, 
   PartyHaveItem, PartyNotHaveItem,
   Daytime, Nighttime,
   RandomChance,
   PartySearching, PartyNotSearching,
   FacingDirection,
   QuestComplete, QuestFailed, QuestInProgress,
   PartyDetectingTraps, PartyNotDetectingTraps,
   PartySeeInvisible, PartyNotSeeInvisible,
   ClassInParty, ClassNotInParty,
   RaceInParty, RaceNotInParty,
   QuestPresent, QuestNotPresent,
   GenderInParty, GenderNotInParty,
   FacingDirectionAnyTime,
   NPCInParty,NPCNotInParty,
   ExecuteGPDL,
   SpellMemorized,
   PartyAtXY,
   BaseclassInParty, BaseclassNotInParty,
   PartyHaveSpecialItem, PartyNotHaveSpecialItem,
   PartyHaveSpecialKey, PartyNotHaveSpecialKey,
   QuestStageEqual, QuestStageNotEqual,
   LAST_EVENT_TRIGGER
};

const int NUM_EVENTTRIG_TYPES = LAST_EVENT_TRIGGER;
enum eventResultType { HasNeverHappened, HasHappenedAtLeastOnce };

enum chainTriggerType { AlwaysChain, IfEventHappen, IfEventNotHappen };
const DWORD NUM_CHAINTRIG_TYPES = 3;

enum encounterButtonResultType 
{ 
   NoResult, DecreaseRange, CombatNoSurprise, 
   CombatSlowPartySurprised, CombatSlowMonsterSurprised,
   Talk, EscapeIfFastPartyElseCombat, ChainToEvent 
};
const int NUM_ENCOUNTER_BUTTON_OPTION_TYPES = 8;

enum labelPostChainOptionsType 
{ 
   LabelDoNothing, 
   LabelReturnToQuestion, 
   LabelBackupOneStep
};
const int NUM_LABEL_POST_CHAIN_OPTION_TYPES = 3;

enum passwordActionType { NoAction, ChainEvent, Teleport, BackupOneStep };
const int NUM_PASSACTION_TYPES = 4;
enum passwordMatchType { ExactMatch, PswdInTyped, TypedInPswd };
const int NUM_PASSMATCH_TYPES = 3;

enum takeItemsActionType { TakeInventoryAction=1, TakeMoneyAction=2, TakeGemsAction=4, TakeJewelryAction=8 };
const int NUM_TAKEITEMS_TYPES = 4;

enum takeItemQtyType { TakeSpecifiedQty, TakeRndQty, TakePcntQty, TakeAll };
const int NUM_TAKEITEMQTY_TYPES = 4;

enum takeItemsAffectsType { TakeAffectsParty, TakeAffectsRndChar, TakeAffectsActiveChar };
const int NUM_TAKEAFFECTS_TYPES = 3;

const int MAX_TALES = 20;
const int MAX_DRINKS = 5;
const int MAX_DRINK_LEN = 15;

enum taleOrderType { InOrderTales, RandomTales };
const int NUM_TALEORDER_TYPES = 2;
const int NUM_LITERALORPERCENT_TYPES = 3;

int GetCurrentLevel(void);

class TRANSFER_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( TRANSFER_DATA )
   TRANSFER_DATA() { Clear(); }
   virtual ~TRANSFER_DATA() {  }
   TRANSFER_DATA(const TRANSFER_DATA& src) { *this = src; }
   void Clear() 
   {
     execEvent=TRUE;
     m_facing=FaceNorth;
     destEP=-1;
     destLevel=GetCurrentLevel();
     destX=0;
     destY=0;
   };
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
   TRANSFER_DATA& operator =(const TRANSFER_DATA& src);
#ifdef UAFEDITOR
   BOOL Validate(FILE *pFile);
   void Export(JWriter& jw, const char *name);
   bool Import(JReader& jr, const char *name);
   bool operator ==(const TRANSFER_DATA& src) const;
#endif

   BOOL execEvent;
   eventFacingType m_facing;  // new facing at destination
   int destEP;    // destination entry point
   int destLevel;
   int destX;
   int destY;
};

class MONSTER_EVENT //: public CObject
{
public:
//   DECLARE_SERIAL( MONSTER_EVENT )
   MONSTER_EVENT() { Clear(); }
   MONSTER_EVENT(const MONSTER_EVENT& src) { *this = src; }
   virtual ~MONSTER_EVENT() {  }
   void Clear() 
   { 
     moraleAdj=0;    // This is no longer used.
                     // Originally, this was designed such that the editor set
                     // moraleAdj for each monster in the combat.
                     // But now, the runtime engine determines the moraleAdj
                     // when the combat begins.  Otherwise, a designer who changed
                     // the morale of a monster would have to re-edit each combat
                     // event that included the modified monster.
                     // Now we do this adjustment at runtime to always use the latest
                     // values for the monsters.
     qty=0;
     m_type=MONSTER_TYPE;
     //monster=NO_MONSTER;
     monsterID.Clear();
     characterID.Clear();
     friendly=FALSE;
     items.Clear();
     money.Clear(); 
     qtyDiceSides=0;
     qtyDiceQty=0;
     qtyBonus=0;
     UseQty = meUseQty;
   }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const MONSTER_EVENT& src) const;
#endif
   MONSTER_EVENT& operator =(const MONSTER_EVENT& src);

   enum { meUseQty, meUsePercent };
   
   int UseQty; // use qty or roll dice
   int qty; // absolute number of monsters

   int qtyDiceSides;    // roll dice for number of monsters
   int qtyDiceQty;
   int qtyBonus;

   //int monster; // monster db key
   MONSTER_ID monsterID; // Used to be monster db key
   CHARACTER_ID characterID;
   int m_type;    // monster or npc
   BOOL friendly; // whose side is monster on?
   int moraleAdj;    // This is no longer used.
                     // Originally, this was designed such that the editor set
                     // moraleAdj for each monster in the combat.
                     // But now, the runtime engine determines the moraleAdj
                     // when the combat begins.  Otherwise, a designer who changed
                     // the morale of a monster would have to re-edit each combat
                     // event that included the modified monster.
                     // Now we do this adjustment at runtime to always use the latest
                     // values for the monsters.
   MONEY_SACK money; // goodies in monster's inventory
   ITEM_LIST items;
};

class MONSTER_EVENT_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( MONSTER_EVENT_DATA )
   MONSTER_EVENT_DATA() { Clear(); }
   virtual ~MONSTER_EVENT_DATA() { Clear(); }
   void Clear() { monsters.RemoveAll(); }
   int GetCount() { return monsters.GetSize(); }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator == (const MONSTER_EVENT_DATA& src) const;
#endif
   MONSTER_EVENT_DATA& operator =(const MONSTER_EVENT_DATA& src);
   int Add(const MONSTER_EVENT &data)
   {
      monsters.Add(const_cast<MONSTER_EVENT&>(data));
      return monsters.GetUpperBound();
   }
   void Delete(int index)
   {
     if ((index >= 0) && (index < monsters.GetSize()))
       monsters.RemoveAt(index);
   }

   CArray<MONSTER_EVENT, MONSTER_EVENT&> monsters;
};

class TOUR_STEP //: public CObject 
{
public:
//   DECLARE_SERIAL( TOUR_STEP )
   TOUR_STEP() { Clear(); }
   virtual ~TOUR_STEP() {  }
   void Clear() { text="";step=TStep_NoMove; }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw, int stepNum);
   int  Import(JReader& jr);
   bool operator ==(const TOUR_STEP &src) const;
#endif
   TOUR_STEP& operator =(const TOUR_STEP& src);

   CString text;
   eventStepType step;
};

class ENCOUNTER_BUTTON_OPTION //: public CObject
{
public:
//   DECLARE_SERIAL( ENCOUNTER_BUTTON_OPTION )
   ENCOUNTER_BUTTON_OPTION() : chain(0) {   }
   virtual ~ENCOUNTER_BUTTON_OPTION() { Clear(); }
   void Clear() { label="";present=FALSE;allowedUpClose=TRUE;optionResult=NoResult;chain=0;onlyUpClose=FALSE; }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const ENCOUNTER_BUTTON_OPTION& src) const;
#endif
   ENCOUNTER_BUTTON_OPTION& operator =(const ENCOUNTER_BUTTON_OPTION& src);

   CString label;
   BOOL present;
   BOOL allowedUpClose;
   BOOL onlyUpClose;
   encounterButtonResultType optionResult;
   DWORD chain;
};

class ENCOUNTER_BUTTON_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( ENCOUNTER_BUTTON_DATA )
   ENCOUNTER_BUTTON_DATA() { Clear(); }
   virtual ~ENCOUNTER_BUTTON_DATA() {  }
   void Clear() { numButtons=0; for (int i=0;i<MAX_BUTTONS;i++) buttons[i].Clear(); }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const ENCOUNTER_BUTTON_DATA& src) const;
#endif
   ENCOUNTER_BUTTON_DATA& operator =(const ENCOUNTER_BUTTON_DATA& src);

   int numButtons;   
   ENCOUNTER_BUTTON_OPTION buttons[MAX_BUTTONS];
};

class QLIST_OPTION //: public CObject
{
public:
//   DECLARE_SERIAL( QLIST_OPTION )
   QLIST_OPTION() : chain(0) { Clear(); }
   virtual ~QLIST_OPTION() {  }
   void Clear() { label=""; postChainAction=LabelDoNothing;present=FALSE;chain=0; }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const QLIST_OPTION& src) const;
#endif
   QLIST_OPTION& operator =(const QLIST_OPTION& src);

   CString label;
   labelPostChainOptionsType postChainAction;
   BOOL present;
   DWORD chain;
};

class QLIST_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( QLIST_DATA )
   QLIST_DATA() { Clear(); }
   virtual ~QLIST_DATA() { }
   void Clear() { title="";numListButtons=0;for (int i=0;i<MAX_BUTTONS;i++) buttons[i].Clear(); }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);

#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const QLIST_DATA& src) const;
#endif
   QLIST_DATA& operator =(const QLIST_DATA& src);

   CString title;
   int numListButtons;
   QLIST_OPTION buttons[MAX_BUTTONS];
};

class QBUTTON_OPTION //: public CObject
{
public:
//   DECLARE_SERIAL( QBUTTON_OPTION )
   QBUTTON_OPTION() : chain(0) { Clear(); }
   virtual ~QBUTTON_OPTION() { }
   void Clear() { label=""; postChainAction=LabelDoNothing;present=FALSE;chain=0; }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const QBUTTON_OPTION& src) const;
#endif
   QBUTTON_OPTION& operator =(const QBUTTON_OPTION& src);
     
   CString label;
   labelPostChainOptionsType postChainAction;
   BOOL present;
   DWORD chain;
};


class QBUTTON_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( QBUTTON_DATA )
   QBUTTON_DATA() { Clear(); }
   virtual ~QBUTTON_DATA() {  }
   void Clear() { numListButtons=0; for (int i=0;i<MAX_BUTTONS;i++) buttons[i].Clear(); }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const QBUTTON_DATA& src) const;
#endif
   QBUTTON_DATA& operator =(const QBUTTON_DATA& src);

   int numListButtons;
   QBUTTON_OPTION buttons[MAX_BUTTONS];
};

class TALE //: public CObject
{
public:
//  DECLARE_SERIAL( TALE )
  TALE() { Clear(); }
  virtual ~TALE() {  }
  void Clear() { tale="";m_count=0; }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
  TALE& operator =(const TALE& src);
#ifdef UAFEDITOR
  void Export(JWriter& jw, int index);
  int  Import(JReader& jr);
  bool operator ==(const TALE& src) const;
#endif
  CString tale;
  int m_count;
};

class DRINK //: public CObject
{
public:
//  DECLARE_SERIAL( DRINK )
  DRINK() { Clear(); }
  virtual ~DRINK() {  }
  void Clear() { name="";points=0; }
  void Serialize(CArchive &ar);
  void Serialize(CAR &ar);
  DRINK& operator =(const DRINK& src);
#ifdef UAFEDITOR
  void Export(JWriter& jw, int index);
  int  Import(JReader& jr);
  bool operator ==(const DRINK& src) const;
#endif

  CString name;
  int points;
};

class REST_EVENT //: public CObject
{
public:
//   DECLARE_SERIAL( REST_EVENT )
   REST_EVENT() : restEvent(0) { Clear(); }
   virtual ~REST_EVENT() {  }
   void Clear() { allowResting=TRUE;restEvent=0;chance=0;everyMin=0;prevMinChecked=0; }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
   REST_EVENT& operator =(const REST_EVENT& src);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   void Import(JReader& jr);
   bool operator ==(const REST_EVENT& src);
#endif

   BOOL allowResting;
   DWORD restEvent;
   int chance;
   int everyMin;
   int prevMinChecked;
};

class SPECIAL_OBJECT_EVENT_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( SPECIAL_OBJECT_EVENT_DATA )
   SPECIAL_OBJECT_EVENT_DATA() { Clear(); }
   SPECIAL_OBJECT_EVENT_DATA(const SPECIAL_OBJECT_EVENT_DATA &src) { *this = src; }
   virtual ~SPECIAL_OBJECT_EVENT_DATA() {  }
   void Clear() { ItemType=0; operation=SPECIAL_OBJECT_NOTHING; index=-1; id=-1; }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference);
   void Export(JWriter& jw);
   int Import(JReader& jr);
   bool operator ==(const SPECIAL_OBJECT_EVENT_DATA& src) const;
#endif
   SPECIAL_OBJECT_EVENT_DATA& operator =(const SPECIAL_OBJECT_EVENT_DATA& src);
     
   BYTE ItemType;
   BYTE operation;
   int index;
   int id;
};

class SPECIAL_OBJECT_EVENT_LIST //: public CObject
{
public:
//   DECLARE_SERIAL( SPECIAL_OBJECT_EVENT_LIST )
   SPECIAL_OBJECT_EVENT_LIST() { Clear(); }
   virtual ~SPECIAL_OBJECT_EVENT_LIST() { Clear(); }
   void Clear() { list.RemoveAll(); }
   void Serialize(CArchive &ar);
   void Serialize(CAR &ar);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const SPECIAL_OBJECT_EVENT_LIST& src) const;
#endif
   SPECIAL_OBJECT_EVENT_LIST& operator =(const SPECIAL_OBJECT_EVENT_LIST& src);
   BOOL Get(int id, SPECIAL_OBJECT_EVENT_DATA &data);
   int Add(SPECIAL_OBJECT_EVENT_DATA &data);
   BOOL Delete(int id);
   void CrossReference(CR_LIST *pCRList, CR_REFERENCE *pCRReference);
   CArray<SPECIAL_OBJECT_EVENT_DATA, SPECIAL_OBJECT_EVENT_DATA&> list;
};

class CR_EVENT_INFO;

// same event can be triggered in more than one
// zone.
class STEP_EVENT_DATA //: public CObject
{
public:
//   DECLARE_SERIAL( STEP_EVENT )
   STEP_EVENT_DATA() :
    // ASLs are named "STEPEVENT_ATTR"
    stepevent_asl(),
    temp_asl(),
    stepEvent(0)
   { Clear(); }
   virtual ~STEP_EVENT_DATA() {  }
   //void Clear() { stepEvent=0;stepCount=0; for(int i=0;i<MAX_ZONES;i++) stepTrigger[i]=FALSE; }
   void Clear() { stepEvent=0;stepCount=0; zoneMask=0; name.Empty();}
   void Serialize(CArchive &ar, double version);
   void Serialize(CAR &ar, double version);
   void PreSerialize(BOOL IsStoring);
   void PostSerialize(BOOL IsStoring, double version);
   void CrossReference(CR_EVENT_INFO *crEI);
   STEP_EVENT_DATA& operator =(const STEP_EVENT_DATA& src);
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   void Import(JReader& jr);
   bool operator ==(const STEP_EVENT_DATA& src)const;
#endif

   DWORD stepEvent;
   int stepCount;
   int zoneMask;
   CString name;
   //BOOL stepTrigger[MAX_ZONES];

  A_ASLENTRY_L stepevent_asl;
  A_ASLENTRY_L temp_asl;
};

class TIME_EVENT_DATA //: public CObject
{
public:
//  DECLARE_SERIAL( TIME_EVENT )

  enum
  { 
    ABSOLUTE_TIME,  // trigger event at specified time only
    REPEAT_TIME     // trigger event every multiple of specified time
  };

    TIME_EVENT_DATA() :
    // ASLs are named "TIME_EVENT_ATTR"
    timeevent_asl(),
    temp_asl(),
    timeEvent(0),
    day(-1),
    hour(-1),
    minute(-1)
  { Clear(); }
  virtual ~TIME_EVENT_DATA() {  }
  void Clear() { timeEvent=0; day=-1;hour=-1;minute=-1;dayCounter=-1;hourCounter=-1;minuteCounter=-1; }
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring);
  TIME_EVENT_DATA& operator =(const TIME_EVENT_DATA& src);    

  DWORD timeEvent;

  int triggerType; // absolute, or repeating
  int day;     
  int hour;    
  int minute;  
  
  int dayCounter;
  int hourCounter;
  int minuteCounter;

  A_ASLENTRY_L timeevent_asl;
  A_ASLENTRY_L temp_asl;
};

struct CLASS_BASECLASS_ID
{
  CLASS_ID classID;
  BASECLASS_ID baseclassID;
  void Serialize(CAR& car, bool baseclass)
  {
    if (!baseclass) classID.Serialize(car);
    else baseclassID.Serialize(car);
  };
  void Serialize(CArchive& ar, BOOL baseclass)
  {
    if (!baseclass) classID.Serialize(ar);
    else baseclassID.Serialize(ar);
  };
};

class EVENT_CONTROL //: public CObject
{
public:
//  DECLARE_SERIAL(EVENT_CONTROL)
private:
  friend class GameEvent;
public:
   EVENT_CONTROL() :
    // ASLs are named "EVENTCONT_ATTR"
    eventcontrol_asl(),
    temp_asl()
    { Clear(); }

   EVENT_CONTROL(const EVENT_CONTROL& src);
   virtual ~EVENT_CONTROL() { /*Clear();*/ }

   void Serialize( CArchive& archive, double version );
   void Serialize( CAR& archive, double version );
#ifdef UAFEDITOR
   void Export(JWriter& jw);
   bool Import(JReader& jr);
#endif
   void PreSerialize(BOOL IsStoring);
   void PostSerialize(BOOL IsStoring);

   EVENT_CONTROL& operator =(const EVENT_CONTROL& src);
#ifdef UAFEDITOR
   bool operator ==(const EVENT_CONTROL& src) const;
#endif
   void Clear();  
#ifdef UAFEngine
   BOOL EventShouldTrigger(DWORD id, int level, int x, int y); 
#endif
   
   BOOL onceOnly;
private:
   int eventStatusUnused;
   int eventResultUnused;
public:
   chainTriggerType chainTrigger;
   eventTriggerType eventTrigger;
   eventDirType facing;
   //raceType race;
   RACE_ID raceID;
   //classType charClass;
   //CLASS_ID classID;
   CLASS_BASECLASS_ID classBaseclassID;
   genderType gender; // stored as attribute
   ITEM_ID itemID;
   int quest;
   int chance;
   int specialItem;
   int specialKey;
   //int npc;
   CHARACTER_ID characterID;
   int partyX, partyY;
   //GLOBAL_SPELL_ID memorizedSpellDbKey;
   SPELL_ID memorizedSpellID;
   DWORD memorizedSpellClass;
   DWORD memorizedSpellLevel;
   CString gpdlData;
   BOOL gpdlIsBinary;

   A_ASLENTRY_L eventcontrol_asl;
   A_ASLENTRY_L temp_asl;   
};

enum TASKTYPE;
enum TASKMESSAGE;

class CR_EVENT_INFO;


class TEXT_EVENT_DATA;
struct SELECTION_PAGE_FORMAT;

void die(int);

class GameEvent //: public CObject
{
  friend class GameEventList;
public:
//  DECLARE_SERIAL( GameEvent )
  GameEvent();
  GameEvent(const GameEvent& src);
  GameEvent(const GameEvent *src);

  virtual ~GameEvent() {  }
  virtual GameEvent& operator =(const GameEvent& src);
#ifdef UAFEDITOR
          bool Equals(const GameEvent& src) const;
          virtual bool operator ==(const GameEvent& src) const;
  CString Name(void) const;
  virtual void Export(JWriter& jw);
          void ExportEvent(JWriter& jw);
  virtual bool Import(JReader& jr);
          bool ImportEvent(JReader& jr);

#else
  const char *Name(void) const;
#endif

  virtual void Serialize( CArchive& archive, double version );
  virtual void Serialize( CAR& archive, double version );
  virtual void PrepareForRestore(void);
  virtual void Restore(CArchive& ar);
  virtual void CommitRestore(void);
  virtual void PreSerialize(BOOL IsStoring) { }
  virtual void PostSerialize(BOOL IsStoring, double version);

  void Clear();
  virtual BOOL IsChained();

#ifdef UAFEDITOR
  virtual BOOL GetChainedEvent(DWORD &id);
#endif

  virtual void GetChainCount(int &curr, int &total);

  // not likely to need over-ride
#ifdef UAFEngine
  virtual BOOL EventShouldTrigger();
#endif
  virtual void FreeEventResources();
  virtual void SaveEventResources(int level);
  virtual void SaveEventSounds(int level) {  };

#if (defined UAFEDITOR)
  // helper functions for CEventViewer
  virtual void GetEVRootText(char *pText) { strcpy(pText, GetEventIdDescription(id,GetEventSource())); }
  virtual BOOL GetEVChainText(int num, char *text);
  virtual BOOL GetEVChain(int num, DWORD &ID) { return (num==0)&&GameEvent::GetChainedEvent(ID); }
  virtual void ChainToEventId(int num, DWORD id);  
  virtual int UnchainToEventId(int num, DWORD id);
  virtual BOOL Validate(FILE *pFile); 
  void    CreateCRReference           (CR_EVENT_INFO *pCREI);
  void    CrossReference              (CR_EVENT_INFO *pCREI);
  virtual void DetailedCrossReference (CR_EVENT_INFO *pCREI);
  void    CrossReferenceEventItems    (CR_EVENT_INFO *pCREI, ITEM_LIST *itemList);
  void    CrossReferenceEventItems    (CR_EVENT_INFO *pCREI, SPECIAL_OBJECT_EVENT_LIST *itemList);
  void    CrossReferenceEventSound    (CR_EVENT_INFO *pCREI, const CString& sound);
  void    CrossReferenceEventChain    (CR_EVENT_INFO *pCREI, DWORD eventID);
  void    CrossReferenceEventSpells   (CR_EVENT_INFO *pCREI, spellBookType *pSpellBook);
#endif
  
  virtual BOOL WaitUntilReturnPressed() { return TRUE; }
  virtual BOOL ForcePartyBackup() { return FALSE; }
  virtual BOOL AllowDestEventExecute() { return TRUE; }
  
  EVENT_CONTROL &GetEventControl() { return control; }
  const eventType &GetEventType() const { return event; }
  const virtual char *GetEventTypeName(void) const {return Name();};
  DWORD &GetEventId()  { return id; }
  PIC_DATA &GetEventPic()  { return pic; }
  PIC_DATA &GetEventPic2()  { return pic2; }
#ifdef UAFEngine
  virtual CString &GetEventText();
  CString &GetEventText2()  { return text2; }
  CString &GetEventText3()  { return text3; }
  CString FindEventAttribute(const CString& name) const;
#endif
  void SetEventText(const CString& t) {text = t;};
  int &GetEventX()  { return x; }
  int &GetEventY()  { return y; }

  //void SetGlobalEvent(BOOL flag) { IsGlobal=flag; }
  void SetEventSource(EventSourceType type) { m_source = type; }
  //BOOL IsGlobalEvent() const { return IsGlobal; }
  EventSourceType GetEventSource() const { return m_source; }
  BOOL IsAChainEvent() const { return ((x == -1) && (y == -1)); }
  BOOL IsValidEvent() const { return ((event != NoEvent) && (id > 0)); }
  CString getGlobalEventMessage(CString defaultMessage, CString hookName, HOOK_PARAMETERS hookParameters, CString attributeName, EVENT_CONTROL control);
#ifdef UAFEngine  
  BOOL IsGlobalEvent() const { return (m_source==GlobalEventSrc); }
  GameEvent *EventType2EventPointer (eventType evType, unsigned int id);
  void StandardMenuKeyboardAction(key_code key, char ascii);
  void StandardKeyboardShortcutAction(key_code key, char ascii);
  void HMenuVPartyKeyboardAction(key_code key, char ascii);
  void HMenuVItemsKeyboardAction(key_code key, 
                                 char ascii, 
                                 void (*prev)(void), 
                                 void (*next)(void));
  void HMenuVInventoryKeyboardAction(key_code key, char ascii, void (*prev)(void), void (*next)(void));
  void VMenuHPartyKeyboardAction(key_code key, char ascii);
  void StandardMenuDraw(void);
  void Invalidate(void); // Mark to call OnDraw.
  void Draw(void); // Simply calls taskList::Draw(this);
  void PopEvent(void);

  // for levelData.eventData events
  void ReplaceEvent(GameEvent *event, EventDisposition disposition);
  void PushEvent   (GameEvent *event, EventDisposition disposition);
  void InsertEvent (GameEvent *event, EventDisposition disposition);
  void ReplaceEvent(DWORD id);
  void PushEvent   (DWORD id);
  void InsertEvent (DWORD id);

  // for globalData.eventData events
  void ReplaceGlobalEvent(GameEvent *event, EventDisposition disposition);
  void PushGlobalEvent   (GameEvent *event, EventDisposition disposition);
  void InsertGlobalEvent (GameEvent *event, EventDisposition disposition);
  void ReplaceGlobalEvent(DWORD id);
  void PushGlobalEvent   (DWORD id);
  void InsertGlobalEvent (DWORD id);

  virtual BOOL ShouldSimulateEnterKey();
  void SetMyState(TASKSTATE state);
  void TaskMessage(TASKMESSAGE msg);
  TASKSTATE GetMyState(void);
  void ChainHappened(void);
  void ChainOrQuit(DWORD chainID);
  void MouseMenu(int x, int y); // See if mouse intersects menu item.
  void MouseParty(int x, int y);  // See if mouse intersects party member.
  bool TABParty(key_code key); //Tab active Character to next party member.
  bool MouseInventory(int x, int y); // See if mouse intersects inventory
  void MouseMenuInventory(int x, int y);
  void MouseCombatMapMenu(int x, int y);
  void MouseCombatMap(int x, int y);
  bool SaveGame(void);
  bool LoadGame(void);
  bool HandleTransfer(TRANSFER_DATA transferData,
                      CString msg,
                      PIC_DATA pic,
                      CString Hook,
                      BOOL destroyDrow,
                      bool draw);

  bool CheckForMultiBoxText(void);
  void UpdatePartyMovementData(void);
  void CheckSecretDoors(void);

  virtual void OnEnterQueue(void);
  virtual void OnReturnToTopOfQueue(void);

  // Reply 'true' if you processed the timer, false otherwise
  virtual bool OnTimer(UINT timerId);
  void SetEventTimer(UINT timerId, UINT delay);

  // Reply 'true' if your trigger condition is met.  We will then
  // call either OnInitialEvent or OnChainNotHappen, depending
  // on your response.  
  // The default is to allow a forward step to proceed if the
  // event is successfully triggered.  An event that has
  // no player interaction should override this behavior.
  // Also, a TRANSFER event with 'PartyAttemptsEntry' should
  // override this behavior.
  virtual bool OnTestTrigger(bool allowForwardStep=true);

  // Reply 'true' if you are an event that should happen before
  // the party enters your square.  (Only Transfer Event, I think).
  // This function may be called before you are initialized.
  virtual bool OnTestPreMove(void) { return false;};

  // OnInitialEvent is not called when you are put on the queue.
  // It is called just after we call OnTestTrigger and you reply with 'true'.  
  virtual void OnInitialEvent(void); // m_state automatically set to TASK_initialized.

  // OnLeaveQueue is called when you replace yourself, pop yourself,
  // or when the program terminates.  You may not be at the top
  // of the queue in the last case.  You should not add anything
  // to the queue as you leave.  Cleanup only!
  virtual void OnLeaveQueue(void);

  // OnIdle is called when there is no input to process.
  virtual bool OnIdle(void); // Return true if waiting for input

  // OnFilterKeypress default:
  // If not multi-box or all text shown do nothing.
  // If CarriageReturn - Increment text - OnDraw()
  // If space -  End Text - OnDraw()
  // Return should be true if more keys to follow.
  // Filter should not change the event state (Pop, Push, etc).
  virtual bool OnFilterKeypress(key_code *key, char *ascii);
  virtual void OnKeypress(key_code key, char ascii);
  virtual void OnMouseMove(int x, int y);
  virtual void OnMouseClickLeft(int x, int y);
  virtual void OnInvalidate(void);
  virtual void OnDraw(void); // Draw the screen.
  virtual bool OnCycle(void); // Called one per scheduler cycle after all else
  virtual void OnUpdateUI(void); // called to let event prepare menu options  

  // OnTaskMessage is called when an event want to notify its
  // parents of some special occurance.  For example, the TRANSFER
  // event needs to let the MOVE event know not to do events that
  // may have disappeared when the level changed.
  // OnTaskMessage will be called in any event state except TASK_Deleted.
  virtual unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);

  // OnChainNotHappened is called when your OnTriggerTest has returned
  // a false.  You should pop yourself off the stack or replace
  // yourself with the event that occurs when you "don't happen".
  virtual void OnChainNotHappened();

  // Default is to crash and burn!  Some events cannot 
  // save themselves because they contain pointers or
  // other data that is not saved.  You should override
  // this only if you know how to save and restore yourself
  // from saveArea.
  virtual int OnSaveGame(unsigned int *saveArea); // Return # used
  virtual int OnLoadGame(unsigned int *saveArea); // Return # used
  void RunEventScript(LPCSTR scriptname);
#endif  // UAFEngine

  eventType event;
  DWORD id;
  int x, y;
  EVENT_CONTROL control;
  PIC_DATA pic;
  PIC_DATA pic2;
#ifdef UAFEngine
private:
#endif
  CString text;
  CString text2;
  CString text3;
#ifdef UAFEngine
public:
#endif
  DWORD chainEventHappen;
  DWORD chainEventNotHappen;

  LONGLONG timer;  // virtualGameTime at which an OnTimer event should be posted. (or zero).
  UINT timerID;     

  //BOOL IsGlobal; // member of level events, or global events?
  EventSourceType m_source;
  A_ASLENTRY_L event_asl;
  A_ASLENTRY_L temp_asl;
};


class NULL_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( NULL_EVENT_DATA )
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "NULL_EVENT_DATA";};
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
#endif
};



class CR_LIST;

// one per level
class GameEventList //: public CObject
{
public:
//  DECLARE_SERIAL( GameEventList )

  GameEventList() { m_level=0; Clear(); }
  //GameEventList(int level) { m_level = level; Clear(); }
  virtual ~GameEventList() { Clear(); }

  GameEventList& operator =(GameEventList& src);
#ifdef UAFEDITOR
  bool operator ==(GameEventList& src);
#endif
  POSITION GetHeadPosition() const { return m_events.GetHeadPosition(); }



        GameEvent* GetAtPos(POSITION p)        { return m_events.GetAtPos(p); }
  const GameEvent* PeekAtPos(POSITION p) const { return m_events.PeekAtPos(p); }
        GameEvent* GetNext(POSITION &p)        { return m_events.GetNext(p); }
  const GameEvent* PeekNext(POSITION &p) const { return *m_events.PeekNext(p); }
  //GameEvent* GetPrev(POSITION &p) { return m_events.GetPrev(p); }


  void Clear();
  int GetCount() const {return m_events.GetCount(); }

#ifdef UAFEDITOR
  DWORD GetLastEventInNormalChain(DWORD SrcId);
  GameEvent* CreateEventCopy(GameEvent *pSrcEvent);
  void CopyEvent(DWORD &RootId, DWORD SrcId, GameEventList *pSrcList);
  BOOL IsEventTypeUsed(eventType type);
  BOOL Validate();
  void CrossReference(CR_EVENT_INFO *pCREI);
  void LogEventText();
#endif
  // called when loading from scripts, or
  // when adding from editor GUI
  int AddEvent(GameEvent *data);

  // called when loading binary monster data that
  // was saved with key info
  int AddEventWithCurrKey(GameEvent *data);

  GameEvent *GetFirstEvent(int x, int y);
//  GameEvent *FindMarker(const CString& markerName, int thisChainRoot, int *markerRoot, int *markerID);
  GameEvent *GetEvent(DWORD id);
  BOOL IsChained(GameEvent *event);
#ifdef UAFEDITOR
  BOOL GetChainedEvent(GameEvent **event /* , BOOL CheckResult*/);
  CONFIG_ITEM_STATUS Export(JWriter& jw);
  CONFIG_ITEM_STATUS Import(JReader& jr);
  CONFIG_ITEM_STATUS ImportEvent(JReader& jr);
#endif
  BOOL IsValidEvent(DWORD num);
  BOOL IsValidEventChainWord(DWORD num);
  BOOL IsValidEvent(GameEvent *event) const; 
#ifdef UAFEDITOR
  void BuildMarkerIndex(void);
#endif
  void Serialize( CArchive& archive, double version );
  void Serialize( CAR& archive, double version );
  void Restore(CArchive& ar);
  void Restore(CAR& car);
  void CommitRestore(void);
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double version);


  GameEvent *CreateNewEvent(int type);

  void saveUsedEventArt(int level);
  void saveUsedEventSounds(int level);

  void DeleteEvents(GameEvent *event);
  void DeleteEvents(DWORD id);
  // deletes event and all custom chains,
  // but not the normal chains to allow
  // deletion of single event from chain
  void DeleteEvent(GameEvent *event);

  void markEventHappened(GameEvent *event);
  BOOL HasEventHappened(DWORD id);

  EVENT_CONTROL &GetEventControl(DWORD id) ;  
  const eventType &GetEventType(DWORD id) const;  
  PIC_DATA &GetEventPic(DWORD id);  
  PIC_DATA &GetEventPic2(DWORD id);  
  CString &GetEventText(DWORD id);  
  CString &GetEventText2(DWORD id);  
  CString &GetEventText3(DWORD id);  
  int &GetEventX(DWORD id);  
  int &GetEventY(DWORD id);  

  //void SetGlobalEvent(BOOL flag) { IsGlobal=flag; }
  //BOOL IsGlobalList() const { return IsGlobal; }
  void SetEventSource(EventSourceType type) { m_ListType = type; }
  EventSourceType GetEventSource() { return m_ListType; }

  int m_level;
  //BOOL IsGlobal; // list of level events, or global events?
  EventSourceType m_ListType; // level, global, or copy

private:
  //POSITION pos;
  CCriticalSection m_CS;
  NULL_EVENT_DATA NullEvent; //GameEvent NullEvent;
  OrderedQueue<GameEvent*, MAX_EVENTS> m_events;

  DWORD GetNextKey();
  BOOL ValidEventNum(DWORD num) const;
  void DeleteEventAndAllChains(GameEvent *event);
  DWORD m_nextKey;
};



class PLAY_MOVIE_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( PLAY_MOVIE_DATA )

  // fullscreen, in big pic window, or small pic window (viewport)
  enum { movieFull=0, movieBig=1, movieSmall=2 };

  PLAY_MOVIE_DATA() : GameEvent() { Clear(); event=PlayMovieEvent; }
  PLAY_MOVIE_DATA(const char *fname, int mode) : GameEvent() 
    { Clear(); event=PlayMovieEvent; m_filename=fname; m_mode=mode; }
  virtual ~PLAY_MOVIE_DATA() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  void SaveEventResources(int level);
  PLAY_MOVIE_DATA& operator =(const PLAY_MOVIE_DATA& src);
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    bool OnIdle(void);
#endif
#ifdef UAFEDITOR
    void DetailedCrossReference(CR_EVENT_INFO *pCREI);
    void Export(JWriter& jw);
    bool Import(JReader& jr);
    bool operator ==(const GameEvent& src) const;
#endif

    
  CString m_filename;
  int m_mode;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "PLAY_MOVIE_DATA";};
#endif
};

class ADD_NPC_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( ADD_NPC_DATA )
  ADD_NPC_DATA() : GameEvent() { Clear(); event=AddNpc; }
  virtual ~ADD_NPC_DATA() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  ADD_NPC_DATA& operator =(const ADD_NPC_DATA& src);
#ifdef UAFEngine
  BOOL EventShouldTrigger();
#endif
#ifdef UAFEDITOR
    void DetailedCrossReference(CR_EVENT_INFO *pCREI);
    void Export(JWriter& jw);
    bool Import(JReader& jr);
    bool operator ==(const GameEvent& src) const;
#endif
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
#endif
  eventDistType distance;
  //int charAdded;
  CHARACTER_ID characterID;
  int hitPointMod;
  BOOL useOriginal;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ADD_NPC_DATA";};
#endif
};

class REMOVE_NPC_DATA : public GameEvent
{
public:
//   DECLARE_SERIAL( REMOVE_NPC_DATA )
   REMOVE_NPC_DATA() : GameEvent() { Clear(); event=RemoveNPCEvent; }
   virtual ~REMOVE_NPC_DATA() {  }
   void Clear();
   void Serialize(CArchive &ar, double version);
   void Serialize(CAR &ar, double version);
   REMOVE_NPC_DATA& operator =(const REMOVE_NPC_DATA& src);
   BOOL EventShouldTrigger();
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
#endif
#ifdef UAFEDITOR
    void DetailedCrossReference(CR_EVENT_INFO *pCREI);
    void Export(JWriter& jw);
    bool Import(JReader& jr);
    bool operator ==(const GameEvent& src) const;
#endif

   eventDistType distance;
   //int charRemoved;
   CHARACTER_ID characterID;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "REMOVE_NPC_DATA";};
#endif
};

class CAMP_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( CAMP_EVENT_DATA )
  CAMP_EVENT_DATA() : GameEvent() { Clear(); event=Camp; }
  virtual ~CAMP_EVENT_DATA() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  CAMP_EVENT_DATA& operator =(const CAMP_EVENT_DATA& src);
  BOOL ForcePartyBackup() { return ForceExit; }

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnReturnToTopOfQueue(void);
    void OnKeypress(key_code key, char ascii) { /* ignore all input */ }
    void OnMouseClickLeft(int mouseX, int mouseY) { /*no input allowed*/ }
    int OnSaveGame(unsigned int *saveArea) { return 0; }
    int OnLoadGame(unsigned int *saveArea) { return 0; }
#endif
#ifdef UAFEDITOR
    void DetailedCrossReference(CR_EVENT_INFO *pCREI);
    void Export(JWriter& jw);
    bool Import(JReader& jr);
    bool operator ==(const GameEvent& src) const;
#endif

  BOOL ForceExit;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CAMP_EVENT_DATA";};
#endif
};

class COMBAT_TREASURE : public GameEvent
{
  //GLOBAL_SPELL_ID m_detectSpellKey;
public:
//  DECLARE_SERIAL( COMBAT_TREASURE )
  COMBAT_TREASURE() : GameEvent() { Clear(); event=CombatTreasure;}
  virtual ~COMBAT_TREASURE() {  }
  void Clear();
//  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  COMBAT_TREASURE& operator =(const COMBAT_TREASURE& src);
#ifdef UAFEngine
    void OnInitialEvent(void);
    bool OnIdle(void);
    //void OnKeypress(key_code key, char ascii);
    //void OnReturnToTopOfQueue(void);
    //void OnUpdateUI(void);
    //void OnLeaveQueue(void);
    bool OnTestTrigger(bool allowForwardStep);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

//  BOOL SilentGiveToActiveChar;

  // original quantities
  ITEM_LIST items;
  MONEY_SACK money;

//  // temp storage used when event is in progress
//  MONEY_SACK moneyRemain;
//  ITEM_LIST itemsRemain;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_TREASURE";};
#endif
};

class COMBAT_EVENT_DATA : public GameEvent
{
public:
//   DECLARE_SERIAL( COMBAT_EVENT_DATA )
   COMBAT_EVENT_DATA();
   virtual ~COMBAT_EVENT_DATA();
   void Clear();

   void Serialize(CArchive &ar, double version);
   void Serialize(CAR &ar, double version);

#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
#endif

   COMBAT_EVENT_DATA& operator =(const COMBAT_EVENT_DATA& src);
   void SaveEventSounds(int level);

#ifdef UAFEDITOR
   BOOL Validate(FILE *pFile);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnKeypress(key_code key, char ascii);
    void OnEnterQueue(void);
    bool OnTimer(UINT timerId);
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnUpdateUI(void);
    unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);
    void OnReturnToTopOfQueue(void);
    void ChangeToTaskState(TASKSTATE state);
    void OnMouseClickLeft(int x, int y);
    void OnLeaveQueue(void);
    bool OnIdle(void);
    bool OnCycle(void);
    CString ListAdjacentCombatants(int currCombatant);
    void CheckForSpellLingeringScripts(void);
#endif

   BOOL m_UseOutdoorMap; // set at run-time as event activates
   eventDistType distance;
   eventDirType direction;
   eventSurpriseType m_eSurprise;
   eventSurpriseType m_eTEMPsurprise;
   BOOL autoApproach;
   BOOL outdoors;
   BOOL noMonsterTreasure;
   BOOL partyNeverDies;
   BOOL partyNoExperience;
   BOOL noMagic;
   BOOL randomMonster;
   CString DeathSound;
   CString MoveSound;
   CString TurnUndeadSound;
   int monsterMorale;
   eventTurnUndeadModType turningMod;
   MONSTER_EVENT_DATA monsters;
   BACKGROUND_SOUND_DATA bgSounds;
   BOOL needDeathSprite;
   BOOL needDyingSprite;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_EVENT_DATA";};
#endif
#ifdef UAFEngine
  bool  m_stoppedSounds; // Runtime only.  Not serialized.
  key_code m_curKey;     // Runtime only.  Not Serialized
#endif
};

class GUIDED_TOUR : public GameEvent
{
public:
//  DECLARE_SERIAL( GUIDED_TOUR )
    GUIDED_TOUR() : GameEvent() { Clear(); event=GuidedTour; }
  virtual ~GUIDED_TOUR() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  GUIDED_TOUR& operator =(const GUIDED_TOUR& src);
  BOOL AllowDestEventExecute() { return executeEvent; }
  
  CString &GetEventText()  { return steps[currStep].text; }
  eventStepType GetEventStep() 
    { if (currStep < MAX_TOUR_STEPS) return steps[currStep].step; 
      else return TStep_NoMove; }
  eventStepType GetEventPrevStep() 
    { if (((currStep-1) < MAX_TOUR_STEPS) && ((currStep-1) >= 0)) return steps[currStep-1].step; 
      else return TStep_NoMove; }
  
  BOOL TourOver() 
  { return ((currStep >= MAX_TOUR_STEPS) || (GetEventStep() == TStep_NoMove)); }


#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    bool OnTimer(UINT timerId);
    void TakeNextStep();
    void OnReturnToTopOfQueue(void);
    void OnEnterQueue(void);
    void OnLeaveQueue(void);
#endif
#ifdef UAFEDITOR
    void DetailedCrossReference(CR_EVENT_INFO *pCREI);
    void Export(JWriter& jw);
    bool Import(JReader& jr);
    bool operator ==(const GameEvent& src) const;
#endif

  int tour_x,tour_y;  // start location
  int facing; // start facing
  BOOL executeEvent;
  BOOL useStartLoc;
  int currStep;
  TOUR_STEP steps[MAX_TOUR_STEPS];
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GUIDED_TOUR";};
#endif
  BOOL  guidedTourMoving; //temporary; Not Serialized
};

class ENCOUNTER_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( ENCOUNTER_DATA )
  ENCOUNTER_DATA() : GameEvent(),combatChain(0),talkChain(0),escapeChain(0) { Clear(); event=EncounterEvent; }
  virtual ~ENCOUNTER_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  ENCOUNTER_DATA& operator =(const ENCOUNTER_DATA& src);
  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int UnchainToEventId(int num, DWORD id);  
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnUpdateUI(void);
#endif

  BYTE currDist;
  eventDistType distance;
  int monsterSpeed;
  encounterButtonResultType zeroRangeResult;
  DWORD combatChain;
  DWORD talkChain;
  DWORD escapeChain;
  long Unused;   
  ENCOUNTER_BUTTON_DATA buttons;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ENCOUNTER_DATA";};
#endif
};

class PASSWORD_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( PASSWORD_DATA )
  PASSWORD_DATA() : GameEvent(), successChain(0),failChain(0) { Clear(); event=EnterPassword; }
  virtual ~PASSWORD_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  PASSWORD_DATA& operator =(const PASSWORD_DATA& src);
  BOOL IsChained();
  void GetChainCount(int &curr, int &total);  

  // store/retrieve new event params into attribute map
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double version);
  BOOL PasswordMatches(CString typed);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
  BOOL Validate(FILE *pFile);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
#endif

  CString password; 
  passwordActionType successAction;
  passwordActionType failAction;
  passwordMatchType matchCriteria;
  BOOL matchCase;
  int nbrTries;
  int currTry;
  long Unused;  
  DWORD successChain;
  DWORD failChain; 
  TRANSFER_DATA successTransferData;
  TRANSFER_DATA failTransferData;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "PASSWORD_DATA";};
#endif
};

class GAIN_EXP_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( GAIN_EXP_DATA )
  GAIN_EXP_DATA() : GameEvent() { Clear(); event=GainExperience; }
  virtual ~GAIN_EXP_DATA() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  GAIN_EXP_DATA& operator =(const GAIN_EXP_DATA& src);
  void SaveEventSounds(int level);
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  CString sound;
  int hSound;
  int experience; 
  int tempExp;
  int chance;
  eventPartyAffectType who;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GAIN_EXP_DATA";};
#endif
};

class GIVE_TREASURE_DATA : public GameEvent
{
  //GLOBAL_SPELL_ID m_detectSpellKey;
#ifdef SpellDetectMAgic
  SPELL_ID m_detectSpellID;
#endif
public:
//  DECLARE_SERIAL( GIVE_TREASURE_DATA )
  GIVE_TREASURE_DATA() : GameEvent() { Clear(); event=GiveTreasure;SilentGiveToActiveChar=FALSE; }
  virtual ~GIVE_TREASURE_DATA() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  GIVE_TREASURE_DATA& operator =(const GIVE_TREASURE_DATA& src);
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnReturnToTopOfQueue(void);
    void OnUpdateUI(void);
    void OnLeaveQueue(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BOOL SilentGiveToActiveChar;

  // original quantities
  ITEM_LIST items;
  MONEY_SACK money;

  // temp storage used when event is in progress
  MONEY_SACK moneyRemain;
  ITEM_LIST itemsRemain;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GIVE_TREASURE_DATA";};
#endif
};

class GIVE_DAMAGE_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( GIVE_DAMAGE_DATA )
  GIVE_DAMAGE_DATA() : GameEvent() { Clear(); event=Damage; }
  virtual ~GIVE_DAMAGE_DATA() {  }
  void Clear();               
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  GIVE_DAMAGE_DATA& operator =(const GIVE_DAMAGE_DATA& src);
#ifdef UAFEngine
  void OnInitialEvent(void);
  void OnKeypress(key_code key, char ascii);
  void OnDraw(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  int nbrAttacks;
  int chancePerAttack;
  int dmgDice;
  int dmgDiceQty;
  int dmgBonus;
  int saveBonus;
  int attackTHAC0;

  spellSaveEffectType eventSave;

  spellSaveVersusType spellSave;

  eventPartyAffectType who;

  eventDistType distance;

#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GIVE_DAMAGE_DATA";};
#endif
};

class NPC_SAYS_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( NPC_SAYS_DATA )
  NPC_SAYS_DATA() : GameEvent() { Clear(); event=NPCSays; }
  virtual ~NPC_SAYS_DATA() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  NPC_SAYS_DATA& operator =(const NPC_SAYS_DATA& src);
  void SaveEventSounds(int level);
  BOOL EventShouldTrigger();
  BOOL WaitUntilReturnPressed() { return mustHitReturn; }
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnUpdateUI(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  CString sound;
  int hSound;
  BOOL mustHitReturn;
  BOOL highlight;
  eventDistType distance;  
  //int partySeesChar;
  CHARACTER_ID characterID;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "NPC_SAYS_DATA";};
#endif
};

class QUESTION_YES_NO : public GameEvent
{
public:
//  DECLARE_SERIAL( QUESTION_YES_NO )
  QUESTION_YES_NO() : GameEvent(),YesChain(0),NoChain(0) { Clear();event=QuestionYesNo; }
  virtual ~QUESTION_YES_NO() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  QUESTION_YES_NO& operator =(const QUESTION_YES_NO& src);
  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnReturnToTopOfQueue();
    int OnSaveGame(unsigned int *saveArea);
    int OnLoadGame(unsigned int *saveArea);
#endif

  labelPostChainOptionsType YesChainAction;
  labelPostChainOptionsType NoChainAction; 
  DWORD YesChain;
  DWORD NoChain;
  long Unused;
  int sx,sy;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "QUESTION_YES_NO";};
#endif
};

class QUESTION_LIST_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( QUESTION_LIST_DATA )
  QUESTION_LIST_DATA() : GameEvent() { Clear();event=QuestionList; }
  virtual ~QUESTION_LIST_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  QUESTION_LIST_DATA& operator =(const QUESTION_LIST_DATA& src);

  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int UnchainToEventId(int num, DWORD id);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnReturnToTopOfQueue();
    void OnUpdateUI(void);
    int OnSaveGame(unsigned int *saveArea);
    int OnLoadGame(unsigned int *saveArea);
#endif

  long UserResult;
  QLIST_DATA buttons;
  int sx,sy;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "QUESTION_LIST_DATA";};
#endif
};

class QUESTION_BUTTON_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( QUESTION_BUTTON_DATA )
  QUESTION_BUTTON_DATA() : GameEvent() { Clear(); event=QuestionButton;}
  virtual ~QUESTION_BUTTON_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  QUESTION_BUTTON_DATA& operator =(const QUESTION_BUTTON_DATA& src);

  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int UnchainToEventId(int num, DWORD id);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnReturnToTopOfQueue();
    void OnUpdateUI(void);
    int OnSaveGame(unsigned int *saveArea);
    int OnLoadGame(unsigned int *saveArea);
#endif

  long UserResult;
  QBUTTON_DATA buttons;
  int sx,sy;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "QUESTION_BUTTON_DATA";};
#endif
};

class TEXT_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( TEXT_EVENT_DATA )
  TEXT_EVENT_DATA(bool displaySpecialGraphics) : GameEvent() { Clear(); m_displaySpecialGraphics=displaySpecialGraphics; event=TextStatement;}
  virtual ~TEXT_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  TEXT_EVENT_DATA& operator =(const TEXT_EVENT_DATA& src);
  BOOL ForcePartyBackup() { return ForceBackup; }
  void SaveEventSounds(int level);
  // store/retrieve new event params into attribute map
  void PreSerialize(BOOL IsStoring);
  void PostSerialize(BOOL IsStoring, double version);

#ifdef UAFEngine
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
    void OnInitialEvent(void);
    void OnUpdateUI(void);
    BOOL WaitUntilReturnPressed();
    bool OnTimer(UINT timerID);
#endif
#ifdef UAFEDITOR
    void DetailedCrossReference(CR_EVENT_INFO *pCREI);
    void Export(JWriter& jw);
    bool Import(JReader& jr);
    bool operator ==(const GameEvent& src) const;
#endif

  BOOL WaitForReturn;
  BOOL HighlightText;
  BOOL ForceBackup;
  BOOL KillSound;
  CString sound;
  int hSound;
  eventDistType distance;  
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TEXT_EVENT_DATA";};
#endif
  bool m_displaySpecialGraphics;  // temporary; not serialized
};

enum VALUE_MODIFICATION
{
  MODIFICATION_ILLEGAL = 0,
  NO_CHANGE,
  SET_VARIABLE,
  INCREMENT_VARIABLE,
  DECREMENT_VARIABLE
};
const int NUM_VALUE_MODIFICATION_TYPES = 5;

enum ACTION
{
  ACTION_ILLEGAL = 0,
  ACTION_NONE,
  ACTION_GOTO,
  ACTION_CALL,
  ACTION_RETURN,
  ACTION_POP
};
const int NUM_ACTION_TYPES = 6;

enum ACTION_CONDITION
{
  CONDITION_ILLEGAL = 0,
  CONDITION_ALWAYS,
  CONDITION_EQUALS,
  CONDITION_NOTEQUALS
};
const int NUM_ACTION_CONDITION_TYPES = 4;

enum FLOW_CONTROL_FLAGS
{
  FCF_LocalChainOnly = 0x00000001,
};

class FLOW_CONTROL_EVENT_DATA : public GameEvent
{
public:
  FLOW_CONTROL_EVENT_DATA() : GameEvent() { Clear(); event = FlowControl; }
  virtual ~FLOW_CONTROL_EVENT_DATA() { }
  void Clear();
  //void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  FLOW_CONTROL_EVENT_DATA& operator =(const FLOW_CONTROL_EVENT_DATA& src);

#ifdef UAFEngine
//  void OnKeypress(key_code key, char ascii);
//  void OnDraw(void);
  void OnInitialEvent(void);
//  void OnUpdateUI(void);
//  BOOL WaitUntilReturnPressed();
#endif
#ifdef UAFEDITOR
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

  CString entryMarkerName;
  CString exitMarkerName;
  CString destinationMarkerName;
  CString globalVariableName;
  CString value;
  DWORD   flags;  // FLOW_CONTROL_FLAGS
  DWORD   destID; // The editor ignores this when reading a level.
                  // THe editor computes this at the last instant
                  //    when writing a level
                  // The editor computes this to save the engine
                  //    from having to do it.
  VALUE_MODIFICATION valueModification;
  ACTION_CONDITION   actionCondition;
  ACTION action;

#ifdef TASKTRACE
  const char *GetEventTypeName(void) { return "FLOW_CONTROL_EVENT_DATA"; };
#endif
};

class TRANSFER_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( TRANSFER_EVENT_DATA )
  TRANSFER_EVENT_DATA() : GameEvent() { Clear();event=TransferModule; }
  virtual ~TRANSFER_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  TRANSFER_EVENT_DATA& operator =(const TRANSFER_EVENT_DATA& src);
  BOOL AllowDestEventExecute() { return transferData.execEvent; }
  BOOL IsAutoTeleportEvent() { return (!askYesNo); }

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    bool OnIdle(void);
    void OnKeypress(key_code key, char ascii);
    unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);
    bool OnTestPreMove(void) { return activateBeforeEntry==TRUE;};
    bool OnTestTrigger(bool allowForwardStep);
#endif
#ifdef UAFEDITOR
    BOOL Validate(FILE *pFile);
    void DetailedCrossReference(CR_EVENT_INFO *pCREI);
    void Export(JWriter& jw);
    bool Import(JReader& jr);
    bool operator ==(const GameEvent& src) const;
#endif

  BOOL askYesNo;
  BOOL transferOnYes;
  BOOL destroyDrow;
  BOOL activateBeforeEntry;
  TRANSFER_DATA transferData;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TRANSFER_EVENT_DATA";};
#endif
};

class WHO_PAYS_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( WHO_PAYS_EVENT_DATA )
  WHO_PAYS_EVENT_DATA() : GameEvent(),successChain(0),failChain(0) { Clear(); event=WhoPays;}
  virtual ~WHO_PAYS_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  WHO_PAYS_EVENT_DATA& operator =(const WHO_PAYS_EVENT_DATA& src);

  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
  BOOL Validate(FILE *pFile);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
#endif

  BOOL impossible;
  passwordActionType successAction;
  passwordActionType failAction;
  int gems;
  int jewels;
  int platinum;
  itemClassType moneyType; // which money type to take (plat,gold,etc)
  long Unused;
  DWORD successChain;
  DWORD failChain;
  TRANSFER_DATA successTransferData;
  TRANSFER_DATA failTransferData;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "WHO_PAYS_EVENT_DATA";};
#endif
};

class WHO_TRIES_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( WHO_TRIES_EVENT_DATA )
  WHO_TRIES_EVENT_DATA() : GameEvent(),successChain(0),failChain(0) { Clear(); event=WhoTries;}
  virtual ~WHO_TRIES_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  void CheckSkill(const char *name, BOOL *check);
  void CheckAbility(const char *name, BOOL *check);
  void CheckOldSkills(void);
  void CheckOldAbilities(void);
  WHO_TRIES_EVENT_DATA& operator =(const WHO_TRIES_EVENT_DATA& src);

  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
  BOOL Validate(FILE *pFile);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
#endif

  BOOL alwaysSucceeds;
  BOOL alwaysFails;
  BOOL checkSTR;
  BOOL checkINT;
  BOOL checkWIS;
  BOOL checkDEX;
  BOOL checkCON;
  BOOL checkCHA;
  BOOL checkPP;
  BOOL checkOL;
  BOOL checkFT;
  BOOL checkMS;
  BOOL checkHS;
  BOOL checkHN;
  BOOL checkCW;
  BOOL checkRL;
  BYTE strBonus;
  BOOL compareToDie; // if false, then compareDie is actual compare value
  passwordActionType successAction;
  passwordActionType failAction;
  int  compareDie;   // dice sides, or actual compare value
  int  NbrTries;
  int  currTry;
  long Unused;
  DWORD successChain;
  DWORD failChain;
  TRANSFER_DATA successTransferData;
  TRANSFER_DATA failTransferData;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "WHO_TRIES_EVENT_DATA";};
#endif
};

enum MathOperationType  { MO_NoOperation, MO_StoredIn, MO_AddedTo, MO_SubtractFrom };
const int MAX_MATH_OPERATION_TYPES = 4;

enum MultiItemCheckType { MIC_None, MIC_AllItems, MIC_AtLeastOneItem };
const int MAX_MULTI_ITEM_CHECK_TYPES = 3;

class UTILITIES_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( UTILITIES_EVENT_DATA )
  UTILITIES_EVENT_DATA() : GameEvent() { Clear(); event=Utilities;}
  virtual ~UTILITIES_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  UTILITIES_EVENT_DATA& operator =(const UTILITIES_EVENT_DATA& src);  

#ifdef UAFEngine
    void OnInitialEvent(void);
    bool OnIdle(void);
    void OnKeypress(key_code key, char ascii) { /*ignore all input*/ }
    void OnMouseClickLeft(int mouseX, int mouseY) { /*no input allowed*/ }
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BOOL endPlay;
  MathOperationType operation;
  MultiItemCheckType itemCheck;  
  BYTE mathItemType;
  BYTE resultItemType;
  WORD mathAmount;
  int  mathItemIndex;  
  int  resultItemIndex;
  SPECIAL_OBJECT_EVENT_LIST items;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "UTILITIES_EVENT_DATA";};
#endif
};

class SPECIAL_ITEM_KEY_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( SPECIAL_ITEM_KEY_EVENT_DATA )
  SPECIAL_ITEM_KEY_EVENT_DATA() : GameEvent() { Clear(); event=SpecialItem;}
  virtual ~SPECIAL_ITEM_KEY_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  SPECIAL_ITEM_KEY_EVENT_DATA& operator =(const SPECIAL_ITEM_KEY_EVENT_DATA& src);
  BOOL ForcePartyBackup() { return forceExit; }
  BOOL WaitUntilReturnPressed() { return WaitForReturn; }
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
    bool OnTestTrigger(bool allowForwardStep);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BOOL WaitForReturn;
  BOOL forceExit;
  SPECIAL_OBJECT_EVENT_LIST items;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SPECIAL_ITEM_KEY_EVENT_DATA";};
#endif
};


enum QuestAcceptType { QA_Impossible, QA_OnYes, QA_OnNo, QA_OnYesOrNo, 
                       QA_ImpossibleAuto, QA_AutoAccept };
const int MAX_QUEST_ACCEPT_TYPES = 6;


class QUEST_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( QUEST_EVENT_DATA )
  QUEST_EVENT_DATA() : GameEvent(),acceptChain(0),rejectChain(0) { Clear(); event=QuestStage;}
  virtual ~QUEST_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  QUEST_EVENT_DATA& operator =(const QUEST_EVENT_DATA& src);
  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    bool OnTestTrigger(bool allowForwardStep);
    void OnUpdateUI(void);
#endif

  QuestAcceptType accept;
  BOOL completeOnAccept;
  BOOL failOnRejection;
//-----
private: // PRS July 2009 - Encode quest type in top 4 bits (quest, item, key)
         // These four functions are sufficient to manipulate m_quest.
  int m_quest;
public:
  inline int EncodeQuestTypeAndID(QUEST_TYPE type, int questID){return ((type&7)<<28)|questID;};
  inline void SetQuestTypeAndID(QUEST_TYPE type, int questID)
  {
    if (questID < 0) 
    {
      m_quest = -1;
    }
    else
    {
      m_quest=EncodeQuestTypeAndID(type,questID);
    };
  };
//  inline void SetQuestTypeAndID(int taid){m_quest=taid;};
  inline QUEST_TYPE QuestType(void)
  {
    if (m_quest < 0) return NO_FLAG;
    if ((m_quest & 0x70000000) == 0) return QUEST_FLAG; // Default
    return (QUEST_TYPE)((m_quest>>28) & 0x7);
  };
  inline int QuestID(void){return m_quest&0xfffffff;};
//-----
  WORD stage;
  DWORD acceptChain;
  DWORD rejectChain;
  long Unused;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "QUEST_EVENT_DATA";};
#endif
};

class PASS_TIME_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( PASS_TIME_EVENT_DATA )
  PASS_TIME_EVENT_DATA() : GameEvent() { Clear();event=PassTime; }
  virtual ~PASS_TIME_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  PASS_TIME_EVENT_DATA& operator =(const PASS_TIME_EVENT_DATA& src);
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnReturnToTopOfQueue(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BOOL AllowStop;
  BOOL SetTime;
  BOOL PassSilent;
  BYTE Days;
  BYTE Hours;
  BYTE Minutes;
  BYTE daysLeftUnused;
  BYTE hoursLeftUnused;
  BYTE minutesLeftUnused;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "PASS_TIME_EVENT_DATA";};
#endif
};

class PASSING_TIME_EVENT_DATA : public GameEvent
{
public:
  PASSING_TIME_EVENT_DATA(__int64 days, __int64 hours, __int64 mins,BOOL allowstop) : GameEvent() 
  { 
    Clear();
    Days = days;
    Hours = hours;
    Minutes = mins;
    daysLeft = Days;
    hoursLeft = Hours;
    minutesLeft = Minutes;
    event = CONTROL_PassingTime;
    AllowStop=allowstop;
  }
  virtual ~PASSING_TIME_EVENT_DATA() { }
  void Clear();
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    bool OnCycle(void);
    void OnKeypress(key_code key, char ascii);
    //void OnMouseClickLeft(int x, int y) { /*no input allowed*/ }
#endif

  BOOL AllowStop;
  __int64 Days;
  __int64 Hours;
  __int64 Minutes;
  __int64 daysLeft;
  __int64 hoursLeft;
  __int64 minutesLeft;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "PASSIN_TIME_EVENT_DATA";};
#endif
};


class VAULT_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( VAULT_EVENT_DATA )
  VAULT_EVENT_DATA() : GameEvent() { Clear(); event=Vault;}
  virtual ~VAULT_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  VAULT_EVENT_DATA& operator =(const VAULT_EVENT_DATA& src);
  BOOL ForcePartyBackup() { return ForceBackup; }
#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnReturnToTopOfQueue(void);
    void OnUpdateUI(void);
    void OnLeaveQueue(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BYTE WhichVault;
  BOOL ForceBackup;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "VAULT_EVENT_DATA";};
#endif
};

enum TAVERN_TALES_FLAGS
{
  TTF_CUMULATIVE = 0x00000001,
  TTF_IF_DRINK   = 0x00000002,
  TTF_IF_DRUNK   = 0x00000004,
  TTF_REPLACE    = 0x00000008,
  TTF_HIGHLIGHT  = 0x00000010,
};

struct TAVERN_TALES_TALE
{
  TAVERN_TALES_TALE(void){ Clear();};
  ~TAVERN_TALES_TALE(void){ Clear();};
  void Clear(void){};
  void operator =(const TAVERN_TALES_TALE& src);
  bool operator ==(const TAVERN_TALES_TALE& src) const;
  void Export(JWriter& jw);
  void Import(JReader& jr);

  CString      m_tale;
  DWORD        m_flags;
  A_ASLENTRY_L m_asl;  // Just in case
#ifdef UAFEngine
  int count;
#endif
  void Serialize(CAR &ar, double version);
};

class TAVERN_TALES : public GameEvent
{
public:
  TAVERN_TALES() : GameEvent(){ Clear(); event=TavernTales;}
  virtual ~TAVERN_TALES() { /*Clear();*/ };
  void operator =(const TAVERN_TALES& src);
  void Clear(void);
  void Serialize(CAR &ar, double version);

#ifdef UAFEngine
void TAVERN_TALES::OnInitialEvent(void);
bool TAVERN_TALES::OnTestTrigger(bool allowForwardStep);
bool TAVERN_TALES::OnIdle(void);
#endif


#ifdef UAFEDITOR
  void Export(JWriter& jw);
  bool Import(JReader& jr);
#endif

  bool operator ==(const GameEvent& s) const;
  A_ASLENTRY_L m_asl; // Just in case.
  DWORD m_flags; // TAVERN_TALES_FLAGS
  mCArray<TAVERN_TALES_TALE, TAVERN_TALES_TALE&> m_tales;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Tale,               /* array data name      */ \
                                  TAVERN_TALES_TALE,  /* Search variable type */ \
                                  m_tale,             /* search variable name */ \
                                  m_tales,            /* array name           */ \
                                  TAVERN_TALES_TALE,  /* array data type      */ \
                                  TAVERN_TALES)       /* type of *this        */

#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TAVERN_TALES";};
#endif
};


class TAVERN : public GameEvent
{
public:
//  DECLARE_SERIAL( TAVERN )
  TAVERN() : GameEvent(),fightChain(0),drinkChain(0) { Clear(); event=TavernEvent;}
  virtual ~TAVERN() {  }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  TAVERN& operator =(const TAVERN& src);
  BOOL IsChained();
  void GetChainCount(int &curr, int &total);
  BOOL ForcePartyBackup() { return forceExit; }

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnLeaveQueue(void);
    void OnDraw(void);
    bool OnIdle(void);
    void OnKeypress(key_code key, char ascii);
    void OnUpdateUI(void);
#endif

	BOOL forceExit;
  BOOL allowFights;
  BOOL allowDrinks;
  BOOL EachTaleOnceOnly;
	int inflation;
	int barkeep;
  DWORD fightChain;
  DWORD drinkChain;
  int drinkPointTrigger;
  long Unused;
  taleOrderType taleOrder;
  TALE tales[MAX_TALES];
  DRINK drinks[MAX_DRINKS];
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TAVERN";};
#endif
};


class TEMPLE : public GameEvent
{
  //CHARACTER *pTempleCleric; // temp, used to cast temple spells
  //int tcKey;
  CHARACTER_ID characterID;
public:
//  DECLARE_SERIAL( TEMPLE )
  TEMPLE();
  TEMPLE(const TEMPLE &src);
  virtual ~TEMPLE();
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  TEMPLE& operator =(const TEMPLE& src);
  BOOL ForcePartyBackup() { return forceExit; }

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnDraw(void);
    void OnKeypress(key_code key, char ascii);
    void OnReturnToTopOfQueue();
    void OnUpdateUI(void);
    void OnLeaveQueue(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif


  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
#endif

  long m_state;
	BOOL forceExit;
  BOOL allowDonations;
  enum costFactorType costFactor;
  int maxLevel;
  int donationTrigger;
  int totalDonation;
  DWORD donationChain;
  spellBookType& templeSpells;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TEMPLE";};
#endif
};

struct TRAINABLE_BASECLASS
{
  BASECLASS_ID baseclassID;
  int minLevel;
  int maxLevel;
  CString whateverthosenutsmightwant;
  CAR& Serialize(CAR& car);
#ifdef UAFEDITOR
  void Export(JWriter& jw) const;
  bool Import(JReader& jr);
  bool operator ==(const TRAINABLE_BASECLASS& src) const; 
#endif
};

class TRAININGHALL : public GameEvent
{
public:
//  DECLARE_SERIAL( TRAININGHALL )
  TRAININGHALL() : GameEvent() { Clear(); event=TrainingHallEvent;}
  virtual ~TRAININGHALL() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  TRAININGHALL& operator =(const TRAININGHALL& src);
  BOOL ForcePartyBackup() { return forceExit; }

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
    void OnReturnToTopOfQueue();
    int  OnSaveGame(unsigned int *saveArea);
    int  OnLoadGame(unsigned int *saveArea);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

	BOOL forceExit;
  //BOOL TrainCleric;
  //BOOL TrainFighter;
  //BOOL TrainMagicUser;
  //BOOL TrainThief;
  //BOOL TrainRanger;
  //BOOL TrainPaladin;
  //BOOL TrainDruid;
  mCArray<TRAINABLE_BASECLASS, TRAINABLE_BASECLASS&> trainableBaseclasses;
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(TrainableBaseclass,    /* array data name      */ \
                                 BASECLASS_ID,          /* Search variable type */ \
                                 baseclassID,           /* search variable name */ \
                                 trainableBaseclasses,  /* array name           */ \
                                 TRAINABLE_BASECLASS,   /* array data type      */ \
                                 TRAININGHALL)          /* type of *this        */
	int Cost;  
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TRAININGHALL";};
#endif
};

class SHOP : public GameEvent
{
public:
//  DECLARE_SERIAL( SHOP )
  SHOP() : GameEvent() { Clear(); event=ShopEvent;}
  virtual ~SHOP() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  SHOP& operator =(const SHOP& src);
  BOOL ForcePartyBackup() { return forceExit; }

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
    void OnReturnToTopOfQueue(void);
    void OnUpdateUI(void);
    void OnLeaveQueue(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

	BOOL forceExit;
  costFactorType costFactor;
  int costToId;
  int buybackPercentage;
  BOOL buyItemsSoldOnly;
  int sellprice;
  BOOL canId;
  BOOL canApprGems;
  BOOL canApprJewels;
	ITEM_LIST itemsAvail;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SHOP";};
#endif
};

class SMALL_TOWN_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( SMALL_TOWN_DATA )
  SMALL_TOWN_DATA() : GameEvent(),TempleChain(0),TrainingHallChain(0),ShopChain(0),InnChain(0),TavernChain(0),VaultChain(0) { Clear();event=SmallTown; }
  virtual ~SMALL_TOWN_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  SMALL_TOWN_DATA& operator =(const SMALL_TOWN_DATA& src);

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
    void OnReturnToTopOfQueue(void);
    int OnSaveGame(unsigned int *saveArea) { return 0; }
    int OnLoadGame(unsigned int *saveArea) { return 0; }
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
#endif

  long Unused;
  DWORD TempleChain;
  DWORD TrainingHallChain;
  DWORD ShopChain;
  DWORD InnChain;
  DWORD TavernChain;
  DWORD VaultChain;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SMALL_TOWN_DATA";};
#endif
};

class RANDOM_EVENT_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( RANDOM_EVENT_DATA )
  RANDOM_EVENT_DATA() : GameEvent() { memset(EventChains,0,sizeof(EventChains)); Clear();event=RandomEvent; }
  virtual ~RANDOM_EVENT_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  RANDOM_EVENT_DATA& operator =(const RANDOM_EVENT_DATA& src);

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
#endif

  enum { MAX_RANDOM_EVENTS=14 };
  DWORD EventChains[MAX_RANDOM_EVENTS];
  BYTE EventChance[MAX_RANDOM_EVENTS];
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "RANDOM_EVENT_DATA";};
#endif
};

class CHAIN_EVENT : public GameEvent
{
public:
//  DECLARE_SERIAL( CHAIN_EVENT )
  CHAIN_EVENT() : GameEvent(),Chain(0) { Clear(); event=ChainEventType;}
  virtual ~CHAIN_EVENT() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  CHAIN_EVENT& operator =(const CHAIN_EVENT& src);

#ifdef UAFEngine
    void OnInitialEvent(void);
    bool OnTestTrigger(bool allowForwardStep);
    void OnKeypress(key_code key, char ascii) { /* ignore all input */ }
    void OnMouseClickLeft(int mouseX, int mouseY) { /*no input allowed*/ }
#endif

  BOOL IsChained();
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &id);
  void ChainToEventId(int num, DWORD id);
  int  UnchainToEventId(int num, DWORD id);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  bool Import(JReader& jr);
  bool operator ==(const GameEvent& src) const;
#endif

  DWORD Chain;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CHAIN_EVENT";};
#endif
};

class JOURNAL_EVENT : public GameEvent
{
public:
//  DECLARE_SERIAL( JOURNAL_EVENT )
  JOURNAL_EVENT() : GameEvent() { Clear(); event=JournalEvent;}
  virtual ~JOURNAL_EVENT() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  JOURNAL_EVENT& operator =(const JOURNAL_EVENT& src);
  BOOL EventShouldTrigger();

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii) { /* ignore all input */ }
    void OnMouseClickLeft(int mouseX, int mouseY) { /*no input allowed*/ }
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  int journal_entry;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "JOURNAL_EVENT";};
#endif
};

class SOUND_EVENT : public GameEvent
{
public:
//  DECLARE_SERIAL( SOUND_EVENT )
  SOUND_EVENT() : GameEvent() { Clear();event=Sounds; }
  virtual ~SOUND_EVENT() { }
  void Clear();
  void SaveEventSounds(int level); 
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  SOUND_EVENT& operator =(const SOUND_EVENT& src);
  BOOL operator ==(SOUND_EVENT& src)
  {
    if (sounds.GetCount()!=src.sounds.GetCount()) return FALSE;
    return TRUE;
  }

#ifdef UAFEngine
    void OnInitialEvent(void);
    bool OnIdle(void);
    void OnKeypress(key_code key, char ascii) { /*ignore all input*/ }
    void OnMouseClickLeft(int mouseX, int mouseY) { /*no input allowed*/ }
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif


  CList<CString, CString&> sounds;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SOUND_EVENT";};
#endif
};

class TAKE_PARTY_ITEMS_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( TAKE_PARTY_ITEMS_DATA )
  TAKE_PARTY_ITEMS_DATA() : GameEvent() { Clear(); event=TakePartyItems;}
  virtual ~TAKE_PARTY_ITEMS_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  TAKE_PARTY_ITEMS_DATA& operator =(const TAKE_PARTY_ITEMS_DATA& src);
  BOOL WaitUntilReturnPressed() { return mustHitReturn; }

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
    void OnUpdateUI(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

  BOOL StoreItems; // put taken items in vault
  BYTE WhichVault;
  BOOL mustHitReturn;
  BYTE takeItems; // flag that indicates item types to take
  takeItemsAffectsType takeAffects; // who gets taken from

  takeItemQtyType itemSelectFlags; // random, percent, specified, all
  takeItemQtyType platinumSelectFlags;
  takeItemQtyType gemsSelectFlags;
  takeItemQtyType jewelrySelectFlags;
  
  // what to take if percent or specified
  int platinum;
  int gems;
  int jewelry;
  int itemPcnt;
  itemClassType moneyType; // which money type to take (plat,gold,etc)
  ITEM_LIST items;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TAKE_PARTY_ITEMS_DATA";};
#endif
};

class HEAL_PARTY_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( HEAL_PARTY_DATA )
  HEAL_PARTY_DATA() : GameEvent() { Clear(); event=HealParty;}
  virtual ~HEAL_PARTY_DATA() { }
  void Clear();
  void Serialize(CArchive &ar, double version);
  void Serialize(CAR &ar, double version);
  HEAL_PARTY_DATA& operator =(const HEAL_PARTY_DATA& src);

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii);
    void OnDraw(void);
#endif
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   void Export(JWriter& jw);
   bool Import(JReader& jr);
   bool operator ==(const GameEvent& src) const;
#endif

    
  BOOL HealHP;
  BOOL HealDrain;
  BOOL HealCurse;
  BYTE chance;
  int  HowMuchHP;
  BYTE LiteralOrPercent; // 0=literal,1=percent in HowMuchHP
  eventPartyAffectType who;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "HEAL_PARTY_DATA";};
#endif
};

enum LOGIC_BLOCK_GATE_TYPE {
  LBGT_top = 0,// Copy top input
  LBGT_side,   // Copy side input
  LBGT_land,   // Logical AND .......  &&
  LBGT_lor,    // Logical OR  ........ ||
  LBGT_aplus,  // Arithmetic plus ...  +
  LBGT_aminus, // Arithmetic minus ... -
  LBGT_grep,   // String Grep ........ grep
  LBGT_true,   // Force true output .. true
  LBGT_false,  // Force false output . false
  LBGT_stringEqual,
  LBGT_agear,     // Arithmetic multiply........*
  LBGT_aslash,    // Arithmetic divide........../
  LBGT_agreater,  // Arithmetic greater than....>
  LBGT_amod,      // Arithmetic moudlo..........%
  LBGT_NotImplemented = 0xff
};

enum LOGIC_BLOCK_INPUT_TYPE {
  LBIT_literal           = 0,
  LBIT_globalASL         = 1,
  LBIT_partySize         = 2,
  LBIT_charInfo          = 3,
  LBIT_dirFacing         = 4,
  LBIT_levelASL          = 5,
  LBIT_questStage        = 6,
  LBIT_itemList          = 7,
  LBIT_NPCList           = 8,
  LBIT_RunTimeIf         = 9,
  LBIT_charASL           = 10,
  LBIT_partyASL          = 11,
  LBIT_Wiggle            = 12,
  LBIT_SourceGPDL        = 13,
  LBIT_BinaryGPDL        = 14, //After it is compiled in Runtime
  LBIT_tempASL          = 15,
  LBIT_NotImplemented    = 0xff
};

enum LOGIC_BLOCK_ACTION_TYPE {
  LBAT_nothing             = 0,
  LBAT_setGlobalASL        = 1,
  LBAT_setLevelASL         = 2,
  LBAT_removeGlobalASL     = 3,
  LBAT_removeLevelASL      = 4,
  LBAT_setQuestStage       = 5,
  LBAT_tempASL             = 6,
  LBAT_setIconIndexByName  = 7,
  LBAT_setCharASL          = 8,
  LBAT_setPartyASL         = 9,
  LBAT_removePartyASL      = 10,
  LBAT_SourceGPDL,
  LBAT_BinaryGPDL, //After it is compiled in Runtime
  LBAT_NotImplemented     = 0xff
};

enum LOGIC_BLOCK_FLAGS {
  LBF_RUNTIME_DEBUG = 1,
  LBF_RECORD_VALUES = 2
};

class LOGIC_BLOCK_DATA : public GameEvent
{
public:
//  DECLARE_SERIAL( LOGIC_BLOCK_DATA )
  LOGIC_BLOCK_DATA();
  virtual ~LOGIC_BLOCK_DATA();
  void Clear();
  void Serialize(CArchive& ar, double version);
  void Serialize(CAR &ar, double version);
  LOGIC_BLOCK_DATA& operator =(const LOGIC_BLOCK_DATA& cdData);

  BOOL IsChained();
  bool OnTestTrigger(bool allowForwardStep);
  void GetChainCount(int &curr, int &total);

#if (defined UAFEDITOR)
  void ChainToEventId(int num, DWORD cid);
  int  UnchainToEventId(int num, DWORD id);
  BOOL GetEVChainText(int num, char *text);
  BOOL GetEVChain(int num, DWORD &cid);
  void DetailedCrossReference(CR_EVENT_INFO *pCREI);
  void Export(JWriter& jw);
  void ExportInput(JWriter& jw, char c, BYTE& type, CString& value);
  void ExportGate(JWriter& jw, char c, BYTE& type, BYTE *negate);
  void ExportAction(JWriter& jw, const CString& name, BYTE& type, BYTE& cond, CString& value);
  bool Import(JReader& jr);
  void ImportInput(JReader& jw, char c, BYTE& type, CString& value);
  void ImportGate(JReader& jw, char c, BYTE& type, BYTE *negate);
  void ImportAction(JReader& jw, const CString& name, BYTE& type, BYTE& cond, CString& value);
  bool operator ==(const GameEvent& src) const;
#endif

#ifdef UAFEngine
    void OnInitialEvent(void);
    void OnKeypress(key_code key, char ascii) { /*no input allowed*/ }
    void OnMouseClickLeft(int mouseX, int mouseY) { /*no input allowed*/ }
    bool OnIdle(void);
#endif

  int Result;  // 0=false, 1=true, -1 = undetermined.
  DWORD falseChain;
  DWORD trueChain;
  CString m_Misc; // Not used yet.  Just in case we need something.
	CString	m_InputParamA;
	CString	m_InputParamB;
	CString	m_InputParamD;
	CString	m_InputParamF;
	CString	m_InputParamG;
	CString	m_ActionParam1;
	CString	m_ActionParam2;
	BYTE	m_GateTypeC;   // LOGIC_BLOCK_GATE_TYPE
	BYTE	m_GateTypeE;
	BYTE	m_GateTypeH;
	BYTE	m_GateTypeI;
	BYTE	m_GateTypeJ;
	BYTE	m_GateTypeK;
	BYTE	m_GateTypeL;
	BYTE	m_InputTypeA;  // LOGIC_BLOCK_INPUT_TYPE
	BYTE	m_InputTypeB;
	BYTE	m_InputTypeD;
	BYTE	m_InputTypeF;
	BYTE	m_InputTypeG;
	BYTE	m_ActionType1; // LOGIC_BLOCK_ACTION_TYPE
	BYTE	m_ActionType2;
	BYTE	m_ChainIfFalse;
	BYTE	m_ChainIfTrue;
	BYTE	m_NoChain;
	BYTE	m_NotC;
	BYTE	m_NotE;
	BYTE	m_NotH;
	BYTE	m_NotI;
	BYTE	m_NotJ;
	BYTE	m_NotK;
  BYTE  m_IfTrue1;
  BYTE  m_IfTrue2;
  BYTE  m_Flags;      // LOGIC_BLOCK_FLAGS
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "LOGIC_BLOCK_DATA";};
#endif
};

class GPDL;
class A_CStringPAIR_L;
enum GPDL_STATE;

class SHAZAM_CACHE
{
  A_CStringPAIR_L cache;
public:
  GPDL_STATE LoadBinary(const CString& eventText, GPDL *talkGPDL);
};

class GPDL_EVENT : public GameEvent
{
//  DECLARE_SERIAL ( GPDL_EVENT)
private:
  BOOL WaitForReturn;
  BOOL HighlightText;
  BOOL ForceBackup;
  BOOL KillSound;
  CString sound;
  int hSound;
  GPDL *m_talkGPDL;
public:
  GPDL_EVENT(void) { event=GPDLEvent; WaitForReturn=TRUE;HighlightText=FALSE;ForceBackup=FALSE;KillSound=FALSE;hSound=-1;};
  GPDL_EVENT(TEXT_EVENT_DATA *txtEvnt);
  virtual ~GPDL_EVENT();
  GPDL_EVENT& operator =(GPDL_EVENT& src);
  BOOL ForcePartyBackup() { return ForceBackup; }
  BOOL WaitUntilReturnPressed() { return WaitForReturn; }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnUpdateUI(void);
#ifdef UAFEDITOR
   void DetailedCrossReference(CR_EVENT_INFO *pCREI);
   //void Export(JWriter& jw);
   //bool Import(JReader& jr);
   //bool operator ==(const GameEvent& src) const;
#endif
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GPDL_EVENT";};
#endif
};

#ifdef UAFEngine

class DO_NOTHING_EVENT : public GameEvent
{ // All programming languages have a no-operation code!
public:
  DO_NOTHING_EVENT(void) { event=CONTROL_DoNothing; };
  virtual ~DO_NOTHING_EVENT() { }
  bool OnTestTrigger(bool allowForwardStep);
  bool OnIdle(void);
  void OnInitialEvent(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "DO_NOTHING_EVENT";};
#endif
};

class SPLASH_DATA : public GameEvent
{
  long hImage;
  CString GameVersionInfo;
  POSITION currPos;
  bool IsFirstScreen;
  bool LoadNextScreen();
  void EndSplash();
public:
  SPLASH_DATA(void) { event=CONTROL_Splash;hImage=-1;currPos=NULL;IsFirstScreen=true; }
  virtual ~SPLASH_DATA() {  }
  void OnKeypress(key_code key, char ascii);
  void OnMouseClickLeft(int x, int y);
  void PerformWarp(void);
  bool OnIdle(void);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnInvalidate(void);
  void OnLeaveQueue(void);  // releases current image
  bool OnTimer(UINT timerId);// Reply 'true' if you processed the timer
  void OnReturnToTopOfQueue(void); // after movie is played
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SPLASH_DATA";};
#endif
};

class START_MENU_DATA : public GameEvent
{
public:
  START_MENU_DATA(void) { event=CONTROL_Start; };
  virtual ~START_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue(void);
  int OnSaveGame(unsigned int *saveArea) { return 0; }
  int OnLoadGame(unsigned int *saveArea) { return 0; }
  void PerformWarp(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "START_MENU_DATA";};
#endif
};

class MAIN_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
public:
  MAIN_MENU_DATA(GameEvent *pEvent) { event=CONTROL_Main; m_pOrigEvent=pEvent; };
  virtual ~MAIN_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue(void);
  int  OnSaveGame(unsigned int *saveArea);
  int  OnLoadGame(unsigned int *saveArea);
  void PerformWarp(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "MAIN_MENU_DATA";};
#endif
};

class ADD_CHARACTER_DATA : public GameEvent
{
public:
  ADD_CHARACTER_DATA(void) { event=CONTROL_AddCharacter; };
  virtual ~ADD_CHARACTER_DATA() { }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void FillMenu(void);
  int  m_firstCharIndex;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ADD_CHARACTER_DATA";};
#endif
};

class CREATE_CHARACTER_DATA : public GameEvent
{
  int m_CurrClassFlagIdx;
public:
  CREATE_CHARACTER_DATA(void) { event=CONTROL_CreateCharacter;m_AbortCharCreation=false;m_CurrClassFlagIdx=0; };
  virtual ~CREATE_CHARACTER_DATA() { }
  void OnInitialEvent(void);
  bool OnIdle(void);
  bool m_AbortCharCreation;  
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CREATE_CHARACTER_DATA";};
#endif
};

class RACE_MENU_DATA : public GameEvent
{
  CREATE_CHARACTER_DATA *m_pOrig;
public:
  RACE_MENU_DATA(CREATE_CHARACTER_DATA *pOrig) { event=CONTROL_Race;m_pOrig=pOrig; };
  virtual ~RACE_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "RACE_MENU_DATA";};
#endif
};

class GENDER_MENU_DATA : public GameEvent
{
  CREATE_CHARACTER_DATA *m_pOrig;
public:
  GENDER_MENU_DATA(CREATE_CHARACTER_DATA *pOrig) { event=CONTROL_Gender; m_pOrig=pOrig;};
  virtual ~GENDER_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GENDER_MENU_DATA";};
#endif
};

class CLASS_MENU_DATA : public GameEvent
{
  CREATE_CHARACTER_DATA *m_pOrig;
public:
  CLASS_MENU_DATA(CREATE_CHARACTER_DATA *pOrig) { event=CONTROL_Class;m_pOrig=pOrig; };
  virtual ~CLASS_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CLASS_MENU_DATA";};
#endif
};

class ALIGNMENT_MENU_DATA : public GameEvent
{
  CREATE_CHARACTER_DATA *m_pOrig;
public:
  ALIGNMENT_MENU_DATA(CREATE_CHARACTER_DATA *pOrig) { event=CONTROL_Alignment;m_pOrig=pOrig; };
  virtual ~ALIGNMENT_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ALLIGNMENT_MENU_DATA";};
#endif
};

class CHOOSESTATS_MENU_DATA : public GameEvent
{
  bool m_CreateNewChar;
public:
  CHOOSESTATS_MENU_DATA(bool CreateNewChar) { event=CONTROL_ChooseStats; m_CreateNewChar=CreateNewChar;};
  virtual ~CHOOSESTATS_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnMouseClickLeft(int x, int y);
  void OnLeaveQueue(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CHOOSESTATS_MENU_DATA";};
#endif
};

class GETCHARNAME_MENU_DATA : public GameEvent
{
  CString name;
public:
  GETCHARNAME_MENU_DATA(void) { event=CONTROL_GetCharName; };
  virtual ~GETCHARNAME_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnReturnToTopOfQueue(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GETCHARNAME_MENU_DATA";};
#endif
};

class GETCHARICON_MENU_DATA : public GameEvent
{
public:
  GETCHARICON_MENU_DATA(void) { event=CONTROL_GetCharIcon;numIconPics=0;currIconSelected=0; };
  virtual ~GETCHARICON_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
private:
  int numIconPics;
  int currIconSelected;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GETCHARICON_MENU_DATA";};
#endif
};

class GETCHARSMALLPIC_MENU_DATA : public GameEvent
{
public:
  GETCHARSMALLPIC_MENU_DATA(void) { event=CONTROL_GetCharSmallPic;numSmallPics=0;currSmallPicSelected=0; };
  virtual ~GETCHARSMALLPIC_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
private:
  int numSmallPics;
  int currSmallPicSelected;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GETCHARSMALLPIC_MENU_DATA";};
#endif
};

class EXIT_DATA : public GameEvent
{
  LONGLONG splashScreenStart;
public:
  EXIT_DATA(void) { event=CONTROL_Exit; splashScreenStart=0; }
  virtual ~EXIT_DATA() { }
  bool OnIdle(void);
  void OnDraw(void) { }
  void OnInitialEvent(void);
  void OnKeypress(key_code key, char ascii);
  int  OnSaveGame(unsigned int *saveArea);
  int  OnLoadGame(unsigned int *saveArea);
  unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "EXIT_DATA";};
#endif
};


enum BackupStatus
{
  Backup_None = 0,
  Backup_PreMove,    // The party was prevented from moving into the next aquare
  Backup_PostEvent   // The party needs to backup at the end of event chain. 
};

class ADVENTURE_MENU_DATA : public GameEvent
{
private:
  int m_relDir; // set when move attempted
  int m_origPartyX;
  int m_origPartyY;
  int m_BackupStatus;
  CHARACTER *m_pOpenSpellChar;
  SPELL_ID   m_openSpellID;

public:
  ADVENTURE_MENU_DATA(void) 
  { 
    event=CONTROL_Adventure;
    m_relDir=m_origPartyX=m_origPartyY=-1;
    m_BackupStatus=Backup_None;
  };
  ADVENTURE_MENU_DATA(int partyX, int partyY)
  { 
    event=CONTROL_Adventure;
    m_relDir=-1;
    m_BackupStatus=Backup_None;
    m_origPartyX=partyX;
    m_origPartyY=partyY;
  };
  virtual ~ADVENTURE_MENU_DATA() { }
  void OnInitialEvent(void);
  bool OnCycle(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnKeypress(key_code key, char ascii);
  void OnMouseClickLeft(int x, int y);
  void OnMouseMove(int x, int y);
  void OnReturnToTopOfQueue(void);
  unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);
  int  OnSaveGame(unsigned int *saveArea);
  int  OnLoadGame(unsigned int *saveArea);
  void ProcessMenuRequest(key_code key, char ascii);
  void ProcessDungeonMoveRequest(key_code key, char ascii);
  void ProcessOverlandMoveRequest(key_code key, char ascii);
  void QueueMapEvents(bool execute_XY_events, bool execute_Step_events);
  //void PerformWarp(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ADVENTURE_MENU_DATA";};
#endif
};

class MOVE_MENU_DATA : public GameEvent
{
public:
  MOVE_MENU_DATA(void) { event=CONTROL_Move; };
  virtual ~MOVE_MENU_DATA() { }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnReturnToTopOfQueue();
  unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);
  int OnSaveGame(unsigned int *saveArea);
  int OnLoadGame(unsigned int *saveArea);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "MOVE_MENU_DATA";};
#endif
};

class SAVEGAME_MENU_DATA : public GameEvent
{
public:
  SAVEGAME_MENU_DATA(void) { event=CONTROL_Save; };
  virtual ~SAVEGAME_MENU_DATA() { }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SAVEGAME_MENU_DATA";};
#endif
};

class LOADGAME_MENU_DATA : public GameEvent
{
public:
  LOADGAME_MENU_DATA(void) { event=CONTROL_Load; };
  virtual ~LOADGAME_MENU_DATA() { }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "LOADGAME_MENU_DATA";};
#endif
};

class BEGIN_XY_EVENT : public GameEvent
{
private:
  int m_x;
  int m_y;
  int m_level;
public:
  bool m_forceEventProcessing;
  BEGIN_XY_EVENT(int x, int y, int level);
  BEGIN_XY_EVENT& operator = (BEGIN_XY_EVENT& src);
  virtual ~BEGIN_XY_EVENT() { }
  void OnInitialEvent(void);
  unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "BEGIN_XY_EVENT";};
#endif
};

class BEGIN_XY_FACING_EVENT : public GameEvent
{
private:
  int m_x;
  int m_y;
  int m_level;
public:
  BEGIN_XY_FACING_EVENT(int x, int y, int level);
  BEGIN_XY_FACING_EVENT& operator = (BEGIN_XY_FACING_EVENT& src);
  virtual ~BEGIN_XY_FACING_EVENT() { }
  void OnInitialEvent(void);
  unsigned int OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "BEGIN_XY_FACING_DATA";};
#endif
};

class BEGIN_STEP_EVENT : public GameEvent
{
private:
  int m_x;
  int m_y;
  int m_level;
public:
  BEGIN_STEP_EVENT(int x, int y, int level);
  BEGIN_STEP_EVENT& operator = (BEGIN_STEP_EVENT& src);
  virtual ~BEGIN_STEP_EVENT() { }
  void OnInitialEvent(void);
  int OnSaveGame(unsigned int *saveArea);
  int OnLoadGame(unsigned int *saveArea);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "BEGIN_STEP_EVENT";};
#endif
};

class BEGIN_TIME_EVENT : public GameEvent
{
private:
  int m_x;
  int m_y;
  int m_level;
  int m_day;
  int m_hour;
  int m_minute;
public:
  BEGIN_TIME_EVENT(int x, int y, int level, int day, int hour, int minute);
  BEGIN_TIME_EVENT& operator = (BEGIN_TIME_EVENT& src);
  virtual ~BEGIN_TIME_EVENT() { }
  void OnInitialEvent(void);
  int OnSaveGame(unsigned int *saveArea);
  int OnLoadGame(unsigned int *saveArea);
};

/*
class BEGIN_REST_EVENT : public GameEvent
{
private:
  int m_x;
  int m_y;
public:
  BEGIN_REST_EVENT(int x, int y);
  BEGIN_REST_EVENT& operator=(BEGIN_REST_EVENT& src);
  void OnInitialEvent(void);
  int OnSaveGame(unsigned int *saveArea);
  int OnLoadGame(unsigned int *saveArea);
};
*/
class BEGIN_ZONE_EVENT : public GameEvent
{
private:
  int m_x;
  int m_y;
public:
  BEGIN_ZONE_EVENT(int x, int y);
  BEGIN_ZONE_EVENT& operator=(BEGIN_ZONE_EVENT& src);
  virtual ~BEGIN_ZONE_EVENT() { }
  void OnInitialEvent(void);
  int OnSaveGame(unsigned int *saveArea);
  int OnLoadGame(unsigned int *saveArea);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "BEGIN_ZONE_EVENT";};
#endif
};

class PLAYSTEPSOUND_EVENT : public GameEvent
{
public:
  PLAYSTEPSOUND_EVENT(void){ event=CONTROL_PlayStepsound; };
  virtual ~PLAYSTEPSOUND_EVENT() { }
  void OnInitialEvent(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "PLAYSTEPSOUND_EVENT";};
#endif
};

class GET_MONEY_QTY_DATA : public GameEvent
{
private:
  eventType    m_CallerEventType;
  GameEvent   *m_pOrigEvent;
public:
  GET_MONEY_QTY_DATA(eventType caller, GameEvent *orig) 
  {
    event=CONTROL_GetMoneyQty;
    m_CallerEventType=caller;
    m_pOrigEvent=orig;
  };
  virtual ~GET_MONEY_QTY_DATA() { }
  GET_MONEY_QTY_DATA& operator = (GET_MONEY_QTY_DATA& src);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnMouseClickLeft(int x, int y);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GET_MONEY_QTY_DATA";};
#endif
};

class TAKE_TREASURE_DATA : public GameEvent
{
private:
  GameEvent *m_pCallerEvent;
  GET_MONEY_QTY_DATA m_getMoneyQty;
public:
  TAKE_TREASURE_DATA(GameEvent *callerEvent) :
            m_getMoneyQty(CONTROL_TakeTreasure, callerEvent)
  { 
    event=CONTROL_TakeTreasure;
    m_pCallerEvent=callerEvent;
  };
  virtual ~TAKE_TREASURE_DATA() { }
  TAKE_TREASURE_DATA& operator = (TAKE_TREASURE_DATA& src);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnMouseClickLeft(int x, int y);
  void OnReturnToTopOfQueue();
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TAKE_TREASURE_DATA";};
#endif
};

class VIEW_CHARACTER_DATA : public GameEvent
{
private:
  eventType m_callerEventType;
  GameEvent *m_pOrigEvent;
#ifdef NEW_LAY
  CString  m_layOrWhateverMenuText;
  int      m_layOrWhateverShortcut;
#endif
  CString  m_cureOrWhateverMenuText;
  int      m_cureOrWhateverShortcut;
public:
  VIEW_CHARACTER_DATA(eventType callerEventType, GameEvent *pOrigEvent) 
  { 
    event=CONTROL_ViewCharacter;
    m_callerEventType=callerEventType;
    m_pOrigEvent=pOrigEvent;
  };
  virtual ~VIEW_CHARACTER_DATA() { }
  VIEW_CHARACTER_DATA& operator = (VIEW_CHARACTER_DATA& src);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnReturnToTopOfQueue();
  void OnUpdateUI(void);
#ifdef NEW_LAY
  void CheckLayOrWhatever(void);
#endif
  void CheckCureOrWhatever(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "VIEW_CHARACTER_DATA";};
#endif
};

class SCRIPT_CHOICE_DATA : public GameEvent
{
private:
  //GameEvent *m_pCallerEvent;
  //GET_MONEY_QTY_DATA m_getMoneyQty;
public:
  SCRIPT_CHOICE_DATA(/*GameEvent *callerEvent) :
            m_getMoneyQty(CONTROL_TakeTreasure, callerEvent*/)
  { 
    event=CONTROL_ScriptChoice;
    //m_pCallerEvent=callerEvent;
  };
  virtual ~SCRIPT_CHOICE_DATA() { }
  SCRIPT_CHOICE_DATA& operator = (SCRIPT_CHOICE_DATA& src);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnMouseClickLeft(int x, int y);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SCRIPT_CHOICE_DATA";};
#endif
};

class ITEMS_MENU_DATA : public GameEvent
{
private:
  eventType  m_callerEventType;
  GameEvent *m_pOrigEvent;
  BOOL (*m_pFilter)(ITEM *pItem, void *pFilterPkt);
  void *m_pFilterPkt;
  int m_flags;
  BOOL m_bCallIdentifyHooks;
  CString m_saveParameter5;
  CString m_examineOrWhateverMenuText;
  int     m_examineOrWhateverShortcut;
public:
  ITEMS_MENU_DATA(eventType callerEventType, 
                  GameEvent *pOrigEvent, 
                  BOOL (*pFilter)(ITEM *pItem,void *pkt),
                  void *pFilterPkt,
                  int flags) 
  { 
    m_callerEventType=callerEventType;
    m_pOrigEvent=pOrigEvent;
    event=CONTROL_ItemsMenu;
    m_pFilter = pFilter;
    m_pFilterPkt=pFilterPkt;
    m_flags = flags;
  };
  virtual ~ITEMS_MENU_DATA() { }
  ITEMS_MENU_DATA& operator = (ITEMS_MENU_DATA& src);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnMouseClickLeft(int x, int y);
  void OnReturnToTopOfQueue();
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ITEMS_MENU_DATA";};
#endif
};

class ENCAMP_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  BOOL       m_zapCmd;
public:
  ENCAMP_MENU_DATA(GameEvent *origEvent) 
  { 
    event=CONTROL_Encamp;
    m_pOrigEvent=origEvent;
  };
  virtual ~ENCAMP_MENU_DATA() { }
  ENCAMP_MENU_DATA& operator = (ENCAMP_MENU_DATA& src);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnReturnToTopOfQueue(void);
  int  OnSaveGame(unsigned int *saveArea);
  int  OnLoadGame(unsigned int *saveArea);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ENCAMP_MENU_DATA";};
#endif
};

class BUY_SHOP_ITEMS_DATA : public GameEvent
{
private:
  eventType m_callerEventType;
  SHOP *m_pOrigEvent;
public:
  BUY_SHOP_ITEMS_DATA(eventType callerEventType, SHOP *pOrigEvent) 
  { 
    event=CONTROL_BuyItems;
    m_callerEventType=callerEventType;
    m_pOrigEvent=pOrigEvent;
  };
  virtual ~BUY_SHOP_ITEMS_DATA() { }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnKeypress(key_code key, char ascii);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "BUY_SHOP_ITEMS_DATA";};
#endif
};


class COMBAT_AIM_MENU_DATA : public GameEvent
{
//private:
//  GameEvent *m_pOrigEvent;
public:
  COMBAT_AIM_MENU_DATA(/*GameEvent *pEvent*/) { event=CONTROL_CombatAimAuto; /*m_pOrigEvent=pEvent;*/ };
  virtual ~COMBAT_AIM_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_AIM_MENU_DATA";};
#endif
};

class COMBAT_AIM_MANUAL_MENU_DATA : public GameEvent
{
//private:
//  GameEvent *m_pOrigEvent;
public:
  COMBAT_AIM_MANUAL_MENU_DATA(/*GameEvent *pEvent*/) { event=CONTROL_CombatAimManual; /*m_pOrigEvent=pEvent;*/ };
  virtual ~COMBAT_AIM_MANUAL_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_AIM_MANUAL_MENU_DATA";};
#endif
};

class COMBAT_SPELL_AIM_MENU_DATA : public GameEvent
{
private:
  COMBATANT *m_pCaster;
  COMBATANT *m_pTarget;
  SPELL_DATA *m_pSpell;
  bool m_choosingTarget;
  bool m_selectingUnits;
  CString m_chooseTargetResult;
public:
  COMBAT_SPELL_AIM_MENU_DATA( COMBATANT *pCaster, SPELL_DATA *pSpell)
  { 
    event=CONTROL_CombatSpellAimAuto; 
    m_pCaster=pCaster; 
    m_pTarget = NULL;
    m_pSpell = pSpell;
    m_choosingTarget = false;
  };
  virtual ~COMBAT_SPELL_AIM_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
  void OnReturnToTopOfQueue(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_SPELL_AIM_MENU_DATA";};
#endif
};

class COMBAT_SPELL_AIM_MANUAL_MENU_DATA : public GameEvent
{
private:
  COMBATANT *m_pCaster;
  COMBATANT *m_pTarget;
  SPELL_DATA *m_pSpell;
  char m_isValidTarget;  // This is the response from the "IsValidTarget" script.
                         // Interesting values are 'Y', 'N' and ' '.
  int m_maxRange;
  bool m_choosingTarget;
  bool m_selectingUnits;
  CString m_chooseTargetResult;

public:
  COMBAT_SPELL_AIM_MANUAL_MENU_DATA( COMBATANT *pCaster, SPELL_DATA *pSpell ) 
  {
    event=CONTROL_CombatSpellAimManual; 
    m_pCaster=pCaster;
    m_pTarget = NULL;
    m_isValidTarget = ' ';
    m_pSpell = pSpell;
    m_choosingTarget = false;
    m_selectingUnits = false;
  };
  virtual ~COMBAT_SPELL_AIM_MANUAL_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
  void OnReturnToTopOfQueue(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_SPELL_AIM_MANUAL_MENU_DATA";};
#endif
};

class COMBAT_MOVE_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  key_code m_initialKey;
public:
  COMBAT_MOVE_MENU_DATA(GameEvent *pEvent, key_code initialKey) 
  { 
    event=CONTROL_CombatMove;
    m_pOrigEvent=pEvent;
    m_initialKey = initialKey;
  }
  virtual ~COMBAT_MOVE_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  bool OnIdle(void);
  bool OnCycle(void);
  void OnUpdateUI(void);
  void OnMouseClickLeft(int x, int y);
  void OnReturnToTopOfQueue(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_MOVE_MENU_DATA";};
#endif
};

class COMBAT_RESULTS_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  GIVE_TREASURE_DATA *m_pTreasEvent;
  DWORD m_exptotal;
public:
  COMBAT_RESULTS_MENU_DATA(GameEvent *pEvent) { event=CONTROL_CombatResults; m_pOrigEvent=pEvent; m_exptotal=0; m_pTreasEvent=NULL;}
  virtual ~COMBAT_RESULTS_MENU_DATA() {if (m_pTreasEvent != NULL) delete m_pTreasEvent; }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnReturnToTopOfQueue(void);
  void OnLeaveQueue(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_RESULTS_MENU_DATA";};
#endif
};

class COMBAT_SPRITE_ANIMATE_DATA : public GameEvent
{
public:
  COMBAT_SPRITE_ANIMATE_DATA() { event=CONTROL_CombatSpriteAnimate; };
  virtual ~COMBAT_SPRITE_ANIMATE_DATA() { }
  void OnKeypress(key_code key, char ascii) { /*no input allowed*/ }
  void OnMouseClickLeft(int mx, int my) { /*no input allowed*/ }
  void OnInitialEvent(void);
  void OnDraw(void);
  bool OnCycle(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_SPRITE_ANIMATE_DATA";};
#endif
};

class COMBAT_SPELL_SPRITE_ANIMATE_DATA : public GameEvent
{
public:
  enum SpellAnimateType { MISSILE=0,COVERAGE=1,HIT=2,LINGER=3,CAST=4, REDIRECTED_MISSILE=5 };

private:
  int m_pCaster;
  int m_target;
  int m_mapx;
  int m_mapy;
  int m_sCombatant;  // REDIRECTED _MISSILE sets these source and destination combatants
  int m_dCombatant;
  SpellAnimateType m_type;
  CString m_msg;
  int m_totalTime;
  bool m_picLoaded;

public:
  COMBAT_SPELL_SPRITE_ANIMATE_DATA(int caster, int target, SpellAnimateType type, CString msg) 
    { event=CONTROL_CombatSpellSpriteAnimate; m_pCaster=caster; m_target=target; m_type=type; m_msg=msg;m_totalTime=0; }
  COMBAT_SPELL_SPRITE_ANIMATE_DATA(int caster, int mapx, int mapy, SpellAnimateType type, CString msg) 
    { event=CONTROL_CombatSpellSpriteAnimate; m_target=-1; m_pCaster=caster; m_mapx=mapx;m_mapy=mapy; m_type=type; m_msg=msg;m_totalTime=0;}
  virtual ~COMBAT_SPELL_SPRITE_ANIMATE_DATA() { }
  void OnKeypress(key_code key, char ascii) { /*no input allowed*/ }
  void OnMouseClickLeft(int mx, int my) { /*no input allowed*/ }
  void OnInitialEvent(void);
  void OnDraw(void);
  bool OnCycle(void);
  void OnLeaveQueue(void);
  void SourceCombatant(int n){m_sCombatant = n;};
  void DestinationCombatant(int n){m_dCombatant = n;};
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_SPELL_SPRITE_ANIMATE_DATA";};
#endif
};

enum overallCombatState;

class TIME_DELAY_MSG_DATA : public GameEvent
{
private:
  DWORD m_delay;
  CString m_msg;
  CString m_preRecordedMessage;
  overallCombatState m_state;
  bool m_timerrcvd;
  bool m_msgswitch;
  int  m_iDeathIndex; // Index of character that was killed.
public:
  TIME_DELAY_MSG_DATA(DWORD DelayTime, overallCombatState state, CString preRecordedMessage) 
  { 
    event=CONTROL_TimeDelayMsg;
    m_timerrcvd=false;
    m_delay=DelayTime;
    m_state=state;
    m_msgswitch=false;
    m_preRecordedMessage = preRecordedMessage;
  };
  virtual ~TIME_DELAY_MSG_DATA() { }
  void OnKeypress(key_code key, char ascii);//We will notice a <space> PRS 20101229 { /*no input allowed*/ }
  void OnMouseClickLeft(int mx, int my) { /*no input allowed*/ }
  void OnInitialEvent(void);
  void OnDraw(void);
  bool OnTimer(UINT TimerId);
  bool OnCycle(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TIME_DELAY_MSG_DATA";};
#endif
};

class COMBAT_ROUND_DELAY_DATA : public GameEvent
{
private:
  DWORD m_delay;
public:
  COMBAT_ROUND_DELAY_DATA(DWORD DelayTime) { event=CONTROL_CombatRoundDelay; m_delay=DelayTime; }
  virtual ~COMBAT_ROUND_DELAY_DATA() { }
  void OnKeypress(key_code key, char ascii) { /*no input allowed*/ }
  void OnMouseClickLeft(int mx, int my) { /*no input allowed*/ }
  void OnInitialEvent(void);
  void OnDraw(void);
  bool OnTimer(UINT TimerId);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_ROUND_DELAY_DATA";};
#endif
};


class COMBAT_SPECIAL_ACTION_DATA : public GameEvent
{
private:
  int m_combatantIndex;
  struct COMBATANT *m_fakeCombatant;

public:
  COMBAT_SPECIAL_ACTION_DATA(void);
  virtual ~COMBAT_SPECIAL_ACTION_DATA(void);
  void OnKeypress(key_code key, char ascii) { /*no input allowed*/ }
  void OnMouseClickLeft(int mx, int my) { /*no input allowed*/ }
  void OnInitialEvent(void);
  void OnReturnToTopOfQueue(void);
  //void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "COMBAT_SPECIAL_ACTION_DATA";};
#endif
};

class REST_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
public:
  REST_MENU_DATA(GameEvent *origEvent) 
  { 
    event=CONTROL_RestTimeMenu; 
    m_pOrigEvent=origEvent;
    m_days=m_hours=m_mins=0;
  }
  virtual ~REST_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnMouseClickLeft(int x, int y);
  bool OnCycle(void);
  void OnUpdateUI(void);

  __int64 m_days;
  __int64 m_hours;
  __int64 m_mins;
  BOOL m_showMenu;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "REST_MENU_DATA";};
#endif
};

class DISPLAY_SPELL_EFFECTS_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
public:
  DISPLAY_SPELL_EFFECTS_MENU_DATA(GameEvent *pEvent) { event=CONTROL_DisplaySpellAffectsMenu; m_pOrigEvent=pEvent; }
  virtual ~DISPLAY_SPELL_EFFECTS_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "DISPLAY_SPELL_EFFECTS_MENU_DATA";};
#endif
};

class SCRIBE_MENU_DATA : public GameEvent
{
public:
  SCRIBE_MENU_DATA(CHARACTER* pDude) { m_pDude=pDude; event=CONTROL_ScribeMenu; }
  virtual ~SCRIBE_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue(void);
  //GLOBAL_ITEM_ID m_giID_scroll;
  ITEM_ID m_itemID_scroll;
  CHARACTER *m_pDude;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "SCRIBE_MENU_DATA";};
#endif
};

class MEMORIZE_VERIFY_MENU_DATA : public GameEvent
{
public:
  MEMORIZE_VERIFY_MENU_DATA() { event=CONTROL_MemorizeVerifyMenu; }
  virtual ~MEMORIZE_VERIFY_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
};

class MEMORIZE_MENU_DATA : public GameEvent
{
public:
  MEMORIZE_MENU_DATA() { event=CONTROL_MemorizeMenu; }
  virtual ~MEMORIZE_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnMouseClickLeft(int mx, int my);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "MEMORIZE_MENU_DATA";};
#endif
};

class NEW_MU_SPELL_LEVEL_MENU_DATA : public GameEvent
{
  //int m_level;
  CHARACTER &m_dude;
public:
  NEW_MU_SPELL_LEVEL_MENU_DATA(/*int spelllevel,*/ CHARACTER &dude) : m_dude(dude) { /*m_level=spelllevel;*/event=CONTROL_NewMuSpellLevelMenu; }
  virtual ~NEW_MU_SPELL_LEVEL_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue();
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "NEW_MU_SPELL_LEVEL_MENU_DATA";};
#endif
};


struct AVAILABLE_SPELL
{
  SPELL_DATA *pSpellData;
  bool       referenced;
  bool       learned;
  int        probability;  // Percent chance of success
  bool       levelLimited; // Not currently available because of character's level
  AVAILABLE_SPELL(void) {pSpellData = NULL; learned = referenced = false;};
  ~AVAILABLE_SPELL(void) {};
  bool operator >(const AVAILABLE_SPELL& avs) const;
};


struct MNMC
{
  int min;     // Additional rounds until he reaches this minimum.
  int num;     // Number he must try before going to next level.
  int max;     // Absolute maximum number at this level.
  int certain; // Number he gets with 100 percent certainty.
  MNMC(void){min=num=max=certain = 0;};
  MNMC(int a, int b, int c, int d){min=a;num=b;max=c;certain=d;};
};

struct AcquireState
{
  MNMC  mnmc;
  int   numAvail;
  int   numAcquired;
  int   numAttempted;
  AcquireState(const MNMC *pmnmc)
  {
    mnmc = *pmnmc;
    numAvail=numAcquired=numAttempted=0;
  };
  AcquireState(void){numAvail=numAcquired=numAttempted=0;};
};

class INITIAL_MU_SPELLS_MENU_DATA : public GameEvent
{
  //int m_level;
  CHARACTER &m_dude;
  int        m_currentSpellLevel;
  int        m_passNumber;
  CArray<AcquireState, AcquireState&> m_acquireStates; // level zero provides overall limits for all levels.
  enum 
  {
    FinishedAllLevels = 0x0001,
    FinishedThisLevel = 0x0002,
  };
  CString m_successMsg;
public:
  INITIAL_MU_SPELLS_MENU_DATA(/*int spelllevel,*/ CHARACTER &dude) : m_dude(dude) { /*m_level=spelllevel;*/event=CONTROL_InitialMUSpellsMenu; }
  virtual ~INITIAL_MU_SPELLS_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue();
  CArray<AVAILABLE_SPELL, AVAILABLE_SPELL&> m_spellAvailabilityList;
  int  ListAllSpells(void);
  void FillSpellListText(bool newSpellLevel);
  int  AreWeDone(void);
  void AutoScribe(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "INITIAL_MU_SPELLS_MENU_DATA";};
#endif
};

class LEARN_SPELLS_MENU : public GameEvent
{
  //int m_level;
  enum 
  {
    FinishedAllLevels = 0x0001,
    FinishedThisLevel = 0x0002,
  };
  CHARACTER &m_dude;
  int        m_currentSpellLevel;
  CString m_successMsg; // Not serialized
  CArray<AcquireState, AcquireState&> m_acquireStates; // level zero provides overall limits for all levels.
public:
  LEARN_SPELLS_MENU(/*int spelllevel,*/ CHARACTER &dude) : m_dude(dude) 
  { /*m_level=spelllevel;*/
    event=CONTROL_InitialMUSpellsMenu; 
  };
  virtual ~LEARN_SPELLS_MENU() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue();
  CArray<AVAILABLE_SPELL, AVAILABLE_SPELL&> m_spellAvailabilityList;
  int  ListAllSpells(void);
  void FillSpellListText(bool newSpellLevel);
  int  AreWeDone(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "LEARN_SPELLS_MENU";};
#endif
};


class LEVEL_UP_LEARN_SPELLS_MENU : public GameEvent
{
  //int m_level;
  enum 
  {
    FinishedAllLevels = 0x0001,
    FinishedThisLevel = 0x0002,
  };
  CHARACTER &m_dude;
  CArray<AcquireState, AcquireState&> m_acquireStates; // level zero provides overall limits for all levels.
public:
  LEVEL_UP_LEARN_SPELLS_MENU(/*int spelllevel,*/ CHARACTER &dude) : m_dude(dude) 
  { /*m_level=spelllevel;*/
    event=CONTROL_LevelUpLearnSpellsMenu; 
  };
  virtual ~LEVEL_UP_LEARN_SPELLS_MENU() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue();
  CArray<AVAILABLE_SPELL, AVAILABLE_SPELL&> m_spellAvailabilityList;
  int  ListAllSpells(void);
  void FillSpellListText(bool newSpellLevel);
  int  AreWeDone(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "LEVEL_UP_LEARN_SPELLS_MENU";};
#endif
};

class CHOOSE_FROM_LIST_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  eventType m_origEventType;
  CString *m_pResult;
  CString m_list;
public:
  CHOOSE_FROM_LIST_MENU_DATA(CString list, CString *pResult)
     {m_list = list; event=CONTROL_ChooseFromList; m_pResult=pResult;};
  virtual ~CHOOSE_FROM_LIST_MENU_DATA() {};
  void OnUpdateUI(void);
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CHOOSE_FROM_LIST_MENU_DATA";};
#endif
};


class CAST_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  //GLOBAL_SPELL_ID m_cspell;
  SPELL_ID m_cspellID;
  CASTING_ENVIRONMENT m_castingEnvironment;
  bool m_choosingSpellFromList;
  CString m_chooseFromListResult;
public:
  CAST_MENU_DATA(GameEvent *pEvent, CASTING_ENVIRONMENT sce)
  { 
    event=CONTROL_CastMenu;
    m_pOrigEvent=pEvent; 
    m_castingEnvironment = sce;
    m_choosingSpellFromList = false;
  };
  virtual ~CAST_MENU_DATA() {};
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue();
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CAST_MENU_DATA";};
#endif
};

class CAST_TARGET_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
public:
  CAST_TARGET_MENU_DATA(GameEvent *pEvent) { event=CONTROL_CastTargetMenu; m_pOrigEvent=pEvent; }
  virtual ~CAST_TARGET_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
};

class TARGET_SELECT_NONCOMBAT_EVENT_DATA : public GameEvent
{
  CHARACTER *m_pCaster;
public:
  TARGET_SELECT_NONCOMBAT_EVENT_DATA(CHARACTER *data);
  virtual ~TARGET_SELECT_NONCOMBAT_EVENT_DATA();
  void Clear();

  void OnInitialEvent(void);
  void OnKeypress(key_code key, char ascii);
  void OnDraw(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TARGET_SELECT_NONCOMBAT_EVENT_DATA";};
#endif
};
/*
class TARGET_SELECT_COMBAT_EVENT_DATA : public GameEvent
{
  COMBATANT *m_caster;
  GameEvent *m_pOrigEvent;
  int m_cspellKey; // character spell book key
  DWORD m_spellKey; // spell db key
  int tempActive;
public:
  TARGET_SELECT_COMBAT_EVENT_DATA(GameEvent *pEvent, COMBATANT *data, int key);
  virtual ~TARGET_SELECT_COMBAT_EVENT_DATA();
  void Clear();
  void OnInitialEvent(void);
  void OnKeypress(key_code key, char ascii);
  void OnDraw(void);
  void OnUpdateUI(void);
};
*/

class CAST_NON_COMBAT_SPELL_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  eventType m_origEventType;
  CHARACTER *m_pCaster;
  //GLOBAL_SPELL_ID m_cspellKey; // character spell book key
  //GLOBAL_SPELL_ID m_spellKey; // spell db key
  SPELL_ID m_spellID; // spell db key
  int tempActive;
  bool m_layOrCureOrWhatever;
public:
  //CAST_NON_COMBAT_SPELL_MENU_DATA(eventType evType, GameEvent *pEvent, CHARACTER *pChar, GLOBAL_SPELL_ID key) 
  CAST_NON_COMBAT_SPELL_MENU_DATA(eventType evType,
                                  GameEvent *pEvent, 
                                  CHARACTER *pChar, 
                                  const SPELL_ID& spellID,
                                  bool layOrCureOrWhatever) 
  //{ event=CONTROL_CastNonCombatSpell; m_pCaster=pChar;m_pOrigEvent=pEvent;m_cspellKey=key;m_origEventType=evType; }
  { 
    event=CONTROL_CastNonCombatSpell; 
    m_pCaster=pChar;
    m_pOrigEvent=pEvent;
    //m_cspellKey=key;
    m_spellID=spellID;
    m_origEventType=evType;
    m_layOrCureOrWhatever = layOrCureOrWhatever;
  };
  virtual ~CAST_NON_COMBAT_SPELL_MENU_DATA() { }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnReturnToTopOfQueue();
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CAST_NON_COMBAT_SPELL_MENU_DATA";};
#endif
};

class CAST_COMBAT_SPELL_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  eventType m_origEventType;
  COMBATANT *m_pCaster;
  //GLOBAL_SPELL_ID m_cspellKey; // character spell book key
  //GLOBAL_SPELL_ID m_spellKey; // spell db key
  SPELL_ID m_spellID; // spell db key
  int tempActive;
public:
  //CAST_COMBAT_SPELL_MENU_DATA(eventType evType, GameEvent *pEvent, COMBATANT *pChar, GLOBAL_SPELL_ID key) 
  CAST_COMBAT_SPELL_MENU_DATA(eventType evType, GameEvent *pEvent, COMBATANT *pChar, const SPELL_ID& spellID) 
  {
    event=CONTROL_CastCombatSpell;
    m_pCaster=pChar;
    m_pOrigEvent=pEvent;
    //m_cspellKey=key;
    m_spellID=spellID;
    m_origEventType=evType;
  };
  virtual ~CAST_COMBAT_SPELL_MENU_DATA() { }
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnReturnToTopOfQueue();
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CAST_CMBAT_SPELL_MENU_DATA";};
#endif
};



class MAGIC_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
  CString    m_scribeOrWhateverMenuText;
  int        m_scribeOrWhateverMenuShortcut;
  CHARACTER *m_pActiveCharacter;
public:
  MAGIC_MENU_DATA(GameEvent *pEvent)
  { 
    event=CONTROL_MagicMenu;
    m_pOrigEvent=pEvent;
    m_pActiveCharacter = NULL;
  };
  virtual ~MAGIC_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  void OnReturnToTopOfQueue();
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "MAGIC_MENU_DATA";};
#endif
};

class GAME_SPEED_MENU_DATA : public GameEvent
{
private:
  GameEvent *m_pOrigEvent;
public:
  GAME_SPEED_MENU_DATA(GameEvent *pEvent) { event=CONTROL_GameSpeed; m_pOrigEvent=pEvent; }
  virtual ~GAME_SPEED_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "GAME_SPEED_MENU_DATA";};
#endif
};

class CHANGE_CLASS_MENU_DATA : public GameEvent
{
public:
  CHANGE_CLASS_MENU_DATA(void) { event=CONTROL_HumanChangeClass; }
  virtual ~CHANGE_CLASS_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
  //void ListAllClasses(void);
  //void FillSpellListText(void);
  CHARACTER *pChar;
  CArray<CLASS_ID, CLASS_ID&> m_classList;
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "CHANGE_CLASS_MENU_DATA";};
#endif
};

class TRADE_TAKER_SELECT_MENU_DATA : public GameEvent
{
private:
  GameEvent   *m_pOrigEvent;
public:
  TRADE_TAKER_SELECT_MENU_DATA(GameEvent *orig) { event=CONTROL_TradeTakerSelect; m_pOrigEvent=orig; }
  virtual ~TRADE_TAKER_SELECT_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnReturnToTopOfQueue(void);
  void OnLeaveQueue(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "TRADE_TAKER_SELECT_MENU_DATA";};
#endif
};

class ALTER_GAME_MENU_DATA : public GameEvent
{
public:
  ALTER_GAME_MENU_DATA(void) { event=CONTROL_AlterGame; }
  virtual ~ALTER_GAME_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnReturnToTopOfQueue(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ALTER_GAME_MENU_DATA";};
#endif
};

class ALTER_LEVEL_MENU_DATA : public GameEvent
{
public:
  ALTER_LEVEL_MENU_DATA(void) { event=CONTROL_AlterGameLevel; }
  virtual ~ALTER_LEVEL_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ALTER_LEWVEL_MENU_DATA";};
#endif
};

class ALTER_SOUND_MENU_DATA : public GameEvent
{
public:
  ALTER_SOUND_MENU_DATA(void) { event=CONTROL_AlterGameSound; }
  virtual ~ALTER_SOUND_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ALTER_SOUND_MENU_DATA";};
#endif
};

class ALTER_ORDER_MENU_DATA : public GameEvent
{
public:
  ALTER_ORDER_MENU_DATA(void) { event=CONTROL_AlterPartyOrder; }
  virtual ~ALTER_ORDER_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ALTER_ORDER_MENU_DATA";};
#endif
};

class DISPLAY_PARTY_JOURNAL_DATA : public GameEvent
{
public:
  DISPLAY_PARTY_JOURNAL_DATA(void) { event=CONTROL_DisplayPartyJournal; }
  virtual ~DISPLAY_PARTY_JOURNAL_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "DISPLAY_PARTY_JOURNAL_DATA";};
#endif
};

class PARTY_DIED_MENU_DATA : public GameEvent
{
public:
  PARTY_DIED_MENU_DATA(void) { event=CONTROL_PartyDied; }
  virtual ~PARTY_DIED_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "PARTY_DIED_MENU_DATA";};
#endif
};

class APPRAISE_SELECT_DATA : public GameEvent
{
  MONEY_SACK *m_pMoney;
  BOOL m_apprGems;
  BOOL m_apprJewelry;
public:
  APPRAISE_SELECT_DATA(MONEY_SACK *pMoney,BOOL apprGems=TRUE,BOOL apprJewels=TRUE) 
    { event=CONTROL_AppraiseSelect; m_pMoney=pMoney;m_apprJewelry=apprJewels;m_apprGems=apprGems;}
  virtual ~APPRAISE_SELECT_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnReturnToTopOfQueue(void);
  void OnUpdateUI(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "APPRAISE_SELECT_DATA";};
#endif
};

class APPRAISE_EVALUATE_DATA : public GameEvent
{
  int m_value;
  int m_type;
  CString m_name;
  // asks to sell or keep item, put result in party.tradeItem - 1=sell,0=keep
public:
  APPRAISE_EVALUATE_DATA(int type, CString name, int value) 
    { event=CONTROL_AppraiseEvaluate;m_name=name;m_value=value; m_type=type; }
  virtual ~APPRAISE_EVALUATE_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "APPRAISE_EVALUATE_DATA";};
#endif
};

class ASK_YES_NO_MENU_DATA : public GameEvent
{
  CString m_question;
  BOOL m_IsYesNo; // else just 'Press Enter To Continue' msg
  eventType m_OnDrawType;

public:
  // asks yes/no, put result in party.tradeItem - 1=yes,0=no
  ASK_YES_NO_MENU_DATA(CString question, BOOL IsYesNo=TRUE, eventType drawType=CONTROL_DoNothing) { event=CONTROL_AskYesNo; m_question=question;m_IsYesNo=IsYesNo; m_OnDrawType=drawType; }
  virtual ~ASK_YES_NO_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ASK_YES_NO_MENU_DATA";};
#endif
};

class ASK_CUSTOM_MENU_DATA : public GameEvent
{
  CString m_question;
  CString m_option1;
  CString m_option2;
  CString m_option3;
  CString m_option4;
  eventType m_OnDrawType;
  
public:
  // asks question with custom menu options, puts result in party.tradeItem
  ASK_CUSTOM_MENU_DATA( const CString &question,
                        const CString &opt1, 
                        const CString &opt2, 
                        const CString &opt3, 
                        const CString &opt4, 
                        eventType drawType=CONTROL_DoNothing) 
  { 
    event=CONTROL_AskCustom; 
    m_question=question;
    m_option1=opt1;
    m_option2=opt2;
    m_option3=opt3;
    m_option4=opt4;
    m_OnDrawType=drawType; 
  };
  virtual ~ASK_CUSTOM_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "ASK_CUSTOM_DATA";};
#endif
};


#ifdef SIMPLE_STRUCTURE
#else
class DESIGN_SELECT_MENU_DATA : public GameEvent
{
  struct DESIGN_DETAIL
  {
    DESIGN_DETAIL() { ver=0.0;name="";path=""; }
    double ver;
    CString name;    
    CString path;
  };

  CArray<DESIGN_DETAIL, DESIGN_DETAIL&> m_designs;
  CString m_Drive;

public:
  DESIGN_SELECT_MENU_DATA() { event=CONTROL_DesignSelect; m_Drive="";m_designs.RemoveAll(); }
  virtual ~DESIGN_SELECT_MENU_DATA() { }
  void OnKeypress(key_code key, char ascii);
  void OnInitialEvent(void);
  void OnDraw(void);
  void OnUpdateUI(void);

  int NumDesigns() { return m_designs.GetSize(); }
  void GetDesignVersion(const char *fname, double &ver, CString &name);
  BOOL SearchForDesignFolders(const CString &path);

  BOOL AddDesigns();
  void AddAllDesignsInFolder(CString &path);
  int AddDesign(DESIGN_DETAIL &data)
  {
    int index = NumDesigns();
    m_designs.SetAtGrow(index, data);
    return index;
  }    
#ifdef TASKTRACE
  const char *GetEventTypeName(void){return "DESIGN_SELECT_MENU_DATA";};
#endif
};
#endif

#endif // UAFEngine

BOOL AllowedAsGlobalEvent(eventType type);
BOOL AllowedAsGlobalCombatEvent(eventType type);

#ifdef UAFEDITOR
extern GameEventList CopyEventData; // temp storage for copy/paste of events
#endif

//lint +e1411
#endif