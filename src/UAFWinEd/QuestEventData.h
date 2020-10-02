/******************************************************************************
* Filename: QuestEventData.h
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
#if !defined(AFX_QUESTEVENTDATA_H__A7A7FB53_887F_11D3_88C8_00105AA4563A__INCLUDED_)
#define AFX_QUESTEVENTDATA_H__A7A7FB53_887F_11D3_88C8_00105AA4563A__INCLUDED_

#include "GameEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CQuestEventData dialog

class CQuestEventData : public CDialog
{
// Construction
public:
	CQuestEventData(QUEST_EVENT_DATA data, CWnd* pParent = NULL);   // standard constructor
  void GetData(QUEST_EVENT_DATA& data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CQuestEventData)
	enum { IDD = IDD_QUESTDLG };
	CComboBox	m_Accept;
	CButton	m_Quest;
	CButton	m_SeeArt;
	BOOL	m_CompleteOnAccept;
	BOOL	m_FailOnReject;
	int		m_Stage;
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuestEventData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  QUEST_EVENT_DATA m_data;

	// Generated message map functions
	//{{AFX_MSG(CQuestEventData)
	virtual BOOL OnInitDialog();
	afx_msg void OnArt();
	afx_msg void OnQuestselect();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUESTEVENTDATA_H__A7A7FB53_887F_11D3_88C8_00105AA4563A__INCLUDED_)
