/******************************************************************************
* Filename: WhoTriesDlg.h
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
#if !defined(AFX_WHOTRIESDLG_H__A13B5F43_137B_11D3_8872_00105AA4563A__INCLUDED_)
#define AFX_WHOTRIESDLG_H__A13B5F43_137B_11D3_8872_00105AA4563A__INCLUDED_



/////////////////////////////////////////////////////////////////////////////
// CWhoTriesDlg dialog

class CWhoTriesDlg : public CDialog
{
// Construction
public:
	CWhoTriesDlg(WHO_TRIES_EVENT_DATA& data, CWnd* pParent = NULL);   // standard constructor
  void GetData(WHO_TRIES_EVENT_DATA& data);

// Dialog Data
	//{{AFX_DATA(CWhoTriesDlg)
	enum { IDD = IDD_WHOTRIES };
	CButton	m_ChooseArt;
	CButton	m_SuccessTeleport;
	CComboBox	m_SuccessAction;
	CButton	m_FailTeleport;
	CComboBox	m_FailAction;
	BOOL	m_AlwaysFails;
	BOOL	m_AlwaysSucceeds;
	//BOOL	m_CheckCha;
	//BOOL	m_CheckCon;
	//BOOL	m_CheckDex;
	//BOOL	m_CheckInt;
	//BOOL	m_CheckStr;
	//int		m_CheckStrBonus;
	//BOOL	m_CheckWis;
	//int		m_CompareTo;
	//int		m_CompareDieSides;
	CString	m_FailText;
	int		m_NumTries;
	CString	m_SuccessText;
	CString	m_Text;
  A_ASLENTRY_L m_skills;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhoTriesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  WHO_TRIES_EVENT_DATA m_data;
  void SetControlStates(void);

	// Generated message map functions
	//{{AFX_MSG(CWhoTriesDlg)
	virtual BOOL OnInitDialog();
	//afx_msg void OnCheckstr();
	afx_msg void OnAlwaysfails();
	afx_msg void OnAlwayssucceeds();
	afx_msg void OnSelchangeFailaction();
	afx_msg void OnFailteleport();
	afx_msg void OnSelchangeSuccessaction();
	afx_msg void OnSuccessteleport();
	virtual void OnOK();
	afx_msg void OnChooseart();
	afx_msg void OnSelectSkills();
  afx_msg void OnWhoTriesQuestion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WHOTRIESDLG_H__A13B5F43_137B_11D3_8872_00105AA4563A__INCLUDED_)
