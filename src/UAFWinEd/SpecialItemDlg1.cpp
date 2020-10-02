/******************************************************************************
* Filename: SpecialItemDlg1.cpp
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
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "ItemKeyQuestDlg.h"

#include "SpecialItemDlg1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD GetItemFlag(int index) { return ((ITEM_FLAG << 28) | index); }
static DWORD GetKeyFlag(int index) { return ((KEY_FLAG << 28) | index); }
//static DWORD GetQuestFlag(int index) { return ((QUEST_FLAG << 28) | index); }
static void SplitFlag(DWORD flag, WORD &type, int &index)
{
  type = static_cast<WORD>((flag >> 28) & 0x0000000F);
  index = static_cast<int>(flag & 0x0FFFFFFF);
}

/////////////////////////////////////////////////////////////////////////////
// CSpecialItemDlg dialog


CSpecialItemDlg::CSpecialItemDlg(SPECIAL_ITEM_KEY_EVENT_DATA& data, CWnd* pParent /*=NULL*/)
	: CDialog(CSpecialItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecialItemDlg)
	m_Text = _T("");
	m_ForceBackup = FALSE;
	//}}AFX_DATA_INIT

  m_data = data;
}

/*
const BYTE SPECIAL_OBJECT_NOTHING = 0x00;
const BYTE SPECIAL_OBJECT_TAKE    = 0x01;
const BYTE SPECIAL_OBJECT_GIVE    = 0x02;
*/

void CSpecialItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecialItemDlg)
	DDX_Control(pDX, IDC_GIVELIST, m_GiveList);
	DDX_Control(pDX, IDC_TAKELIST, m_TakeList);
	DDX_Control(pDX, IDC_CHOOSEART, m_ChooseArt);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Check(pDX, IDC_FORCEBACKUP, m_ForceBackup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpecialItemDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecialItemDlg)
	ON_BN_CLICKED(IDC_CHOOSEART, OnChooseart)
	ON_BN_CLICKED(IDC_REMOVEGIVE, OnRemoveFromGiveList)
	ON_BN_CLICKED(IDC_REMOVETAKE, OnRemoveFromTakeList)
	ON_BN_CLICKED(IDC_GIVEKEY, OnAddToGiveList)
	ON_BN_CLICKED(IDC_TAKEKEY, OnAddToTakeList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecialItemDlg message handlers

void CSpecialItemDlg::OnOK() 
{  	
  UpdateData(TRUE);
  m_data.text = m_Text;
  m_data.forceExit=m_ForceBackup;
	CDialog::OnOK();
}

BOOL CSpecialItemDlg::OnInitDialog() 
{
   CDialog::OnInitDialog();
      
   m_Text = m_data.text;
   m_ForceBackup=m_data.forceExit;
      
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseArt.SetWindowText(name);

	// insert columns
	LV_COLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Type";
	lvc.cx = 70;
	lvc.fmt = LVCFMT_LEFT;
	m_GiveList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);
	m_TakeList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Name";
	lvc.cx = 390;
	lvc.fmt = LVCFMT_LEFT;
  m_GiveList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);
  m_TakeList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	m_GiveList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_TakeList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int gcount = 0;
  int tcount = 0;

  for (int i=0;i<m_data.items.list.GetSize(); i++)
  {
    switch (m_data.items.list[i].ItemType)
    {
    case ITEM_FLAG:
      if (m_data.items.list[i].operation == SPECIAL_OBJECT_GIVE)
      {
 	      AddItemToGiveList(m_data.items.list[i].index, i);
        gcount++;
      }
      else
      {
        AddItemToTakeList(m_data.items.list[i].index, i);
        tcount++;
      }
      break;
    case KEY_FLAG:
      if (m_data.items.list[i].operation == SPECIAL_OBJECT_GIVE)
      {
        AddKeyToGiveList(m_data.items.list[i].index, i);
        gcount++;
      }
      else
      {
        AddKeyToTakeList(m_data.items.list[i].index, i);
        tcount++;
      }
      break;
    }
  }

  if (gcount > 0)
  {
    m_GiveList.Sort();
    //m_GiveList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_GiveList.EnsureVisible(0, FALSE);
    m_GiveList.SelectItem(0, TRUE);
  }

  if (tcount > 0)
  {
    m_TakeList.Sort();
    //m_TakeList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_TakeList.EnsureVisible(0, FALSE);
    m_TakeList.SelectItem(0, TRUE);
  }

  UpdateData(FALSE);
   
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecialItemDlg::OnChooseart() 
{
   CSelectPicDlg dlg(m_data.pic, SmallPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_data.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
   }
}

void CSpecialItemDlg::OnRemoveFromGiveList() 
{
  POSITION pos = m_GiveList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     while (pos != NULL)
     {
        int nItem = m_GiveList.GetNextSelectedItem(pos);
        DWORD flag = m_GiveList.GetItemData(nItem);

        WORD type;
        int idx;
        SplitFlag(flag, type, idx);
        m_data.items.Delete(idx);
     }

     m_GiveList.DeleteAllItems();

    int gcount = 0;
    for (int i=0;i<m_data.items.list.GetSize(); i++)
    {
      switch (m_data.items.list[i].ItemType)
      {
      case ITEM_FLAG:
        if (m_data.items.list[i].operation == SPECIAL_OBJECT_GIVE)
        {
 	        AddItemToGiveList(m_data.items.list[i].index, i);
          gcount++;
        }
        break;
      case KEY_FLAG:
        if (m_data.items.list[i].operation == SPECIAL_OBJECT_GIVE)
        {
          AddKeyToGiveList(m_data.items.list[i].index, i);
          gcount++;
        }
        break;
      }
    }

    if (gcount > 0)
    {
      //m_GiveList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_GiveList.EnsureVisible(0, FALSE);
      m_GiveList.SelectItem(0, TRUE);
    }
  }		
}

void CSpecialItemDlg::OnRemoveFromTakeList() 
{
  POSITION pos = m_TakeList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     while (pos != NULL)
     {
        int nItem = m_TakeList.GetNextSelectedItem(pos);
        DWORD flag = m_TakeList.GetItemData(nItem);

        WORD type;
        int idx;
        SplitFlag(flag, type, idx);
        m_data.items.Delete(idx);
     }

     m_TakeList.DeleteAllItems();

    int tcount = 0;
    for (int i=0;i<m_data.items.list.GetSize(); i++)
    {
      switch (m_data.items.list[i].ItemType)
      {
      case ITEM_FLAG:
        if (m_data.items.list[i].operation != SPECIAL_OBJECT_GIVE)
        {
          AddItemToTakeList(m_data.items.list[i].index, i);
          tcount++;
        }
        break;
      case KEY_FLAG:
        if (m_data.items.list[i].operation != SPECIAL_OBJECT_GIVE)
        {
          AddKeyToTakeList(m_data.items.list[i].index, i);
          tcount++;
        }
        break;
      }
    }

    if (tcount > 0)
    {
      //m_TakeList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_TakeList.EnsureVisible(0, FALSE);
      m_TakeList.SelectItem(0, TRUE);
    }
  }		
}


void CSpecialItemDlg::GetData(SPECIAL_ITEM_KEY_EVENT_DATA &data)
{
  data = m_data;
}

void CSpecialItemDlg::OnAddToGiveList() 
{
  CItemKeyQuestDlg dlg(ITEM_FLAG | KEY_FLAG, &globalData);
	
  if (dlg.DoModal() == IDOK)
  {
    int eventIdx;
    SPECIAL_OBJECT_EVENT_DATA sitem;
    dlg.GetData(&globalData);
    sitem.operation = SPECIAL_OBJECT_GIVE;

    if (dlg.m_selectedID >= 0)
    {
      if (dlg.m_type == KEY_FLAG)
      {
        if (!IsObjectInList(dlg.m_selectedID, KEY_FLAG))
        {
          sitem.index = dlg.m_selectedID;
          sitem.ItemType = KEY_FLAG;
          eventIdx = m_data.items.Add(sitem);
          AddKeyToGiveList(dlg.m_selectedID, eventIdx);
        }
      }
      else
      {
        if (!IsObjectInList(dlg.m_selectedID, ITEM_FLAG))
        {
          sitem.index = dlg.m_selectedID;
          sitem.ItemType = ITEM_FLAG;
          eventIdx = m_data.items.Add(sitem);
          AddItemToGiveList(dlg.m_selectedID, eventIdx);
        }
      }
    }
  }	
}

void CSpecialItemDlg::OnAddToTakeList() 
{
  CItemKeyQuestDlg dlg(ITEM_FLAG | KEY_FLAG);
	
  if (dlg.DoModal() == IDOK)
  {
    int eventIdx;
    SPECIAL_OBJECT_EVENT_DATA sitem;
    sitem.operation = SPECIAL_OBJECT_TAKE;

    if (dlg.m_selectedID >= 0)
    {
      if (dlg.m_type == KEY_FLAG)
      {
        if (!IsObjectInList(dlg.m_selectedID, KEY_FLAG))
        {
          sitem.index = dlg.m_selectedID;
          sitem.ItemType = KEY_FLAG;
          eventIdx = m_data.items.Add(sitem);         
          AddKeyToTakeList(dlg.m_selectedID, eventIdx);
        }
      }
      else
      {
        if (!IsObjectInList(dlg.m_selectedID, ITEM_FLAG))
        {
          sitem.index = dlg.m_selectedID;
          sitem.ItemType = ITEM_FLAG;
          eventIdx = m_data.items.Add(sitem);         
          AddItemToTakeList(dlg.m_selectedID, eventIdx);
        }
      }
    }
  }	
}


BOOL CSpecialItemDlg::IsObjectInList(int index, BYTE type) 
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

void CSpecialItemDlg::AddItemToGiveList(int idx, int eventIdx)
{
  int listIdx = m_GiveList.GetItemCount();

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

  m_GiveList.InsertItem(&item);
  m_GiveList.SetItemText(listIdx, 1, globalData.specialItemData.GetName(idx)); 
  //m_GiveList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_GiveList.SelectItem(listIdx, TRUE);
}

void CSpecialItemDlg::AddItemToTakeList(int idx, int eventIdx)
{
  int listIdx = m_TakeList.GetItemCount();

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

  m_TakeList.InsertItem(&item);
  m_TakeList.SetItemText(listIdx, 1, globalData.specialItemData.GetName(idx)); 
  //m_TakeList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_TakeList.SelectItem(listIdx, TRUE);
}

void CSpecialItemDlg::AddKeyToGiveList(int idx, int eventIdx)
{
  int listIdx = m_GiveList.GetItemCount();

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

  m_GiveList.InsertItem(&item);
  m_GiveList.SetItemText(listIdx, 1, globalData.keyData.GetName(idx));
  //m_GiveList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_GiveList.SelectItem(listIdx, TRUE);
}

void CSpecialItemDlg::AddKeyToTakeList(int idx, int eventIdx)
{
  int listIdx = m_TakeList.GetItemCount();

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

  m_TakeList.InsertItem(&item);
  m_TakeList.SetItemText(listIdx, 1, globalData.keyData.GetName(idx));
  //m_TakeList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_TakeList.SelectItem(listIdx, TRUE);
}