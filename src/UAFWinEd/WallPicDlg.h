/******************************************************************************
* Filename: WallPicDlg.h
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
// CWallPicDlg dialog

#include "DlgPicture.h"

class CWallPicDlg : public CDialog
{
// Construction
public:
	CWallPicDlg(CString filename, CString sound, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWallPicDlg)
	enum { IDD = IDD_WALLPICDLG };
	CButton	m_SelectSound;
	CButton	m_SelectFile;
	C3DSelectPicture	m_Picture;
	int		m_ImageFormat;
	BOOL	m_DrawAV;
	//}}AFX_DATA

  CString m_Filename;
  CString m_Sound;
  BOOL m_UseAV;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWallPicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   void LoadFile(CString file);

	// Generated message map functions
	//{{AFX_MSG(CWallPicDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectfile();
	afx_msg void OnClear();
	afx_msg void OnSelectsound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
