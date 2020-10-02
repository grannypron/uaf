/******************************************************************************
* Filename: SmallPicDlg.h
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
#ifndef __SMALLPICDLG_H__
#define __SMALLPICDLG_H__

#include "DlgPicture.h"

/////////////////////////////////////////////////////////////////////////////
// CSmallPicDlg dialog

class CSmallPicDlg : public CDialog
{
// Construction
public:
	CSmallPicDlg(PIC_DATA &pic, CWnd* pParent = NULL);   // standard constructor
  PIC_DATA m_SmallPic;

// Dialog Data
	//{{AFX_DATA(CSmallPicDlg)
	enum { IDD = IDD_SMALLPICDLG };
	CButton	m_SelectFile;
	CButton	m_ToggleAnim;
	CButton	m_UpdateStats;
	CDlgSmallPicture	m_SmallPicResult;
	int		m_TimeDelay;
	int		m_NumFrames;
	int		m_FrameWidth;
	int		m_FrameHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmallPicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   CString m_CurrentFile;
   enum { SmallPicTimer = 99 };
   void LoadFile();

	// Generated message map functions
	//{{AFX_MSG(CSmallPicDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelectfile();
	afx_msg void OnViewallpic();
	afx_msg void OnUpdatestats();
	afx_msg void OnToggleanim();
	afx_msg void OnNextframe();
	afx_msg void OnAnimationstyle();
	afx_msg void OnClearpic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
