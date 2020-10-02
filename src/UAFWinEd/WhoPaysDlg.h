/******************************************************************************
* Filename: WhoPaysDlg.h
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
#if !defined(AFX_WHOPAYSDLG_H__596EC8F4_0C6C_11D3_8869_00105AA4563A__INCLUDED_)
#define AFX_WHOPAYSDLG_H__596EC8F4_0C6C_11D3_8869_00105AA4563A__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CWhoPaysDlg dialog

class CWhoPaysDlg : public CDialog
{
// Construction
public:
	CWhoPaysDlg(WHO_PAYS_EVENT_DATA& data, CWnd* pParent = NULL);   // standard constructor
  void GetData(WHO_PAYS_EVENT_DATA& data);

// Dialog Data
	//{{AFX_DATA(CWhoPaysDlg)
	enum { IDD = IDD_WHOPAYSDLG };
	CComboBox	m_MoneyType;
	CButton	m_SuccessTeleport;
	CComboBox	m_SuccessAction;
	CButton	m_FailTeleport;
	CComboBox	m_FailAction;
	CButton	m_ChooseArt;
	CString	m_FailText;
	int		m_PayGems;
	int		m_PayPlatinum;
	int		m_PayJewels;
	BOOL	m_PayImpossible;
	CString	m_SuccessText;
	CString	m_Text;
	CString	m_GemName;
	CString	m_JewelryName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhoPaysDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  WHO_PAYS_EVENT_DATA m_data;
  void SetControlStates(void);

	// Generated message map functions
	//{{AFX_MSG(CWhoPaysDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseart();
	afx_msg void OnSelchangeFailaction();
	afx_msg void OnFailteleport();
	afx_msg void OnSelchangeSuccessaction();
	afx_msg void OnSuccessteleport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WHOPAYSDLG_H__596EC8F4_0C6C_11D3_8869_00105AA4563A__INCLUDED_)
