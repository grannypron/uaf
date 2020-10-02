/******************************************************************************
* Filename: SelectLevel.h
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
#if !defined(AFX_SELECTLEVEL_H__9C7AE532_1C1B_11D2_ABF6_00C04F8E081C__INCLUDED_)
#define AFX_SELECTLEVEL_H__9C7AE532_1C1B_11D2_ABF6_00C04F8E081C__INCLUDED_

#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectLevel dialog

class CSelectLevel : public CDialog
{
// Construction
public:
	CSelectLevel(int level, CWnd* pParent = NULL);   // standard constructor
  ~CSelectLevel();

  int m_level;
  BOOL m_testMode;
  CString m_ovName;

// Dialog Data
	//{{AFX_DATA(CSelectLevel)
	enum { IDD = IDD_LEVELLIST };
	CSortableListCtrl	m_LevelList;
	BOOL	m_SaveWarn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectLevel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   int m_CopyLevel;
   int CheckCurrentLevel();
   void AddToAvailList(int level);
   void UpdateAvailList(int ListItem);
   void CheckForUnsavedLevel(bool quiet);

  // Generated message map functions
	//{{AFX_MSG(CSelectLevel)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnCopylevel();
	afx_msg void OnDeletelevel();
	afx_msg void OnClearlevel();
	afx_msg void OnDblclkLevellist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditprops();
	afx_msg void OnPastelevel();
	afx_msg void OnSavewarn();
	afx_msg void OnExportLevel();
	afx_msg void OnImportLevel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTLEVEL_H__9C7AE532_1C1B_11D2_ABF6_00C04F8E081C__INCLUDED_)
