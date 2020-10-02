#if !defined(AFX_COMBATMONSTERDETAILS_H__1148AFF7_7E7C_4B2C_B1FF_EFD4922B99BA__INCLUDED_)
#define AFX_COMBATMONSTERDETAILS_H__1148AFF7_7E7C_4B2C_B1FF_EFD4922B99BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CombatMonsterDetails.h : header file
//

#include "GameEvent.h"
/////////////////////////////////////////////////////////////////////////////
// CCombatMonsterDetails dialog

class CCombatMonsterDetails : public CDialog
{
// Construction
public:
	CCombatMonsterDetails(CString name, MONSTER_EVENT &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(MONSTER_EVENT &data) { data = m_data; }
  
// Dialog Data
	//{{AFX_DATA(CCombatMonsterDetails)
	enum { IDD = IDD_COMBATMONSTERDATA };
	BOOL	m_Friendly;
	int		m_Morale;
	int		m_Qty;
	CString	m_Name;
	int		m_QuantityType;
	int		m_DiceSides;
	int		m_DiceBonus;
	int		m_NbrDice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCombatMonsterDetails)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  MONSTER_EVENT m_data;
  CString m_name;

	// Generated message map functions
	//{{AFX_MSG(CCombatMonsterDetails)
	afx_msg void OnItems1();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnMonstermoney();
	afx_msg void OnQuantitytype();
	afx_msg void OnQuantitytype2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBATMONSTERDETAILS_H__1148AFF7_7E7C_4B2C_B1FF_EFD4922B99BA__INCLUDED_)
