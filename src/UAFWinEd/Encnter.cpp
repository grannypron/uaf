/******************************************************************************
* Filename: Encnter.cpp
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
#include "buttons.h"
#include "SelectPicDlg.h"
#include "Encnter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEncounter dialog


CEncounter::CEncounter(ENCOUNTER_DATA& encounter, CWnd* pParent /*=NULL*/)
: CDialog(CEncounter::IDD, pParent)
{
   //{{AFX_DATA_INIT(CEncounter)
   m_MonsterSpeed = 0;
   m_Text = _T("");
	//}}AFX_DATA_INIT
   
   m_encounter = encounter;
}


void CEncounter::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CEncounter)
	DDX_Control(pDX, IDC_SEEMONSTER, m_SeeMonster);
   DDX_Control(pDX, IDC_DISTANCE, m_Distance);
   DDX_Control(pDX, IDC_RANGEZERO, m_RangeZero);
   DDX_Text(pDX, IDC_MONSTERSPEED, m_MonsterSpeed);
   DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncounter, CDialog)
//{{AFX_MSG_MAP(CEncounter)
ON_BN_CLICKED(IDC_BUTTONCHOICES, OnButtonchoices)
ON_CBN_SELCHANGE(IDC_DISTANCE, OnSelchangeDistance)
ON_CBN_SELCHANGE(IDC_RANGEZERO, OnSelchangeRangezero)
ON_BN_CLICKED(IDC_SEEMONSTER, OnSeemonster)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncounter message handlers

BOOL CEncounter::OnInitDialog() 
{
   int i;
   CDialog::OnInitDialog();
   
   m_RangeZero.ResetContent();
   m_Distance.ResetContent();
   
   for (i=0;i<NUM_DIST_TYPES;i++)
      m_Distance.AddString(DistanceText[i]);

   /*
   enum encounterButtonResultType 
   { 
     NoResult, 
     DecreaseRange, 
     CombatNoSurprise, 
     CombatSlowPartySurprised, 
     CombatSlowMonsterSurprised,
     Talk, 
     EscapeIfFastPartyElseCombat, 
     ChainToEvent 
   }
   */

   int selidx = -1;
   for (i=0;i<NUM_ENCOUNTER_BUTTON_OPTION_TYPES;i++)
   {
      encounterButtonResultType type = (encounterButtonResultType)i;

       // exclude DecreaseRange and ChainToEvent options
      if ((type != DecreaseRange) && (type != ChainToEvent))
      {
        int index = m_RangeZero.AddString(ButtonOptionText[i]);
        m_RangeZero.SetItemData(index, (int)type);

        if (type == m_encounter.zeroRangeResult)
          selidx = index;
      }
   }
   
   m_Distance.SetCurSel((int)m_encounter.distance);
   
   if (m_encounter.zeroRangeResult == DecreaseRange)
     m_encounter.zeroRangeResult = NoResult;

   if (selidx >= 0)
     m_RangeZero.SetCurSel(selidx);
   else
     m_RangeZero.SetCurSel(0);
   
   m_MonsterSpeed = m_encounter.monsterSpeed;
   m_Text = m_encounter.text;
   
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_encounter.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_SeeMonster.SetWindowText(name);

   GotoDlgCtrl(GetDlgItem(IDC_TEXT));

   UpdateData(FALSE);
   
   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncounter::OnButtonchoices() 
{
   CButtons dlg(m_encounter.buttons);
   
   UINT result = dlg.DoModal();
   
   if (result == IDOK)
      dlg.GetButtonData(m_encounter.buttons);
}

void CEncounter::OnSelchangeDistance() 
{
   m_encounter.distance = 
      (eventDistType) m_Distance.GetCurSel();
}

void CEncounter::OnSelchangeRangezero() 
{
  m_encounter.zeroRangeResult = 
    (encounterButtonResultType) m_RangeZero.GetItemData(m_RangeZero.GetCurSel()); 
}

void CEncounter::OnOK() 
{
   UpdateData(TRUE);
   
   m_encounter.zeroRangeResult = 
     (encounterButtonResultType)m_RangeZero.GetItemData(m_RangeZero.GetCurSel()); 
   m_encounter.distance = (eventDistType)m_Distance.GetCurSel();
   m_encounter.monsterSpeed = m_MonsterSpeed;
   m_encounter.text = m_Text;   
   
   CDialog::OnOK();
}

void CEncounter::GetEncounterData(ENCOUNTER_DATA& encounter)
{
   encounter = m_encounter;
}

void CEncounter::OnSeemonster() 
{
   CSelectPicDlg dlg(m_encounter.pic, SmallPicDib | SpriteDib | BigPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_encounter.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_encounter.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SeeMonster.SetWindowText(name);
   }
}
