/******************************************************************************
* Filename: VaultEvent.h
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
#if !defined(AFX_VAULTEVENT_H__999EAA53_5955_11D3_88A6_00105AA4563A__INCLUDED_)
#define AFX_VAULTEVENT_H__999EAA53_5955_11D3_88A6_00105AA4563A__INCLUDED_



/////////////////////////////////////////////////////////////////////////////
// CVaultEvent dialog

class CVaultEvent : public CDialog
{
// Construction
public:
	CVaultEvent(VAULT_EVENT_DATA& event, CWnd* pParent = NULL);   // standard constructor
  void GetData(VAULT_EVENT_DATA& event);

// Dialog Data
	//{{AFX_DATA(CVaultEvent)
	enum { IDD = IDD_VAULTDATADIALOG };
	CComboBox	m_WhichVault;
	CButton	m_SeeArt;
	BOOL	m_ForceBackup;
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVaultEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  VAULT_EVENT_DATA m_event;

	// Generated message map functions
	//{{AFX_MSG(CVaultEvent)
	afx_msg void OnChooseart();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VAULTEVENT_H__999EAA53_5955_11D3_88A6_00105AA4563A__INCLUDED_)
