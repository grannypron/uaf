/******************************************************************************
* Filename: DiffLvlSettings.cpp
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
#include "GlobalData.h"

#include "DiffLvlSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiffLvlSettings dialog


CDiffLvlSettings::CDiffLvlSettings(DIFFICULTY_LEVEL_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CDiffLvlSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiffLvlSettings)
	m_DefLvl = 0;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CDiffLvlSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiffLvlSettings)
	DDX_Control(pDX, IDC_DIFFLVL5, m_Lvl5);
	DDX_Control(pDX, IDC_DIFFLVL4, m_Lvl4);
	DDX_Control(pDX, IDC_DIFFLVL3, m_Lvl3);
	DDX_Control(pDX, IDC_DIFFLVL2, m_Lvl2);
	DDX_Control(pDX, IDC_DIFFLVL1, m_Lvl1);
	DDX_Text(pDX, IDC_DEFLVL, m_DefLvl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiffLvlSettings, CDialog)
	//{{AFX_MSG_MAP(CDiffLvlSettings)
	ON_BN_CLICKED(IDC_DIFFLVL5, OnDifflvl5)
	ON_BN_CLICKED(IDC_DIFFLVL4, OnDifflvl4)
	ON_BN_CLICKED(IDC_DIFFLVL3, OnDifflvl3)
	ON_BN_CLICKED(IDC_DIFFLVL2, OnDifflvl2)
	ON_BN_CLICKED(IDC_DIFFLVL1, OnDifflvl1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiffLvlSettings message handlers

void CDiffLvlSettings::OnDifflvl5() 
{
  CDiffLvlData dlg(m_data.m_data[4]);
  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(m_data.m_data[4]);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.m_data[4].m_Name, name, MAX_MEDITBUTTON_TEXT);
    m_Lvl5.SetWindowText(name);
  }	
}

void CDiffLvlSettings::OnDifflvl4() 
{
  CDiffLvlData dlg(m_data.m_data[3]);
  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(m_data.m_data[3]);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.m_data[3].m_Name, name, MAX_MEDITBUTTON_TEXT);
    m_Lvl4.SetWindowText(name);
  }	
}

void CDiffLvlSettings::OnDifflvl3() 
{
  CDiffLvlData dlg(m_data.m_data[2]);
  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(m_data.m_data[2]);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.m_data[2].m_Name, name, MAX_MEDITBUTTON_TEXT);
    m_Lvl3.SetWindowText(name);
  }		
}

void CDiffLvlSettings::OnDifflvl2() 
{
  CDiffLvlData dlg(m_data.m_data[1]);
  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(m_data.m_data[1]);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.m_data[1].m_Name, name, MAX_MEDITBUTTON_TEXT);
    m_Lvl2.SetWindowText(name);
  }	
}

void CDiffLvlSettings::OnDifflvl1() 
{
  CDiffLvlData dlg(m_data.m_data[0]);
  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(m_data.m_data[0]);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.m_data[0].m_Name, name, MAX_MEDITBUTTON_TEXT);
    m_Lvl1.SetWindowText(name);
  }	
}

void CDiffLvlSettings::OnOK() 
{
  UpdateData(TRUE);
  
  if ((m_DefLvl<1) || (m_DefLvl > 5))
    m_DefLvl=3;

  m_data.m_defaultLvl = m_DefLvl-1;
  CDialog::OnOK();
}

BOOL CDiffLvlSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.m_data[0].m_Name, name, MAX_MEDITBUTTON_TEXT);
  m_Lvl1.SetWindowText(name);
  getBaseName(m_data.m_data[1].m_Name, name, MAX_MEDITBUTTON_TEXT);
  m_Lvl2.SetWindowText(name);
  getBaseName(m_data.m_data[2].m_Name, name, MAX_MEDITBUTTON_TEXT);
  m_Lvl3.SetWindowText(name);
  getBaseName(m_data.m_data[3].m_Name, name, MAX_MEDITBUTTON_TEXT);
  m_Lvl4.SetWindowText(name);
  getBaseName(m_data.m_data[4].m_Name, name, MAX_MEDITBUTTON_TEXT);
  m_Lvl5.SetWindowText(name);
	m_DefLvl = m_data.m_defaultLvl+1;

  UpdateData(FALSE);
  GotoDlgCtrl(GetDlgItem(IDC_DEFLVL));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
