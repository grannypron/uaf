/******************************************************************************
* Filename: MonsterDBDlg.h
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

#include "Monster.h"
#include "DlgPicture.h"

/////////////////////////////////////////////////////////////////////////////
// CMonsterDBDlg dialog

class CMonsterDBDlg : public CDialog
{
// Construction
public:
	CMonsterDBDlg(CWnd* pParent = NULL);   // standard constructor
  MONSTER_DATA m_MonsterData;

// Dialog Data
	//{{AFX_DATA(CMonsterDBDlg)
	enum { IDD = IDD_MONSTERDB };
	CDlgPicture	m_MonsterPic;
	//CComboBox	m_UndeadType;
  CString m_undeadType;
	CButton	m_MoveSound;
	CButton	m_DeathSound;
	CButton	m_MissSound;
	CButton	m_HitSound;
	CButton	m_Icon;
	CComboBox	m_Size;
  CComboBox m_Class;
  CComboBox m_Race;
	int		m_ArmorClass;
	int		m_HitDiceBonus;
	int		m_Intelligence;
	int		m_MagicResistance;
	int		m_Morale;
	int		m_Movement;
	CString	m_Name;
	int		m_THAC0;
	int		m_XPValue;
	float	m_HitDice;
	BOOL	m_FAnimal;
	BOOL	m_FGiant;
	BOOL	m_FLarge;
	BOOL	m_FMammal;
	BOOL	m_FSnake;
	BOOL	m_IConfusion;
	BOOL	m_IDeath;
	BOOL	m_IPoison;
	BOOL	m_IVorpal;
	BOOL	m_OAffByDispelEvil;
	BOOL	m_OCanBeHeldCharm;
	BOOL	m_PDwAC;
	BOOL	m_PDwTHAC0;
	BOOL	m_PGnAC;
	BOOL	m_PGnTHAC0;
	BOOL	m_PRangDmg;
	CString	m_AttackCount;
	int		m_UseHitDice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonsterDBDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:  
//  int m_CurrMonster;

  CArray<CLASS_ID, CLASS_ID&>m_availClasses;
  CArray<RACE_ID,  RACE_ID&> m_availRaces;

	// Generated message map functions
	//{{AFX_MSG(CMonsterDBDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseSpAttacks();
	afx_msg void OnChooseicon();
	afx_msg void OnMisssound();
	afx_msg void OnHitsound();
	afx_msg void OnMovesound();
	afx_msg void OnDeathsound();
	afx_msg void OnItems();
	afx_msg void OnAttackdata();
	virtual void OnOK();
	afx_msg void OnDefaultmoney();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
