/******************************************************************************
* Filename: ItemEditor.cpp
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
#include "GlobText.h"
#include "ItemEditor.h"
#include "GlobalData.h"
#include "ItemDbDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CStringFromBase38(unsigned int v);

/////////////////////////////////////////////////////////////////////////////
// CItemEditor dialog

//static GLOBAL_ITEM_ID copyLevel_giID;
static ITEM_ID copyLevel_itemID;

CItemEditor::CItemEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CItemEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_pItemData=NULL;
}


void CItemEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemEditor)
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemEditor, CDialog)
	//{{AFX_MSG_MAP(CItemEditor)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
	ON_BN_CLICKED(IDC_EDITITEM, OnEdititem)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_PASTELEVEL, OnPastelevel)
	ON_BN_CLICKED(IDC_COPYLEVEL, OnCopylevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemEditor message handlers

void CItemEditor::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdititem();	
	*pResult = 0;
}

void CItemEditor::OnAdditem() 
{
  bool success;
  CString msg;
  ASSERT(m_pItemData!=NULL);

  SET_MODIFIED;
  
  CItemDBDlg dlg;
  //dlg.m_ItemData.Usable_by_Class = AllClasses;
  {
    BASECLASS_ID baseclassID;
    int i, n;
    n = baseclassData.GetCount();
    for (i=0; i<n; i++)
    {
      baseclassID = baseclassData.PeekBaseclass(i)->BaseclassID();
      dlg.m_ItemData.usableByBaseclass.Add(baseclassID);
    };
  };
  dlg.m_ItemData.SetIdName("New Item");
 
  do 
  {
    success=false;

    if (dlg.DoModal() != IDOK)
      break;

    success=true;

    //if (m_pItemData->FindName(dlg.m_ItemData.IdName()).IsValidItem()) 
    //if (m_pItemData->FindItemID(dlg.m_ItemData.ItemID()) != NULL) 
    if (m_pItemData->LocateItem(dlg.m_ItemData.ItemID()) >= 0) 
    {
      msg.LoadString(IDS_HELP_item_DupName);
      MsgBoxError(msg);
      success=false;
    }
  } while (success==false);

  if (success)
  {
    //GLOBAL_ITEM_ID giID;
    //giID = m_pItemData->AddItem(dlg.m_ItemData);
    //ITEM_ID itemID;
    int itemIndex;
    itemIndex = m_pItemData->AddItem(dlg.m_ItemData);
    //if (giID.IsValidItem())
    if (itemIndex >= 0)
    {
      int i, n;
      AddToAvailList(itemIndex);
      n = m_AvailList.GetItemCount();
      m_AvailList.Sort();
      for (i=0; i<n; i++)
      {
        if ((signed)m_AvailList.GetItemData(i) == itemIndex)
        {
          m_AvailList.SelectItem(i, TRUE);
          break;
        };
      };
    }
    else
    {
      MessageBox("Cannot Add that item","WARNING");
    };

  }
  else
  {
    globalData.eventData.DeleteEvents(dlg.m_ItemData.m_useEvent);
    globalData.eventData.DeleteEvents(dlg.m_ItemData.ExamineEvent);
    //OnDeleteitem(); No! no item was actually added!
  }
}

void CItemEditor::OnDeleteitem() 
{
  ASSERT(m_pItemData!=NULL);
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    SET_MODIFIED;
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //GLOBAL_ITEM_ID giID;
    ITEM_ID itemID;
    int itemIndex;
    //giID.UniqueIdentifier(m_AvailList.GetItemData(nItem));
    itemIndex = m_AvailList.GetItemData(nItem);
    if (itemIndex >= 0)
    {
      //m_pItemData->DeleteItemEvents(giID);
      //m_pItemData->RemoveItem(giID);
      itemID = m_pItemData->PeekItem(itemIndex)->ItemID();
      m_pItemData->DeleteItemEvents(itemID);
      m_pItemData->RemoveItem(itemID);
    };

    m_AvailList.DeleteAllItems();
    int a_count = 0;
    //pos = m_pItemData->GetHeadPosition();
    {
      //while (pos != NULL)
      int i, n;
      n = m_pItemData->GetCount();
      for (i=0; i<n; i++)
      {
        //AddToAvailList(m_pItemData->GetNext(pos));
        AddToAvailList(i);
        a_count++;
      }
    };

    if (a_count > 0)
    {
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);
      m_AvailList.Sort();
      m_AvailList.SelectItem(0, TRUE);
    }	
  }	
}

void CItemEditor::OnEdititem() 
{
  ASSERT(m_pItemData!=NULL);
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {    
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    //GLOBAL_ITEM_ID giID;
    //giID.UniqueIdentifier(m_AvailList.GetItemData(nItem));
    ITEM_ID itemID;
    itemID = m_AvailList.GetItemText(nItem,0);
    CItemDBDlg dlg;

    //ITEM_DATA *pData = m_pItemData->GetItemData(giID);    
    ITEM_DATA *pData = m_pItemData->GetItem(itemID);    
    if (pData == NULL)
    {
      dlg.m_ItemData.Clear();
      dlg.m_ItemData.SetIdName("Invalid Item");
    }
    else
      dlg.m_ItemData = *pData;

    DWORD tempEvent = dlg.m_ItemData.m_useEvent;
    DWORD tempEvent2 = dlg.m_ItemData.ExamineEvent;

    if (dlg.DoModal() == IDOK)
    {
      SET_MODIFIED;
      //m_pItemData->SetItem(giID, dlg.m_ItemData);
      m_pItemData->SetItem(itemID, dlg.m_ItemData);
      UpdateAvailList(nItem, dlg.m_ItemData);
    }
    else
    {
      // cannot undo item event edits
      if (tempEvent != dlg.m_ItemData.m_useEvent)
      {
        // use event chain was changed, force update in db
        tempEvent = dlg.m_ItemData.m_useEvent;
        //pData = m_pItemData->GetItemData(giID);  
        pData = m_pItemData->GetItem(itemID);  
        pData->m_useEvent = tempEvent;
        //m_pItemData->SetItem(giID, *pData);
        m_pItemData->SetItem(itemID, *pData);
      }
      if (tempEvent2 != dlg.m_ItemData.ExamineEvent)
      {
        // examine event chain was changed, force update in db
        tempEvent2 = dlg.m_ItemData.ExamineEvent;
        //pData = m_pItemData->GetItemData(giID);  
        pData = m_pItemData->GetItem(itemID);  
        pData->ExamineEvent = tempEvent2;
        //m_pItemData->SetItem(giID, *pData);
        m_pItemData->SetItem(itemID, *pData);
      }
    }
  }		
}

BOOL CItemEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();

  ASSERT(m_pItemData!=NULL);
  
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Id Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Location";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 3;
	lvc.pszText = "Wpn Type";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 4;
	lvc.pszText = "Bundle Qty";
	lvc.cx = 75;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(4,&lvc, CListSort::dtINT, true);

  m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  //POSITION pos = m_pItemData->GetHeadPosition();
  //while (pos != NULL)
  {
    int i, n;
    n = m_pItemData->GetCount();
    for (i=0; i<n; i++)
    {
      //AddToAvailList(m_pItemData->GetNext(pos));
      AddToAvailList(i);
      a_count++;
    }
  };

  if (a_count > 0)
  {
    m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }	
  //copyLevel_giID.Clear();
  copyLevel_itemID.Clear();
  CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
  if (pWnd!=NULL) pWnd->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemEditor::AddToAvailList(int itemIndex)
{
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  char idname[MAX_ITEM_NAME+1];
  char location[30];
  char type[30];
  char qty[30];

  const ITEM_DATA *pItem;

  pItem = m_pItemData->PeekItem(itemIndex);

  strncpy(name, pItem->UniqueName(),MAX_ITEM_NAME);
  name[MAX_ITEM_NAME]='\0';
  strncpy(idname, pItem->IdName(),MAX_ITEM_NAME);
  idname[MAX_ITEM_NAME]='\0';
  //strcpy(location, LocationCarriedText[data.Location_Carried]);
#ifdef OldLocation20180129
  strcpy(location, CStringFromBase38(pItem->Location_Readied.Location()));
#else
  strcpy(location, CStringFromBase38(pItem->Location_Readied));
#endif
  strcpy(type, WeaponClassText[pItem->Wpn_Type]);
  sprintf(qty, "%u", pItem->Bundle_Qty);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	//item.lParam = data.m_giID.UniqueIdentifier();
	item.lParam = itemIndex;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, idname); 
  m_AvailList.SetItemText(listIdx, 2, location); 
  m_AvailList.SetItemText(listIdx, 3, type); 
  m_AvailList.SetItemText(listIdx, 4, qty); 

  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CItemEditor::UpdateAvailList(int item, ITEM_DATA &data)
{
  char name[MAX_ITEM_NAME+1];
  char idname[MAX_ITEM_NAME+1];
  char location[30];
  char type[30];
  char qty[30];

  strcpy(name, data.UniqueName());
  strcpy(idname, data.IdName());
  //strcpy(location, LocationCarriedText[data.Location_Carried]);
#ifdef OldLocation20180129
  strcpy(location, CStringFromBase38(data.Location_Readied.Location()));
#else
  strcpy(location, CStringFromBase38(data.Location_Readied));
#endif
  strcpy(type, WeaponClassText[data.Wpn_Type]);
  sprintf(qty, "%u", data.Bundle_Qty);

  m_AvailList.SetItemText(item, 0, name); 
  m_AvailList.SetItemText(item, 1, idname); 
  m_AvailList.SetItemText(item, 2, location); 
  m_AvailList.SetItemText(item, 3, type); 
  m_AvailList.SetItemText(item, 4, qty); 

  //m_AvailList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(item, TRUE);
}

void CItemEditor::OnOk() 
{
  CDialog::OnOK();
}

void CItemEditor::OnPastelevel() 
{
  //if (!copyLevel_giID.IsValidItem()) return;
  if (!copyLevel_itemID.IsValidItem()) return;
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_AvailList.GetNextSelectedItem(pos);
  //GLOBAL_ITEM_ID pasteLevel_giID;
  ITEM_ID pasteLevel_itemID;
  //pasteLevel_giID.UniqueIdentifier(m_AvailList.GetItemData(nItem));
  pasteLevel_itemID = m_AvailList.GetItemText(nItem,0);
  //if (!m_pItemData->HaveItem(pasteLevel_giID)) return;
  if (!m_pItemData->HaveItem(pasteLevel_itemID)) return;

  // copy src item to dest item
  //ITEM_DATA *srcItem = m_pItemData->GetItemData(copyLevel_giID);
  ITEM_DATA *srcItem = m_pItemData->GetItem(copyLevel_itemID);
  //ITEM_DATA *dstItem = m_pItemData->GetItemData(pasteLevel_giID);
  ITEM_DATA *dstItem = m_pItemData->GetItem(pasteLevel_itemID);
  if (srcItem==NULL) return;
  if (dstItem==NULL) return;
/*
  CWaitCursor cursor; // this name search may take awhile...
  DWORD index=1;
  BOOL validname=false;
  CString name;
  while (!validname)
  {
    name.Format("%s/%u", srcItem->UniqueName(), index);

    if (name == dstItem->IdName())
      validname=true;
    else
      validname = !m_pItemData->FindName(name).IsValidItem();
    if (index >= MAX_ITEMS)
      break;
    index++;
  }

  if (validname)
  {
    ITEM_DATA newData = *srcItem;
    
    globalData.eventData.CopyEvent(newData.m_useEvent, srcItem->m_useEvent, &globalData.eventData);
    globalData.eventData.CopyEvent(newData.ExamineEvent, srcItem->ExamineEvent, &globalData.eventData);

    newData.SetIdName(name);
    newData.m_giID = pasteLevel_giID;
    m_pItemData->SetItem(pasteLevel_giID, newData);
    UpdateAvailList(nItem, newData);
  }
  */
  {
    ITEM_DATA newData = *srcItem;
    globalData.eventData.CopyEvent(newData.m_useEvent, srcItem->m_useEvent, &globalData.eventData);
    globalData.eventData.CopyEvent(newData.ExamineEvent, srcItem->ExamineEvent, &globalData.eventData);

    newData.SetUniqueName(dstItem->UniqueName());
    //newData.m_giID = pasteLevel_giID;
    //m_pItemData->SetItem(pasteLevel_giID, newData);
    m_pItemData->SetItem(pasteLevel_itemID, newData);
    UpdateAvailList(nItem, newData);
  };
}

void CItemEditor::OnCopylevel() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();
  if (pos == NULL) return;
  int nItem = m_AvailList.GetNextSelectedItem(pos);
  //copyLevel_giID.UniqueIdentifier(m_AvailList.GetItemData(nItem));
  copyLevel_itemID = m_AvailList.GetItemText(nItem,0);

  CWnd *pWnd = GetDlgItem(IDC_PASTELEVEL);
  //if (m_pItemData->HaveItem(copyLevel_giID))
  if (m_pItemData->HaveItem(copyLevel_itemID))
  {
    if (pWnd!=NULL) pWnd->EnableWindow(TRUE);
  }
  else
  {
    if (pWnd!=NULL) pWnd->EnableWindow(FALSE);
  }
}
