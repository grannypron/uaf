/******************************************************************************
* Filename: Addnpc.cpp
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
#include "GlobalData.h"
#include "GameEvent.h"
#include "globtext.h"
#include "SelectPicDlg.h"
#include "ChooseNPC.h"
#include "AddNPC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AddNPC dialog


AddNPC::AddNPC(ADD_NPC_DATA& add, CWnd* pParent /*=NULL*/)
	: CDialog(AddNPC::IDD, pParent)
{
	//{{AFX_DATA_INIT(AddNPC)
	m_PcntHitPoints = 100;
	m_Text = _T("");
	m_UseOriginal = TRUE;
	//}}AFX_DATA_INIT

	m_add = add;
}


void AddNPC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddNPC)
	DDX_Control(pDX, IDC_SEECHAR, m_SeeChar);
	DDX_Control(pDX, IDC_DISTANCE, m_Distance);
	DDX_Control(pDX, IDC_ADDCHAR, m_AddChar);
	DDX_Text(pDX, IDC_PCNTHITPOINTS, m_PcntHitPoints);
	DDV_MinMaxInt(pDX, m_PcntHitPoints, 0, 100);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Check(pDX, IDC_USEORIGINAL, m_UseOriginal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddNPC, CDialog)
	//{{AFX_MSG_MAP(AddNPC)
	ON_BN_CLICKED(IDC_ADDCHAR, OnAddchar)
	ON_CBN_SELCHANGE(IDC_DISTANCE, OnSelchangeDistance)
	ON_BN_CLICKED(IDC_SEECHAR, OnSeechar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddNPC message handlers

void AddNPC::OnAddchar() 
{
	//CChooseNPC dlg(m_add.charAdded);
	CChooseNPC dlg(m_add.characterID);
	
	UINT result = dlg.DoModal();

	if (result == IDOK)
  {
    //m_add.charAdded = dlg.m_CharNum;
    m_add.characterID = dlg.m_characterID;
    //if (globalData.charData.HaveKey(dlg.m_CharNum))
    if (globalData.charData.HaveCharacter(dlg.m_characterID))
    {
      char name[MAX_MEDITBUTTON_TEXT+1];
      //getBaseName(globalData.charData.GetCharacterName(m_add.charAdded), name, MAX_MEDITBUTTON_TEXT);
      getBaseName(globalData.charData.GetCharacterName(m_add.characterID), name, MAX_MEDITBUTTON_TEXT);
      m_AddChar.SetWindowText(name);
    }
    else
      m_AddChar.SetWindowText("Nobody");
  }
}

void AddNPC::OnSelchangeDistance() 
{
	m_add.distance = 
		(eventDistType) m_Distance.GetCurSel();	
}

void AddNPC::OnSeechar() 
{
   CSelectPicDlg dlg(m_add.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_add.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_add.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SeeChar.SetWindowText(name);
      //UpdateData(FALSE);
   }
}

void AddNPC::GetNPCData(ADD_NPC_DATA& add)
{
	add = m_add;
}

void AddNPC::OnOK() 
{
	UpdateData(TRUE);

	m_add.hitPointMod = m_PcntHitPoints;
	m_add.text = m_Text;
	m_add.distance = (eventDistType)m_Distance.GetCurSel();
  m_add.useOriginal = m_UseOriginal;
	
	CDialog::OnOK();
}

BOOL AddNPC::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Distance.ResetContent();

	for (int i=0;i<NUM_DIST_TYPES;i++)
		m_Distance.AddString(DistanceText[i]);
	
  int temp = m_add.distance;
  if ((temp < 0) || (temp >= NUM_DIST_TYPES))
    temp = 0;

	m_Distance.SetCurSel(temp);
	m_Text = m_add.text;

	if (m_add.hitPointMod >= 0)
		m_PcntHitPoints = m_add.hitPointMod;
	else
		m_PcntHitPoints = 100;

  char name[MAX_MEDITBUTTON_TEXT+1];

  //if (globalData.charData.HaveKey(m_add.charAdded))
  if (globalData.charData.HaveCharacter(m_add.characterID))
  {
    //getBaseName(globalData.charData.GetCharacterName(m_add.charAdded), name, MAX_MEDITBUTTON_TEXT);
    getBaseName(globalData.charData.GetCharacterName(m_add.characterID), name, MAX_MEDITBUTTON_TEXT);
    m_AddChar.SetWindowText(name);
  }
  else
    m_AddChar.SetWindowText("Nobody");

  getBaseName(m_add.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeChar.SetWindowText(name);
  m_UseOriginal = m_add.useOriginal;

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

	UpdateData(FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
