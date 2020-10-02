/******************************************************************************
* Filename: UtilitiesEventData.cpp
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
#include "GlobalData.h"
#include "ItemKeyQuestDlg.h"
#include "UtilitiesEventData.h"
#include "Help.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD GetItemFlag(int index) { return ((ITEM_FLAG << 28) | index); }
static DWORD GetKeyFlag(int index) { return ((KEY_FLAG << 28) | index); }
static DWORD GetQuestFlag(int index) { return ((QUEST_FLAG << 28) | index); }
static void SplitFlag(DWORD flag, WORD &type, int &index)
{
  type = static_cast<WORD>((flag >> 28) & 0x0000000F);
  index = static_cast<int>(flag & 0x0FFFFFFF);
}

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesEventData dialog


CUtilitiesEventData::CUtilitiesEventData(UTILITIES_EVENT_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CUtilitiesEventData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUtilitiesEventData)
	m_EndPlay = FALSE;
	m_Qty = 0;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CUtilitiesEventData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUtilitiesEventData)
	DDX_Control(pDX, IDC_VAROPERATION, m_VarOperation);
	DDX_Control(pDX, IDC_RESULTSELECT, m_ResultSelect);
	DDX_Control(pDX, IDC_OBJECTSELECT, m_ObjectSelect);
	DDX_Control(pDX, IDC_ITEMLIST, m_ItemList);
	DDX_Control(pDX, IDC_ITEMCHECK, m_ItemCheck);
	DDX_Check(pDX, IDC_ENDPLAY, m_EndPlay);
	DDX_Text(pDX, IDC_QTY, m_Qty);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUtilitiesEventData, CDialog)
	//{{AFX_MSG_MAP(CUtilitiesEventData)
	ON_BN_CLICKED(IDC_RESULTSELECT, OnResultselect)
	ON_BN_CLICKED(IDC_OBJECTSELECT, OnObjectselect)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
  ON_BN_CLICKED(IDC_UTILITIES_EVENT, OnUtilitiesEventQuestion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
  BOOL endPlay;
  MathOperationType operation;
  MultiItemCheckType itemCheck;  
  BYTE mathItemType;
  BYTE resultItemType;
  WORD mathAmount;
  int  mathItemIndex;  
  int  resultItemIndex;
  SPECIAL_OBJECT_EVENT_LIST items;
*/

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesEventData message handlers

BOOL CUtilitiesEventData::OnInitDialog() 
{
	CDialog::OnInitDialog();
  
  m_VarOperation.ResetContent();
  m_ItemCheck.ResetContent();

  int i;
  for (i=0;i<MAX_MATH_OPERATION_TYPES;i++)
    m_VarOperation.AddString(MathOperationText[i]);
  for (i=0;i<MAX_MULTI_ITEM_CHECK_TYPES;i++)
    m_ItemCheck.AddString(MultiItemCheckText[i]);

  m_VarOperation.SetCurSel(m_data.operation);
  m_ItemCheck.SetCurSel(m_data.itemCheck);

  m_EndPlay = m_data.endPlay;
  m_Qty = m_data.mathAmount;

  char name[MAX_MEDITBUTTON_TEXT+1];

  switch (m_data.resultItemType)
  {
    case ITEM_FLAG:
 	    getBaseName(GetSpecialItemName(m_data.resultItemIndex),name, MAX_MEDITBUTTON_TEXT);      
      break;
    case KEY_FLAG:
      getBaseName(GetSpecialKeyName(m_data.resultItemIndex),name, MAX_MEDITBUTTON_TEXT);
      break;
    case QUEST_FLAG:
      getBaseName(globalData.questData.GetName(m_data.resultItemIndex),name, MAX_MEDITBUTTON_TEXT);
      break;
    default:
      strcpy(name, "No Item");
      break;
  }

  m_ResultSelect.SetWindowText(name);

  switch (m_data.mathItemType)
  {
    case ITEM_FLAG:
 	    getBaseName(GetSpecialItemName(m_data.mathItemIndex),name, MAX_MEDITBUTTON_TEXT);      
      break;
    case KEY_FLAG:
      getBaseName(GetSpecialKeyName(m_data.mathItemIndex),name, MAX_MEDITBUTTON_TEXT);
      break;
    case QUEST_FLAG:
      getBaseName(globalData.questData.GetName(m_data.mathItemIndex),name, MAX_MEDITBUTTON_TEXT);
      break;
    default:
      strcpy(name, "No Item");
      break;
  }

  m_ObjectSelect.SetWindowText(name);

	// insert columns
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Type";
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;
	m_ItemList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Name";
	lvc.cx = 390;
	lvc.fmt = LVCFMT_LEFT;
  m_ItemList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	m_ItemList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  for (i=0;i<m_data.items.list.GetSize(); i++)
  {
    switch (m_data.items.list[i].ItemType)
    {
    case ITEM_FLAG:
 	    AddSpecialItemToList(m_data.items.list[i].index, i);
      break;
    case KEY_FLAG:
      AddSpecialKeyToList(m_data.items.list[i].index, i);
      break;
    case QUEST_FLAG:
      AddQuestToList(m_data.items.list[i].index, i);
      break;
    }
  }

  if (m_data.items.list.GetSize() > 0)
  {
    //m_ItemList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_ItemList.EnsureVisible(0, FALSE);
    m_ItemList.SelectItem(0, TRUE);
  }

  UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUtilitiesEventData::OnOK() 
{
  UpdateData(TRUE);
	
  m_data.operation = (MathOperationType)m_VarOperation.GetCurSel();
  m_data.itemCheck = (MultiItemCheckType)m_ItemCheck.GetCurSel();

  m_data.endPlay = m_EndPlay;
  m_data.mathAmount = m_Qty;

	CDialog::OnOK();
}

void CUtilitiesEventData::OnResultselect() 
{
	CItemKeyQuestDlg dlg(ITEM_FLAG | KEY_FLAG | QUEST_FLAG);

  if (dlg.DoModal() == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];

    switch (dlg.m_type)
    {
    case ITEM_FLAG:
 	    getBaseName(GetSpecialItemName(dlg.m_selectedID),name, MAX_MEDITBUTTON_TEXT);      
      break;
    case KEY_FLAG:
      getBaseName(GetSpecialKeyName(dlg.m_selectedID),name, MAX_MEDITBUTTON_TEXT);
      break;
    case QUEST_FLAG:
      getBaseName(globalData.questData.GetName(dlg.m_selectedID),name, MAX_MEDITBUTTON_TEXT);
      break;
    default:
      strcpy(name, "No Item");
      break;
    }
    m_ResultSelect.SetWindowText(name);

    m_data.resultItemType = dlg.m_type;
    m_data.resultItemIndex = dlg.m_selectedID;
  }	
}

void CUtilitiesEventData::OnObjectselect() 
{
	CItemKeyQuestDlg dlg(ITEM_FLAG | KEY_FLAG | QUEST_FLAG);

  if (dlg.DoModal() == IDOK)
  {
    char name[MAX_MEDITBUTTON_TEXT+1];

    switch (dlg.m_type)
    {
    case ITEM_FLAG:
 	    getBaseName(GetSpecialItemName(dlg.m_selectedID),name, MAX_MEDITBUTTON_TEXT);      
      break;
    case KEY_FLAG:
      getBaseName(GetSpecialKeyName(dlg.m_selectedID),name, MAX_MEDITBUTTON_TEXT);
      break;
    case QUEST_FLAG:
      getBaseName(globalData.questData.GetName(dlg.m_selectedID),name, MAX_MEDITBUTTON_TEXT);
      break;
    default:
      strcpy(name, "No Item");
      break;
    }
    m_ObjectSelect.SetWindowText(name);
    m_data.mathItemType = dlg.m_type;
    m_data.mathItemIndex = dlg.m_selectedID;
  }	
}

void CUtilitiesEventData::OnAdditem() 
{
	CItemKeyQuestDlg dlg(ITEM_FLAG | KEY_FLAG | QUEST_FLAG);

  if (dlg.DoModal() == IDOK)
  {
    int eventIdx;
    SPECIAL_OBJECT_EVENT_DATA sitem;
    sitem.operation = SPECIAL_OBJECT_CHECK;

    switch (dlg.m_type)
    {
    case ITEM_FLAG:
      if (!IsObjectInList(dlg.m_selectedID, ITEM_FLAG))
      { 	      
        sitem.ItemType = ITEM_FLAG;
        sitem.index = dlg.m_selectedID;
        eventIdx = m_data.items.Add(sitem);
        AddSpecialItemToList(dlg.m_selectedID, eventIdx);
      }
      break;
    case KEY_FLAG:
      if (!IsObjectInList(dlg.m_selectedID, KEY_FLAG))
      {
        sitem.ItemType = KEY_FLAG;
        sitem.index = dlg.m_selectedID;
        eventIdx = m_data.items.Add(sitem);
        AddSpecialKeyToList(dlg.m_selectedID, eventIdx);
      }
      break;
    case QUEST_FLAG:
      if (!IsObjectInList(dlg.m_selectedID, QUEST_FLAG))
      {
        sitem.ItemType = QUEST_FLAG;
        sitem.index = dlg.m_selectedID;
        eventIdx = m_data.items.Add(sitem);
        AddQuestToList(dlg.m_selectedID, eventIdx);
      }
      break;
    }
  }
}

void CUtilitiesEventData::OnDeleteitem() 
{
  POSITION pos = m_ItemList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     while (pos != NULL)
     {
        int nItem = m_ItemList.GetNextSelectedItem(pos);
        DWORD flag = m_ItemList.GetItemData(nItem);

        WORD type;
        int idx;
        SplitFlag(flag, type, idx);
        m_data.items.Delete(idx);
     }

    m_ItemList.DeleteAllItems();

    for (int i=0;i<m_data.items.list.GetSize(); i++)
    {
      switch (m_data.items.list[i].ItemType)
      {
      case ITEM_FLAG:
 	      AddSpecialItemToList(m_data.items.list[i].index, i);
        break;
      case KEY_FLAG:
        AddSpecialKeyToList(m_data.items.list[i].index, i);
        break;
      case QUEST_FLAG:
        AddQuestToList(m_data.items.list[i].index, i);
        break;
      }
    }

    if (m_data.items.list.GetSize() > 0)
    {
      //m_ItemList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_ItemList.EnsureVisible(0, FALSE);
      m_ItemList.SelectItem(0, TRUE);
    }
  }		
}

void CUtilitiesEventData::AddSpecialItemToList(int idx, int eventIdx)
{
  int listIdx = m_ItemList.GetItemCount();

  char temp[10];
  LVITEM item;
  strcpy(temp, "Item");

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = temp;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = GetItemFlag(eventIdx); 

  m_ItemList.InsertItem(&item);
  m_ItemList.SetItemText(listIdx, 1, globalData.specialItemData.GetName(idx)); 
  //m_ItemList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ItemList.SelectItem(listIdx, TRUE);
}

void CUtilitiesEventData::AddSpecialKeyToList(int idx, int eventIdx)
{
  int listIdx = m_ItemList.GetItemCount();

  char temp[10];
  LVITEM item;

  strcpy(temp, "Key");

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = temp;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = GetKeyFlag(eventIdx); 

  m_ItemList.InsertItem(&item);
  m_ItemList.SetItemText(listIdx, 1, globalData.keyData.GetName(idx));
  //m_ItemList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ItemList.SelectItem(listIdx, TRUE);
}

void CUtilitiesEventData::AddQuestToList(int idx, int eventIdx)
{
  int listIdx = m_ItemList.GetItemCount();

  char temp[10];
  LVITEM item;

  strcpy(temp, "Quest");

	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = temp;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = GetQuestFlag(eventIdx); 

  m_ItemList.InsertItem(&item);
  m_ItemList.SetItemText(listIdx, 1, globalData.questData.GetName(idx));
  //m_ItemList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ItemList.SelectItem(listIdx, TRUE);
}

BOOL CUtilitiesEventData::IsObjectInList(int index, BYTE type) 
{
  BOOL found = FALSE;
  int i=0;
  while ((i < m_data.items.list.GetSize()) && (!found))
  {
    if (   (m_data.items.list[i].index == index)
        && (m_data.items.list[i].ItemType == type))
      found = TRUE;
    i++;
  }
  return found;
}

void CUtilitiesEventData::OnUtilitiesEventQuestion(void) 
{
  CHelp dlg("UTILITIES EVENT");
  dlg.DoModal();
}
