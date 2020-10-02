#if !defined(AFX_CHOOSEOPTIONS_H__7C3F17A6_5E6E_4F7C_B9EC_B743285EBB04__INCLUDED_)
#define AFX_CHOOSEOPTIONS_H__7C3F17A6_5E6E_4F7C_B9EC_B743285EBB04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseOptions.h : header file
//
#include "GameEvent.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CChooseOptions dialog

class CChooseOptions : public CDialog
{
// Construction
public:
	CChooseOptions(      SUBSTRINGS *pSrc, 
                 const CString& suggestionKey, 
                       CArray<CStringPAIR, CStringPAIR&> *pSuggestionList,
                 const CString& helpKey,
                 CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CChooseOptions)
	enum { IDD = IDD_CHOOSEOPTIONS };
	CSortableListCtrl	m_ChosenList;
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
  CString m_suggestionKey;
  CArray<CStringPAIR, CStringPAIR&> *m_pSuggestionList;
  CString m_helpKey;
  CArray<CStringPAIR, CStringPAIR&> m_chosen;
  SUBSTRINGS m_result;
  CArray<CStringPAIR, CStringPAIR&> m_all;  // Including selected items.

  void AddToAvailList(int i);
  void AddToChosenList(int i);
  void DeleteChosen(int index);
  void UpdateChosenList(DWORD i);
  int  AvailIndex(const CString& name, const CString& value);
  int  ChosenIndex(const CString& name, const CString& value);
  void RebuildAvailList(void);
  void SUBSTRINGS2CArray(const SUBSTRINGS *pSrc, CArray<CStringPAIR, CStringPAIR&> *pDst);

	// Generated message map functions
	//{{AFX_MSG(CChooseOptions)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnHelp();
  afx_msg void OnNew(void);
	afx_msg void OnDelete();
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSEOPTIONS_H__7C3F17A6_5E6E_4F7C_B9EC_B743285EBB04__INCLUDED_)
