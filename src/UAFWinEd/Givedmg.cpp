/******************************************************************************
* Filename: Givedmg.cpp
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
#include "globtext.h"
#include "GlobalData.h"
#include "GameEvent.h"

#include "SelectPicDlg.h"
#include "GiveDmg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GiveDmg dialog


GiveDmg::GiveDmg(GIVE_DAMAGE_DATA& damage, CWnd* pParent /*=NULL*/)
	: CDialog(GiveDmg::IDD, pParent)
{
	//{{AFX_DATA_INIT(GiveDmg)
	m_AttackTHAC0 = 20;
	m_Chance = 100;
	m_NbrAttacks = 1;
	m_NbrDice = 1;
	m_NbrSides = 6;
	m_SavingThrowBonus = 0;
	m_DmgBonus = 0;
	m_Text = _T("");
	//}}AFX_DATA_INIT

	m_damage = damage;
}


void GiveDmg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(GiveDmg)
	DDX_Control(pDX, IDC_WHO, m_Who);
	DDX_Control(pDX, IDC_SEEART, m_SeeArt);
	DDX_Control(pDX, IDC_SAVEVERSUS, m_SaveVersus);
	DDX_Control(pDX, IDC_SAVEEFFECT, m_SaveEffect);
	DDX_Control(pDX, IDC_DISTANCE, m_Distance);
	DDX_Text(pDX, IDC_ATTACKTHAC0, m_AttackTHAC0);	
	DDX_Text(pDX, IDC_CHANCE, m_Chance);
	DDX_Text(pDX, IDC_NBRATTACKS, m_NbrAttacks);
	DDX_Text(pDX, IDC_NBRDICE, m_NbrDice);
	DDX_Text(pDX, IDC_NBRSIDES, m_NbrSides);
	DDX_Text(pDX, IDC_SAVINGTHROWBONUS, m_SavingThrowBonus);
	DDX_Text(pDX, IDC_DMGBONUS, m_DmgBonus);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GiveDmg, CDialog)
	//{{AFX_MSG_MAP(GiveDmg)
	ON_CBN_SELCHANGE(IDC_DISTANCE, OnSelchangeDistance)
	ON_CBN_SELCHANGE(IDC_SAVEEFFECT, OnSelchangeSaveeffect)
	ON_CBN_SELCHANGE(IDC_SAVEVERSUS, OnSelchangeSaveVersus)
	ON_BN_CLICKED(IDC_SEEART, OnSeeart)
	ON_CBN_SELCHANGE(IDC_WHO, OnSelchangeWho)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GiveDmg message handlers

void GiveDmg::OnSelchangeDistance() 
{
	m_damage.distance = 
		(eventDistType) m_Distance.GetCurSel();		
}

void GiveDmg::OnSelchangeSaveeffect() 
{
	m_damage.eventSave = 
		(spellSaveEffectType) m_SaveEffect.GetCurSel();
}

void GiveDmg::OnSelchangeSaveVersus() 
{
	m_damage.spellSave = 
		(spellSaveVersusType) m_SaveVersus.GetCurSel();	
}

void GiveDmg::OnSeeart() 
{
   CSelectPicDlg dlg(m_damage.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_damage.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_damage.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SeeArt.SetWindowText(name);
      //UpdateData(FALSE);
   }
}

void GiveDmg::OnSelchangeWho() 
{
	m_damage.who = 
		(eventPartyAffectType) m_Who.GetCurSel();
}

void GiveDmg::OnOK() 
{
	UpdateData(TRUE);

	m_damage.who = (eventPartyAffectType) m_Who.GetCurSel();
	m_damage.spellSave = (spellSaveVersusType) m_SaveVersus.GetCurSel();
	m_damage.eventSave = (spellSaveEffectType) m_SaveEffect.GetCurSel();
	m_damage.distance = (eventDistType) m_Distance.GetCurSel();
	m_damage.attackTHAC0 = m_AttackTHAC0;
	m_damage.chancePerAttack = m_Chance;
	m_damage.nbrAttacks = m_NbrAttacks;
	m_damage.dmgDiceQty = m_NbrDice;
	m_damage.dmgDice = m_NbrSides;
	m_damage.saveBonus = m_SavingThrowBonus;
	m_damage.dmgBonus = m_DmgBonus;
	m_damage.text = m_Text;

	CDialog::OnOK();
}

BOOL GiveDmg::OnInitDialog() 
{
	int i;
	CDialog::OnInitDialog();

	m_Who.ResetContent();
	m_SaveVersus.ResetContent();
	m_SaveEffect.ResetContent();
	m_Distance.ResetContent();

	for (i=0;i<NUM_AFFECT_TYPES;i++)
		m_Who.AddString(AffectsWhoText[i]);

	for (i=0;i<NUM_SAVE_VERSUS_TYPES;i++)
		m_SaveVersus.AddString(SaveVersusText[i]);

	for (i=0;i<NUM_SAVE_EFFECT_TYPES;i++)
		m_SaveEffect.AddString(SaveEffectText[i]);

	for (i=0;i<NUM_DIST_TYPES;i++)
		m_Distance.AddString(DistanceText[i]);

	m_Who.SetCurSel((int) m_damage.who);
	m_SaveVersus.SetCurSel((int) m_damage.spellSave);
	m_SaveEffect.SetCurSel((int)m_damage.eventSave);
	m_Distance.SetCurSel((int)m_damage.distance);

	m_AttackTHAC0 = m_damage.attackTHAC0;
	m_Chance = m_damage.chancePerAttack;
	m_NbrAttacks = m_damage.nbrAttacks;
	m_NbrDice = m_damage.dmgDiceQty ;
	m_NbrSides = m_damage.dmgDice;
	m_SavingThrowBonus = m_damage.saveBonus;
	m_DmgBonus = m_damage.dmgBonus;
	m_Text = m_damage.text;
	
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_damage.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_SeeArt.SetWindowText(name);

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

	UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void GiveDmg::GetDamageData(GIVE_DAMAGE_DATA& damage)
{
	damage = m_damage;
}
