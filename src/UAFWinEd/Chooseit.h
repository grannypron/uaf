/******************************************************************************
* Filename: Chooseit.h
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

#include "items.h"
/////////////////////////////////////////////////////////////////////////////
// CChooseItems dialog

class CChooseItems : public CDialog
{
// Construction
public:
	CChooseItems(ITEM_LIST& items, CWnd* pParent = NULL);   // standard constructor
	void GetItemData(ITEM_LIST& items);
	int GetItemIndex(CString& text);

// Dialog Data
	//{{AFX_DATA(CChooseItems)
	enum { IDD = IDD_CHOOSEITEMS };
	CListBox	m_ItemList;
	CListBox	m_Item8;
	CListBox	m_Item7;
	CListBox	m_Item6;
	CListBox	m_Item5;
	CListBox	m_Item4;
	CListBox	m_Item3;
	CListBox	m_Item2;
	CListBox	m_Item1;
	CButton	m_DeleteItem;
	CButton	m_AddItem;
	BOOL	m_Id1;
	BOOL	m_Id2;
	BOOL	m_Id3;
	BOOL	m_Id4;
	BOOL	m_Id5;
	BOOL	m_Id6;
	BOOL	m_Id7;
	BOOL	m_Id8;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseItems)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ITEM_LIST m_items;

	// Generated message map functions
	//{{AFX_MSG(CChooseItems)
	afx_msg void OnAdditem();
	afx_msg void OnDeleteitem();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeItem1();
	afx_msg void OnSelchangeItem2();
	afx_msg void OnSelchangeItem3();
	afx_msg void OnSelchangeItem4();
	afx_msg void OnSelchangeItem5();
	afx_msg void OnSelchangeItem6();
	afx_msg void OnSelchangeItem7();
	afx_msg void OnSelchangeItem8();
	afx_msg void OnSetfocusItem1();
	afx_msg void OnSetfocusItem2();
	afx_msg void OnSetfocusItem3();
	afx_msg void OnSetfocusItem4();
	afx_msg void OnSetfocusItem5();
	afx_msg void OnSetfocusItem6();
	afx_msg void OnSetfocusItem7();
	afx_msg void OnSetfocusItem8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
