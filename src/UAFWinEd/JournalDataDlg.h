#if !defined(AFX_JOURNALDATADLG_H__9892D50C_5604_46A0_8F66_B2438710806D__INCLUDED_)
#define AFX_JOURNALDATADLG_H__9892D50C_5604_46A0_8F66_B2438710806D__INCLUDED_
/******************************************************************************
* Filename: JournalDataDlg.h
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

#include "SortableListCtrl.h"
#include "GlobalData.h"

/////////////////////////////////////////////////////////////////////////////
// CJournalDataDlg dialog

class CJournalDataDlg : public CDialog
{
// Construction
public:
	CJournalDataDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CJournalDataDlg)
	enum { IDD = IDD_JOURNALEDITOR };
	CSortableListCtrl	m_Avail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJournalDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void AddToAvailList(JOURNAL_ENTRY &data);
  void UpdateAvailList(int entry, JOURNAL_ENTRY &data);

	// Generated message map functions
	//{{AFX_MSG(CJournalDataDlg)
	afx_msg void OnAdditem();
	afx_msg void OnDeleteitem();
	afx_msg void OnEdititem();
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOURNALDATADLG_H__9892D50C_5604_46A0_8F66_B2438710806D__INCLUDED_)
