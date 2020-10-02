/******************************************************************************
* Filename: Removnpc.cpp
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

#include "ChooseNPC.h"
#include "SelectPicDlg.h"
#include "RemovNPC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// RemoveNPC dialog


RemoveNPC::RemoveNPC(REMOVE_NPC_DATA& data, CWnd* pParent /*=NULL*/)
	: CDialog(RemoveNPC::IDD, pParent)
{
	//{{AFX_DATA_INIT(RemoveNPC)
	m_Text = _T("");
	//}}AFX_DATA_INIT

	m_remove = data;
}


void RemoveNPC::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RemoveNPC)
	DDX_Control(pDX, IDC_SEECHAR, m_SeeChar);
	DDX_Control(pDX, IDC_REMOVECHAR, m_RemoveChar);
	DDX_Control(pDX, IDC_DISTANCE, m_Distance);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RemoveNPC, CDialog)
	//{{AFX_MSG_MAP(RemoveNPC)
	ON_CBN_SELCHANGE(IDC_DISTANCE, OnSelchangeDistance)
	ON_BN_CLICKED(IDC_REMOVECHAR, OnRemovechar)
	ON_BN_CLICKED(IDC_SEECHAR, OnSeechar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RemoveNPC message handlers

void RemoveNPC::OnSelchangeDistance() 
{
	m_remove.distance = 
		(eventDistType) m_Distance.GetCurSel();	
}

void RemoveNPC::OnRemovechar() 
{
	//CChooseNPC dlg(m_remove.charRemoved);
	CChooseNPC dlg(m_remove.characterID);
	
	UINT result = dlg.DoModal();

	if (result == IDOK)
  {
    //m_remove.charRemoved = dlg.m_CharNum;
    m_remove.characterID = dlg.m_characterID;
    //if (globalData.charData.HaveKey(m_remove.charRemoved))
    if (globalData.charData.LocateCharacter(m_remove.characterID) >= 0)
    {
      char name[MAX_MEDITBUTTON_TEXT+1];
      //getBaseName(globalData.charData.GetCharacterName(m_remove.charRemoved), name, MAX_MEDITBUTTON_TEXT);
      getBaseName(globalData.charData.GetCharacterName(m_remove.characterID), name, MAX_MEDITBUTTON_TEXT);
	    m_RemoveChar.SetWindowText(name);
    }
    else
      m_RemoveChar.SetWindowText("Nobody");
  }
}

void RemoveNPC::OnSeechar() 
{
   CSelectPicDlg dlg(m_remove.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_remove.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_remove.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SeeChar.SetWindowText(name);
      //UpdateData(FALSE);
   }
}

void RemoveNPC::OnOK() 
{
	UpdateData(TRUE);
	m_remove.text = m_Text;
	m_remove.distance = (eventDistType)m_Distance.GetCurSel();

	CDialog::OnOK();
}

BOOL RemoveNPC::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();
	
	m_Distance.ResetContent();

	for (i=0;i<NUM_DIST_TYPES;i++)
		m_Distance.AddString(DistanceText[i]);
	
	m_Distance.SetCurSel((int)m_remove.distance);
	m_Text = m_remove.text;

   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_remove.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_SeeChar.SetWindowText(name);

    //if (globalData.charData.HaveKey(m_remove.charRemoved))
    if (globalData.charData.LocateCharacter(m_remove.characterID))
    {
      //getBaseName(globalData.charData.GetCharacterName(m_remove.charRemoved), name, MAX_MEDITBUTTON_TEXT);
      getBaseName(globalData.charData.GetCharacterName(m_remove.characterID), name, MAX_MEDITBUTTON_TEXT);
      m_RemoveChar.SetWindowText(name);
    }
    else
      m_RemoveChar.SetWindowText("Nobody");      

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

  UpdateData(FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RemoveNPC::GetRemoveNPCData(REMOVE_NPC_DATA& data)
{
	data = m_remove;
}