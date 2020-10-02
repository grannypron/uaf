/******************************************************************************
* Filename: TestPartySelectDlg.cpp
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
#include "TestPartySelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestPartySelectDlg dialog


CTestPartySelectDlg::CTestPartySelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestPartySelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestPartySelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTestPartySelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestPartySelectDlg)
	DDX_Control(pDX, IDC_PARTYLIST, m_PartyList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestPartySelectDlg, CDialog)
	//{{AFX_MSG_MAP(CTestPartySelectDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PARTYLIST, OnItemchangedPartylist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestPartySelectDlg message handlers

BOOL CTestPartySelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_PartyList.SetExtendedStyle(LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);

	// insert columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.pszText = "Character";
	lvc.cx = 325;
	lvc.fmt = LVCFMT_LEFT;
	m_PartyList.InsertColumn(0,&lvc, CListSort::dtSTRING, true);
	

  m_PartyList.SetDefaultSortAttributes(0, CSortableListCtrl::sdASCENDING);

  // search save game folder and common char folder for characters
  // and add them to available temps,

  // save game folder first
  CString searchpath = GetSavePath();
  if (searchpath[searchpath.GetLength()-1] != '\\')
    searchpath += "\\";
  searchpath += "*.chr";
 
  CFileFind finder;
  BOOL found = finder.FindFile(searchpath);

  while (found)
  {
     found = finder.FindNextFile();

     if (finder.IsDots())
        continue;
     if (finder.IsDirectory())
       continue;

     if (!IsNameInList(finder.GetFileTitle()))
       m_PartyList.InsertItem(m_PartyList.GetItemCount(), finder.GetFileTitle());
  }

  finder.Close();

  // now the common char folder
  searchpath = GetCommonCharPath();
  if (!searchpath.IsEmpty())
  {
    if (searchpath[searchpath.GetLength()-1] != '\\')
      searchpath += "\\";
    searchpath += "*.chr";
 
    found = finder.FindFile(searchpath);

    while (found)
    {
       found = finder.FindNextFile();
       if (finder.IsDots())
          continue;
       if (finder.IsDirectory())
         continue;

       if (!IsNameInList(finder.GetFileTitle()))
         m_PartyList.InsertItem(m_PartyList.GetItemCount(), finder.GetFileTitle());
    }

    finder.Close();
  }

  POSITION pos = PartyList.GetHeadPosition();

  while (pos != NULL)
    EnableCheckIfNameInList(PartyList.GetNext(pos));
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTestPartySelectDlg::EnableCheckIfNameInList(CString pc)
{
  DWORD count = CountCheckedNames();
  if (count >= MAX_PARTY_PCS)
    return FALSE;

  for (int i=0;i<m_PartyList.GetItemCount();i++)
  {
    if (m_PartyList.GetItemText(i,0).CompareNoCase(pc) == 0)
    {
      m_PartyList.SetCheck(i, TRUE);
      return TRUE;
    }
  }

  return FALSE;
}

void CTestPartySelectDlg::OnOK() 
{
  PartyList.RemoveAll();
  
  for (int i=0;i<m_PartyList.GetItemCount();i++)
  {
    int IsChecked = m_PartyList.GetCheck(i);

    if (IsChecked)
    {
      CString pc = m_PartyList.GetItemText(i,0);
      PartyList.AddTail(pc);
    }
  }
  
	CDialog::OnOK();
}

BOOL CTestPartySelectDlg::IsNameInList(CString pc)
{
  for (int i=0;i<m_PartyList.GetItemCount();i++)
  {
    if (m_PartyList.GetItemText(i,0).CompareNoCase(pc) == 0)
      return TRUE;
  }

  return FALSE;
}

DWORD CTestPartySelectDlg::CountCheckedNames()
{
  DWORD count = 0;
  for (int i=0;i<m_PartyList.GetItemCount();i++)
  {
    if (m_PartyList.GetCheck(i))
      count++;
  }
  return count;
}

void CTestPartySelectDlg::OnItemchangedPartylist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

  if (   (pNMListView->iItem >= 0) 
      && (pNMListView->uChanged & LVIF_STATE))
  {
    DWORD count = CountCheckedNames();
    if (count > MAX_PARTY_PCS)
    {
      if (m_PartyList.GetCheck(pNMListView->iItem))
        m_PartyList.SetCheck(pNMListView->iItem, FALSE);
    }
  }
	
	*pResult = 0;
}
