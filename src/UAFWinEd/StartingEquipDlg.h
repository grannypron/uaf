#if !defined(AFX_STARTINGEQUIPDLG_H__E945B04A_B4B8_4CD5_BF3F_5220D1E77961__INCLUDED_)
#define AFX_STARTINGEQUIPDLG_H__E945B04A_B4B8_4CD5_BF3F_5220D1E77961__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StartingEquipDlg.h : header file
//
#include "GlobalData.h"

/////////////////////////////////////////////////////////////////////////////
// CStartingEquipDlg dialog

class CStartingEquipDlg : public CDialog
{
// Construction
public:
	CStartingEquipDlg(GLOBAL_STATS *data, CWnd* pParent = NULL);   // standard constructor
  //void GetData(GLOBAL_STATS &data) { data = m_data; }
// Dialog Data
	//{{AFX_DATA(CStartingEquipDlg)
	enum { IDD = IDD_STARTINGEQUIPDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStartingEquipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  GLOBAL_STATS *m_data;

	// Generated message map functions
	//{{AFX_MSG(CStartingEquipDlg)
	afx_msg void OnBaseitems();
	afx_msg void OnClericitems();
	afx_msg void OnDruiditems();
	afx_msg void OnFighteritems();
	afx_msg void OnMuitems();
	afx_msg void OnPaladinitems();
	afx_msg void OnRangeritems();
	afx_msg void OnThiefitems();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STARTINGEQUIPDLG_H__E945B04A_B4B8_4CD5_BF3F_5220D1E77961__INCLUDED_)
