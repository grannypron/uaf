/******************************************************************************
* Filename: CProcinp.cpp
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

///////////////////////////////////////////////////////////////////////////////
// 
//             The Life of an Event
//
// Put on task list                  Call OnEnterQueue
//
// Check trigger condition           Call OnTestTrigger
//   If trigger condition not met    Call ChainNotHappen (remove or replace yourself)
//   and check new top-of-list
//
// Becomes available for execution   Mark invalid
//                                   Call OnInitialEvent 
//                                   Call OnDraw if still invalid
//                                   Mark valid
//
// While input is available          Mark invalid
//                                   Call OnKeypress 
//                                   Call OnDraw if still invalid
//                                   Mark valid
//
// While no input is available       Call OnIdle until it returns true
//
// Pops to top of queue again        Mark invalid
//                                   Call OnReturnToTopOfQueue 
//                                   Call OnDraw if still invalid
//                                   Mark as valid
//
// Once per scheduler loop           Call OnCycle
//
// Leaves task list                  Call OnLeaveQueue
//
///////////////////////////////////////////////////////////////////////////////

#include "..\shared\stdafx.h"
//#include "..\Shared\ProjectVersion.h"

#include "externs.h"

#include "GetInput.h"
#include "class.h"
#include "CProcInp.h"
#include "party.h"
#include "globaldata.h"
#include "MouseThread.h"
#ifdef UAFEDITOR
#include "..\UAFWinEd\Resource.h"
#else
#include "..\UAFWin\Resource.h"
#endif

#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef WIN10Problem
extern bool WIN10ProblemActive;
#endif

extern bool pretendEditorRunning;
extern bool pretendEditorNotRunning;
extern int setPartyXY_x;
extern int setPartyXY_y;

extern const double ENGINE_VER;


#ifdef TRACE_TIMER_DETAILS
class DETAIL_TRACE
{
//  FILE *f;
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





TASKENTRY& TASKENTRY::operator = (TASKENTRY& src)
{
  if (this==&src) return *this;
  m_event=src.m_event;
  m_state=src.m_state;
  m_disposition=src.m_disposition;
  m_invalid=src.m_invalid;
  m_poppedToTop=src.m_poppedToTop;
  return *this;
}

TASKLIST::TASKLIST(void)
{
  m_hash = 0;
  m_eventToBeDeleted = NULL;
#ifdef TASKTRACE
  m_traceFile = NULL;
  m_IMCCount = 0;
  TaskTrace(TTR_TASKLIST);
#endif
  for (int i=0; i<MAXTASK; i++)
    m_tasklist[i].Clear();
}

TASKLIST::~TASKLIST(void)
{
  Clear();
}

void TASKLIST::Clear(void)
{
  m_hash += m_HASH_Clear;
#ifdef TASKTRACE
  TaskTrace(TTR_Clear);
#endif
  for (int i=0; i<MAXTASK; i++) 
  {
    if (m_tasklist[i].m_event!=NULL)
    {
      m_tasklist[i].m_event->OnLeaveQueue();
      if (m_tasklist[i].m_disposition==DeleteEvent) 
      {
         delete m_tasklist[i].m_event;
         m_tasklist[i].m_event = NULL;
         m_tasklist[i].m_state = TASK_empty;  //20100814
      }
    }
    m_tasklist[i].Clear();
  }
  if (m_eventToBeDeleted != NULL)
  {
    delete m_eventToBeDeleted;
    m_eventToBeDeleted = NULL;
  };
}


CString TASKLIST::FindEventAttribute(int depth, const CString& name)
{
  if (depth < MAXTASK)
  {
    if (m_tasklist[depth].m_event != NULL)
    {
      const GameEvent *pEvent;
      pEvent = m_tasklist[depth].m_event;
      return pEvent->FindEventAttribute(name);
    };
  };
  return "-?-?-";
}

CString FindEventAttribute(int depth, const CString& name)
{
  return taskList.FindEventAttribute(depth, name);
}

#ifdef TASKTRACE
void TASKLIST::ToggleTrace(void)
{
  if (m_traceFile != NULL)
  {
    fclose(m_traceFile);
    m_traceFile = NULL;
  }
  else
  {
    CString fileName;
    fileName = rte.LogDir() + "TaskTrace.txt";
    m_traceFile = fopen(fileName,"w");
  };
}


void TASKLIST::TraceLine(TTR_CODE ttrCode, int flags)
{
  char *name;
  int wallClockTime;
  switch (ttrCode)
  {
  case TTR_OnIdle:               name = "OnIdle ('I')";               break;
  case TTR_TASKLIST:             name = "TASKLIST";                   break;
  case TTR_LoadTaskStack:        name = "LoadTaskStack";              break;
  case TTR_GetEventType:         name = "GetEventType";               break;
  case TTR_Clear:                name = "Clear";                      break;
  case TTR_RemoveAll:            name = "RemoveAll";                  break;
  case TTR_SaveTaskStack:        name = "SaveTaskStack";              break;
  case TTR_MakeSureEventIsReady: name = "MakeSureEventIsReady ('M')"; break;
  case TTR_OnCycle:              name = "OnCycle ('C')";              break;
  case TTR_SetEventState:        name = "SetEventState";              break;
  case TTR_GetEventState:        name = "GetEventState ('G')";        break;
  case TTR_PushEvent:            name = "PushEvent";                  break;
  case TTR_InsertEvent:          name = "InsertEvent";                break;
  case TTR_PopEvent:             name = "PopEvent";                   break;
  case TTR_ReplaceEvent:         name = "ReplaceEvent";               break;
  case TTR_TaskMessage:          name = "TaskMessage";                break;
  case TTR_OnKeypress:           name = "OnKeypress";
    break;
  case TTR_OnMouseClickLeft:     name = "OnMouseClickLeft";           break;
  case TTR_OnTimer:              name = "OnTimer";                    break;
//  case TTR_OnMouseMove:          name = "OnMouseMove";                break;
  default: name = "Unknown"; break;
  };

  wallClockTime = timeGetTime();
  fprintf(m_traceFile,"%08x %I64x %08x %08x %s %s\n",
          wallClockTime, 
          virtualGameTime, 
          m_hash, flags, 
          name, 
          (m_tasklist[0].m_event==NULL)?"":m_tasklist[0].m_event->GetEventTypeName());
}

void TASKLIST::DetailTrace(const CString& line)
{
  if (m_traceFile != NULL)
  {
    fprintf(m_traceFile,"%s", (LPCSTR)line);
  };
}

void TASKLIST::TaskTrace(TTR_CODE ttrCode, int flags)
{
  char c;
  if (m_traceFile == NULL) return;
  c = ' ';
  switch (ttrCode)
  {
  case TTR_OnIdle:               if (flags == 0) c = 'I'; break;
  case TTR_MakeSureEventIsReady:                 c = 'M'; break;
  case TTR_OnCycle:              if (flags == 0) c = 'C'; break;
  case TTR_GetEventState:        if (flags == 0) c = 'G'; break;
  };
  bool criticalTime;
  criticalTime = (virtualGameTime > 0x16600) && (virtualGameTime < 0x16700);
  if ((c == ' ') || criticalTime)
  {
    if (m_IMCCount != 0)
    {
      fprintf(m_traceFile,"\n");
      m_IMCCount = 0;
    };
    TraceLine(ttrCode, flags);
  }
  else
  {
    if (m_IMCCount < 70)
    {
      if (m_IMCCount < 4)
      {
        TraceLine(ttrCode, flags);
      }
      else
      {
        fprintf(m_traceFile, "%c", c);
        if (c == 'M')
        {
          fprintf(m_traceFile,"%x",flags);
          m_IMCCount += 2;
        };
      };
      m_IMCCount++;
    }
    else
    {
      fprintf(m_traceFile, "\n");
      m_IMCCount = 1;
      TraceLine(ttrCode, flags);
    };
  }
}

#endif

void TASKLIST::RemoveAll(void)
{
  m_hash += m_HASH_RemoveAll;
#ifdef TASKTRACE
  TaskTrace(TTR_RemoveAll);
#endif
  for (int i=0; i<MAXTASK; i++)
  {
    if (m_tasklist[i].m_event!=NULL)
    {
      m_tasklist[i].m_event->OnLeaveQueue();
      if (m_eventToBeDeleted != NULL)
      {
        delete m_eventToBeDeleted;
        m_eventToBeDeleted = NULL;
      };
      if (m_tasklist[i].m_disposition==DeleteEvent)
      {
        m_eventToBeDeleted = m_tasklist[i].m_event;      
        m_tasklist[i].m_event = NULL;
        m_tasklist[i].m_state = TASK_empty; //20100814
      }
    }
    m_tasklist[i].Clear();
  };
}


void TASKLIST::MakeSureEventIsReady(void)
{
  m_hash += m_HASH_MakeSureEventIsReady;
#ifdef TASKTRACE
  TaskTrace(TTR_MakeSureEventIsReady, m_tasklist[0].m_state);
#endif
  while (1)
  {
    if ((m_tasklist[0].m_state==TASK_empty) ||  (m_tasklist[0].m_state==TASK_Deleted))
    {
      if (!PopEvent(m_tasklist[0].m_event))
      {
        continue;
      };
    };
    if (m_tasklist[0].m_state==TASK_uninitialized)
    {
      ASSERT(m_tasklist[0].m_event!=NULL);
      // Test the game state (quest variables, facing, etc. ) and
      // let us know if you want to be triggered.
      m_tasklist[0].m_poppedToTop=false;
      setPartyXY_x = -1;
      if (m_tasklist[0].m_event->OnTestTrigger())
      {
        m_tasklist[0].m_state=TASK_initialized;
        if (m_tasklist[0].m_event->IsGlobalEvent())
          globalData.eventData.markEventHappened(m_tasklist[0].m_event);    
        else
          levelData.eventData.markEventHappened(m_tasklist[0].m_event);    
        // Now it is safe to 

        // Setting m_invalid unconditionally here causes unnecessary OnDraw().
        // OnTrigger or OnInitialEven() can Invalid() if necessary.
        //m_tasklist[0].m_invalid=true;
        


        m_tasklist[0].m_event->OnInitialEvent(); // may replace self with a new event
        // If it pops an event we will call OnDraw an extra time...no harm.
        // If it pushes an event then m_invalid will be false.
        if ((m_tasklist[0].m_invalid) && (m_tasklist[0].m_state!=TASK_Deleted))
        {
          if (m_tasklist[0].m_event != NULL)
          {
            unsigned int oldHash;
            oldHash = taskList.Hash();
            m_tasklist[0].m_event->OnUpdateUI();
            ASSERT(oldHash == taskList.Hash());
            m_tasklist[0].m_event->OnDraw();
            ASSERT(oldHash == taskList.Hash());
          };
        }
        m_tasklist[0].m_invalid=false;
        //return;
      }
      else
      {
        // You were not triggered.  So you should tell us what 
        // to do.  Many events have a Chain-if-not-happened condition
        // or a chain-always condition.  If so, they
        // should replace themselves on the stack with the 
        // chained event.  If no such event exists then you should
        // simply pop yourself off the stack and that will be the end
        // of the chain.
        if (setPartyXY_x >= 0)
        {
          // THe event should delete itself and be gone (Except ADVENTURE, of course)
          TaskMessage(TASKMSG_SetPartyXY);
        }
        else
        {
          m_tasklist[0].m_event->OnChainNotHappened();
        };
      };
      continue;
    };
    if (m_tasklist[0].m_poppedToTop)
    {
      m_tasklist[0].m_poppedToTop=false;

      // Setting m_invalid here unconditionally causes OnDraw
      // operations that are totally unnecessary.  If drawing is necessary
      // under some circumstances then OnReturnToTopOfQueue should set m_invalid.
      //m_tasklist[0].m_invalid=true;

      m_tasklist[0].m_event->OnReturnToTopOfQueue();
      if (    (m_tasklist[0].m_invalid) 
           && (m_tasklist[0].m_state!=TASK_Deleted)
           && (m_tasklist[0].m_state!=TASK_empty)
         )
      {
        unsigned int oldHash;
        oldHash = taskList.Hash();
        m_tasklist[0].m_event->OnUpdateUI();
        m_tasklist[0].m_event->OnDraw();
        ASSERT(oldHash == taskList.Hash());
      }
      m_tasklist[0].m_invalid=false;
      continue;
    };
    break; // Event is ready to run.
  };
}


BOOL TASKLIST::IsEventReady(void)
{
  if (m_tasklist[0].m_state==TASK_Deleted)
  {
    return FALSE;
  };
  if (m_tasklist[0].m_state==TASK_uninitialized)
  {
    return FALSE;
  };
  if (m_tasklist[0].m_poppedToTop)
  {
    return FALSE;
  };
  return TRUE; // Event is ready to run.
}


void TASKLIST::OnCycle(void)
{
  m_hash += m_HASH_OnCycle;
#ifdef TASKTRACE
  TaskTrace(TTR_OnCycle);
#endif
  DETAILTRACE(0x84010,0);
  ASSERT(m_tasklist[0].m_event!=NULL);
  if (m_tasklist[0].m_state==TASK_uninitialized) return;
  MakeSureEventIsReady();
  DETAILTRACE(0x84011,0);

//  // Why should OnCycle be here????  
//  // If you want to redraw the screen you should set Invalid.
//  bool draw = /*m_tasklist[0].m_event->OnCycle() | */ m_tasklist[0].m_invalid;
  bool draw = m_tasklist[0].m_event->OnCycle() | m_tasklist[0].m_invalid;
  if (draw)
  {
    DETAILTRACE(0x84012,m_tasklist[0].m_event->event);
    m_tasklist[0].m_event->OnDraw();
    DETAILTRACE(0x84013,0);
    m_tasklist[0].m_invalid=false;
    DETAILTRACE(0x84014,0);
  }
}


//void TASKLIST::CheckForDraw(void)
//{
//  ASSERT(m_tasklist[0].m_event!=NULL);
//  if (m_tasklist[0].m_state==TASK_uninitialized) return;
//  if (m_tasklist[0].m_invalid)
//  {
//    m_tasklist[0].m_event->OnDraw();
//    m_tasklist[0].m_invalid=false;
//    m_tasklist[0].m_event->OnPostDraw();
//  }  
//}


void TASKLIST::Invalidate(void)
{
  if (m_tasklist[0].m_state == TASK_empty) return;
  Invalidate(m_tasklist[0].m_event);
}

void TASKLIST::Invalidate(GameEvent *active)
{
#ifdef WIN10Problem
  if (WIN10ProblemActive)
  WriteDebugString("TASKLIST::Invalidate(GameEvent *)\n");
#endif

  ASSERT(m_tasklist[0].m_event!=NULL);
  ASSERT(m_tasklist[0].m_event==active);
  m_tasklist[0].m_invalid=true;
  m_tasklist[0].m_event->OnInvalidate();
}

void TASKLIST::SetEventState(TASKSTATE state, GameEvent *active)
{
  m_hash += m_HASH_SetEventState;
#ifdef TASKTRACE
  TaskTrace(TTR_SetEventState);
#endif
  int i;
  for (i=0; i<MAXTASK; i++)
  {
//    if (m_tasklist[i].m_event==NULL) break;
    if (m_tasklist[i].m_event==active) break;
  };
  ASSERT (i<MAXTASK);
  if (i>=MAXTASK) return;
  ASSERT (m_tasklist[i].m_event!=NULL);
  if (m_tasklist[i].m_event==NULL) return;
  ASSERT (m_tasklist[i].m_state!=TASK_Deleted);
  m_tasklist[i].m_state=state;
}

TASKSTATE TASKLIST::GetEventState(GameEvent *active)
{
  // You can get your state for free.  // m_hash += m_HASH_GetEventState;
#ifdef TASKTRACE
  TaskTrace(TTR_GetEventState);
#endif
  ASSERT(m_tasklist[0].m_event!=NULL);
  ASSERT(m_tasklist[0].m_event==active);
  return m_tasklist[0].m_state;
}

void TASKLIST::PushEvent(GameEvent *event, 
                         EventDisposition disposition)
{
  m_hash += m_HASH_PushEvent;
#ifdef TASKTRACE
  TaskTrace(TTR_PushEvent);
#endif
  ASSERT(event!=NULL);
  // Inefficient way of doing things.  We can change it however
  // we please later.  I did it this way so I can look at the top
  // entries in the debugger by looking at m_tasklist[0], [1], [2].
  if (m_tasklist[MAXTASK-1].m_event!=NULL)
  {
    for (int i=0;i<MAXTASK;i++)
    {
      if (m_tasklist[i].m_event!=NULL)
        WriteDebugString("TASKLIST[%i]=%i(%i,%i)\n", 
                         i, 
                         m_tasklist[i].m_event->GetEventId(),
                         m_tasklist[i].m_event->GetEventX(),
                         m_tasklist[i].m_event->GetEventY());
      else
        WriteDebugString("TASKLIST[%i]=NULL\n", i);
    }
    MsgBoxError("TASKLIST overflow (PushEvent)");
  };
  for (int i=MAXTASK-1; i>0; i--) m_tasklist[i]=m_tasklist[i-1];
  m_tasklist[0].m_event=event;
  m_tasklist[0].m_state=TASK_uninitialized;
  m_tasklist[0].m_disposition=disposition;
  m_tasklist[0].m_invalid=false;
  event->OnEnterQueue();
}


void TASKLIST::InsertEvent(GameEvent *event, 
                           EventDisposition disposition)
{
  m_hash += m_HASH_InsertEvent;
#ifdef TASKTRACE
  TaskTrace(TTR_InsertEvent);
#endif
  if (m_tasklist[MAXTASK-1].m_event!=NULL)
  {
    for (int i=0;i<MAXTASK;i++)
    {
      if (m_tasklist[i].m_event!=NULL)
        WriteDebugString("TASKLIST[%i]=%i(%i,%i)\n", 
                         i, 
                         m_tasklist[i].m_event->GetEventId(),
                         m_tasklist[i].m_event->GetEventX(),
                         m_tasklist[i].m_event->GetEventY());
      else
        WriteDebugString("TASKLIST[%i]=NULL\n", i);
    }
    MsgBoxError("TASKLIST overflow (InsertEvent)");
  };
  for (int i=MAXTASK-1; i>1; i--) m_tasklist[i]=m_tasklist[i-1];
  m_tasklist[1].m_event=event;
  m_tasklist[1].m_state=TASK_uninitialized;
  m_tasklist[1].m_disposition=disposition;
  m_tasklist[1].m_invalid=false;
  event->OnEnterQueue();
}

bool TASKLIST::PopEvent(GameEvent *active)
{
  bool allEmpty;
  m_hash += m_HASH_PopEvent;
#ifdef TASKTRACE
  TaskTrace(TTR_PopEvent);
#endif
  ASSERT(  (m_tasklist[0].m_event!=NULL)
         ||(m_tasklist[0].m_state==TASK_Deleted)
         ||(m_tasklist[0].m_state==TASK_empty));

  ASSERT(    (m_tasklist[0].m_event==active)
          || (m_tasklist[0].m_state==TASK_Deleted)
          || (m_tasklist[0].m_state==TASK_empty));

  if (    (m_tasklist[0].m_state!=TASK_Deleted)
       && (m_tasklist[0].m_state!=TASK_empty) )
  {
    m_tasklist[0].m_event->OnLeaveQueue();
    {
      if (m_tasklist[0].m_disposition==DeleteEvent) 
      {
        // 20120203 PRS
        // Don't actually delete the event instance right now.
        // After calling PopEvent(), the code may reference
        // instance variables before returning.
        // We will keep a one-deep queue of things to delete.
        if (m_eventToBeDeleted != NULL) 
        {
          delete m_eventToBeDeleted;
          m_eventToBeDeleted = NULL;
        };
        m_eventToBeDeleted = m_tasklist[0].m_event;
        m_tasklist[0].m_event = NULL;
        m_tasklist[0].m_state = TASK_empty; //20100814
      }
    };
  };
  allEmpty = true;
  for (int i=0; i<MAXTASK-1; i++) 
  {
    m_tasklist[i]=m_tasklist[i+1];
    if (m_tasklist[i].m_state != TASK_empty)
    {
      allEmpty = false;
    };
  };
  m_tasklist[MAXTASK-1].Clear();
  if (m_tasklist[0].m_state!=TASK_uninitialized)
  {
    ASSERT(    (m_tasklist[0].m_event!=NULL)
            || (m_tasklist[0].m_state==TASK_Deleted)
            || (m_tasklist[0].m_state==TASK_empty)
          );
    if (    (m_tasklist[0].m_state != TASK_Deleted)
         && (m_tasklist[0].m_state != TASK_empty)
       )
    {
      m_tasklist[0].m_poppedToTop=true;
      m_tasklist[0].m_invalid=false;
    };
  };
  return allEmpty;
}

void TASKLIST::StackTrace(void)
{
  WriteDebugString("TASKLIST STACK TRACE: level %i hash %u in_combat %i\n", 
                   GetCurrentLevel(),m_hash, IsCombatActive());
  for (int i=0;i<MAXTASK;i++)
  {
    if (m_tasklist[i].m_event!=NULL)
      WriteDebugString("TASKLIST[%i]= evtype %i evid %i at %i,%i, evsrc %i, tstate %i tdispo %i\n", 
                       i, 
                       m_tasklist[i].m_event->GetEventType(),
                       m_tasklist[i].m_event->GetEventId(),
                       m_tasklist[i].m_event->GetEventX(),
                       m_tasklist[i].m_event->GetEventY(),
                       m_tasklist[i].m_event->GetEventSource(),
                       m_tasklist[i].m_state,
                       m_tasklist[i].m_disposition);
    else
      WriteDebugString("TASKLIST[%i]=NULL\n", i);
  }
  WriteDebugString("TASKLIST STACK TRACE END\n");
}

void TASKLIST::ReplaceEvent(GameEvent *event, 
                            GameEvent *active, 
                            EventDisposition disposition) 
{
  m_hash += m_HASH_ReplaceEvent;
#ifdef TASKTRACE
  TaskTrace(TTR_ReplaceEvent);
#endif
  ASSERT((m_tasklist[0].m_event!=NULL)||(m_tasklist[0].m_state==TASK_Deleted));
  ASSERT((m_tasklist[0].m_event==active)||(m_tasklist[0].m_state==TASK_Deleted));
  if (m_tasklist[0].m_state!=TASK_Deleted)
  {
    m_tasklist[0].m_event->OnLeaveQueue();
    if (m_eventToBeDeleted != NULL) 
    {
      delete m_eventToBeDeleted;
      m_eventToBeDeleted = NULL;
    };
    if (m_tasklist[0].m_disposition==DeleteEvent) 
    {
      m_eventToBeDeleted = m_tasklist[0].m_event;
      m_tasklist[0].m_event = NULL;
      m_tasklist[0].m_state = TASK_empty; //20100814
    }
  };
  m_tasklist[0].m_event=event;
  m_tasklist[0].m_disposition=disposition;
  m_tasklist[0].m_state=TASK_uninitialized;
  m_tasklist[0].m_invalid=false;
  event->OnEnterQueue();
}

void TASKLIST::TaskMessage(TASKMESSAGE msg)
{
  unsigned int result;
  m_hash += m_HASH_TaskMessage;
#ifdef TASKTRACE
  TaskTrace(TTR_TaskMessage);
#endif
  for (int i=0; i<MAXTASK; i++)
  {
    if (m_tasklist[i].m_event!=NULL) 
    {
      if (m_tasklist[i].m_state!=TASK_Deleted)
      {
        result=m_tasklist[i].m_event->OnTaskMessage(msg, m_tasklist[i].m_state);
        // I think maybe we should try to doubleguess any level
        // events.  If a teleport between levels occurred then
        // things will go very badly if level events do not delete
        // themselves.
        ASSERT (   (msg!=TASKMSG_TeleportBetweenLevels) 
                || (result&TASKMSG_DELETE) 
                || (m_tasklist[i].m_disposition!=LevelEvent));
        if (result & TASKMSG_DELETE) 
        {
          m_tasklist[i].m_state=TASK_Deleted;
          // We will do everything PopEvent does except that
          // we will leave the entry on the stack.
          ASSERT(m_tasklist[i].m_event!=NULL);
          // Since you asked to be deleted we assume you have
          // cleaned up after yourself.  You can call OnLeaveQueue yourself
          // if you like.
          // m_tasklist[0].m_event->OnLeaveQueue();
          if (m_tasklist[i].m_disposition==DeleteEvent) 
          {
            delete m_tasklist[i].m_event;
          };
          m_tasklist[i].m_event=NULL;
          m_tasklist[i].m_state = TASK_empty; //20100814
        };
        if (result & TASKMSG_STOP) break;
      };
    };
  };
}




void InitiateTaskMessage_SetPartyXY(int x, int y)
{
  setPartyXY_x = x;
  setPartyXY_y = y;
}

bool TASKLIST::OnIdle(void)
{
  bool result;
  MakeSureEventIsReady();
  {
    if (m_tasklist[0].m_event->timer != 0)
    {
      if (m_tasklist[0].m_event->timer <= virtualGameTime)
      {
        m_hash += m_HASH_OnTimer;
#ifdef TASKTRACE
        TaskTrace(TTR_OnIdle);
#endif
        m_tasklist[0].m_event->timer = 0;
        return m_tasklist[0].m_event->OnTimer(m_tasklist[0].m_event->timerID);
      };
    };
    m_hash += m_HASH_OnIdle;
#ifdef TASKTRACE
    TaskTrace(TTR_OnIdle);
#endif
     result = m_tasklist[0].m_event->OnIdle();
  };
  return result;
}

void TASKLIST::OnKeypress(KEY_DATA& kd) //key_code key, char ascii)
{
  bool more;
  key_code key;
  char ascii;
  key = kd.code;
  ascii = kd.key;
  m_hash += m_HASH_OnKeypress;
#ifdef TASKTRACE
  TaskTrace(TTR_OnKeypress,(key<<16) | (ascii<<8) | kd.vkey);
#endif
  do 
  { // This allows filter to generate multiple keys
    MakeSureEventIsReady();
    more=m_tasklist[0].m_event->OnFilterKeypress(&key, &ascii);
    if (key!=KC_NONE) 
    {
      m_tasklist[0].m_invalid=true;
      m_tasklist[0].m_event->OnKeypress(key, ascii);
      // The topmost event on the stack may have removed
      // itself from the stack in OnKeypress(). The new
      // topmost event is not always present. It may
      // be deleted (m_event=NULL) with state=TASK_Deleted.
      if (    (m_tasklist[0].m_invalid) 
           && (m_tasklist[0].m_state!=TASK_Deleted)
           && (m_tasklist[0].m_state!=TASK_empty)
         )
      {
        unsigned int oldHash;
        oldHash = taskList.Hash();
        m_tasklist[0].m_event->OnUpdateUI();
        m_tasklist[0].m_event->OnDraw();
        ASSERT(oldHash == taskList.Hash());
      }
      m_tasklist[0].m_invalid=false;
      if (   (m_tasklist[0].m_state!=TASK_Deleted)
          && (m_tasklist[0].m_state!=TASK_empty)
         )
      {
        if ((!m_tasklist[0].m_event->IsAChainEvent()) && (key==KC_RETURN))
          flushOldImages();
      }
      key=KC_NONE;
    }
    else
    {
      m_tasklist[0].m_invalid=true;
      m_tasklist[0].m_event->OnUpdateUI();
      m_tasklist[0].m_event->OnDraw();
      m_tasklist[0].m_invalid=false;
    }    
  } while (more);
}

void TASKLIST::OnMouseClickLeft(int x, int y)
{
  m_hash += m_HASH_OnMouseClickLeft;
#ifdef TASKTRACE
  TaskTrace(TTR_OnMouseClickLeft);
#endif
  MakeSureEventIsReady();
  m_tasklist[0].m_event->OnMouseClickLeft(x, y);
}



void TASKLIST::OnMouseMove(int x, int y)
{
#ifdef TASKTRACE
  //TaskTrace(TTR_OnMouseMove);
#endif
  //m_hash += m_HASH_OnMouseMove;
  /* We use mouse movement to update the viewport.  It must
   * NEVER be used to modify any part of the game state!
   * We do not record mouse movements.  The only time we
   * examine the mouse position is when you click one
   * of the mouse buttons.
   */
  //MakeSureEventIsReady();
  /* Calling MakeSureEventIsReady can cause lots of things to happen.
   * We might call 'OnReturnToTopOfQueue' for example.
   * So we simply will wait until the event is ready before calling 'OnMouseMove'.
   * A bit of delay of mouse movement certainly won't hurt.
   */
  if (IsEventReady())
  {
    m_tasklist[0].m_event->OnMouseMove(x, y);
    m_tasklist[0].m_invalid=true;  // Why?  We probably did nothing!  Oh, well.
                                   // When I disabled this line, the mouse cursor
                                   // no longer moved. PRS 19Feb2014
                                  
  };
}




void TASKLIST::OnTimer(UINT timerId)
{
  m_hash += m_HASH_OnTimer;
#ifdef TASKTRACE
  TaskTrace(TTR_OnTimer);
#endif
  MakeSureEventIsReady();
  if (   (m_tasklist[0].m_event!=NULL) 
      && (m_tasklist[0].m_state!=TASK_Deleted))
  {
    m_tasklist[0].m_event->OnTimer(timerId);
  }
}

void stuff (unsigned int *pack, unsigned int val, int bits)
{
  unsigned int mask;
  mask  = 0xffffffff << bits;
  *pack = *pack << bits;
  *pack = *pack & mask;
  val   = val & (~mask);
  *pack = *pack | val;
}
  
unsigned int unstuff(unsigned int *pack, int bits)
{
  unsigned int temp, mask;
  mask = ~(0xffffffff << bits);
  temp = *pack & mask;
  *pack = *pack >> bits;
  return temp;
}

bool TASKLIST::SaveTaskStack(void)
{
  unsigned int n, j;
  unsigned int temp=0;

  m_hash += m_HASH_SaveTaskStack;
#ifdef TASKTRACE
  TaskTrace(TTR_SaveTaskStack);
#endif

  // clear all stateStack entries and count event stack depth
  for (n=0; n<MAXTASK; n++)
  {
    party.stateStack.tdata[n].Clear();
    if (m_tasklist[n].m_event != NULL)
    {
      temp++;
    };
  }
  party.stateStack.count=temp;
  ASSERT( party.stateStack.count > 0 );

  // don't save top event, which should be CONTROL_Save
  j = 1;
  for (n=1; n<party.stateStack.count; n++, j++)
  {
    temp=0;
    while (m_tasklist[j].m_event == NULL)
    {
      j++;
    };
    ASSERT( m_tasklist[j].m_event != NULL );
    WriteDebugString("SaveTaskStack: Saving event %u\n", 
                     m_tasklist[j].m_event->event);

    // Encode several pieces of information into party.stateStack;    
    stuff(&temp, m_tasklist[j].m_event->event, 13);
    stuff (&temp, m_tasklist[j].m_disposition, 4); 
    stuff (&temp, m_tasklist[j].m_invalid,     1); // bool
    stuff (&temp, m_tasklist[j].m_poppedToTop, 1); // bool
    stuff (&temp, m_tasklist[j].m_state,      13); // 8192 states

    party.stateStack.tdata[n].flags=temp;
    party.stateStack.tdata[n].id=m_tasklist[j].m_event->GetEventId();
    party.stateStack.tdata[n].datacount = m_tasklist[j].m_event->OnSaveGame(party.stateStack.tdata[j].data);
    ASSERT( party.stateStack.tdata[n].datacount <= MAX_TASK_STATE_SAVE_BYTES );
  }
  return true;
}

void TASKLIST::PopAllButTopEvent()
{
  // clear out all but top event
  for (int i=1; i<MAXTASK; i++)
  {
    if (m_tasklist[i].m_event!=NULL)
    {
      m_tasklist[i].m_event->OnLeaveQueue();
      if (m_tasklist[i].m_disposition==DeleteEvent)
      {
        delete m_tasklist[i].m_event;      
        m_tasklist[i].m_event = NULL;
        m_tasklist[i].m_state = TASK_empty; //20100814
      }
    }
    m_tasklist[i].Clear();
  }
}

bool TASKLIST::LoadTaskStack(void) 
{
  unsigned int i;
//  GameEvent ev;
  DO_NOTHING_EVENT ev;
  eventType evType;
  unsigned int temp;

  m_hash += m_HASH_LoadTaskStack;
#ifdef TASKTRACE
  TaskTrace(TTR_LoadTaskStack);
#endif

  ASSERT( party.stateStack.count > 0 );

  // clear out all but top event
  PopAllButTopEvent();

  for (i=1; i<party.stateStack.count; i++)
  {
    temp=party.stateStack.tdata[i].flags;
    m_tasklist[i].m_state       = (TASKSTATE)unstuff(&temp,13);
    m_tasklist[i].m_poppedToTop = unstuff(&temp,1) != 0;
    m_tasklist[i].m_invalid     = unstuff(&temp,1) != 0;
    m_tasklist[i].m_disposition = (EventDisposition)unstuff(&temp,4);
    evType=(eventType)unstuff(&temp,13);
    if (evType!=NoEvent)
    {
      WriteDebugString("LoadTaskStack: Creating %u event\n", evType);
      m_tasklist[i].m_event = 
              ev.EventType2EventPointer(evType, party.stateStack.tdata[i].id);
      ASSERT (m_tasklist[i].m_event!=NULL);
      if (m_tasklist[i].m_event==NULL) return false;
      // All events are created dynamically on reload.
      if (m_tasklist[i].m_disposition == KeepEvent)
        m_tasklist[i].m_disposition = DeleteEvent;
    }
    else
    {
      m_tasklist[i].m_event=NULL;
      m_tasklist[i].m_state = TASK_empty; //20100814
    };
    if (m_tasklist[i].m_event!=NULL) 
      party.stateStack.tdata[i].datacount = m_tasklist[i].m_event->OnLoadGame(party.stateStack.tdata[i].data);
    ASSERT( party.stateStack.tdata[i].datacount <= MAX_TASK_STATE_SAVE_BYTES );
  };
  return true;
}
  
eventType /*int*/ TASKLIST::GetEventType() 
{ 
  m_hash += m_HASH_GetEventType;
#ifdef TASKTRACE
  TaskTrace(TTR_GetEventType);
#endif
  if (m_tasklist[0].m_event!=NULL) 
    return (m_tasklist[0].m_event->GetEventType()); 
  else 
    return NoEvent; 
}


TASKLIST taskList;
SIMULATEDKEYQUEUE simulatedKeyQueue;

CInput *CProcessInput::GetInputInstance(HWND hwnd)
{
  if (m_pLocalCInput == NULL)
  {
    m_pLocalCInput = new CInput(hwnd);
    if (m_pLocalCInput != NULL)
      m_pLocalCInput->ClearInput();
  }
  return m_pLocalCInput;
}


void CProcessInput::OnMouseMove(int x, int y)
{
  taskList.OnMouseMove(x, y);
}

void CProcessInput::StackTrace(void)
{
  taskList.StackTrace();
}

//#define TRACE_EVENT_TIMING

#ifdef TRACE_EVENT_TIMING
FILE *pOutput=NULL;
DWORD start;
DWORD stop;
#endif

BOOL CProcessInput::ProcessInput(HWND hwnd)
{
#ifdef TRACE_EVENT_TIMING
  if (pOutput==NULL)
    pOutput=fopen("timing.txt", "wt+");  
#endif

  if (ExitSignaled) return FALSE;
  UINT vkey;
  BOOL inputExists=FALSE;
  m_pLocalCInput = GetInputInstance(hwnd);
  if (m_pLocalCInput==NULL) return FALSE;

#ifdef TRACE_EVENT_TIMING  
  start = timeGetTime();  
#endif

  WORD counter=0;
  BOOL gameEvent = false;
  do
  {
    DETAILTRACE(0x83a2,0);
    if (!ExitSignaled)
      inputExists=m_pLocalCInput->GetInput(m_inputData);
      DETAILTRACE(0x83a3, inputExists);
    if ((!ExitSignaled) && (inputExists))
    {
      DETAILTRACE(0x83a4, 0);
      if (m_inputData.mouse.mouseMovement)
      {
        taskList.OnMouseMove(m_inputData.mouse.x, m_inputData.mouse.y);
      };
      // gameEvent not set.  Mouse movement has visual effect only!
      // No need to record it or do any game processing because of it.

      if ((!ExitSignaled) && (m_inputData.key.code!=KC_NONE))
      {
        taskList.OnKeypress(m_inputData.key); //m_inputData.key.code,m_inputData.key.key, m_inputData.key.vkey);
        gameEvent = true;
      };
      if ((!ExitSignaled) && (m_inputData.mouse.button1))
      {
        taskList.OnMouseClickLeft(m_inputData.mouse.x, m_inputData.mouse.y);
        gameEvent = true;
      };
      //if (m_inputData.mouse.button2)
      //{
      //  taskList.OnMouseClickRight(m_inputData.mouse.x, m_inputData.mouse.y);
      //};
      if ((!ExitSignaled) && (m_inputData.timer_expired))
      {
        taskList.OnTimer(m_inputData.timerId);
        gameEvent = true;
      };

      if ((!ExitSignaled) && (m_inputData.clockTick))
      {
        gameEvent = true;
      };
    }
    else if (gameEvent &!ExitSignaled)
    {
      DETAILTRACE(0x83a5,0);
      while (!taskList.OnIdle())
        ;  // While not needing input, loop until TRUE is returned

      vkey=simulatedKeyQueue.PopKey();
      if (vkey!=0xffffffff) 
      {
        KeySimulate(vkey);
        inputExists=TRUE;
      };
    };
    DETAILTRACE(0x83a8,0)
    if ((!ExitSignaled) && (miscError != NoError))
    {
      // really just formats the error text
      DETAILTRACE(0x83a6,0);
      displayMiscError(miscError);
      miscError = NoError;
    }
    counter++;
    ASSERT( counter < 100 ); // should not loop this much
  } while ((!ExitSignaled) && (inputExists));

  DETAILTRACE(0x83a9,0);
  if (gameEvent && !ExitSignaled)
  {
    DETAILTRACE(0x83ab,0);
    taskList.OnIdle();  
    DETAILTRACE(0x83ac,0);
    taskList.OnCycle();
    DETAILTRACE(0x83ad,0);
  }
  DETAILTRACE(0x83aa,0);

#ifdef TRACE_EVENT_TIMING
  stop = timeGetTime()-start;
  fprintf(pOutput, "procinp %I64u, event %u, used %u\n", 
          virtualGameTime, 
          (taskList.m_tasklist[0].m_event==NULL)?NoEvent:taskList.m_tasklist[0].m_event->GetEventType(),
          stop);
  if (ExitSignaled)
    fclose(pOutput);
#endif

  return TRUE;
}



//*****************************************************************************
//    NAME: CProcessInput::CProcessInput
//
// PURPOSE: 
//
//*****************************************************************************
CProcessInput::CProcessInput() : m_pLocalCInput(NULL)
{
  PartyList.RemoveAll();
  HaveWarp=FALSE;
  memset(&m_inputData, 0, sizeof(m_inputData));
  wx=wy=wl=wf=-1;
}

// First state used after design files are loaded
void CProcessInput::InitializeToSplash(void)
{
  taskList.RemoveAll();
  taskList.PushEvent(new EXIT_DATA, DeleteEvent);
  taskList.PushEvent(new SPLASH_DATA, DeleteEvent);
}

// Used when party dies to go back to main menu screen,
// or if splash screen not loaded during startup
void CProcessInput::InitializeToMain(void)
{
  taskList.RemoveAll();
  taskList.PushEvent(new EXIT_DATA, DeleteEvent);
  taskList.PushEvent(new MAIN_MENU_DATA(NULL), DeleteEvent);
}

// used when party dies to go back to main menu screen
void CProcessInput::InitializeToDesignSelect(void)
{
  taskList.RemoveAll();
  taskList.PushEvent(new EXIT_DATA, DeleteEvent);
#ifdef SIMPLE_STRUCTURE
#else
  taskList.PushEvent(new DESIGN_SELECT_MENU_DATA, DeleteEvent);
#endif
}

//*****************************************************************************
//    NAME: CProcessInput::AddPartyMember
//
// PURPOSE: 
//
//*****************************************************************************
void CProcessInput::AddPartyMember(CString PlayerCharacter)
{
  PartyList.AddTail(PlayerCharacter);
}

//*****************************************************************************
//    NAME: CProcessInput::SetWarp
//
// PURPOSE: 
//
//*****************************************************************************
void CProcessInput::SetWarp(int x, int y, int level, int facing)
{
  WriteDebugString("SetWarp to %i,%i on level %i, facing %i\n",
                   x,y,level,facing);
  HaveWarp = TRUE;
  wx=x;
  wy=y;
  wl=level;
  wf=facing;
}

//*****************************************************************************
// NAME:    CProcessInput::KeyPressed
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
void CProcessInput::KeyPressed(UINT vkey)
{
  if (m_pLocalCInput != NULL)
    m_pLocalCInput->KeyPressed(vkey);
}

//*****************************************************************************
//    NAME: CProcessInput::KeySimulate
//
// PURPOSE: 
//
//*****************************************************************************
void CProcessInput::KeySimulate(UINT vkey)
{
  if (m_pLocalCInput != NULL)
    m_pLocalCInput->KeySimulate(vkey);
}

//*****************************************************************************
// NAME:    CProcessInput::handleTimer
//
// PURPOSE: 
//
// RETURNS: None
//*****************************************************************************
BOOL CProcessInput::handleTimer(UINT timerId)
{
  // We need to pass the timer to the current event
  if (m_pLocalCInput != NULL)
    m_pLocalCInput->TimerExpired(timerId); 
  return TRUE;
}

//*****************************************************************************
//    NAME: CProcessInput::HandleMouseClick
//
// PURPOSE: 
//
//*****************************************************************************
void CProcessInput::HandleMouseClick(long x, long y)
{
  if (m_pLocalCInput != NULL)
    m_pLocalCInput->SetMouseLeftClick(x, y);
}

//*****************************************************************************
//    NAME: CProcessInput::HandleClockTick
//
// PURPOSE: 
//
//*****************************************************************************
void CProcessInput::HandleClockTick(void)
{
  if (m_pLocalCInput != NULL)
    m_pLocalCInput->SetClockTick();
}

//*****************************************************************************
//    NAME: CProcessInput::HandleMouseMove
//
// PURPOSE: 
//
//*****************************************************************************
void CProcessInput::HandleMouseMove(long x, long y)
{
  if (m_pLocalCInput != NULL)
    m_pLocalCInput->SetMouseMove(x, y);
}

//*****************************************************************************
// NAME: BackupPartyOneStep
//
// PURPOSE:  
//*****************************************************************************
void CProcessInput::BackupPartyOneStep()
{
  party.movePartyBackward();
}

//*****************************************************************************
//    NAME: CProcessInput::GetCurrEventType
//
// PURPOSE: 
//
//*****************************************************************************
eventType CProcessInput::GetCurrEventType(void)
{
  return (/*(eventType)*/taskList.GetEventType());
}

SIMULATEDKEYQUEUE::SIMULATEDKEYQUEUE(void)
{
  for (int i=0; i<10; i++) keystrokes[i]=0xffffffff;
}

UINT SIMULATEDKEYQUEUE::PopKey(void)
{
  UINT temp;
  temp=keystrokes[0];
  for (int i=0; i<9; i++) keystrokes[i]=keystrokes[i+1];
  keystrokes[9]=0xffffffff;
  return temp;
}

void SIMULATEDKEYQUEUE::PushKey(UINT key)
{
  for (int i=9; i>0; i--) keystrokes[i]=keystrokes[i-1];
  keystrokes[0]=key;
}



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
// In order to do this it is necessary to capture the time
// of each input.  And to make that practical, it is necessary
// that all input be routed through one place.  This includes
// the passage of time.
//
// virtualGameTime is kept in units of (virtualGametimeUnits) milliseconds.
// There is no reason to attempt to keep virtualGameTime in 
// milliseconds except as a kind of convenience to avoid conversions
// in the code and in the mind.  We could just as well (and perhaps
// should) simply say  virtualGameTime++.
// Each time interval is treated as follows:
// 1 - Increment virtualGameTime
// 2 - Call scheduler()
// 3 - For each queued message during that interval
//     a - post the message
//     b - call scheduler();
//
// 10 March 2002 Paul Stevens
// Initial definition of class INPUTQUEUE.
// Route CMainFrame::OnKeyDown messages through the INPUTQUEUE.
//
//

void scheduler(void);
extern LONGLONG gameTime;

INPUTQUEUE inputQueue;


unsigned int RecordHash(FILE *f)
{
  int len;
  int index;
  unsigned int result;
  unsigned char buf[4096];
  result = 111;
  while ((len = fread(buf,1,4096,f)) > 0)
  {
    index = 0;
    while (len > 3)
    {
      result += *(unsigned int *)(buf+index);
      result = result *69069 + 1;
      len -= 4;
      index += 4;
    };
    while (len)
    {
      result += *(buf+index);
      result = result * 69069 + 1;
      len -= 1;
      index += 1;
    };
  };
  return result;
}



PLAYBACKFILE::PLAYBACKFILE(void)
{
  m_file = NULL;
  m_inputEOF = false;
  m_dataReady = false;
  m_fastPlay = 0;
}

PLAYBACKFILE::~PLAYBACKFILE(void)
{
  if (m_file != NULL) fclose(m_file);
}

void PLAYBACKFILE::Open(const CString& filename)
{
  WriteDebugString("PLAYBACKFILE::Open(%s); m_file=%s\n", filename, m_file);
  if (m_file != NULL) return;
  if (filename.IsEmpty())
  {
    m_file = fopen("Playfile.log","r");
  }
  else
  {
    m_file = fopen(filename, "r");
  };
  if (m_file == NULL) return;
  m_dataReady = false;
  m_inputEOF = false;
  m_fastPlay = 0;
}

void PLAYBACKFILE::Close(void)
{
  if (m_file == NULL) return;
  fclose(m_file);
  m_file = NULL;
  m_dataReady = false;
  m_fastPlay = 0;
}

bool PLAYBACKFILE::m_ProcessCommand(char *line)
{ //Return true if playback should continue.
  char *inputLine;
  inputLine = line;
  char fields[10][80];
  int numField, col;
  int len;
  len = strlen(line);
  for (numField=0; numField<10; numField++)
  {
    //Skip leading blanks.
    while (len > 0)
    {
      if (   (*line != ' ')
          && (*line != '\t')
          && (*line != '\n')
         ) break;
      line++;
      len--;
      continue;
    };
    if (len == 0) break; //No more fields
    col = 0;
    while (len > 0)
    {
      if (   (*line == ' ')
          || (*line == '\t')
          || (*line == '\n')
         ) break;
      fields[numField][col] = *line;
      col++;
      line++;
      len--;
      if (col >= 79) break;
    };
    fields[numField][col] = 0;
  }; //For numField
  if (numField == 0) return true;
  if (strcmp(fields[0],"#VERSION") == 0)
  {
    if (numField < 2) return false;
    char version[20];
    double ver = ENGINE_VER;
    sprintf(version,"%+016.10f", ver);
    if (strcmp(version, fields[1]) != 0)
    {
      CString msg;
      msg.Format("The recording was created with version:\n"
                  "%s\n"
                  "but the current runtime engine is:\n"
                  "%s\n\n"
                  "Things may not work very well.",fields[1], version);
      MsgBoxInfo(msg, "Warning");
      return false;
    }
    return true;
  };
  if (strcmp(fields[0], "#SEEDMT") == 0)
  {
    if (numField < 3) return true;
    DWORD randomSeed;
    unsigned int hash;
    if (sscanf(fields[1],"%08x",&randomSeed) != 1) return true;
    seedMT(randomSeed);
    if (sscanf(fields[2],"%08x",&hash) != 1) return true;
    taskList.Hash(hash);
    return true;
  };
  if (stricmp(fields[0], "#HASH") == 0)
  {
    FILE *f;
    unsigned int hashf, hashr;
    if (sscanf(fields[3], "%08x", &hashr) != 1)
    {
      CString msg;
      if (strstr(inputLine,"*FileMissing*") == NULL)
      {
        msg.Format("Unintelligible hash record in playback file\n%s",inputLine); 
        MsgBoxInfo(msg,"Playback Error");
      };
      return true;
    };
    f = fopen(rte.DesignDir() + fields[1], "rb");
    if (f == NULL)
    {
      CString msg;
      msg.Format("Cannot open file '%s' to check hash code", fields[1]), 
      MsgBoxInfo(msg, "Playback Error");
      return true;
    };
    hashf = RecordHash(f);
    fclose(f);
    if (hashr != hashf)
    {
      CString msg;
      msg.Format("File '%s' has changed since recording was created", fields[1]);
      MsgBoxInfo(msg, "Playback Error");
      return true;
    };
    return true;
  };
  if (stricmp(fields[0], "#FASTPLAY") == 0)
  {
    if (numField < 2) return true;
    DWORD temp;
    if (sscanf(fields[1],"%08x",&temp) != 1) return true;
    m_fastPlay = temp;
    return true;
  };
  if (stricmp(fields[0], "#EditorRunning") == 0)
  {
    if (stricmp(fields[1], "YES") == 0)
    {
      EditorRunning = TRUE;
    }
    else if (stricmp(fields[1], "NO") == 0)
    {
      EditorRunning = FALSE;
    }
    else return true; // Treat as comment
  };
  if (stricmp(fields[0], "#PretendEditorRunning") == 0)
  {
    pretendEditorRunning = true;
    pretendEditorNotRunning = false;;
  };
  if (stricmp(fields[0], "#PretendEditorNotRunning") == 0)
  {
    pretendEditorNotRunning = true;
    pretendEditorRunning = false;;
  };
  return true; //treat as a comment
};


void PLAYBACKFILE::Read(INPUTQUEUE_ENTRY *ent)
{
  ent->m_entryType = INPUTQUEUE_ENTRY::IQE_UNKNOWN;
  if ( m_inputEOF && (m_file!=NULL))
  {
    Close();
  };
  if (m_file == NULL) return;
  while (!m_dataReady) // We may use this for backspace someday.
  {
    if (m_inputEOF) return;
    if (fgets(m_inputLine, 80, m_file) != NULL)
    {
      if (m_inputLine[0] == '#')
      {
        m_ProcessCommand(m_inputLine); //Perhaps a comment.
        continue;
      };
      if (sscanf(m_inputLine,
                 "%016I64x %02x %08x %08x",
                 &m_lineTime,
                 &m_lineType,
                 &m_lineData,
                 &m_lineHash) == 4) 
      {
        m_dataReady = true;
      }
      else
      {
        m_inputEOF = true;
        return;
      };
    };
    if (!m_dataReady) return;
    if (m_lineType == 0xff)
    {
      virtualGameTime = m_lineTime;
      m_dataReady = false;
      continue;
    };
  };
  ASSERT(m_lineTime >= virtualGameTime);
  if (m_lineTime < virtualGameTime)
  {
    MessageBox(NULL,"Timing Error","Playback Error",MB_OK|MB_TOPMOST);
    Close();
    return;
  };
  if (m_lineTime > virtualGameTime) return;





  ASSERT(m_lineHash == taskList.Hash());
  if (m_lineHash != taskList.Hash())
  {
    CString msg;
    msg.Format("Sequence Error\nvirtualTime=0x%08I64x\nm_lineHash = 0x%08x\ntaskList.Hash() = 0x%08x",
                virtualGameTime,
                m_lineHash,
                taskList.Hash()
              );
    MessageBox(NULL,msg,"Playback Error",MB_OK|MB_TOPMOST);
    //Close();
    //return;
    taskList.Hash(m_lineHash);
  };
  ent->m_entryType = 
         (INPUTQUEUE_ENTRY::INPUTQUEUE_ENTRY_TYPE)m_lineType;
  ent->m_data = m_lineData;
  ent->m_timeOfEntry = m_lineTime;
  m_dataReady = false;
  return;
}

bool PLAYBACKFILE::IsEntryAvailable(void)
{
  if ( m_inputEOF && (m_file!=NULL))
  {
    Close();
  };
  if (m_file == NULL) return false;
  while (!m_dataReady) // We may use this for backspace someday.
  {
    if (m_inputEOF) return false;
    if (fgets(m_inputLine, 80, m_file) != NULL)
    {
      if (m_inputLine[0] == '#')
      {
        m_ProcessCommand(m_inputLine); //Perhaps a comment.
        continue;
      };
      if (sscanf(m_inputLine,
                 "%016I64x %02x %08x %08x",
                 &m_lineTime,
                 &m_lineType,
                 &m_lineData,
                 &m_lineHash) == 4) 
      {
        m_dataReady = true;
      }
      else
      {
        m_inputEOF = true;
        return false;
      };
    }
    else
    {
      m_inputEOF = true;;
    };
    if (!m_dataReady) return false;
    if (m_lineType == 0xff)
    {
      virtualGameTime = m_lineTime;
      m_dataReady = false;
      continue;
    };
  };
  ASSERT(m_lineTime >= virtualGameTime);
  if (m_lineTime > virtualGameTime) return false;

  return true;
}


bool PLAYBACKFILE::IsPlaying(void)
{
  return m_file != NULL;
}


bool FastPlayback(void)
{
  return inputQueue.FastPlay() != 0;
}

RECORDFILE::RECORDFILE(void)
{
  m_file = NULL;
}

RECORDFILE::~RECORDFILE(void)
{
  if (m_file != NULL) fclose(m_file);
  m_file = NULL;
}


bool RECORDFILE::IsRecording(void)
{
  return m_file != NULL;;
}


void RECORDFILE::PrintRecordHash(const CString& folder, const CString& fname)
{
  FILE *f;
  unsigned int hash;
  f = fopen(rte.DesignDir()+folder+'\\'+fname,"rb");
  if (f == NULL)
  {
    fprintf(m_file,"#HASH %s = *FileMissing*\n", (LPCSTR)(folder+'\\'+fname));
    return;
  };
  hash = RecordHash(f);
  fprintf(m_file, "#HASH %s = %08x\n", (LPCSTR)(folder+'\\'+fname), hash);
  fclose(f);
}

#define MaxRecord 999
void RECORDFILE::Open(void)
{
  int recordNum;
  char recordName[20];
  if (m_file != NULL) return;
  for (recordNum=0; recordNum<=MaxRecord; recordNum++)
  {
    sprintf(recordName,"Record%03d.log",recordNum);
    m_file = fopen(recordName,"r");
    if (m_file == NULL) break;
    fclose(m_file);
    m_file = NULL;
  };
  if (recordNum <= MaxRecord) 
  {
    m_file = fopen(recordName,"w");
    if (m_file == NULL) return;
    double ver = ENGINE_VER;
    sprintf(recordName,"%+016.10f", ver);
    fprintf(m_file,
            "#VERSION %s Remove this line to ignore version\n",
            recordName);
    fprintf(m_file,"#EditorRunning %s\n", EditorRunning?"YES":"NO");
    fprintf(m_file,"%016I64x ff 00000000 00000000\n",virtualGameTime);
    DWORD randomSeed = GetTickCount();
    seedMT(randomSeed);
    fprintf(m_file, "#SEEDMT %08x %08x\n",randomSeed, taskList.Hash());
    fprintf(m_file, "##Remove one '#' from following line to replay fast\n");
    fprintf(m_file, "##FASTPLAY 1\n");
  }
  else
  {
    MessageBox(NULL,"There are too many recordings already.  No recording will be created","Warning",MB_OK);
  };
  if (m_file != NULL)
  {
    int handle;
    int result;
    _finddata_t findData;
    PrintRecordHash("data", "config.txt");
    PrintRecordHash("data", "game.dat");
    PrintRecordHash("data", "ability.dat");
    PrintRecordHash("data", "baseclass.dat");
    PrintRecordHash("data", "classes.dat");
    PrintRecordHash("data", "items.dat");
    PrintRecordHash("data", "monsters.dat");
    PrintRecordHash("data", "races.dat");
    PrintRecordHash("data", "specialAbilities.dat");
    PrintRecordHash("data", "spells.dat");
    PrintRecordHash("data", "AI_Script.BLK");
    handle = _findfirst(rte.DataDir() + "*.lvl", &findData);
    result = handle >= 0;
    while (result)
    {
      PrintRecordHash("data", findData.name);
      result = _findnext(handle, &findData) >= 0;
    };
  };
}

void RECORDFILE::Write(INPUTQUEUE_ENTRY *ent)
{

  if (m_file != NULL)
  {
    fprintf(m_file,
            "%016I64x %02x %08x %08x\n",
            virtualGameTime,
            ent->m_entryType,
            ent->m_data,
            taskList.Hash()); 
    fflush(m_file);
  };
}








INPUTQUEUE_ENTRY::INPUTQUEUE_ENTRY(void)
{
  m_nextEntry   = -1;
  m_timeOfEntry = 0;
  m_entryType   = IQE_UNKNOWN;
  m_data = 0;
}

INPUTQUEUE_ENTRY::~INPUTQUEUE_ENTRY(void)
{
}


void INPUTQUEUE_ENTRY::destruct(void)
{
  switch (m_entryType)
  {
  case IQE_ON_KEY_DOWN:
  case IQE_ON_LBUTTON_DOWN:
  case IQE_ON_MOUSEMOVE:
    break; // These are easy
  default:
    die(0xac519);
    ASSERT(false);
  };
  m_entryType = IQE_UNKNOWN;
  m_nextEntry = -1;
  m_data = 0;
}

INPUTQUEUE::INPUTQUEUE(void)
{
  int i;
  m_firstInQueue = -1;
  m_lastInQueue  = -1;
  m_available    = -1;
  m_recordOK     = true;
  m_playOK       = true;
  m_previousTime = 0;
  for (i=0; i<INPUTQUEUE_MAX_ENTRIES; i++)
  {
    m_queue[i].m_entryType = INPUTQUEUE_ENTRY::IQE_UNKNOWN;
    m_queue[i].m_nextEntry = -1;
    m_releaseEntry(i);
  };
}

INPUTQUEUE::~INPUTQUEUE(void)
{
  int i;
  while ((i=m_removeEntry()) != -1) 
  {
    m_queue[i].destruct();
    m_releaseEntry(i);
  };
}

void INPUTQUEUE::m_addEntry(int i)
{ // Add entry to end of queue
  ASSERT(i >= 0);
  ASSERT(i < INPUTQUEUE_MAX_ENTRIES);
  ASSERT(m_queue[i].m_nextEntry == -1);
  if (m_firstInQueue == -1)
  {
    m_firstInQueue = i;
  }
  else
  {
    ASSERT(m_queue[m_lastInQueue].m_nextEntry == -1);
    m_queue[m_lastInQueue].m_nextEntry = i;
  };
  m_lastInQueue = i;
}

int INPUTQUEUE::m_removeEntry(void)
{ //Remove the first entry from queue.
  int result;
  if (m_playFile.IsPlaying())
  {
    int index;
    index = inputQueue.m_allocateEntry();
    if (index == -1) return -1;
    m_playFile.Read(&inputQueue.m_queue[index]);

    if (inputQueue.m_queue[index].m_entryType
                  == INPUTQUEUE_ENTRY::IQE_UNKNOWN)
    {
      inputQueue.m_releaseEntry(index);
      m_goalTime = virtualGameTime;
      return -1;
    };
    return index;
  };                           
  if (m_firstInQueue == -1) return -1;
  result = m_firstInQueue;
  ASSERT(result >= 0);
  ASSERT(result < INPUTQUEUE_MAX_ENTRIES);
  m_firstInQueue = m_queue[result].m_nextEntry;
  if (m_firstInQueue == -1) m_lastInQueue = -1;
  return result;
}

void INPUTQUEUE::StartPlayfile(const CString& filename)
{
  if (!m_playOK) return;
  m_playFile.Open(filename);
  if (m_playFile.IsPlaying())
  {
    m_playOK = false;
  }
  else
  {
    WriteDebugString("Unsuccessful attempt to open playvack file \"%s\"\n", filename);
  };
}

bool INPUTQUEUE::m_isEntryAvailable(void)
{
  if (m_playFile.IsPlaying())
  {
    return m_playFile.IsEntryAvailable();
  };
  return m_firstInQueue != -1;
}

int INPUTQUEUE::m_allocateEntry(bool important)
{ // Return -1 if no entry available.
  // Right now we ignore the 'important' parameter.  But we may have 
  // to add an important field to the queue entries and
  // be prepared to discard non-important entries to make
  // room for important entries.
  // We could also allocate entries from the heap and thereby
  // make room for a very large number of them.  I don't think
  // this is a good idea.
  int result;
  ASSERT(m_available != -1);
  if (m_available == -1) return -1;
  result = m_available;
  ASSERT(result >= 0);
  ASSERT(result < INPUTQUEUE_MAX_ENTRIES);
  m_available = m_queue[result].m_nextEntry;
  m_queue[result].m_nextEntry = -1;
  return result;
}

void INPUTQUEUE::m_releaseEntry(int i)
{ // Put into available list
  ASSERT(i >= 0);
  ASSERT(i < INPUTQUEUE_MAX_ENTRIES); 
  ASSERT(m_queue[i].m_entryType == INPUTQUEUE_ENTRY::IQE_UNKNOWN);
  ASSERT(m_queue[i].m_nextEntry == -1); //Destruct it before releasing it!!!
  m_queue[i].m_nextEntry = m_available;
  m_available = i;
}

int INPUTQUEUE::m_adjustGoal(void)
{ //returns distance to goal
  DWORD curTime;
  int distanceToGoal;
  int elapsedTime;
  curTime = timeGetTime();
  elapsedTime = curTime - m_previousTime; // A signed value!
  m_previousTime = curTime;
  if (elapsedTime < 0) elapsedTime = 0; //Might happen first time.
  m_goalTime += elapsedTime;
  distanceToGoal = m_goalTime - virtualGameTime;
  // We can chop virtualGametimeUnits off the distance
  // each time around this loop.  Of course, we may add 
  // some distance in the process!
  if (distanceToGoal < 0) distanceToGoal = 0;
  //Average over a few periods.  But not too many.
  if (distanceToGoal >= 5*virtualGametimeUnits)
  {
    distanceToGoal = 5*virtualGametimeUnits;
    m_goalTime = virtualGameTime + distanceToGoal;
  };
  return distanceToGoal;
}


//#define TRACE_TIMER_DETAILS


#ifdef TRACE_TIMER_DETAILS

DETAIL_TRACE::DETAIL_TRACE(void)
{
//  m_nextIndex = 0;
//  memset(m_id, 0, sizeof m_id);
//  f = fopen("detailTrace.txt","w");
}

DETAIL_TRACE::~DETAIL_TRACE(void)
{
//  if (f != NULL) fclose(f);
//  f = NULL;
}

void DETAIL_TRACE::trace(DWORD id, DWORD data)
{
//  if (id != 0xffffffff)
//  {
//    m_id[m_nextIndex] = id;
//    m_data[m_nextIndex] = data;
//    m_time[m_nextIndex] = timeGetTime();
//    m_virtualTime[m_nextIndex] = (DWORD)virtualGameTime;
//    m_nextIndex = (m_nextIndex +1)%1000;
//  }
//  else
//  {
//    int i = m_nextIndex;
//    do
//    {
//      if (m_id[i] != 0)
//      {
//        WriteDebugString("%08x %08x %08x %08x\n",
//                         m_time[i],m_virtualTime[i],m_id[i], m_data[i]);
//      };
//      i = (i+1)%1000;
//    } while (i != m_nextIndex);
  if ((virtualGameTime > 0x16600) && (virtualGameTime < 0x16700))
  {
    //if (f != NULL) 
    {
      CString line;
      line.Format("%08x %08x %08x %08x\n",
                          timeGetTime(),(DWORD)virtualGameTime,id, data);
      taskList.DetailTrace(line);
    };
  };
//  };
}

DETAIL_TRACE detailTrace;

#endif //TRACE_TIMER_DETAILS

int globalLoggingFlags;



#ifdef TRACE_TIMER_DETAILS
#define DETAILTRACE(a,b) detailTrace.trace((a),(b));
#else
#define DETAILTRACE(a,b)
#endif

void StrangeMessage(int msg)
{
  //DETAILTRACE(0x3412a, msg);
}


DWORD INPUTQUEUE::DistributeInput(void)
{
  // We will call scheduler only if:
  //   1 - A queue entry has been removed and processed.
  //   2 - virtualGameTime has incremented.
  // It is silly to call scheduler for any other
  // reason.  If nothing has changed then nothing needs to
  // be done!  Otherwise, we need to fix scheduler so that
  // it does everything that needs doing at any one instant
  // of time.
  //
  // Return value: how long to wait before calling again.
  int n, distanceToGoal;
  BOOL forceAdvanceTime = FALSE;
  DWORD result = virtualGametimeUnits;
  //DETAILTRACE(0xed1,0); // Enter DistributeInput
  if ((n = m_removeEntry()) != -1)
  {
    switch(m_queue[n].m_entryType)
    {
    case INPUTQUEUE_ENTRY::IQE_ON_KEY_DOWN:
      m_recordFile.Write(&m_queue[n]);
      forceAdvanceTime = TRUE;
      DETAILTRACE(0xd10dc,m_queue[n].m_data); //Distribute input On Down Call
      ProcInput.KeyPressed(m_queue[n].m_data);
      DETAILTRACE(0xd10d0,0); //Distribute input On Down Out
      break;
    case INPUTQUEUE_ENTRY::IQE_ON_LBUTTON_DOWN:
      m_recordFile.Write(&m_queue[n]);
      forceAdvanceTime = TRUE;
      DETAILTRACE(0xd10dc1,m_queue[n].m_data); //Distribute input On LButton Call
      ProcInput.HandleMouseClick(
                     (m_queue[n].m_data>>16)&0xffff, //x 
                      m_queue[n].m_data&0xffff);     //y
      DETAILTRACE(0xd10d01,0); //Distribute input On LButton Out
      break;
    case INPUTQUEUE_ENTRY::IQE_ON_MOUSEMOVE:
      {
        // No need to write moue moves to record file because it only affects display.
        ProcInput.HandleMouseMove(
                     (m_queue[n].m_data>>16)&0xffff, //x 
                      m_queue[n].m_data&0xffff);     //y
      };
      break;
    default:
      die(0xac51a);
    };
    m_queue[n].destruct();
    m_releaseEntry(n);
    DETAILTRACE(0xd15ca,0);//Distribute Input Scheduler Call a
    scheduler();
    DETAILTRACE(0xd150a,0);//Distribute Input Scheduler Out a
    if (ExitSignaled) return 0;
  };
  distanceToGoal = m_adjustGoal();
  if (    (distanceToGoal >= virtualGametimeUnits) 
       || this->m_playFile.FastPlay()
       || forceAdvanceTime)
  {
    virtualGameTime += virtualGametimeUnits;
    distanceToGoal -= virtualGametimeUnits;
    DETAILTRACE(0xd15cb,0); // Distribute Input Scheduler Call b
    ProcInput.HandleClockTick();
    DETAILTRACE(0xd15ab,0);
    {
      scheduler();  //Somebody may have added something to the queue.
    };
    DETAILTRACE(0xd150b,0);// Distribute Input Scheduler Out b
    distanceToGoal = m_adjustGoal();
  }
  else
  {
  };
  if ((distanceToGoal >= virtualGametimeUnits) || this->m_playFile.FastPlay())
  {
    result = 0;
  }
  else
  {
    result = virtualGametimeUnits - distanceToGoal;
  };  
  if (m_isEntryAvailable())
  {
    result = 0;
  };
  // Now you may find that some Windows messages are not handled
  // as quickly as they ought to be.  That is probably because they
  // are not being processed by inputQueue and therefore they only
  // get processed once every 30 milliseconds.
  //DETAILTRACE(0x4432, result);
  return result;
} 



class LoggingDlg:public CDialog
{
public:
  LoggingDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL):CDialog(nIDTemplate, pParentWnd){};
//  BOOL OnInitDialog(void);
//  void OnOK(void);
  DECLARE_MESSAGE_MAP()
//  afx_msg void OnButton12(void);
//  afx_msg void OnButton13(void);
//  afx_msg void OnButton14(void);
//  afx_msg void OnButton15(void);
//  afx_msg void OnButton16(void);
//  afx_msg void OnButton17(void);
  void LoggingDlg::DoDataExchange(CDataExchange* pDX);
  BOOL m_logAllCheck;
public:
  int m_loggingFlags;
};


void LoggingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacter)
	DDX_Check(pDX, IDC_AllSAScripts, m_logAllCheck);
	//}}AFX_DATA_MAP
  if (pDX->m_bSaveAndValidate)
  {
    m_loggingFlags = 0;
    if (m_logAllCheck) m_loggingFlags |= 1;
  };
};





BEGIN_MESSAGE_MAP(LoggingDlg,CDialog)
//  ON_BN_CLICKED(IDC_GPDLBUTTON12, OnButton12)
//  ON_BN_CLICKED(IDC_GPDLBUTTON13, OnButton13)
//  ON_BN_CLICKED(IDC_GPDLBUTTON14, OnButton14)
//  ON_BN_CLICKED(IDC_GPDLBUTTON15, OnButton15)
//  ON_BN_CLICKED(IDC_GPDLBUTTON16, OnButton16)
//  ON_BN_CLICKED(IDC_GPDLBUTTON17, OnButton17)
END_MESSAGE_MAP()








void INPUTQUEUE::OnKeyDown(unsigned int nChar)
{
  static bool CONTROL = false;
  int i;
  if (CONTROL && (nChar == 0x4c)) //'L'
  {
    LoggingDlg logDlg(IDD_LoggingDialog, NULL);
    logDlg.m_loggingFlags = globalLoggingFlags;
    if (logDlg.DoModal() == IDOK)
    {
      globalLoggingFlags = logDlg.m_loggingFlags;
    };
  };
  if (CONTROL && (nChar == 0x52)) //'R'
  { //Start recording
    if (m_recordOK)
    {
      m_recordFile.Open();
      m_recordOK = false;
    };
  };
  if (CONTROL && (nChar == 0x50)) // 'P'
  { //Start playback
    WriteDebugString("INPUTQUEUE Ctrl-P; m_playOK = %d\n",m_playOK);
    if (m_playOK)
    {
      m_playFile.Open("");
      m_playOK = false;
      m_recordOK = false;  // This fixed playback errors when mouse clicks occurred!!
      return;
    };
  };
#ifdef TASKTRACE
  if (CONTROL && (nChar == 0x54)) // 'T'
  { // Toggle Task Tracing
    taskList.ToggleTrace();
    return;
  };
#endif
  CONTROL = nChar == 0x11;
  if (CONTROL) return;
#define AUTORECORD
#ifdef AUTORECORD
  if (m_recordOK)
  {
    m_recordFile.Open();
    m_recordOK = false;
  };
#endif
  if (m_playFile.IsPlaying())
  {
    if (nChar == 0x1b) 
    {
      m_playFile.Close();
      DETAILTRACE(0xffffffff,0);
      return;
    };
  };
  i = m_allocateEntry();
  if (i==-1) return; // Sorry about that.
  m_queue[i].m_data = nChar;
  m_queue[i].m_entryType = INPUTQUEUE_ENTRY::IQE_ON_KEY_DOWN;
  m_addEntry(i);
}

void INPUTQUEUE::OnLButtonDown(int x, int y)
{
  int i;
  i = m_allocateEntry();
  if (i==-1) return; // Sorry about that.

#ifdef AUTORECORD
  if (m_recordOK)
  {
    m_recordFile.Open();
    m_recordOK = false;
  };
#endif

  m_queue[i].m_data = ((x&0xffff) << 16) | (y&0xffff);
  m_queue[i].m_entryType = INPUTQUEUE_ENTRY::IQE_ON_LBUTTON_DOWN;
  m_addEntry(i);
}

void INPUTQUEUE::OnMouseMove(UINT flags, CPoint point)
{
  int i;
  i = m_firstInQueue;
  if ( (i == -1) || (m_queue[i].m_entryType != INPUTQUEUE_ENTRY::IQE_ON_MOUSEMOVE))
  {
    i = m_allocateEntry();
    if (i == -1) return;  // No big loss.
    m_queue[i].m_entryType = INPUTQUEUE_ENTRY::IQE_ON_MOUSEMOVE;
    m_queue[i].m_data = ((point.x&0xffff) << 16) | (point.y&0xffff);
    m_addEntry(i);
  };
}
