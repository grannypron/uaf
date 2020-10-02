#if !defined(AFX_SPECABMSGS_H__3EC079E6_2579_4460_BB6F_57FE40CE2EEA__INCLUDED_)
#define AFX_SPECABMSGS_H__3EC079E6_2579_4460_BB6F_57FE40CE2EEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecAbMsgs.h : header file
//
#include "specab.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSpecAbMsgs dialog

class CSpecAbMsgs : public CDialog
{
// Construction
public:
	CSpecAbMsgs(const SPECIAL_ABILITIES& list, specialAbilitiesType sa, CWnd* pParent = NULL);   // standard constructor
  void GetData(SPECIAL_ABILITIES &data) { data.Copy(m_list); }

// Dialog Data
	//{{AFX_DATA(CSpecAbMsgs)
	enum { IDD = IDD_SPECABMSGTYPES };
	CSortableListCtrl	m_SpecAbMsgList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecAbMsgs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  SPECIAL_ABILITIES m_list;
  specialAbilitiesType m_sa;

	// Generated message map functions
	//{{AFX_MSG(CSpecAbMsgs)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnEditmsg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECABMSGS_H__3EC079E6_2579_4460_BB6F_57FE40CE2EEA__INCLUDED_)
