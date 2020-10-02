#if !defined(AFX_SPECIALKEYBLOCKAGE_H__46DC58CD_62C9_4C5D_9297_1E7DF2EC7649__INCLUDED_)
#define AFX_SPECIALKEYBLOCKAGE_H__46DC58CD_62C9_4C5D_9297_1E7DF2EC7649__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpecialKeyBlockage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpecialKeyBlockage dialog

class CSpecialKeyBlockage : public CDialog
{
// Construction
public:
	CSpecialKeyBlockage(BLOCKAGE_KEYS &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(BLOCKAGE_KEYS &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CSpecialKeyBlockage)
	enum { IDD = IDD_SPECIALKEYBLOCKAGE };
	CString	m_KeyName1;
	CString	m_KeyName2;
	CString	m_KeyName3;
	CString	m_KeyName4;
	CString	m_KeyName5;
	CString	m_KeyName6;
	CString	m_KeyName7;
	CString	m_KeyName8;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecialKeyBlockage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  BLOCKAGE_KEYS m_data;
	// Generated message map functions
	//{{AFX_MSG(CSpecialKeyBlockage)
	afx_msg void OnPickkey1();
	afx_msg void OnPickkey2();
	afx_msg void OnPickkey3();
	afx_msg void OnPickkey4();
	afx_msg void OnPickkey5();
	afx_msg void OnPickkey6();
	afx_msg void OnPickkey7();
	afx_msg void OnPickkey8();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECIALKEYBLOCKAGE_H__46DC58CD_62C9_4C5D_9297_1E7DF2EC7649__INCLUDED_)
