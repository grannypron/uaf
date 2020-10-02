/******************************************************************************
* Filename: ChurchDlg.cpp
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
#include "Char.h"
#include "GlobalData.h"
#include "GameEvent.h"
#include "SpellDlg.h"
#include "GlobText.h"
#include "SelectPicDlg.h"
#include "ChurchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChurch dialog

CChurch::CChurch(TEMPLE& data, CWnd* pParent /*=NULL*/)
	: CDialog(CChurch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChurch)
	m_ForceExit = FALSE;
	m_EntryText = _T("");
	m_AllowDonations = FALSE;
	m_DonateTrigger = 0;
	m_Text = _T("");
	m_CoinName = _T("");
	//}}AFX_DATA_INIT

	m_data = data;
}


void CChurch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChurch)
	DDX_Control(pDX, IDC_MAXLEVEL, m_MaxLevel);
	DDX_Control(pDX, IDC_COSTFACTOR, m_CostFactor);
	DDX_Control(pDX, IDC_CLERIC, m_Cleric);
	DDX_Check(pDX, IDC_FORCEEXIT, m_ForceExit);
	DDX_Text(pDX, IDC_ENTERTEXT, m_EntryText);
	DDX_Check(pDX, IDC_ALLOWDONATIONS, m_AllowDonations);
	DDX_Text(pDX, IDC_DONATETRIGGER, m_DonateTrigger);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Text(pDX, IDC_COINNAME, m_CoinName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChurch, CDialog)
	//{{AFX_MSG_MAP(CChurch)
	ON_BN_CLICKED(IDC_CLERIC, OnCleric)
	ON_BN_CLICKED(IDC_SPELLLIST, OnSpelllist)
  ON_BN_CLICKED(IDC_InitSpells, OnInitSpells)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChurch message handlers

void CChurch::OnCleric() 
{
  CSelectPicDlg dlg(m_data.pic, SmallPicDib);

  if (dlg.DoModal() == IDOK)
  {
    dlg.GetData(m_data.pic);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
    m_Cleric.SetWindowText(name);
  }
}

BOOL CChurch::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ForceExit = m_data.forceExit;
  m_EntryText = m_data.text;
  m_Text = m_data.text2;
  m_AllowDonations = m_data.allowDonations;
  m_DonateTrigger = m_data.donationTrigger;

	m_CostFactor.ResetContent();
  m_MaxLevel.ResetContent();

  int i;
  for (i=0;i<NUM_COST_FACTOR_TYPES;i++)
    m_CostFactor.AddString(costFactorText[i]);

  char temp[3];

  for (i=1;i<10;i++)
  {
    itoa(i, temp, 10);
    m_MaxLevel.AddString(temp);
  }

  m_CostFactor.SetCurSel(m_data.costFactor);
  m_MaxLevel.SetCurSel(m_data.maxLevel-1);

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_Cleric.SetWindowText(name);

  GotoDlgCtrl(GetDlgItem(IDC_ENTERTEXT));
  m_CoinName = globalData.moneyData.GetName(globalData.moneyData.GetDefaultType());

  UpdateData(FALSE);	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChurch::GetTempleData(TEMPLE& data)
{
	data = m_data;
}

void CChurch::OnOK() 
{
	UpdateData(TRUE);

	m_data.forceExit = m_ForceExit;
  m_data.text = m_EntryText;
  m_data.text2 = m_Text;
  m_data.costFactor = (costFactorType)m_CostFactor.GetCurSel();
  m_data.allowDonations = m_AllowDonations;
  m_data.donationTrigger = m_DonateTrigger;

  // remove spells in spell books that exceed
  // the configured spell level
  m_data.maxLevel = m_MaxLevel.GetCurSel()+1;  

  for (int i=m_data.maxLevel+1; i<=MAX_SPELL_LEVEL; i++)
    m_data.templeSpells.RemoveSpellLevel(i);

	CDialog::OnOK();
}


void CChurch::OnSpelllist() 
{
  UpdateData(TRUE);
  m_data.maxLevel = m_MaxLevel.GetCurSel()+1;
 
  // set up spell book to allow all spells
	m_data.templeSpells.spellLimits.SetToMaxLimits();
	
  for (int i=m_data.maxLevel+1;i<=MAX_SPELL_LEVEL;i++)
    m_data.templeSpells.RemoveSpellLevel(m_data.maxLevel);

  CSpell dlg(m_data.templeSpells);

  if (dlg.DoModal() == IDOK)
    dlg.GetSpellList(m_data.templeSpells);
}


void CChurch::OnInitSpells() 
{
  UpdateData(TRUE);
  m_data.maxLevel = m_MaxLevel.GetCurSel()+1;
 
	m_data.templeSpells = globalData.fixSpellBook;
	
}
