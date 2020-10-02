/******************************************************************************
* Filename: ItemEditor.h
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
#if !defined(AFX_ITEMEDITOR_H__86EC0763_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_)
#define AFX_ITEMEDITOR_H__86EC0763_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_


#include "items.h"
#include "SortableListCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CItemEditor dialog

class CItemEditor : public CDialog
{
// Construction
public:
	CItemEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CItemEditor)
	enum { IDD = IDD_ITEMEDITOR };
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA

  ITEM_DATA_TYPE *m_pItemData;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void AddToAvailList(int itemIndex);
  void UpdateAvailList(int item, ITEM_DATA &data);

	// Generated message map functions
	//{{AFX_MSG(CItemEditor)
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdditem();
	afx_msg void OnDeleteitem();
	afx_msg void OnEdititem();
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	afx_msg void OnPastelevel();
	afx_msg void OnCopylevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMEDITOR_H__86EC0763_BE9B_11D3_BF9A_00A0CC25685D__INCLUDED_)
