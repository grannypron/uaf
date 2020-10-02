/******************************************************************************
* Filename: PassTimeDlg.h
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
#if !defined(AFX_PASSTIMEDLG_H__AD6513E4_1CE6_11D3_8876_00105AA4563A__INCLUDED_)
#define AFX_PASSTIMEDLG_H__AD6513E4_1CE6_11D3_8876_00105AA4563A__INCLUDED_



/////////////////////////////////////////////////////////////////////////////
// CPassTimeDlg dialog

class CPassTimeDlg : public CDialog
{
// Construction
public:
	CPassTimeDlg(PASS_TIME_EVENT_DATA& data, CWnd* pParent = NULL);   // standard constructor
  void GetData(PASS_TIME_EVENT_DATA& data) { data = m_event; }

// Dialog Data
	//{{AFX_DATA(CPassTimeDlg)
	enum { IDD = IDD_PASSTIMEDLG };
	CButton	m_ChooseArt;
	int		m_Days;
	int		m_Hours;
	int		m_Minutes;
	CString	m_Text;
	BOOL	m_AllowExit;
	BOOL	m_SetTime;
	BOOL	m_PassSilent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPassTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  PASS_TIME_EVENT_DATA m_event;

	// Generated message map functions
	//{{AFX_MSG(CPassTimeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChooseart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSTIMEDLG_H__AD6513E4_1CE6_11D3_8876_00105AA4563A__INCLUDED_)
