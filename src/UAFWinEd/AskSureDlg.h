#if !defined(AFX_ASKSUREDLG_H__9ACBAC41_57C5_11D4_A03D_85A5E928090B__INCLUDED_)
#define AFX_ASKSUREDLG_H__9ACBAC41_57C5_11D4_A03D_85A5E928090B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AskSureDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AskSureDlg dialog

class AskSureDlg : public CDialog
{
// Construction
public:
	AskSureDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AskSureDlg)
	enum { IDD = IDD_ASKSURE };
	BOOL	m_dontAsk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AskSureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AskSureDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASKSUREDLG_H__9ACBAC41_57C5_11D4_A03D_85A5E928090B__INCLUDED_)
