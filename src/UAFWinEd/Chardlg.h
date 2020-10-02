/******************************************************************************
* Filename: Chardlg.h
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
#ifndef __CHARACTERDLG_H__
#define __CHARACTERDLG_H__
#include "char.h"

class CCharacter : public CDialog
{
// Construction
public:
	//CCharacter(int CharNum, CWnd* pParent = NULL);   // standard constructor
	CCharacter(const CHARACTER_ID& characterID, CWnd* pParent = NULL);   // standard constructor
  //int m_CharNum;
  CHARACTER_ID m_characterID;

public:
  // Dialog Data
  CHARACTER m_CharData;

	//{{AFX_DATA(CCharacter)
	enum { IDD = IDD_CHARACTER };
	CButton	m_ExamineEvent;
	CButton	m_TalkEvent;
	CButton	m_ChooseSpic;
	CButton	m_ChooseIcon;
	CComboBox	m_ChooseRace;
	CComboBox	m_ChooseGender;
	CComboBox	m_ChooseClass;
	CComboBox	m_ChooseAlignment;
	CButton	m_ChooseSpells;
	CButton	m_CharItems;
	CString	m_CharacterName;
	int		m_HitPoints;
	int		m_Charisma;
  int   m_Morale;
	int		m_Constitution;
	int		m_Dexterity;
	int		m_Intelligence;
	int		m_Strength;
	int		m_Wisdom;
	BOOL	m_IsPreGen;
	//int		m_Level1;
	//int		m_Level2;
	//int		m_Level3;
  int m_Level[3];
	//CString	m_L1Class;
	//CString	m_L2Class;
	//CString	m_L3Class;
  CString m_Class[3];
	int		m_StartAge;
	int		m_StrengthBonus;
	BOOL	m_CanTradeItems;
	BOOL	m_CanBeSaved;
	CString	m_TalkLabel;
	CString	m_ExamineLabel;
	//int		m_ExpPoints1;
	//int		m_ExpPoints2;
	//int		m_ExpPoints3;
  int   m_ExpPoints[3];
	BOOL	m_DisableTalkIfDead;
	BOOL	m_AllowInCombat;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCharacter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

   void UpdateLevels(BOOL ddx);
   void GetLevels();
   void SetLevels();
   void UpdateSpellAbility();


	// Generated message map functions
	//{{AFX_MSG(CCharacter)
	virtual BOOL OnInitDialog();
	afx_msg void OnCharitems();
	afx_msg void OnCharspic();
	afx_msg void OnChariconpic();
	afx_msg void OnChoosespells();
	afx_msg void OnSelchangeChooseclass();
	afx_msg void OnMoney();
	afx_msg void OnRollstats();
	virtual void OnOK();
	afx_msg void OnTalkevent();
  afx_msg void OnSpecAbs() ;
  afx_msg void OnSpecAbsHelp();
	afx_msg void OnExamineevent();
	afx_msg void OnKillfocusExppts1();
	afx_msg void OnKillfocusExppts2();
	afx_msg void OnKillfocusExppts3();
	afx_msg void OnKillfocusLevel1();
	afx_msg void OnKillfocusLevel2();
	afx_msg void OnKillfocusLevel3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnEnChangeEdit1();
};

#endif
