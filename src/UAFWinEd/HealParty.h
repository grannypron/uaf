#if !defined(AFX_HEALPARTY_H__905F3DB4_6D2D_11D4_9776_00A0CC5F87BE__INCLUDED_)
#define AFX_HEALPARTY_H__905F3DB4_6D2D_11D4_9776_00A0CC5F87BE__INCLUDED_
/******************************************************************************
* Filename: HealParty.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CHealParty dialog

class CHealParty : public CDialog
{
// Construction
public:
	CHealParty(HEAL_PARTY_DATA &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(HEAL_PARTY_DATA &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CHealParty)
	enum { IDD = IDD_HEALPARTY };
	CButton	m_SeeChar;
	CComboBox	m_HealWho;
	BOOL	m_HealCurse;
	BOOL	m_HealDrain;
	BOOL	m_HealHP;
	CString	m_Text;
	int		m_Chance;
	int		m_LiteralOrPercent;
	int		m_HowMuchHP;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHealParty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  HEAL_PARTY_DATA m_data;
  void UpdateControls();
	// Generated message map functions
	//{{AFX_MSG(CHealParty)
	virtual BOOL OnInitDialog();
	afx_msg void OnSeechar();
	afx_msg void OnSelchangeHealwho();
	virtual void OnOK();
	afx_msg void OnHealhp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEALPARTY_H__905F3DB4_6D2D_11D4_9776_00A0CC5F87BE__INCLUDED_)
