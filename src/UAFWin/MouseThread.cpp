/******************************************************************************
* Filename: MouseRender.cpp
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
#include "Dungeon.h"
//#include "externs.h"
#include "Graphics.h"
//#include "Screen.h"
#include "PicSlot.h"
#include "GetInput.h"
#include "CProcInp.h"

#include "MouseThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

HHOOK CMouseRender::m_hWindowsHook = NULL;
CMouseRender *CMouseRender::m_pSelf = NULL;

CMouseRender MouseRender;

CMouseRender::CMouseRender()
{
  m_width=0;
  m_height=0;
  m_ShowCursor=TRUE;
  ForceWaitCursor=FALSE;
  UseCustomCursor=FALSE;
  hScreenData=-1;
  m_oldX=m_oldY=0;
  memset(&m_SrcRect,0,sizeof(m_SrcRect));
}

CMouseRender::~CMouseRender()
{
  UseCustomCursor=FALSE;
  m_csMThreadFlip.Unlock();
}

void CMouseRender::FlushSaveArea()
{
  if (!UseCustomCursor) return;
  GraphicsMgr.ClearSurface(MOUSESAVENUM);
}

void CMouseRender::ShowMouseCursor(BOOL show)
{ 
  if (!UseCustomCursor) return;
  if (show==m_ShowCursor) return;
  m_ShowCursor=show;
  
  ///*
  if (!show)
  {
    // restore original image where mouse used to be
    RECT rect1 = { m_oldX, m_oldY, m_oldX+m_width, m_oldY+m_height };
    GraphicsMgr.BlitImage(FRONTBUFFERNUM, MOUSESAVENUM, &m_SrcRect, &rect1);
  }
  //*/
}

BOOL CMouseRender::LoadSurfaces()
{ 
  UseCustomCursor = GraphicsMgr.AddMouseSurfaces(CursorArt.filename);
  
  if (!UseCustomCursor)
  {
    WriteDebugString("Cursor Art %s not found\n",CursorArt.filename);
    //if (CursorArt.filename.Find(DEFAULT_CURSOR)<0)
    if (CursorArt.filename.CompareNoCase(DEFAULT_CURSOR)!=0)
    {      
      //CursorArt.filename = rte.CursorArtDir() + DEFAULT_CURSOR;
      CursorArt.filename = DEFAULT_CURSOR;
      WriteDebugString("Cursor Art now trying default %s\n", CursorArt.filename);
      UseCustomCursor = GraphicsMgr.AddMouseSurfaces(CursorArt.filename);
      if (!UseCustomCursor)
        WriteDebugString("Cursor Art default also failed\n");
    }
  }

  ShowCursor(!UseCustomCursor);
  if (UseCustomCursor)
  {
    CursorArt.key = MOUSEBUFFERNUM;
    CursorArt.FrameHeight = 32;
    CursorArt.FrameWidth = 32;
    CursorArt.NumFrames = 15;
    CursorArt.picType = TransBufferDib;
    CursorArt.style = PIC_DATA::AS_None;
    CursorArt.timeDelay = 0;
    POINT point;
    GetCursorPos(&point);
    CursorArt.sx=point.x;
    CursorArt.sy=point.y;

    RECT rect;
    if (!GraphicsMgr.GetSpriteRect(CursorArt.key, &rect))
      TRACE("Failed to get mouse rect data\n");
    m_width = (rect.right-rect.left)+1;
    m_height = (rect.bottom-rect.top)+1;
    m_SrcRect.left=0;
    m_SrcRect.top = 0;
    m_SrcRect.right = m_width;
    m_SrcRect.bottom = m_height;

    int x = CursorArt.sx;
    int y = CursorArt.sy;
    // save current screen where mouse is
    RECT rect2 = { x, y, x+m_width, y+m_height };
    m_oldX=x;
    m_oldY=y;
    GraphicsMgr.BlitImage(MOUSESAVENUM, BACKBUFFERNUM, &rect2, &m_SrcRect);
  }

  return TRUE;
}

BOOL CMouseRender::InitInstance()
{
  if (!UseCustomCursor) return FALSE;

  DWORD ThreadId = AfxGetApp()->m_nThreadID;
  CMouseRender::m_pSelf = this;
  m_hWindowsHook = SetWindowsHookEx(WH_MOUSE, GetMsgProc, 
                                    NULL, ThreadId);

  if (m_hWindowsHook == NULL)
    TRACE("Failed to acquire windows system hook\n");
  return TRUE;
}

void CMouseRender::ExitInstance()
{
  if (!UseCustomCursor) return;
  UseCustomCursor=FALSE;
  ShowCursor(TRUE);
  if (m_hWindowsHook != NULL)
  {
    UnhookWindowsHookEx(m_hWindowsHook);
    m_hWindowsHook = NULL;
  }
}

void CMouseRender::EnableWaitCursor() 
{ 
  if (!UseCustomCursor) return;
  ForceWaitCursor=TRUE;
  theApp.AppSleep(50); // to allow cursor to change
}
void CMouseRender::DisableWaitCursor() 
{ 
  if (!UseCustomCursor) return;
  ForceWaitCursor=FALSE; 
  CursorArt.SetFirstFrame(); 
  theApp.AppSleep(50); // to allow cursor to change
}

void CMouseRender::DrawMouseOnBack()
{
  if (!UseCustomCursor) return;
  if (!m_ShowCursor) return;
  if (ForceWaitCursor) CursorArt.SetFrame(WAIT_CURSOR);
  int x = CursorArt.sx;
  int y = CursorArt.sy;
  // save current screen where mouse is going
  RECT rect = { x, y, x+m_width, y+m_height };
  GraphicsMgr.BlitImage(MOUSESAVENUM, BACKBUFFERNUM, &rect, &m_SrcRect);

  m_oldX=x;
  m_oldY=y;
  GraphicsMgr.BlitSprite(x, 
                         y, 
                         MOUSEBUFFERNUM,
                         FALSE,
                         BACKBUFFERNUM); 
}

void CMouseRender::DrawMouseOnFront(int x, int y)
{
  if (!UseCustomCursor) return;
  //if (!m_ShowCursor) return;
  if (ForceWaitCursor) CursorArt.SetFrame(WAIT_CURSOR);
  
//  CSingleLock sl(&m_csMThreadFlip, TRUE);

  if (m_ShowCursor)
  {
    // restore original image where mouse used to be
    RECT rect1 = { m_oldX, m_oldY, m_oldX+m_width, m_oldY+m_height };
    GraphicsMgr.BlitImage(FRONTBUFFERNUM, MOUSESAVENUM, &m_SrcRect, &rect1);
  }

  // save current screen where mouse is going
  RECT rect2 = { x, y, x+m_width, y+m_height };
  GraphicsMgr.BlitImage(MOUSESAVENUM, FRONTBUFFERNUM, &rect2, &m_SrcRect);

  m_oldX=x;
  m_oldY=y;

  if (m_ShowCursor)
  {
    // blit mouse to front buffer
    GraphicsMgr.BlitSprite(x, 
                           y, 
                           MOUSEBUFFERNUM,
                           FALSE,
                           FRONTBUFFERNUM); 
  }
}

void CMouseRender::OnMouseMove(int x, int y) 
{
  CursorArt.sx = x;
  CursorArt.sy = y;
  //ProcInput.HandleMouseMove(x, y);
  //DrawMouseOnFront(x, y);
}

LRESULT CALLBACK CMouseRender::GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
   if (code < 0)
    return CallNextHookEx(m_hWindowsHook, code, wParam, lParam);

   LRESULT result = 0;
   if ((code == HC_ACTION) || (code == HC_NOREMOVE))
   {
     switch (wParam)
     {
     case WM_MOUSEMOVE:
       {
         CMouseRender *pObj = GetPtrToObject();
         if (pObj != NULL)
         {
           MOUSEHOOKSTRUCT *pData = (MOUSEHOOKSTRUCT*)lParam;
           if (pData != NULL)
             pObj->OnMouseMove(pData->pt.x, pData->pt.y);
         }
         result = CallNextHookEx(m_hWindowsHook, code, wParam, lParam);
       }
       break;

     default:
        result = CallNextHookEx(m_hWindowsHook, code, wParam, lParam);
        break;
     }
   }
   else
     result = CallNextHookEx(m_hWindowsHook, code, wParam, lParam);

   return result;
}
