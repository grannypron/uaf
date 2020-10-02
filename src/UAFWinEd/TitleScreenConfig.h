#if !defined(AFX_TITLESCREENCONFIG_H__55C85CEF_63C7_4EE4_937B_7F1F7480AA06__INCLUDED_)
#define AFX_TITLESCREENCONFIG_H__55C85CEF_63C7_4EE4_937B_7F1F7480AA06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/******************************************************************************
* Filename: TitleScreenConfig.h
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
#include "GlobalData.h"

/////////////////////////////////////////////////////////////////////////////
// CTitleScreenConfig dialog

class CTitleScreenConfig : public CDialog
{
// Construction
public:
	CTitleScreenConfig(TITLE_SCREEN &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(TITLE_SCREEN &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CTitleScreenConfig)
	enum { IDD = IDD_TITLESCREEN };
	CButton	m_SelectTitle;
	BOOL	m_UseBlend;
	int		m_DisplayStyle;
	BOOL	m_UseTrans;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleScreenConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  TITLE_SCREEN m_data;

	// Generated message map functions
	//{{AFX_MSG(CTitleScreenConfig)
	afx_msg void OnDisplaystyle3();
	afx_msg void OnDisplaystyle2();
	afx_msg void OnDisplaystyle();
	afx_msg void OnSelecttitlefilename();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLESCREENCONFIG_H__55C85CEF_63C7_4EE4_937B_7F1F7480AA06__INCLUDED_)
