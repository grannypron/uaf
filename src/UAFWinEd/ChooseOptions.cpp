/******************************************************************************
* Filename: ChooseOptions.cpp
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

#include "FileParse.h"
#include "ChooseOptions.h"
#include "AttibuteDataEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseOptions dialog


//
CChooseOptions::CChooseOptions(       SUBSTRINGS *pSrc,     // contains a delimited list
                                                         // List delimited by comma and slash
                                const CString& suggestionKey, 
                                      CArray<CStringPAIR, CStringPAIR&> *pSuggestionList,
                                const CString& helpKey,
                                      CWnd* pParent /*=NULL*/)
	: CDialog(CChooseOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChooseOptions)
		// NOTE: the ClassWizard will add member initialization here
  m_chosen.RemoveAll();
	//}}AFX_DATA_INIT
  SUBSTRINGS2CArray(pSrc, &m_chosen);
  m_suggestionKey = suggestionKey;
  m_pSuggestionList = pSuggestionList;
  m_helpKey = helpKey;
}


void CChooseOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseOptions)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseOptions, CDialog)
	//{{AFX_MSG_MAP(CChooseOptions)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
  ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
  ON_BN_CLICKED(IDC_Help, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseOptions message handlers

void CChooseOptions::OnOK() 
{
	// We need to create a SUBSTRINGS result
  m_result.Empty();
  int i, n;
  n = m_chosen.GetSize();
  for (i=0; i<n; i++)
  {
    CString newField;
    newField.Format("/,%s,%s", m_chosen[i].Key(), m_chosen[i].Value());
    m_result += newField;
  };
	CDialog::OnOK();
}

BOOL CChooseOptions::OnInitDialog() 
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
	lvc.pszText = "Value";
	lvc.cx = 450;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 150;
	lvc.fmt = LVCFMT_LEFT;
  m_ChosenList.InsertColumn(0,&lvc, CListSort::dtINT, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Value";
	lvc.cx = 350;
	lvc.fmt = LVCFMT_LEFT;
  m_ChosenList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);


  m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);


  int a_count = 0;

  //POSITION pos = monsterData.GetHeadPosition();
  //while (pos != NULL)
  {
    //int i, n;
    //n = monsterData.GetCount();
    //for (i=0; i<n; i++)
    //{
    //  //AddToAvailList(monsterData.GetNext(pos));
    //  AddToAvailList(i), 
    //  a_count++;
    //};
  };

  if (m_pSuggestionList != NULL)
  {
    int i, n;
    n = m_pSuggestionList->GetSize();
    for (i=0; i<n; i++)
    {
      CStringPAIR *pEntry;
      pEntry = &((*m_pSuggestionList)[i]);
      m_all.Add(*pEntry);
      AddToAvailList(a_count); 
      a_count++;
    };
  }
  else if (!m_suggestionKey.IsEmpty())
  {
    int i, n;
    CString key, value;
    n = ConfigFile.GetCount();
    for (i=0; i<n; i++)
    {
      if (ConfigFile[i].Token == m_suggestionKey)
      {
        int col;
        key = ConfigFile[i].Value;
        col = key.Find(',');
        if (col >= 0)
        {
          value = key.Right(key.GetLength()-col-1);
          key = key.Left(col);
        }
        else
        {
          value.Empty();
        };
        {
          CStringPAIR entry(key, value);
          m_all.Add(entry);
          AddToAvailList(a_count); 
          a_count++;
        };
      };
    };
  };
  {
    int i, n;
    n = m_chosen.GetSize();
    for (i=0; i<n; i++)
    {
      int j, m;
      m = m_all.GetSize();
      for (j=0; j<m; j++)
      {
        if (m_chosen[i] == m_all[j]) break;
      };
      if (j == m)
      {
        m_all.Add(m_chosen[i]);
      }
      else
      {
        m_all[i].m_value = m_chosen[i].m_value;
      };
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
  for (int i=0;i<m_chosen.GetSize(); i++)
  {
    AddToChosenList(AvailIndex(m_chosen[i].m_key, m_chosen[i].m_value));
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

void CChooseOptions::RebuildAvailList(void)
{
  DWORD i, n;
  m_AvailList.DeleteAllItems();
  n = m_all.GetSize();
  for (i=0; i<n; i++)
  {
    const CStringPAIR *pEntry;
    pEntry = &m_all[i];
    // See if it is in the ChosenList
    {
      int j, m;
      m = m_ChosenList.GetItemCount();
      for (j=0; j<m; j++)
      {
        if (m_ChosenList.GetItemData(j) == i) break;
      };
      if (j != m) continue;
    };
    {
      int listIdx;
      listIdx = m_AvailList.GetItemCount();
      LVITEM item;
	    item.mask = LVIF_TEXT | LVIF_PARAM;
	    item.iItem = listIdx;
	    item.iSubItem = 0;
	    item.state = 0; 
	    item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	    item.pszText = m_all[i].m_key.GetBuffer(1);
	    item.cchTextMax = 0;
	    item.iImage = 0;
	    item.lParam = i;

      m_AvailList.InsertItem(&item);
      m_AvailList.SetItemText(listIdx, 1, m_all[i].m_value.GetBuffer(1)); 
    }
  };
}
void CChooseOptions::OnAdd() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    DWORD index = m_AvailList.GetItemData(nItem);
    //MONSTER_EVENT monsterEvent;
    //MONSTER_ID monsterID;
    //monsterID = monsterData.PeekMonster(flag)->MonsterID();
    //monsterEvent.monster = flag;
    //monsterEvent.monsterID = monsterID;
    //monsterEvent.qty = 1;
    //monsterEvent.UseQty = MONSTER_EVENT::meUseQty;
    CString name;

    //if (flag > 0) // add ASL
    //{
      //monsterEvent.m_type = MONSTER_TYPE;
      //monsterEvent.monster = flag;

      //MONSTER_DATA *pData = monsterData.GetMonsterData(monsterEvent.monster);
      //MONSTER_DATA *pData = monsterData.GetMonster(monsterEvent.monsterID);
      //if (pData != NULL)
      //  name = pData->Name;
      //else
      //  name = "Unknown Monster";

      //CCombatMonsterDetails dlg(name, monsterEvent);
      //if (dlg.DoModal()==IDOK)
      //{
      //  dlg.GetData(monsterEvent);
      //  int index = m_data.Add(monsterEvent);
        AddToChosenList(index);
        m_chosen.Add(m_all[index]);
     // }
    //}
    //else if (flag == 0) // add NPC
    //{
    //  monsterEvent.m_type = NPC_TYPE;
 
    //  // get NPC from character list
	  //  //CChooseNPC dlg(-1);
    //  CHARACTER_ID defaultEmptyCharId;
	  //  CChooseNPC dlg(defaultEmptyCharId);
    //  if (dlg.DoModal()==IDOK)
    //  {
    //    //if (globalData.charData.HaveKey(dlg.m_CharNum))
    //    if (globalData.charData.HaveCharacter(dlg.m_characterID))
    //    {
    //      //monsterEvent.monster = dlg.m_CharNum;
    //      monsterEvent.characterID = dlg.m_characterID;
    //      monsterEvent.monsterID = "";
    //      CHARACTER cdata;
    //      //if (globalData.charData.GetCharacterData(monsterEvent.monster, cdata))
    //      if (globalData.charData.FetchCharacter(monsterEvent.characterID, &cdata))
    //        name = cdata.GetName();
    //      else
    //        name= "Unknown NPC";

    //      CCombatMonsterDetails mddlg(name, monsterEvent);
    //      if (mddlg.DoModal()==IDOK)
    //      {
    //        mddlg.GetData(monsterEvent);
    //        int index = m_data.Add(monsterEvent);
    //        AddToChosenList(index);
    //      }       
    //    }
    //  }
    //}
  }	
}

void CChooseOptions::OnEdit() 
{
  POSITION pos = m_ChosenList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int allIndex, chosenIndex;
    int nItem = m_ChosenList.GetNextSelectedItem(pos);
    //DWORD index = m_ChosenList.GetItemData(nItem);
    //CString name("Unknown");
    //if (m_data.monsters[flag].m_type == MONSTER_TYPE)
    //{
    //  //MONSTER_DATA *pMonster = monsterData.GetMonster(m_data.monsters[flag].monster);
    //  MONSTER_DATA *pMonster = monsterData.GetMonster(m_data.monsters[flag].monsterID);
    //  if (pMonster != NULL)
    //    name = pMonster->Name;
    //  else
    //    name = "Unknown Monster";
    //}
    //else
    //{
    //  CHARACTER cdata;
    //  //if (globalData.charData.GetCharacterData(m_data.monsters[flag].monster, cdata))
    //  if (globalData.charData.FetchCharacter(m_data.monsters[flag].characterID, &cdata))
    //    name = cdata.GetName();
    //  else
    //    name= "Unknown NPC";
    //}

    //CCombatMonsterDetails dlg(name, m_data.monsters[flag]);
    //if (dlg.DoModal()==IDOK)
    //{
    //  dlg.GetData(m_data.monsters[flag]);
    //  UpdateChosenList(nItem, flag);
    //}
    allIndex = m_ChosenList.GetItemData(nItem);
    chosenIndex = ChosenIndex(m_all[allIndex].m_key, m_all[allIndex].m_value);
    CAttibuteDataEdit dlg(m_all[allIndex].m_key,m_all[allIndex].m_value);
    if (dlg.DoModal() != IDOK) return;
    if (dlg.m_Name.IsEmpty()) return;
    allIndex = AvailIndex(dlg.m_Name, dlg.m_Value);
    if (allIndex < 0)
    {
      CStringPAIR entry;
      entry.m_key = dlg.m_Name;
      entry.m_value = dlg.m_Value;
      allIndex = m_all.Add(entry);
      AddToAvailList(allIndex);
    };
    {
      m_chosen[chosenIndex].m_key = dlg.m_Name;
      m_chosen[chosenIndex].m_value = dlg.m_Value;
      m_ChosenList.SetItemData(nItem, allIndex);
      UpdateChosenList(allIndex);
    };
  }	
}

int CChooseOptions::AvailIndex(const CString& name, const CString& value)
{
  int i, n;
  n = m_all.GetSize();
  for (i=0; i<n; i++)
  {
    if ((m_all[i].Key() == name) && (m_all[i].Value() == value)) return i;
  }
  return -1;
}

int CChooseOptions::ChosenIndex(const CString& name, const CString& value)
{
  int i, n;
  n = m_chosen.GetSize();
  for (i=0; i<n; i++)
  {
    if ((m_chosen[i].Key() == name) && (m_chosen[i].Value() == value)) return i;
  }
  return -1;
}

void CChooseOptions::OnNew() 
{
  int i, j;
  CAttibuteDataEdit dlg("","");
  if (dlg.DoModal() != IDOK) return;
  if (dlg.m_Name.IsEmpty()) return;
  CStringPAIR entry;
  entry.m_key = dlg.m_Name;
  entry.m_value = dlg.m_Value;
  i = AvailIndex(entry.m_key, entry.m_value);
  if (i < 0)
  {
    i = m_all.Add(entry);
  }
  j = ChosenIndex(entry.m_key, entry.m_value);
  if (j < 0)
  {
    AddToChosenList(i);
    m_chosen.Add(entry);
  }
  else
  {
    UpdateChosenList(i);
  };
}


void CChooseOptions::OnDelete() 
{
  POSITION pos = m_ChosenList.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_ChosenList.GetNextSelectedItem(pos);
    DWORD index = m_ChosenList.GetItemData(nItem);
    //m_data.Delete(index);
    
    //m_ChosenList.DeleteAllItems();
    //int c_count=0;
    //for (int i=0;i<m_data.GetCount(); i++)
    //{
    //  AddToChosenList(i);
    //  c_count++;
    //}
    m_ChosenList.DeleteItem(nItem);
    DeleteChosen(index);
    if (m_ChosenList.GetItemCount() > 0)
    {
      m_ChosenList.Sort();
      //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_ChosenList.EnsureVisible(0, FALSE);
      m_ChosenList.SelectItem(0, TRUE);
    }
  }
  RebuildAvailList();
}

void CChooseOptions::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();	
	*pResult = 0;
}

void CChooseOptions::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDelete();	
	*pResult = 0;
}

void CChooseOptions::AddToAvailList(int i)
{
  int listIdx = m_AvailList.GetItemCount();

  //char name[MAX_MONSTER_NAME+1];
  //char ac[10];
  //char hd[10];
  //char thac0[10];

  //MONSTER_DATA *pMonster;
  //pMonster = monsterData.GetMonster(i);
  //if (pMonster == NULL) return;
  //strncpy(name, pMonster->Name, MAX_MONSTER_NAME);
  //name[MAX_MONSTER_NAME]='\0';
  //sprintf(ac, "%i", pMonster->Armor_Class);
  //if (pMonster->Hit_Dice_Bonus != 0)
  //  sprintf(hd, "%02.1f+%i", pMonster->Hit_Dice, pMonster->Hit_Dice_Bonus);
  //else
  //  sprintf(hd, "%02.1f", pMonster->Hit_Dice);
  //sprintf(thac0, "%i", pMonster->THAC0);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = m_all[i].m_key.GetBuffer(1);
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = i;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, m_all[i].m_value.GetBuffer(1)); 
  //m_AvailList.SetItemText(listIdx, 2, hd); 
  //m_AvailList.SetItemText(listIdx, 3, thac0); 
  //m_AvailList.SelectItem(listIdx, TRUE);
}

void CChooseOptions::AddToChosenList(int i)
{
  int listIdx = m_ChosenList.GetItemCount();
  
  //char qty[100];
  //char name[MAX_MONSTER_NAME+1];
  //char friendly[10];

  //if (m_data.monsters[i].UseQty == MONSTER_EVENT::meUseQty)
  //{
  //  sprintf(qty, "%i", m_data.monsters[i].qty);
  //}
  //else
  //{
  //  if (m_data.monsters[i].qtyBonus == 0)
  //    sprintf(qty, "%iD%i", 
  //            m_data.monsters[i].qtyDiceQty,
  //            m_data.monsters[i].qtyDiceSides);
  //  else
  //    sprintf(qty, "%iD%i+%i", 
  //            m_data.monsters[i].qtyDiceQty,
  //            m_data.monsters[i].qtyDiceSides,
  //            m_data.monsters[i].qtyBonus);

  //}
  
  //if (m_data.monsters[i].m_type == MONSTER_TYPE)
  //{
  //  MONSTER_DATA *pData = monsterData.GetMonster(m_data.monsters[i].monsterID);
  //  if (pData != NULL)
  //  {
  //    strncpy(name, pData->Name, MAX_MONSTER_NAME);
  //    name[MAX_MONSTER_NAME]='\0';
  //  }
  //  else
  //    strcpy(name, "Unknown Monster");
  //}
  //else
  //{
  //  CHARACTER cdata;
  //  if (globalData.charData.FetchCharacter(m_data.monsters[i].characterID, &cdata))
  //    strcpy(name, cdata.GetName());
  //  else
  //    strcpy(name, "Unknown NPC");
  //}

  //if (m_data.monsters[i].friendly) strcpy(friendly, "Yes");
  //else strcpy(friendly, "No");

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = m_all[i].m_key.GetBuffer(1);
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = i;

  m_ChosenList.InsertItem(&item);  //  qty
  m_ChosenList.SetItemText(listIdx, 1, m_all[i].m_value.GetBuffer(1)); 
  //m_ChosenList.SetItemText(listIdx, 2, friendly); 
  //m_ChosenList.SelectItem(listIdx, TRUE);
  RebuildAvailList();
}

void CChooseOptions::DeleteChosen(int index)
{
  int i, n;
  n = m_chosen.GetSize();
  for (i=0; i<n; i++)
  {
    if (m_chosen[i].m_key == m_all[index].m_key)
    {
      m_chosen.RemoveAt(i);
      return;
    };
  };
}

void CChooseOptions::UpdateChosenList(DWORD i)
{
  //char qty[100];
  //char name[MAX_MONSTER_NAME+1];
  //char friendly[10];

  //if (m_data.monsters[i].UseQty == MONSTER_EVENT::meUseQty)
  //{
  //  sprintf(qty, "%i", m_data.monsters[i].qty);
  //}
  //else
  //{
  //  if (m_data.monsters[i].qtyBonus == 0)
  //    sprintf(qty, "%iD%i", 
  //    m_data.monsters[i].qtyDiceQty,
  //    m_data.monsters[i].qtyDiceSides);
  //  else
  //    sprintf(qty, "%iD%i+%i", 
  //    m_data.monsters[i].qtyDiceQty,
  //    m_data.monsters[i].qtyDiceSides,
  //    m_data.monsters[i].qtyBonus);
  //  
  //}
  
  //if (m_data.monsters[i].m_type == MONSTER_TYPE)
  //{
  //  MONSTER_DATA *pData = monsterData.GetMonster(m_data.monsters[i].monsterID);
  //  if (pData != NULL)
  //  {
  //    strncpy(name, pData->Name, MAX_MONSTER_NAME);
  //    name[MAX_MONSTER_NAME]='\0';
  //  }
  //  else
  //    strcpy(name, "Unknown Monster");
  //}
  //else
  //{
  //  CHARACTER cdata;
  //  if (globalData.charData.FetchCharacter(m_data.monsters[i].characterID, &cdata))
  //    strcpy(name, cdata.GetName());
  //  else
  //    strcpy(name, "Unknown NPC");
  //}

  //if (m_data.monsters[i].friendly) strcpy(friendly, "Yes");
  //else strcpy(friendly, "No");

  int item, count;
  count = m_ChosenList.GetItemCount();
  for (item=0; item < count; item++)
  {
    if (m_ChosenList.GetItemData(item) == i) break;
  };
  if (item == count) return;
  //m_ChosenList.SetItemText(item, 0, qty); 
  m_ChosenList.SetItemText(item, 1, m_all[i].m_value.GetBuffer(1)); 
  //m_ChosenList.SetItemText(item, 2, friendly); 
  //m_ChosenList.SelectItem(item, TRUE);
}

void CChooseOptions::SUBSTRINGS2CArray(const SUBSTRINGS *pSrc, CArray<CStringPAIR, CStringPAIR&> *pDst)
{
  int col;
  SUBSTRINGS field;
  pDst->RemoveAll();
  col = -1;
  CString head, tail;
  while (pSrc->NextField(&col, &field))
  {
    field.HeadAndTail(&head, &tail);
    tail = tail.Right(tail.GetLength()-1);
    CStringPAIR pair(head, tail);
    pDst->Add(pair);
  };
}

void CChooseOptions::OnHelp(void)
{
  CString msg;
  if (!m_helpKey.IsEmpty())
  {
    int i, n;
    n = ConfigFile.GetCount();
    for (i=0; i<n; i++)
    {
      if (ConfigFile[i].Token == m_helpKey)
      {
        msg += '\n';
        msg += ConfigFile[i].Value;
      };
    };
  };
  if (msg.IsEmpty())
  {
    msg = "Sorry, no help is available";
  };
  MsgBoxInfo(msg, "Here is the Help you requested");
}
