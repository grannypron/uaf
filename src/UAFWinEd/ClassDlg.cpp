/******************************************************************************
* Filename: ClassDlg.cpp
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
#include "ClassDlg.h"
#include "..\Shared\GameRules.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CClassDlg dialog


//CClassDlg::CClassDlg(int selectedClassesMask, CWnd* pParent /*=NULL*/)
CClassDlg::CClassDlg(CArray<BASECLASS_ID, BASECLASS_ID&> *pBaseclasses, CWnd* pParent /*=NULL*/)
	: CDialog(CClassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClassDlg)
	//m_AllItemsAreIdentified = FALSE;
	//}}AFX_DATA_INIT
  


  // Translate the 'Can be used by class mask' to classNumbers in classData
  // On our way back ( OnOK ) we will do the reverse tranlation.
  // The hope is that someday we will allow arbitrary classes and then
  // it will be easy to modify this dialog to accomodate a list of classes.


/*
  m_selectedClassesMask = 0;
  if (selectedClassesMask & MagicUserFlag) m_selectedClassesMask |= (1<<MagicUser);
  if (selectedClassesMask & ClericFlag) m_selectedClassesMask |= (1<<Cleric);
  if (selectedClassesMask & ThiefFlag) m_selectedClassesMask |= (1<<Thief);
  if (selectedClassesMask & FighterFlag) m_selectedClassesMask |= (1<<Fighter);
  if (selectedClassesMask & PaladinFlag) m_selectedClassesMask |= (1<<Paladin);
  if (selectedClassesMask & RangerFlag) m_selectedClassesMask |= (1<<Ranger);
  if (selectedClassesMask & DruidFlag) m_selectedClassesMask |= (1<<Druid);
*/
  m_selectedBaseclasses = pBaseclasses;


}


void CClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClassDlg)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClassDlg, CDialog)
	//{{AFX_MSG_MAP(CClassDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	//ON_BN_CLICKED(IDC_ADD, OnAdd)
	//ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//ON_BN_CLICKED(IDC_DELETEALL, OnDeleteall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassDlg message handlers


//void CClassDlg::GetItemList(ITEM_LIST& list)
//{
//	list = m_list;
//}


BOOL CClassDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Class Name";
	lvc.cx = 700;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);
	m_ChosenList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

  /*
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
*/
	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  int c_count = 0;

  //POSITION pos = classData.GetHeadPosition();


   // First add all the currently selected baseclasses.
  m_allBaseclasses = m_selectedBaseclasses;
  //while ((pos != NULL) && (a_count+c_count < 7))
  {
    BASECLASS_ID baseclassID;
    // Now add all the known baseclasses.
    int i, n;
    n = baseclassData.GetCount();
    for (i=0; i<n; i++)
    {
      int j;
      baseclassID = baseclassData.PeekBaseclass(i)->BaseclassID();
      if (baseclassID == "$$Help") continue;
      //int key;
      //CLASS_DATA *pClass;
      //pClass = &classData.GetNext(pos);
      //key = pClass->m_classKey;
      //if (m_selectedClassesMask & (1 << key))
      j = m_allBaseclasses.LocateBaseclassID(baseclassID);
      if (j >= 0)
      {
        AddToChosenList(j);
        c_count++;
      }
      else
      {
        m_allBaseclasses.Add(baseclassID);
        AddToAvailList(m_allBaseclasses.GetCount()-1);
        a_count++;
      };
    };
    n = m_allBaseclasses.GetCount();
    for (i=0; i<n; i++)
    {
      baseclassID = *m_allBaseclasses.PeekBaseclassID(i);
      if (baseclassData.LocateBaseclass(baseclassID) < 0)
      {
        AddToChosenList(i);
        c_count++;
      };
    };
  };


//  pos = m_list.GetHeadPosition();
//  while (pos != NULL)
//  {    
//    TRACE("name %s, qty %i\n", 
//      itemData.GetItemName(m_list.GetAt(pos).m_giID), 
//      m_list.GetAt(pos).qty);
//    m_list.GetNext(pos);
//  }

  //pos = m_list.GetHeadPosition();
  //while (pos != NULL)
  //{
  //  int key = m_list.GetKeyAt(pos);
  //  AddToChosenList(key, m_list.GetQty(key));
  //  m_list.GetNext(pos);
  //  c_count++;
  //}

  UnselectAllAvailableClasses();
  UnselectAllChosenClasses();

  if (a_count > 0)
  {
    m_AvailList.Sort();
    m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }

  if (c_count > 0)
  {
    m_ChosenList.Sort();
    m_ChosenList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    m_ChosenList.EnsureVisible(0, FALSE);
    m_ChosenList.SelectItem(0, TRUE);
  }

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//void CClassDlg::AddToAvailList(CLASS_DATA *pClassData)
void CClassDlg::AddToAvailList(int indx)
{
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  //char idname[MAX_ITEM_NAME+1];
  //char location[50];
  //char type[50];
  //char qty[30];

  //strcpy(name, pClassData->m_name);
  strcpy(name, *m_allBaseclasses.PeekBaseclassID(indx));
  //strcpy(idname, data.Id_Name);
  //strcpy(location, LocationCarriedText[data.Location_Carried]);
  //strcpy(type, WeaponClassText[data.Wpn_Type]);
  //sprintf(qty, "%u", data.Bundle_Qty);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	//item.lParam = pClassData->m_classKey;
	item.lParam = indx;

  m_AvailList.InsertItem(&item);
  //m_AvailList.SetItemText(listIdx, 1, idname); 
  //m_AvailList.SetItemText(listIdx, 2, location); 
  //m_AvailList.SetItemText(listIdx, 3, type); 
  //m_AvailList.SetItemText(listIdx, 4, qty); 

  //UnselectAllAvailableItems();
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

//void CClassDlg::AddToChosenList(CLASS_DATA *pClassData)
void CClassDlg::AddToChosenList(int indx)
{

  int listIdx = m_ChosenList.GetItemCount();

  char name[MAX_ITEM_NAME+1];
  //ar idname[MAX_ITEM_NAME+1];
  //ar location[50];
  //ar type[50];
  //ar qty[30];

  //strcpy(name, pClassData->m_name);
  strcpy(name, *m_allBaseclasses.PeekBaseclassID(indx));
  //rcpy(idname, data->Id_Name);
  //strcpy(location, LocationCarriedText[data->Location_Carried]);
  //strcpy(type, WeaponClassText[data->Wpn_Type]);
  //sprintf(qty, "%u", itemCount);

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	//item.lParam = pClassData->m_classKey;
	item.lParam = indx;

  m_ChosenList.InsertItem(&item);
  //m_ChosenList.SetItemText(listIdx, 1, idname); 
  //m_ChosenList.SetItemText(listIdx, 2, location); 
  //m_ChosenList.SetItemText(listIdx, 3, type); 
  //m_ChosenList.SetItemText(listIdx, 4, qty); 

  //UnselectAllChosenItems();
  //m_ChosenList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(listIdx, TRUE);
}


void CClassDlg::UnselectAllChosenClasses()
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

void CClassDlg::UnselectAllAvailableClasses()
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

//BOOL CClassDlg::FindItemInList(GLOBAL_ITEM_ID giID)
//{
//  return m_list.HaveItem(giID);
//}

void CClassDlg::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnAdd();
	*pResult = 0;
}

void CClassDlg::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnDelete();
	*pResult = 0;
}

void CClassDlg::OnAdd() 
{

  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    ITEM temp;     
    while (pos != NULL)
    {
      int nItem;
      //int key;
      //CLASS_DATA *pClassData;
      int indx;
      nItem = m_AvailList.GetNextSelectedItem(pos);
      //key = m_AvailList.GetItemData(nItem);
      indx = m_AvailList.GetItemData(nItem);
      //pClassData = classData.GetClassData(key);
      //if (pClassData != NULL)
      {
        //AddToChosenList(pClassData);
        AddToChosenList(indx);
        //m_AvailList.DeleteItem(nItem);
        m_AvailList.DeleteItem(nItem);
        //m_selectedClassesMask |= (1 << key);
        m_selectedBaseclasses.Add(*m_allBaseclasses.PeekBaseclassID(indx));
      };
    };
  };	
}

void CClassDlg::OnDelete() 
{
  POSITION pos = m_ChosenList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
     while (pos != NULL)
     {
        int nItem;
        //int key;
        //CLASS_DATA *pClassData;
        int indx;
        nItem = m_ChosenList.GetNextSelectedItem(pos);
        //key = m_ChosenList.GetItemData(nItem); 
        indx = m_ChosenList.GetItemData(nItem); 
        //pClassData = classData.GetClassData(key);
        //if (pClassData != NULL)
        {
          m_ChosenList.DeleteItem(nItem);
          //AddToAvailList(pClassData);
          AddToAvailList(indx);
          //m_selectedClassesMask &= ~(1 << key);
          m_selectedBaseclasses.Remove(*m_allBaseclasses.PeekBaseclassID(indx));
        };
     };
  };
}


void CClassDlg::OnOK() 
{
  //int temp;
  UpdateData(TRUE);

  
/*
  // Translate classNumbers in classData to the 'Can be used by class mask'
  // On our way in ( Constructor ) we did the reverse tranlation.
  temp = 0;
  if (m_selectedClassesMask & (1<<MagicUser))  temp |= MagicUserFlag;
  if (m_selectedClassesMask & (1<<Cleric))     temp |= ClericFlag;
  if (m_selectedClassesMask & (1<<Thief))      temp |= ThiefFlag;
  if (m_selectedClassesMask & (1<<Fighter))    temp |= FighterFlag;
  if (m_selectedClassesMask & (1<<Paladin))    temp |= PaladinFlag;
  if (m_selectedClassesMask & (1<<Ranger))     temp |= RangerFlag;
  if (m_selectedClassesMask & (1<<Druid))      temp |= DruidFlag;
  m_selectedClassesMask = temp;
*/

	CDialog::OnOK();
}
