/******************************************************************************
* Filename: SmallTownEvent.h
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
#if !defined(AFX_SMALLTOWNEVENT_H__AFE63143_7B31_11D3_88BD_00105AA4563A__INCLUDED_)
#define AFX_SMALLTOWNEVENT_H__AFE63143_7B31_11D3_88BD_00105AA4563A__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CSmallTownEvent dialog

class CSmallTownEvent : public CDialog
{
// Construction
public:
	CSmallTownEvent(SMALL_TOWN_DATA &event, CWnd* pParent = NULL);   // standard constructor
  void GetData(SMALL_TOWN_DATA &event) { event = m_event; }
  
// Dialog Data
	//{{AFX_DATA(CSmallTownEvent)
	enum { IDD = IDD_SMALLTOWN };
	CButton	m_SeeArt;
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallTownEvent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  SMALL_TOWN_DATA m_event;

	// Generated message map functions
	//{{AFX_MSG(CSmallTownEvent)
	afx_msg void OnTownart();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMALLTOWNEVENT_H__AFE63143_7B31_11D3_88BD_00105AA4563A__INCLUDED_)
