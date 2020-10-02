/******************************************************************************
* Filename: TextEvent.cpp
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

//#include "UAFWinEd.h"
//#include "GlobalData.h"
#include "Char.h"
#include "GameEvent.h"
#include "SoundChooser.h"

#include "SelectPicDlg.h"
#include "TextEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextEvent dialog


CTextEvent::CTextEvent(TEXT_EVENT_DATA& event, CWnd* pParent /*=NULL*/)
: CDialog(CTextEvent::IDD, pParent), m_event(false)
{
   //{{AFX_DATA_INIT(CTextEvent)
   m_Text = _T("");
	m_ForceBackup = FALSE;
	m_HighlightText = FALSE;
	m_WaitForReturn = FALSE;
	m_KillSound = FALSE;
	//}}AFX_DATA_INIT
   
   m_event = event;
}


void CTextEvent::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CTextEvent)
	DDX_Control(pDX, IDC_DISTANCE, m_Distance);
	DDX_Control(pDX, IDC_SOUNDSELECT, m_Sound);
   DDX_Control(pDX, IDC_CHOOSETEXTEVENTART, m_ChooseArt);
   DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Check(pDX, IDC_FORCEBACKUP, m_ForceBackup);
	DDX_Check(pDX, IDC_HIGHLIGHTTEXT, m_HighlightText);
	DDX_Check(pDX, IDC_WAITFORRETURN, m_WaitForReturn);
	DDX_Check(pDX, IDC_KILLSOUND, m_KillSound);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextEvent, CDialog)
//{{AFX_MSG_MAP(CTextEvent)
ON_BN_CLICKED(IDC_CHOOSETEXTEVENTART, OnChoosetexteventart)
	ON_BN_CLICKED(IDC_SOUNDSELECT, OnSoundselect)
	ON_CBN_SELCHANGE(IDC_DISTANCE, OnSelchangeDistance)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEvent message handlers

void CTextEvent::OnChoosetexteventart() 
{
   CSelectPicDlg dlg(m_event.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_event.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
      //UpdateData(FALSE);
   }
}

void CTextEvent::OnOK() 
{
   UpdateData(TRUE);
   
   m_event.text = m_Text;
   m_event.WaitForReturn = m_WaitForReturn;
   m_event.ForceBackup = m_ForceBackup;
   m_event.HighlightText = m_HighlightText;
   m_event.KillSound = m_KillSound;
   m_event.distance = (eventDistType)m_Distance.GetCurSel();

   CDialog::OnOK();
}

BOOL CTextEvent::OnInitDialog() 
{  
   CDialog::OnInitDialog();
   
   m_Text = m_event.text;
   m_WaitForReturn = m_event.WaitForReturn;
   m_ForceBackup = m_event.ForceBackup;
   m_HighlightText = m_event.HighlightText;
   m_KillSound = m_event.KillSound;
   
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseArt.SetWindowText(name);

   getBaseName(m_event.sound, name, MAX_MEDITBUTTON_TEXT);
   m_Sound.SetWindowText(name);	
   
	 m_Distance.ResetContent();

	 for (int i=0;i<NUM_DIST_TYPES;i++)
		 m_Distance.AddString(DistanceText[i]);
	
   int temp = m_event.distance;
   if ((temp < 0) || (temp >= NUM_DIST_TYPES))
     temp = 0;

	 m_Distance.SetCurSel(temp);

   GotoDlgCtrl(GetDlgItem(IDC_TEXT));

   UpdateData(FALSE);
   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CTextEvent::GetData(TEXT_EVENT_DATA &event)
{
   event = m_event;
}

void CTextEvent::OnSoundselect() 
{
  CSoundChooser dlg(m_event.sound, ALL_SOUND_TYPES);
  if (dlg.DoModal() == IDOK)
  {
    m_event.sound = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_event.sound, name, MAX_MEDITBUTTON_TEXT);
    m_Sound.SetWindowText(name);	
  }	
}

void CTextEvent::OnSelchangeDistance() 
{
	m_event.distance = 
		(eventDistType) m_Distance.GetCurSel();		
}
