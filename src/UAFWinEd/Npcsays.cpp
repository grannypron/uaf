/******************************************************************************
* Filename: Npcsays.cpp
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
#include "globtext.h"
#include "GlobalData.h"
#include "GameEvent.h"
#include "SoundChooser.h"

#include "SelectPicDlg.h"
#include "ChooseNPC.h"
#include "NPCSays.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNPCSays dialog


CNPCSays::CNPCSays(NPC_SAYS_DATA& says, CWnd* pParent /*=NULL*/)
	: CDialog(CNPCSays::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNPCSays)
	m_Text = _T("");
	m_HighlightText = FALSE;
	m_MustHitReturn = FALSE;
	//}}AFX_DATA_INIT

	m_says = says;
}


void CNPCSays::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNPCSays)
	DDX_Control(pDX, IDC_ART, m_ChooseArt);
	DDX_Control(pDX, IDC_SOUND, m_ChooseSound);
	DDX_Control(pDX, IDC_DISTANCE, m_Distance);
	DDX_Control(pDX, IDC_CHAR, m_ChooseChar);
	DDX_Text(pDX, IDC_NPCSAYS, m_Text);
	DDX_Check(pDX, IDC_HIGHLIGHTTEXT, m_HighlightText);
	DDX_Check(pDX, IDC_MUSTHITRETURN, m_MustHitReturn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNPCSays, CDialog)
	//{{AFX_MSG_MAP(CNPCSays)
	ON_BN_CLICKED(IDC_CHAR, OnChar)
	ON_CBN_SELCHANGE(IDC_DISTANCE, OnSelchangeDistance)
	ON_BN_CLICKED(IDC_SOUND, OnSound)
	ON_BN_CLICKED(IDC_ART, OnArt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNPCSays message handlers

void CNPCSays::OnArt() 
{
   CSelectPicDlg dlg(m_says.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_says.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_says.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
      //UpdateData(FALSE);
   }
}

void CNPCSays::OnChar() 
{
	//CChooseNPC dlg(m_says.partySeesChar);
	CChooseNPC dlg(m_says.characterID);
	
	UINT result = dlg.DoModal();

	if (result == IDOK)
  {
    //m_says.partySeesChar = dlg.m_CharNum;
    m_says.characterID = dlg.m_characterID;
    //if (globalData.charData.HaveKey(m_says.partySeesChar))
    //if (globalData.charData.HaveKey(m_says.characterID))
    if (globalData.charData.LocateCharacter(m_says.characterID) >= 0)
    {
      char name[MAX_MEDITBUTTON_TEXT+1];
      //getBaseName(globalData.charData.GetCharacterName(m_says.partySeesChar), name, MAX_MEDITBUTTON_TEXT);
      getBaseName(globalData.charData.GetCharacterName(m_says.characterID), name, MAX_MEDITBUTTON_TEXT);
      m_ChooseChar.SetWindowText(name);
    }
    else
      m_ChooseChar.SetWindowText("Nobody");
  }
}

void CNPCSays::OnSelchangeDistance() 
{
	m_says.distance = 
		(eventDistType) m_Distance.GetCurSel();
}

void CNPCSays::OnSound() 
{
  CSoundChooser dlg(m_says.sound, ALL_SOUND_TYPES);
  if (dlg.DoModal() == IDOK)
  {
    m_says.sound = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_says.sound, name, MAX_MEDITBUTTON_TEXT);
    m_ChooseSound.SetWindowText(name);	
  }	
}

void CNPCSays::OnOK() 
{
	UpdateData(TRUE);
	m_says.distance = (eventDistType)m_Distance.GetCurSel();
	m_says.text = m_Text;
   m_says.mustHitReturn = m_MustHitReturn;
   m_says.highlight = m_HighlightText;

	CDialog::OnOK();
}

BOOL CNPCSays::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();
	
	m_Distance.ResetContent();

	 for (i=0;i<NUM_DIST_TYPES;i++)
	 	 m_Distance.AddString(DistanceText[i]);
	
   m_Distance.SetCurSel((int)m_says.distance);
	 m_Text = m_says.text;
   m_MustHitReturn = m_says.mustHitReturn;
   m_HighlightText = m_says.highlight;

   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_says.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseArt.SetWindowText(name);

    //if (globalData.charData.HaveKey(m_says.partySeesChar))
    if (globalData.charData.LocateCharacter(m_says.characterID) >= 0)
    {
      //getBaseName(globalData.charData.GetCharacterName(m_says.partySeesChar), name, MAX_MEDITBUTTON_TEXT);
      getBaseName(globalData.charData.GetCharacterName(m_says.characterID), name, MAX_MEDITBUTTON_TEXT);
      m_ChooseChar.SetWindowText(name);
    }
    else
      m_ChooseChar.SetWindowText("Nobody");

   getBaseName(m_says.sound, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseSound.SetWindowText(name);	

  GotoDlgCtrl(GetDlgItem(IDC_NPCSAYS));

  UpdateData(FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNPCSays::GetNPCSaysData(NPC_SAYS_DATA& says)
{
	says = m_says;
}

