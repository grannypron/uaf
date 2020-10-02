/******************************************************************************
* Filename: CombatEvent.h
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

/////////////////////////////////////////////////////////////////////////////
// CCombat dialog

class CCombat : public CDialog
{
// Construction
public:
	CCombat(COMBAT_EVENT_DATA& combat, CWnd* pParent = NULL);   // standard constructor
	void GetCombatData(COMBAT_EVENT_DATA& combat);

// Dialog Data
	//{{AFX_DATA(CCombat)
	enum { IDD = IDD_COMBAT };
	CButton	m_TurnUndeadSound;
	CButton	m_DeathSound;
	CButton	m_MoveSound;
	CButton	m_SeeMonster;
	CComboBox	m_TurnUndead;
	CComboBox	m_Surprise;
	CComboBox	m_Distance;
	CComboBox	m_Direction;
	BOOL	m_AutoApproach;
	BOOL	m_NoMagic;
	int		m_Morale;
	BOOL	m_NoTreasure;
	BOOL	m_Outdoors;
	BOOL	m_PartyNeverDies;
	CString	m_Text;
	BOOL	m_RandomMonster;
	BOOL	m_PartyNoExperience;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCombat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	COMBAT_EVENT_DATA m_combat;

	// Generated message map functions
	//{{AFX_MSG(CCombat)
	afx_msg void OnChoosemonsters();
	afx_msg void OnSelchangeDirection();
	afx_msg void OnSelchangeDistance();
	afx_msg void OnSeemonster();
	afx_msg void OnSelchangeSurprise();
	afx_msg void OnSelchangeTurnundead();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeathsound();
	afx_msg void OnMovesound();
	afx_msg void OnTurnundeadsound();
	afx_msg void OnBgsounds();
  afx_msg void OnCombatEventQuestion();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
