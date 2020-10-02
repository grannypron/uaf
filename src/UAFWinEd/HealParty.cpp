/******************************************************************************
* Filename: HealParty.cpp
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

#include "globtext.h"
//#include "GlobalData.h"
//#include "GameEvent.h"
#include "PicData.h"

#include "SelectPicDlg.h"
#include "class.h"
#include "HealParty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHealParty dialog


CHealParty::CHealParty(HEAL_PARTY_DATA &data, CWnd* pParent /*=NULL*/)
: CDialog(CHealParty::IDD, pParent)
{
  //{{AFX_DATA_INIT(CHealParty)
  m_HealCurse = FALSE;
  m_HealDrain = FALSE;
  m_HealHP = FALSE;
  m_Text = _T("");
  m_Chance = 0;
  m_LiteralOrPercent = 1;
  m_HowMuchHP = 100;
  //}}AFX_DATA_INIT
  m_data = data;
}


void CHealParty::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CHealParty)
  DDX_Control(pDX, IDC_SEECHAR, m_SeeChar);
  DDX_Control(pDX, IDC_HEALWHO, m_HealWho);
  DDX_Check(pDX, IDC_HEALCURSE, m_HealCurse);
  DDX_Check(pDX, IDC_HEALDRAIN, m_HealDrain);
  DDX_Check(pDX, IDC_HEALHP, m_HealHP);
  DDX_Text(pDX, IDC_TEXT, m_Text);
  DDX_Text(pDX, IDC_CHANCE, m_Chance);
  DDX_Radio(pDX, IDC_LITERALORPERCENT, m_LiteralOrPercent);
  DDX_Text(pDX, IDC_HOWMUCHHP, m_HowMuchHP);
  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHealParty, CDialog)
//{{AFX_MSG_MAP(CHealParty)
ON_BN_CLICKED(IDC_SEECHAR, OnSeechar)
ON_CBN_SELCHANGE(IDC_HEALWHO, OnSelchangeHealwho)
ON_BN_CLICKED(IDC_HEALHP, OnHealhp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CHealParty::UpdateControls()
{
  UpdateData(TRUE);
  CWnd *pwnd;
  BOOL enable = m_HealHP;
  pwnd = GetDlgItem(IDC_LITERALORPERCENT);
  if (pwnd!=NULL) pwnd->EnableWindow(enable);
  pwnd = GetDlgItem(IDC_HOWMUCHHP);
  if (pwnd!=NULL) pwnd->EnableWindow(enable);
  pwnd = GetDlgItem(IDC_LITERALORPERCENT2);
  if (pwnd!=NULL) pwnd->EnableWindow(enable);
  pwnd = GetDlgItem(IDC_LITERALORPERCENT3);
  if (pwnd!=NULL) pwnd->EnableWindow(enable);
}

/////////////////////////////////////////////////////////////////////////////
// CHealParty message handlers

BOOL CHealParty::OnInitDialog() 
{
  CDialog::OnInitDialog();
  
  m_HealHP = m_data.HealHP;
  m_HealCurse = m_data.HealCurse;
  m_HealDrain = m_data.HealDrain;
  m_Text = m_data.text;
  m_HowMuchHP = m_data.HowMuchHP;
  m_LiteralOrPercent = m_data.LiteralOrPercent;
  
  m_HealWho.ResetContent();
  
  for(int i=0;i<NUM_AFFECT_TYPES;i++)
    m_HealWho.AddString(AffectsWhoText[i]);
  
  m_HealWho.SetCurSel((int)m_data.who);
  
  m_Chance = m_data.chance;
  
  if (m_data.who != ChanceOnEach)
  {
    m_Chance = 100;
    CWnd *pWnd = GetDlgItem(IDC_CHANCE);
    if (pWnd != NULL) pWnd->EnableWindow(FALSE);
  }
  
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeChar.SetWindowText(name);
  
  GotoDlgCtrl(GetDlgItem(IDC_TEXT));
  UpdateData(FALSE);
  UpdateControls();
  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CHealParty::OnSeechar() 
{
  UpdateData(TRUE);
  
  CSelectPicDlg dlg(m_data.pic, SmallPicDib | BigPicDib | SpriteDib);
  
  if (dlg.DoModal() == IDOK)
  {
    dlg.GetData(m_data.pic);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
    m_SeeChar.SetWindowText(name);
  }  
}

void CHealParty::OnSelchangeHealwho() 
{
  m_data.who = (eventPartyAffectType) m_HealWho.GetCurSel();
  CWnd *pWnd = GetDlgItem(IDC_CHANCE);
  
  if (m_data.who != ChanceOnEach)
  {
    m_Chance = 100;
    if (pWnd != NULL) pWnd->EnableWindow(FALSE);
    UpdateData(TRUE);
    UpdateData(FALSE);
  }
  else
  {
    if (pWnd != NULL) pWnd->EnableWindow(TRUE);
  } 
}


void CHealParty::OnOK() 
{
  UpdateData(TRUE);
  m_data.who = (eventPartyAffectType) m_HealWho.GetCurSel();
  m_data.text = m_Text;
  m_data.chance = m_Chance;
  m_data.HealHP = m_HealHP;
  m_data.HealCurse = m_HealCurse;
  m_data.HealDrain = m_HealDrain;
  m_data.HowMuchHP = m_HowMuchHP;
  m_data.LiteralOrPercent = m_LiteralOrPercent;
  
  CDialog::OnOK();
}

void CHealParty::OnHealhp() 
{
  UpdateControls();   
}
