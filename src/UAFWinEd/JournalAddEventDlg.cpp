/******************************************************************************
* Filename: JournalAddEventDlg.cpp
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

#include "JournalAddEventDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJournalAddEventDlg dialog


CJournalAddEventDlg::CJournalAddEventDlg(JOURNAL_EVENT &data, CWnd* pParent /*=NULL*/)
	: CDialog(CJournalAddEventDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJournalAddEventDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_data = data;
}


void CJournalAddEventDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJournalAddEventDlg)
	DDX_Control(pDX, IDC_CHOSENLIST, m_ChosenList);
	DDX_Control(pDX, IDC_AVAILLIST, m_AvailList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJournalAddEventDlg, CDialog)
	//{{AFX_MSG_MAP(CJournalAddEventDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_AVAILLIST, OnDblclkAvaillist)
	ON_NOTIFY(NM_DBLCLK, IDC_CHOSENLIST, OnDblclkChosenlist)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJournalAddEventDlg message handlers

void CJournalAddEventDlg::OnAdd() 
{
  POSITION pos = m_AvailList.GetFirstSelectedItemPosition();

  if (pos != NULL)
  {
    int nItem = m_AvailList.GetNextSelectedItem(pos);
    DWORD flag = m_AvailList.GetItemData(nItem);

    m_ChosenList.DeleteAllItems();
    AddToChosenList((int)flag);
  }	
}

void CJournalAddEventDlg::OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();		
	*pResult = 0;
}

void CJournalAddEventDlg::OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
  OnDelete();	
	*pResult = 0;
}

void CJournalAddEventDlg::OnDelete() 
{
	m_ChosenList.DeleteAllItems();	
}

BOOL CJournalAddEventDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Journal Entry Text (first 90 characters)";
	lvc.cx = 450;
	lvc.fmt = LVCFMT_LEFT;
	m_AvailList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);
  m_ChosenList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);

	m_AvailList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);
  m_ChosenList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  int a_count = 0;
  POSITION pos = globalData.journalData.jlist.GetHeadPosition();
  while (pos != NULL)
  {
    AddToAvailList(globalData.journalData.jlist.GetNext(pos));
    a_count++;
  }

  if (a_count > 0)
  {
    //m_AvailList.Sort();
    //m_AvailList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    //m_AvailList.EnsureVisible(0, FALSE);
    m_AvailList.SelectItem(0, TRUE);
  }

  AddToChosenList(m_data.journal_entry);
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJournalAddEventDlg::OnOK() 
{
	UpdateData(TRUE);

  if (m_ChosenList.GetItemCount() > 0)
    m_data.journal_entry = (int)m_ChosenList.GetItemData(0);;
	
	CDialog::OnOK();
}

#define MAX_JOURNAL_DISPLAY_LEN 90

void CJournalAddEventDlg::AddToAvailList(JOURNAL_ENTRY &data)
{
  int listIdx = m_AvailList.GetItemCount();

  char name[MAX_JOURNAL_DISPLAY_LEN+1+3+1];

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

  m_AvailList.InsertItem(&item);
  //m_AvailList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_AvailList.SelectItem(listIdx, TRUE);
}

void CJournalAddEventDlg::AddToChosenList(int index)
{
  JOURNAL_ENTRY data;
  if (!globalData.journalData.Get(index, data)) 
  {
    WriteDebugString("Bogus journal entry index in CJournalAddEventDlg::AddToChosenList\n");
    return;
  }

  int listIdx = m_ChosenList.GetItemCount();

  char name[MAX_JOURNAL_DISPLAY_LEN+1+3+1];

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

  m_ChosenList.InsertItem(&item);
  //m_ChosenList.SetItemState(listIdx, LVIS_SELECTED, LVIS_SELECTED);
  m_ChosenList.SelectItem(listIdx, TRUE);
}