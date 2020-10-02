/******************************************************************************
* Filename: JournalEntryDlg.cpp
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
#include "JournalEntryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJournalEntryDlg dialog


CJournalEntryDlg::CJournalEntryDlg(JOURNAL_ENTRY &data, CWnd* pParent /*=NULL*/)
	: CDialog(CJournalEntryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJournalEntryDlg)
	m_Text = _T("");
	//}}AFX_DATA_INIT
  m_data = data;
}


void CJournalEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJournalEntryDlg)
	DDX_Text(pDX, IDC_JOURNALTEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJournalEntryDlg, CDialog)
	//{{AFX_MSG_MAP(CJournalEntryDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJournalEntryDlg message handlers

BOOL CJournalEntryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Text = m_data.text;	
  GotoDlgCtrl(GetDlgItem(IDC_JOURNALTEXT));
  UpdateData(FALSE);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJournalEntryDlg::OnOK() 
{
	UpdateData(TRUE);
  m_data.text = m_Text;	
	CDialog::OnOK();
}
