/******************************************************************************
* Filename: ChooseMonster.h
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
#if !defined(AFX_CHOOSEMONSTER_H__47D96C93_C77B_11D3_88FA_00105AA4563A__INCLUDED_)
#define AFX_CHOOSEMONSTER_H__47D96C93_C77B_11D3_88FA_00105AA4563A__INCLUDED_

#include "Monster.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CChooseMonster dialog

class CChooseMonster : public CDialog
{
// Construction
public:
	//CChooseMonster(int MonsterNum, int max=1, CWnd* pParent = NULL);   // standard constructor
	CChooseMonster(const MONSTER_ID& monsterID, int max=1, CWnd* pParent = NULL);   // standard constructor
  //int m_MonsterNum;
  MONSTER_ID m_monsterID;
  int m_max;

// Dialog Data
	//{{AFX_DATA(CChooseMonster)
	enum { IDD = IDD_CHOOSEMONSTER };
	CSortableListCtrl	m_ChosenList;
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseMonster)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  //void AddToChosenList(int index);
  void AddToChosenList(const MONSTER_ID& monsterID);
  //void AddToAvailList(MONSTER_DATA &data);
  void AddToAvailList(int i);

	// Generated message map functions
	//{{AFX_MSG(CChooseMonster)
	afx_msg void OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSEMONSTER_H__47D96C93_C77B_11D3_88FA_00105AA4563A__INCLUDED_)
