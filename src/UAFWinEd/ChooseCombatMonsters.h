#if !defined(AFX_CHOOSECOMBATMONSTERS_H__7C3F17A6_5E6E_4F7C_B9EC_B743285EBB04__INCLUDED_)
#define AFX_CHOOSECOMBATMONSTERS_H__7C3F17A6_5E6E_4F7C_B9EC_B743285EBB04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseCombatMonsters.h : header file
//
#include "GameEvent.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CChooseCombatMonsters dialog

class CChooseCombatMonsters : public CDialog
{
// Construction
public:
	CChooseCombatMonsters(MONSTER_EVENT_DATA &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(MONSTER_EVENT_DATA &data) { data = m_data; }

// Dialog Data
	//{{AFX_DATA(CChooseCombatMonsters)
	enum { IDD = IDD_CHOOSECOMBATMONSTER };
	CSortableListCtrl	m_ChosenList;
	CSortableListCtrl	m_AvailList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseCombatMonsters)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  MONSTER_EVENT_DATA m_data;

  void AddToAvailList(int i);
  void AddToChosenList(int i);
  void UpdateChosenList(int item, int i);
  void AddNPCToAvailList();

	// Generated message map functions
	//{{AFX_MSG(CChooseCombatMonsters)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnDblclkAvaillist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkChosenlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSECOMBATMONSTERS_H__7C3F17A6_5E6E_4F7C_B9EC_B743285EBB04__INCLUDED_)
