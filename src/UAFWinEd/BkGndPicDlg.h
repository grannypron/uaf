/******************************************************************************
* Filename: BkGndPicDlg.h
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

#include "DlgPicture.h"

/////////////////////////////////////////////////////////////////////////////
// CBkGndPicDlg dialog

class CBkGndPicDlg : public CDialog
{
// Construction
public:
	CBkGndPicDlg(BackgroundSlotMemType &data, CWnd* pParent = NULL);   // standard constructor
  //~CBkGndPicDlg(void);

// Dialog Data
	//{{AFX_DATA(CBkGndPicDlg)
	enum { IDD = IDD_BACKGROUNDPICDLG };
	CButton	m_SelectSound;
	CDlgPicture	m_Picture2;
	CDlgPicture	m_Picture;
	int		m_EndTime;
	int		m_StartTime;
	BOOL	m_UseBg2;
	CString	m_AltFilename;
	CString	m_Filename;
	int		m_BlendPcnt;
	BOOL	m_UseAlphaBlend;
	BOOL	m_UseTransparency;
	BOOL	m_SuppressStepSound;
	//}}AFX_DATA


  BackgroundSlotMemType m_data;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBkGndPicDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   CString m_Sound;
   void LoadFile(CString file);
   void LoadAltFile(CString file);

	// Generated message map functions
	//{{AFX_MSG(CBkGndPicDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUsebg2();
	afx_msg void OnSelectdefault();
	afx_msg void OnSelectalt();
	virtual void OnOK();
	afx_msg void OnSelectsound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
