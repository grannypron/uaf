/******************************************************************************
* Filename: SmallPicPoolDlg.h
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
// CSmallPicPoolDlg dialog

class CSmallPicPoolDlg : public CDialog
{
// Construction
public:
	CSmallPicPoolDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSmallPicPoolDlg)
	enum { IDD = IDD_SMALLPICPOOL };
	CButton	m_DeletePic;
	CButton	m_ViewPic;
	CListBox	m_PicList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallPicPoolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void ResetPicList(void);

	// Generated message map functions
	//{{AFX_MSG(CSmallPicPoolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoadpic();
	afx_msg void OnViewpic();
	afx_msg void OnSetfocusPiclist();
	afx_msg void OnDeletepic();
	afx_msg void OnDblclkPiclist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
