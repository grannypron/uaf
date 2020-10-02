/******************************************************************************
* Filename: StepEventDlg.h
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
#if !defined(AFX_STEPEVENTDLG_H__872F6AD2_78D3_11D2_AC18_00C04F8E081C__INCLUDED_)
#define AFX_STEPEVENTDLG_H__872F6AD2_78D3_11D2_AC18_00C04F8E081C__INCLUDED_
#include "GameEvent.h"
#include "GlobalData.h"
#include "Level.h"

/////////////////////////////////////////////////////////////////////////////
// CStepEventDlg dialog

class CStepEventDlg : public CDialog
{
// Construction
public:
	CStepEventDlg(LEVEL &data, int curr = 0, CWnd* pParent = NULL);   // standard constructor
   void GetData(LEVEL &data);

// Dialog Data
	//{{AFX_DATA(CStepEventDlg)
	enum { IDD = IDD_STEPEVENTS };
	CSpinButtonCtrl	m_StepEventSpin;
	int		m_StepCount;
	int		m_StepEventNum;
	CString	m_StepEventName;
	BOOL	m_Zone1;
	BOOL	m_Zone2;
	BOOL	m_Zone3;
	BOOL	m_Zone4;
	BOOL	m_Zone5;
	BOOL	m_Zone6;
	BOOL	m_Zone7;
	BOOL	m_Zone8;
	BOOL	m_Zone10;
	BOOL	m_Zone11;
	BOOL	m_Zone12;
	BOOL	m_Zone13;
	BOOL	m_Zone14;
	BOOL	m_Zone15;
	BOOL	m_Zone16;
	BOOL	m_Zone9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStepEventDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   STEP_EVENT_DATA m_StepEvents[MAX_STEP_EVENTS];
	// Generated message map functions
	//{{AFX_MSG(CStepEventDlg)
	afx_msg void OnDeltaposStepeventspin(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEPEVENTDLG_H__872F6AD2_78D3_11D2_AC18_00C04F8E081C__INCLUDED_)
