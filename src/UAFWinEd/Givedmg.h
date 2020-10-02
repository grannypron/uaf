/******************************************************************************
* Filename: Givedmg.h
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
// GiveDmg dialog

class GiveDmg : public CDialog
{
// Construction
public:
	GiveDmg(GIVE_DAMAGE_DATA& damage, CWnd* pParent = NULL);   // standard constructor
	void GetDamageData(GIVE_DAMAGE_DATA& damage);

// Dialog Data
	//{{AFX_DATA(GiveDmg)
	enum { IDD = IDD_DAMAGE };
	CComboBox	m_Who;
	CButton	m_SeeArt;
	CComboBox	m_SaveVersus;
	CComboBox	m_SaveEffect;
	CComboBox	m_Distance;
	int		m_AttackTHAC0;
	int		m_Chance;
	int		m_NbrAttacks;
	int		m_NbrDice;
	int		m_NbrSides;
	int		m_SavingThrowBonus;
	int		m_DmgBonus;
	CString	m_Text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(GiveDmg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	GIVE_DAMAGE_DATA m_damage;

	// Generated message map functions
	//{{AFX_MSG(GiveDmg)
	afx_msg void OnSelchangeDistance();
	afx_msg void OnSelchangeSaveeffect();
	afx_msg void OnSelchangeSaveVersus();
	afx_msg void OnSeeart();
	afx_msg void OnSelchangeWho();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
