/******************************************************************************
* Filename: SoundChooser.h
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
#if !defined(AFX_SOUNDCHOOSER_H__06CAAEF3_8C7C_11D3_88D6_00105AA4563A__INCLUDED_)
#define AFX_SOUNDCHOOSER_H__06CAAEF3_8C7C_11D3_88D6_00105AA4563A__INCLUDED_


#include "SoundMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CSoundChooser dialog

class CSoundChooser : public CDialog
{
// Construction
public:
	CSoundChooser(CString filename, BYTE filter, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSoundChooser)
	enum { IDD = IDD_SOUNDCHOOSER };
	CButton	m_Stop;
	CButton	m_Play;
	CString	m_Filename;
	BOOL	m_Loop;
	CString	m_Filetypes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundChooser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
  BYTE m_filter;
  bool isWave;
  long m_buf;
  //CWave SndBuf;
  HANDLE m_hPlayIcon;
  HANDLE m_hStopIcon;

	// Generated message map functions
	//{{AFX_MSG(CSoundChooser)
	afx_msg void OnStop();
	afx_msg void OnPlay();
	afx_msg void OnFilebrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDCHOOSER_H__06CAAEF3_8C7C_11D3_88D6_00105AA4563A__INCLUDED_)
