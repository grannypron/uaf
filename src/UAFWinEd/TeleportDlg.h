/******************************************************************************
* Filename: TeleportDlg.h
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
#if !defined(AFX_TELEPORTDLG_H__596EC8F1_0C6C_11D3_8869_00105AA4563A__INCLUDED_)
#define AFX_TELEPORTDLG_H__596EC8F1_0C6C_11D3_8869_00105AA4563A__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CTeleportDlg dialog

class CTeleportDlg : public CDialog
{
// Construction
public:
	CTeleportDlg(TRANSFER_DATA& data, BOOL AllowLevel=TRUE, CWnd* pParent = NULL);   // standard constructor
  void GetData(TRANSFER_DATA& data);

// Dialog Data
	//{{AFX_DATA(CTeleportDlg)
	enum { IDD = IDD_TELEPORTDLG };
//	CSpinButtonCtrl	m_LevelSpin;
	CComboBox	m_FacingList;
	CComboBox	m_EPList;
  CComboBox m_levelSelect;
	BOOL	m_ExecEvent;
	int		m_DestX;
	int		m_DestY;
	CString	m_XMax;
	CString	m_YMax;
//	CString	m_LevelText;
	//}}AFX_DATA

//  int m_LevelNum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTeleportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	TRANSFER_DATA m_teleport;
  void FormatEPList(int destEP, int destLevel);
  void FillLevelList(int defaultLevel);    
  BOOL m_AllowLevel;

	// Generated message map functions
	//{{AFX_MSG(CTeleportDlg)
	virtual void OnOK();
	afx_msg void OnDeltaposLevelspin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFacing();
	afx_msg void OnSelchangeEntrypointlist();
  afx_msg void OnLevelChange(void);
  //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELEPORTDLG_H__596EC8F1_0C6C_11D3_8869_00105AA4563A__INCLUDED_)
