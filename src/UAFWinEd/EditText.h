#if !defined(AFX_EDITTEXT_H__EAEEEB01_8370_4AE0_905F_561E38EBC1FC__INCLUDED_)
#define AFX_EDITTEXT_H__EAEEEB01_8370_4AE0_905F_561E38EBC1FC__INCLUDED_

/******************************************************************************
* Filename: EditText.h
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
// EditText.h : header file
//
#include "resource.h"

struct SScriptInfo
{
  SScriptInfo &operator=(const SScriptInfo &rhs)
  {
    if (this == &rhs) return *this;
    name = rhs.name;
    script = rhs.script;
    return *this;
  }

  CString name;
  CString script;
};

typedef CArray<SScriptInfo, SScriptInfo&> ScriptList;

/////////////////////////////////////////////////////////////////////////////
// CEditText dialog

class CEditText : public CDialog
{
// Construction
public:
	CEditText(const CString &pre, 
            const CString &post, 
            const CString &func, 
            bool  isScript,
            ScriptList& list,
            CWnd* pParent = NULL);   // standard constructor
  ~CEditText();

// Dialog Data
	//{{AFX_DATA(CEditText)
	enum { IDD = IDD_TEXTEDIT };
	CComboBox	m_ScriptCombo;
	CEdit	m_textEditor;
	CListBox	m_ScriptFunctionList;
	CString	m_Text;
	CString	m_ScriptName1;
	//}}AFX_DATA

  CString m_pre;
  CString m_post;
  CString m_func;
  CString m_helpKey;
  bool    m_isScript;
  DWORD m_ScriptFilter;
  ScriptList m_list;
  DWORD availableContexts;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditText)
	protected:
  BOOL PreTranslateMessage(MSG* pMsg);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditText)
	virtual BOOL OnInitDialog();
	afx_msg void OnCompile();
	virtual void OnOK();
	afx_msg void OnSelchangeScriptfunctionlist();
	afx_msg void OnHelp();
	afx_msg void OnDblclkScriptfunctionlist();
	afx_msg void OnSelchangeScriptcombo();
	afx_msg void OnDropdownScriptcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTEXT_H__EAEEEB01_8370_4AE0_905F_561E38EBC1FC__INCLUDED_)
