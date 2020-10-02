#if !defined(AFX_TITLESCREENDATA_H__C1CB8D04_3001_4D4B_B15B_6C9B12476B9D__INCLUDED_)
#define AFX_TITLESCREENDATA_H__C1CB8D04_3001_4D4B_B15B_6C9B12476B9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/******************************************************************************
* Filename: TitleScreenData.h
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
#include "GlobalData.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTitleScreenData dialog

class CTitleScreenData : public CDialog
{
// Construction
public:
	CTitleScreenData(TITLE_SCREEN_DATA &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(TITLE_SCREEN_DATA &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CTitleScreenData)
	enum { IDD = IDD_TITLESCREENDATA };
	CSortableListCtrl	m_AvailList;
	DWORD	m_Delay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleScreenData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  TITLE_SCREEN_DATA m_data;

  void AddToAvailList(TITLE_SCREEN &data, POSITION pos);
  void UpdateAvailList(int item, TITLE_SCREEN &data, POSITION pos);

	// Generated message map functions
	//{{AFX_MSG(CTitleScreenData)
	afx_msg void OnAddtitle();
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeletetitle();
	afx_msg void OnEdittitle();
	virtual BOOL OnInitDialog();
	afx_msg void OnMovetitledown();
	afx_msg void OnMovetitleup();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLESCREENDATA_H__C1CB8D04_3001_4D4B_B15B_6C9B12476B9D__INCLUDED_)
