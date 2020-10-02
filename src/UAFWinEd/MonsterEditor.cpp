/******************************************************************************
* Filename: MonsterEditor.cpp
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
#include "MonsterDbDlg.h"
#include "MonsterEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//static int m_CopyLevel;
static MONSTER_ID copyMonsterID;


/////////////////////////////////////////////////////////////////////////////
// CMonsterEditor dialog


CMonsterEditor::CMonsterEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CMonsterEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonsterEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_pMonsterData=NULL;
}


void CMonsterEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonsterEditor)
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonsterEditor, CDialog)
	//{{AFX_MSG_MAP(CMonsterEditor)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
	ON_BN_CLICKED(IDC_EDITITEM, OnEdititem)
	ON_BN_CLICKED(IDC_COPYLEVEL, OnCopylevel)
	ON_BN_CLICKED(IDC_PASTELEVEL, OnPastelevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonsterEditor message handlers

void CMonsterEditor::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdititem();		
	*pResult = 0;
}

 
void CMonsterEditor::OnAdditem() 
{
  bool success;
  int indx;
  CString msg;
  ASSERT(m_pMonsterData!=NULL); 
  
  CMonsterDBDlg dlg;
  dlg.m_MonsterData.Name = "New Monster";
 
  do {
    success=false;
    if (dlg.DoModal()!=IDOK) break;
    success=true;
    //if (m_pMonsterData->FindName(dlg.m_MonsterData.Name)!=0) 

    indx = m_pMonsterData->LocateMonster(dlg.m_MonsterData.MonsterID());
    if (indx >= 0)
    {
      msg.LoadString(IDS_HELP_monster_DupName);
      MsgBoxError(msg);
      success=false;
    };
  } while (success==false);

  if (success)
  {
    m_pMonsterData->AddMonster(&dlg.m_MonsterData);
    AddToAvailList(m_pMonsterData->GetCount()-1);
    SET_MODIFIED;
  };
}


void CMonsterEditor::OnDeleteitem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //DWORD flag = m_AvailList.GetItemData(nItem);
    int indx = m_AvailList.GetItemData(nItem);
    //m_pMonsterData->RemoveMonster(flag);
    m_pMonsterData->RemoveMonster(m_pMonsterData->PeekMonster(indx)->MonsterID());

    m_AvailList.DeleteAllItems();

    int a_count = 0;
    //pos = m_pMonsterData->GetHeadPosition();
    //while (pos != NULL)
    {
      int i, n;
      n = m_pMonsterData->GetCount();
      for (i=0; i<n; i++)
      {
        //AddToAvailList(m_pMonsterData->GetNext(pos));
        AddToAvailList(i);
        a_count++;
      };
    };

    if (a_count > 0)
    {
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);
      m_AvailList.SelectItem(0, TRUE);
    }	
  }	
}

void CMonsterEditor::OnEdititem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //DWORD flag = m_AvailList.GetItemData(nItem);
    int indx = m_AvailList.GetItemData(nItem);
    //MONSTER_DATA *pData = m_pMonsterData->GetMonsterData(flag);
    MONSTER_DATA *pData = m_pMonsterData->GetMonster(indx);
    CMonsterDBDlg dlg;

    if (pData == NULL)
    {
      dlg.m_MonsterData.Clear();
      dlg.m_MonsterData.Name="Invalid Monster";
    }
    else
      dlg.m_MonsterData = *pData;

    if (dlg.DoModal() == IDOK)
    {
      SET_MODIFIED;
      //m_pMonsterData->SetMonster(flag, dlg.m_MonsterData);
      m_pMonsterData->SetMonster(m_pMonsterData->GetMonster(indx)->MonsterID(), &dlg.m_MonsterData);
      UpdateAvailList(nItem, dlg.m_MonsterData);
    }
  }			
}

BOOL CMonsterEditor::OnInitDialog() 
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

	lvc.iSubItem = 1;
	lvc.pszText = "AC";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 2;
	lvc.pszText = "HD";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 3;
	lvc.pszText = "THAC0";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 4;
	lvc.pszText = "Nbr Attacks";
	lvc.cx = 75;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(4,&lvc, CListSort::dtINT, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  //POSITION pos = m_pMonsterData->GetHeadPosition();
  //while (pos != NULL)
  {
    int i, n;
    n = m_pMonsterData->GetCount();
    for (i=0; i<n; i++)
    {
      //AddToAvailList(m_pMonsterData->GetNext(pos));
      AddToAvailList(i);
      a_count++;
    };
  };

  if (a_count > 0)
  {
    m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }	
  
  //m_CopyLevel=-1;
  copyMonsterID.Clear();
  CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonsterEditor::AddToAvailList(int monsterIndex)
{
  const MONSTER_DATA *pMonster;
  pMonster = monsterData.PeekMonster(monsterIndex);
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_MONSTER_NAME+1];
  char ac[10];
  char hd[10];
  char thac0[10];
  char nbrattacks[10];

  strncpy(name, pMonster->Name, MAX_MONSTER_NAME);
  name[MAX_MONSTER_NAME]='\0';
  sprintf(ac, "%i", pMonster->Armor_Class);
  if (pMonster->Hit_Dice_Bonus != 0)
    sprintf(hd, "%02.1f+%i", pMonster->Hit_Dice, pMonster->Hit_Dice_Bonus);
  else
    sprintf(hd, "%02.1f", pMonster->Hit_Dice);
  sprintf(thac0, "%i", pMonster->THAC0);
  sprintf(nbrattacks, "%i", pMonster->attackData.GetMonsterAttackDetailsCount());

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	//item.lParam = pMonster->key;
  item.lParam = monsterIndex;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, ac); 
  m_AvailList.SetItemText(listIdx, 2, hd); 
  m_AvailList.SetItemText(listIdx, 3, thac0); 
  m_AvailList.SetItemText(listIdx, 4, nbrattacks); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CMonsterEditor::UpdateAvailList(int item, MONSTER_DATA &data)
{
  char name[MAX_MONSTER_NAME+1];
  char ac[10];
  char hd[10];
  char thac0[10];
  char nbrattacks[10];

  strncpy(name, data.Name, MAX_MONSTER_NAME);
  name[MAX_MONSTER_NAME]='\0';
  sprintf(ac, "%i", data.Armor_Class);
  if (data.Hit_Dice_Bonus != 0)
    sprintf(hd, "%02.1f+%i", data.Hit_Dice, data.Hit_Dice_Bonus);
  else
    sprintf(hd, "%02.1f", data.Hit_Dice);
  sprintf(thac0, "%i", data.THAC0);
  sprintf(nbrattacks, "%i", data.attackData.GetMonsterAttackDetailsCount());

  m_AvailList.SetItemText(item, 0, name); 
  m_AvailList.SetItemText(item, 1, ac); 
  m_AvailList.SetItemText(item, 2, hd); 
  m_AvailList.SetItemText(item, 3, thac0); 
  m_AvailList.SetItemText(item, 4, nbrattacks); 
  //m_AvailList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(item, TRUE);
}

void CMonsterEditor::OnPastelevel() 
{
  //if (m_CopyLevel < 0) return;
  if (copyMonsterID.IsNoMonster()) return;
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_AvailList.GetNextSelectedItem(pos);
  int indx = m_AvailList.GetItemData(nItem);
  //if (!m_pMonsterData->HaveMonster(PasteLevel)) return;
  if (indx >= m_pMonsterData->GetCount()) return;

  // copy src item to dest item
  //MONSTER_DATA *srcItem = m_pMonsterData->GetMonster(m_CopyLevel);
  const MONSTER_DATA *srcItem = m_pMonsterData->PeekMonster(copyMonsterID);
  //MONSTER_DATA *dstItem = m_pMonsterData->GetMonster(PasteLevel);
  MONSTER_DATA *dstItem = m_pMonsterData->GetMonster(indx);
  if (srcItem==NULL) return;
  if (dstItem==NULL) return;
/*
  CWaitCursor cursor; // this name search may take awhile...
  DWORD index=1;
  BOOL validname=false;
  CString name;
  while (!validname)
  {
    name.Format("%s/%u", srcItem->Name, index);
    if (name == dstItem->Name)
      validname=true;
    else
      validname = (m_pMonsterData->FindName(name)==0);
    if (index >= MAX_MONSTERS)
      break;
    index++;
  }

  if (validname)
  {
    MONSTER_DATA newData = *srcItem;
    newData.Name=name;
    newData.key = PasteLevel;
    m_pMonsterData->SetMonster(PasteLevel, newData);
    UpdateAvailList(nItem, newData);
  }
  */
  {
    MONSTER_DATA newData = *srcItem;
    newData.Name = dstItem->Name;
    //newData.key = PasteLevel;
    //m_pMonsterData->SetMonster(PasteLevel, newData);
    MONSTER_ID monsterID = m_pMonsterData->PeekMonster(indx)->MonsterID();
    m_pMonsterData->SetMonster(monsterID, &newData);
    UpdateAvailList(nItem, newData);
  };

}

void CMonsterEditor::OnCopylevel() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_AvailList.GetNextSelectedItem(pos);
  int indx = m_AvailList.GetItemData(nItem);
  //m_CopyMonster = m_pMonsterData->PeekMonster(indx)->MonsterID();
  copyMonsterID = m_pMonsterData->PeekMonster(indx)->MonsterID();

  CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
  //if (m_pMonsterData->HaveMonster(m_CopyLevel))
  if (m_pMonsterData->LocateMonster(copyMonsterID) >= 0)
  {
    if (pWnd!=NULL) pWnd->EnableWindow(TRUE);
  }
  else
  {
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }
}
