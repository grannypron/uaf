#if !defined(AFX_BASEDIALOG_H__A6AE4F31_1C29_11D2_BC27_0060970A2B51__INCLUDED_)
#define AFX_BASEDIALOG_H__A6AE4F31_1C29_11D2_BC27_0060970A2B51__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BaseDialog.h : header file
/////////////////////////////////////////////////////////////////////////////
//
// Author:	if (it works)
//				Author = _T("Patty You");
//			else
//				Author = _T("IUnknown");
// Date:	07/16/98
// email:   ptn@worldnet.att.net
/////////////////////////////////////////////////////////////////////////////
// CBaseDialog window

class CBaseDialog : public CDialog
{
	DECLARE_DYNAMIC(CBaseDialog)

// Construction
public:
	CBaseDialog();
	CBaseDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseDialog)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
protected:
	virtual void OnOK();
	virtual void OnCancel();

// Implementation
public:
	void SetBackgroundColor(COLORREF crBackground);
	void SetModelessFlag(bool bIsModeless);
	virtual ~CBaseDialog();

	// Generated message map functions
protected:
	COLORREF m_crBackground;
	bool m_bIsModeless;
	CBrush m_wndbkBrush;	// background brush

	//{{AFX_MSG(CBaseDialog)
	afx_msg void OnNcDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam); // works for modal dialog
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASEDIALOG_H__A6AE4F31_1C29_11D2_BC27_0060970A2B51__INCLUDED_)
