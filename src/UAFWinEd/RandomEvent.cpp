/******************************************************************************
* Filename: SmallTownEvent.cpp
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
#include "SelectPicDlg.h"

#include "RandomEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRandomEvent dialog


CRandomEvent::CRandomEvent(RANDOM_EVENT_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CRandomEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRandomEvent)
	m_Text = _T("");
	m_Chance1 = 0;
	m_Chance10 = 0;
	m_Chance11 = 0;
	m_Chance12 = 0;
	m_Chance13 = 0;
	m_Chance2 = 0;
	m_Chance3 = 0;
	m_Chance4 = 0;
	m_Chance5 = 0;
	m_Chance6 = 0;
	m_Chance7 = 0;
	m_Chance8 = 0;
	m_Chance9 = 0;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CRandomEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRandomEvent)
	DDX_Control(pDX, IDC_SEEART, m_SeeArt);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Text(pDX, IDC_CHANCE1, m_Chance1);
	DDX_Text(pDX, IDC_CHANCE10, m_Chance10);
	DDX_Text(pDX, IDC_CHANCE11, m_Chance11);
	DDX_Text(pDX, IDC_CHANCE12, m_Chance12);
	DDX_Text(pDX, IDC_CHANCE13, m_Chance13);
	DDX_Text(pDX, IDC_CHANCE2, m_Chance2);
	DDX_Text(pDX, IDC_CHANCE3, m_Chance3);
	DDX_Text(pDX, IDC_CHANCE4, m_Chance4);
	DDX_Text(pDX, IDC_CHANCE5, m_Chance5);
	DDX_Text(pDX, IDC_CHANCE6, m_Chance6);
	DDX_Text(pDX, IDC_CHANCE7, m_Chance7);
	DDX_Text(pDX, IDC_CHANCE8, m_Chance8);
	DDX_Text(pDX, IDC_CHANCE9, m_Chance9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRandomEvent, CDialog)
	//{{AFX_MSG_MAP(CRandomEvent)
	ON_BN_CLICKED(IDC_SEEART, OnSeeart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRandomEvent message handlers

BOOL CRandomEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_Text = m_data.text;

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeArt.SetWindowText(name);

  m_Chance1 = m_data.EventChance[0];
  m_Chance2 = m_data.EventChance[1];
  m_Chance3 = m_data.EventChance[2];
  m_Chance4 = m_data.EventChance[3];
  m_Chance5 = m_data.EventChance[4];
  m_Chance6 = m_data.EventChance[5];
  m_Chance7 = m_data.EventChance[6];
  m_Chance8 = m_data.EventChance[7];
  m_Chance9 = m_data.EventChance[8];
  m_Chance10 = m_data.EventChance[9];
  m_Chance11 = m_data.EventChance[10];
  m_Chance12 = m_data.EventChance[11];
  m_Chance13 = m_data.EventChance[12];

  UpdateData(FALSE);
  GotoDlgCtrl(GetDlgItem(IDC_TEXT));	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRandomEvent::OnSeeart() 
{
  CSelectPicDlg dlg(m_data.pic, SmallPicDib);

  if (dlg.DoModal() == IDOK)
  {
    dlg.GetData(m_data.pic);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
    m_SeeArt.SetWindowText(name);
  }		
}

void CRandomEvent::OnOK() 
{
  UpdateData(TRUE);

  m_data.text = m_Text;
  m_data.EventChance[0] = m_Chance1;
  m_data.EventChance[1] = m_Chance2;
  m_data.EventChance[2] = m_Chance3;
  m_data.EventChance[3] = m_Chance4;
  m_data.EventChance[4] = m_Chance5;
  m_data.EventChance[5] = m_Chance6;
  m_data.EventChance[6] = m_Chance7;
  m_data.EventChance[7] = m_Chance8;
  m_data.EventChance[8] = m_Chance9;
  m_data.EventChance[9] = m_Chance10;
  m_data.EventChance[10] = m_Chance11;
  m_data.EventChance[11] = m_Chance12;
  m_data.EventChance[12] = m_Chance13;

	CDialog::OnOK();
}
