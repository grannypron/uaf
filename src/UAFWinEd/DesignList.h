/******************************************************************************
* Filename: DesignList.h
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
// CDesignList dialog
#include "SortableListCtrl.h"

class CDesignList : public CDialog
{
// Construction
public:
	CDesignList(CString name, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDesignList)
	enum { IDD = IDD_DESIGNLIST };
	CSortableListCtrl	m_List;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesignList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void AddToAvailList(CString FolderName, CString DesignName, double version);
  void GetDesignVersion(const char *fname, double &ver, CString &name);

	// Generated message map functions
	//{{AFX_MSG(CDesignList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkDesignlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickDesignlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
