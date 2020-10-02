/******************************************************************************
* Filename: ChooseSpeclAbDlg.h
* Copyright (c) 2000, UAF Development Team (email CocoaSpud@hotmail.com)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/
#include "specab.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CChooseSpeclAbDlg dialog

class CChooseSpeclAbDlg : public CDialog
{
// Construction
public:
#ifdef OLD_SPECIAL_ABILITIES
	CChooseSpeclAbDlg(const SPECIAL_ABILITIES &sa, const CString& title, CWnd* pParent = NULL);   // standard constructor
  void GetData(SPECIAL_ABILITIES &sa) { sa.Copy(m_sa); }
  BOOL m_ReadOnly; // disable MsgTypes and Script edits
  BOOL m_NoMsgTypes; // disable only MsgTypes
#else
	CChooseSpeclAbDlg(const A_SPECABILITY_DEF_L &saList, const CString& title, CWnd* pParent = NULL);   // standard constructor
  bool GetData(A_SPECABILITY_DEF_L &saList); // Has data changed?
#endif

// Dialog Data
	//{{AFX_DATA(CChooseSpeclAbDlg)
	enum { IDD = IDD_CHOOSESPECIALABILITIES };
	CSortableListCtrl	m_SpecAbList;
	CComboBox	m_SpecAbCombo;
  CButton m_script;
  CButton m_constant;
  CButton m_variable;
  CButton m_include;
  CButton m_integerTable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseSpeclAbDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  BOOL AddSpecialAbilityToList(const ASLENTRY *pEntry);
  void FillListBox(void);
  void RebuildDialogBox(void);
  void UpdateSpecialAbilityList(int index, specialAbilitiesType sa);
  void ProcessStringSelection(void);
#ifdef OLD_SPECIAL_ABILITIES
  SPECIAL_ABILITIES m_sa;
  int m_ScriptCol;
  int m_MsgCol;
#else
  int m_listSelection;
  ASLENTRY        *FindCurrentlySelectedString(void);
  SPECABILITY_DEF *FindCurrentlySelectedAbility(void);
  void SetStringType(SPECAB_STRING_TYPE type);
  void UpdateStringType(SPECAB_STRING_TYPE type);
  A_SPECABILITY_DEF_L m_saList;
  CString m_newAbilityName;
#endif
  CString m_title;

	// Generated message map functions
	//{{AFX_MSG(CChooseSpeclAbDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAddspecab();
  afx_msg void OnAdd();
	afx_msg void OnDeleteLine();
	afx_msg void OnDeleteSpecAbility();
	afx_msg void OnDuplicateSpecAbility();
	afx_msg void OnRenameSpecAbility();
	afx_msg void OnEditspecab();
	afx_msg void OnDblclkSpecablist(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnClkSpecablist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditmsgtypes();
  afx_msg void OnSelchangeAbility();
  afx_msg void OnClickScript();
  afx_msg void OnClickConstant();
  afx_msg void OnClickVariable();
  afx_msg void OnClickInclude();
  afx_msg void OnClickIntegerTable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
