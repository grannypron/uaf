/******************************************************************************
* Filename: Choosemn.cpp
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

#include "monster.h"
#include "char.h"
#include "ChooseNPC.h"
#include "GlobalData.h"
#include "CombatMonsterDetails.h"
#include "ChooseCombatMonsters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseCombatMonsters dialog


CChooseCombatMonsters::CChooseCombatMonsters(MONSTER_EVENT_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CChooseCombatMonsters::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseCombatMonsters)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_data = data;
}


void CChooseCombatMonsters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseCombatMonsters)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChooseCombatMonsters, CDialog)
	//{{AFX_MSG_MAP(CChooseCombatMonsters)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseCombatMonsters message handlers

void CChooseCombatMonsters::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CChooseCombatMonsters::OnInitDialog() 
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

	lvc.iSubItem = 0;
	lvc.pszText = "Qty";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_ChosenList.InsertColumn(0,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Name";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_LEFT;
  m_ChosenList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Friendly";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_ChosenList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

  m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  AddNPCToAvailList();

  int a_count = 0;

  //POSITION pos = monsterData.GetHeadPosition();
  //while (pos != NULL)
  {
    int i, n;
    n = monsterData.GetCount();
    for (i=0; i<n; i++)
    {
      //AddToAvailList(monsterData.GetNext(pos));
      AddToAvailList(i), 
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

  int c_count=0;
  for (int i=0;i<m_data.GetCount(); i++)
  {
    AddToChosenList(i);
    c_count++;
  }

  if (c_count > 0)
  {
    m_ChosenList.Sort();
    //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_ChosenList.EnsureVisible(0, FALSE);
    m_ChosenList.SelectItem(0, TRUE);
  }
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseCombatMonsters::OnAdd() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    int flag = m_AvailList.GetItemData(nItem);
    MONSTER_EVENT monsterEvent;

    if (flag >= 0) // add monster
    {
      MONSTER_ID monsterID;
      monsterID = monsterData.PeekMonster(flag)->MonsterID();
      //monsterEvent.monster = flag;
      monsterEvent.monsterID = monsterID;
      monsterEvent.qty = 1;
      monsterEvent.UseQty = MONSTER_EVENT::meUseQty;
      CString name;
      monsterEvent.m_type = MONSTER_TYPE;
      //monsterEvent.monster = flag;

      //MONSTER_DATA *pData = monsterData.GetMonsterData(monsterEvent.monster);
      MONSTER_DATA *pData = monsterData.GetMonster(monsterEvent.monsterID);
      if (pData != NULL)
        name = pData->Name;
      else
        name = "Unknown Monster";

      CCombatMonsterDetails dlg(name, monsterEvent);
      if (dlg.DoModal()==IDOK)
      {
        dlg.GetData(monsterEvent);
        int index = m_data.Add(monsterEvent);
        AddToChosenList(index);
      }
    }
    else if (flag == -1) // add NPC
    {
      CString name;
      monsterEvent.m_type = NPC_TYPE;

      // get NPC from character list
	    //CChooseNPC dlg(-1);
      CHARACTER_ID defaultEmptyCharId;
	    CChooseNPC dlg(defaultEmptyCharId);
      if (dlg.DoModal()==IDOK)
      {
        //if (globalData.charData.HaveKey(dlg.m_CharNum))
        if (globalData.charData.HaveCharacter(dlg.m_characterID))
        {
          //monsterEvent.monster = dlg.m_CharNum;
          monsterEvent.characterID = dlg.m_characterID;
          monsterEvent.monsterID = "";
          CHARACTER cdata;
          //if (globalData.charData.GetCharacterData(monsterEvent.monster, cdata))
          if (globalData.charData.FetchCharacter(monsterEvent.characterID, &cdata))
            name = cdata.GetName();
          else
            name= "Unknown NPC";

          CCombatMonsterDetails mddlg(name, monsterEvent);
          if (mddlg.DoModal()==IDOK)
          {
            mddlg.GetData(monsterEvent);
            int index = m_data.Add(monsterEvent);
            AddToChosenList(index);
          }       
        }
      }
    }
  }	
}

void CChooseCombatMonsters::OnEdit() 
{
  POSITION pos = m_ChosenList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_ChosenList.GetNextSelectedItem(pos);
    DWORD flag = m_ChosenList.GetItemData(nItem);
    CString name("Unknown");
    if (m_data.monsters[flag].m_type == MONSTER_TYPE)
    {
      //MONSTER_DATA *pMonster = monsterData.GetMonster(m_data.monsters[flag].monster);
      MONSTER_DATA *pMonster = monsterData.GetMonster(m_data.monsters[flag].monsterID);
      if (pMonster != NULL)
        name = pMonster->Name;
      else
        name = "Unknown Monster";
    }
    else
    {
      CHARACTER cdata;
      //if (globalData.charData.GetCharacterData(m_data.monsters[flag].monster, cdata))
      if (globalData.charData.FetchCharacter(m_data.monsters[flag].characterID, &cdata))
        name = cdata.GetName();
      else
        name= "Unknown NPC";
    }

    CCombatMonsterDetails dlg(name, m_data.monsters[flag]);
    if (dlg.DoModal()==IDOK)
    {
      dlg.GetData(m_data.monsters[flag]);
      UpdateChosenList(nItem, flag);
    }
  }	
}

void CChooseCombatMonsters::OnDelete() 
{
  POSITION pos = m_ChosenList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_ChosenList.GetNextSelectedItem(pos);
    DWORD flag = m_ChosenList.GetItemData(nItem);
    m_data.Delete(flag);
    
    m_ChosenList.DeleteAllItems();
    int c_count=0;
    for (int i=0;i<m_data.GetCount(); i++)
    {
      AddToChosenList(i);
      c_count++;
    }

    if (c_count > 0)
    {
      m_ChosenList.Sort();
      //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_ChosenList.EnsureVisible(0, FALSE);
      m_ChosenList.SelectItem(0, TRUE);
    }
  }
}

void CChooseCombatMonsters::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();	
	*pResult = 0;
}

void CChooseCombatMonsters::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDelete();	
	*pResult = 0;
}

//void CChooseCombatMonsters::AddToAvailList(MONSTER_DATA &data)
void CChooseCombatMonsters::AddToAvailList(int i)
{
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_MONSTER_NAME+1];
  char ac[10];
  char hd[10];
  char thac0[10];

  MONSTER_DATA *pMonster;
  pMonster = monsterData.GetMonster(i);
  if (pMonster == NULL) return;
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

void CChooseCombatMonsters::AddToChosenList(int i)
{
  int listIdx = m_ChosenList.GetItemCount();
  
  char qty[100];
  char name[MAX_MONSTER_NAME+1];
  char friendly[10];

  if (m_data.monsters[i].UseQty == MONSTER_EVENT::meUseQty)
  {
    sprintf(qty, "%i", m_data.monsters[i].qty);
  }
  else
  {
    if (m_data.monsters[i].qtyBonus == 0)
      sprintf(qty, "%iD%i", 
              m_data.monsters[i].qtyDiceQty,
              m_data.monsters[i].qtyDiceSides);
    else
      sprintf(qty, "%iD%i+%i", 
              m_data.monsters[i].qtyDiceQty,
              m_data.monsters[i].qtyDiceSides,
              m_data.monsters[i].qtyBonus);

  }
  
  if (m_data.monsters[i].m_type == MONSTER_TYPE)
  {
    MONSTER_DATA *pData = monsterData.GetMonster(m_data.monsters[i].monsterID);
    if (pData != NULL)
    {
      strncpy(name, pData->Name, MAX_MONSTER_NAME);
      name[MAX_MONSTER_NAME]='\0';
    }
    else
      strcpy(name, "Unknown Monster");
  }
  else
  {
    CHARACTER cdata;
    if (globalData.charData.FetchCharacter(m_data.monsters[i].characterID, &cdata))
      strcpy(name, cdata.GetName());
    else
      strcpy(name, "Unknown NPC");
  }

  if (m_data.monsters[i].friendly) strcpy(friendly, "Yes");
  else strcpy(friendly, "No");

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = qty;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = i;

  m_ChosenList.InsertItem(&item);  //  qty
  m_ChosenList.SetItemText(listIdx, 1, name); 
  m_ChosenList.SetItemText(listIdx, 2, friendly); 
  //m_ChosenList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(listIdx, TRUE);
}

void CChooseCombatMonsters::UpdateChosenList(int item, int i)
{
  char qty[100];
  char name[MAX_MONSTER_NAME+1];
  char friendly[10];

  if (m_data.monsters[i].UseQty == MONSTER_EVENT::meUseQty)
  {
    sprintf(qty, "%i", m_data.monsters[i].qty);
  }
  else
  {
    if (m_data.monsters[i].qtyBonus == 0)
      sprintf(qty, "%iD%i", 
      m_data.monsters[i].qtyDiceQty,
      m_data.monsters[i].qtyDiceSides);
    else
      sprintf(qty, "%iD%i+%i", 
      m_data.monsters[i].qtyDiceQty,
      m_data.monsters[i].qtyDiceSides,
      m_data.monsters[i].qtyBonus);
    
  }
  
  if (m_data.monsters[i].m_type == MONSTER_TYPE)
  {
    MONSTER_DATA *pData = monsterData.GetMonster(m_data.monsters[i].monsterID);
    if (pData != NULL)
    {
      strncpy(name, pData->Name, MAX_MONSTER_NAME);
      name[MAX_MONSTER_NAME]='\0';
    }
    else
      strcpy(name, "Unknown Monster");
  }
  else
  {
    CHARACTER cdata;
    if (globalData.charData.FetchCharacter(m_data.monsters[i].characterID, &cdata))
      strcpy(name, cdata.GetName());
    else
      strcpy(name, "Unknown NPC");
  }

  if (m_data.monsters[i].friendly) strcpy(friendly, "Yes");
  else strcpy(friendly, "No");

  m_ChosenList.SetItemText(item, 0, qty); 
  m_ChosenList.SetItemText(item, 1, name); 
  m_ChosenList.SetItemText(item, 2, friendly); 
  //m_ChosenList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(item, TRUE);
}

void CChooseCombatMonsters::AddNPCToAvailList()
{
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_MONSTER_NAME+1];
  char ac[10];
  char hd[10];
  char thac0[10];

  strcpy(name, "NPC Combatant");
  strcpy(ac, "NA");
  strcpy(hd, "NA");
  strcpy(thac0, "NA");

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = -1;  // generic NPC

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, ac); 
  m_AvailList.SetItemText(listIdx, 2, hd); 
  m_AvailList.SetItemText(listIdx, 3, thac0); 
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}