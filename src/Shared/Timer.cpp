/******************************************************************************
* Filename: Timer.cpp
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

#ifdef UAFEDITOR
#else
#endif
#include "externs.h"
#include "Timer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTimer::CTimer(UINT TargetRes)
{
  TIMECAPS tc;

  if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) 
    ASSERT(0);

  m_TimerId = 0;
  m_Active = FALSE;
  m_Periodic = FALSE;
  m_Resolution = min(max(tc.wPeriodMin, TargetRes), tc.wPeriodMax);
  timeBeginPeriod(m_Resolution); 
}

CTimer::~CTimer()
{
  StopTimer();
  timeEndPeriod(m_Resolution); 
}

BOOL CTimer::StartOneTimeTimer(UINT MsInterval)
{
  if (m_Active)
    return FALSE;

  m_Periodic = FALSE;
  m_TimerId = timeSetEvent(MsInterval,
                           m_Resolution,
                           TimerCallback,
                           reinterpret_cast<DWORD>(this),
                           TIME_ONESHOT );
  if(!m_TimerId)
    return FALSE;
  else
    m_Active = TRUE;
  return TRUE;
}

BOOL CTimer::StartPeriodicTimer(UINT MsInterval)
{
  if (m_Active)
    return FALSE;

  m_Periodic = TRUE;
  m_TimerId = timeSetEvent(MsInterval,    
                           m_Resolution,  
                           TimerCallback, 
                           reinterpret_cast<DWORD>(this),
                           TIME_PERIODIC );
  if(!m_TimerId)
    return FALSE;
  else
    m_Active = TRUE;
  return TRUE;
}

VOID CTimer::StopTimer()
{
  if (m_Active)
  {
    timeKillEvent(m_TimerId);
    m_TimerId = 0;
    m_Active = FALSE;
  }
}

void CALLBACK TimerCallback(UINT TimerId, UINT msg, DWORD UserData, DWORD dw1, DWORD dw2)
{
  CTimer* pTimerObj = reinterpret_cast<CTimer*>(UserData);

  if (!pTimerObj->m_Periodic)
    pTimerObj->m_Active = FALSE;

  pTimerObj->OnTimerEvent(TimerId);
}

