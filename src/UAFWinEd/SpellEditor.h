/******************************************************************************
* Filename: SpellEditor.h
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
#if !defined(AFX_SPELLEDITOR_H__86EC0762_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_)
#define AFX_SPELLEDITOR_H__86EC0762_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_

#include "items.h"
#include "char.h"
#include "spell.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellEditor dialog

class CSpellEditor : public CDialog
{
// Construction
public:
	CSpellEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpellEditor)
	enum { IDD = IDD_SPELLEDITOR };
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA

  SPELL_DATA_TYPE *m_pSpellData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void AddToAvailList(int spellIndex);
  void UpdateAvailList(int item, SPELL_DATA &data);

  //GLOBAL_SPELL_ID m_CopyLevel;
  SPELL_ID m_CopySpellID;

	// Generated message map functions
	//{{AFX_MSG(CSpellEditor)
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddspell();
	afx_msg void OnDeletespell();
	afx_msg void OnEditspell();
	virtual BOOL OnInitDialog();
	afx_msg void OnCopylevel();
	afx_msg void OnPastelevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPELLEDITOR_H__86EC0762_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_)
