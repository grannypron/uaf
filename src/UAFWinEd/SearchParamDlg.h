#if !defined(AFX_SEARCHPARAMDLG_H__97D65B13_322B_43BF_877D_368C41A6289B__INCLUDED_)
#define AFX_SEARCHPARAMDLG_H__97D65B13_322B_43BF_877D_368C41A6289B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SearchParamDlg.h : header file
//
//#include <afxwin.h>
#include "resource.h"
#include "BaseDialog.h"

// optional search flags
#define ENABLE_EVENT_SRCH_FLAGS 0x0001

// windows message notification
#define MSG_FINDNEXT 0xB420

// search modifiers
#define SEARCH_MATCH_CASE       0x0001
#define SEARCH_MATCH_ENTIRE     0x0002
#define SEARCH_MATCH_EVENT_TEXT 0x0004
#define SEARCH_MATCH_EVENT_ID   0x0008

// easy flag checks
inline BOOL MatchCaseInSearch(WORD type) { return (type & SEARCH_MATCH_CASE); }
inline BOOL MatchEntireInSearch(WORD type) { return (type & SEARCH_MATCH_ENTIRE); }
inline BOOL MatchEventTextInSearch(WORD type) { return (type & SEARCH_MATCH_EVENT_TEXT); }
inline BOOL MatchEventIdInSearch(WORD type) { return (type & SEARCH_MATCH_EVENT_ID); }

typedef void (*SEARCHPROC)(CString value, WORD type, DWORD userdata);


/////////////////////////////////////////////////////////////////////////////
// CSearchParamDlg dialog

class CSearchParamDlg : public CBaseDialog
{
// Construction
public:
	CSearchParamDlg(CWnd* pParent = NULL);   // standard constructor

  void SetSearchCallback(SEARCHPROC pFunc, DWORD UserData);
  void ClearSearchCallback();

  bool m_ShowEventFlags;

  // Dialog Data
	//{{AFX_DATA(CSearchParamDlg)
	enum { IDD = IDD_SEARCHPARAMS };
	CString	m_SearchText;
	BOOL	m_SearchEventText;
	BOOL	m_MatchWholeString;
	BOOL	m_MatchCase;
	BOOL	m_SearchEventId;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  
  SEARCHPROC m_pSearchFunc;
  DWORD m_UserData;

	// Generated message map functions
	//{{AFX_MSG(CSearchParamDlg)
	afx_msg void OnFind();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


CSearchParamDlg *GetSearchDlg(WORD flags=0);
void DestroySearchDlg();


#endif // !defined(AFX_SEARCHPARAMDLG_H__97D65B13_322B_43BF_877D_368C41A6289B__INCLUDED_)
