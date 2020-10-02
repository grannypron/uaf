#if !defined(AFX_JOURNALENTRYDLG_H__11CB9761_D0A3_4F5F_8F0D_200B4BBAF35F__INCLUDED_)
#define AFX_JOURNALENTRYDLG_H__11CB9761_D0A3_4F5F_8F0D_200B4BBAF35F__INCLUDED_
/******************************************************************************
* Filename: JournalEntryDlg.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CJournalEntryDlg dialog

class CJournalEntryDlg : public CDialog
{
// Construction
public:
	CJournalEntryDlg(JOURNAL_ENTRY &data, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJournalEntryDlg)
	enum { IDD = IDD_JOURNALENTRYDATA };
	CString	m_Text;
	//}}AFX_DATA

  JOURNAL_ENTRY m_data;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJournalEntryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJournalEntryDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOURNALENTRYDLG_H__11CB9761_D0A3_4F5F_8F0D_200B4BBAF35F__INCLUDED_)
