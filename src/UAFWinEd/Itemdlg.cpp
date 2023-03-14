/******************************************************************************
* Filename: Itemdlg.cpp
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
#include "ItemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString CStringFromBase38(unsigned int v);

/////////////////////////////////////////////////////////////////////////////
// CItemDlg dialog


CItemDlg::CItemDlg(DWORD MaxSelectable, CWnd* pParent /*=NULL*/)
	: CDialog(CItemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CItemDlg)
	m_AllItemsAreIdentified = FALSE;
	//}}AFX_DATA_INIT
  m_list.Clear();
  m_max = MaxSelectable; // 0 = no limit
}

CItemDlg::CItemDlg(ITEM_LIST &list, DWORD MaxSelectable, CWnd* pParent /*=NULL*/)
	: CDialog(CItemDlg::IDD, pParent)
{
	m_list = list;
  m_max = MaxSelectable; // 0 = no limit
}


void CItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemDlg)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemDlg, CDialog)
	//{{AFX_MSG_MAP(CItemDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_DELETEALL, OnDeleteall)
	ON_BN_CLICKED(IDC_MarkIdentified, OnMarkIdentified)
    ON_BN_CLICKED(IDC_MarkReady, OnMarkReady)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemDlg message handlers


void CItemDlg::GetItemList(ITEM_LIST& list)
{
	list = m_list;
}

void CItemDlg::SetCharacterContext(CHARACTER* p) {
    pChar = p;
}

BOOL CItemDlg::OnInitDialog() 
{
  POSITION pos;
	CDialog::OnInitDialog();

	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);
	m_ChosenList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Id Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 2;
	lvc.pszText = "Location";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(2,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 3;
	lvc.pszText = "Wpn Type";
	lvc.cx = 100;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(3,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(3,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 4;
	lvc.pszText = "Bundle Qty";
	lvc.cx = 50;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(4,&lvc, CListSort::dtINT, true);

  lvc.pszText = "Qty";
  lvc.cx = 30;
  m_ChosenList.InsertColumn(4,&lvc, CListSort::dtINT, true);

  lvc.pszText = "Identified?";
  lvc.cx = 70;
  m_ChosenList.InsertColumn(5,&lvc, CListSort::dtINT, true);

  lvc.pszText = "Ready";
  lvc.cx = 100;
  m_ChosenList.InsertColumn(6, &lvc, CListSort::dtINT, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  int c_count = 0;

  //POSITION pos = itemData.GetHeadPosition();
  //while (pos != NULL)
  {
    int i, n;
    n = itemData.GetCount();
    for (i=0; i<n; i++)
    {
      //AddToAvailList(itemData.GetNext(pos));
      AddToAvailList(*itemData.GetItem(i), i);
      a_count++;
    }
  };

  pos = m_list.GetHeadPosition();
  while (pos != NULL)
  {    
    TRACE("name %s, qty %i\n", 
      //itemData.GetItemUniqueName(m_list.PeekAtPos(pos).m_giID), 
    itemData.GetItemUniqueName(m_list.PeekAtPos(pos).itemID), 
    m_list.PeekAtPos(pos).qty);
    m_list.GetNext(pos);
  }

  pos = m_list.GetHeadPosition();
  while (pos != NULL)
  {
    int key = m_list.GetKeyAt(pos);
    AddToChosenList(key, m_list.GetQty(key), m_list.GetId(key), m_list.IsReady(key));
    m_list.GetNext(pos);
    c_count++;
  }

  UnselectAllAvailableItems();
  UnselectAllChosenItems();

  if (a_count > 0)
  {
    m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }

  if (c_count > 0)
  {
    m_ChosenList.Sort();
    //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_ChosenList.EnsureVisible(0, FALSE);
    m_ChosenList.SelectItem(0, TRUE);
  }

  GetDlgItem(IDC_MarkReady)->EnableWindow(pChar != NULL);


	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemDlg::AddToAvailList(ITEM_DATA &data, int index)
{
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  char idname[MAX_ITEM_NAME+1];
  char location[50];
  char type[50];
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

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemData(listIdx, index); 
  m_AvailList.SetItemText(listIdx, 1, idname); 
  m_AvailList.SetItemText(listIdx, 2, location); 
  m_AvailList.SetItemText(listIdx, 3, type); 
  m_AvailList.SetItemText(listIdx, 4, qty); 

  UnselectAllAvailableItems();
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CItemDlg::AddToChosenList(DWORD index, int itemCount, int identified, bool isReady)
{
  ITEM_DATA *data = itemData.GetItem(m_list.GetItem(index));
  if (data == NULL)
  {
    WriteDebugString("Bogus item index in CItemDlg::AddToChosenList\n");
    return;
  }

  int listIdx = m_ChosenList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  char idname[MAX_ITEM_NAME+1];
  char location[50];
  char type[50];
  char qty[30];

  strcpy(name, data->UniqueName());
  strcpy(idname, data->IdName());
  //strcpy(location, LocationCarriedText[data->Location_Carried]);
#ifdef OldLocation20180129
  strcpy(location, CStringFromBase38(data->Location_Readied.Location()));
#else
  strcpy(location, CStringFromBase38(data->Location_Readied));
#endif
  strcpy(type, WeaponClassText[data->Wpn_Type]);
  sprintf(qty, "%u", itemCount);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = index;

  m_ChosenList.InsertItem(&item);
  m_ChosenList.SetItemText(listIdx, 1, idname); 
  m_ChosenList.SetItemText(listIdx, 2, location); 
  m_ChosenList.SetItemText(listIdx, 3, type); 
  m_ChosenList.SetItemText(listIdx, 4, qty); 
  m_ChosenList.SetItemText(listIdx, 5, identified?"yes":"no");
  m_ChosenList.SetItemText(listIdx, 6, isReady?"yes":"no");

  UnselectAllChosenItems();
  //m_ChosenList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(listIdx, TRUE);
}

void CItemDlg::UnselectAllChosenItems()
{
  int uSelectedCount = m_ChosenList.GetSelectedCount();
  int  nItem = -1;

  // Update all of the selected items.
  if (uSelectedCount > 0)
  {
     for (int i=0;i < uSelectedCount;i++)
     {
        nItem = m_ChosenList.GetNextItem(nItem, LVNI_SELECTED);
        m_ChosenList.SetItemState(nItem, 0, LVIS_SELECTED);
     }
  }
}

void CItemDlg::UnselectAllAvailableItems()
{
  int uSelectedCount = m_AvailList.GetSelectedCount();
  int  nItem = -1;

  // Update all of the selected items.
  if (uSelectedCount > 0)
  {
     for (int i=0;i < uSelectedCount;i++)
     {
        nItem = m_AvailList.GetNextItem(nItem, LVNI_SELECTED);
        m_AvailList.SetItemState(nItem, 0, LVIS_SELECTED);
     }
  }
}

//BOOL CItemDlg::FindItemInList(GLOBAL_ITEM_ID giID)
BOOL CItemDlg::FindItemInList(ITEM_ID itemID)
{
  //return m_list.HaveItem(giID);
  return m_list.HaveItem(itemID);
}

void CItemDlg::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnAdd();
	*pResult = 0;
}

void CItemDlg::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnDelete();
	*pResult = 0;
}

void CItemDlg::OnAdd() 
{
  if ((m_max > 0) && ((DWORD)m_list.GetCount() >= m_max))
    return;

  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     ITEM temp;     
     while (pos != NULL)
     {
        int indx;
        int nItem = m_AvailList.GetNextSelectedItem(pos);
        //GLOBAL_ITEM_ID giID;
        ITEM_ID itemID;
        //giID.UniqueIdentifier(m_AvailList.GetItemData(nItem));
        indx = m_AvailList.GetItemData(nItem);
        //ITEM_DATA *data = itemData.GetItemData(giID);
        ITEM_DATA *data = itemData.GetItem(indx);
        itemID = data->ItemID();

        if (data != NULL)
        {
          //if (!FindItemInList(giID))
          if (!FindItemInList(itemID))
          {
			      //temp.m_giID = giID;
			      temp.itemID = itemID;
			      temp.ReadyLocation(NotReady);
			      temp.qty = max(1, data->Bundle_Qty);
			      temp.charges = data->Num_Charges;
            temp.cursed = data->Cursed;
			      temp.identified = FALSE;

            AddToChosenList(m_list.AddItem(temp), temp.qty, temp.identified, FALSE);
          }
          else
          {
             int qty = max(1, data->Bundle_Qty);
             if (qty == 1) // cannot be joined
             {
			        //temp.m_giID = giID;
			        temp.itemID = itemID;
			        temp.ReadyLocation(NotReady);
			        temp.qty = qty;
			        temp.charges = data->Num_Charges;
              temp.cursed = data->Cursed;
			        temp.identified = FALSE;
              m_list.AddItem(temp);
             }

             m_ChosenList.DeleteAllItems();
             
             pos = m_list.GetHeadPosition();
             while (pos != NULL)
             {
               int key = m_list.GetKeyAt(pos);
               
               // only items that can halve/join can have a
               // qty > 1
               //if ((giID == m_list.PeekAtPos(pos).m_giID) && (qty > 1))
               if ((itemID == m_list.PeekAtPos(pos).itemID) && (qty > 1))
                 m_list.AdjustQty(key, qty);

               AddToChosenList(key, m_list.GetQty(key), m_list.GetId(key), FALSE);
               m_list.GetNext(pos);
             }

             UnselectAllChosenItems();

             if (m_ChosenList.GetItemCount() > 0)
             {
               //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
               //m_ChosenList.EnsureVisible(0, FALSE);
               m_ChosenList.SelectItem(0, TRUE);
             }
          }
        }
     }
  }	
}

void CItemDlg::OnDelete() 
{
  POSITION pos = m_ChosenList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     while (pos != NULL)
     {
        int nItem = m_ChosenList.GetNextSelectedItem(pos);
        DWORD flag = m_ChosenList.GetItemData(nItem);        
        m_list.DeleteItem(flag);
     }

     m_ChosenList.DeleteAllItems();

     pos = m_list.GetHeadPosition();
     while (pos != NULL)
     {
       int key = m_list.GetKeyAt(pos);
       AddToChosenList(key, m_list.GetQty(key), m_list.GetId(key), m_list.IsReady(key));
       m_list.GetNext(pos);
     }

     UnselectAllChosenItems();

     if (m_ChosenList.GetItemCount() > 0)
     {
       //m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
       //m_ChosenList.EnsureVisible(0, FALSE);
       m_ChosenList.SelectItem(0, TRUE);
     }
  }
}

void CItemDlg::OnDeleteall() 
{
  m_list.Clear();
  m_ChosenList.DeleteAllItems();
}


void CItemDlg::OnMarkIdentified() 
{
  int listIdx, i;
  POSITION pos = m_list.GetHeadPosition();
  while (pos != NULL)
    m_list.GetNext(pos).identified = 1;
  listIdx = m_ChosenList.GetItemCount();
  for (i=0; i<listIdx; i++)
  {
    m_ChosenList.SetItemText(i, 5, "yes");
  };
} 

void CItemDlg::OnMarkReady()
{
    if (pChar == NULL) {
        return;
    }
    int listIdx, i;
    POSITION pos = m_list.GetHeadPosition();
    listIdx = m_ChosenList.GetItemCount();
    for (i = 0; i < m_ChosenList.GetItemCount(); i++)
    {
        ITEM* item = m_list.GetITEM(i+1);
        const ITEM_ID& id = item->itemID;
        ITEM_DATA* data = itemData.GetItem(id);
        bool readied = m_list.Ready(id, data->Location_Readied, pChar);
        m_ChosenList.SetItemText(i, 6, readied?"yes":"no");
    };

    MsgBoxInfo("Warning: Changes to the owner of these items may affect their ability to ready some of the items.  Items from being readied against game rules may cause unexpected results or errors.  Consider editing items last when modifying characters/NPCs", "Warning");
}

void CItemDlg::OnOK() 
{
  UpdateData(TRUE);
  
	CDialog::OnOK();
}
