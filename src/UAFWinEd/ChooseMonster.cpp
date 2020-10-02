/******************************************************************************
* Filename: ChooseMonster.cpp
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

//#include "UAFWinEd.h"
#include "ChooseMonster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseMonster dialog


//CChooseMonster::CChooseMonster(int MonsterNum, int max, CWnd* pParent /*=NULL*/)
CChooseMonster::CChooseMonster(const MONSTER_ID& monsterID, int max, CWnd* pParent /*=NULL*/)
	: CDialog(CChooseMonster::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseMonster)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  //m_MonsterNum = MonsterNum;
  m_monsterID = monsterID;
  m_max = max;
}


void CChooseMonster::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseMonster)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseMonster, CDialog)
	//{{AFX_MSG_MAP(CChooseMonster)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseMonster message handlers

void CChooseMonster::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnDelete();	
	*pResult = 0;
}

void CChooseMonster::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();	
	*pResult = 0;
}

void CChooseMonster::OnAdd() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //DWORD flag = m_AvailList.GetItemData(nItem);
    int indx = m_AvailList.GetItemData(nItem);

    m_ChosenList.DeleteAllItems();
    //AddToChosenList((int)flag);
    AddToChosenList(monsterData.PeekMonster(indx)->MonsterID());
  }	
}

void CChooseMonster::OnDelete() 
{
	m_ChosenList.DeleteAllItems();		
}


BOOL CChooseMonster::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "AC";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtINT, true);
  m_ChosenList.InsertColumn(1,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 2;
	lvc.pszText = "HD";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtINT, true);
  m_ChosenList.InsertColumn(2,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 3;
	lvc.pszText = "THAC0";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtINT, true);
  m_ChosenList.InsertColumn(3,&lvc, CListSort::dtINT, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  //POSITION pos = monsterData.GetHeadPosition();
  //while (pos != NULL)
  {
    int i, n;
    n = monsterData.GetCount();
    for (i=0; i<n; i++)
    {
      //AddToAvailList(monsterData.GetNext(pos));
      AddToAvailList(i);
      a_count++;
    };
  }

  if (a_count > 0)
  {
    m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }

  //AddToChosenList(m_MonsterNum);
  AddToChosenList(m_monsterID);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//void CChooseMonster::AddToAvailList(MONSTER_DATA &data)
void CChooseMonster::AddToAvailList(int i)
{

  MONSTER_DATA *pMonster;
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_MONSTER_NAME+1];
  char ac[10];
  char hd[10];
  char thac0[10];

  pMonster = monsterData.GetMonster(i);
  if (pMonster->MonsterID() == "$$Help")
  {
    return;
  };

  //strncpy(name, data.Name, MAX_MONSTER_NAME);
  strncpy(name, pMonster->Name, MAX_MONSTER_NAME);
  name[MAX_MONSTER_NAME]='\0';
  //sprintf(ac, "%i", data.Armor_Class);
  sprintf(ac, "%i", pMonster->Armor_Class);
  //if (data.Hit_Dice_Bonus != 0)
  if (pMonster->Hit_Dice_Bonus != 0)
    //sprintf(hd, "%02.1f+%i", data.Hit_Dice, data.Hit_Dice_Bonus);
    sprintf(hd, "%02.1f+%i", pMonster->Hit_Dice, pMonster->Hit_Dice_Bonus);
  else
    //sprintf(hd, "%02.1f", data.Hit_Dice);
    sprintf(hd, "%02.1f", pMonster->Hit_Dice);
  //sprintf(thac0, "%i", data.THAC0);
  sprintf(thac0, "%i", pMonster->THAC0);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	//item.lParam = data.key;
	item.lParam = i;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, ac); 
  m_AvailList.SetItemText(listIdx, 2, hd); 
  m_AvailList.SetItemText(listIdx, 3, thac0); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

//void CChooseMonster::AddToChosenList(int index)
void CChooseMonster::AddToChosenList(const MONSTER_ID& monsterID)
{
  //MONSTER_DATA *pMonster;
  const MONSTER_DATA *pMonster = NULL;
  //pMonster = monsterData.GetMonster(index);
  int monsterIndex;
  monsterIndex = monsterData.LocateMonster(monsterID);
  if (monsterIndex >= 0)
  {
    pMonster = monsterData.PeekMonster(monsterIndex);
  };
  if (pMonster == NULL)
  {
    WriteDebugString("Bogus monster index in CChooseMonster::AddToChosenList\n");
    return;
  }

  int listIdx = m_ChosenList.GetItemCount();

  char name[MAX_MONSTER_NAME+1];
  char ac[10];
  char hd[10];
  char thac0[10];

  //strncpy(name, data->Name, MAX_MONSTER_NAME);
  strncpy(name, pMonster->Name, MAX_MONSTER_NAME);
  name[MAX_MONSTER_NAME]='\0';
  //sprintf(ac, "%i", data->Armor_Class);
  sprintf(ac, "%i", pMonster->Armor_Class);
  //if (data->Hit_Dice_Bonus != 0)
  if (pMonster->Hit_Dice_Bonus != 0)
    //sprintf(hd, "%02.1f+%i", data->Hit_Dice, data->Hit_Dice_Bonus);
    sprintf(hd, "%02.1f+%i", pMonster->Hit_Dice, pMonster->Hit_Dice_Bonus);
  else
    //sprintf(hd, "%02.1f", data->Hit_Dice);
    sprintf(hd, "%02.1f", pMonster->Hit_Dice);
  //sprintf(thac0, "%i", data->THAC0);
  sprintf(thac0, "%i", pMonster->THAC0);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
  item.lParam = monsterIndex;
  //sprintf(thac0, "%i", data->THAC0);
  sprintf(thac0, "%i", pMonster->THAC0);

  m_ChosenList.InsertItem(&item);
  m_ChosenList.SetItemText(listIdx, 1, ac); 
  m_ChosenList.SetItemText(listIdx, 2, hd); 
  m_ChosenList.SetItemText(listIdx, 3, thac0); 
  //m_ChosenList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(listIdx, TRUE);

  //bool enable = (m_ChosenList.GetItemCount() < m_max);
  //CWnd *pwnd = GetDlgItem(IDC_ADD);    
  //if (pwnd != NULL) pwnd->EnableWindow(enable);  
}

void CChooseMonster::OnOK() 
{
  UpdateData(TRUE);

  if (m_ChosenList.GetItemCount() > 0)
  {
    //m_MonsterNum = (int)m_ChosenList.GetItemData(0);
    const MONSTER_DATA *pMonster;
    pMonster = monsterData.PeekMonster(m_ChosenList.GetItemData(0));
    if (pMonster != NULL)
    {
      m_monsterID = pMonster->MonsterID();
    }
    else
    {
      m_monsterID.Empty();
    };
  };
	
	CDialog::OnOK();
}
