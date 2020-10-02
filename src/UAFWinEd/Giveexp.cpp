/******************************************************************************
* Filename: Giveexp.cpp
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
//#include "GlobalData.h"
#include "items.h"
#include "char.h"
#include "GameEvent.h"
#include "SoundChooser.h"

#include "SelectPicDlg.h"
#include "GiveExp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GiveExp dialog


GiveExp::GiveExp(GAIN_EXP_DATA& exp, CWnd* pParent /*=NULL*/)
	: CDialog(GiveExp::IDD, pParent)
{
	//{{AFX_DATA_INIT(GiveExp)
	m_ExpBonus = 0;
	m_Text = _T("");
	m_Chance = 100;
	//}}AFX_DATA_INIT

   m_exp = exp;
}


void GiveExp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GiveExp)
	DDX_Control(pDX, IDC_WHO, m_Who);
	DDX_Control(pDX, IDC_SEE, m_See);
	DDX_Control(pDX, IDC_HEAR, m_Hear);
	DDX_Text(pDX, IDC_EXPBONUS, m_ExpBonus);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Text(pDX, IDC_CHANCE, m_Chance);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GiveExp, CDialog)
	//{{AFX_MSG_MAP(GiveExp)
	ON_BN_CLICKED(IDC_HEAR, OnHear)
	ON_BN_CLICKED(IDC_SEE, OnSee)
	ON_CBN_SELCHANGE(IDC_WHO, OnSelchangeWho)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GiveExp message handlers

BOOL GiveExp::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();
	
	m_Who.ResetContent();

	for(i=0;i<NUM_AFFECT_TYPES;i++)
		m_Who.AddString(AffectsWhoText[i]);

	m_Who.SetCurSel((int)m_exp.who);
	m_ExpBonus = m_exp.experience;
	m_Text = m_exp.text;
  m_Chance = m_exp.chance;
	
  if (m_exp.who != ChanceOnEach)
  {
     m_Chance = 100;
     CWnd *pWnd = GetDlgItem(IDC_CHANCE);
     if (pWnd != NULL)
        pWnd->EnableWindow(FALSE);
  }

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_exp.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_See.SetWindowText(name);

  getBaseName(m_exp.sound, name, MAX_MEDITBUTTON_TEXT);
  m_Hear.SetWindowText(name);	

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

	UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void GiveExp::OnHear() 
{
  CSoundChooser dlg(m_exp.sound, ALL_SOUND_TYPES);
  if (dlg.DoModal() == IDOK)
  {
    m_exp.sound = dlg.m_Filename;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_exp.sound, name, MAX_MEDITBUTTON_TEXT);
    m_Hear.SetWindowText(name);	
  }		
}

void GiveExp::OnSee() 
{
  UpdateData(TRUE);

   CSelectPicDlg dlg(m_exp.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_exp.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_exp.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_See.SetWindowText(name);
      //UpdateData(FALSE);
   }
}

void GiveExp::OnSelchangeWho() 
{
	m_exp.who = 
		(eventPartyAffectType) m_Who.GetCurSel();

  CWnd *pWnd = GetDlgItem(IDC_CHANCE);

   if (m_exp.who != ChanceOnEach)
   {
      m_Chance = 100;
      if (pWnd != NULL)
         pWnd->EnableWindow(FALSE);
      UpdateData(TRUE);
      UpdateData(FALSE);
   }
   else
   {
     if (pWnd != NULL)
        pWnd->EnableWindow(TRUE);
   }
}

void GiveExp::OnOK() 
{
	UpdateData(TRUE);

	m_exp.who = 
		(eventPartyAffectType) m_Who.GetCurSel();
	m_exp.experience = m_ExpBonus;
	m_exp.text = m_Text;
   m_exp.chance = m_Chance;

	CDialog::OnOK();
}

void GiveExp::GetExpData(GAIN_EXP_DATA& exp)
{
	exp = m_exp;
}
