/******************************************************************************
* Filename: MonsterEditor.h
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
#if !defined(AFX_MONSTEREDITOR_H__86EC0764_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_)
#define AFX_MONSTEREDITOR_H__86EC0764_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_

#include "monster.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMonsterEditor dialog

class CMonsterEditor : public CDialog
{
// Construction
public:
	CMonsterEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMonsterEditor)
	enum { IDD = IDD_MONSTEREDITOR };
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA

  MONSTER_DATA_TYPE *m_pMonsterData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonsterEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void AddToAvailList(int monsterIndex);
  void UpdateAvailList(int item, MONSTER_DATA &data);

	// Generated message map functions
	//{{AFX_MSG(CMonsterEditor)
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdditem();
	afx_msg void OnDeleteitem();
	afx_msg void OnEdititem();
	virtual BOOL OnInitDialog();
	afx_msg void OnCopylevel();
	afx_msg void OnPastelevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONSTEREDITOR_H__86EC0764_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_)
