/******************************************************************************
* Filename: Procinp.h
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
#ifndef __PROCINPUT_H__
#define __PROCINPUT_H__


// EventDisposition
enum EventDisposition { DeleteEvent=1,  // The event was created dynamically
                                        // and should be deleted when removed
                                        // from the taskList.
                        KeepEvent,      // The event is allocated statically and
                                        // can be left lying about when removed
                                        // from the taskList.
                        LevelEvent      // The event is part of levelData.  When
                                        // the level changes it should be removed
                                        // from the taskList.  It should not be
                                        // deleted.
};


class CProcessInput 
{
private:
  CInput *m_pLocalCInput;    
  INPUT_DATA m_inputData;

public:
  CProcessInput();
  ~CProcessInput() { if (m_pLocalCInput != NULL) delete m_pLocalCInput; m_pLocalCInput = NULL; }
  CInput *GetInputInstance(HWND hwnd);
  eventType GetCurrEventType(void);
  BOOL ProcessInput(HWND hwnd);
  void StackTrace();
  void AllowInput(BOOL allow) 
  { 
    if (m_pLocalCInput != NULL) 
    { 
      m_pLocalCInput->AllowInput(allow); 
      if (!allow) m_pLocalCInput->ClearInput();
    }
  };
  void InitializeToSplash(void);
  void InitializeToMain(void);
  void InitializeToDesignSelect(void);
  
  void KeyPressed(UINT vkey);
  void KeySimulate(UINT vkey);
  BOOL handleTimer(UINT state);

  void HandleClockTick(void);

  void OnMouseMove(int x, int y);
  void HandleMouseClick(long x, long y);
  void HandleMouseMove(long x, long y);
  void MoveMouseTo(long x, long y) 
  {
    mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
                x,y,0,NULL);
    HandleMouseMove(x,y);
  }
  void BackupPartyOneStep();

  // if editor launches engine, warp info is passed on
  // command line. Skip intro screens and go to
  // indicated area.
  void SetWarp(int x, int y, int level, int facing);
  void AddPartyMember(CString PlayerCharacter);
  BOOL HaveWarp;
  int wx; // warp x,y
  int wy;
  int wl; // warp level
  int wf; // warp facing
  CList<CString, CString&> PartyList;
};

class GameEvent;

enum TASKSTATE
// WARNING *****  WARNING  ******
// These states are saved with a game.  That is why I have
// put absolute numbers on them.  You can add, rearrange,
// and delete as long as you don't change the numbers on
// events that may be on the stack when a game is saved.
// WARNING *****  WARNING  ******
{
  TASK_empty=0,        // m_event should == NULL
  TASK_uninitialized=1,  // m_event->initialize not yet called.
  TASK_initialized=2,
  TASK_Create_Character_Start=3,
  TASK_Create_Character_Race=4,
  TASK_Create_Character_Gender=5,
  TASK_Create_Character_Class=6,
  TASK_Create_Character_Alignment=7,
  TASK_Create_Character_ChooseStats=8,
  TASK_Create_Character_GetCharName=9,
  TASK_Create_Character_IconPick=10,
  TASK_Create_Character_SmallPicPick=11,
  TASK_QuestionYesNo_asking=12,
  TASK_QuestionYesNo_yes=13,
  TASK_QuestionYesNo_no=14,
  TASK_QuestionYesNo_yesChain=15,
  TASK_QuestionYesNo_noChain=16,
  TASK_TakeTreasureGetMoneyQty=17,
  TASK_MoveAwaitKey=18,
  TASK_MovePreMove0=19,
  TASK_TransferMenu=20,
  TASK_TextDisplayed=21,
  TASK_TransferGo=22,
  TASK_TransferStay=23,
  TASK_Deleted=24,  // When OnTaskMsg() returns TASKMSG_DELETE.
  TASK_TavernMenu=25,
  TASK_TavernDrink=26,
  TASK_WhoPaysGet=27,
  TASK_WhoPaysSuccess=28,
  TASK_WhoPaysFailure=29,
  TASK_WhoTriesGet=30,
  TASK_WhoTriesSuccess=31,
  TASK_WhoTriesFailure=32,
  TASK_TempleWelcome=33,
  TASK_TempleMenu=34,
  TASK_TempleGive=35,
  TASK_TempleDonate=36,
  TASK_TempleHeal=37,
  TASK_TempleCast=38,
  TASK_PasswordGet=39,
  TASK_PasswordFailure=40,
  TASK_PasswordSuccess=41,
  TASK_GuidedTourPausing=42,
  TASK_GuidedTourMoving=43,
    
  TASK_CombatWelcome=44,
  TASK_CombatMainMenu=45,
  TASK_CombatViewCharMenu=46,
  TASK_CombatQuickMenu=47,
  TASK_CombatSpeedMenu=48,

  TASK_CombatMoveMenu=49,
  TASK_CombatAimMenu=50,
  TASK_CombatUseMenu=51,
  TASK_CombatCastMenu=52,
  TASK_CombatTurnMenu=53,
  TASK_CombatGuardMenu=54,
  TASK_CombatBandageMenu=55,
  TASK_CombatDelayMenu=56,  
  TASK_CombatEndTurnMenu=57,  

  TASK_CombatMoving=58,
  TASK_CombatAttacking=59,
  TASK_CombatUsing=60,
  TASK_CombatBeginCasting=61,
  TASK_CombatTurning=62,
  TASK_CombatGuarding=63,
  TASK_CombatBandaging=64,
  TASK_CombatDelaying=65,
  TASK_CombatTurnEnded=66,

  TASK_CombatInRouteSpriteAnimating=67,
  TASK_SetPartyXY=68,
  //TASK_CombatDelay_StartRound=69,
  TASK_CombatOver=70,

  TASK_TakeTreasure=71,
  TASK_MainMenuModifyChar=72,
  TASK_MainMenu=73,

  TASK_MovePreMove1=74,
  TASK_MovePreMove2=75,
  TASK_MovePreMove3=76,
  TASK_MovePreMove4=77,
  TASK_MovePreMove5=78,
  TASK_MovePreMove6=79,
  TASK_MovePreMove7=80,

  TASK_RestMenuInput=81,
  TASK_RestMenuEngage=82,
  TASK_MainMenuDeleteChar=83,
  TASK_CastSpellTargetSelect=84,
  TASK_CastSpellSpellSelect=85,
  TASK_SearchForDesigns=86,
  TASK_DisplayDesigns=87,
  TASK_CombatHitSpriteAnimating=88,
  TASK_Create_Character_SpellPick=89,
  TASK_MainMenuRemoveChar=90,
  TASK_AskToIdItem=91,
  TASK_AskToSellItem=92,
  TASK_BashDoor=93,
  TASK_MoveExecuteEvent=94,
  TASK_AlterMusicPlayback=95,
  TASK_AlterCharacter=96,
  
  TASK_CombatForceWinMenu=97,
  TASK_CombatForcingWin=98,
  TASK_CombatStartBgMusic=99,
  TASK_CombatSelectSpellTargets=100,
  TASK_CombatActivateSpell=101,
  TASK_AskLeaveMoneyBehind=102,
  TASK_Create_Character_AskToSave=103,
  TASK_AskLeaveTreasureBehind=104,
  TASK_AskQuitGame=105,
  TASK_CombatSpellMissileAnimate=106,
  TASK_CombatSpellCoverageAnimate=107,
  TASK_CombatSpellHitAnimate=108,
  TASK_CombatSpellLingerAnimate=109,
  TASK_CombatSpellCastAnimate=110,
  TASK_CombatActivateSpellMsg=111,
  TASK_CombatAskAbortSpell=112,
  TASK_MainMenuTrain_SpellPick=113,
  TASK_UnspellDoor=114,  
  TASK_BashPickDoor=115, 
  TASK_CombatSpellFinalStage=116,  
  TASK_DropCharFromParty=117,
  TASK_AskBeforeFleeMap=118,
  TASK_BashPickUnspellDoor=119,
  TASK_BashUnspellDoor=120,
  TASK_CombatScriptChoice=121,
  TASK_MovePreNoMove=122,

  TASK_CombatSpecialAction=123,
  TASK_CombatSpellRedirectedMissileAnimate=124,
  TASK_CombatRedirectedSpellCoverageAnimate = 125,
  TASK_CombatRedirectedSpellLingerAnimate=126,
  TASK_CombatRedirectedSpellHitAnimate=127,
  TASK_CombatRedirectedSpellFinalStage = 128,
  TASK_CombatActivateRedirectedSpell = 129,
  TASK_CombatAutoHitSpriteAnimating=130,

  TASK_MovePostMove0=131,
  TASK_MovePostMove1=132,
  TASK_MovePostMove2=133,
  TASK_MovePostMove3=134,
  TASK_MovePostMove4=135,
  TASK_MovePostMove5=136,
  TASK_MovePostMove6=137,
  TASK_MovePostMove7=138,

  TASK_CombatStartingNewRound,
  TASK_CombatAutoMoving,
  TASK_CombatAutoAttacking,
  TASK_CombatDelay_NewCombatant,
  TASK_CombatDelay_AutoAttackMsg,
  TASK_CombatDelay_DisplayAttackMsg,
  TASK_CombatDelay_HitSpriteAnimateMsg,
  TASK_CombatDelay_AutoMove,
  TASK_CombatBeginItemCasting,
  TASK_CombatAttacking_DisplayAttacker,
  TASK_Create_Character_SpellLearn,
  TASK_MainMenuLearn_SpellPick,
  TASK_CharacterCastSpell,
  TASK_StartFreeAttack,
  TASK_MainMenuChangeClass_SpellPick,


  // You can add more if you like for your own use.  12 bits of space.
  // 69 is available if you need fixed numbers.
};

// Response flags returned by OnTaskMessage()
#define TASKMSG_DELETE 0x0001  // Mark this event deleted
#define TASKMSG_STOP   0x0002  // Message should go no further up the stack

enum TASKMESSAGE 
{
  TASKMSG_None=0,
  TASKMSG_CancelForwardStep=1, // Future move (after premove)
  TASKMSG_TeleportWithinLevel,
  TASKMSG_TeleportBetweenLevels,
  TASKMSG_MovePartyBackward,
  TASKMSG_Combat_MonstersSurpised,
  TASKMSG_Combat_PartySurpised,
  TASKMSG_Combat_NoSurpise,
  TASKMSG_ExecuteEvent,
  TASKMSG_SetPartyXY,
  TASKMSG_CharacterCastSpell,
  TASKMSG_AllowForwardStep, // If forward movement was supressed waiting
                            // to see if the first triggered event wants
                            // movement delayed.  For example, TELEPORTER
                            // with 'Party Attempts Entry' wants to do
                            // some things before the party actually moves
                            // forward.  Then the event chain determins that
                            // the party can finally take that forward step,
                            // then it sends a TASK_AllowForwardStep.
};

enum TASKTIMER
{
  TASKTIMER_GuidedTour,
  TASKTIMER_TimeDelayMsg,
  TASKTIMER_CombatRoundDelay,
  TASKTIMER_CombatAutoMoveDelay,
};

class TASKENTRY
{
  friend class TASKLIST;
  friend BOOL CProcessInput::ProcessInput(HWND hwnd);
public:
private:
  GameEvent       *m_event;
  TASKSTATE        m_state;
  EventDisposition m_disposition;  // When removed from queue
  bool             m_invalid;      // OnDraw needs to be called.
  bool             m_poppedToTop;
public:
  TASKENTRY(void) {Clear();};
  ~TASKENTRY(void) {Clear();};
  TASKENTRY& operator = (TASKENTRY& src);  
  void Clear(void) 
  {
    m_event=NULL; 
    m_state=TASK_empty; 
    m_disposition=KeepEvent;
    m_invalid=false;
  };
//  void operator = (GameEvent *event) {m_event=event;};
};


#define TaskTraceHistory 4 // Examine this many lines for repeats.

class TASKLIST
{
  enum
  {
    m_HASH_Clear = 1,
    m_HASH_RemoveAll = 5,
    m_HASH_MakeSureEventIsReady = 0,
    m_HASH_OnCycle = 11,
    m_HASH_SetEventState = 13,
    m_HASH_GetEventState = 23,
    m_HASH_PushEvent = 35,
    m_HASH_InsertEvent = 57,
    m_HASH_PopEvent = 91,
    m_HASH_ReplaceEvent = 147,
    m_HASH_TaskMessage = 237,
    m_HASH_OnIdle = 383,
    m_HASH_OnKeypress = 619,
    m_HASH_OnMouseClickLeft = 1001,
    m_HASH_OnMouseMove = 0,
    m_HASH_OnTimer = 2619,
    m_HASH_SaveTaskStack = 4237,
    m_HASH_LoadTaskStack = 6855,
    m_HASH_GetEventType = 11091
  };
  enum TTR_CODE
  {
    TTR_EMPTY = 0,
    TTR_OnIdle,
    TTR_TASKLIST,
    TTR_RemoveAll,
    TTR_MakeSureEventIsReady,
    TTR_OnCycle,
    TTR_SetEventState,
    TTR_GetEventState,
    TTR_PushEvent,
    TTR_InsertEvent,
    TTR_PopEvent,
    TTR_ReplaceEvent,
    TTR_TaskMessage,
    TTR_OnKeypress,
    TTR_OnMouseClickLeft,
    TTR_OnMouseMove_NotUsed,  // Mouse movement ONLY modifies graphic display
    TTR_OnTimer,
    TTR_SaveTaskStack,
    TTR_LoadTaskStack,
    TTR_GetEventType,
    TTR_Clear,
  };
  friend class GameEvent;  // Not derived events!
  friend class CProcessInput;
  friend void InitiateTaskMessage_SetPartyXY(void);
  TASKENTRY m_tasklist[MAXTASK];
  unsigned int m_hash; //Sorta like hash of TASKLIST activity for debugging.
#ifdef TASKTRACE
  void TaskTrace(TTR_CODE ttrCode, int flags = 0);
  void TraceLine(TTR_CODE ttrCode, int flags);
  FILE *m_traceFile;
  int m_IMCCount;
#endif
  GameEvent *m_eventToBeDeleted;
public:
  TASKLIST(void);
  ~TASKLIST(void);
  void RemoveAll(void);
  void Clear(void);
  void PopAllButTopEvent();
  eventType /*int*/ GetEventType();
  unsigned int Hash(void) {return m_hash;};
  void Hash(unsigned int hash) {m_hash = hash;};
  CString FindEventAttribute(int depth, const CString& name);
#ifdef TASKTRACE
  void ToggleTrace(void);
#endif
  void StackTrace(void);    
  void Invalidate(void);
  void DetailTrace(const CString& line);
private:  
  void MakeSureEventIsReady(void);
  BOOL IsEventReady(void);
  void Invalidate(GameEvent *active);
//  void CheckForDraw(void); // Call OnDraw if m_invalid;
  void OnCycle(void);
  void PushEvent(GameEvent *event, 
                 EventDisposition disposition = KeepEvent);
  void InsertEvent(GameEvent *event, 
                   EventDisposition disposition = KeepEvent);
  bool PopEvent(GameEvent *active); // allEmpty?
  void ReplaceEvent(GameEvent *event, 
                    GameEvent *active, 
                    EventDisposition disposition = KeepEvent); 
  bool OnIdle(void);
  void OnKeypress(KEY_DATA& kd); //key_code key, char ascii);
  void OnMouseClickLeft(int x, int y);
  void OnMouseMove(int x, int y);
  void OnTimer(UINT timerId);
  void SetEventState(TASKSTATE state, GameEvent *active);
  TASKSTATE GetEventState(GameEvent *active);
  bool SaveTaskStack(void); // To party.stateStack.
  bool LoadTaskStack(void); // From party.stateStack
  void TaskMessage(TASKMESSAGE msg);  
};


class SIMULATEDKEYQUEUE
{
private:
  UINT keystrokes[10];
public:
  SIMULATEDKEYQUEUE (void);
  void PushKey(UINT key);
  UINT PopKey (void);
};



extern CProcessInput ProcInput;
extern TASKLIST taskList;
extern SIMULATEDKEYQUEUE simulatedKeyQueue;



//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
//****************************************************************
//
// All of this is an early attempt to provide a mechanism
// for recording gameplay in such a way that it can be played
// back exactly in order to recreate hard-to-reproduce problems,
// provide 'movies' of gameplay for neophytes, etc.
//
// 10 March 2002 Paul Stevens
// Initial definition of class INPUTQUEUE.
// Route CMainFrame::OnKeyDown messages through the INPUTQUEUE.
//
//

//
#define INPUTQUEUE_MAX_ENTRIES 100  // That should be sufficient!
const int virtualGametimeUnits=30;  // 30 milliseconds per clock tick.

class INPUTQUEUE_ENTRY
{
  friend class INPUTQUEUE;
  friend class RECORDFILE;
  friend class PLAYBACKFILE;
  enum INPUTQUEUE_ENTRY_TYPE
  {
    IQE_UNKNOWN,
    IQE_ON_KEY_DOWN,
    IQE_ON_LBUTTON_DOWN,
    IQE_ON_MOUSEMOVE
  };
private:
  int                    m_nextEntry;   //index of next entry in list
  LONGLONG               m_timeOfEntry; //for debugging
  INPUTQUEUE_ENTRY_TYPE  m_entryType;
  unsigned int           m_data;        //Make these small integers if possible
  INPUTQUEUE_ENTRY(void);
  ~INPUTQUEUE_ENTRY(void);
  void destruct(void); //remove any allocated memory and clear.
};

class RECORDFILE
{
private:
  FILE   *m_file;
public:
  RECORDFILE(void);
  ~RECORDFILE(void);
  void Open(void);
  void PrintRecordHash(const CString& path, const CString& fname);
  void Write(INPUTQUEUE_ENTRY *ent);
  bool IsRecording(void);
};

class PLAYBACKFILE
{
private:
  FILE    *m_file;
  INPUTQUEUE_ENTRY::INPUTQUEUE_ENTRY_TYPE m_entryType;
  char         m_inputLine[81];
  bool         m_inputEOF;
  bool         m_dataReady;
  LONGLONG     m_lineTime;
  int          m_lineType;
  int          m_lineData;
  unsigned int m_lineHash;
  bool         m_ProcessCommand(char *line);
  int          m_fastPlay;
public:
  PLAYBACKFILE(void);
  ~PLAYBACKFILE(void);
  void Open(const CString& filename);
  void Close(void);
  void Read(INPUTQUEUE_ENTRY *ent);
  bool IsEntryAvailable(void);
  bool IsPlaying(void);
  int  FastPlay(void) {return IsPlaying()?m_fastPlay:0;};
};



class INPUTQUEUE
{
private: //everything is private!
  INPUTQUEUE_ENTRY  m_queue[INPUTQUEUE_MAX_ENTRIES];
  RECORDFILE        m_recordFile;
  PLAYBACKFILE      m_playFile;
  int               m_firstInQueue; // or -1 if none
  int               m_lastInQueue;  // or -1 if none
  int               m_available;    // or -1 if none
  DWORD             m_previousTime;
  LONGLONG          m_goalTime; // What we would like virtualGameTime to be
  //bool              m_recording;
  //bool              m_playing;
  bool              m_recordOK;
  bool              m_playOK;
  //bool              m_dataReady;
  //bool              m_inputEOF;
  //char              m_inputLine[81];
  //LONGLONG          m_lineTime;
  //int               m_lineData;
  //int               m_lineType;
  int               m_allocateEntry(bool important=false);
  void              m_releaseEntry(int);
  void              m_addEntry(int i);
  int               m_removeEntry(void); //Get first entry from queue
  bool              m_isEntryAvailable(void);
  int               m_adjustGoal(void);
public:
  INPUTQUEUE(void);
  ~INPUTQUEUE(void);
  void OnKeyDown(UINT nChar);
  void OnLButtonDown(int x, int y);
  void OnMouseMove(UINT flags, CPoint point); 
  DWORD DistributeInput(void); // Returns how long you should wait before
                               // calling again.
  int FastPlay(void){return m_playFile.FastPlay();};
  bool PlayOK(void){return m_playOK;};
  void StartPlayfile(const CString& filename);
};

extern INPUTQUEUE inputQueue;



#endif