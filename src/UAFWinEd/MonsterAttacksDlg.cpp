/******************************************************************************
* Filename: MonsterAttacksDlg.cpp
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
#include "MonsterAttackDetails.h"
#include "MonsterAttacksDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonsterAttacksDlg dialog


//CMonsterAttacksDlg::CMonsterAttacksDlg(MONSTER_ATTACK_DATA &data, CWnd* pParent /*=NULL*/)
CMonsterAttacksDlg::CMonsterAttacksDlg(ATTACK_DATA& data, CWnd* pParent /*=NULL*/)
  : CDialog(CMonsterAttacksDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonsterAttacksDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_data = data;
}


void CMonsterAttacksDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonsterAttacksDlg)
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonsterAttacksDlg, CDialog)
	//{{AFX_MSG_MAP(CMonsterAttacksDlg)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
	ON_BN_CLICKED(IDC_EDITITEM, OnEdititem)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonsterAttacksDlg message handlers

void CMonsterAttacksDlg::OnAdditem() 
{
  //MONSTER_ATTACK_DETAILS data;
  ATTACK_DETAILS data;
  data.attackMsg = "attacks";
  data.sides = 6;
  CMonsterAttackDetails dlg(data);
  if (dlg.DoModal()==IDOK)
  {
    dlg.GetData(data);
    AddToAvailList(m_data.Add(data), data);
  }
}

void CMonsterAttacksDlg::OnDeleteitem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    int key = m_AvailList.GetItemData(nItem);
    m_data.Delete(key);    

    // refill avail list
    m_AvailList.DeleteAllItems();
	  for (int i=0;i<m_data.GetMonsterAttackDetailsCount();i++)
      AddToAvailList(i, *m_data.GetMonsterAttackDetails(i));
 
    if (m_AvailList.GetItemCount() > 0)
    {
      m_AvailList.Sort();
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);  
      m_AvailList.SelectItem(0, TRUE);
    }	
  }		
}

void CMonsterAttacksDlg::OnEdititem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    int key = m_AvailList.GetItemData(nItem);
    //MONSTER_ATTACK_DETAILS data;
    ATTACK_DETAILS data;
    data = *m_data.GetMonsterAttackDetails(key);
    CMonsterAttackDetails dlg(data);
    if (dlg.DoModal()==IDOK)
    {
      dlg.GetData(data);
      m_data.SetMonsterAttackDetailsAt(key, data);
      UpdateAvailList(nItem, data);
    }
  }	
}

void CMonsterAttacksDlg::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdititem();	
	*pResult = 0;
}

BOOL CMonsterAttacksDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  /*
  int sides; // nbr sides on each dice
  int nbr;   // nbr of dice to roll
  int bonus; // bonus per roll
  CString msg; // attack msg
  */

	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Sides";
	lvc.cx = 75;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Nbr";
	lvc.cx = 75;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Bonus";
	lvc.cx = 75;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 3;
	lvc.pszText = "Message";
	lvc.cx = 200;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtSTRING, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

	for (int i=0;i<m_data.GetMonsterAttackDetailsCount();i++)
    AddToAvailList(i, *m_data.GetMonsterAttackDetails(i));
 
  if (m_AvailList.GetItemCount() > 0)
  {
    m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);  
    m_AvailList.SelectItem(0, TRUE);
  }	
  UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//void CMonsterAttacksDlg::AddToAvailList(int index, MONSTER_ATTACK_DETAILS &data)
void CMonsterAttacksDlg::AddToAvailList(int index, ATTACK_DETAILS& data)
{
  /*
  int sides; // nbr sides on each dice
  int nbr;   // nbr of dice to roll
  int bonus; // bonus per roll
  CString msg; // attack msg
  */

  int listIdx = m_AvailList.GetItemCount();  
  char sides[10];
  char nbr[10];
  char bonus[10];
  char msg[MAX_MONSTER_ATTACK_MSG_LEN+1];

  strncpy(msg, data.attackMsg, MAX_MONSTER_ATTACK_MSG_LEN);
  msg[MAX_MONSTER_ATTACK_MSG_LEN]='\0';
  sprintf(sides, "%i", data.sides);
  sprintf(nbr, "%i", data.nbr);
  sprintf(bonus, "%i", data.bonus);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = sides;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = index; // array index for this attack data

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, nbr); 
  m_AvailList.SetItemText(listIdx, 2, bonus); 
  m_AvailList.SetItemText(listIdx, 3, msg); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

//void CMonsterAttacksDlg::UpdateAvailList(int listIdx, MONSTER_ATTACK_DETAILS &data)
void CMonsterAttacksDlg::UpdateAvailList(int listIdx, ATTACK_DETAILS& data)
{
  /*
  int sides; // nbr sides on each dice
  int nbr;   // nbr of dice to roll
  int bonus; // bonus per roll
  CString msg; // attack msg
  */

  char sides[10];
  char nbr[10];
  char bonus[10];
  char msg[MAX_MONSTER_ATTACK_MSG_LEN+1];

  strncpy(msg, data.attackMsg, MAX_MONSTER_ATTACK_MSG_LEN);
  msg[MAX_MONSTER_ATTACK_MSG_LEN]='\0';
  sprintf(sides, "%i", data.sides);
  sprintf(nbr, "%i", data.nbr);
  sprintf(bonus, "%i", data.bonus);

  m_AvailList.SetItemText(listIdx, 0, sides);
  m_AvailList.SetItemText(listIdx, 1, nbr); 
  m_AvailList.SetItemText(listIdx, 2, bonus); 
  m_AvailList.SetItemText(listIdx, 3, msg); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}
