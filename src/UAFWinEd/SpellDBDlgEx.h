#if !defined(AFX_SPELLDBDLGEX_H__BCE10191_E25C_4A42_99AC_7F1788BA7320__INCLUDED_)
#define AFX_SPELLDBDLGEX_H__BCE10191_E25C_4A42_99AC_7F1788BA7320__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/******************************************************************************
* Filename: SpellDBDlgEx.h
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

#include "Spell.h"
#include "SortableListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellDBDlgEx dialog

class CSpellDBDlgEx : public CDialog
{
// Construction
public:
	CSpellDBDlgEx(SPELL_DATA &data, CWnd* pParent = NULL);   // standard constructor

  SPELL_DATA m_SpellData;

// Dialog Data
	//{{AFX_DATA(CSpellDBDlgEx)
	enum { IDD = IDD_SPELLDBDLGEX };
	CButton	m_CoverageSound;
	CButton	m_InRouteSound;
	CButton	m_LingerSound;
	CButton	m_HitSound;
	CButton	m_CastAnim;
	CComboBox	m_CastTimeType;
	CComboBox	m_DurationRate;
	CComboBox	m_TargetingType;
	CButton	m_TargetCovAnim;
	CButton	m_Sound;
	CComboBox	m_SaveVersus;
	CComboBox	m_SaveResult;
	CButton	m_LingerAnim;
	CButton	m_InRouteAnim;
	CButton	m_HitAnim;
	CComboBox	m_Class;
	CSortableListCtrl	m_AttList;
	int		m_Price;
  int   m_priority;
	int		m_CastTime;
	BOOL	m_AllowedInCombat;
  BOOL  m_AllowedInCamp;
	CString	m_DurationText;
	BOOL	m_canTargetFriend;
  BOOL  m_canTargetEnemy;
	int		m_Level;
	CString	m_Name;
	//CString	m_NumTargetText;
	CString	m_TargetP1Text;
	//CString	m_TargetRangeText;
	CString	m_TargetP2Text;
	CString	m_TargetP3Text;
	CString	m_TargetP4Text;
	CString	m_TargetP5Text;
	CString	m_CoinName;
	//BOOL	m_UsableByParty;
	BOOL	m_CanMemorize;
	//BOOL	m_CanScribeAtLevelChange;
	BOOL	m_AllowScribing;
	BOOL	m_IsCumulative;
	CString	m_CastMsg;
	//CString	m_TargRangeHelp;
	//CString	m_NumTargHelp;
	//CString	m_TargNumberLabel;
	//CString	m_TargRangeLabel;
  CString m_TargP1Label;
  CString m_TargP2Label;
  CString m_TargP3Label;
  CString m_TargP4Label;
  CString m_TargP5Label;
	BOOL	m_Lingers;
	BOOL	m_LingerOnceOnly;
	CString	m_EffectDurationText;
	BOOL	m_CanBeDispelled;
	//BOOL	m_AutoScribeAtLevelChange;
	BOOL	m_AutoScribe;
	//BOOL	m_MagicUser;
	//BOOL	m_Fighter;
	//BOOL	m_Druid;
	//BOOL	m_Cleric;
	//BOOL	m_Paladin;
	//BOOL	m_Ranger;
	//BOOL	m_Thief;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellDBDlgEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

   CArray<SCHOOL_ID, SCHOOL_ID&> m_schools;


protected:

  void AddToAvailList(SPELL_EFFECTS_DATA &data);
  void UpdateAvailList(int item, SPELL_EFFECTS_DATA &data);

  void EnableCheckBox(int id);
  void DisableCheckBox(int id);

	// Generated message map functions
	//{{AFX_MSG(CSpellDBDlgEx)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeDurationrate();
	afx_msg void OnSelchangeTargetingtype();
	afx_msg void OnSpellsound();
	afx_msg void OnHitanim();
	afx_msg void OnInrouteanim();
	afx_msg void OnLingeranim();
	afx_msg void OnTargcovanim();
	afx_msg void OnDeleteattribute();
	afx_msg void OnAddattribute();
	virtual void OnOK();
	afx_msg void OnEditattribute();
	afx_msg void OnDblclkAttributelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeClass();
  afx_msg void OnChangeSave();
	afx_msg void OnCastanim();
	afx_msg void OnInroutesound();
	afx_msg void OnCoveragesound();
	afx_msg void OnSpellhitsound();
	afx_msg void OnSpelllingersound();
	afx_msg void OnSpecabs();
	afx_msg void OnEditspellattribs();
	afx_msg void OnSpellscripts();
	afx_msg void OnLingers();
	afx_msg void OnClassList();
	afx_msg void OnAutoscribeatlevelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPELLDBDLGEX_H__BCE10191_E25C_4A42_99AC_7F1788BA7320__INCLUDED_)
