/******************************************************************************
* Filename: MagicalItemProp.cpp
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
#include "PicData.h"
//#include "attrib.h"
#include "spell.h"
#include "SpellDlg.h"
#include "GlobalData.h"
#include "ChooseSpeclAbDlg.h"
#include "GameEvent.h"
#include "EventViewer.h"

#include "MagicalItemProp.h"
#include "EditAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMagicalItemProp dialog


CMagicalItemProp::CMagicalItemProp(ITEM_DATA& item, CWnd* pParent /*=NULL*/)
	: CDialog(CMagicalItemProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMagicalItemProp)
	m_AttackBonus = 0;
	m_Cursed = FALSE;
	m_IDName = _T("");
	m_ProtectionBonus = 0;
	m_Usable = FALSE;
  m_Scribable = FALSE;
  m_NotMagical = FALSE;
	m_NumCharges = 0;
	m_SpellLevel = 0;
	m_RechargeRate = 0;
	//}}AFX_DATA_INIT

  m_ItemData = item;
  m_SpellNum=-1;
}


void CMagicalItemProp::DoDataExchange(CDataExchange* pDX)
{
  if (!pDX->m_bSaveAndValidate)
  {
    m_IDName = m_ItemData.IdName();
    m_AttackBonus = m_ItemData.Attack_Bonus;
    m_Cursed = m_ItemData.Cursed;
    m_NumCharges = m_ItemData.Num_Charges;
    m_ProtectionBonus = m_ItemData.Protection_Bonus;
    m_Usable = m_ItemData.IsUsable();
    m_Scribable = m_ItemData.IsScribable();
    m_NotMagical = m_ItemData.IsNotMagical();
    m_RechargeRate = m_ItemData.Recharge_Rate;

    //m_SpellLevel = m_ItemData.Spell_Level;
    
    CWnd *pWnd;
    BOOL enable;

    enable = m_ItemData.IsUsable() || m_ItemData.IsScribable();
      
    pWnd = GetDlgItem(IDC_SPELLLEVEL);
    if (pWnd != NULL)
      pWnd->EnableWindow(enable);

    pWnd = GetDlgItem(IDC_NUMCHARGES);
    if (pWnd != NULL)
      pWnd->EnableWindow(enable);

    if (m_ChooseSpell.m_hWnd != NULL)
    {
      m_ChooseSpell.EnableWindow(enable);
      CString spellName;
      spellName = m_ItemData.SpellID();
      if (spellName.IsEmpty())
      {
        spellName = "<Choose Spell>";
      };
      m_ChooseSpell.SetWindowText(spellName);
    };
      
  }

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMagicalItemProp)
	DDX_Control(pDX, IDC_USEEVENT, m_UseEvent);
	DDX_Control(pDX, IDC_CHOOSESPELL, m_ChooseSpell);
	DDX_Text(pDX, IDC_ATTACKBONUS, m_AttackBonus);
	DDX_Check(pDX, IDC_CURSED, m_Cursed);
	DDX_Text(pDX, IDC_IDENTIFIEDNAME, m_IDName);
	DDX_Text(pDX, IDC_PROTECTIONBONUS, m_ProtectionBonus);
	DDX_Check(pDX, IDC_USABLE, m_Usable);
	DDX_Check(pDX, IDC_SCRIBABLE, m_Scribable);
  DDX_Check(pDX, IDC_NotMagical, m_NotMagical);
	DDX_Text(pDX, IDC_NUMCHARGES, m_NumCharges);
	DDX_Text(pDX, IDC_SPELLLEVEL, m_SpellLevel);
	DDX_Radio(pDX, IDC_CHARGESTOTAL, m_RechargeRate);
	//}}AFX_DATA_MAP

  if (pDX->m_bSaveAndValidate)
  {
    m_ItemData.SetIdName(m_IDName);
    m_ItemData.Attack_Bonus = m_AttackBonus;
    m_ItemData.Cursed = m_Cursed;
    m_ItemData.Num_Charges = m_NumCharges;
    m_ItemData.Protection_Bonus = m_ProtectionBonus;
    m_ItemData.IsUsable(m_Usable);
    m_ItemData.IsScribable(m_Scribable);
    m_ItemData.IsNotMagical(m_NotMagical);
    //m_ItemData.Spell_Level = m_SpellLevel;
    m_ItemData.Recharge_Rate = (itemRechargeRate)m_RechargeRate;
    
    CWnd *pWnd;
    BOOL enable;

    enable = m_ItemData.IsUsable() || m_ItemData.IsScribable();
      
    pWnd = GetDlgItem(IDC_SPELLLEVEL);
    if (pWnd != NULL)
      pWnd->EnableWindow(enable);

    pWnd = GetDlgItem(IDC_NUMCHARGES);
    if (pWnd != NULL)
      pWnd->EnableWindow(enable);

    m_ChooseSpell.EnableWindow(enable);
  }
}


BEGIN_MESSAGE_MAP(CMagicalItemProp, CDialog)
	//{{AFX_MSG_MAP(CMagicalItemProp)
	ON_BN_CLICKED(IDC_CHOOSESPELL, OnChoosespell)
	ON_BN_CLICKED(IDC_USABLE, OnUsable)
	ON_BN_CLICKED(IDC_SCRIBABLE, OnScribable)
	ON_BN_CLICKED(IDC_SPECABS, OnSpecabs)
	ON_BN_CLICKED(IDC_USEEVENT, OnUseevent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMagicalItemProp message handlers

BOOL CMagicalItemProp::OnInitDialog() 
{
  CDialog::OnInitDialog();

  char name[MAX_MEDITBUTTON_TEXT+1];
  //getBaseName(spellData.GetSpellName(m_ItemData.m_gsID), 
  getBaseName(spellData.GetSpellName(m_ItemData.SpellID()), 
              name, MAX_MEDITBUTTON_TEXT);
  m_ChooseSpell.SetWindowText(name);

  if (!globalData.eventData.IsValidEvent(m_ItemData.m_useEvent))
    m_ItemData.m_useEvent=NO_EVENT;

  getBaseName(GetEventIdDescription(m_ItemData.m_useEvent, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
  m_UseEvent.SetWindowText(name);

  if (m_ItemData.m_useEvent != NO_EVENT)
    m_ItemData.IsUsable(TRUE);



  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMagicalItemProp::OnChoosespell() 
{
  UpdateData(TRUE);
  spellBookType m_books;
  m_books.spellLimits.SetToMaxLimits();

  // add single spell to book
  //if ((m_ItemData.m_gsID.IsValidSpell()))
  if ((m_ItemData.SpellID().IsValidSpell()))
  {
    //m_books.KnowSpell(m_ItemData.Spell_Class,
    //                  m_ItemData.Spell_Class,                       
    //                  //m_ItemData.m_gsID, 
    //                  m_ItemData.SpellID(), 
    //                  m_ItemData.Spell_Level,
    //                  TRUE);
    BASECLASS_ID baseclassID;
    m_books.KnowSpell(baseclassID,
                      SCHOOL_ID(),                       
                      //m_ItemData.m_gsID, 
                      m_ItemData.SpellID(), 
                      0,
                      TRUE);
  }

  // allow any spell class to be chosen
  CSpell dlg(m_books, 1);

  if (dlg.DoModal() == IDOK)
  {
    //m_ItemData.Spell_Class = 1;
    //m_ItemData.Spell_Level = 0;
    //m_ItemData.m_gsID.Clear();    
    m_ItemData.spellID.Clear();    
    
    dlg.GetSpellList(m_books);

    //m_books.SortSpells();
/*
    POSITION pos = m_books.GetHeadPosition();
    if (pos != NULL)
    {
      SPELL_DATA *data;
      if ((data = spellData.GetSpellData(m_books.PeekAtPos(pos).spell)) != NULL)
      {
        m_ItemData.Spell_Class = data->schoolMask;
        m_ItemData.Spell_Level = data->Level;
        m_ItemData.m_gsID = m_books.PeekAtPos(pos).spell;
      }
    }
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(spellData.GetSpellName(m_ItemData.m_gsID), 
                name, MAX_MEDITBUTTON_TEXT);
    m_ChooseSpell.SetWindowText(name);     
  }
*/
    int i,n;
    n = m_books.GetCount();
    for (i=0; i<1; i++)
    {
      const SPELL_DATA *pSpell;
      if ((pSpell = spellData.PeekSpell(m_books.PeekCharacterSpell(i)->spellID)) != NULL)
      {
        //m_ItemData.Spell_Class = pSpell->schoolMask;
        //m_ItemData.Spell_Level = pSpell->Level;
        //m_ItemData.m_gsID = m_books.PeekCharacterSpell(i)->spellID;
        m_ItemData.SpellID(m_books.PeekCharacterSpell(i)->spellID);
      }
    };
  };
  UpdateData(FALSE);
}

void CMagicalItemProp::OnUsable() 
{
	UpdateData(TRUE);
  UpdateData(FALSE);	
}

void CMagicalItemProp::OnScribable() 
{
	UpdateData(TRUE);
  UpdateData(FALSE);	
}

void CMagicalItemProp::OnSpecabs() 
{
  CString title;
  title.Format("Special Abilities for item %s", m_ItemData.IdName());
  CEditAttributes dlg(m_ItemData.specAbs, title);
  if (dlg.DoModal() == IDOK)
  {;
    m_ItemData.specAbs.Copy(dlg.m_data);
  };
//   CChooseSpeclAbDlg	dlg(m_ItemData.specAbs, title);
//   if (dlg.DoModal() == IDOK)
//     dlg.GetData(m_ItemData.specAbs);
}

void CMagicalItemProp::OnUseevent() 
{
  UpdateData(TRUE);
  CEventViewer dlg(m_ItemData.GetItemEventText(), m_ItemData.m_useEvent);
  if (dlg.DoModal() == IDOK)
  {
    m_ItemData.m_useEvent = dlg.m_SingleEventRoot;

    if (!globalData.eventData.IsValidEvent(m_ItemData.m_useEvent))
      m_ItemData.m_useEvent=NO_EVENT;

    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetEventIdDescription(m_ItemData.m_useEvent,GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
    m_UseEvent.SetWindowText(name);

    if (m_ItemData.m_useEvent != NO_EVENT)
      m_ItemData.IsUsable(TRUE);
  }
  UpdateData(FALSE);
}
