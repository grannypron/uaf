#if !defined(AFX_ACCESSDLG_H__38B40500_4EC6_11D4_A03D_DEDD8D6DD431__INCLUDED_)
#define AFX_ACCESSDLG_H__38B40500_4EC6_11D4_A03D_DEDD8D6DD431__INCLUDED_


void My_free(void *addr);


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccessDlg.h : header file
//


class LEVEL_ACCESS
{
private:
  char *m_access; // 0 thru 7 for three different access types
  char *m_stack;  // Only while computing access
  int   m_size;
  void ComputeOneAccess(int x, int y, int mask, CString types);
public:
  LEVEL_ACCESS(void) { m_access=NULL; m_size=0;m_stack=NULL; };
  ~LEVEL_ACCESS(void) {if (m_access!=NULL) My_free(m_access);if (m_stack!=NULL) free(m_stack);};
  void ComputeAccess(int x, int y, const CString& aTypes);
  int GetAccess(int x, int y);
};


extern LEVEL_ACCESS levelAccess;


/////////////////////////////////////////////////////////////////////////////
// CAccessDlg dialog

class CAccessDlg : public CDialog
{
// Construction
public:
	CAccessDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAccessDlg)
	enum { IDD = IDD_ACCESS };
	CColorBox	m_Red;
	CColorBox	m_Green;
	CColorBox	m_Blue;
	//}}AFX_DATA

  BOOL  m_Check[48];
  CString m_accessParameters;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccessDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAccessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckBox();
	afx_msg void OnDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_ACCESSDLG_H__38B40500_4EC6_11D4_A03D_DEDD8D6DD431__INCLUDED_)
