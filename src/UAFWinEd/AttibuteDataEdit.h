#if !defined(AFX_ATTIBUTEDATAEDIT_H__4F074552_6171_4E12_8489_C748A73CF903__INCLUDED_)
#define AFX_ATTIBUTEDATAEDIT_H__4F074552_6171_4E12_8489_C748A73CF903__INCLUDED_
/******************************************************************************
* Filename: AttibuteDataEdit.h
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
/////////////////////////////////////////////////////////////////////////////
// CAttibuteDataEdit dialog

class CAttibuteDataEdit : public CDialog
{
// Construction
public:
	CAttibuteDataEdit(CString name, CString value,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttibuteDataEdit)
	enum { IDD = IDD_ATTRIBUTEDATAEDIT };
	CString	m_Name;
	CString	m_Value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttibuteDataEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttibuteDataEdit)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTIBUTEDATAEDIT_H__4F074552_6171_4E12_8489_C748A73CF903__INCLUDED_)
