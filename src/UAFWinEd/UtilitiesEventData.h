/******************************************************************************
* Filename: UtilitiesEventData.h
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
#if !defined(AFX_UTILITIESEVENTDATA_H__901861E6_8ADB_11D3_88C9_00105AA4563A__INCLUDED_)
#define AFX_UTILITIESEVENTDATA_H__901861E6_8ADB_11D3_88C9_00105AA4563A__INCLUDED_


#include "GameEvent.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesEventData dialog

class CUtilitiesEventData : public CDialog
{
// Construction
public:
	CUtilitiesEventData(UTILITIES_EVENT_DATA &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(UTILITIES_EVENT_DATA &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CUtilitiesEventData)
	enum { IDD = IDD_UTILITIESEVENTDLG };
	CComboBox	m_VarOperation;
	CButton	m_ResultSelect;
	CButton	m_ObjectSelect;
	CSortableListCtrl	m_ItemList;
	CComboBox	m_ItemCheck;
	BOOL	m_EndPlay;
	int		m_Qty;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUtilitiesEventData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  UTILITIES_EVENT_DATA m_data;

  void AddSpecialItemToList(int idx, int eventIdx);
  void AddSpecialKeyToList(int idx, int eventIdx);
  void AddQuestToList(int idx, int eventIdx);

  BOOL IsObjectInList(int index, BYTE type);

	// Generated message map functions
	//{{AFX_MSG(CUtilitiesEventData)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnResultselect();
	afx_msg void OnObjectselect();
	afx_msg void OnAdditem();
	afx_msg void OnDeleteitem();
  afx_msg void OnUtilitiesEventQuestion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UTILITIESEVENTDATA_H__901861E6_8ADB_11D3_88C9_00105AA4563A__INCLUDED_)
