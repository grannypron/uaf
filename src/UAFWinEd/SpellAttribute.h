#if !defined(AFX_SPELLATTRIBUTE_H__D72A4220_B1C8_46BD_891A_BE9CADA441EE__INCLUDED_)
#define AFX_SPELLATTRIBUTE_H__D72A4220_B1C8_46BD_891A_BE9CADA441EE__INCLUDED_
/******************************************************************************
* Filename: SpellAttribute.h
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

#include "class.h"
#include "resource.h"

#include "..\Shared\GPDLOpCodes.h"
#include "..\Shared\GPDLcomp.h"
#include "..\Shared\GPDLexec.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellAttribute dialog

class CSpellAttribute : public CDialog
{
// Construction
public:
	CSpellAttribute(SPELL_EFFECTS_DATA &data, CWnd* pParent = NULL);   // standard constructor
  ~CSpellAttribute();
  void GetData(SPELL_EFFECTS_DATA &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CSpellAttribute)
	enum { IDD = IDD_SPELLATTRIBUTEEDIT };
	CComboBox	m_AttNameList;
	BOOL	m_AffectTarget;
	BOOL	m_AffectTargeter;
	CString	m_ChangeText;
	int		m_ChangeType;
	BOOL	m_IsCumulative;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellAttribute)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  
  SPELL_EFFECTS_DATA m_data;

	// Generated message map functions
	//{{AFX_MSG(CSpellAttribute)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRolldice();
	afx_msg void OnEditscripts();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPELLATTRIBUTE_H__D72A4220_B1C8_46BD_891A_BE9CADA441EE__INCLUDED_)
