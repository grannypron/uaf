/******************************************************************************
* Filename: MonsterAttackDetails.cpp
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
//#include "..\Shared\Version.h"
#include "resource.h"

#include "UAFWinEd.h"
#include "Spell.h"
#include "SpellDlg.h"
#include "MonsterAttackDetails.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonsterAttackDetails dialog


//CMonsterAttackDetails::CMonsterAttackDetails(MONSTER_ATTACK_DETAILS &data, CWnd* pParent /*=NULL*/)
CMonsterAttackDetails::CMonsterAttackDetails(ATTACK_DETAILS& data, CWnd* pParent /*=NULL*/)
  : CDialog(CMonsterAttackDetails::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonsterAttackDetails)
	m_Bonus = data.bonus;
	m_Sides = data.sides;
	m_Nbr = data.nbr;
	m_Msg = data.attackMsg;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CMonsterAttackDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonsterAttackDetails)
	DDX_Control(pDX, IDC_INVOKESPELL, m_InvokeSpell);
	DDX_Text(pDX, IDC_ATTACKBONUS, m_Bonus);
	DDX_Text(pDX, IDC_ATTACKDICE, m_Sides);
	DDX_Text(pDX, IDC_ATTACKNBRDICE, m_Nbr);
	DDX_Text(pDX, IDC_MONSTERATTACKMSG, m_Msg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonsterAttackDetails, CDialog)
	//{{AFX_MSG_MAP(CMonsterAttackDetails)
	ON_BN_CLICKED(IDC_INVOKESPELL, OnInvokespell)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonsterAttackDetails message handlers

void CMonsterAttackDetails::OnOK() 
{
	UpdateData(TRUE);

  if (m_Msg.GetLength() > MAX_MONSTER_ATTACK_MSG_LEN) m_Msg = m_Msg.Left(MAX_MONSTER_ATTACK_MSG_LEN);

  //if (!m_data.spelldbkey.IsValidSpell())
  if (!m_data.spellID.IsValidSpell())
  {
    // if no spell, then some sort of attack is required
	  if (m_Sides < 1) m_Sides = 1;
    if (m_Nbr < 1) m_Nbr = 1;
  }
  
  // bonus can be positive or negative

  m_data.attackMsg = m_Msg;
  m_data.sides = m_Sides;
  m_data.nbr = m_Nbr;
  m_data.bonus = m_Bonus;

	CDialog::OnOK();
}

BOOL CMonsterAttackDetails::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  if (m_Msg.GetLength() > MAX_MONSTER_ATTACK_MSG_LEN) m_Msg = m_Msg.Left(MAX_MONSTER_ATTACK_MSG_LEN);
	if (m_Sides < 1) m_Sides = 1;
  if (m_Nbr < 1) m_Nbr = 1;
	
  char name[MAX_MEDITBUTTON_TEXT+1];
  //getBaseName(spellData.GetSpellName(m_data.spelldbkey), 
  getBaseName(spellData.GetSpellName(m_data.spellID), 
              name, MAX_MEDITBUTTON_TEXT);
  m_InvokeSpell.SetWindowText(name);       
  
  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonsterAttackDetails::OnInvokespell() 
{
  spellBookType m_books;
  m_books.spellLimits.SetToMaxLimits();
  
  // add single spell to book
  //if (m_data.spelldbkey.IsValidSpell())
  if (m_data.spellID.IsValidSpell())
  {
    //m_books.KnowSpell(m_data.spellclass,
    //                  m_data.spellclass,                       
    //                  //m_data.spelldbkey, 
    //                  m_data.spellID, 
    //                  m_data.spelllevel,
    //                  TRUE);
    BASECLASS_ID baseclassID;
    m_books.KnowSpell(baseclassID,
                      SCHOOL_ID(),                       
                      //m_data.spelldbkey, 
                      m_data.spellID, 
                      0,
                      TRUE);
  }
  
  // allow any spell class to be chosen
  CSpell dlg(m_books, 1);
  
  if (dlg.DoModal() == IDOK)
  {
    m_data.spellclass = 0;
    m_data.spelllevel = 0;
    //m_data.spelldbkey.Clear();      
    m_data.spellID.Clear();      
    
    dlg.GetSpellList(m_books);
/*
    POSITION pos = m_books.GetHeadPosition();
    if (pos != NULL)
    {
      SPELL_DATA *data;
      if ((data = spellData.GetSpellData(m_books.PeekAtPos(pos).spell)) != NULL)
      {
        m_data.spellclass = data->schoolMask;
        m_data.spelllevel = data->Level;
        m_data.spelldbkey = m_books.PeekAtPos(pos).spell;      
      }
    }
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(spellData.GetSpellName(m_data.spelldbkey), 
                name, MAX_MEDITBUTTON_TEXT);
    m_InvokeSpell.SetWindowText(name);     
  }
*/
    //int i;
    //for (i=0; i<1; i++)
    //{
    //  SPELL_DATA *pSpell;
    //  if ((pSpell = spellData.GetSpellData(m_books.PeekCharacterSpell(i)->spellID)) != NULL)
    //  {
    //    m_data.spellclass = pSpell->schoolMask;
    //    m_data.spelllevel = pSpell->Level;
    //    //m_data.spelldbkey = m_books.PeekAtPos(pos).spell;      
    //  }
    //}
    SPELL_ID spellID;
    if (m_books.GetCount() > 0)
    {
      spellID = m_books.PeekCharacterSpell(0)->spellID;
    }
    else
    {
      spellID.Empty();
    }
    m_data.spellID = spellID;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(spellData.GetSpellName(spellID), 
                name, MAX_MEDITBUTTON_TEXT);
    m_InvokeSpell.SetWindowText(name);     
  }
}
