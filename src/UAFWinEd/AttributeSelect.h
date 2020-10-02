#if !defined(AFX_ATTRIBUTESELECT_H__ACA659B7_306C_4A42_90C0_B756C3CA4296__INCLUDED_)
#define AFX_ATTRIBUTESELECT_H__ACA659B7_306C_4A42_90C0_B756C3CA4296__INCLUDED_
/******************************************************************************
* Filename: AttributeSelect.h
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
// CAttributeSelect dialog

class CAttributeSelect : public CDialog
{
// Construction
public:
	CAttributeSelect(BOOL AllowReadOnly=TRUE,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAttributeSelect)
	enum { IDD = IDD_RUNTIMEATTRIBUTESELECT };
	CListBox	m_AttList;
	//}}AFX_DATA

  CString attribute;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttributeSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  BOOL m_AllowReadOnly;
	// Generated message map functions
	//{{AFX_MSG(CAttributeSelect)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkAttlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIBUTESELECT_H__ACA659B7_306C_4A42_90C0_B756C3CA4296__INCLUDED_)
