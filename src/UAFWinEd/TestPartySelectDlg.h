#if !defined(AFX_TESTPARTYSELECTDLG_H__0E0A7CFD_7B26_449D_9865_6D7AE0799691__INCLUDED_)
#define AFX_TESTPARTYSELECTDLG_H__0E0A7CFD_7B26_449D_9865_6D7AE0799691__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/******************************************************************************
* Filename: TestPartySelectDlg.h
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

/////////////////////////////////////////////////////////////////////////////
// CTestPartySelectDlg dialog

#include "SortableListCtrl.h"

class CTestPartySelectDlg : public CDialog
{
// Construction
public:
	CTestPartySelectDlg(CWnd* pParent = NULL);   // standard constructor

  CList<CString, CString&> PartyList;
  
// Dialog Data
	//{{AFX_DATA(CTestPartySelectDlg)
	enum { IDD = IDD_TESTPARTY };
	CSortableListCtrl	m_PartyList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestPartySelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  BOOL EnableCheckIfNameInList(CString pc);
  BOOL IsNameInList(CString pc);
  DWORD CountCheckedNames();

	// Generated message map functions
	//{{AFX_MSG(CTestPartySelectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnItemchangedPartylist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTPARTYSELECTDLG_H__0E0A7CFD_7B26_449D_9865_6D7AE0799691__INCLUDED_)
