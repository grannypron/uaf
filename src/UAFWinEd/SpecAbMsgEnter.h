#if !defined(AFX_SPECABMSGENTER_H__CFC57CC8_4996_4583_8115_E9F6FCFD4977__INCLUDED_)
#define AFX_SPECABMSGENTER_H__CFC57CC8_4996_4583_8115_E9F6FCFD4977__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecAbMsgEnter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpecAbMsgEnter dialog

class CSpecAbMsgEnter : public CDialog
{
// Construction
public:
	CSpecAbMsgEnter(CString msg, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpecAbMsgEnter)
	enum { IDD = IDD_GETSPECABMSG };
	CString	m_Msg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecAbMsgEnter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecAbMsgEnter)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECABMSGENTER_H__CFC57CC8_4996_4583_8115_E9F6FCFD4977__INCLUDED_)
