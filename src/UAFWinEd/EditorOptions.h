#if !defined(AFX_EDITOROPTIONS_H__F431639F_3ED0_4D47_A899_44BDA93D983A__INCLUDED_)
#define AFX_EDITOROPTIONS_H__F431639F_3ED0_4D47_A899_44BDA93D983A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/******************************************************************************
* Filename: EditorOptions.cpp
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
// CEditorOptions dialog

class CEditorOptions : public CDialog
{
// Construction
public:
	CEditorOptions(CEditorOptionsData &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(CEditorOptionsData &data) { data=m_data; }

// Dialog Data
	//{{AFX_DATA(CEditorOptions)
	enum { IDD = IDD_EDITOROPTIONS };
//	BOOL	m_ReloadDesign;
	BOOL	m_DblFillWall;
	BOOL	m_DynaCell;
	BOOL	m_KwikKlik;
	BOOL	m_TileMap;
	BOOL	m_PassWall;
	BOOL	m_QuadFillBg;
	int		m_EditorScreenRes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  CEditorOptionsData m_data;

	// Generated message map functions
	//{{AFX_MSG(CEditorOptions)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITOROPTIONS_H__F431639F_3ED0_4D47_A899_44BDA93D983A__INCLUDED_)
