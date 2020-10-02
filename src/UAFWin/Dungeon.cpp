/******************************************************************************
* Filename: Dungeon.cpp
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
#include "..\shared\stdafx.h"
//#include "..\Shared\ProjectVersion.h"
//#include "externs.h"
#include "Dungeon.h"
#include "DgnGame.h"
#include "MainFrm.h"
#include "Graphics.h"
#include "SoundMgr.h"
#include "GetInput.h"
#include "GlobalData.h"
#include "CProcInp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CString cmdPlayfileName;

int ExitSignaled = 0;
//#ifdef _DEBUG
//BOOL EditorRunning = TRUE;
//#else
BOOL EditorRunning = FALSE;
BOOL EngineRunning = FALSE;
bool pretendEditorRunning = false;
bool pretendEditorNotRunning = false;

extern const double ENGINE_VER;


bool EditorMode(void)
{
  return (EditorRunning && !pretendEditorNotRunning) || pretendEditorRunning;
}

//#endif

/////////////////////////////////////////////////////////////////////////////
// CDungeonApp

BEGIN_MESSAGE_MAP(CDungeonApp, CWinApp)
//{{AFX_MSG_MAP(CDungeonApp)
//}}AFX_MSG_MAP
// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDungeonApp construction

CDungeonApp::CDungeonApp()
{
  m_EventTimerId=0xFFFFFFFF;
  m_SleepEvent=NULL;
  m_TimerResolution=0;

  // Enabling these flags can be helpful for
  // those mysterious HEAP exceptions, but 
  // program execution is 15-20 times slower
  //
  //int debugFlags = 0;
  //debugFlags |= _CRTDBG_CHECK_ALWAYS_DF;
  //debugFlags |= _CRTDBG_CHECK_CRT_DF;
  //debugFlags |= _CRTDBG_LEAK_CHECK_DF;
  //debugFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
  //debugFlags |= _CRTDBG_ALLOC_MEM_DF;
  //_CrtSetDbgFlag(debugFlags);
}

HANDLE m_hAppEvent=NULL;

/////////////////////////////////////////////////////////////////////////////
// The one and only CDungeonApp object

CDungeonApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDungeonApp initialization

#ifdef CDX_LOG
extern CDXLog g_cdxlog;
#endif


BOOL CDungeonApp::InitInstance()
{
  // only allow one instance of the editor to run at a time
#ifdef CDX_LOG
  g_cdxlog.LogType(0);  // Wait to see if config.txt asks for cdx logging.
#endif
  m_hAppEvent = CreateEvent(NULL, FALSE, FALSE, ENGINE_APP_EVENT);
  if (ERROR_ALREADY_EXISTS == GetLastError())
  {
    MsgBoxError("An instance of the engine is running already");
    CloseHandle(m_hAppEvent);
    m_hAppEvent=NULL;
    return FALSE;
  }

  m_SleepEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
  TIMECAPS tc;
  if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
    ASSERT(0);
  m_TimerResolution = min(max(tc.wPeriodMin, 1), tc.wPeriodMax);

//#ifdef _DEBUG
//  EditorRunning = TRUE;
//#else
  EditorRunning = FALSE;
  EngineRunning = TRUE;
//#endif

  HANDLE tempHANDLE = CreateEvent(NULL, FALSE, FALSE, EDITOR_APP_EVENT);
  if (ERROR_ALREADY_EXISTS == GetLastError())
    EditorRunning = TRUE;    

  if (tempHANDLE != NULL)
    CloseHandle(tempHANDLE);

  ParseCommandLine( cmdLine );

  CString designDir;
  rte.Clear();


  if (cmdLine.m_strFileName.GetLength() > 0)
  {
    int len = cmdLine.m_strFileName.GetLength();
    
    if (len <= 2)
      return FALSE;

    designDir = cmdLine.m_strFileName;

  }
  else
  {
    // all resources are loaded from same folder that
    // the engine exec is in.
    char tmp[_MAX_PATH+1];
    GetModuleFileName(NULL, tmp, _MAX_PATH);
    StripOneFolderFromFilenamePath(tmp, designDir);
    
  }

  // Create a default game directory
  rte.DefaultFoldersFromDesign(designDir);

  WriteDebugString("Engine Version: %2.10f\n", ENGINE_VER);
  
  
  /*
  strcpy(global_dataDir, global_designDir);
  strcat(global_dataDir, "Data\\");

  strcpy(global_configDir,  global_designDir);
  strcat(global_configDir,  "Data\\");

  strcpy(global_saveDir,  global_designDir);
  strcat(global_saveDir,  "Saves\\");

  strcpy(global_soundDir  , global_designDir);
  strcat(global_soundDir  , "Resources\\");

  strcpy(global_artDir    , global_designDir);
  strcat(global_artDir    , "Resources\\");
  */
  WriteDebugString("Working directory = %s\n", designDir);

  // Standard initialization
  globalData.fontLib.LoadFonts();
  if (!LoadConfigFile(rte.ConfigDir()+"config.txt"))
  {
    debugSeverity = 8;
    WriteDebugString("Failed to load config.txt file in InitInstance()\n");
    //return FALSE;
  }
  
  // Don't create a doc template, just load the frame
  CMainFrame* pMainFrame = new CMainFrame;
  int w,h;
  
  w = SCREEN_WIDTH;
  h = SCREEN_HEIGHT;

  WriteDebugString("CreateEx(0,0,%d,%d)\n", w, h);
  if (!pMainFrame->CreateEx(WS_EX_TOPMOST,
                            pMainFrame->winClassName,
                            "",
                            WS_POPUP | WS_VISIBLE, 
                            0,0,
                            w,h,
                            AfxGetMainWnd()->GetSafeHwnd(), 
                            NULL))
    return FALSE;
  {
    RECT r;
    GetClientRect(AfxGetMainWnd()->GetSafeHwnd(), &r);
    WriteDebugString("CreateEx ClientRect = %d %d %d %d\n", r.left, r.top, r.right, r.bottom);
  };
  
#ifdef _DEBUG
  pMainFrame->ShowWindow(SW_SHOWNORMAL);
#else
  if (RunFullscreen)
  {
    pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
  }
  else
  {
    pMainFrame->ShowWindow(SW_SHOWNORMAL);
  };

#endif

  pMainFrame->UpdateWindow();
  pMainFrame->SetFocus();
  m_pMainWnd = pMainFrame;
    
  if (!CheckDxInstall())
    return FALSE;
  
  if (!InitGame(AfxGetMainWnd()->GetSafeHwnd()))
  {
    ProcInput.AllowInput(TRUE);
    GraphicsMgr.Clear();
    if (pSndMgr != NULL)
    {
      delete pSndMgr;
      pSndMgr = NULL;
    }
    
    WriteDebugString("Failed to initialize game data - miscError %u\n", miscError);
    if (miscError == DesignVersionError)
    {
       // version in design file is greater than the
       // version of this build, and it was rejected.
       CString errorMsg;
       errorMsg.Format("You are trying to load a design saved with newer version of UAF\n"
                       "which is not compatible with your current UAF version %4.7f.\n"
                       "You must upgrade UAF to load this design",
                       ENGINE_VER);
       MsgBoxError(errorMsg, "Design Version Incompatible");     
    }
    else
    {
      // this dialog may be hidden by DirectX buffer, depending on how far along the init got.
      // if it can't be seen the user will think the game has hung.
      MsgBoxError("Failed to initialize game data\nPlease check error log for problem indication", 
                   "Game Init Error");
    }
    return FALSE;
  }
  else
  {
    if ((EditorRunning) && (globalData.version > 0.0))
    {
      if ((cmdLine.m_sx >= 0) && (cmdLine.m_sy >= 0) && (cmdLine.m_level >= 0))
      {
        ProcInput.SetWarp(cmdLine.m_sx, cmdLine.m_sy, cmdLine.m_level, cmdLine.m_facing);
        POSITION pos = cmdLine.m_Party.GetHeadPosition();
        while (pos != NULL)
          ProcInput.AddPartyMember(cmdLine.m_Party.GetNext(pos));
      }
    }
    if (cmdLine.m_forceAV)
      WriteDebugString("Forcing area map view to be allowed\n");
  }
  
  // keep hard-drives from going into sleep mode
  // not supported on Win95 and WinNT4
  //SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_CONTINUOUS);

  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDungeonApp commands

/*
void CDungeonApp::SetIntervalTimer(UINT idTimer, int msecs)
{
  //AfxGetMainWnd()->KillTimer(idTimer);
  StopTimer();
  m_EventTimerId=0xFFFFFFFF;

  if (msecs <= 0)
  {
    msecs = 2000;
    WriteDebugString("Zero millisecs passed to SetIntervalTimer()\n");
  }

  //WriteDebugString("Setting event timer for %u msecs\n", msecs);
  if (!StartOneTimeTimer(msecs))
    WriteDebugString("Failed to set event timer for id %u, %u msecs\n", idTimer, msecs);
  else // can't force StartOneTimeTimer to use idTimer, save the id we want to use
    m_EventTimerId=idTimer; 
}
*/
VOID CDungeonApp::OnTimerEvent(UINT TimerId)
{
  // TimerId is the one used internally by StartOneTimeTimer,
  // not the idTimer provided by the calling event code.
  //WriteDebugString("Rcvd timer %u\n", m_EventTimerId);
  ProcInput.handleTimer(m_EventTimerId);
}


void StrangeMessage(int id);

int CDungeonApp::Run() 
{
  //int kkk;
  MSG msgCur;
  DWORD waitTime;
  //sthread.Resume();
  while (!ExitSignaled)
  {
    bool forceDistribute;
    waitTime = virtualGametimeUnits;


    if (bActive) waitTime = inputQueue.DistributeInput();
    while (::PeekMessage(&msgCur, NULL, NULL, NULL, PM_NOREMOVE))
    {
      //if (bActive) inputQueue.DistributeInput(); 
      //WriteDebugString("CDungeonApp::Run loop - msgCur.message = 0x%x\n", msgCur.message);
      //WriteDebugString("PeekMessage = %08x\n", msgCur.message);
      forceDistribute = false;
      switch (msgCur.message)
      {
      case WM_AFXFIRST+2:
      case WM_AFXFIRST+10:
        break;
      case WM_MOUSEMOVE:
        //kkk=1;
        break;
      case WM_KEYDOWN:
        break;
      case WM_SYSKEYDOWN:
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
      case WM_KEYUP:
      case WM_CHAR:
      case WM_DEVICECHANGE:
      case WM_LBUTTONDBLCLK:
      case WM_RBUTTONDBLCLK:
        break;
      case WM_USER:
        if (msgCur.wParam == 0x54dd) break;
        // Note: fallthrough
      case WM_PAINT:
        forceDistribute = true;
        break;
      default:
        StrangeMessage(msgCur.message);
      };
      if (!PumpMessage()) return ExitInstance();

      if (bActive || forceDistribute)
      {
        //WriteDebugString("CDungeonApp::Run Distribute input\n");

        waitTime = inputQueue.DistributeInput();
        //WriteDebugString("CDungeonApp::Run DistributeInput returned %d\n",waitTime);
      };
    };
    const DWORD MsgFilter = QS_ALLINPUT|QS_ALLEVENTS|QS_ALLPOSTMESSAGE;
    if (waitTime > 0)
    {
      MsgWaitForMultipleObjects(
              0,          //DWORD nCount,          // number of handles in the handle array
              NULL,       //LPHANDLE pHandles,     // pointer to the object-handle array
              false,      //BOOL fWaitAll,         // wait for all or wait for one
              waitTime,   //DWORD dwMilliseconds,  // time-out interval in milliseconds
              MsgFilter   //DWORD dwWakeMask       // type of input events to wait for
              );
      PostMessage(NULL, WM_USER, 0x54dd, 0x123abcd);
    };
  };  
  return ExitInstance();
}

bool FastPlayback(void);

void CDungeonApp::AppSleep(DWORD timeout)
{
  static HANDLE objects[2];
  static DWORD timerid;
  MSG msgCur;

  if (FastPlayback()) return;
  //return; //Temporary PRS

  ASSERT(timeout > 0);

  ResetEvent(m_SleepEvent);
  timerid = timeSetEvent(timeout,
                         m_TimerResolution,
                         (LPTIMECALLBACK)m_SleepEvent,
                         NULL,
                         TIME_ONESHOT|TIME_CALLBACK_EVENT_SET );

  if (timerid == NULL)
  {
    WriteDebugString("Failed call to timeSetEvent(%u)\n", timeout);
    Sleep(timeout);
    return;
  }

  objects[0]=hShutdown;
  objects[1]=m_SleepEvent;
  DWORD MsgFilter = QS_ALLINPUT|QS_ALLEVENTS|QS_ALLPOSTMESSAGE; //QS_MOUSEMOVE|QS_TIMER|QS_PAINT
  
  bool finish=false;
  while (!finish)
  {
    switch(MsgWaitForMultipleObjects(2, objects, FALSE, timeout, MsgFilter))
    {
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0+1:
    case WAIT_TIMEOUT:
      finish=true;
      break;
    case WAIT_OBJECT_0+2:
      if (::PeekMessage(&msgCur,NULL,0,0,PM_NOREMOVE))
      {
        if (!PumpMessage()) // this func blocks if no msgs are pending
        {
          SignalShutdown();
          finish=true;
        }
      }
      break;
    default:
      finish=true;
      break;
    }
  }
  timeKillEvent(timerid);
}

int CDungeonApp::ExitInstance() 
{   
  ShutdownGame();
  CloseErrorLog();
  
  if (m_pMainWnd != NULL)
    m_pMainWnd->DestroyWindow();

  CloseShutdownEvent();
  CloseHandle(m_SleepEvent);
  if (m_hAppEvent!=NULL)
    CloseHandle(m_hAppEvent);

  //Closedown the OLE subsystem
  ::CoUninitialize();

  return CWinApp::ExitInstance();
}



