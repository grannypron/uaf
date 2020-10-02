/******************************************************************************
* Filename: ScrollPicDlg.h
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
// CScrollPicDlg dialog

#include "DlgPicture.h"

class CScrollPicDlg : public CDialog
{
// Construction
public:
	CScrollPicDlg(const CString& filename, 
                const CString& path1, 
                const CString& path2, 
                SurfaceType type=BufferDib, 
                CWnd* pParent = NULL);   // standard constructor
  CRect m_srcRect;

// Dialog Data
	//{{AFX_DATA(CScrollPicDlg)
	enum { IDD = IDD_SCROLLPICTUREDLG };
	CScrollBar	m_VertScroll;
	CDlgPartialPicture	m_Picture;
	CScrollBar	m_HorzScroll;
	CString	m_Filename;
	//}}AFX_DATA

  SurfaceType m_type;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollPicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  enum { PAGE_SIZE = 32, LINE_SIZE = 10 };
 
  int currX;
  int currY;
  CRect m_BufferRect;
  void SetSBData(CRect rect);
  BOOL LoadFile(CString file);
  CString m_path1;
  CString m_path2;

	// Generated message map functions
	//{{AFX_MSG(CScrollPicDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelectfile();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
