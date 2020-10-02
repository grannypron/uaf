#if !defined(AFX_JOURNALADDEVENTDLG_H__C09807C0_08CD_4489_B512_79311D838601__INCLUDED_)
#define AFX_JOURNALADDEVENTDLG_H__C09807C0_08CD_4489_B512_79311D838601__INCLUDED_
/******************************************************************************
* Filename: JournalAddEventDlg.h
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
#include "GameEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CJournalAddEventDlg dialog

class CJournalAddEventDlg : public CDialog
{
// Construction
public:
	CJournalAddEventDlg(JOURNAL_EVENT &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(JOURNAL_EVENT &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CJournalAddEventDlg)
	enum { IDD = IDD_JOURNALEVENT };
	CSortableListCtrl	m_ChosenList;
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJournalAddEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  JOURNAL_EVENT m_data;

  void AddToAvailList(JOURNAL_ENTRY &data);
  void AddToChosenList(int index);

	// Generated message map functions
	//{{AFX_MSG(CJournalAddEventDlg)
	afx_msg void OnAdd();
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JOURNALADDEVENTDLG_H__C09807C0_08CD_4489_B512_79311D838601__INCLUDED_)
