#if !defined(AFX_EDITATTRIBUTES_H__9A4B798B_7EBD_4D1B_A511_E3660BA4E64A__INCLUDED_)
#define AFX_EDITATTRIBUTES_H__9A4B798B_7EBD_4D1B_A511_E3660BA4E64A__INCLUDED_
/******************************************************************************
* Filename: EditAttributes.h
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

//#include "resource.h"
#include "SortableListCtrl.h"
#include "Specab.h"

/////////////////////////////////////////////////////////////////////////////
// CEditAttributes dialog

class CEditAttributes : public CDialog
{
// Construction
public:
	CEditAttributes(A_ASLENTRY_L &data, 
                  const CString& suggestions,
                  CWnd* pParent = NULL);   // standard constructor
	CEditAttributes(const SPECIAL_ABILITIES &data, 
                  CString& listName, 
                  CWnd* pParent = NULL);   // standard constructor
  void GetData(A_ASLENTRY_L &data) { data.Copy(m_data); }
  A_ASLENTRY_L m_data;
  bool m_peekOnly;
  CString m_suggestionList;

// Dialog Data
	//{{AFX_DATA(CEditAttributes)
	enum { IDD = IDD_ATTRIBUTEEDIT };
	CSortableListCtrl	m_AvailList;
	CString	m_AttListName;
  CButton m_suggestions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditAttributes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void AddToAvailList(const CString *name, const CString *value);
  void UpdateAvailList(int item, const CString *name, const CString *value);


	// Generated message map functions
	//{{AFX_MSG(CEditAttributes)
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnAdditem();
	afx_msg void OnDeleteitem();
	afx_msg void OnEdititem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITATTRIBUTES_H__9A4B798B_7EBD_4D1B_A511_E3660BA4E64A__INCLUDED_)
