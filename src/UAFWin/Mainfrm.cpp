/******************************************************************************
* Filename: Mainfrm.cpp
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
#include "GetInput.h"
#include "CProcInp.h"
#include "MainFrm.h"
#include "SoundMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL RunFullscreen;
#ifdef WIN10Problem
extern bool WIN10ProblemActive;
#endif

char *APP_NAME = "UAFWin";

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
  ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() : winClassName("")
{
  // this class name is used in PreCreateWindow()
  /*
	winClassName = 
		AfxRegisterWndClass((CS_DBLCLKS | CS_OWNDC),
								        LoadCursor(NULL,IDC_ARROW ),
								        (HBRUSH)GetStockObject(NULL_BRUSH),
                        //NULL);	
								        LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON2)));	
  */
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
  //if (!CFrameWnd::PreCreateWindow(cs))
  //      return FALSE;

  if (winClassName.IsEmpty())
  {
	  winClassName = 
		  AfxRegisterWndClass((/*CS_HREDRAW|CS_VREDRAW|CS_BYTEALIGNCLIENT|*/ CS_DBLCLKS | CS_OWNDC),
								          LoadCursor(NULL,IDC_ARROW ),
								          (HBRUSH)GetStockObject(NULL_BRUSH),
                          //NULL);	
								          LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON2)));	
  }

	cs.lpszClass = winClassName;
	//cs.style = WS_POPUP | WS_VISIBLE;
  cs.x = 0;
	cs.y = 0;
	cs.hMenu = NULL;
	cs.lpszName = APP_NAME;

  // use WS_OVERLAPPED instead of WS_POPUP if you want a moveable window frame
#ifdef _DEBUG
  cs.style =  WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
  cs.dwExStyle &= ~WS_EX_TOPMOST;
  cs.cx = SCREEN_WIDTH;
  cs.cy = SCREEN_HEIGHT;
#else
  if (RunFullscreen)
  {
    cs.style = WS_POPUP | WS_VISIBLE | WS_MAXIMIZE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    cs.dwExStyle |= WS_EX_TOPMOST;
	  cs.cx = GetSystemMetrics(SM_CXFULLSCREEN);
	  cs.cy = GetSystemMetrics(SM_CYFULLSCREEN);
  }
  else
  {
    cs.style =  WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    cs.dwExStyle &= ~WS_EX_TOPMOST;
    cs.cx = SCREEN_WIDTH;
    cs.cy = SCREEN_HEIGHT;
  };
#endif
	
  // this function adds a window border and
  // we don't need to call it since we are
  // registering our own window class
  //
  // if (!CFrameWnd::PreCreateWindow(cs))
  //   return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	WriteDebugString("Processing WM_DESTROY\n");
	ShowCursor( TRUE );	
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//ProcInput.KeyPressed(nChar);
  if (nChar != VK_SHIFT)
  { //Skip shifts...makes breakpoints impossible!
    nChar &= 0xff;
    if (GetKeyState(VK_SHIFT) & 0x80000000) nChar |= (SHIFT_KEY<<8);
    inputQueue.OnKeyDown(nChar);
  };
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CMainFrame::OnTimer(UINT nIDEvent) 
{
  //WriteDebugString("Rcvd timer %u\n", nIDEvent);
  //KillTimer(nIDEvent);
  //ProcInput.handleTimer(nIDEvent);
  /*
  {
    TRACE("Killing event timer\n");
    KillTimer(nIDEvent);
  }
  else
  */
  CFrameWnd::OnTimer(nIDEvent);   
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
  //int kkk;
  switch (message)
  {
  case WM_POWERBROADCAST:
   if(wParam == PBT_APMQUERYSUSPEND)
      return BROADCAST_QUERY_DENY;
   return TRUE;

  case MM_MCINOTIFY:
    MidiStopNotify(wParam, lParam);
    break;

  case WM_SHOWWINDOW:
#ifndef _DEBUG
    if (RunFullscreen)
    {
      if ((bActive == FALSE) && ((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE)))
      { 
        // going active again, restore full screen
        ShowWindow(SW_SHOWMAXIMIZED);      
        ::SetForegroundWindow( AfxGetMainWnd()->m_hWnd );
        WriteDebugString("Engine going active1: bActive= %s; wParam=%d\n", (bActive?"Yes":"No"), wParam);
        //bActive = TRUE;
      }
    };
#endif    
    break;

  case WM_ACTIVATE:
#ifdef WIN10Problem
    //if (WIN10ProblemActive)
      WriteDebugString("CMainFrame::WindowProc:WM_ACTIVATE(wParam = %d)\n", wParam);
#endif
#ifndef _DEBUG
    if (RunFullscreen)
    {
      if ((bActive == FALSE) && ((wParam==WA_ACTIVE)||(wParam==WA_CLICKACTIVE)))
      { 
        // going active again, restore full screen
        ShowWindow(SW_SHOWMAXIMIZED);      
        ::SetForegroundWindow( AfxGetMainWnd()->m_hWnd );
        WriteDebugString("Engine going active1: bActive= %s; wParam=%d\n", (bActive?"Yes":"No"), wParam);
        //bActive = TRUE;
      }
    };
#endif    
    break;

  case WM_ACTIVATEAPP:
#ifdef WIN10Problem
    //if (WIN10ProblemActive)
      WriteDebugString("CMainFrame::WindowProc:WM_ACTIVATEAPP(wParam = %d)\n", wParam);
#endif

#ifndef _DEBUG
    if (RunFullscreen)
    {
      if ((bActive == FALSE) && (wParam==1))
      {
        // going active again, restore full screen
        ShowWindow(SW_SHOWMAXIMIZED);
        ::SetForegroundWindow( AfxGetMainWnd()->m_hWnd );
        WriteDebugString("Engine going active2: bActive= %s; wParam=%d\n", (bActive?"Yes":"No"), wParam);
      };
    }
#endif

    bActive = wParam;
//    WriteDebugString("WM_ACTIVATEAPP: bActive= %s; wParam=%d\n", (bActive?"Yes":"No"), wParam);
    if (pSndMgr!=NULL)
    {
      if (bActive) pSndMgr->UnMuteVolume();
      else pSndMgr->MuteVolume();
    }

    //WriteDebugString("Engine app active: %s\n", (bActive?"Yes":"No"));
    break;

  case WM_SYSCOMMAND: // don't let screen saver kick in while engine runs
    if (   ((wParam&0xFFF0) == SC_SCREENSAVE)
        || ((wParam&0xFFF0) == SC_MONITORPOWER))
    {
      if (!DisableScreenSaver) 
        return 0;
    }
    break;
  case WM_CREATE:             // 0x0001
  case WM_DESTROY:            // 0x0002
  case WM_MOVE:               // 0x0003
  case WM_SIZE:               // 0x0005
  case WM_SETFOCUS:           // 0x0007
  case WM_KILLFOCUS:          // 0x0008
  case WM_ERASEBKGND:         // 0x0014
  case WM_CANCELMODE:         // 0x001f
  case WM_SETCURSOR:          // 0x0020
  case WM_MOUSEACTIVATE:      // 0x0021
  case WM_WINDOWPOSCHANGING:  // 0x0046
    break;
  case WM_WINDOWPOSCHANGED:   // 0x0047
    {
      WINDOWPOS *wPos;
      wPos = (WINDOWPOS *)lParam;
      // SWP_NOSIZE == 0x01
      // SWP_NOMOVE == 0x02
      if ((wPos->flags & (SWP_NOSIZE|SWP_NOMOVE)) != (SWP_NOSIZE|SWP_NOMOVE))
      {
        taskList.Invalidate();
      };
    };
    break;
  case WM_PAINT:              // 0x000f
    {
        //taskList.Invalidate();
        break;
    };
  case WM_GETICON:            // 0x007f
  case WM_NCHITTEST:          // 0x0084
  case WM_NCCREATE:           // 0x0081
  case WM_NCDESTROY:          // 0x0082
  case WM_NCCALCSIZE:         // 0x0083
  case WM_NCPAINT:            // 0x0085
  case WM_NCACTIVATE:         // 0x0086
  case WM_SYNCPAINT:          // 0x0088
  case WM_KEYFIRST:           // 0x0100
  case WM_KEYUP:              // 0x0101
  case WM_CHAR:               // 0x0102
    break;
  case WM_SYSKEYDOWN:         // 0x0104
  case WM_SYSKEYUP:           // 0x0105
    break;
  case WM_MOUSEMOVE:          // 0x0200
    //kkk=1;
    break;
  case WM_LBUTTONDOWN:        // 0x0201
  case WM_DEVICECHANGE:       // 0x0219
  case WM_IME_SETCONTEXT:     // 0x0281
  case WM_IME_NOTIFY:         // 0x0282
  case WM_AFXFIRST+2:         // 0x0362
  case WM_AFXFIRST+13:        // 0x036d
  case WM_AFXFIRST+14:        // 0x036e
    break;
  default: ;
  }
      
  return CFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (nFlags & MK_LBUTTON)
     //ProcInput.HandleMouseClick(point.x, point.y);
     inputQueue.OnLButtonDown(point.x, point.y);

  CFrameWnd::OnLButtonDown(nFlags, point);
}

void CMainFrame::OnPaint() 
{
#ifdef WIN10Problem
  if (WIN10ProblemActive)
  WriteDebugString("CMainFrame::OnPaint()\n");
#endif
  taskList.Invalidate();
  CFrameWnd::OnPaint();
}

void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
		
	CFrameWnd::OnLButtonDblClk(nFlags, point);
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	//ProcInput.HandleMouseMove(point.x, point.y);
  inputQueue.OnMouseMove(nFlags, point);
	CFrameWnd::OnMouseMove(nFlags, point);
}