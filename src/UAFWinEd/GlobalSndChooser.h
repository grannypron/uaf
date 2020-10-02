/******************************************************************************
* Filename: GlobalSndChooser.h
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
#if !defined(AFX_GLOBALSNDCHOOSER_H__06CAAEF8_8C7C_11D3_88D6_00105AA4563A__INCLUDED_)
#define AFX_GLOBALSNDCHOOSER_H__06CAAEF8_8C7C_11D3_88D6_00105AA4563A__INCLUDED_

#include "GlobalData.h"

/////////////////////////////////////////////////////////////////////////////
// CGlobalSndChooser dialog

class CGlobalSndChooser : public CDialog
{
// Construction
public:
	CGlobalSndChooser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGlobalSndChooser)
	enum { IDD = IDD_GLOBALSOUNDS };
	CButton	m_BgndQueueStop;
	CButton	m_SimulMidiTest;
	CButton	m_SoundTestStopSimul;
	CButton	m_SoundTextStopQueue;
	CButton	m_SoundTestQueued;
	CButton	m_SoundTest;
	CButton	m_CharMiss;
	CButton	m_CharHit;
	CButton	m_PartyStep;
	CButton	m_PartyBump;
	CButton	m_IntroMusic;
	CButton	m_DeathMusic;
	//}}AFX_DATA

  CString PartyStep;
  CString PartyBump;
  BACKGROUND_SOUNDS IntroMusic;
  BACKGROUND_SOUNDS CampMusic;
  CString DeathMusic;
  CString CharHit;
  CString CharMiss;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGlobalSndChooser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGlobalSndChooser)
	afx_msg void OnDeathmusic();
	afx_msg void OnIntromusic();
	afx_msg void OnPartybump();
	afx_msg void OnPartystep();
	virtual BOOL OnInitDialog();
	afx_msg void OnCharmiss();
	afx_msg void OnCharhit();
	afx_msg void OnSoundtest();
	afx_msg void OnSoundtestqueued();
	afx_msg void OnSoundteststop();
	afx_msg void OnStopsimulsounds();
	afx_msg void OnSimulmidi();
	afx_msg void OnStopbgndqueue();
	afx_msg void OnCampmusic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLOBALSNDCHOOSER_H__06CAAEF8_8C7C_11D3_88D6_00105AA4563A__INCLUDED_)
