/******************************************************************************
* Filename: SoundEventDlg.h
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
#if !defined(AFX_SOUNDEVENTDLG_H__91CA2F41_8E9E_11D3_BF9A_00A0CC25685D__INCLUDED_)
#define AFX_SOUNDEVENTDLG_H__91CA2F41_8E9E_11D3_BF9A_00A0CC25685D__INCLUDED_


#include "GameEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CSoundEventDlg dialog

class CSoundEventDlg : public CDialog
{
// Construction
public:
	CSoundEventDlg(SOUND_EVENT &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(SOUND_EVENT &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CSoundEventDlg)
	enum { IDD = IDD_SOUNDEVENTDLG };
	CListBox	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  SOUND_EVENT m_data;

  void AddSound(CString &sound);
  void RemoveSound(int index);
  bool FindSound(CString &sound);

	// Generated message map functions
	//{{AFX_MSG(CSoundEventDlg)
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnEdit();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOUNDEVENTDLG_H__91CA2F41_8E9E_11D3_BF9A_00A0CC25685D__INCLUDED_)
