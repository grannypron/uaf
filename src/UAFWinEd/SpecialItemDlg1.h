/******************************************************************************
* Filename: SpecialItemDlg1.h
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
#if !defined(AFX_SPECIALITEMDLG1_H__084E2954_1836_11D3_8873_00105AA4563A__INCLUDED_)
#define AFX_SPECIALITEMDLG1_H__084E2954_1836_11D3_8873_00105AA4563A__INCLUDED_



#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSpecialItemDlg dialog

class CSpecialItemDlg : public CDialog
{

// Construction
public:
	CSpecialItemDlg(SPECIAL_ITEM_KEY_EVENT_DATA& data, CWnd* pParent = NULL);   // standard constructor
  void GetData(SPECIAL_ITEM_KEY_EVENT_DATA &data);

// Dialog Data
	//{{AFX_DATA(CSpecialItemDlg)
	enum { IDD = IDD_SPECIALITEMDLG };
	CSortableListCtrl	m_GiveList;
	CSortableListCtrl	m_TakeList;
	CButton	m_ChooseArt;
	CString	m_Text;
	BOOL	m_ForceBackup;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecialItemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  SPECIAL_ITEM_KEY_EVENT_DATA m_data;

  void AddItemToGiveList(int idx, int eventIdx);
  void AddItemToTakeList(int idx, int eventIdx);
  void AddKeyToGiveList(int idx, int eventIdx);
  void AddKeyToTakeList(int idx, int eventIdx);

  BOOL  IsObjectInList(int index, BYTE type);

	// Generated message map functions
	//{{AFX_MSG(CSpecialItemDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseart();
	afx_msg void OnRemoveFromGiveList();
	afx_msg void OnRemoveFromTakeList();
	virtual void OnOK();
	afx_msg void OnAddToGiveList();
	afx_msg void OnAddToTakeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECIALITEMDLG1_H__084E2954_1836_11D3_8873_00105AA4563A__INCLUDED_)
