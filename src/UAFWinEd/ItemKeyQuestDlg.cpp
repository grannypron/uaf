/******************************************************************************
* Filename: ItemKeyQuestDlg.cpp
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
#include "items.h"
#include "char.h"
#include "GameEvent.h"
#include "GlobalData.h"
#include "AddSpecialObject.h"
#include "ItemKeyQuestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD GetItemFlag(int index) { return ((ITEM_FLAG << 28) | index); }
static DWORD GetKeyFlag(int index) { return ((KEY_FLAG << 28) | index); }
static DWORD GetQuestFlag(int index) { return ((QUEST_FLAG << 28) | index); }
static void SplitFlag(DWORD flag, QUEST_TYPE &type, int &index)
{
  type = (QUEST_TYPE)((flag >> 28) & 0x0000000F);
  index = static_cast<int>(flag & 0x0FFFFFFF);
}

/////////////////////////////////////////////////////////////////////////////
// CItemKeyQuestDlg dialog


CItemKeyQuestDlg::CItemKeyQuestDlg(BYTE IncludeMask, 
                                   GLOBAL_STATS *data, /*=NULL*/
                                   CWnd* pParent /*=NULL*/ )
	: CDialog(CItemKeyQuestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemKeyQuestDlg)
	//}}AFX_DATA_INIT

  m_pdata=data;  // If NULL then No changes allowed.  Read-only
  if (data==NULL)
  {
    m_keyData=globalData.keyData;
    m_specialItemData=globalData.specialItemData;
    m_questData=globalData.questData;
  }
  else
  {
    m_keyData=data->keyData;
    m_specialItemData=data->specialItemData;
    m_questData=data->questData;
  };
  m_Mask = IncludeMask;
  m_selectedID = -1;
  m_type = NO_FLAG;
}

void CItemKeyQuestDlg::GetData(GLOBAL_STATS *data)
{
  data->keyData=m_keyData;
  data->specialItemData=m_specialItemData;
  data->questData=m_questData;
}

void CItemKeyQuestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemKeyQuestDlg)
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_ADD, m_Add);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemKeyQuestDlg, CDialog)
	//{{AFX_MSG_MAP(CItemKeyQuestDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemKeyQuestDlg message handlers

void CItemKeyQuestDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnEdit();
	*pResult = 0;
}

void CItemKeyQuestDlg::OnAdd() 
{
  CAddSpecialObject dlg("", 0, "EXAMINE", TRUE, ITEM_FLAG, m_Mask);
  
  if (dlg.DoModal()==IDOK)
  {
    SET_MODIFIED;
    int id;
    switch (dlg.m_Mask)
    {
    case ITEM_FLAG:
      {
        SPECIAL_OBJECT_DATA item;
        item.name = dlg.m_Name;
        item.ExamineEvent = dlg.m_Event;
        item.ExamineLabel = dlg.m_ExamineLabel;
        item.CanBeDropped = dlg.m_CanBeDropped;
        id = m_specialItemData.AddItem(item);
        if (id > 0) AddSpecialItemToList(id);
      }
      break;
    case KEY_FLAG:
      {
        SPECIAL_OBJECT_DATA key;
        key.name = dlg.m_Name;
        key.ExamineEvent = dlg.m_Event;
        key.ExamineLabel = dlg.m_ExamineLabel;
        key.CanBeDropped = dlg.m_CanBeDropped;
        id = m_keyData.AddItem(key);
        if (id > 0) AddSpecialKeyToList(id);
      }
      break;
    case QUEST_FLAG:
      {
        QUEST_DATA quest;
        quest.name = dlg.m_Name;
        id = m_questData.AddQuest(quest);
        if (id > 0) AddQuestToList(id);
        globalData.eventData.DeleteEvents(dlg.m_Event);
      }
      break;
    }
  }
  else
  {
    globalData.eventData.DeleteEvents(dlg.m_Event);
  }
}

void CItemKeyQuestDlg::OnEdit() 
{
  POSITION pos = m_List.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_List.GetNextSelectedItem(pos);
    DWORD flag = m_List.GetItemData(nItem);

    QUEST_TYPE type;
    int idx;
    SplitFlag(flag, type, idx);
    CString name="";
    CString exlabel="EXAMINE";
    DWORD exevent=0;
    BOOL drop=FALSE;

    switch (type)
    {
    case ITEM_FLAG:
      name = m_specialItemData.GetName(idx);
      exlabel = m_specialItemData.GetExamineLabel(idx);
      exevent = m_specialItemData.GetExamineEvent(idx);
      drop = m_specialItemData.CanBeDropped(idx);
      break;
    case KEY_FLAG:
      name = m_keyData.GetName(idx);
      exlabel = m_keyData.GetExamineLabel(idx);
      exevent = m_keyData.GetExamineEvent(idx);
      drop = m_keyData.CanBeDropped(idx);
      break;
    case QUEST_FLAG:
      name = m_questData.GetName(idx);
      break;
    }

    CAddSpecialObject dlg(name, exevent, exlabel, drop, type, type);

    if (dlg.DoModal() == IDOK)
    {
      SET_MODIFIED;
      switch (dlg.m_Mask)
      {
      case ITEM_FLAG:
        m_specialItemData.SetName(idx, dlg.m_Name);
        m_specialItemData.SetExamineLabel(idx, dlg.m_ExamineLabel);
        m_specialItemData.SetExamineEvent(idx, dlg.m_Event);
        m_specialItemData.SetCanBeDropped(idx, dlg.m_CanBeDropped);
        break;
      case KEY_FLAG:
        m_keyData.SetName(idx, dlg.m_Name);
        m_keyData.SetExamineLabel(idx, dlg.m_ExamineLabel);
        m_keyData.SetExamineEvent(idx, dlg.m_Event);
        m_keyData.SetCanBeDropped(idx, dlg.m_CanBeDropped);
        break;
      case QUEST_FLAG:
        m_questData.SetName(idx, dlg.m_Name);
        break;
      }

      m_List.SetItemText( nItem, 1, dlg.m_Name/*dlg.m_Name.LockBuffer()*/ );
      //dlg.m_Name.UnlockBuffer();
    }
    else
    {
      // cannot undo event edits
      if (exevent != dlg.m_Event)
      {
        // event chain was changed, force update in db
        exevent = dlg.m_Event;
        SET_MODIFIED;
        switch (dlg.m_Mask)
        {
        case ITEM_FLAG:
          m_specialItemData.SetExamineEvent(idx, exevent);
          break;
        case KEY_FLAG:
          m_keyData.SetExamineEvent(idx, exevent);
          break;
        case QUEST_FLAG:
          break;
        }       
      }
    }
  }
}

void CItemKeyQuestDlg::OnDelete() 
{
  POSITION pos = m_List.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     SET_MODIFIED;
     while (pos != NULL)
     {
        int nItem = m_List.GetNextSelectedItem(pos);
        DWORD flag = m_List.GetItemData(nItem);

        QUEST_TYPE type;
        int idx;
        SplitFlag(flag, type, idx);

        switch (type)
        {
        case ITEM_FLAG:
          m_specialItemData.DeleteSpecialObjectEvents(idx);
          m_specialItemData.DeleteItem(idx);          
          break;
        case KEY_FLAG:
          m_keyData.DeleteSpecialObjectEvents(idx);
          m_keyData.DeleteItem(idx);          
          break;
        case QUEST_FLAG:
          m_questData.DeleteQuest(idx);
          break;
        }
     }

     m_List.DeleteAllItems();

    int count=0;
    if ((m_Mask & ITEM_FLAG) == ITEM_FLAG)
    {
      pos = m_specialItemData.GetHeadPosition();
      while (pos != NULL)
      {
        AddSpecialItemToList(m_specialItemData.GetNext(pos).id);
        count++;
      }
    }

    if ((m_Mask & KEY_FLAG) == KEY_FLAG)
    {
      pos = m_keyData.GetHeadPosition();
      while (pos != NULL)
      {
        AddSpecialKeyToList(m_keyData.GetNext(pos).id);
        count++;
      }
    }
  
    if ((m_Mask & QUEST_FLAG) == QUEST_FLAG)
    {
      pos = m_questData.GetHeadPosition();
      while (pos != NULL)
      {
        AddQuestToList(m_questData.GetNext(pos).id);
        count++;
      }
    }

    if (count > 0)
    {
      //m_List.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_List.EnsureVisible(0, FALSE);
      m_List.SelectItem(0, TRUE);
    }
  }	
}

BOOL CItemKeyQuestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  // Hide unused buttons.
  if (m_pdata==NULL)
  {
    m_Add.ShowWindow(false);
    m_Delete.ShowWindow(false);
    m_Edit.ShowWindow(false);
  };

	// insert columns
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Type";
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;
	m_List.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Name";
	lvc.cx = 390;
	lvc.fmt = LVCFMT_LEFT;
  m_List.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	m_List.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  POSITION pos;
  int count=0;
  if ((m_Mask & ITEM_FLAG) == ITEM_FLAG)
  {
    pos = m_specialItemData.GetHeadPosition();
    while (pos != NULL)
    {
      AddSpecialItemToList(m_specialItemData.GetNext(pos).id);
      count++;
    }
  }

  if ((m_Mask & KEY_FLAG) == KEY_FLAG)
  {
    pos = m_keyData.GetHeadPosition();
    while (pos != NULL)
    {
      AddSpecialKeyToList(m_keyData.GetNext(pos).id);
      count++;
    }
  }

  if ((m_Mask & QUEST_FLAG) == QUEST_FLAG)
  {
    pos = m_questData.GetHeadPosition();
    while (pos != NULL)
    {
      AddQuestToList(m_questData.GetNext(pos).id);
      count++;
    }
  }

  if (count > 0)
  {
    m_List.Sort();
    //m_List.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_List.EnsureVisible(0, FALSE);
    m_List.SelectItem(0, TRUE);
  }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemKeyQuestDlg::AddSpecialItemToList(int idx)
{
  char temp[10];
  LVITEM item;
  strcpy(temp, "Item");

  int listIdx = m_List.GetItemCount();

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = temp;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = GetItemFlag(idx); 

  m_List.InsertItem(&item);
  m_List.SetItemText(listIdx, 1, m_specialItemData.GetName(idx));
  //m_List.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_List.SelectItem(listIdx, TRUE);
}

void CItemKeyQuestDlg::AddSpecialKeyToList(int idx)
{
  char temp[10];
  LVITEM item;

  strcpy(temp, "Key");

  int listIdx = m_List.GetItemCount();

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = temp;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = GetKeyFlag(idx); 

  m_List.InsertItem(&item);
  m_List.SetItemText(listIdx, 1, m_keyData.GetName(idx));
  //m_List.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_List.SelectItem(listIdx, TRUE);
}

void CItemKeyQuestDlg::AddQuestToList(int idx)
{
  char temp[10];
  LVITEM item;

  strcpy(temp, "Quest");

  int listIdx = m_List.GetItemCount();

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = temp;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = GetQuestFlag(idx); 

  m_List.InsertItem(&item);
  m_List.SetItemText(listIdx, 1, m_questData.GetName(idx));
  //m_List.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_List.SelectItem(listIdx, TRUE);
}

void CItemKeyQuestDlg::OnOK() 
{
  POSITION pos = m_List.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_List.GetNextSelectedItem(pos);
    SplitFlag(m_List.GetItemData(nItem), m_type, m_selectedID);
  }	
  else
  {
    m_selectedID = -1;
    m_type = NO_FLAG;
  };
  
	CDialog::OnOK();
}

