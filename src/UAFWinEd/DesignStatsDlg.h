#if !defined(AFX_DESIGNSTATSDLG_H__8AFDD83F_C59C_4329_8D22_D134AE6716AE__INCLUDED_)
#define AFX_DESIGNSTATSDLG_H__8AFDD83F_C59C_4329_8D22_D134AE6716AE__INCLUDED_
/******************************************************************************
* Filename: DesignStatsDlg.h
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

/////////////////////////////////////////////////////////////////////////////
// CDesignStatsDlg dialog

class CDesignStatsDlg : public CDialog
{
// Construction
public:
	CDesignStatsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignStatsDlg)
	enum { IDD = IDD_DESIGNSTATS };
	CString	m_Events;
	CString	m_Items;
	CString	m_Levels;
	CString	m_Monsters;
	CString	m_NPCs;
	CString	m_Quests;
	CString	m_SizeOnDisk;
	CString	m_Spells;
	CString	m_SpItems;
	CString	m_SpKeys;
	CString	m_Files;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignStatsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDesignStatsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESIGNSTATSDLG_H__8AFDD83F_C59C_4329_8D22_D134AE6716AE__INCLUDED_)
