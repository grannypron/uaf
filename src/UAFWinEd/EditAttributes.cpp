/******************************************************************************
* Filename: EditAttributes.cpp
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
//#include "uafwined.h"
#include "AttibuteDataEdit.h"

#include "EditAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditAttributes dialog

CEditAttributes::CEditAttributes(A_ASLENTRY_L &data, 
                                 const CString& suggestions,
                                 CWnd* pParent /*=NULL*/)
#ifdef mapNames
	: CDialog(CEditAttributes::IDD, pParent), m_data("NULL_LIST")
#else
  // ASL m_data is named "NULL_LIST"
	: CDialog(CEditAttributes::IDD, pParent), m_data()
#endif
{
	//{{AFX_DATA_INIT(CEditAttributes)
	m_AttListName = _T("");
	//}}AFX_DATA_INIT
  m_data.Copy(data);
  m_suggestionList = suggestions;
  m_peekOnly = false;
}

CEditAttributes::CEditAttributes(const SPECIAL_ABILITIES &data, 
                                 CString& listName, 
                                 CWnd* pParent /*=NULL*/)
  // ASL m_data is named "NULL_LIST"
	: CDialog(CEditAttributes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditAttributes)
	m_AttListName = _T("");
	//}}AFX_DATA_INIT
  m_data.Copy(data.SpecialAbilities());
  m_AttListName = listName;
  m_peekOnly = false;
}


void CEditAttributes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditAttributes)
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	DDX_Text(pDX, IDC_ATTLISTNAME, m_AttListName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditAttributes, CDialog)
	//{{AFX_MSG_MAP(CEditAttributes)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
	ON_BN_CLICKED(IDC_EDITITEM, OnEdititem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditAttributes message handlers

BOOL CEditAttributes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Name";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	lvc.iSubItem = 1;
	lvc.pszText = "Value";
	lvc.cx = 120;
	lvc.fmt = LVCFMT_LEFT;
  m_AvailList.InsertColumn(1,&lvc, CListSort::dtSTRING, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
#ifdef mapNames
  m_AttListName = m_data.GetName();
#else
#endif

	CString key;
	CString value;
  POSITION pos = m_data.GetStartPosition();
	while (pos!=NULL) 
  {
    const ASLENTRY *pEntry;
		m_data.GetNextAssoc(pos, &pEntry); //key, value);
    AddToAvailList(pEntry->pKey(), pEntry->pValue());
	}
 
  if (m_AvailList.GetItemCount() > 0)
  {
    m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);  
    m_AvailList.SelectItem(0, TRUE);
  }	;
  if (m_peekOnly)
  {
    CWnd *pWnd;
    pWnd = GetDlgItem(IDC_ADDITEM);
    if (pWnd != NULL) pWnd->ShowWindow(false);
    pWnd = GetDlgItem(IDC_EDITITEM);
    if (pWnd != NULL) pWnd->ShowWindow(false);
    pWnd = GetDlgItem(IDC_DELETEITEM);
    if (pWnd != NULL) pWnd->ShowWindow(false);
  };
  {
    CWnd *pWnd;
    pWnd = GetDlgItem(IDC_Suggestions);
    if (pWnd != NULL) pWnd->ShowWindow(!m_suggestionList.IsEmpty());
  };
 

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditAttributes::AddToAvailList(const CString *name, const CString *value)
{
  int listIdx = m_AvailList.GetItemCount();

  if (name->GetLength() == 0) return;
  char *pname = new char[name->GetLength()+1];
  strcpy(pname, *name);

  char *pvalue = NULL;
  if (value->GetLength() > 0) 
  {
    pvalue = new char[value->GetLength()+1];
    strcpy(pvalue, *value);
  }
  else
  {
    pvalue = new char[1];
    strcpy(pvalue, "");
  }  

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = pname;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = 0;

  m_AvailList.InsertItem(&item);
  m_AvailList.SetItemText(listIdx, 1, pvalue); 
  
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  //m_AvailList.EnsureVisible(listIdx, FALSE);
  m_AvailList.SelectItem(listIdx, TRUE);

  if (pname != NULL) delete [] pname;
  if (pvalue != NULL) delete [] pvalue;
}

void CEditAttributes::UpdateAvailList(int item, const CString *name, const CString *value)
{
  if (name->GetLength() == 0) return;
  char *pname = new char[name->GetLength()+1];
  strcpy(pname, *name);

  char *pvalue = NULL;
  if (value->GetLength() > 0) 
  {
    pvalue = new char[value->GetLength()+1];
    strcpy(pvalue, *value);
  }
  else
  {
    pvalue = new char[1];
    strcpy(pvalue, "");
  }  

  m_AvailList.SetItemText(item, 0, pname); 
  m_AvailList.SetItemText(item, 1, pvalue); 
  
  //m_AvailList.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  //m_AvailList.EnsureVisible(item, FALSE);
  m_AvailList.SelectItem(item, TRUE);

  if (pname != NULL) delete [] pname;
  if (pvalue != NULL) delete [] pvalue;
}

void CEditAttributes::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdititem();		
	*pResult = 0;
}

void CEditAttributes::OnDeleteitem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    CString name = m_AvailList.GetItemText(nItem,0);
    m_data.Delete(name);
    m_AvailList.DeleteAllItems();

	  CString key;
	  CString value;
    POSITION pos2 = m_data.GetStartPosition();
	  while (pos2!=NULL) 
    {
      const ASLENTRY *pEntry;
		  m_data.GetNextAssoc(pos2, &pEntry);// key, value);
      AddToAvailList(pEntry->pKey(), pEntry->pValue());
	  }
 
    if (m_AvailList.GetItemCount() > 0)
    {
      m_AvailList.Sort();
      //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_AvailList.EnsureVisible(0, FALSE);  
      m_AvailList.SelectItem(0, TRUE);
    }	
  }	
}

void CEditAttributes::OnEdititem() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    CString name = m_AvailList.GetItemText(nItem,0);
    CString value = m_AvailList.GetItemText(nItem,1);
    CString origname = name;
    CAttibuteDataEdit dlg(name, value);
    if (dlg.DoModal()==IDOK)
    {
      //name = dlg.m_Name;
      //value = dlg.m_Value;

      UpdateAvailList(nItem, &dlg.m_Name, &dlg.m_Value);
      if (origname != dlg.m_Name)
        m_data.Delete(origname);
      m_data.Update(dlg.m_Name, dlg.m_Value, ASLF_EDITOR); // updates existing
    }
  }	
}

void CEditAttributes::OnAdditem() 
{ 
  CAttibuteDataEdit dlg("", "");
  if (dlg.DoModal()==IDOK)
  {
    m_data.Update(dlg.m_Name,dlg.m_Value, ASLF_EDITOR);
    AddToAvailList(&dlg.m_Name, &dlg.m_Value);
  }
}
