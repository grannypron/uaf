/******************************************************************************
* Filename: MouseThread.h
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
#ifndef _MOUSETHREAD_H_
#define _MOUSETHREAD_H_

#include "Thread.h"

//#define DEFAULT_CURSOR 0
#define NOALLOW_CURSOR 1
#define CIRCLE_CURSOR 2
#define NE_CURSOR 3
#define E_CURSOR 4
#define SE_CURSOR 5
#define S_CURSOR 6
#define SW_CURSOR 7
#define W_CURSOR 8
#define NW_CURSOR 9
#define N_CURSOR 10
#define TURNRIGHT_CURSOR 11
#define TURNLEFT_CURSOR 12
#define TURN180_CURSOR 13
#define WAIT_CURSOR 14


class CMouseRender
{
public:
  CMouseRender();
  ~CMouseRender();

  //DWORD Thread();
  void DrawMouseOnBack();
  void DrawMouseOnFront(int x, int y);
  void UpdateMouse(void);
  void LockMouse(void) { m_csMThreadFlip.Lock();/*WaitForSingleObject(m_hMThreadFlip,INFINITE);*/ }
  void UnlockMouse(void) { m_csMThreadFlip.Unlock();/*ReleaseMutex(m_hMThreadFlip);*/ }
  void OnMouseMove(int x, int y);
  BOOL LoadSurfaces();
  void FlushSaveArea();

  void EnableWaitCursor();
  void DisableWaitCursor();
  void ShowMouseCursor(BOOL show=TRUE);

  static LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam);  
  static HHOOK m_hWindowsHook;
  static CMouseRender *m_pSelf;
  static CMouseRender *GetPtrToObject(void) { return CMouseRender::m_pSelf; }   
  
  BOOL InitInstance();
  void ExitInstance();

private:
  BOOL ForceWaitCursor;
  BOOL m_ShowCursor;
  int m_width;
  int m_height;
  //HANDLE m_hMThreadFlip;
  CCriticalSection m_csMThreadFlip;
  long hScreenData;
  int m_oldX;
  int m_oldY;
  RECT m_SrcRect;
};

extern CMouseRender MouseRender;

#endif // _MOUSETHREAD_H_
