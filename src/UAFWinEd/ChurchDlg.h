/******************************************************************************
* Filename: ChurchDlg.h
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
// CChurch dialog
#include "GameEvent.h"

class CChurch : public CDialog
{
// Construction
public:
	CChurch(TEMPLE& data, CWnd* pParent = NULL);   // standard constructor
	void GetTempleData(TEMPLE& data);

// Dialog Data
	//{{AFX_DATA(CChurch)
	enum { IDD = IDD_CHURCH };
	CComboBox	m_MaxLevel;
	CComboBox	m_CostFactor;
	CButton	m_Cleric;
	BOOL	m_ForceExit;
	CString	m_EntryText;
	BOOL	m_AllowDonations;
	int		m_DonateTrigger;
	CString	m_Text;
	CString	m_CoinName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChurch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	TEMPLE m_data;

	// Generated message map functions
	//{{AFX_MSG(CChurch)
	afx_msg void OnCleric();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSpelllist();
	afx_msg void OnInitSpells();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
