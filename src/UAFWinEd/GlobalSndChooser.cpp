/******************************************************************************
* Filename: GlobalSndChooser.cpp
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
#include "resource.h"

#include "UAFWinEd.h"
#include "SoundChooser.h"
#include "class.h"
#include "SoundEventDlg.h"
#include "GlobalSndChooser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// for sound testing in debug mode only
long bufs[10];

/////////////////////////////////////////////////////////////////////////////
// CGlobalSndChooser dialog


CGlobalSndChooser::CGlobalSndChooser(CWnd* pParent /*=NULL*/)
	: CDialog(CGlobalSndChooser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGlobalSndChooser)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGlobalSndChooser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGlobalSndChooser)
	DDX_Control(pDX, IDC_STOPBGNDQUEUE, m_BgndQueueStop);
	DDX_Control(pDX, IDC_SIMULMIDI, m_SimulMidiTest);
	DDX_Control(pDX, IDC_STOPSIMULSOUNDS, m_SoundTestStopSimul);
	DDX_Control(pDX, IDC_SOUNDTESTSTOP, m_SoundTextStopQueue);
	DDX_Control(pDX, IDC_SOUNDTESTQUEUED, m_SoundTestQueued);
	DDX_Control(pDX, IDC_SOUNDTEST, m_SoundTest);
	DDX_Control(pDX, IDC_CHARMISS, m_CharMiss);
	DDX_Control(pDX, IDC_CHARHIT, m_CharHit);
	DDX_Control(pDX, IDC_PARTYSTEP, m_PartyStep);
	DDX_Control(pDX, IDC_PARTYBUMP, m_PartyBump);
	DDX_Control(pDX, IDC_INTROMUSIC, m_IntroMusic);
	DDX_Control(pDX, IDC_DEATHMUSIC, m_DeathMusic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGlobalSndChooser, CDialog)
	//{{AFX_MSG_MAP(CGlobalSndChooser)
	ON_BN_CLICKED(IDC_DEATHMUSIC, OnDeathmusic)
	ON_BN_CLICKED(IDC_INTROMUSIC, OnIntromusic)
	ON_BN_CLICKED(IDC_PARTYBUMP, OnPartybump)
	ON_BN_CLICKED(IDC_PARTYSTEP, OnPartystep)
	ON_BN_CLICKED(IDC_CHARMISS, OnCharmiss)
	ON_BN_CLICKED(IDC_CHARHIT, OnCharhit)
	ON_BN_CLICKED(IDC_SOUNDTEST, OnSoundtest)
	ON_BN_CLICKED(IDC_SOUNDTESTQUEUED, OnSoundtestqueued)
	ON_BN_CLICKED(IDC_SOUNDTESTSTOP, OnSoundteststop)
	ON_BN_CLICKED(IDC_STOPSIMULSOUNDS, OnStopsimulsounds)
	ON_BN_CLICKED(IDC_SIMULMIDI, OnSimulmidi)
	ON_BN_CLICKED(IDC_STOPBGNDQUEUE, OnStopbgndqueue)
	ON_BN_CLICKED(IDC_CAMPMUSIC, OnCampmusic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGlobalSndChooser message handlers

void CGlobalSndChooser::OnDeathmusic() 
{
  CSoundChooser dlg(DeathMusic, ALL_SOUND_TYPES);
  if (dlg.DoModal() == IDOK)
  {
    DeathMusic = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(DeathMusic, name, MAX_MEDITBUTTON_TEXT);
    m_DeathMusic.SetWindowText(name);	
  }
}

void CGlobalSndChooser::OnIntromusic() 
{
  UpdateData(TRUE);
  SOUND_EVENT tempEvent;
  POSITION pos = IntroMusic.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    tempEvent.sounds.AddTail(IntroMusic.sounds.GetAt(pos));
    IntroMusic.sounds.GetNext(pos);
  }

  CSoundEventDlg dlg(tempEvent);
  if (dlg.DoModal()==IDOK)
  {
    IntroMusic.Clear();
    dlg.GetData(tempEvent);

    pos = tempEvent.sounds.GetHeadPosition();
    while (pos != NULL)
    {
      IntroMusic.sounds.AddTail(tempEvent.sounds.GetAt(pos));
      tempEvent.sounds.GetNext(pos);
    }
  }
  UpdateData(FALSE);
}

void CGlobalSndChooser::OnPartybump() 
{
  CSoundChooser dlg(PartyBump, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    PartyBump = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(PartyBump, name, MAX_MEDITBUTTON_TEXT);
    m_PartyBump.SetWindowText(name);	
  }
}

void CGlobalSndChooser::OnPartystep() 
{
  CSoundChooser dlg(PartyStep, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    PartyStep = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(PartyStep, name, MAX_MEDITBUTTON_TEXT);
    m_PartyStep.SetWindowText(name);	
  }	
}

BOOL CGlobalSndChooser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
#ifndef _DEBUG
  m_SimulMidiTest.EnableWindow(FALSE);
  m_SimulMidiTest.ShowWindow(SW_HIDE);
  m_BgndQueueStop.EnableWindow(FALSE);
  m_BgndQueueStop.ShowWindow(SW_HIDE);
  m_SoundTest.EnableWindow(FALSE);
  m_SoundTest.ShowWindow(SW_HIDE);
  m_SoundTestQueued.EnableWindow(FALSE);
  m_SoundTestQueued.ShowWindow(SW_HIDE);
  m_SoundTestStopSimul.EnableWindow(FALSE);
  m_SoundTestStopSimul.ShowWindow(SW_HIDE);
  m_SoundTextStopQueue.EnableWindow(FALSE);
  m_SoundTextStopQueue.ShowWindow(SW_HIDE);
#endif

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(DeathMusic, name, MAX_MEDITBUTTON_TEXT);
  m_DeathMusic.SetWindowText(name);	
  //getBaseName(IntroMusic, name, MAX_MEDITBUTTON_TEXT);
  //m_IntroMusic.SetWindowText(name);	
  getBaseName(PartyBump, name, MAX_MEDITBUTTON_TEXT);
  m_PartyBump.SetWindowText(name);	
  getBaseName(PartyStep, name, MAX_MEDITBUTTON_TEXT);
  m_PartyStep.SetWindowText(name);	
  getBaseName(CharHit, name, MAX_MEDITBUTTON_TEXT);
  m_CharHit.SetWindowText(name);	
  getBaseName(CharMiss, name, MAX_MEDITBUTTON_TEXT);
  m_CharMiss.SetWindowText(name);	

  int i;
  for (i=0;i<10;i++)
    bufs[i]=-2;
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGlobalSndChooser::OnCharmiss() 
{
  CSoundChooser dlg(CharMiss, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    CharMiss = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(CharMiss, name, MAX_MEDITBUTTON_TEXT);
    m_CharMiss.SetWindowText(name);	
  }	
}

void CGlobalSndChooser::OnCharhit() 
{
  CSoundChooser dlg(CharHit, ST_SAMPLE);
  if (dlg.DoModal() == IDOK)
  {
    CharHit = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(CharHit, name, MAX_MEDITBUTTON_TEXT);
    m_CharHit.SetWindowText(name);	
  }	
}

void CGlobalSndChooser::OnSoundtest() 
{
  UpdateData(TRUE);
  OnStopsimulsounds();

  int i;
 
  //bufs[0] = LoadSound("C:\\UAF_Dev\\UAF\\DefaultArt\\redrum.mp3");
  //bufs[0] = LoadSound(DeathMusic);
  //bufs[1] = LoadSound(IntroMusic);
  //bufs[2] = LoadSound(PartyBump);
  //bufs[3] = LoadSound(PartyStep);
  //bufs[4] = LoadSound(CharHit);
  //bufs[5] = LoadSound(CharMiss);
  //bufs[6] = LoadSound("C:\\UAF_Dev\\UAF\\DefaultArt\\TestSnd.mid");
  //bufs[7] = LoadSound("C:\\UAF_Dev\\UAF\\DefaultArt\\redrum.mp3");
  //bufs[8] = LoadSound("C:\\UAF_Dev\\UAF\\DefaultArt\\x-girlfriend.mp3");
  bufs[7] = LoadSound("C:\\UAF_Dev\\UAF\\DefaultArt\\sepways.mid");
  for (i=0;i<10;i++)
  {
    if (bufs[i] == -1)
    {
      CString tmp;
      tmp.Format("Failed to load test sound %i", i);
      MsgBoxError(tmp);
    }
  }

  for (i=0;i<10;i++)
  {
    if (bufs[i] >= 0)
      PlaySound(bufs[i], FALSE);
  }
}
/*
rain.mod
railway.xm
radioactive.s3m
r-lite.it
*/
void CGlobalSndChooser::OnSoundtestqueued() 
{
  UpdateData(TRUE);
  pSndMgr->StopQueue();	
  //pSndMgr->QueueSound(DeathMusic);
  //pSndMgr->QueueSound(IntroMusic);
  //pSndMgr->QueueSound(PartyBump);
  //pSndMgr->QueueSound(PartyStep);
  //pSndMgr->QueueSound(CharHit);
  pSndMgr->QueueSound(CharMiss);
  pSndMgr->QueueSound("C:\\UAF_Dev\\UAF\\DefaultArt\\r-lite.it");
  //pSndMgr->QueueSound("C:\\UAF_Dev\\UAF\\DefaultArt\\redrum.mp3");
  //pSndMgr->QueueSound("C:\\UAF_Dev\\UAF\\DefaultArt\\sepways.mid");
  pSndMgr->QueueSound(PartyBump);
  pSndMgr->PlayQueue();	
}

void CGlobalSndChooser::OnSoundteststop() 
{
	pSndMgr->StopQueue();	
}

void CGlobalSndChooser::OnStopsimulsounds() 
{
  for (int i=0;i<10;i++)
  {
    if (bufs[i] >= 0)
      //ClearSound(bufs[i]);
      ClearSound(bufs[i]);
  }  	

#ifdef _DEBUG
  //pSndMgr->StopDebugMidi();
#endif
}

void CGlobalSndChooser::OnSimulmidi() 
{
#ifdef _DEBUG
  //pSndMgr->PlayDebugMidi("C:\\UAF_Dev\\UAF\\DefaultArt\\Hawaii50.mid",
  //                       "C:\\UAF_Dev\\UAF\\DefaultArt\\sepways.mid");
  //pSndMgr->PlayDebugMidi("C:\\UAF_Dev\\UAF\\DefaultArt\\TestSnd.mid", NULL);
#endif
  pSndMgr->StopBgndQueue();	
  //pSndMgr->QueueSound(DeathMusic);
  //pSndMgr->QueueSound(IntroMusic);
  //pSndMgr->QueueSound(PartyBump);
  //pSndMgr->QueueSound(PartyStep);
  //pSndMgr->QueueSound(CharHit);
  pSndMgr->QueueSound(CharMiss);
  pSndMgr->QueueBgndSound("C:\\UAF_Dev\\UAF\\DefaultArt\\TestSnd.mid",FALSE);
  pSndMgr->QueueSound("C:\\UAF_Dev\\UAF\\DefaultArt\\redrum.mp3");
  //pSndMgr->QueueSound("C:\\UAF_Dev\\UAF\\DefaultArt\\sepways.mid");
  //pSndMgr->QueueSound(PartyBump);
  pSndMgr->PlayBgndQueue();	
}

void CGlobalSndChooser::OnStopbgndqueue() 
{
  pSndMgr->StopBgndQueue();	
}

void CGlobalSndChooser::OnCampmusic() 
{
  UpdateData(TRUE);
  SOUND_EVENT tempEvent;
  POSITION pos = CampMusic.sounds.GetHeadPosition();
  while (pos != NULL)
  {
    tempEvent.sounds.AddTail(CampMusic.sounds.GetAt(pos));
    CampMusic.sounds.GetNext(pos);
  }

  CSoundEventDlg dlg(tempEvent);
  if (dlg.DoModal()==IDOK)
  {
    CampMusic.Clear();
    dlg.GetData(tempEvent);

    pos = tempEvent.sounds.GetHeadPosition();
    while (pos != NULL)
    {
      CampMusic.sounds.AddTail(tempEvent.sounds.GetAt(pos));
      tempEvent.sounds.GetNext(pos);
    }
  }
  UpdateData(FALSE);	
}
