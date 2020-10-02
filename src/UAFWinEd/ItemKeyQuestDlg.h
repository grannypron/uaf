/******************************************************************************
* Filename: ItemKeyQuestDlg.h
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
#if !defined(AFX_ITEMKEYQUESTDLG_H__15A93C76_86FB_11D3_88C7_00105AA4563A__INCLUDED_)
#define AFX_ITEMKEYQUESTDLG_H__15A93C76_86FB_11D3_88C7_00105AA4563A__INCLUDED_



#include "externs.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CItemKeyQuestDlg dialog

class CItemKeyQuestDlg : public CDialog
{
// Construction
public:
	CItemKeyQuestDlg(BYTE IncludeMask=0xFF, 
                   GLOBAL_STATS *data=NULL,   // standard constructor
                   CWnd* pParent = NULL);
  void GetData(GLOBAL_STATS *data);
  int m_selectedID;
  QUEST_TYPE m_type;

// Dialog Data
	//{{AFX_DATA(CItemKeyQuestDlg)
	enum { IDD = IDD_ITEMKEYQUESTDLG };
	CButton	m_Edit;
	CButton	m_Add;
	CButton	m_Delete;
	CSortableListCtrl	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemKeyQuestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  BYTE m_Mask;

   SPECIAL_OBJECT_LIST m_keyData;
   SPECIAL_OBJECT_LIST m_specialItemData;
   QUEST_LIST          m_questData;

   GLOBAL_STATS *m_pdata;
  
  void AddQuestToList(int idx);
  void AddSpecialKeyToList(int idx);
  void AddSpecialItemToList(int idx);

	// Generated message map functions
	//{{AFX_MSG(CItemKeyQuestDlg)
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	virtual void OnOK();
	afx_msg void OnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMKEYQUESTDLG_H__15A93C76_86FB_11D3_88C7_00105AA4563A__INCLUDED_)
