/******************************************************************************
* Filename: Dungeon.h
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
#ifndef __DNGN_H__
#define __DNGN_H__

#ifdef UAFEDITOR
#include "..\UAFWinEd\Resource.h"
#else
#include "..\UAFWin\Resource.h"
#endif

#include "items.h"
#include "char.h"
#include "spell.h"
#include "Timer.h"

/////////////////////////////////////////////////////////////////////////////
// CDungeonApp:
// See Dungeon.cpp for the implementation of this class
//

class CDungeonApp : public CWinApp, public CTimer
{
  UINT m_EventTimerId;
  HANDLE m_SleepEvent;
  UINT m_TimerResolution;

public:
	CDungeonApp();
//	void SetIntervalTimer(UINT idTimer, int secs);
  VOID OnTimerEvent(UINT TimerId);
  void AppSleep(DWORD timeout);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDungeonApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDungeonApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CDungeonApp theApp;

/////////////////////////////////////////////////////////////////////////////

#endif
