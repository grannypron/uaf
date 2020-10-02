/******************************************************************************
* Filename: Itemdlg.h
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
// CItemDlg dialog
#include "items.h"
#include "SortableListCtrl.h"

class CItemDlg : public CDialog
{
// Construction
public:
	CItemDlg(DWORD MaxSelectable=0, CWnd* pParent = NULL);   // standard constructor
	CItemDlg(ITEM_LIST& list, DWORD MaxSelectable=0, CWnd* pParent = NULL);

	void GetItemList(ITEM_LIST& list);


// Dialog Data
	//{{AFX_DATA(CItemDlg)
	enum { IDD = IDD_ITEMS };
	CSortableListCtrl	m_ChosenList;
	CSortableListCtrl	m_AvailList;
	BOOL	m_AllItemsAreIdentified;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  void AddToChosenList(DWORD index, int itemCount, int identified);
  void AddToAvailList(ITEM_DATA &data, int index);
  //BOOL FindItemInList(GLOBAL_ITEM_ID giID);
  BOOL FindItemInList(ITEM_ID itemID);
  void UnselectAllAvailableItems();
  void UnselectAllChosenItems();

  DWORD m_max;
	ITEM_LIST m_list;

	// Generated message map functions
	//{{AFX_MSG(CItemDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnDeleteall();
	afx_msg void OnMarkIdentified();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
