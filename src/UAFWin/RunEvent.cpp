/******************************************************************************
* Filename: RunEvent.cpp
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

#include "externs.h"

//#include "..\Shared\ProjectVersion.h"
#include "class.h"
#include "GameEvent.h"

//#include "externs.h"
#include "GetInput.h"
#include "CProcInp.h"
#include "Screen.h"
#include "Dgngame.h"
#include "PicSlot.h"
#include "Graphics.h"
#include "Viewport.h"
#include "path.h"
#include "DrawTile.h"

#include "Level.h"
#include "party.h"
#include "FormattedText.h"
#include "DispText.h"
#include "GlobalData.h"
#include "monster.h"
#include "SoundMgr.h"
#include "GameMenu.h"
#include "..\Shared\GPDLopCodes.h"
#include "GPDLexec.h"
#include "GPDLComp.h"
#include "treas.h"
#include "combatants.h"
#include "RunTimeIf.h"
#include "CharStatsForm.h"
#include "RestTimeForm.h"
#include "MouseThread.h"
#include "ItemsForm.h"
#include "SpellForm.h"
#include "XBrowseForFolder.h"
#include "ramfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef WIN10Problem
extern bool WIN10ProblemActive = false;
#endif

void Convert$Wall(int level, const CString& key, const CString& value);
void DumpActions(CArray<int,int> *index, COMBAT_SUMMARY *pCombatSummary, bool evaluated);
bool LoadAI_Script(void);
CString OverrideTargetingHook(SPELL_DATA *pSpell, 
                              COMBATANT *pAttacker, 
                              COMBATANT *pTarget, 
                              int mapx, int mapy, const char *type);

extern COMBAT_SUMMARY combatSummary;
extern const double ENGINE_VER;
extern CHARACTER FakeCharacter;
extern CString cmdPlayfileName;
struct TALES
{
private:
  mCArray<TAVERN_TALES_TALE, TAVERN_TALES_TALE&>tavernTales;
public:
  DEFINE_mCARRAY_ACCESS_FUNCTIONS(Tale,               /* array data name      */ \
                                  TAVERN_TALES_TALE,  /* Search variable type */ \
                                  m_tale,             /* search variable name */ \
                                  tavernTales,        /* array name           */ \
                                  TAVERN_TALES_TALE,  /* array data type      */ \
                                  TALES)              /* type of *this        */
  void Clear(void) {tavernTales.RemoveAll();};
  void Append(const mCArray<TAVERN_TALES_TALE, TAVERN_TALES_TALE&>& src)
  {
    tavernTales.Append(src);
  };
  int NumTale(void){return tavernTales.GetSize();};
};



TALES tavernTales;

extern RECT CharViewViewportFrameSrc;

#pragma warning(disable:4189)

#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
  FILE *f;
public:
  DETAIL_TRACE(void);
  ~DETAIL_TRACE(void);
//  DWORD    m_id[1000];
//  DWORD    m_data[1000];
//  DWORD    m_time[1000];
//  DWORD    m_virtualTime[1000];
//  int      m_nextIndex;
  void     trace (DWORD id, DWORD data);
};
extern DETAIL_TRACE detailTrace;
#endif


#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif

extern unsigned int gamedatSignature;
extern int setPartyXY_x;
extern int setPartyXY_y;
extern DISPLAY_FORM initialSpellForm[];
extern DISPLAY_FORM memorizeSpellForm[];
extern const CString Skill_OpenLocks;
extern DICEPLUS infinity;


bool FastPlayback(void);
BOOL MyDeleteFile(LPCSTR name);
void gpdlCleanup(void);
int ScriptAtoI(const char **p, int min, int max);
CString RunGlobalScript(const char *SAname, const char *scriptName, bool nullSA);
CMyMenu menu;  

void die(int errorNum)
{
  CString msg;
  msg.Format("0x%08x", errorNum);
  MessageBox(NULL, msg, "Fatal Error in RunEvent", MB_TOPMOST|MB_OK);
  abort();
}




// make sure you declare the lvalue properly when using this
// function.
//
// CHARACTER temp = GetActiveChar() <--- BAD!!!, destructor will ruin source char
//
// CHARACTER &temp = GetActiveChar() <-- correct way
//
CHARACTER &GetActiveChar(GameEvent *pEvent=NULL, int PartyIdx=-1)
{
  if ((pEvent!=NULL) && (pEvent->GetEventType() == Combat))
  {
    // PRS 20110104.  "USE" in combat referred to active adventuring character.
    //if (PartyIdx == -1)
      return *combatData.getActiveCombatCharacter().m_pCharacter;
    //else
    //  return combatData.getCombatPartyCharacter(PartyIdx);
  }
  else
  {
    if (PartyIdx == -1)
      return party.getActiveChar();
    else
      return party.GetCharacter(PartyIdx, NULL);
  }
}

BOOL GameEvent::ShouldSimulateEnterKey()
{
  if (!WaitUntilReturnPressed()) 
    return TRUE;
  else if ((currPic.filename.IsEmpty()) && (textData.numLines==0))
    return TRUE;
  return FALSE;
}

void GameEvent::OnEnterQueue(void)
{
  // Default is to do nothing.
}

void GameEvent::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.reset(NULL);
}

void GameEvent::OnLeaveQueue(void)
{
  // Default is to do nothing.
}


void GameEvent::SetEventTimer(UINT ID, UINT delay)
{
  timerID = ID;
  timer = virtualGameTime + delay;
};

bool GameEvent::OnTimer(UINT timerId)
{
  // default is to say that we did not process the timer
  return false;
}

bool GameEvent::OnIdle(void) 
{
  return true; // Say that we are waiting for input.
}

unsigned int GameEvent::OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState)
{
  // default is to say we want off the stack when
  // teleporting
  switch (msg)
  {
  case TASKMSG_TeleportWithinLevel:
  case TASKMSG_TeleportBetweenLevels:
  case TASKMSG_SetPartyXY:
    return TASKMSG_DELETE;
  default: 
    return 0;
  }
}

void GameEvent::OnReturnToTopOfQueue(void)
{ 
  // Default is to ignore
}

bool GameEvent::CheckForMultiBoxText(void)
{
  if (miscError != NoError)
  {
    // really just formats the error text
    displayMiscError(miscError);
    miscError = NoError;
  }

  if (errorTextData.numLines > 0) 
    return false;

  if (TextIsMultiBox(textData))
  {
    bool override;
    if (textData.IsFirstBox())
    {
      override = true;
    }
    else
    { 
       if (!textData.IsLastBox())
       {
         override = true;
       }
       else
       {
         if (textData.eventRequiresReturn)
         {
           override = false;
         }
         else
         {
            if (textData.PeekTextLine(textData.numLines-1)->waitForReturn)
            {
              override = true;
            }
            else
            {
              override = false;
            };
         };
       };
    };
    if (override)
    {
      if (textData.IsLastBox())
      {
        menu.RemoveOverrideMenu();
        menu.AddOverrideMenu("BACKSPACE=Prev",-1,TRUE);
      }
      else if (textData.IsFirstBox())
      {
        menu.RemoveOverrideMenu();
        menu.AddOverrideMenu("ENTER=Next, SPACE=Last",-1,TRUE);
      }
      else
      {
        menu.RemoveOverrideMenu();
        menu.AddOverrideMenu("ENTER=Next, BACKSPACE=Prev, SPACE=Last",-1,TRUE);
      };
      return true;
    }
    else
    {
      menu.RemoveOverrideMenu();
      return true;
    }
  }
  else
  {
    menu.RemoveOverrideMenu();
    menu.setAllItemsActive();
  }
  return false;
}


bool GameEvent::OnFilterKeypress(key_code *key, char *ascii)
{
  if (*key==KC_NONE) return false;

  if (TextIsMultiBox(textData))
  {
    switch (*key)
    {
    case KC_SPACE:
      // end the multibox text
      if (!textData.IsLastBox())
        textData.Quit();
      *key=KC_NONE;
      return false;

    case KC_RETURN:
      // goto next box of text
      if (!textData.IsLastBox())
      {
        textData.NextBox();      
        *key=KC_NONE;
        return false;
      }
      break;

    case KC_BACK:
      // goto prev box of text
      if (!textData.IsFirstBox())
        textData.PrevBox();      
      *key=KC_NONE;
      return false;

    default: // no input accepted while multibox is active
      if (!textData.IsLastBox())
        *key=KC_NONE;
      return false;
    }
  }

  if (*key==KC_FC1)
  {
    // user requested a screen capture
    CString path=GetDesignPath();
    path += "DCScreenCapture.bmp";
    MyDeleteFile(path);
    GraphicsMgr.DoScreenCapture(path);
  }

  int item = menu.ConvertKeyCodeToMenuItem(*key);
  if (item > 0)
  {
    menu.setCurrentItem(item);
    simulatedKeyQueue.PushKey(VK_RETURN);
  }
  return false;
}

void GameEvent::OnKeypress(key_code key, char ascii)
{ // Ignore the keystroke by default unless it is a KC_RETURN
  if (key==KC_RETURN) PopEvent();
}

void GameEvent::OnMouseClickLeft(int mx, int my)
{ // Default mouse processing assumes walking state with 
  // menu, party, and inventory
  MouseMenu(mx, my);
  MouseParty(mx, my);
  MouseInventory(mx, my);
}
void GameEvent::OnMouseMove(int mx, int my)
{ 
  // Default mouse uses pointer only
  CursorArt.SetFirstFrame();
  CursorArt.sx = mx;
  CursorArt.sy = my;
}

bool GameEvent::OnTestTrigger(bool allowForwardStep)
{
  // We must either return true or get ourselves off the top of the stack.
  bool result = false;
  setPartyXY_x = -1;
  if (EventShouldTrigger()) result = true;
  if (result)
  {
    if (allowForwardStep)
    {
      TaskMessage(TASKMSG_AllowForwardStep);
    };
  };
  return result;
} 

void GameEvent::OnInvalidate(void)
{
}


void GameEvent::OnDraw(void)
{
  // Default is to draw the mouse cursor
  DisplayTextAndFlip(0);
}

void GameEvent::OnUpdateUI(void)
{
  // default is to enable all menu items
  CheckForMultiBoxText();
}

// default is to automatically set up small pics
// to animate if loaded.
bool GameEvent::OnCycle(void)
{
  // this can be called even before a design
  // is loaded (ex: DesignSelect)
  if (levelData.area_width <= 0) return false;

  LONGLONG currTime = virtualGameTime;//timeGetTime();
  bool result = false;
  // update drunk points, spell effects, etc
  if (IsCombatActive())
    result = combatData.ProcessTimeSensitiveData(currTime);
  else
    result = party.ProcessTimeSensitiveData(currTime);
  BackgroundSlotMemType currBg = BackgroundSets[currBgSlot];
  // Advance the frame counter for the background.  RenderBackground will take care of the rest
  if ((currTime - lastBgUpdate) >= (DWORD)currBg.timeDelay) {
      currBgFrame++;
      lastBgUpdate = currTime;
      result = true;
  }

  // this advances to next frame if timeDelay ms
  // have passed since last update.  
  if (currPic.AnimateNextFrame(currTime))
    result = true;

  if (UpdateAnimateList())
    result = true;
  
  if (errorTextData.numLines > 0) 
  {
    long diff = currTime - errorTextData.startTime;
    if ((diff > 2000) || (diff < 0))
    {
      ClearFormattedText(errorTextData);
      result=true;
    }
  }
  /*  I removed OnCycle from GameEvent.   16 May 2012
   *  Things were happening behind my back because of this
   *  function.  And I think this function was running
   *  many more times than necessary.  So I am going to
   *  deal with the many places it is necessary....one-at-a-time.
   *  Each event processor will have to do its own OnCycle
   *  processing if it seems necessary.  So perhaps we can
   *  restore the following code that was removed on 14 June 2010.
   */

  /*
   *
   * I removed this .....14 June 2010 PRS.
   * Perhaps it is needed.  But it is a terrible thing
   * to do in 'OnCyle'.  It is expensive.
   * If any action takes place that might require this us to 
   * recompute 'readyToTrain' then it can perform
   * this computation explicitly.  (It is already done
   * in a couple of places!)
   *
   * for (int i=0;i<party.numCharacters;i++)
   * {
   *   // sets character name to different color when
   *   // ready to train.
   *   party.characters[i].SetReadyToTrain(party.characters[i].IsReadyToTrain());
   *   if (party.characters[i].GetAge() > party.characters[i].GetMaxAge())
   *     party.characters[i].SetStatus(Dead);
   * };
   */
  return result;
}

int GameEvent::OnSaveGame(unsigned int *saveArea)
{ // Perhaps you meant to override this.
  ASSERT(0);
  MsgBoxError("Illegal game state for save");
  return 0; // Make compiler happy.
}

int GameEvent::OnLoadGame(unsigned int *saveArea)
{
  ASSERT(0);
  MsgBoxError("Illegal game state in load");
  return 0;
}

void GameEvent::PushEvent(GameEvent *ev, EventDisposition disposition)
{
  ASSERT( ev != NULL );
  if (ev == NULL) return;
  ASSERT( ev->IsGlobalEvent() == FALSE );
  taskList.PushEvent(ev, disposition);
}
void GameEvent::PushGlobalEvent(GameEvent *ev, EventDisposition disposition)
{
  ASSERT( ev != NULL );
  if (ev == NULL) return;
  ASSERT( ev->IsGlobalEvent() == TRUE );
  taskList.PushEvent(ev, disposition);
}

void GameEvent::InsertEvent(GameEvent *ev, EventDisposition disposition)
{
  ASSERT( ev != NULL );
  if (ev == NULL) return;
  ASSERT( ev->IsGlobalEvent() == FALSE );
  taskList.InsertEvent(ev, disposition);
}
void GameEvent::InsertEvent(DWORD evid)
{
  InsertEvent(levelData.eventData.GetEvent(evid), LevelEvent);
}

void GameEvent::InsertGlobalEvent(GameEvent *ev, EventDisposition disposition)
{
  ASSERT( ev != NULL );
  if (ev == NULL) return;
  ASSERT( ev->IsGlobalEvent() == TRUE );
  taskList.InsertEvent(ev, disposition);
}
void GameEvent::InsertGlobalEvent(DWORD evid)
{
  InsertGlobalEvent(globalData.eventData.GetEvent(evid), LevelEvent);
}

void GameEvent::PushEvent(DWORD evid)
{
  PushEvent(levelData.eventData.GetEvent(evid), LevelEvent);
}
void GameEvent::PushGlobalEvent(DWORD evid)
{
  PushGlobalEvent(globalData.eventData.GetEvent(evid), LevelEvent);
}

void GameEvent::ReplaceEvent(GameEvent *ev, EventDisposition disposition)
{
  ASSERT( ev != NULL );
  if (ev == NULL) return;
  ASSERT( ev->IsGlobalEvent() == FALSE );
  taskList.ReplaceEvent(ev, this, disposition);
}
void GameEvent::ReplaceEvent(DWORD eid)
{
  ReplaceEvent(levelData.eventData.GetEvent(eid), LevelEvent);
}

void GameEvent::ReplaceGlobalEvent(GameEvent *ev, EventDisposition disposition)
{
  ASSERT( ev != NULL );
  if (ev == NULL) return;
  ASSERT( ev->IsGlobalEvent() == TRUE );
  taskList.ReplaceEvent(ev, this, disposition);
}
void GameEvent::ReplaceGlobalEvent(DWORD evid)
{
  ReplaceGlobalEvent(globalData.eventData.GetEvent(evid), LevelEvent);
}

void GameEvent::SetMyState(TASKSTATE state)
{
  taskList.SetEventState(state, this);
}

TASKSTATE GameEvent::GetMyState(void)
{
  return taskList.GetEventState(this);
}

void GameEvent::PopEvent(void)
{
  taskList.PopEvent(this);
}

void GameEvent::Invalidate(void)
{
  taskList.Invalidate(this);
}

void GameEvent::Draw(void)
{
   OnDraw();
}

void GameEvent::StandardKeyboardShortcutAction(key_code key, char ascii)
{
  if ((key != KC_CHAR) && (key != KC_NUM)) return;
  if (!UseKeyboardShortcuts) return;
  int item = menu.LookupShortcut(ascii);
  if (item >= 0)
  {
    menu.setCurrentItem(item+1);
    simulatedKeyQueue.PushKey(VK_RETURN);
  }
  else
  {
    // some small pics won't loop back to the first
    // frame until a key is pressed.
    if (character.smallPic.IsPicLoaded())
    {
      if (character.smallPic.flags & PIC_DATA::AF_KeypressBeforeLoop)
      {
        if (character.smallPic.IsLastFrame())
          character.smallPic.SetFirstFrame();
      }
    }
    if (character.icon.IsPicLoaded())
    {
      if (character.icon.flags & PIC_DATA::AF_KeypressBeforeLoop)
      {
        if (character.icon.IsLastFrame())
          character.icon.SetFirstFrame();
      }
    }
    if (currPic.IsPicLoaded())
    {
      if (currPic.flags & PIC_DATA::AF_KeypressBeforeLoop)
      {
        if (currPic.IsLastFrame())
          currPic.SetFirstFrame();
      }
    }    
  }
}

void GameEvent::StandardMenuKeyboardAction(key_code key, char ascii)
{
  switch (key)
  {
  case KC_UP:   menu.prevItem();  break;
  case KC_DOWN: menu.nextItem();  break;
  case KC_RIGHT:menu.nextItem();  break;
  case KC_LEFT: menu.prevItem();  break;
  case KC_NUM:
  case KC_CHAR: StandardKeyboardShortcutAction(key,ascii); break;
  default: 
    // some small pics won't loop back to the first
    // frame until a key is pressed.
    if (character.smallPic.IsPicLoaded())
    {
      if (character.smallPic.flags & PIC_DATA::AF_KeypressBeforeLoop)
      {
        if (character.smallPic.IsLastFrame())
          character.smallPic.SetFirstFrame();
      }
    }
    if (character.icon.IsPicLoaded())
    {
      if (character.icon.flags & PIC_DATA::AF_KeypressBeforeLoop)
      {
        if (character.icon.IsLastFrame())
          character.icon.SetFirstFrame();
      }
    }
    if (currPic.IsPicLoaded())
    {
      if (currPic.flags & PIC_DATA::AF_KeypressBeforeLoop)
      {
        if (currPic.IsLastFrame())
          currPic.SetFirstFrame();
      }
    }    
    break;
  };
}

void GameEvent::HMenuVPartyKeyboardAction(key_code key, char ascii)
{ // Horizontal menu / vertical party

  if (party.numCharacters <= 1) 
  {
    // if only one selectable party member, let all
    // arrows change menu options
    StandardMenuKeyboardAction(key, ascii);
    return;
  }
  switch (key)
  {
  case KC_UP:   party.prevCharacter(); break;
  case KC_DOWN: party.nextCharacter(); break;
  default: StandardMenuKeyboardAction(key, ascii);
  };
}

void GameEvent::VMenuHPartyKeyboardAction(key_code key, char ascii)
{ // Horizontal menu / vertical party
  if (party.numCharacters <= 1) 
  {
    // if only one selectable party member, let all
    // arrows change menu options
    StandardMenuKeyboardAction(key, ascii);
    return;
  }
  switch (key)
  {
  case KC_LEFT:   party.prevCharacter(); break;
  case KC_RIGHT: party.nextCharacter(); break;
  default: StandardMenuKeyboardAction(key, ascii);
  };
}

void GameEvent::HMenuVItemsKeyboardAction(key_code key, 
                                          char ascii,
                                          void (*prev)(void),
                                          void (*next)(void))
{ // Horizontal menu / vertical items
  switch (key)
  {
  case KC_UP:   party.prevItem(); break;
  case KC_DOWN: party.nextItem(); break;
  case KC_SE:
    if (next != NULL) (*next)();
    break;
  case KC_NE:
    if (prev != NULL) (*prev)();
    break;
  default: StandardMenuKeyboardAction(key, ascii);
  };
}

void GameEvent::HMenuVInventoryKeyboardAction(key_code key, 
                                              char ascii,
                                              void (*prev)(void),
                                              void (*next)(void))
{ // Horizontal menu / vertical inventory
  switch (key)
  {
  case KC_UP:   party.prevItem(); break;
  case KC_DOWN: party.nextItem(); break;
  case KC_SE:
    if (next != NULL) (*next)();
    break;
  case KC_NE:
    if (prev != NULL) (*prev)();
    break;
  default: StandardMenuKeyboardAction(key, ascii);
  };
}


void GameEvent::MouseMenu(int mx, int my)
{
  int item = menu.IntersectPointWithMenu(mx, my);
  if (item >= 0)
  {
    TRACE("Mouse click hits menu item %i\n",item);
    menu.setCurrentItem(item+1);
    simulatedKeyQueue.PushKey(VK_RETURN);
  };
}

void GameEvent::MouseParty(int mx, int my)
{
  int item;
  item = IntersectPointWithParty(mx, my);
  if (item >= 0)
  {
    TRACE("Mouse click hits party member %i\n",item);
    party.activeCharacter = item;
    OnUpdateUI();
    Draw();
  }
}

bool GameEvent::TABParty(key_code key)
{
  if (key != KC_TAB) return false;
  TRACE("TAB from party member %i\n",party.activeCharacter);
  party.activeCharacter++;
  if (party.activeCharacter >= party.numCharacters)
  {
    party.activeCharacter = 0;
  };
  OnUpdateUI();
  Draw();
  return true;
}


bool GameEvent::MouseInventory(int mx, int my)
{
  int item;
  item = IntersectPointWithInventory(mx, my);
  if (item >= 0)
  {
    TRACE("Mouse click hits inventory item %i\n",item);
    party.activeItem = item;
    OnUpdateUI();
    Draw();
    return true;
  }
  return false;
}

void GameEvent::MouseCombatMap(int mx, int my)
{
  int tx=0, ty=0;
  if (IntersectPointWithCombatMap(mx, my, tx, ty))
  {
    TRACE("mouse click hits combat map\n");
    combatData.m_iMoveToX = tx;
    combatData.m_iMoveToY = ty;
    simulatedKeyQueue.PushKey(VK_INSERT);
  }
}

void GameEvent::MouseCombatMapMenu(int mx, int my)
{
  MouseMenu(mx, my);
  MouseCombatMap(mx,my);
}

void GameEvent::MouseMenuInventory(int mx, int my)
{
  MouseMenu(mx, my);
  MouseInventory(mx, my);
}


void GameEvent::StandardMenuDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void GameEvent::ChainHappened(void)
{
  GameEvent *newEvent;
  if ((control.chainTrigger==IfEventHappen) && (chainEventHappen > 0))
  {
    if (IsGlobalEvent())
    {
      newEvent = globalData.eventData.GetEvent(chainEventHappen);    
      ReplaceGlobalEvent(newEvent, LevelEvent);
    }
    else
    {
      newEvent = levelData.eventData.GetEvent(chainEventHappen);    
      ReplaceEvent(newEvent, LevelEvent);
    }
    if (newEvent==NULL) PopEvent();
  }
  else if ((control.chainTrigger==AlwaysChain) && (chainEventHappen > 0))
  {
    if (IsGlobalEvent())
    {
      newEvent = globalData.eventData.GetEvent(chainEventHappen);
      ReplaceGlobalEvent(newEvent, LevelEvent);
    }
    else
    {
      newEvent = levelData.eventData.GetEvent(chainEventHappen);
      ReplaceEvent(newEvent, LevelEvent);
    }
    if (newEvent==NULL) PopEvent();
  }
  else
  {
    PopEvent();
  };
}

// this is not called directly by any event, only by the
// task list manager if an event does not trigger
void GameEvent::OnChainNotHappened(void)
{
  GameEvent *newEvent;
  if ((control.chainTrigger == IfEventNotHappen) && (chainEventNotHappen > 0))
  {
    if (IsGlobalEvent())
    {
      newEvent = globalData.eventData.GetEvent(chainEventNotHappen);
      ReplaceGlobalEvent(newEvent, LevelEvent);
    }
    else
    {
      newEvent = levelData.eventData.GetEvent(chainEventNotHappen);
      ReplaceEvent(newEvent, LevelEvent);
    }
    if (newEvent==NULL) PopEvent();
  }
  else if ((control.chainTrigger == AlwaysChain)&&(chainEventHappen > 0))
  {
    if (IsGlobalEvent())
    {
      newEvent = globalData.eventData.GetEvent(chainEventHappen);
      ReplaceGlobalEvent(newEvent, LevelEvent);
    }
    else
    {
      newEvent = levelData.eventData.GetEvent(chainEventHappen);
      ReplaceEvent(newEvent, LevelEvent);
    }
    if (newEvent==NULL) PopEvent();
  }
  else
  {
    PopEvent();
  };
}

void GameEvent::ChainOrQuit(DWORD chainID)
{
  GameEvent *newEvent;
  if (chainID > 0)
  {
    if (IsGlobalEvent())
    {
      newEvent = globalData.eventData.GetEvent(chainID);
      if (newEvent != NULL)
        ReplaceGlobalEvent(newEvent, LevelEvent);
      else
        ChainHappened();
    }
    else
    {
      newEvent = levelData.eventData.GetEvent(chainID);
      if (newEvent != NULL)
        ReplaceEvent(newEvent, LevelEvent);
      else
        ChainHappened();
    }
  }
  else
  {
    ChainHappened();
  };
}

bool GameEvent::HandleTransfer(TRANSFER_DATA transferData, 
                               CString msg, 
                               PIC_DATA picData, 
                               CString hook, 
                               BOOL destroyDrow,
                               bool draw)
{ // msg is text to display after transfer is complete.
  // Note that it is call-by-value for a good reason.
  HOOK_PARAMETERS hookParameters;
  ProcInput.AllowInput(FALSE);
  
  TEXT_EVENT_DATA *textEvent;
  BOOL Execute = transferData.execEvent; // Because transferData will disappear!
  BOOL UseWaitCursor = FALSE;
  // If the transfer destination type is 'Script' then we need
  // to run the script right now to determine the level, x, and y destination.
  if (transferData.destEP == -3)
  {
    CString result;
    CString scriptName;
    int l, xPos, yPos;
    scriptName.Format("/%d/%d/%d", transferData.destLevel+1, transferData.destX, transferData.destY);
    result = RunGlobalScript("TeleporterDestinations", scriptName, true);
    if (sscanf(result,"/%d/%d/%d", &l, &xPos, &yPos) == 3)
    {
      transferData.destLevel = l-1;
      transferData.destX = xPos;
      transferData.destY = yPos;
    }
    else
    {
      WriteDebugString("Cannot find TeleporterDestination [%s]", scriptName);
    };
  }
  StopBackgroundMusic();
  if (transferData.destLevel==GetCurrentLevel())
  {        
    TaskMessage(TASKMSG_TeleportWithinLevel); // Deletes us, too.
  }
  else 
  {
    StopAllSounds();
    UseWaitCursor=TRUE;
    MouseRender.EnableWaitCursor();
    menu.AddOverrideMenu("LOADING...", -1, FALSE);
    if (draw)
    {
      OnDraw();
    };
    TaskMessage(TASKMSG_TeleportBetweenLevels); // Deletes us, too.
  };
  //// load level
  party.TeleportParty(transferData);
  
  StartBackgroundMusic();
  if (UseWaitCursor)
    MouseRender.DisableWaitCursor();

  if (!hook.IsEmpty())
  {  // Call the transfer hook
    hookParameters[5] = destroyDrow?"1":"";
    RunGlobalScript("Global_Transfer", hook, true);
  };


  // if not allowed to use Area View in current zone, make sure it is turned off
  if ((levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].allowMap == NoMapping)
      && (!cmdLine.m_forceAV))
  {
    globalData.useAreaView = FALSE;
  }
  else
  {
    if (!cmdLine.m_forceAV)
    {
      switch (globalData.levelInfo.stats[globalData.currLevel].AVStyle)
      {
      case OnlyAreaView: globalData.useAreaView = TRUE;  break;   
      case Only3DView:   globalData.useAreaView = FALSE; break;
      default:           // do nothing 
        break;
      }    
    }
  }

  // turn this off with each transfer
  //globalData.useAreaView = FALSE;

  // I am gone.  'this' points to garbage.  We must be very
  // careful not to use any pointer to us either here or in CProcinp
  if (Execute)
  {
    InsertEvent(new BEGIN_XY_EVENT(
            party.Posx,
            party.Posy,
            GetCurrentLevel()), DeleteEvent);
  };

  if (!hookParameters[6].IsEmpty())
  {
    textEvent=new TEXT_EVENT_DATA(false);
    textEvent->text = hookParameters[6];
    textEvent->pic.filename = "";
    textEvent->HighlightText=FALSE;
    InsertEvent(textEvent,DeleteEvent);
  };

  //if ((msg!="") || (!pic.filename.IsEmpty()))
  // 20140307  Jadefang says the picture should not be shown after the transfer/
  if (msg!="") 
  {
    textEvent=new TEXT_EVENT_DATA(false);
    textEvent->text=msg;
    // 20140307 JAdefang says the pic should not be shown after the transfer.
    //textEvent->pic = picData;
    textEvent->HighlightText=FALSE;
    InsertEvent(textEvent,DeleteEvent);
  };


  PopEvent();
  if (transferData.destLevel!=GetCurrentLevel())
    menu.RemoveOverrideMenu();

  ProcInput.AllowInput(TRUE);
  return false; // Means nothing as far as I know.
}


bool GameEvent::SaveGame(void)
{
  return taskList.SaveTaskStack();
}

bool GameEvent::LoadGame(void)
{
  return taskList.LoadTaskStack();
}

void GameEvent::TaskMessage(TASKMESSAGE msg)
{
  taskList.TaskMessage(msg);
}

GameEvent *GameEvent::EventType2EventPointer(eventType evType, unsigned int evid)
{
  switch (evType)
  {
  case CONTROL_Move:      return new ADVENTURE_MENU_DATA; // was MOVE_MENU_DATA
  case CONTROL_Main:      return new MAIN_MENU_DATA(NULL);
  case CONTROL_Exit:      return new EXIT_DATA;
  case CONTROL_Encamp:    return new ENCAMP_MENU_DATA(NULL);
  case CONTROL_Adventure: return new ADVENTURE_MENU_DATA;
  case CONTROL_BeginStepEvent: return new BEGIN_STEP_EVENT(-1,-1,-1);
  //case CONTROL_BeginRestEvent: return new BEGIN_REST_EVENT(-1,-1);
  case CONTROL_BeginZoneEvent: return new BEGIN_ZONE_EVENT(-1,-1);
  default: 
    {
      ASSERT( (evType < 1000) && (evid > 0) );
      if ( (evType < 1000) && (evid > 0) )
      {
        if (IsGlobalEvent())
          return globalData.eventData.GetEvent(evid);
        else
          return levelData.eventData.GetEvent(evid);
      }
    }
  }
  WriteDebugString("EventType2EventPointer(%u,%u) returning NULL\n", evType,evid);
  die(0xab4e5);
  return NULL;
}

int GetTimeInc(BOOL searching)
{
  int inc = 0;
  if (globalData.levelInfo.stats[globalData.currLevel].overland)
  {
    if (searching)
      inc = globalData.WildernessSearchTimeDelta;
    else
      inc = globalData.WildernessTimeDelta;
  }
  else
  {
    if (searching)
      inc = globalData.DungeonSearchTimeDelta;
    else
      inc = globalData.DungeonTimeDelta;
  }
  return inc;
}

void GameEvent::CheckSecretDoors(void)
{
  if (levelData.haveSecretDoor(party.Posx, party.Posy))
  {
    // at least one side of this map square has a secret door
    for (int s=0;s<4;s++)
    {
      //RACE_ID raceID;
      if (levelData.haveSecretDoor(party.Posx, party.Posy, s))
      {
        int need;
        for (int i=0;i<party.numCharacters;i++)
        {
          //if (party.characters[i].race() == Elf)
          //{
          //  if (party.searching)
          //    need = 5;
          //  else
          //    need = 2;
          //}
          //else
          //{
          //  if (party.searching)
          //    need = 2;
          //  else
          //    need = 1;
          //}
          const RACE_DATA *pRace;
          //raceID = party.characters[i].RaceID();
          //pRace = raceData.PeekRace(raceID);
          pRace = party.characters[i].PeekRaceData();
          if (party.searching)
            need = pRace->m_findSecretDoorSearching;
          else
            need = pRace->m_findSecretDoor;
          int result = RollDice(6,1,0.0);
          if (result <= need)
          {
            // success
            party.blockageData.ClearSecret(GetCurrentLevel(), party.Posx, party.Posy, s);
            CString msg;
            CString dirText;
            switch (s) 
            {
            case FACE_NORTH:
               dirText = "North";
               break;
            case FACE_EAST:
               dirText = "East";
               break;
            case FACE_SOUTH:
               dirText = "South";
               break;
            case FACE_WEST:
               dirText = "West";
               break;
            }
            msg.Format("%s discovers a secret door to the %s!", 
                        party.characters[i].GetName(), dirText);
            FormatPausedText(pausedTextData, msg);
            DisplayPausedText(pausedTextData, whiteColor, 0);
            ClearFormattedText(textData);
            break;
          }
        }
      }
    }
  }
}

CString GameEvent::getGlobalEventMessage(CString defaultMessage, CString hookName, HOOK_PARAMETERS hookParameters, CString attributeName, EVENT_CONTROL control) {
    CString giveTreasureMessage = "";
    if (!attributeName.IsEmpty()) {
        const ASLENTRY* pASL;
        pASL = control.eventcontrol_asl.Find(attributeName);
        if (pASL != NULL)
        {
            giveTreasureMessage = pASL->Value();
        };
    }
    if (giveTreasureMessage.IsEmpty()) {
        giveTreasureMessage = RunGlobalScript("Global_Messages", hookName, true);
    }
    if (giveTreasureMessage.IsEmpty()) {
        giveTreasureMessage = defaultMessage;  // Default treasure message
    }
    return giveTreasureMessage;
}
void GameEvent::UpdatePartyMovementData(void)
{
  int inc = GetTimeInc(party.searching);
  party.incrementClock(inc);

  //for (int i=0;i<inc;i++)
  //  levelData.IncTimeEventTime();
  
  // update visit flag
  party.SetVisited(GetCurrentLevel(), party.Posx,party.Posy);  

  // set current zone into PrevZone so future zone
  // checks will know when zone changes happen
  int zone = levelData.GetCurrZone(party.Posx,party.Posy); 
  levelData.SetPrevZone(zone);

  // update zone step counter
  party.IncZoneStepCount(GetCurrentLevel(), zone);

  if (!cmdLine.m_forceAV)
  {
    // check global view settings for this level
    switch (globalData.levelInfo.stats[globalData.currLevel].AVStyle)
    {
    case OnlyAreaView: globalData.useAreaView = TRUE;  break;   
    case Only3DView:   globalData.useAreaView = FALSE; break;
    default:           // do nothing 
      break;
    }
  }

  // if not allowed to use Area View in current zone, make sure it is turned off
  if ((levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].allowMap == NoMapping)
      && (!cmdLine.m_forceAV))
    globalData.useAreaView = FALSE;

  // Make sure that at least one pc or npc is still alive!
  // Party will normally happen in combat, and is checked afterwards.
  // It can also happen due to spell effects or non-combat events, which
  // is why we need to check at least once per movement.
  int alive = 0;

  // Make sure that only alive dudes are considered active.
  // This only affects events that act upon the active dude.
  int firstAlive = 0;      
  

  for (int i=0;i<party.numCharacters;i++)
  {
    // sets character name to different color when
    // ready to train.
    party.characters[i].SetReadyToTrain(party.characters[i].IsReadyToTrain());

    if (party.characters[i].GetAge() > party.characters[i].GetMaxAge())
      party.characters[i].SetStatus(Dead);

    if (party.characters[i].GetAdjStatus() == Okay)
    {
      alive++;
      if (firstAlive == -1) firstAlive = i;
    }
  }

  if (!alive)
  {
    PushEvent(new PARTY_DIED_MENU_DATA, DeleteEvent);
    return;
  }
  
  if (GetActiveChar(this).GetAdjStatus() != Okay)
    party.activeCharacter = firstAlive;

  CheckSecretDoors();
}

//////////////////////////////////////////////////////////////////SPLASH_DATA
void SPLASH_DATA::OnInitialEvent(void)
{
  if (ProcInput.HaveWarp)
  {
    PerformWarp();
    return;
  }  
  if (intro)
    mScreens = &(globalData.titleData);
  else 
    mScreens = &(globalData.creditsData);  // If it's not the intro, it's the outro, so use those title screens instead

  if (intro)
    GameVersionInfo.Format("Ver %01.7f", ENGINE_VER);
  currPos = ((TITLE_SCREEN_DATA*)mScreens)->Titles.GetHeadPosition();
  if ((currPos==NULL)||(!LoadNextScreen()))
  {
    EndSplash();
  }
  else
  {
    Invalidate();
  }
}

bool SPLASH_DATA::LoadNextScreen()
{   
  if (currPos==NULL) 
  {
    if (hImage > 0)
    {
      // fade out current image
      GraphicsMgr.FadeToBlack();
    }
    return false;
  }

  TITLE_SCREEN data = ((TITLE_SCREEN_DATA*)mScreens)->Titles.GetNext(currPos);

  if (   (data.DisplayBy == TITLE_SCREEN::tsFadeIn)
      && (!IsFirstScreen))
  {
    // fade out current image
    GraphicsMgr.FadeToBlack();
  }
  
  GraphicsMgr.ReleaseSurface(hImage);

  SurfaceType stype;
  if (data.UseTrans)
    stype = TransBufferDib;
  else
    stype = TitleDib;

  bool usetimer=false;
  bool success=false;
  switch (data.DisplayBy)
  {
  case TITLE_SCREEN::tsSplash:
    hImage = GraphicsMgr.AddFileSurface(data.TitleBgArt, stype, rte.TitleArtDir());
    success = (hImage >= 0);
    if (success)
      usetimer = true;
    break;
  case TITLE_SCREEN::tsFadeIn:
    success = (GraphicsMgr.FadeInImage(data.TitleBgArt, stype, rte.TitleArtDir())?true:false);
    if (success)
    {
      hImage = GraphicsMgr.AddFileSurface(data.TitleBgArt, stype, rte.TitleArtDir());
      if (hImage < 0)
        success = false;
      else
        usetimer = true;
    }
    break;
  case TITLE_SCREEN::tsMovie:
    PushEvent(new PLAY_MOVIE_DATA(data.TitleBgArt, PLAY_MOVIE_DATA::movieFull), DeleteEvent);
    success=true;
    break;
  default: 
    die(0xab4e6); 
    break;
  }

  if (((TITLE_SCREEN_DATA*)mScreens)->Timeout < 30)
    usetimer=false;

  if ((success)&&(usetimer))
    //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_GuidedTour,globalData.titleData.Timeout);
    SetEventTimer(TASKTIMER_GuidedTour, ((TITLE_SCREEN_DATA*)mScreens)->Timeout);

  return success;
}

bool SPLASH_DATA::OnTimer(UINT timerId)
{
  if (timerId != TASKTIMER_GuidedTour)
    return false;
  IsFirstScreen=false;
  if (!LoadNextScreen())
    EndSplash();
  return true;
}

void SPLASH_DATA::OnReturnToTopOfQueue(void)
{
  IsFirstScreen=false;
  if (!LoadNextScreen())
    EndSplash();
}


void SPLASH_DATA::OnInvalidate(void)
{
  //WriteDebugString("SPLASH_DATA::OnInvalidate()\n");
  OnDraw();
}

void SPLASH_DATA::OnDraw(void)
{
  // force image to stretch if needed...
  RECT dstRect;
  dstRect.top=dstRect.left=0;
  dstRect.bottom = SCREEN_HEIGHT;
  dstRect.right = SCREEN_WIDTH;

  if ((hImage >= 0)/* && (bActive)*/)
  {
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    GraphicsMgr.BlitImage(0, 0, hImage, NULL, FALSE, BACKBUFFERNUM, &dstRect);
    //WriteDebugString("SPLASH_DATA::OnDraw-Call DisplayTextAndFlip()\n");
#ifdef WIN10Problem
    WIN10ProblemActive = true;
#endif
    DisplayTextAndFlip(0);
    //WriteDebugString("SPLASH_DATA::OnDraw-Return from DisplayTextAndFlip()\n");

    // if first title screen
    if (IsFirstScreen)
    {
      // blit to front buffer, fonts are not loaded yet
      //GraphicsMgr.DrawText(VersionText_x,VersionText_y, GameVersionInfo, RGB(255,184,0), FRONTBUFFERNUM);
      //WriteDebugString("SPLASH_DATA::OnDraw - Call DrawFont()\n");



      /* prs 20191220
      GraphicsMgr.DrawFont(VersionText_x, VersionText_y, GameVersionInfo, globalData.WhiteFont, FALSE, FALSE, FALSE);      GraphicsMgr.DrawFont(VersionText_x, VersionText_y, GameVersionInfo, globalData.WhiteFont, FALSE, FALSE, FALSE);
      */
      GraphicsMgr.DrawFont(VersionText_x, VersionText_y, 
                           0, whiteColor,
                           GameVersionInfo, 
                           globalData.fontLib.GetFont(0, whiteColor, FALSE), // No color escapes
                           FALSE, FALSE, FALSE);



      //WriteDebugString("SPLASH_DATA::OnDraw - Return from DrawFont()\n");
    }
  }
}

void SPLASH_DATA::OnLeaveQueue(void)
{
  GraphicsMgr.ReleaseSurface(hImage);
}

void SPLASH_DATA::EndSplash()
{
  PlayIntro(FALSE);
  ClearFormattedText(errorTextData);
  if (intro) {
      if (tempChars.numChars() > 0)
          ReplaceEvent(new MAIN_MENU_DATA(NULL), DeleteEvent);
      else
          ReplaceEvent(new START_MENU_DATA, DeleteEvent);
      ProcInput.MoveMouseTo(0, 0);
  }
  else {
      SignalShutdown();
      ExitSignaled = 1;
  }
}

void SPLASH_DATA::OnKeypress(key_code key, char ascii)
{ 
  if (key == KC_TIMER) return;

  if (ProcInput.HaveWarp)
  {
    PerformWarp();
    return;
  }  

  IsFirstScreen=false;
  if (!LoadNextScreen())
    EndSplash();
}

void SPLASH_DATA::OnMouseClickLeft(int mx, int my)
{
  OnKeypress(KC_RETURN,0);
}

void FindTestCharArt(CHARACTER &dude)
{
  if (dude.icon.filename.IsEmpty())
  {
    //dude.icon.filename = rte.IconArtDir() + DEFAULT_NPC_ICON;
    dude.icon.filename = DEFAULT_NPC_ICON;
    dude.icon.picType = IconDib;
    dude.icon.SetDefaults();
  }

  if (dude.smallPic.filename.IsEmpty())
  {
    dude.smallPic.filename = DEFAULT_NPC_PIC;
    dude.smallPic.picType = SmallPicDib;
    dude.smallPic.SetDefaults();
  }

  BOOL iexist = FileExists(dude.icon.filename);
  BOOL spexist = FileExists(dude.smallPic.filename);
  if (iexist && spexist)
    return; // no need to search
  
  // scan NPC list for art first
  if (globalData.charData.GetCount()>0)
  {
    //POSITION pos = globalData.charData.GetHeadPosition();
    //while ((pos != NULL) && ((!iexist)||(!spexist)) )
    int i, n;
    n = globalData.charData.GetCount();
    for (i=0; (i<n) && ((!iexist)||(!spexist)); i++)
    {
      if (!iexist)
      {
        //if (FileExists(globalData.charData.GetAt(pos).icon.filename))
        if (FileExists(globalData.charData.PeekCharacter(i)->icon.filename))
        {
          //dude.icon = globalData.charData.GetAt(pos).icon;
          dude.icon = globalData.charData.GetCharacter(i)->icon;
          iexist=TRUE;
        }
      }
      if (!spexist)
      {
        //if (FileExists(globalData.charData.GetAt(pos).smallPic.filename))
        if (FileExists(globalData.charData.PeekCharacter(i)->smallPic.filename))
        {
          //dude.smallPic = globalData.charData.GetAt(pos).smallPic;
          dude.smallPic = globalData.charData.PeekCharacter(i)->smallPic;
          spexist=TRUE;
        }
      }
      //globalData.charData.GetNext(pos);
    }
  }

  if (iexist && spexist)
    return; // no need to search anymore

  // now try monster art
  if (monsterData.GetCount()>0)
  {
    //POSITION pos = monsterData.GetHeadPosition();
    //while ((pos != NULL) && (!iexist))
    int i, n;
    n = monsterData.GetCount();
    for (i=0; i<n; i++)
    {
      //if (FileExists(monsterData.PeekAtPos(pos).Icon.filename))
      if (FileExists(monsterData.PeekMonster(i)->Icon.filename))
      {
        //dude.icon = monsterData.PeekAtPos(pos).Icon;
        dude.icon = monsterData.PeekMonster(i)->Icon;
        iexist=TRUE;
      }
      //monsterData.GetNext(pos);
    }
  }
}

void SPLASH_DATA::PerformWarp(void)
{
  PlayIntro(FALSE);
  //LoadOffscreenWalkingSurfaces();
  ProcInput.HaveWarp=FALSE;
  ClearFormattedText(textData);
  WriteDebugString("Warping to x,y,level,facing: %i, %i, %i, %i\n",
                   ProcInput.wx,ProcInput.wy,ProcInput.wl,ProcInput.wf);

  mCList<CHARACTER, CHARACTER&> PartyMembers;
  CHARACTER dude;

  if (ProcInput.PartyList.IsEmpty())
  {
    // load temp character into party    
    dude.SetAlignment(ChaoticNeutral);

    //dude.race((raceType)0);
    dude.RaceID(raceData.PeekRace(0)->RaceID());

    dude.SetGender(Male);
    //dude.SetClass(Fighter);
    dude.SetClass(classData.PeekClass(0)->ClassID());
    dude.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
    dude.SetName("Super Spud");
    FindTestCharArt(dude);  
    //dude.serializeCharacter(TRUE, dude.GetName());
    if (dude.GetMaxHitPoints() > 0)
      PartyMembers.AddTail(dude);
  }
  else
  {
    POSITION pos = ProcInput.PartyList.GetHeadPosition();
    while (pos != NULL)
    {
      dude.Clear();
      if (dude.serializeCharacter(FALSE, ProcInput.PartyList.GetAt(pos)))
      {
        PartyMembers.AddTail(dude);
      }
      else
      {
        debugSeverity = 7;
        WriteDebugString("Failed to load starting party member %s\n", ProcInput.PartyList.GetAt(pos));
      }

      ProcInput.PartyList.GetNext(pos);
    }
  }

  MouseRender.EnableWaitCursor();
  if (globalData.levelInfo.stats[ProcInput.wl].used)
  {
    if (levelExists(ProcInput.wl))
    {
      StopAllSounds();
      if (LoadLevel(ProcInput.wl))
      {
        TRACE("Warp Design/Level data loaded successfully\n");
        party.setPartyAdventureState();
        party.setPartyLevelState(ProcInput.wl, ProcInput.wx, ProcInput.wy, ProcInput.wf);
        
        POSITION pos = PartyMembers.GetHeadPosition();
        while (pos != NULL)
        {
          party.addTempToParty(*PartyMembers.GetNext(pos));
        };

        StartBackgroundMusic();
        ReplaceEvent(new ADVENTURE_MENU_DATA(party.Posx, party.Posy), DeleteEvent);
        ProcInput.MoveMouseTo(0,0);
      }
      else 
      {
        debugSeverity = 8;
        WriteDebugString("Failed to load start warp level data\n");
      };
    }
    else
      WriteDebugString("Bogus warp level data, start level does not exist\n");
  }
  else
    WriteDebugString("Bogus warp level data, start level not used\n");
  MouseRender.DisableWaitCursor();
}

bool SPLASH_DATA::OnIdle(void)
{
  if (ProcInput.HaveWarp) PerformWarp();
  if (inputQueue.PlayOK())
  {
    if (!cmdPlayfileName.IsEmpty())
    {
      inputQueue.StartPlayfile(cmdPlayfileName);
    };
  };

  return true; // warp failed if we get here
}

//////////////////////////////////////////////////////////////DO_NOTHING_EVENT
bool DO_NOTHING_EVENT::OnIdle(void)
{
  PopEvent();
  return false;
}

bool DO_NOTHING_EVENT::OnTestTrigger(bool allowForwardStep)
{
  return true;
}

void DO_NOTHING_EVENT::OnInitialEvent(void)
{
  menu.reset(NULL);
  // this makes the DO_NOTHING_EVENT clear
  // quest text when no question is asked
  //ClearFormattedText(textData);
}

void EXIT_DATA::OnKeypress(key_code key, char ascii)
{
    DWORD timeout = ((TITLE_SCREEN_DATA*)mScreens)->Timeout;
    splashScreenStart = virtualGameTime - timeout; // Reset the timer X seconds back
    if (!SPLASH_DATA::LoadNextScreen()) {
        GraphicsMgr.FadeToBlack();
        SignalShutdown();
        //gpdlCleanup();
        ExitSignaled = 1;
    }
}

void EXIT_DATA::OnInitialEvent(void)
{
    splashScreenStart = virtualGameTime;
    PlayOuttro(TRUE);
    SPLASH_DATA::OnInitialEvent();
}

bool EXIT_DATA::OnIdle(void)
{
    if (ExitSignaled) return true;
    return true;  // Wait for input.  SPLASH_DATA::OnIdle is actually handling this
}

int EXIT_DATA::OnSaveGame(unsigned int* saveArea)
{
    return 0; // Nothing needs to be saved.
}

int EXIT_DATA::OnLoadGame(unsigned int* saveArea)
{
    return 0;
}

unsigned int EXIT_DATA::OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState)
{
    // this one stays put even after a teleport msg
    return 0;
}

///////////////////////////////////////////////////////////////START_MENU_DATA
void START_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  ClearFormattedText(errorTextData);
  
  if (TABParty(key)) return;
  if (key != KC_RETURN) 
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  };
  
  switch (menu.currentItem()) 
  {    
  case 1: // Create new character
    if (tempChars.numChars() < MAX_TEMP_CHARS)
      PushEvent(new CREATE_CHARACTER_DATA, DeleteEvent);
    break;    
  case 2: // Add character to party
    //if (party.numCharacters < GetMaxPartyCharacters())
    if (party.numCharacters < GetMaxPartyPCs())
      PushEvent(new ADD_CHARACTER_DATA, DeleteEvent);
    break;    
  case 3: // Load saved Game
    PushEvent(new LOADGAME_MENU_DATA, DeleteEvent);
    break;    
  case 4: // Exit      
    SetMyState(TASK_AskQuitGame);
    PushEvent(new ASK_YES_NO_MENU_DATA("QUIT THE GAME?",TRUE, CONTROL_Main), DeleteEvent);
    //ReplaceEvent(new EXIT_DATA, DeleteEvent);
    break;    
  default:
    WriteDebugString("Unhandled start menu item - handleMenu()\n");
    miscError = InvalidMenuState;
  };
}


void START_MENU_DATA::OnInitialEvent(void)
{
  if (ProcInput.HaveWarp)
  {
    PerformWarp();
    return;
  }  
  SetMyState(TASK_initialized);
  StopAllSounds();
  menu.setMenu(StartMenuData, NULL, FALSE, this, "START");
  menu.setVertOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  ClearFormattedText(textData);
  Invalidate();
  //ClearFormattedText(errorTextData);
}

void START_MENU_DATA::OnReturnToTopOfQueue(void)
{
  if (GetMyState()==TASK_AskQuitGame)
  {
    if (party.tradeItem==1) // yes, quit the game
    {
      ReplaceEvent(new EXIT_DATA, DeleteEvent);
      return;
    } 
    else 
      OnInitialEvent();
  }
  else
  {
    if (party.numCharacters > 0) 
    {
      ReplaceEvent(new MAIN_MENU_DATA(NULL), DeleteEvent);
      return;
    }
    else 
      OnInitialEvent();
  }
}

void START_MENU_DATA::OnDraw(void)
{
  UpdateStartScreen(&Select_Main_Function_Format, 0);
}

void START_MENU_DATA::OnUpdateUI(void)
{

  if (CheckForMultiBoxText()) return;

  //if (!globalData.allowCharCreate)
  if (!globalData.GetAllowCharCreate())
    menu.setItemInactive(1); // create
}
void START_MENU_DATA::PerformWarp(void)
{
  PlayIntro(FALSE);
  ProcInput.HaveWarp=FALSE;
  ClearFormattedText(textData);
  WriteDebugString("Warping to x,y,level,facing: %i, %i, %i, %i\n",
                   ProcInput.wx,ProcInput.wy,ProcInput.wl,ProcInput.wf);

  mCList<CHARACTER, CHARACTER&> PartyMembers;
  CHARACTER dude;

  if (ProcInput.PartyList.IsEmpty())
  {
    // load temp character into party    
    dude.SetAlignment(ChaoticNeutral);
    //dude.race((raceType)0);
    dude.RaceID(raceData.PeekRace(0)->RaceID());
    dude.SetGender(Male);
    //dude.SetClass(Fighter);
    dude.SetClass(classData.PeekClass(0)->ClassID());
    dude.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
    dude.SetName("Super Spud");
    FindTestCharArt(dude);  
    //dude.serializeCharacter(TRUE, dude.GetName());
    if (dude.GetMaxHitPoints() > 0)
      PartyMembers.AddTail(dude);
  }
  else
  {
    POSITION pos = ProcInput.PartyList.GetHeadPosition();
    while (pos != NULL)
    {
      dude.Clear();
      if (dude.serializeCharacter(FALSE, ProcInput.PartyList.GetAt(pos)))
      {
        PartyMembers.AddTail(dude);
      }
      else
      {
        debugSeverity = 7;
        WriteDebugString("Failed to load starting party member %s\n", ProcInput.PartyList.GetAt(pos));
      }

      ProcInput.PartyList.GetNext(pos);
    }
  }
  
  MouseRender.EnableWaitCursor();
  if (globalData.levelInfo.stats[ProcInput.wl].used)
  {
    if (levelExists(ProcInput.wl))
    {
      StopAllSounds();
      if (LoadLevel(ProcInput.wl))
      {
        TRACE("Warp Design/Level data loaded successfully\n");
        party.setPartyAdventureState();
        party.setPartyLevelState(ProcInput.wl, ProcInput.wx, ProcInput.wy, ProcInput.wf);

        POSITION pos = PartyMembers.GetHeadPosition();
        while (pos != NULL)
        {
          party.addTempToParty(*PartyMembers.GetNext(pos));
        }

        StartBackgroundMusic();
        ReplaceEvent(new ADVENTURE_MENU_DATA(party.Posx, party.Posy), DeleteEvent);

        BEGIN_ZONE_EVENT *zoneEvent=NULL;
        zoneEvent=new BEGIN_ZONE_EVENT(party.Posx,
                                       party.Posy);
        // this event will check for, and load, any events that
        // should trigger based on current x,y position in map
        BEGIN_XY_EVENT *xyEvent;
        xyEvent=new BEGIN_XY_EVENT(party.Posx,
                                   party.Posy,
                                   GetCurrentLevel());
        // this event will check for, and load, any step events
        // that are available.
        BEGIN_STEP_EVENT *stepEvent;
        stepEvent=new BEGIN_STEP_EVENT(party.Posx,
                                       party.Posy,
                                       GetCurrentLevel());

/*
        BEGIN_TIME_EVENT *timeEvent;
        timeEvent=new BEGIN_TIME_EVENT(party.Posx,
          party.Posy,
          GetCurrentLevel(),
          party.days, party.hours, party.minutes);
*/

        // queue up the events  
        //PushEvent(timeEvent, DeleteEvent);
        PushEvent(xyEvent,   DeleteEvent);
        PushEvent(stepEvent, DeleteEvent);
        PushEvent(zoneEvent, DeleteEvent);

        ProcInput.MoveMouseTo(0,0);
      }
      else 
      {
        debugSeverity = 8;
        WriteDebugString("Failed to load start warp level data\n");
      };
    }
    else
      WriteDebugString("Bogus warp level data, start level does not exist\n");
  }
  else
    WriteDebugString("Bogus warp level data, start level not used\n");
  MouseRender.DisableWaitCursor();
}

///////////////////////////////////////////////////////////////MAIN_MENU_DATA
void MAIN_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN) 
  {
    VMenuHPartyKeyboardAction(key,ascii);
    return;
  };
  
  /*
    ** original order ** 
   1 { "Create Character", 0 },
   2 { "Delete Character", 0 },
   3 { "Modify Character", 0 },
   4 { "Train Character", 0 },
   5 { "Human Change Class", 0 },
   6 { "View Character", 0 },
   7 { "Add Character", 0 },
   8 { "Remove Character", 0 },
   9 { "Load Saved Game", 0 },
   10 { "Save Current Game", 0 },
   11 { "Begin Adventuring", 0 },
   12 { "Exit From Game", 1 }

   ** new order ** 
   1 { "Add Character", 0 },
   2 { "Remove Character", 0 },
   3 { "Modify Character", 0 },
   4 { "Train Character", 0 },
   5 { "Human Change Class", 0 },
   6 { "View Character", 0 },
   7 { "Create Character", 0 },
   8 { "Delete Character", 0 },
   9 { "Load Saved Game", 0 },
   10 { "Save Current Game", 0 },
   11 { "Begin Adventuring", 0 },
   12 { "Exit From Game", 1 }

  */

  switch (menu.currentItem()) 
  { 
  case 1: // Add
    //if (party.numCharacters < GetMaxPartyCharacters())
    {
      int numPC;
      int i;
      for (i=0, numPC=0; i<party.numCharacters; i++)
      {
        if (party.characters[i].GetAdjHitPoints() < 0)
          party.characters[i].SetStatus(Dead);
        if (party.characters[i].GetType() == CHAR_TYPE)
        {
          numPC++;
        };
      };
      if ((numPC < GetMaxPartyPCs()) && (party.numCharacters < GetMaxPartyMembers()))
        PushEvent(new ADD_CHARACTER_DATA, DeleteEvent);
    };
    break;
       
  case 2: // Remove, Drop
    {
      CString msg;
      msg.Format("REMOVE %s FROM PARTY?", GetActiveChar(this).GetName());
      SetMyState(TASK_MainMenuRemoveChar);
      PushEvent(new ASK_YES_NO_MENU_DATA(msg), DeleteEvent);      
    }
    break;

  case 3: // modify
    character = GetActiveChar(this);
    SetMyState(TASK_MainMenuModifyChar);
    PushEvent(new CHOOSESTATS_MENU_DATA(false), DeleteEvent); // false: use existing charcter
    break;
    
  case 4: // train
    if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType() == TrainingHallEvent))
    {
      const TRAININGHALL *pTrainingHallEvent;
      pTrainingHallEvent = (TRAININGHALL*)m_pOrigEvent;
      if (GetActiveChar(this).enoughMoney(pTrainingHallEvent->Cost,0,0,globalData.moneyData.GetDefaultType()))
      {
        int i, n;
        CHARACTER *pChar = &GetActiveChar(this);        
        pChar->payForItem(pTrainingHallEvent->Cost, globalData.moneyData.GetDefaultType());
/*        
        int flvl = tempCHAR.GetCurrentLevel(FighterFlag);
        int clvl = tempCHAR.GetCurrentLevel(ClericFlag);
        int rlvl = tempCHAR.GetCurrentLevel(RangerFlag);
        int plvl = tempCHAR.GetCurrentLevel(PaladinFlag);
        int mlvl = tempCHAR.GetCurrentLevel(MagicUserFlag);
        int tlvl = tempCHAR.GetCurrentLevel(ThiefFlag);
        int dlvl = tempCHAR.GetCurrentLevel(DruidFlag);
*/
        {
          n = pChar->GetBaseclassStatsCount();
          for (i=0; i<n; i++)
          {
            BASECLASS_STATS *pBaseclassStats;
            pBaseclassStats = pChar->GetBaseclassStats(i);
            pBaseclassStats->temp__origLevel = pBaseclassStats->currentLevel;
          };
        };

        pChar->TrainCharacter(&pTrainingHallEvent->trainableBaseclasses);  // Sets baseclassStats[n].currentlevel for each baseclass
/*
        int newflvl = tempCHAR.GetCurrentLevel(FighterFlag);
        int newclvl = tempCHAR.GetCurrentLevel(ClericFlag);
        int newrlvl = tempCHAR.GetCurrentLevel(RangerFlag);
        int newplvl = tempCHAR.GetCurrentLevel(PaladinFlag);
        int newmlvl = tempCHAR.GetCurrentLevel(MagicUserFlag);
        int newtlvl = tempCHAR.GetCurrentLevel(ThiefFlag);
        int newdlvl = tempCHAR.GetCurrentLevel(DruidFlag);
*/

        {
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          CString result;
          scriptContext.SetCharacterContext(pChar);
          result = pChar->RunCharacterScripts
                      (TRAINING_CHARACTER,
                      ScriptCallback_RunAllScripts,
                      NULL,
                      "Player selected \"TRAIN\"");
        };

        CString temp;
/*
        if (flvl != newflvl)
        {
          temp.Format("%s IS NOW A %i LEVEL %s",
                      tempCHAR.GetName(),
                      newflvl,
                      classData[Fighter].m_name);

          FormatPausedText(pausedTextData, temp);
          DisplayPausedText(pausedTextData);          
        }
        if (clvl != newclvl)
        {
          temp.Format("%s IS NOW A %i LEVEL %s",
            tempCHAR.GetName(),
            newclvl,
            classData[Cleric].m_name);

          FormatPausedText(pausedTextData, temp);
          DisplayPausedText(pausedTextData);          
        }
        if (rlvl != newrlvl)
        {
          temp.Format("%s IS NOW A %i LEVEL %s",
            tempCHAR.GetName(),
            newrlvl,
            classData[Ranger].m_name);
          
          FormatPausedText(pausedTextData, temp);
          DisplayPausedText(pausedTextData);          
        }
        if (plvl != newplvl)
        {
          temp.Format("%s IS NOW A %i LEVEL %s",
            tempCHAR.GetName(),
            newplvl,
            classData[Paladin].m_name);
          
          FormatPausedText(pausedTextData, temp);
          DisplayPausedText(pausedTextData);          
        }
        if (mlvl != newmlvl)
        {
          temp.Format("%s IS NOW A %i LEVEL %s",
            tempCHAR.GetName(),
            newmlvl,
            classData[MagicUser].m_name);
          
          FormatPausedText(pausedTextData, temp);
          DisplayPausedText(pausedTextData);          
        }
        if (tlvl != newtlvl)
        {
          temp.Format("%s IS NOW A %i LEVEL %s",
            tempCHAR.GetName(),
            newtlvl,
            classData[Thief].m_name);
          
          FormatPausedText(pausedTextData, temp);
          DisplayPausedText(pausedTextData);          
        }
        if (dlvl != newdlvl)
        {
          temp.Format("%s IS NOW A %i LEVEL %s",
            tempCHAR.GetName(),
            newdlvl,
            classData[Druid].m_name);
          
          FormatPausedText(pausedTextData, temp);
          DisplayPausedText(pausedTextData);          
        }
*/
        n = pChar->GetBaseclassStatsCount();
        for (i=0; i<n; i++)
        {
          BASECLASS_STATS *pBaseclassStats;
          pBaseclassStats = pChar->GetBaseclassStats(i);
          if (pBaseclassStats->currentLevel != 0)
          {
            if (pBaseclassStats->temp__origLevel != pBaseclassStats->currentLevel)
            {
              temp.Format("%s IS NOW A %i LEVEL %s",
                pChar->GetName(),
                pBaseclassStats->currentLevel,
                pChar->PeekBaseclassStats(i)->baseclassID.UniqueName());
              FormatPausedText(pausedTextData, temp);
              pausedTextData.slowText = TRUE;
              DisplayPausedText(pausedTextData, whiteColor, 0);
            };
          };
        };
        BOOL PickSpells = TRUE;


/*
        PickSpells = CanCastMagicUserSpells(pChar->GetClass());
*/ //Not Implemented(0xbe54a8, false); PickSpells = false;

        PickSpells = FALSE;
        {
          SetMyState(TASK_MainMenuTrain_SpellPick);
          if (PickSpells)
          {
            PushEvent(new INITIAL_MU_SPELLS_MENU_DATA(*pChar), DeleteEvent);
          }
          else
          {
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          };
        }
      }
    }
    break;

  case 5: // change class 
    SetMyState(TASK_MainMenuChangeClass_SpellPick);
    PushEvent(new CHANGE_CLASS_MENU_DATA(), DeleteEvent);
    break;

  case 6: // View
    PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),NULL), DeleteEvent);
    break; 
           
  case 7: // create
    PushEvent(new CREATE_CHARACTER_DATA, DeleteEvent);
    break;
    
  case 8: // delete
    {
      CString msg;
      msg.Format("%s WILL BE PERMANENTLY REMOVED. CONTINUE?", GetActiveChar(this).GetName());
      SetMyState(TASK_MainMenuDeleteChar);
      PushEvent(new ASK_YES_NO_MENU_DATA(msg), DeleteEvent);      
    }
    break;

  case 9: // Load
    PushEvent(new LOADGAME_MENU_DATA, DeleteEvent);
    break;
    
  case 10: // Save
    PushEvent(new SAVEGAME_MENU_DATA, DeleteEvent);
    break;
    
  case 11: // Begin
    {
      miscError = NoError;
      /*
      if (party.numCharacters < globalData.minPartySize)
      {  
        TEXT_EVENT_DATA *textEvent;
        textEvent=new TEXT_EVENT_DATA;
        textEvent->text = "Sorry, there are too few characters in your party";
        textEvent->pic.filename = "";
        textEvent->HighlightText=FALSE;
        textEvent->WaitForReturn = TRUE;
        PushEvent(textEvent,DeleteEvent);
        break;
      };
      */

      MouseRender.EnableWaitCursor();
      if (m_pOrigEvent == NULL) // if beginning for first time
      {
        menu.AddOverrideMenu("LOADING...", -1, FALSE);
        OnDraw();
        if (globalData.levelInfo.stats[globalData.startLevel].used)
        {
          if (levelExists(globalData.startLevel))
          {
            if (LoadLevel(globalData.startLevel))
            {
              TRACE("Design/Level data loaded successfully\n");
              party.setPartyAdventureState();
              party.setPartyLevelState(globalData.startLevel,
                         globalData.startX, globalData.startY,
                         globalData.startFacing);
              StartBackgroundMusic();
            }
            else 
            {
              debugSeverity = 8;
              WriteDebugString("Failed to load start level data\n");
              miscError = LevelLoadError;
            };
          }
          else
          {
            WriteDebugString("Bogus level data, start level does not exist\n");
            miscError = LevelLoadError;
          };
        }
        else
        {
          WriteDebugString("Bogus level data, start level not used\n");
          miscError = LevelLoadError;
        };
        menu.RemoveOverrideMenu();
      }
      MouseRender.DisableWaitCursor();
      if (NoError == miscError)
      {
        if (m_pOrigEvent != NULL)
        {
          // in this case, we are here because an event
          // pushed a MAIN_MENU_DATA onto the stack
          PopEvent();
        }
        else
        {
          ReplaceEvent(new ADVENTURE_MENU_DATA(party.Posx, party.Posy), DeleteEvent);

          BEGIN_ZONE_EVENT *zoneEvent=NULL;
          zoneEvent=new BEGIN_ZONE_EVENT(party.Posx,
                                         party.Posy);
          // this event will check for, and load, any events that
          // should trigger based on current x,y position in map
          BEGIN_XY_EVENT *xyEvent;
          xyEvent=new BEGIN_XY_EVENT(party.Posx,
                                     party.Posy,
                                     GetCurrentLevel());
          // this event will check for, and load, any step events
          // that are available.
          BEGIN_STEP_EVENT *stepEvent;
          stepEvent=new BEGIN_STEP_EVENT(party.Posx,
                                         party.Posy,
                                         GetCurrentLevel());

 /*
          BEGIN_TIME_EVENT *timeEvent;
           timeEvent=new BEGIN_TIME_EVENT(party.Posx,
             party.Posy,
             GetCurrentLevel(),
             party.days, party.hours, party.minutes);
 */
 
          // queue up the events  
          //PushEvent(restEvent, DeleteEvent);
          //PushEvent(timeEvent, DeleteEvent);
          PushEvent(xyEvent,   DeleteEvent);
          PushEvent(stepEvent, DeleteEvent);
          PushEvent(zoneEvent, DeleteEvent);
        }

        /*
        // test runtimeif lookup
        int i=0;
        char tmp[128];
        ActorType actor = GetActiveChar(this).GetContext();

        for (i=FIRST_CHAR_KEYWORD;i<=LAST_CHAR_KEYWORD;i++)
        {
          if (GetDataAsText(i, tmp, 127, &actor))
            WriteDebugString("CHAR_IF (%s): %s\n", NumToKeywordText(i), tmp);
          else
            ASS ERT(FALSE);
        }
        for (i=FIRST_PARTY_KEYWORD;i<=LAST_PARTY_KEYWORD;i++)
        {
          if (GetDataAsText(i, tmp, 127, &actor))
            WriteDebugString("PARTY_IF (%s): %s\n", NumToKeywordText(i), tmp);
          else
            ASS ERT(FALSE);
        }
        for (i=FIRST_GAME_KEYWORD;i<=LAST_GAME_KEYWORD;i++)
        {
          if (GetDataAsText(i, tmp, 127, &actor))
            WriteDebugString("GAME_IF (%s): %s\n", NumToKeywordText(i), tmp);
          else
            ASS ERT(FALSE);
        }
        */
      }
    }
    break;
    
  case 12: // exit
    SetMyState(TASK_AskQuitGame);
    PushEvent(new ASK_YES_NO_MENU_DATA("QUIT THE GAME?",TRUE, CONTROL_Main), DeleteEvent);
    //ReplaceEvent(new EXIT_DATA, DeleteEvent);
    break;
  };
}

void MAIN_MENU_DATA::OnInitialEvent(void)
{ 
  if (ProcInput.HaveWarp)
  {
    PerformWarp();
    return;
  }  
  SetMyState(TASK_initialized);
  StopAllSounds();
  menu.setVertOrient();
  menu.setMenu(MainMenuData, &Select_Main_Function_Format, FALSE, NULL, NULL);
  menu.setVertOrient(); // Yes....We insist
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 12);
  SetMyState(TASK_MainMenu);
  ClearFormattedText(textData);
  Invalidate();
  //ClearFormattedText(errorTextData);
}

void MAIN_MENU_DATA::OnReturnToTopOfQueue(void)
{
  switch (GetMyState())
  {
  case TASK_MainMenuModifyChar:
    GetActiveChar(this) = character;
    break;
  case TASK_MainMenuDeleteChar:
    if (party.tradeItem==1) // yes, delete character
      party.purgeCharacter(party.activeCharacter);
    break;
  case TASK_MainMenuRemoveChar:    
    if (party.tradeItem==1) // yes, remove char
    {
      MouseRender.EnableWaitCursor();
      //GetActiveChar(m_pOrigEvent, party.activeCharacter).serializeCharacter(TRUE, GetActiveChar(m_pOrigEvent, party.activeCharacter).GetName());
      party.removeCharacter(party.activeCharacter);     
      MouseRender.DisableWaitCursor();
    }
    break;
  case TASK_AskQuitGame:
    if (party.tradeItem==1) // yes, quit the game
    {
      ReplaceEvent(new EXIT_DATA, DeleteEvent);
      return;
    } 
    break;
  case TASK_MainMenuChangeClass_SpellPick:
    SetMyState(TASK_MainMenuLearn_SpellPick);
    PushEvent(new LEVEL_UP_LEARN_SPELLS_MENU(party.getActiveChar()), DeleteEvent);
    return;
  case TASK_MainMenuTrain_SpellPick:
    {
      SetMyState(TASK_MainMenuLearn_SpellPick);
      PushEvent(new LEVEL_UP_LEARN_SPELLS_MENU(party.getActiveChar()), DeleteEvent);
      return;
    };
  case TASK_MainMenuLearn_SpellPick:
    break;
  case TASK_MainMenu:
    break;
  default: 
    {
      TASKSTATE n = GetMyState();
      die("Unknown Main Menu state");
    };
  }
  OnInitialEvent();
}

void MAIN_MENU_DATA::OnDraw(void)
{
  UpdateStartScreen(&Select_Main_Function_Format, 0);
}

int MAIN_MENU_DATA::OnSaveGame(unsigned int *saveArea)
{
  if (m_pOrigEvent==NULL)
  {
    saveArea[0]=0;
  }
  else
  {
    saveArea[0]=m_pOrigEvent->GetEventId();
  };
  return 1;
}

int MAIN_MENU_DATA::OnLoadGame(unsigned int *saveArea)
{
  if (saveArea[0]==0) 
    m_pOrigEvent=NULL;
  else 
    m_pOrigEvent=levelData.eventData.GetEvent(saveArea[0]);
  return 1;
}

void MAIN_MENU_DATA::OnUpdateUI(void)
{
  /*
    ** original order ** 
   1 { "Create Character", 0 },
   2 { "Delete Character", 0 },
   3 { "Modify Character", 0 },
   4 { "Train Character", 0 },
   5 { "Human Change Class", 0 },
   6 { "View Character", 0 },
   7 { "Add Character", 0 },
   8 { "Remove Character", 0 },
   9 { "Load Saved Game", 0 },
   10 { "Save Current Game", 0 },
   11 { "Begin Adventuring", 0 },
   12 { "Exit From Game", 1 }

   ** new order ** 
   1 { "Add Character", 0 },
   2 { "Remove Character", 0 },
   3 { "Modify Character", 0 },
   4 { "Train Character", 0 },
   5 { "Human Change Class", 0 },
   6 { "View Character", 0 },
   7 { "Create Character", 0 },
   8 { "Delete Character", 0 },
   9 { "Load Saved Game", 0 },
   10 { "Save Current Game", 0 },
   11 { "Begin Adventuring", 0 },
   12 { "Exit From Game", 1 }
  */

  if (CheckForMultiBoxText()) return;

  if (IsGlobalEvent())
    menu.setItemInactive(10); // save

  if (IsCombatActive())
    menu.setItemInactive(10); // save

  if (m_pOrigEvent != NULL)
  {
    if (m_pOrigEvent->IsGlobalEvent())
      menu.setItemInactive(10); // save

    if (m_pOrigEvent->GetEventType() != TrainingHallEvent)
    {
      menu.setItemInactive(4); // train
      menu.setItemInactive(5); // change class
    }
    else
    {
      //if (!GetActiveChar(this).CanChangeClassHuman())
      //if (!GetActiveChar(this).CanChangeClass())
      {
        if (GetActiveChar(this).CreateChangeClassList(NULL) == 0)
        {
          menu.setItemInactive(5); // change class
        };
      };

      //
      // not handling multi-class properly here!
      //
      // 20110207 PRS.....Manikus says that only Humans with a single baseclass
      // can be trained.  They can only change to a class with a single
      // baseclass.  So I don't see any worry about multi-class!
      //
/*
      if (   (!((TRAININGHALL*)m_pOrigEvent)->TrainCleric)
          && (GetActiveChar(this).GetClass() == Cleric))
        menu.setItemInactive(4); // train
      if (   (!((TRAININGHALL*)m_pOrigEvent)->TrainFighter)
          && (GetActiveChar(this).GetClass() == Fighter))
        menu.setItemInactive(4); // train
      if (   (!((TRAININGHALL*)m_pOrigEvent)->TrainMagicUser)
          && (GetActiveChar(this).GetClass() == MagicUser))
        menu.setItemInactive(4); // train
      if (   (!((TRAININGHALL*)m_pOrigEvent)->TrainThief)
          && (GetActiveChar(this).GetClass() == Thief))
        menu.setItemInactive(4); // train
      if (   (!((TRAININGHALL*)m_pOrigEvent)->TrainRanger)
          && (GetActiveChar(this).GetClass() == Ranger))
        menu.setItemInactive(4); // train
      if (   (!((TRAININGHALL*)m_pOrigEvent)->TrainPaladin)
          && (GetActiveChar(this).GetClass() == Paladin))
        menu.setItemInactive(4); // train
      if (   (!((TRAININGHALL*)m_pOrigEvent)->TrainDruid)
          && (GetActiveChar(this).GetClass() == Druid))
        menu.setItemInactive(4); // train
*/
      BOOL trainActive = FALSE;
      if (     (GetActiveChar(this).GetAdjReadyToTrain())
            && (GetActiveChar(this).poolCharacterGold() >= ((TRAININGHALL*)m_pOrigEvent)->Cost))
      {
        const CHARACTER *pChar;
        const TRAININGHALL *pEvent;
        int i, n;
        pChar = &GetActiveChar(this);
        pEvent = (TRAININGHALL *)m_pOrigEvent;
        {
          n = pChar->GetBaseclassStatsCount();
          for (i=0; i<n; i++)
          {
            const BASECLASS_STATS *pBaseclassStats;
            pBaseclassStats = pChar->PeekBaseclassStats(i);
            if (pChar->IsReadyToTrain(pBaseclassStats))
            {
              if (pEvent->LocateTrainableBaseclass(pBaseclassStats->baseclassID) >= 0)
              {
                trainActive = TRUE;  // "TRAIN"
              };
            };
          };
        };
      };
      if (trainActive) menu.setItemActive(4);
      else menu.setItemInactive(4);
    };
  }
  else
  {
    menu.setItemInactive(4); // train
    menu.setItemInactive(5); // change class
  //  menu.setItemInactive(10); // Save game
  }

  {
    int pcAvail  = 0;
    int npcAvail = 0;
    int numPC    = 0;
    // check for active characters
    for (int i=0; i<party.numCharacters; i++)
    {
      if (party.characters[i].GetAdjHitPoints() < 0)
        party.characters[i].SetStatus(Dead);
      if (party.characters[i].GetType() == CHAR_TYPE)
      {
        numPC++;
      };
      if (party.characters[i].GetAdjStatus() == Okay)
      {
        if (party.characters[i].GetType() == NPC_TYPE)
        {
          npcAvail++;
        }
        else
        {
          pcAvail++;
        };
      };
    }
    if (pcAvail+npcAvail == 0)
      menu.setItemInactive(11); // Begin Adventuring

    if (!EditorMode())
    {
      // 20140307  Jadefaang reported that if maxPartSize is 2 and there
      // are two PCs and 1 NPC then "BeginAdventuring" did not appear.
      // He says that maxPartySize only applies to the PCs, not the NPCs.
      // So be it.

      //if (party.numCharacters < globalData.minPartySize)
      //if (party.numCharacters < globalData.GetMinPCS)
      if (party.numCharacters < globalData.GetMinPCs())
        menu.setItemInactive(11); // Begin Adventuring
      //if (numPC > globalData.maxPartySize)
      if (numPC > globalData.GetMaxPCs())
        menu.setItemInactive(11); // Begin Adventuring
    }
  };

  if (!GetActiveChar(this).CanBeModified())
    menu.setItemInactive(3); // modify

  if (party.numCharacters == 0)
  {
    menu.setItemInactive(8); // delete
    menu.setItemInactive(3); // modify
    menu.setItemInactive(6); // view
    menu.setItemInactive(2); // remove
    menu.setItemInactive(11); // begin
  }

  //if (tempChars.numChars() <= 0)
  //  menu.setItemInactive(1); // add
  
  //if (party.numCharacters >= GetMaxPartyCharacters())
  {
    int numPC;
    int i;
    for (i=0, numPC=0; i<party.numCharacters; i++)
    {
      if (party.characters[i].GetType() == CHAR_TYPE)
      {
        numPC++;
      };
    };
    if (numPC >= GetMaxPartyPCs())
      menu.setItemInactive(1); // add
    //if (party.numCharacters >= globalData.maxPartySize)
    if (party.numCharacters >= globalData.GetMaxPartySize())
      menu.setItemInactive(1); // add
  };

  //if (!globalData.allowCharCreate)
  if (!globalData.GetAllowCharCreate())
    menu.setItemInactive(7); // create

  if (tempChars.numChars() >= MAX_TEMP_CHARS)
  {
    menu.setItemInactive(7); // create
    menu.setItemInactive(2); // remove
  }
  // NPC is highlighted
  if (GetActiveChar(this).GetType() == NPC_TYPE)
  {
    //menu.setItemInactive(8); // delete
    menu.setItemInactive(3); // modify    
  }
}

void MAIN_MENU_DATA::PerformWarp(void)
{
  PlayIntro(FALSE);
  ProcInput.HaveWarp=FALSE;
  ClearFormattedText(textData);
  WriteDebugString("Warping to x,y,level,facing: %i, %i, %i, %i\n",
                   ProcInput.wx,ProcInput.wy,ProcInput.wl,ProcInput.wf);

  mCList<CHARACTER, CHARACTER&> PartyMembers;
  CHARACTER dude;

  if (ProcInput.PartyList.IsEmpty())
  {
    // load temp character into party    
    dude.SetAlignment(ChaoticNeutral);
    //dude.race((raceType)0);
    dude.RaceID(raceData.PeekRace(0)->RaceID());
    dude.SetGender(Male);
    //dude.SetClass(Fighter);
    dude.SetClass(classData.PeekClass(0)->ClassID());
    dude.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
    dude.SetName("Super Spud");
    FindTestCharArt(dude);  
    //dude.serializeCharacter(TRUE, dude.GetName());
    if (dude.GetMaxHitPoints() > 0)
      PartyMembers.AddTail(dude);
  }
  else
  {
    POSITION pos = ProcInput.PartyList.GetHeadPosition();
    while (pos != NULL)
    {
      dude.Clear();
      if (dude.serializeCharacter(FALSE, ProcInput.PartyList.GetAt(pos)))
      {
        PartyMembers.AddTail(dude);
      }
      else
      {
        debugSeverity = 8;
        WriteDebugString("Failed to load starting party member %s\n", ProcInput.PartyList.GetAt(pos));
      }

      ProcInput.PartyList.GetNext(pos);
    }
  }

  MouseRender.EnableWaitCursor();
  if (globalData.levelInfo.stats[ProcInput.wl].used)
  {
    if (levelExists(ProcInput.wl))
    {
      StopAllSounds();
      if (LoadLevel(ProcInput.wl))
      {
        TRACE("Warp Design/Level data loaded successfully\n");
        party.setPartyAdventureState();
        party.setPartyLevelState(ProcInput.wl, ProcInput.wx, ProcInput.wy, ProcInput.wf);

        POSITION pos = PartyMembers.GetHeadPosition();
        while (pos != NULL)
        {
          party.addTempToParty(*PartyMembers.GetNext(pos));
        }

        StartBackgroundMusic();
        ReplaceEvent(new ADVENTURE_MENU_DATA(party.Posx, party.Posy), DeleteEvent);

        BEGIN_ZONE_EVENT *zoneEvent=NULL;
        zoneEvent=new BEGIN_ZONE_EVENT(party.Posx,
                                       party.Posy);
        // this event will check for, and load, any events that
        // should trigger based on current x,y position in map
        BEGIN_XY_EVENT *xyEvent;
        xyEvent=new BEGIN_XY_EVENT(party.Posx,
                                   party.Posy,
                                   GetCurrentLevel());
        // this event will check for, and load, any step events
        // that are available.
        BEGIN_STEP_EVENT *stepEvent;
        stepEvent=new BEGIN_STEP_EVENT(party.Posx,
                                       party.Posy,
                                       GetCurrentLevel());

/*
        BEGIN_TIME_EVENT *timeEvent;
        timeEvent=new BEGIN_TIME_EVENT(party.Posx,
          party.Posy,
          GetCurrentLevel(),
          party.days, party.hours, party.minutes);*/


        // queue up the events  
        //PushEvent(timeEvent, DeleteEvent);
        PushEvent(xyEvent,   DeleteEvent);
        PushEvent(stepEvent, DeleteEvent);
        PushEvent(zoneEvent, DeleteEvent);

        ProcInput.MoveMouseTo(0,0);
      }
      else 
      {
        debugSeverity = 8;
        WriteDebugString("Failed to load start warp level data\n");
      };
    }
    else
      WriteDebugString("Bogus warp level data, start level does not exist\n");
  }
  else
    WriteDebugString("Bogus warp level data, start level not used\n");
  MouseRender.DisableWaitCursor();
}
////////////////////////////////////////////////////////CREATE_CHARACTER_DATA
void CREATE_CHARACTER_DATA::OnInitialEvent(void)
{
  menu.reset(NULL);
  character.Clear();
  //GraphicsMgr.EnableFontColorTags(FALSE);
  SetMyState(TASK_Create_Character_Start);
  ClearFormattedText(textData);
  ClearFormattedText(errorTextData);
  Invalidate();
}

bool CREATE_CHARACTER_DATA::OnIdle(void)
{  
  if (m_AbortCharCreation)
  {
    PopEvent(); // user signaled to exit from character creation
    return true; // break out of OnIdle loop
  }

  switch (GetMyState())
  {
  case TASK_Create_Character_Start:
      SetMyState(TASK_Create_Character_Race);
      PushEvent(new RACE_MENU_DATA(this),DeleteEvent);
      break;
  case TASK_Create_Character_Race:
      SetMyState(TASK_Create_Character_Gender);
      PushEvent(new GENDER_MENU_DATA(this), DeleteEvent);
      break;
  case TASK_Create_Character_Gender:
      SetMyState(TASK_Create_Character_Class);
      PushEvent(new CLASS_MENU_DATA(this), DeleteEvent);
      break;
  case TASK_Create_Character_Class:
    {
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetCreatedCharacterContext(&character);
      RunGlobalScript(CHARACTER_CREATED_SA, POST_CLASS, true);
      SetMyState(TASK_Create_Character_Alignment);
      PushEvent(new ALIGNMENT_MENU_DATA(this), DeleteEvent);
    };
      break;
  case TASK_Create_Character_Alignment:
      SetMyState(TASK_Create_Character_ChooseStats);
      PushEvent(new CHOOSESTATS_MENU_DATA(true), DeleteEvent); // true: generate new character
      break;
  case TASK_Create_Character_ChooseStats:
      SetMyState(TASK_Create_Character_GetCharName);
      PushEvent(new GETCHARNAME_MENU_DATA, DeleteEvent);
      break;
  case TASK_Create_Character_GetCharName:
      SetMyState(TASK_Create_Character_IconPick);
      PushEvent(new GETCHARICON_MENU_DATA, DeleteEvent);
      break;
  case TASK_Create_Character_IconPick:
      SetMyState(TASK_Create_Character_SmallPicPick);
      PushEvent(new GETCHARSMALLPIC_MENU_DATA, DeleteEvent);
      break;
  case TASK_Create_Character_SmallPicPick:
    {
      character.ClearSpellbook();
      character.UpdateSkillBasedStats();

      // Don't assume that newly created characters are first level. Default
      // settings may provide enough experience points to make the new 
      // character a higher level.      
      


      BOOL knowSpellsAtCreation;
      
/*      
      PickMUSpells = CanCastMagicUserSpells(character.GetClass());
 Not Implemented(0xbe41bb, false); */ knowSpellsAtCreation = TRUE;

      
      if (knowSpellsAtCreation)
      {
        SetMyState(TASK_Create_Character_SpellPick);
        character.ClrKnowableSpell();
        PushEvent(new INITIAL_MU_SPELLS_MENU_DATA(character), DeleteEvent);
      }
      else
      {        
        SetMyState(TASK_Create_Character_AskToSave);
        CString msg;
        character.UpdateSpellAbility();
        msg.Format("DO YOU WANT TO SAVE %s?", character.GetName());
        PushEvent(new ASK_YES_NO_MENU_DATA(msg,TRUE, CONTROL_CreateCharacter), DeleteEvent);
      }
    }
    break;
  case TASK_Create_Character_SpellPick:      
    {
      SetMyState(TASK_Create_Character_SpellLearn);
      character.UpdateSpellAbility();
      PushEvent(new LEARN_SPELLS_MENU(character), DeleteEvent);
    }
    break;
  case TASK_Create_Character_SpellLearn:
    {
      SetMyState(TASK_Create_Character_AskToSave);
      CString msg;
      msg.Format("DO YOU WANT TO SAVE %s?", character.GetName());
      PushEvent(new ASK_YES_NO_MENU_DATA(msg,TRUE, CONTROL_CreateCharacter), DeleteEvent);
    }
    break;
  case TASK_Create_Character_AskToSave:
    if (party.tradeItem==1) // yes, save character
    {
      MouseRender.EnableWaitCursor();


      {
        SCRIPT_CONTEXT scriptContext;
        HOOK_PARAMETERS hookParameters;
        CString result;
        const CLASS_DATA *pClass;
        const RACE_DATA *pRace;
        CHARACTER_ID characterID;
        characterID = character.GetName();
        character.CharacterID(characterID);
        //pClass = classData.GetClassData(character.GetClass());
        pClass = classData.PeekClass(character.GetClass());
        //pRace =  raceData.GetRaceData(character.race());
        //pRace =  raceData.PeekRace(character.RaceID());
        pRace = character.PeekRaceData();
        scriptContext.SetClassContext(pClass);
        scriptContext.SetRaceContext(pRace);
        scriptContext.SetCreatedCharacterContext(&character);
        RunGlobalScript(CHARACTER_CREATED_SA, POST_CREATE_CHARACTER, true);



        pRace->RunRaceScripts(
                            POST_CREATE_CHARACTER,
                            ScriptCallback_RunAllScripts,
                            &hookParameters,
                            "Finished creating new character");

        pClass->RunClassScripts(
                            POST_CREATE_CHARACTER,
                            ScriptCallback_RunAllScripts,
                            &hookParameters,
                            "Finished creating new character");
        {
          int i, n;
          //baseclassKey baseClass;
          BASECLASS_ID baseclassID;
          //baseClass = pClass->GetNextBaseclass(classUnknown);    
          n = pClass->GetCount();
          for (i=0; i<n; i++)
          {
          //baseClassID = pClass->GetNextBaseclass(classUnknown);    
          //for  (;baseClass != classUnknown; baseClass = pClass->GetNextBaseclass(baseClass))
          //{
            const BASE_CLASS_DATA *pBaseclass;
            //pBaseClass = baseclassData.GetBaseclassData(baseClass);
            baseclassID = *pClass->PeekBaseclassID(i);
            pBaseclass = baseclassData.PeekBaseclass(baseclassID);
            if (pBaseclass == NULL) continue;
            scriptContext.SetBaseClassContext(pBaseclass);
            pBaseclass->RunBaseClassScripts(
                            POST_CREATE_CHARACTER,
                            ScriptCallback_RunAllScripts,
                            &hookParameters,
                            "Finished creating new character");
          };
        };
      };


      character.serializeCharacter(TRUE, character.GetName());
      MouseRender.DisableWaitCursor();
    }
    //GraphicsMgr.EnableFontColorTags(TRUE);
    PopEvent(); // All done
    break;

  default:
    //GraphicsMgr.EnableFontColorTags(TRUE);
    WriteDebugString("Bogus state in CREATE_CHARACTER_DATA\n");
    die(0xab4e7);
    PopEvent();
  };
  return false;
}

///////////////////////////////////////////////////////////ADD_CHARACTER_DATA
void ADD_CHARACTER_DATA::OnKeypress(key_code key, char ascii)
{ 
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key,ascii);
    return;
  };
  {
    int selectedMenuIndex;
    int selectedCharIndex;
    int numCharDisplayed;
    int numLinesAvailable;
    int i;
    char temp[100];
    CHARACTER tempCHARACTER;

    numLinesAvailable = Items_Per_Page - 1;  // "EXIT"
    selectedMenuIndex = menu.currentItem() - 1;
    selectedCharIndex = m_firstCharIndex + selectedMenuIndex;
    numCharDisplayed = tempChars.numChars() - m_firstCharIndex;
    if (m_firstCharIndex != 0)
    {
      selectedCharIndex--;
      numCharDisplayed--;
      numLinesAvailable--;
    };
    if ( (m_firstCharIndex != 0) && (selectedMenuIndex == 0) )
    {
      m_firstCharIndex -= Items_Per_Page - 2;
      if (m_firstCharIndex == 1)
      {
        m_firstCharIndex = 0;
      };
      FillMenu();
      return;
    };

    if (numCharDisplayed > numLinesAvailable)
    {
      numLinesAvailable--; // "NEXT"
      numCharDisplayed = numLinesAvailable;
      if (selectedMenuIndex == Items_Per_Page-2)
      {
        // "NEXT -->"
        m_firstCharIndex += numCharDisplayed;
        FillMenu();
        return;
      };
    };



    if (selectedMenuIndex == menu.totalItems() - 1) // exit menu
    {
      for (i=0;i<tempChars.numChars();i++)
      {
        //tempCHARACTER = tempChars.GetCharacter(i);
        tempCHARACTER = *tempChars.GetCharacter(i);
        if (tempChars.InParty(i)) // add to party
        {
          CHARACTER *pAddedCharacter;
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          pAddedCharacter = party.addTempToParty(tempCHARACTER);
          if (pAddedCharacter != NULL)
          {
            scriptContext.SetCharacterContext(pAddedCharacter);
            pAddedCharacter->RunCharacterScripts(
                             JOIN_PARTY,
                             ScriptCallback_RunAllScripts,
                             NULL,
                             "A character has joined ed the party");
          };
        }
        else // remove from party
        {
          for (int j=0;j<party.numCharacters;j++)
          {
            if (party.characters[j].IsSameCharacter(tempCHARACTER))
              party.removeCharacter(j,FALSE);
          }
        }
      }    
      PopEvent();
      return;
    }
    CHARACTER *pSelChar;
    pSelChar = tempChars.GetCharacter(selectedMenuIndex);
    if (tempChars.InParty(selectedCharIndex))
    {
      menu.changeMenuItem(selectedMenuIndex+1, tempChars.GetCharName(selectedCharIndex));
      tempChars.SetInParty(selectedCharIndex, FALSE, pSelChar->GetType() == CHAR_TYPE);
    }
    else 
    {
      int countParty, countPC;
      countParty=tempChars.CountInPartyMembers();
      countPC   =tempChars.CountInPartyPCs();
      //if (count  < GetMaxPartyCharacters())
      if (countParty < GetMaxPartyMembers())
      {
        if (    (pSelChar->GetType() != CHAR_TYPE) 
             || (countPC  < GetMaxPartyPCs()))
        {
          sprintf(temp, "* %s", (LPCSTR)tempChars.GetCharName(selectedCharIndex));
          menu.changeMenuItem(selectedMenuIndex+1, temp);
          tempChars.SetInParty(selectedCharIndex, TRUE, pSelChar->GetType() == CHAR_TYPE);
        };
      }
    }
  };
}


void ADD_CHARACTER_DATA::FillMenu(void)
{
  int numLinesDisplayed;
  int numLinesRemaining;
  int numCharRemaining;
  int numCharToDisplay;
  bool needNext;

  {
  // Sort the characters so that their order
  // will not depend on the operating system
  // that supplied the file names.
  // A very simple sort should suffice for a
  // few characters
    int i, n;
    n = tempChars.numChars();
    for (i=0; i<n-1; i++)
    {
      if (tempChars.GetCharName(i).CompareNoCase(tempChars.GetCharName(i+1)) > 0)
      {
        tempChars.Swap(i, i+1);  // Just a key and a flag.
        i=-1;
      };
    };
  };



  menu.reset(&Select_Add_Character_Format);
  //menu.setStartCoord(AddCharMenuX, AddCharMenuY);
  menu.setStartCoord(Select_Add_Character_Format.Menu_x, Select_Add_Character_Format.Menu_y, FALSE);
  menu.setTitle(Select_Add_Character_Format.Title_Format);
  menu.setTitleXY(Select_Add_Character_Format.Title_x, Select_Add_Character_Format.Title_y);
  menu.setHeader(Select_Add_Character_Format.Header_Format);
  menu.setHeaderXY(Select_Add_Character_Format.Header_x, Select_Add_Character_Format.Header_y);

  numLinesDisplayed = 0;
  if (m_firstCharIndex != 0)
  {
    menu.addMenuItem("<--- PREV");
    numLinesDisplayed = 1;
  };
  numCharRemaining = tempChars.numChars() - m_firstCharIndex;
  numLinesRemaining = Items_Per_Page - numLinesDisplayed - 1; // Room for "EXIT"
  if (numCharRemaining > numLinesRemaining)
  {
    numCharToDisplay = numLinesRemaining-1;
    needNext = true;
  }
  else
  {
    numCharToDisplay = numCharRemaining;
    needNext = false;
  };

  for (int i=0;i<numCharToDisplay;i++)
  {
    if (tempChars.InParty(i+m_firstCharIndex))
    {
      CString tmp;
      tmp.Format("* %s", tempChars.GetCharName(i+m_firstCharIndex));
      menu.addMenuItem(tmp);
    }
    else
      menu.addMenuItem(tempChars.GetCharName(i+m_firstCharIndex));
  }
  if (needNext)
  {
    menu.addMenuItem("NEXT --->");
  };
  menu.addMenuItem("EXIT", 1);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, menu.totalItems());  
  Invalidate();
}
  
void ADD_CHARACTER_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);

  tempChars.Clear();
  // add saved characters, doesn't include PreGen NPCs
  tempChars.AddSavedCharacters();

  // add pre-gen NPC's
  //POSITION pos = globalData.charData.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = globalData.charData.GetCount();
  for (i=0; i<n; i++)
  {
    //if (globalData.charData.GetAt(pos).IsPreGen)
    if (globalData.charData.PeekCharacter(i)->IsPreGen)
    {
      //tempChars.addPreGenCharacter(globalData.charData.GetAt(pos), globalData.charData.GetAt(pos).uniqueKey);    
      tempChars.addPreGenCharacter(*globalData.charData.PeekCharacter(i));    
    };
    //globalData.charData.GetNext(pos);
  }

  m_firstCharIndex = 0;
  FillMenu();

}

void ADD_CHARACTER_DATA::OnDraw(void)
{
  StandardMenuDraw();
}

////////////////////////////////////////////////////////////////RACE_MENU_DATA
void RACE_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  switch (menu.currentItem()) 
  {
  case 1: // select
    {
      //raceType rt = (raceType)spellListText.GetSpellIndex(currSpellTextIndex());
      RACE_ID raceID = spellListText.GetRace(currSpellTextIndex());
      //character.race(rt);
      character.RaceID(raceID);
    //ReplaceEvent(new GENDER_MENU_DATA(m_pOrig), DeleteEvent); 
      PopEvent();
    }
    break;    
  case 2: // next
    nextSpellPage(&Select_Race_Format);
    break;
  case 3: // prev
    prevSpellPage(&Select_Race_Format);
    break;
  case 4: // exit
    m_pOrig->m_AbortCharCreation=true;
    PopEvent(); // pop to exit event
    break;
  default:
    WriteDebugString("Unhandled menu item in RACE_MENU_DATA");
    break;
  }   
}


void RACE_MENU_DATA::OnMouseClickLeft(int mx, int my)
{  
  if (MouseInventory(mx, my))
  {
    simulatedKeyQueue.PushKey(VK_RETURN);
    menu.setCurrentItem(1);  //"SELECT"
  }
  else
  {
    MouseMenu(mx, my);
  };
}

void RACE_MENU_DATA::OnInitialEvent(void)
{
  RACE_ID raceID;
  ClearFormattedText(textData);
  //menu.setMenu(RaceMenuData);
  //menu.setVertOrient();    
  menu.setMenu(SelectMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);

  clearSpellItemState();
  spellListText.FormatSpellListTitle(Select_Race_Format.Title_Format);
  spellListText.m_flags = SHOW_DESIGN_FLAG;    
  //POSITION pos = raceData.GetHeadPosition();
  //while (pos!=NULL)
  spellListText.m_flags = RACE_FORMAT_FLAG;
  int i, n;
  n = raceData.GetCount();
  for (i=0; i<n; i++)
  {
    //GLOBAL_SPELL_ID gsID;
    //gsID.IllegalConversion(raceData.GetKeyAt(pos));
    //spellListText.Add(gsID);
    //spellListText.Add((raceType)raceData.GetKeyAt(pos));
    const RACE_DATA *pRace;
    pRace = raceData.PeekRace(i);
    if (pRace == NULL) continue;
    if (pRace->Name() == "$$Help") continue;
    raceID = pRace->Name();
    {
      CString className;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetRaceContext(pRace);
      if (!pRace->RunRaceScripts(
                                  SELECT_CHARACTER_RACE,
                                  ScriptCallback_RunAllScripts, 
                                  NULL,
                                  "We are about to ask the player to  select a race").IsEmpty()) continue;
    };

    spellListText.Add(raceID);
    //raceData.GetNext(pos);
  }

  int scnt = spellListText.GetRaceCount();
  //ItemsOnPage = min(Items_Per_Page, scnt);
  ItemsOnPage = min(Select_Race_Format.Lines_Per_Page, scnt);
  if (ItemsOnPage > 0)
  {
    if (party.activeItem >= spellListText.GetRaceCount())
      party.activeItem = (BYTE)(spellListText.GetRaceCount()-1);
    if (party.activeItem >= (startItem+ItemsOnPage))
      party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    //currShopItem = 0;
  }
  Invalidate();
}

void RACE_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  
  menu.setAllItemsActive();
  if (spellListText.GetRaceCount() <= 0)
    menu.setItemInactive(1); // select    
  if (!allowSpellNextPage(&Select_Race_Format))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(&Select_Race_Format))
    menu.setItemInactive(3); // prev
}

void RACE_MENU_DATA::OnDraw(void)
{
  //StandardMenuDraw();
  UpdateRaceMenuScreen(Select_Race_Format);
}

/////////////////////////////////////////////////////GETCHARSMALLPIC_MENU_DATA
void GETCHARSMALLPIC_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(GetCharSmallPicMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();

  // find all small pics in design folder
  int i=1;
  CString temp;
  PIC_DATA spic;
  spic.picType = SmallPicDib;
  spic.SetDefaults();  
  int origCount = global_SmallPicImport.GetCount();

  while (i <= 50)
  {
   temp.Format("prt_SPic%u.png", i);
   // uses root name to search for any matches
   // using valid image extensions (bmp, pcx, etc)
   if (FindImageWithValidExt(temp, rte.SmallPicDir()))
   {
     spic.filename = temp;
     // make sure this pic is not already in the list
     // only check 1..origCount
     POSITION pos = global_SmallPicImport.GetHeadPosition();
     int c=0;
     BOOL found=FALSE;
     while ((pos != NULL) && (c < origCount) && (!found))
     {
       if (global_SmallPicImport.GetAt(pos)->filename == spic.filename)
         found = TRUE;
       c++;
       global_SmallPicImport.PeekNext(pos);
     }
     if (!found)
       global_SmallPicImport.AddTail(spic);
   }
   i++;
  }

  numSmallPics = global_SmallPicImport.GetCount();
  character.smallPic.FreePic();
  currSmallPicSelected = 0;
  if (numSmallPics > 0)
  {
    for (i=0;i<numSmallPics;i++)
    {
      POSITION pos;
      pos = global_SmallPicImport.FindIndex(i);
      if (character.smallPic == *global_SmallPicImport.PeekAt(pos))
      {
        currSmallPicSelected = i;
        break;
      }
    }
    {
      POSITION pos;
      pos = global_SmallPicImport.FindIndex(currSmallPicSelected);
      character.smallPic = *global_SmallPicImport.GetAt(pos);
      character.smallPic.picType = SmallPicDib;
      //character.smallPic.LoadPicSurfaces("prt_");
      character.smallPic.LoadPicSurfaces("");
    };
  }
  else 
  {
    character.smallPic.Clear();
    miscError = NoSmallPics;
    //PopEvent();
  };
  Invalidate();
}
void GETCHARSMALLPIC_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  if (numSmallPics <= 1)
  {
    menu.setItemInactive(1); // next
    menu.setItemInactive(2); // prev
  }    
}

void GETCHARSMALLPIC_MENU_DATA::OnDraw(void)
{
  UpdateSmallPicScreen();
}

void GETCHARSMALLPIC_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  POSITION pos;
  if (key != KC_RETURN) 
  {
    StandardMenuKeyboardAction(key,ascii);
    return;
  };

  switch (menu.currentItem()) 
  {
  case 1: // next
    if (currSmallPicSelected < numSmallPics-1)
      currSmallPicSelected++;
    else
      currSmallPicSelected = 0;    
    character.smallPic.FreePic();
    pos = global_SmallPicImport.FindIndex(currSmallPicSelected);
    character.smallPic = *global_SmallPicImport.GetAt(pos);
    character.smallPic.picType = SmallPicDib;
    //character.smallPic.LoadPicSurfaces("prt_");
    character.smallPic.LoadPicSurfaces("");
    break;    
  case 2: // prev
    if (currSmallPicSelected > 0)
      currSmallPicSelected--;
    else
      currSmallPicSelected = numSmallPics-1;

    character.smallPic.FreePic();
    pos = global_SmallPicImport.FindIndex(currSmallPicSelected);
    character.smallPic = *global_SmallPicImport.GetAt(pos);
    character.smallPic.picType = SmallPicDib;
    //character.smallPic.LoadPicSurfaces("prt_");
    character.smallPic.LoadPicSurfaces("");
    break;    
  case 3: // select
    character.smallPic.FreePic();
    currPic.Clear();
    PopEvent();
    break;    
  default:
    WriteDebugString("Unhandled menu item in handleSmallPicPick");
    miscError = InvalidMenuState;
    PopEvent();
    break;
  }  
}
/////////////////////////////////////////////////////////GETCHARICON_MENU_DATA
void GETCHARICON_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  POSITION pos;
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  };
 
  switch (menu.currentItem()) 
  {
  case 1: // next
    if (currIconSelected < numIconPics-1)
      currIconSelected++;
    else
      currIconSelected = 0;

    character.icon.FreePic();
    pos = global_IconPicImport.FindIndex(currIconSelected);
    character.icon = *global_IconPicImport.GetAt(pos);
    character.icon.picType = IconDib;
    //character.icon.LoadPicSurfaces("cn_");
    character.icon.LoadPicSurfaces("");
    break;
    
  case 2: // prev
    if (currIconSelected > 0)
      currIconSelected--;
    else
      currIconSelected = numIconPics-1;
    character.icon.FreePic();
    pos = global_IconPicImport.FindIndex(currIconSelected);
    character.icon = *global_IconPicImport.GetAt(pos);
    character.icon.picType = IconDib;
    //character.icon.LoadPicSurfaces("cn_");
    character.icon.LoadPicSurfaces("");
    break;
    
  case 3: // select
    character.icon.FreePic();
    currPic.Clear();
    GraphicsMgr.ReleaseSurface(SpriteBkGndArt);
    PopEvent(); // All done.
    break;
    
  default:
    WriteDebugString("Unhandled menu item in handleIconPicPick");
    miscError = InvalidMenuState;
    PopEvent();
    break;
  }
}

void GETCHARICON_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(GetCharIconMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();        
  if (SpriteBkGndArt > 0)
    GraphicsMgr.ReleaseSurface(SpriteBkGndArt);
  character.icon.FreePic();

  // find all small pics in design folder
  int i=1;
  CString temp;
  PIC_DATA spic;
  spic.picType = IconDib;
  spic.SetDefaults();
  int origCount = global_IconPicImport.GetCount();

  while (i <= 50)
  {
     temp.Format("cn_Icon%u.png", i);
     // uses root name to search for any matches
     // using valid image extensions (bmp, pcx, etc)
     if (FindImageWithValidExt(temp, rte.IconArtDir()))
     {
       spic.filename = temp;
       // make sure this pic is not already in the list
       // only check 1..origCount
       POSITION pos = global_IconPicImport.GetHeadPosition();
       int c=0;
       BOOL found=FALSE;
       
       while ((pos != NULL) && (c < origCount) && (!found))
       {
         CString ConfigIconFilename(global_IconPicImport.GetAt(pos)->filename);
         StripFilenamePath(ConfigIconFilename);
         CString FoundIconFilename(spic.filename);
         StripFilenamePath(FoundIconFilename);

         if (ConfigIconFilename.CompareNoCase(FoundIconFilename) == 0)
           found = TRUE;

         c++;
         global_IconPicImport.GetNext(pos);
       }

       if (!found)
         global_IconPicImport.AddTail(spic);
     }
     i++;
  }

  numIconPics = global_IconPicImport.GetCount();
  character.icon.FreePic();
  currIconSelected = 0;
  if (numIconPics > 0)
  {
    for (i=0;i<numIconPics;i++)
    {
      POSITION pos;
      pos = global_IconPicImport.FindIndex(i);
      if (character.icon == *global_IconPicImport.GetAt(pos))
      {
        currIconSelected = i;
        break;
      }
    }
    {
      POSITION pos;
      pos = global_IconPicImport.FindIndex(currIconSelected);
      SpriteBkGndArt = GraphicsMgr.AddFileSurface(globalData.IconBgArt, CommonDib, rte.IconArtDir());
      character.icon = *global_IconPicImport.GetAt(pos);
      character.icon.picType=IconDib;
      //character.icon.LoadPicSurfaces("cn_");
      character.icon.LoadPicSurfaces("");
    };
  }
  else 
  {
    character.icon.Clear();
    miscError = NoIcons;
  };
  Invalidate();
}

void GETCHARICON_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  if (numIconPics <= 1)
  {
    menu.setItemInactive(1); // next
    menu.setItemInactive(2); // prev
  }
}

void GETCHARICON_MENU_DATA::OnDraw(void)
{
  UpdateIconPickScreen();
}

/////////////////////////////////////////////////////////GETCHARNAME_MENU_DATA
void GETCHARNAME_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  int i;
  if (key == KC_RETURN)
  {
	  char newName[50];
	  newName[0] = '\0';
    // ignore blank on the end
    for (i=1;i<=menu.totalItems();i++)
      strcat(newName, menu.getMenuItem(i));
    character.SetName(newName);
    bool isdup=false;
    for (i=0;i<party.numCharacters;i++)
    {
      if (party.characters[i].IsSameCharacter(character))
        isdup=true;
    }
    if (tempChars.IsCharacterInTemps(character))
      isdup=true;
    if (!isdup)
    {
      //if (menu.totalItems() > 1)
      {
        tempChars.Clear();
        PopEvent();
      }
    }
    else
    {
      name = character.GetName();
      CString msg;
      msg.Format("%s ALREADY EXISTS AS A SAVED CHARACTER, REPLACE?", name);
      PushEvent(new ASK_YES_NO_MENU_DATA(msg), DeleteEvent);      
      //miscError=DupCharName;
    }
    return; // Need at least one character
  }
  else if (( (key == KC_CHAR) 
          || (key == KC_NUM) 
          || (key == KC_SPACE)
          || (key == KC_PUNCTUATION))
             //&& (menu.totalItems() < MAX_CHAR_NAME)) // 15 chars + blank on end
             && (name.GetLength() < MAX_CHAR_NAME)) // 15 chars + blank on end
  {
    // dont allow space to be first char in name
    //if ((menu.totalItems() == 1)  && (key == KC_SPACE))
    if ((name.GetLength() == 0) && (key == KC_SPACE))
    {
      return;
    };
    //sprintf(temp, "%c", ascii);
    //menu.changeMenuItem(menu.totalItems(), temp);
    if (ascii == '?') return;
    if (ascii == '/') return;
    name += ascii;
    menu.changeMenuItem(1, name);
    //menu.addMenuItem(" ");
    //menu.setCurrentItem(menu.totalItems());

  }
  else if (   ((key == KC_BACK) || (key == KC_LEFT))
           && (menu.totalItems() > 0) )
  {
    //menu.deleteLastMenuItem();
    //menu.changeMenuItem(menu.totalItems(), " ");
    //menu.setItemActive(menu.totalItems());
    //if (menu.totalItems() == 0)
      //menu.addMenuItem(" ");    
    //menu.setCurrentItem(menu.totalItems());
    if (!name.IsEmpty())
    {
      name = name.Left(name.GetLength()-1);
    };
    menu.changeMenuItem(1, name);
  };
  return;
}


void GETCHARNAME_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.reset(NULL);
  menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
  menu.setTitle(GetCharNameTitle);
  menu.setHorzOrient();
  menu.setItemSeparation(0);
  menu.addMenuItem(" ");
  menu.setCurrentItem(menu.totalItems());
  currPic = character.smallPic;
  //currPic.LoadPicSurfaces("prt_");     
  currPic.LoadPicSurfaces("");     
  
  tempChars.Clear();
  // add saved characters, doesn't include NPCs
  tempChars.AddSavedCharacters();
  // add pre-gen NPC's
  //POSITION pos = globalData.charData.GetHeadPosition();
  //while (pos != NULL)
  int i, n;
  n = globalData.charData.GetCount();
  for (i=0; i<n; i++)
  {
    //if (globalData.charData.GetAt(pos).IsPreGen)
    if (globalData.charData.PeekCharacter(i)->IsPreGen)
      //tempChars.addPreGenCharacter(globalData.charData.GetAt(pos), globalData.charData.GetAt(pos).uniqueKey);
      tempChars.addPreGenCharacter(*globalData.charData.PeekCharacter(i));
    //globalData.charData.GetNext(pos);
  }
  Invalidate();
}

void GETCHARNAME_MENU_DATA::OnReturnToTopOfQueue(void)
{
  if (party.tradeItem!=1) // no, don't replace character
  {    
    ClearFormattedText(textData);
    currPic = character.smallPic;
    //currPic.LoadPicSurfaces("prt_");     
    currPic.LoadPicSurfaces("");     

    menu.reset(NULL);
    menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
    menu.setTitle(GetCharNameTitle);
    menu.setHorzOrient();
    menu.setItemSeparation(0);
    menu.addMenuItem(" ");
    menu.setCurrentItem(menu.totalItems());
    char temp[6];
    for (int i=0;i<name.GetLength();i++)
    {
      sprintf(temp, "%c", name[i]);
      menu.changeMenuItem(menu.totalItems(), temp);
      menu.addMenuItem(" ");
      menu.setCurrentItem(menu.totalItems());
    }
  }
  else
    PopEvent();

}

void GETCHARNAME_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();

  drawViewportFrame(VPFrameXOffset, VPFrameY+VPFrameYOffset, &CharViewFrameVPArt, &CharViewViewportFrameSrc);
  if (currPic.key > 0)
  {
    if (currPic.picType == SmallPicDib)
    {
       GraphicsMgr.BlitSprite(VPFrameXOffset+(ViewportX-VPFrameX), 
                              VPFrameY+VPFrameYOffset+(ViewportY-VPFrameY), 
                              currPic.key);
    }
  }

  if (errorTextData.numLines == 0)
    displayCreatedCharStats(character);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

////////////////////////////////////////////////////////////////GENDER_MENU_DATA
void GENDER_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  switch (menu.currentItem()) 
  {
  case 1: // select
    {
      //genderType type = (genderType)spellListText.GetSpellIndex(currSpellTextIndex());
      genderType type = spellListText.GetGender(currSpellTextIndex());
      character.SetGender(type);
      PopEvent();
    }
    break;    
  case 2: // next
    nextSpellPage(&Select_Gender_Format);
    break;
  case 3: // prev
    prevSpellPage(&Select_Gender_Format);
    break;
  case 4: // exit
    m_pOrig->m_AbortCharCreation=true;
    PopEvent(); // pop to exit event
    break;
  default:
    WriteDebugString("Unhandled menu item in GENDER_MENU_DATA");
    break;
  }
}

void GENDER_MENU_DATA::OnMouseClickLeft(int mx, int my)
{  
  if (MouseInventory(mx, my))
    simulatedKeyQueue.PushKey(VK_RETURN);
  else
    MouseMenu(mx, my);
}

//NUM_GENDER_TYPES
void GENDER_MENU_DATA::OnInitialEvent(void)
{  
  ClearFormattedText(textData);
  menu.setHorzOrient();
  menu.setMenu(SelectMenuData, &Select_Gender_Format, FALSE, NULL, NULL);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  clearSpellItemState();

  CString title("Choose a character gender:");
  CString tmp;
  //tmp.Format(" ( %s )", raceData.GetRaceName(character.race()));
  //tmp.Format(" ( %s )", raceData.PeekRace(character.RaceID())->Name());
  tmp.Format(" ( %s )", character.PeekRaceData()->Name());
  title += tmp;
  spellListText.FormatSpellListTitle(title);
  spellListText.m_flags = SHOW_DESIGN_FLAG;    
  
  {
    //GLOBAL_SPELL_ID gsID;
    //gsID.IllegalConversion(Male);
    //spellListText.Add(gsID);
    //gsID.IllegalConversion(Female);
    //spellListText.Add(gsID);
    spellListText.Add(Male);
    spellListText.Add(Female);
  };

  int scnt = spellListText.GetGenderCount();
  ItemsOnPage = min(Items_Per_Page, scnt);
  if (ItemsOnPage > 0)
  {
    if (party.activeItem >= spellListText.GetGenderCount())
      party.activeItem = (BYTE)(spellListText.GetGenderCount()-1);
    if (party.activeItem >= (startItem+ItemsOnPage))
      party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    //currShopItem = 0;
  }
  Invalidate();
}
void GENDER_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  
  menu.setAllItemsActive();
  if (spellListText.GetGenderCount() <= 0)
    menu.setItemInactive(1); // select    
  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // prev
}
void GENDER_MENU_DATA::OnDraw(void)
{
  //StandardMenuDraw();
  UpdateGenderMenuScreen(Select_Gender_Format);
}

/////////////////////////////////////////////////////////////ALIGNMENT_MENU_DATA
void ALIGNMENT_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  switch (menu.currentItem()) 
  {
  case 1: // select
    {
      //alignmentType type = (alignmentType)spellListText.GetSpellIndex(currSpellTextIndex());
      alignmentType type = spellListText.GetAlignment(currSpellTextIndex());
      character.SetAlignment(type);
      character.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
      if (character.GetMaxHitPoints() == 0)
      {
        miscError = CharacterCreationError;
        m_pOrig->m_AbortCharCreation=true;
        WriteDebugString("Failed to generateNewCharacter() in ALIGNMENT_MENU_DATA\n");
      }
      PopEvent();
    }
    break;    
  case 2: // next
    nextSpellPage(&Select_Alignment_Format);
    break;
  case 3: // prev
    prevSpellPage(&Select_Alignment_Format);
    break;
  case 4: // exit
    m_pOrig->m_AbortCharCreation=true;
    PopEvent(); // pop to exit event
    break;
  default:
    WriteDebugString("Unhandled menu item in ALIGNMENT_MENU_DATA");
    break;
  }
}
void ALIGNMENT_MENU_DATA::OnMouseClickLeft(int mx, int my)
{  
  if (MouseInventory(mx, my))
    simulatedKeyQueue.PushKey(VK_RETURN);
  else
    MouseMenu(mx, my);
}

void ALIGNMENT_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData); 
  menu.setHorzOrient();
  menu.setMenu(SelectMenuData, &Select_Alignment_Format, FALSE, NULL, NULL);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);

  clearSpellItemState();
  CString title("Choose a character alignment:");
  CString tmp;
  tmp.Format(" ( %s, %s, %s )", 
               //raceData.GetRaceName(character.race()),
               //raceData.PeekRace(character.RaceID())->Name(),
               character.PeekRaceData()->Name(),
               GetGenderName(character.GetGender()),
               //classData[character.GetClass()].m_name);
               classData.PeekClass(character.GetClass())->m_name);
  title+=tmp;

  spellListText.FormatSpellListTitle(title);
  spellListText.m_flags = SHOW_DESIGN_FLAG;    

  WORD allowedAlignments=character.getAllowedAlignments();
  for (int i=0; i<9; i++)
  {
    //GLOBAL_SPELL_ID gsID;
    if ((allowedAlignments & (1 << i))!=0)
    {
      //gsID.IllegalConversion(i);
      //spellListText.Add(gsID);  
      spellListText.Add((alignmentType)i);
    };
  }

  int scnt = spellListText.GetAlignmentCount();
  ItemsOnPage = min(Select_Alignment_Format.Lines_Per_Page, scnt);
  if (ItemsOnPage > 0)
  {
    if (party.activeItem >= spellListText.GetAlignmentCount())
      party.activeItem = (BYTE)(spellListText.GetAlignmentCount()-1);
    if (party.activeItem >= (startItem+ItemsOnPage))
      party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    //currShopItem = 0;
  }
  Invalidate();
}

void ALIGNMENT_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  
  menu.setAllItemsActive();
  if (spellListText.GetAlignmentCount() <= 0)
    menu.setItemInactive(1); // select    
  if (!allowSpellNextPage(&Select_Alignment_Format))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(&Select_Alignment_Format))
    menu.setItemInactive(3); // prev

  //WORD allowedAlignments=character.getAllowedAlignments();
  //for (int i=0; i<9; i++)
  //  if ((allowedAlignments & (1 << i))==0)
  //    menu.setItemInactive(i+1);
}

void ALIGNMENT_MENU_DATA::OnDraw(void)
{
  //StandardMenuDraw();
  UpdateAlignmentMenuScreen(Select_Alignment_Format);
}

/////////////////////////////////////////////////////////////CHOOSESTATS_MENU_DATA
void CHOOSESTATS_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  int rslt;
  if (key != KC_RETURN) 
  {
    rslt=handleChooseStatsInput(CHOOSESTATS_key,&character, key,0,&Select_Choosestats_Format);
    if (rslt&2) return;
    StandardMenuKeyboardAction(key,ascii);
    return;
  };
  handleChooseStatsInput(CHOOSESTATS_exit,NULL,0,0,&Select_Choosestats_Format); // Terminate modifications
  if (menu.currentItem() == 2) // don't re-roll
  {
      PopEvent();
      return;
  };
  CHARACTER tempChar = character;
  if (m_CreateNewChar)
  {
    character.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
    if (character.GetMaxHitPoints() == 0)
    {
      WriteDebugString("Failed to generateNewCharacter() in CHOOSESTATS_MENU_DATA\n");
      character = tempChar;
    }
  }
  else
  {
    character.generateNewCharacter(GetDesignCharStartExp(),GetDesignCharStartExpType());
    if (character.GetMaxHitPoints() == 0)
    {
      WriteDebugString("Failed to modifyNewCharacter() in CHOOSESTATS_MENU_DATA\n");
      character = tempChar;
    }
  }

  if (handleChooseStatsInput(CHOOSESTATS_initial, &character,0,0,&Select_Choosestats_Format))
  {
  };
}

void CHOOSESTATS_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  int rslt;
  rslt=handleChooseStatsInput(CHOOSESTATS_mouseClick,
                              &character,
                              mx, 
                              my,
                              &Select_Choosestats_Format
                             );
  if (rslt&1) ;
  else GameEvent::OnMouseClickLeft(mx, my);
}

extern bool AllowModifyStats;

void CHOOSESTATS_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  currPic = character.smallPic;
  //currPic.LoadPicSurfaces("prt_");
  currPic.LoadPicSurfaces("");
  menu.setMenu(ChooseStatsMenuData, &Select_Choosestats_Format, FALSE, NULL, NULL);
  menu.setHorzOrient();
  if (AllowModifyStats)
  {
    menu.setTitle("<TAB><Up/Down> Modify Character Stats");
  }
  else
  {
    menu.setTitle("Character Stats");
  };
  handleChooseStatsInput(CHOOSESTATS_initial, &character,0,0,&Select_Choosestats_Format);
  Invalidate();
}

void CHOOSESTATS_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  drawViewportFrame(VPFrameXOffset, VPFrameY+VPFrameYOffset, &CharViewFrameVPArt, &CharViewViewportFrameSrc);
  if (currPic.key > 0)
  {
    if (currPic.picType == SmallPicDib)
    {
       GraphicsMgr.BlitSprite(VPFrameXOffset+(ViewportX-VPFrameX), VPFrameY+VPFrameYOffset+(ViewportY-VPFrameY), 
                       currPic.key);
    }
  }

  handleChooseStatsInput(CHOOSESTATS_refresh, &character,0,0,&Select_Choosestats_Format);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void CHOOSESTATS_MENU_DATA::OnLeaveQueue(void)
{ // Needed if ESCAPE pressed and cleanup not performed. 
  handleChooseStatsInput(CHOOSESTATS_exit,NULL,0,0,&Select_Choosestats_Format); // Terminate modifications
}


////////////////////////////////////////////////////////////////CLASS_MENU_DATA
void CLASS_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  switch (menu.currentItem()) 
  {
  case 1: // select
    {
      //classType ct = (classType)spellListText.GetSpellIndex(currSpellTextIndex());
      CLASS_ID classID = spellListText.GetClass(currSpellTextIndex());
      //character.SetClass(ct);
      character.SetClass(classID);
//      xx 
      PopEvent();
    }
    break;    
  case 2: // next
    nextSpellPage(&Select_Class_Format);
    break;
  case 3: // prev
    prevSpellPage(&Select_Class_Format);
    break;
  case 4: // exit
    m_pOrig->m_AbortCharCreation=true;
    PopEvent(); // pop to exit event
    break;
  default:
    WriteDebugString("Unhandled menu item in CLASS_MENU_DATA");
    break;
  }   
}
void CLASS_MENU_DATA::OnMouseClickLeft(int mx, int my)
{  
  if (MouseInventory(mx, my))
  {
    simulatedKeyQueue.PushKey(VK_RETURN);
    menu.setCurrentItem(1);
  }
  else
    MouseMenu(mx, my);
}

void CLASS_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setHorzOrient();
  menu.setMenu(SelectMenuData, &Select_Class_Format, FALSE, NULL, NULL);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);

  CString title("Choose a character class:");
  CString tmp;
  tmp.Format(" ( %s, %s )", 
               //raceData.GetRaceName(character.race()),
               //raceData.PeekRace(character.RaceID())->Name(),
               character.PeekRaceData()->Name(),
               GetGenderName(character.GetGender()));
  title+=tmp;
  //menu.setTitle(title);

  clearSpellItemState();
  spellListText.FormatSpellListTitle(title);

  spellListText.m_flags = SHOW_DESIGN_FLAG | CLASS_FORMAT_FLAG;    
   
  // Step through the loop to see possible classes.
  ALLOWED_CLASSES allowedClasses;
  allowedClasses.Initialize();  //Start with ALL classes
  //allowedClasses.RestrictByRace(character.race());
  allowedClasses.Restrictions(character.RaceID(), (character.GetGender()==Male)?"M":"F");
  for (int i=0; i<allowedClasses.GetCount(); i++)
  {
    //GLOBAL_SPELL_ID gsID;
    //classType ct = allowedClasses.GetAt(i);
    //gsID.IllegalConversion((DWORD)ct);
    //spellListText.Add(gsID);
    //spellListText.Add((classType)i);
    spellListText.Add(*allowedClasses.PeekClassID(i));
  };

  int scnt = spellListText.GetClassCount();
//  ItemsOnPage = min(Items_Per_Page, scnt);
  ItemsOnPage = min(Select_Class_Format.Lines_Per_Page, scnt);
  if (ItemsOnPage > 0)
  {
    if (party.activeItem >= spellListText.GetClassCount())
      party.activeItem = (BYTE)(spellListText.GetClassCount()-1);
    if (party.activeItem >= (startItem+ItemsOnPage))
      party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    //currShopItem = 0;
  }
  Invalidate();
}

void CLASS_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  
  menu.setAllItemsActive();
  if (spellListText.GetClassCount() <= 0)
    menu.setItemInactive(1); // select    
  if (!allowSpellNextPage(&Select_Class_Format))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(&Select_Class_Format))
    menu.setItemInactive(3); // prev
}

void CLASS_MENU_DATA::OnDraw(void)
{
  //StandardMenuDraw();
  UpdateClassMenuScreen(Select_Class_Format);
}

//////////////////////////////////////////////////////////ADVENTURE_MENU_DATA
void ADVENTURE_MENU_DATA::OnInitialEvent(void)
{
  //ClearFormattedText(textData);
  menu.setMenu(AdventureMenuData, NULL, FALSE, NULL, NULL);
  /* 1 { "AREA"     , 0 },
   * 2 { "CAST"     , 0 },
   * 3 { "VIEW"     , 0 },
   * 4 { "ENCAMP"   , 0 },
   * 5 { "SEARCH"   , 0 },
   * 6 { "LOOK"     , 0 },
   * 7 { "INVENTORY", 0 }
   */
  menu.setHorzOrient();
  menu.setUseActive(FALSE);
  SetMyState(TASK_MoveAwaitKey);
  globalData.temp_asl.Clear(); // remove temp vars
  m_origPartyX=party.Posx;
  m_origPartyY=party.Posy;
  Invalidate();
}


bool ADVENTURE_MENU_DATA::OnCycle(void)
{
  // This only serves to make the OnCycle call more visible.
  return GameEvent::OnCycle();
}


void ADVENTURE_MENU_DATA::OnDraw(void)
{
  if (!cmdLine.m_forceAV)
  {
    // check global view settings for this level
    switch (globalData.levelInfo.stats[globalData.currLevel].AVStyle)
    {
    case OnlyAreaView: globalData.useAreaView = TRUE;  break;   
    case Only3DView:   globalData.useAreaView = FALSE; break;
    default:           // do nothing 
      break;
    }
  }
  DETAILTRACE(0x4c3320,0);
  UpdateAdventureScreen();
}

void ADVENTURE_MENU_DATA::OnMouseMove(int mx, int my)
{ 
  CursorArt.sx = mx;
  CursorArt.sy = my;

  if (globalData.levelInfo.stats[globalData.currLevel].overland)
  {
    // determine relative dir between mouse click and party position
    int MapClick=Mouse2OverlandMap(mx,my);
    // NW  N  NE
    // W      E
    // SW  S  SE
    switch (MapClick)
    {
    case 0: CursorArt.SetFrame(NW_CURSOR); return;
    case 1: CursorArt.SetFrame(N_CURSOR); return;
    case 2: CursorArt.SetFrame(NE_CURSOR); return;
    case 3: CursorArt.SetFrame(W_CURSOR); return;
    case 4: CursorArt.SetFrame(CIRCLE_CURSOR); return;
    case 5: CursorArt.SetFrame(E_CURSOR); return;
    case 6: CursorArt.SetFrame(SW_CURSOR); return;
    case 7: CursorArt.SetFrame(S_CURSOR); return;
    case 8: CursorArt.SetFrame(SE_CURSOR); return;
    }
  }
  else
  {
    if (!globalData.useAreaView)
    {
      int ViewPortClick=Mouse2Viewport(mx, my);
      // TL  F  TR
      // TL  F  TR
      //  L  B  R
      switch (ViewPortClick)
      {
      case 0:
      case 6:
      case 3: CursorArt.SetFrame(TURNLEFT_CURSOR); return; // turn left      
      case 1:
      case 7:
      case 4: CursorArt.SetFrame(N_CURSOR); return; // move forward  
      case 2: 
      case 8:
      case 5: CursorArt.SetFrame(TURNRIGHT_CURSOR); return; // turn right
      //case 6: CursorArt.SetFrame(W_CURSOR); return; // slide left
      //case 7: CursorArt.SetFrame(S_CURSOR); return; // move back
      //case 8: CursorArt.SetFrame(E_CURSOR); return; // slide right
      }
    }
  }
  // default to standard cursor
  CursorArt.SetFirstFrame();
}

void ADVENTURE_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  if (globalData.levelInfo.stats[globalData.currLevel].overland)
  {
    // determine relative dir between mouse click and party position
    int MapClick=Mouse2OverlandMap(mx,my);
    // NW  N  NE
    // W      E
    // SW  S  SE
    //
    // relative dir works according to:
    //
    // 4  0  5
    // 3     1
    // 6  2  7
    //
    switch (MapClick)
    {
    case 0: simulatedKeyQueue.PushKey(VK_NUMPAD7); return;
    case 1: simulatedKeyQueue.PushKey(VK_NUMPAD8); return;
    case 2: simulatedKeyQueue.PushKey(VK_NUMPAD9); return;
    case 3: simulatedKeyQueue.PushKey(VK_NUMPAD4); return;
    //case 4: simulatedKeyQueue.PushKey(VK_NUMPAD5); return;
    case 5: simulatedKeyQueue.PushKey(VK_NUMPAD6); return;
    case 6: simulatedKeyQueue.PushKey(VK_NUMPAD1); return; 
    case 7: simulatedKeyQueue.PushKey(VK_NUMPAD2); return;
    case 8: simulatedKeyQueue.PushKey(VK_NUMPAD3); return;
    };
  }
  else
  {
    if (!globalData.useAreaView)
    {
      int ViewPortClick=Mouse2Viewport(mx, my);
      // TL  F  TR
      // TL  F  TR
      //  L  B  R
      switch (ViewPortClick)
      {
      case 0:
      case 3: simulatedKeyQueue.PushKey(VK_LEFT); return; // turn left      
      case 1: 
      case 4: simulatedKeyQueue.PushKey(VK_UP); return; // move forward  
      case 2: 
      case 5: simulatedKeyQueue.PushKey(VK_RIGHT); return; // turn right

      case 6: simulatedKeyQueue.PushKey(VK_NUMPAD1); return; // slide left
      case 7: simulatedKeyQueue.PushKey(VK_NUMPAD5); return; // move back
      case 8: simulatedKeyQueue.PushKey(VK_NUMPAD3); return; // slide right
      }; // Otherwise it ain't in the viewport!
    }
  }

  MouseMenu(mx, my);
  MouseParty(mx, my);
}

// relative dir works according to:
//
// 4  0  5
// 3     1
// 6  2  7
//
void ADVENTURE_MENU_DATA::ProcessOverlandMoveRequest(key_code key, char ascii)
{
  int newX, newY;
  int relDir = 0;
  TASKSTATE newstate = TASK_MovePreMove0;

  switch (key)
  {
  case KC_NW:    newstate=TASK_MovePreMove4; relDir=4; break;
  case KC_UP:    newstate=TASK_MovePreMove0; relDir=0; break;
  case KC_NE:    newstate=TASK_MovePreMove5; relDir=5; break;
  case KC_LEFT:  newstate=TASK_MovePreMove3; relDir=3; break;
  //case KC_CENTER:newstate=TASK_MovePreMove0; relDir=0; break;
  case KC_RIGHT: newstate=TASK_MovePreMove1; relDir=1; break;
  case KC_SW:    newstate=TASK_MovePreMove6; relDir=6; break;
  case KC_DOWN:  newstate=TASK_MovePreMove2; relDir=2; break;
  case KC_SE:    newstate=TASK_MovePreMove7; relDir=7; break;
  default: return;
  }

  // just testing to see if party *can* move forward.
  if (party.newForwardPosition(relDir, newX, newY) == PartyBlock_none)
  {
    // We have the x and y coordiates of a square that the
    // party is about to move into.  It is possible that the square
    // contains an event that should be activated before the party
    // actually occupies the square.  If so, we will activate it now.
    SetMyState(newstate);  // Doing Pre-movement actions
    if (levelData.haveEvent(newX, newY))
    {
      GameEvent *ev=levelData.eventData.GetFirstEvent(newX, newY);
      if (ev!=NULL) 
      {
        // does this event want to trigger before the move?
        if (ev->OnTestPreMove())
        {
          PushEvent(ev, LevelEvent);
          return;
        };
      };
    };

    // No event triggered before the move.
    // The actual movement will happen after this
    // empty event pops off the queue in
    // OnReturnToTopOfQueue()
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    return;
  }
  else
  {
    // overland maps display a zone message from the 
    // zone in front when passage is blocked
    if (levelData.GetCurrZone(party.Posx,party.Posy) != levelData.GetCurrZone(newX,newY))
    {
      PushEvent(new BEGIN_ZONE_EVENT(newX,newY), DeleteEvent);
    }
    // impassable obstical in our way
    PlayPartyBump();
  }
}

const char FacingText[5] = "NESW";


void ADVENTURE_MENU_DATA::ProcessDungeonMoveRequest(key_code key, char ascii)
{
  int newX, newY;
  int relDir = 0;
  PartyBlockageType blockType;
  TASKSTATE newstate = TASK_MovePreMove0;

  switch(key)
  {
  case KC_UP:
  //case KC_SE:
  //case KC_CENTER:
  //case KC_SW:
  case KC_SPACE:
    {
      switch(key)
      { // Put direction in state to avoid having to save more stuff
      case KC_UP    : newstate=TASK_MovePreMove0;  relDir=0;  break;
      //case KC_SE    : newstate=TASK_MovePreMove1;  relDir=1;  break;
      //case KC_CENTER: newstate=TASK_MovePreMove2;  relDir=2;  break;
      //case KC_SW    : newstate=TASK_MovePreMove3;  relDir=3;  break;
      case KC_SPACE : newstate=TASK_MovePreNoMove; relDir=-1; break;
      };

      m_relDir=relDir;
      // just testing to see if party *can* move forward.
      if (relDir == -1) 
      {
        blockType = PartyBlock_none;
        newX = party.Posx;
        newY = party.Posy;
      }
      else
      {
        blockType = party.newForwardPosition(relDir, newX, newY);
      };
      if (blockType == PartyBlock_none)
      {
        // We have the x and y coordinates of a square that the
        // party is about to move into.  It is possible that the square
        // contains an event that should be activated before the party
        // actually occupies the square.  If so, we will activate it now.
        SetMyState(newstate);  // Doing Pre-movement actions

        if ((relDir!=-1) && levelData.haveEvent(newX, newY))
        {
          GameEvent *ev=levelData.eventData.GetFirstEvent(newX, newY);
          if (ev!=NULL) 
          {
            // If the first event in the chain calls for 'Backup One Step'
            // then we do not want to actually enter the new position.
            // We will activate the event chain but no wall sounds will
            // be produced and we will not see any movement occur.
            if (    ev->ForcePartyBackup() 
                 || ((ev->event == TransferModule) && ev->OnTestPreMove())
               )

            // does this event want to trigger before the move?

            // Delay this decision until we see if the event even triggers.
            // We want the 'PreMove' decision to depend not on the first
            // event in the chain (whether or not it triggers) but rather
            // on the first event that DOES trigger and which has some sort
            // of player interaction.  A CHAIN event, for example, should
            // not affect the party movement.
            /*
            if (ev->OnTestPreMove())
            */
            {
              PushEvent(ev, LevelEvent);
              return;
            };
          };
        };

        // No event triggered before the move.
        // The actual movement will happen after this
        // empty event pops off the queue in
        // OnReturnToTopOfQueue()
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
        return;
      }
      else // impassable obstacle in our path
      {
        // Check blockage for locked, or spell-locked door
        // Secret doors must be found before any attempt at
        // unlocking can be made.

        // check for locked door
        BOOL isLocked       = levelData.haveLockedDoor (party.Posx, party.Posy, party.RelativeToActualFacing(relDir));
        BOOL isSpelled      = levelData.haveSpelledDoor(party.Posx, party.Posy, party.RelativeToActualFacing(relDir));
        BOOL isSpecialKeyed = (blockType >= PartyBlock_specialKey0) && (blockType <= PartyBlock_specialKey7);

        bool caster = false;
        bool lockPicker = false;
        TASKSTATE task = TASK_empty;


        if (isLocked || isSpelled || isSpecialKeyed)
        {
          // See if we have a Lock Picker in the crowd.
          {
            int lockPickSkill;
            party.GetBestLockpicker(&lockPickSkill);
            if (lockPickSkill > 0)
            {
              lockPicker = true;
            };
          };

          // Does anyone know the proper spell?
          {
            int i, n;
            SPELL_ID spellID_or_Hook;
            spellID_or_Hook = levelData.GetUnlockSpell(party.Posx, party.Posy, party.facing);
            if (!spellID_or_Hook.IsEmpty())
            {
              n = party.numCharacters;
              for (i=0; i<n; i++)
              {
                CHARACTER *pCharacter;
                pCharacter = &party.characters[i];
                if (pCharacter->HaveSpell(spellID_or_Hook, true))
                {
                  caster = true;
                  m_openSpellID = spellID_or_Hook;
                  m_pOpenSpellChar = pCharacter;
                  break;
                };
              };
            
            
              if (!caster)
              {
                //Perhaps it is a Special Ability name.
                // We can search the party's memorized spells to see
                // if any one of them has a Special Ability with 
                // this name.
                int k, num;
                HOOK_PARAMETERS hookParameters;
                SCRIPT_CONTEXT scriptContext;
                hookParameters[5].Format("%d/%d/%d/%c",
                                          GetCurrentLevel()+1, 
                                          party.Posx, 
                                          party.Posy, 
                                          FacingText[party.facing]);
                num = party.numCharacters;
                for (k=0; (k<num)&&(!caster); k++)
                {
                  CHARACTER *pCharacter;
                  int j, m;
                  pCharacter = &party.characters[k];
                  m = pCharacter->GetSpellCount();
                  for (j=0; j<m; j++)
                  {
                    CHARACTER_SPELL *pCharSpell;
                    pCharSpell = pCharacter->GetCharacterSpell(j);
                    if (pCharSpell->IsMemorized())
                    {
                      const SPELL_DATA *pSpell;
                      CString result;
                      pSpell = spellData.PeekSpell(pCharSpell->spellID);
                      if (pSpell == NULL) continue;
                      scriptContext.SetSpellContext(pSpell);
                      result = pSpell->RunSpellScripts(
                                          spellID_or_Hook,
                                          ScriptCallback_RunAllScripts, 
                                          NULL,
                                          "Party moved into obstacle.  Will this spell open it?");
                      if (!result.IsEmpty() && (result[0] == 'Y'))
                      {
                        m_pOpenSpellChar = pCharacter;
                        m_openSpellID = pSpell->SpellID();
                        caster = true;
                        break;
                      };
                    };
                  };
                }
              };       
            };
          };


          
          //if (party.PartyHasLockPicker())
          if (lockPicker)
          {
            if (caster)
            {
              task = TASK_BashPickUnspellDoor;
            }
            else
            {
              task = TASK_BashPickDoor;
            };
          }
          else
          {
            if (caster)
            {
              task = TASK_BashUnspellDoor;
            };
          };;
          SetMyState(task);
          switch(task)
          {
          case TASK_BashPickDoor:
            PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED:",
                                             "BASH",
                                             "PICK",
                                             "EXIT",
                                             "",
                                             CONTROL_Adventure), 
                     DeleteEvent);
            break;
          case TASK_BashPickUnspellDoor:
            PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED:",
                                               "BASH",
                                               "PICK",
                                               "USE MAGIC",
                                               "EXIT",
                                               CONTROL_Adventure), 
                      DeleteEvent);
            break;
          case TASK_BashUnspellDoor:
            PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED:",
                                              "USE MAGIC",
                                              "EXIT",
                                              "",
                                              "",
                                              CONTROL_Adventure), 
                      DeleteEvent);
            break;
          default:
            {
              SetMyState(TASK_BashDoor);
              PlayPartyBump();
              PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED",
                                                "BASH",
                                                "EXIT",
                                                "",
                                                "",
                                                CONTROL_Adventure),
                        DeleteEvent);
              break;
            };
          };
        }
/*
//          CHARACTER *dude;
          BOOL hasMagicUser;

          if (isSpelled)
          {
            //GLOBAL_SPELL_ID unlockspellkey = levelData.GetUnlockSpell(party.Posx, party.Posy, party.RelativeToActualFacing(m_relDir));
            hasMagicUser = party.PartyHasClass(MagicUser);
            if (hasMagicUser)
            {
              SetMyState(TASK_UnspellDoor);
              PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED:",
                                                "USE MAGIC",
                                                "EXIT",
                                                "",
                                                "",
                                                CONTROL_Adventure), 
                        DeleteEvent);
              break;
            };
          }
          
          if (party.PartyHasClass(Thief))
          {
            if (party.PartyHasClass(MagicUser))
            {
              SetMyState(TASK_BashPickUnspellDoor);
              PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED:",
                                                 "BASH",
                                                 "PICK",
                                                 "USE MAGIC",
                                                 "EXIT",
                                                 CONTROL_Adventure), 
                        DeleteEvent);
            }
            else
            {
              SetMyState(TASK_BashPickDoor);
              PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED:",
                                                 "BASH",
                                                 "PICK",
                                                 "EXIT",
                                                 "",
                                                 CONTROL_Adventure), 
                        DeleteEvent);
            };
            break;
          }
          if (party.PartyHasClass(MagicUser))
          {
            SetMyState(TASK_BashUnspellDoor);
            PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED:",
                                               "BASH",
                                               "USE MAGIC",
                                               "EXIT",
                                               "",
                                               CONTROL_Adventure),
                      DeleteEvent);
            break;
          };
   
*/
        else if (isSpecialKeyed)
        {
            PlayPartyBump();
            PushEvent(new ASK_CUSTOM_MENU_DATA("THIS DOOR IS LOCKED WITH A SPECIAL KEY",
                                               "EXIT",
                                               "",
                                               "",
                                               "",
                                               CONTROL_Adventure),
                      DeleteEvent);
            break; // Stay right here
        }
        else
          PlayPartyBump();
      }
    }
    break; // Stay right here 

  case KC_RIGHT:
    {
      party.turnPartyRight();
      // check for events that trigger based on facing only
      PushEvent(new BEGIN_XY_FACING_EVENT(party.Posx,party.Posy,GetCurrentLevel()), DeleteEvent);
    }
    break;
  case KC_LEFT:
    {
      party.turnPartyLeft();
      // check for events that trigger based on facing only
      PushEvent(new BEGIN_XY_FACING_EVENT(party.Posx,party.Posy,GetCurrentLevel()), DeleteEvent);
    }
    break;
  case KC_DOWN:
    {
      party.turnParty180();
      // check for events that trigger based on facing only
      PushEvent(new BEGIN_XY_FACING_EVENT(party.Posx,party.Posy,GetCurrentLevel()), DeleteEvent);
    }
    break;
  default:
    break;
  }
}

void ADVENTURE_MENU_DATA::ProcessMenuRequest(key_code key, char ascii)
{
  switch (menu.currentItem()) 
  {
  case 1: // Area
    globalData.useAreaView = !globalData.useAreaView;
    break;
  case 2: // Cast
    PushEvent(new CAST_MENU_DATA(NULL, CAST_ENV_ADVENTURE), DeleteEvent);
    break;
  case 3: // View
    PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),NULL), DeleteEvent);
    break;
  case 4: // Encamp
    globalData.useAreaView = FALSE;
    PushEvent(new ENCAMP_MENU_DATA(NULL), DeleteEvent);
    break;
  case 5: // Search
    party.searching = !party.searching;      
    break;
  case 6: // Look 
    party.looking=TRUE; // for one game loop, will trigger events based on search status
    party.incrementClock(GetTimeInc(TRUE));

    {
      GameEvent *ev=levelData.eventData.GetFirstEvent(party.Posx, party.Posy);
      if (   (ev == NULL)
          || (ev->GetEventText().IsEmpty())
         )
      {

        if (party.numCharacters == 1)
          FormatDisplayText(textData, "You look around...", TRUE, FALSE);
        else if (party.numCharacters == 2)
          FormatDisplayText(textData, "Both of you look around...", TRUE, FALSE);
        else
          FormatDisplayText(textData, "The party looks around...", TRUE, FALSE);

        // display the above text before triggering any events
        // in this square
        DisplayTextAndFlip(0);
      };
    };

    // ** no need to check for zone/step event since we haven't moved **

    // this event will check for, and load, any events that
    // should trigger based on current x,y position in map
    BEGIN_XY_EVENT *xyEvent;
    xyEvent=new BEGIN_XY_EVENT(party.Posx,
                               party.Posy,
                               GetCurrentLevel());
    PushEvent(xyEvent, DeleteEvent);
    break;
  case 7: // Inventory
    {
      PushEvent (new ITEMS_MENU_DATA(GetEventType(), NULL, NULL, NULL, SPECIAL_ONLY), DeleteEvent);
    };
    break;
  }
  menu.setCurrentItem(1); // default back to first menu option
}

void ADVENTURE_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  ClearFormattedText(textData);
  if (GetMyState()!=TASK_MoveAwaitKey) return; // discard keyin

  if (TABParty(key)) return;


  switch(key)
  {

  case KC_NW:
  case KC_UP:
  case KC_NE:
  case KC_LEFT:  
  case KC_CENTER:
  case KC_RIGHT:
  case KC_SW:
  case KC_DOWN:
  case KC_SE:  
    if (globalData.levelInfo.stats[globalData.currLevel].overland)
      ProcessOverlandMoveRequest(key,ascii);
    else
      // Added 20161022 PRS to fix missing backup on QUESTION_YES_NO
      m_BackupStatus = Backup_None;
      ProcessDungeonMoveRequest(key,ascii);
    break;
  case KC_SPACE:
    if (!globalData.levelInfo.stats[globalData.currLevel].overland)
    {
      ProcessDungeonMoveRequest(key,ascii);
      break;
    };
    // *** NOTE FAllthrough to default ********
  default:
    StandardKeyboardShortcutAction(key,ascii);
    break;
  case KC_RETURN: // only get here by mouse or menu shortcut key
    ProcessMenuRequest(key, ascii);
    break;
  }
}

void ADVENTURE_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;

  if (!cmdLine.m_forceAV)
  {
    if (globalData.levelInfo.stats[globalData.currLevel].AVStyle != AnyView)
      menu.setItemInactive(1); // area view  
    else if (levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].allowMap == NoMapping)
      menu.setItemInactive(1); // area view
  }

  if (!GetActiveChar(this).CanCastSpells())
    menu.setItemInactive(2); // cast 
}

bool Probability(int n, int k)
{
  return RollDice(k, 1, 0) <= n;
}

void ADVENTURE_MENU_DATA::OnReturnToTopOfQueue()
{
  int reldir;
  TASKSTATE myState;
  enum
  {
    TRY_Nothing = 0,
    TRY_Bash,
    TRY_Pick,
    TRY_Spell,
    TRY_Exit
  } whatToTry;
  myState = GetMyState();
  if (myState == TASK_CharacterCastSpell)
  {
    /* Really */ NotImplemented(0xc9080, false);
  };
  if ((myState == TASK_SetPartyXY)  && (setPartyXY_x >= 0))
  {
    party.Posx = setPartyXY_x;
    party.Posy = setPartyXY_y;
    OnInitialEvent();
    BEGIN_XY_EVENT *xyEvent;
    xyEvent=new BEGIN_XY_EVENT(party.Posx,
                               party.Posy,
                               GetCurrentLevel());
    xyEvent->m_forceEventProcessing = true;
    PushEvent(xyEvent, DeleteEvent);
    return;
  };
  if (party.looking)
    CheckSecretDoors();
  party.looking = FALSE;  // added 20110127 PRS  - don't know how long look should last

  whatToTry = TRY_Nothing;
  switch (GetMyState())
  {
  case TASK_BashDoor:
    if (party.tradeItem == 1) whatToTry = TRY_Bash;
    break;
  case TASK_UnspellDoor: 
    if (party.tradeItem == 1) whatToTry = TRY_Spell;
    break;    
  case TASK_BashPickDoor: 
    if (party.tradeItem == 1) whatToTry = TRY_Bash;
    if (party.tradeItem == 2) whatToTry = TRY_Pick;
    break;
  case TASK_BashPickUnspellDoor:
    if (party.tradeItem == 1) whatToTry = TRY_Bash;
    if (party.tradeItem == 2) whatToTry = TRY_Pick;
    if (party.tradeItem == 3) whatToTry = TRY_Spell;
    break;
  case TASK_BashUnspellDoor:
    if (party.tradeItem == 1) whatToTry = TRY_Bash;
    if (party.tradeItem == 1) whatToTry = TRY_Spell;
    break;
  };
  if (whatToTry != TRY_Nothing)
  {
    BOOL openSuccess;
    openSuccess = FALSE;
    CString location;
    CString result;
    location.Format("%d/%d/%d/%d/",
                       GetCurrentLevel(), 
                       party.Posx, 
                       party.Posy, 
                       party.RelativeToActualFacing(m_relDir));

    switch (whatToTry)
    {
      case TRY_Bash:
        {
          CHARACTER *pChar;
          result = "$B";
          int strength, strMod;
          pChar = &party.GetStrongestCharacter();
          strength = pChar->GetAdjStr();
          strMod = pChar->GetAdjStrMod();
          pChar->blockageData.ClearLocked(GetCurrentLevel(), party.Posx, party.Posy, party.RelativeToActualFacing(m_relDir));
                
               if (strength < 3) openSuccess = false;
          else if (strength < 8) openSuccess = Probability(1,6);
          else if (strength < 16) openSuccess = Probability(2,6);
          else if (strength < 18) openSuccess = Probability(3,6);
          else if (strength == 18)
          {
                 if (strMod < 75) openSuccess = Probability(3,6);
            else if (strMod < 100) openSuccess = Probability(4,6);
            else openSuccess = Probability(5,6);
          }
          else if (strength < 21) openSuccess = Probability(7,8);
          else if (strength < 22) openSuccess = Probability(9,10);
          else if (strength < 24) openSuccess = Probability(11,12);
          else if (strength < 25) openSuccess = Probability(19,20);
          else openSuccess = Probability(23,24);
        };
        break;
      case TRY_Pick:
        {
          CHARACTER *pChar;
          result = "$P";
          int lockPickSkill;
          int chance;
          //int need;
          pChar = &party.GetBestLockpicker(&lockPickSkill);
          chance = RollDice(100,1)-1;
          if (chance < lockPickSkill)
          {
            openSuccess = TRUE;
          };
          //need = pChar->GetAdjOpenLocks();
          //pChar->blockageData.ClearLocked(GetCurrentLevel(), party.Posx, party.Posy, party.RelativeToActualFacing(m_relDir));
          //openSuccess = need >= 30;
        };
        break;
      case TRY_Spell:
        {
          //GLOBAL_SPELL_ID openSpell;
          result = "$S";
          //openSpell = levelData.GetUnlockSpell(party.Posx, 
          //openSpellID = levelData.GetUnlockSpell(party.Posx, 
          //                                       party.Posy, 
          //                                       party.RelativeToActualFacing(m_relDir));
          //for (pChar=NULL,i=0;i<party.numCharacters;i++)
          //{
          //  //if (party.characters[i].HaveSpell(openSpell,TRUE))
          //  if (party.characters[i].HaveSpell(openSpellID,TRUE))
          //  {
          //    pChar = &party.characters[i];
          //    break;
          //  };
          //};
          //if ((pChar!=NULL) && pChar->GetSpell(pChar->GetSpellBookKey(openSpell, FALSE), cspell))
          //if ((pChar!=NULL) && pChar->FetchCharacterSpell(openSpellID, &charSp))
          //{
          //  //SPELL_DATA *pSdata = spellData.GetSpellData(cspell.spell);
          //  SPELL_DATA *pSdata = spellData.GetSpell(charSp.spellID);
          //  if (pSdata!=NULL)
          //  {
          //    //pChar->DecMemorized(openSpell, 1);
              m_pOpenSpellChar->DecMemorized(m_openSpellID, 1);
              openSuccess = TRUE;
          //  };
          //};
        };
        break;
      case TRY_Exit:
        break;
    };
    if (openSuccess)
    {
      // success, bashing clears the wizard lock if present also
      result += 'S';
      party.blockageData.ClearLocked(GetCurrentLevel(), party.Posx, party.Posy, party.RelativeToActualFacing(m_relDir));          
      party.blockageData.ClearSpelled(GetCurrentLevel(), party.Posx, party.Posy, party.RelativeToActualFacing(m_relDir));          
      FormatPausedText(pausedTextData, "THE DOOR IS OPENED!");
      DisplayPausedText(pausedTextData, whiteColor, 0);
    }
    else
    {
      // fail
      result += 'F';
      FormatPausedText(pausedTextData, "THE DOOR IS STILL LOCKED");
      PlayPartyBump();
      DisplayPausedText(pausedTextData, whiteColor, 0);
    };
    ;
    CString aslVal = globalData.global_asl.Lookup(location);
    aslVal = result + aslVal;
    globalData.global_asl.Insert(location, aslVal, 0);
    OnInitialEvent(); return;
  };

  myState = GetMyState();
  reldir = -1;
  switch (myState) 
  {
    case TASK_MovePreMove0: 
      reldir = 0; 
#ifdef EARLY_WALL_SOUNDS
      {
        int slot=-1;
        switch (party.facing) 
        //switch (party.RelativeToActualFacing(party.PrevRelFacing)) 
        {      
        case FACE_NORTH:
          slot = levelData.area[party.Posy][party.Posx].wall[0];
          break;      
        case FACE_EAST:
          slot = levelData.area[party.Posy][party.Posx].wall[2];
          break;      
        case FACE_SOUTH:
          slot = levelData.area[party.Posy][party.Posx].wall[1];
          break;      
        case FACE_WEST:
          slot = levelData.area[party.Posy][party.Posx].wall[3];
          break;
        };
        if ((slot > 0) && (slot < MAX_WALLSETS))
          WallSets[slot].PlaySlotSound();
      };
#else
#endif

      myState = TASK_MovePostMove0; 
      break;
    case TASK_MovePreMove1: reldir = 1; myState = TASK_MovePostMove0; break;
    case TASK_MovePreMove2: reldir = 2; myState = TASK_MovePostMove0; break;
    case TASK_MovePreMove3: reldir = 3; myState = TASK_MovePostMove0; break;
    case TASK_MovePreMove4: reldir = 4; myState = TASK_MovePostMove0; break;
    case TASK_MovePreMove5: reldir = 5; myState = TASK_MovePostMove0; break;
    case TASK_MovePreMove6: reldir = 6; myState = TASK_MovePostMove0; break;
    case TASK_MovePreMove7: reldir = 7; myState = TASK_MovePostMove0; break;
  };
  if (reldir >= 0)
  {
    // We will take the step now.
    SetMyState(myState);
    BOOL result = party.movePartyForward(reldir);
    ASSERT(result);

    // to filter out repeated requests by events
    // to backup the party one step on event exit.
    m_origPartyX=party.Posx;
    m_origPartyY=party.Posy;
    //m_BackupNeeded=FALSE;
    QueueMapEvents(true, true);
  };


  switch (myState)
  {
  case TASK_MovePreMove0: reldir = 0; break;
  case TASK_MovePreMove1: reldir = 1; break;
  case TASK_MovePreMove2: reldir = 2; break;
  case TASK_MovePreMove3: reldir = 3; break;
  case TASK_MovePreMove4: reldir = 4; break;
  case TASK_MovePreMove5: reldir = 5; break;
  case TASK_MovePreMove6: reldir = 6; break;
  case TASK_MovePreMove7: reldir = 7; break;
  case TASK_MovePreNoMove:reldir = -1;break;

  case TASK_MovePostMove0:
  case TASK_MovePostMove1:
  case TASK_MovePostMove2:
  case TASK_MovePostMove3:
  case TASK_MovePostMove4:
  case TASK_MovePostMove5:
  case TASK_MovePostMove6:
  case TASK_MovePostMove7:
    {
      SetMyState(TASK_MoveAwaitKey);

      // to filter out repeated requests by events
      // to backup the party one step on event exit.
      //m_origPartyX=party.Posx;
      //m_origPartyY=party.Posy;
      //m_BackupNeeded=FALSE;

      QueueMapEvents(false, false); // false because we already did the events here.
      PushEvent(new PLAYSTEPSOUND_EVENT, DeleteEvent);
  
      UpdatePartyMovementData();
      //ClearFormattedText(textData);


      // Here is what it used to say:  PRS Aug 2016
      /*
      if (m_BackupNeeded)
      {
        party.movePartyBackward();
        m_BackupNeeded=FALSE;
      };
      */
      // And here is how I changed it.
      if (m_BackupStatus == Backup_PostEvent)
      {
        party.movePartyBackward();
        m_BackupStatus=Backup_None;
      };

    };
    return;
  case TASK_MoveExecuteEvent:
    OnInitialEvent();
    BEGIN_XY_EVENT *xyEvent;
    xyEvent=new BEGIN_XY_EVENT(party.Posx,
                               party.Posy,
                               GetCurrentLevel());
    PushEvent(xyEvent, DeleteEvent);
    return;
    break;
  default:  
    OnInitialEvent(); 


    // Here is what it used to say:  PRS Aug 2016
    /*
    if (m_BackupNeeded)
    {
      party.movePartyBackward();
      m_BackupNeeded=FALSE;
      // Put it back // I put this here but it did not fix my problem so I removed it.  PRS 11May2016
      StartBackgroundZoneOnlyMusic();
    }
    */
    // And here is how I changed it:
    if (m_BackupStatus == Backup_PostEvent)
    {
      party.movePartyBackward();
      m_BackupStatus=Backup_None;
      // Put it back // I put this here but it did not fix my problem so I removed it.  PRS 11May2016
      StartBackgroundZoneOnlyMusic();
    }



    return;
  };

  // The only states that get here are the TASK_MovePreMove states.
  // perform the actual party movement now
  
  SetMyState(TASK_MoveAwaitKey);

  BOOL result = party.movePartyForward(reldir);
  ASSERT(result);

  // to filter out repeated requests by events
  // to backup the party one step on event exit.
  m_origPartyX=party.Posx;
  m_origPartyY=party.Posy;
  m_BackupStatus=Backup_None;

  QueueMapEvents(true, true);
  PushEvent(new PLAYSTEPSOUND_EVENT, DeleteEvent);

  UpdatePartyMovementData();
  ClearFormattedText(textData);
}

void ADVENTURE_MENU_DATA::QueueMapEvents(bool execute_XY_events, bool execute_Step_events)
{
  int currZone = levelData.GetCurrZone(party.Posx,party.Posy);  

  BEGIN_ZONE_EVENT *zoneEvent=NULL;
  if (currZone != levelData.GetPrevZone())
  {
    zoneEvent=new BEGIN_ZONE_EVENT(party.Posx,
                                   party.Posy);
    StartBackgroundZoneOnlyMusic();
  }

  // this event will check for, and load, any events that
  // should trigger based on current x,y position in map
  if (execute_XY_events)
  {
    BEGIN_XY_EVENT *xyEvent;
    xyEvent=new BEGIN_XY_EVENT(party.Posx,
                               party.Posy,
                               GetCurrentLevel());

    PushEvent(xyEvent, DeleteEvent);
  };
  // this event will check for, and load, any step events
  // that are available.
  if (execute_Step_events)
  {
    BEGIN_STEP_EVENT *stepEvent;
    stepEvent=new BEGIN_STEP_EVENT(party.Posx,
                                   party.Posy,
                                   GetCurrentLevel());

    /*
    BEGIN_TIME_EVENT *timeEvent;
    timeEvent=new BEGIN_TIME_EVENT(party.Posx,
      party.Posy,
      GetCurrentLevel(),
      party.days, party.hours, party.minutes);
    */
  
    // queue up the events  
    //PushEvent(timeEvent, DeleteEvent);
    PushEvent(stepEvent, DeleteEvent);
  };
  if (zoneEvent != NULL) PushEvent(zoneEvent, DeleteEvent);
}

unsigned int ADVENTURE_MENU_DATA::OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState)
{
  BOOL entryDenied;
  switch (msg)
  {
  case TASKMSG_TeleportBetweenLevels:
  case TASKMSG_TeleportWithinLevel:
    m_BackupStatus=Backup_None;
    // intentional fall through
  case TASKMSG_CancelForwardStep:
    SetMyState(TASK_MoveAwaitKey); 
    break;
  case TASKMSG_AllowForwardStep:
    {

      int slot=-1;
      if (taskState == TASK_MovePreMove0)
      {
        SetMyState(TASK_MovePostMove0);

        BOOL result = party.movePartyForward(0);
        ASSERT(result);
        // Posx and Posy are now the position after the move.

        // Determine if the party is not allowed entry to the new position.
        // If the first event at the new position 'Force the party backward'
        // then the party is not allowed to enter.  The events at the new
        // position will be triggered but no wall sounds will be heard and
        // we will not see what is at the new position.
        GameEvent *ev = levelData.eventData.GetFirstEvent(party.Posx, party.Posy);
        if (ev != NULL)
        {
          entryDenied = ev->ForcePartyBackup();  // We may want to change this to "IsEntryAllowed"
                                                 // so that a TAVERN is treated differently than (eg.) TELEPORT.
        }
        else
        {
          entryDenied = FALSE;
        };
#ifdef EARLY_WALL_SOUNDS
        // We will take the step now.  But some things will have to wait
        // until this event chain returns
        // Posx and Posy are before the move took place.
        switch (party.facing) 
        //switch (party.RelativeToActualFacing(party.PrevRelFacing)) 
        {      
        case FACE_NORTH:
          slot = levelData.area[party.Posy][party.Posx].wall[0];
          break;      
        case FACE_EAST:
          slot = levelData.area[party.Posy][party.Posx].wall[2];
          break;      
        case FACE_SOUTH:
          slot = levelData.area[party.Posy][party.Posx].wall[1];
          break;      
        case FACE_WEST:
          slot = levelData.area[party.Posy][party.Posx].wall[3];
          break;
        }
      if ((slot > 0) && (slot < MAX_WALLSETS) && !entryDenied)
        WallSets[slot].PlaySlotSound();
      if (entryDenied)
      {
        party.Posx = party.PrevPosx;
        party.Posy = party.PrevPosy;
        m_BackupStatus = Backup_PreMove;
      };
#else
#endif



        // to filter out repeated requests by events
        // to backup the party one step on event exit.
        m_origPartyX=party.Posx;
        m_origPartyY=party.Posy;
        //m_BackupNeeded=FALSE;

        return TASKMSG_STOP;
      };
    };
    break;
  case TASKMSG_MovePartyBackward: 
    if ((!IsCombatActive())&&(m_BackupStatus == Backup_None))
    {
      if ((m_origPartyX==party.Posx)&&(m_origPartyY==party.Posy))
      {
        //party.movePartyBackward();
        m_BackupStatus=Backup_PostEvent;
      }
    }
    return TASKMSG_STOP; // Should remember where we were so this won't happen twice?
  case TASKMSG_SetPartyXY:
    SetMyState(TASK_SetPartyXY);
    return TASKMSG_STOP;
  case TASKMSG_ExecuteEvent:
    if (GetRunAsVersion(GuidedTourVersion) <= _VERSION_0697_)
      SetMyState(TASK_MovePreMove0);
    else
      SetMyState(TASK_MoveExecuteEvent);
    return TASKMSG_STOP;
  case TASKMSG_CharacterCastSpell:
    SetMyState(TASK_CharacterCastSpell);
    return TASKMSG_STOP;
  default:
    break;
  };
  return 0;
}

int ADVENTURE_MENU_DATA::OnSaveGame(unsigned int *saveArea)
{
  saveArea[0]=m_origPartyX;
  saveArea[1]=m_origPartyY;
  saveArea[2]=m_BackupStatus;
  return 3;
}

int ADVENTURE_MENU_DATA::OnLoadGame(unsigned int *saveArea)
{
  m_origPartyX=saveArea[0];
  m_origPartyY=saveArea[1];
  m_BackupStatus=saveArea[2];
  return 3;
}

////////////////////////////////////////////////////////////SAVEGAME_MENU_DATA
void SAVEGAME_MENU_DATA::OnInitialEvent(void)
{
  FormatDisplayText(textData, "CHOOSE WHICH SLOT TO SAVE GAME INTO", FALSE);
  menu.setMenu(SaveMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, menu.totalItems());
  Invalidate();
}

void SAVEGAME_MENU_DATA::OnDraw(void)
{
  StandardMenuDraw();
}

void SAVEGAME_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  };
  int temp;
  if (menu.currentItem() != menu.totalItems())
  {
    temp = menu.currentItem()-1;
    MouseRender.EnableWaitCursor();
    SaveGame();  // Record event stack information
    {
      CString signature;
      signature.Format("%08x", gamedatSignature);
      globalData.global_asl.Insert("GAME.DAT_Signature", signature, 0);
      if (!serializeGame(TRUE, temp))      
          miscError = FileSaveError;
    };
    MouseRender.DisableWaitCursor();
  }

  PopEvent();  // don't include this event in saved stack data
}

////////////////////////////////////////////////////////////LOADGAME_MENU_DATA
void LOADGAME_MENU_DATA::OnInitialEvent(void)
{  
  menu.setMenu(LoadMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  int count = 0;
  for (int i=0;i<MAX_SAVE_GAME_SLOTS;i++)
  {
    if (SaveGameExists(i))
    {
      count++;
      break;
    }
  }
  if (count == 0)
    FormatDisplayText(textData, "THERE ARE NO SAVED GAMES AVAILABLE", FALSE);
  else
    FormatDisplayText(textData, "CHOOSE WHICH SLOT TO LOAD GAME FROM", FALSE);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, menu.totalItems());
  Invalidate();
}

void LOADGAME_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  for (int i=0;i<MAX_SAVE_GAME_SLOTS;i++)
  {
    if (!SaveGameExists(i))
      menu.setItemInactive(i+1);
  }
}

void LOADGAME_MENU_DATA::OnDraw(void)
{
  StandardMenuDraw();
}

void LOADGAME_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  };
  if (menu.currentItem() != menu.totalItems())
  {
    int temp,px,py,dir;  
    temp = menu.currentItem()-1; 
    MouseRender.EnableWaitCursor();
    if (serializeGame(FALSE, temp))
    {
      {
        const ASLENTRY *pSignatureASL;
        pSignatureASL = globalData.global_asl.Find("GAME.DAT_Signature");
        if (pSignatureASL != NULL)
        {
          unsigned int signature;
          sscanf(pSignatureASL->Value(), "%x", &signature);
          if (signature != gamedatSignature)
          {
#ifdef WIN10Problem
            WIN10ProblemActive = true;
            WriteDebugString("Setting WIN10ProblemActive = true\n");
#endif
            MsgBoxInfo("The saved game was created with a different version of the design");
#ifdef WIN10Problem
            WIN10ProblemActive = false;
            WriteDebugString("Setting WIN10ProblemActive = false\n");
#endif
          };
        };
      };
      dir = party.facing;
      px   = party.Posx;
      py   = party.Posy;      

      // events in taskList may be pointing to events
      // that will be deleted when LoadLevel() is called
      //
      // since we call event->OnLeaveQueue for each event
      // before cleaning it out of taskList, we need to do this
      // before deleting the level's events.
      taskList.PopAllButTopEvent(); // all but current event
      BOOL result = LoadLevel(party.level);

      party.Posx = px;
      party.Posy = py;
      party.facing = (BYTE)dir;

      if (result)
      {
        // restore event stack from loaded data
        while (!LoadGame()) // ie: forever!
        {
          MsgBoxError("Cannot Load Saved State");
          miscError = LevelLoadError;
        }
        StartBackgroundMusic();
      }
      else
        miscError = LevelLoadError;
    }
    else
      miscError = LevelLoadError;
    MouseRender.DisableWaitCursor();
  }
  if (miscError != NoError)
  {
    SignalShutdown();
  };
  PopEvent(); // pop this event
}

////////////////////////////////////////////////////////////BEGIN_TIME_EVENT
BEGIN_TIME_EVENT::BEGIN_TIME_EVENT(int sx, int sy, int level, int day, int hour, int minute) : GameEvent()
{
  event=CONTROL_BeginTimeEvent;
  m_x=sx;
  m_y=sy;
  m_level=level;
  m_day = day;
  m_hour = hour;
  m_minute = minute;
}

BEGIN_TIME_EVENT& BEGIN_TIME_EVENT::operator = (BEGIN_TIME_EVENT& src)
{
  if (this==&src) return *this;
  GameEvent::operator = (src);
  m_x=src.m_x;
  m_y=src.m_y;
  m_level=src.m_level;
  m_day=src.m_day;
  m_hour = src.m_hour;
  m_minute = src.m_minute;
  return *this;
}

void BEGIN_TIME_EVENT::OnInitialEvent(void)
{ 
  
  PopEvent();

  //TIME_EVENT *pTimeEvent = NULL;
  //if (levelData.haveTimeEvent(m_x, m_y, m_day, m_hour, m_minute, &pTimeEvent))
  //{
  //  PushEvent(pTimeEvent, LevelEvent);
  //}
}

int BEGIN_TIME_EVENT::OnSaveGame(unsigned int *saveArea)
{
  saveArea[0]=m_x;
  saveArea[1]=m_y;
  saveArea[2]=m_level;
  saveArea[3]=m_day;
  saveArea[4]=m_hour;
  saveArea[5]=m_minute;
  return 6;
}

int BEGIN_TIME_EVENT::OnLoadGame(unsigned int *saveArea)
{
  m_x = saveArea[0];
  m_y = saveArea[1];
  m_level = saveArea[2];
  m_day = saveArea[3];
  m_hour = saveArea[4];
  m_minute = saveArea[5];
  return 6;
}

////////////////////////////////////////////////////////////BEGIN_STEP_EVENT
BEGIN_STEP_EVENT::BEGIN_STEP_EVENT(int sx, int sy, int level) : GameEvent()
{
  event=CONTROL_BeginStepEvent;
  m_x=sx;
  m_y=sy;
  m_level=level;
}

BEGIN_STEP_EVENT& BEGIN_STEP_EVENT::operator = (BEGIN_STEP_EVENT& src)
{
  if (this==&src) return *this;
  GameEvent::operator = (src);
  m_x=src.m_x;
  m_y=src.m_y;
  m_level=src.m_level;
  return *this;
}

void BEGIN_STEP_EVENT::OnInitialEvent(void)
{
  if ( (m_x == -1) || (m_y == -1) || (m_level == -1) )
  {
     PopEvent();
     return;
  }

  int zone = levelData.GetCurrZone(m_x,m_y);  
  int stepCount = party.GetZoneStepCount(m_level, zone);
  int i=MAX_STEP_EVENTS-1; // push events in reverse order
  
  PopEvent();

  while (i>=0)
  {
    STEP_EVENT_DATA &data = levelData.stepEvents[i];
    // if this zone can trigger and count is multiple of step count
    if (   ((data.zoneMask & (1 << zone)) != 0) // does this zone trigger?
        && (data.stepCount > 0)         // must have non-zero trigger count
        && ((stepCount % data.stepCount)==0)  // does current step count match trigger count?
        && (stepCount > 0)) // Must have taken at least one step
    {
      // if event data is present...
      if (levelData.eventData.IsValidEvent(data.stepEvent))
      {
        PushEvent(levelData.eventData.GetEvent(levelData.stepEvents[i].stepEvent), 
                  LevelEvent);
      }
    }
    i--;
  }
}

int BEGIN_STEP_EVENT::OnSaveGame(unsigned int *saveArea)
{
  saveArea[0]=m_x;
  saveArea[1]=m_y;
  saveArea[2]=m_level;
  return 3;
}

int BEGIN_STEP_EVENT::OnLoadGame(unsigned int *saveArea)
{
  m_x = saveArea[0];
  m_y = saveArea[1];
  m_level = saveArea[2];
  return 3;
}

/*
////////////////////////////////////////////////////////////BEGIN_REST_EVENT
BEGIN_REST_EVENT::BEGIN_REST_EVENT(int x, int y)
{
  event=CONTROL_BeginRestEvent;
  m_x=x;
  m_y=y;
}

BEGIN_REST_EVENT& BEGIN_REST_EVENT::operator = (BEGIN_REST_EVENT& src)
{
  if (this==&src) return *this;
  GameEvent::operator = (src);
  m_x=src.m_x;
  m_y=src.m_y;
  return *this;
}

void BEGIN_REST_EVENT::OnInitialEvent(void)
{
  if ( (m_x == -1) || (m_y == -1) )
  {
     PopEvent();
     return;
  }
  GameEvent *tempEvent=NULL;
  if (levelData.haveRestEvent(
           m_x,
           m_y,
           &tempEvent))
  {
    ReplaceEvent(tempEvent, LevelEvent);
  }
  else
  {
    PopEvent(); // No rest events here
  };
}

int BEGIN_REST_EVENT::OnSaveGame(unsigned int *saveArea)
{
  saveArea[0]=m_x;
  saveArea[1]=m_y;
  return 2;
}

int BEGIN_REST_EVENT::OnLoadGame(unsigned int *saveArea)
{
  m_x = saveArea[0];
  m_y = saveArea[1];
  return 2;
}
*/

////////////////////////////////////////////////////////////BEGIN_ZONE_EVENT
BEGIN_ZONE_EVENT::BEGIN_ZONE_EVENT(int zx, int zy)  : GameEvent()
{
  event=CONTROL_BeginZoneEvent;
  m_x=zx;
  m_y=zy;
}

BEGIN_ZONE_EVENT& BEGIN_ZONE_EVENT::operator = (BEGIN_ZONE_EVENT& src)
{
  if (this==&src) return *this;
  GameEvent::operator = (src);
  m_x=src.m_x;
  m_y=src.m_y;
  return *this;
}

void BEGIN_ZONE_EVENT::OnInitialEvent(void)
{
  if ( (m_x == -1) || (m_y == -1) )
  {
     PopEvent();
     return;
  }

  if (levelData.haveZoneMsg(m_x, m_y))
  {
    TRACE("Activating Zone Msg Event\n");
    FormatDisplayText(textData, levelData.zoneData.zones[levelData.area[m_y][m_x].zone].zoneMsg);
  }
  PopEvent();
}

int BEGIN_ZONE_EVENT::OnSaveGame(unsigned int *saveArea)
{
  saveArea[0]=m_x;
  saveArea[1]=m_y;
  return 2;
}

int BEGIN_ZONE_EVENT::OnLoadGame(unsigned int *saveArea)
{
  m_x = saveArea[0];
  m_y = saveArea[1];
  return 2;
}

////////////////////////////////////////////////////////////BEGIN_XY_EVENT
BEGIN_XY_EVENT::BEGIN_XY_EVENT(int ex, int ey, int level) : GameEvent()
{
  event=CONTROL_BeginXYEvent;
  m_x=ex;
  m_y=ey;
  m_level=level;
  m_forceEventProcessing = false;
}

BEGIN_XY_EVENT& BEGIN_XY_EVENT::operator = (BEGIN_XY_EVENT& src)
{
  if (this==&src) return *this;
  GameEvent::operator = (src);
  m_x=src.m_x;
  m_y=src.m_y;
  m_level=src.m_level;
  m_forceEventProcessing = src.m_forceEventProcessing;
  return *this;
}

void BEGIN_XY_EVENT::OnInitialEvent(void)
{
  //int testflag=1;
  if (levelData.haveEvent(party.Posx,party.Posy))
  {
    // GetEvent will return the event that should be acted upon for current x,y.
    GameEvent *ev=levelData.eventData.GetFirstEvent(party.Posx, party.Posy);
    if (ev!=NULL) 
    {
      // The following made no sense.  An ordinary text event is triggered here.
      // Because the test seems backward.  And a TRANSFER that is supposed to
      // happen before entering does not ever get triggered.  Because the test seems
      // backward.  I tried simply removing the test and things seem OK.
      // PRS 20160625
      //
      //if (!ev->OnTestPreMove() ||  m_forceEventProcessing)
      {
        //if (testflag) flushOldImages();
        ReplaceEvent(ev, LevelEvent);
        return;
      };
    };
  };
  PopEvent(); // No events to be done here
}

unsigned int BEGIN_XY_EVENT::OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState)
{
  switch (msg)
  {
  case TASKMSG_SetPartyXY:
  case TASKMSG_TeleportWithinLevel:
  case TASKMSG_TeleportBetweenLevels:
    return TASKMSG_DELETE;
  default:
    break;
  };
  return 0;
}

////////////////////////////////////////////////////////////BEGIN_XY_FACING_EVENT
BEGIN_XY_FACING_EVENT::BEGIN_XY_FACING_EVENT(int fx, int fy, int level)  : GameEvent()
{
  event=CONTROL_BeginXYEvent;
  m_x=fx;
  m_y=fy;
  m_level=level;
}

BEGIN_XY_FACING_EVENT& BEGIN_XY_FACING_EVENT::operator = (BEGIN_XY_FACING_EVENT& src)
{
  if (this==&src) return *this;
  GameEvent::operator = (src);
  m_x=src.m_x;
  m_y=src.m_y;
  m_level=src.m_level;
  return *this;
}

void BEGIN_XY_FACING_EVENT::OnInitialEvent(void)
{
  if (levelData.haveEvent(party.Posx,party.Posy))
  {
    // GetEvent will return the event that should be acted upon for current x,y.
    GameEvent *ev=levelData.eventData.GetFirstEvent(party.Posx, party.Posy);
    if (ev!=NULL) 
    {
      if (ev->control.eventTrigger == FacingDirectionAnyTime)
      {
        ReplaceEvent(ev, LevelEvent);
        return;
      }
    }
  }
  PopEvent(); // No events to be done here
}

unsigned int BEGIN_XY_FACING_EVENT::OnTaskMessage(TASKMESSAGE msg, TASKSTATE staskState)
{
  switch (msg)
  {
  case TASKMSG_TeleportWithinLevel:
  case TASKMSG_TeleportBetweenLevels:
    return TASKMSG_DELETE;
  default:
    break;
  };
  return 0;
}

////////////////////////////////////////////////////////////PLAYSTEPSOUND_EVENT

void PLAYSTEPSOUND_EVENT::OnInitialEvent(void)
{
  if (!globalData.levelInfo.stats[globalData.currLevel].overland)
  {
     int slot=-1;
     switch (party.RelativeToActualFacing(party.PrevRelFacing)) 
     {      
     case FACE_NORTH:
        slot = levelData.area[party.Posy][party.Posx].wall[1];
        break;      
     case FACE_EAST:
        slot = levelData.area[party.Posy][party.Posx].wall[3];
        break;      
     case FACE_SOUTH:
         slot = levelData.area[party.Posy][party.Posx].wall[0];
        break;      
     case FACE_WEST:
        slot = levelData.area[party.Posy][party.Posx].wall[2];
        break;
     }
#ifdef EARLY_WALL_SOUNDS
#else
    if ((slot > 0) && (slot < MAX_WALLSETS))
      WallSets[slot].PlaySlotSound();
#endif
    slot = levelData.area[party.Posy][party.Posx].backgrounds(party.facing);
    if ((slot < 0) || (slot >= MAX_BACKGROUNDS))
    {
      slot = 0;
      WriteDebugString("Bogus backdrop slot num %u in PLAYSTEPSOUND_EVENT()\n", slot);
    }

    if (BackgroundSets[slot].suppressStepSound)
    {
      if (slot > 0) BackgroundSets[slot].PlaySlotSound();
    }
    else
    {
      if (slot > 0)  BackgroundSets[slot].PlaySlotSound();
      PlayPartyStep();
    }
  }
  else
    PlayPartyStep();
  PopEvent();
}

BOOL TEXT_EVENT_DATA::WaitUntilReturnPressed(void)
{
   if (WaitForReturn) return TRUE;
   switch (distance)
   {
     case AutoFarAway:
     case AutoNearby:
     case AutoUpClose:
       return TRUE;
   };
   if (textData.WaitForReturn()) return TRUE;
   return FALSE;
}

///////////////////////////////////////////////////////////////TEXT_EVENT_DATA
void TEXT_EVENT_DATA::OnInitialEvent(void)
{
  {
    CString txt;
    txt = GetEventText();
    // check to see GPDL should be invoked
    if (GetEventText().Left(8)=="**SHAZAM")
    {
      int len;
      len = txt.GetLength();
      {
        int i;
        for (i=8; i<len-1; i++)
        {
          if ((txt.GetAt(i) == '*') && (txt.GetAt(i+1) == '*'))
          {
            break;
          };
        };
        if (i < len-1)
        {
          menu.reset(NULL);
          ReplaceEvent(new GPDL_EVENT(this), DeleteEvent);
          return;
        };
      };
    };
  };
  menu.setMenu(TextEventData, NULL, FALSE, this, "TEXT");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText(), TRUE, HighlightText, WaitForReturn);

  currPic = GetEventPic();
  currPic.LoadPicSurfaces("");
  if (currPic.picType == SpriteDib)
  {
    currPic.SetFrame(party.DetermineSpriteDistance(distance));
  }

  if (PlayEventSounds && (sound != "")) hSound = PlaySound(sound); 
  if (   (distance == AutoUpClose)
      || (distance == AutoFarAway)
      || (distance == AutoNearby))
  {
    int delay = pic.timeDelay;
    if (delay < 30) delay = 30;
    SetEventTimer(37, delay);
  };


  Invalidate();
}

bool TEXT_EVENT_DATA::OnTimer(UINT tid)
{
  int delay;
  delay = pic.timeDelay;
  if (delay < 30) delay = 30;
  switch (distance)
  {
    case AutoFarAway:  distance = AutoNearby;  SetEventTimer(37, delay);  break;
    case AutoNearby:   distance = AutoUpClose; SetEventTimer(37, delay);  break;
    default:
    case AutoUpClose:  distance = UpClose;     break;
  };
  if (currPic.picType == SpriteDib)
  {
    currPic.SetFrame(party.DetermineSpriteDistance(distance));
  }
  OnUpdateUI();
  Invalidate();
  return true;
}

/*
   1  { "EXIT", 1 },
   2  { "PRESS ENTER TO CONTINUE", 7 },
*/

void TEXT_EVENT_DATA::OnKeypress(key_code key, char ascii)
{ 
  if (    (distance == AutoFarAway)
      ||  (distance == AutoNearby)
      ||  (distance == AutoUpClose))
  {
    return;
  };

  if (TABParty(key)) return;
  if (key != KC_RETURN) 
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  };
  //if (textData.currLine < textData.numLines)
  //{
  //  textData.currLine++;
  //  Invalidate();
  //  return;
  //};
  // all text has been displayed
  if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
  currPic.Clear();
  // let text continue being displayed if return not needed
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);
  // let music/sound continue playing?
  if (KillSound) ClearSound(&hSound);
  ChainHappened();
}

void TEXT_EVENT_DATA::OnDraw(void)
{


  UpdateTextEventScreen(this, m_displaySpecialGraphics ? 'B' : 0);
 // if (GetMyState()!=TASK_TextDisplayed) 
 //   SetMyState(TASK_TextDisplayed);
}

/*
   1  { "EXIT", 1 },
   2  { "PRESS ENTER TO CONTINUE", 7 },
*/

void TEXT_EVENT_DATA::OnUpdateUI(void)
{
  //if (CheckForMultiBoxText()) return;
  CheckForMultiBoxText();

  if (   (distance == AutoUpClose)
      || (distance == AutoFarAway)
      || (distance == AutoNearby) )
  {
    menu.setItemInactive(2); // "Press Enter"
  }
  else
  {
    //menu.setItemActive(2);
  };

  menu.setItemInactive(1); // Exit
  if (ShouldSimulateEnterKey()) 
  {
    menu.setItemInactive(2); // Press Enter to continue
    simulatedKeyQueue.PushKey(VK_RETURN);
  }
  Invalidate();
}


///////////////////////////////////////////////////////////////QUESTION_YES_NO

void QUESTION_YES_NO::OnInitialEvent(void)
{
  menu.setMenu(QuestionYesNoData, NULL, FALSE, this, "Quest_YesNo");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  SetMyState(TASK_QuestionYesNo_asking);
  sx=party.Posx;sy=party.Posy;
  Invalidate();
}

void QUESTION_YES_NO::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  };
  switch (GetMyState())
  {
  case TASK_QuestionYesNo_asking:
    if (menu.currentItem() == 1) // Yes
    {
      if (GetEventText2().GetLength() > 0)
      {
        menu.setMenu(QuestionYesData, NULL, FALSE, NULL, NULL);
        menu.setHorzOrient();
        FormatDisplayText(textData, GetEventText2());
        currPic = GetEventPic();
        //currPic.LoadPicSurfaces("sp_");
        currPic.LoadPicSurfaces("");
        SetMyState(TASK_QuestionYesNo_yes);
      }
      else
      {
        // skip the text and just chain
        SetMyState(TASK_QuestionYesNo_yesChain);
        if (IsGlobalEvent())
        {
          if (!globalData.eventData.IsValidEvent(YesChain))
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else 
            PushGlobalEvent(YesChain);
        }
        else
        {
          if (!levelData.eventData.IsValidEvent(YesChain))
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else 
            PushEvent(YesChain);
        }
        currPic.Clear();
        ClearFormattedText(textData);
      }
    }
    else // No
    {
      if (GetEventText3().GetLength() > 0)
      {
        menu.setMenu(QuestionNoData, NULL, FALSE, NULL, NULL);
        menu.setHorzOrient();
        FormatDisplayText(textData, GetEventText3());
        currPic = GetEventPic();
        //currPic.LoadPicSurfaces("sp_");
        currPic.LoadPicSurfaces("");
        SetMyState(TASK_QuestionYesNo_no);
      }
      else
      {
        // skip the text and just chain
        SetMyState(TASK_QuestionYesNo_noChain);
        
        if (IsGlobalEvent())
        {
          if (!globalData.eventData.IsValidEvent(NoChain))
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else 
            PushGlobalEvent(NoChain);
        }
        else
        {
          if (!levelData.eventData.IsValidEvent(NoChain))
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else 
            PushEvent(NoChain);
        }
        currPic.Clear();
        ClearFormattedText(textData);
      }
    };
    break;
  case TASK_QuestionYesNo_yes:
    SetMyState(TASK_QuestionYesNo_yesChain);

    if (IsGlobalEvent())
    {
      if (!globalData.eventData.IsValidEvent(YesChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      else 
        PushGlobalEvent(YesChain);
    }
    else
    {
      if (!levelData.eventData.IsValidEvent(YesChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      else 
        PushEvent(YesChain);
    }
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  case TASK_QuestionYesNo_no:
    SetMyState(TASK_QuestionYesNo_noChain);
    if (IsGlobalEvent())
    {
      if (!globalData.eventData.IsValidEvent(NoChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      else 
        PushGlobalEvent(NoChain);
    }
    else
    {
      if (!levelData.eventData.IsValidEvent(NoChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      else 
        PushEvent(NoChain);
    }
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  default:
    WriteDebugString("Bogus QUESTION_YES_NO state\n");
    ChainHappened();
  };
}

void QUESTION_YES_NO::OnDraw(void)
{
  UpdateQuestionYesNoScreen('l');
}

void QUESTION_YES_NO::OnReturnToTopOfQueue(void)
{
  currPic.Clear();
  ClearFormattedText(textData);
  switch (GetMyState())
  {
  case TASK_QuestionYesNo_yesChain:
    switch (YesChainAction)
    {
    case LabelDoNothing:
      ChainHappened();
      break;
    case LabelReturnToQuestion:
      // only if still in starting location
      if ((sx==party.Posx)&&(sy==party.Posy))
        OnInitialEvent();
      else
        ChainHappened();
      break;
    case LabelBackupOneStep:
      ChainHappened();
      if ((sx==party.Posx)&&(sy==party.Posy))
        TaskMessage(TASKMSG_MovePartyBackward);
      break;
    };
    break;
  case TASK_QuestionYesNo_noChain:
    switch (NoChainAction)
    {
    case LabelDoNothing:
      ChainHappened();
      break;
    case LabelReturnToQuestion:
      // only if still in starting location
      if ((sx==party.Posx)&&(sy==party.Posy))
        OnInitialEvent();
      else
        ChainHappened();
      break;
    case LabelBackupOneStep:
      if ((sx==party.Posx)&&(sy==party.Posy))
        TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
      break;
    };
    break;
  default:
    WriteDebugString("Bogus QuestioYesNoState\n");
    ChainHappened();
  };
}

int QUESTION_YES_NO::OnSaveGame(unsigned int *saveArea)
{
  // task state is automatically saved
  return 0; 
}

int QUESTION_YES_NO::OnLoadGame(unsigned int *saveArea)
{
  // task state is automatically restored
  return 0;
}

/////////////////////////////////////////////////////////////VAULT_EVENT_DATA

void VAULT_EVENT_DATA::OnInitialEvent(void)
{
  menu.setMenu(VaultMenuData, NULL, FALSE, this, "Vault");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 5);
  FormatDisplayText(textData, GetEventText());
  setTreasureEventState(this);
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  Invalidate();
}

void VAULT_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  };
  switch (menu.currentItem()-1) 
  {
  case 0: // View
    PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),this), DeleteEvent);
    break;
  case 1: // Take
    setTreasureEventState(this);
    PushEvent(new TAKE_TREASURE_DATA(this), DeleteEvent);
    break;
  case 2: // Pool
    // move party funds into vault
    party.AddPooledMoneyToVault(WhichVault);
    setTreasureEventState(this);
    break;     
  case 3: // Share
    // distribute vault funds to party members
    party.RemovePooledMoneyFromVault(WhichVault);    
    // rebuild item list text
    setTreasureEventState(this);
    break;      
  case 4: // Items
    PushEvent(new ITEMS_MENU_DATA(GetEventType(),this,NULL, NULL, 0),DeleteEvent);
    break;      
  case 5: // Exit
    // no point to auto-pooling money, the vault has the pooled money stored
    treasureItemListText.Clear();
    currPic.Clear();
    if (WaitUntilReturnPressed()) ClearFormattedText(textData);
    if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
    ChainHappened();
  };
}

void VAULT_EVENT_DATA::OnDraw(void)
{
  UpdateVaultScreen('i');
}

void VAULT_EVENT_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;

  if (!party.PartyHasMoney())
    menu.setItemInactive(3); // pool
  if (!globalData.VaultHasMoney(WhichVault)) 
    menu.setItemInactive(4); // share
  if (treasureItemListText.GetCount() <= 0)
    menu.setItemInactive(2); // take
}

void VAULT_EVENT_DATA::OnReturnToTopOfQueue(void)
{
  if (GetMyState()==TASK_AskLeaveMoneyBehind)
  {
    if (party.tradeItem==0) // no, don't leave money behind
    {
      // return to vault menu
      menu.setMenu(VaultMenuData, NULL, FALSE, NULL, NULL);
      menu.setHorzOrient();
      setTreasureEventState(this);
      currPic = GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      ClearFormattedText(textData);
      Invalidate();
    }
    else
    {
      party.moneyPooled=FALSE;
      party.poolSack.Clear(); // forfeit pooled funds

      treasureItemListText.Clear();
      currPic.Clear();
      ClearFormattedText(textData);
      if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
    }
  }
  else
  {
    //menu.setMenu(VaultMenuData);
    //menu.setHorzOrient();
    //setTreasureEventState(this);
    //currPic = GetEventPic();
    //currPic.LoadPicSurfaces("sp_");
    //currPic.LoadPicSurfaces("");
    //ClearFormattedText(textData);
    OnInitialEvent();
    //Invalidate();
  }
}

void VAULT_EVENT_DATA::OnLeaveQueue(void)
{
  //if (party.moneyPooled)
  //  party.sharePartyGold();
}

////////////////////////////////////////////////////////////GIVE_TREASURE_DATA

void GIVE_TREASURE_DATA::OnInitialEvent(void)
{
  if (SilentGiveToActiveChar)
  {
    CHARACTER &dude = GetActiveChar(this);

    POSITION pos = items.GetHeadPosition();
    while (pos!=NULL)
      dude.myItems.AddItem(items.GetNext(pos));

    dude.money.Transfer(money);

    ChainHappened();
    return;
  }

  SetMyState(TASK_initialized);
  menu.setMenu(GiveTreasureMenuData, NULL, FALSE, this, "GiveTreasure");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 5);
  HOOK_PARAMETERS hookParameters;
   FormatDisplayText(textData, getGlobalEventMessage("You Have Found Treasure!", "EventGiveTreasure", hookParameters, "TreasureMessage", control));

  // load treasure picture
  int zone = levelData.GetCurrZone(party.Posx,party.Posy);
  currPic = levelData.zoneData.zones[zone].treasurePicture;
  //PRS 22July2011  currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  
  moneyRemain.Clear();
  moneyRemain=money;

  itemsRemain = items;
  setTreasureEventState(this);

  //m_detectSpellKey = GetActiveChar(this).CanCastDetectMagic();
#ifdef SpellDetectMagic
  m_detectSpellID = GetActiveChar(this).CanCastDetectMagic();
#endif
  Invalidate();
}

void GIVE_TREASURE_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  };

  // the treasure message is only displayed once
  ClearFormattedText(textData);
  switch (menu.currentItem()-1) 
  {
  case 0: // View character
    SetMyState(TASK_initialized);
    PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),this), DeleteEvent);
    break;
  case 1: // Take item
    setTreasureEventState(this);
    SetMyState(TASK_initialized);
    PushEvent(new TAKE_TREASURE_DATA(this), DeleteEvent);
    break;
  case 2: // Pool
    // put all party funds into treasure data
    party.poolPartyGold();
    moneyRemain.Transfer(party.poolSack);
    setTreasureEventState(this);
    break;
  case 3: // Share
    // share all platinum, even from treasure event
    party.poolSack.Transfer(moneyRemain);
    party.sharePartyGold();
    // restore any leftovers into treasure list
    moneyRemain.Transfer(party.poolSack);
    setTreasureEventState(this);
    break;
  case 4: // Detect
    {
      SetMyState(TASK_initialized);
      // cast detect spell by active dude    
      //m_detectSpellKey = GetActiveChar(this).CanCastDetectMagic();
#ifdef SpellDetectMagic
      m_detectSpellID = GetActiveChar(this).CanCastDetectMagic();
      //PushEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(),NULL,&GetActiveChar(this),m_detectSpellKey), DeleteEvent);
      PushEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(),
                                                    NULL,
                                                    &GetActiveChar(this),
                                                    //m_detectSpellKey),
                                                    m_detectSpellID),
                                                        DeleteEvent);
#endif
    } 
    break;
  case 5: // Exit
    if (party.moneyPooled)
    {      
      SetMyState(TASK_AskLeaveMoneyBehind);
      HOOK_PARAMETERS hookParameters;
      CString message = getGlobalEventMessage("LEAVE ALL TREASURE BEHIND?", "EventGiveTreasureLeft", hookParameters, "TreasureLeftMessage", control);
      PushEvent(new ASK_YES_NO_MENU_DATA(message,TRUE, GiveTreasure), DeleteEvent);
    }
    else if (treasureItemListText.GetCount() > 0)
    {
      SetMyState(TASK_AskLeaveTreasureBehind);
      HOOK_PARAMETERS hookParameters;
      CString message = getGlobalEventMessage("THERE IS STILL TREASURE HERE\r\nDO YOU WANT TO LEAVE IT BEHIND?", "EventGiveTreasureLeft", hookParameters, "TreasureLeftMessage", control);
      PushEvent(new ASK_YES_NO_MENU_DATA(message,TRUE, GiveTreasure), DeleteEvent);
    }
    else
    {
      clearTreasureItemState();
      currPic.Clear();
      if (WaitUntilReturnPressed()) ClearFormattedText(textData);
      ChainHappened();
    }
    break;
  default:
    WriteDebugString("Unhandled choice in GiveTreasureMenu()\n");
    miscError = InvalidMenuState;
    ChainHappened();
  }
}

void GIVE_TREASURE_DATA::OnDraw(void)
{
  UpdateGiveTreasureScreen('h');
}

void GIVE_TREASURE_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;

  // enable SHARE menu for two cases:
  //   - party funds are pooled
  //   - funds available in GiveTreasure
  //
  // this allows SHARE menu to auto distribute the money
  // given by the event without needing to force a character to
  // TAKE the money first.
  //
  // when party money is pooled, it goes into the
  // GiveTreasure money holder called moneyRemain.
  //

  // ** original code, broken **
  //if (party.moneyPooled)
  //{
  //  if (moneyRemain.Total()==0.0)
  //    party.moneyPooled=FALSE;
  //}
  
  party.moneyPooled = (moneyRemain.Total() > 0.0);

  if (party.moneyPooled)
    menu.setItemInactive(3); // pool
  else
    menu.setItemInactive(4); // share
  
  if (treasureItemListText.GetCount() <= 0)
    menu.setItemInactive(2); // take
  
  if (!levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].AllowMagic)
    menu.setItemInactive(5); // detect

  if (!GetActiveChar(this).CanCastSpells())
  {
    menu.setItemInactive(5); // detect
  }
  else 
  {
    // check for char detect spell memorized
    //m_detectSpellKey = globalData.m_spellSpecialData.HasDetectMagicSpell(GetActiveChar());
    //m_detectSpellKey = GetActiveChar(this).CanCastDetectMagic();
#ifdef SpellDetectMagic
    m_detectSpellID = GetActiveChar(this).CanCastDetectMagic();
    //if (!m_detectSpellKey.IsValidSpell())
    if (!m_detectSpellID.IsValidSpell())
#endif
       menu.setItemInactive(5); // detect
  }
}

void GIVE_TREASURE_DATA::OnReturnToTopOfQueue(void)
{
  if (GetMyState()==TASK_AskLeaveMoneyBehind)
  {
    if (party.tradeItem==0) // no, don't leave money behind
    {
      //party.poolSack.Transfer(moneyRemain);
      //party.sharePartyGold(); 
      SetMyState(TASK_initialized);
      menu.setMenu(GiveTreasureMenuData, NULL, FALSE, NULL, NULL);
      menu.setHorzOrient();
      int zone = levelData.GetCurrZone(party.Posx,party.Posy);
      currPic = levelData.zoneData.zones[zone].treasurePicture;
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      setTreasureEventState(this);
      ClearFormattedText(textData);
    }
    else
    {
      party.moneyPooled=FALSE;
      party.poolSack.Clear(); // forfeit pooled funds

      clearTreasureItemState();
      currPic.Clear();
      if (WaitUntilReturnPressed()) ClearFormattedText(textData);
      ChainHappened();
      return; // Do not Invalidate() because we popped.
    }
  }
  else if (GetMyState()==TASK_AskLeaveTreasureBehind)
  {
    if (party.tradeItem==0) // no, don't leave treasure behind
    {
      SetMyState(TASK_initialized);
      menu.setMenu(GiveTreasureMenuData, NULL, FALSE, NULL, NULL);
      menu.setHorzOrient();
      int zone = levelData.GetCurrZone(party.Posx,party.Posy);
      currPic = levelData.zoneData.zones[zone].treasurePicture;
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      setTreasureEventState(this);
      ClearFormattedText(textData);
    }
    else
    {
      clearTreasureItemState();
      currPic.Clear();
      if (WaitUntilReturnPressed()) ClearFormattedText(textData);
      ChainHappened();
      return; // Do not Invalidate() because we popped
    }
  }
  else
  {
    SetMyState(TASK_initialized);
    menu.setMenu(GiveTreasureMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
    int zone = levelData.GetCurrZone(party.Posx,party.Posy);
    currPic = levelData.zoneData.zones[zone].treasurePicture;
    //currPic.LoadPicSurfaces("sp_");
    currPic.LoadPicSurfaces("");
    setTreasureEventState(this);
    ClearFormattedText(textData);
  }
  Invalidate();
}

void GIVE_TREASURE_DATA::OnLeaveQueue(void)
{
}

////////////////////////////////////////////////////////////TAKE_TREASURE_DATA

void TAKE_TREASURE_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  party.tradeQty = 0;
  setTreasureEventState(m_pCallerEvent);
  menu.setMenu(TakeTreasureMenuData, NULL, FALSE, this, "TakeTreasure");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
  SetMyState(TASK_TakeTreasure);
  handleItemsFormInput(ITEMSFORM_exit, treasureItemListText);
  handleItemsFormInput(ITEMSFORM_initial, treasureItemListText);
  Invalidate();
}


void TAKE_TREASURE_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key,ascii, NULL, NULL);
    return;
  };
/*
  1 { "NEXT", 0 },
  2 { "PREV", 0 },
  3 { "TAKE", 0 },
  4 { "HALVE", 0 },
  5 { "JOIN", 0 },
  6 { "SHARE", 0 },
  7 { "EXIT", 1 }
};
*/
  int textIndex = currTreasureTextIndex();
  switch (menu.currentItem()) 
  {
  case 1: // next
    nextTreasurePage();
    break;
  case 2: // prev
    prevTreasurePage();
    break;
  case 3: // take
    if (itemIsMoney(treasureItemListText.GetItemClass(textIndex)))    
    {
       SetMyState(TASK_TakeTreasureGetMoneyQty);
       PushEvent(new GET_MONEY_QTY_DATA(GetEventType(),m_pCallerEvent),DeleteEvent);
       return;
    }
    else 
    { 
      int qty = treasureItemListText.GetItemQty(textIndex);
      int bq = itemData.GetItemBundleQty(treasureItemListText.GetItemID(textIndex));
      if (bq < 1) bq = 1;
      if (bq < qty) qty = bq;

      int cost=-1;
      switch (m_pCallerEvent->GetEventType())
      {
      case Vault:
        cost = globalData.vault[((VAULT_EVENT_DATA*)m_pCallerEvent)->WhichVault].items.GetPurchasePrice(treasureItemListText.GetCharIndex(textIndex));
        break;
      case Combat:
        {
          die(0xab4e8);
          int dude = treasureItemListText.GetItemData(textIndex);
          cost = combatData.GetCombatant(dude)->m_pCharacter->myItems.GetPurchasePrice(treasureItemListText.GetCharIndex(textIndex));
        }
        break;
      default:
        cost=-1;
        break;
      }

      // taking an item (not money, gems, jewelry)
      BOOL success = takeTreasure(treasureItemListText.GetItemID(textIndex),
                                  qty,//treasureItemListText.GetItemQty(textIndex),
                                  treasureItemListText.GetItemCharges(textIndex),
                                  treasureItemListText.GetItemIdentified(textIndex),
                                  GetActiveChar(m_pCallerEvent, party.activeCharacter),
                                  cost);
       
      if ((success) && (miscError == NoError))
      {
        switch (m_pCallerEvent->GetEventType())
        {
        case CombatTreasure:
        case GiveTreasure:
          ((GIVE_TREASURE_DATA *)m_pCallerEvent)->
                   itemsRemain.deleteItem(treasureItemListText.
                                          GetCharIndex(textIndex), 
                                          qty);
          break;
        case Vault:
          globalData.RemoveItemFromVault(((VAULT_EVENT_DATA*)m_pCallerEvent)->WhichVault, treasureItemListText.GetCharIndex(textIndex), qty);
          break;
          
        case Combat:
          die(0xab4e9);
          /*
          {
            // remove treasure from source combatant inventory
           int dude = treasureItemListText.GetItemData(textIndex); // original src char index
           int itemKey = treasureItemListText.GetCharIndex(textIndex); // original inv key
           combatData.combatants[dude].myItems.deleteItem(itemKey, qty);
          }
          */
          break;
        default:
          die(0xab4ea); // is this an error?
          break;
        }
        // rebuild item list text
        setTreasureEventState(m_pCallerEvent);
      }
    }
    break;
  case 4: // Halve
    ASSERT( (m_pCallerEvent != NULL) && (m_pCallerEvent->GetEventType() == Vault) );

    if (treasureItemListText.ItemCanBeHalved(textIndex))
    {
      globalData.vault[((VAULT_EVENT_DATA*)m_pCallerEvent)->WhichVault].items.halveItem(treasureItemListText.GetCharIndex(textIndex));     
      // rebuild item list text
      setTreasureEventState(m_pCallerEvent);
    }
    break;
  case 5: // Join
    ASSERT( (m_pCallerEvent != NULL) && (m_pCallerEvent->GetEventType() == Vault) );
    if (treasureItemListText.ItemCanBeJoined(textIndex))
    {
       globalData.vault[((VAULT_EVENT_DATA*)m_pCallerEvent)->WhichVault].items.joinItems(treasureItemListText.GetCharIndex(textIndex));
       // rebuild item list text
       setTreasureEventState(m_pCallerEvent);
    }
    break;
  case 6: // Exit    
    handleItemsFormInput(ITEMSFORM_exit, treasureItemListText);
    PopEvent();
    break;
  default:
    WriteDebugString("Unhandled choice in takeTreasureMenu()\n");
    miscError = InvalidMenuState;
    PopEvent();
    break;
  };
}

void TAKE_TREASURE_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseMenuInventory(mx, my);
}

void TAKE_TREASURE_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
/*
  1 { "NEXT", 0 },
  2 { "PREV", 0 },
  3 { "TAKE", 0 },
  4 { "HALVE", 0 },
  5 { "JOIN", 0 },
  6 { "EXIT", 1 }
};
*/
  if (treasureItemListText.GetCount() <= 0)
  {
    menu.setAllItemsInactive();
    menu.setItemActive(6); // exit
    return;
  }

  if (!allowTreasureItemNextPage())
    menu.setItemInactive(1); // next
  if (!allowTreasureItemPrevPage())
    menu.setItemInactive(2); // prev

  if (   (m_pCallerEvent != NULL) 
      && (m_pCallerEvent->GetEventType() == Vault))
  {
    switch (treasureItemListText.GetItemClass(currTreasureTextIndex()))
    {
    case itemType:   
      if (!treasureItemListText.ItemCanBeHalved(currTreasureTextIndex()))
      {
        menu.setItemInactive(4); // halve
        menu.setItemInactive(5); // join
      }  
      if (treasureItemListText.GetItemQty(currTreasureTextIndex()) <= 1)
      {
        menu.setItemInactive(4); // halve
      }
      break;
    case PlatinumType:
    case ElectrumType:
    case GoldType:
    case CopperType:
    case SilverType:
    case GemType:
    case JewelryType:
    case SpecialItemType:
    case SpecialKeyType:
    case CoinType6:
    case CoinType7:
    case CoinType8:
    case CoinType9:
    case CoinType10:
      menu.setItemInactive(4); // halve
      menu.setItemInactive(5); // join
      break;
    default:
      menu.setItemInactive(4); // halve
      menu.setItemInactive(5); // join
    };
  }
  else
  {
    // can never join/halve treasure except for vault event
    menu.setItemInactive(4); // halve
    menu.setItemInactive(5); // join
  }
}

void TAKE_TREASURE_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  //displayTreasure();
  if (errorTextData.numLines == 0)
    handleItemsFormInput(ITEMSFORM_refresh, treasureItemListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
  //UpdateTakeTreasureScreen();
}

void TAKE_TREASURE_DATA::OnReturnToTopOfQueue(void)
{
  if ((party.tradeQty > 0) && (GetMyState() == TASK_TakeTreasureGetMoneyQty))
  {
    // returning here after GetMoneyQty pops itself
    //
    // add sum to character, and remove it from source
    BOOL success = takeTreasureMoney(treasureItemListText.GetItemClass(currTreasureTextIndex()),
                                     party.tradeQty,
                                     GetActiveChar(m_pCallerEvent,party.activeCharacter));
  
    // determine source of money
    if ((success) && (m_pCallerEvent!=NULL))
    {
      switch (m_pCallerEvent->GetEventType())
      {
      case Vault: // remove money from vault      
        globalData.UpdateMoneyInVault(((VAULT_EVENT_DATA*)m_pCallerEvent)->WhichVault,
                                      treasureItemListText.GetItemClass(currTreasureTextIndex()),
                                      -party.tradeQty);
        break;

      case ShopEvent:
      case TempleEvent: // remove from pooled funds
        party.deductPoolMoney(treasureItemListText.GetItemClass(currTreasureTextIndex()), party.tradeQty);
        break;

      case Combat:
        die(0xab4eb);
        /*
        {
          // remove treasure from source combatant inventory
         int dude = treasureItemListText.GetItemData(currTreasureTextIndex()); // original src char index
         itemClassType type = treasureItemListText.GetItemClass(currTreasureTextIndex());
          switch (type)
          {
          case ElectrumType:
          case GoldType:
          case SilverType:
          case CopperType:
          case CoinType6:
          case CoinType7:
          case CoinType8:
          case CoinType9:
          case CoinType10:
          case PlatinumType: // even pooled platinum is added to platinumRemain
            combatData.combatants[dude].money.Subtract(type, party.tradeQty);
            break;
          case GemType:
            combatData.combatants[dude].money.RemoveMultGems(party.tradeQty);
            break;
          case JewelryType:
            combatData.combatants[dude].money.RemoveMultJewelry(party.tradeQty);
            break;
          default:
            break;
          }
        }
        */
        break;

      case CombatTreasure:
      case GiveTreasure: // update treasureItemList money    
        {
          GIVE_TREASURE_DATA *data = 
               reinterpret_cast<GIVE_TREASURE_DATA*>(m_pCallerEvent);
          if (data != NULL)
          {
            // decrement temp event quantity
            switch (treasureItemListText.GetItemClass(currTreasureTextIndex()))
            {
            case ElectrumType:
            case GoldType:
            case SilverType:
            case CopperType:
            case CoinType6:
            case CoinType7:
            case CoinType8:
            case CoinType9:
            case CoinType10:
            case PlatinumType: // even pooled platinum is added to platinumRemain
              data->moneyRemain.Subtract(treasureItemListText.GetItemClass(currTreasureTextIndex()), party.tradeQty);
              break;
            case GemType:
              data->moneyRemain.RemoveMultGems(party.tradeQty);
              break;
            case JewelryType:
              data->moneyRemain.RemoveMultJewelry(party.tradeQty);
              break;
            default:
              break;
            }
          }
        }
        break;

      default:
        break;
      }
    }
  }

  OnInitialEvent();
}











////////////////////////////////////////////////////////////SCRIPT_CHOICE_DATA

void SCRIPT_CHOICE_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  //party.tradeQty = 0;
  //setTreasureEventState(m_pCallerEvent);
  menu.setMenu(ScriptChoiceMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
  //SetMyState(TASK_TakeTreasure);

  {
    ActorType actor;
    HOOK_PARAMETERS hookPArameters;
    SCRIPT_CONTEXT scriptContext;
    COMBATANT *pCombatant;
    CString choices;
    pCombatant = &combatData.getActiveCombatCharacter();
    pCombatant->GetContext(&actor);
    SetCharContext(&actor);
#ifdef newCombatant
    scriptContext.SetCharacterContext(pCombatant->m_pCharacter);
#else
    scriptContext.SetCharacterContext(pCombatant);
#endif
    choices = pCombatant->RunCombatantScripts(
                        ON_READYCOMBATEQUIPMENT, 
                        ScriptCallback_RunAllScripts, 
                        NULL,
                        "During combat player seleced a scripted choice menu item");
    ClearCharContext();
    menu.setCurrentItem(1);
    if (choices.IsEmpty())
    {
      PopEvent();
      return;
    };
    scriptChoiceListText.Clear();
    handleItemsFormInput(ITEMSFORM_exit, scriptChoiceListText);
    scriptChoiceListText.UseRdy = FALSE;
    scriptChoiceListText.UseQty = FALSE;
    {
      int start;
      int col;
      int len;
      len = choices.GetLength();
      for (start=col=0; col<len; col++)
      {
        if (choices[col] == '/')
        {
          if (col > start)
          {
            scriptChoiceListText.Add(1,EquipmentSetType,choices.Mid(start,col-start));
            start = col+1;
          };
        };
      };
      if (col > start) scriptChoiceListText.Add(1,EquipmentSetType,choices.Mid(start,col-start));
      ItemsOnPage = scriptChoiceListText.GetCount();
      if (ItemsOnPage == 0) 
      {
        PopEvent();
        return;
      };
      startItem = 0;
      party.activeItem = 0;
    };
  };


  //handleItemsFormInput(ITEMSFORM_initial, scriptChoiceListText);
  Invalidate();
}


void SCRIPT_CHOICE_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key,ascii, NULL, NULL);
    return;
  };
/*
  1 { "SELECT", 0 },
  2 { "EXIT",   1 }
};
*/
  int index = startItem+party.activeItem;
  switch (menu.currentItem()) 
  {
  case 1: // select
    {
      ActorType actor;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      COMBATANT *pCombatant;
      CString choices;
      pCombatant = &combatData.getActiveCombatCharacter();
      pCombatant->GetContext(&actor);
      SetCharContext(&actor);
      hookParameters[5] = scriptChoiceListText.GetItemNameText(index);
#ifdef newCombatant
      scriptContext.SetCharacterContext(pCombatant->m_pCharacter);
#else
      scriptContext.SetCharacterContext(pCombatant);
#endif
      pCombatant->RunCombatantScripts(
                          ON_READYCOMBATEQUIPMENT,
                          ScriptCallback_RunAllScripts,
                          NULL,
                          "Combat - Scripted Choice 'SELECT'");
#ifdef newCombatant
      pCombatant->m_pCharacter->RunCharacterScripts(
                                    ON_READYCOMBATEQUIPMENT, 
                                    ScriptCallback_RunAllScripts,
                                    NULL,
                                    "Combat - Scripted choice 'SELECT'");
#else
      pCombatant->RunCharacterScripts(ON_READYCOMBATEQUIPMENT, ScriptCallback_RunAllScripts, NULL);
#endif
      ClearCharContext();
      PopEvent();
    };
    break;
  case 2: // prev
    PopEvent();
    break;
  default:
    WriteDebugString("Unhandled choice in takeTreasureMenu()\n");
    miscError = InvalidMenuState;
    PopEvent();
    break;
  };
}

void SCRIPT_CHOICE_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseMenuInventory(mx, my);
}

void SCRIPT_CHOICE_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;

  // 1 { "SELECT", 0 },
  // 2 { "EXIT",   1 }

  if (scriptChoiceListText.GetCount() <= 0)
  {
    menu.setItemActive(2); // exit
    return;
  }

}

void SCRIPT_CHOICE_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  //displayTreasure();
  if (errorTextData.numLines == 0)
    handleItemsFormInput(ITEMSFORM_refresh, scriptChoiceListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
  //UpdateTakeTreasureScreen();
 
}


////////////////////////////////////////////////////////////VIEW_CHARACTER_DATA

void VIEW_CHARACTER_DATA::OnInitialEvent(void)
{
  //CHARACTER *pCharacter;
  //CLASS_DATA *pClass;
  //classType classtype;
  ClearFormattedText(textData);
  //pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
  //classtype = pCharacter->GetClass();
  //pClass = classData.GetClassData(classtype);

  menu.setMenu(ViewCharMenuData, NULL, FALSE, this, "ViewCharacter");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 8);
  if ((m_pOrigEvent!=NULL) && (m_pOrigEvent->GetEventType()==Combat))
  {
    // Need to set activeCharacter based upon current combatant
    //
    // If we get this far, the character being viewed must be a
    // party member.
    //
    // The party member may be an NPC pre-gen, or an original
    // created character.
    //

    int index = -1;
    // figure out which party member this equates to.
    for (int i=0;i<party.numCharacters;i++)
    {
      if (party.characters[i].IsSameCharacter(GetActiveChar(m_pOrigEvent)))
      {
        index = i;
        break;
      }
    }

    ASSERT( (index>=0) && (index < party.numCharacters) );
    if ( (index<0) || (index >= party.numCharacters) )
      index = 0;

    party.activeCharacter = index;
  }
  currPic = GetActiveChar(m_pOrigEvent,party.activeCharacter).smallPic;
  //currPic.LoadPicSurfaces("prt_");
  currPic.LoadPicSurfaces("");

  CString examinetext(GetActiveChar(m_pOrigEvent,party.activeCharacter).ExamineLabel);
  if (examinetext.GetLength()>0)
  {
    menu.changeMenuItem(7, examinetext);
    menu.SetFirstLettersShortcuts(TRUE);
  }

  //GraphicsMgr.EnableFontColorTags(FALSE);
  SetMyState(TASK_initialized);
  CheckCureOrWhatever();
#ifdef NEW_LAY
  CheckLayOrWhatever();
#endif
  GetActiveChar(m_pOrigEvent,party.activeCharacter).ComputeCharacterViewValues();
  Invalidate();
}

void VIEW_CHARACTER_DATA::CheckCureOrWhatever(void)
{
  // Determine if the "CURE" menu selection should appear.
  CHARACTER *pCharacter;
  CLASS_DATA *pClass;
  //classType classtype;
  CLASS_ID classID;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  ActorType actor;

  pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
  //classtype = pCharacter->GetClass();
  classID = pCharacter->GetClass();
  //pClass = classData.GetClassData(classtype);
  pClass = classData.GetClass(classID);
  
  pCharacter->GetContext(&actor);
  SetCharContext(&actor);
  scriptContext.SetCharacterContext(pCharacter);
  scriptContext.SetClassContext(pClass);
  pCharacter->RunCharacterScripts(CAN_CURE_OR_WHATEVER,
                                  ScriptCallback_RunAllScripts,
                                  "",
                                  "Should 'CURE' menu item be displayed?"
                                  );
  if (pClass != NULL)
  {
    pClass->RunClassScripts(CAN_CURE_OR_WHATEVER,
                            ScriptCallback_RunAllScripts,
                            "",
                            "Should 'CURE' menu item be displayed?");
  };
  ClearCharContext();
  this->m_cureOrWhateverMenuText = hookParameters[5];
  sscanf(hookParameters[6],"%d",&m_cureOrWhateverShortcut);
    
};
  
  
#ifdef NEW_LAY
void VIEW_CHARACTER_DATA::CheckLayOrWhatever(void)
{
  // Determine if the "CURE" menu selection should appear.
  CHARACTER *pCharacter;
  CLASS_DATA *pClass;
  //classType classtype;
  CLASS_ID classID;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  ActorType actor;

  pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
  //classtype = pCharacter->GetClass();
  classID = pCharacter->GetClass();
  //pClass = classData.GetClassData(classtype);
  pClass = classData.GetClass(classID);
  
  pCharacter->GetContext(&actor);
  SetCharContext(&actor);
  scriptContext.SetCharacterContext(pCharacter);
  scriptContext.SetClassContext(pClass);
  pCharacter->RunCharacterScripts(CAN_LAY_OR_WHATEVER,
                                  ScriptCallback_RunAllScripts,
                                  "",
                                  "Should 'LAY' menu item be displayed?");
  if (pClass != NULL)
  {
    pClass->RunClassScripts(CAN_LAY_OR_WHATEVER,
                            ScriptCallback_RunAllScripts,
                            "",
                            "Should 'LAY' menu item be displayed?");
  };
  ClearCharContext();
  this->m_layOrWhateverMenuText = hookParameters[5];
  sscanf(hookParameters[6],"%d",&m_layOrWhateverShortcut);
    
};
  
#endif



void VIEW_CHARACTER_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key,ascii);
    return;
  };
  switch (menu.currentItem())
  {
  case 1: // Items
    PushEvent (new ITEMS_MENU_DATA(GetEventType(), m_pOrigEvent, NULL, NULL, 0), DeleteEvent);
    break;
  case 2: // Display (was Spells - not used)
    PushEvent(new DISPLAY_SPELL_EFFECTS_MENU_DATA(m_pOrigEvent), DeleteEvent);    
    break;
  case 3: // Trade
    // never active - go to items screen to trade
    break;
  case 4: // Drop
    // never active - go to items screen to drop
    break;

    /*
    LAY is a healing ability of paladins.  Paladins may lay on hands and heal two
    hit points per level of damage a day.  Select the LAY command, then select the
    character to be healed.  This command is only displayed when a paladin has not
    yet laid on hands in a day.
    */
  case 5: // Lay On Hands - PALADIN ONLY
#ifdef NEW_LAY
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    ActorType actor;

    CHARACTER *pCharacter;
    CLASS_DATA *pClass;
    CString result;
    //classType classtype;
    CLASS_ID classID;
    pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
    classID = pCharacter->GetClass();
    pClass = classData.GetClass(classID);

    pCharacter->GetContext(&actor);
    SetCharContext(&actor);
    scriptContext.SetCharacterContext(pCharacter);
    scriptContext.SetClassContext(pClass);
    pCharacter->RunCharacterScripts(LAY_OR_WHATEVER,
                                    ScriptCallback_RunAllScripts,
                                    "",
                                    "Player selected 'LAY' (or whatever)");
    result = pClass->RunClassScripts(LAY_OR_WHATEVER,
                                     ScriptCallback_RunAllScripts,
                                     "",
                                     "Player selected 'LAY' (or whatever)");
    ClearCharContext();

    if (result == "CastSpell")
    {
      SPELL_DATA  *pSpell;
      SPELL_ID spellID;
      //spellName = hookParameters[1];
      spellID = hookParameters[1];
      pSpell = spellData.GetSpell(spellID);
      if (pSpell == NULL)
      {
        if (!debugStrings.AlreadyNoted(CString("LOWRISN")))
        {
          writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
          WriteDebugString("LayOrWhatever returned illegal spell name");
        };
      }
      else
      {
        if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType()== Combat))
        {
          TaskMessage(TASKMSG_CharacterCastSpell);
        }
        else
        {
          PushEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(),
                                                        m_pOrigEvent,
                                                        &GetActiveChar(m_pOrigEvent), //&FakeCharacter, //
                                                        //pSpell->m_gsID),
                                                        pSpell->SpellID(),
                                                        true),
                                                            DeleteEvent);
          return;
        };
      };
      currPic.Clear();
      //GraphicsMgr.EnableFontColorTags(TRUE);
      PopEvent();
      break;
    };

    CheckLayOrWhatever();
    OnUpdateUI();
    Invalidate();
    
  };
#else
    {
      ActorType actor;
      CHARACTER& dude = GetActiveChar(m_pOrigEvent);
      dude.GetContext(&actor);
      SetCharContext(&actor);
      SetTargetContext(&actor);

      traitType tdata = traitData.FindName("heal by laying on hands");
      double result=0.0;
      traitData.EvaluteTrait(tdata, result);

      if (result > 0.0)
        dude.SetHitPoints( dude.GetHitPoints() + result );

      // once per day
      dude.HasLayedOnHandsToday=TRUE;

      ClearTargetContext();
      ClearCharContext();
    }
    break;
#endif
    /*
    CURE is another healing ability of paladins--they may perform one Cure Disease
    per week for every 5 levels.  For example, at 1st to 5th
    levels, a paladin may perform one, at 6th to 10th levels two, etc.  This command
    is only displayed if the paladin has a cure available.


    21 November 2010...PRS .....All of this has been moved to the Special Abilities Hooks.
    We call class/character hook CAN_CURE_OR_WHATEVER to decide what if anything to display 
    as a menu item   Then, when the itm is selected, we call CURE_OR_WHATEVER to do
    any actual actions that are required.
    */
  case 6: // CureOrWhatever
  {
    CString result;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    ActorType actor;

    CHARACTER *pCharacter;
    CLASS_DATA *pClass;
    //classType classtype;
    CLASS_ID classID;
    pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
    classID = pCharacter->GetClass();
    pClass = classData.GetClass(classID);

    pCharacter->GetContext(&actor);
    SetCharContext(&actor);
    scriptContext.SetCharacterContext(pCharacter);
    scriptContext.SetClassContext(pClass);
    pCharacter->RunCharacterScripts(CURE_OR_WHATEVER,
                                    ScriptCallback_RunAllScripts,
                                    "",
                                    "Player selected 'CURE' (or whatever)");
    result = pClass->RunClassScripts(CURE_OR_WHATEVER,
                                     ScriptCallback_RunAllScripts,
                                     "",
                                     "Player selected 'CURE' (or whatever)");
    ClearCharContext();
    if (result == "CastSpell")
    {
      SPELL_DATA  *pSpell;
      SPELL_ID spellID;
      //spellName = hookParameters[1];
      spellID = hookParameters[1];
      pSpell = spellData.GetSpell(spellID);
      if (pSpell == NULL)
      {
        if (!debugStrings.AlreadyNoted(CString("COWRISN")))
        {
          writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
          WriteDebugString("CureOrWhatever returned illegal spell name");
        };
      }
      else
      {
        if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType()== Combat))
        {
          TaskMessage(TASKMSG_CharacterCastSpell);
        }
        else
        {
          PushEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(),
                                                        m_pOrigEvent,
                                                        &GetActiveChar(m_pOrigEvent), 
                                                        //pSpell->m_gsID),
                                                        pSpell->SpellID(),
                                                        true),
                                                            DeleteEvent);
          return;
        };
      };
      currPic.Clear();
      //GraphicsMgr.EnableFontColorTags(TRUE);
      PopEvent();
      break;
    };

    CheckCureOrWhatever();
    OnUpdateUI();
    Invalidate();
    
  };
  
    break;


  case 7: // Examine chain
    {
      GameEvent *pEvent = globalData.eventData.GetEvent(GetActiveChar(m_pOrigEvent,party.activeCharacter).ExamineEvent);
      if ((pEvent==NULL)||(!globalData.eventData.IsValidEvent(pEvent)))
      {
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      else 
      {
        if (AllowedAsGlobalEvent(pEvent->GetEventType()))
          PushGlobalEvent(pEvent, LevelEvent);
        else
          PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
    }
    break; 
  case 8: // Exit
    currPic.Clear();
    //GraphicsMgr.EnableFontColorTags(TRUE);
    PopEvent();
    break;
  };
}

void VIEW_CHARACTER_DATA::OnDraw(void)
{
  if (m_pOrigEvent!=NULL)
  {
    switch (m_pOrigEvent->GetEventType())
    {
    case Combat:
      UpdateCombatViewCharacterScreen();
      break;
    default:      
      UpdateViewCharacterScreen();
      break;
    }
  }
  else
    UpdateViewCharacterScreen();
}

void VIEW_CHARACTER_DATA::OnUpdateUI(void)
{

  CHARACTER *pCharacter;
  //classType classtype;
  CLASS_ID classID;
  CLASS_DATA *pClass;
  pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
  classID = pCharacter->GetClass();
  pClass = classData.GetClass(classID);
  if (CheckForMultiBoxText()) return;

  /*
  1   { "ITEMS", 0 },  
  2   { "DISPLAY", 0 },
  3   { "TRADE", 0 },  // always disabled, goto ITEMS menu for this
  4   { "DROP", 0 },   // always disabled, goto ITEMS menu for this
  5   { "LAY", 0 },
  6   { "CURE", 0 },
  7   { "EXAMINE", 0 },
  8   { "EXIT", 1 }

  in shop:
  in combat:
    always disable 5,6
  */




  // check conditional menu options
  menu.setItemInactive(4); // drop
  menu.setItemInactive(3); // trade

#ifdef NEW_LAY
  {
    if (m_layOrWhateverMenuText.IsEmpty())
    {
      menu.setItemInactive(5); // lay or whatever
    }
    else
    {
      menu.changeMenuItem(5,m_layOrWhateverMenuText, m_layOrWhateverShortcut);
    };
  };
#else
  if (!isPaladin(classtype))
  {
    menu.setItemInactive(5); // lay on hands to heal
    //menu.setItemInactive(6); // cure disease
  }
  else
  {
    if (GetActiveChar(m_pOrigEvent).HasLayedOnHandsToday)
      menu.setItemInactive(5); // lay
    else if (GetActiveChar(m_pOrigEvent).GetStatus() != Okay)
      menu.setItemInactive(5); // lay


    // disable this until it can actually do something
    //menu.setItemInactive(6); // cure disease
  }
#endif
  if (m_cureOrWhateverMenuText.IsEmpty())
  {
    menu.setItemInactive(6); // cure or whatever
  }
  else
  {
    menu.changeMenuItem(6,m_cureOrWhateverMenuText, m_cureOrWhateverShortcut);
  };


  // check unconditional menu disables
  if (m_pOrigEvent!=NULL)
  {
    switch (m_pOrigEvent->GetEventType())
    {
    case ShopEvent:
    case Combat:
      //menu.setItemInactive(5); // lay
      //menu.setItemInactive(6); // cure disease
      break;
    default:      
      break;
    }
  }

  if (   (!globalData.eventData.IsValidEvent(GetActiveChar(m_pOrigEvent,party.activeCharacter).ExamineEvent))
      || (GetActiveChar(m_pOrigEvent,party.activeCharacter).ExamineLabel.GetLength()==0))
    menu.setItemInactive(7); // examine
}

void VIEW_CHARACTER_DATA::OnReturnToTopOfQueue(void)
{
  OnInitialEvent();
}

////////////////////////////////////////////////////////////ITEMS_MENU_DATA

void ITEMS_MENU_DATA::OnInitialEvent(void)
{
  CHARACTER *pCharacter;
  ClearFormattedText(textData);
  party.tradeQty = 0;
  menu.setMenu(ItemsMenuData, NULL, FALSE, this, "items");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 14);
  pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
  setCharItemTextState(*pCharacter, m_pFilter, m_pFilterPkt, m_flags);
  SetMyState(TASK_initialized);
  handleItemsFormInput(ITEMSFORM_exit, charItemListText);
  handleItemsFormInput(ITEMSFORM_initial, charItemListText);



  Invalidate();
}

void ITEMS_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseMenuInventory(mx, my);
}

/*   
       1   =   "READY" or "UNREADY"
       2   =   "USE"
       3   =   "TRADE"
       4   =   "DROP"
       5   =   "DEPOSIT"
       6   =   "HALVE"
       7   =   "JOIN"
       8   =   "SELL"   or "SCRIBE"
       9   =   "ID ITEM"
       10  =   "EXAMINE"
       11  =   "EXAMINE"
       12  =   "NEXT"
       13  =   "PREV"
       14  =   "EXIT"
*/

void ITEMS_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVInventoryKeyboardAction(key, ascii, prevCharItemsPage,  nextCharItemsPage);
    return;
  }
  int itemTextIndex = currCharItemsTextIndex();

  CHARACTER &dude = GetActiveChar(m_pOrigEvent,party.activeCharacter);

  switch (menu.currentItem()) 
  {
  case 1: // "READY"
    if (itemCanBeReadied(charItemListText.GetItemClass(itemTextIndex)))
    {
      // 'itemsModified' will detect if the hooks associated with
      // Ready/Unready change the list of items being carried.
      dude.myItems.Modified(false);
      if ((m_pOrigEvent!=NULL) && (m_pOrigEvent->GetEventType()==Combat))
        combatData.toggleReadyItem(charItemListText.GetCharIndex(itemTextIndex));
      else
        dude.toggleReadyItem(charItemListText.GetCharIndex(itemTextIndex));
      charItemListText.
        UpdateIsReady(itemTextIndex, dude.myItems.IsReady(charItemListText.GetCharIndex(itemTextIndex)));
      if (dude.myItems.IsModified())
      {
        // Rebuild the list.
        setCharItemTextState(dude, NULL, NULL, 0);
      };
    }
    break;
    
  case 2: // "USE"
    {
      ITEM data;
      int i = charItemListText.GetCharIndex(itemTextIndex);
      dude.myItems.GetItem(i ,data);

      // decrement available charges
      int ch = dude.myItems.GetCharges(i)-1;
      ch = max(0,ch);
      dude.myItems.SetCharges(i, ch);

      //ITEM_DATA *idata = itemData.GetItemData(data.m_giID);
      ITEM_DATA *idata = itemData.GetItem(data.itemID);
      if (idata!=NULL)
      {
        //if (idata->m_gsID.IsValidSpell())
        if (idata->spellID.IsValidSpell())
        {
          if ((m_pOrigEvent!=NULL) && (m_pOrigEvent->GetEventType()==Combat))
          {
            //combatData.getActiveCombatCharacter().SetItemSpellKeyBeingCast(idata->m_gsID);
            combatData.getActiveCombatCharacter().SetItemSpellIDBeingCast(idata->spellID);
            //dude.InvokeSpellOnTarget(dude, -1, idata->Spell);
            PopEvent();
            break;
          }


          if (m_callerEventType == /*(eventType) */CONTROL_ViewCharacter)
          {
            //ReplaceEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(), m_pOrigEvent, &party.getActiveChar(), idata->m_gsID), DeleteEvent);
            ReplaceEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(), 
                                                             m_pOrigEvent, 
                                                             &party.getActiveChar(), 
                                                             idata->spellID,
                                                             false), 
                                                                      DeleteEvent);
            break;    
          };
        }




        
        if (!globalData.eventData.IsValidEvent(idata->m_useEvent))
        {
          PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
        }
        else 
        {
          if (GetRunAsVersion(ItemUseEventVersion) >= _VERSION_0681_)
          {
            GameEvent *pEvent = globalData.eventData.GetEvent(idata->m_useEvent);
            if ((pEvent != NULL) && (AllowedAsGlobalEvent(pEvent->GetEventType())))
              PushGlobalEvent(idata->m_useEvent);
            else
              PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          }
          else // not safe, didn't work properly in old versions
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
        }
      }
    }
    break;

  case 3: // "TRADE"
    if (itemCanBeTraded(charItemListText.GetItemClass(itemTextIndex)))
      PushEvent(new TRADE_TAKER_SELECT_MENU_DATA(m_pOrigEvent),DeleteEvent);
    break;

  case 4: // "DROP"
    {
      int charIndex;
      charIndex = charItemListText.GetCharIndex(itemTextIndex);
      if (itemCanBeDropped(charItemListText.GetItemClass(itemTextIndex)))
      {
        if (!itemIsMoney(charItemListText.GetItemClass(itemTextIndex)))
        {
          if (!dude.myItems.IsReady(charIndex))
          {
            if ((m_pOrigEvent!=NULL) && (m_pOrigEvent->GetEventType()==Combat))
            {
              combatData.delCharacterItem(charIndex, 
                                          charItemListText.GetItemQty(itemTextIndex));
            }
            else
            {
              switch (charItemListText.GetItemClass(currCharItemsTextIndex()))
              {
              case itemType:
                {
                  /* Replaced with CELL_LEVEL_CONTENTS  20181216 PRS
                  DROPPED_ITEM droppedItem;
                  ASLENTRY *pASL;
                  int curLevel;
                  CString address, data;
                  curLevel = globalData.currLevel;
                  address.Format("%d~%d",party.Posx, party.Posy);
                  droppedItem.Copy(*dude.myItems.GetITEM(charIndex));
                  data = CString((char *)(&droppedItem), sizeof(droppedItem));
                  pASL = globalData.levelInfo.stats[curLevel].level_asl.Find(address);
                  if (pASL == NULL)
                  {
                    globalData.levelInfo.stats[curLevel].level_asl.Insert(address, data, ASLF_MODIFIED);
                  }
                  else
                  {
                    pASL->Value(pASL->Value() + data);
                  };
                  */
                  CELL_ITEM *pDroppedItem;
                  int curLevel;
                  pDroppedItem = new CELL_ITEM;
                  *pDroppedItem = dude.myItems.GetITEM(charIndex);
                  curLevel = globalData.currLevel;
                  globalData.levelInfo.stats[curLevel].m_cellContents.Add(pDroppedItem, party.Posx, party.Posy);
                  dude.delCharacterItem(charIndex, 
                                        charItemListText.GetItemQty(itemTextIndex));
                };
                break;
              case SpecialItemType:
                {
                  if (party.hasSpecialItem(charIndex))
                    party.removeSpecialItem(charIndex);
                }
                break;
              case SpecialKeyType:
                {
                  if (party.hasSpecialKey(charIndex))
                    party.removeSpecialKey(charIndex);
                }
                break;
              }
            }
            // rebuild item list
            setCharItemTextState(dude, NULL, NULL, 0);
          }
          else
            miscError = ItemIsReadied;
        }
        else 
        {
          PushEvent(new GET_MONEY_QTY_DATA(GetEventType(),m_pOrigEvent),DeleteEvent);
          return;
        }
      }
    };
    break;

  case 5: // "DEPOSIT" (vault only)
    ASSERT( m_pOrigEvent->GetEventType() == Vault );
    
    if (itemCanBeDeposited(charItemListText.GetItemClass(itemTextIndex)))
    {
      if (!itemIsMoney(charItemListText.GetItemClass(itemTextIndex)))
      {
        if (!dude.myItems.IsReady(charItemListText.GetCharIndex(itemTextIndex)))
        {
          ITEM data;
          dude.myItems.GetItem(charItemListText.GetCharIndex(itemTextIndex) ,data);
          globalData.AddItemToVault(((VAULT_EVENT_DATA*)m_pOrigEvent)->WhichVault, data);
          dude.delCharacterItem(charItemListText.GetCharIndex(itemTextIndex), 
                           charItemListText.GetItemQty(itemTextIndex));
          // rebuild item list
          setCharItemTextState(dude, NULL, NULL, 0);
        }
        else
          miscError = ItemIsReadied;
      }
      else 
      {
        PushEvent(new GET_MONEY_QTY_DATA(GetEventType(),m_pOrigEvent),DeleteEvent);
        return;
      }
    }
    break;

  case 6: // "HALVE"
    if (charItemListText.ItemCanBeHalved(itemTextIndex))
    {
      dude.myItems.halveItem(charItemListText.GetCharIndex(itemTextIndex));   
      // rebuild item list
      setCharItemTextState(dude, NULL, NULL, 0);
    }
    break;

  case 7: // "JOIN"
    if (charItemListText.ItemCanBeJoined(itemTextIndex))
    {
       dude.myItems.joinItems(charItemListText.GetCharIndex(itemTextIndex));
       // rebuild item list
       setCharItemTextState(dude, NULL, NULL, 0);
    }
    break;

  case 8: // "SELL" (Or "SCRIBE" if CONTROL_ScribeMenu)
    if (m_pOrigEvent != NULL)
    {
      switch (m_pOrigEvent->GetEventType())
      {
      case ShopEvent:
        {
          SHOP *pData = (SHOP*)m_pOrigEvent;
          if (pData->buybackPercentage > 0)
          {          
            int i = charItemListText.GetCharIndex(itemTextIndex);          
            int paid = dude.myItems.GetPurchasePrice(i);
            ITEM data;
            dude.myItems.GetItem(i ,data);
  
            if (paid < 0)
              //paid = itemData.GetItemCost(data.m_giID);
              paid = itemData.GetItemCost(data.itemID);

            //int bqty = itemData.GetItemBundleQty(data.m_giID);
            int bqty = itemData.GetItemBundleQty(data.itemID);
            double unitprice = (double)paid/(double)bqty;
            
            int worth = unitprice * (double)dude.myItems.GetQty(i);
  
            pData->sellprice = worth * (((double)pData->buybackPercentage)/100.0);
            if (pData->sellprice < 0) pData->sellprice=0;
            if (pData->sellprice > worth) pData->sellprice=worth;
            SetMyState(TASK_AskToSellItem);
            CString msg;
            msg.Format("I WILL GIVE YOU %i %s FOR THIS ITEM, SELL IT?",
                        pData->sellprice, globalData.moneyData.GetName(globalData.moneyData.GetDefaultType()));
            PushEvent(new ASK_YES_NO_MENU_DATA(msg,TRUE, CONTROL_ItemsMenu), DeleteEvent);
          };
        };
        break;
      case CONTROL_ScribeMenu:
        {
          ITEM data;
          SCRIBE_MENU_DATA *pData;
          int i;
          pData = (SCRIBE_MENU_DATA *)m_pOrigEvent;
          i = charItemListText.GetCharIndex(itemTextIndex);          
          dude.myItems.GetItem(i ,data);
          //pData->m_giID_scroll = data.m_giID;
          pData->m_itemID_scroll = data.itemID;

          // decrement available charges
          int ch = dude.myItems.GetCharges(i)-1;
          ch = max(0,ch);
          dude.myItems.SetCharges(i, ch);

          PopEvent();
          return;
        };
        break;
      default:
        die(0xab4ec);
      };
    };
    break;

  case 9: // "ID ITEM"
    if (m_bCallIdentifyHooks)
    {
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      CHARACTER *pDude;
      CString result;
      //GLOBAL_ITEM_ID giID;
      ITEM_ID itemID;
      ITEM_DATA *pItem;
      //giID = charItemListText.GetItemIndex(currCharItemsTextIndex());
      itemID = charItemListText.GetItemID(currCharItemsTextIndex());
      //pItem = itemData.GetItemData(giID);
      pItem = itemData.GetItem(itemID);
      scriptContext.SetItemContext(pItem);
      pDude = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
      scriptContext.SetCharacterContext(pDude);
      hookParameters[4].Format("%d", charItemListText.GetCharIndex(currCharItemsTextIndex()));
      hookParameters[5] = m_saveParameter5;
      result = pDude->RunCharacterScripts(
                                 IDENTIFY, 
                                 ScriptCallback_LookForChar, 
                                 "Y",
                                 "Player seleced 'ID Item'" );
      if (!result.IsEmpty())
      {
        int i = charItemListText.GetCharIndex(itemTextIndex);
        dude.myItems.SetId(i, TRUE);
        charItemListText.UpdateIdentified(currCharItemsTextIndex(), TRUE);
        Invalidate();
      };
      m_bCallIdentifyHooks = FALSE;
      break;
    };
    if (m_pOrigEvent != NULL)
    {
      ASSERT(m_pOrigEvent->GetEventType()==ShopEvent);
      if (m_pOrigEvent->GetEventType()==ShopEvent)
      {
        SHOP *pData = (SHOP*)m_pOrigEvent;
        if (pData->costToId > 0)
        {
          SetMyState(TASK_AskToIdItem);
          CString msg;
          if (GetActiveChar(m_pOrigEvent,party.activeCharacter).enoughMoney(pData->costToId,0,0,globalData.moneyData.GetDefaultType()))
          {
            msg.Format("IDENTIFICATION WILL COST %i %s, PAY IT?", pData->costToId, globalData.moneyData.GetName(globalData.moneyData.GetDefaultType()));
            PushEvent(new ASK_YES_NO_MENU_DATA(msg,TRUE, CONTROL_ItemsMenu), DeleteEvent);      
          }
          else
          {
            msg.Format("THE COST OF IDENTIFICATION IS %i %s\r\nYOU DO NOT HAVE ENOUGH", pData->costToId, globalData.moneyData.GetName(globalData.moneyData.GetDefaultType()));
            PushEvent(new ASK_YES_NO_MENU_DATA(msg,FALSE, CONTROL_ItemsMenu), DeleteEvent);
          }        
        }
        else
        {
          // id is free
          int i = charItemListText.GetCharIndex(itemTextIndex);
          dude.myItems.SetId(i, TRUE);
        }
      }
    };
    break;
  case 10: // "EXAMINE" chain

    
    if (!m_examineOrWhateverMenuText.IsEmpty())
    {
      // We will call the EXAMINE_OR_WHATEVER hook and ignore all the other
      // 'USE' options.
      HOOK_PARAMETERS hookParameters;
      ITEM data;
      SCRIPT_CONTEXT scriptContext;
      ActorType actor;

      CHARACTER *pCharacter;
      ITEM_DATA *pItem;
      CString result;
      //classType classtype;
      int i;
      i = charItemListText.GetCharIndex(itemTextIndex);
      dude.myItems.GetItem(i ,data);

      //ITEM_DATA *idata = itemData.GetItemData(data.m_giID);
      pItem = itemData.GetItem(data.itemID);

      
      pCharacter = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
      scriptContext.SetCharacterContext(pCharacter);
      scriptContext.SetItemContext(pItem);
      pCharacter->RunCharacterScripts(EXAMINE_OR_WHATEVER,
                                      ScriptCallback_RunAllScripts,
                                      "",
                                      "Player selected 'EXAMINE' (or whatever)");
      result = pItem->RunItemScripts(EXAMINE_OR_WHATEVER,
                                      ScriptCallback_RunAllScripts,
                                      "",
                                      "Player selected 'EXAMINE' (or whatever)");

      if (result == "CastSpell")
      {
        SPELL_DATA  *pSpell;
        SPELL_ID spellID;
        //spellName = hookParameters[1];
        spellID = hookParameters[1];
        pSpell = spellData.GetSpell(spellID);
        if (pSpell == NULL)
        {
          if (!debugStrings.AlreadyNoted(CString("LOWRISN")))
          {
            writeDebugDialog = WRITE_DEBUG_DIALOG_WARNING;
            WriteDebugString("LayOrWhatever returned illegal spell name");
          };  
        }
        else
        {
          if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType()== Combat))
          {
            TaskMessage(TASKMSG_CharacterCastSpell);
          }
          else
          {
            PushEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(),
                                                          m_pOrigEvent,
                                                          &GetActiveChar(m_pOrigEvent), //&FakeCharacter, //
                                                          //pSpell->m_gsID),
                                                          pSpell->SpellID(),
                                                          true),
                                                              DeleteEvent);
            return;
          };
        };
        currPic.Clear();
        //GraphicsMgr.EnableFontColorTags(TRUE);
        PopEvent();
        break;
      };
      setCharItemTextState(dude, NULL, NULL, 0);
      Invalidate();
    };
    
    {
      switch (charItemListText.GetItemClass(currCharItemsTextIndex()))
      {
        case itemType:
        {
          ITEM data;
          int i = charItemListText.GetCharIndex(itemTextIndex);
          dude.myItems.GetItem(i ,data);
          //ITEM_DATA *idata = itemData.GetItemData(data.m_giID);
          ITEM_DATA *idata = itemData.GetItem(data.itemID);
          if (idata!=NULL)
          {
            GameEvent *pEvent = globalData.eventData.GetEvent(idata->ExamineEvent);
            if ((pEvent==NULL)||(!globalData.eventData.IsValidEvent(pEvent)))
            {
              PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
            }
            else 
            {
              if (AllowedAsGlobalEvent(pEvent->GetEventType()))
                PushGlobalEvent(pEvent, LevelEvent);
              else
                PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
            }
          }
        }
        break;
      }
    }
    break; 
  case 11: // "EXAMINE" chain
    {
      switch (charItemListText.GetItemClass(currCharItemsTextIndex()))
      {
        case SpecialItemType:
        {
          int i = charItemListText.GetCharIndex(itemTextIndex);
          GameEvent *pEvent = globalData.eventData.GetEvent(globalData.specialItemData.GetExamineEvent(i));
          if ((pEvent==NULL)||(!globalData.eventData.IsValidEvent(pEvent)))
          {
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          }
          else 
          {
            if (AllowedAsGlobalEvent(pEvent->GetEventType()))
              PushGlobalEvent(pEvent, LevelEvent);
            else
              PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          }
        }
        break;

        case SpecialKeyType:
        {
          int i = charItemListText.GetCharIndex(itemTextIndex);
          GameEvent *pEvent = globalData.eventData.GetEvent(globalData.keyData.GetExamineEvent(i));
          if ((pEvent==NULL)||(!globalData.eventData.IsValidEvent(pEvent)))
          {
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          }
          else 
          {
            if (AllowedAsGlobalEvent(pEvent->GetEventType()))
              PushGlobalEvent(pEvent, LevelEvent);
            else
              PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          }
        }
        break;
      }
    }
    break; 
  case 12: // "NEXT"
    nextCharItemsPage();
    break;
  case 13: // "PREV"
    prevCharItemsPage();
    break;
  case 14: // "EXIT"
    clearCharItemState();
    handleItemsFormInput(ITEMSFORM_exit, charItemListText);
    PopEvent();
    return;
  }
}

void ITEMS_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  if (errorTextData.numLines == 0)
    handleItemsFormInput(ITEMSFORM_refresh, charItemListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void ITEMS_MENU_DATA::OnUpdateUI(void)
{
  int i;
  bool bCreateExamineShortcut = false;
  if (CheckForMultiBoxText()) return;

  /*
   1{ "READY", 0 },
   2{ "USE", 0 },
   3{ "TRADE", 0 },
   4{ "DROP", 0 },
   5{ "DEPOSIT", 2 },
   6{ "HALVE", 0 },
   7{ "JOIN", 0 },
   8{ "SELL", 0 },
   9{ "ID ITEM", 0 },
   10{ "EXAMINE", -1 }, // normal items
   11{ "EXAMINE", -1 }, // special items and keys
   12{ "NEXT", 0 },
   13{ "PREV", 0 },
   14{ "EXIT", 1 }
  */

  if (charItemListText.GetCount() <= 0)
  {
    menu.setAllItemsInactive();
    menu.setItemActive(14);  // "EXIT"
    return;
  }

  CHARACTER &dude = GetActiveChar(m_pOrigEvent,party.activeCharacter);
  i = charItemListText.GetCharIndex(currCharItemsTextIndex());

  if (m_pOrigEvent!=NULL)
  {
    switch (m_pOrigEvent->GetEventType())
    {
    case ShopEvent:
      {
        menu.setItemInactive(5); // deposit (only active when in a vault)
        
        if ( ((SHOP*)m_pOrigEvent)->buybackPercentage <= 0 )
          menu.setItemInactive(8); // sell (won't buy back items)

        if (dude.myItems.IsReady(i))
          menu.setItemInactive(8); // sell (must not be readied)

        if ( !((SHOP*)m_pOrigEvent)->canId )
          menu.setItemInactive(9); // id (not allowed for this shop)
        
        if (((SHOP*)m_pOrigEvent)->buyItemsSoldOnly)
        {
          if (!((SHOP*)m_pOrigEvent)->itemsAvail.HaveItem(dude.myItems.GetItem(i)))
            menu.setItemInactive(8); // sell (must not be readied)
        }        
      }
      break;
    case Vault:
      menu.setItemInactive(4); // drop (only active when *not* in a vault)
      menu.setItemInactive(8); // sell
      menu.setItemInactive(9); // id
      break;
    case CONTROL_ScribeMenu:
      {
        //menu.setItemInactive(1);  // Ready
        menu.setItemInactive(2);    //Use
        menu.setItemInactive(3);    //Trade
        menu.setItemInactive(4);    //Drop
        menu.setItemInactive(5);    //Deposit
        menu.setItemInactive(6);    //Halve
        menu.setItemInactive(7); // drop (only active when *not* in a vault)
        menu.changeMenuItem(8,"SCRIBE", 0); //Sell
        menu.setItemInactive(9);    //ID
        menu.setItemInactive(10);   //Examine
        menu.setItemInactive(11);   //Examine
        menu.setItemInactive(12);   //Next
      };
      break;
    default:
      menu.setItemInactive(5); // deposit (only active when in a vault)
      menu.setItemInactive(8); // sell
      menu.setItemInactive(9); // id
      break;
    }
  }
  else
  {
    menu.setItemInactive(5); // deposit (only active when in a vault)
    menu.setItemInactive(8); // sell    (only active when in a shop)
    menu.setItemInactive(9); // id      (only active when in a shop)
  }  

  if (!dude.IsAbleToTrade())
    menu.setItemInactive(3); // trade

  // item class checks
  if (!itemCanBeTraded(charItemListText.GetItemClass(currCharItemsTextIndex())))
    menu.setItemInactive(3); // trade  
  if (!itemCanBeDropped(charItemListText.GetItemClass(currCharItemsTextIndex())))
    menu.setItemInactive(4); // drop
  if (!itemCanBeDeposited(charItemListText.GetItemClass(currCharItemsTextIndex())))
    menu.setItemInactive(5); // deposit
  if (!itemCanBeSold(charItemListText.GetItemClass(currCharItemsTextIndex())))
    menu.setItemInactive(8); // sell
  {
  // specific item database checks
    itemClassType itemType;
    itemType = charItemListText.GetItemClass(currCharItemsTextIndex());
    if (!itemIsMoney(itemType))
    {
      if (!itemCanBeTraded(dude.myItems.GetItem(i)))
        menu.setItemInactive(3); // trade  
      if (!itemCanBeDropped(dude.myItems.GetItem(i)))
        menu.setItemInactive(4); // drop
    };
    if (!itemCanBeDeposited(dude.myItems.GetItem(i)))
      menu.setItemInactive(5); // deposit
    if (!itemCanBeSold(dude.myItems.GetItem(i)))
      menu.setItemInactive(8); // sell
  };  
  if (dude.myItems.GetCount() == 0)
  {
    menu.setItemInactive(8); // sell
    menu.setItemInactive(9); // id
  }


  m_bCallIdentifyHooks = FALSE;
  switch (charItemListText.GetItemClass(currCharItemsTextIndex()))
  {
  case itemType:
    {     
      //i = charItemListText.GetCharIndex(currCharItemsTextIndex());
      m_bCallIdentifyHooks = TRUE;
      ITEM_DATA *pdata = itemData.GetItem(dude.myItems.GetItem(i));
      ASSERT(pdata != NULL);
      if (    (m_pOrigEvent!=NULL)
           && (m_pOrigEvent->GetEventType() == CONTROL_ScribeMenu)  )
      {
        if (!dude.myItems.GetId(i))
        {
          menu.setItemInactive(8);  //Scribe
        };
      };

      if (dude.myItems.IsReady(i))
      {
        menu.changeMenuItem(1, "UNREADY", 2);
        menu.setItemInactive(3); // trade
        menu.setItemInactive(4); // drop

        if (!itemData.GetItemUsable(dude.myItems.GetItem(i)))      
        {
          menu.setItemInactive(2); // use 
        }
        else 
        {
          // older versions didn't store the charge count
          if (GetRunAsVersion(ItemUseEventVersion) >= _VERSION_0690_)
          {
            if (dude.myItems.GetCharges(i) <= 0)
              menu.setItemInactive(2); // use 
          }
        }
      }
      else
      {
        if ((pdata==NULL) || (pdata->Location_Readied != Cannot))
        {
          // not 'useable' if not readied
          menu.setItemInactive(2); // use
          menu.changeMenuItem(1, "READY", 0);
        }
        else
        {
          // item that does not need to be readied
          menu.setItemInactive(1); // ready
          
          if (!itemData.GetItemUsable(dude.myItems.GetItem(i)))
            menu.setItemInactive(2); // use
        }
      }

      if (itemData.GetItemBundleQty(dude.myItems.GetItem(i)) <= 1)
      {
        menu.setItemInactive(6); // halve
        menu.setItemInactive(7); // join
      } 
      if (!itemCanBeHalved(dude.myItems.GetItem(i)))
      {
        menu.setItemInactive(6); // halve
      };
      if (!itemCanBeJoined(dude.myItems.GetItem(i)))
      {
        menu.setItemInactive(7); // join
      };
      if (dude.myItems.GetQty(i) <= 1)
      {
        menu.setItemInactive(6); // halve
      }  
      if (dude.myItems.GetQty(i) >= MAX_ITEM_QTY)
      {
        menu.setItemInactive(7); // join
      } 
      if (!itemCanBeReadied(dude.myItems.GetItem(i)))
        menu.setItemInactive(1); // ready

      //ITEM_DATA *pdata = itemData.GetItemData(dude.myItems.GetItem(i));
      if (pdata != NULL)
      {






#define NEW_EXAMINE
#ifdef NEW_EXAMINE

        if (menu.isItemActive(10))
        {
          ITEM_ID itemID;
          ITEM_DATA *pItem;
          //giID = charItemListText.GetItemIndex(currCharItemsTextIndex());
          itemID = charItemListText.GetItemID(currCharItemsTextIndex());
          //pItem = itemData.GetItemData(giID);
          pItem = itemData.GetItem(itemID);
          if ( (pItem != NULL) && (!pItem->ExamineLabel.IsEmpty()))
          {
            HOOK_PARAMETERS hookParameters;
            SCRIPT_CONTEXT scriptContext;
            CHARACTER *pDude;
            CString result;
          //GLOBAL_ITEM_ID giID;
            scriptContext.SetItemContext(pItem);
            pDude = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
            scriptContext.SetCharacterContext(pDude);
            hookParameters[4].Format("%d", i);
            hookParameters[5] = pItem->ExamineLabel;
            hookParameters[6] = "";
            pDude->RunCharacterScripts(
                          CAN_EXAMINE_OR_WHATEVER,
                          ScriptCallback_RunAllScripts,
                          NULL,
                          "Should 'EXAMINE' (or whatever) menu item be displayed");
            result = pItem->RunItemScripts(
                          CAN_EXAMINE_OR_WHATEVER, 
                          ScriptCallback_RunAllScripts, 
                          NULL,
                          "Should 'EXAMINE' (or whatever) menu item be displayed");
            m_saveParameter5 = hookParameters[5];
            m_examineOrWhateverMenuText = hookParameters[5];
            if (hookParameters[6].IsEmpty())
            {
              hookParameters[6] = "-1";
              bCreateExamineShortcut = true;  // later, when all items have been activated.
            };
            sscanf(hookParameters[6],"%d",&m_examineOrWhateverShortcut);
            menu.changeMenuItem(10,m_examineOrWhateverMenuText, m_examineOrWhateverShortcut);
            if (!result.IsEmpty())
            {
              if (result[0] == 'Y')
              {
              }
              else
              {
                menu.setItemInactive(10);
              };
            //ClearCharContext();
            };
          }
          else
          {
            menu.setItemInactive(10);
          };
        };


#else
        if (   (!globalData.eventData.IsValidEvent(pdata->ExamineEvent))
            || (pdata->ExamineLabel.GetLength()==0))
        {
          menu.setItemInactive(10); // examine
        }
        else
        {
          CString examinetext(pdata->ExamineLabel);
          if (examinetext.GetLength()>0)
          {
            menu.changeMenuItem(10, examinetext);
            menu.SetFirstLettersShortcuts(TRUE);
          }
        }
#endif
      }
      else
        menu.setItemInactive(10); // examine
      



      menu.setItemInactive(11); // examine

      // cannot be used if charges are used up
      if (dude.myItems.GetCharges(i) <= 0)
        menu.setItemInactive(2); // use
    };
    break;
  case PlatinumType:
  case ElectrumType:
  case GoldType:
  case CopperType:
  case SilverType:
  case GemType:
  case JewelryType:
  case CoinType6:
  case CoinType7:
  case CoinType8:
  case CoinType9:
  case CoinType10:
    menu.setItemInactive(1); // ready
    menu.setItemInactive(2); // use
    menu.setItemInactive(6); // halve
    menu.setItemInactive(7); // join
    menu.setItemInactive(8); // sell
    menu.setItemInactive(9); // id
    menu.setItemInactive(10); // examine
    menu.setItemInactive(11); // examine
    break;
  case SpecialItemType:
    menu.setItemInactive(1); // ready
    menu.setItemInactive(2); // use
    menu.setItemInactive(3); // trade
    menu.setItemInactive(6); // halve
    menu.setItemInactive(7); // join
    menu.setItemInactive(8); // sell
    menu.setItemInactive(9); // id
    menu.setItemInactive(10); // examine
    if (!globalData.specialItemData.CanBeDropped(i))
      menu.setItemInactive(4); // drop

    if (   (!globalData.eventData.IsValidEvent(globalData.specialItemData.GetExamineEvent(i)))
        || (globalData.specialItemData.GetExamineLabel(i).GetLength()==0))
    {
      menu.setItemInactive(11); // examine
    }
    else
    {
      CString examinetext(globalData.specialItemData.GetExamineLabel(i));
      if (examinetext.GetLength()>0)
      {
        menu.changeMenuItem(11, examinetext);
        menu.SetFirstLettersShortcuts(TRUE);
      }
    }
    break;
  case SpecialKeyType:
    menu.setItemInactive(1); // ready
    menu.setItemInactive(2); // use
    menu.setItemInactive(3); // trade
    menu.setItemInactive(6); // halve
    menu.setItemInactive(7); // join
    menu.setItemInactive(8); // sell
    menu.setItemInactive(9); // id
    menu.setItemInactive(10); // examine
    if (!globalData.keyData.CanBeDropped(i))
      menu.setItemInactive(4); // drop

    if (   (!globalData.eventData.IsValidEvent(globalData.keyData.GetExamineEvent(i)))
        || (globalData.keyData.GetExamineLabel(i).GetLength()==0))
    {
      menu.setItemInactive(11); // examine
    }
    else
    {
      CString examinetext(globalData.keyData.GetExamineLabel(i));
      if (examinetext.GetLength()>0)
      {
        menu.changeMenuItem(11, examinetext);
        menu.SetFirstLettersShortcuts(TRUE);
      }
    }
    break;
  default: // empty item list
    menu.setItemInactive(1); // ready
    menu.setItemInactive(2); // use
    menu.setItemInactive(3); // trade
    menu.setItemInactive(4); // drop
    menu.setItemInactive(5); // deposit
    menu.setItemInactive(6); // halve
    menu.setItemInactive(7); // join
    menu.setItemInactive(8); // sell
    menu.setItemInactive(9); // id
    menu.setItemInactive(10); // examine
    menu.setItemInactive(11); // examine
    break;
  };

  if (menu.isItemActive(9))
  {
    m_bCallIdentifyHooks = FALSE;
  };
  if (dude.myItems.GetId(i)) 
  {
    m_bCallIdentifyHooks = FALSE;
  };
  if (m_bCallIdentifyHooks)
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CHARACTER *pDude;
    CString result;
    //GLOBAL_ITEM_ID giID;
    ITEM_ID itemID;
    ITEM_DATA *pItem;
    //giID = charItemListText.GetItemIndex(currCharItemsTextIndex());
    itemID = charItemListText.GetItemID(currCharItemsTextIndex());
    //pItem = itemData.GetItemData(giID);
    pItem = itemData.GetItem(itemID);
    scriptContext.SetItemContext(pItem);
    pDude = &GetActiveChar(m_pOrigEvent,party.activeCharacter);
    scriptContext.SetCharacterContext(pDude);
    hookParameters[4].Format("%d", i);
    result = pDude->RunCharacterScripts(
                       CAN_IDENTIFY, 
                       ScriptCallback_LookForChar,
                       "Y",
      "Should 'IDENTIFY' menu item be displayed");
    m_saveParameter5 = hookParameters[5];
    if (result.IsEmpty())
    {
      m_bCallIdentifyHooks = FALSE;
    }
    else
    {
      menu.setItemActive(9); // "ID ITEM"
    };
  };

  if (bCreateExamineShortcut)
  {
    menu.AttemptToCreateUniqueShortcut(10);
  };

  if (!allowCharItemNextPage())
    menu.setItemInactive(12); // next
  if (!allowCharItemPrevPage())
    menu.setItemInactive(13); // prev
}

void ITEMS_MENU_DATA::OnReturnToTopOfQueue(void)
{
  switch (GetMyState())
  {
  case TASK_AskToIdItem:
    {
      if (party.tradeItem==1) // yes, id the item
      {
        CHARACTER &dude = GetActiveChar(m_pOrigEvent,party.activeCharacter);
        SHOP *pData = (SHOP*)m_pOrigEvent;
        int i = charItemListText.GetCharIndex(currCharItemsTextIndex());
        dude.payForItem(pData->costToId, globalData.moneyData.GetDefaultType());
        dude.myItems.SetId(i,TRUE);
      }
    }
    break;

  case TASK_AskToSellItem:
    {
      if (party.tradeItem==1) // yes, sell the item
      {
        CHARACTER &dude = GetActiveChar(m_pOrigEvent,party.activeCharacter);
        SHOP *pData = (SHOP*)m_pOrigEvent;
        int i = charItemListText.GetCharIndex(currCharItemsTextIndex());
        dude.money.Add(globalData.moneyData.GetDefaultType(), pData->sellprice);
        dude.myItems.DeleteItem(i);
      }
    }
    break;

  default:
    {
      if (party.tradeQty > 0)
      {
        // either dropping money or depositing it in the vault
        // and you can't drop in a vault
        if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType() == Vault))
        {
          // add money to vault
          globalData.UpdateMoneyInVault(((VAULT_EVENT_DATA*)m_pOrigEvent)->WhichVault,
                                        charItemListText.GetItemClass(currCharItemsTextIndex()),
                                        party.tradeQty);                           
        }
        GetActiveChar(m_pOrigEvent,party.activeCharacter).removeCharMoney(charItemListText.GetItemClass(currCharItemsTextIndex()), party.tradeQty);
        party.tradeQty=0;
      }
    }
    break;
  }

  OnInitialEvent();
}

////////////////////////////////////////////////////////////GET_MONEY_QTY_DATA
void GET_MONEY_QTY_DATA::OnKeypress(key_code key, char ascii)
{
  if (   (key != KC_RETURN)
      && (key != KC_NUM)
      && (key != KC_BACK))
  {
    return;
  };
  char temp[10], temp2[10];
  int i, sum; 
  
  if (key == KC_RETURN)
  {
    temp[0] = '\0';
    if (menu.totalItems() > 0)
    {
      for (i=1;i<menu.totalItems();i++)
        strcat(temp, menu.getMenuItem(i));      
      sum = atoi(temp); 
      party.tradeQty = sum;
      PopEvent();
    };
  }
  else 
  {
    int qty;
    switch (m_CallerEventType) 
    {
    case CONTROL_TakeTreasure:
      qty = treasureItemListText.GetItemQty(currTreasureTextIndex());
      break;
    case CONTROL_ItemsMenu:
    case CONTROL_TradeTakerSelect:
//    case GS_CombatItemsMenu:
      qty = charItemListText.GetItemQty(currCharItemsTextIndex());
      break;
    default:
      WriteDebugString("Unhandled game state in GET_MONEY_QTY_DATA()\n");
      miscError = InvalidGameState;
      qty = 0;
    };
    if (key == KC_NUM)          
    {
      temp[0] = '\0';
      
      // ignore last char, it is a blank
      for (i=1;i<menu.totalItems();i++)
        strcat(temp, menu.getMenuItem(i));
      
      sprintf(temp2, "%c", ascii);
      strcat(temp, temp2);
      sum = atoi(temp);
      
      if ((sum > qty) || (sum < 0))
      {
        i=menu.totalItems();
        
        while (i > 0)
        {
          menu.deleteLastMenuItem();
          i--;
        }
        
        itoa(qty, temp, 10);
        
        for (i=0;i<(int)strlen(temp);i++)
        {
          strncpy(temp2, &temp[i], 1);
          temp2[1] = '\0';
          menu.addMenuItem(temp2);
        }
        
        menu.addMenuItem(" ");
        menu.setCurrentItem(menu.totalItems());
      }
      else 
      {
        if (menu.totalItems() == 0)
        {
          menu.addMenuItem(temp2);
          menu.addMenuItem(" ");
        }
        else 
        {
          menu.changeMenuItem(menu.totalItems(), temp2);
          menu.addMenuItem(" ");
        }
        menu.setCurrentItem(menu.totalItems());
      };

    }
    else if (   ((key == KC_BACK) || (key == KC_LEFT))
             && (menu.totalItems() > 0) )
    {
      menu.deleteLastMenuItem();
      menu.changeMenuItem(menu.totalItems(), " ");      
      if (menu.totalItems() == 0)
        menu.addMenuItem(" ");      
      menu.setCurrentItem(menu.totalItems());
    };
  };
}

void GET_MONEY_QTY_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  party.tradeQty = 0;
  menu.setMenu(GetMoneyQtyMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.setItemSeparation(0);
  itemClassType theType;
  switch (m_CallerEventType) 
  {
  case CONTROL_TakeTreasure:
    theType = treasureItemListText.GetItemClass(currTreasureTextIndex());
    break;
  case CONTROL_TradeTakerSelect:
  case CONTROL_ItemsMenu:
    theType = charItemListText.GetItemClass(currCharItemsTextIndex());
    break;
  default:
    theType = itemType;
    WriteDebugString("Unhandled state in GET_MONEY_QTY_DATA\n");
    miscError = InvalidGameState;
    break;
  };
  CString tmp;
  switch (theType) 
  {
  case PlatinumType:
  case ElectrumType:
  case GoldType:
  case SilverType:
  case CopperType:
  case CoinType6:
  case CoinType7:
  case CoinType8:
  case CoinType9:
  case CoinType10:
  case GemType:
  case JewelryType:
    tmp.Format("HOW MANY %s?:", globalData.moneyData.GetName(theType));
    menu.setTitle(tmp);
    break;
  default:
    menu.setTitle("HOW MANY?:");
    break;
  };
  menu.addMenuItem(" ");
  menu.setCurrentItem(menu.totalItems());
  Invalidate();
}

void GET_MONEY_QTY_DATA::OnMouseClickLeft(int mx, int my)
{
  // no mouse input allowed
}

void GET_MONEY_QTY_DATA::OnDraw(void)
{
  switch (m_CallerEventType) 
  {
  case CONTROL_TakeTreasure:
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    drawBorder();
    if (errorTextData.numLines == 0)
      handleItemsFormInput(ITEMSFORM_refresh, treasureItemListText);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    break;
  case CONTROL_TradeTakerSelect:
  case CONTROL_ItemsMenu:
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    drawBorder();
    if (errorTextData.numLines == 0)
      handleItemsFormInput(ITEMSFORM_refresh, charItemListText);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    break;
  default:
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    drawBorder();
    if (errorTextData.numLines == 0)
      handleItemsFormInput(ITEMSFORM_refresh, charItemListText);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    break;
  }
}

//////////////////////////////////////////////////////////ENCAMP_MENU_DATA
void ENCAMP_MENU_DATA::OnInitialEvent(void)
{   
  //CString zapCmd;
  ASLENTRY *zapCmd;
  menu.setMenu(EncampMenuData, NULL, FALSE, this, "Encamp");
  menu.setHorzOrient();    
  SetMyState(TASK_initialized);

  
  zapCmd = globalData.global_asl.Find("_ZAPCMD_");
  m_zapCmd = FALSE;
  if (zapCmd != NULL)
  {
    int col;
    CString label, shortcut;
    col = zapCmd->Value().Find(',');
    if ((col > 0 && (col < zapCmd->Value().GetLength()-1)))
    {
      label = zapCmd->Value().Left(col);
      shortcut = zapCmd->Value().Right(zapCmd->Value().GetLength()-col-1);
      col = 0;
      sscanf(shortcut,"%d",&col);
      menu.changeMenuItem(10, label, col);
      m_zapCmd = TRUE;
    };
  };


  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 11);  

  // if using Camp event
  if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType() == Camp))
  {
    FormatDisplayText(textData, m_pOrigEvent->GetEventText());
    if (m_pOrigEvent->GetEventPic().picType != BogusDib)
    {
      currPic = m_pOrigEvent->GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
    }
    else
    {
      int zone = levelData.GetCurrZone(party.Posx, party.Posy);
      currPic = levelData.zoneData.zones[zone].campArt;
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
    };
  }
  else
  {
    ClearFormattedText(textData);
    // no event, just 'Encamp' menu option
    int zone = levelData.GetCurrZone(party.Posx, party.Posy);
    currPic = levelData.zoneData.zones[zone].campArt;
    currPic.LoadPicSurfaces("");
    //  20110527 PRS  currPic.LoadPicSurfaces("sp_");
  }

  PlayCampMusic();
  Invalidate();
}

void ENCAMP_MENU_DATA::OnDraw(void)
{
  UpdateEncampScreen('g');
}

void ENCAMP_MENU_DATA::OnUpdateUI(void)
{
  /*
1    { "SAVE",       0 },
2    { "LOAD",       0 },
3    { "VIEW",       0 },
4    { "MAGIC",      0 },
5    { "REST",       0 },
6    { "ALTER",      0 },
7    { "FIX",        0 },
8    { "TALK",       0 },
9    { "JOURNAL",    0 },
10   { "ZAP",        0 },
10   { "EXIT",       1 },
11   { "QUIT",       0 }
  */


  if (CheckForMultiBoxText()) return;
  CHARACTER& dude = GetActiveChar(m_pOrigEvent);  

  if (!m_zapCmd)
  {
    menu.setItemInactive(10); // "ZAP"
  };

  if (!levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].AllowMagic)
    menu.setItemInactive(4); // magic
  
  //if (!dude.canCastSpells()) 
    //menu.setItemInactive(4); // magic  

  int zone = levelData.GetCurrZone(party.Posx, party.Posy);
  if (!levelData.zoneData.zones[zone].restEvent.allowResting)
  {
    if (m_pOrigEvent == NULL)
    {
      menu.setItemInactive(5); // rest
    };
    menu.setItemInactive(7); // fix
  }

  if (   (!globalData.eventData.IsValidEvent(dude.TalkEvent))
      || (dude.TalkLabel.GetLength()==0))
  {
    menu.setItemInactive(8); // talk
  }
  else
  {
    menu.changeMenuItem(8,dude.TalkLabel);
    // try to give custom talk menu text a shortcut
    menu.SetFirstLettersShortcuts(TRUE);
  }

  if ((dude.DisableTalkIfDead)&&(dude.GetAdjStatus()!=Okay))
    menu.setItemInactive(8); // talk

  if (party.journal.GetCount() == 0)
    menu.setItemInactive(9); // journal

  if (m_pOrigEvent!=NULL)
  {
    if (m_pOrigEvent->IsGlobalEvent())
      menu.setItemInactive(1); // save
  }
  if (IsGlobalEvent())
  {
    menu.setItemInactive(1); // save
    menu.setItemInactive(2); // load
  }
  if (IsCombatActive())
  {
    menu.setItemInactive(1); // save
    menu.setItemInactive(2); // load
  }
}

void ENCAMP_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  };

  /*
1    { "SAVE",       0 },
2    { "LOAD",       0 },
3    { "VIEW",       0 },
4    { "MAGIC",      0 },
5    { "REST",       0 },
6    { "ALTER",      0 },
7    { "FIX",        0 },
8    { "TALK",       0 },
9    { "JOURNAL",    0 },
10   { "ZAP",        0 },
10   { "EXIT",       1 },
11   { "QUIT",       0 }
  */
  switch (menu.currentItem()) 
  {
  case 1: // Save
    PushEvent(new SAVEGAME_MENU_DATA, DeleteEvent);
    break;      
  case 2: // Load
    PushEvent(new LOADGAME_MENU_DATA, DeleteEvent);
    break;      
  case 3: // View
    PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),NULL), DeleteEvent);
    break;
  case 4: // Magic
    PushEvent(new MAGIC_MENU_DATA(m_pOrigEvent), DeleteEvent);
    break;
  case 5: // Rest
    PushEvent(new REST_MENU_DATA(m_pOrigEvent), DeleteEvent);
    break;
  case 6: // Alter    
    PushEvent(new ALTER_GAME_MENU_DATA(), DeleteEvent);
    break;
  case 7: // Fix (using party spells)
    party.FixParty(0);
    break;
  case 8: // Talk chain
    if (!globalData.eventData.IsValidEvent(GetActiveChar(m_pOrigEvent).TalkEvent))
    {
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    else 
    {
      if (GetRunAsVersion(ItemUseEventVersion) >= _VERSION_0681_)
      {
        GameEvent *pEvent = globalData.eventData.GetEvent(GetActiveChar(m_pOrigEvent).TalkEvent);
        if ((pEvent != NULL) && (AllowedAsGlobalEvent(pEvent->GetEventType())))
          PushGlobalEvent(GetActiveChar(m_pOrigEvent).TalkEvent);
        else
          PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      else // not safe, didn't work properly in old versions
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    break; 
  case 9: // journal
    PushEvent( new DISPLAY_PARTY_JOURNAL_DATA(), DeleteEvent);
    break;

  case 10: // "ZAP"
    {
      TEXT_EVENT_DATA *pTextEvent;
      pTextEvent=new TEXT_EVENT_DATA(false);
      pTextEvent->GetEventText()="**SHAZAM**ZapCmd()";
      //textEvent->pic = pic;
      //textEvent->HighlightText=FALSE;
      PushEvent(pTextEvent,DeleteEvent);
    };
    break;
  case 11: // Exit menu
    {
      currPic.Clear();
    
      PlayCampMusic(FALSE);// if camp music playing, stop it

      if (WaitUntilReturnPressed()) ClearFormattedText(textData);
      PopEvent(); // CAMP_EVENT pushes ENCAMP and calls ChainHappened() upon return

      // restart background music if available
      StartBackgroundMusic();
    }
    break;

  case 12: // quit UAF
    SetMyState(TASK_AskQuitGame);
    PushEvent(new ASK_YES_NO_MENU_DATA("QUIT THE GAME?",TRUE, CONTROL_Encamp), DeleteEvent);
    //ReplaceEvent(new EXIT_DATA, DeleteEvent);
    break;
  };
}

void ENCAMP_MENU_DATA::OnReturnToTopOfQueue()
{
  if (GetMyState()==TASK_AskQuitGame)
  {
    if (party.tradeItem==1) // yes, quit the game
    {
      ReplaceEvent(new EXIT_DATA, DeleteEvent);
      return;
    } 
  }

  OnInitialEvent();
}

int ENCAMP_MENU_DATA::OnSaveGame(unsigned int *saveArea)
{
  if (m_pOrigEvent==NULL)
  {
    saveArea[0]=0;
  }
  else
  {
    saveArea[0]=m_pOrigEvent->GetEventId();
  };
  return 1;
}

int ENCAMP_MENU_DATA::OnLoadGame(unsigned int *saveArea)
{
  if (saveArea[0]==0) m_pOrigEvent=NULL;
  else m_pOrigEvent=levelData.eventData.GetEvent(saveArea[0]);
  return 1;
}

//////////////////////////////////////////////////////////TRANSFER_EVENT_DATA


void TRANSFER_EVENT_DATA::OnInitialEvent(void)
{
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");

  if (askYesNo)
  {
    menu.setMenu(TransferMenuData, NULL, FALSE, this, "Transfer");
    menu.setHorzOrient();
    SetMyState(TASK_TransferMenu);
    Invalidate();
  }
  else
  {
    //ClearFormattedText(textData);
    if (textData.numLines==0)
    {
      if (currPic.IsPicLoaded())
      {
        menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "Transfer");
        menu.setHorzOrient();
        SetMyState(TASK_TransferGo);
        if (!activateBeforeEntry)
        {
          Invalidate();
        };
      }
      else
      {
        menu.reset(NULL);
        SetMyState(TASK_TransferGo);
        if (!activateBeforeEntry)
        {
          Invalidate();
        };
      };
    }
    else 
    {
      if (currPic.IsPicLoaded())
      {
        menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
        menu.setHorzOrient();
        SetMyState(TASK_TransferGo);
        //if (!activateBeforeEntry)
        {
          Invalidate();
        };
      }
      else
      {
        menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
        menu.setHorzOrient();
        SetMyState(TASK_TransferGo);
        Invalidate();
      }
    };
  };
}

bool TRANSFER_EVENT_DATA::OnTestTrigger(bool allowForwardStep)
{
  if (!GameEvent::OnTestTrigger(false))
  {
    return false;
  };
  if (!activateBeforeEntry)
  {
    TaskMessage(TASKMSG_AllowForwardStep);
  }
  return true;
}

void TRANSFER_EVENT_DATA::OnDraw(void)
{
  if (activateBeforeEntry)
  {
    // PRS 20120529
    // This 'if' statement was commented-out.  Why?  I don't know.
        // PRS 20120530
        // Now I think I know.
        // GetMyState() updates the TASKLIST hashcode.  'OnDraw is not supposed
        // to do that.  So we need to supress the 'Invalidate Call' appropriately.

    // When the Update was called unconditionally it caused the
    // scene at the location of the transfer module to be displayed
    // befre the transfer took place.  But the transfer was supposed
    // to happen BEFORE entering the square.  So the player should 
    // never see the square containing the transfer.
    // So I put the 'if' back in.  Perhaps we will need a new
    // state to make everything work right under all circumstances.
    // This was changed to accomdate $SET_PARTY_XY when a transfer
    // was the first event and the player should not see the 
    // cell containing the transfer.
    //if (GetMyState() != TASK_TransferGo)
    //if (GetMyState() != TASK_TransferGo)
      UpdateTransferMenuScreen('f');
  }
  else
  {
    UpdateTransferMenuScreen('f');
  }
}

void TRANSFER_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  };
  switch (GetMyState())
  {
  case TASK_TransferGo:
    HandleTransfer(transferData, GetEventText2(), currPic, TRANSFER_MODULE, destroyDrow, true);
    break;

  case TASK_TransferMenu:
    currPic.Clear();    
    ClearFormattedText(textData);
    switch (menu.currentItem()) 
    {
    case 1: // Yes
      if (transferOnYes)
      {
        SetMyState(TASK_TransferGo);
      }
      else
      {
        SetMyState(TASK_TransferStay);
      };
      break;
    case 2: // No
      if (!transferOnYes)
      {
        SetMyState(TASK_TransferGo);
      }
      else
      {
        SetMyState(TASK_TransferStay);
      };
      break;
    }; // Case MenuItem()
    break;

    default:
      break;
  };
}

bool TRANSFER_EVENT_DATA::OnIdle(void)
{
//  int newLevel, oldLevel;
  switch (GetMyState())
  {
  case TASK_TransferGo:
    if (textData.numLines==0)
    {
      HandleTransfer(transferData,
                     GetEventText2(), 
                     currPic, 
                     TRANSFER_MODULE, 
                     destroyDrow, 
                     !activateBeforeEntry);
      return false;
    }
    break;

  case TASK_TransferStay:
    if (activateBeforeEntry)
    {
      TaskMessage(TASKMSG_CancelForwardStep);
    };
    ChainHappened();
    //PopEvent();
    return false;

  default:
    break;
  };
  return true;
}

unsigned int TRANSFER_EVENT_DATA::OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState)
{
  switch (msg)
  {
  case TASKMSG_TeleportWithinLevel:
  case TASKMSG_TeleportBetweenLevels:
    return TASKMSG_DELETE; // I'm outta here.
  default:
    break;
  };
  return 0;
}


////////////////////////////////////////////////////////////////////////COMBAT_TREASURE
void COMBAT_TREASURE::OnInitialEvent(void)
{
  setPartyXY_x = -1;
  setPartyXY_y = -1;
  menu.reset(NULL);
  //if ((m_flags & TTF_CUMULATIVE) == 0) tavernTales.Clear();
  //combatTreasure.Append(m_tales);
  const ITEM *pItem;
  POSITION pos;
  items.PeekFirst(pos);
  pItem = items.PeekNext(pos); 
  while (pItem!=NULL)
  {
    globalData.combatTreasure.items.AddItem(*pItem);
    pItem = items.PeekNext(pos);
  };
  globalData.combatTreasure.money.Add(money);
}


bool COMBAT_TREASURE::OnTestTrigger(bool allowForwardStep)
{
  return GameEvent::OnTestTrigger(false);
}

bool COMBAT_TREASURE::OnIdle(void)
{

  {
    //TaskMessage(TASKMSG_SetPartyXY);
    //PopEvent();
    ChainHappened();
    return false;
  };

}




////////////////////////////////////////////////////////////////////////TAVERN_TALES
void TAVERN_TALES::OnInitialEvent(void)
{
  setPartyXY_x = -1;
  setPartyXY_y = -1;
  menu.reset(NULL);
  if ((m_flags & TTF_CUMULATIVE) == 0) tavernTales.Clear();
  tavernTales.Append(m_tales);
}


bool TAVERN_TALES::OnTestTrigger(bool allowForwardStep)
{
  return GameEvent::OnTestTrigger(false);
}

bool TAVERN_TALES::OnIdle(void)
{
    //TaskMessage(TASKMSG_SetPartyXY);
    //PopEvent();
    ChainHappened();
    return false;
}



////////////////////////////////////////////////////////////////////////TAVERN
void TAVERN::OnInitialEvent(void)
{
  menu.setMenu(TavernMenuData, NULL, FALSE, this, "TAVERN");
 // menu.changeMenuText(&GetEventControl().eventcontrol_asl,"TAVERN");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 3);
  FormatDisplayText(textData, GetEventText());
  SetMyState(TASK_TavernMenu);
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");

  for (int i=0; i<MAX_TALES; i++)
  {
    if (!tales[i].tale.IsEmpty())
    {
      TAVERN_TALES_TALE tale;
      // Add our tales to the tavernTales list.
      tale.m_asl.Clear();
      tale.m_flags = 0;
      tale.m_tale = tales[i].tale;
      tavernTales.Add(tale);
    };
  };
  {
    int i, n;
    n = tavernTales.NumTale();
    for (i=0; i<n; i++)
    {
      tavernTales.GetTale(i)->count = 0;
    };
  };
  //I don't think count is ever used.
  //Moreover, i is not well-defined at this point.
  //tales[i].count = 0;
  Invalidate();
}

void TAVERN::OnLeaveQueue(void)
{
  tavernTales.Clear();
}

void TAVERN::OnDraw(void)
{
  switch (GetMyState())
  {
  case TASK_TavernDrink:
    UpdateTavernMenuScreen('e');
    break;
  case TASK_TavernMenu:
    UpdateTavernMenuScreen('e');
    //ClearFormattedText(textData);
    break;
  default:
    break;
  }; // switch (GetMyState())
}

void TAVERN::OnUpdateUI(void)
{
  int i;
  if (CheckForMultiBoxText()) return;
  switch (GetMyState())
  {
  case TASK_TavernDrink:
    for (i=0;i<MAX_DRINKS;i++)
    {
      if (drinks[i].name.GetLength() == 0)
        menu.setItemInactive(i+1);
    }
    break;
  case TASK_TavernMenu:
    if (!allowDrinks)
      menu.setItemInactive(2); // drinks
    if (!allowFights)
      menu.setItemInactive(1); // fights
    break;
  default:
    break;
  }; // switch (GetMyState())
}

BOOL IsUsableTale(const TAVERN_TALES_TALE *tale, BOOL OnceOnly)
{
  if (tale->m_tale.IsEmpty())
    return FALSE;

  if (OnceOnly)
  {
    if (tale->count > 0)
      return FALSE;
  }

  return TRUE;
}

void TAVERN::OnKeypress(key_code key, char ascii)
{
  static int LastRumor = -1;
  BOOL found;
  int i, index;

  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  };
  switch (GetMyState())
  {
  case TASK_TavernMenu:
    switch (menu.currentItem()-1) 
    {
    case 0: // Fight
      if (levelData.eventData.IsValidEvent(fightChain))
      {
        currPic.Clear();
        ClearFormattedText(textData);
        ReplaceEvent(levelData.eventData.GetEvent(fightChain), LevelEvent);
        return;
      }
      else
      {
        FormatDisplayText(textData, "Everyone runs away. There's no one to fight!");
      };
      break;
    case 1: // Drink
      found = FALSE;
      i=0;
      while ((i < MAX_DRINKS) && (!found))
      {
        if (drinks[i].name.GetLength() > 0)
          found = TRUE;
        i++;
      }
      
      if (found)
      {
        SetMyState(TASK_TavernDrink);
        menu.reset(NULL);
        menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
        menu.setHorzOrient();
        menu.setItemSeparation(7);
        FormatDisplayText(textData, "What is your pleasure?");
        currPic = GetEventPic();
        //currPic.LoadPicSurfaces("sp_");
        currPic.LoadPicSurfaces("");
        for (i=0;i<MAX_DRINKS;i++)
        {
          if (drinks[i].name.GetLength() > 0)
          {
            menu.addMenuItem(drinks[i].name);
          }
          else 
          {
            // need a placeholder to properly determine drink points
            // when handling menu selections. These empty drink names
            // will be disabled in OnUpdateUI
            int newItem = menu.addMenuItem(" ");
            menu.setItemInactive(newItem);
          }
        }
        menu.addMenuItem("DONE");

        menu.SetFirstLettersShortcuts();
        menu.setCurrentItem(0);        
        menu.MapKeyCodeToMenuItem(KC_ESCAPE, menu.totalItems());
      }
      break;
    case 2: // Talk
      if (taleOrder == InOrderTales)
      {
        int r;
        if (LastRumor == -1)
        {
          // search for non-empty tale
          r=0;
          //while ((r < MAX_TALES) && (!IsUsableTale(tales[r], EachTaleOnceOnly))) //(tales[r].tale.GetLength() == 0))
          while (      (r < tavernTales.NumTale()) 
                    && (!IsUsableTale(tavernTales.GetTale(r), EachTaleOnceOnly))) //(tales[r].tale.GetLength() == 0))
          {
            r++;
          };
          if (r < tavernTales.NumTale())
          {
            LastRumor = r;
            tavernTales.GetTale(r)->count++;
            FormatDisplayText(textData, tavernTales.PeekTale(r)->m_tale);
          }
        }
        else
        {
          r=LastRumor+1;
          BOOL StartedSearchWithFirstRumor = FALSE;
          
          if (r >= tavernTales.NumTale())
          {
            StartedSearchWithFirstRumor = TRUE;
            r = 0;
          };

          // search for non-empty tales in r..MAX_TALES-1
          //while ((r < MAX_TALES) && (!IsUsableTale(tales[r], EachTaleOnceOnly))) //(tales[r].tale.GetLength() == 0))
          while (     (r < tavernTales.NumTale()) 
                   && (!IsUsableTale(tavernTales.GetTale(r), EachTaleOnceOnly))) //(tales[r].tale.GetLength() == 0))
          {
            r++;
          };

          if ((r >= tavernTales.NumTale()) && (!StartedSearchWithFirstRumor))
          {
            // no rumor in r..MAX_TALES-1, so look in 0..r-1
            // if we did not already start with r = 0
            r=0;
            //while ((r < MAX_TALES) && (!IsUsableTale(tales[r], EachTaleOnceOnly))) //(tales[r].tale.GetLength() == 0))
            while (    (r < tavernTales.NumTale()) 
                    && (!IsUsableTale(tavernTales.GetTale(r), EachTaleOnceOnly))) //(tales[r].tale.GetLength() == 0))
              r++;
          }

          if (r < tavernTales.NumTale())
          {
            LastRumor = r;
            tavernTales.GetTale(r)->count++;
            FormatDisplayText(textData, tavernTales.PeekTale(r)->m_tale);
          }
          else
          {
            LastRumor = -1;
            ClearFormattedText(textData);
          }
        }
      }
      else // pick a random non-empty rumor
      {
//        int ValidRumors[MAX_TALES];
        int *ValidRumors = new int[tavernTales.NumTale()];
        memset(ValidRumors, 0, tavernTales.NumTale()*sizeof(int));
        int numValid=0;
        for (i=0; i<tavernTales.NumTale(); i++)
        {
          //if (IsUsableTale(tales[i], EachTaleOnceOnly)) //(tales[i].tale.GetLength() > 0)
          if (IsUsableTale(tavernTales.GetTale(i), EachTaleOnceOnly)) //(tales[i].tale.GetLength() > 0)
          {
            ValidRumors[numValid] = i;
            numValid++;
          };
        }
        if (numValid > 0)
        {
          // get random number 0..numValid-1
          int rd = (RollDice(numValid, 1, 0)-1);
          //tales[ValidRumors[rd]].count++;
          tavernTales.GetTale(ValidRumors[rd])->count++;
          FormatDisplayText(textData, tavernTales.PeekTale(ValidRumors[rd])->m_tale);
        }
        else
        {
          FormatDisplayText(textData,"This seems to be a quiet crowd");
        };
        delete ValidRumors;
      };
      break;

    case 3: // Exit
      party.setPartyAdventureState();
      if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
      currPic.Clear();
      ClearFormattedText(textData);
      ChainHappened();
      return;
      
    }; // case menuItem()
    break;

  case TASK_TavernDrink:
    //if (drinkPointTrigger <= 0)
    //  return; // Drink as much as you like
    index = menu.currentItem()-1;
    if ((index >= 0) && (index < MAX_DRINKS))
    {
      DRINK &drink = drinks[index];
      CString temp;
      {
        HOOK_PARAMETERS hookParameters;
        hookParameters[5] = drinks[index].name;
        RunEventScript(EVENT_TAVERN_DRINK);
        if (hookParameters[0].IsEmpty())
        {
          temp.Format("You drink some %s.", drinks[index].name);
        }
        else
        {
          temp = hookParameters[0];
        };
      }
      FormatDisplayText(textData, temp);

      if (drinkPointTrigger > 0)
      {
        party.drinkPoints += drink.points;
        if (party.drinkPoints >= drinkPointTrigger)
        {
          if (levelData.eventData.IsValidEvent(drinkChain))
          {
            ReplaceEvent(levelData.eventData.GetEvent(drinkChain),LevelEvent);
          };
        }
      }
    }
    else
    {
      if (index == MAX_DRINKS) // Done
      {
        OnInitialEvent(); // Probably should not repeat entry text
      };
    };
    break;

  default:
    break;
  }; // switch (GetMyState())    
}

bool TAVERN::OnIdle(void)
{
  return true;
}

//////////////////////////////////////////////////////////////GIVE_DAMAGE_DATA
void GIVE_DAMAGE_DATA::OnInitialEvent(void)
{
  menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "GiveDamage");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  if (currPic.picType == SpriteDib)
  {
    currPic.SetFrame(party.DetermineSpriteDistance(distance));
  };
  if (ShouldSimulateEnterKey()) 
    simulatedKeyQueue.PushKey(VK_RETURN);
  Invalidate();
}

void GIVE_DAMAGE_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN) return;
  int i,result;
  if ((who == ActiveChar)||(who==OneAtRandom))
    nbrAttacks=1;

  for (int a=0;a<nbrAttacks;a++)
  {
    switch (who)
    {
    case NoPartyMember:
      break;

    case EntireParty:
      for (i=0;i<party.numCharacters;i++)
      {
        ActorType actor;
        party.characters[i].GetContext(&actor);
        SetCharContext(&actor);
        SetTargetContext(&actor);

        party.characters[i].
          giveCharacterDamage(eventSave, 
                              attackTHAC0, 
                              dmgDice, 
                              dmgDiceQty, 
                              dmgBonus, 
                              spellSave, 
                              saveBonus,
                              NULL         // The Attacker
                              );
        ClearTargetContext();
        ClearCharContext();
      };
      break;

    case ActiveChar:
      if (party.activeCharacter >= 0)
      {
        ActorType actor;
        party.characters[party.activeCharacter].GetContext(&actor);
        SetCharContext(&actor);
        SetTargetContext(&actor);

        party.characters[party.activeCharacter].
          giveCharacterDamage(eventSave, 
                              attackTHAC0, 
                              dmgDice, 
                              dmgDiceQty, 
                              dmgBonus, 
                              spellSave, 
                              saveBonus,
                              NULL        // The Attacker
                              );
        ClearTargetContext();
        ClearCharContext();
      };
      break;

     case OneAtRandom:
       {
         ActorType actor;
         i = RollDice(party.numCharacters, 1);

         party.characters[i-1].GetContext(&actor);
         SetCharContext(&actor);
         SetTargetContext(&actor);

         party.characters[i-1].
           giveCharacterDamage(eventSave, 
                               attackTHAC0, 
                               dmgDice, 
                               dmgDiceQty, 
                               dmgBonus, 
                               spellSave, 
                               saveBonus,
                               NULL       // The Attacker
                               );
         ClearTargetContext();
         ClearCharContext();
       };
     break;

     case ChanceOnEach:
       for (i=0;i<party.numCharacters;i++)
       {
         result = RollDice(100,1);
         if (result <= chancePerAttack)
         {
           ActorType actor;
           party.characters[i].GetContext(&actor);
           SetCharContext(&actor);
           SetTargetContext(&actor);

           party.characters[i].
             giveCharacterDamage(eventSave, 
                                 attackTHAC0, 
                                 dmgDice, 
                                 dmgDiceQty, 
                                 dmgBonus, 
                                 spellSave, 
                                 saveBonus,
                                 NULL           // The Attacker
                                 );
           ClearTargetContext();
           ClearCharContext();
         };
       };
       break;
     }; // switch (who)
  }; // for nbr of attacks

  party.setPartyAdventureState();      
  ChainHappened();   
}


void GIVE_DAMAGE_DATA::OnDraw(void)
{
  UpdateGiveDamageScreen('d');
}

//////////////////////////////////////////////////////////////GAIN_EXP_DATA
void GAIN_EXP_DATA::OnInitialEvent(void)
{
  menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "GainExperience");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  if (PlayEventSounds)
    hSound = PlaySound(sound);

  // check difficulty settings for exp mods
  double mod = GetAllExpMod(); 

  tempExp = experience;
  if (mod != 0.0) tempExp += ((mod/100.0)*tempExp);
  //if (tempExp < 0) tempExp=0;
  Invalidate();
}

void GAIN_EXP_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN) return;
  int i,result;  
  switch (who)
  {
  case NoPartyMember:
    break;
  case EntireParty:
    for (i=0;i<party.numCharacters;i++)
    {
      party.characters[i].giveCharacterExperience(tempExp);
      party.characters[i].SetReadyToTrain(party.characters[i].IsReadyToTrain());
    };
    break;
  case ActiveChar:
    i = party.activeCharacter;
    if (i >= 0)
    {
      party.characters[i].giveCharacterExperience(tempExp);
      party.characters[i].SetReadyToTrain(party.characters[i].IsReadyToTrain());
    };
    break;
  case OneAtRandom:
    i = party.numCharacters;
    result = RollDice(i,1);
    i = result;
    party.characters[i-1].giveCharacterExperience(tempExp);
    party.characters[i-1].SetReadyToTrain(party.characters[i-1].IsReadyToTrain());
    break;
  case ChanceOnEach:
    for (i=0;i<party.numCharacters;i++)
    {
      result = RollDice(100,1);
      if (result <= chance)
      {
        party.characters[i].giveCharacterExperience(tempExp);
        party.characters[i].SetReadyToTrain(party.characters[i].IsReadyToTrain());
      };
    }
    break;
  }; // switch (who)
  
  party.setPartyAdventureState();      
  currPic.Clear();
  ClearSound(&hSound);

  for (i=0;i<party.numCharacters;i++)
  {
    // sets character name to different color when
    // ready to train.
    party.characters[i].SetReadyToTrain(party.characters[i].IsReadyToTrain());
  }

  ChainHappened();
}


void GAIN_EXP_DATA::OnDraw(void)
{
  UpdateGainExpScreen('c');
}

/////////////////////////////////////////////////////////////HEAL_PARTY_DATA
void HEAL_PARTY_DATA::OnInitialEvent(void)
{
  //if (IsCombatActive())
  //{
  //  party.HealParty(*this);
  //  ChainHappened();
  //}
  //else
  {
    menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "HealParty");
    menu.setHorzOrient();
    FormatDisplayText(textData, GetEventText());
    currPic = GetEventPic();
    //currPic.LoadPicSurfaces("sp_");
    currPic.LoadPicSurfaces("");
  }
  Invalidate();
}

void HEAL_PARTY_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN) return;
  party.HealParty(*this);
  party.setPartyAdventureState();      
  currPic.Clear();
  ChainHappened();
}


void HEAL_PARTY_DATA::OnDraw(void)
{
  UpdateHealPartyScreen('b');
}

////////////////////////////////////////////////////////WHO_PAYS_EVENT_DATA
void WHO_PAYS_EVENT_DATA::OnInitialEvent(void)
{
  menu.setMenu(WhoPaysMenuData, NULL, FALSE, this, "WhoPays");
  menu.setHorzOrient();
  menu.enableTitle(FALSE);
  FormatDisplayText(textData,GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  SetMyState(TASK_WhoPaysGet);
  Invalidate();
}

void WHO_PAYS_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  bool failed=false;  
  if (TABParty(key)) return;
  if (key != KC_RETURN) 
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  }
  CHARACTER &dude = GetActiveChar(this);
  switch (GetMyState())
  {
  case TASK_WhoPaysGet:
    // if impossible to pay 
    //   go to fail
    // else
    //   get selected char
    //   check char funds against event requirements
    //   if sufficient funds
    //     subtract funds from char
    //     go to success
    //   else
    //     go to fail
    //   end
    // end

    failed = false;
    if (menu.currentItem() == 1) // select
    {      
      if (!impossible)
      {
        if (!dude.enoughMoney(platinum,
                              gems,
                              jewels,
                              moneyType))
        {
          failed = true;
        }
      }
      else
        failed = true;
    }
    else
    {
      // nobody chosen to pay, exit event
      switch (failAction)
      {
      case NoAction:
        ChainHappened();
        return;
      case ChainEvent: // chained event will be detected later
        ChainOrQuit(failChain);
        return;    
      case Teleport: 
        currPic.Clear();
        HandleTransfer(failTransferData,"", currPic, "", FALSE, true);
        break;      
      case BackupOneStep:
        TaskMessage(TASKMSG_MovePartyBackward);
        ChainHappened();
        break;
      }; // switch (failAction)
      return;
    }

    if (failed)
    {
      SetMyState(TASK_WhoPaysFailure);
      menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
      menu.setHorzOrient();
      menu.enableTitle(FALSE);
      FormatDisplayText(textData, GetEventText3());
      currPic = GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      if (GetEventText3().GetLength() <= 0)
        simulatedKeyQueue.PushKey(VK_RETURN);
      return;
    }
    else
    {
      menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
      menu.setHorzOrient();
      menu.enableTitle(FALSE);
      FormatDisplayText(textData, GetEventText2());
      currPic = GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      dude.payForItem(platinum, moneyType, gems, jewels);
      SetMyState(TASK_WhoPaysSuccess);
      if (GetEventText2().GetLength() <= 0)
        simulatedKeyQueue.PushKey(VK_RETURN);
      return;
    };
    break;

  case TASK_WhoPaysFailure:
    currPic.Clear();
    ClearFormattedText(textData);
    switch (failAction)
    {
    case NoAction:
      ChainHappened();
      return;
    case ChainEvent: // chained event will be detected later
      ChainOrQuit(failChain);
      return;    
    case Teleport: 
      currPic.Clear();
      HandleTransfer(failTransferData,"", currPic, "", FALSE, true);
      break;      
    case BackupOneStep:
      TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
      break;
    }; // switch (failAction)
    break;
  case TASK_WhoPaysSuccess:
    currPic.Clear();
    ClearFormattedText(textData);
    switch (successAction)
    {
    case NoAction:
      ChainHappened();
      return;
    case ChainEvent: // chained event will be detected later
      ChainOrQuit(successChain);
      return;
    case Teleport: 
      currPic.Clear();
      HandleTransfer(successTransferData,"", currPic, "", FALSE, true);    
      break;
    case BackupOneStep:
      TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
      break;
    }; // Switch (successAction)
    break;

  default:
    break;
  }; // switch (GetMyState())
}


void WHO_PAYS_EVENT_DATA::OnDraw(void)
{
  UpdateWhoPaysScreen('a');
}


////////////////////////////////////////////////////////////PASS_TIME_EVENT_DATA
void PASS_TIME_EVENT_DATA::OnInitialEvent(void)
{
  menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "PassTime");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  if (ShouldSimulateEnterKey()) 
    simulatedKeyQueue.PushKey(VK_RETURN);
  Invalidate();
}

void PASS_TIME_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
      return;
 
  currPic.Clear();
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);  

  if (PassSilent)
  {
    DWORD minuteTotal = (this->Days*1440) + (this->Hours*60) + this->Minutes;
    for (DWORD d=0;d<minuteTotal;d++)
      party.incrementClock(1); 
    ChainHappened();
  }
  else if (SetTime)
  {
    DWORD minuteTotal=0;

    if (this->Days > 0)    minuteTotal += this->Days*1440;
    else minuteTotal += party.days*1440;

    if (this->Hours > 0)   minuteTotal += this->Hours*60;
    else minuteTotal += party.hours*60;

    if (this->Minutes > 0) minuteTotal += this->Minutes;
    else minuteTotal += party.minutes;

    DWORD currTotal = party.getElapsedMinutes();
    
    int delta = minuteTotal - currTotal;
    if (delta > 0)
    {
      for (int d=0;d<delta;d++)
        party.incrementClock(1); 
    }
    ChainHappened();
  }
  else
    PushEvent(new PASSING_TIME_EVENT_DATA(this->Days,this->Hours,this->Minutes,AllowStop), DeleteEvent);
}

void PASS_TIME_EVENT_DATA::OnDraw(void)
{
  UpdatePassTimeScreen('Z');
}

void PASS_TIME_EVENT_DATA::OnReturnToTopOfQueue(void)
{
  ChainHappened();
}

////////////////////////////////////////////////////////////PASSING_TIME_EVENT_DATA

__int64 GetMinuteDelta(int days, int hours, int mins)
{
  __int64 minuteDelta = 15;
  __int64 minuteTotal = (days*1440) + (hours*60) + mins;
  
  if (minuteTotal <= 0) return 0;

  // pass time faster when longer time
  // remains to wait
  /*
  if (minuteTotal >= 181*1440)      // for anything greater than 180 days
  {
    double tmp = minuteTotal;
    tmp *= 0.05;                    //    5% of what's left
    minuteDelta = tmp;
  }
  else if (minuteTotal >= 180*1440) // 180 days
    minuteDelta = 1440*60;          //    60 days
  else if (minuteTotal >= 120*1440) // 120 days
    minuteDelta = 1440*30;          //    30 days
  else if (minuteTotal >= 90*1440)  // 90 days
    minuteDelta = 1440*20;          //    20 days
  else*/ if (minuteTotal >= 60*1440)  // 60 days
    minuteDelta = 1440*14;          //    14 days  
  else if (minuteTotal >= 30*1440)  // 30 days
    minuteDelta = 1440*2;           //    2 days
  else if (minuteTotal >= 2*1440)   // 2 days
    minuteDelta = 1440;             //    1 day
  else if (minuteTotal >= 1440)     // 24 hours
    minuteDelta = 120;              //    2 hours
  else if (minuteTotal >= 12*60)    // 12 hours
    minuteDelta = 60;               //    1 hour
  else if (minuteTotal >= 6*60)     // 6 hours
    minuteDelta = 30;               //    30 mins
  else if (minuteTotal >= 60)       // 1 hour
    minuteDelta = 15;               //    15 mins
  else if (minuteTotal >= 30)       // 1/2 hour
    minuteDelta = 5;                //    5 mins
  else if (minuteTotal >= 15)       // 1/4 hour
    minuteDelta = 1;                //    1 min
  else
    minuteDelta = 1;

  if (minuteTotal < minuteDelta)
    minuteDelta = 1;

  return minuteDelta;
}

void PASSING_TIME_EVENT_DATA::OnInitialEvent(void)
{
  menu.reset(NULL);
  if (AllowStop)
  {
    menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
    menu.addMenuItem("PRESS ENTER TO STOP", 7);    
    menu.setHorzOrient();
  }
  FormatDisplayText(textData, GetEventText());
}

void PASSING_TIME_EVENT_DATA::OnKeypress(key_code key, char ascii)
{ 
  if (TABParty(key)) return;
  if (AllowStop)
  {
    if (key != KC_RETURN)
      return;
    currPic.Clear();    
    PopEvent();
  }
}

bool PASSING_TIME_EVENT_DATA::OnCycle(void)
{  
  __int64 dleft = daysLeft;
  __int64 hleft = hoursLeft;
  __int64 mleft = minutesLeft;

  __int64 minuteDelta = GetMinuteDelta(dleft, hleft, mleft);
  __int64 minuteTotal = (dleft*1440) + (hleft*60) + mleft;

  for (__int64 delta=0;delta<minuteDelta;delta++)
    party.incrementClock(1);
  
  minuteTotal -= minuteDelta;
  if (minuteTotal > 0)
  {
    dleft = minuteTotal / 1440;
    minuteTotal %= 1440;
    hleft = minuteTotal / 60;
    minuteTotal %= 60;
    mleft = minuteTotal;
  }
  else
  {
    dleft = 0;
    hleft = 0;
    mleft = 0;
  }

  dleft = max(0, dleft);
  hleft = max(0, min(23,hleft));
  mleft = max(-1, min(59,mleft));

  daysLeft = dleft;
  hoursLeft = hleft;
  minutesLeft = mleft;

  GameEvent::OnCycle();
  return true;
}

void PASSING_TIME_EVENT_DATA::OnDraw(void)
{ 
  // format new message box text
  CString ptext;
  ptext.Format("Passing Time: %I64i days, %I64i hours, %I64i minutes...",
              daysLeft, hoursLeft, minutesLeft);
  FormatDisplayText(textData, ptext, FALSE); // turn off slow text    
  UpdatePassTimeScreen('Z');

  if (!((daysLeft > 0) || (hoursLeft > 0) || (minutesLeft > 0)))
  {
    currPic.Clear();    
    PopEvent();
  }
}

////////////////////////////////////////////////////////////RANDOM_EVENT_DATA
void RANDOM_EVENT_DATA::OnInitialEvent(void)
{
  menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "Random");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");

  if (ShouldSimulateEnterKey())
    simulatedKeyQueue.PushKey(VK_RETURN);
  Invalidate();
}

void RANDOM_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  }

  int i;
  int total=0;
  int count=0;
  DWORD eventlist[RANDOM_EVENT_DATA::MAX_RANDOM_EVENTS];
  DWORD chancelist[RANDOM_EVENT_DATA::MAX_RANDOM_EVENTS];

  memset(eventlist, 0, sizeof(eventlist));
  memset(chancelist, 0, sizeof(chancelist));

  for (i=1;i<RANDOM_EVENT_DATA::MAX_RANDOM_EVENTS;i++)
  {
    bool valid=false;
    if (IsGlobalEvent())
    {
      valid = (   (globalData.eventData.IsValidEvent(EventChains[i-1]))
               && (EventChance[i-1] > 0));
    }
    else
    {
      valid = (   (levelData.eventData.IsValidEvent(EventChains[i-1]))
               && (EventChance[i-1] > 0));
    }

    if (valid)
    {
      eventlist[count]=EventChains[i-1];
      chancelist[count]=EventChance[i-1];
      total += EventChance[i-1];
      count++;
    }
  }

  if ((count==0) || (total == 0))
  {
    ChainHappened();
    return;
  }

  int result = RollDice(total, 1, 0.0);
  total=0;
  int index = -1;

  for (i=0;i<count;i++)
  {
    total += chancelist[i];
    if (total >= result)
    {
      index = i;
      break;
    }
  }

  if (index >= 0)
  {
    if (IsGlobalEvent())
      ReplaceGlobalEvent(eventlist[index]);
    else
      ReplaceEvent(eventlist[index]);
  }
  else
    ChainHappened();
}

void RANDOM_EVENT_DATA::OnDraw(void)
{
  UpdateRandomEventScreen('X');
}

////////////////////////////////////////////////////////////SMALL_TOWN_DATA

void SMALL_TOWN_DATA::OnInitialEvent(void)
{
  menu.setMenu(SmallTownMenuData, NULL, FALSE, this, "SmallTown");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 7);
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  Invalidate();
}

void SMALL_TOWN_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  };

  switch (menu.currentItem())
  {
  case 1: // temple
    if (!levelData.eventData.IsValidEvent(TempleChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushEvent(TempleChain);
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  case 2: // training hall
    if (!levelData.eventData.IsValidEvent(TrainingHallChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushEvent(TrainingHallChain);
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  case 3: // shop
    if (!levelData.eventData.IsValidEvent(ShopChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushEvent(ShopChain);
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  case 4: // Inn
    if (!levelData.eventData.IsValidEvent(InnChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushEvent(InnChain);
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  case 5: // Pub
    if (!levelData.eventData.IsValidEvent(TavernChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushEvent(TavernChain);
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  case 6: // Vault
    if (!levelData.eventData.IsValidEvent(VaultChain))
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushEvent(VaultChain);
    currPic.Clear();
    ClearFormattedText(textData);
    break;
  case 7: // exit
    currPic.Clear();
    if (WaitUntilReturnPressed()) ClearFormattedText(textData);
    ChainHappened();
    break;
  }
}

void SMALL_TOWN_DATA::OnDraw(void)
{
  UpdateSmallTownScreen('Y');
}

void SMALL_TOWN_DATA::OnReturnToTopOfQueue(void)
{
  currPic.Clear();
  ClearFormattedText(textData);
  
  menu.setMenu(SmallTownMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  Invalidate();
}

////////////////////////////////////////////////////////////ADD_NPC_DATA
void ADD_NPC_DATA::OnInitialEvent(void)
{
  menu.setMenu(AddNPCData, NULL, FALSE, this, "AddNPC");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("prt_");
  currPic.LoadPicSurfaces("");
  if (currPic.picType == SpriteDib)
  {
    currPic.SetFrame(party.DetermineSpriteDistance(distance));
  };
  // no text and no pic? then auto exit
  if (ShouldSimulateEnterKey())
    simulatedKeyQueue.PushKey(VK_RETURN);
  Invalidate();
}

void ADD_NPC_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key,ascii);
    return;
  }

  currPic.Clear();
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);

  //if (!party.addNPCToParty(charAdded, hitPointMod, useOriginal))
  if (!party.addNPCToParty(characterID, hitPointMod, useOriginal))
  {
    WriteDebugString("Failed to add NPC in ADD_NPC_DATA::OnKeypress()\n");
    //if (charAdded > 0) // if NPC specified (value is init to zero)
    if (characterID.IsValidNPC()) // if NPC specified (value is init to zero)
      miscError = NPCPartyLimitReached;
  }

  ChainHappened();
}

void ADD_NPC_DATA::OnDraw(void)
{
  UpdateAddNPCScreen('P');
}

////////////////////////////////////////////////////////////REMOVE_NPC_DATA
void REMOVE_NPC_DATA::OnInitialEvent(void)
{
  menu.setMenu(RemoveNPCData, NULL, FALSE, this, "RemoveNPC");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("prt_");
  currPic.LoadPicSurfaces("");
  if (currPic.picType == SpriteDib)
  {
    currPic.SetFrame(party.DetermineSpriteDistance(distance));
  };
  // no text and no pic? then auto exit
  if (ShouldSimulateEnterKey())
    simulatedKeyQueue.PushKey(VK_RETURN);
  Invalidate();
}

void REMOVE_NPC_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key,ascii);
    return;
  }

  currPic.Clear();
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);
  //party.removeNPCFromParty(charRemoved);
  party.removeNPCFromParty(characterID);
  ChainHappened();
}

void REMOVE_NPC_DATA::OnDraw(void)
{
  UpdateRemoveNPCScreen('O');
}

////////////////////////////////////////////////////////////NPC_SAYS_DATA
void NPC_SAYS_DATA::OnInitialEvent(void)
{
  menu.setMenu(NPCSaysData, NULL, FALSE, this, "NPCSays");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("prt_");
  currPic.LoadPicSurfaces("");
  if (currPic.picType == SpriteDib)
  {
    currPic.SetFrame(party.DetermineSpriteDistance(distance));
  };
  if (PlayEventSounds) hSound = PlaySound(sound);  
  Invalidate();
}

void NPC_SAYS_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key,ascii);
    return;
  }

  currPic.Clear();
  // let text continue being displayed if return not needed
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);
  ClearSound(&hSound);
  ChainHappened();
}

void NPC_SAYS_DATA::OnDraw(void)
{
  UpdateNPCSaysScreen('E');
}

void NPC_SAYS_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  if (ShouldSimulateEnterKey()) simulatedKeyQueue.PushKey(VK_RETURN);
}

////////////////////////////////////////////////////////////SHOP

void SHOP::OnInitialEvent(void)
{
  menu.setMenu(ShopMenuData, NULL, FALSE, this, "Shop");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 8);
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  Invalidate();
}          

void SHOP::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  };

  switch (menu.currentItem())
  {
  case 1: // Buy    
    PushEvent(new BUY_SHOP_ITEMS_DATA(GetEventType(),this),DeleteEvent);
    break;
  case 2: // Items (view items on character)
    PushEvent(new ITEMS_MENU_DATA(GetEventType(),this,NULL, NULL, 0),DeleteEvent);
    break;
  case 3: // View (character stats page)
    PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),this), DeleteEvent);
    break;
  case 4: // Take (pooled treasure/money)
    setTreasureEventState(this);
    PushEvent(new TAKE_TREASURE_DATA(this), DeleteEvent);
    break;
  case 5: // Pool (all money into lump sum)
    party.poolPartyGold();
    setTreasureEventState(this);
    break;
  case 6: // Share (evenly distribute pooled money)
    party.AutoUpConvertPoolMoney();
    party.sharePartyGold();
    // rebuild item list text
    setTreasureEventState(this);
    break;
  case 7: // Appraise (determine value of gem/jewelry)    
    PushEvent(new APPRAISE_SELECT_DATA(&GetActiveChar(this).money,canApprGems,canApprJewels), DeleteEvent);
    break;
  case 8: // exit the shop
    if (party.moneyPooled)
    {      
      SetMyState(TASK_AskLeaveMoneyBehind);
      PushEvent(new ASK_YES_NO_MENU_DATA("LEAVE YOUR MONEY BEHIND?",TRUE, ShopEvent), DeleteEvent);
    }
    else
    {
      treasureItemListText.Clear();
      itemListText.Clear();
      currPic.Clear();
      if (WaitUntilReturnPressed()) ClearFormattedText(textData);
      party.setPartyAdventureState();
      if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
    }
    break;
  }
}

void SHOP::OnDraw(void)
{
  UpdateShopScreen('F');
}

void SHOP::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  
  if (party.moneyPooled)
  {
    menu.setItemInactive(5); // pool
  }
  else 
  {
    menu.setItemInactive(4); // take
    menu.setItemInactive(6); // share
  }

  if (GetActiveChar(this).GetAdjStatus() != Okay)
    menu.setItemInactive(1); // buy
  if ((canApprGems==FALSE)&&(canApprJewels==FALSE))
    menu.setItemInactive(7); // appraise
}

void SHOP::OnReturnToTopOfQueue(void)
{
  if (GetMyState()==TASK_AskLeaveMoneyBehind)
  {
    if (party.tradeItem==0) // no, don't leave money behind
    {
      //party.sharePartyGold(); 
      ClearFormattedText(textData);
      setTreasureEventState(this);
      menu.setMenu(ShopMenuData, NULL, FALSE, NULL, NULL);
      menu.setHorzOrient();
      currPic = GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
    }
    else
    {
      party.moneyPooled=FALSE;
      party.poolSack.Clear(); // forfeit pooled funds
     
      treasureItemListText.Clear();
      itemListText.Clear();
      currPic.Clear();
      if (WaitUntilReturnPressed()) ClearFormattedText(textData);
      party.setPartyAdventureState();
      if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
    }
  }
  else
  {
    //ClearFormattedText(textData);
    //setTreasureEventState(this);
    //menu.setMenu(ShopMenuData);
    //menu.setHorzOrient();
    //currPic = GetEventPic();
    //currPic.LoadPicSurfaces("sp_");
    //currPic.LoadPicSurfaces("");
    OnInitialEvent();
    return;
  }
  Invalidate();
}

void SHOP::OnLeaveQueue(void)
{
  //if (party.moneyPooled)
  //  party.sharePartyGold();
}

////////////////////////////////////////////////////////////BUY_SHOP_ITEMS_DATA

void BUY_SHOP_ITEMS_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  ASSERT( m_pOrigEvent->event == ShopEvent );

  POSITION pos = ((SHOP*)m_pOrigEvent)->itemsAvail.GetHeadPosition();
  while (pos != NULL)
    ((SHOP*)m_pOrigEvent)->itemsAvail.GetNext(pos).identified = TRUE; // shops disclose full name

  setShopItemState(((SHOP*)m_pOrigEvent)->itemsAvail, 
                   ((SHOP*)m_pOrigEvent)->costFactor);
  
  menu.setMenu(BuyMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  handleItemsFormInput(ITEMSFORM_exit, itemListText);
  handleItemsFormInput(ITEMSFORM_initial, itemListText);
  Invalidate();
}          

void BUY_SHOP_ITEMS_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key,ascii,prevShopPage,nextShopPage);
    return;
  };

  switch (menu.currentItem())
  {
  case 1: // Buy (add to char inventory, adjust char money)
    GetActiveChar(m_pOrigEvent).buyItem(itemListText.GetItemID(currShopTextIndex()),
                                        ((SHOP*)m_pOrigEvent)->costFactor);
    break;
  case 2: // Next page of items
    nextShopPage();
    break;
  case 3: // Prev page of items
    prevShopPage();
    break;
  case 4: // exit to shop
    clearShopItemState();
    handleItemsFormInput(ITEMSFORM_exit, itemListText);
    PopEvent();
    break;
  }
}

void BUY_SHOP_ITEMS_DATA::OnDraw(void)
{  
  //UpdateBuyItemsScreen();
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();        
  if (errorTextData.numLines == 0)
    handleItemsFormInput(ITEMSFORM_refresh, itemListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void BUY_SHOP_ITEMS_DATA::OnUpdateUI(void)
{
  ITEM_DATA *pItem = NULL;
  //GLOBAL_ITEM_ID giID;
  ITEM_ID itemID;
  int costOfSelectedItem;
  if (CheckForMultiBoxText()) return;
  CHARACTER& dude = GetActiveChar(m_pOrigEvent);
  
  if (itemListText.GetCount() > 0)
  {
    //giID = itemListText.GetItemIndex(currShopTextIndex());
    itemID = itemListText.GetItemID(currShopTextIndex());

    //pItem = itemData.GetItemData(giID);
    pItem = itemData.GetItem(itemID);
  };
  if (pItem == NULL)
  {
    costOfSelectedItem = 0;
  }
  else
  {
    costOfSelectedItem = ApplyCostFactor((m_pOrigEvent)->costFactor, pItem->Cost);
  };
  
  if ((!dude.enoughMoney(costOfSelectedItem,0,0,globalData.moneyData.GetDefaultType())) || (itemListText.GetCount() <= 0))
    menu.setItemInactive(1); // buy

  if (!allowShopItemNextPage())
    menu.setItemInactive(2); // next

  if (!allowShopItemPrevPage())
    menu.setItemInactive(3); // prev
}                               

////////////////////////////////////////////////////////////CAMP_EVENT_DATA

void CAMP_EVENT_DATA::OnInitialEvent(void)
{  
  menu.reset(NULL);
  PushEvent(new ENCAMP_MENU_DATA(this), DeleteEvent);
}          


void CAMP_EVENT_DATA::OnReturnToTopOfQueue(void)
{
  if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
  ChainHappened();
}


int FindMarker(const CString& markerName, int thisChainRoot)
{
  //GameEvent *pEvent;
  //int rootID, eventID;
  //pEvent = levelData.eventData.FindMarker(markerName, thisChainRoot, &rootID, &eventID);

  //globalData.eventData.FindMarker(markerName, &rootID, &eventID);
  /* Really */ NotImplemented(0x410b, false);
  return 0;
}

void FLOW_CONTROL_EVENT_DATA::OnInitialEvent(void)
{
  menu.reset(NULL);
  bool conditionMet = false;
  bool equals = value.IsEmpty();
  bool doAction = false;
  // Firstly, we must do the global variable modification.
  if (!globalVariableName.IsEmpty())
  {
    ASLENTRY *pAsl;
    CString newValue;
    int i;
    switch (valueModification)
    {
    case NO_CHANGE:
      pAsl = globalData.global_asl.Find(globalVariableName);
      if (pAsl == NULL) break;
      equals = value == pAsl->Value();
      break;
    case SET_VARIABLE:
      globalData.global_asl.Insert(globalVariableName, value, 0);
      equals = true;
      break;
    case INCREMENT_VARIABLE:
      pAsl = globalData.global_asl.Find(globalVariableName);
      if (pAsl == NULL) break;
      i = atoi(pAsl->Value());
      i++;
      newValue.Format("%d", i);
      globalData.global_asl.Insert(globalVariableName, newValue, ASLF_MODIFIED);
      equals = value == newValue;
      break;
    case DECREMENT_VARIABLE:
      pAsl = globalData.global_asl.Find(globalVariableName);
      if (pAsl == NULL) break;
      i = atoi(pAsl->Value());
      i--;
      newValue.Format("%d", i);
      globalData.global_asl.Insert(globalVariableName, newValue, ASLF_MODIFIED);
      equals = value == newValue;
      break;
    };
  };

  if (action == ACTION_NONE)
  {
    ChainHappened();  // The easy case.
    return;
  };
  switch (actionCondition)
  {
  case CONDITION_ALWAYS:
    doAction = true;
    break;
  case CONDITION_EQUALS:
    doAction = equals;
    break;
  case CONDITION_NOTEQUALS:
    doAction = !equals;
    break;
  };

  if (doAction)
  {
    DWORD eventID;
    eventID = destID;
    if (eventID != 0)
    {
      if (IsGlobalEvent())
      {
        if (globalData.eventData.IsValidEvent(eventID))
        {
          ReplaceGlobalEvent(eventID); // replace yourself with CHAIN A.
        }
        else
        {
          PopEvent(); // if event is not valid, don't continue chaining
        };
      }
      else
      {
        if (levelData.eventData.IsValidEvent(eventID))
        {
          ReplaceEvent(eventID); // replace yourself with CHAIN A.
        }
        else
        {
          PopEvent(); // if event is not valid, don't continue chaining
        };
      };
    }
    else
    {
      ChainHappened();
    };
  }
  else
  {
    ChainHappened();
  };


  //if (IsGlobalEvent())
  //{
  //  if (globalData.eventData.IsValidEvent(Chain))
  //  {
  //    //ClearFormattedText(textData);
  //    ReplaceGlobalEvent(Chain); // replace yourself with CHAIN A.
  //  }
  //  else
  //    PopEvent(); // if event is not valid, don't continue chaining
  // }
  //else
  //{
  //  if (levelData.eventData.IsValidEvent(Chain))
  //  {
  //    //ClearFormattedText(textData);
  //    ReplaceEvent(Chain); // replace yourself with CHAIN A.
  //  }
  //  else
  //    PopEvent(); // if event is not valid, don't continue chaining
  //}
}




////////////////////////////////////////////////////////////CHAIN_EVENT

// The CHAIN_EVENT performs an IF condition THEN CHAIN A, ELSE CHAIN B.
// CHAIN A happens only if the CHAIN_EVENT trigger conditions are
// satisfied. CHAIN B is allowed to happen only if CHAIN A does not.
// Even if CHAIN B is set to 'Chain Always', it will not trigger if
// CHAIN A does.
//
void CHAIN_EVENT::OnInitialEvent(void)
{  
  menu.reset(NULL);
  // event trigger conditions met
  if (IsGlobalEvent())
  {
    if (globalData.eventData.IsValidEvent(Chain))
    {
      //ClearFormattedText(textData);
      ReplaceGlobalEvent(Chain); // replace yourself with CHAIN A.
    }
    else
      PopEvent(); // if event is not valid, don't continue chaining
  }
  else
  {
    if (levelData.eventData.IsValidEvent(Chain))
    {
      //ClearFormattedText(textData);
      ReplaceEvent(Chain); // replace yourself with CHAIN A.
    }
    else
      PopEvent(); // if event is not valid, don't continue chaining
  }
}          

bool CHAIN_EVENT::OnTestTrigger(bool allowForwardStep)
{
  if (!GameEvent::OnTestTrigger(false)) return false;
  
  //if (!levelData.eventData.IsValidEvent(Chain)) return false;
  return true;
} 

////////////////////////////////////////////////////////////UTILITIES_EVENT_DATA
void UTILITIES_EVENT_DATA::OnInitialEvent(void)
{
  menu.reset(NULL);
}

bool UTILITIES_EVENT_DATA::OnIdle(void)
{
  long temp;

  if (mathItemIndex >= 0)
  {
    switch (operation)
    {
    case MO_NoOperation:
      break;
    case MO_StoredIn:
      switch (mathItemType)
      {
      case ITEM_FLAG:
        globalData.specialItemData.SetStage(mathItemIndex,mathAmount);
        break;
      case KEY_FLAG:
        globalData.keyData.SetStage(mathItemIndex,mathAmount);
        break;
      case QUEST_FLAG:
        globalData.questData.SetStage(mathItemIndex,mathAmount);
        break;
      }
      break;
    case MO_AddedTo:
      switch (mathItemType)
      {
      case ITEM_FLAG:
        temp = globalData.specialItemData.GetStage(mathItemIndex) + mathAmount;
        temp = max( 0, min(temp, 65535) );
        globalData.specialItemData.SetStage(mathItemIndex, temp);
        break;
      case KEY_FLAG:
        temp = globalData.keyData.GetStage(mathItemIndex) + mathAmount;
        temp = max( 0, min(temp, 65535) );
        globalData.keyData.SetStage(mathItemIndex, temp);
        break;
      case QUEST_FLAG: // cannot add to a quest and make it fail
        globalData.questData.IncStage(mathItemIndex, mathAmount);
        break;
      }
      break;
    case MO_SubtractFrom:
      switch (mathItemType)
      {
      case ITEM_FLAG:
        temp = globalData.specialItemData.GetStage(mathItemIndex) - mathAmount;
        temp = max( 0, min(temp, 65535) );
        globalData.specialItemData.SetStage(mathItemIndex, temp);
        break;
      case KEY_FLAG:
        temp = globalData.keyData.GetStage(mathItemIndex) - mathAmount;
        temp = max( 0, min(temp, 65535) );
        globalData.keyData.SetStage(mathItemIndex, temp);
        break;
      case QUEST_FLAG:
        temp = globalData.questData.GetStage(mathItemIndex) - mathAmount;
        temp = max( 0, min(temp, 65535) );
        globalData.questData.SetStage(mathItemIndex, temp);
        break;
      }
      break;
    }
  }

  BOOL activate = FALSE;

  switch (itemCheck)
  {
  case MIC_None:
    break;
  case MIC_AllItems:
    {
      BOOL missing = FALSE;
      int i=0;
      while ((!missing) && (i < items.list.GetSize()))
      {
        if (items.list[i].index >= 0)
        {
          switch (items.list[i].ItemType)
          {
          case ITEM_FLAG:
            if (!party.hasSpecialItem(items.list[i].index))
              missing = TRUE;
            break;
          case KEY_FLAG:
            if (!party.hasSpecialKey(items.list[i].index))
              missing = TRUE;
            break;
          case QUEST_FLAG:
            if (!globalData.questData.IsPresent(items.list[i].index))
              missing = TRUE;
            break;
          }
        }
        i++;
      }
      if (items.list.GetSize() > 0)
        activate = (missing == FALSE);
      else
        activate = FALSE;
    }
    break;

  case MIC_AtLeastOneItem:
    {
      BOOL found = FALSE;
      int i=0;
      while ((!found) && (i < items.list.GetSize()))
      {
        if (items.list[i].index >= 0)
        {
          switch (items.list[i].ItemType)
          {
          case ITEM_FLAG:
            if (party.hasSpecialItem(items.list[i].index))
              found = TRUE;
            break;
          case KEY_FLAG:
            if (party.hasSpecialKey(items.list[i].index))
              found = TRUE;
            break;
          case QUEST_FLAG:
            if (globalData.questData.IsPresent(items.list[i].index))
              found = TRUE;
            break;
          }
        }
        i++;
      }
      if (items.list.GetSize() > 0)
        activate = found;
      else
        activate = FALSE;
    }
    break;
  }

  if (activate)
  {
    if (resultItemIndex >= 0)
    {
      switch (resultItemType)
      {
      case ITEM_FLAG:
        temp = globalData.specialItemData.GetStage(resultItemIndex) + 1;
        temp = max( 0, min(temp, 65535) );
        globalData.specialItemData.SetStage(resultItemIndex,temp);
        break;
      case KEY_FLAG:
        temp = globalData.keyData.GetStage(resultItemIndex) + 1;
        temp = max( 0, min(temp, 65535) );
        globalData.keyData.SetStage(resultItemIndex,temp);
        break;
      case QUEST_FLAG:
        globalData.questData.SetStage(resultItemIndex,1);
        break;
      }
    }
  }

  if (endPlay)
    ReplaceEvent(new EXIT_DATA, DeleteEvent);
  else
    ChainHappened();

  return false;
}

////////////////////////////////////////////////////////////SOUND_EVENT

void SOUND_EVENT::OnInitialEvent(void)
{
  menu.reset(NULL);
}

bool SOUND_EVENT::OnIdle(void)
{

  if ((pSndMgr != NULL) && (PlayEventSounds))
  {
    pSndMgr->StopQueue();

    POSITION pos = sounds.GetHeadPosition();

    while (pos != NULL)
      pSndMgr->QueueSound(sounds.GetNext(pos));

    pSndMgr->PlayQueue();
  }

  ChainHappened();
  return false;
} 

////////////////////////////////////////////////////////////SPECIAL_ITEM_KEY_EVENT_DATA

void SPECIAL_ITEM_KEY_EVENT_DATA::OnInitialEvent(void)
{
  menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "SpecislItem");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());  
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");

  if (ShouldSimulateEnterKey()) 
  {
    menu.setItemInactive(1); // Press Enter to continue
    simulatedKeyQueue.PushKey(VK_RETURN);
  }
  Invalidate();
}

void SPECIAL_ITEM_KEY_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key,ascii);
    return;
  }
  
  int i;
  for (i=0; i<items.list.GetSize(); i++)
  {
    if (items.list[i].operation == SPECIAL_OBJECT_GIVE)
    {
      switch (items.list[i].ItemType)
      {
      case ITEM_FLAG:
        if (!party.hasSpecialItem(items.list[i].index))
          party.addSpecialItem(items.list[i].index);
        break;
      case KEY_FLAG:
        if (!party.hasSpecialKey(items.list[i].index))
          party.addSpecialKey(items.list[i].index);
        break;
      }
    }
    else if (items.list[i].operation == SPECIAL_OBJECT_TAKE)
    {
      switch (items.list[i].ItemType)
      {
      case ITEM_FLAG:
        if (party.hasSpecialItem(items.list[i].index))
          party.removeSpecialItem(items.list[i].index);
        break;
      case KEY_FLAG:
        if (party.hasSpecialKey(items.list[i].index))
          party.removeSpecialKey(items.list[i].index);
        break;
      }
    }
  }

  currPic.Clear();
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);
  if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
  ChainHappened();
}

void SPECIAL_ITEM_KEY_EVENT_DATA::OnDraw(void)
{
  UpdateSpecialItemKeyScreen('G');
}

bool SPECIAL_ITEM_KEY_EVENT_DATA::OnTestTrigger(bool allowForwardStep)
{
  if (!GameEvent::OnTestTrigger())
    return false;

  bool canHappen = false;

  int i;
  for (i=0; (i<items.list.GetSize()) && (!canHappen); i++)
  {
    if (items.list[i].operation == SPECIAL_OBJECT_GIVE)
    {
      switch (items.list[i].ItemType)
      {
      case ITEM_FLAG:          
        if (!party.hasSpecialItem(items.list[i].index))
          canHappen = true;
        break;
      case KEY_FLAG:
        if (!party.hasSpecialKey(items.list[i].index))
          canHappen = true;
        break;
      }
    }
    else if (items.list[i].operation == SPECIAL_OBJECT_TAKE)
    {
      switch (items.list[i].ItemType)
      {
      case ITEM_FLAG:          
        if (party.hasSpecialItem(items.list[i].index))
          canHappen = true;
        break;
      case KEY_FLAG:
        if (party.hasSpecialKey(items.list[i].index))
          canHappen = true;
        break;
      }
    }
  }

#ifdef _DEBUG
  if (!canHappen)
    WriteDebugString("SPECIAL_ITEM event not triggered\n");
  else
    WriteDebugString("SPECIAL_ITEM event triggered\n");
#endif

  return canHappen;
}

SHAZAM_CACHE shazamCache;

GPDL_STATE SHAZAM_CACHE::LoadBinary(const CString& eventText, GPDL *talkGPDL)
{
  CString appendix;
  CString binary;
  CString path, filename;
  GPDL_STATE dscState;
  int i;
  // Search the text starting at column 8.
  // Count the characters occuring befor "**"
  for (i=8; ; i++)   // We are guaranteed to find a "**".
  {
    if ( (eventText.GetAt(i) == '*') && (eventText.GetAt(i+1) == '*'))
    {
      break;
    };
  };
  appendix = eventText.Mid(8,i-8);
  binary = cache.Lookup(appendix);
  if (!binary.IsEmpty())
  {
    RAM_FILE ramfile;
    if (binary == "Error")
    {
      dscState = GPDL_READ_ERROR;
      return GPDL_READ_ERROR;
    };
    ramfile.Initialize(binary);
    dscState=talkGPDL->Load(ramfile);
    return GPDL_OK;
  };
  path = rte.DataDir();
  path += "talk";
  path += appendix;
  try // CFile/CArchive throw many exceptions
  {
    CFile f;
    filename = path + ".bin";
    if (f.Open(filename,CFile::modeRead)) 
    {
	    CArchive ar(&f,CArchive::load);
		  dscState=talkGPDL->Load(ar);
		  ar.Close();
		  f.Close();
      return GPDL_OK;
	  }
    else
    {
      FILE *tf;
      // Look for a .txt file
      filename = path + ".txt";
      if ((tf = fopen(filename,"rb")) != NULL) 
      {
        GPDLCOMP gpdlcomp;
        RAM_FILE ramfile;
        binary = gpdlcomp.CompileProgram(tf);
        CStringPAIR entry(appendix, binary);
        cache.Insert(entry);
        if (binary == "Error")
        {
          dscState = GPDL_READ_ERROR;
          return GPDL_READ_ERROR;
        };
        ramfile.Initialize(binary);
    		dscState=talkGPDL->Load(ramfile);
		    fclose(tf);
        return GPDL_OK;
	    }
      else
      {
        // Try to compile the text from the text event;
        // There should be
        //  "abc(3,4,5,...)"
        //  Function abc(x,y,z,...)
        // text[i] = "**"  so we start by looking at text[i+2];
        int len;
        len = eventText.GetLength();
        for (i=i+2; i<len; i++)
        {
          if (eventText.GetAt(i) == '(')
          {
            break;
          };
        };
        if (i == len)
        {
          return GPDL_EVENT_ERROR;
        };
        for (;i<len; i++)
        {
          if (eventText.GetAt(i) == ')')
          {
            break;
          };
        };
        if (i == len)
        {
          return GPDL_EVENT_ERROR;
        };
        // i now points to the program itself
        CString program;
        program = eventText.Right(len-i-1);
        if (program.GetLength() < 20)   //"$PUBLIC $FUNC A(){};"
        {
          return GPDL_EVENT_ERROR;
        };
        {
          GPDLCOMP gpdlcomp;
          RAM_FILE ramfile;
          binary = gpdlcomp.CompileProgram(program);
          CStringPAIR entry(appendix, binary);
          cache.Insert(entry);
          ramfile.Initialize(binary);
    		  dscState=talkGPDL->Load(ramfile);
        };
        return GPDL_OK;
      };
    };
  }
  catch (...)
  {
    WriteDebugString("GPDL event failed to open %s\n", filename);
    return GPDL_READ_ERROR;
  };

}


//////////////////////////////////////////////////////////////GPDL_EVENT
void GPDL_EVENT::OnInitialEvent(void)
{
  ActorType actor;
  m_talkGPDL = new GPDL;
  ClearFormattedText(textData);
  menu.setMenu(GPDLEventMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.setUseActive(false);
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");

  GPDL_STATE dscState;
	dscState=m_talkGPDL->State();	
  CString fname;

  if (dscState==GPDL_UNINITIALIZED) 
  {
    dscState = shazamCache.LoadBinary(this->GetEventText(), m_talkGPDL);
    if (dscState == GPDL_OK)
    {
      dscState = GPDL_IDLE;
    }
    else
    {
      Invalidate();
      PopEvent();        
      return;
    };
	}
  GetActiveChar(this).GetContext(&actor);
  SetCharContext(&actor);
	if (dscState!=GPDL_UNINITIALIZED) 
  {
		if (dscState==GPDL_IDLE) 
    {     
      CString funcCall;
      CString eventText;
      int len;
      int n;
      int i, j;
      eventText = GetEventText();
      len = eventText.GetLength();
      n = 0;
      for (i=0; i<len-1; i++)
      {
        if ((eventText.GetAt(i) == '*') && (eventText.GetAt(i+1) == '*'))
        {
          n++;
          i++;
          if (n == 2) break;
        };
      };
      if (n != 2)
      {
        ClearCharContext();
        Invalidate();
        PopEvent();        
        return;
      };
      for (j=i; j<len; j++)
      {
        if (eventText.GetAt(j) == '(') break;
      };
      for (;j<len; j++)
      {
        if (eventText.GetAt(j) == ')') break;
      };
      funcCall = eventText.Mid(i+1, j-i);

 			m_talkGPDL->BeginExecute(funcCall, this);
      if (   (m_talkGPDL->State() != GPDL_WAIT_ACK)
           &&(m_talkGPDL->State() != GPDL_WAIT_INPUT) )
      {
        ClearCharContext();
        Invalidate();
        PopEvent();        
        return;
      }
		}
	}
  Invalidate();
}

void GPDL_EVENT::OnKeypress(key_code key, char ascii)
{
  if (m_talkGPDL->State()==GPDL_WAIT_ACK) 
  {
    if (TABParty(key)) return;
    switch (key)
    {
    case KC_SPACE: 
	    m_talkGPDL->Char(' ');
      currPic = GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      break;
    case KC_UP:     m_talkGPDL->Char(UP_ARROW);        break;
    case KC_DOWN:   m_talkGPDL->Char(DOWN_ARROW);      break;
    case KC_RETURN: m_talkGPDL->Char(CARRIAGE_RETURN); break;
    default:
      break;
    }
	} 
  else if (m_talkGPDL->State()==GPDL_WAIT_INPUT) 
  {
    switch (key)
    {
    case KC_RETURN:
		  m_talkGPDL->Char(CARRIAGE_RETURN);
      currPic = GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      break;
    case KC_SPACE: m_talkGPDL->Char(' ');        break;
    case KC_UP:    m_talkGPDL->Char(UP_ARROW);   break;
    case KC_BACK:  m_talkGPDL->Char(BACKSPACE);  break;
    case KC_DOWN:  m_talkGPDL->Char(DOWN_ARROW); break;
    default:       m_talkGPDL->Char(ascii);      break; // includes letters, numbers, punctuation
    }
	}

	if (m_talkGPDL->State()==GPDL_IDLE) 
  {
    if (TABParty(key)) return;
    if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
    ClearCharContext();
    ChainHappened();
	  //PopEvent();    
  } 
}

void GPDL_EVENT::OnDraw(void)
{
  if (pic.key > 0)
  {
    UpdateGPDLScreen(m_talkGPDL, &pic);
  }
  else
  {
    UpdateGPDLScreen(m_talkGPDL, &currPic);
  };
}

void GPDL_EVENT::OnUpdateUI(void)
{
  /*
   { "UP/Down scrolls text", -1 },
   { "SPACE to enter text", -1 },
   { "RETURN to quit", -1 },
   { "Enter Text followed by RETURN", -1 },
   { "(just RETURN to quit)", -1 }
  */
  if (CheckForMultiBoxText()) return;
  if (m_talkGPDL->State()==GPDL_WAIT_ACK) 
  {
	  menu.setItemInactive(4); // Text followed by RETURN
    menu.setItemInactive(5); // RETURN part 2
	} 
  else if (m_talkGPDL->State()==GPDL_WAIT_INPUT) 
  {
	  menu.setItemInactive(1); // Up/Down Arrow
	  menu.setItemInactive(2); // SPACE
    menu.setItemInactive(3); // RETURN
  }
  Invalidate();
}



////////////////////////////////////////////////////////////TAKE_PARTY_ITEMS_DATA

void TAKE_PARTY_ITEMS_DATA::OnInitialEvent(void)
{
  menu.setMenu(TextEventData, NULL, FALSE, this, "TakeItems");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");  
  currPic.LoadPicSurfaces("");  
  Invalidate();
}

void TAKE_PARTY_ITEMS_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN) 
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  };

  party.TakePartyItems(*this);
  if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
  currPic.Clear();
  // let text continue being displayed if return not needed
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);
  ChainHappened();
}

void TAKE_PARTY_ITEMS_DATA::OnDraw(void)
{
  UpdateTakePartyItemsScreen('H');
}

void TAKE_PARTY_ITEMS_DATA::OnUpdateUI(void)
{
  //{ "EXIT", 1 },
  //{ "PRESS ENTER TO CONTINUE", 7 }

  if (CheckForMultiBoxText()) return;

  menu.setItemInactive(1); // Exit

  if (ShouldSimulateEnterKey()) 
  {
    menu.setItemInactive(2); // Press Enter to continue
    simulatedKeyQueue.PushKey(VK_RETURN);
  }
}

////////////////////////////////////////////////////////////TRAININGHALL

void TRAININGHALL::OnInitialEvent(void)
{
  menu.setMenu(TrainWelcomeMenuData, NULL, FALSE, this, "Training");
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  Invalidate();
}

void TRAININGHALL::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key,ascii);
    return;
  }

  currPic.Clear();
  ClearFormattedText(errorTextData);
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);

  switch (menu.currentItem()) 
  {
  case 1: // Yes
    PushEvent(new MAIN_MENU_DATA(this), DeleteEvent);
    break;
  case 2: // No
    if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
    ChainHappened();
    break;
  }
}

void TRAININGHALL::OnReturnToTopOfQueue()
{
  if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
  ChainHappened();
}

void TRAININGHALL::OnDraw(void)
{
  UpdateTrainHallWelcomeScreen('I');
}

int TRAININGHALL::OnSaveGame(unsigned int *saveArea)
{
  return 0; // Nothing needs to be saved.
}

int TRAININGHALL::OnLoadGame(unsigned int *saveArea)
{
  return 0;
}

int TRAININGHALL::LocateTrainableBaseclass(const BASECLASS_ID& baseclassID) const
{
  int i, n;
  n = trainableBaseclasses.GetSize();
  for (i=0; i<n; i++)
  {
    if (PeekTrainableBaseclass(i)->baseclassID == baseclassID)
    {
      return i;
    };
  };
  return -1;
}


////////////////////////////////////////////////////////WHO_TRIES_EVENT_DATA
void WHO_TRIES_EVENT_DATA::OnInitialEvent(void)
{
  switch (GetMyState())
  {
  case TASK_WhoTriesFailure:
    menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "WhoTries");
    menu.setHorzOrient();
    menu.enableTitle(FALSE);
    FormatDisplayText(textData, GetEventText3());
    if (GetEventText3().GetLength() <= 0)
      simulatedKeyQueue.PushKey(VK_RETURN);
    break;
  
  case TASK_WhoTriesSuccess:
    menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
    menu.enableTitle(FALSE);
    FormatDisplayText(textData, GetEventText2());
    if (GetEventText2().GetLength() <= 0)
      simulatedKeyQueue.PushKey(VK_RETURN);
    break;


  case TASK_uninitialized:
  case TASK_initialized:
    currTry=0;
    // intentional fall-through

  default: // 
    menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
    menu.enableTitle(FALSE);
    FormatDisplayText(textData, GetEventText());
    currPic = GetEventPic();
    //currPic.LoadPicSurfaces("sp_");
    currPic.LoadPicSurfaces("");
    SetMyState(TASK_WhoTriesGet);    
    break;
  }
  Invalidate();
}

void WHO_TRIES_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN) 
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  }

  bool failed=false;  
  switch (GetMyState())
  {
  case TASK_WhoTriesGet:
    if (alwaysFails)
    {
      failed = true;
      currTry = NbrTries;
    }
    else if (alwaysSucceeds)
    {
      failed = false;
    }
    else
    {
      // determine success/fail based on skills/attributes
      CHARACTER &dude = GetActiveChar(this);
      int MinSuccessVal;
      if (compareToDie)
        MinSuccessVal = RollDice(compareDie,1,0);
      else
        MinSuccessVal = max(0,compareDie);

      if (checkSTR)
      {
        if (dude.GetAdjStr() < MinSuccessVal)
          failed = true;
        else if (dude.GetAdjStrMod() < strBonus)
          failed = true;
      }

      if ((checkINT) && (dude.GetAdjInt() < MinSuccessVal))
        failed = true;

      if ((checkWIS) && (dude.GetAdjWis() < MinSuccessVal))
        failed = true;

      if ((checkDEX) && (dude.GetAdjDex() < MinSuccessVal))
        failed = true;

      if ((checkCON) && (dude.GetAdjCon() < MinSuccessVal))
        failed = true;

      if ((checkCHA) && (dude.GetAdjCha() < MinSuccessVal))
        failed = true;
/*
      if ((checkPP) && (dude.GetAdjPickPockets() < MinSuccessVal))
        failed = true;

      if ((checkOL) && (dude.GetAdjOpenLocks() < MinSuccessVal))
        failed = true;

      if ((checkFT) && (dude.GetAdjFindTraps() < MinSuccessVal))
        failed = true;

      if ((checkMS) && (dude.GetAdjMoveSilent() < MinSuccessVal))
        failed = true;

      if ((checkHS) && (dude.GetAdjHideInShadows() < MinSuccessVal))
        failed = true;

      if ((checkHN) && (dude.GetAdjHearNoise() < MinSuccessVal))
        failed = true;

      if ((checkCW) && (dude.GetAdjClimbWalls() < MinSuccessVal))
        failed = true;

      if ((checkRL) && (dude.GetAdjReadLanguages() < MinSuccessVal))
        failed = true;
      */
      if (!failed)
      {
        SCRIPT_CONTEXT scriptContext;
        scriptContext.SetCharacterContext(&dude);
        const ASLENTRY *pEntry;
        pEntry = event_asl.Find("Attempt");
        if (pEntry != NULL)
        {
          HOOK_PARAMETERS hookParameters;
          CString scriptName;
          int col;
          col = 0;
          SUBSTRINGS aslValue, scriptParameters;
          aslValue = pEntry->Value();
          while (aslValue.NextField(&col, &scriptParameters))
          {
            SUBSTRINGS temp;
            scriptParameters.HeadAndTail(&scriptName, &temp);
            temp.HeadAndTail(&hookParameters[5], &hookParameters[6]);
            hookParameters[6] = hookParameters[6].Right(hookParameters[6].GetLength()-1);
            RunGlobalScript("$EVENT_WhoTries_Attempt", scriptName, true);
          };
          if (!hookParameters[0].IsEmpty() && (hookParameters[0] == 'N'))
          {
            failed = true;
          };
        };
      };
    };
    currTry++;

    if (failed)
    {
      // only need retries when comparing to RollDice value
      if ((currTry >= NbrTries) || (!compareToDie))
        SetMyState(TASK_WhoTriesFailure);
      OnInitialEvent();
      return;
    }
    else
    {
      SetMyState(TASK_WhoTriesSuccess);
      OnInitialEvent();
      return;
    }
    break;

  case TASK_WhoTriesFailure:
    currPic.Clear();
    ClearFormattedText(textData);

    switch (failAction)
    {
    case NoAction:
      ChainHappened();
      break;
    case ChainEvent:
      ChainOrQuit(failChain);
      break;    
    case Teleport: 
      currPic.Clear();
      HandleTransfer(failTransferData,"", currPic, "", FALSE, true);
      break;      
    case BackupOneStep:
      TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
      break;
    }
    break;

  case TASK_WhoTriesSuccess:
    currPic.Clear();
    ClearFormattedText(textData);
    switch (successAction)
    {
    case NoAction:
      ChainHappened();
      break;
    case ChainEvent:
      ChainOrQuit(successChain);
      break;
    case Teleport: 
      currPic.Clear();
      HandleTransfer(successTransferData,"", currPic, "", FALSE, true);    
      break;
    case BackupOneStep:
      TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
      break;
    }
    break;

    default:
      break;
  }
}


void WHO_TRIES_EVENT_DATA::OnDraw(void)
{
  UpdateWhoTriesScreen('J');
}

////////////////////////////////////////////////////////////////////////TEMPLE
void TEMPLE::OnInitialEvent(void)
{
  switch(m_state) //switch (GetMyState())
  {
  case TASK_TempleMenu:
    WriteDebugString("OnInitialEvent: TempleMenu\n");
    menu.setMenu(TempleMenuData, NULL, FALSE, this, "Temple");
    menu.setHorzOrient();
    menu.MapKeyCodeToMenuItem(KC_ESCAPE, 7);
    FormatDisplayText(textData, GetEventText2());
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    break;

  case TASK_TempleDonate:
    WriteDebugString("OnInitialEvent: TempleDonate\n");
    menu.setMenu(TempleDonateMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
    menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
    FormatDisplayText(textData, GetEventText2());
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    break;

  case TASK_TempleHeal:
    WriteDebugString("OnInitialEvent: TempleHeal\n");
    menu.setMenu(TempleHealMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
    menu.MapKeyCodeToMenuItem(KC_ESCAPE, 8);
    FormatDisplayText(textData, GetEventText2());
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    break;

  case TASK_TempleGive: // enter money qty for donations
    WriteDebugString("OnInitialEvent: TempleGive\n");
    menu.reset(NULL);
    menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
    menu.setTitle(TempleGiveMenuTitle);
    menu.setHorzOrient();
    menu.setItemSeparation(0);
    menu.addMenuItem(" ");
    menu.setCurrentItem(menu.totalItems());
    ClearFormattedText(textData);
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    break;

  case TASK_TempleCast:
    WriteDebugString("OnInitialEvent: TempleCast\n");
    {
      //if (!globalData.charData.HaveKey(tcKey))
      if (!globalData.charData.HaveNPC(characterID))
      {
        int globalDataIndex;
        // create max level Cleric/MagicUser so any
        // spell can be cast.
        //
        //DWORD StartExp;



       // StartExp = 1+GetMinClericExpForLevel(29);
       // StartExp += 1+GetMinMagicUserExpForLevel(29);

        CHARACTER TempleBishop;
        TempleBishop.SetAlignment(ChaoticNeutral);
        //TempleBishop.race(Human);
        RACE_ID raceID;
        raceID = raceData.PeekRace(0)->RaceID();
        TempleBishop.RaceID(raceID);
        TempleBishop.SetGender(Bishop);
        //TempleBishop.SetClass(C_MU); // 
        CLASS_ID classID;
        classID = classData.PeekClass(0)->ClassID();
        TempleBishop.SetClass(classID); // 
        TempleBishop.SetType(NPC_TYPE);
        TempleBishop.SetName("@TempleBishop");
        TempleBishop.generateNewCharacter(0,0);  
        TempleBishop.SetPermStr(18);
        TempleBishop.SetPermInt(18);
        TempleBishop.SetPermWis(18);
        TempleBishop.SetPermDex(18);
        TempleBishop.SetPermCon(18);
        TempleBishop.SetPermCha(18);
        TempleBishop.CopySpellbook(templeSpells);

        if (TempleBishop.GetMaxHitPoints() == 0)
          WriteDebugString("Default temple cleric not created correctly, might have bogus race/class data files\n");

        //tcKey = globalData.charData.AddCharacter(TempleBishop);
        //characterID = globalData.charData.AddCharacter(&TempleBishop);
        globalDataIndex = globalData.charData.AddCharacter(&TempleBishop);
        CHARACTER *pTempleBishop = globalData.charData.GetCharacter(globalDataIndex);
        pTempleBishop->origIndex = globalDataIndex;
        characterID = TempleBishop.CharacterID();
      }

      //CHARACTER *pTempleBishop = globalData.charData.GetCharacterData(tcKey);
      CHARACTER *pTempleBishop = globalData.charData.GetCharacter(characterID);
      {
        int i,n;
        n = pTempleBishop->GetSpellCount();
        for (i=0; i<n; i++)
      //POSITION pos = pTempleBishop->GetFirstSpellPosition();
      //while (pos!=NULL)
        {
          //pTempleBishop->GetSpellAt(pos).memorized=255;
          //pTempleBishop->GetSpellAt(pos).selected=255;
          //pTempleBishop->GetNextSpell(pos);
          pTempleBishop->GetCharacterSpell(i)->memorized=255;
          pTempleBishop->GetCharacterSpell(i)->selected=255;
        };
      };

      menu.setMenu(TempleCastMenuData, NULL, FALSE, NULL, NULL);
      menu.setHorzOrient();
      menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
      ClearFormattedText(textData);
      setSpellTempleCastItemState(*pTempleBishop, costFactor);
      handleSpellFormInput(SPELLFORM_exit, spellListText);
      handleSpellFormInput(SPELLFORM_initial, spellListText);
    }
    break;

  default:
    WriteDebugString("OnInitialEvent: default\n");
    m_state = TASK_TempleWelcome; //SetMyState(TASK_TempleWelcome);
    menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
    menu.MapKeyCodeToMenuItem(KC_ESCAPE, 1);
    FormatDisplayText(textData, GetEventText());
    currPic = GetEventPic();
    //currPic.LoadPicSurfaces("sp_");
    currPic.LoadPicSurfaces("");
    //if (ShouldSimulateEnterKey())    
    if (textData.numLines == 0)
      simulatedKeyQueue.PushKey(VK_RETURN);
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    break;    
  }
  Invalidate();
}

void TEMPLE::OnDraw(void)
{
  switch(m_state) //switch (GetMyState())
  {
  case TASK_TempleCast:
    {
      //UpdateTempleCastScreen();
      GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
      drawBorder();
      // draw middle horz bar, but 
      // not viewport and middle ver bar
      drawViewport(FALSE, FALSE, TRUE, FALSE);
      if (errorTextData.numLines == 0)
        handleSpellFormInput(SPELLFORM_refresh, spellListText);      
      menu.DisplayMenu(0);
      if (errorTextData.numLines == 0) 
        displayCharacterHP(GetActiveChar(this), 0);
      DisplayTextAndFlip(0);
    }
    break;
  case TASK_TempleGive:
    UpdateTempleGiveScreen();
    break;
  default:
    UpdateTempleScreen('K');
    break;
  }  
}

void TEMPLE::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  // Not legal to do this in 'OnUpdateUI'.   switch (GetMyState())
  switch(m_state)
  {
  case TASK_TempleMenu:    
    if (!allowDonations)
      menu.setItemInactive(2);

    if (party.moneyPooled)
      menu.setItemInactive(5); // pool
    else
      menu.setItemInactive(6); // share
  
    if (treasureItemListText.GetCount() <= 0)
      menu.setItemInactive(4); // take
    break;

  case TASK_TempleDonate:
    if (party.moneyPooled)
      menu.setItemInactive(2); // pool
    else
      menu.setItemInactive(3); // share
  
    if (treasureItemListText.GetCount() <= 0)
      menu.setItemInactive(1); // take
    int qty;
    if (party.moneyPooled)
      qty = !party.poolSack.IsEmpty(); //GetPoolGoldValue();
    else
      qty = !GetActiveChar(this).money.IsEmpty(); //poolCharacterGold();
    if (qty <= 0)
      menu.setItemInactive(5); // give
    break;

  case TASK_TempleHeal:
    if (costFactor != Free)
      menu.setItemInactive(3); // fix

    if (party.moneyPooled)
      menu.setItemInactive(5); // pool
    else
      menu.setItemInactive(6); // share
  
    if (treasureItemListText.GetCount() <= 0)
      menu.setItemInactive(4); // take
    break;

  case TASK_TempleCast:
    if (!levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].AllowMagic)
      menu.setItemInactive(1); // cast

    if (spellListText.GetCount() <= 0)
      menu.setItemInactive(1); // cast
      
    if (!allowSpellNextPage(NULL))
      menu.setItemInactive(2); // next

    if (!allowSpellPrevPage(NULL))
      menu.setItemInactive(3); // prev
    break;
  default:
    break;
  }
}

void TEMPLE::OnKeypress(key_code key, char ascii)
{
  switch(m_state) //switch (GetMyState())
  {
  case TASK_TempleWelcome:
    WriteDebugString("OnKeypress: TASK_TempleWelcome\n");

    if (TABParty(key)) return;
    if (key != KC_RETURN)
      return;
    ClearFormattedText(textData);
    m_state = TASK_TempleMenu; //SetMyState(TASK_TempleMenu);
    OnInitialEvent();
    break;

  case TASK_TempleMenu:
    WriteDebugString("OnKeypress: TASK_TempleMenu\n");
    if (TABParty(key)) return;
    if (key != KC_RETURN)
    {
      HMenuVPartyKeyboardAction(key, ascii);
      return;  
    }
 
    switch (menu.currentItem()) 
    {
    case 1: // Heal
      m_state = TASK_TempleHeal; //SetMyState(TASK_TempleHeal);
      OnInitialEvent();
      break;
    case 2: // Donate
      m_state = TASK_TempleDonate; //SetMyState(TASK_TempleDonate);
      OnInitialEvent();
      break;  
    case 3: // View
      PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),this), DeleteEvent);
      break;
    case 4: // Take
      setTreasureEventState(this);
      PushEvent(new TAKE_TREASURE_DATA(this), DeleteEvent);
      break;
    case 5: // Pool
      party.poolPartyGold();
      setTreasureEventState(this);
      break;
    case 6: // Share
      party.AutoUpConvertPoolMoney();
      party.sharePartyGold();
      setTreasureEventState(this);
      break;
    case 7: // Exit
      if (party.moneyPooled)
      {      
        m_state = TASK_AskLeaveMoneyBehind; //SetMyState(TASK_AskLeaveMoneyBehind);
        PushEvent(new ASK_YES_NO_MENU_DATA("LEAVE YOUR MONEY BEHIND?",TRUE, TempleEvent), DeleteEvent);
      }
      else
      {
        if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
        currPic.Clear();
        if (WaitUntilReturnPressed()) ClearFormattedText(textData);
        handleSpellFormInput(SPELLFORM_exit, spellListText);

        if (totalDonation >= donationTrigger)
        {
          totalDonation = 0;
          // set up for donation trigger
          if (levelData.eventData.IsValidEvent(donationChain))
            ReplaceEvent(donationChain);
          else
            ChainHappened();
        }
        else
          ChainHappened();
      }
      break;
    }
    break;

  case TASK_TempleCast:
    WriteDebugString("OnKeypress: TASK_TempleCast\n");
    if (key != KC_RETURN)
    {
      HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
      return;
    }
  
    switch (menu.currentItem()) 
    {
    case 1: // cast
      {      
        //SPELL_DATA *pSpData = spellData.GetSpellData(spellListText.GetSpellDatabaseKey(currSpellTextIndex()));
        SPELL_DATA *pSpData = spellData.GetSpell(spellListText.GetSpellDatabaseID(currSpellTextIndex()));
        if (pSpData!=NULL)
        {
          //GLOBAL_SPELL_ID gsID;
          SPELL_ID spellID;
          //CHARACTER *pTempleBishop = globalData.charData.GetCharacterData(tcKey);
          CHARACTER *pTempleBishop = globalData.charData.GetCharacter(characterID);
          pTempleBishop->InitTargeting(TouchedTargets, //pSpData->Targeting, 
                                       TRUE, //pSpData->CanTargetFriend, 
                                       TRUE, //pSpData->CanTargetEnemy
                                       TRUE, //pTempleBishop->friendly,
                                       1, //targs, 
                                       1, //range
                                       1, //width
                                       1, //height
                                       pSpData->Lingers); //range);

          pTempleBishop->AddTarget(GetActiveChar(this));
          //gsID = spellListText.GetSpellBookKey(currSpellTextIndex());
          spellID = spellListText.GetSpellBookID(currSpellTextIndex());
          //pTempleBishop->CastSpell(gsID, false);
          pTempleBishop->CastSpell(spellID, false, false);
        }
      }
      break;
    case 2: // next
      nextSpellPage(NULL);
      break;
    case 3: // prev
      prevSpellPage(NULL);
      break;
    case 4: // Exit
      m_state = TASK_TempleMenu; //SetMyState(TASK_TempleMenu);
      OnInitialEvent();
      break;
    }
    break;

  case TASK_TempleDonate:
    WriteDebugString("OnKeypress: TASK_TempleDonate\n");
    if (TABParty(key)) return;
    if (key != KC_RETURN)
    {
      HMenuVPartyKeyboardAction(key, ascii);
      return;  
    }
    
    switch (menu.currentItem()) 
    {
    case 1: // take
      setTreasureEventState(this);
      PushEvent(new TAKE_TREASURE_DATA(this), DeleteEvent);
      break;
    case 2: // pool
      party.poolPartyGold();
      setTreasureEventState(this);
      break;
    case 3: // share
      party.AutoUpConvertPoolMoney();
      party.sharePartyGold();
      setTreasureEventState(this);
      break;
    case 4: // appraise
      PushEvent(new APPRAISE_SELECT_DATA(&GetActiveChar(this).money), DeleteEvent);
      break;
    case 5: // give
      m_state = TASK_TempleGive; //SetMyState(TASK_TempleGive);
      OnInitialEvent();
      break;
    case 6: // Exit
      m_state = TASK_TempleMenu; //SetMyState(TASK_TempleMenu);
      OnInitialEvent();
      break;
    }
    break;

  case TASK_TempleGive:
    WriteDebugString("OnKeypress: TASK_TempleGive\n");
    if (TABParty(key)) return;
    if (   (key != KC_RETURN)
        && (key != KC_NUM)
        && (key != KC_BACK))
      return;
  
    char temp[10], temp2[10];
    int i, sum;   
   
    if (key == KC_RETURN)
    {
      temp[0] = '\0';
    
      if (menu.totalItems() > 0)
      {
        for (i=1;i<menu.totalItems();i++)
          strcat(temp, menu.getMenuItem(i));
      
        sum = atoi(temp);

        if (sum > 0)
        {
          GetActiveChar(this).payForItem(sum,globalData.moneyData.GetDefaultType());
          totalDonation += sum;
        }
      }

      m_state = TASK_TempleMenu; //SetMyState(TASK_TempleMenu);
      OnInitialEvent();
    }
    else 
    {
      int qty;
      if (party.moneyPooled)
        qty = party.GetPoolGoldValue();
      else
        qty = GetActiveChar(this).poolCharacterGold();

      if (key == KC_NUM)          
      {
        temp[0] = '\0';
      
        // ignore last char, it is a blank
        for (i=1;i<menu.totalItems();i++)
          strcat(temp, menu.getMenuItem(i));
      
        sprintf(temp2, "%c", ascii);
        strcat(temp, temp2);
        sum = atoi(temp);
      
        if ((sum > qty) || (sum < 0))
        {
          i=menu.totalItems();
        
          while (i > 0)
          {
            menu.deleteLastMenuItem();
            i--;
          }
        
          itoa(qty, temp, 10);
        
          for (i=0;i<(int)strlen(temp);i++)
          {
            strncpy(temp2, &temp[i], 1);
            temp2[1] = '\0';
            menu.addMenuItem(temp2);
          }
        
          menu.addMenuItem(" ");
          menu.setCurrentItem(menu.totalItems());
        }
        else 
        {
          if (menu.totalItems() == 0)
          {
            menu.addMenuItem(temp2);
            menu.addMenuItem(" ");
          }
          else 
          {
            menu.changeMenuItem(menu.totalItems(), temp2);
            menu.addMenuItem(" ");
          }
          menu.setCurrentItem(menu.totalItems());
        }
      }
      else if (   ((key == KC_BACK) || (key == KC_LEFT))
               && (menu.totalItems() > 0))
      {
        menu.deleteLastMenuItem();
        menu.changeMenuItem(menu.totalItems(), " ");
      
        if (menu.totalItems() == 0)
          menu.addMenuItem(" ");
      
        menu.setCurrentItem(menu.totalItems());
      }
    }
    break;

  case TASK_TempleHeal:
    WriteDebugString("OnKeypress: TASK_TempleHeal\n");
    if (TABParty(key)) return;
    if (key != KC_RETURN)
    {
      HMenuVPartyKeyboardAction(key, ascii);
      return;  
    }
  
    switch (menu.currentItem()) 
    {
    case 1: // Cast
      m_state = TASK_TempleCast; //SetMyState(TASK_TempleCast);
      OnInitialEvent();
      break;
    case 2: // View
      PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),this), DeleteEvent);
      break;
    case 3: // Fix (using party spells)
      party.FixParty(1);
      break;
    case 4: // Take
      setTreasureEventState(this);
      PushEvent(new TAKE_TREASURE_DATA(this), DeleteEvent);
      break;
    case 5: // Pool
      party.poolPartyGold();
      setTreasureEventState(this);
      break;
    case 6: // share
      party.AutoUpConvertPoolMoney();
      party.sharePartyGold();
      setTreasureEventState(this);
      break;
    case 7: // appraise
      PushEvent(new APPRAISE_SELECT_DATA(&GetActiveChar(this).money), DeleteEvent);
      break;
    case 8: // Exit
      m_state = TASK_TempleMenu; //SetMyState(TASK_TempleMenu);
      OnInitialEvent();
      break;
    }
    break;
    default:
      break;
  }
}

void TEMPLE::OnReturnToTopOfQueue()
{
  WriteDebugString("OnReturnToTopOfQueue\n");
  if (m_state==TASK_AskLeaveMoneyBehind) //if (GetMyState()==TASK_AskLeaveMoneyBehind)
  {
    if (party.tradeItem==0) // no, don't leave money behind
    {
      party.AutoUpConvertPoolMoney();
      party.sharePartyGold();
      currPic = GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
      OnInitialEvent();
    }
    else
    {
      party.moneyPooled=FALSE;
      party.poolSack.Clear(); // forfeit pooled funds

      if (ForcePartyBackup()) TaskMessage(TASKMSG_MovePartyBackward);
      currPic.Clear();
      if (WaitUntilReturnPressed()) ClearFormattedText(textData);
      if (totalDonation >= donationTrigger)
      {
        totalDonation = 0;
        // set up for donation trigger
        if (levelData.eventData.IsValidEvent(donationChain))
          ReplaceEvent(donationChain);
        else
          ChainHappened();
      }
      else
        ChainHappened();
    }
  }
  else
  {
    currPic = GetEventPic();
    //currPic.LoadPicSurfaces("sp_");
    currPic.LoadPicSurfaces("");
    OnInitialEvent();
  }
}

void TEMPLE::OnLeaveQueue(void)
{
  m_state = -1;  // Back to virgin state.
  WriteDebugString("OnLeaveQueue\n");
  //globalData.charData.RemoveCharacter(tcKey);
  globalData.charData.RemoveCharacter(characterID);
  //if (party.moneyPooled)
  //  party.sharePartyGold();
}

////////////////////////////////////////////////////////////////////////PASSWORD_DATA
void PASSWORD_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  switch (GetMyState())
  {
  case TASK_PasswordFailure:
    menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "PAssword");
    menu.setHorzOrient();
    menu.enableTitle(FALSE);
    FormatDisplayText(textData, GetEventText3());
    if (GetEventText3().GetLength() <= 0)
      simulatedKeyQueue.PushKey(VK_RETURN);
    break;
  
  case TASK_PasswordSuccess:
    menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
    menu.enableTitle(FALSE);
    FormatDisplayText(textData, GetEventText2());
    if (GetEventText2().GetLength() <= 0)
      simulatedKeyQueue.PushKey(VK_RETURN);
    break;

  case TASK_uninitialized:
  case TASK_initialized:
    currTry=0;
    FormatDisplayText(textData, GetEventText());
    // intentional fall-through

  default:
    menu.reset(NULL);
    menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
    menu.setTitle(GetPasswordTitle);
    menu.setHorzOrient();
    menu.setItemSeparation(0);
    menu.addMenuItem(" ");
    menu.setCurrentItem(menu.totalItems());
    currPic = GetEventPic();
    //currPic.LoadPicSurfaces("sp_");
    currPic.LoadPicSurfaces("");
    SetMyState(TASK_PasswordGet); 
    menu.changeMenuText(&control.eventcontrol_asl, this);
    break;
  }
  Invalidate();
}

void PASSWORD_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  switch (GetMyState())
  {
  case TASK_PasswordFailure:
    if (key != KC_RETURN) 
    {
      HMenuVPartyKeyboardAction(key,ascii);
      //StandardMenuKeyboardAction(key, ascii);
      return;
    }
    currPic.Clear();
    ClearFormattedText(textData);

    switch (failAction)
    {
    case NoAction:
      ChainHappened();
      break;
    case ChainEvent:
      ChainOrQuit(failChain);
      break;    
    case Teleport: 
      currPic.Clear();
      HandleTransfer(failTransferData,"", currPic, "", FALSE, true);
      break;      
    case BackupOneStep:
      TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
      break;
    }
    break;
  
  case TASK_PasswordSuccess:
    if (key != KC_RETURN) 
    {
      HMenuVPartyKeyboardAction(key,ascii);
      //StandardMenuKeyboardAction(key, ascii);
      return;
    }
    currPic.Clear();
    ClearFormattedText(textData);

    switch (successAction)
    {
    case NoAction:
      ChainHappened();
      break;
    case ChainEvent:
      ChainOrQuit(successChain);
      break;    
    case Teleport: 
      currPic.Clear();
      HandleTransfer(successTransferData,"", currPic, "", FALSE, true);
      break;      
    case BackupOneStep:
      TaskMessage(TASKMSG_MovePartyBackward);
      ChainHappened();
      break;
    }
    break;

  case TASK_PasswordGet:
    {
    static BOOL showingFailText = FALSE;
    char temp[6];
    int i;
  
    if (showingFailText)
    {
      showingFailText = FALSE;
      ClearFormattedText(textData);
    }
  
    if (key == KC_RETURN)
    {
      CString pswd="";
      
      // ignore blank on the end
      for (i=1;i<menu.totalItems();i++)
        pswd += menu.getMenuItem(i);
    
      if (PasswordMatches(pswd))
      {
        SetMyState(TASK_PasswordSuccess);
        OnInitialEvent();
        return;
      }
      else
      {
        currTry++;
      
        if (currTry >= nbrTries)
        {
          SetMyState(TASK_PasswordFailure);
          OnInitialEvent();
          return;
        }
        else
        {
          // reset for next attempt
          OnInitialEvent();
          FormatDisplayText(textData, "That is not the correct answer");
          showingFailText = TRUE;
          return;
        }
      }
    }
    else if (   ((key == KC_CHAR) || (key == KC_NUM) || (key == KC_SPACE))
             && (menu.totalItems() < MAX_PSWD_TEXT))
    {
      sprintf(temp, "%c", ascii);
      menu.changeMenuItem(menu.totalItems(), temp);
      menu.addMenuItem(" ");
      menu.setCurrentItem(menu.totalItems());
      return;
    }
    else if (   ((key == KC_BACK) || (key == KC_LEFT))
             && (menu.totalItems() > 0))
    {
      menu.deleteLastMenuItem();
      menu.changeMenuItem(menu.totalItems(), " ");
      menu.setItemActive(menu.totalItems());      
      if (menu.totalItems() == 0)
        menu.addMenuItem(" ");      
      menu.setCurrentItem(menu.totalItems());
      return;
    }
    }
    break;
  default:
    break;
  }
}

void PASSWORD_DATA::OnDraw(void)
{
  UpdatePasswordScreen('L');
}

////////////////////////////////////////////////////////////////////////QUESTION_LIST_DATA

void QUESTION_LIST_DATA::OnInitialEvent(void)
{
  UserResult=NoUserResult;
  menu.reset(NULL);
  menu.setStartCoord(USE_DEFAULT_TEXTBOX_MENU, 0, FALSE);
  menu.setVertOrient();
  menu.setTitle(buttons.title);
  menu.setItemSeparation(2);
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  int count=0;
  int i;
  
  /*
  BOOL AllUnique=TRUE;
  for (i=0;i<MAX_BUTTONS;i++)
  {
    if (!buttons.buttons[i].label.IsEmpty())
    {
      for (int y=i+1;y<MAX_BUTTONS;y++)
      {
        if (!buttons.buttons[y].label.IsEmpty())
        {
          if (buttons.buttons[i].label[0] == buttons.buttons[y].label[0])
            AllUnique = FALSE;
        }
      }
    }
  }
  */

  for (i=0;i<MAX_BUTTONS;i++)
  {
    if (buttons.buttons[i].label != "")
    {
      //if (AllUnique)
      //  menu.addMenuItem(buttons.buttons[i].label,0);
      //else
        menu.addMenuItem(buttons.buttons[i].label);      
      count++;
    }
    else
    {
      int newItem = menu.addMenuItem(" "); // placeholder
      menu.setItemInactive(newItem);
    }
  }
  
  menu.SetFirstLettersShortcuts();
  menu.setCurrentItem(0);
  sx=party.Posx;sy=party.Posy;
  Invalidate();
  if (count == 0) ChainHappened();
}

void QUESTION_LIST_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  }
  currPic.Clear();
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);
  UserResult = menu.currentItem();  
  QLIST_OPTION &button = buttons.buttons[UserResult-1];
  if (IsGlobalEvent())
  {
    if (!globalData.eventData.IsValidEvent(button.chain))
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushGlobalEvent(button.chain);
  }
  else
  {
    if (!levelData.eventData.IsValidEvent(button.chain))
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    else PushEvent(button.chain);
  }
}

void QUESTION_LIST_DATA::OnDraw(void)
{
  UpdateQuestionListScreen('k');
}

void QUESTION_LIST_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  for (int i=0;i<MAX_BUTTONS;i++)
  {
    if (!buttons.buttons[i].present)
      menu.setItemInactive(i+1);
  }
}

void QUESTION_LIST_DATA::OnReturnToTopOfQueue(void)
{
  currPic.Clear();
  ClearFormattedText(textData);
  if (UserResult == NoUserResult)
  {
    ChainHappened();
    return;
  }
  QLIST_OPTION &button = buttons.buttons[UserResult-1];
  switch (button.postChainAction)
  {
  case LabelDoNothing:
    ChainHappened();
    break;
  case LabelReturnToQuestion:
    // only if still in starting location
    if ((sx==party.Posx)&&(sy==party.Posy))
      OnInitialEvent();
    else
      ChainHappened();
    break;
  case LabelBackupOneStep:
    ChainHappened();
    if ((sx==party.Posx)&&(sy==party.Posy))
      TaskMessage(TASKMSG_MovePartyBackward);
    break;
  }
}

int QUESTION_LIST_DATA::OnSaveGame(unsigned int *saveArea)
{
  saveArea[0]=UserResult;
  return 1;
}

int QUESTION_LIST_DATA::OnLoadGame(unsigned int *saveArea)
{
  UserResult=saveArea[0];
  return 1;
}
////////////////////////////////////////////////////////////////////////QUESTION_BUTTON_DATA

void QUESTION_BUTTON_DATA::OnInitialEvent(void)
{
  UserResult=NoUserResult;
  menu.reset(NULL);
  menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
  menu.setHorzOrient();
  menu.setItemSeparation(7);
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  int count=0;
  int i;

  for (i=0;i<MAX_BUTTONS;i++)
  {
    if (buttons.buttons[i].label != "")
    {
      menu.addMenuItem(buttons.buttons[i].label);
      count++;
    }   
    else
    {
      int newItem = menu.addMenuItem(" "); // placeholder
      menu.setItemInactive(newItem);
    }
  }
  menu.SetFirstLettersShortcuts();
  menu.setCurrentItem(0);
  sx=party.Posx;sy=party.Posy;
  Invalidate();
  if (count == 0) ChainHappened();  
}

void QUESTION_BUTTON_DATA::OnKeypress(key_code key, char ascii)
{
  int i;
  bool afterChainActionExists = false;
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  };
  for (i=0; i<MAX_BUTTONS; i++)
  {
    if (    (buttons.buttons[i].present) && (buttons.buttons[i].postChainAction!=LabelDoNothing)
         || (chainEventHappen > 0)  )
    {
      afterChainActionExists = true;
      break;
    };
  };
  currPic.Clear();
  if (WaitUntilReturnPressed()) ClearFormattedText(textData);
  UserResult = menu.currentItem();  
  QBUTTON_OPTION &button = buttons.buttons[UserResult-1];
  if (IsGlobalEvent())
  {
    if (!globalData.eventData.IsValidEvent(button.chain))
    {
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    else 
    {
      if (afterChainActionExists)
      {
        PushGlobalEvent(button.chain);
      }
      else
      {
        ReplaceGlobalEvent(button.chain);
      };
    };
  }
  else
  {
    if (!levelData.eventData.IsValidEvent(button.chain))
    {
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    else 
    {
      if (afterChainActionExists)
      {
        PushEvent(button.chain);
      }
      else
      {
        ReplaceEvent(button.chain);;
      };
    };
  }
}

void QUESTION_BUTTON_DATA::OnDraw(void)
{
  UpdateQuestionButtonScreen('j');
}

void QUESTION_BUTTON_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  for (int i=0;i<MAX_BUTTONS;i++)
  {
    if (!buttons.buttons[i].present)
      menu.setItemInactive(i+1);
  }
}

void QUESTION_BUTTON_DATA::OnReturnToTopOfQueue(void)
{
  currPic.Clear();
  ClearFormattedText(textData);
  if (UserResult == NoUserResult)
  {
    ChainHappened();
    return;
  }

  QBUTTON_OPTION &button = buttons.buttons[UserResult-1];
  switch (button.postChainAction)
  {
  case LabelDoNothing:
    ChainHappened();
    break;
  case LabelReturnToQuestion:
    // only if still in starting location
    if ((sx==party.Posx)&&(sy==party.Posy))
      OnInitialEvent();
    else
      ChainHappened();
    break;
  case LabelBackupOneStep:
    ChainHappened();
    if ((sx==party.Posx)&&(sy==party.Posy))
      TaskMessage(TASKMSG_MovePartyBackward);
    break;
  }
}

int QUESTION_BUTTON_DATA::OnSaveGame(unsigned int *saveArea)
{
  saveArea[0]=UserResult;
  return 1;
}

int QUESTION_BUTTON_DATA::OnLoadGame(unsigned int *saveArea)
{
  UserResult=saveArea[0];
  return 1;
}

////////////////////////////////////////////////////////////////////////QUEST_EVENT_DATA
void QUEST_EVENT_DATA::OnInitialEvent(void)
{
  FormatDisplayText(textData, GetEventText());
  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");

  if ((accept == QA_ImpossibleAuto) || (accept == QA_AutoAccept))
  {
    if (textData.numLines == 0)
    {
      menu.reset(NULL);
    }
    else
    {
      menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "Quest");
      menu.setHorzOrient();
    }
    //simulatedKeyQueue.PushKey(VK_RETURN); <--- called in OnUpdateUI
  }
  else
  {
    menu.setMenu(QuestMenuData, NULL, FALSE, NULL, NULL);
    menu.setHorzOrient();
  }
  Invalidate();
}

void QUEST_EVENT_DATA::OnDraw(void)
{
  UpdateQuestScreen('N');
}

void QUEST_EVENT_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  if ((accept == QA_ImpossibleAuto) || (accept == QA_AutoAccept))
  {
    if (textData.numLines == 0)
      simulatedKeyQueue.PushKey(VK_RETURN);
  }
}

void QUEST_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  }

  bool accepted = false;

  switch (accept)
  {
  case QA_AutoAccept:
  case QA_OnYesOrNo:
    accepted = true;
    break;
  case QA_ImpossibleAuto:
  case QA_Impossible:
    accepted = false;
    break;
  default:
    switch (menu.currentItem())
    {      
    case 1: // Yes
      accepted = (accept == QA_OnYes);
      break;
    case 2: // No
      accepted = (accept == QA_OnNo);
      break;
    }
  }

  if (accepted)
  {
    switch (QuestType())
    {
    case QUEST_FLAG:
      globalData.questData.SetStage(QuestID(), stage);
      break;
    case ITEM_FLAG:
      globalData.specialItemData.SetStage(QuestID(), stage);
      break;
    case KEY_FLAG:
      globalData.keyData.SetStage(QuestID(), stage);
      break;
    };
    if (completeOnAccept)
      globalData.questData.SetComplete(QuestID());
    else if (stage == 1)
      globalData.questData.SetInProgress(QuestID());

    // could be automatic success, and there would be no
    // acceptChain so follow normal chain
    if (IsGlobalEvent())
    {
      if (!globalData.eventData.IsValidEvent(acceptChain))
      {
        if (accept == QA_AutoAccept)
          ChainHappened();
        else
          ReplaceGlobalEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      else ReplaceGlobalEvent(acceptChain);
    }
    else
    {
      if (!levelData.eventData.IsValidEvent(acceptChain))
      {
        if (accept == QA_AutoAccept)
          ChainHappened();
        else
          ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      else ReplaceEvent(acceptChain);
    }
  }
  else
  {
    // could be automatic fail, and there would be no
    // rejectChain so follow normal chain
    if (failOnRejection)
      globalData.questData.SetFailed(QuestID());

    if (IsGlobalEvent())
    {
      if (!globalData.eventData.IsValidEvent(rejectChain))
      {
        if (accept == QA_ImpossibleAuto)
          ChainHappened();
        else
          ReplaceGlobalEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      else ReplaceGlobalEvent(rejectChain);
    }
    else
    {
      if (!levelData.eventData.IsValidEvent(rejectChain))
      {
        if (accept == QA_ImpossibleAuto)
          ChainHappened();
        else
          ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      else ReplaceEvent(rejectChain);
    }
  }

  //currPic.Clear();
  //ClearFormattedText(textData);
}

bool QUEST_EVENT_DATA::OnTestTrigger(bool allowForwardStep)
{
  if (!GameEvent::OnTestTrigger()) return false;
  WORD currStage;
  switch (QuestType())
  {
  default:
  case QUEST_FLAG:
    currStage = globalData.questData.GetStage(QuestID());
    break;
  case ITEM_FLAG:
    currStage = globalData.specialItemData.GetStage(QuestID());
    break;
  case KEY_FLAG:
    currStage = globalData.keyData.GetStage(QuestID());
    break;
  };
  bool result = (currStage == (stage-1));
//#ifdef _DEBUG
  WriteDebugString("Quest Event %u at %i,%i level %i %s triggered (Curr=%i, Quest=%i)\n",
                    id, x,y, globalData.currLevel, 
                   (result==true ? "is" : "not"),
                   currStage, stage);
//#endif
  return result;
}

////////////////////////////////////////////////////////////////////////LOGIC_BLOCK_DATA
void LOGIC_BLOCK_DATA::OnInitialEvent(void)
{
  setPartyXY_x = -1;
  setPartyXY_y = -1;
  menu.reset(NULL);
}

void LogicBlockErrorMsg(CString& msg, LOGIC_BLOCK_DATA *data)
{
  CString LBMSG;
  LBMSG.Format("%s -- id=%d,x=%d,y=%d\n",msg,data->id,party.Posx,party.Posy);
  WriteDebugString(LBMSG);
}

CString LBsubst(const CString& p, CString *w)
{
  int index;
  index=p.Find('&');
  if (index<0) return p;
  CString result;
  int start=0;
  int col=0;
  int len=p.GetLength();
  while ( col < len )
  {
    if (p[col]=='&')
    {
      if ((col<len-1) && (p[col+1]>='A') && (p[col+1]<='L'))
      {
        result += p.Mid(start, col-start);
        col++;
        result += w[p[col]-'A'];
        col++;
        start=col;
      };
    }
    else col++;
  };
  if (col!=start) result += p.Mid(start, col-start);
  return result;
}

int LBseparateCharacter(CString& sp)
{ //Removes the character selector from sp and returns the
  //character index (or -1);
  int len, i, n;
  len = sp.GetLength();
  if ( (len==0) || (sp.GetAt(0)!='(') ) return party.activeCharacter;
  if (len < 3) return -1; // Shortest possible is (^) or (a) or (*)
  if (sp.GetAt(0) != '(') return -1;
  if (sp.Left(3) == "(*)")
  {
    sp = sp.Right(len-3);
    return 999;
  };
  if (sp.GetAt(1) == '^')
  { // A numbered character
    if (sp == "(^)") return party.activeCharacter;
    if (len < 4) return -1; // Should be (^3)
    if (sp.GetAt(3) != ')') return -1;
    if ((sp.GetAt(2)<'1') || (sp.GetAt(2)>'9')) return -1;
    n = sp.GetAt(2) - '0' - 1;
    if (n >= party.numCharacters) return -1;
    sp = sp.Right(len-4);
    return n;
  };
  for (i=1; i<len; i++)
  {
    if (sp.GetAt(i) == ')') break;
  };
  if (sp.GetAt(i) != ')') return -1;
  CString name = sp.Mid(1,i-1);
  for (n=0; n<party.numCharacters; n++)
  {
    if (name.CompareNoCase(party.characters[n].GetName())==0)
    {
      sp = sp.Right(len-i-1);
      return n;
    };
  };
  return -1;
}

/*
enum LOGIC_BLOCK_INPUT_TYPE {
  LBIT_literal = 0,
  LBIT_globalASL,
  LBIT_partySize,
  LBIT_charInfo,
  LBIT_dirFacing,
  LBIT_levelASL,
  LBIT_questStage,
  LBIT_itemList,
  LBIT_NPCList,
  LBIT_RunTimeIf,
  LBIT_NotImplemented = 0xff
}
*/


static int LBString2Integer(const CString& str)
{ //We accept 'dirty' numbers.
  CString clean;
  char *digits;
  int i, num;
  int result = 0;
  digits = (char *)_alloca(str.GetLength()+10);
  num = CleanNumber(str, digits);
  for (i=2; i<num; i++)
  {
    result=10*result + digits[i]-'0';
  };
  if (digits[0] == '-') result = -result;
  return result;
}

static void SplitLevelKey(const CString& str, int& level, CString& key)
{
  int i, len = str.GetLength();
  level = 0;
  if ( (len>1) && (str[0] == '/') )
  {
    for (i=1; i<len; i++)
    {
      if ( (str[i] >= '0') && (str[i] <= '9') ) 
      {
        level = 10 * level + str[i] - '0';
        continue;
      };
      if (str[i] == '/')
      {
        key = str.Right(len-i-1);
        return;
      };
    };
  };
  level = GetCurrentLevel();
  key = str;
}

// The re_registers are made kept here so that after doing a grep you
// can return and get the sub-fields matched (using Wiggle).
static re_registers grepReg;
CString MostRecentlyGreppedString;


void ProcessLBInput(BYTE *type,             // LOGIC_BLOCK_INPUT_TYPE
                    CString *param,         // source param
                    CString& result,        // result string
                    LOGIC_BLOCK_DATA *data, // original logic data
                    char inputName,         // input name letter
                    CString *w)             // array of result strings
{
  switch(*type)
  {
  case LBIT_literal: result=LBsubst(*param,w); break;
  case LBIT_questStage:
    result.Format("%d",globalData.questData.GetStage(LBsubst(*param,w))); 
    break;
  case LBIT_partySize:
    result.Format("%d",party.numCharacters);
    break;
  case LBIT_dirFacing:
    result.Format("%d",party.facing);
    break;
  case LBIT_globalASL:
    result=globalData.global_asl.Lookup((LPCSTR) LBsubst(*param,w));
    break;
  case LBIT_tempASL:
    result=globalData.temp_asl.Lookup((LPCSTR) LBsubst(*param,w));
    break;
  case LBIT_charASL:
    {
      CString sp;
      int n;
      sp = LBsubst(*param, w);
      n = LBseparateCharacter(sp); // sp<--ASL name;
      if ( (n < 0) || (n >= party.numCharacters) ) 
      {
        sp = "Bogus character selector in Input";
        LogicBlockErrorMsg(sp, data);
        result = "";
        break;
      };
      result = party.characters[n].char_asl.Lookup(sp);
      break;
    };
  case LBIT_partyASL:
    result=party.party_asl.Lookup((LPCSTR) LBsubst(*param,w));
    break;
  case LBIT_itemList:
    {
      result="";
      for (int charNum=0; charNum<party.numCharacters; charNum++)
      {
        result += party.characters[charNum].getItemList(charNum);
      };
    };
    break;
  case LBIT_levelASL:
    {
      int level;
      CString key;
      SplitLevelKey(LBsubst(*param,w), level, key);
      result=globalData.levelInfo.stats[level].level_asl.Lookup(key);
    };
    break;
  case LBIT_RunTimeIf:
    { 
      CString sp;
      int n;
      sp = LBsubst(*param, w);
      n = LBseparateCharacter(sp); // sp will have ASL name after return
      if ( (n < 0) || (n >= party.numCharacters) ) 
      {
        ActorType actor;
        GetActiveChar(NULL).GetContext(&actor);
        SetCharContext(&actor);
        SetTargetContext(&actor);
      }
      else
      {
        ActorType actor;
        party.characters[n].GetContext(&actor);
        SetCharContext(&actor);
        SetTargetContext(&actor);
      }

      unsigned long key = KeywordTextToNum(sp);
      switch (GetDataSize(key))
      {
      case IF_TYPE_STRING:
          result = GetDataSTRING(key);
        break;
      case IF_TYPE_BYTE:
        {
          char tmp = GetDataBYTE(key, NULL, "Logic Block runtime information");
          result.Format("%i", tmp);
        }
        break;
      case IF_TYPE_WORD:
        {
          short tmp = GetDataWORD(key);
          result.Format("%i", tmp);
        }
        break;
      case IF_TYPE_DWORD:
        {
          long tmp = GetDataDWORD(key);
          result.Format("%i", tmp);
        }
        break;
      case IF_TYPE_FLOAT:
        {
          float tmp = GetDataFLOAT(key);
          result.Format("%f", tmp);
        }
        break;
      case IF_TYPE_UBYTE:
        {
          unsigned char tmp = GetDataUBYTE(key);
          result.Format("%u", tmp);
        }
        break;
      case IF_TYPE_UWORD:
        {
          unsigned short tmp = GetDataUWORD(key);
          result.Format("%u", tmp);
        }
        break;
      case IF_TYPE_UDWORD:
        {
          unsigned long tmp = GetDataUDWORD(key);
          result.Format("%u", tmp);
        }
        break;
      default:
        CString msg;
        msg.Format("Bogus Logic Input Param for LBIT_RunTimeIf %s", param);
        LogicBlockErrorMsg(msg, data);
        break;
      }
      ClearCharContext();
      ClearTargetContext();
    }
    break;
  case LBIT_NPCList:
    {
      CString temp;
      result="";
      for (int i=0; i<party.numCharacters; i++)
      {
        if (party.characters[i].GetType() == NPC_TYPE)
        {
          temp.Format("%s/%s/%d/",result,party.characters[i].GetName(),i);
          result=temp;
        }
      };
    }
    break;
  case LBIT_charInfo:
    {
      result = "";
      for (int i=0; i<party.numCharacters; i++)
      {
        if (party.characters[i].GetType() == CHAR_TYPE)
        {
          result = result+"/" + party.characters[i].getInfo() + "/";
        };
      };
    };
    break;
  case LBIT_Wiggle:
    {
      int n;
      result = "";
      n = atoi ((LPCTSTR) LBsubst(*param,w));
      if ((n<0) || (n >= (int)grepReg.num_regs)) break;
      result = MostRecentlyGreppedString.Mid(grepReg.start[n],
                                             grepReg.end[n]-grepReg.start[n]);

    };
    break;
  case LBIT_SourceGPDL:
    {
      GPDLCOMP gpdlComp;
      const char *entries[] = {"input", NULL};
      CString sourcePgm, binaryPgm;
      sourcePgm = "$PUBLIC $FUNC input(A,B,C,D,E,F) {"
                  + *param
                  + "}input;";
      binaryPgm = gpdlComp.CompileScript(sourcePgm, entries);
      if (binaryPgm[0] != 0)
      { // error ... henceforth just use a literal ""
        WriteDebugString("%s",binaryPgm);
        result = "";
        *param = "";
        *type = LBIT_literal;
        break;
      };
      *param = binaryPgm;
      *type = LBIT_BinaryGPDL;
    };
    // !!!!! **** NOTE ***** !!!!!
    //    We fall through here.
    // !!!!! **** NOTE ***** !!!!!
  case LBIT_BinaryGPDL:
    gpdlStack.Push();
    result = gpdlStack.activeGPDL()->ExecuteScript(*param,1,w,6); 
    gpdlStack.Pop();
    break;
  default:
    {
      CString msg;
      msg.Format("Bogus Logic Input-%c Type = 0x%02x",inputName,type);
      LogicBlockErrorMsg(msg, data);
    };
  };

  if ((data->m_Flags & LBF_RUNTIME_DEBUG) != 0)
  {
    CString msg;
    msg.Format("Logic Block Input %c = %s",inputName,result);
    LogicBlockErrorMsg(msg, data);
  }
}



static CString LBagreater(CString& s1, CString& s2)
{
  CString result;
  result = LongSubtract(s2, s1);
  if (result[0] =='-') return CString("1");
  else return CString("");
}

static CString LBaslash(CString& s1, CString& s2)
{
  CString quot, remain;
  LongDivide(s1, s2, quot, remain);
  return quot;
}

static CString LBamod(CString& s1, CString& s2)
{
  CString quot, remain;
  LongDivide(s1, s2, quot, remain);
  return remain;
}

static void ProcessLBGate(BYTE type,      // LOGIC_BLOCK_GATE_TYPE
                          CString& top,   // input 1
                          CString& side,  // input 2
                          CString& result,
                          LOGIC_BLOCK_DATA *data,
                          char gateName)
{
  switch(type)
  {
  
  case LBGT_top: result=top; break;
  case LBGT_side: result=side; break;
  case LBGT_land:
    result = (top!="")&&(side!="") ? "1" : "";
    break;
  case LBGT_lor:
    result = (top!="")||(side!="") ? "1" : "";
    break;
  case LBGT_stringEqual:
    result = (top==side) ? "1" : "";
    break;
  case LBGT_aplus:
    result=LongAdd(top,side);
    break;
  case LBGT_aminus:
    result=LongSubtract(top,side);
    break;
  case LBGT_agreater:
    result = LBagreater(top, side);
    break;
  case LBGT_agear:
    result = LongMultiply(top,side);
    break;
  case LBGT_aslash:
    result = LBaslash(top,side);
    break;
  case LBGT_amod:
    result = LBamod(top,side);
    break;
  case LBGT_true:
    result = "1";
    break;
  case LBGT_false:
    result = "";
    break;
  case LBGT_grep:
    {
      re_pattern_buffer grepPat;
      re_Set_EGrep();
      const char *m_grepCResult=re_compile(LPCTSTR(side), &grepPat);
      if (m_grepCResult!=NULL) 
      {
          CString msg;
          msg="Error compiling LogicBlock Grep pattern\n";
          msg+=side;
          msg+=CString(m_grepCResult);
          LogicBlockErrorMsg(msg, data);
          result="";
          break;
      };
      MostRecentlyGreppedString = top;
      int srchRslt=re_search(
                 &grepPat,
                 LPCTSTR(top),
                 top.GetLength(),
                 0,
                 top.GetLength(),
                 &grepReg);
      if (srchRslt<0) 
      {
        result="";
        break;
      };
      result=top.Mid(grepReg.start[0],grepReg.end[0]-grepReg.start[0]);
      break;
    };
    break;
  default:
    {
      CString msg;
      msg.Format("Bogus Logic Gate-%c Type = 0x%02x",gateName,type);
      LogicBlockErrorMsg(msg, data);
    };
  };

  if ((data->m_Flags & LBF_RUNTIME_DEBUG) != 0)
  {
    CString msg;
    msg.Format("Logic Block Gate %c = %s",gateName,result);
    LogicBlockErrorMsg(msg, data);
  };
}

static void ProcessLBNot(BYTE type, CString& input, CString& result)
{
  if (type==0) // not NOT, who's there?
  {
    result=input;
  }
  else // perform NOT
  {
    result = input=="" ? "1" : "";
  };
}

static bool SplitKeyValue(const CString& str, CString& key, CString& value, char token='=')
{
  int i;
  i=str.Find(token);
  if (i<0)
  {
    key=str;
    value="";
    return false;
  }
  else
  {
    key=str.Left(i);
    value=str.Right(str.GetLength()-i-1);    
  };
  return true;
}

/*
enum LOGIC_BLOCK_ACTION_TYPE {
  LBAT_nothing=0,
  LBAT_setGlobalASL,
  LBAT_setLevelASL,
  LBAT_removeGlobalASL,
  LBAT_removeLevelASL,
  LBAT_setQuestStage,
  LBAT_tempASL,
  LBAT_NotImplemented = 0xff
}
*/
static void ProcessLBAction(BYTE If,                // process action? // 0=if true, 1=if false, 2=always
                            BYTE Result,            // result so far
                            BYTE *type,             // LOGIC_BLOCK_ACTION_TYPE
                            CString *param,         // input param
                            LOGIC_BLOCK_DATA *data, // ptr to orig logic data
                            char actionName,        // name of action
                            CString *w)             // array of result strings
{
  if ((If == 0) && (Result == 0)) // only process if true?
  {
    CString msg;
    msg.Format("Logic Action-%c Type = 0x%02x not performed, result=0",actionName,type);
    LogicBlockErrorMsg(msg, data);
    return;
  }

  if ((If == 1) && (Result == 1)) // only process if false?
  {
    CString msg;
    msg.Format("Logic Action-%c Type = 0x%02x not performed, result=1",actionName,type);
    LogicBlockErrorMsg(msg, data);
    return;
  }

  switch(*type)
  {
  case LBAT_nothing: break;
  case LBAT_setQuestStage:
    {
      CString key, value;
      SplitKeyValue(LBsubst(*param,w), key, value);
      globalData.questData.SetStage(key, atoi(value)); 
    };
    break;
  case LBAT_setGlobalASL:
    {
      CString key, value;
      SplitKeyValue(LBsubst(*param,w), key, value);
      globalData.global_asl.Insert(key, value, 0);
    };
    break;
  case LBAT_removeGlobalASL:
    {
      globalData.global_asl.Delete(LBsubst(*param,w));
    };
    break;
  case LBAT_setLevelASL:
    {
      CString levelKey, key, value;
      int level;
      SplitKeyValue(LBsubst(*param,w), levelKey, value,'=');
      SplitLevelKey(levelKey, level, key);
      if (key.GetLength() >= 6)
      {
        LPCTSTR pCh;
        pCh = key.GetString();
        if (   (pCh[0] == '$')
             &&(pCh[1] == 'W')
             &&(pCh[2] == 'a')
             &&(pCh[3] == 'l')
             &&(pCh[4] == 'l')
             &&(pCh[5] == ','))
        {
          Convert$Wall(level,key,value);
          break;
        };
      };
      globalData.levelInfo.stats[level].level_asl.Insert(key, value, 0);
    };
    break;
  case LBAT_removeLevelASL:
    {
      CString key;
      int level;
      SplitLevelKey(LBsubst(*param,w), level, key);
      globalData.levelInfo.stats[level].level_asl.Delete(key);
    };
    break;
  case LBAT_setCharASL:
    {
      CString sp, key, value;
      int n;
      sp = LBsubst(*param,w);
      n = LBseparateCharacter(sp); // sp <-- asl key=value
      if (n < 0)
      {
        sp = "Bogus character identifier in Set Character ASL";
        LogicBlockErrorMsg(sp, data);
        break;
      };
      SplitKeyValue(sp, key, value);
      if (n < 999) // 999 means all party members
      {
        party.characters[n].char_asl.Insert(key, value, 0);
        break;
      };
      for (n=0; n<party.numCharacters; n++)
      {
        party.characters[n].char_asl.Insert(key, value, 0);
      };
      break;
    };
    break;
  case LBAT_setIconIndexByName:
    {
      CString key, value;
      SplitKeyValue(*param, key, value);
      for (int i=0;i<party.numCharacters;i++)
      {
        if (party.characters[i].GetName()==key)
        {
          party.characters[i].iconIndex=atoi(value);
          if (party.characters[i].iconIndex < 1)
            party.characters[i].iconIndex = 1;
          break;
        }
      }
    }
    break;
  case LBAT_setPartyASL:
    {
      CString key, value;
      SplitKeyValue(LBsubst(*param,w), key, value);
      party.party_asl.Insert(key, value, 0);
    };
    break;
  case LBAT_removePartyASL:
    {
      party.party_asl.Delete(LBsubst(*param,w));
    };
    break;
  case LBAT_tempASL:
    {
      CString key, value;
      SplitKeyValue(LBsubst(*param,w), key, value);
      globalData.temp_asl.Insert(key, value, 0);
    };
    break;
  case LBAT_SourceGPDL:
    {
      GPDLCOMP gpdlComp;
      const char *entries[] = {"action", NULL};
      CString sourcePgm, binaryPgm;
      sourcePgm = "$PUBLIC $FUNC action() {"
                  + *param
                  + "}action;";
      binaryPgm = gpdlComp.CompileScript(sourcePgm, entries);
      if (binaryPgm[0] != 0)
      { // error ... henceforth just do nothing
        WriteDebugString("%s",binaryPgm);
        //result = "";
        //*param = "";
        *type = LBAT_nothing;
        break;
      };
      *param = binaryPgm;
      *type = LBAT_BinaryGPDL;
    };
    // !!!!! **** NOTE ***** !!!!!
    //    We fall through here.
    // !!!!! **** NOTE ***** !!!!!
  case LBAT_BinaryGPDL:
    gpdlStack.Push();
    gpdlStack.activeGPDL()->ExecuteScript(*param,1,NULL,0); 
    gpdlStack.Pop();
    break;
  default:
    {
      CString msg;
      msg.Format("Bogus Logic Action-%c Type = 0x%02x",actionName,type);
      LogicBlockErrorMsg(msg, data);
    };
  };
}


static void RecordLogicBlockValues(CString *w, LOGIC_BLOCK_DATA *data)
{
  int i, len, totalLen;
  char *buffer;
  for (len=0, i=0; i<12; i++) len += w[i].GetLength();
  if (len >30000) 
  {
    CString msg("Values too large to record");
    LogicBlockErrorMsg(msg,data);
    return;
  };
  totalLen = len+12*sizeof(int);
  CString str;
  buffer = str.GetBuffer(totalLen);// To avoid the 18 mallocs that would be required to
                                   // build the result using concatenation.
  for (len=0, i=0; i<12; i++)
  {
    len = w[i].GetLength();
    memcpy(buffer,(char *)(&len),sizeof(int));
    buffer += sizeof(int);
    memcpy(buffer,(LPCTSTR)w[i],len);
    buffer += len;
  };
  str.ReleaseBuffer(totalLen);
  globalData.global_asl.Insert("LOGICBLOCKVALUES", str, 0);
}

static BOOL ProcessLogicBlock(LOGIC_BLOCK_DATA *data)
{
  CString w[18];// 0=InputA, 1=InputB, 2=GateC, 3=InputD, 4=GateE;
                // 5=InputF, 6=InputG, 7=GateH, 8=GateI, 9=GateJ, 10=GateK, 11=GateL;
                // 12=NotC, 13=NotE, 14=NotH, 15=NotI, 16=NotJ, 17=NotK;
  /* 
      Inputs: A, B, D, F, G
       Gates: C, E, H, I, J, K, L

  Inputs    B              F
            |              |
            C  <-  A  ->   H
            |              |
           not?           not?
            |              |
            E  <-  D  ->   J
            |              |
           not?           not?
            |              |
            I  <-  G  ->   K
            |              |
           not?           not?
            |_______       |
                   |       |
                   L <-----|
                   |
        Action1---------Action2 // take action(s) based on result of gate L

   w0 = inputA
   w1 = inputB
   w2 = gateC(w0,w1)
   w12 = not(w2)
   w3 = inputD
   w4 = gateE(w3,w12)
   w13 = not(w4)
   w5 = inputF
   w6 = inputG
   w7 = gateH(w5,w0)
   w14 = not(w7)
   w8 = gateI(w6,w13)
   w15 = not(w8)
   w9 = gateJ(w3,w14)
   w16 = not(w9)
   w10 = gateK(w6,w16)
   w17 = not(w10)
   w11 = gateL(w15,w17)

  Inputs    w1          w5
            |           |
            w2 <- w0 -> w7
            |           |
            w12         w14
            |           |
            w4 <- w3 -> w9
            |           |
            w13         w16
            |           |
            w8 <- w6 -> w10
            |           |
            w15         w17
            |____       |
                |       |
               w11 <----|
                |
     Action1---------Action2  // take action(s) based on result of gate L
  */

  //ClearCharContextStack();
  //SetCharContext( GetActiveChar().GetContext() );

  ProcessLBInput (&data->m_InputTypeA, &data->m_InputParamA, w[0], data, 'A', w);
  ProcessLBInput (&data->m_InputTypeB, &data->m_InputParamB, w[1], data, 'B', w);
  ProcessLBGate  (data->m_GateTypeC, w[1], w[0],             w[2], data, 'C');   
  ProcessLBNot   (data->m_NotC, w[2], w[12]);                                  
  ProcessLBInput (&data->m_InputTypeD, &data->m_InputParamD, w[3], data, 'D', w);
  ProcessLBGate  (data->m_GateTypeE, w[12], w[3],            w[4], data, 'E');   
  ProcessLBNot   (data->m_NotE, w[4], w[13]);                                  
  ProcessLBInput (&data->m_InputTypeF, &data->m_InputParamF, w[5], data, 'F', w);
  ProcessLBInput (&data->m_InputTypeG, &data->m_InputParamG, w[6], data, 'G', w);
  ProcessLBGate  (data->m_GateTypeH, w[5], w[0],             w[7], data, 'H');   
  ProcessLBNot   (data->m_NotH, w[7], w[14]);                                  
  ProcessLBGate  (data->m_GateTypeI, w[13], w[6],            w[8], data, 'I');   
  ProcessLBNot   (data->m_NotI, w[8], w[15]);                                  
  ProcessLBGate  (data->m_GateTypeJ, w[14], w[3],            w[9], data, 'J');   
  ProcessLBNot   (data->m_NotJ, w[9], w[16]);                                  
  ProcessLBGate  (data->m_GateTypeK, w[16], w[6],            w[10],data, 'K');   
  ProcessLBNot   (data->m_NotK, w[10], w[17]);                                 
  ProcessLBGate  (data->m_GateTypeL, w[15], w[17],           w[11],data, 'L');   

  data->Result = w[11]=="" ? 0 : 1; // result of all logic ops


  // Now record the results for posterity
  if ((data->m_Flags & LBF_RECORD_VALUES) != 0)RecordLogicBlockValues(w, data);


  // perform conditional action based on result
  ProcessLBAction(data->m_IfTrue1, // 0=if true, 1=if false, 2=always
                  data->Result, 
                  &data->m_ActionType1, 
                  &data->m_ActionParam1, 
                  data, 
                  '1',
                  w);
  ProcessLBAction(data->m_IfTrue2,
                  data->Result, 
                  &data->m_ActionType2, 
                  &data->m_ActionParam2, 
                  data, 
                  '2',
                  w);
  //ClearCharContext();


  return data->Result;
}

bool LOGIC_BLOCK_DATA::OnTestTrigger(bool allowForwardStep)
{
  return GameEvent::OnTestTrigger(false);
}

bool LOGIC_BLOCK_DATA::OnIdle(void)
{
  Result=0;
  // Here we will compute the result
  ProcessLogicBlock(this);


  if (setPartyXY_x >= 0)
  {
    TaskMessage(TASKMSG_SetPartyXY);
    PopEvent();
    return false;
  };

  switch (m_NoChain)
  {
  case 0:  // No chain
    PopEvent();
    break;
  case 1:  // Chain Always
    ChainHappened();
    break;
  case 2:  // Chain conditionally
    switch (Result)
    {
    case 0:
      if (m_ChainIfFalse)
      {
        if (IsGlobalEvent())
        {
          if (!globalData.eventData.IsValidEvent(falseChain))
              ReplaceGlobalEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else ReplaceGlobalEvent(falseChain);
        }
        else
        {
          if (!levelData.eventData.IsValidEvent(falseChain))
              ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else ReplaceEvent(falseChain);
        }
      }
      else
        PopEvent();
      break;
    case 1:
      if (m_ChainIfTrue)
      {
        if (IsGlobalEvent())
        {
          if (!globalData.eventData.IsValidEvent(trueChain))
              ReplaceGlobalEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else ReplaceGlobalEvent(trueChain);
        }
        else
        {
          if (!levelData.eventData.IsValidEvent(trueChain))
              ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
          else ReplaceEvent(trueChain);
        }
      }
      else
        PopEvent();
      break;
    }
    break;
  default:
    PopEvent();
    break;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////GUIDED_TOUR
const int TOUR_TIMEOUT = 1000;

void GUIDED_TOUR::OnEnterQueue(void)
{
  MouseRender.EnableWaitCursor();
}

void GUIDED_TOUR::OnLeaveQueue(void)
{
  MouseRender.DisableWaitCursor();
}

void GUIDED_TOUR::OnInitialEvent()
{
  menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  // GetEventText() is over-ridden for this
  // event to return text for each tour step,
  // so it's critical to init currStep before
  // calling GetEventText() for first time.
  if (GetRunAsVersion(GuidedTourVersion) <= _VERSION_0697_)
  {
    currStep=0;
    FormatDisplayText(textData, GetEventText());
  }
  else
  {
    currStep=-1;
  }

  currPic = GetEventPic();
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");
  if (useStartLoc)
  {
    if (   (tour_x < 0) || (tour_x >= levelData.area_width)
        || (tour_y < 0) || (tour_y >= levelData.area_height))
    {
      WriteDebugString("Invalid starting x,y in GUIDED_TOUR event\n");
      PopEvent();
      return;
    }
    party.Posx = tour_x;
    party.Posy = tour_y;
    party.facing = facing;
  }
  if (GetEventStep() != TStep_Pause) 
  {
    SetMyState(TASK_GuidedTourMoving);
    guidedTourMoving = TRUE;
    //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_GuidedTour,TOUR_TIMEOUT);
    SetEventTimer(TASKTIMER_GuidedTour,TOUR_TIMEOUT);
  }
  else
    SetMyState(TASK_GuidedTourPausing);
    guidedTourMoving = FALSE;
  //Invalidate();
}

void GUIDED_TOUR::OnKeypress(key_code key, char ascii)
{  
  if (TABParty(key)) return;
  if (GetMyState() != TASK_GuidedTourPausing)
    return;

  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  }

  TakeNextStep();
}

void GUIDED_TOUR::OnDraw()
{
  menu.setAllItemsActive();
  //if (GetMyState() == TASK_GuidedTourMoving)
  if (guidedTourMoving)
  {
    menu.setAllItemsInactive();
    UpdateGuidedTourMoveScreen('C');
  }
  else
    UpdateGuidedTourScreen('D');
}

bool GUIDED_TOUR::OnTimer(UINT timerId)
{
  if (timerId != TASKTIMER_GuidedTour)
    return false;
  TakeNextStep();
  return true;
}

void GUIDED_TOUR::TakeNextStep()
{
  currStep++;
  // skip invalid steps
  while (   (currStep < MAX_TOUR_STEPS)
         && (GetEventStep() == TStep_NoMove))
    currStep++;

  if (!TourOver())
  {
    switch (GetEventStep())
    {
    case TStep_Pause:  // display text only      
      FormatDisplayText(textData, GetEventText());
      SetMyState(TASK_GuidedTourPausing);
      guidedTourMoving = FALSE;
      Invalidate();
      break;
    case TStep_Forward:// move party forward and display text
#ifdef EARLY_WALL_SOUNDS
      {
        int slot=-1;
        switch (party.facing) 
        //switch (party.RelativeToActualFacing(party.PrevRelFacing)) 
        {      
        case FACE_NORTH:
          slot = levelData.area[party.Posy][party.Posx].wall[0];
          break;      
        case FACE_EAST:
          slot = levelData.area[party.Posy][party.Posx].wall[2];
          break;      
        case FACE_SOUTH:
          slot = levelData.area[party.Posy][party.Posx].wall[1];
          break;      
        case FACE_WEST:
          slot = levelData.area[party.Posy][party.Posx].wall[3];
          break;
        };
        if ((slot > 0) && (slot < MAX_WALLSETS))
          WallSets[slot].PlaySlotSound();
      };
#else
#endif
      party.movePartyForward(0);
      UpdatePartyMovementData();
      {
        // Added follosing 11 May2016 PRS
        StartBackgroundZoneOnlyMusic();
      };
      PushEvent(new PLAYSTEPSOUND_EVENT, DeleteEvent);
      SetMyState(TASK_GuidedTourMoving);
      guidedTourMoving = TRUE;
      break;
    case TStep_Left:   // move party left and display text
      party.turnPartyLeft();
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      SetMyState(TASK_GuidedTourMoving);
      guidedTourMoving = TRUE;
      break;
    case TStep_Right:  // move party right and display text
      party.turnPartyRight();
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      SetMyState(TASK_GuidedTourMoving);
      guidedTourMoving = TRUE;
      break;
    case TStep_NoMove: // shouldn't happen
      die(0xab4ed);
      break;
    }    
  }
  else
  {
    if (WaitUntilReturnPressed()) ClearFormattedText(textData);
    currPic.Clear();
    if (executeEvent)
      TaskMessage(TASKMSG_ExecuteEvent);
    ChainHappened();
    //local->ClearUserInput();    
  }
}

void GUIDED_TOUR::OnReturnToTopOfQueue(void)
{
  menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
  if (GetMyState() == TASK_GuidedTourMoving)
  {
    FormatDisplayText(textData, GetEventText());
    //  Seems redundant PRS 20170707  if (GetMyState() == TASK_GuidedTourMoving)
    {
      //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_GuidedTour,TOUR_TIMEOUT);
      SetEventTimer(TASKTIMER_GuidedTour,TOUR_TIMEOUT);
    }
    //else
    //{
    //  die("GUIDED_TOUR::Impossible state"); // Impossible!
    //};
  }
  Invalidate();
}

////////////////////////////////////////////////////////////////////////ENCOUNTER_DATA
void ENCOUNTER_DATA::OnInitialEvent(void)
{
  menu.reset(NULL);
  menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
  menu.setHorzOrient();
  FormatDisplayText(textData, GetEventText());


  currPic = GetEventPic();  
  //currPic.LoadPicSurfaces("sp_");
  currPic.LoadPicSurfaces("");



  if (currPic.picType == SpriteDib)
  {
    currDist = party.DetermineSpriteDistance(distance);


    currPic.SetFrame(currDist);
    currPic.timeDelay = 0; // encounters use manual approach
  }

  int count=0;
  int i;
  for (i=0;i<MAX_BUTTONS;i++)
  {
    if (buttons.buttons[i].label != "")
    {
      menu.addMenuItem(buttons.buttons[i].label);
      count++;
    }
    else
    {
      int newItem = menu.addMenuItem(" "); // placeholder
      menu.setItemInactive(newItem);
    }
  }
  menu.SetFirstLettersShortcuts();
  menu.setCurrentItem(0);  
  Invalidate();
  if (count == 0) ChainHappened();  
}

void ENCOUNTER_DATA::OnDraw(void)
{
  UpdateEncounterScreen('M');
}

void ENCOUNTER_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  int i;
  for (i=0;i<MAX_BUTTONS;i++)
  {
    if (!buttons.buttons[i].present)
      menu.setItemInactive(i+1);
  }
  if (currDist == UpClose)
  {
    for (i=0;i<MAX_BUTTONS;i++)
    {
      if (!buttons.buttons[i].allowedUpClose)
        menu.setItemInactive(i+1);
    }
  }
  else
  {
    for (i=0;i<MAX_BUTTONS;i++)
    {
      if (buttons.buttons[i].onlyUpClose)
        menu.setItemInactive(i+1);
    }
  }
}

void ENCOUNTER_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  }  

  bool finished = false;
  TASKMESSAGE msg=TASKMSG_None;

  // This event provides a list of choices (buttons) to the player with
  // a sprite pic displayed at a specified distance.
  //
  // The event stays active until a button with a chain event is selected,
  // or until the sprite pic range is reduced to 0=UpClose. Selecting a button
  // may do nothing, chain to an event, or bring the sprite closer to the 
  // party. If no action has been taken when the sprite reaches 0 range, the
  // event may exit or branch to one of the zero range event chains.
  //
  ENCOUNTER_BUTTON_OPTION &button = buttons.buttons[menu.currentItem()-1];

  int prevDist = currDist;
  switch (button.optionResult)
  {
  case NoResult:
    // change nothing
    break;

  case DecreaseRange:
    // move sprite pic a little closer to party
    if (currDist > 0)
    {
      currDist--;
      if (currPic.picType == SpriteDib)
        currPic.SetFrame(currDist);
    }    
    break;

  case ChainToEvent:
    finished = true;
    if (levelData.eventData.IsValidEvent(button.chain))
      ReplaceEvent(button.chain);
    else
      ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case CombatNoSurprise:
    finished = true;
    if (levelData.eventData.IsValidEvent(combatChain))
    {
      InsertEvent(combatChain);
      msg = TASKMSG_Combat_NoSurpise;
    }
    else
      ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case CombatSlowPartySurprised:    
    finished = true;
    if (levelData.eventData.IsValidEvent(combatChain))
    {        
      InsertEvent(combatChain);
      if (party.getPartySpeed() < monsterSpeed)
        msg = TASKMSG_Combat_PartySurpised;
      else
        msg = TASKMSG_Combat_NoSurpise;
    }
    else
      ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case CombatSlowMonsterSurprised:    
    finished = true;
    if (levelData.eventData.IsValidEvent(combatChain))
    {        
      InsertEvent(combatChain);
      if (party.getPartySpeed() > monsterSpeed)
        msg = TASKMSG_Combat_MonstersSurpised;
      else
        msg = TASKMSG_Combat_NoSurpise;
    }
    else
      ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case Talk:
    finished = true;
    if (levelData.eventData.IsValidEvent(talkChain))
      ReplaceEvent(talkChain);
    else
      ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case EscapeIfFastPartyElseCombat:    
    finished = true;
    if (party.getPartySpeed() >= monsterSpeed)
    {
      if (levelData.eventData.IsValidEvent(escapeChain))      
        ReplaceEvent(escapeChain);
      else
        ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    else
    {
      if (levelData.eventData.IsValidEvent(combatChain))      
      {
        InsertEvent(combatChain);
        msg = TASKMSG_Combat_NoSurpise;
      }
      else
        ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    break;
  default:
    WriteDebugString("Unhandled zeroRangeResult %i in encounter event\n",
                     zeroRangeResult);
    break;
  }

  // if parties have reached zero proximity.
  // UpClose must be displayed and allow user input at least once,
  // and this is tracked using prevDist.
  if ((!finished) && (prevDist == UpClose))
  {      
    switch (zeroRangeResult)
    {
    case NoResult:      
    case DecreaseRange: // has no meaning here
    case ChainToEvent:  // has no meaning here
      finished = true;
      ChainHappened();
      break;

    case CombatNoSurprise:
      finished = true;
      if (levelData.eventData.IsValidEvent(combatChain))
      {
        InsertEvent(combatChain);
        msg = TASKMSG_Combat_NoSurpise;
      }
      else
        ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      break;
    case CombatSlowPartySurprised:      
      finished = true;
      if (levelData.eventData.IsValidEvent(combatChain))
      {        
        InsertEvent(combatChain);
        if (party.getPartySpeed() < monsterSpeed)
          msg = TASKMSG_Combat_PartySurpised;
        else
          msg = TASKMSG_Combat_NoSurpise;
      }
      else
        ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      break;
    case CombatSlowMonsterSurprised:
      finished = true;
      if (levelData.eventData.IsValidEvent(combatChain))
      {        
        InsertEvent(combatChain);
        if (party.getPartySpeed() > monsterSpeed)
          msg = TASKMSG_Combat_MonstersSurpised;
        else
          msg = TASKMSG_Combat_NoSurpise;
      }
      else
        ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      break;
    case Talk:
      finished = true;
      if (levelData.eventData.IsValidEvent(talkChain))
        ReplaceEvent(talkChain);
      else
        ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      break;
    case EscapeIfFastPartyElseCombat:
      finished = true;
      if (party.getPartySpeed() > monsterSpeed)
      {
        if (levelData.eventData.IsValidEvent(escapeChain))      
          ReplaceEvent(escapeChain);
        else
          ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      else
      {
        if (levelData.eventData.IsValidEvent(combatChain))      
        {
          InsertEvent(combatChain);
          msg = TASKMSG_Combat_NoSurpise;
        }
        else
          ReplaceEvent(new DO_NOTHING_EVENT, DeleteEvent);
      }
      break;
    default:
      WriteDebugString("Unhandled zeroRangeResult %i in encounter event\n",
                       zeroRangeResult);
      break;
    }
  }

  if (finished)
  {
    currPic.Clear();
    if (WaitUntilReturnPressed()) ClearFormattedText(textData);
    if (msg != TASKMSG_None)
    {
      TaskMessage(msg);
      PopEvent();
    }
  }    
}

////////////////////////////////////////////////////////////////////////COMBAT_EVENT_DATA


void COMBAT_EVENT_DATA::OnEnterQueue(void)
{
  m_stoppedSounds = false; // We have not yet interrupted background sounds
  GameEvent::OnEnterQueue();
}

void COMBAT_EVENT_DATA::OnInitialEvent(void)
{  
  menu.reset(NULL);
  m_eTEMPsurprise = m_eSurprise;
  m_UseOutdoorMap = 
     ((globalData.levelInfo.stats[globalData.currLevel].overland) || (outdoors));

  // check to see if we need a combat welcome event
  if (   (GetEventText().GetLength()>0)
      || (GetEventPic().filename.GetLength()>0))
  {
    SetMyState(TASK_CombatWelcome);
    TEXT_EVENT_DATA *textEvent;
    textEvent=new TEXT_EVENT_DATA(false);
    textEvent->SetEventText(GetEventText());
    textEvent->HighlightText=FALSE;
    textEvent->pic = GetEventPic();
    if (autoApproach)
    {
      textEvent->WaitForReturn = TRUE;
      textEvent->distance = AutoFarAway;;

    }
    else
    {
      textEvent->WaitForReturn = TRUE;
      textEvent->distance = distance;
    };

    PushEvent(textEvent, DeleteEvent);
    return;
  }

  ClearFormattedText(textData);
  currPic.Clear();
  if (!LoadOffscreenCombatSurfaces(m_UseOutdoorMap))
  {
    WriteDebugString("LoadOffscreenCombatSurfaces failed\n");
    ChainHappened();
    return;
  }

  combatData.InitCombatData(this);
  //ChangeToTaskState(TASK_CombatMainMenu);  
  ChangeToTaskState(TASK_CombatStartBgMusic);
}

bool COMBAT_EVENT_DATA::OnTimer(UINT timerId)
{
  TASKSTATE state;
  state = GetMyState();
  switch (state)
  {
  case TASK_CombatDelay_AutoMove:
    if (timerId == TASKTIMER_CombatAutoMoveDelay)
    {
      ChangeToTaskState(TASK_CombatAutoMoving);
    }
    else
    {
      die(0x312ccb);
    };
    break;
  case TASK_CombatMainMenu:
    break;
  default:
    die(0xc54cc1);
  };
  //if (timerId != TASKTIMER_GuidedTour)
  //  return false;
  return true;
}



void COMBAT_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  m_curKey = key;
  // need to handle this regardless of state
  if (key == KC_SPACE)
  {
    if (     combatData.OnAuto(false)
         && (combatData.getActiveCombatCharacter().IsPartyMember())
       )
    {
      combatData.Quick(FALSE);
      PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      return;
    };
  };

  if (GetMyState() != TASK_CombatMainMenu)
    return;

  switch (key)
  {
  case KC_RIGHT:
  case KC_DOWN:
  case KC_UP:
  case KC_LEFT:
  case KC_NE:
  case KC_SE:
  case KC_SW:
  case KC_NW:
    ChangeToTaskState(TASK_CombatMoveMenu);
    return;
  default: break;
  };

  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  }
/*
   1  { "MOVE",    0 },
   2  { "AIM",     0 },
   3  { "USE",     0 },
   4  { "CAST",    0 },
   5  { "TURN",    0 },
   6  { "GUARD",   0 },
   7  { "QUICK",   0 },
   8  { "DELAY",   0 },
   9  { "BANDAGE", 0 },
   10 { "VIEW",    0 },
   11 { "SPEED",   1 },
   12 { "WIN",     0 },
   13 { "READY",   0 }
   14 { "END",     0 },
   15 { "SWEEP",   0 }
*/

  if (menu.currentItem() != 5)
  {
    combatData.turningSummary.Clear();
  };
  switch (menu.currentItem()) 
  {
  case 1: // move
    ChangeToTaskState(TASK_CombatMoveMenu);
    break;
  case 2: // aim
    ChangeToTaskState(TASK_CombatAimMenu);
    break;
  case 3: // use
    ChangeToTaskState(TASK_CombatUseMenu);
    break;
  case 4: // cast
    ChangeToTaskState(TASK_CombatCastMenu);
    break;
  case 5: // turn
    ChangeToTaskState(TASK_CombatTurnMenu);
    break;
  case 6: // guard
    ChangeToTaskState(TASK_CombatGuardMenu); //(TASK_CombatGuarding);
    break;
  case 7: // quick
    ChangeToTaskState(TASK_CombatQuickMenu);
    break;
  case 8: // delay
    ChangeToTaskState(TASK_CombatDelayMenu);//(TASK_CombatDelaying);
    break;
  case 9: // bandage
    ChangeToTaskState(TASK_CombatBandageMenu);//(TASK_CombatBandaging);
    break;
  case 10: // view
    ChangeToTaskState(TASK_CombatViewCharMenu);
    break;
  case 11: // speed
    ChangeToTaskState(TASK_CombatSpeedMenu);
    break;
  case 12: // win
    ChangeToTaskState(TASK_CombatForceWinMenu);    
    break;
  //case 13: // ready
  //  ChangeToTaskState(TASK_CombatScriptChoice);
  //  break;
  case 14: // end
    ChangeToTaskState(TASK_CombatEndTurnMenu); //(TASK_CombatEndingTurn);
    break;
  case 15: // specialAction
    ChangeToTaskState(TASK_CombatSpecialAction);
    break;
  default:
    WriteDebugString("Unhandled state in handleCombatMenu()\n");
    miscError = InvalidMenuState;
    Invalidate();
    break;
  }
}

void COMBAT_EVENT_DATA::ChangeToTaskState(TASKSTATE state)
{

  if (state == GetMyState())
    int kkk=1; //return;



  SetMyState(state);
  combatData.m_bCombatActive=TRUE;

#ifdef TraceFreeAttacks
  {
    if (combatData.IsFreeAttacker() || combatData.IsGuardAttacker())
    {
      CString msg;
      switch(state)
      {
      case TASK_CombatAttacking:                msg = "TASK_CombatAttacking";              break;
      case TASK_CombatDelay_DisplayAttackMsg:   msg = "TASK_CombatDelay_DisplayAttackMsg"; break;
      case TASK_CombatInRouteSpriteAnimating:   msg = "TASK_CombatInRouteSpriteAnimating"; break;
      case TASK_CombatDelay_AutoMove:           msg = "TASK_CombatDelay_AutoMove";         break;
      case TASK_CombatAutoMoving:               msg = "TASK_CombatAutoMoving";             break;
      case TASK_CombatAutoAttacking:            msg = "TASK_CombatAutoAttacking";          break;
      case TASK_CombatDelay_AutoAttackMsg:      msg = "TASK_CombatDelay_AutoAttackMsg";    break;
      case TASK_CombatDelay_NewCombatant:       msg = "TASK_CombatDelay_NewCombatant";     break;
      case TASK_CombatGuardMenu:                msg = "TASK_CombatGuardMenu";              break;
      case TASK_CombatGuarding:                 msg = "TASK_CombatGuarding";               break;
      default:                                  msg = "?????";                             break;
      };
      WriteDebugString("TFA - changing to %s\n", msg);
    };
  };
#endif


  switch (state)
  {
  case TASK_CombatStartBgMusic:
    if (   (pSndMgr != NULL) 
        && (PlayEventSounds) 
        && (bgSounds.backgroundSounds.sounds.GetCount()>0)
        && (pSndMgr->GetMusicEnable()))
    {    
      POSITION pos = bgSounds.backgroundSounds.sounds.GetHeadPosition();
      while (pos != NULL)
        pSndMgr->QueueBgndSound(bgSounds.backgroundSounds.sounds.GetNext(pos), TRUE);
    }  
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case TASK_CombatMainMenu:
    menu.setMenu(CombatMenuData, NULL, TRUE, this, "Combat");
    menu.setHorzOrient();    
    menu.setCurrentItem(1);
    //combatData.PlaceCursorOnCurrentDude();
    combatData.SetActiveCastingCharacter(NO_DUDE);
    Invalidate();
    {
      int curr = combatData.GetCurrCombatant();
      if (curr != NO_DUDE)
      {
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        ActorType actor;
        int index;
        COMBATANT *dude;
        CLASS_DATA *pClass;
        dude = combatData.GetCombatant(curr);
        dude->m_specialActionName = "";
        hookParameters[5] = "Menu";
        scriptContext.SetCombatantContext(dude);
        scriptContext.SetCharacterContext(dude->m_pCharacter);
        //pClass = &classData[dude->GetClass()];
        pClass = classData.GetClass(dude->GetClass());
        scriptContext.SetClassContext(pClass);
        dude->GetContext(&actor);
        SetCharContext(&actor);
        dude->RunCombatantScripts(
                          COMBAT_MAIN_MENU, 
                          ScriptCallback_RunAllScripts,
                          NULL,
                          "Combat main menu is being displayed");
        dude->m_specialActionName = hookParameters[6];
        index = -1;
        sscanf(hookParameters[7],"%d",&index);
        dude->m_specialActionIndex = index;
        ClearCharContext();
       
      };
    };



    break;
  case TASK_CombatAttacking:

    // ******** 21 Oct 2009
    // ******** PRS Trying to fix the free attack causes
    // ******** crash when target has disappeared.
    if (!combatData.StartAttack())
    {
//      WriteDebugString("DEBUG - Return to line 13836\n");
      if (combatData.IsFreeAttacker() || combatData.IsGuardAttacker())
      {
        overallCombatState OCS;
        combatData.EndTurn();
        OCS = combatData.GetCombatState();
        switch(OCS)
        {
        case OCS_NewCombatant:
          PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          //ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          return;
        };
        combatData.UpdateCombat();
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_NewCombatant:
          ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          return;
        case OCS_Attacking:
          SetMyState(TASK_CombatEndTurnMenu);
          break;
        case OCS_CombatOver:
          break;
        case OCS_StartNewRound:
          ChangeToTaskState(TASK_CombatStartingNewRound);
          return;
        default:
          die(0xc500660);
        };
      //SetMyState(TASK_CombatDelay_NewCombatant);
      }
      else
      {
        combatData.SetCombatantState(ICS_Guarding);
        SetMyState(TASK_CombatGuarding);
        //die(0x400398b);
      };
    }
    else  // StartAttack was successful
    {
      {
        // Inserted 11May2016 due to infinite free attacks PRS 
        if (combatData.QComb.NumFreeAttacks() > 0) 
        {
          combatData.QComb.DecrementFreeAttacks();
        }
        else if (combatData.QComb.NumGuardAttacks() > 0)
        {
          combatData.QComb.DecrementGuardAttacks();
        };
      };

    };
    //  ********* PRS 21 OCt 2009
//    WriteDebugString("DEBUG - Return to line 13875\n");
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case TASK_CombatAutoAttacking:

    // ******** 21 Oct 2009
    // ******** PRS Trying to fix the free attack causes
    // ******** crash when target has disappeared.
    if (!combatData.StartAttack())
    {
//      WriteDebugString("DEBUG - Return to line 13885\n");
      SetMyState(TASK_CombatGuarding);
    };
    //  ********* PRS 21 OCt 2009
//    WriteDebugString("DEBUG - Return to line 13889\n");
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case TASK_CombatViewCharMenu:
    PushEvent(new VIEW_CHARACTER_DATA(GetEventType(),this), DeleteEvent);
    break;
  case TASK_CombatScriptChoice:
    PushEvent(new SCRIPT_CHOICE_DATA, DeleteEvent);
    break;
  case TASK_CombatQuickMenu:
    combatData.Quick(TRUE);
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;
  case TASK_CombatSpeedMenu:
    PushEvent(new GAME_SPEED_MENU_DATA(this), DeleteEvent);
    break;
  case TASK_CombatAimMenu: // non-spell target selection
    PushEvent(new COMBAT_AIM_MENU_DATA(), DeleteEvent);
    break;

  case TASK_CombatCastMenu: 
    combatData.SetActiveCastingCharacter(combatData.GetCurrCombatant());
    PushEvent(new CAST_MENU_DATA(this, CAST_ENV_COMBAT), DeleteEvent);
    break;
  case TASK_CombatBeginCasting:
    {
      // spell is selected, now enter casting state
      COMBATANT &dude = combatData.getActiveCombatCharacter();
      combatData.SetActiveCastingCharacter(dude.self);
      //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
      //GLOBAL_SPELL_ID secondaryKey = dude.GetSecondarySpellKeyBeingCast();
      SPELL_ID spellID = dude.GetSpellIDBeingCast();
      SPELL_ID secondarySpellID = dude.GetSecondarySpellIDBeingCast();
      //combatData.StartInitialSpellCasting(key, secondaryKey); 
      combatData.StartInitialSpellCasting(spellID, secondarySpellID); 
      //key = dude.GetSpellKeyBeingCast();
      spellID = dude.GetSpellIDBeingCast();
      //if (key.IsValidSpell())
      if (spellID.IsValidSpell())
      {
        PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), 
                                          (overallCombatState)combatData.GetCombatantState(),""), 
                                          DeleteEvent);    
      }
      else
      {
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      };
    }
    break;

  

  case TASK_CombatBeginItemCasting:
    {
      // spell is selected, now enter casting state
      COMBATANT &dude = combatData.getActiveCombatCharacter();
      combatData.SetActiveCastingCharacter(dude.self);
      //GLOBAL_SPELL_ID key = dude.GetItemSpellKeyBeingCast();
      SPELL_ID spellID = dude.GetItemSpellIDBeingCast();
      //combatData.StartInitialItemSpellCasting(key); 
      combatData.StartInitialItemSpellCasting(spellID); 
      //key = dude.GetItemSpellKeyBeingCast();
      spellID = dude.GetItemSpellIDBeingCast();
      //if (key.IsValidSpell())
      if (spellID.IsValidSpell())
      {
        PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(),
                                          (overallCombatState)combatData.GetCombatantState(),""), 
                                          DeleteEvent);    
      }
      else
      {
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
      };
    }
    break;


  
  
  case TASK_CombatSelectSpellTargets:
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        //GLOBAL_SPELL_ID gsID, secondarySpell;
        //gsID = dude->GetEffectiveSpellKeyBeingCast();
        SPELL_ID spellID;
        spellID = dude->GetEffectiveSpellIDBeingCast();
        PushEvent(new CAST_COMBAT_SPELL_MENU_DATA(GetEventType(),
                                                  this,
                                                  dude, 
                                                  //gsID),
                                                  spellID),
                  DeleteEvent);
      }
      else
        ChangeToTaskState(TASK_CombatActivateSpell);
    }
    break;
        
  case TASK_CombatActivateSpellMsg:
    {
      COMBATANT &dude = combatData.getActiveCombatCharacter();
      combatData.SetActiveCastingCharacter(dude.self);
      PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), OCS_ActivateSpell,""), DeleteEvent);
    }
    break;

  case TASK_CombatActivateSpell:
    {
      //COMBATANT &dude = combatData.getActiveCombatCharacter();
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      BOOL MoreTargets = FALSE;

      if (dude != NULL)
      {
        // caster may have killed himself with spell!
        if (dude->charOnCombatMap(false, true))
        {
#ifdef newCombatant
          if (dude->m_pCharacter->targets.m_SelectingUnits)
            MoreTargets = (dude->m_pCharacter->targets.NumTargets()>0);
          else
            MoreTargets = ((dude->m_pCharacter->targets.m_MapTargetX>=0)&&(dude->m_pCharacter->targets.m_MapTargetY>=0));
#else
          if (dude->targets.m_SelectingUnits)
            MoreTargets = (dude->targets.NumTargets()>0);
          else
            MoreTargets = ((dude->targets.m_MapTargetX>=0)&&(dude->targets.m_MapTargetY>=0));
#endif

          if (MoreTargets)
          {        
            ChangeToTaskState(TASK_CombatSpellMissileAnimate);
          }
          else
          {
#ifdef SpellInitiationScript
            if (dude->combatant_activeSpellKey < 0)
            { // Else removing the activeSpellList entry will do this for us
              if (dude->combatant_activeSpellKey != -2)
              {
                die(0xab528);  //The SpellInitiationScript was not run
              };
              SPELL_ID spellID;
              spellID = dude->GetEffectiveSpellIDBeingCast();
              if (spellID.IsValidSpell())
              {
                const SPELL_DATA *pSpell;
                pSpell = spellData.PeekSpell(spellID);
                pSpell->ExecuteSpellTerminationScript(dude->m_pCharacter);
              };
            };
            dude->combatant_activeSpellKey=-1;
#endif
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          }
        }
        else
        {
          dude->combatant_activeSpellKey=-1;
          PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
        }
      }
      else
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    break;

  case TASK_CombatActivateRedirectedSpell:
    {
      //COMBATANT &dude = combatData.getActiveCombatCharacter();
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      BOOL MoreTargets = FALSE;

      if (dude != NULL)
      {
        // caster may have killed himself with spell!
        if (dude->charOnCombatMap(false, true))
        {
#ifdef newCombatant
          if (dude->m_pCharacter->targets.m_SelectingUnits)
            MoreTargets = (dude->m_pCharacter->targets.NumTargets()>0);
          else
            MoreTargets = ((dude->m_pCharacter->targets.m_MapTargetX>=0)&&(dude->m_pCharacter->targets.m_MapTargetY>=0));
#else
          if (dude->targets.m_SelectingUnits)
            MoreTargets = (dude->targets.NumTargets()>0);
          else
            MoreTargets = ((dude->targets.m_MapTargetX>=0)&&(dude->targets.m_MapTargetY>=0));
#endif

          if (MoreTargets)
          {        
            ChangeToTaskState(TASK_CombatSpellMissileAnimate);
          }
          else
          {
            dude->combatant_activeSpellKey=-1;
            PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
          }
        }
        else
        {
          dude->combatant_activeSpellKey=-1;
          PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
        }
      }
      else
        PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    }
    break;

  case TASK_CombatSpellCastAnimate:
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      
      if (dude!=NULL)
      {
        // go ahead with spell animations for each target
        dude->forceAttackPose=TRUE;
        if (dude->m_pCharacter->targets.m_SelectingUnits)
        {
          if (dude->m_pCharacter->targets.NumTargets()==0)
          {
            // spell aborted, no targets chosen
            ChangeToTaskState(TASK_CombatActivateSpell);
          }
          else
          {
            int targ = dude->m_pCharacter->targets.m_targets[0];
            PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::CAST,""), DeleteEvent);          
          }
        }
        else // targeting a map location
        {
          if ((dude->m_pCharacter->targets.m_MapTargetX<0)||(dude->m_pCharacter->targets.m_MapTargetY<0))
          {
            // spell aborted, no targets chosen
            ChangeToTaskState(TASK_CombatActivateSpell);
          }
          else
          {
            PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self,
                                                           dude->m_pCharacter->targets.m_MapTargetX, 
                                                           dude->m_pCharacter->targets.m_MapTargetY, 
                                                           COMBAT_SPELL_SPRITE_ANIMATE_DATA::CAST,""),
                      DeleteEvent);
          }
        }
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;

  case TASK_CombatSpellMissileAnimate: 
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude != NULL)
      {
        dude->forceAttackPose=TRUE; // turn on attack pose while casting
        if (dude->m_pCharacter->targets.m_SelectingUnits)
        {
          int targ = dude->m_pCharacter->targets.m_targets[0];
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::MISSILE,""), DeleteEvent);          
        }
        else // targeting a map location
        {
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self,
                                                         dude->m_pCharacter->targets.m_MapTargetX,
                                                         dude->m_pCharacter->targets.m_MapTargetY,
                                                         COMBAT_SPELL_SPRITE_ANIMATE_DATA::MISSILE,""),
                    DeleteEvent);
        }
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;

  case TASK_CombatSpellRedirectedMissileAnimate: 
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude != NULL)
      {
        dude->forceAttackPose=TRUE; // turn on attack pose while casting
        if (dude->m_pCharacter->targets.m_SelectingUnits)
        {
          int targ = dude->m_pCharacter->targets.m_targets[0];
          COMBAT_SPELL_SPRITE_ANIMATE_DATA *pGameEvent;
          pGameEvent = new COMBAT_SPELL_SPRITE_ANIMATE_DATA(
                                             dude->self, 
                                             targ, 
                                             COMBAT_SPELL_SPRITE_ANIMATE_DATA::REDIRECTED_MISSILE,
                                             combatData.m_sRedirectedSpellMessage);
          pGameEvent->SourceCombatant(targ);
          pGameEvent->DestinationCombatant(combatData.m_iRedirectedSpellTarget);
          PushEvent(pGameEvent, DeleteEvent);          
        }
        else // targeting a map location
        {
          ChangeToTaskState(TASK_CombatSpellFinalStage);
          //PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, dude->targets.m_MapTargetX, dude->targets.m_MapTargetY, COMBAT_SPELL_SPRITE_ANIMATE_DATA::MISSILE,""), DeleteEvent);
        }
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;

  case TASK_CombatSpellCoverageAnimate:
    {      
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        dude->forceAttackPose=FALSE; // turn off attack pose
        if (dude->m_pCharacter->targets.m_SelectingUnits)
        {
          int targ = dude->m_pCharacter->targets.m_targets[0];
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::COVERAGE,""), DeleteEvent);          
        }
        else // targeting a map location
        {
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self,
                                                         dude->m_pCharacter->targets.m_MapTargetX,
                                                         dude->m_pCharacter->targets.m_MapTargetY,
                                                         COMBAT_SPELL_SPRITE_ANIMATE_DATA::COVERAGE,""),
                    DeleteEvent);
        }
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;
    
  case TASK_CombatRedirectedSpellCoverageAnimate:
    {      
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        dude->forceAttackPose=FALSE; // turn off attack pose
#ifdef newCombatant
        if (dude->m_pCharacter->targets.m_SelectingUnits)
#else
        if (dude->targets.m_SelectingUnits)
#endif
        {
          //int targ = dude->targets.m_targets[0];
          int targ = combatData.m_iRedirectedSpellTarget;
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::COVERAGE,""), DeleteEvent);          
        }
        else // targeting a map location
        {
#ifdef newCombatant
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self,
                                                         dude->m_pCharacter->targets.m_MapTargetX,
                                                         dude->m_pCharacter->targets.m_MapTargetY,
                                                         COMBAT_SPELL_SPRITE_ANIMATE_DATA::COVERAGE,""),
                    DeleteEvent);
#else
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, dude->targets.m_MapTargetX, dude->targets.m_MapTargetY, COMBAT_SPELL_SPRITE_ANIMATE_DATA::COVERAGE,""), DeleteEvent);
#endif
        }
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;
    
  case TASK_CombatSpellLingerAnimate:
    {
      //COMBATANT &dude = combatData.getActiveCombatCharacter();
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        if (dude->m_pCharacter->targets.m_SelectingUnits)
        {
          int targ = dude->m_pCharacter->targets.m_targets[0];
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::LINGER,""), DeleteEvent);          
        }
        else // targeting a map location
        {
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self,
                                                         dude->m_pCharacter->targets.m_MapTargetX,
                                                         dude->m_pCharacter->targets.m_MapTargetY,
                                                         COMBAT_SPELL_SPRITE_ANIMATE_DATA::LINGER,""),
                    DeleteEvent);
        }
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;

  case TASK_CombatRedirectedSpellLingerAnimate:
    {
      //COMBATANT &dude = combatData.getActiveCombatCharacter();
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
#ifdef newCombatant
        if (dude->m_pCharacter->targets.m_SelectingUnits)
        {
          int targ = combatData.m_iRedirectedSpellTarget;
#else
        if (dude->targets.m_SelectingUnits)
        {
          int targ = combatData.m_iRedirectedSpellTarget;
#endif
          //int targ = dude->targets.m_targets[0];
          PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::LINGER,""), DeleteEvent);          
        }
        //else // targeting a map location
        //{
        //  PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, dude->targets.m_MapTargetX, dude->targets.m_MapTargetY, COMBAT_SPELL_SPRITE_ANIMATE_DATA::LINGER,""), DeleteEvent);
        //}
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;

  case TASK_CombatSpellHitAnimate:
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        if (dude->charOnCombatMap(false, true))
        {
          if (dude->m_pCharacter->targets.m_targets.GetSize() > 0)
          {
            CString msg;
            int targ = dude->m_pCharacter->targets.m_targets[0];
            if (!combatData.m_sSpellEffectMessage.IsEmpty())
            {
              msg = FormatSpellHitText(combatData.getCombatantPtr(targ), &combatData.m_sSpellEffectMessage);
              combatData.m_sSpellEffectMessage.Empty();
            }
            else
            {
              msg = FormatSpellHitText(combatData.getCombatantPtr(targ), NULL);
            };
            PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::HIT,msg), DeleteEvent);          
          }
          else
            ChangeToTaskState(TASK_CombatSpellFinalStage);
        }
        else
          ChangeToTaskState(TASK_CombatSpellFinalStage);
      }
      else
        ChangeToTaskState(TASK_CombatSpellFinalStage);
    }
    break;

  case TASK_CombatRedirectedSpellHitAnimate:
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        if (dude->charOnCombatMap(false, true))
        {
#ifdef newCombatant
          if (dude->m_pCharacter->targets.m_targets.GetSize() > 0)
#else
          if (dude->targets.m_targets.GetSize() > 0)
#endif
          {
            CString msg;
            //int targ = dude->targets.m_targets[0];
            int targ = combatData.m_iRedirectedSpellTarget;
            if (!combatData.m_sSpellEffectMessage.IsEmpty())
            {
              msg = FormatSpellHitText(combatData.getCombatantPtr(targ), &combatData.m_sSpellEffectMessage);
              combatData.m_sSpellEffectMessage.Empty();
            }
            else
            {
              msg = FormatSpellHitText(combatData.getCombatantPtr(targ), NULL);
            };
            PushEvent(new COMBAT_SPELL_SPRITE_ANIMATE_DATA(dude->self, targ, COMBAT_SPELL_SPRITE_ANIMATE_DATA::HIT,msg), DeleteEvent);          
          }
          else
            ChangeToTaskState(TASK_CombatRedirectedSpellFinalStage);
        }
        else
          ChangeToTaskState(TASK_CombatRedirectedSpellFinalStage);
      }
      else
        ChangeToTaskState(TASK_CombatRedirectedSpellFinalStage);
    }
    break;

  case TASK_CombatSpellFinalStage:
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatRedirectedSpellFinalStage:
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatUseMenu:
     PushEvent(new ITEMS_MENU_DATA(GetEventType(),this,NULL, NULL, 0),DeleteEvent);
     break;
  case TASK_CombatUsing:
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatMoveMenu:
    PushEvent(new COMBAT_MOVE_MENU_DATA(this, m_curKey), DeleteEvent);
    break;   
  case TASK_CombatMoving:
    menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL);
    menu.setTitle(FormatCombatMoveText());
    break;

  case TASK_CombatAutoMoving:
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatDelay_AutoMove:
    {
      DWORD interval = GetUserGameSpeed()*120;
      if (interval < 210) interval = 210;
      if (interval > 990) interval = 990;
      //if (FastPlayback()) interval = 1;
      SetEventTimer(TASKTIMER_CombatAutoMoveDelay,interval);
    };
    break;
  case TASK_CombatTurnMenu: 
    combatData.StartTurnUndead(); 
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), 
                                      (overallCombatState)combatData.GetCombatantState(),""), 
                                      DeleteEvent);
    break;
  case TASK_CombatTurning:  
    combatData.TurnUndead();    
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatGuardMenu: 
    combatData.Guard();
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), 
                                      (overallCombatState)combatData.GetCombatantState(),""), 
                                      DeleteEvent);
    break;
  case TASK_CombatGuarding:    
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatBandageMenu:
    combatData.Bandage();
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), 
                                      (overallCombatState)combatData.GetCombatantState(),""),
                                      DeleteEvent);
    break;
  case TASK_CombatBandaging:   
    combatData.EndTurn();
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatDelayMenu:     
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), OCS_Delaying,""), DeleteEvent);
    break;
  case TASK_CombatDelaying:  
    combatData.DelayAction();
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatEndTurnMenu: 
    // leave the End Turn delay at the default game speed of
    // 3 * 210 so that the last round of combat where each character
    // gets an extra turn doesn't take so long to cycle through.
    PushEvent(new TIME_DELAY_MSG_DATA(3*210, OCS_EndingTurn,""), DeleteEvent);
    break;
  case TASK_CombatTurnEnded:
    combatData.EndTurn();
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatForceWinMenu:
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), OCS_ForcingWin,""), DeleteEvent);
    break;
  case TASK_CombatForcingWin:
    MouseRender.ShowMouseCursor(FALSE);
    combatData.ForcePartyVictory();
    combatData.EndTurn();
    MouseRender.ShowMouseCursor(TRUE);
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
    break;

  case TASK_CombatInRouteSpriteAnimating:
    combatData.InitMissileAnimation();
    PushEvent(new COMBAT_SPRITE_ANIMATE_DATA(), DeleteEvent);
    break;

  case TASK_CombatHitSpriteAnimating:
  case TASK_CombatAutoHitSpriteAnimating:
    combatData.InitHitAnimation();
    PushEvent(new COMBAT_SPRITE_ANIMATE_DATA(), DeleteEvent);
    break;

  case TASK_CombatDelay_DisplayAttackMsg:
  case TASK_CombatDelay_AutoAttackMsg:
//  case TASK_CombatTimeDelayMsg:
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), 
                                      (overallCombatState)combatData.GetCombatantState(),""), 
                                      DeleteEvent);
    break;

  case TASK_CombatAttacking_DisplayAttacker:
//  case TASK_CombatTimeDelayMsg:
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), OCS_DisplayingAttacker,""), DeleteEvent);
    break;


  case TASK_CombatStartingNewRound:
  case TASK_CombatDelay_NewCombatant:
    PushEvent(new COMBAT_ROUND_DELAY_DATA(600), DeleteEvent);
    break;

  case TASK_CombatOver:
    PushEvent(new COMBAT_RESULTS_MENU_DATA(this), DeleteEvent);
    break;

  case TASK_CombatSpecialAction:
    PushEvent(new COMBAT_SPECIAL_ACTION_DATA(), DeleteEvent);
    break;

  default:
    die(0xab4ee);
    break;
  }
}

void CreateSortedMapList(COMBATANT *pCombatant, DWORD **list, int *listSize)
{
  POSITION pos;
  POINT point;
  int i;
  *listSize = pCombatant->m_pCharacter->targets.m_maptarglist.GetCount();
  *list = (DWORD *)malloc(*listSize * sizeof(DWORD));
  pos = pCombatant->m_pCharacter->targets.m_maptarglist.GetHeadPosition();
  for (i=0; i<*listSize; i++)
  {
    point = pCombatant->m_pCharacter->targets.m_maptarglist.GetNext(pos);
    (*list)[i] = (point.x << 16) + point.y;
  };
  // Now, sort the list.
  {
    DWORD temp;
    int e;
    int j, k, n;
    e = (*listSize);  // The last entry
    for (n = (*listSize)/2; n>0; n/=2)
    {
      for (i=0; i<n; i++)
      {
        for (j=i; j<e-n; j+=n)
        {
          if ((*list)[j] > (*list)[j+n])
          {
            k = j;
            do
            {
              temp = (*list)[k];
              (*list)[k] = (*list)[k+n];
              (*list)[k+n] = temp;
              k -= n;
            } while ((k>=0) && ((*list)[k] > (*list)[k+n]));
          };
        };
      };
    };
  };
  return;
}

bool BinSearch(DWORD value, DWORD *list, int listSize)
{
  int m;
  while (listSize > 1)
  {
    m = listSize/2;
    if (value >= list[m])
    {
      list += m;
      listSize = listSize-m;
    }
    else
    {
      listSize = m;
    };
  };
  
  return value == *list;
}

void COMBAT_EVENT_DATA::CheckForSpellLingeringScripts(void)
{
  int dude;
  COMBATANT *pCaster;
  SPELL_DATA *pSdata;
  int casterLevel;
  dude = combatData.GetCurrCombatant();
  pCaster = combatData.GetCombatant(dude);
  //GLOBAL_SPELL_ID gsID;
  //gsID = pCaster->GetSpellKeyBeingCast();
  SPELL_ID spellID;
  spellID = pCaster->GetSpellIDBeingCast();
  //if (!gsID.IsValidSpell()) return;
  if (!spellID.IsValidSpell())
  {
    spellID = pCaster->GetItemSpellIDBeingCast();
    if (!spellID.IsValidSpell()) return;
  };
  casterLevel = pCaster->m_pCharacter->m_spellCastingLevel;
  //pSdata = spellData.GetSpellData(gsID);
  pSdata = spellData.GetSpell(spellID);
  {
    HOOK_PARAMETERS hookParameters;
    CString scriptName;
    SCRIPT_CONTEXT scriptContext;
    scriptContext.SetSpellContext(pSdata);
    scriptContext.SetCombatantContext(pCaster);
    hookParameters[5] = pSdata->uniqueName;
    hookParameters[6].Format("%d", casterLevel);
    scriptName = pSdata->RunSpellScripts(
                        SPELL_LINGERING_SCRIPTS,
                        ScriptCallback_RunAllScripts, 
                        NULL,
                        "Combat check for lingering spell scripts");
    // Save hook parameters for scripts to be called.   ClearHookParameters();
    scriptContext.Clear();
    if (!scriptName.IsEmpty())
    {
      POSITION pos;
      DWORD *sortedMapList = NULL;
      int   numSortedMapList = 0;  // Make compiler happy
      
      for (pos = activeSpellList.GetHeadPosition(); pos != NULL;)
      {
        ACTIVE_SPELL *pActiveSpell;
        pActiveSpell = &activeSpellList.GetNext(pos);
        if (pActiveSpell->lingerData.mapdata.GetCount() > 0)
        {
          CString result;
          SPELL_DATA *pSpell;
          //GLOBAL_SPELL_ID gsID;
          //gsID = pActiveSpell->spellKey;
          //pSpell = spellData.GetSpellData(gsID);
          SPELL_ID activeSpellID;
          activeSpellID = pActiveSpell->spellID;
          pSpell = spellData.GetSpell(activeSpellID);
          scriptContext.SetSpellContext(pSpell);
          hookParameters[7].Format("%d", pActiveSpell->casterLevel);
          result = pSpell->RunSpellScripts(scriptName,
                                           ScriptCallback_RunAllScripts,
                                           NULL,
                                           "Combat check for lingering spell scripts");
          if (!result.IsEmpty())
          {
            // I am not anxious to work my way through the map lists
            // N-squared times.  So I will make a list of the source
            // spell's map squares, sort the list, and do binary searches
            // on the result.  But only at the last moment, if necessary.

            
            if (result == "ALL")
            {
              continue;
            }
            if (result == "AREA")
            {
              POSITION tmp, dataPos;
              if (sortedMapList == NULL) 
              {
                CreateSortedMapList(pCaster, &sortedMapList, &numSortedMapList);
              };
              // For each of *pActiveSpell's map locations
              // {
              //   if (location is in sorted list)
              //   {
              //     delete location from pActiveSpell's map list
              //   };
              // };
              for (dataPos = pActiveSpell->lingerData.mapdata.GetHeadPosition();
                   dataPos != NULL;
                  )
              {
                DWORD lingerLocation;
                POINT point;
                tmp = dataPos;
                point = pActiveSpell->lingerData.mapdata.GetNext(dataPos);
                lingerLocation = (point.x << 16) | point.y;
                if (BinSearch(lingerLocation, sortedMapList, numSortedMapList))
                {
                  pActiveSpell->lingerData.mapdata.RemoveAt(tmp);
                };
                // Now remove any of *pCombatSprite's graphics at this location.
              };
              // Now remove any of *pCombatSprite's graphics at this location.
              // Search the 'AnimateList' for any animations due to this ACTIVE_SPELL
                   dataPos = AnimateList.GetHeadPosition();
              while (dataPos != NULL)
              {
                COMBAT_SPRITE_DATA *pCombatSpriteData;
                tmp = dataPos;
                pCombatSpriteData = AnimateList.GetNext(dataPos);
                if (pCombatSpriteData->activeSpellKey == pActiveSpell->key)
                {
                  DWORD lingerLocation;
                  lingerLocation = (pCombatSpriteData->stx << 16) | pCombatSpriteData->sty;
                  if (BinSearch(lingerLocation, sortedMapList, numSortedMapList))
                  {
                    AnimateList.RemoveAt(tmp);
                  };
                };
              };
              continue;
            };
            if (result == "TARGETS")
            {
              continue;
            };
          };
        };
      };
      if (sortedMapList != NULL)
      {
        free (sortedMapList);
        sortedMapList = NULL;
      };
    };
  };
}

_int64 lastVirtualGameTime = 0;

void COMBAT_EVENT_DATA::OnReturnToTopOfQueue(void)
{
  //overallCombatState OCS;
  TASKSTATE state;
//  if (combatData.m_bPlayerTypedSpace)
//  {
//    if (     combatData.OnAuto()
//         && (combatData.getActiveCombatCharacter().IsPartyMember())
//       )
//    {
//      combatData.Quick(false);
//    };
//    combatData.m_bPlayerTypedSpace = FALSE;
//  };
  lastVirtualGameTime = virtualGameTime;
  state = GetMyState();
//  WriteDebugString("DEBUG - Return to line 14751 state = %d\n", state);
  switch (state)
  {
  case TASK_CombatWelcome:
    ClearFormattedText(textData);
    currPic.Clear();

    m_stoppedSounds = true;
    if (!LoadOffscreenCombatSurfaces(m_UseOutdoorMap))
    {
      WriteDebugString("LoadOffscreenCombatSurfaces failed\n");
      ChainHappened();
      return;
    }

    combatData.InitCombatData(this);


    ChangeToTaskState(TASK_CombatStartBgMusic);
    break;

  case TASK_CombatStartBgMusic:
    pSndMgr->PlayBgndQueue();
    //combatData.UpdateCombat();  // Get the first combatant ready
    {
      overallCombatState OCS;
      OCS = combatData.GetCombatState();
      switch (OCS)
      {
      case OCS_StartNewRound:
        TRACE("Start new round delay\n");
        TRACE("Round delay\n");
        combatData.m_bStartingNewCombatRound = FALSE;
        combatData.m_forceRoundDelay = FALSE;
        ChangeToTaskState(TASK_CombatStartingNewRound);
        break;
      default: die(0x49caded);
      };
    };
    break;




    //ChangeToTaskState(TASK_CombatMainMenu);
    //break;

  case TASK_CombatMainMenu:
    // do nothing
  {
    overallCombatState OCS;
    OCS = combatData.GetCombatState();
    switch (OCS)
    {
    case OCS_Fled:
    {
      combatData.UpdateCombat();
      OCS = combatData.GetCombatState();
      switch (OCS)
      {
      case OCS_NewCombatant:
        ChangeToTaskState(TASK_CombatDelay_NewCombatant);
        break;
      default:
        die(0x31a11b);
        break;
      };
    };
    break;
    case OCS_NewCombatant:
      combatData.UpdateCombat();
      OCS = combatData.GetCombatState();
      switch (OCS)
      {
      case OCS_Guarding:
      {
        ChangeToTaskState(TASK_CombatGuardMenu);
      };
      break;
      case OCS_None:
      {
        ChangeToTaskState(TASK_CombatMainMenu);
        break;
        //    combatData.UpdateCombat();
        //    OCS = combatData.GetCombatState();
        //    switch(OCS)
        //    {
        //    default:
        //      die(0x21affd);
        //    };
      };
      default:
        die(0xc449abc);
      };
    default:
      break;
    };
  };
    break;

  case TASK_CombatViewCharMenu: 
    /// if using item caused spell to be cast, then begin casting process
    //if (!combatData.getActiveCombatCharacter().GetItemSpellKeyBeingCast().IsValidSpell())
    if (!combatData.getActiveCombatCharacter().GetItemSpellIDBeingCast().IsValidSpell())
      ChangeToTaskState(TASK_CombatMainMenu); 
    else
      ChangeToTaskState(TASK_CombatBeginItemCasting); 
    break;
    //ChangeToTaskState(TASK_CombatMainMenu);
    //break;
  case TASK_CombatQuickMenu:
    {
      overallCombatState OCS;
      combatData.UpdateCombat();
      OCS = combatData.GetCombatState();
      if (combatData.OnAuto(false))
      {
        int updateScreen;
        // moves dude, or can end turn
        updateScreen = combatData.HandleCurrState(FALSE);
        if (updateScreen != 0) 
        {
          OnUpdateUI();
          OnDraw();
          ChangeToTaskState(TASK_CombatDelay_AutoMove);
        }
        else
        {
          int dude;
          dude = combatData.GetCurrCombatant();
          if (dude == NO_DUDE)
          {
            combatData.UpdateCombat();
          };
          OCS = combatData.GetCombatState();
          switch (OCS)
          {
          case OCS_Attacking:
            if ((GetMyState() != TASK_CombatInRouteSpriteAnimating) && (combatData.NeedMissileAnimation()))
            {
              ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
            }
            else
            {
              ChangeToTaskState(TASK_CombatAutoAttacking);
            };
            break;
          case OCS_Guarding:
            {
              ChangeToTaskState(TASK_CombatGuardMenu);
            };
            break;
          case OCS_Casting:
            {
              COMBATANT &combatant = combatData.getActiveCombatCharacter();
              //GLOBAL_SPELL_ID key = combatant.GetSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              SPELL_ID spellID = combatant.GetSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                //key = combatant.GetItemSpellKeyBeingCast();
                //if (!key.IsValidSpell())
                spellID = combatant.GetItemSpellIDBeingCast();
                if (!spellID.IsValidSpell())
                {
                  spellID = combatant.GetItemSpellIDBeingCast();
                  if (!spellID.IsValidSpell())
                  {
                    combatData.EndTurn();
                    ChangeToTaskState(TASK_CombatMainMenu);         
                  }
                  else
                  {   
                    // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                    /* Really */ NotImplemented(0x5510b, false);
                    ChangeToTaskState(TASK_CombatBeginItemCasting);
                  };
                }
                else  
                {   
                  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                };
              }
              else  
              {   
                ChangeToTaskState(TASK_CombatActivateSpellMsg);
              }
            }
            break;
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            break;
          case OCS_StartNewRound:
            ChangeToTaskState(TASK_CombatStartingNewRound);
            break;
          default:
            die(0x45cb2);
          };
        };
      }
      else
      {
        switch(OCS)
        {
        case OCS_None:
          ChangeToTaskState(TASK_CombatMainMenu);
          break;
        default: 
          die(0xc62d69);
        };
      };
    };
    break;
  case TASK_CombatSpeedMenu: ChangeToTaskState(TASK_CombatMainMenu); break;

  case TASK_CombatMoving: ChangeToTaskState(TASK_CombatMainMenu); break;

  case TASK_CombatAutoMoving:
    {
      combatData.UpdateCombat();
      if (combatData.OnAuto(true))
      {
        int updateScreen;
        // moves dude, or can end turn
        updateScreen = combatData.HandleCurrState(FALSE);
        if (updateScreen != 0) 
        {
          overallCombatState OCS;
          OnUpdateUI();
          OnDraw();
          OCS = combatData.GetCombatState();
          switch(OCS)
          {
          case OCS_Moving:
            ChangeToTaskState(TASK_CombatDelay_AutoMove);
            break;
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            break;
          case OCS_Attacking: // auto
            if (combatData.NeedMissileAnimation())
            {
              ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
            }
            else
            {
              ChangeToTaskState(TASK_CombatAttacking);
            };
            break;;
          default:
            die(0x90098);
          };
        }
        else
        {
          int dude;
          overallCombatState OCS;
          dude = combatData.GetCurrCombatant();
          if (dude == NO_DUDE)
          {
            combatData.UpdateCombat();
          };
          OCS = combatData.GetCombatState();
          switch (OCS)
          {
          case OCS_Attacking:
            if ((GetMyState() != TASK_CombatInRouteSpriteAnimating) && (combatData.NeedMissileAnimation()))
            {
              ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
            }
            else
            {
              ChangeToTaskState(TASK_CombatAutoAttacking);
            };
            break;
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            break;
          case OCS_StartNewRound:
            ChangeToTaskState(TASK_CombatStartingNewRound);
            break;
          case OCS_Guarding:
            {
              ChangeToTaskState(TASK_CombatGuardMenu);
            };
            break;
          case OCS_CombatOver:
            ChangeToTaskState(TASK_CombatOver);
            break;
          case OCS_Casting:
            {
              COMBATANT &combatant = combatData.getActiveCombatCharacter();
              //GLOBAL_SPELL_ID key = combatant.GetSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              SPELL_ID spellID = combatant.GetSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                //if (!combatant.GetItemSpellKeyBeingCast().IsValidSpell())
                if (!combatant.GetItemSpellIDBeingCast().IsValidSpell())
                {
                  spellID = combatant.GetItemSpellIDBeingCast();
                  if (!spellID.IsValidSpell())
                  {
                    combatData.EndTurn();
                    ChangeToTaskState(TASK_CombatMainMenu);         
                  }
                  else
                  {   
                    // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                    /* Really */ NotImplemented(0x5510b, false);
                    ChangeToTaskState(TASK_CombatBeginItemCasting);
                  };
                }
                else
                {
                  // 6 August 2014 "Basilisk Gazes Beholder" ChangeToTaskState(TASK_CombatActivateSpellMsg);
                  ChangeToTaskState(TASK_CombatBeginItemCasting);
                };
              }
              else  
              {   
                ChangeToTaskState(TASK_CombatActivateSpellMsg);
              }
            }
            break;

          case OCS_None:
            combatData.UpdateCombat();
            OCS = combatData.GetCombatState();
            switch (OCS)
            {
            case OCS_NewCombatant:
              ChangeToTaskState(TASK_CombatDelay_NewCombatant);
              break;
            //  //ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
            //  //PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
  //        // case OCS_None:
  //        //    ChangeToTaskState(TASK_CombatMainMenu);
  //        //    break;
  //        //  case OCS_Moving:
  //        //    if (combatData.OnAuto(false))
  //        //    {
  //        //      ChangeToTaskState(TASK_CombatDelay_AutoMove);
  //        //    }
  //        //    else
  //        //    {
  //        //      die(0xa3c12a);
  //        //    };
  //        //    break;
            //case OCS_StartNewRound:
  //        //    // 20120605 PRS  This case was undefined.
  //        //    // In some places we set the two additional boolean variables.
  //        //    // I didn't know whether should or not.  I have commented them out
  //        //    // for the present.
  //        //    //TRACE("Start new round delay\n");
  //        //    //combatData.m_bStartingNewCombatRound = FALSE;
  //        //    //combatData.m_forceRoundDelay = FALSE;
            //  ChangeToTaskState(TASK_CombatStartingNewRound);
            //  break;
            //case OCS_Casting:
            //  {
            //    COMBATANT &dude = combatData.getActiveCombatCharacter();
            //    //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
            //    //if (!key.IsValidSpell())
            //    SPELL_ID spellID = dude.GetSpellIDBeingCast();
            //    if (!spellID.IsValidSpell())
            //    {
            //      //key = dude.GetItemSpellKeyBeingCast();
            //      //if (!key.IsValidSpell())
            //      spellID = dude.GetItemSpellIDBeingCast();
            //      if (!spellID.IsValidSpell())
            //      {
            //        spellID = dude.GetItemSpellIDBeingCast();
            //        if (!spellID.IsValidSpell())
            //        {
            //          combatData.EndTurn();
            //          ChangeToTaskState(TASK_CombatMainMenu);         
            //        }
            //        else
            //        {   
            //          // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
            //          Not Implemented(0x5510b, false);
            //          ChangeToTaskState(TASK_CombatBeginItemCasting);
            //        };
            //      }
            //      else
            //      {   
            //        ChangeToTaskState(TASK_CombatActivateSpellMsg);
            //        return;
            //      };
            //    }
            //    else  
            //    {   
            //      ChangeToTaskState(TASK_CombatActivateSpellMsg);
            //      return;
            //    }
            //  }
            //  break;
            //case OCS_CombatOver:
            //  ChangeToTaskState(TASK_CombatOver);
            //  break;
            //};
            default: die(0xc42108b);
          };
          break;
          






          default:
            die(0x45ca1);
          };
        };
      }
      else
      {
        overallCombatState OCS;
        OCS = combatData.GetCombatState();
        switch(OCS)
        {
        case OCS_Attacking: // auto
          if (combatData.NeedMissileAnimation())
          {
            ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
          }
          else
          {
            ChangeToTaskState(TASK_CombatAttacking);
          };
          break;
        case OCS_Guarding:
          ChangeToTaskState(TASK_CombatGuardMenu);
          break;
        case OCS_None:
          {
            combatData.UpdateCombat();
            OCS = combatData.GetCombatState();
            switch(OCS)
            {
            case OCS_None:
              ChangeToTaskState(TASK_CombatMainMenu);
              break;
            //case OCS_NewCombatant:
            //  ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            //  break;
            //case OCS_StartNewRound:
            //  ChangeToTaskState(TASK_CombatStartingNewRound);
            //  break;
            default:
              die(0xc42423);
              break;
            };
          };
          break;
        default:
          die(0xc67d65);
        };
      };
    };
    break;

  case TASK_CombatMoveMenu:
    {
      overallCombatState OCS;
      if (combatData.IsStartOfTurn())
      {
        // Moving must have caused a free attack by one of the enemy combatants.
        if (!combatData.IsFreeAttacker()  && !combatData.IsGuardAttacker())
        {
          die (0xcc31ab);
        };
#ifdef TraceFreeAttacks
        {
          CString msg;
          int combatant, targ;
          const COMBATANT *pDude;
          combatant = combatData.GetCurrCombatant();
          pDude = combatData.GetCombatant(combatant);
          targ = pDude->m_target;
          if (combatData.IsFreeAttacker()) msg = "Free Attack"; else msg = "Guard Attack";
          WriteDebugString("TFA - COMBAT_EVENT_DATA::OnReturnToTopOfQueue(TASK_CombatMoveMenu) dude# %d %s against %d\n",
                                        combatant, msg, targ);
        };
#endif
        combatData.UpdateCombat(); // Remove the start-of-turn flags
//20111024        combatData.AddTarget(combatData.Get_FreeAttack_Target(), true);
        combatData.StartAttack();
//        WriteDebugString("DEBUG - Return to line 15106\n");
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_Attacking: // auto
          /* 20111022 PRS
             Free Attacks and Guarding Attacks are Melee attacks.
             The target may have moved already but the attack takes 
             place as if the target were still adjacent.
          if (combatData.NeedMissileAnimation())
          {
            ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
          }
          else
          */
          {
            /*
            if (LoadAI_Script())
            {
              int i, additionalAttacks;
              int hash;
              CArray<int,int>actionIndex;
              COMBATANT *pAttacker, *pTarget;
              hash = taskList.Hash();
              combatSummary.Clear();
              pAttacker = combatData.GetCombatant(combatData.GetCurrCombatant());
              pTarget = combatData.GetCombatant(pAttacker->m_target);
              combatData.ListCombatants(&combatSummary, pAttacker, pTarget);
              combatData.ListWeapons(combatSummary.GetCombatant(0)); // List weapons for attacker only.
              combatData.ListAmmo(combatSummary.GetCombatant(0));    // List ammo for attacker only.
              additionalAttacks = combatData.QComb.NumFreeAttacks() + combatData.QComb.NumGuardAttacks();
              // List attacks for attacker only.
              combatData.ListAttacks(combatSummary.GetCombatant(0), additionalAttacks); 
              //combatData.ListReachableCells(&combatSummary, combatSummary.GetCombatant(0)); // Number of steps to each cell.
              combatData.ListActions(&combatSummary, 0, additionalAttacks);
            
              actionIndex.SetSize(combatSummary.GetActionCount());
              for(i=0; i<combatSummary.GetActionCount(); i++)
              {
                actionIndex.SetAt(i,i);
              };
              if (hash != taskList.Hash())
              {
                int kkk=1;
              };
          
              DumpActions(&actionIndex, &combatSummary, false);
            };
            */
            COMBATANT *pAttacker;
            pAttacker = combatData.GetCombatant(combatData.GetCurrCombatant());
            pAttacker->FreeThink();
          };
          ChangeToTaskState(TASK_CombatAttacking);
          break;;
        case OCS_Guarding:
          ChangeToTaskState(TASK_CombatGuardMenu);
          break;
        case OCS_None:
          combatData.UpdateCombat();
          OCS = combatData.GetCombatState();
          switch (OCS)
          {
            case OCS_None:
            {
              if (combatData.IsFreeAttacker() || combatData.IsGuardAttacker())
              {
                OCS = combatData.GetCombatState();
                switch(OCS)
                {
                default:
                  die(0xf4aa127);
                //case OCS_NewCombatant: //added 20111114
                //  ChangeToTaskState(TASK_CombatAttacking);
                //  return;
                //case OCS_Guarding:
                //  ChangeToTaskState(TASK_CombatGuardMenu);
                //  return;
                case OCS_None:
                  ChangeToTaskState(TASK_CombatAttacking);
                  return;
                };
                break;
              };
            default: die(0xc421089);
            };
          };
          break;
        default:
          die(0xcabbf54);
        };
        break;
      };
      OCS = combatData.GetCombatState();
      switch (OCS)
      {
      case OCS_Attacking:
        {
          int curr = combatData.GetCurrCombatant();
          if (curr != NO_DUDE)
          {
            HOOK_PARAMETERS hookParameters;
            SCRIPT_CONTEXT scriptContext;
            ActorType actor;
            int index;
            COMBATANT *dude;
            CLASS_DATA *pClass;
            dude = combatData.GetCombatant(curr);
            dude->m_specialActionName = "";
            hookParameters[5] = "Move";
            scriptContext.SetCombatantContext(dude);
            scriptContext.SetCharacterContext(dude->m_pCharacter);
            //pClass = &classData[dude->GetClass()];
            pClass = classData.GetClass(dude->GetClass());
            scriptContext.SetClassContext(pClass);
            dude->GetContext(&actor);
            SetCharContext(&actor);
            dude->RunCombatantScripts(
                           COMBAT_MAIN_MENU, 
                           ScriptCallback_RunAllScripts,
                           NULL,
                           "Return to combat main menu");
            dude->m_specialActionName = hookParameters[6];
            sscanf(hookParameters[7],"%d",&index);
            dude->m_specialActionIndex = index;
            ClearCharContext();
          };
        };


        if (combatData.GetCurrTarget(true) != NO_DUDE)
        {
          ChangeToTaskState(TASK_CombatAttacking);
        }
        else
        {
          combatData.StopMoving();
          ChangeToTaskState(TASK_CombatMainMenu);
        }
        break;
      case OCS_Moving:
        {
          int curr;
          COMBATANT *pCombatant;
          combatData.UpdateCombat();
          curr = combatData.GetCurrCombatant();
          pCombatant = combatData.GetCombatant(curr);
          combatData.HandleCurrState(FALSE);
          OCS = combatData.GetCombatState();
          switch(OCS)
          {
          case OCS_Fled:
            {
              combatData.StopMoving();
              combatData.UpdateCombat();
              OCS = combatData.GetCombatState();
              switch (OCS)
              {
              case OCS_NewCombatant:
                ChangeToTaskState(TASK_CombatDelay_NewCombatant);
                break;
                //ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
                //PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
              case OCS_StartNewRound:
                ChangeToTaskState(TASK_CombatStartingNewRound);
                break;
              default: die(0xc421098);
              };
              break;
            };
          //case OCS_Fled:
          //  {
          //    // Cause another OnIdle in order to UpdateCombat
          //    ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
          //    PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
          //    break; // BUGFIX
          //  };
          case OCS_Moving:
            if (combatData.OnAuto(false))
            {
              ChangeToTaskState(TASK_CombatDelay_AutoMove);
            }
            else
            {
              die(0xa3c12a);
            };
            break;
          default:
            die(0x431c90);
          };
          break;
        };
      case OCS_Fled:
        {
          combatData.StopMoving();
          combatData.UpdateCombat();
          OCS = combatData.GetCombatState();
          switch (OCS)
          {
          case OCS_StartNewRound:
            //TRACE("Start new round delay\n");
            //TRACE("Round delay\n");
            /* It worked with or without the following two lines.
             * So I left them out until it appears that they are useful. PRS20111229
             *
             * combatData.m_bStartingNewCombatRound = FALSE;
             * combatData.m_forceRoundDelay = FALSE;
             */
            ChangeToTaskState(TASK_CombatStartingNewRound);
            break;
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            break;
            //ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
            //PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
          default: die(0xc421099);
          };
          break;
        };
      case OCS_Guarding:
          ChangeToTaskState(TASK_CombatGuardMenu);
          break;
      case OCS_None:
          combatData.UpdateCombat();
          OCS = combatData.GetCombatState();
          switch (OCS)
          {
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            break;
            //ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
            //PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
          case OCS_None:
            ChangeToTaskState(TASK_CombatMainMenu);
            break;
          case OCS_Moving:
            if (combatData.OnAuto(false))
            {
              ChangeToTaskState(TASK_CombatDelay_AutoMove);
            }
            else
            {
              die(0xa3c12a);
            };
            break;
          case OCS_StartNewRound:
            // 20120605 PRS  This case was undefined.
            // In some places we set the two additional boolean variables.
            // I didn't know whether should or not.  I have commented them out
            // for the present.
            //TRACE("Start new round delay\n");
            //combatData.m_bStartingNewCombatRound = FALSE;
            //combatData.m_forceRoundDelay = FALSE;
            ChangeToTaskState(TASK_CombatStartingNewRound);
            break;
          default: die(0xc42108a);
          };
          break;
      case OCS_CombatOver:
        ChangeToTaskState(TASK_CombatOver);
        break;
      default:
        die(0x56c1a);
        combatData.StopMoving();
        ChangeToTaskState(TASK_CombatMainMenu);
        break;
      };
      break;
    };

  case TASK_CombatAimMenu:
    if (combatData.GetCurrTarget(true) != NO_DUDE)
    {
      if (combatData.NeedMissileAnimation())
        ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
      else
        ChangeToTaskState(TASK_CombatAttacking);
    }
    else
      ChangeToTaskState(TASK_CombatMainMenu);
    break; 

  case TASK_CombatCastMenu:
    /// if spell chosen to cast, then begin casting process
    //if (!combatData.getActiveCombatCharacter().GetSpellKeyBeingCast().IsValidSpell())
    if (!combatData.getActiveCombatCharacter().GetSpellIDBeingCast().IsValidSpell())
    {
      ChangeToTaskState(TASK_CombatMainMenu); 
    }
    else
    {
      ChangeToTaskState(TASK_CombatBeginCasting); 
    };
    break;
  case TASK_CombatUseMenu:
    /// if using item caused spell to be cast, then begin casting process
    //if (!combatData.getActiveCombatCharacter().GetItemSpellKeyBeingCast().IsValidSpell())
    if (!combatData.getActiveCombatCharacter().GetItemSpellIDBeingCast().IsValidSpell())
      ChangeToTaskState(TASK_CombatMainMenu); 
    else
      ChangeToTaskState(TASK_CombatBeginItemCasting); 
    break;



  case TASK_CombatBeginCasting:
    {
      //COMBATANT &dude = combatData.getActiveCombatCharacter();
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude != NULL)
      {
        //if (dude->GetSpellKeyBeingCast().IsValidSpell())
        if (dude->GetSpellIDBeingCast().IsValidSpell())
        {
          if (dude->IsSpellPending())
          {
            combatData.EndInitialSpellCasting();  // Ends turn
            ChangeToTaskState(TASK_CombatTurnEnded);
          }
          else
          {
            ChangeToTaskState(TASK_CombatActivateSpellMsg);
          };
        }
        else
        {
          ChangeToTaskState(TASK_CombatTurnEnded);
        };
      }
      else
        ChangeToTaskState(TASK_CombatActivateSpell);
    }
    break;    

  
  case TASK_CombatBeginItemCasting:
    {
      //COMBATANT &dude = combatData.getActiveCombatCharacter();
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude != NULL)
      {
        //if (dude->GetSpellKeyBeingCast().IsValidSpell())
        if (dude->GetSpellIDBeingCast().IsValidSpell())
        {
          if (dude->IsSpellPending())
          {
            combatData.EndInitialSpellCasting();  // Ends turn
            ChangeToTaskState(TASK_CombatTurnEnded);
          }
          else
          {
            ChangeToTaskState(TASK_CombatActivateSpellMsg);
          };
        }
        else
        {
          ChangeToTaskState(TASK_CombatTurnEnded);
        };
      }
      else
        ChangeToTaskState(TASK_CombatActivateSpell);
    }
    break;    

  
  
  
  
  
  
  case TASK_CombatSelectSpellTargets:
#ifdef SpellInitiationScript
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        const SPELL_DATA *pSpell;
        SPELL_ID spellID;
        if (dude->combatant_activeSpellKey != -1)
        {
          die(0x4419e);  // Why?
        };
        spellID = dude->GetSpellIDBeingCast();
        if (spellID.IsValidSpell())
        {
          pSpell = spellData.PeekSpell(spellID);
          pSpell->ExecuteSpellInitiationScript(dude->m_pCharacter);
          dude->combatant_activeSpellKey = -2;   // Indicate that the SpellInitiationScript has been executed
        };
      };
    };
#endif

    ChangeToTaskState(TASK_CombatSpellCastAnimate); 
    break;

  case TASK_CombatActivateSpellMsg:
    {
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        if (   (dude->m_pCharacter->targets.NumTargets() == 0)
             &&(dude->m_pCharacter->targets.NumMapTargets() == 0) )
        {
          ChangeToTaskState(TASK_CombatSelectSpellTargets);
        }
        else // there could be preselected targets if on automatic
        {
          // Should we ExecuteSpellInitiationScripts() ??
#ifdef SpellInitiationScript
          {
            const SPELL_DATA *pSpell;
            SPELL_ID spellID;
            if (dude->combatant_activeSpellKey != -1)
            {
              die(0x4419e);  // Why?
            };
            spellID = dude->GetSpellIDBeingCast();
            if (spellID.IsValidSpell())
            {
              pSpell = spellData.PeekSpell(spellID);
              pSpell->ExecuteSpellInitiationScript(dude->m_pCharacter);
              dude->combatant_activeSpellKey = -2;   // Indicate that the SpellInitiationScript has been executed
            };
          };
#endif
          ChangeToTaskState(TASK_CombatSpellCastAnimate);
        };
      }
      else
        ChangeToTaskState(TASK_CombatActivateSpell);
    }
    break;

  case TASK_CombatActivateSpell:
    {
      int dude;
      dude = combatData.GetCurrCombatant();
      if (dude != NO_DUDE)
      {
        CheckForSpellLingeringScripts();
      };


      dude = combatData.GetCurrCombatant();
      combatData.StopSpellCasting();
      if (dude == NO_DUDE)
      {
        overallCombatState OCS;
        combatData.UpdateCombat();  //See if another combatant is ready.
        dude = combatData.GetCurrCombatant();
        if (dude == NO_DUDE)
        {
          die(0xa2cbc77);
        };
        OCS = combatData.GetCombatState();
        switch(OCS)
        {
        case OCS_NewCombatant:
          ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          break;
        case OCS_StartNewRound:
          ChangeToTaskState(TASK_CombatStartingNewRound);
          break;
        case OCS_CombatOver:
          ChangeToTaskState(TASK_CombatOver);
          break;
        case OCS_Casting:
          {
            COMBATANT &activeDude = combatData.getActiveCombatCharacter();
            if(activeDude.self == 2)
            {
              int kkk=1;
            };
//GLOBAL_SPELL_ID key = activeDude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = activeDude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              spellID = activeDude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                spellID = activeDude.GetItemSpellIDBeingCast();
                if (!spellID.IsValidSpell())
                {
                  combatData.EndTurn();
                  ChangeToTaskState(TASK_CombatMainMenu);         
                }
                else
                {   
                  // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                  /* Really */ NotImplemented(0x5510b, false);
                  ChangeToTaskState(TASK_CombatBeginItemCasting);
                };
              }
              else
              {   
                // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                /* Really */ NotImplemented(0x5510b, false);
                ChangeToTaskState(TASK_CombatBeginItemCasting);
              };
            }
            else  
            { 
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
            }
          }
          break;
        default:
          die(0xcffee12);
        };
        break;
      };
      //updatecombat
      //currcombatant?
      {
        overallCombatState OCS;
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_Casting:
        {
          COMBATANT &activeDude = combatData.getActiveCombatCharacter();
          //GLOBAL_SPELL_ID key = activeDude.GetSpellKeyBeingCast();
          //if (!key.IsValidSpell())
          SPELL_ID spellID = activeDude.GetSpellIDBeingCast();
          if (!spellID.IsValidSpell())
          {
            spellID = activeDude.GetItemSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              spellID = activeDude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                combatData.EndTurn();
                ChangeToTaskState(TASK_CombatMainMenu);
              }
              else
              {
                // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                /* Really */ NotImplemented(0x5510b, false);
                ChangeToTaskState(TASK_CombatBeginItemCasting);
              };
            }
            else
            {
              // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
              /* Really */ NotImplemented(0x5510b, false);
              ChangeToTaskState(TASK_CombatBeginItemCasting);
            };
          }
          else
          {
            ChangeToTaskState(TASK_CombatActivateSpellMsg);
          }
        };
        break;
        case OCS_None:
          combatData.UpdateCombat();
          OCS = combatData.GetCombatState();
          switch (OCS)
          {
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            break;
            //ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
            //PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
//          case OCS_None:
//            ChangeToTaskState(TASK_CombatMainMenu);
//            break;
//          case OCS_Moving:
//            if (combatData.OnAuto(false))
//            {
//              ChangeToTaskState(TASK_CombatDelay_AutoMove);
//            }
//            else
//            {
//              die(0xa3c12a);
//            };
//            break;
          case OCS_StartNewRound:
            //            // 20120605 PRS  This case was undefined.
            //            // In some places we set the two additional boolean variables.
            //            // I didn't know whether should or not.  I have commented them out
            //            // for the present.
            //            //TRACE("Start new round delay\n");
            //            //combatData.m_bStartingNewCombatRound = FALSE;
            //            //combatData.m_forceRoundDelay = FALSE;
            ChangeToTaskState(TASK_CombatStartingNewRound);
            break;
          case OCS_Casting:
          {
            COMBATANT &activeDude = combatData.getActiveCombatCharacter();
            //GLOBAL_SPELL_ID key = activeDude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = activeDude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              //key = activeDude.GetItemSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              spellID = activeDude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                spellID = activeDude.GetItemSpellIDBeingCast();
                if (!spellID.IsValidSpell())
                {
                  combatData.EndTurn();
                  ChangeToTaskState(TASK_CombatMainMenu);
                }
                else
                {
                  // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                  /* Really */ NotImplemented(0x5510b, false);
                  ChangeToTaskState(TASK_CombatBeginItemCasting);
                };
              }
              else
              {
                ChangeToTaskState(TASK_CombatActivateSpellMsg);
                return;
              };
            }
            else
            {
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
              return;
            }
          }
          break;
          case OCS_CombatOver:
            ChangeToTaskState(TASK_CombatOver);
            break;
          default: die(0xc42108b);
          };
          break;
        default:
          die(0xcc321aa);
        };
      };
      break;

      // 20120512 PRS  ChangeToTaskState(TASK_CombatMainMenu); 
    };
    break;

  case TASK_CombatActivateRedirectedSpell:
    combatData.StopSpellCasting();
    ChangeToTaskState(TASK_CombatMainMenu); 
    break;

  case TASK_CombatAttacking_DisplayAttacker:
    ChangeToTaskState(TASK_CombatAttacking);
    break;

  case TASK_CombatAttacking: 
    {
      //WriteDebugString("DEBUG - case TASK_CombatAttacking @ %I64d virtualGameTime\n");
      if (combatData.NeedHitAnimation())
        ChangeToTaskState(TASK_CombatHitSpriteAnimating);
      else
      {
#ifdef TraceFreeAttacks
        {
          CString msg;
          if (combatData.IsFreeAttacker()) msg = "Free Attack";
          if (combatData.IsGuardAttacker()) msg = "Guard Attack";
          if (!msg.IsEmpty())
          {
            int dude, targ;
            const COMBATANT *pDude;
            dude = combatData.GetCurrCombatant();
            pDude = combatData.GetCombatant(dude);
            targ = pDude->m_target;
            WriteDebugString("TFA - COMBAT_EVENT_DATA::OnReturnToTopOfQueue(TASK_CombatAttacking) dude# %d %s %d\n",
                                dude, msg, targ); 
          };
        };
#endif
        ChangeToTaskState(TASK_CombatDelay_DisplayAttackMsg);
      };
      break;
    }
  
  case TASK_CombatAutoAttacking: 
    {
      //WriteDebugString("DEBUG - TASK_CombatAutoAttacking\n");
      if (combatData.NeedHitAnimation())
      {
        ChangeToTaskState(TASK_CombatAutoHitSpriteAnimating);
        //WriteDebugString("DEBUG - Change to TASK_CombatAutoHitSpriteAnimating\n");
      }
      else
      {
        ChangeToTaskState(TASK_CombatDelay_AutoAttackMsg);
        //WriteDebugString("DEBUG - Change to TASK_CombatDelay_AutoAttackMsg\n");
      };
    }
    break;

  //case TASK_CombatUseMenu: ChangeToTaskState(TASK_CombatUsing); break;      
  //case TASK_CombatUsing: ChangeToTaskState(TASK_CombatMainMenu); break;      

  case TASK_CombatTurnMenu: ChangeToTaskState(TASK_CombatTurning); break;
  case TASK_CombatTurning:
    {  
      if (combatData.GetCurrCombatant() != NO_DUDE)
      {
        overallCombatState OCS;
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_NewCombatant:
          die(0x231cac);  // just to get rid of 'no case' warning!
        default:
          die(0xeae141);
          ChangeToTaskState(TASK_CombatMainMenu); // What used to be done unconditionally.
        };
      };
      {
        overallCombatState OCS;
        combatData.UpdateCombat();  // Find next Combatant
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_NewCombatant:
          ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          break;
        case OCS_StartNewRound:
          ChangeToTaskState(TASK_CombatStartingNewRound);
          break;
        case OCS_CombatOver:
          ChangeToTaskState(TASK_CombatOver);
          break;
        case OCS_Casting:
        {
          COMBATANT &dude = combatData.getActiveCombatCharacter();
          //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
          //if (!key.IsValidSpell())
          SPELL_ID spellID = dude.GetSpellIDBeingCast();
          if (!spellID.IsValidSpell())
          {
            //key = dude.GetItemSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            spellID = dude.GetItemSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              spellID = dude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                combatData.EndTurn();
                ChangeToTaskState(TASK_CombatMainMenu);
              }
              else
              {
                // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                /* Really */ NotImplemented(0x5510b, false);
                ChangeToTaskState(TASK_CombatBeginItemCasting);
              };
            }
            else
            {
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
              return;
            };
          }
          else
          {
            ChangeToTaskState(TASK_CombatActivateSpellMsg);
            return;
          };
        };
        break;
        default:
          die(0xc34776);
        };
      };
    };
    break;
 
  case TASK_CombatBandageMenu: 
    {
      ChangeToTaskState(TASK_CombatBandaging);
    };
    break;
  case TASK_CombatBandaging: 
    {
      overallCombatState OCS;
      OCS = combatData.GetCombatState();
      switch(OCS)
      {
      case OCS_None:
        {
          combatData.UpdateCombat();
          OCS = combatData.GetCombatState();
          switch(OCS)
          {
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            break;
          case OCS_StartNewRound:
            ChangeToTaskState(TASK_CombatStartingNewRound);
            break;
          default:
            die(0xc42423);
            break;
          };
        };
        break;
      default:
        die(0x431aba);
        break;
      };
    };
    break;

  case TASK_CombatGuardMenu:
    {
      ChangeToTaskState(TASK_CombatGuarding); 
    };
    break;
  case TASK_CombatGuarding: 
    {
      if (combatData.GetCurrCombatant() != NO_DUDE)
      {
        overallCombatState OCS;
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_NewCombatant:
          ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          break;
        case OCS_Casting: // Added 20120307 PRS
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = dude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              //key = dude.GetItemSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              spellID = dude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                spellID = dude.GetItemSpellIDBeingCast();
                if (!spellID.IsValidSpell())
                {
                  combatData.EndTurn();
                  ChangeToTaskState(TASK_CombatMainMenu);         
                }
                else
                {   
                  // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                  /* Really */ NotImplemented(0x5510b, false);
                  ChangeToTaskState(TASK_CombatBeginItemCasting);
                };
              }
              else  
              {   
                ChangeToTaskState(TASK_CombatActivateSpellMsg);
              };
            }
            else  
            {   
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
            }
          }
          break;
        case OCS_Attacking: // auto
          {
            //die(0x710a4);
            int curr;
            curr = combatData.GetCurrTarget(FALSE);
            if (curr == NO_DUDE)
            {
              combatData.StopAttack();
              ChangeToTaskState(TASK_CombatMainMenu);
            }
            else
            {
              if (combatData.NeedMissileAnimation())
                ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
              else
                ChangeToTaskState(TASK_CombatAttacking);
            };
          };
          break;
        case OCS_Guarding:
          ChangeToTaskState(TASK_CombatGuardMenu);
          break;
        default:
          die(0xeae151);
        };
      }
      else
      {
        overallCombatState OCS;
        combatData.UpdateCombat();  // Find next Combatant
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_NewCombatant:
          ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          break;
        case OCS_StartNewRound:
          ChangeToTaskState(TASK_CombatStartingNewRound);
          break;
        case OCS_CombatOver:
          ChangeToTaskState(TASK_CombatOver);
          break;
        case OCS_Guarding:
          ChangeToTaskState(TASK_CombatGuardMenu);
          break;
        case OCS_Casting:
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = dude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              //key = dude.GetItemSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              spellID = dude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                spellID = dude.GetItemSpellIDBeingCast();
                if (!spellID.IsValidSpell())
                {
                  combatData.EndTurn();
                  ChangeToTaskState(TASK_CombatMainMenu);         
                }
                else
                {   
                  // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                  /* Really */ NotImplemented(0x5510b, false);
                  ChangeToTaskState(TASK_CombatBeginItemCasting);
                };
              }
              else
              {    
                ChangeToTaskState(TASK_CombatActivateSpellMsg);
              };
            }
            else  
            {    
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
            }
          };
          break;
        default:
          die(0xc34776);
        };
      }
    };
    break;
  
  case TASK_CombatDelayMenu: ChangeToTaskState(TASK_CombatDelaying);break;
  case TASK_CombatDelaying: 
    {
      overallCombatState OCS;
      if (combatData.GetCurrCombatant() != NO_DUDE)
      {
        die(0xeae121);
      };
      combatData.UpdateCombat();  // Find next Combatant
      OCS = combatData.GetCombatState();
      switch (OCS)
      {
      case OCS_NewCombatant:
        ChangeToTaskState(TASK_CombatDelay_NewCombatant);
        break;
      case OCS_StartNewRound:
        ChangeToTaskState(TASK_CombatStartingNewRound);
        break;
      case OCS_Casting:
        {
          COMBATANT &dude = combatData.getActiveCombatCharacter();
          //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
          //if (!key.IsValidSpell())
          SPELL_ID spellID = dude.GetSpellIDBeingCast();
          if (!spellID.IsValidSpell())
          {
            //key = dude.GetItemSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            spellID = dude.GetItemSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              combatData.EndTurn();
              ChangeToTaskState(TASK_CombatMainMenu);
            }
            else
            {    
              ChangeToTaskState(TASK_CombatBeginItemCasting);
            };
          }
          else  
          {    
            ChangeToTaskState(TASK_CombatActivateSpellMsg);
          }
        }
        break;
      default:
        die(0xc34777);
      };
    }
    break;

  case TASK_CombatEndTurnMenu:
    ChangeToTaskState(TASK_CombatTurnEnded);
    break;  
  case TASK_CombatTurnEnded: 
    {
      if (combatData.GetCurrCombatant() != NO_DUDE)
      {
        overallCombatState OCS;
        OCS = combatData.GetCombatState();
        switch(OCS)
        {
        case OCS_Casting:
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = dude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              //key = dude.GetItemSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              spellID = dude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                combatData.EndTurn();
                ChangeToTaskState(TASK_CombatMainMenu);         
              }
              else
              {   
                ChangeToTaskState(TASK_CombatBeginItemCasting);
              };
            }
            else
            {   
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
            }
          };
          break;
        default:
          die(0xeae121);
        };
        break;
      };
      {
        overallCombatState OCS;
        combatData.UpdateCombat();  // Find next Combatant (Performs Think()????)
        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        case OCS_NewCombatant:
          ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          break;
        case OCS_StartNewRound:
          ChangeToTaskState(TASK_CombatStartingNewRound);
          break;
        case OCS_Casting:
        {
          const SPELL_DATA *pSpell;
          COMBATANT &dude = combatData.getActiveCombatCharacter();
          //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
          //if (!key.IsValidSpell())
          SPELL_ID spellID = dude.GetSpellIDBeingCast();
          pSpell = spellData.PeekSpell(spellID);
          if (pSpell == NULL)
          {
            //key = dude.GetItemSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            spellID = dude.GetItemSpellIDBeingCast();
            pSpell = spellData.PeekSpell(spellID);
            if (pSpell == NULL)
            {
              combatData.EndTurn();
              ChangeToTaskState(TASK_CombatMainMenu);
            }
            else
            {
              ChangeToTaskState(TASK_CombatBeginItemCasting);
            };
          }
          else
          {
            if ((dude.Wpn_Type == SpellCaster)
              || pSpell->CastMsg.IsEmpty())
            {
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
            }
            else // Skip the A casts B message
            {
              SetMyState(TASK_CombatActivateSpellMsg);
              combatData.SetActiveCastingCharacter(dude.self);
              PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
            };
          }
        };
        break;
        case OCS_CombatOver:
          ChangeToTaskState(TASK_CombatOver);
          return;
        default:
          die(0xc34775);
        };
      };
    };
    break;

  case TASK_CombatForceWinMenu: 
    ChangeToTaskState(TASK_CombatForcingWin); 
    break;  

  case TASK_CombatForcingWin: //ChangeToTaskState(TASK_CombatMainMenu); 
    combatData.UpdateCombat();
    ChangeToTaskState(TASK_CombatOver);
    break;

  case TASK_CombatInRouteSpriteAnimating: 
#ifdef TraceFreeAttacks
    {
      CString msg;
      if (combatData.IsFreeAttacker()) msg = "Free Attack";
      if (combatData.IsGuardAttacker()) msg = "GuardAttack";
      if (!msg.IsEmpty())
      {
        int dude, targ;
        const COMBATANT *pDude;
        dude = combatData.GetCurrCombatant();
        pDude = combatData.GetCombatant(dude);
        targ = pDude->m_target;
        WriteDebugString("TFA - COMBAT_EVENT_DATA::OnReturnToTopOfQueue(TASK_CombatInRouteSpriteAnimating) dude# %d %s %d\n",
                             dude, msg, targ);
      };
    };
#endif
    ChangeToTaskState(TASK_CombatAttacking); 
    break;

  case TASK_CombatHitSpriteAnimating: 
    //ChangeToTaskState(TASK_CombatDelay_HitSpriteAnimateMsg); 
    ChangeToTaskState(TASK_CombatDelay_DisplayAttackMsg);
    break;


  case TASK_CombatAutoHitSpriteAnimating:
    {
        ChangeToTaskState(TASK_CombatDelay_AutoAttackMsg);
//        WriteDebugString("DEBUG - Change to TASK_CombatDelay_AutoAttackMsg\n");
    };
    break;
  



  case TASK_CombatSpellCastAnimate: 
    ChangeToTaskState(TASK_CombatActivateSpell); 
    break;

  case TASK_CombatSpellMissileAnimate: 
    {
      unsigned int newTarget;
      //GLOBAL_SPELL_ID gsID;
      SPELL_ID spellID;
      HOOK_PARAMETERS hookParameters;
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      COMBATANT *pTarget;
      //gsID = dude->GetSpellKeyBeingCast();
      spellID = dude->GetSpellIDBeingCast();
      //if (!gsID.IsValidSpell())
      if (!spellID.IsValidSpell())
      {
        //gsID = dude->GetItemSpellKeyBeingCast();
        //if (!gsID.IsValidSpell())
        spellID = dude->GetItemSpellIDBeingCast();
        if (!spellID.IsValidSpell())
        {
          die(0x413413);
        };
      };
      //SPELL_DATA *pSdata = spellData.GetSpellData(gsID);
      SPELL_DATA *pSdata = spellData.GetSpell(spellID);
      if (dude->m_pCharacter->targets.NumTargets() > 0)
      {
        SCRIPT_CONTEXT scriptContext;
        ActorType actor;
        pTarget = combatData.GetCombatant(dude->m_pCharacter->targets.m_targets[0]);
        scriptContext.SetAttackerContext(dude);
        scriptContext.SetSpellContext(pSdata);
        scriptContext.SetTargetContext(pTarget);
        pTarget->GetContext(&actor);
        SetTargetContext(&actor);
        pSdata->RunSpellScripts(
                     REDIRECT_SPELL,
                     ScriptCallback_RunAllScripts,
                     NULL,
                     "Combat - continue missile spell animate");
        pTarget->m_pCharacter->RunCharacterScripts(
                     REDIRECT_SPELL, 
                     ScriptCallback_RunAllScripts, 
                     NULL,
                     "Combat - continue missile spell animate");
        ClearTargetContext();
        if (    !hookParameters[5].IsEmpty()
             && (sscanf(hookParameters[5],"%d",&newTarget) == 1)
             && (newTarget < (unsigned)combatData.NumCombatants()))
        {
          combatData.m_iRedirectedSpellTarget = newTarget;
          combatData.m_sRedirectedSpellMessage = hookParameters[6];
          ChangeToTaskState(TASK_CombatSpellRedirectedMissileAnimate);
          break;
        };
      };
      ChangeToTaskState(TASK_CombatSpellCoverageAnimate);
      break;
    };
  case TASK_CombatSpellRedirectedMissileAnimate: 
    {
      //HOOK_PARAMETERS hookParameters;
      //COMBATANT *dude = combatData.getActiveCastingCharacter();
      //COMBATANT *pTarget;
      //Not Implemented(0x3894bb, false);
      //SPELL_DATA *pSdata = spellData.GetSpellData(dude->GetSpellKeyBeingCast());
      //pTarget = &combatData.GetCombatant(dude->targets.m_targets[0]);
      //scriptContext.SetAttackerContext(dude);
      //scriptContext.SetSpellContext(pSdata);
      //scriptContext.SetTargetContext(pTarget);
      //pSdata->specAbs.RunScripts(REDIRECT_SPELL, ScriptCallback_RunAllScripts, NULL, "Spell", pSdata->Name);
      //pTarget->specAbs.RunScripts(REDIRECT_SPELL, ScriptCallback_RunAllScripts, NULL, "Spell", pSdata->Name);
      //scriptContext.Clear();
      //if (!hookParameters[5].IsEmpty())
      //{
      //  ChangeToTaskState(TASK_CombatSpellRedirectedMissileAnimate);
      //}
      //else
      //{
        ChangeToTaskState(TASK_CombatRedirectedSpellCoverageAnimate);
      //};
      break;
    };
  case TASK_CombatSpellCoverageAnimate:
    ChangeToTaskState(TASK_CombatSpellLingerAnimate); 
    break;
  case TASK_CombatRedirectedSpellCoverageAnimate:
    ChangeToTaskState(TASK_CombatRedirectedSpellLingerAnimate); 
    break;
  case TASK_CombatSpellLingerAnimate: 
    {
      //June 2, 2010  PRS
      // I moved this "InvokePendingSpellOnCombatant" from TASK_CombatSpellFinalStage
      // so that the damage will be computed BEFORE formatting the spell results message.
      // We wanted the message to have a "/d" replacement. "Caster Blasts /t for /d damage".
      // I have my fingers crossed.
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      COMBATANT *pTarget;

      if (dude!=NULL)
      {
        int target;
        InvokeSpellOptions invokeOptions;
        if (dude->m_pCharacter->targets.NumTargets() > 0)
        {
                  //{ 
                  //  CString result;
                  //  HOOK_PARAMETERS hookParameters;
                  //  SCRIPT_CONTEXT scriptContext;
                  //  scriptContext.SetSpellContext(pSpData);
                  //  scriptContext.SetAttackerContext(this);
                  //  //hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
                  //  result = pSpData->RunSpellScripts(SPELL_CASTER_LEVEL, ScriptCallback_RunAllScripts, NULL);
                  //  if (result.GetLength()> 0)
                  //  {
                  //    sscanf(result,"%d",&rollerLevel);
                  //  };
          target = dude->m_pCharacter->targets.m_targets[0];
          pTarget = combatData.GetCombatant(target);
          pTarget->m_spellDamageHP = pTarget->GetHitPoints();
          pTarget->m_tempHP = pTarget->m_spellDamageHP;
          invokeOptions.m_supressBlitSprite = DeathSprite | DyingSprite;
          invokeOptions.m_whatSpriteNeeded = 0;
          setPartyXY_x = -1;
          pTarget->m_useTempHP = true;
//#ifdef SpellInitiationScript
//          dude->combatant_activeSpellKey = -1;
// Should do this before first target
//#endif
          dude->combatant_activeSpellKey = 
              combatData.InvokePendingSpellOnCombatant(dude->self, 
                                                      target, 
                                                      dude->combatant_activeSpellKey,
                                                      true,
                                                      &combatData.m_sSpellEffectMessage,
                                                      NULL,
                                                      //GLOBAL_SPELL_ID(),
                                                      SPELL_ID(),
                                                      &invokeOptions);
          if (setPartyXY_x >= 0)
          {
            ChangeToTaskState(TASK_CombatOver);
            break;
          };
          needDeathSprite = (invokeOptions.m_whatSpriteNeeded & DeathSprite) != 0; 
          needDyingSprite = (invokeOptions.m_whatSpriteNeeded & DyingSprite) != 0;
          //pTarget->m_spellDamageHP -= pTarget->GetHitPoints();
          pTarget->m_spellDamageHP -= pTarget->m_tempHP;
        };
        if (dude->m_pCharacter->targets.NumMapTargets() > 0)
        {
          target = NO_DUDE;
          //pTarget = combatData.GetCombatant(target);
          //pTarget->m_spellDamageHP = pTarget->GetHitPoints();
          //invokeOptions.m_supressBlitSprite = DeathSprite | DyingSprite;
          //invokeOptions.m_whatSpriteNeeded = 0;
          //setPartyXY_x = -1;
          //dude->combatant_activeSpellKey = 
          //dude->activeSpellKey = 
          //     combatData.InvokePendingSpellOnMap(dude->self, dude->activeSpellKey);          
          if (setPartyXY_x >= 0)
          {
            ChangeToTaskState(TASK_CombatOver);
            break;
          };
          //needDeathSprite = (invokeOptions.m_whatSpriteNeeded & DeathSprite) != 0; 
          //needDyingSprite = (invokeOptions.m_whatSpriteNeeded & DyingSprite) != 0;
          //pTarget->m_spellDamageHP -= pTarget->GetHitPoints();
        };
      };
      ChangeToTaskState(TASK_CombatSpellHitAnimate);
    };
    break;
  case TASK_CombatRedirectedSpellLingerAnimate: 
    {
      //June 2, 2010  PRS
      // I moved this "InvokePendingSpellOnCombatant" from TASK_CombatSpellFinalStage
      // so that the damage will be computed BEFORE formatting the spell results message.
      // We wanted the message to have a "/d" replacement. "Caster Blasts /t for /d damage".
      // I have my fingers crossed.
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      COMBATANT *pTarget;
      if (dude!=NULL)
      {
        int target;
        //target = dude->targets.m_targets[0];
        target = combatData.m_iRedirectedSpellTarget;
        pTarget = combatData.GetCombatant(target);
        pTarget->m_spellDamageHP = pTarget->GetHitPoints();
        pTarget->m_useTempHP = false;
        dude->combatant_activeSpellKey = 
            combatData.InvokePendingSpellOnCombatant(dude->self, 
                                                     target, 
                                                     dude->combatant_activeSpellKey,
                                                     false);
        pTarget->m_spellDamageHP -= pTarget->GetHitPoints();
      };
      ChangeToTaskState(TASK_CombatRedirectedSpellHitAnimate);
    };
    break;
  case TASK_CombatSpellHitAnimate: 
    if (needDeathSprite)
    {
      COMBATANT *dude;
      dude = combatData.getActiveCastingCharacter();
      if (dude != NULL)
      {
        int target;
        COMBATANT *pTarget;
        target = dude->m_pCharacter->targets.m_targets[0];
        pTarget = combatData.GetCombatant(target);
        pTarget->blitDeadSprite();
      };
    };
    if (needDyingSprite)
    {
      COMBATANT *dude;
      dude = combatData.getActiveCastingCharacter();
      if (dude != NULL)
      {
        int target;
        COMBATANT *pTarget;
        target = dude->m_pCharacter->targets.m_targets[0];
        pTarget = combatData.GetCombatant(target);
        pTarget->blitDyingSprite();
      };
    };
    ChangeToTaskState(TASK_CombatSpellFinalStage); 
    break; 
  case TASK_CombatRedirectedSpellHitAnimate: 
    ChangeToTaskState(TASK_CombatRedirectedSpellFinalStage); 
    break; 

  case TASK_CombatSpellFinalStage:
    { 
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        if (dude->m_pCharacter->targets.NumTargets()>0) // targeting self may halt spell casting and  remove targets
        {
          int target;
          COMBATANT *pTarget;
          target = dude->m_pCharacter->targets.m_targets[0];
          // The following was moved up to TASK_CombatSpellLingerAnimate
          //dude->activeSpellKey = 
          //  combatData.InvokePendingSpellOnCombatant(dude->self, 
          //                                           target, 
          //                                           dude->activeSpellKey);
          // But we had to move the actual application of the damage
          // to here (Final Stage) so that the death sprite would not
          // be displayed before the hit animation.
          pTarget = combatData.GetCombatant(target);
          if ((pTarget != NULL) && pTarget->m_useTempHP)
          {
            int finalHP;
            finalHP = pTarget->GetHitPoints();
            pTarget->m_useTempHP = false;
            pTarget->SetHitPoints(finalHP);
            pTarget->TakeDamage(0, FALSE, NULL, FALSE, NULL);
          };
          if ( (dude->m_pCharacter->targets.NumTargets() > 0) && (dude->m_pCharacter->targets.m_targets[0] == target) )
          {
            dude->m_pCharacter->targets.m_targets.RemoveAt(0);
          };
        }

        if (!dude->m_pCharacter->targets.m_SelectingUnits)
        {
          // ****** need to do this before linger animation so that active
          // ****** spell key is available.          
          //
          //if (dude->targets.m_MapTargetX != -1)
          //{
          //  dude->activeSpellKey = 
          //    combatData.InvokePendingSpellOnMap(dude->self, dude->activeSpellKey);          
          //}
          dude->m_pCharacter->targets.m_MapTargetX=dude->m_pCharacter->targets.m_MapTargetY=-1;
        }        
        // if targeting a map x,y, the missile/coverage animation only plays once
        if ((dude->m_pCharacter->targets.m_SelectingUnits)||(dude->m_pCharacter->targets.NumTargets()==0))
          ChangeToTaskState(TASK_CombatActivateSpell);
        else
        {
          COMBATANT *castingDude = combatData.getActiveCastingCharacter();
          COMBATANT *pTarget;
          if (castingDude!=NULL)
          {
            int target;
            InvokeSpellOptions invokeOptions;
            target = castingDude->m_pCharacter->targets.m_targets[0];
            pTarget = combatData.GetCombatant(target);
            //if (DoesSpellSucceed())
            {
              pTarget->m_spellDamageHP = pTarget->GetHitPoints();
              invokeOptions.m_supressBlitSprite = DeathSprite | DyingSprite;
              invokeOptions.m_whatSpriteNeeded = 0;
              pTarget->m_useTempHP = false;
              castingDude->combatant_activeSpellKey =
                  combatData.InvokePendingSpellOnCombatant(castingDude->self,
                                                           target, 
                                                           castingDude->combatant_activeSpellKey,
                                                           false,
                                                           &combatData.m_sSpellEffectMessage,
                                                           NULL,
                                                           //GLOBAL_SPELL_ID(),
                                                           SPELL_ID(),
                                                           &invokeOptions);
              needDeathSprite = (invokeOptions.m_whatSpriteNeeded & DeathSprite) != 0; 
              needDyingSprite = (invokeOptions.m_whatSpriteNeeded & DyingSprite) != 0;
              pTarget->m_spellDamageHP -= pTarget->GetHitPoints();
            };
          };
          ChangeToTaskState(TASK_CombatSpellHitAnimate);// hit animate needs unit targets
        };
      }
      else
        ChangeToTaskState(TASK_CombatActivateSpell);
    }
    break;

  case TASK_CombatRedirectedSpellFinalStage:
    { 
      COMBATANT *dude = combatData.getActiveCastingCharacter();
      if (dude!=NULL)
      {
        if (dude->m_pCharacter->targets.NumTargets()>0) // targeting self may halt spell casting and  remove targets
        {
          int target;
          target = dude->m_pCharacter->targets.m_targets[0];
          //target = combatData.m_iRedirectedSpellTarget;
          // The following was moved up to TASK_CombatSpellLingerAnimate
          //dude->activeSpellKey = 
          //  combatData.InvokePendingSpellOnCombatant(dude->self, 
          //                                           target, 
          //                                           dude->activeSpellKey);
          if ( (dude->m_pCharacter->targets.NumTargets() > 0) && (dude->m_pCharacter->targets.m_targets[0] == target) )
          {
            dude->m_pCharacter->targets.m_targets.RemoveAt(0);
          };
        }
  
        if (!dude->m_pCharacter->targets.m_SelectingUnits)
        {
          // ****** need to do this before linger animation so that active
          // ****** spell key is available.          
          //
          //if (dude->targets.m_MapTargetX != -1)
          //{
          //  dude->activeSpellKey = 
          //    combatData.InvokePendingSpellOnMap(dude->self, dude->activeSpellKey);          
          //}

          dude->m_pCharacter->targets.m_MapTargetX=dude->m_pCharacter->targets.m_MapTargetY=-1;
        }        
        // if targeting a map x,y, the missile/coverage animation only plays once
        if ((dude->m_pCharacter->targets.m_SelectingUnits)||(dude->m_pCharacter->targets.NumTargets()==0))
          ChangeToTaskState(TASK_CombatActivateRedirectedSpell);
        else
        {
          /* Really */ NotImplemented(0x5321aba,false);
          //COMBATANT *dude = combatData.getActiveCastingCharacter();
          //COMBATANT *pTarget;
          //if (dude!=NULL)
          //{
          //  int target;
          //  target = dude->targets.m_targets[0];
          //  pTarget = &combatData.GetCombatant(target);
          //  pTarget->m_spellDamageHP = pTarget->GetHitPoints();
          //  dude->activeSpellKey = 
          //      combatData.InvokePendingSpellOnCombatant(dude->self, 
          //                                               target, 
          //                                              dude->activeSpellKey);
          //  pTarget->m_spellDamageHP -= pTarget->GetHitPoints();
          //};
          //ChangeToTaskState(TASK_CombatSpellHitAnimate);// hit animate needs unit targets
        };
      }
      else
        ChangeToTaskState(TASK_CombatActivateSpell);
    }
    break;

  case TASK_CombatDelay_AutoAttackMsg:
    if (combatData.IsStartOfTurn())
    {
      ChangeToTaskState(TASK_CombatDelay_NewCombatant);
      break;
    };
    {
      overallCombatState OCS;
      combatData.UpdateCombat();   // Next combatant if prev is done
      OCS = combatData.GetCombatState();
      switch (OCS)
      {
      case OCS_NewCombatant:
        ChangeToTaskState(TASK_CombatDelay_NewCombatant);
        break;
      case OCS_StartNewRound:
        ChangeToTaskState(TASK_CombatStartingNewRound);
        break;
      case OCS_CombatOver:
        ChangeToTaskState(TASK_CombatOver);
        break;
      case OCS_Casting:
      {
        COMBATANT &dude = combatData.getActiveCombatCharacter();
        //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
        //if (!key.IsValidSpell())
        SPELL_ID spellID = dude.GetSpellIDBeingCast();
        if (!spellID.IsValidSpell())
        {
          //key = dude.GetItemSpellKeyBeingCast();
          //if (!key.IsValidSpell())
          spellID = dude.GetItemSpellIDBeingCast();
          if (!spellID.IsValidSpell())
          {
            combatData.EndTurn();
            ChangeToTaskState(TASK_CombatMainMenu);
          }
          else
          {
            ChangeToTaskState(TASK_CombatBeginItemCasting);
          };
        }
        else
        {
          ChangeToTaskState(TASK_CombatActivateSpellMsg);
        }
      }
      break;
      case OCS_Attacking: // auto
        if (combatData.NeedMissileAnimation())
          ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
        else
          ChangeToTaskState(TASK_CombatAttacking);
        break;
      case OCS_Moving:
        ChangeToTaskState(TASK_CombatDelay_AutoMove);
        break;
      case OCS_Guarding:
        ChangeToTaskState(TASK_CombatGuardMenu);
        break;
      default:
        die(0x332cad);
      };
      break;
    }
    case TASK_CombatDelay_DisplayAttackMsg:
      {      
        if (combatData.IsStartOfTurn())
        {
          COMBATANT &dude = combatData.getActiveCombatCharacter();
          if (     (dude.GetStatus() != Dead)
               &&  (dude.m_target >= 0)
               &&  (combatData.getCombatantPtr(dude.m_target)->charOnCombatMap(false, true))
             )
          {
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            return;
          }
          combatData.UpdateCombat();   // Remove StartOfTurn status
          PlaceCursorOnCurrentDude();
        }
#ifdef TraceFreeAttacks
        {
          CString msg;
          int dude, targ;
          const COMBATANT *pDude;
          if (combatData.IsFreeAttacker()) msg = "Free Attack";
          if (combatData.IsGuardAttacker()) msg = "Guard Attack";
          if (!msg.IsEmpty())
          {
            dude = combatData.GetCurrCombatant();
            pDude = combatData.GetCombatant(dude);
            targ = pDude->m_target;
            WriteDebugString("TFA - COMBAT_EVENT_DATA::OnReturnToTopOfQueue(TASK_CombatDelay_DisplayAttackMsg) dude# %d %s %d\n",
                               dude, msg, targ);
          };
        };
#endif
        {
          overallCombatState OCS;
          combatData.UpdateCombat();   // Next combatant if prev is done
          OCS = combatData.GetCombatState();
#ifdef TraceFreeAttacks
          if (combatData.IsFreeAttacker() || combatData.IsGuardAttacker())
          {
            CString msg;
            switch (OCS)
            {
            case OCS_CombatOver:    msg = "CombatOver";    break;
            case OCS_StartNewRound: msg = "StartNewRound"; break;
            case OCS_NewCombatant:  msg = "NewCombatant";  break;
            case OCS_Attacking:     msg = "Attacking";     break;
            case OCS_Casting:       msg = "Casting";       break;
            case OCS_Moving:        msg = "Moving";        break;
            case OCS_Guarding:      msg = "Guarding";      break;
            case OCS_None:          msg = "None";          break;
            default:                msg = "?????";         break;
            };
            WriteDebugString("TFA - After UpdateCombat() OCS = %s\n", msg);
          };
#endif
          switch (OCS)
          {
          case OCS_CombatOver:
            ChangeToTaskState(TASK_CombatOver);
            return;
          case OCS_StartNewRound:
            TRACE("Start new round delay\n");
            combatData.m_bStartingNewCombatRound = FALSE;
            combatData.m_forceRoundDelay = FALSE;
            ChangeToTaskState(TASK_CombatStartingNewRound);
            return;
          case OCS_NewCombatant:
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            return;
          case OCS_Attacking: // auto
          {
            int curr;
            curr = combatData.GetCurrTarget(FALSE);
            if (curr == NO_DUDE)
            {
              combatData.StopAttack();
              ChangeToTaskState(TASK_CombatMainMenu);
            }
            else
            {
              if (combatData.NeedMissileAnimation())
                ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
              else
                ChangeToTaskState(TASK_CombatAttacking);
            };
          };
          break;
          case OCS_Casting:
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = dude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              //key = dude.GetItemSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              spellID = dude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                combatData.EndTurn();
                ChangeToTaskState(TASK_CombatMainMenu);
              }
              else
              {
                ChangeToTaskState(TASK_CombatBeginItemCasting);
              };
            }
            else
            {
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
            }
          }
          break;
          case OCS_Moving:
            if (combatData.OnAuto(false))
            {
              ChangeToTaskState(TASK_CombatDelay_AutoMove);
            }
            else
            {
              die(0xa3c1c2a);
            };
            break;
          case OCS_Guarding:
            ChangeToTaskState(TASK_CombatGuardMenu);
            break;
          case OCS_None:
          {
            ChangeToTaskState(TASK_CombatMainMenu);
          };
          break;
          default:
            die(0x2884dd);
            break;
          };
        };
//      
//      if (combatData.OnAuto())
//      {
//        if (combatData.IsStartOfTurn()) combatData.UpdateCombat(); // remove start-of-turn flags
//        combatData.UpdateCombat();  // Performs a Think()
//
//        OCS = combatData.GetCombatState();
//        switch (OCS)
//        {
//        //case OCS_None: // auto
//        //  break;
//      
//        case OCS_Attacking: // auto
//          if (combatData.NeedMissileAnimation())
//            ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
//          else
//            ChangeToTaskState(TASK_CombatAttacking);
//          break;
//
//        //case OCS_Guarding: // auto
//        //  ChangeToTaskState(TASK_CombatGuardMenu);
//        //  break;
//
//        //case OCS_Fleeing:
//        case OCS_Moving: // auto
//          {
//            int updateScreen;
//            // moves dude, or can end turn
//            updateScreen = combatData.HandleCurrState();
//            if (updateScreen != 0) 
//            {
//              OnUpdateUI();
//              OnDraw();
//              ChangeToTaskState(TASK_CombatAutoMoving);
//            }
//            else
//            {
//              die(0x1a4d5c2);
//            };
//          };
//          break;
//
//        //case OCS_CombatOver:
//        //  ChangeToTaskState(TASK_CombatOver);      
//        //  break;

        //case OCS_StartNewRound:
        //  TRACE("Start new round delay\n");
        //case OCS_CombatRoundDelay:
        //  TRACE("Round delay\n");
        //  combatData.m_bStartingNewCombatRound = FALSE;
        //  combatData.m_forceRoundDelay = FALSE;
        //  ChangeToTaskState(TASK_CombatRoundDelay);
        //  break;

        //case OCS_Casting:
        //  {
        //    COMBATANT &dude = combatData.getActiveCombatCharacter();
        //    GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
        //    if (!key.IsValidSpell())
        //    {
        //      combatData.EndTurn();
        //      ChangeToTaskState(TASK_CombatMainMenu);         
        //    }
        //    else  
        //    {   
        //      ChangeToTaskState(TASK_CombatActivateSpellMsg);
        //    }
        //  }
        //  break;

        //case OCS_Bandaging:
        //  ChangeToTaskState(TASK_CombatBandageMenu);
        //  break;

        //case OCS_Using:
        //  //ChangeToTaskState(TASK_CombatUsing);
        // break;

        //case OCS_Turning:
        //  ChangeToTaskState(TASK_CombatTurnMenu);
        //  break;

        //case OCS_Delaying:
        //  ChangeToTaskState(TASK_CombatDelayMenu);
        //  break;

        //case OCS_EndingTurn:
        //  ChangeToTaskState(TASK_CombatEndTurnMenu);
        //  break;

        //case OCS_Fled:  // BUGFIX
        //  // Cause another OnIdle in oreder to UpdateCombat
        //  ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
        //  PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
        //  break; // BUGFIX

//        default:
//          die(0x2319fd);//ASS ERT(FALSE);
//          break;
//        }
        
        
        
//      }
//      else
//      {
//        ChangeToTaskState(TASK_CombatMainMenu); 
//      };
      
      
      
//    };   
//    break;
      
      
      
      };
      break;

//  case TASK_CombatTimeDelayMsg:
//    {
//      combatData.UpdateCombat();   // Next combatant if prev is done
//      OCS = combatData.GetCombatState();
//      switch (OCS)
//      {
//      case OCS_CombatOver:
//        ChangeToTaskState(TASK_CombatOver);
//        return;
//      case OCS_StartNewRound:
//        break;
//      case OCS_CombatRoundDelay:
//        ChangeToTaskState(TASK_CombatRoundDelay);
//        return;
//      default:
//        die(0x2114dd);
//        break;
//      };      
//      
//      if (combatData.OnAuto())
//      {
//        if (combatData.IsStartOfTurn()) combatData.UpdateCombat(); // remove start-of-turn flags
//        combatData.UpdateCombat();  // Performs a Think()
//
//        OCS = combatData.GetCombatState();
//        switch (OCS)
//        {
//        //case OCS_None: // auto
//        //  break;
//      
//        case OCS_Attacking: // auto
//          if (combatData.NeedMissileAnimation())
//            ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
//          else
//            ChangeToTaskState(TASK_CombatAttacking);
//          break;
//
//        //case OCS_Guarding: // auto
//        //  ChangeToTaskState(TASK_CombatGuardMenu);
//        //  break;
//
//        //case OCS_Fleeing:
//        case OCS_Moving: // auto
//          {
//            int updateScreen;
//            // moves dude, or can end turn
//            updateScreen = combatData.HandleCurrState();
//            if (updateScreen != 0) 
//            {
//              OnUpdateUI();
//              OnDraw();
//              ChangeToTaskState(TASK_CombatAutoMoving);
//            }
//            else
//            {
//              die(0x1a4d5c2);
//            };
//          };
//          break;
//
//        //case OCS_CombatOver:
//        //  ChangeToTaskState(TASK_CombatOver);      
//        //  break;

        //case OCS_StartNewRound:
        //  TRACE("Start new round delay\n");
        //case OCS_CombatRoundDelay:
        //  TRACE("Round delay\n");
        //  combatData.m_bStartingNewCombatRound = FALSE;
        //  combatData.m_forceRoundDelay = FALSE;
        //  ChangeToTaskState(TASK_CombatRoundDelay);
        //  break;

        //case OCS_Casting:
        //  {
        //    COMBATANT &dude = combatData.getActiveCombatCharacter();
        //    GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
        //    if (!key.IsValidSpell())
        //    {
        //      combatData.EndTurn();
        //      ChangeToTaskState(TASK_CombatMainMenu);         
        //    }
        //    else  
        //    {   
        //      ChangeToTaskState(TASK_CombatActivateSpellMsg);
        //    }
        //  }
        //  break;

        //case OCS_Bandaging:
        //  ChangeToTaskState(TASK_CombatBandageMenu);
        //  break;

        //case OCS_Using:
        //  //ChangeToTaskState(TASK_CombatUsing);
        // break;

        //case OCS_Turning:
        //  ChangeToTaskState(TASK_CombatTurnMenu);
        //  break;

        //case OCS_Delaying:
        //  ChangeToTaskState(TASK_CombatDelayMenu);
        //  break;

        //case OCS_EndingTurn:
        //  ChangeToTaskState(TASK_CombatEndTurnMenu);
        //  break;

        //case OCS_Fled:  // BUGFIX
        //  // Cause another OnIdle in oreder to UpdateCombat
        //  ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
        //  PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
        //  break; // BUGFIX

//        default:
//          die(0x2319fd);//ASS ERT(FALSE);
//          break;
//        }
        
        
        
//      }
//      else
//      {
//        ChangeToTaskState(TASK_CombatMainMenu); 
//      };
      
      
      
//    };   
//    break;
  case TASK_CombatStartingNewRound:
    {
      if (combatData.GetCurrCombatant() == NO_DUDE)
      {
        combatData.UpdateCombat();  // Queue up the first combatant
      };
      combatData.turningSummary.Clear();
      ChangeToTaskState(TASK_CombatDelay_NewCombatant);
      combatData.m_bStartingNewCombatRound = FALSE;
    };
    break;

  case TASK_CombatDelay_NewCombatant: // Should be the first time we see this fellow.
    {
      //combatData.DetermineIfCombatOver();
      //if (combatData.IsCombatOver())
      //{
      //  Not Implemented(0x16ac, false);
      //};
//      WriteDebugString("DEBUG - Return TASK_CombatDelay_NewCombatant\n");
//      WriteDebugString("DEBUG - case TASK_CombatDelay_NewCombatant\n");
      if (!combatData.IsStartOfTurn())
      {
        overallCombatState OCS;
        WriteDebugString("DEBUG - Return TASK_CombatDelay_NewCombatant; IsStartOfTurn\n");
        OCS = combatData.GetCombatState();
        switch(OCS)
        {
        case OCS_None:
          combatData.UpdateCombat();
          OCS = combatData.GetCombatState();
          switch(OCS)
          {
          case OCS_StartNewRound:
            ChangeToTaskState(TASK_CombatStartingNewRound);
            return;
          default:
            die(0xc310b);
          };
        case OCS_Casting:
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = dude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              //key = dude.GetItemSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              spellID = dude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                combatData.EndTurn();
                ChangeToTaskState(TASK_CombatMainMenu);         
              }
              else
              {   
                ChangeToTaskState(TASK_CombatBeginItemCasting);
                return;
              };
            }
            else  
            {   
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
              return;
            }
          }
          break;
        case OCS_CombatOver:
          ChangeToTaskState(TASK_CombatOver);
          break;
        default:
          die(0x7008ed);
        };
      };
//      WriteDebugString("DEBUG - IsFreeAttacker=%d; IsGuardAttacker=%d\n",combatData.IsFreeAttacker(), combatData.IsGuardAttacker());
      combatData.UpdateCombat();   //Simply removes StartOfTurn flags
      
//      WriteDebugString("DEBUG - IsFreeAttacker=%d; IsGuardAttacker=%d\n",combatData.IsFreeAttacker(), combatData.IsGuardAttacker());
      if (combatData.IsFreeAttacker() || combatData.IsGuardAttacker())
      {
        overallCombatState OCS;
        OCS = combatData.GetCombatState();
//        WriteDebugString("DEBUG - OCS = %d\n", OCS);
        switch(OCS)
        {
        default:
          die(0xf4aa127);
        case OCS_NewCombatant: //added 20111114
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            dude.FreeThink();  // Judo or Melee?  Select weapon. 
            ChangeToTaskState(TASK_CombatAttacking);
          };
          return;
        case OCS_Guarding:
          ChangeToTaskState(TASK_CombatGuardMenu);
          return;
        case OCS_None:
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            dude.FreeThink();  // Judo or Melee?  Select weapon.
            ChangeToTaskState(TASK_CombatAttacking);
          };
          return;
        case OCS_Attacking:
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            dude.FreeThink();  // Judo or Melee?  Select weapon.
            ChangeToTaskState(TASK_CombatAttacking);
          };
          return;
        };
      };
      
      if (combatData.OnAuto(true))
      {
        overallCombatState OCS;
        combatData.UpdateCombat();  // Performs a Think()

        OCS = combatData.GetCombatState();
        switch (OCS)
        {
        //case OCS_None: // auto
        //  break;
      
        case OCS_Attacking: // auto
          if (combatData.NeedMissileAnimation())
            ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
          else
          {
            ChangeToTaskState(TASK_CombatAttacking_DisplayAttacker);
          };
            /* Auto combatants never get their stats displayed.
             * We switch to TASK_CombatAttacking and then the target
             * gets displayed.  So we have inserted an additional
             * state...to display the attacker before displaying
             * the target.  OnReturnToTopOfQueue will do nothing
             * except change to TASK_CombatAttacking.
             * PRS 20110114
             *
             *ChangeToTaskState(TASK_CombatAttacking)
             */

          return;

        case OCS_Guarding: // auto
          ChangeToTaskState(TASK_CombatGuardMenu);
          break;

        //case OCS_Fleeing:
        case OCS_Moving: // auto
          {
            int updateScreen;
            // moves dude, or can end turn
            updateScreen = combatData.HandleCurrState(FALSE);
            if (updateScreen != 0) 
            {
              OnUpdateUI();
              OnDraw();
              // 20111104 ChangeToTaskState(TASK_CombatDelay_AutoMove);
              // 20111104 We will check the state first- the current combatant may have changed
              OCS = combatData.GetCombatState();
              switch (OCS)
              {
              //case OCS_None:
              //  combatData.UpdateCombat();
              //  OCS = combatData.GetCombatState();
              //  switch(OCS)
              //  {
                case OCS_NewCombatant:
                  ChangeToTaskState(TASK_CombatDelay_NewCombatant);
                  break;
                case OCS_Moving:
                  ChangeToTaskState(TASK_CombatDelay_AutoMove);
                  break;
              //  default:
              //   die(0x805021);
              //  };
              //  break;
              //case OCS_NewCombatant:
              //  ChangeToTaskState(TASK_CombatDelay_NewCombatant);
              //  break;
              case OCS_Attacking: 
                // Added 20130202 PRS.
                // Copied some code from elsewhere.
                // Not sure I copied the right stuff.
                if ((GetMyState() != TASK_CombatInRouteSpriteAnimating) && (combatData.NeedMissileAnimation()))
                  ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
                else
                  ChangeToTaskState(TASK_CombatAttacking);
                break;
              default:
                die(0x1a4d5c2);
              };
            }
            else
            {
              OCS = combatData.GetCombatState();
              switch (OCS)
              {
              case OCS_None:
                combatData.UpdateCombat();
                OCS = combatData.GetCombatState();
                switch(OCS)
                {
                case OCS_Casting:
                  {
                    COMBATANT &dude = combatData.getActiveCombatCharacter();
                    //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
                    //if (!key.IsValidSpell())
                    SPELL_ID spellID = dude.GetSpellIDBeingCast();
                    if (!spellID.IsValidSpell())
                    {
                      //key = dude.GetItemSpellKeyBeingCast();
                      //if (!key.IsValidSpell())
                      spellID = dude.GetItemSpellIDBeingCast();
                      if (!spellID.IsValidSpell())
                      {
                        combatData.EndTurn();
                        ChangeToTaskState(TASK_CombatMainMenu);         
                      }
                      else
                      {   
                        // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                        ChangeToTaskState(TASK_CombatBeginItemCasting);
                      };
                    }
                    else  
                    {   
                      ChangeToTaskState(TASK_CombatActivateSpellMsg);
                    }
                  }
                  break;
                case OCS_NewCombatant:
                  ChangeToTaskState(TASK_CombatDelay_NewCombatant);
                  break;
                case OCS_StartNewRound:
                  //TRACE("Start new round delay\n");
                  //TRACE("Round delay\n");
                  /* It worked with or without the following two lines.
                   * So I left them out until it appears that they are useful. PRS20111229
                   *
                   * combatData.m_bStartingNewCombatRound = FALSE;
                   * combatData.m_forceRoundDelay = FALSE;
                   */
                  ChangeToTaskState(TASK_CombatStartingNewRound);
                  break;
                case OCS_CombatOver:
                  ChangeToTaskState(TASK_CombatOver);
                  break;
                default:
                  die(0x805021);
                };
                break;
              case OCS_NewCombatant:
                ChangeToTaskState(TASK_CombatDelay_NewCombatant);
                break;
              default:
                die(0x1a4d5c3);
              };
            };
          };
          break;
        //  ChangeToTaskState(TASK_CombatMoving);
        //  break;

        //case OCS_CombatOver:
        //  ChangeToTaskState(TASK_CombatOver);      
        //  break;

        //case OCS_StartNewRound:
        //  TRACE("Start new round delay\n");
          case OCS_NewCombatant:
            TRACE("Round delay\n");
            combatData.m_bStartingNewCombatRound = FALSE;
            combatData.m_forceRoundDelay = FALSE;
            ChangeToTaskState(TASK_CombatDelay_NewCombatant);
            return;

        case OCS_Casting:
          {
            COMBATANT &dude = combatData.getActiveCombatCharacter();
            //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
            //if (!key.IsValidSpell())
            SPELL_ID spellID = dude.GetSpellIDBeingCast();
            if (!spellID.IsValidSpell())
            {
              //key = dude.GetItemSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              spellID = dude.GetItemSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                combatData.EndTurn();
                ChangeToTaskState(TASK_CombatMainMenu);         
              }
              else
              {   
                // 20120512 PRS  ChangeToTaskState(TASK_CombatActivateSpellMsg);
                ChangeToTaskState(TASK_CombatBeginItemCasting);
              };
            }
            else  
            {   
              ChangeToTaskState(TASK_CombatActivateSpellMsg);
            }
          }
          break;

        //case OCS_Bandaging:
        //  ChangeToTaskState(TASK_CombatBandageMenu);
        //  break;

        //case OCS_Using:
        //  //ChangeToTaskState(TASK_CombatUsing);
        // break;

        //case OCS_Turning:
        //  ChangeToTaskState(TASK_CombatTurnMenu);
        //  break;

        //case OCS_Delaying:
        //  ChangeToTaskState(TASK_CombatDelayMenu);
        //  break;

        //case OCS_EndingTurn:
        //  ChangeToTaskState(TASK_CombatEndTurnMenu);
        //  break;

        case OCS_Fled:  // BUGFIX
          // Cause another OnIdle in order to UpdateCombat
          ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
          PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
          break; // BUGFIX

        case OCS_CombatOver:
          ChangeToTaskState(TASK_CombatOver);
          break;
        default:
          die(0x2319fe);//ASS ERT(FALSE);
          break;
        }
        
        
        
      }
      else
      {
        overallCombatState OCS;
        OCS = combatData.GetCombatState();
        switch(OCS)
        {
          //case OCS_Casting:
          //  {
          //    COMBATANT &dude = combatData.getActiveCombatCharacter();
          //    GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
          //    if (!key.IsValidSpell())
          //    {
          //      combatData.EndTurn();
          //      ChangeToTaskState(TASK_CombatMainMenu);         
          //    }
          //    else
          //    {   
          //      ChangeToTaskState(TASK_CombatActivateSpellMsg);
          //    }
          //  };
          //  break;
          //case OCS_StartNewRound:
          //  break;
          case OCS_None:
            //combatData.UpdateCombat();
            //OCS = combatData.GetCombatState();
            //switch(OCS)
            //{
            //case OCS_None:
            ChangeToTaskState(TASK_CombatMainMenu);
            break;
          case OCS_StartNewRound:
            TRACE("Start new round delay\n");
            TRACE("Round delay\n");
            combatData.m_bStartingNewCombatRound = FALSE;
            combatData.m_forceRoundDelay = FALSE;
            ChangeToTaskState(TASK_CombatStartingNewRound);
            break;
          case OCS_CombatOver:
            ChangeToTaskState(TASK_CombatOver);
            break;
          case OCS_Attacking:
            if ((GetMyState() != TASK_CombatInRouteSpriteAnimating) && (combatData.NeedMissileAnimation()))
              ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
            else
              ChangeToTaskState(TASK_CombatAttacking);
            break;
          case OCS_Guarding: // auto
            ChangeToTaskState(TASK_CombatGuardMenu);
            break;
          //case OCS_NewCombatant:
          //  ChangeToTaskState(TASK_CombatDelay_NewCombatant);
          //  break;
          case OCS_Casting:  // added 20120315 PRS
            {
              COMBATANT &dude = combatData.getActiveCombatCharacter();
              //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
              //if (!key.IsValidSpell())
              SPELL_ID spellID = dude.GetSpellIDBeingCast();
              if (!spellID.IsValidSpell())
              {
                //key = dude.GetItemSpellKeyBeingCast();
                //if (!key.IsValidSpell())
                spellID = dude.GetItemSpellIDBeingCast();
                if (!spellID.IsValidSpell())
                {
                  combatData.EndTurn();
                  ChangeToTaskState(TASK_CombatMainMenu);         
                }
                else  
                {   
                  ChangeToTaskState(TASK_CombatBeginItemCasting);
                };
              }
              else  
              {   
                ChangeToTaskState(TASK_CombatActivateSpellMsg);
              }
            }
            break;
          default:
            die(0x5caaa23);
        };
      };
    
    };
   
    break;

  case TASK_CombatOver: 
    {
      COMBATANT *pCombatant;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      int i;
      for (i=0; i<combatData.NumCombatants(); i++)
      {
        pCombatant = combatData.GetCombatant(i);
        ClearHookParameters();
        scriptContext.SetCombatantContext(pCombatant);
        pCombatant->RunCombatantScripts(
                        ON_END_COMBAT,
                        ScriptCallback_RunAllScripts,
                        NULL,
                        "End of combat");
        scriptContext.Clear();
      };
    }
    {
      int i;
      for (i=0; i<combatData.NumCombatants(); i++)
      {
        CHARACTER *pChar;
        COMBATANT *pCombatant;
        pCombatant = combatData.GetCombatant(i);
        pChar = pCombatant->m_pCharacter;
        //combatData.GetCombatant(i)->m_pCharacter->m_pCombatant = NULL;
        if (!pCombatant->IsPartyMember())
        {
          //The following two lines will be handled by combatData.Clear()
          //pChar->m_pCombatant = NULL;
          //pCombatant->m_pCharacter = NULL;
          //delete pChar;
          pCombatant->deleteChar = TRUE;
        }
        else
        {
          //The following two lines will be handled by combatData.Clear()
          //pChar->m_pCombatant = NULL;
          //pCombatant->m_pCharacter = NULL;
          pCombatant->deleteChar = FALSE;
        }
      };

#ifdef SpellInitiationScript
      /*
       *   We need to convert any references to combat characters.
       *   The problem that started this was that spell effects survived
       *   beyond the end of combat.  The activeSpellList contained
       *   Actor references to combatants.  When we leave combat, any
       *   surviving combatant should have their references converted
       *   to non-combat actors.
       */
  activeSpellList.ExitCombat();
#endif

      combatData.Clear();
    };
    EnableUserInput(TRUE);
    ChainHappened(); break; 
  
  case TASK_CombatSpecialAction:
    {
      overallCombatState OCS;
      combatData.EndTurn();
      //    ChangeToTaskState(TASK_CombatEndTurnMenu); //(TASK_CombatEndingTurn);
      combatData.UpdateCombat();
      OCS = combatData.GetCombatState();
      switch (OCS)
      {
      case OCS_CombatOver:
        ChangeToTaskState(TASK_CombatOver);
        break;
      case OCS_NewCombatant:
        ChangeToTaskState(TASK_CombatDelay_NewCombatant);
        break;
      case OCS_StartNewRound:
        ChangeToTaskState(TASK_CombatStartingNewRound);
        break;
      case OCS_Casting:
      {
        COMBATANT &dude = combatData.getActiveCombatCharacter();
        //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
        //if (!key.IsValidSpell())
        SPELL_ID spellID = dude.GetSpellIDBeingCast();
        if (!spellID.IsValidSpell())
        {
          //key = dude.GetItemSpellKeyBeingCast();
          //if (!key.IsValidSpell())
          spellID = dude.GetItemSpellIDBeingCast();
          if (!spellID.IsValidSpell())
          {
            combatData.EndTurn();
            ChangeToTaskState(TASK_CombatMainMenu);
          }
          else
          {
            ChangeToTaskState(TASK_CombatBeginItemCasting);
          };
        }
        else
        {
          ChangeToTaskState(TASK_CombatActivateSpellMsg);
        }
      }
      break;
      default: die(0xc3317f);
        //ChangeToTaskState(TASK_CombatMainMenu); //(TASK_CombatEndingTurn);
      };
    };
    break;
  case TASK_CombatDelay_AutoMove:
    ChangeToTaskState(TASK_CombatAutoMoving);
    break;

  default: 
    die(0x8005a);
    ChangeToTaskState(TASK_CombatMainMenu); break;
  }  
}        

static LONGLONG lastIdleTime = 0;
bool COMBAT_EVENT_DATA::OnIdle(void)
{
  //LONGLONG currTime;
  overallCombatState OCS;

  OCS = combatData.GetCombatState();

  return true;// PRS 20110224
              // I don't see any reason for an OnIdle().
              // We should be able to keep things going without
              // any prodding from an idle loop.
              // Wish me luck

  //
  // PRS July 2009
  // Added this switch statement to prevent any combat
  // processing while in TASK_CombatStartBgMusic state.
  // Perhaps other states should be ignored, too?????
  /*
  switch (GetMyState())
  {
  case TASK_CombatStartBgMusic:
    return true;
  default:
    break;
  };
  currTime = virtualGameTime;//timeGetTime();
  if (currTime < lastIdleTime) currTime = lastIdleTime; // rollover
  else if (currTime==lastIdleTime) return true;


  if (combatData.OnAuto(false))
  {
    // need to throttle automatic dudes        
    DWORD interval = GetUserGameSpeed()*120;
    if (interval < 210) interval = 210;
    if (interval > 990) interval = 990;

    if ((currTime-lastIdleTime) < interval)
      return true;
  }
  else if (OCS == OCS_None) 
  {
    ChangeToTaskState(TASK_CombatMainMenu);
    return true;
  }

  // for auto-combatants, the above can
  // return one of these states after Think()
  //
  // SCS_None
  // SCS_Moving
  // SCS_Guarding
  // SCS_Attacking
  // SCS_Fled
  //
  // OCS_Casting  (I want to add this PRS 22Dec2009)

  switch (OCS)
  {
  case OCS_None: // auto
    break;

  case OCS_Attacking: // auto
    if ((GetMyState() != TASK_CombatInRouteSpriteAnimating) && (combatData.NeedMissileAnimation()))
      ChangeToTaskState(TASK_CombatInRouteSpriteAnimating);
    else
      ChangeToTaskState(TASK_CombatAttacking);
    break;

  case OCS_Guarding: // auto
    ChangeToTaskState(TASK_CombatGuardMenu);
    break;

  case OCS_Fleeing:
  case OCS_Moving: // auto
    ChangeToTaskState(TASK_CombatMoving);
    break;

  case OCS_CombatOver:
    ChangeToTaskState(TASK_CombatOver);      
    break;

  case OCS_StartNewRound:
    TRACE("Start new round delay\n");
  case OCS_NewCombatant:
    TRACE("Round delay\n");
    combatData.m_bStartingNewCombatRound = FALSE;
    combatData.m_forceRoundDelay = FALSE;
    ChangeToTaskState(TASK_CombatDelay_NewCombatant);
    break;

  case OCS_Casting:
    {
      COMBATANT &dude = combatData.getActiveCombatCharacter();
      //GLOBAL_SPELL_ID key = dude.GetSpellKeyBeingCast();
      //if (!key.IsValidSpell())
      SPELL_ID spellID = dude.GetSpellIDBeingCast();
      if (!spellID.IsValidSpell())
      {
        combatData.EndTurn();
        ChangeToTaskState(TASK_CombatMainMenu);         
      }
      else
      {   
        ChangeToTaskState(TASK_CombatActivateSpellMsg);
      }
    }
    break;

  case OCS_Bandaging:
    ChangeToTaskState(TASK_CombatBandageMenu);
    break;

  case OCS_Using:
    //ChangeToTaskState(TASK_CombatUsing);
    break;

  case OCS_Turning:
    ChangeToTaskState(TASK_CombatTurnMenu);
    break;

  case OCS_Delaying:
    ChangeToTaskState(TASK_CombatDelayMenu);
    break;

  case OCS_EndingTurn:
    ChangeToTaskState(TASK_CombatEndTurnMenu);
    break;

  case OCS_Fled:  // BUGFIX
    // Cause another OnIdle in oreder to UpdateCombat
    ChangeToTaskState(TASK_CombatMainMenu); // BUGFIX
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent); // BUGFIX
    break; // BUGFIX

  default:
    die(0xab4ef);
    break;
  }

  {
    int updateScreen;
    // moves dude, or can end turn
    updateScreen = combatData.HandleCurrState(FALSE);
    if (updateScreen != 0) 
    {
      OnUpdateUI();
      OnDraw();
    };
  };
  lastIdleTime=currTime;//timeGetTime();
  return true;
  */
}

void COMBAT_EVENT_DATA::OnDraw(void)
{
  DETAILTRACE(0x1b540,0);
  UpdateCombatMainMenuScreen();
  DETAILTRACE(0x1b541,0);
}

bool COMBAT_EVENT_DATA::OnCycle(void)
{
  bool result = false;
  //GameEvent::OnCycle();
  LONGLONG currTime;

  //PRS July 2009
  //
  // Added switch statment to skip combat processing while
  // in CombatStartBgMusic state
  // Perhaps we should do the same for other states???
  switch(GetMyState())
  {
  case TASK_CombatStartBgMusic:
    return true;
  default:
    break;
  };

  currTime = virtualGameTime;//timeGetTime();

  // update drunk points, spell effects, etc
  combatData.ProcessTimeSensitiveData(currTime);
    
  if (UpdateAnimateList())
    result = true;
  
  if (errorTextData.numLines > 0) 
  {
    long diff = currTime - errorTextData.startTime;
    if ((diff > 2000) || (diff < 0))
    {
      ClearFormattedText(errorTextData);
    }
  }

  //OnUpdateUI();
  // auto combatants never leave this event, but
  // updates to the screen still need to be made
  //return true;
  return result;
}

void COMBAT_EVENT_DATA::OnLeaveQueue(void)
{
  combatData.m_bCombatActive = FALSE;
  //combatData.RestoreToParty();
  combatData.Clear();
  if (m_stoppedSounds)
  {
    StartBackgroundMusic();
  };
}

void COMBAT_EVENT_DATA::OnUpdateUI(void)
{  
  // if in quick mode, no menus are displayed
  // if already in action, no menus are displayed
  // if no dude is active, no menus are displayed
  overallCombatState OCS = combatData.GetCombatState();
  int dude = combatData.GetCurrCombatant();

  if (dude == NO_DUDE)
  {
    menu.setAllItemsInactive();
    return;
  }

  //if (OCS != OCS_None)
  //{
    if ((OCS == OCS_Moving)&&(combatData.OnAuto(false)))
    {
      menu.setTitle(FormatCombatMoveText());
      return;
    }

  //  menu.setAllItemsInactive();
  //  return;
  //}
  else if (combatData.OnAuto(false))
  {
    menu.setAllItemsInactive();
    return;
  }


/*
   1  { "MOVE",    0 },
   2  { "AIM",     0 },
   3  { "USE",     0 },
   4  { "CAST",    0 },
   5  { "TURN",    0 },
   6  { "GUARD",   0 },
   7  { "QUICK",   0 },
   8  { "DELAY",   0 },
   9  { "BANDAGE", 0 },
   10 { "VIEW",    0 },
   11 { "SPEED",   0 },
   12 { "WIN",     0 },
   13 { "READY",   0 },
   14 { "END",     0 }
*/

  menu.setAllItemsActive();

  menu.setItemInactive(13);  //ready
  if (!combatData.CanMove())
    menu.setItemInactive(1); // move

  if (!levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].AllowMagic)
    menu.setItemInactive(4); // cast

  if (!combatData.CanCast())
    menu.setItemInactive(4); // cast

  if (!combatData.CanTurnUndead())
    menu.setItemInactive(5); // turn

  if (!combatData.CanGuard(GUARD_CombatMenu))
    menu.setItemInactive(6); // guard    

  if (!combatData.CanDelay())
    menu.setItemInactive(8); // delay      

  if (!combatData.CanBandage())
    menu.setItemInactive(9); // Bandage   

  if (!EditorMode())
    menu.setItemInactive(12); // Win  

  if (combatData.SpecialActionName() == "")
  {
    menu.setItemInactive(15); // Sweep, for example
  }
  else
  {
    menu.changeMenuItem(15,combatData.SpecialActionName(), combatData.SpecialActionIndex());
  };


}

unsigned int COMBAT_EVENT_DATA::OnTaskMessage(TASKMESSAGE msg, TASKSTATE taskState)
{
  m_eTEMPsurprise = m_eSurprise;
  switch (msg)
  {
  case TASKMSG_Combat_MonstersSurpised:
    m_eTEMPsurprise = MonsterSurprised;
    return TASKMSG_STOP;
  case TASKMSG_Combat_PartySurpised:
    m_eTEMPsurprise = PartySurprised;
    return TASKMSG_STOP;
  case TASKMSG_Combat_NoSurpise:
    m_eTEMPsurprise = Neither;
    return TASKMSG_STOP;
  default:
    break;
  }
  return 0;
}

void COMBAT_EVENT_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseMenu(mx, my);
}

////////////////////////////////////////////////////////////////////////COMBAT_RESULTS_MENU_DATA
void COMBAT_RESULTS_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  }

  ClearFormattedText(textData);

  //if (ItemsOnPage > 0)
  //  PushEvent(new TAKE_TREASURE_DATA(m_pOrigEvent), DeleteEvent);
  if (m_pTreasEvent != NULL)
  {
    PushEvent(m_pTreasEvent, DeleteEvent);
    m_pTreasEvent = NULL;
  }
  else
    PopEvent();
}

void COMBAT_RESULTS_MENU_DATA::OnInitialEvent(void)
{
  activeSpellList.RemoveLingerSpells();

  if (!LoadOffscreenWalkingSurfaces())
    WriteDebugString("LoadOffscreenWalkingSurfaces failed\n");  

  menu.setMenu(CombatResultsMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();

  currPic.Clear();
  ClearFormattedText(textData);
  clearTreasureItemState();
  
  m_exptotal = combatData.m_iExpPointTotal;
  combatData.m_iExpPointTotal = 0;

  combatData.RestoreToParty();
  
  if (m_exptotal > 0)
    party.distributeExpPoints(m_exptotal);

  CString tempText;
  switch (combatData.m_eCombatVictor)
  {
  case MonsterWins:
    if (combatData.m_bPartyNeverDies)
    {
      globalData.global_asl.Insert("Combat Result", "LoseButNeverDies", ASLF_MODIFIED);
      if (party.numCharacters == 1)
        tempText = "YOU HAVE SURVIVED!";
      else
        tempText = "THE PARTY HAS SURVIVED!";
      FormatDisplayText(textData, tempText);
    }
    else
    {
      globalData.global_asl.Insert("Combat Result", "Lose", ASLF_MODIFIED);
      //combatData.RestoreToParty();
      bool allHaveDied = true;
      for (int i=0;i<party.numCharacters;i++)
      {
        if (party.characters[i].GetStatus() == Okay)
          allHaveDied = false;          
      }

      if (allHaveDied)
      {
        ReplaceEvent(new PARTY_DIED_MENU_DATA, DeleteEvent);
        return;
      }
      else
      {
        if (party.numCharacters == 1)
          FormatDisplayText(textData, "YOU HAVE LOST");
        else
          FormatDisplayText(textData, "THE PARTY HAS LOST");
      }      
    }
    break;   
  
  case PartyWins: {
    bool stopMessage = false;
    globalData.global_asl.Insert("Combat Result", "Win", ASLF_MODIFIED);
    CString winMessage;
    const ASLENTRY* pASL = m_pOrigEvent->control.eventcontrol_asl.Find("CombatWinMessage");
    if (pASL != NULL) { winMessage = pASL->Value(); }
    CString leaveTreasureMessage;
    const ASLENTRY* pASL2 = m_pOrigEvent->control.eventcontrol_asl.Find("TreasureLeftMessage");
    if (pASL2 != NULL) { leaveTreasureMessage = pASL2->Value(); }
    char s_exptotal[20];
    itoa(m_exptotal, s_exptotal, 10);
    if (winMessage.IsEmpty()) {
        HOOK_PARAMETERS hookParameters;
        hookParameters[1] = s_exptotal;
        winMessage = getGlobalEventMessage("", "EventCombatWin", hookParameters, "", control);
    }
    if (!winMessage.IsEmpty()) {
      tempText = winMessage;
      tempText.Replace("^X", s_exptotal);
      stopMessage = true;
    } else
    {
      if (party.numCharacters == 1)
        tempText = "YOU HAVE WON!";
      else
        tempText = "THE PARTY HAS WON!";

      if (m_exptotal > 0)
      {
        CString tmp;
        if (party.numCharacters == 1)
          tmp.Format("\r\nYOU RECEIVE %i EXPERIENCE POINTS",
            m_exptotal);
          else
            tmp.Format("\r\nTHE PARTY RECEIVES %i EXPERIENCE POINTS",
                 m_exptotal);
        tempText += tmp;

        // don't do this here
        //party.distributeExpPoints(combatData.expPointTotal);
        //exptotal = combatData.expPointTotal;
        //combatData.expPointTotal = 0;
      }
    }
    {
      int i;
      for (i=0; i<party.numCharacters; i++)
      {
        if (party.characters[i].GetStatus() == Fled)
        {
          party.characters[i].SetStatus(Okay);
        };
      };
    };
    m_pTreasEvent = new GIVE_TREASURE_DATA();
    if (!((COMBAT_EVENT_DATA*)(m_pOrigEvent))->noMonsterTreasure)
    {
      //setTreasureEventState(m_pOrigEvent);

	    for (int i=0; i<combatData.NumCombatants(); i++)
	    {
        if ((!combatData.GetCombatant(i)->IsPartyMember()) && (combatData.GetCombatant(i)->GetAdjStatus() == Dead))
        {
          POSITION pos = combatData.GetCombatant(i)->m_pCharacter->myItems.GetHeadPosition();
          while (pos != NULL)
          {
            ITEM_DATA *pItem;
            ITEM *pMyItem;
            pMyItem = &combatData.GetCombatant(i)->m_pCharacter->myItems.GetNext(pos);
            //pItem = itemData.GetItemData(pMyItem->m_giID);
            pItem = itemData.GetItem(pMyItem->itemID);
            if (    (pItem->Wpn_Type != SpellCaster) && (pItem->Wpn_Type != SpellLikeAbility)
                 || pItem->CanBeTradeDropSoldDep)
            {
              m_pTreasEvent->items.AddItem(*pMyItem);
            };
          };
          m_pTreasEvent->money.Add(combatData.GetCombatant(i)->m_pCharacter->money);
        }
      }

      {
        POSITION pos;
        pos = combatData.hurledWeapons.GetHeadPosition();
        while (pos != NULL)
        {
          ITEM *pHurledItem;
          pHurledItem = &combatData.hurledWeapons.GetNext(pos);
          m_pTreasEvent->items.AddItem(*pHurledItem);
        };
      };
    };
    {
      ITEM *pItem;
      POSITION pos;
      pos = globalData.combatTreasure.items.GetHeadPosition();
      while (pos != NULL)
      {
        pItem = &globalData.combatTreasure.items.GetNext(pos);
        m_pTreasEvent->items.AddItem(*pItem);
      };
      globalData.combatTreasure.items.Clear();
    };

    {
      m_pTreasEvent->money.Add(globalData.combatTreasure.money);
      globalData.combatTreasure.money.Clear();
    };

      //if (ItemsOnPage > 0)
    if ((m_pTreasEvent->items.GetHeadPosition() != NULL) || (m_pTreasEvent->money.Total() > 0.0))
    {
      CString tmp;
      const char* scriptName;
      // Insert the treasure message for the subsequent GiveTreasure event
      HOOK_PARAMETERS hookParameters;
      m_pTreasEvent->control.eventcontrol_asl.Insert("TreasureMessage", getGlobalEventMessage("", "EventCombatTreasure", hookParameters, "TreasureMessage", control), ASLF_MODIFIED | ASLF_READONLY);

      // If treasure leaving attribute is supplied, pass it on to the subsequent GiveTreasure
      if (!leaveTreasureMessage.IsEmpty()) {
        m_pTreasEvent->control.eventcontrol_asl.Insert("TreasureLeftMessage", leaveTreasureMessage, ASLF_MODIFIED | ASLF_READONLY);
      }
      if (party.numCharacters == 1)
        tmp += "\r\nYOU HAVE FOUND TREASURE!";
      else
        tmp += "\r\nTHE PARTY HAS FOUND TREASURE!";
      if (!stopMessage) {
        tempText += tmp;
      }
    }
    else
    {
      delete m_pTreasEvent;
      m_pTreasEvent = NULL;
    };
    FormatDisplayText(textData, tempText);
    break;
    }
  case PartyRanAway:
    globalData.global_asl.Insert("Combat Result", "Flee", ASLF_MODIFIED);
    if (party.numCharacters == 1)
      FormatDisplayText(textData, "YOU HAVE RUN AWAY");
    else
      FormatDisplayText(textData, "THE PARTY HAS RUN AWAY");

    {
      int i;
      for (i=0; i< party.numCharacters; i++)
      {
        if (party.characters[i].GetStatus() == Fled)
        {
          party.characters[i].SetStatus(Okay);
        };
      };
    };
    break;

  default: // likely to be an idle timeout
    FormatDisplayText(textData, "THIS COMBAT HAS NO WINNER");
    break;
  }
  EnableUserInput(TRUE);
  Invalidate();
}

void COMBAT_RESULTS_MENU_DATA::OnDraw(void)
{
  UpdateCombatResultsScreen();
}

void COMBAT_RESULTS_MENU_DATA::OnReturnToTopOfQueue(void)
{
  // returning from TAKE_ITEMS_EVENT
  PopEvent();
}

void COMBAT_RESULTS_MENU_DATA::OnLeaveQueue(void)
{
  //combatData.RestoreToParty();
  //if (exptotal > 0)
  //  party.distributeExpPoints(exptotal);
}

////////////////////////////////////////////////////////////////////////COMBAT_AIM_MENU_DATA
void COMBAT_AIM_MENU_DATA::OnInitialEvent(void)
{
  //ClearFormattedText(textData);
  combatData.RemoveCurrTarget();
  combatData.ResetAim();
  menu.setMenu(AimMenuData, NULL, TRUE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
  Invalidate();
}
void COMBAT_AIM_MENU_DATA::OnKeypress(key_code key, char ascii)
{

//   1 "NEXT"
//   2 "PREV"
//   3 "MANUAL"
//   4 "TARGET"
//   5 "CENTER"
 //  6 "EXIT"


  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  }

  switch (menu.currentItem()) 
  {
  case 1: // next
    combatData.GetNextAim();
    Invalidate();
    break;
  case 2: // prev
    combatData.GetPrevAim();
    Invalidate();
    break;
  case 3: // manual
    ReplaceEvent(new COMBAT_AIM_MANUAL_MENU_DATA(), DeleteEvent);
    break;
  case 4: // target    
    if (combatData.CanAttack(combatData.GetCurrAim()))
    {
      combatData.AddTarget(combatData.GetCurrAim());
      //combatData.StartAttack();
      PopEvent();
      return;
    }
    else
      combatData.RemoveCurrTarget();
    break;
  case 5: // center
    EnsureVisibleCursor(TRUE);
    break;
  case 6: // exit, no target chosen
    combatData.PlaceCursorOnCurrentDude();
    combatData.RemoveCurrTarget();
    PopEvent();
    break;
  default:
    WriteDebugString("Unhandled state in COMBAT_AIM_MENU_DATA::OnKeyPress()\n");
    miscError = InvalidMenuState;
    break;
  }
}

void COMBAT_AIM_MENU_DATA::OnDraw(void)
{
  UpdateCombatAimScreen();
}

void COMBAT_AIM_MENU_DATA::OnUpdateUI(void)
{

//   1 "NEXT"
//   2 "PREV"
//   3 "MANUAL"
//   4 "TARGET"
//   5 "CENTER"
 //  6 "EXIT"

  menu.setAllItemsActive();
  if (!combatData.IsAttackPossible()) // Eg: Range weapon with adjacent opponents.
  {
    //menu.setItemInactive(1); //NEXT
    //menu.setItemInactive(2); //PREV
    //menu.setItemInactive(3); //MANUAL
    menu.setItemInactive(4); //TARGET
    menu.setItemInactive(5); //CENTER
  }
  else
  {
    if (!combatData.CanTarget(false))
    {
      menu.setItemInactive(4); // target
    }
    else
    {
      int target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
      if ((target != NO_DUDE) && (target < combatData.NumCombatants()))
      {
        if (!combatData.CanAttack(target))
          menu.setItemInactive(4); // target
      }
      else
        menu.setItemInactive(4); // target
    };
  };
}

void COMBAT_AIM_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseMenu(mx, my);
}

////////////////////////////////////////////////////////////////////////COMBAT_AIM_MANUAL_MENU_DATA
void COMBAT_AIM_MANUAL_MENU_DATA::OnInitialEvent(void)
{
  //ClearFormattedText(textData);
  combatData.RemoveCurrTarget();
  combatData.ResetAim();
  menu.setMenu(AimManualMenuData, NULL, TRUE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 2); //exit
  Invalidate();
}

void COMBAT_AIM_MANUAL_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  switch (key)
  {
  case KC_RETURN: // target=1 exit=2
  {
    int target;
    switch (menu.currentItem()) 
    {

    case 1: //target
      target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
      if ((target != NO_DUDE) && (target < combatData.NumCombatants()))
      {
        if (combatData.CanTarget(false))
        {
          if (combatData.CanAttack(target))
            combatData.AddTarget(target);
          else
            combatData.RemoveCurrTarget();
        }
        else
          combatData.RemoveCurrTarget();
      }
      else
        combatData.RemoveCurrTarget();
      PopEvent();
      return;
    case 2:
      PlaceCursorOnCurrentDude();
      PopEvent();
      return;
    };
  };

  case KC_POSITION: // Numpad_0 or Insert
  {
    if (!ValidTerrainViewportCoords(combatData.m_iMoveToX,combatData.m_iMoveToY))
    {
      combatData.m_iMoveToX=-1;
      combatData.m_iMoveToY=-1;
      break;
    }

    int oldX = combatData.m_iCursorX;
    int oldY = combatData.m_iCursorY;
    combatData.m_iCursorX = combatData.m_iMoveToX;
    combatData.m_iCursorY = combatData.m_iMoveToY;
    ForceMapToMoveAlong(combatData.m_iMoveToX, combatData.m_iMoveToY, 
                        GetDir(oldX, oldY, combatData.m_iMoveToX, combatData.m_iMoveToY),
                        1);
    combatData.m_iMoveToX=-1;
    combatData.m_iMoveToY=-1;
    break;
  }
  case KC_CENTER: // Numpad_5
    combatData.PlaceCursorOnCurrentDude(TRUE);
    break;
  case KC_LEFT:
    combatData.moveCursorLeft();
    break;
  case KC_RIGHT:
    combatData.moveCursorRight();
    break;
  case KC_UP:
    combatData.moveCursorForward();
    break;
  case KC_DOWN:
    combatData.moveCursorBackward();
    break;
  case KC_NW:
    combatData.moveCursorLeft();
    combatData.moveCursorForward();
    break;
  case KC_NE:
    combatData.moveCursorRight();
    combatData.moveCursorForward();
    break;
  case KC_SE:
    combatData.moveCursorRight();
    combatData.moveCursorBackward();
    break;
  case KC_SW:
    combatData.moveCursorLeft();
    combatData.moveCursorBackward();
    break;
  default:
    StandardMenuKeyboardAction(key,ascii);
    break;
  }
  Invalidate();
}

void COMBAT_AIM_MANUAL_MENU_DATA::OnDraw(void)
{
  UpdateCombatAimManualScreen();
}

void COMBAT_AIM_MANUAL_MENU_DATA::OnUpdateUI(void)
{
  menu.setAllItemsActive();
  if (!combatData.CanTarget(false))
  {
    menu.setItemInactive(1); // target
  }
  else
  {
    int target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
    if ((target != NO_DUDE) && (target < combatData.NumCombatants()))
    {
      if (!combatData.CanAttack(target))
        menu.setItemInactive(1); // target
    }
    else
      menu.setItemInactive(1); // target
  }
}

void COMBAT_AIM_MANUAL_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseCombatMapMenu(mx,my);
}

////////////////////////////////////////////////////////////////////////COMBAT_SPELL_AIM_MENU_DATA
void COMBAT_SPELL_AIM_MENU_DATA::OnInitialEvent(void)
{
  if (m_pCaster==NULL)
  {
    die(0xab4f0);
    PopEvent();
    return;
  }
  if (!m_pCaster->m_pCharacter->targets.ValidNumTargets())
  {
    die(0xab4f1);
    PopEvent();
    return;
  }
  combatData.ResetAim();
  menu.setMenu(AimMenuData, NULL, TRUE, NULL, NULL);
  menu.setHorzOrient();
  CString title(m_pCaster->m_pCharacter->targets.FormatRemainingTargsText());
  menu.setTitle(title);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
  SetMyState(TASK_initialized);
  Invalidate();

}

void COMBAT_SPELL_AIM_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key, ascii);
    return;
  }

  switch (menu.currentItem()) 
  {
  case 1: // next
    combatData.GetNextAim();
    break;
  case 2: // prev
    combatData.GetPrevAim();
    break;
  case 3: // manual
    PushEvent(new COMBAT_SPELL_AIM_MANUAL_MENU_DATA(m_pCaster, m_pSpell), DeleteEvent);
    break;
  case 4: // target
    {    
      // automatic aiming always points to a valid target
      int target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
      if (target != NO_DUDE)
      {
        m_pTarget = combatData.getCombatantPtr(target);
        ASSERT(m_pTarget!=NULL);
        if (m_pCaster->m_pCharacter->targets.m_SelectingUnits)
        {
          // This version of 'distance' accounts for icons that are
          // larger than 1x1. The x,y is always the upper,left. The
          // distance is computed based on the edges of the icons that
          // are closest to each other.
          int targrange = Distance(m_pCaster->self, m_pCaster->x ,m_pCaster->y, 
                                   m_pTarget->self, m_pTarget->y);
          if (!m_pCaster->C_AddTarget(*m_pTarget, targrange))
            WriteDebugString("Failed AddTarget() in COMBAT_SPELL_AIM_MENU_DATA\n");
          {
            CString choices;
            choices = OverrideTargetingHook(m_pSpell, m_pCaster, 
                                            m_pTarget, 
                                            m_pTarget->GetCenterX(), m_pTarget->GetCenterY(), "C");
            if (!choices.IsEmpty())
            {
              m_choosingTarget = true;
              m_selectingUnits = true;
              PushEvent(new CHOOSE_FROM_LIST_MENU_DATA(choices, &m_chooseTargetResult), DeleteEvent);
              return;
            };
          };
        }
        else
        {
          PATH_DIR dir = GetSpellTargetingDir(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                                              m_pTarget->GetCenterX(),m_pTarget->GetCenterY());
          int dirX, dirY;
          dirX = m_pTarget->GetCenterX() - m_pCaster->GetCenterX();
          dirY = m_pTarget->GetCenterY() - m_pCaster->GetCenterY();
          if (!m_pCaster->AddMapTarget(m_pTarget->GetCenterX(), m_pTarget->GetCenterY(), dir, dirX, dirY))
            WriteDebugString("Failed AddTarget() in COMBAT_SPELL_AIM_MENU_DATA\n");
          {
            CString choices;
            choices = OverrideTargetingHook(m_pSpell, 
                                            m_pCaster, 
                                            m_pTarget,
                                            m_pTarget->GetCenterX(), m_pTarget->GetCenterY(), "M");
            if (!choices.IsEmpty())
            {
              m_choosingTarget = true;
              m_selectingUnits = false;
              PushEvent(new CHOOSE_FROM_LIST_MENU_DATA(choices, &m_chooseTargetResult), DeleteEvent);
            };
            //return;
          };
        };
        if (m_pCaster->m_pCharacter->targets.AllTargetsChosen())
        {
          PopEvent(); // all targets chosen
          return;
        }
        else
        {
          CString title(m_pCaster->m_pCharacter->targets.FormatRemainingTargsText());
          menu.setTitle(title);
          combatData.GetNextAim();
        }
      }
    }
    break;
  case 5: // center
    EnsureVisibleCursor(TRUE);
    break;
  case 6: // exit
    if (m_pCaster->m_pCharacter->targets.m_SelectingUnits)
    {
      if (m_pCaster->m_pCharacter->targets.NumTargets()==0)
      {
        SetMyState(TASK_CombatAskAbortSpell);
        PushEvent(new ASK_YES_NO_MENU_DATA("ABORT THIS SPELL?",TRUE, CONTROL_CombatSpellAimAuto), DeleteEvent);
        return;
      }
    }
    else
    {
      if ((m_pCaster->m_pCharacter->targets.m_MapTargetX<0)||(m_pCaster->m_pCharacter->targets.m_MapTargetY<0))
      {
        SetMyState(TASK_CombatAskAbortSpell);
        PushEvent(new ASK_YES_NO_MENU_DATA("ABORT THIS SPELL?",TRUE, CONTROL_CombatSpellAimAuto), DeleteEvent);
        return;
      }
    }
    PopEvent();
    break;
  default:
    WriteDebugString("Unhandled state in COMBAT_SPELL_AIM_MENU_DATA::OnKeyPress()\n");
    miscError = InvalidMenuState;
    break;
  }
}

void COMBAT_SPELL_AIM_MENU_DATA::OnReturnToTopOfQueue(void)
{
  if (m_choosingTarget)
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    hookParameters[5] = m_chooseTargetResult;
    scriptContext.SetSpellContext(m_pSpell->SpellID());
    scriptContext.SetAttackerContext(m_pCaster);
    if (m_pTarget == NULL)
    {
      die(0xc3107);
    };
    scriptContext.SetTargetContext(m_pTarget);
    m_pSpell->RunSpellScripts(PROCESS_SPELL_TARGET_OVERRIDE, 
                              ScriptCallback_RunAllScripts, 
                              NULL,
                              "Choosing target for spell");
    m_choosingTarget = false;
    if (!m_selectingUnits)
    {
      PopEvent();
      return;
    }
    if (m_pCaster->m_pCharacter->targets.AllTargetsChosen())
    {
      PopEvent(); // all targets chosen
      return;
    }
    else
    {
      CString title(m_pCaster->m_pCharacter->targets.FormatRemainingTargsText());
      menu.setTitle(title);
      combatData.GetNextAim();
      return;
    }
  };
  if (GetMyState()==TASK_CombatAskAbortSpell)
  {
    if (party.tradeItem==1) // yes, abort current spell
    {
      PopEvent();
      return;
    }
    else
      OnInitialEvent();
  }
  else
  {
    if (m_pCaster->m_pCharacter->targets.AllTargetsChosen())
      PopEvent();
    else
      OnInitialEvent();
  }
}

void COMBAT_SPELL_AIM_MENU_DATA::OnDraw(void)
{
  UpdateCombatAimScreen();
}

void COMBAT_SPELL_AIM_MENU_DATA::OnUpdateUI(void)
{
  menu.setAllItemsActive();
  if (m_pCaster->m_pCharacter->targets.m_SelectingUnits)
  {
    int target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
    if (target != NO_DUDE)
    {
      COMBATANT *pTarget = combatData.getCombatantPtr(target);
      ASSERT(pTarget!=NULL);

      if (!HaveLineOfSight(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                           pTarget->GetCenterX(), pTarget->GetCenterY(), NULL))
      {
        menu.setItemInactive(4); // target
      }
      if (!m_pCaster->m_pCharacter->targets.m_canTargetFriend)
      {
        if (pTarget->GetIsFriendly())
          menu.setItemInactive(4); // target
      }
      if (!m_pCaster->m_pCharacter->targets.m_canTargetEnemy)
      {
        if (!pTarget->GetIsFriendly())
          menu.setItemInactive(4); // target
      }

      // these targeting types use the direction between caster and targeted
      // map square to determine which way to direct the spell
      //
      // the path will be PathBAD if the cursor is on the caster, which
      // gives an indeterminate direction from caster to targeted square.
      //      
      if (  (m_pCaster->m_pCharacter->targets.m_targType==NowAreaLinePickStart)
          ||(m_pCaster->m_pCharacter->targets.m_targType==LingerAreaLinePickStart)
          ||(m_pCaster->m_pCharacter->targets.m_targType==NowAreaCone)
          ||(m_pCaster->m_pCharacter->targets.m_targType==LingerAreaCone)
         )
      {
        PATH_DIR dir = GetSpellTargetingDir(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                                            pTarget->GetCenterX(),pTarget->GetCenterY());

        if (dir == PathBAD)
          menu.setItemInactive(4); // target
      }

      int targrange = Distance(m_pCaster->self, m_pCaster->x,m_pCaster->y, 
                               pTarget->self, pTarget->x,pTarget->y);
      //if (targrange > m_pCaster->m_pCharacter->targets.m_MaxRange)
      if (targrange > m_pCaster->m_pCharacter->targets.MaxRange())
      {
        menu.setItemInactive(4); // target
      };
      {
        int dx, dy;
        dx = abs(m_pCaster->GetCenterX()-combatData.m_iCursorX);
        dy = abs(m_pCaster->GetCenterY()-combatData.m_iCursorY);
        if (dx > m_pCaster->m_pCharacter->targets.m_maxRangeX)
        {
          menu.setItemInactive(4); // target
        };
        if (dy > m_pCaster->m_pCharacter->targets.m_maxRangeY)
        {
          menu.setItemInactive(4); // target
        };
      };

      if (!m_pCaster->GetAdjDetectingInvisible())
      {
        if (pTarget->GetAdjSpecAb(SA_Invisible))
          menu.setItemInactive(4); // target
      
        //if (m_pCaster->GetUndeadType() != NotUndead)
        if (!m_pCaster->GetUndeadType().IsEmpty())
        {
          if (pTarget->GetAdjSpecAb(SA_InvisibleToUndead))
            menu.setItemInactive(4); // target
        }
      
        if (m_pCaster->IsAnimal())
        {
          if (pTarget->GetAdjSpecAb(SA_InvisibleToAnimals))
            menu.setItemInactive(4); // target
        }
      }
    }
    else
      menu.setItemInactive(4); // target
  }
  else
  {
    // these targeting types use the direction between caster and targeted
    // map square to determine which way to direct the spell
    //
    // the path will be PathBAD if the cursor is on the caster, which
    // gives an indeterminate direction from caster to targeted square.
    //
    if (   (m_pCaster->m_pCharacter->targets.m_targType==NowAreaLinePickStart)
         ||(m_pCaster->m_pCharacter->targets.m_targType==LingerAreaLinePickStart)
         ||(m_pCaster->m_pCharacter->targets.m_targType==NowAreaCone)
         ||(m_pCaster->m_pCharacter->targets.m_targType==LingerAreaCone)
      )
    {
      PATH_DIR dir = GetSpellTargetingDir(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                                          combatData.m_iCursorX, combatData.m_iCursorY);
      if (dir == PathBAD)
        menu.setItemInactive(4); // target
    }
  }
}

void COMBAT_SPELL_AIM_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseMenu(mx, my);
}

////////////////////////////////////////////////////////////////////////COMBAT_SPELL_AIM_MANUAL_MENU_DATA
void COMBAT_SPELL_AIM_MANUAL_MENU_DATA::OnInitialEvent(void)
{
  combatData.ResetAim();
  menu.setMenu(AimMenuData, NULL, TRUE, NULL, NULL);
  menu.setHorzOrient();
  CString title(m_pCaster->m_pCharacter->targets.FormatRemainingTargsText());
  menu.setTitle(title);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
  m_isValidTarget = ' ';
  Invalidate();
}

void COMBAT_SPELL_AIM_MANUAL_MENU_DATA::OnReturnToTopOfQueue()
{








  if (m_choosingTarget)
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    hookParameters[5] = m_chooseTargetResult;
    scriptContext.SetSpellContext(m_pSpell->SpellID());
    scriptContext.SetAttackerContext(m_pCaster);
    if (m_pTarget == NULL)
    {
      die(0xc3107);
    };
    scriptContext.SetTargetContext(m_pTarget);
    m_pSpell->RunSpellScripts(PROCESS_SPELL_TARGET_OVERRIDE, 
                              ScriptCallback_RunAllScripts, 
                              NULL,
                              "Combat manual spell aim");
    m_choosingTarget = false;
    if (!m_selectingUnits)
    {
      PopEvent();
      return;
    }
    if (m_pCaster->m_pCharacter->targets.AllTargetsChosen())
    {
      PopEvent(); // all targets chosen
      return;
    }
    else
    {
      CString title(m_pCaster->m_pCharacter->targets.FormatRemainingTargsText());
      menu.setTitle(title);
      combatData.GetNextAim();
      return;
    }
  };






}


void COMBAT_SPELL_AIM_MANUAL_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  /*
   { "NEXT", 0 },
   { "PREV", 0 },
   { "MANUAL", 0 },
   { "TARGET", 0 },
   { "CENTER", 0 },
   { "EXIT", 1 }
   */
  switch (key)
  {
  case KC_RETURN:
  {
    switch (menu.currentItem()) 
    {
    case 1: // next disabled
      //combatData.GetNextAim();
      break;
    case 2: // prev disabled
      //combatData.GetPrevAim();
      break;
    case 3: // manual disabled
      //PushEvent(new COMBAT_SPELL_AIM_MANUAL_MENU_DATA(m_pCaster), DeleteEvent);
      break;
    case 4: // target
      {
        int target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
        // manual aiming can point to map or character target
        if (m_pCaster->m_pCharacter->targets.m_SelectingUnits)
        {
          if (target != NO_DUDE)
          {
            COMBATANT *pTarget = combatData.getCombatantPtr(target);
            ASSERT(pTarget!=NULL);

            // This version of 'distance' accounts for icons that are
            // larger than 1x1. The x,y is always the upper,left. The
            // distance is computed based on the edges of the icons that
            // are closest to each other.
            int targrange = Distance(m_pCaster->self, m_pCaster->x,m_pCaster->y, 
                                     pTarget->self, pTarget->x,pTarget->y);
            if (!m_pCaster->C_AddTarget(*pTarget, targrange))
              WriteDebugString("Failed AddTarget() in COMBAT_SPELL_AIM_MENU_DATA\n");
          }
        }
        else
        {
          PATH_DIR dir = GetSpellTargetingDir(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                                              combatData.m_iCursorX, combatData.m_iCursorY);
          int dirX, dirY;
          dirX = combatData.m_iCursorX - m_pCaster->GetCenterX();
          dirY = combatData.m_iCursorY - m_pCaster->GetCenterY();
          if (!m_pCaster->AddMapTarget(combatData.m_iCursorX, combatData.m_iCursorY, dir, dirX, dirY))
          {
            WriteDebugString("Failed AddTarget() in COMBAT_SPELL_AIM_MANUAL_MENU_DATA\n");
          };
          if (target!= NO_DUDE)

          {
            CString choices;
            m_pTarget = combatData.getCombatantPtr(target);
            ASSERT(m_pTarget!=NULL);
            choices = OverrideTargetingHook(m_pSpell, 
                                            m_pCaster, 
                                            m_pTarget,
                                            m_pTarget->GetCenterX(), m_pTarget->GetCenterY(), "M");
            if (!choices.IsEmpty())
            {
              m_choosingTarget = true;
              m_selectingUnits = false;
              PushEvent(new CHOOSE_FROM_LIST_MENU_DATA(choices, &m_chooseTargetResult), DeleteEvent);
              return;
            };
          };

        }
        if (m_pCaster->m_pCharacter->targets.AllTargetsChosen())
        {
          PopEvent(); // all targets chosen
          return;
        }
        else
        {
          CString title(m_pCaster->m_pCharacter->targets.FormatRemainingTargsText());
          menu.setTitle(title);
        }
      }
      break;
    case 5: // center disabled
      //EnsureVisibleCursor(TRUE);
      break;
    case 6: // exit
      PopEvent();
      return;
    default:
      WriteDebugString("Unhandled state in COMBAT_SPELL_AIM_MANUAL_MENU_DATA::OnKeyPress()\n");
      miscError = InvalidMenuState;
      break;
    }
  }
  Invalidate();
  return;

  case KC_POSITION: // Numpad_0 or Insert
  {
    if (!ValidTerrainViewportCoords(combatData.m_iMoveToX,combatData.m_iMoveToY))
    {
      combatData.m_iMoveToX=-1;
      combatData.m_iMoveToY=-1;
      break;
    }

    int oldX = combatData.m_iCursorX;
    int oldY = combatData.m_iCursorY;
    combatData.m_iCursorX = combatData.m_iMoveToX;
    combatData.m_iCursorY = combatData.m_iMoveToY;
    ForceMapToMoveAlong(combatData.m_iMoveToX, combatData.m_iMoveToY, 
                        GetDir(oldX, oldY, combatData.m_iMoveToX, combatData.m_iMoveToY),
                        1);
    combatData.m_iMoveToX = -1;
    combatData.m_iMoveToY = -1;
    break;
  }
  case KC_CENTER: // Numpad_5
    combatData.PlaceCursorOnCurrentDude(TRUE);
    break;
  case KC_LEFT:
    if (combatData.CanMoveCursorLeft(m_pCaster))
    {
      combatData.moveCursorLeft();
    };
    break;
  case KC_RIGHT:
    if (combatData.CanMoveCursorRight(m_pCaster))
    {
      combatData.moveCursorRight();
    };
    break;
  case KC_UP:
    if (combatData.CanMoveCursorForward(m_pCaster))
    {
      combatData.moveCursorForward();
    };
    break;
  case KC_DOWN:
    if (combatData.CanMoveCursorBackward(m_pCaster))
    {
      combatData.moveCursorBackward();
    };
    break;
  case KC_NW:
    if (combatData.CanMoveCursorNW(m_pCaster))
    {
      combatData.moveCursorLeft();
      combatData.moveCursorForward();
    };
    break;
  case KC_NE:
    if (combatData.CanMoveCursorNE(m_pCaster))
    {
      combatData.moveCursorRight();
      combatData.moveCursorForward();
    };
    break;
  case KC_SE:
    if (combatData.CanMoveCursorSE(m_pCaster))
    {
      combatData.moveCursorRight();
      combatData.moveCursorBackward();
    };
    break;
  case KC_SW:
    if (combatData.CanMoveCursorSW(m_pCaster))
    {
      combatData.moveCursorLeft();
      combatData.moveCursorBackward();
    };
    break;
  default:
    StandardMenuKeyboardAction(key,ascii);
    break;
  };
  {
    CString result;
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    COMBATANT *pTarget;
    

    int target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
    if (target != NO_DUDE)
    {
      pTarget = combatData.getCombatantPtr(target);
      ASSERT(pTarget!=NULL);
      scriptContext.SetCombatantContext(pTarget);
      scriptContext.SetTargetContext(pTarget);
    }
    else
    {
      pTarget = NULL;
    };
    scriptContext.SetAttackerContext(m_pCaster);
    scriptContext.SetSpellContext(m_pSpell);
    hookParameters[5].Format("%i",combatData.m_iCursorX);
    hookParameters[6].Format("%i",combatData.m_iCursorY);

    result = m_pSpell->RunSpellScripts(IS_VALID_TARGET, 
                                       ScriptCallback_LookForChar, 
                                       "YN",
                                       "Combat manual spell aim");
    if (result == "")
    {
      if (pTarget != NULL)
      {
        result = pTarget->RunCombatantScripts(
                          IS_VALID_TARGET, 
                          ScriptCallback_LookForChar, 
                          "YN",
                          "Combat manual spell aim");
      };
    };
    m_isValidTarget = ' ';
    if (!result.IsEmpty())
    {
      m_isValidTarget = result[0];
    }
  };

  Invalidate();
}

void COMBAT_SPELL_AIM_MANUAL_MENU_DATA::OnDraw(void)
{
  UpdateCombatAimManualScreen();
}

void COMBAT_SPELL_AIM_MANUAL_MENU_DATA::OnUpdateUI(void)
{
  /*
   1 "NEXT"
   2 "PREV"
   3 "MANUAL"
   4 "TARGET"
   5 "CENTER"
   6 "EXIT"
   */

  menu.setAllItemsActive();
  menu.setItemInactive(1); // next
  menu.setItemInactive(2); // prev
  menu.setItemInactive(3); // manual
  menu.setItemInactive(5); // center


  if (m_isValidTarget == ' ')
  {

    // these targeting types use the direction between caster and targeted
    // map square to determine which way to direct the spell
    //
    // the path will be PathBAD if the cursor is on the caster, which
    // gives an indeterminate direction from caster to targeted square.
    //
    if (   (m_pCaster->m_pCharacter->targets.m_targType==NowAreaLinePickStart)
         ||(m_pCaster->m_pCharacter->targets.m_targType==LingerAreaLinePickStart)
         ||(m_pCaster->m_pCharacter->targets.m_targType==NowAreaCone)
         ||(m_pCaster->m_pCharacter->targets.m_targType==LingerAreaCone)
       )
    {
      PATH_DIR dir = GetSpellTargetingDir(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                                          combatData.m_iCursorX, combatData.m_iCursorY);
      if (dir == PathBAD)
        menu.setItemInactive(4); // target
    };
  
    if (   (m_pCaster->m_pCharacter->targets.m_targType==NowAreaCircle)
         ||(m_pCaster->m_pCharacter->targets.m_targType==LingerAreaCircle)
       )
    {
      if (!HaveLineOfSight(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                           combatData.m_iCursorX, combatData.m_iCursorY, NULL))
        menu.setItemInactive(4); // target
      if (!HaveMovability(combatData.m_iCursorY, combatData.m_iCursorX))
        menu.setItemInactive(4); // target
    };
    
    if (m_pCaster->m_pCharacter->targets.m_SelectingUnits)
    {
      int target = getCombatantInCell(combatData.m_iCursorX, combatData.m_iCursorY, 1,1);
      if (target != NO_DUDE)
      {
        {
          COMBATANT *pTarget = combatData.getCombatantPtr(target);
          ASSERT(pTarget!=NULL);

          if (!HaveLineOfSight(m_pCaster->GetCenterX(), m_pCaster->GetCenterY(), 
                              pTarget->GetCenterX(), pTarget->GetCenterY(),NULL))
          {
            menu.setItemInactive(4); // target
          }
          
          if (!m_pCaster->m_pCharacter->targets.m_canTargetFriend)
          {
            if (pTarget->GetIsFriendly())
            {
              menu.setItemInactive(4); // target
            };
          };
          if (!m_pCaster->m_pCharacter->targets.m_canTargetEnemy)
          {
            if (!pTarget->GetIsFriendly())
            {
              menu.setItemInactive(4); // target
            };
          };
        };
        {
          COMBATANT *pTarget = combatData.getCombatantPtr(target);
          if (pTarget != NULL)
          {
            if (!m_pCaster->CanAddTarget(target)) 
            {
              menu.setItemInactive(4); //TARGET
            };
          };
        };
      }
      else
        menu.setItemInactive(4); // target

      int targrange = Distance(m_pCaster->x,m_pCaster->y, 
                              combatData.m_iCursorX, combatData.m_iCursorY);
      //if (targrange > m_pCaster->m_pCharacter->targets.m_MaxRange)
      if (targrange > m_pCaster->m_pCharacter->targets.MaxRange())
        menu.setItemInactive(4); // target
      {
        int dx, dy;
        dx = abs(m_pCaster->GetCenterX()-combatData.m_iCursorX);
        dy = abs(m_pCaster->GetCenterY()-combatData.m_iCursorY);
        if (dx > m_pCaster->m_pCharacter->targets.m_maxRangeX)
        {
          menu.setItemInactive(4); // target
        };
        if (dy > m_pCaster->m_pCharacter->targets.m_maxRangeY)
        {
          menu.setItemInactive(4); // target
        };
      };
    };


  }
  else
  {
    if (m_isValidTarget == 'N')
    {
      menu.setItemInactive(4);
    };
  };
  Invalidate();
}

void COMBAT_SPELL_AIM_MANUAL_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseCombatMapMenu(mx,my);
}

////////////////////////////////////////////////////////////////////////COMBAT_MOVE_MENU_DATA
void COMBAT_MOVE_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  switch (key)
  {
  case KC_RETURN: // exit move
    combatData.StopMoving();
    PopEvent();
    return;

  case KC_POSITION: // Numpad_0 or Insert
    if (!ValidTerrainViewportCoords(combatData.m_iMoveToX,combatData.m_iMoveToY))
    {
      combatData.m_iMoveToX=-1;
      combatData.m_iMoveToY=-1;
      return; // PRS 20110502
      break;
    }
    if (   (combatData.getActiveCombatCharacter().x == combatData.m_iMoveToX)
        && (combatData.getActiveCombatCharacter().y == combatData.m_iMoveToY))
    {
      combatData.m_iMoveToX = -1;
      combatData.m_iMoveToY = -1;
      break;
    }

    switch (GetDir(combatData.getActiveCombatCharacter().x,
                   combatData.getActiveCombatCharacter().y,
                   combatData.m_iMoveToX, 
                   combatData.m_iMoveToY))
    {
    case PathNorth:
      combatData.moveForward();
      break;
    case PathEast:
      combatData.moveRight();
      break;
    case PathSouth:
      combatData.moveBackward();
      break;
    case PathWest:
      combatData.moveLeft();
      break;
    case PathNW:
      combatData.moveNW();
      break;
    case PathNE:
      combatData.moveNE();
      break;
    case PathSW:
      combatData.moveSW();
      break;
    case PathSE:
      combatData.moveSE();
      break;
    default:
      break;
    }
    combatData.m_iMoveToX = -1;
    combatData.m_iMoveToY = -1;
    break;

  case KC_CENTER: // Numpad_5
    combatData.PlaceCursorOnCurrentDude(TRUE);
    combatData.m_iMoveToX = -1;
    combatData.m_iMoveToY = -1;
    break;

  case KC_LEFT:
    combatData.moveLeft();
    break;
  case KC_RIGHT:
    combatData.moveRight();
    break;
  case KC_UP:
    combatData.moveForward();
    break;
  case KC_DOWN:
    combatData.moveBackward();
    break;
  case KC_NW:
    combatData.moveNW();
    break;
  case KC_NE:
    combatData.moveNE();
    break;
  case KC_SE:
    combatData.moveSE();
    break;
  case KC_SW:
    combatData.moveSW();
    break;
  default:
    StandardMenuKeyboardAction(key,ascii);
    return;
  }

  if (combatData.CurrMoveWouldFleeMap())
  {
    SetMyState(TASK_AskBeforeFleeMap);
    PushEvent(new ASK_YES_NO_MENU_DATA("FLEE THE COMBAT MAP?",TRUE,CONTROL_CombatMove), DeleteEvent);
  }
  else
  {

    // Copied from 'OnIdle()'
    combatData.UpdateCombat();

    combatData.HandleCurrState(TRUE);

    overallCombatState OCS = combatData.GetCombatState();
  
    switch(OCS)
    {
    case OCS_CombatOver:
      PopEvent();
      break;
    case OCS_Moving: 
      if (combatData.OnAuto(false))
      {
        PopEvent();
        return;
      };
      //if (
      break;
    case OCS_Attacking:
      PopEvent();
      break;
    case OCS_NewCombatant:
      PopEvent();
      return;
    case OCS_Fled:
      PopEvent();
      return;
    case OCS_None:
      // 20120605 PRS  PopEvent added.  The moving combatant died as a 
      // result of movement.  The Death Sprite was displayed.  The 
      // Combatant no longer exists.  We're outta here!
      PopEvent();
      break;
    case OCS_Guarding:
      PopEvent();
      return;
    default: die(0xe0e045);
    };
  };
}

void COMBAT_MOVE_MENU_DATA::OnReturnToTopOfQueue(void)
{
  if (GetMyState() == TASK_AskBeforeFleeMap)
  {
    if (party.tradeItem==1) // yes, flee the map
    {
      SetMyState(TASK_initialized);
      OnIdle();
      PopEvent();
    }
    else // no, stay on the map (but still exit this move event)
    {
      combatData.m_iMoveToX = -1;
      combatData.m_iMoveToY = -1;    
      //SetMyState(TASK_initialized);
      combatData.StopMoving();
      PopEvent();
    }
  }
  else
    OnIdle();
}

void COMBAT_MOVE_MENU_DATA::OnInitialEvent(void)
{
  //int dude;
  ClearFormattedText(textData);
  combatData.PlaceCursorOnCurrentDude();
  menu.setMenu(ExitMenuData, NULL, TRUE, NULL, NULL);
  menu.setHorzOrient();
  menu.setTitle(FormatCombatMoveText());
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 1);
  SetMyState(TASK_initialized);
  //dude = combatData.GetCurrCombatant();
  //ASSERT (dude != NO_DUDE);
  //if (dude != NO_DUDE)
  //{
  //  combatData.GetCombatant(dude).state = ICS_None;
  //};
  Invalidate();
  switch (m_initialKey)
  {
  case KC_RIGHT:
  case KC_LEFT:
  case KC_UP:
  case KC_DOWN:
  case KC_NE:
  case KC_SE:
  case KC_SW:
  case KC_NW:
    OnKeypress(m_initialKey, 0);
    break;
  default: break;
  };
}

void COMBAT_MOVE_MENU_DATA::OnDraw(void)
{
  UpdateCombatMoveScreen();
}


bool COMBAT_MOVE_MENU_DATA::OnCycle(void)
{
  // This only serves to make the OnCyle more visible
  return GameEvent::OnCycle();  // Animate lingering effects
}

void COMBAT_MOVE_MENU_DATA::OnUpdateUI(void)
{
  /*
  COMBATANT *pCombatant;
  {
    CString result;
    const char *pParam;
    pCombatant = &combatData.getActiveCombatCharacter();
    PrepareHooks();
    hookParameters[1].Format("%d",pCombatant->movement);
    scriptContext.SetCombatantContext(pCombatant);
    scriptContext.SetCharacterContext(pCombatant);
    result = pCombatant->combatantSA.RunScripts(
               DRAW_MOVE_MENU,
               ScriptCallback_RunAllScripts,
               NULL,
               "COMBAT_MOVE_MENU_DATA",
               pCombatant->GetName());
    pParam = (LPCSTR)hookParameters[1];
    pCombatant->movement = ScriptAtoI(&pParam,0,99);
    scriptContext.Clear();
  };
  if (pCombatant->movement == 0)
  {
    PopEvent();
    return;
  };
  */
  menu.setTitle(FormatCombatMoveText());
  menu.setAllItemsActive();
}

void COMBAT_MOVE_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  MouseCombatMapMenu(mx,my);
}

bool COMBAT_MOVE_MENU_DATA::OnIdle(void)
{

  TASKSTATE taskState;
  taskState = GetMyState();

  switch(taskState)
  {
  case TASK_initialized:
    {
      overallCombatState OCS;
      OCS = combatData.GetCombatState();
      switch(OCS)
      {
      case OCS_Moving:
      case OCS_None:
        return true;
      default:
        die(0xcca0044);
      };
    };
  default:
    die(0xcdd351);
  };

  return true;

  //if (GetMyState() != TASK_AskBeforeFleeMap)
  //{
//    combatData.UpdateCombat();
//
    //combatData.HandleCurrState();
//
//
    //overallCombatState OCS = combatData.GetCombatState();
//
    //if (OCS == OCS_Moving)           return true;
    //if (OCS == OCS_None)             return true;
    ////if (OCS == OCS_NewCombatant)     return true;
//
    //PopEvent();
    //return false;
  //}
//
//  return false;  
}

////////////////////////////////////////////////////////////////////////COMBAT_SPRITE_ANIMATE_DATA
void COMBAT_SPRITE_ANIMATE_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.reset(NULL);
  if (!combatSprite.picData.IsPicLoaded())
  {
    PopEvent();
    return;
  }
  {
    COMBATANT *pCombatant;
    pCombatant = combatData.getCombatantPtr(combatData.GetCurrCombatant());
    pCombatant->PlayLaunch();
    //PlayLaunch();
  };
  EnableUserInput(FALSE);
}

void COMBAT_SPRITE_ANIMATE_DATA::OnDraw(void)
{
  UpdateSpriteAnimateScreen(NO_DUDE);// no combatant stats displayed
}

bool COMBAT_SPRITE_ANIMATE_DATA::OnCycle(void)
{
  //GameEvent::OnCycle();
  // Stay in this state until animation reaches target, or 
  // until last frame is displayed.
  // Moving sprites are moved along path by calls in DgnScrn.cpp
  bool result = false;

  if (UpdateAnimateList())
    result = true;

  if (combatSprite.IsSpriteDone())
  {
    combatSprite.Clear();
    EnableUserInput(TRUE);
    PopEvent();    
  }
  else
  {    
    if (combatSprite.Update()) // this function can change startTerrainX and Y
      result = true;
  }
  
  return result;
}

////////////////////////////////////////////////////////////////////////COMBAT_SPELL_SPRITE_ANIMATE_DATA
void COMBAT_SPELL_SPRITE_ANIMATE_DATA::OnInitialEvent(void)
{

  ClearFormattedText(textData);
  menu.reset(NULL);
  
  {  // I have no idea what this is for or if it is used  PRS 20101109
    if (!m_msg.IsEmpty())
      FormatCombatText(combatTextData, m_msg);
  };

  if (!m_msg.IsEmpty())
  {
    menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // starts empty
    menu.changeMenuItem(1, m_msg);
    menu.setHorzOrient();
    menu.setUseActive(FALSE); // no highlighting
    Invalidate();  
  };
    //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_TimeDelayMsg, m_delay);

  //COMBATANT &dude = combatData.getActiveCombatCharacter();
  COMBATANT *dude = combatData.getActiveCastingCharacter();
  if (dude==NULL)
  {
    PopEvent();
    return;
  }
  //GLOBAL_SPELL_ID key = dude->GetEffectiveSpellKeyBeingCast();
  SPELL_ID spellID = dude->GetEffectiveSpellIDBeingCast();
  // PRS 20111013  spellbook key is equal to global spell id
  //CHARACTER_SPELL cspell;
  SPELL_DATA *pSdata=NULL;
  //if (dude->GetSpell(key, cspell))
  //  pSdata = spellData.GetSpellData(cspell.spell);   
  //if (!key.IsValidSpell())
  if (!spellID.IsValidSpell())
  {
    //key = dude->GetItemSpellKeyBeingCast();
    //if (!key.IsValidSpell())
    spellID = dude->GetItemSpellIDBeingCast();
    if (!spellID.IsValidSpell())
    {
      die(0x41404);
    };
  };

  //pSdata = spellData.GetSpellData(key);
  pSdata = spellData.GetSpell(spellID);

  switch (m_type)
  {
  case CAST:
    if (combatData.NeedSpellCastAnimation(m_target))
    {
      combatData.InitSpellCastAnimation(m_target);
    };
    if (pSdata!=NULL) pSdata->PlayCast();
    break;

  case MISSILE:
    if (dude->m_pCharacter->targets.m_SelectingUnits)
    {
      COMBATANT *pTarget=GetCombatantPtr(m_target);
      if (pTarget!=NULL)
      {
        if (combatData.NeedSpellInRouteAnimation(pTarget->x,pTarget->y, FALSE))
          combatData.InitSpellInRouteAnimation(pTarget->x,pTarget->y);
      }
    }
    else
    {
      if (combatData.NeedSpellInRouteAnimation(m_mapx,m_mapy, FALSE))
        combatData.InitSpellInRouteAnimation(m_mapx,m_mapy);
    }
    if (pSdata!=NULL) pSdata->PlayMissile();
    break;

  case REDIRECTED_MISSILE:
    {
#ifdef newCombatant
      if (dude->m_pCharacter->targets.m_SelectingUnits)
#else
      if (dude->targets.m_SelectingUnits)
#endif
      {
        //COMBATANT *pTarget=GetCombatantPtr(m_target);
        COMBATANT *pDestination=GetCombatantPtr(m_dCombatant);
        COMBATANT *pSource     =GetCombatantPtr(m_sCombatant);
        if (pDestination!=NULL)
        {
          if (combatData.NeedSpellInRouteAnimation(pDestination->x,pDestination->y, TRUE))
            combatData.InitSpellInRouteAnimation(pSource->x,      pSource->y,
                                                 pDestination->x, pDestination->y);
        }
      }
      else
      {
        /* Really */ NotImplemented(0xc4429a,false);
        if (combatData.NeedSpellInRouteAnimation(m_mapx,m_mapy, FALSE))
          combatData.InitSpellInRouteAnimation(m_mapx,m_mapy);
      }
      if (pSdata!=NULL) pSdata->PlayMissile();
    };
    break;

  case COVERAGE:
    if (dude->m_pCharacter->targets.m_SelectingUnits)
    {
      COMBATANT *pTarget=GetCombatantPtr(m_target);
      if (pTarget!=NULL)
      {
        if (combatData.NeedSpellCoverageAnimation(pTarget->x,pTarget->y))
          combatData.InitSpellCoverageAnimation(pTarget->x,pTarget->y);
      }
    }
    else
    {
      if (combatData.NeedSpellCoverageAnimation(m_mapx,m_mapy))
        combatData.InitSpellCoverageAnimation(m_mapx,m_mapy);
    }
    if (pSdata!=NULL) pSdata->PlayCoverage();
    break;

  case HIT:
    if (combatData.NeedSpellHitAnimation(m_target))
      combatData.InitSpellHitAnimation(m_target);
    if (pSdata!=NULL) pSdata->PlayHit();
    break;

  case LINGER:    
    if (combatData.NeedSpellLingerAnimation())
    {
      if (combatData.IsAreaLingerSpell(dude))
      {
        dude->combatant_spellCastingLevel = dude->m_pCharacter->m_spellCastingLevel;
        dude->combatant_activeSpellKey = 
                combatData.InvokePendingSpellOnMap(dude); //->self, dude->combatant_activeSpellKey);      
        combatData.InitSpellLingerAnimation();    
        if (pSdata!=NULL) pSdata->PlayLinger();
      }
      else
      {
        // Lingering spell effects attached to a target.
        combatData.InitSpellLingerAnimation(m_target);    
      };
      
    }   
    else
    {
      if (combatData.IsAreaLingerSpell(dude))
      {
        dude->combatant_spellCastingLevel = dude->m_pCharacter->m_spellCastingLevel;
        dude->combatant_activeSpellKey = 
                combatData.InvokePendingSpellOnMap(dude); //->self, dude->combatant_activeSpellKey);      
      }
    };
    break;

  default:
    die(0xab4f2); // unhandled animation type
    PopEvent();
    return;
  }

  m_picLoaded = combatSprite.picData.IsPicLoaded()?true:false;
  if ((!m_picLoaded)&&(m_msg.IsEmpty()))
  {
    PopEvent();
    return;
  }

  m_totalTime=0;
  EnableUserInput(FALSE);
}

void COMBAT_SPELL_SPRITE_ANIMATE_DATA::OnDraw(void)
{  
  switch (m_type)
  {
  case CAST:
    UpdateSpriteAnimateScreen(m_pCaster);
    break;
  case MISSILE:
    UpdateSpriteAnimateScreen(NO_DUDE);
    break;
  case COVERAGE:
    UpdateSpriteAnimateScreen(NO_DUDE);
    break;
  case HIT:
    UpdateSpriteAnimateScreen(m_target);
    break;
  case LINGER:
    UpdateSpriteAnimateScreen(NO_DUDE);
    break;
  default:
    UpdateSpriteAnimateScreen(NO_DUDE);
    return;
  }
}

bool COMBAT_SPELL_SPRITE_ANIMATE_DATA::OnCycle(void)
{
  //GameEvent::OnCycle();
  // Stay in this state until animation reaches target, or 
  // until last frame is displayed.
  // Moving sprites are moved along path by calls in DgnScrn.cpp
  bool result = false;

  if (UpdateAnimateList())
    result = true;

  static LONGLONG lastTime = 0;
  LONGLONG currTime = virtualGameTime;
  if (currTime < lastTime) currTime = lastTime; // rollover
  m_totalTime += (currTime-lastTime);
  
  if (m_picLoaded)
  {
    if (combatSprite.IsSpriteDone())
    {
      if (!m_msg.IsEmpty())
      {
        // Make sure the msg is displayed for the minimum
        // time, in case the animation is very short.
        if (m_totalTime >= GetMsgTimeDelay())
        {
          PopEvent();
          return result;
        }
      }
      else // no message
      {
        PopEvent();
        return result;
      }
    }
    else
    {    
      if (combatSprite.Update()) // this function can change startTerrainX and Y
        result = true;
    }
  }
  else
  {
    // no animation, but there might still be
    // a message to display
    ASSERT(!m_msg.IsEmpty());

    // Make sure the msg is displayed for the minimum time
    if (m_totalTime >= GetMsgTimeDelay())
    {
      PopEvent();
      return result;
    }
  }
  lastTime=currTime;
  return result;
}

void COMBAT_SPELL_SPRITE_ANIMATE_DATA::OnLeaveQueue(void)
{
  combatSprite.Clear();
  EnableUserInput(TRUE);      
  ClearFormattedText(combatTextData);
}


void TIME_DELAY_MSG_DATA::OnKeypress(key_code key, char ascii)
{
  if (key == KC_SPACE)
  {
    if (     combatData.OnAuto(false)
         && (combatData.getActiveCombatCharacter().IsPartyMember())
       )
    {
      combatData.Quick(false);
      combatData.getActiveCombatCharacter().EndTurn();
    };
  };
}


////////////////////////////////////////////////////////////////////////TIME_DELAY_MSG_DATA
void TIME_DELAY_MSG_DATA::OnInitialEvent(void)
{
  int attackResult;
  int dude;
  m_iDeathIndex = -1;
  m_timerrcvd=false;
  ClearFormattedText(textData);
  menu.setMenu(EmptyMenuData, NULL, TRUE, this, "TimeDelay"); // starts empty
  if (!m_preRecordedMessage.IsEmpty())
  {
    menu.changeMenuItem(1, m_preRecordedMessage);
    menu.setHorzOrient();
    menu.setUseActive(FALSE); // no highlighting
    Invalidate();  
    //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_TimeDelayMsg, m_delay);
    SetEventTimer(TASKTIMER_TimeDelayMsg, m_delay);
    return;
  };
  dude = combatData.GetCurrCombatant(); // Get it now in case he kills himself!
  m_msg.Empty();
  {
    COMBATANT *pCombatant;
    CHARACTER_SPELL chSpell;
    pCombatant = combatData.GetCombatant(dude);
    pCombatant->FetchSpellBeingCast(&chSpell);
    if (pCombatant->Wpn_Type != SpellLikeAbility)
    {
      m_msg = combatData.FormatTimeDelayMsg((overallCombatState)m_state);
    };
  };


  switch (m_state)
  {
  case OCS_DisplayingAttacker:
    break;
  default:
    {
      m_iDeathIndex = -1;
      attackResult = combatData.HandleTimeDelayMsgBegin(&m_iDeathIndex); // make attack if in CS_Attacking state
      if (attackResult == 1)
      {
        m_msg = combatData.FormatTimeDelayMsg(OCS_Guarding);
      };
    };
  };

  CString specAbMsg("");
  DWORD MsgType=SPECAB_MSG_NONE;
  switch (m_state)
  {
  case OCS_Casting:       MsgType=SPECAB_MSG_CASTING; break;
  case OCS_ActivateSpell: MsgType=SPECAB_MSG_ACTIVATESPELL; break;
  case OCS_Fleeing:       MsgType=SPECAB_MSG_FLEE; break;
  case OCS_Turning:       MsgType=SPECAB_MSG_TURNUNDEAD; break;
  case OCS_Bandaging:     MsgType=SPECAB_MSG_BANDAGE; break;
  case OCS_Guarding:      MsgType=SPECAB_MSG_GUARD; break;
  case OCS_Attacking:     MsgType=SPECAB_MSG_ATTACK; break;
  case OCS_Moving:        MsgType=SPECAB_MSG_MOVE; break;
  case OCS_EndingTurn:    MsgType=SPECAB_MSG_ENDTURN; break;
  case OCS_Delaying:      MsgType=SPECAB_MSG_DELAY; break;
  }

  specAbMsg = combatData.FormatSpecAbMsg(MsgType, dude);

  if (m_msg.GetLength()==0) 
  {
    // use spec ab msg instead if no regular msg
    m_msg=specAbMsg;
    if (m_msg.GetLength()>0)
    {
      menu.changeMenuItem(1, m_msg);
      menu.setHorzOrient();
      menu.setUseActive(FALSE); // no highlighting
      m_msg="";
    }
  }
  else
  {
    // use regular msg, then spec ab msg
    menu.changeMenuItem(1, m_msg);
    menu.setHorzOrient();
    menu.setUseActive(FALSE); // no highlighting
    m_msg=specAbMsg;
  }
  Invalidate();  
  //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_TimeDelayMsg, m_delay);
  SetEventTimer(TASKTIMER_TimeDelayMsg, m_delay);
}

void TIME_DELAY_MSG_DATA::OnDraw(void)
{
  switch (m_state)
  {
  case OCS_Attacking:  UpdateCombatTimeDelayMsg(combatData.GetCurrTarget(false)); break; // stats for target
  default:             UpdateCombatTimeDelayMsg(); break; // stats for curr dude
  }
}

bool TIME_DELAY_MSG_DATA::OnTimer(UINT TimerId)
{
   if (TimerId == TASKTIMER_TimeDelayMsg)
   {
    if (!m_msg.IsEmpty())
    {
      m_msgswitch=true;
      menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL);
      menu.changeMenuItem(1, m_msg);
      menu.setHorzOrient();
      menu.setUseActive(FALSE);
      
      // get next spec ab msg, if any
      DWORD MsgType=SPECAB_MSG_NONE;
      switch (m_state)
      {
      case OCS_Casting:       MsgType=SPECAB_MSG_CASTING; break;
      case OCS_ActivateSpell: MsgType=SPECAB_MSG_ACTIVATESPELL; break;
      case OCS_Fleeing:       MsgType=SPECAB_MSG_FLEE; break;
      case OCS_Turning:       MsgType=SPECAB_MSG_TURNUNDEAD; break;
      case OCS_Bandaging:     MsgType=SPECAB_MSG_BANDAGE; break;
      case OCS_Guarding:      MsgType=SPECAB_MSG_GUARD; break;
      case OCS_Attacking:     MsgType=SPECAB_MSG_ATTACK; break;
      case OCS_Moving:        MsgType=SPECAB_MSG_MOVE; break;
      case OCS_EndingTurn:    MsgType=SPECAB_MSG_ENDTURN; break;
      case OCS_Delaying:      MsgType=SPECAB_MSG_DELAY; break;
      }
      m_msg = combatData.FormatSpecAbMsg(MsgType, combatData.GetCurrCombatant());

      //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_TimeDelayMsg, m_delay);   
      SetEventTimer(TASKTIMER_TimeDelayMsg, m_delay);
      return true;
    }
    else
    {   
      switch(m_state)
      {
      case OCS_DisplayingAttacker:
        PopEvent();
        break;
      default:
        {
          m_timerrcvd=true;

          if (!combatSprite.picData.IsPicLoaded())
          {
            combatSprite.Clear();
            combatData.CheckForDeadGuys();
            combatData.HandleTimeDelayMsgExpired(m_iDeathIndex); // end attack if in CS_Attacking state
            PopEvent();
          }
          else if (combatSprite.IsSpriteDone())
          {
            combatSprite.Clear();
            combatData.CheckForDeadGuys();
            combatData.HandleTimeDelayMsgExpired(m_iDeathIndex); // end attack if in CS_Attacking state
            PopEvent();
          }
        }
      };
    };
  };
  return m_timerrcvd;
}

bool TIME_DELAY_MSG_DATA::OnCycle(void)
{
  bool result = UpdateAnimateList();

  if (!combatSprite.picData.IsPicLoaded())
  {
    if (m_timerrcvd) 
    {
      combatSprite.Clear();
      combatData.CheckForDeadGuys();
      combatData.HandleTimeDelayMsgExpired(-1); // end attack if in CS_Attacking state
      PopEvent();
    }
    else
    {
      if (m_msgswitch==true)
      {
        m_msgswitch=false;
        result=true;
      }
    }
  }
  else if (!combatSprite.IsSpriteDone())
  {
    if (combatSprite.Update()) // this function can change startTerrainX and Y
      result = true;
    
    if (m_msgswitch==true)
    {
      m_msgswitch=false;
      result=true;
    }
  }
  else if (m_timerrcvd) 
  {
    combatSprite.Clear();
    combatData.CheckForDeadGuys();
    combatData.HandleTimeDelayMsgExpired(-1); // end attack if in CS_Attacking state
    PopEvent();
    result=true;
  }
  else
  {
    if (m_msgswitch==true)
    {
      m_msgswitch=false;
      result=true;
    }
  }
  return result;
}
////////////////////////////////////////////////////////////////////////COMBAT_ROUND_DELAY_DATA
void COMBAT_ROUND_DELAY_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(EmptyMenuData, NULL, TRUE, NULL, NULL); // erase menu
  menu.setHorzOrient();
  menu.setUseActive(FALSE); // no highlighting
  //((CDungeonApp*)AfxGetApp())->SetIntervalTimer(TASKTIMER_CombatRoundDelay, m_delay);
  SetEventTimer(TASKTIMER_CombatRoundDelay,m_delay);
}

void COMBAT_ROUND_DELAY_DATA::OnDraw(void)
{
  UpdateCombatRoundDelayScreen();
}

bool COMBAT_ROUND_DELAY_DATA::OnTimer(UINT TimerId)
{
  if (TimerId == TASKTIMER_CombatRoundDelay)
  {
    if (IsCombatActive())
      combatData.HandleCombatRoundMsgExpired();
    PopEvent();
    return true;
  }
  return false;
}



COMBAT_SPECIAL_ACTION_DATA::COMBAT_SPECIAL_ACTION_DATA(void)
{
  event=CONTROL_CombatSpecialAction;
  m_fakeCombatant = new COMBATANT;
}

COMBAT_SPECIAL_ACTION_DATA::~COMBAT_SPECIAL_ACTION_DATA(void)
{
  delete m_fakeCombatant;
}


////////////////////////////////////////////////////////////////////////COMBAT_SPECIAL_ACTION_DATA
void COMBAT_SPECIAL_ACTION_DATA::OnInitialEvent(void)
{
  //For each combatant except the current combatant
  // Call script "SpecialAction" in attacker Special Abilities.
  // Provide  attacker/target context.
  //If successful then we show the attack animation and deduct hitpoints.

  
  m_combatantIndex = -2;  
  PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
}

void COMBAT_SPECIAL_ACTION_DATA::OnReturnToTopOfQueue(void)
{
  int j;
  ActorType actor;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  COMBATANT *pTarget;
  COMBATANT *pAttacker;
  if (m_combatantIndex == combatData.NumCombatants())
  {
    PopEvent();
    return;
  };
  m_combatantIndex++;
  pAttacker = combatData.GetCombatant(combatData.GetCurrCombatant());
  if (m_combatantIndex < 0)
  {
    pTarget   = m_fakeCombatant;
    m_fakeCombatant->self = -2;
    actor.Clear();
    actor.Instance = (unsigned)0xfffffffe;
    //pTarget->GetContext(&actor);
    actor.SetFakeCombatantSrc();
  }
  else if (m_combatantIndex == combatData.NumCombatants())
  {
    pTarget   = m_fakeCombatant;
    m_fakeCombatant->self = -1;
    actor.Clear();
    //pTarget->GetContext(&actor);
    actor.SetFakeCombatantSrc();
    actor.Instance = (unsigned)0xffffffff;
  }
  else
  {
    pTarget = combatData.GetCombatant(m_combatantIndex);
    pTarget->GetContext(&actor);
  };
  scriptContext.SetAttackerContext(pAttacker);
  scriptContext.SetTargetContext(&actor);
  SetTargetContext (&actor);
  pAttacker->RunCombatantScripts(
                           COMBAT_SPECIAL_ACTION, 
                           ScriptCallback_RunAllScripts,
                           NULL,
                           "Return to Combat Special Action");
  ClearTargetContext();
  combatData.CheckForDeadGuys();
  for (j=0; j<combatData.NumCombatants(); j++)
  {
    COMBATANT *pCombatant;
    pCombatant = combatData.GetCombatant(j);
    if (pCombatant->GetAdjStatus() == Dead) continue;
    if (pCombatant->blitDeathTile)
    {
      pCombatant->SetStatus(Dead);
    };
  };
  if (hookParameters[5].IsEmpty())
  {
    PushEvent(new DO_NOTHING_EVENT, DeleteEvent);
  }
  else
  {
    PushEvent(new TIME_DELAY_MSG_DATA(GetMsgTimeDelay(), 
                                      (overallCombatState)combatData.GetCombatantState(),hookParameters[5]), 
                                      DeleteEvent);    
  };
}




////////////////////////////////////////////////////////////////////////GAME_SPEED_MENU_DATA
void GAME_SPEED_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  }
  party.speed = (BYTE)menu.currentItem();
  PopEvent();
}

void GAME_SPEED_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(SpeedMenuData, NULL, FALSE, this, "GameSpeed");
  menu.setHorzOrient();
  menu.setCurrentItem(party.speed);
  Invalidate();
}

void GAME_SPEED_MENU_DATA::OnDraw(void)
{
  if (m_pOrigEvent != NULL)
  {
    if (m_pOrigEvent->GetEventType() == Combat)
      UpdateSpeedCombatScreen();
    else
      UpdateSpeedScreen('Q');
  }
  else
    UpdateSpeedScreen('Q');
}

////////////////////////////////////////////////////////////////////////CHANGE_CLASS_MENU_DATA
void CHANGE_CLASS_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  /*
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  }

  CHARACTER& dude = GetActiveChar(this);
  switch (menu.currentItem()) 
  {
  case 1: // cleric
    dude.HumanChangeClass(Cleric);
    break;
  case 2: // druid
    dude.HumanChangeClass(Druid);
    break;
  case 3: // fighter
    dude.HumanChangeClass(Fighter);
    break;
  case 4: // ranger
    dude.HumanChangeClass(Ranger);
    break;
  case 5: // paladin
    dude.HumanChangeClass(Paladin);
    break;
  case 6: // Magic User
    dude.HumanChangeClass(MagicUser);
    break;
  case 7: // thief
    dude.HumanChangeClass(Thief);
    break;
  case 8: // exit;
    PopEvent(); 
    return;
  }
*/
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  switch (menu.currentItem()) 
  {
  case 1: // select
    {
      //classType ct = (classType)spellListText.GetSpellIndex(currSpellTextIndex());
      CLASS_ID classID = spellListText.GetClass(currSpellTextIndex());
      //character.SetClass(ct);
      //character.SetClass(classID);
      pChar->HumanChangeClass(classID);
      pChar->spellAbility.Clear();
      pChar->UpdateSpellAbility();
      
#define Spells20171107

#ifdef Spells20171107
      {
        BOOL knowSpellsAtCreation;
        knowSpellsAtCreation = TRUE;     
        if (knowSpellsAtCreation)
        {
          SetMyState(TASK_Create_Character_SpellPick);
          pChar->ClrKnowableSpell();
          ReplaceEvent(new INITIAL_MU_SPELLS_MENU_DATA(*pChar), DeleteEvent);
        };
      };
#else
      
      
      PopEvent();
#endif
    }
    break;    
  case 2: // next
    nextSpellPage(NULL);
    break;
  case 3: // prev
    prevSpellPage(NULL);
    break;
  case 4: // exit
//    m_pOrig->m_AbortCharCreation=true;
    PopEvent(); // pop to exit event
    break;
  default:
    WriteDebugString("Unhandled menu item in CLASS_MENU_DATA");
    break;
  }   
}
/*
void CHANGE_CLASS_MENU_DATA::ListAllClasses(void)
{
  pChar->CreateChangeClassList(&m_classList);
}
*/
/*
void CHANGE_CLASS_MENU_DATA::FillSpellListText(void)
{
  int cnt, i, n;
  spellListText.Clear();
  spellListText.arbitraryText[1] = "Choose a new class";
  spellListText.m_flags |= ARBITRARY_TEXT_FORMAT_FLAG;
  if (m_classList.GetSize() == 0)
  {
    return;
  };
  n = m_classList.GetSize();
  for (i=0; i<n; i++)
  {
    CLASS_ID *pClassID;
    pClassID = &m_classList[i];
    spellListText.Add(*pClassID);
  };

  cnt = spellListText.GetCount() - startItem;
  ItemsOnPage = min(Items_Per_Page, cnt);

  //if ( (ItemsOnPage > 0) && !newSpellLevel)
  if (ItemsOnPage > 0)
  {
    if (party.activeItem >= spellListText.GetCount() - startItem)
      party.activeItem = (BYTE)(spellListText.GetCount()-1-startItem);
    if (party.activeItem >= ItemsOnPage)
      party.activeItem = (BYTE)(ItemsOnPage-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    cnt = spellListText.GetCount() - startItem;
    ItemsOnPage = min(Items_Per_Page, cnt);
      //currShopItem = 0;
  };
}
*/



void CHANGE_CLASS_MENU_DATA::OnInitialEvent(void)
{
//  ClearFormattedText(textData);
//  menu.setMenu(ChangeClassMenuData);
//  menu.setVertOrient();
//  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 8);
//  Invalidate();

  pChar = &GetActiveChar(this);
/*
  ClearFormattedText(textData);
  ClearFormattedText(errorTextData);
  menu.setMenu(ChangeClassMenuData);
  menu.setHorzOrient();
  //menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  spellListText.Clear();
  ListAllClasses();  // --> m_classList

  
  //BOOL PopIt = FALSE;

  handleSpellFormInput(SPELLFORM_exit, spellListText);
  clearSpellItemState();
  FillSpellListText();
  //setSpellNewLevelItemState(m_dude);
  handleSpellFormInput(SPELLFORM_initial, spellListText, initialSpellForm);

  Invalidate();
*/

  ClearFormattedText(textData);
  menu.setMenu(SelectMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);

  CString title("Choose a character class:");
  CString tmp;
  tmp.Format(" ( %s, %s )", 
               //raceData.GetRaceName(character.race()),
               classData.PeekClass(pChar->GetClass())->m_name,
               GetGenderName(pChar->GetGender()));
  title+=tmp;
  //menu.setTitle(title);

  clearSpellItemState();
  spellListText.FormatSpellListTitle(title);

  spellListText.m_flags = SHOW_DESIGN_FLAG;    
   
  // Step through the loop to see possible classes.
  //ALLOWED_CLASSES allowedClasses;
  //allowedClasses.Initialize();  //Start with ALL classes
  //allowedClasses.RestrictByRace(character.race());
  //allowedClasses.RestrictByRace(character.RaceID());
  pChar->CreateChangeClassList(&m_classList);
  for (int i=0; i<m_classList.GetSize(); i++)
  {
    //GLOBAL_SPELL_ID gsID;
    //classType ct = allowedClasses.GetAt(i);
    //gsID.IllegalConversion((DWORD)ct);
    //spellListText.Add(gsID);
    //spellListText.Add((classType)i);
    spellListText.Add(m_classList[i]);
  };

  int scnt = spellListText.GetClassCount();
  //ItemsOnPage = min(Items_Per_Page, scnt);
  ItemsOnPage = min(Select_Class_Format.Lines_Per_Page, scnt);
  if (ItemsOnPage > 0)
  {
    if (party.activeItem >= spellListText.GetClassCount())
      party.activeItem = (BYTE)(spellListText.GetClassCount()-1);
    if (party.activeItem >= (startItem+ItemsOnPage))
      party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    //currShopItem = 0;
  }
  Invalidate();

}

void CHANGE_CLASS_MENU_DATA::OnDraw(void)
{
  /*
  UpdateHumanChangeClassScreen();
  */
  UpdateClassMenuScreen(Select_Class_Format);
}

void CHANGE_CLASS_MENU_DATA::OnUpdateUI(void)
{
  /*
  CHARACTER& dude = GetActiveChar(this);
  if (!dude.CanDualClassCleric())
    menu.setItemInactive(1);
  if (!dude.CanDualClassDruid())
    menu.setItemInactive(2);
  if (!dude.CanDualClassFighter())
    menu.setItemInactive(3);
  if (!dude.CanDualClassRanger())
    menu.setItemInactive(4);
  if (!dude.CanDualClassPaladin())
    menu.setItemInactive(5);
  if (!dude.CanDualClassMagicUser())
    menu.setItemInactive(6);
  if (!dude.CanDualClassThief())
    menu.setItemInactive(7);
  */
  if (CheckForMultiBoxText()) return;
  
  menu.setAllItemsActive();
  if (spellListText.GetClassCount() <= 0)
    menu.setItemInactive(1); // select    
  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // prev
}

////////////////////////////////////////////////////////////////////////TRADE_TAKER_SELECT_MENU_DATA
void TRADE_TAKER_SELECT_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    if ((key == KC_UP) || (key == KC_DOWN))
    {
      CHARACTER &tempCHAR = GetActiveChar(m_pOrigEvent,party.activeCharacter);
      // avoid direct reference to character small pic data
      currPic = tempCHAR.smallPic;
      //currPic.LoadPicSurfaces("prt_");
      currPic.LoadPicSurfaces("");
    }
    return;
  }

  switch (menu.currentItem()) 
  {
  case 1: // select char

    // trading to self allows re-arranging of inventory items
    //if (party.tradeGiver != party.activeCharacter)
    {
      if (!itemIsMoney(charItemListText.GetItemClass(party.tradeItem)))
      {
        if (!GetActiveChar(m_pOrigEvent,party.tradeGiver).myItems.IsReady(charItemListText.GetCharIndex(party.tradeItem)))
        {
          tradeItem(charItemListText.GetItemID(party.tradeItem), 
                    charItemListText.GetItemQty(party.tradeItem), 
                    charItemListText.GetItemCharges(party.tradeItem), 
                    charItemListText.GetItemIdentified(party.tradeItem), 
                    charItemListText.GetCharIndex(party.tradeItem), 
                    GetActiveChar(m_pOrigEvent, party.tradeGiver), 
                    GetActiveChar(m_pOrigEvent, party.activeCharacter));
        }
      }
      else
      {
        if (party.tradeGiver != party.activeCharacter)
        {
          if (party.tradeQty > 0)
          {
            tradeItemMoney(charItemListText.GetItemClass(party.tradeItem), 
                           party.tradeQty, 
                           GetActiveChar(m_pOrigEvent, party.tradeGiver),  // giver
                           GetActiveChar(m_pOrigEvent, party.activeCharacter) ); // taker
          }
        }
      }
    }
    break;
  case 2: // exit
    break;
  default:
    WriteDebugString("Unhandled menu state in handleTradeTaker()\n");
    miscError = InvalidMenuState;
    break;
  }

  PopEvent();
}

void TRADE_TAKER_SELECT_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  int itemTextIndex = currCharItemsTextIndex();
  party.tradeItem = (BYTE)itemTextIndex;
  party.tradeGiver = party.activeCharacter;

  CHARACTER &tempCHAR = GetActiveChar(m_pOrigEvent,party.activeCharacter);
  currPic = tempCHAR.smallPic;
  //currPic.LoadPicSurfaces("prt_");
  currPic.LoadPicSurfaces("");

  if (itemIsMoney(charItemListText.GetItemClass(itemTextIndex)))
  {
    menu.reset(NULL);
    party.tradeQty = 0;
    PushEvent(new GET_MONEY_QTY_DATA(GetEventType(),m_pOrigEvent),DeleteEvent);
    return;
  }

  party.tradeQty = 1;
  menu.setMenu(TradeTakerMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 2);
  Invalidate();
}

void TRADE_TAKER_SELECT_MENU_DATA::OnDraw(void)
{
  if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType()==Combat))
    UpdateCombatTradeTakerSelectScreen();
  else
    UpdateTradeTakerSelectScreen('R');
}

void TRADE_TAKER_SELECT_MENU_DATA::OnUpdateUI(void)
{
  // 1 { "SELECT", 0 },
  // 2 { "EXIT", 1 }

  if (CheckForMultiBoxText()) return;

  if (IsCombatActive())
  {
    // during combat, disallow trading to character that is
    // not directly adjacent to trade giver.
    int dist = combatData.GetDistanceBetween_ByPartyIndex(party.tradeGiver, party.activeCharacter);

    if (dist > 1)
      menu.setItemInactive(1); // select      
  }

  //if (party.tradeGiver == party.activeCharacter)
  //  menu.setItemInactive(1); // select
}

void TRADE_TAKER_SELECT_MENU_DATA::OnReturnToTopOfQueue(void)
{
  menu.setMenu(TradeTakerMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  Invalidate();
}

void TRADE_TAKER_SELECT_MENU_DATA::OnLeaveQueue(void)
{
  party.activeCharacter = party.tradeGiver;
  party.tradeGiver = 0;
  party.tradeItem = 0;
  party.tradeQty = 0;  
}

////////////////////////////////////////////////////////////////////////ALTER_GAME_MENU_DATA
void ALTER_GAME_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  }

  character = GetActiveChar(this);    

  switch (menu.currentItem()) 
  {
  case 1:  // Order
    PushEvent(new ALTER_ORDER_MENU_DATA, DeleteEvent);
    break;
  case 2: // Drop character
    {
      CString msg;
      msg.Format("REMOVE %s FROM PARTY?", character.GetName());
      SetMyState(TASK_DropCharFromParty);
      PushEvent(new ASK_YES_NO_MENU_DATA(msg,TRUE,CONTROL_AlterGame), DeleteEvent);
    }
    //party.removeCharacter(party.activeCharacter);
    break;
  case 3: // Speed
    PushEvent(new GAME_SPEED_MENU_DATA(this), DeleteEvent);
    break;
  case 4: // Icon  
    SetMyState(TASK_AlterCharacter);
    PushEvent(new GETCHARICON_MENU_DATA, DeleteEvent);
    break;
  case 5: // Small Pic    
    SetMyState(TASK_AlterCharacter);
    PushEvent(new GETCHARSMALLPIC_MENU_DATA, DeleteEvent);
    break;
  case 6: // Level    
    PushEvent(new ALTER_LEVEL_MENU_DATA(), DeleteEvent);
    break;
  case 7: // Sound    
    PushEvent(new ALTER_SOUND_MENU_DATA(), DeleteEvent);
    break;
  case 8: // Music
    SetMyState(TASK_AlterMusicPlayback);
    //PushEvent(new ASK_YES_NO_MENU_DATA("PLAY BACKGROUND MUSIC?",TRUE,CONTROL_Adventure), DeleteEvent);
    //Changed 22 May 2009 PRS to make ENCAMP/ALTER/MUSIC work
    PushEvent(new ASK_YES_NO_MENU_DATA("PLAY BACKGROUND MUSIC?",TRUE,CONTROL_Encamp), DeleteEvent);
    break;
  case 9: // Exit
    PopEvent(); 
    return;
  }
}

void ALTER_GAME_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(AlterMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 9);
  party.tradeItem=0;
  SetMyState(TASK_initialized);
  Invalidate();
}

void ALTER_GAME_MENU_DATA::OnDraw(void)
{
  UpdateAlterGameScreen('S');
}

void ALTER_GAME_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  if (party.numCharacters <= 1)
  {
    menu.setItemInactive(1); // order
    menu.setItemInactive(2); // drop
  }
}

void ALTER_GAME_MENU_DATA::OnReturnToTopOfQueue(void)
{
  if (GetMyState()==TASK_AlterMusicPlayback)
  {
    if (party.tradeItem==1) // yes, play background music
    {
      if (pSndMgr!=NULL) pSndMgr->SetMusicEnable(TRUE);
      StartBackgroundMusic();      
    }
    else
    {
      StopBackgroundMusic();
      if (pSndMgr!=NULL) pSndMgr->SetMusicEnable(FALSE);
    }
  }
  else if (GetMyState()==TASK_AlterCharacter)
  {
    // to make sure this works, init 'character' with
    // active party character before pushing an event.
    GetActiveChar(this) = character;
  }
  else if (GetMyState()==TASK_DropCharFromParty)
  {
    if (party.tradeItem==1) // yes, drop character
      party.removeCharacter(party.activeCharacter);
  }

  OnInitialEvent();
}

////////////////////////////////////////////////////////////////////////ALTER_LEVEL_MENU_DATA
void ALTER_LEVEL_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  }
  party.skillLevel = menu.currentItem()-1; 
  PopEvent();
}

void ALTER_LEVEL_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  //menu.setMenu(LevelMenuData);
  menu.reset(NULL);
  menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
  
  for (int i=0; i<DIFFICULTY_LEVEL_DATA::NUM_DIFF_LVLS; i++)
  {
    if (globalData.m_diffLvlData.m_data[i].m_Name.GetLength() > 0)
    {
      menu.addMenuItem(globalData.m_diffLvlData.m_data[i].m_Name);
    }
    else
    {
      int newItem = menu.addMenuItem(" ");
      menu.setItemInactive(newItem);
    }
  }

  menu.setHorzOrient();
  menu.SetFirstLettersShortcuts();
  menu.setCurrentItem(party.skillLevel+1);
  Invalidate();
}

void ALTER_LEVEL_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  menu.setAllItemsActive();

  for (int i=0; i<DIFFICULTY_LEVEL_DATA::NUM_DIFF_LVLS; i++)
  {
    if (globalData.m_diffLvlData.m_data[i].m_Name.GetLength() == 0)
      menu.setItemInactive(i);
  }   
}

void ALTER_LEVEL_MENU_DATA::OnDraw(void)
{
  UpdateAlterGameScreen('S');
}

////////////////////////////////////////////////////////////////////////ALTER_SOUND_MENU_DATA
void ALTER_SOUND_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key == KC_RETURN)
  {
    int slevel = menu.currentItem()-1;
    slevel *= 10;
    if (pSndMgr!=NULL) pSndMgr->SetMasterVolume(slevel);
    PopEvent();
  }

  switch (key)
  {
  case KC_NUM:   
    {
      if ((ascii >= '0')&&(ascii <= '9'))
      {
        int slevel = ascii-'0';
        slevel *= 10;
        if (pSndMgr!=NULL) pSndMgr->SetMasterVolume(slevel);
        slevel = (ascii-'0')+1;
        menu.setCurrentItem(slevel);
      }
    }
    break;
  default: 
    {
      HMenuVPartyKeyboardAction(key,ascii);
      //StandardMenuKeyboardAction(key, ascii); 
      int slevel = menu.currentItem()-1;
      slevel *= 10;
      if (pSndMgr!=NULL) pSndMgr->SetMasterVolume(slevel);
    }
    break;
  }

  PlayPartyBump();
}

void ALTER_SOUND_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(SoundMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  int slevel = 0;
  if (pSndMgr!=NULL) slevel = pSndMgr->GetMasterVolume();
  if (slevel > 0) slevel = slevel / 10;
  if (slevel < 0) slevel = 0;
  if (slevel > 10) slevel = 10;
  menu.setCurrentItem(slevel+1);
  Invalidate();
}

void ALTER_SOUND_MENU_DATA::OnDraw(void)
{
  UpdateAlterGameScreen('S');
}

////////////////////////////////////////////////////////////////////////ALTER_ORDER_MENU_DATA

void ALTER_ORDER_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key == KC_RETURN)
  {
    // prevents problems when alter menu gets to top of queue
    character = GetActiveChar(this);    
    PopEvent();
    return;
  }

  switch (key)
  {
  case KC_UP:   party.DecCharacterOrder(); break;
  case KC_DOWN: party.IncCharacterOrder(); break;
  default: StandardMenuKeyboardAction(key, ascii);
  }  
}

void ALTER_ORDER_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(ExitMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  Invalidate();
}

void ALTER_ORDER_MENU_DATA::OnDraw(void)
{
  UpdateAlterOrderScreen('T');
}

////////////////////////////////////////////////////////////////////////PARTY_DIED_MENU_DATA
void PARTY_DIED_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    //StandardMenuKeyboardAction(key, ascii);
    return;
  }

  ResetAdventureData(); // flags, quests, spells, etc
  PlayDeath(FALSE);
  currPic.Clear();
  ClearFormattedText(textData);
  ClearFormattedText(errorTextData);
  ProcInput.InitializeToMain();  
  //Invalidate();
}

void PARTY_DIED_MENU_DATA::OnInitialEvent(void)
{
  menu.setMenu(PartyDiedMenuData, NULL, FALSE, this, "PartyDied");
  menu.setHorzOrient();  
  HOOK_PARAMETERS hookParameters;
  if (party.numCharacters == 1)
    FormatDisplayText(textData, getGlobalEventMessage("YOU HAVE DIED!", "EventPartyDied", hookParameters, "", control));
  else
    FormatDisplayText(textData, getGlobalEventMessage("THE PARTY HAS DIED!", "EventPartyDied", hookParameters, "", control));
  PlayDeath(TRUE);
  Invalidate();
}

void PARTY_DIED_MENU_DATA::OnDraw(void)
{
  UpdatePartyDiedScreen();
}

////////////////////////////////////////////////////////////////////////REST_MENU_DATA
void REST_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  //if (GetMyState() == TASK_RestMenuInput)
  {
    int rslt;

    if (TABParty(key)) return;

    if (key != KC_RETURN) 
    {
      if (GetMyState() == TASK_RestMenuInput)
      {
        rslt=handleRestTimeInput(RESTTIME_key, this, key);
        if (rslt&2) return;
      }
      
      StandardMenuKeyboardAction(key,ascii);

      if (GetMyState() == TASK_RestMenuInput)
      {
        // make sure menu left/right navigation keeps
        // in synch with form fields
        switch (menu.currentItem()) 
        {
        case 2: // days
          handleRestTimeInput(RESTTIME_activate, this, RTF_DAYS_FIELD);
          break;
        case 3: // hours
          handleRestTimeInput(RESTTIME_activate, this, RTF_HOURS_FIELD);
          break;
        case 4: // mins
          handleRestTimeInput(RESTTIME_activate, this, RTF_MINUTES_FIELD);
          break;    
        }
      }
      return;
    }

    /*
     1 { "REST", 0 },
     2 { "DAYS", 0 },
     3 { "HOURS", 0 },
     4 { "MINS", 0 },
     5 { "ADD", 0 },
     6 { "SUB", 0 },
     7 { "EXIT", 1 }
    */
    switch (menu.currentItem()) 
    {
    case 1: // rest
      SetMyState(TASK_RestMenuEngage);
      m_showMenu = FALSE;
      party.resting=TRUE;
      break;
    case 2: // days
      handleRestTimeInput(RESTTIME_activate, this, RTF_DAYS_FIELD);
      break;
    case 3: // hours
      handleRestTimeInput(RESTTIME_activate, this, RTF_HOURS_FIELD);
      break;
    case 4: // mins
      handleRestTimeInput(RESTTIME_activate, this, RTF_MINUTES_FIELD);
      break;    
    case 5: // add
      handleRestTimeInput(RESTTIME_key, this, KC_PLUS);
      break;    
    case 6: // sub    
      handleRestTimeInput(RESTTIME_key, this, KC_MINUS);
      break;    
    case 7: // exit
      if (GetMyState() == TASK_RestMenuInput)
      {
        handleRestTimeInput(RESTTIME_exit, this); // Terminate modifications
        PopEvent();
      }
      else
      {
        handleRestTimeInput(RESTTIME_exit, this);
        SetMyState(TASK_RestMenuInput);
        m_showMenu = TRUE;
        party.resting=FALSE;
        handleRestTimeInput(RESTTIME_initial, this);
      }
      break;    
    default:
      handleRestTimeInput(RESTTIME_exit,this); // Terminate modifications
      WriteDebugString("Unhandled menu item in REST_MENU_DATA");
      miscError = InvalidMenuState;
      currPic.Clear();
      PopEvent();
      break;
    } 
  }
}

void REST_MENU_DATA::OnMouseClickLeft(int mx, int my)
{
  if (GetMyState() == TASK_RestMenuInput)
  {
    int rslt=handleRestTimeInput(RESTTIME_mouseClick,
                             this,
                             mx, my);
    if (!(rslt&1))
    {
      GameEvent::OnMouseClickLeft(mx, my);
    }
    else
    {

    }
  }
  else
  {
    GameEvent::OnMouseClickLeft(mx, my);
  }
}

void REST_MENU_DATA::OnInitialEvent(void)
{
  SetMyState(TASK_RestMenuInput);
  party.resting=FALSE;
  ClearFormattedText(textData);
  menu.setMenu(RestMenuData, NULL, FALSE, this, "Rest");
  menu.setHorzOrient();
  m_showMenu = TRUE;

  DWORD MinuteInc = party.CalcRestTime();
  m_days = MinuteInc / 1440;
  MinuteInc %= 1440;
  m_hours  = MinuteInc / 60;
  MinuteInc %= 60;
  m_mins  = MinuteInc;
  
  // if using Camp event
  if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType() == Camp))
  {
    if (m_pOrigEvent->GetEventPic().picType != BogusDib)
    {
      currPic = m_pOrigEvent->GetEventPic();
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
    }
    else
    {
      int zone = levelData.GetCurrZone(party.Posx, party.Posy);
      currPic = levelData.zoneData.zones[zone].campArt;
      //currPic.LoadPicSurfaces("sp_");
      currPic.LoadPicSurfaces("");
    };
  }
  else
  {    
    // no event, just 'Encamp' menu option
    int zone = levelData.GetCurrZone(party.Posx, party.Posy);
    currPic = levelData.zoneData.zones[zone].campArt;
    //currPic.LoadPicSurfaces("sp_");
    currPic.LoadPicSurfaces("");
  }

  handleRestTimeInput(RESTTIME_initial, this);
  party.BeginResting();
}

bool REST_MENU_DATA::OnCycle(void)
{
  if (GetMyState() == TASK_RestMenuInput) 
  {
    LONGLONG currTime = virtualGameTime;//timeGetTime();
    bool result = false;
    // update drunk points, spell effects, etc
    if (IsCombatActive())
      result = combatData.ProcessTimeSensitiveData(currTime);
    else
      result = party.ProcessTimeSensitiveData(currTime);
    return result;
    //return GameEvent::OnCycle();
  }
  else
  {
    __int64 dleft = m_days;
    __int64 hleft = m_hours;
    __int64 mleft = m_mins;

    __int64 minuteDelta = GetMinuteDelta(dleft, hleft, mleft);
    __int64 minuteTotal = (dleft*1440) + (hleft*60) + mleft;

    // update rest event minute tracker for this zone
    __int64 delta=0;
    GameEvent *tempEvent=NULL;
    while (delta < minuteDelta)
    {
      //levelData.IncTimeEventTime();
      levelData.IncRestEventTime(levelData.GetCurrZone(party.Posx, party.Posy), 1);

      party.incrementClock(1);

/*
      if (levelData.haveTimeEvent(
        party.Posx,
        party.Posy,
        party.days,
        party.hours,
        party.minutes,
        &tempEvent))
      {
        break;
      }
*/
      if (m_pOrigEvent == NULL)
      {
        if (levelData.haveRestEvent(
                 party.Posx,
                 party.Posy,
                 &tempEvent))
        {
          break;
        };
      };

      delta++;
    }    

    minuteTotal -= delta;

    if (minuteTotal > 0)
    {
      dleft = minuteTotal / 1440;
      minuteTotal %= 1440;
      hleft = minuteTotal / 60;
      minuteTotal %= 60;
      mleft = minuteTotal;
    }
    else
    {
      dleft = 0;
      hleft = 0;
      mleft = 0;
    }

    dleft = max(0, dleft);
    hleft = max(0, min(23,hleft));
    mleft = max(-1, min(59,mleft));

    m_days = dleft;
    m_hours = hleft;
    m_mins = mleft;
   
    //GameEvent::OnCycle();
    {
      LONGLONG currTime = virtualGameTime;//timeGetTime();
      bool result = false;
      // update drunk points, spell effects, etc
      if (IsCombatActive())
        result = combatData.ProcessTimeSensitiveData(currTime);
      else
        result = party.ProcessTimeSensitiveData(currTime);
    };
    //GameEvent *tempEvent=NULL;
    //if (levelData.haveRestEvent(
    //         party.Posx,
    //         party.Posy,
    //         &tempEvent))
    if (tempEvent!=NULL)
    {
      handleRestTimeInput(RESTTIME_exit, this);

      FormatPausedText(pausedTextData, "YOUR REST IS INTERRUPTED!");
      DisplayPausedText(pausedTextData, whiteColor, 0);
      ReplaceEvent(tempEvent, LevelEvent);
#ifndef OLD_AUTO_HEAL
      party.resting = FALSE;
#endif
      return false;
    }

    if ((dleft > 0) || (hleft > 0) || (mleft > 0))
    {
      handleRestTimeInput(RESTTIME_refresh, this);      
    }
    else
    {
      SetMyState(TASK_RestMenuInput);
#ifdef OLD_AUTO_HEAL
      party.resting=FALSE;
#endif
      menu.setCurrentItem(1);
    }

    return true;
  }
}

void REST_MENU_DATA::OnUpdateUI(void)
{
  if (CheckForMultiBoxText()) return;
  menu.setAllItemsActive();
  //if (GetMyState() != TASK_RestMenuInput)
  if (!m_showMenu)
  {
    menu.setItemInactive(1); // rest
    menu.setItemInactive(2); // days
    menu.setItemInactive(3); // hours
    menu.setItemInactive(4); // minutes
    menu.setItemInactive(5); // add
    menu.setItemInactive(6); // sub
  }
}

void REST_MENU_DATA::OnDraw(void)
{
  handleRestTimeInput(RESTTIME_refresh, this);
}

////////////////////////////////////////////////////////////////////////NEW_MU_SPELL_LEVEL_MENU_DATA
void NEW_MU_SPELL_LEVEL_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "SELECT", 0 },
   { "UNSELECT", 0 },
   { "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  int currIndex = currSpellTextIndex();
  switch (menu.currentItem()) 
  {
  case 1: // select
    spellListText.ToggleSelected(currIndex);
    break;
  case 2: // unselect
    spellListText.ToggleSelected(currIndex);
    break;
  case 3: // remove
    // unused for this event type
    break;
  case 4: // next
    nextSpellPage(NULL);
    break;    
  case 5: // prev
    prevSpellPage(NULL);
    break;    
  case 6: // exit
    {
      BOOL success = FALSE;
      for (int i=0; i<spellListText.GetCount(); i++)
      {
        if (spellListText.IsSelected(i))
        {
          //GLOBAL_SPELL_ID SpellDbKey;
          SPELL_ID spellID;
          //SpellDbKey.IllegalConversion(spellListText.GetSpellIndex(i));
          spellID = spellListText.GetSpellID(i);
/*
          if (!m_dude.KnowSpell( MagicUserFlag,
                                 //spellData.GetSpellSchool(SpellDbKey), 
                                 //SpellDbKey, 
                                 //spellData.GetSpellLevel(SpellDbKey),
                                 spellData.GetSpellSchool(spellID), 
                                 spellID, 
                                 //spellData.GetSpellLevel(spellName),
                                 spellData.GetSpellLevel(spellID),
                                 TRUE ))
*/
          /* Really */ NotImplemented(0x1d98fa, false);
          if (1)
          {
            WriteDebugString("failed to knowSpell that was selected in NEW_MU_SPELL_LEVEL_MENU_DATA\n");
          }
          else
            success = TRUE;
        }
      }

      if (success)
      {
        handleSpellFormInput(SPELLFORM_exit, spellListText);
        PopEvent();
      }
      else
      {
        PushEvent(new ASK_YES_NO_MENU_DATA("YOU DID NOT SELECT ANY SPELLS, EXIT ANYWAY?",TRUE, CONTROL_CreateCharacter), DeleteEvent);
      }
    }    
    break;    
  default:
    WriteDebugString("Unhandled menu item in NEW_MU_SPELL_LEVEL_MENU_DATA");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  } 
}

void NEW_MU_SPELL_LEVEL_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(MemorizeMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
  spellListText.Clear();
  
  BOOL PopIt = FALSE;

  setSpellNewLevelItemState(m_dude);
  handleSpellFormInput(SPELLFORM_exit, spellListText);
  handleSpellFormInput(SPELLFORM_initial, spellListText);

  // character has no capacity for additional spells
  if (!spellListText.CanSelectAnything())
    PopIt = TRUE;

  if (PopIt)
  {
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    return;
  }
  Invalidate();
}

void NEW_MU_SPELL_LEVEL_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  // draw middle horz bar, but 
  // not viewport and middle ver bar
  drawViewport(FALSE, FALSE, TRUE, TRUE);
  //if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void NEW_MU_SPELL_LEVEL_MENU_DATA::OnUpdateUI(void)
{
  /*
   { "SELECT", 0 },
   { "UNSELECT", 0 },
   { "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  menu.setAllItemsActive();
  menu.setItemInactive(3); // remove

  if (spellListText.GetCount() <= 0)
  {
    menu.setItemInactive(1); // select
    menu.setItemInactive(2); // unselect
    menu.setItemInactive(4); // next
    menu.setItemInactive(5); // prev
    return;
  }
 
  int currIndex = currSpellTextIndex();

  if (spellListText.IsSelected(currIndex))
    menu.setItemInactive(1); // select
  else
    menu.setItemInactive(2); // unselect

  if (!spellListText.CanSelect(currIndex))
    menu.setItemInactive(1); // select

  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(4); // next
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(5); // prev
}

void NEW_MU_SPELL_LEVEL_MENU_DATA::OnReturnToTopOfQueue()
{
  if (party.tradeItem==1) // yes, exit anyway
  {
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
  }
  else
  {
    OnInitialEvent();
  }
}

////////////////////////////////////////////////////////////////////////INITIAL_MU_SPELLS_MENU_DATA
int INITIAL_MU_SPELLS_MENU_DATA::AreWeDone(void)
{
  // In pass 0:
    // We are done if every level has acquired its MAX (or has no spells available).
    // We are done if the total of all levels has reached its MAX.
  // In pass 1 -> n:
    // We continue if a level<MIN and has spells available.
    // We continue if total<MIN and a level (with spells available) < MAX
  int i, totalAcquired, result, remaining;
  bool oneLevelNotMin;   // A level (with spells available) < MIN
  bool oneLevelNotMax;   // A level (with spells available) < MAX
  bool allLevelsDone;
  bool thisLevelNotMax;
  bool thisLevelNotMin;
  AcquireState *pA;
  totalAcquired = 0;
  allLevelsDone = true;
  oneLevelNotMax = false;
  oneLevelNotMin = false;
  thisLevelNotMax = false;
  result = FinishedThisLevel;
  remaining = 0;
  for (i=1; i<m_acquireStates.GetSize(); i++)
  {
    // For each level
    pA = &m_acquireStates[i];
    totalAcquired += pA->numAcquired;
    if (pA->numAvail > 0)
    {
      if (pA->numAcquired < pA->mnmc.max) 
      {
        oneLevelNotMax = true;
        if (i == m_currentSpellLevel)
        {
          thisLevelNotMax = true;
        };
      }
      if (pA->numAcquired < pA->mnmc.min)
      {
        oneLevelNotMin = true;      
        if (i == m_currentSpellLevel)
        {
          thisLevelNotMin = true;
        };
      };
    };
  };
  if (m_passNumber == 0)
  {
    if (oneLevelNotMax)
    {
      // One level < MAX     
      if (totalAcquired >= m_acquireStates[0].mnmc.max)
      {
        result |= FinishedAllLevels;  // We reached a global maximum
      }
      else
      {
        if (thisLevelNotMax)
        {
          result &= ~FinishedThisLevel;
          pA = &m_acquireStates[m_currentSpellLevel];
          remaining = pA->mnmc.max - pA->numAcquired;
          if (remaining > spellListText.GetCount())
          {
            remaining = spellListText.GetCount();
          };
        };
      };
    }
    else
    { 
      result |= FinishedAllLevels;  // Pass 0 and all levels at MAX
    };
  }
  else
  { // pass 1 -> n
    if (oneLevelNotMin)
    {  // One level < min
      if (totalAcquired >= m_acquireStates[0].mnmc.max)
      {
        result |= FinishedAllLevels;
      }
    }
    else
    {  // All levels >= min
      if (totalAcquired < m_acquireStates[0].mnmc.min)
      {
        if (!oneLevelNotMax)
        {
          result |= FinishedAllLevels;
        }
        else
        {
          if (thisLevelNotMax)
          {
            result &= ~FinishedThisLevel;
            pA = &m_acquireStates[m_currentSpellLevel];
            remaining = pA->mnmc.min - pA->numAcquired;
            if (remaining > spellListText.GetCount())
            {
              remaining = spellListText.GetCount();
            };
          };
        };
      }
      else
      {
        result |= FinishedAllLevels;
      };
    };
  };
  if (result & FinishedAllLevels)
  {
    result |= FinishedThisLevel;
    return result;
  };
  if (remaining > 0)
  {
    char plural[2];
    if (remaining > 1)
    {
      plural[0] = 's';
      plural[1] = 0;
    }
    else
    {
      plural[0] = 0;
    };
    spellListText.arbitraryText[0].Format("You can select %d level %d spell%s",
                                          remaining, 
                                          m_currentSpellLevel, 
                                          plural);
  };
  return result;
}

void INITIAL_MU_SPELLS_MENU_DATA::AutoScribe(void)
{
  int i, n;
  n = m_spellAvailabilityList.GetSize();
  for (i=0; i<n; i++)
  {
    const SPELL_DATA *pSpell;
    pSpell = m_spellAvailabilityList[i].pSpellData;
    //if (!pSpell->AutoScribeAtLevelChange) continue;
    if (!pSpell->AutoScribe) continue;
    if (!m_dude.KnowSpellyyy(pSpell, true))        
    {
      CString msg;
      msg.Format("% was unable to learn spell %s",m_dude.GetName(), pSpell->Name);
      MsgBoxInfo(msg);
    };
  };
}

void INITIAL_MU_SPELLS_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    spellListText.arbitraryText[1] = "";
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "SELECT", 0 },
   //{ "UNSELECT", 0 },
   //{ "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  int currIndex = currSpellTextIndex();
  switch (menu.currentItem()) 
  {
  case 1: // "SELECT"
    //spellListText.ToggleSelected(currIndex);
    {
      //GLOBAL_SPELL_ID SpellDbKey;
      SPELL_ID spellID;
      SPELL_DATA *pSpell;
      AcquireState *pAcquire;
      int availSpellIndex;
      int i, n, finishedFlags;
      bool success;
      //SpellDbKey.IllegalConversion(spellListText.GetSpellIndex(currIndex));
      spellID = spellListText.GetSpellID(currIndex);

      //pSpell = spellData.GetSpellData(SpellDbKey);
      pSpell = spellData.GetSpell(spellID);

      
      n = m_spellAvailabilityList.GetSize();
      availSpellIndex = -1;
      for (i=0; i<n; i++)
      {
        if (pSpell == m_spellAvailabilityList[i].pSpellData)
        {
          availSpellIndex = i;
          break;
        };
      };
      if (availSpellIndex < 0)
      {
        die (0x9621c);
      };

      pAcquire = &m_acquireStates[m_currentSpellLevel];
      if (pAcquire->numAcquired < pAcquire->mnmc.certain)
      {
        success = true;
      }
      else
      {
        success = RollDice(100,1) <= m_spellAvailabilityList[availSpellIndex].probability;    
      };
      m_spellAvailabilityList[availSpellIndex].learned = success;
      m_spellAvailabilityList[availSpellIndex].referenced = true;
      if (success)
      {
        m_dude.AddKnowableSpell(pSpell->uniqueName);
        m_successMsg.Format("%s successfully acquired %s.",
                             m_dude.GetName(),
                             pSpell->Name);
      }
      else
      {
        m_successMsg.Format("%s failed to acquire %s.",
                            m_dude.GetName(),
                            pSpell->Name);
      };
      FillSpellListText(false);  // sets m_listNumTotal and m_listNumLearned
      finishedFlags = AreWeDone();
      while ( (finishedFlags & FinishedAllLevels) == 0)
      {
        if (finishedFlags & FinishedThisLevel) 
        {  // Go to next level
          m_currentSpellLevel++;
          if (m_currentSpellLevel >= m_spellAvailabilityList.GetSize())
          {
            int j, num;
            m_currentSpellLevel = 1;
            num = m_spellAvailabilityList.GetSize();
            for (j=0; j<num; j++)
            {
              m_spellAvailabilityList[j].referenced = false;
            };
          };
          FillSpellListText(true);  // sets m_listNumTotal and m_listNumLearned
          finishedFlags = AreWeDone();
          continue;
        };
        return;
      };  
      handleSpellFormInput(SPELLFORM_exit, spellListText);
      AutoScribe();
      PopEvent();
    };
    break;
  //case 2: // unselect
  //  spellListText.ToggleSelected(currIndex);
  //  break;
  //case 3: // remove
  //  // unused for this event type
  //  break;
  case 2: // next
    nextSpellPage(NULL);
    break;    
  case 3: // prev
    prevSpellPage(NULL);
    break;    
  //case 4: // exit
  //  {
  //    BOOL success = FALSE;
  //    for (int i=0; i<spellListText.GetCount(); i++)
  //    {
  //      if (spellListText.IsSelected(i))
  //      {
  //        GLOBAL_SPELL_ID SpellDbKey;
  //        SpellDbKey.IllegalConversion(spellListText.GetSpellIndex(i));
  //
  //        if (!m_dude.KnowSpell( MagicUserFlag,
  //                               spellData.GetSpellSchool(SpellDbKey), 
  //                               SpellDbKey, 
  //                               spellData.GetSpellLevel(SpellDbKey),
  //                               TRUE ))
  //        {
  //          WriteDebugString("failed to knowSpell that was selected in INITIAL_MU_SPELLS_MENU_DATA\n");
  //        }
  //        else
  //          success = TRUE;
  //      }
  //    }
  //
  //    if (success)
  //    {
  //      handleSpellFormInput(SPELLFORM_exit, spellListText);
  //      PopEvent();
  //    }
  //    else
  //    {
  //      PushEvent(new ASK_YES_NO_MENU_DATA("YOU DID NOT SELECT ANY SPELLS, EXIT ANYWAY?",TRUE, CONTROL_CreateCharacter), DeleteEvent);
  //  }
  //}    
  //  break;    
  default:
    WriteDebugString("Unhandled menu item in INITIAL_MU_SPELLS_MENU_DATA");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  } 
}


bool AVAILABLE_SPELL::operator >(const AVAILABLE_SPELL& avs) const
{
  if (this->pSpellData->Level == avs.pSpellData->Level)
  {
    return pSpellData->Name > avs.pSpellData->Name;
  };
  return this->pSpellData->Level > avs.pSpellData->Level;
};

int CreateSpellAvailabilityList( CHARACTER *pChar,
                                 CArray<AVAILABLE_SPELL, AVAILABLE_SPELL&> *pSpellAvailabilityList,
                                 LPCSTR comment);


int INITIAL_MU_SPELLS_MENU_DATA::ListAllSpells(void)
  {
  // Returns max spell level.
  //POSITION pos;
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  int n, del;
  AVAILABLE_SPELL avs;
  int maxSpellLevel;
  m_dude.UpdateSpellAbility();
  // Add all possible spells to m_spellAvailabilityList
  maxSpellLevel = CreateSpellAvailabilityList(&m_dude,
                                              &m_spellAvailabilityList,
                                              "List possible spells at MU creation");
  // OK.  Now we will sort them for presentation to the player.
  n = m_spellAvailabilityList.GetSize();
  for (del=n/2; del>0; del /= 2)
  {
    int i;
    for (i=0; i<del; i++)
    {
      int j;
      for (j=i; j<n-del; j+=del)
      {
        if (m_spellAvailabilityList[j] > m_spellAvailabilityList[j+del])
        {
          int k;
          for (k=j; k>=0; k-=del)
          {
            if (!(m_spellAvailabilityList[k] > m_spellAvailabilityList[k+del])) break;
            avs = m_spellAvailabilityList[k];
            m_spellAvailabilityList[k] = m_spellAvailabilityList[k+del];
            m_spellAvailabilityList[k+del] = avs;
          };
        };
      };
    };
  };
  return maxSpellLevel;
}

void INITIAL_MU_SPELLS_MENU_DATA::FillSpellListText(bool newSpellLevel)
{
   //spellListText.FillNewSpellLevelListText(m_dude);
  int cnt, i, n;
  spellListText.Clear();
  spellListText.arbitraryText[1] = m_successMsg;
  spellListText.m_flags |= ARBITRARY_TEXT_FORMAT_FLAG;
  n = m_acquireStates.GetSize();
  for (i=1; i<n; i++)
  {
    AcquireState *pAcquire;
    pAcquire = &m_acquireStates[i];
    pAcquire->numAcquired = 0;
    pAcquire->numAttempted = 0;
    pAcquire->numAvail = 0;
  };
  if (m_currentSpellLevel >= m_acquireStates.GetSize())
  {
    return;
  };
  if (m_acquireStates[m_currentSpellLevel].mnmc.max == 0)
  {
    return;
  };
  n = m_spellAvailabilityList.GetSize();
  for (i=0; i<n; i++)
  {
    AVAILABLE_SPELL *pAvail;
    AcquireState *pAcquire;
    SPELL_DATA *pSpell;
    int spellLevel;
    pAvail = &m_spellAvailabilityList[i];
    pSpell = pAvail->pSpellData;
    //if (pSpell->AutoScribeAtLevelChange)
    if (pSpell->AutoScribe)
    {
      continue;  // Ignore these.....
                 // They will be automatically added when we are done.
    };
    spellLevel = pSpell->Level;
    if (spellLevel >= m_acquireStates.GetSize())
    {
      spellLevel = m_acquireStates.GetSize()-1;
    };
    pAcquire = &m_acquireStates[spellLevel];

    if (pAvail->referenced)
    {    
      pAcquire->numAttempted++;
      if (pAvail->learned) 
      {
        pAcquire->numAcquired++;
      };
      continue;
    }
    else
    {
      pAcquire->numAvail++;
      if (spellLevel == m_currentSpellLevel)
      {
        //spellListText.Add(m_spellAvailabilityList[i].pSpellData->m_gsID);
        SPELL_ID spellID;
        spellID = m_spellAvailabilityList[i].pSpellData->SpellID();
        spellListText.Add(spellID);
      };
    };
  };


  cnt = spellListText.GetCount() - startItem;
  ItemsOnPage = min(Items_Per_Page, cnt);

  if ( (ItemsOnPage > 0) && !newSpellLevel)
  {
    if (party.activeItem >= spellListText.GetCount() - startItem)
      party.activeItem = (BYTE)(spellListText.GetCount()-1-startItem);
    if (party.activeItem >= ItemsOnPage)
      party.activeItem = (BYTE)(ItemsOnPage-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    cnt = spellListText.GetCount() - startItem;
    ItemsOnPage = min(Items_Per_Page, cnt);
      //currShopItem = 0;
  };
}

//MNMC initialMU[] = {
//  //   min  num    max  certain
//  MNMC(0, 10000, 10000,    0),     // Level 0 is total over all levels
//  MNMC(2, 10000,     4,    1)      // Level 1
  // For now only one level.  Higher levels will use the Level 1 numbers
//};

//const int numInitialMU = sizeof(initialMU) / sizeof(initialMU[0]);


void INITIAL_MU_SPELLS_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  ClearFormattedText(errorTextData);
  int maxSpellLevel;
  menu.setMenu(InitialSpellsMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  //menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  spellListText.Clear();
  maxSpellLevel = ListAllSpells();  // --> m_spellAvailabilityList
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CLASS_DATA *pClass;
    CString result;
    MNMC mnmc;
    int level;
    //Not Implemented(0xe34a9, false);
    //if (m_dude.CharacterID().IsEmpty())
    if (m_dude.uniquePartyID == 0xff)
    //if (m_dude.GetUniqueId() == 0xff)
    {
      scriptContext.SetCreatedCharacterContext(&m_dude);
      hookParameters[4] = "I";
    }
    else
    {
      scriptContext.SetCharacterContext(&m_dude);
      hookParameters[4] = "L";
    };
    pClass = classData.GetClass(m_dude.GetClass());
    scriptContext.SetClassContext(pClass);
    m_acquireStates.RemoveAll();
    {
      AcquireState acquireState(&mnmc);
      m_acquireStates.Add(acquireState);  // Add [0]
    };
    m_acquireStates[0].mnmc.min = 0;
    m_acquireStates[0].mnmc.max = 99999;
    for (level=1; level<=maxSpellLevel; level++)
    {
      hookParameters[5].Format("%d", m_dude.GetLimitedInt());
      hookParameters[6].Format("%d", level);
      hookParameters[7] = "3";
      hookParameters[8] = "5";
      hookParameters[9] = "1";

      result = pClass->RunClassScripts(KNOWABLE_SPELLS_MINMAX,
                              ScriptCallback_RunAllScripts,
                              NULL,
                              "Initilize MU spells");
      sscanf(hookParameters[7], "%d", &mnmc.min);
      sscanf(hookParameters[8], "%d", &mnmc.max);
      sscanf(hookParameters[9], "%d", &mnmc.certain);

  	  mnmc.max = max(mnmc.max, 0);
	    mnmc.min = max(mnmc.min, 0);
	    mnmc.certain = max (mnmc.certain, 0);
	    //mnmc.certain = min(mnmc.certain, 100);
	    mnmc.max = max(mnmc.max, mnmc.min);

      AcquireState acquireState(&mnmc);
      m_acquireStates.Add(acquireState);
    };
  };

  
  //BOOL PopIt = FALSE;

  handleSpellFormInput(SPELLFORM_exit, spellListText);
  clearSpellItemState();


// Go through m_spellAvailability list.
// Automatically add spells for which mnmc.certain == 999999
  {
    int i, n;
    n = m_spellAvailabilityList.GetSize();
    for (i=0; i<n; i++)
    {
      const SPELL_DATA *pSpell;
      int level, certain;
      pSpell = m_spellAvailabilityList[i].pSpellData;
      level = pSpell->Level;
      certain = m_acquireStates[level].mnmc.certain;
      if (certain == 999999)
      {
        m_spellAvailabilityList[i].learned = true;
        m_spellAvailabilityList[i].referenced = true;
        m_dude.AddKnowableSpell(pSpell->uniqueName);
      };
    };
  };





  m_currentSpellLevel = 1;
  m_passNumber = 0;
  FillSpellListText(true);
  while (AreWeDone() & FinishedThisLevel)
  {
    m_currentSpellLevel++;
    if (m_currentSpellLevel >= m_acquireStates.GetSize())
    {
      AutoScribe();
      PopEvent();
      return;
    };
    FillSpellListText(true);
  };
  //setSpellNewLevelItemState(m_dude);
  handleSpellFormInput(SPELLFORM_initial, spellListText, initialSpellForm);

  // character has no capacity for additional spells
  //if (!spellListText.CanSelectAnything())
  //  PopIt = TRUE;

  //if (PopIt)
  //{
  //  handleSpellFormInput(SPELLFORM_exit, spellListText);
  //  PopEvent();
  //  return;
  //}
  Invalidate();
}

void INITIAL_MU_SPELLS_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  // draw middle horz bar, but 
  // not viewport and middle ver bar
  drawViewport(FALSE, FALSE, TRUE, FALSE);
  //if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText, initialSpellForm);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void INITIAL_MU_SPELLS_MENU_DATA::OnUpdateUI(void)
{
  /*
   { "SELECT", 0 },
  // { "UNSELECT", 0 },
  // { "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  menu.setAllItemsActive();
  //menu.setItemInactive(3); // remove

  if (spellListText.GetCount() <= 0)
  {
    menu.setItemInactive(1); // select
    //menu.setItemInactive(2); // unselect
    menu.setItemInactive(2); // next
    menu.setItemInactive(3); // prev
    return;
  }
 
  int currIndex = currSpellTextIndex();

  //if (spellListText.IsSelected(currIndex))
  //  menu.setItemInactive(1); // select
  //else
  //  menu.setItemInactive(2); // unselect

  //if (!spellListText.CanSelect(currIndex))
  //  menu.setItemInactive(1); // select

  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // prev
}

void INITIAL_MU_SPELLS_MENU_DATA::OnReturnToTopOfQueue()
{
  //if (party.tradeItem==1) // yes, exit anyway
  //{
  //  handleSpellFormInput(SPELLFORM_exit, spellListText);
  //  PopEvent();
  //}
  //else
  {
    die(0xce21a); //OnInitialEvent();
  }
}




////////////////////////////////////////////////////////////////////////LEARN_SPELLS_MENU
int LEARN_SPELLS_MENU::AreWeDone(void)
{
    // We are done if m_currentSpellLevel >= sizeof m_acquireStates 
  int result, remaining=0, maxRemaining, level;
  AcquireState *pAcquire;
  if (m_currentSpellLevel >= m_acquireStates.GetSize()) 
  {
    result = FinishedThisLevel | FinishedAllLevels;
  }
  else
  {
    maxRemaining = m_acquireStates[0].mnmc.max - m_acquireStates[0].numAcquired;
    if (m_acquireStates[0].mnmc.max == 0)
    {
      maxRemaining = 99999999;
    };
    if (   (spellListText.GetCount() == 0) 
        || (maxRemaining <= 0)
       )
    {
      result = FinishedThisLevel;
    }
    else
    {
      pAcquire = &m_acquireStates[m_currentSpellLevel];
      remaining = pAcquire->mnmc.max - pAcquire->numAcquired;
      if (remaining > pAcquire->numAvail)
      {
        remaining = pAcquire->numAvail;
      };
      if (remaining > maxRemaining)
      {
        remaining = maxRemaining;
      };
      if (remaining <= 0) 
      {
        result = FinishedThisLevel;
      }
      else
      {
        result = 0;
      };
    };
  };

  if (result & FinishedAllLevels)
  {
    result |= FinishedThisLevel;
    return result;
  };
  if (result & FinishedThisLevel)
  {
    // See if any level has spells yet to be learned.
    result |= FinishedAllLevels;
    for (level=1; level < m_acquireStates.GetSize(); level++)
    {
      //AcquireState *pAcquire;
      pAcquire = &m_acquireStates[level];
      if (pAcquire->numAvail > pAcquire->numAcquired)
      {
        result &= ~FinishedAllLevels;
      };
    };

  };
  if ((result & FinishedThisLevel) == 0)
  {
    char plural[2];
    if (remaining > 1)
    {
      plural[0] = 's';
      plural[1] = 0;
    }
    else
    {
      plural[0] = 0;
    };
    spellListText.arbitraryText[0].Format("You can learn %d level %d spell%s",
                                          remaining, 
                                          m_currentSpellLevel, 
                                          plural);
    spellListText.arbitraryText[1] = m_successMsg;
  };
  return result;
}


void LEARN_SPELLS_MENU::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    spellListText.arbitraryText[1] = "";
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "SELECT", 0 },
   //{ "UNSELECT", 0 },
   //{ "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  int currIndex = currSpellTextIndex();
  switch (menu.currentItem()) 
  {
  case 1: // "SELECT"
    //spellListText.ToggleSelected(currIndex);
    {
      //GLOBAL_SPELL_ID SpellDbKey;
      SPELL_ID spellID;
      SPELL_DATA *pSpell;
      AcquireState *pAcquire;
      int availSpellIndex;
      int i, n, finishedFlags;
      bool success;
      //SpellDbKey.IllegalConversion(spellListText.GetSpellIndex(currIndex));
      spellID = spellListText.GetSpellID(currIndex);

      //pSpell = spellData.GetSpellData(SpellDbKey);
      pSpell = spellData.GetSpell(spellID);

      
      n = m_spellAvailabilityList.GetSize();
      availSpellIndex = -1;
      for (i=0; i<n; i++)
      {
        if (pSpell == m_spellAvailabilityList[i].pSpellData)
        {
          availSpellIndex = i;
          break;
        };
      };
      if (availSpellIndex < 0)
      {
        die (0x9621c);
      };

      pAcquire = &m_acquireStates[m_currentSpellLevel];
      if (pAcquire->numAcquired < pAcquire->mnmc.certain)
      {
        success = true;
      }
      else
      {
        success = RollDice(100,1) <= m_spellAvailabilityList[availSpellIndex].probability;    
      };
      if (success)
      {
        //if (!m_dude.KnowSpell(MagicUserFlag,MagicUserFlag,pSpell->m_gsID,pSpell->Level,true))

        
        
        //if (!m_dude.KnowSpell(MagicUserFlag,MagicUserFlag,pSpell->SpellID(),pSpell->Level,true))
        if (!m_dude.KnowSpellyyy(pSpell, true))        
        {
          die(0xc60aa8);
        };
        m_dude.DelKnowableSpell(pSpell->uniqueName);
        m_successMsg.Format("%s successfully wrote %s into the spellbook.",
                                              m_dude.GetName(),
                                              pSpell->Name);
      }
      else
      {
        m_successMsg.Format("%s failed to learn %s.",
                                              m_dude.GetName(),
                                              pSpell->Name);
      };
      m_spellAvailabilityList[availSpellIndex].learned = success;
      m_spellAvailabilityList[availSpellIndex].referenced = true;
      FillSpellListText(false);  // sets m_listNumTotal and m_listNumLearned
      finishedFlags = AreWeDone();
      while ( (finishedFlags & FinishedAllLevels) == 0)
      {
        if (finishedFlags & FinishedThisLevel) 
        {  // Go to next level
          m_currentSpellLevel++;
          if (m_currentSpellLevel >= m_spellAvailabilityList.GetSize())
          {
            int j, num;
            m_currentSpellLevel = 1;
            num = m_spellAvailabilityList.GetSize();
            for (j=0; j<num; j++)
            {
              m_spellAvailabilityList[j].referenced = false;
            };
          };
          FillSpellListText(true);  // sets m_listNumTotal and m_listNumLearned
          finishedFlags = AreWeDone();
          continue;
        };
        return;
      };  
      handleSpellFormInput(SPELLFORM_exit, spellListText);
      PopEvent();
    };
    break;
  //case 2: // unselect
  //  spellListText.ToggleSelected(currIndex);
  //  break;
  //case 3: // remove
  //  // unused for this event type
  //  break;
  case 2: // next
    nextSpellPage(NULL);
    break;    
  case 3: // prev
    prevSpellPage(NULL);
    break;    
  //case 4: // exit
  //  {
  //    BOOL success = FALSE;
  //    for (int i=0; i<spellListText.GetCount(); i++)
  //    {
  //      if (spellListText.IsSelected(i))
  //      {
  //        GLOBAL_SPELL_ID SpellDbKey;
  //        SpellDbKey.IllegalConversion(spellListText.GetSpellIndex(i));
  //
  //        if (!m_dude.KnowSpell( MagicUserFlag,
  //                               spellData.GetSpellSchool(SpellDbKey), 
  //                               SpellDbKey, 
  //                               spellData.GetSpellLevel(SpellDbKey),
  //                               TRUE ))
  //        {
  //          WriteDebugString("failed to knowSpell that was selected in LEARN_SPELLS_MENU\n");
  //        }
  //        else
  //          success = TRUE;
  //      }
  //    }
  //
  //    if (success)
  //    {
  //      handleSpellFormInput(SPELLFORM_exit, spellListText);
  //      PopEvent();
  //    }
  //    else
  //    {
  //      PushEvent(new ASK_YES_NO_MENU_DATA("YOU DID NOT SELECT ANY SPELLS, EXIT ANYWAY?",TRUE, CONTROL_CreateCharacter), DeleteEvent);
  //  }
  //}    
  //  break;    
  default:
    WriteDebugString("Unhandled menu item in LEARN_SPELLS_MENU");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  } 
}



int LEARN_SPELLS_MENU::ListAllSpells(void)
{
  // Returns max spell level.
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  CLASS_DATA *pClass;
  CString spellList;
  int n, del;
  AVAILABLE_SPELL avs;
  int maxSpellLevel=0;
  // Add all possible spells to m_spellAvailabilityList
  m_spellAvailabilityList.RemoveAll();

  scriptContext.SetCreatedCharacterContext(&m_dude);
  pClass = classData.GetClass(m_dude.GetClass());
  scriptContext.SetClassContext(pClass);
  hookParameters[5].Format("%d",m_dude.GetLimitedInt());


  spellList = m_dude.char_asl.Lookup("$KnowableSpells$");

  while (spellList.GetLength() > 1)
  {
    if (spellList[0] != '?')
    {
      die(0xca12ab);
    };
    {
      SPELL_DATA *pSpell;
      CString spellName;
      SPELL_ID spellID;
      int index;
      CString result;
      int probability;
      index = spellList.Find('?',1);
      if (index < 0)
      {
        index = spellList.GetLength();
      };
      spellName = spellList.Mid(1,index-1);
      //pSpell = spellData.GetSpellData(spellName);
      spellID = spellName;
      pSpell = spellData.GetSpell(spellID);
      spellList = spellList.Right(spellList.GetLength()-index);
      if (pSpell == NULL)
      {
        continue;
      };
      scriptContext.SetSpellContext(pSpell);
      hookParameters[6].Format("%d",pSpell->Level);

    
      result = pClass->RunClassScripts(
                           LEARN_SPELLS, 
                           ScriptCallback_RunAllScripts,
                           NULL,
                           "Listing all 'learnable' spells");
      if (result.IsEmpty())
      {
        result = pSpell->RunSpellScripts(LEARN_SPELLS, 
                           ScriptCallback_RunAllScripts, 
                           NULL,
                           "Listing all 'learnable' spells");
      };
      if (result.IsEmpty())
      {
        probability = 100;
      }
      else
      {
        sscanf(result, "%d", &probability);
      };
      if (probability != 0)
      {
        avs.pSpellData = pSpell;
        avs.probability = probability;
        m_spellAvailabilityList.Add(avs);
        if (avs.pSpellData->Level > maxSpellLevel)
        {
          maxSpellLevel = avs.pSpellData->Level;
        };
      };
    };
  };
  // OK.  Now we will sort them for presentation to the player.
  n = m_spellAvailabilityList.GetSize();
  for (del=n/2; del>0; del /= 2)
  {
    int i;
    for (i=0; i<del; i++)
    {
      int j;
      for (j=i; j<n-del; j+=del)
      {
        if (m_spellAvailabilityList[j] > m_spellAvailabilityList[j+del])
        {
          int k;
          for (k=j; k>=0; k-=del)
          {
            if (!(m_spellAvailabilityList[k] > m_spellAvailabilityList[k+del])) break;
            avs = m_spellAvailabilityList[k];
            m_spellAvailabilityList[k] = m_spellAvailabilityList[k+del];
            m_spellAvailabilityList[k+del] = avs;
          };
        };
      };
    };
  };
  return maxSpellLevel;
}

void LEARN_SPELLS_MENU::FillSpellListText(bool newSpellLevel)
{
   //spellListText.FillNewSpellLevelListText(m_dude);
  int cnt, i, n;
  spellListText.Clear();
  spellListText.m_flags |= ARBITRARY_TEXT_FORMAT_FLAG;
  n = m_acquireStates.GetSize();
  for (i=0; i<n; i++)
  {
    AcquireState *pAcquire;
    pAcquire = &m_acquireStates[i];
    pAcquire->numAcquired = 0;
    pAcquire->numAttempted = 0;
    pAcquire->numAvail = 0;
  };
  n = m_spellAvailabilityList.GetSize();
  for (i=0; i<n; i++)
  {
    AVAILABLE_SPELL *pAvail;
    AcquireState *pAcquire;
    SPELL_DATA *pSpell;
    int spellLevel;
    pAvail = &m_spellAvailabilityList[i];  //Next available spell
    pSpell = pAvail->pSpellData;
    spellLevel = pSpell->Level;
    if (spellLevel >= m_acquireStates.GetSize())
    {
      spellLevel = m_acquireStates.GetSize()-1;
    };
    pAcquire = &m_acquireStates[spellLevel]; //Stats for this level
    if (pAvail->learned)
    {
      pAcquire->numAttempted++;
      pAcquire->numAcquired++;
      m_acquireStates[0].numAcquired++;
      continue;
    };

    if (pAvail->referenced)
    {    
      pAcquire->numAttempted++;
      continue;
    }
    else
    {
      pAcquire->numAvail++;
      if (spellLevel == m_currentSpellLevel)
      {
        //spellListText.Add(m_spellAvailabilityList[i].pSpellData->m_gsID);
        SPELL_ID spellID;
        spellID = m_spellAvailabilityList[i].pSpellData->SpellID();
        spellListText.Add(spellID);
      };
    };
  };


  cnt = spellListText.GetCount() - startItem;
  ItemsOnPage = min(Items_Per_Page, cnt);

  if ( (ItemsOnPage > 0) && !newSpellLevel)
  {
    if (party.activeItem >= spellListText.GetCount() - startItem)
      party.activeItem = (BYTE)(spellListText.GetCount()-1-startItem);
    if (party.activeItem >= ItemsOnPage)
      party.activeItem = (BYTE)(ItemsOnPage-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    cnt = spellListText.GetCount() - startItem;
    ItemsOnPage = min(Items_Per_Page, cnt);
      //currShopItem = 0;
  };
}

//MNMC initialMU[] = {
//  //   min  num    max  certain
//  MNMC(0, 10000, 10000,    0),     // Level 0 is total over all levels
//  MNMC(2, 10000,     4,    1)      // Level 1
  // For now only one level.  Higher levels will use the Level 1 numbers
//};

//const int numInitialMU = sizeof(initialMU) / sizeof(initialMU[0]);


void LEARN_SPELLS_MENU::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  int maxSpellLevel, level;
  menu.setMenu(LearnSpellsMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  //menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  spellListText.Clear();
  maxSpellLevel = ListAllSpells();
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CLASS_DATA *pClass;
    CString result;
    MNMC mnmc;
    AcquireState acquireState(&mnmc);
    scriptContext.SetCreatedCharacterContext(&m_dude);
    pClass = classData.GetClass(m_dude.GetClass());
    scriptContext.SetClassContext(pClass);
    m_acquireStates.RemoveAll();
    for (level=0; level<=maxSpellLevel; level++)
    {
      int numKnown;
      int i,n;
      //POSITION pos;
      CHARACTER_SPELL *pCharSp;
      numKnown = 0;

      //pos = m_dude.GetFirstSpellPosition();
      //while (pos != NULL)
      n = m_dude.GetSpellCount();
      for (i=0; i<n; i++)
      {
        pCharSp = m_dude.GetCharacterSpell(i);
        if (pCharSp->level == level)
        {
          numKnown++;
        };
      };

      hookParameters[4] = "I";
      hookParameters[5].Format("%d", m_dude.GetLimitedInt());
      hookParameters[6].Format("%d", level);
      hookParameters[7].Format("%d", numKnown);
      hookParameters[8] = (level==0)?"0":"3";
      hookParameters[9] = "1";

      result = pClass->RunClassScripts(LEARN_SPELLS_NUM,
                              ScriptCallback_RunAllScripts,
                              NULL,
                              "Character about to be able to learn spells");
      //sscanf(hookParameters[7], "%d", &mnmc.min);
      sscanf(hookParameters[8], "%d", &acquireState.mnmc.max);
      sscanf(hookParameters[9], "%d", &acquireState.mnmc.certain);

  	  acquireState.mnmc.max = max(acquireState.mnmc.max, 0);
	    //mnmc.min = max(mnmc.min, 0);
	    acquireState.mnmc.certain = max(acquireState.mnmc.certain, 0);
	    acquireState.mnmc.certain = min(acquireState.mnmc.certain, 100);
	  //mnmc.max = max(mnmc.max, mnmc.min);

      m_acquireStates.Add(acquireState);
    };
  };

  
  //BOOL PopIt = FALSE;

  handleSpellFormInput(SPELLFORM_exit, spellListText);
  clearSpellItemState();
  m_currentSpellLevel = 1;
  FillSpellListText(true);
  while (AreWeDone() & FinishedThisLevel)
  {
    m_currentSpellLevel++;
    if (m_currentSpellLevel >= m_acquireStates.GetSize())
    {
      PopEvent();
      return;
    };
    FillSpellListText(true);
  };
  //setSpellNewLevelItemState(m_dude);
  handleSpellFormInput(SPELLFORM_initial, spellListText, initialSpellForm);

  // character has no capacity for additional spells
  //if (!spellListText.CanSelectAnything())
  //  PopIt = TRUE;

  //if (PopIt)
  //{
  //  handleSpellFormInput(SPELLFORM_exit, spellListText);
  //  PopEvent();
  //  return;
  //}
  Invalidate();
}

void LEARN_SPELLS_MENU::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  // draw middle horz bar, but 
  // not viewport and middle ver bar
  drawViewport(FALSE, FALSE, TRUE, FALSE);
  //if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText, initialSpellForm);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void LEARN_SPELLS_MENU::OnUpdateUI(void)
{
  /*
   { "SELECT", 0 },
  // { "UNSELECT", 0 },
  // { "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  menu.setAllItemsActive();
  //menu.setItemInactive(3); // remove

  if (spellListText.GetCount() <= 0)
  {
    menu.setItemInactive(1); // select
    //menu.setItemInactive(2); // unselect
    menu.setItemInactive(2); // next
    menu.setItemInactive(3); // prev
    return;
  }
 
  int currIndex = currSpellTextIndex();

  //if (spellListText.IsSelected(currIndex))
  //  menu.setItemInactive(1); // select
  //else
  //  menu.setItemInactive(2); // unselect

  //if (!spellListText.CanSelect(currIndex))
  //  menu.setItemInactive(1); // select

  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // prev
}

void LEARN_SPELLS_MENU::OnReturnToTopOfQueue()
{
  //if (party.tradeItem==1) // yes, exit anyway
  //{
  //  handleSpellFormInput(SPELLFORM_exit, spellListText);
  //  PopEvent();
  //}
  //else
  {
    die(0xce21a); //OnInitialEvent();
  }
}















































////////////////////////////////////////////////////////////////////////LEVEL_UP_LEARN_SPELLS_MENU
int LEVEL_UP_LEARN_SPELLS_MENU::AreWeDone(void)
{
    // We are done if m_currentSpellLevel >= sizeof m_acquireStates 
  int result=0, remaining=0, maxRemaining;
  {
    maxRemaining = m_acquireStates[0].mnmc.max - m_acquireStates[0].numAcquired;
    if (m_acquireStates[0].mnmc.max == 0)
    {
      maxRemaining = 99999999;
    };
    if (   (spellListText.GetCount() == 0) 
        || (maxRemaining <= 0)
       )
    {
      result = FinishedAllLevels;
    }
    else
    {
      int i;
      spellListText.arbitraryText[0] = "";
      spellListText.arbitraryText[1] = "";
      for (i=1; i<m_acquireStates.GetSize(); i++)
      {
        CString num;
        int numCanLearn;
        AcquireState *pAcquire;
        pAcquire = &m_acquireStates[i];
        num.Format("%3d", i);
        spellListText.arbitraryText[0] += num;
        numCanLearn = pAcquire->numAvail;
        if (numCanLearn > pAcquire->mnmc.max - pAcquire->numAcquired)
        {
          numCanLearn = m_acquireStates[i].mnmc.max - pAcquire->numAcquired;
        };
        if (numCanLearn > maxRemaining)
        {
          numCanLearn = maxRemaining;
        };
        num.Format("%3d", numCanLearn);
        m_acquireStates[0].numAvail += numCanLearn;
        spellListText.arbitraryText[1] += num;
      };
    };
  };
  if (m_acquireStates[0].numAvail < maxRemaining)
  {
    maxRemaining = m_acquireStates[0].numAvail;
  };
  {
    char plural[2];
    if (maxRemaining > 1)
    {
      plural[0] = 's';
      plural[1] = 0;
    }
    else
    {
      plural[0] = 0;
    };
    spellListText.arbitraryText[2].Format("You can learn a total of %d spell%s",
                                          maxRemaining, 
                                          plural);
  };
  return result;
}


void LEVEL_UP_LEARN_SPELLS_MENU::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "LEARN", 0 },
   //{ "UNSELECT", 0 },
   //{ "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  int currIndex = currSpellTextIndex();
  switch (menu.currentItem()) 
  {
  case 1: // "LEARN"
    //spellListText.ToggleSelected(currIndex);
    {
      //GLOBAL_SPELL_ID SpellDbKey;
      SPELL_ID spellID;
      SPELL_DATA *pSpell;
      AcquireState *pAcquire;
      int availSpellIndex, spellLevel;
      int i, n, finishedFlags;
      bool success;
      //SpellDbKey.IllegalConversion(spellListText.GetSpellIndex(currIndex));
      spellID = spellListText.GetSpellID(currIndex);

      //pSpell = spellData.GetSpellData(SpellDbKey);
      pSpell = spellData.GetSpell(spellID);

      
      n = m_spellAvailabilityList.GetSize();
      availSpellIndex = -1;
      for (i=0; i<n; i++)
      {
        if (pSpell == m_spellAvailabilityList[i].pSpellData)
        {
          availSpellIndex = i;
          break;
        };
      };
      if (availSpellIndex < 0)
      {
        die (0x9621c);
      };

      spellLevel = pSpell->Level;
      pAcquire = &m_acquireStates[spellLevel];
      if (pAcquire->numAcquired < pAcquire->mnmc.certain)
      {
        success = true;
      }
      else
      {
        success = RollDice(100,1) <= m_spellAvailabilityList[availSpellIndex].probability;    
      };
      if (success)
      {
        //if (!m_dude.KnowSpell(MagicUserFlag,MagicUserFlag,pSpell->m_gsID,pSpell->Level,true))
        if (!m_dude.KnowSpellyyy(pSpell, true))
        {
          die(0xc60aa8);
        };
        m_dude.DelKnowableSpell(pSpell->uniqueName);
        spellListText.arbitraryText[3].Format("%s successfully wrote %s into the spellbook.",
                                              m_dude.GetName(),
                                              pSpell->Name);
      }
      else
      {
        spellListText.arbitraryText[3].Format("%s failed to learn %s.",
                                              m_dude.GetName(),
                                              pSpell->Name);
      };
      m_spellAvailabilityList[availSpellIndex].learned = success;
      m_spellAvailabilityList[availSpellIndex].referenced = true;
      FillSpellListText(false);  // sets m_listNumTotal and m_listNumLearned
      finishedFlags = AreWeDone();
      if ( (finishedFlags & FinishedAllLevels) == 0)
      {
        return;
      };  
      handleSpellFormInput(SPELLFORM_exit, spellListText);
      PopEvent();
    };
    break;
  //case 2: // unselect
  //  spellListText.ToggleSelected(currIndex);
  //  break;
  //case 3: // remove
  //  // unused for this event type
  //  break;
  case 2: // next
    nextSpellPage(NULL);
    break;    
  case 3: // prev
    prevSpellPage(NULL);
    break;    
  //case 4: // exit
  //  {
  //    BOOL success = FALSE;
  //    for (int i=0; i<spellListText.GetCount(); i++)
  //    {
  //      if (spellListText.IsSelected(i))
  //      {
  //        GLOBAL_SPELL_ID SpellDbKey;
  //        SpellDbKey.IllegalConversion(spellListText.GetSpellIndex(i));
  //
  //        if (!m_dude.KnowSpell( MagicUserFlag,
  //                               spellData.GetSpellSchool(SpellDbKey), 
  //                               SpellDbKey, 
  //                               spellData.GetSpellLevel(SpellDbKey),
  //                               TRUE ))
  //        {
  //          WriteDebugString("failed to knowSpell that was selected in LEVEL_UP_LEARN_SPELLS_MENU\n");
  //        }
  //        else
  //          success = TRUE;
  //      }
  //    }
  //
  //    if (success)
  //    {
  //      handleSpellFormInput(SPELLFORM_exit, spellListText);
  //      PopEvent();
  //    }
  //    else
  //    {
  //      PushEvent(new ASK_YES_NO_MENU_DATA("YOU DID NOT SELECT ANY SPELLS, EXIT ANYWAY?",TRUE, CONTROL_CreateCharacter), DeleteEvent);
  //  }
  //}    
  //  break;    
  default:
    WriteDebugString("Unhandled menu item in LEVEL_UP_LEARN_SPELLS_MENU");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  } 
}



int LEVEL_UP_LEARN_SPELLS_MENU::ListAllSpells(void)
{
  // Returns max spell level.
  HOOK_PARAMETERS hookParameters;
  SCRIPT_CONTEXT scriptContext;
  CLASS_DATA *pClass;
  CString spellList;
  int n, del;
  AVAILABLE_SPELL avs;
  int maxSpellLevel=0;
  // Add all possible spells to m_spellAvailabilityList
  m_spellAvailabilityList.RemoveAll();

  scriptContext.SetCharacterContext(&m_dude);
  pClass = classData.GetClass(m_dude.GetClass());
  scriptContext.SetClassContext(pClass);
  hookParameters[5].Format("%d",m_dude.GetLimitedInt());


  spellList = m_dude.char_asl.Lookup("$KnowableSpells$");

  while (spellList.GetLength() > 1)
  {
    if (spellList[0] != '?')
    {
      die(0xca12ab);
    };
    {
      SPELL_DATA *pSpell;
      CString spellName;
      SPELL_ID spellID;
      int index;
      CString result;
      int probability;
      index = spellList.Find('?',1);
      if (index < 0)
      {
        index = spellList.GetLength();
      };
      spellName = spellList.Mid(1,index-1);
      spellID = spellName;
      pSpell = spellData.GetSpell(spellID);
      spellList = spellList.Right(spellList.GetLength()-index);
      if (pSpell == NULL)
      {
        continue;
      };
      scriptContext.SetSpellContext(pSpell);
      hookParameters[6].Format("%d",pSpell->Level);

    
      result = pClass->RunClassScripts(
                         LEARN_SPELLS, 
                         ScriptCallback_RunAllScripts,
                         NULL,
                         "Player can learn spells after level-up");
      if (result.IsEmpty())
      {
        result = pSpell->RunSpellScripts(
                         LEARN_SPELLS, 
                         ScriptCallback_RunAllScripts,
                         NULL,
                         "Player can learn spells after level-up");
      };
      if (result.IsEmpty())
      {
        probability = 100;
      }
      else
      {
        sscanf(result, "%d", &probability);
      };
      if (probability != 0)
      {
        avs.pSpellData = pSpell;
        avs.probability = probability;
        m_spellAvailabilityList.Add(avs);
        if (avs.pSpellData->Level > maxSpellLevel)
        {
          maxSpellLevel = avs.pSpellData->Level;
        };
      };
    };
  };
  // OK.  Now we will sort them for presentation to the player.
  n = m_spellAvailabilityList.GetSize();
  for (del=n/2; del>0; del /= 2)
  {
    int i;
    for (i=0; i<del; i++)
    {
      int j;
      for (j=i; j<n-del; j+=del)
      {
        if (m_spellAvailabilityList[j] > m_spellAvailabilityList[j+del])
        {
          int k;
          for (k=j; k>=0; k-=del)
          {
            if (!(m_spellAvailabilityList[k] > m_spellAvailabilityList[k+del])) break;
            avs = m_spellAvailabilityList[k];
            m_spellAvailabilityList[k] = m_spellAvailabilityList[k+del];
            m_spellAvailabilityList[k+del] = avs;
          };
        };
      };
    };
  };
  return maxSpellLevel;
}

void LEVEL_UP_LEARN_SPELLS_MENU::FillSpellListText(bool newSpellLevel)
{
   //spellListText.FillNewSpellLevelListText(m_dude);
  int cnt, i, n;
  spellListText.Clear();
  spellListText.m_flags |= ARBITRARY_TEXT_FORMAT_FLAG;
  n = m_acquireStates.GetSize();
  for (i=0; i<n; i++)
  {
    AcquireState *pAcquire;
    pAcquire = &m_acquireStates[i];
    pAcquire->numAcquired = 0;
    pAcquire->numAttempted = 0;
    pAcquire->numAvail = 0;
  };
  n = m_spellAvailabilityList.GetSize();
  for (i=0; i<n; i++)
  {  // Summarize number attempted and number acquired for each level
    AVAILABLE_SPELL *pAvail;
    AcquireState *pAcquire;
    SPELL_DATA *pSpell;
    int spellLevel;
    pAvail = &m_spellAvailabilityList[i];
    pSpell = pAvail->pSpellData;
    spellLevel = pSpell->Level;
    if (spellLevel >= m_acquireStates.GetSize())
    {
      spellLevel = m_acquireStates.GetSize()-1;
    };
    pAcquire = &m_acquireStates[spellLevel];

    if (pAvail->referenced)
    {    
      pAcquire->numAttempted++;
      if (pAvail->learned) 
      {
        pAcquire->numAcquired++;
        m_acquireStates[0].numAcquired++;
      };
      continue;
    };
  };
  for (i=0; i<n; i++)
  {
    AVAILABLE_SPELL *pAvail;
    AcquireState *pAcquire;
    SPELL_DATA *pSpell;
    int spellLevel;
    pAvail = &m_spellAvailabilityList[i];
    pSpell = pAvail->pSpellData;
    spellLevel = pSpell->Level;
    if (spellLevel >= m_acquireStates.GetSize())
    {
      spellLevel = m_acquireStates.GetSize()-1;
    };
    pAcquire = &m_acquireStates[spellLevel];
    if (pAcquire->numAcquired >= pAcquire->mnmc.max)
    {
      continue;
    };
    if (pAvail->referenced)
    {    
      pAcquire->numAttempted++;
      if (pAvail->learned) 
      {
        pAcquire->numAcquired++;
        m_acquireStates[0].numAcquired++;
      };
      continue;
    }
    else
    {
      pAcquire->numAvail++;
      //spellListText.Add(m_spellAvailabilityList[i].pSpellData->m_gsID);
      SPELL_ID spellID = m_spellAvailabilityList[i].pSpellData->SpellID();
      spellListText.Add(spellID);
    };
  };


  cnt = spellListText.GetCount() - startItem;
  ItemsOnPage = min(Items_Per_Page, cnt);

  if ( (ItemsOnPage > 0) && !newSpellLevel)
  {
    if (party.activeItem >= spellListText.GetCount() - startItem)
      party.activeItem = (BYTE)(spellListText.GetCount()-1-startItem);
    if (party.activeItem >= ItemsOnPage)
      party.activeItem = (BYTE)(ItemsOnPage-1);
    if (party.activeItem < 0)
      party.activeItem = 0;
  }
  else
  {
    party.activeItem = 0;
    startItem = 0;
    cnt = spellListText.GetCount() - startItem;
    ItemsOnPage = min(Items_Per_Page, cnt);
      //currShopItem = 0;
  };
}

//MNMC initialMU[] = {
//  //   min  num    max  certain
//  MNMC(0, 10000, 10000,    0),     // Level 0 is total over all levels
//  MNMC(2, 10000,     4,    1)      // Level 1
  // For now only one level.  Higher levels will use the Level 1 numbers
//};

//const int numInitialMU = sizeof(initialMU) / sizeof(initialMU[0]);


void LEVEL_UP_LEARN_SPELLS_MENU::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  int maxSpellLevel, level;
  menu.setMenu(LevelUpLearnSpellsMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  //menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  spellListText.Clear();
  maxSpellLevel = ListAllSpells();
  {
    HOOK_PARAMETERS hookParameters;
    SCRIPT_CONTEXT scriptContext;
    CLASS_DATA *pClass;
    CString result;
    MNMC mnmc;
    AcquireState acquireState(&mnmc);
    scriptContext.SetCreatedCharacterContext(&m_dude);
    pClass = classData.GetClass(m_dude.GetClass());
    scriptContext.SetClassContext(pClass);
    m_acquireStates.RemoveAll();
    for (level=0; level<=maxSpellLevel; level++)
    {
      int numKnown;
      int i, n;
      //POSITION pos;
      CHARACTER_SPELL *pCharSp;
      numKnown = 0;
      //pos = m_dude.GetFirstSpellPosition();
      //while (pos != NULL)
      n = m_dude.GetSpellCount();
      for (i=0; i<n; i++)
      {
        pCharSp = m_dude.GetCharacterSpell(i);
        if (pCharSp->level == level)
        {
          numKnown++;
        };
      };
      hookParameters[4] = "L";
      hookParameters[5].Format("%d", m_dude.GetLimitedInt());
      hookParameters[6].Format("%d", level);
      hookParameters[7].Format("%d", numKnown);
      hookParameters[8] = (level==0)?"0":"3";
      hookParameters[9] = "1";

      result = pClass->RunClassScripts(LEARN_SPELLS_NUM,
                              ScriptCallback_RunAllScripts,
                              NULL,
                              "Player can learn spells after level-up");
      //sscanf(hookParameters[7], "%d", &mnmc.min);
      sscanf(hookParameters[8], "%d", &acquireState.mnmc.max);
      sscanf(hookParameters[9], "%d", &acquireState.mnmc.certain);

  	  acquireState.mnmc.max = max(acquireState.mnmc.max, 0);
	    //mnmc.min = max(mnmc.min, 0);
	    acquireState.mnmc.certain = max(acquireState.mnmc.certain, 0);
	    acquireState.mnmc.certain = min(acquireState.mnmc.certain, 100);
	  //mnmc.max = max(mnmc.max, mnmc.min);

      m_acquireStates.Add(acquireState);
    };
  };

  
  //BOOL PopIt = FALSE;

  handleSpellFormInput(SPELLFORM_exit, spellListText);
  clearSpellItemState();
  FillSpellListText(true);
  if (AreWeDone() & FinishedAllLevels)
  {
      PopEvent();
      return;
  };
  //setSpellNewLevelItemState(m_dude);
  handleSpellFormInput(SPELLFORM_initial, spellListText, initialSpellForm);

  // character has no capacity for additional spells
  //if (!spellListText.CanSelectAnything())
  //  PopIt = TRUE;

  //if (PopIt)
  //{
  //  handleSpellFormInput(SPELLFORM_exit, spellListText);
  //  PopEvent();
  //  return;
  //}
  Invalidate();
}

void LEVEL_UP_LEARN_SPELLS_MENU::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  // draw middle horz bar, but 
  // not viewport and middle ver bar
  drawViewport(FALSE, FALSE, TRUE, TRUE);
  //if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText, initialSpellForm);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void LEVEL_UP_LEARN_SPELLS_MENU::OnUpdateUI(void)
{
  /*
   { "SELECT", 0 },
  // { "UNSELECT", 0 },
  // { "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  menu.setAllItemsActive();
  //menu.setItemInactive(3); // remove

  if (spellListText.GetCount() <= 0)
  {
    menu.setItemInactive(1); // select
    //menu.setItemInactive(2); // unselect
    menu.setItemInactive(2); // next
    menu.setItemInactive(3); // prev
    return;
  }
 
  int currIndex = currSpellTextIndex();

  //if (spellListText.IsSelected(currIndex))
  //  menu.setItemInactive(1); // select
  //else
  //  menu.setItemInactive(2); // unselect

  //if (!spellListText.CanSelect(currIndex))
  //  menu.setItemInactive(1); // select

  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // prev
}

void LEVEL_UP_LEARN_SPELLS_MENU::OnReturnToTopOfQueue()
{
  //if (party.tradeItem==1) // yes, exit anyway
  //{
  //  handleSpellFormInput(SPELLFORM_exit, spellListText);
  //  PopEvent();
  //}
  //else
  {
    die(0xce21a); //OnInitialEvent();
  }
}




































////////////////////////////////////////////////////////////////////////MEMORIZE_MENU_DATA
void MEMORIZE_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "SELECT", 0 },
   { "UNSELECT", 0 },
   { "FORGET", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  int currIndex = currSpellTextIndex();
  switch (menu.currentItem()) 
  {
  case 1: // "SELECT"
    spellListText.IncreaseSpellSelectedCount(currIndex);
    break;
  case 2: // "UNSELECT"
    spellListText.IncreaseSpellSelectedCount(currIndex, -1);
    break;
  case 3: // "FORGET"
    spellListText.IncreaseSpellMemorizedCount(currIndex, -1);
    break;
  case 4: // "NEXT"
    nextSpellPage(NULL);
    break;    
  case 5: // "PREV"
    prevSpellPage(NULL);
    break;    
  case 6: // "EXIT"
    // Go through the list of spells and add the 
    // selected spells to the character's spellbook.
    {
      int i, n;
      n = spellListText.memSum.GetMemorizeItemCount();
      for (i=0; i<n; i++)
      {
        const MEMORIZE_ITEM *pMemorizeItem;
        pMemorizeItem = spellListText.memSum.PeekMemorizeItem(i);
        {
          CHARACTER *pChar;
          CHARACTER_SPELL *pCharSpell;
          pChar = &GetActiveChar(this);
          pCharSpell = pChar->GetCharacterSpell(pMemorizeItem->charSpellIndex);
          pCharSpell->selected = pMemorizeItem->numSelected;
          pCharSpell->memorized = pMemorizeItem->numMemorized;
        };
      };
    };
    handleSpellFormInput(SPELLFORM_exit, spellListText, memorizeSpellForm);
    PopEvent();
    break;    
  default:
    WriteDebugString("Unhandled menu item in MEMORIZE_MENU_DATA");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText, memorizeSpellForm);
    PopEvent();
    break;
  } 
}

void MEMORIZE_MENU_DATA::OnInitialEvent(void)
{
  if (!GetActiveChar(this).CanCastSpells())
  {
    PopEvent();
    return;
  }
  ClearFormattedText(textData);
  menu.setMenu(MemorizeMenuData, NULL, FALSE, this, "Memorize");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);
  setSpellMemorizeItemState(GetActiveChar(this));
  handleSpellFormInput(SPELLFORM_exit, spellListText, memorizeSpellForm);
  handleSpellFormInput(SPELLFORM_initial, spellListText, memorizeSpellForm);
  Invalidate();
}

void MEMORIZE_MENU_DATA::OnDraw(void)
{
  //UpdateMemorizeMenuScreen();
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  // draw middle horz bar, but 
  // not viewport and middle ver bar
  drawViewport(FALSE, FALSE, TRUE, FALSE);
  if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText, memorizeSpellForm);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void MEMORIZE_MENU_DATA::OnMouseClickLeft(int mx, int my)
{  
  if (MouseInventory(mx, my))
  {
    simulatedKeyQueue.PushKey(VK_RETURN);
    menu.setCurrentItem(1);
  }
  else
    MouseMenu(mx, my);
}

void MEMORIZE_MENU_DATA::OnUpdateUI(void)
{
  /*
   { "SELECT", 0 },
   { "UNSELECT", 0 },
   { "REMOVE", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  menu.setAllItemsActive();
  if (spellListText.GetMemorizeCount() <= 0)
  {
    menu.setItemInactive(1); // "SELECT
    menu.setItemInactive(2); // "UNSELECT"
    menu.setItemInactive(3); // "FORGET"
    menu.setItemInactive(4); // "NEXT"
    menu.setItemInactive(5); // "PREV"
    return;
  }
 
  int currIndex = currSpellTextIndex();
  const MEMORIZE_ITEM *pMemorizeItem;
  pMemorizeItem = spellListText.memSum.PeekMemorizeItem(currIndex);
  
  //int level;
  //int school;
  //spellListText.GetSpellLevelAndSchoolIndex(currIndex, level, school);

  if (pMemorizeItem->available <= 0)
    menu.setItemInactive(1); // "SELECT"
  if (pMemorizeItem->numSelected <= pMemorizeItem->numMemorized)
    menu.setItemInactive(2); // "UNSELECT"
  //if (numMem <= 0)
  if (pMemorizeItem->numMemorized <= 0)
    menu.setItemInactive(3); // "FORGET"
  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(4); // "NEXT"
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(5); // "PREV"
}

////////////////////////////////////////////////////////////////////////DISPLAY_SPELL_EFFECTS_MENU_DATA
void DISPLAY_SPELL_EFFECTS_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key,ascii);
    return;
  }
  /*
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  switch (menu.currentItem()) 
  {
  case 1: // next
    nextSpellPage(NULL);
    break;    
  case 2: // prev
    prevSpellPage(NULL);
    break;    
  case 3: // exit
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;    
  default:
    WriteDebugString("Unhandled menu item in DISPLAY_SPELL_EFFECTS_MENU_DATA");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  } 
}

void DISPLAY_SPELL_EFFECTS_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(DisplaySpellEffectsMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 3);
  setSpellEffectItemState(GetActiveChar(m_pOrigEvent));
  handleSpellFormInput(SPELLFORM_exit, spellListText);
  handleSpellFormInput(SPELLFORM_initial, spellListText);
  Invalidate();
}

void DISPLAY_SPELL_EFFECTS_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);

}

void DISPLAY_SPELL_EFFECTS_MENU_DATA::OnUpdateUI(void)
{
  /*
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */   
  menu.setAllItemsActive();
  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(1); // next
  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(2); // prev
}

////////////////////////////////////////////////////////////////////////MEMORIZE_VERIFY_MENU_DATA
void MEMORIZE_VERIFY_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "KEEP", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "RESELECT", 0 }
  */
  switch (menu.currentItem()) 
  {
  case 1: // keep
    PopEvent();
    break;    
  case 2: // next
    nextSpellPage(NULL);
    break;    
  case 3: // prev
    prevSpellPage(NULL);
    break;    
  case 4: // reselect
    GetActiveChar(this).UnselectIfNotMemorized();
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;    
  default:
    WriteDebugString("Unhandled menu item in MEMORIZE_VERIFY_MENU_DATA");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  } 
}

void MEMORIZE_VERIFY_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(MemorizeVerifyMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  //GetActiveChar(this).SortSpells();
  // PRS July 2009setSpellItemState(GetActiveChar(this), GetActiveChar(this).SpellBook(), (LEVEL_FLAG | SHOW_SELECTED_FLAG));
  setSpellItemState(GetActiveChar(this), (LEVEL_FLAG | SHOW_SELECTED_FLAG));
  handleSpellFormInput(SPELLFORM_exit, spellListText);
  handleSpellFormInput(SPELLFORM_initial, spellListText);
}

void MEMORIZE_VERIFY_MENU_DATA::OnDraw(void)
{
  //UpdateMemorizeVerifyMenuScreen();
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  //displaySpells();
  if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void MEMORIZE_VERIFY_MENU_DATA::OnUpdateUI(void)
{
  /*
   { "KEEP", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "RESELECT", 0 }
  */
  menu.setAllItemsActive();
  if (!levelData.zoneData.zones[levelData.area[party.Posy][party.Posx].zone].AllowMagic)
    menu.setItemInactive(1); // cast

  if (spellListText.GetCount() <= 0)
    menu.setItemInactive(1); // cast
    
  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // next

  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // prev
}

////////////////////////////////////////////////////////////////////////TARGET_SELECT_NONCOMBAT_EVENT_DATA

TARGET_SELECT_NONCOMBAT_EVENT_DATA::TARGET_SELECT_NONCOMBAT_EVENT_DATA(CHARACTER *data) : GameEvent()
{
  Clear();   
  event=CONTROL_TargetSelectNonCombat; 
  m_pCaster=data; 
  if (m_pCaster == NULL) 
    PopEvent();
}

TARGET_SELECT_NONCOMBAT_EVENT_DATA::~TARGET_SELECT_NONCOMBAT_EVENT_DATA()
{ 
  Clear(); 
}

void TARGET_SELECT_NONCOMBAT_EVENT_DATA::Clear() 
{ 
  GameEvent::Clear(); 
  m_pCaster=NULL;
}

void TARGET_SELECT_NONCOMBAT_EVENT_DATA::OnInitialEvent(void)
{
  if (m_pCaster==NULL)
  {
    PopEvent();
    return;
  }
  if (!m_pCaster->targets.ValidNumTargets())
  {
    WriteDebugString("ValidNumTargets() failed in TARGET_SELECT_NONCOMBAT_EVENT_DATA init\n");
    PopEvent();
    return;
  }
  menu.setMenu(TargetSelectNonCombatMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  CString title(m_pCaster->targets.FormatRemainingTargsText());
  menu.setTitle(title);
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 2);
  Invalidate();
}

void TARGET_SELECT_NONCOMBAT_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (TABParty(key)) return;
  if (key != KC_RETURN) 
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  }
  CHARACTER &dude = GetActiveChar(this); // selected target character

  switch (menu.currentItem())
  {
  case 1: // select
    {
      if (m_pCaster != NULL)
      {
        if (!m_pCaster->AddTarget(dude))
          WriteDebugString("Failed AddTarget() in TARGET_SELECT_NONCOMBAT_EVENT_DATA\n");

        if (m_pCaster->targets.AllTargetsChosen())
        {
          PopEvent(); // all targets chosen
          return;
        }
        CString title(m_pCaster->targets.FormatRemainingTargsText());
        menu.setTitle(title);
      }
    }
    break;
  case 2: // exit
    PopEvent();
    break;
  }
}

void TARGET_SELECT_NONCOMBAT_EVENT_DATA::OnDraw(void)
{
  UpdateTargetSelectNonCombatScreen('W');
}

void TARGET_SELECT_NONCOMBAT_EVENT_DATA::OnUpdateUI(void)
{
  menu.setAllItemsActive();
  if (m_pCaster == NULL)
  {
    menu.setItemInactive(1); // select
  }
  else
  {
    if (!m_pCaster->targets.ValidNumTargets())
      menu.setItemInactive(1); // select
    if (m_pCaster->targets.AllTargetsChosen())
      menu.setItemInactive(1); // select
  }
}


////////////////////////////////////////////////////////////////////////TARGET_SELECT_COMBAT_EVENT_DATA
/*
TARGET_SELECT_COMBAT_EVENT_DATA::TARGET_SELECT_COMBAT_EVENT_DATA(GameEvent *pEvent,COMBATANT *data, int key) : GameEvent()
{
  Clear();   
  event=CONTROL_TargetSelectCombat; 
  m_pCaster=data; 
  m_pOrigEvent=pEvent;
  m_cspellKey=key; // spell database key
  if (m_pCaster == NULL) 
    PopEvent();
}

TARGET_SELECT_COMBAT_EVENT_DATA::~TARGET_SELECT_COMBAT_EVENT_DATA()
{ 
  Clear(); 
}

void TARGET_SELECT_COMBAT_EVENT_DATA::Clear() 
{ 
  GameEvent::Clear(); 
  m_pCaster=NULL;
}

void TARGET_SELECT_COMBAT_EVENT_DATA::OnInitialEvent(void)
{
  if (m_pCaster==NULL)
  {
    PopEvent();
    return;
  }
  if (!m_pCaster->targets.ValidNumTargets())
  {
    WriteDebugString("ValidNumTargets() failed in TARGET_SELECT_COMBAT_EVENT_DATA init\n");
    PopEvent();
    return;
  }

  if (combatData.OnAuto())
  {
    //m_pCaster->AutoChooseSpellTargets();
    PopEvent();
    return;
  }

  menu.setMenu(TargetSelectNonCombatMenuData);
  menu.setHorzOrient();
  CString title(m_pCaster->targets.FormatRemainingTargsText());
  menu.setTitle(title);
}

void TARGET_SELECT_COMBAT_EVENT_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN) 
  {
    HMenuVPartyKeyboardAction(key, ascii);
    return;
  }

  COMBATANT &dude = combatData.getActiveCombatCharacter(); // selected target character

  switch (menu.currentItem())
  {
  case 1: // select
    {
      if (m_pCaster != NULL)
      {
        int targrange = 0;
        if (!m_pCaster->AddTarget(dude, targrange))
          WriteDebugString("Failed AddTarget() in TARGET_SELECT_COMBAT_EVENT_DATA\n");

        if (m_pCaster->targets.AllTargetsChosen())
        {
          PopEvent(); // all targets chosen
          return;
        }
        CString title(m_pCaster->targets.FormatRemainingTargsText());
        menu.setTitle(title);
      }
    }
    break;
  case 2: // exit
    PopEvent();
    break;
  }
}

void TARGET_SELECT_COMBAT_EVENT_DATA::OnDraw(void)
{
  //UpdateTargetSelectCombatScreen();
}

void TARGET_SELECT_COMBAT_EVENT_DATA::OnUpdateUI(void)
{
  menu.setAllItemsActive();
  if (m_pCaster == NULL)
  {
    menu.setItemInactive(1); // select
  }
  else
  {
    if (!m_pCaster->targets.ValidNumTargets())
      menu.setItemInactive(1); // select
    if (m_pCaster->targets.AllTargetsChosen())
      menu.setItemInactive(1); // select
  }
}
*/








////////////////////////////////////////////////////////////////////////CAST_MENU_DATA
void CHOOSE_FROM_LIST_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "SELECT", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
  */
  switch (menu.currentItem()) 
  {
  case 1: // "SELECT"
    {
      *this->m_pResult = lineListText.GetLineLine(currLineTextIndex());
      PopEvent();
    };
    break;    
  case 2: // "NEXT"
    nextLinePage(NULL);
    break;    
  case 3: // "PREV"
    prevLinePage(NULL);
    break;    
  default:
    WriteDebugString("Unhandled menu item in CHOOSE_FROM_LIST_MENU_DATA");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  }  
}

void CHOOSE_FROM_LIST_MENU_DATA::OnInitialEvent(void)
{
  CString choice;
  ClearFormattedText(textData);
  menu.setMenu(SelectFromListMenuData, NULL, FALSE, this, "Choose");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  SetMyState(TASK_CastSpellSpellSelect);
  //setSpellCastItemState(GetActiveChar(m_pOrigEvent), this->m_castingEnvironment);
  //handleSpellFormInput(SPELLFORM_exit, spellListText);
  handleLineFormInput(LINEFORM_exit, lineListText);
  lineListText.Clear();
  lineListText.UseSimpleLayout();
  lineListText.title = "Please Choose One";
  while (!m_list.IsEmpty())
  {
    int len;
    char delimiter;
    len = m_list.GetLength();
    delimiter = m_list[0];
    {
      int end;
      end = m_list.Find(delimiter,1);
      if (end < 0) end = len;
      choice = m_list.Mid(1, end-1);
      m_list = m_list.Right(len-end);
      lineListText.Add(choice);
    };
  };

  int cnt = lineListText.GetCount();
  ItemsOnPage = min(Items_Per_Page, cnt);
  /*
  if (ItemsOnPage > 0)
  {
     if (party.activeItem >= lineListText.GetCount())
       party.activeItem = (BYTE)(lineListText.GetCount()-1);

     if (party.activeItem >= (startItem+ItemsOnPage))
       party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);

     if (party.activeItem < 0)
       party.activeItem = 0;
  }
  else
  */
  {
    party.activeItem = 0;
    startItem = 0;
    //currShopItem = 0;
  }

  handleLineFormInput(LINEFORM_initial, lineListText);
  if (lineListText.GetCount() == 0) 
  {
    PopEvent();
    return;
  };
  Invalidate();
}


void CHOOSE_FROM_LIST_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  if (errorTextData.numLines == 0)
    handleLineFormInput(LINEFORM_refresh, lineListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void CHOOSE_FROM_LIST_MENU_DATA::OnUpdateUI(void)
{
  /*
   { "SELECT", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
  */
  menu.setAllItemsActive();
  int zone = levelData.GetCurrZone(party.Posx,party.Posy);
  if (!levelData.zoneData.zones[zone].AllowMagic)
    menu.setItemInactive(1); // "SELECT"

  if (spellListText.GetCount() <= 0)
    menu.setItemInactive(1); // "SELECT"
    
  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // "NEXT"

  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // "PREV"
}


////////////////////////////////////////////////////////////////////////CAST_MENU_DATA
void CAST_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii, NULL, NULL);
    return;
  }
  /*
   { "CAST", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  switch (menu.currentItem()) 
  {
  case 1: // cast
    {
      //m_cspell = spellListText.GetSpellBookKey(currSpellTextIndex());
      m_cspellID = spellListText.GetSpellBookID(currSpellTextIndex());
      if (GetActiveChar(m_pOrigEvent).IsMemorized(m_cspellID))
      {
        if ((m_pOrigEvent != NULL) && (m_pOrigEvent->GetEventType()== /*(eventType) */Combat))
        {
          {
            HOOK_PARAMETERS hookParameters;
            SCRIPT_CONTEXT scriptContext;
            SPELL_DATA *pSpell;
            CString result;
            //pSpell = spellData.GetSpellData(m_cspell);
            pSpell = spellData.GetSpell(m_cspellID);
            result = pSpell->RunSpellScripts(
                                 OVERRIDE_SPELL_SELECTION,
                                 ScriptCallback_RunAllScripts, 
                                 NULL,
                                 "Player selected \"CAST\"");
            if (!result.IsEmpty())
            {
              m_choosingSpellFromList = true;
              PushEvent(new CHOOSE_FROM_LIST_MENU_DATA(result, &m_chooseFromListResult), DeleteEvent);
              break;
            };
          };
          //combatData.getActiveCombatCharacter().SetSpellKeyBeingCast(m_cspell, GLOBAL_SPELL_ID());
          combatData.getActiveCombatCharacter().SetSpellIDBeingCast(m_cspellID, SPELL_ID());
          PopEvent();
        }
        else
          //PushEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(), m_pOrigEvent,&GetActiveChar(m_pOrigEvent), m_cspell), DeleteEvent);
          PushEvent(new CAST_NON_COMBAT_SPELL_MENU_DATA(GetEventType(), 
                                                        m_pOrigEvent,
                                                        &GetActiveChar(m_pOrigEvent),
                                                        m_cspellID,
                                                        false), 
                                                             DeleteEvent);
      }
    }
    break;    
  case 2: // next
    nextSpellPage(NULL);
    break;    
  case 3: // prev
    prevSpellPage(NULL);
    break;    
  case 4: // Exit
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;    
  default:
    WriteDebugString("Unhandled menu item in CAST_MENU_DATA");
    miscError = InvalidMenuState;
    handleSpellFormInput(SPELLFORM_exit, spellListText);
    PopEvent();
    break;
  }  
}



void CAST_MENU_DATA::OnInitialEvent(void)
{
  if (!GetActiveChar(m_pOrigEvent).CanCastSpells())
  {
    PopEvent();
    return;
  }
  ClearFormattedText(textData);
  menu.setMenu(CastMenuData, NULL, FALSE, this, "Cast");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 4);
  SetMyState(TASK_CastSpellSpellSelect);
  setSpellCastItemState(GetActiveChar(m_pOrigEvent), this->m_castingEnvironment);
  handleSpellFormInput(SPELLFORM_exit, spellListText);
  handleSpellFormInput(SPELLFORM_initial, spellListText);
#ifdef SpellMana
  spellListText.memorizedText = "104 AVAIL";
#endif
  Invalidate();
}

void CAST_MENU_DATA::OnReturnToTopOfQueue()
{
  if (m_choosingSpellFromList)
  {
    CString spellName;
    //GLOBAL_SPELL_ID gsID;
    SPELL_ID spellID;
    spellName = m_chooseFromListResult;
    m_choosingSpellFromList = false;
    if (!spellName.IsEmpty())
    {
      //gsID = spellData.FindUniqueName(spellName);
      spellID = spellName;
      //if (gsID.IsValidSpell())
      if (spellID.IsValidSpell())
      {
        //combatData.getActiveCombatCharacter().SetSpellKeyBeingCast(m_cspell, gsID);
        combatData.getActiveCombatCharacter().SetSpellIDBeingCast(m_cspellID, spellID);
        PopEvent();
        return;
      };
    }
    //gsID = spellListText.GetSpellBookKey(currSpellTextIndex());
    spellID = spellListText.GetSpellBookID(currSpellTextIndex());
    //combatData.getActiveCombatCharacter().SetSpellKeyBeingCast(gsID, GLOBAL_SPELL_ID());
    combatData.getActiveCombatCharacter().SetSpellIDBeingCast(spellID, SPELL_ID());
    PopEvent();
    return;
  }
  OnInitialEvent();
}

void CAST_MENU_DATA::OnDraw(void)
{
  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
  drawBorder();
  if (errorTextData.numLines == 0)
    handleSpellFormInput(SPELLFORM_refresh, spellListText);
  menu.DisplayMenu(0);
  DisplayTextAndFlip(0);
}

void CAST_MENU_DATA::OnUpdateUI(void)
{
  /*
   { "CAST", 0 },
   { "NEXT", 0 },
   { "PREV", 0 },
   { "EXIT", 1 }
  */
  menu.setAllItemsActive();
  int zone = levelData.GetCurrZone(party.Posx,party.Posy);
  if (!levelData.zoneData.zones[zone].AllowMagic)
    menu.setItemInactive(1); // cast

  if (spellListText.GetIntegerCount() <= 0)
    menu.setItemInactive(1); // cast
    
  if (!allowSpellNextPage(NULL))
    menu.setItemInactive(2); // next

  if (!allowSpellPrevPage(NULL))
    menu.setItemInactive(3); // prev
}

////////////////////////////////////////////////////////////////////////CAST_NON_COMBAT_SPELL_MENU_DATA
//
// When not in combat, casting and targeting handled and
// then the actual casting takes place. There is no
// need for spell animations (just the casting sound)
//
void CAST_NON_COMBAT_SPELL_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.reset(NULL);

  // selecting spell targets will change party.activeCharacter
  tempActive=party.activeCharacter;
  if (m_pCaster==NULL)
  {
    WriteDebugString("NULL caster in CAST_NON_COMBAT_SPELL_MENU_DATA\n");
    PopEvent();
    return;
  }

  //CHARACTER_SPELL spell;
  //if (!m_pCaster->GetSpell(m_cspellKey, spell))
  //{
  //  WriteDebugString("bogus cspell key %u in CAST_NON_COMBAT_SPELL_MENU_DATA\n", m_cspellKey);
  //  PopEvent();
  //  return;
  //}
  //m_spellKey = spell.spell;
  //if (!m_spellKey.IsValidSpell())
  if (!m_spellID.IsValidSpell())
  {
    //WriteDebugString("bogus cspell key %s in CAST_NON_COMBAT_SPELL_MENU_DATA\n", m_cspellID.UniqueName());
    WriteDebugString("bogus cspell key %s in CAST_NON_COMBAT_SPELL_MENU_DATA\n", m_spellID.UniqueName());
    PopEvent();
    return;
  }

  //if (!spellData.HaveSpell(m_spellKey))
  if (!spellData.HaveSpell(m_spellID))
  {
    //WriteDebugString("bogus spell key %u in CAST_NON_COMBAT_SPELL_MENU_DATA\n", m_spellKey);
    WriteDebugString("bogus spell key %s in CAST_NON_COMBAT_SPELL_MENU_DATA\n", m_spellID.UniqueName());
    PopEvent();
    return;    
  }

  //SPELL_DATA *pSpData = spellData.GetSpellData(m_spellKey);
  SPELL_DATA *pSpData = spellData.GetSpell(m_spellID);

  if (pSpData==NULL)
  {
    WriteDebugString("NULL ptr to spell db data in CAST_NON_COMBAT_SPELL_MENU_DATA\n");
    PopEvent();
    return;    
  }

  //if (pSpData->NotInCamp())
  if (!pSpData->InCamp())
  {
    WriteDebugString("combat only spell %s in CAST_NON_COMBAT_SPELL_MENU_DATA\n", pSpData->Name);
    PopEvent();
    return;    
  }
  int casterLevel = -1;
    { 
      CString result;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetSpellContext(pSpData);
      scriptContext.SetAttackerContext(m_pCaster);
      //hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
      result = pSpData->RunSpellScripts(
                               SPELL_CASTER_LEVEL,
                               ScriptCallback_RunAllScripts, 
                               NULL,
                               "Casting a non-combat spell");
      if (result.GetLength()> 0)
      {
        sscanf(result,"%d",&casterLevel);
      };
    };


  double result;
  //result = m_pCaster->EvalDiceAsClass(pSpData->NumTargets, pSpData->schoolMask, NULL);
  result = m_pCaster->EvalDiceAsClass(pSpData->TargetQuantity(), 
                                      pSpData->schoolID, 
                                      pSpData->Level,
                                      &casterLevel);
  DWORD targs = result;
  //result = m_pCaster->EvalDiceAsClass(pSpData->TargetRange, pSpData->schoolMask, NULL);
  result = m_pCaster->EvalDiceAsClass(pSpData->TargetRange(), 
                                      pSpData->schoolID, 
                                      pSpData->Level,
                                      &casterLevel);
  DWORD range = result;

  m_pCaster->m_spellCastingLevel = casterLevel;
  m_pCaster->InitTargeting(pSpData->Targeting, 
                            pSpData->CanTargetFriend, 
                            pSpData->CanTargetEnemy, 
                            m_pCaster->IsPartyMember(),
                            targs, 
                            range, 
                            1, // width 
                            1, // height
                            pSpData->Lingers);

  if (NeedSpellTargeting(pSpData))
  {
    if (targs == 0) WriteDebugString("Spell %s targets eval to 0 in CAST_NON_COMBAT_SPELL_MENU_DATA\n", pSpData->Name);
    PushEvent(new TARGET_SELECT_NONCOMBAT_EVENT_DATA(m_pCaster), DeleteEvent);
    return;
  }
  else
  {        
    if (pSpData->Targeting == Self)
    {          
      if (!m_pCaster->AddTargetSelf())
        WriteDebugString("Failed AddTargetSelf() in CAST_NON_COMBAT_SPELL_MENU_DATA\n");
    }
    else
    {
      // can be whole party, or area if combat not active
      for (int i=0;i<party.numCharacters;i++)
      {
        if (!m_pCaster->AddTarget(party.characters[i]))
          WriteDebugString("Failed AddTarget(<party member %i>) in CAST_NON_COMBAT_SPELL_MENU_DATA\n", i);
      }
    }
    if (m_pCaster->targets.NumTargets() > 0)
    {
      setPartyXY_x = -1;
      //m_pCaster->CastSpell(m_cspellKey, true);
      m_pCaster->CastSpell(m_spellID, true, m_layOrCureOrWhatever);
      if (setPartyXY_x >= 0)
      {
        party.activeCharacter = tempActive;
        TaskMessage(TASKMSG_SetPartyXY);
        return;
      };
    }
    else
      WriteDebugString("No spell targets in CAST_NON_COMBAT_SPELL_MENU_DATA, not casting spell\n");
  }
  party.activeCharacter = tempActive;
  PopEvent();
}

void CAST_NON_COMBAT_SPELL_MENU_DATA::OnReturnToTopOfQueue()
{
  if ((m_pCaster!=NULL) && (m_pCaster->targets.NumTargets() > 0))
    //m_pCaster->CastSpell(m_cspellKey, false);
    m_pCaster->CastSpell(m_spellID, false, m_layOrCureOrWhatever);
  else
    WriteDebugString("No targets in CAST_NON_COMBAT_SPELL_MENU_DATA after target select menu, not casting\n");
  party.activeCharacter = tempActive;
  PopEvent();
}

void CAST_NON_COMBAT_SPELL_MENU_DATA::OnDraw(void)
{
  switch (m_origEventType)
  {
  case CONTROL_CastMenu:
    {
      GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
      drawBorder();
      if (errorTextData.numLines == 0)
        handleSpellFormInput(SPELLFORM_refresh, spellListText);
      menu.DisplayMenu(0);
      DisplayTextAndFlip(0);
    }
    break;

  case GiveTreasure:
  case CombatTreasure:
    UpdateGiveTreasureScreen('h');
    break;
  }
}

////////////////////////////////////////////////////////////////////////CAST_COMBAT_SPELL_MENU_DATA
// When in combat, spell casting is begun and there is a
// time delay for the casting time. 
//
void CAST_COMBAT_SPELL_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.reset(NULL);
  
  if (m_pCaster==NULL)
  {
    WriteDebugString("NULL caster in CAST_COMBAT_SPELL_MENU_DATA\n");
    PopEvent();
    return;
  }


//
//  The following test prevented non-spellcasters from using
//  magical weapons in combat.  We removed this test 20170322  PRS
//  if (!m_pCaster->CanCastSpells())
//  {
//    PopEvent();
//    return;
//  }

  //CHARACTER_SPELL charSp;
  ////if (m_pCaster->GetSpell(m_cspellKey, spell))
  ////if (m_pCaster->GetSpell(m_cspellID, spell))
  //if (m_pCaster->FetchCharacterSpell(m_spellID, &charSp))
  //{
  //  //m_spellKey = spell.spell;
  //  m_spellID = charSp.spellID;
  //}
  ////else if (m_cspellKey == m_pCaster->GetSecondarySpellKeyBeingCast())
////  else if (m_spellID == m_pCaster->GetSecondarySpellIDBeingCast())
////  {
////    m_spellKey = m_cspellKey;
////  }
////  else
////  {
////    if (m_pCaster->GetItemSpellKeyBeingCast() == m_cspellKey)
////    {
////      m_spellKey = m_cspellKey;
////    }
  //  else
  //  {
  //    //WriteDebugString("bogus cspell key %u in CAST_COMBAT_SPELL_MENU_DATA\n", m_cspellKey);
  //    WriteDebugString("bogus cspell key %s in CAST_COMBAT_SPELL_MENU_DATA\n", m_spellID.UniqueName());
  //    PopEvent();
  //    return;
  //  };
//  }

  //if (!spellData.HaveSpell(m_spellKey))
  if (!spellData.HaveSpell(m_spellID))
  {
    //WriteDebugString("bogus spell key %u in CAST_COMBAT_SPELL_MENU_DATA\n", m_spellKey);
    WriteDebugString("bogus spell key %s in CAST_COMBAT_SPELL_MENU_DATA\n", m_spellID.UniqueName());
    PopEvent();
    return;    
  }

  //SPELL_DATA *pSpData = spellData.GetSpellData(m_spellKey);
  SPELL_DATA *pSpData = spellData.GetSpell(m_spellID);

  if (pSpData==NULL)
  {
    WriteDebugString("NULL ptr to spell db data in CAST_COMBAT_SPELL_MENU_DATA\n");
    PopEvent();
    return;    
  }

  DWORD targs;
  DWORD range;
  DWORD width, height;
  double val;
  int casterLevel = -1;
  {

    { 
      CString result;
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;
      scriptContext.SetSpellContext(pSpData);
      scriptContext.SetAttackerContext(m_pCaster);
      hookParameters[5] = m_pCaster->GetSecondarySpellIDBeingCast();
      result = pSpData->RunSpellScripts(
                                SPELL_CASTER_LEVEL, 
                                ScriptCallback_RunAllScripts,
                                NULL,
                                "Casting a spell in combat");
      if (result.GetLength()> 0)
      {
        sscanf(result,"%d",&casterLevel);
      };
    };

    
    switch(pSpData->Targeting)
    {
      case Self:
        val = 1;
        break;
      case AreaSquare:
        val = 999999;
        break;
      case AreaCone:
      case AreaCircle:
      case AreaLinePickStart:
        val = 999999;
        break;
      default:
        //val = m_pCaster->EvalDiceAsClass(pSpData->NumTargets, pSpData->schoolMask, &casterLevel);
        /* Really */ NotImplemented(0x20abc, false);
      case TouchedTargets:
        //val = 8;
        //break;
      case SelectedByCount:
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetQuantity(), 
                                     pSpData->schoolID, 
                                     pSpData->Level,
                                     &casterLevel);
        break;
    };
    targs = val;
    switch(pSpData->Targeting)
    {
    case AreaSquare:
      {
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetWidth(), 
                                         pSpData->schoolID, 
                                         pSpData->Level,
                                         &casterLevel);
        width = val;
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetHeight(), 
                                         pSpData->schoolID, 
                                         pSpData->Level,
                                         &casterLevel);
        height = val;
      };
      break;
    case AreaCone:
      {
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetWidth(), 
                                         pSpData->schoolID, 
                                         pSpData->Level,
                                         &casterLevel);
        width = val;
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetHeight(), 
                                         pSpData->schoolID, 
                                         pSpData->Level,
                                         &casterLevel);
        height = val;
      };
      break;
    case AreaLinePickStart:
    case AreaCircle:
      {
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetWidth(), 
                                         pSpData->schoolID, 
                                         pSpData->Level,
                                         &casterLevel);
        width = val;
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetHeight(), 
                                         pSpData->schoolID, 
                                         pSpData->Level,
                                         &casterLevel);
        height = val;
      };
      break;
    case SelectedByCount:
      width = 999999;
      height = 999999;
      break;
    case TouchedTargets:
      width = 3;
      height = 3;
      break;
    case Self:
      width = 1;
      height = 1;
      break;
    default:
      /* Really */ NotImplemented(0x551c0, false);
      width  = 1;
      height = 1;
      { 
        CString result;
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;
        scriptContext.SetSpellContext(pSpData);
        scriptContext.SetAttackerContext(m_pCaster);
        hookParameters[5].Format("%d", int(val));
        result = pSpData->RunSpellScripts(
                            SPELL_NUMTARGETS, 
                            ScriptCallback_RunAllScripts,
                            NULL,
                            "Casting a spell in combat");
        if (result.GetLength()> 0)
        {
          sscanf(result,"%lf",&val);
        };
      };
    };


    switch (pSpData->Targeting)
    {
    default:
      /* Really */ NotImplemented(0x231bb, false);
      break;
    case Self:
      range = 1;
      break;
    case AreaCone:
    case AreaSquare:
    case AreaCircle:
    case AreaLinePickStart:
    case SelectedByCount:
      { 
        //val = m_pCaster->EvalDiceAsClass(pSpData->TargetRange, pSpData->schoolMask, &casterLevel); 
        val = m_pCaster->EvalDiceAsClass(pSpData->TargetRange(), 
                                     pSpData->schoolID, 
                                     pSpData->Level,
                                     &casterLevel); 
        {
          CString result;
          HOOK_PARAMETERS hookParameters;
          SCRIPT_CONTEXT scriptContext;
          scriptContext.SetSpellContext(pSpData);
          scriptContext.SetAttackerContext(m_pCaster);
          hookParameters[5].Format("%d", int(val));
          result = pSpData->RunSpellScripts(
                                 SPELL_RANGE, 
                                 ScriptCallback_RunAllScripts,
                                 NULL,
                                 "Casting a spell in combat");
          if (result.GetLength()> 0)
          {
            sscanf(result,"%lf",&val);
          };
        };
      };
      break;
    case TouchedTargets:
      val = 0;
      break;
    };
    range = val;
  };

  m_pCaster->m_pCharacter->m_spellCastingLevel = casterLevel;

  m_pCaster->InitTargeting(pSpData->Targeting, 
                           pSpData->CanTargetFriend,
                           pSpData->CanTargetEnemy,
                           m_pCaster->GetIsFriendly(),
                           targs, range, width, height, pSpData->Lingers);

  if (NeedSpellTargeting(pSpData))
  {




    /* ********************** 20200204 PRS 
    if (targs == 0) WriteDebugString("Spell %s targets eval to 0 in CAST_COMBAT_SPELL_MENU_DATA\n", pSpData->Name);
      ReplaceEvent(new COMBAT_SPELL_AIM_MENU_DATA(m_pCaster,pSpData), DeleteEvent); 
      return;
      */

    if (targs == 0)
    {
      WriteDebugString("Spell %s targets eval to 0 in CAST_COMBAT_SPELL_MENU_DATA\n", pSpData->Name);
      ReplaceEvent(new COMBAT_SPELL_AIM_MENU_DATA(m_pCaster, pSpData), DeleteEvent);
      return;
    };
    
      
      
      
  }
  else // whole party or self targeting
  {        
    if (pSpData->Targeting == Self)
    {          
      if (!m_pCaster->AddTargetSelf())
        WriteDebugString("Failed AddTargetSelf() in CAST_COMBAT_SPELL_MENU_DATA\n");
    }
    else
    {
      // can only be whole party
      for (int i=0; i<combatData.NumCombatants(); i++)
      {
        if (combatData.GetCombatant(i)->IsPartyMember())
        {
          if (!m_pCaster->C_AddTarget(*combatData.GetCombatant(i)))
            WriteDebugString("Failed AddTarget(<party member %i>) in CAST_COMBAT_SPELL_MENU_DATA\n", i);
        }
      }
    }
  }
  PopEvent();
}

void CAST_COMBAT_SPELL_MENU_DATA::OnReturnToTopOfQueue()
{
  die(0xab4f3);
}

void CAST_COMBAT_SPELL_MENU_DATA::OnDraw(void)
{
    UpdateScribeMenuScreen('V');
}


BOOL CheckScribable(ITEM *pItem, void *pkt)
{
  if (pItem == NULL) return FALSE;
  if (pItem->charges == 0) return FALSE;
  ITEM_DATA *pItemData;
  //GLOBAL_ITEM_ID giID;
  //giID = pItem->m_giID;
  ITEM_ID itemID;
  itemID = pItem->itemID;
  //if (!giID.IsValidItem()) return FALSE;
  if (!itemID.IsValidItem()) return FALSE;
  //pItemData = itemData.GetItemData(giID);
  pItemData = itemData.GetItem(itemID);
  if (pItemData == NULL) return FALSE;
  return pItemData->IsScribable();
}

////////////////////////////////////////////////////////////////////////SCRIBE_MENU_DATA

void SCRIBE_MENU_DATA::OnInitialEvent(void)
{
  //if (!GetActiveChar(m_pOrigEvent).canCastSpells())
  //{
  //  PopEvent();
  //  return;
  //}
  //ClearFormattedText(textData);
  //menu.setMenu(EmptyMenuData);
  //m_giID_scroll.Clear();
  m_itemID_scroll.Clear();
  PushEvent(new ITEMS_MENU_DATA(CONTROL_ScribeMenu, this, CheckScribable, NULL, 0), DeleteEvent);
}

void SCRIBE_MENU_DATA::OnDraw(void)
{
  UpdateScribeMenuScreen('V');
}

void SCRIBE_MENU_DATA::OnKeypress(key_code, char)
{
  PopEvent();
}

void SCRIBE_MENU_DATA::OnUpdateUI(void)
{
}

void SCRIBE_MENU_DATA::OnReturnToTopOfQueue(void)
{
  //if (m_giID_scroll.IsValidItem())
  if (m_itemID_scroll.IsValidItem())
  {
    ITEM_DATA *pScroll;
    //pScroll = itemData.GetItemData(m_giID_scroll);
    pScroll = itemData.GetItem(m_itemID_scroll);
    //ASSERT (pScroll->m_gsID.IsValidSpell());
    ASSERT (pScroll->spellID.IsValidSpell());
    //if (pScroll->m_gsID.IsValidSpell())
    if (pScroll->spellID.IsValidSpell())
    {
      //WORD casterClassFlag, spellClassFlag;
      SPELL_ID spellSchoolID;
      const BASECLASS_LIST *pBaseclassList;
      int spellLevel;
      //spellClassFlag = spellData.GetSpellSchool(pScroll->m_gsID);
      //spellLevel = spellData.GetSpellLevel(pScroll->m_gsID);
      //spellClassFlag = spellData.GetSpellSchool(pScroll->spellID);
      spellSchoolID = spellData.GetSpellSchool(pScroll->spellID);
      spellLevel = spellData.GetSpellLevel(pScroll->spellID);
      //casterClassFlag = SetClassBitsFromCharClass(m_pDude->GetClass());
      pBaseclassList = classData.PeekClass(m_pDude->GetClass())->BaseclassList();

      //m_pDude->KnowSpell(casterClassFlag,spellClassFlag, pScroll->m_gsID,spellLevel,TRUE);


/*
      m_pDude->KnowSpell(casterClassFlag,spellClassFlag, pScroll->spellID,spellLevel,TRUE);
*/ /* Really */ NotImplemented(0x4d67ab9, false);


    };
  };
  PopEvent();
}



////////////////////////////////////////////////////////////////////////MAGIC_MENU_DATA
void MAGIC_MENU_DATA::OnKeypress(key_code key, char ascii)
{  
   //1 { "CAST", 0 },
   //2 { "MEMORIZE", 0 },
   //3 { "SCRIBE", 0 },
   //4 { "DISPLAY", 0 },
   //5 { "REST", 0 },
   //6 { "EXIT", 1 }
  if (key != KC_RETURN) 
  {
    if (TABParty(key)) return;
    //StandardMenuKeyboardAction(key,ascii);
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  };

  switch (menu.currentItem()) 
  {
  case 1: // cast
    PushEvent(new CAST_MENU_DATA(m_pOrigEvent, CAST_ENV_CAMP), DeleteEvent);
    break;    
  case 2: // memorize
    PushEvent(new MEMORIZE_MENU_DATA, DeleteEvent);
    break;    
  case 3: // scribe
    if (   (m_pOrigEvent == NULL) 
        || (m_pOrigEvent->GetEventType() != Combat))
    {
      CHARACTER  *pCharacter;
      CLASS_DATA *pClass;
      pCharacter = &GetActiveChar(m_pOrigEvent);
      {
  
        ActorType actor;
        pClass = classData.GetClass(pCharacter->GetClass());
        HOOK_PARAMETERS hookParameters;
        SCRIPT_CONTEXT scriptContext;

        pCharacter->GetContext(&actor);
        SetCharContext(&actor);
        scriptContext.SetCharacterContext(pCharacter);
        scriptContext.SetClassContext(pClass);
        pCharacter->RunCharacterScripts(
                          SCRIBE_OR_WHATEVER, 
                          ScriptCallback_RunAllScripts,
                          "",
                          "Player selected 'SCRIBE' (or whatever)");
        pClass->RunClassScripts(
                          SCRIBE_OR_WHATEVER, 
                          ScriptCallback_RunAllScripts, 
                          "",
                          "Player selected 'SCRIBE' (or whatever)");
        ClearCharContext();
        if (hookParameters[5].CompareNoCase("scribe") == 0)
        {
          PushEvent(new SCRIBE_MENU_DATA(&GetActiveChar()), DeleteEvent);
        }
        else
        {
          break;
        };
      };
  
    }
    else
    {
      PushEvent(new SCRIBE_MENU_DATA(&GetActiveChar()), DeleteEvent);
    };
    break;    
  case 4: // display
    PushEvent(new DISPLAY_SPELL_EFFECTS_MENU_DATA(m_pOrigEvent), DeleteEvent);
    break;
  case 5: // rest
    PushEvent(new REST_MENU_DATA(m_pOrigEvent), DeleteEvent);
    break;
  case 6: // exit
    PopEvent();
    break;
  default:
    WriteDebugString("Unhandled menu item in MAGIC_MENU_DATA");
    miscError = InvalidMenuState;
    PopEvent();
    break;
  }  
}

void MAGIC_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(MagicMenuData, NULL, FALSE, this, "Magic");
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 6);

  
  Invalidate();
}

void MAGIC_MENU_DATA::OnDraw(void)
{
  if (   (m_pOrigEvent != NULL) 
      && (m_pOrigEvent->GetEventType() == Combat))
  {
    UpdateCombatMagicMenuScreen();
  }
  else
  {
    UpdateMagicMenuScreen('U');
  }
}

void MAGIC_MENU_DATA::OnUpdateUI(void)
{
   //1 { "CAST", 0 },
   //2 { "MEMORIZE", 0 },
   //3 { "SCRIBE", 0 },
   //4 { "DISPLAY", 0 },
   //5 { "REST", 0 },
   //6 { "EXIT", 1 }

  int zone = levelData.GetCurrZone(party.Posx, party.Posy);

  if (   (m_pOrigEvent == NULL) 
      || (m_pOrigEvent->GetEventType() != Combat))
  {
    // Determine if the "SCRIBE" menu selection should appear.
    CHARACTER  *pCharacter;
    CLASS_DATA *pClass;
    pCharacter = &GetActiveChar(m_pOrigEvent);
    if (pCharacter != m_pActiveCharacter) // Avoids excessive script activations
    {
      ActorType actor;
      m_pActiveCharacter = pCharacter;

      pClass = classData.GetClass(pCharacter->GetClass());
      HOOK_PARAMETERS hookParameters;
      SCRIPT_CONTEXT scriptContext;

      pCharacter->GetContext(&actor);
      SetCharContext(&actor);
      scriptContext.SetCharacterContext(pCharacter);
      scriptContext.SetClassContext(pClass);
      pCharacter->RunCharacterScripts(
                                CAN_SCRIBE_OR_WHATEVER, 
                                ScriptCallback_RunAllScripts, 
                                "",
                                "Should we display 'SCRIBE' (or whatever) menu item");
      pClass->RunClassScripts(
                                CAN_SCRIBE_OR_WHATEVER, 
                                ScriptCallback_RunAllScripts, 
                                "",
                                "Should we display 'SCRIBE' (or whatever) menu item");
      ClearCharContext();
      this->m_scribeOrWhateverMenuText = hookParameters[5];
      sscanf(hookParameters[6],"%d",&m_scribeOrWhateverMenuShortcut);
    };
  }; 
  
  menu.setAllItemsActive();

  if (!levelData.zoneData.zones[zone].AllowMagic)
  {
    menu.setItemInactive(1);  // "CAST"
    menu.setItemInactive(2);  // "MEMORIZE"
    menu.setItemInactive(3);  // "SCRIBE"
    menu.setItemInactive(5);  // "REST"
  }
  
  if (!levelData.zoneData.zones[zone].restEvent.allowResting)
  {
    menu.setItemInactive(5); // "REST"
  }

  if (   (m_pOrigEvent != NULL) 
      && (m_pOrigEvent->GetEventType() == Combat))
  {
    menu.setItemInactive(2); // "MEMORIZE"
    menu.setItemInactive(3); // "SCRIBE"
    menu.setItemInactive(5); // "REST"
  }
  else
  {
    if (m_scribeOrWhateverMenuText.IsEmpty())
    {
      menu.setItemInactive(3);  // "SCRIBE" or whatever
    }
    else
    {
      menu.changeMenuItem(3, m_scribeOrWhateverMenuText, m_scribeOrWhateverMenuShortcut);
      menu.setItemActive(3);
    };

    if (!GetActiveChar(m_pOrigEvent).CanCastSpells()) 
    {
      menu.setItemInactive(1); // "CAST"
    }

    if (!GetActiveChar(m_pOrigEvent).CanMemorizeSpells(0)) // Should "MEMORIZE" appear on MAGIC menu
    {
      menu.setItemInactive(2); // "MEMORIZE"
    }

  }
}

void MAGIC_MENU_DATA::OnReturnToTopOfQueue()
{
  OnInitialEvent();
}


////////////////////////////////////////////////////////////////////////APPRAISE_SELECT_DATA
void APPRAISE_SELECT_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key,ascii);
    return;
  }
  /*
   { "GEMS", 0 },
   { "JEWELRY", 0 },
   { "EXIT", 1 }
  */
  int value;
  switch (menu.currentItem()) 
  {
  case 1: // Gems
    m_pMoney->RemoveMultGems(1);
    value = globalData.moneyData.GetGemValue();
    PushEvent(new APPRAISE_EVALUATE_DATA(GemType, globalData.moneyData.GetName(GemType), value), DeleteEvent);    
    break;    
  case 2: // Jewelry
    m_pMoney->RemoveMultJewelry(1);
    value = globalData.moneyData.GetJewelryValue();
    PushEvent(new APPRAISE_EVALUATE_DATA(JewelryType,globalData.moneyData.GetName(JewelryType), value), DeleteEvent);    
    break;    
  case 3: // Exit
    PopEvent();
    break;    
  default:
    WriteDebugString("Unhandled menu item in APPRAISE_SELECT_DATA");
    miscError = InvalidMenuState;
    PopEvent();
    break;
  } 
}

void APPRAISE_SELECT_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(AppraiseSelectMenuData, NULL, FALSE, this, "Appraise");
  menu.changeMenuItem(1, globalData.moneyData.GetName(GemType));
  menu.changeMenuItem(2, globalData.moneyData.GetName(JewelryType));
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 3);
  CString msg;
  msg.Format("YOU HAVE\r\n%u %s\r\n%u %s\r\nNOT YET APPRAISED",
             m_pMoney->NumGems(),
             globalData.moneyData.GetName(GemType),
             m_pMoney->NumJewelry(),
             globalData.moneyData.GetName(JewelryType));            

  FormatPausedText(textData, msg);//, FALSE, FALSE);
  Invalidate();
}

void APPRAISE_SELECT_DATA::OnDraw(void)
{
  UpdateAppraiseSelectMenuScreen();
}

void APPRAISE_SELECT_DATA::OnUpdateUI(void)
{
  /*
   { "GEMS", 0 },
   { "JEWELRY", 0 },
   { "EXIT", 1 }
  */   
  menu.setAllItemsActive();

  if (m_apprGems==FALSE)
    menu.setItemInactive(1); // gems
  if (m_pMoney->NumGems()==0)
    menu.setItemInactive(1); // gems

  if (m_apprJewelry==FALSE)
    menu.setItemInactive(2); // jewelry
  if (m_pMoney->NumJewelry()==0)
    menu.setItemInactive(2); // jewelry
}

void APPRAISE_SELECT_DATA::OnReturnToTopOfQueue(void)
{
  OnInitialEvent();
  if (party.tradeItem==1) // sell
  {
    GetActiveChar(this).money.Add(globalData.moneyData.GetDefaultType(), 
                                  party.tradeQty);
  }
  else // keep
  {
    itemClassType type = (itemClassType)party.tradeGiver;
    if (type == GemType)
    {
      //GLOBAL_ITEM_ID giID;
      ITEM_ID itemID;
      //giID.CreateGem();
      itemID.CreateGem(globalData.moneyData.GetGemName());
      //GetActiveChar(this).addCharacterItem(giID,1,party.tradeQty,FALSE,-1);
      GetActiveChar(this).addCharacterItem(itemID,1,party.tradeQty,FALSE,-1);
    }
    else if (type == JewelryType)
    {
      //GLOBAL_ITEM_ID giID;
      ITEM_ID itemID;
      //giID.CreateJewelry();
      // Determine the item type for appraised jewelry
      itemID.CreateJewelry(globalData.moneyData.GetJewelryName());
      //GetActiveChar(this).addCharacterItem(giID,1,party.tradeQty,FALSE,-1);
      GetActiveChar(this).addCharacterItem(itemID,1,party.tradeQty,FALSE,-1);
    }
  }
}

////////////////////////////////////////////////////////////////////////APPRAISE_EVALUATE_DATA
void APPRAISE_EVALUATE_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key,ascii);
    return;
  }
  /*
   { "SELL", 0 },
   { "KEEP", 0 },
  */
  switch (menu.currentItem()) 
  {
  case 1: // Sell
    party.tradeItem=1;
    party.tradeQty = m_value;
    break;    
  case 2: // Keep
    party.tradeItem=0;
    party.tradeQty = m_value;
    break;    
  default:
    WriteDebugString("Unhandled menu item in APPRAISE_EVALUATE_DATA");
    miscError = InvalidMenuState;
    break;
  } 
  PopEvent();
}

void APPRAISE_EVALUATE_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(AppraiseEvaluateMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  party.tradeItem=0;
  party.tradeQty=0;
  party.tradeGiver = m_type;
  CString msg;
  msg.Format("THIS %s IS VALUED AT %u %s",
             m_name,
             m_value,
             globalData.moneyData.GetName(globalData.moneyData.GetDefaultType()));
  FormatDisplayText(textData, msg, FALSE, FALSE);
  Invalidate();
}

void APPRAISE_EVALUATE_DATA::OnDraw(void)
{
  UpdateAppraiseEvaluateMenuScreen();
}

////////////////////////////////////////////////////////////////////////ASK_CUSTOM_MENU_DATA
void ASK_CUSTOM_MENU_DATA::OnKeypress(key_code key, char ascii)
{

  switch (m_OnDrawType)
  {
  case CONTROL_Adventure:
  case ShopEvent:
  case TempleEvent:
  case Vault:
  case GiveTreasure:
  case CONTROL_Encamp:  if (TABParty(key)) return;
  };
  
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  }

  switch (menu.currentItem()) 
  {
  case 1: // option 1
  case 2: // option 2
  case 3: // option 3
  case 4: // option 4
    party.tradeItem=menu.currentItem();
    break;
  default:
    WriteDebugString("Unhandled menu item in ASK_CUSTOM_MENU_DATA");
    miscError = InvalidMenuState;
    break;
  } 

  ClearFormattedText(textData);
  PopEvent();
}

void ASK_CUSTOM_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.reset(NULL);
  menu.setStartCoord(USE_DEFAULT_HORZ_MENU, 0, FALSE);
  menu.setTitle(m_question);
  menu.setHorzOrient();
  //menu.setItemSeparation();
  
  menu.addMenuItem(m_option1);
  if (m_option2.GetLength() > 0)
    menu.addMenuItem(m_option2);
  
  if (m_option3.GetLength() > 0)
    menu.addMenuItem(m_option3);

  if (m_option4.GetLength() > 0)
    menu.addMenuItem(m_option4);
  
  menu.SetFirstLettersShortcuts();
  menu.setCurrentItem(0);
  party.tradeItem=0; 
  Invalidate();
}

void ASK_CUSTOM_MENU_DATA::OnDraw(void)
{
  switch (m_OnDrawType)
  {
  case CONTROL_Adventure: UpdateAdventureScreen(); break;
  case CONTROL_ItemsMenu: 
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    drawBorder();
    if (errorTextData.numLines == 0)
      handleItemsFormInput(ITEMSFORM_refresh, charItemListText);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    break;
  case ShopEvent: UpdateShopScreen('F'); break;
  case TempleEvent: UpdateTempleScreen('K'); break;
  case Vault: UpdateVaultScreen('i'); break;
  case GiveTreasure: UpdateGiveTreasureScreen('h'); break;
  case CONTROL_Encamp: UpdateEncampScreen('g'); break;
  case CONTROL_CombatSpellAimAuto: UpdateCombatAimScreen(); break;

  case CONTROL_CreateCharacter:
  default: UpdateAskYesNoMenuScreen(); break;
  }
}

////////////////////////////////////////////////////////////////////////ASK_YES_NO_MENU_DATA
void ASK_YES_NO_MENU_DATA::OnKeypress(key_code key, char ascii)
{

  switch (m_OnDrawType)
  {
  case CONTROL_Adventure:
  case ShopEvent:
  case TempleEvent:
  case Vault:
  case GiveTreasure:
  case CONTROL_Encamp:  if (TABParty(key)) return;
  };
  
  if (key != KC_RETURN)
  {
    HMenuVPartyKeyboardAction(key,ascii);
    return;
  }

  if (m_IsYesNo)
  {
    /*
     { "YES", 0 },
     { "NO", 0 },
    */
    switch (menu.currentItem()) 
    {
    case 1: // Yes
      party.tradeItem=1;
      break;    
    case 2: // No
      party.tradeItem=0;
      break;    
    default:
      WriteDebugString("Unhandled menu item in ASK_YES_NO_MENU_DATA");
      miscError = InvalidMenuState;
      break;
    } 
  }
  else // 'Press Enter To Continue'
  {
    party.tradeItem=0;
  }

  ClearFormattedText(textData);
  PopEvent();
}

void ASK_YES_NO_MENU_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  if (m_IsYesNo)
    menu.setMenu(QuestionYesNoData, NULL, FALSE, this, "AskYesNo");
  else
    menu.setMenu(PressEnterMenuData, NULL, FALSE, this, "AskYesNo");

  menu.setHorzOrient();
  party.tradeItem=0;  

  switch (m_OnDrawType)
  {
  case CONTROL_CombatMove:
  case CONTROL_CombatSpellAimAuto:
//  case CONTROL_Adventure:     // Added 22 May 2009 PRS for 'ENCAMP/ALTER/MUSIC'
    menu.setTitle(m_question);
    break;

  default: FormatDisplayText(textData, m_question, FALSE); break;
  };
  Invalidate();
}

void ASK_YES_NO_MENU_DATA::OnDraw(void)
{
  switch (m_OnDrawType)
  {
  case CONTROL_Adventure: UpdateAdventureScreen(); break;
  case CONTROL_ItemsMenu: 
    GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);
    drawBorder();
    if (errorTextData.numLines == 0)
      handleItemsFormInput(ITEMSFORM_refresh, charItemListText);
    menu.DisplayMenu(0);
    DisplayTextAndFlip(0);
    break;
  case ShopEvent: UpdateShopScreen('F'); break;
  case TempleEvent: UpdateTempleScreen('K'); break;
  case Vault: UpdateVaultScreen('i'); break;
  case GiveTreasure: UpdateGiveTreasureScreen('h'); break;
  case CONTROL_Encamp: UpdateEncampScreen('g'); break;
  case CONTROL_CombatSpellAimAuto: UpdateCombatAimScreen(); break;
  case CONTROL_CombatMove: UpdateCombatMoveScreen(); break;

  case CONTROL_CreateCharacter:
  default: UpdateAskYesNoMenuScreen(); break;
  }
}

#ifdef SIMPLE_STRUCTURE
#else
////////////////////////////////////////////////////////////////////////DESIGN_SELECT_MENU_DATA
void DESIGN_SELECT_MENU_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    HMenuVItemsKeyboardAction(key, ascii);
    return;
  }


  if (GetMyState() == TASK_DisplayDesigns)
  {
    /*
     { "SELECT", 0 },
     { "NEXT", 0 },
     { "PREV", 0 },
     { "EXIT", 1 }
    */
    switch (menu.currentItem()) 
    {
    case 1: // select
      {
        //int dindex = spellListText.GetSpellIndex(currSpellTextIndex());
        int dindex = *spellListText.GetInteger(currSpellTextIndex());
        CString designPath = m_designs[dindex].path;
        rte.DefaultFoldersFromDesign(designPath);

        //WriteDebugString("Working directory = %s\n", m_gameDir);
      
        CString filename;
        filename = rte.ConfigDir() + "config.txt";

        if (!LoadConfigFile(filename))
        {
          debugSeverity = 7;
          WriteDebugString("Failed to load config.txt file in InitInstance()\n");
        }
        else
        {
          // pops all events, including this one, from stack
          // if design loaded ok.
          //
          InitGame(AfxGetMainWnd()->GetSafeHwnd()); 
        }
      }
      break;    
    case 2: // next
      nextSpellPage();
      break;
    case 3: // prev
      prevSpellPage();
      break;
    case 4: // exit app
      PopEvent(); // pop to exit event
      break;

    default:
      WriteDebugString("Unhandled menu item in DESIGN_SELECT_MENU_DATA");
      break;
    }   
  }
}

void DESIGN_SELECT_MENU_DATA::OnInitialEvent(void)
{
  //WriteDebugString("Beginning search for all designs...\n");
  ClearFormattedText(textData);
  menu.setMenu(SelectMenuData);
  menu.setHorzOrient();
  SetMyState(TASK_SearchForDesigns);

  char fullPath[_MAX_PATH+1];
  GetModuleFileName(NULL, fullPath, _MAX_PATH);
  CString location = fullPath;
  m_Drive = GetFilenameDrive(location);
  m_Drive.MakeUpper();

  OnDraw();
  if (!AddDesigns())
    PopEvent();
}

void DESIGN_SELECT_MENU_DATA::OnDraw(void)
{
  int StartX = 18;
  int StartY = 38;
  int MaxCharHeight = GraphicsMgr.GetMaxCharacterHeight();

  GraphicsMgr.ClearAdventureBackground(BACKBUFFERNUM);

  {   
    int totalListItems = spellListText.GetCount();
    if (totalListItems == 0)
    {
      CString tmp;
      tmp.Format("NO DESIGNS AVAILABLE ON DRIVE %s, PRESS ENTER TO EXIT", m_Drive);
      displayText(StartX, StartY, tmp);
      menu.DisplayMenu(); 
      DisplayTextAndFlip();
      return;
    }

    displayText(StartX, StartY, spellListText.title);
    int currItem = startItem;
    int currY = StartY+MaxCharHeight+5;
    CString listtext;
    CString dname;

    while (currItem < totalListItems)
    {
      //int d_index = spellListText.GetSpellIndex(currItem);    
      int d_index = *spellListText.GetInteger(currItem);    
      dname = m_designs[d_index].path;
      StripFilenamePath(dname);

      listtext.Format("%6.4f     %s   (%s)",
                  m_designs[d_index].ver,
                  m_designs[d_index].name,
                  dname);

      displayText(StartX, currY, listtext , White, shouldHighlight(currItem));
  
      if (totalListItems > Items_Per_Page)
      {
         if (! ((currItem+1) % Items_Per_Page) )
         {
            currY=StartY+MaxCharHeight+5;
            break;
         }
      }
      currItem++;
      currY += MaxCharHeight;
    }

    menu.DisplayMenu();  
  };
  DisplayTextAndFlip();
}

void DESIGN_SELECT_MENU_DATA::OnUpdateUI(void)
{
  menu.setAllItemsActive();
  if (spellListText.GetCount() <= 0)
    menu.setItemInactive(1); // select    
  if (!allowSpellNextPage())
    menu.setItemInactive(2); // next
  if (!allowSpellPrevPage())
    menu.setItemInactive(3); // prev
}

void DESIGN_SELECT_MENU_DATA::GetDesignVersion(const char *fname, double &ver, CString &name)
{
  CFile myFile;
  ver=0.0;
  name="";
  
  try
  {
    if (!myFile.Open(fname, CFile::modeRead))
    {
      WriteDebugString("Open Error: Unable to determine design version from file %s\n", fname);
      return;
    }   

    CArchive ar(&myFile, CArchive::load);  
    ar >> ver;
    DAS(ar,name);
//    ar >> name;
//    if (name == "*")
//      name="";
    ar.Close();
  }
  catch (...)
  {
    WriteDebugString("Unable to determine design version from file %s\n", fname);
  }

  if (ver <= 0.0)
    WriteDebugString("Error %u trying to determine design version from file %s\n", 
           GetLastError(), fname);
}



BOOL DESIGN_SELECT_MENU_DATA::SearchForDesignFolders(const CString &path)
{
  if (path.GetLength() <= 0) return FALSE;

  {
    /*
    WIN32_FIND_DATA findData;
    HANDLE hSearch;
    char fullPath[_MAX_PATH+1];
    strcpy(fullPath, global_dataDir);
    strcat(fullPath, "game.dat");
    hSearch = FindFirstFile(fullPath, &findData);
    if (hSearch != INVALID_HANDLE_VALUE)
    {
      if (strlen(findData.cFileName) > 0)
      {
        CString dsnpath;
        dsnpath = path;
 //         fullpath += findData.cFileName;
 //         if (fullpath[fullpath.GetLength()-1] != '\\')
 //           fullpath += "\\";

		    if (path.Find("\\BACKUP\\") == -1)	// Check if this is the design
				  								// backup directory. If it is, ignore it when
				  								// checking for designs.
		    {
  			  if (DesignAvail(global_dataDir))
	  		  {
  		  		//CString gamedat;
			  	  //gamedat.Format("%sgame.dat", path);
				    double ver;
				    CString temp;
				    GetDesignVersion(fullPath, ver, temp);
				    if (ver != 0.0)
				    {
    					DESIGN_DETAIL data;
					    data.name = temp;
					    data.path = dsnpath;
					    data.ver = ver;
					    //WriteDebugString("Adding design %s\n", fullpath);
					    AddDesign(data);
					    OnDraw();
				    }
			    }
        }
      }

    };

    FindClose(hSearch);
    */
    char path[_MAX_PATH+1];
    /*BOOL result = */ XBrowseForFolder(NULL, NULL, NULL, "Find Design Folder", path, _MAX_PATH, TRUE);
  };
  return FALSE;
}


BOOL DESIGN_SELECT_MENU_DATA::AddDesigns()
{
  BOOL success = TRUE;

  //CString installDir = m_Drive;
  //if (installDir[installDir.GetLength()-1] != '\\')
  //  installDir += "\\";

  m_designs.RemoveAll();
  //AddAllDesignsInFolder(installDir);
  //SearchForDesignFolders(installDir);
  CString ExePath = rte.ExecutableDir();
  CString OneUpFromExePath;
  if (ExePath.GetLength() > 3)
    StripOneFolderFromFilenamePath(ExePath, OneUpFromExePath);
  SearchForDesignFolders(OneUpFromExePath); 

  for (int p=0; 
        (p < 5) && (NumDesigns()==0) && (OneUpFromExePath.GetLength()>3); 
          p++)
  {
    StripOneFolderFromFilenamePath(OneUpFromExePath, OneUpFromExePath);
    SearchForDesignFolders(OneUpFromExePath); 
  }

  // use list of designs to prepare text listing
  // for user to select from
  //if (success)
  {
    clearSpellItemState();
    spellListText.FormatSpellListTitle("SELECT A DESIGN TO PLAY");

    spellListText.m_flags = SHOW_DESIGN_FLAG;  
    for (int i=0;i<NumDesigns();i++)
    {
      //GLOBAL_SPELL_ID gsID;
      //gsID.IllegalConversion(i);
      //spellListText.Add(gsID);
      spellListText.Add(i);
    };
    
    int scnt = spellListText.GetCount();
    ItemsOnPage = min(Items_Per_Page, scnt);
    if (ItemsOnPage > 0)
    {
      if (party.activeItem >= spellListText.GetCount())
        party.activeItem = (BYTE)(spellListText.GetCount()-1);
      if (party.activeItem >= (startItem+ItemsOnPage))
        party.activeItem = (BYTE)((startItem+ItemsOnPage)-1);
      if (party.activeItem < 0)
        party.activeItem = 0;
    }
    else
    {
      party.activeItem = 0;
      startItem = 0;
      //currShopItem = 0;
    }
  }
  SetMyState(TASK_DisplayDesigns);
  Invalidate();
  return success;
}
#endif
////////////////////////////////////////////////////////////////////////PLAY_MOVIE_DATA
#include <dshow.h>
void PLAY_MOVIE_DATA::OnInitialEvent(void)
{



/*
{
    IGraphBuilder *pGraph = NULL;
    IMediaControl *pControl = NULL;
    IMediaEvent   *pEvent = NULL;

    // Initialize the COM library.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        return;
    }

    // Create the filter graph manager and query for interfaces.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager.");
        return;
    }

    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

    // Build the graph. IMPORTANT: Change this string to a file on your system.
    hr = pGraph->RenderFile(L"E:\\UAF\\Outstanding\\170829_AVI_CtD_MINI-MOD\\Resources\\companion_cube_Mute_SM.avi", NULL);
    if (SUCCEEDED(hr))
    {
        // Run the graph.
        hr = pControl->Run();
        if (SUCCEEDED(hr))
        {
            // Wait for completion.
            long evCode;
            pEvent->WaitForCompletion(INFINITE, &evCode);

            // Note: Do not use INFINITE in a real application, because it
            // can block indefinitely.
        }
    }
    pControl->Release();
    pEvent->Release();
    pGraph->Release();
    CoUninitialize();
}

*/






#ifdef avicrash
  WriteDebugString("PLAY_MOVIE_DATA::OnInitialEvent\n");
#endif
  if (m_filename.IsEmpty())
  {
    ChainHappened();
    return;
  }
  menu.setMenu(PressEnterMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  currPic.Clear();
  //ClearFormattedText(textData);

  RECT *pRect = NULL;
  switch (m_mode)
  {
  case movieFull:
    // do nothing, no rect means fullscreen playback
    break;

  case movieBig:
    pRect = new RECT;
    pRect->left=BigPic_x;
    pRect->top=BigPic_y;
    pRect->right=pRect->left+BigPic_w;
    pRect->bottom=pRect->top+BigPic_h;
    break;

  case movieSmall:
    pRect = new RECT;
    pRect->left=ViewportX;
    pRect->top=ViewportY;
    pRect->right=pRect->left+ViewportWidth;
    pRect->bottom=pRect->top+ViewportHeight;
    break;
  }

  if (pRect!=NULL)
  {
    if (   ((pRect->bottom - pRect->top) < 100)
        || ((pRect->right - pRect->left) < 100))
    {
      // something is wrong with screen coords, use
      // fullscreen
      delete pRect;
      pRect=NULL;
    }
  }

  switch (m_mode)
  {
  case movieFull:
    // do nothing, fullscreen playback
    break;
  case movieBig:
    UpdateMovieBigPicWindow();
    break;
  case movieSmall:
    UpdateMovieSmallPicWindow();
    break;
  }
#ifdef avicrash
  WriteDebugString("PLAY_MOVIE_DATA::OnInitialEvent - Call Playmovie\n");
#endif
  if (!GraphicsMgr.PlayMovie(rte.MovieDir() + m_filename, 
                             pRect, 
                             (pRect==NULL))) // NULL triggers fullscreen
  {
    if (pRect!=NULL) delete pRect;
    GraphicsMgr.StopMovie();
    ChainHappened(); // movie failed to load
    return;
  }

  if (pRect!=NULL) delete pRect;
  if (pSndMgr) pSndMgr->MuteVolume();
}

void PLAY_MOVIE_DATA::OnKeypress(key_code key, char ascii)
{
  // while the movie is playing, the game screen
  // area outside of the movie window is not updated.
  //if (m_mode == movieSmall)
  //  if (TABParty(key)) return;
  if (m_mode != movieFull)
  {
    if (key != KC_RETURN) 
      return;
  }

  // stop movie and exit event
  GraphicsMgr.StopMovie();
  if (pSndMgr) pSndMgr->UnMuteVolume();
  ChainHappened();
}

void PLAY_MOVIE_DATA::OnDraw(void)
{
  OnIdle();
}

bool PLAY_MOVIE_DATA::OnIdle(void)
{
  // draw next movie frame on front buffer
#ifdef avicrash
  WriteDebugString("PLAY_MOVIE_DATA::OnIdle\n");
#endif
  if (!GraphicsMgr.UpdateMovie())
  {
#ifdef avicrash
  WriteDebugString("PLAY_MOVIE_DATA::StopMovie\n");
#endif
    GraphicsMgr.StopMovie();
    if (pSndMgr) pSndMgr->UnMuteVolume();
    ChainHappened();
  }
  else
  {
    switch (m_mode)
    {
    case movieFull:
      // do nothing, fullscreen playback
      break;
    case movieBig:
      //UpdateMovieBigPicWindow();
      break;
    case movieSmall:
      //UpdateMovieSmallPicWindow();
      break;
    }
  }
#ifdef avicrash
  WriteDebugString("PLAY_MOVIE_DATA::return\n");
#endif
  return true;
}

////////////////////////////////////////////////////////////////////////JOURNAL_EVENT

void JOURNAL_EVENT::OnInitialEvent(void)
{
  if (journal_entry < 0)
  {
    ChainHappened();
    return;
  }

  JOURNAL_ENTRY data;
  if (!globalData.journalData.Get(journal_entry, data))
  {
    ChainHappened();
    return;
  }

  data.origentry = data.entry; // save original journal key
  
  char *buffer=NULL;
  // substitute any ^ tokens with proper text before adding
  // text to journal
  PreProcessText(data.text, &buffer);
  data.text = buffer;
  if (buffer != NULL) delete [] buffer;

  party.journal.Add(data);
  ChainHappened();
}

////////////////////////////////////////////////////////////////////////DISPLAY_PARTY_JOURNAL_DATA

void DISPLAY_PARTY_JOURNAL_DATA::OnKeypress(key_code key, char ascii)
{
  if (key != KC_RETURN)
  {
    StandardMenuKeyboardAction(key,ascii);
    return;
  }
  switch (menu.currentItem()) 
  {
  case 1: // next
    if (!journalTextData.IsLastJournalBox())
      journalTextData.NextJournalBox();
    break;    
  case 2: // prev
    if (!journalTextData.IsFirstJournalBox())
      journalTextData.PrevJournalBox();
    break;    
  case 3: // first
    journalTextData.FirstJournalBox();
    break;
  case 4: // last
    journalTextData.LastJournalBox();
    break;
  case 5: // exit
    PopEvent();
    break;    
  default:
    WriteDebugString("Unhandled menu item in DISPLAY_PARTY_JOURNAL_DATA");
    miscError = InvalidMenuState;
    PopEvent();
    break;
  } 
}

void DISPLAY_PARTY_JOURNAL_DATA::OnInitialEvent(void)
{
  ClearFormattedText(textData);
  menu.setMenu(DisplayJournalMenuData, NULL, FALSE, NULL, NULL);
  menu.setHorzOrient();
  menu.MapKeyCodeToMenuItem(KC_ESCAPE, 5);
  FormatJournalText(journalTextData, party.journal);
  journalTextData.LastJournalBox();
}

void DISPLAY_PARTY_JOURNAL_DATA::OnDraw(void)
{
  UpdateDisplayPartyJournalScreen();
}

void DISPLAY_PARTY_JOURNAL_DATA::OnUpdateUI(void)
{
  /*
   1{ "NEXT", 0 },
   2{ "PREV", 0 },
   3{ "FIRST", 0 },
   4{ "LAST", 0 },
   5{ "EXIT", 1 }
  */   
  menu.setAllItemsActive();
  if (journalTextData.IsLastJournalBox())
  {
    menu.setItemInactive(1); // next
    menu.setItemInactive(4); // last
  }
  if (journalTextData.IsFirstJournalBox())
  {
    menu.setItemInactive(2); // prev
    menu.setItemInactive(3); // first
  }
}
