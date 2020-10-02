#if !defined(AFX_GAMERESCHANGE_H__FA4B07D7_C9FE_46D9_8AC3_13CBE53B12DE__INCLUDED_)
#define AFX_GAMERESCHANGE_H__FA4B07D7_C9FE_46D9_8AC3_13CBE53B12DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GameResChange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGameResChange dialog

class CGameResChange : public CDialog
{
// Construction
public:
	CGameResChange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGameResChange)
	enum { IDD = IDD_RESCHANGE_DIALOG };
	CString	m_640Config;
	CString	m_800Config;
	CString	m_1024Config;
	int		m_ChosenRes;
	CString	m_ResText;
	CString	m_InstallText;
	CString	m_DesignPath;
	BOOL	m_UpdateDesign;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGameResChange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGameResChange)
	afx_msg void OnUpdategameres();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GAMERESCHANGE_H__FA4B07D7_C9FE_46D9_8AC3_13CBE53B12DE__INCLUDED_)
