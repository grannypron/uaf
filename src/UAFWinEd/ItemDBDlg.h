/******************************************************************************
* Filename: ItemDBDlg.h
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
// CItemDBDlg dialog

class CItemDBDlg : public CDialog
{
// Construction
public:
	CItemDBDlg(CWnd* pParent = NULL);   // standard constructor
  ITEM_DATA m_ItemData;

// Dialog Data
	//{{AFX_DATA(CItemDBDlg)
	enum { IDD = IDD_ITEMDB };
	CButton	m_ExamineEvent;
	CButton	m_HitArt;
	CComboBox	m_AmmoType;
	CButton	m_LaunchSound;
	CButton	m_MissileArt;
	CButton	m_MissSound;
	CButton	m_HitSound;
  CButton m_ClassList;
	CComboBox	m_WpnType;
	CString	m_LocationCarried;
	CComboBox	m_HandsNeeded;
	int		m_BundleQty;
	//BOOL	m_Cleric;
	int		m_Cost;
	int		m_DmgBonusLg;
	int		m_DmgBonusSm;
	int		m_DmgDiceLg;
	int		m_DmgDiceSm;
	//BOOL	m_Druid;
	int		m_Encumbrance;
	int		m_ExpPts;
	//BOOL	m_Fighter;
	//BOOL	m_MagicUser;
	CString	m_Name;
	int		m_NbrDiceLg;
	int		m_NbrDiceSm;
	//BOOL	m_Paladin;
	int		m_ProtectionBase;
	int		m_RangeMax;
	//BOOL	m_Ranger;
	double	m_ROF;
	//BOOL	m_Thief;
	CString	m_IdName;
	CString	m_CoinName;
	BOOL	m_NonLethal;
	CString	m_ExamineLabel;
	CString	m_AttackMsg;
	BOOL	m_CanBeHalvedJoined;
	BOOL	m_CanBeTradeDropSoldDep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemDBDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  void UpdateControls();

	// Generated message map functions
	//{{AFX_MSG(CItemDBDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeWpntype();
	afx_msg void OnMagicalproperties();
	afx_msg void OnMisssound();
	afx_msg void OnHitsound();
	afx_msg void OnMissileart();
	afx_msg void OnLaunchsound();
	afx_msg void OnHitanim();
	afx_msg void OnSelchangeLocationcarried();
	virtual void OnOK();
	afx_msg void OnKillfocusRange();
	afx_msg void OnChangeRange();
	afx_msg void OnClassList();
	afx_msg void OnExamineevent();
	afx_msg void OnKillfocusBundleqty();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
