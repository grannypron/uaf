#if !defined(AFX_MONSTERATTACKSDLG_H__F6DB2394_E356_4D78_B8F4_6284AA892C6D__INCLUDED_)
#define AFX_MONSTERATTACKSDLG_H__F6DB2394_E356_4D78_B8F4_6284AA892C6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/******************************************************************************
* Filename: MonsterAttacksDlg.h
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

#include "SortableListCtrl.h"
#include "Monster.h"

/////////////////////////////////////////////////////////////////////////////
// CMonsterAttacksDlg dialog

class CMonsterAttacksDlg : public CDialog
{
// Construction
public:
	CMonsterAttacksDlg(ATTACK_DATA &data, CWnd* pParent = NULL);   // standard constructor
  //CMonsterAttacksDlg(MONSTER_ATTACK_DATA& data, CWnd* pParent = NULL);   // standard constructor
  //void GetData(MONSTER_ATTACK_DATA &data) { data=m_data; }
  void GetData(ATTACK_DATA& data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CMonsterAttacksDlg)
	enum { IDD = IDD_MONSTERATTACKDLG };
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonsterAttacksDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  //void AddToAvailList(int index, MONSTER_ATTACK_DETAILS &data);
  void AddToAvailList(int index, ATTACK_DETAILS& data);
  //void UpdateAvailList(int item, MONSTER_ATTACK_DETAILS &data);
  void UpdateAvailList(int item, ATTACK_DETAILS& data);

  //MONSTER_ATTACK_DATA m_data;
  ATTACK_DATA m_data;

	// Generated message map functions
	//{{AFX_MSG(CMonsterAttacksDlg)
	afx_msg void OnAdditem();
	afx_msg void OnDeleteitem();
	afx_msg void OnEdititem();
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONSTERATTACKSDLG_H__F6DB2394_E356_4D78_B8F4_6284AA892C6D__INCLUDED_)
