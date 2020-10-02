/******************************************************************************
* Filename: JournalDataDlg.cpp
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
#include "JournalDataDlg.h"
#include "JournalEntryDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJournalDataDlg dialog


CJournalDataDlg::CJournalDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJournalDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJournalDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CJournalDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJournalDataDlg)
	DDX_Control(pDX, IDC_AVAILLIST, m_Avail);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJournalDataDlg, CDialog)
	//{{AFX_MSG_MAP(CJournalDataDlg)
	ON_BN_CLICKED(IDC_ADDITEM, OnAdditem)
	ON_BN_CLICKED(IDC_DELETEITEM, OnDeleteitem)
	ON_BN_CLICKED(IDC_EDITITEM, OnEdititem)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJournalDataDlg message handlers

void CJournalDataDlg::OnAdditem() 
{
  JOURNAL_ENTRY spData;  

  globalData.journalData.Add(spData);
  AddToAvailList(spData);
  POSITION pos = m_Avail.GetFirstSelectedItemPosition();
  if (pos != NULL)
  {
    int nItem = m_Avail.GetNextSelectedItem(pos);
    DWORD flag = m_Avail.GetItemData(nItem);
    globalData.journalData.Get(flag, spData);
    CJournalEntryDlg dlg(spData);
    if (dlg.DoModal() == IDOK)
    {
      globalData.journalData.Set(flag, dlg.m_data);
      UpdateAvailList(nItem, dlg.m_data);
    }
    else
    {
      globalData.journalData.Delete(spData);
      OnDeleteitem();
    }
  }		
}

void CJournalDataDlg::OnDeleteitem() 
{
  POSITION pos = m_Avail.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_Avail.GetNextSelectedItem(pos);
    DWORD flag = m_Avail.GetItemData(nItem);

    JOURNAL_ENTRY spData;
    globalData.journalData.Get(flag, spData);
    globalData.journalData.Delete(spData);

    m_Avail.DeleteAllItems();

    int a_count = 0;
    pos = globalData.journalData.jlist.GetHeadPosition();
    while (pos != NULL)
    {
      AddToAvailList(globalData.journalData.jlist.GetNext(pos));
      a_count++;
    }

    if (a_count > 0)
    {
      //m_Avail.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
      //m_Avail.EnsureVisible(0, FALSE);
      m_Avail.SelectItem(0, TRUE);
    }	
  }		
}

void CJournalDataDlg::OnEdititem() 
{
  POSITION pos = m_Avail.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_Avail.GetNextSelectedItem(pos);
    DWORD flag = m_Avail.GetItemData(nItem);
    JOURNAL_ENTRY data;
    globalData.journalData.Get(flag, data);
    CJournalEntryDlg dlg(data);

    if (dlg.DoModal() == IDOK)
    {
      globalData.journalData.Set(flag, dlg.m_data);
      UpdateAvailList(nItem, dlg.m_data);
    }
  }	
}

void CJournalDataDlg::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdititem();		
	*pResult = 0;
}

BOOL CJournalDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Journal Entry Text (first 90 characters)";
	lvc.cx = 450;
	lvc.fmt = LVCFMT_LEFT;
	m_Avail.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	m_Avail.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  POSITION pos = globalData.journalData.jlist.GetHeadPosition();
  while (pos != NULL)
     AddToAvailList(globalData.journalData.jlist.GetNext(pos));
 
  if (m_Avail.GetItemCount() > 0)
  {
    //m_Avail.Sort();
    //m_Avail.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_Avail.EnsureVisible(0, FALSE);  
    m_Avail.SelectItem(0, TRUE);
  }
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#define MAX_JOURNAL_DISPLAY_LEN 90

void CJournalDataDlg::AddToAvailList(JOURNAL_ENTRY &data)
{
  int listIdx = m_Avail.GetItemCount();

  char name[MAX_JOURNAL_DISPLAY_LEN+3+1+1];

  strncpy(name, data.text,MAX_JOURNAL_DISPLAY_LEN);
  name[MAX_JOURNAL_DISPLAY_LEN]='\0';
  if (data.text.GetLength() > MAX_JOURNAL_DISPLAY_LEN)
    strcat(name, "...");
  name[MAX_JOURNAL_DISPLAY_LEN+3]='\0';

  LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = listIdx;
	item.iSubItem = 0;
	item.state = 0; 
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED;
	item.pszText = name;
	item.cchTextMax = 0;
	item.iImage = 0;
	item.lParam = data.entry;

  m_Avail.InsertItem(&item);  
  //m_Avail.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  //m_Avail.EnsureVisible(listIdx, FALSE);
  m_Avail.SelectItem(listIdx, TRUE);
}

void CJournalDataDlg::UpdateAvailList(int item, JOURNAL_ENTRY &data)
{
  char name[MAX_JOURNAL_DISPLAY_LEN+3+1+1];

  strncpy(name, data.text,MAX_JOURNAL_DISPLAY_LEN);
  name[MAX_JOURNAL_DISPLAY_LEN]='\0';
  if (data.text.GetLength() > MAX_JOURNAL_DISPLAY_LEN)
    strcat(name, "...");
  name[MAX_JOURNAL_DISPLAY_LEN+3]='\0';

  m_Avail.SetItemText(item, 0, name); 
  //m_Avail.SetItemState(item, LVIS_SELECTED, LVIS_SELECTED);
  //m_Avail.EnsureVisible(item, FALSE);
  m_Avail.SelectItem(item, TRUE);
}
