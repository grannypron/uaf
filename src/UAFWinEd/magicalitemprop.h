/******************************************************************************
* Filename: magicalitemprop.h
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

#include "items.h"
/////////////////////////////////////////////////////////////////////////////
// CMagicalItemProp dialog

class CMagicalItemProp : public CDialog
{
// Construction
public:
	CMagicalItemProp(ITEM_DATA &item, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMagicalItemProp)
	enum { IDD = IDD_MAGICALITEMPROPERTIES };
	CButton	m_UseEvent;
	CButton	m_ChooseSpell;
	int		m_AttackBonus;
	BOOL	m_Cursed;
	CString	m_IDName;
	int		m_ProtectionBonus;
	BOOL	m_Usable;
	BOOL	m_Scribable;
  BOOL  m_NotMagical;
	int		m_NumCharges;
	int		m_SpellLevel;
	int		m_RechargeRate;
	//}}AFX_DATA

  ITEM_DATA m_ItemData;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMagicalItemProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  int m_SpellNum;

	// Generated message map functions
	//{{AFX_MSG(CMagicalItemProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnChoosespell();
	afx_msg void OnUsable();
	afx_msg void OnScribable();
	afx_msg void OnSpecabs();
	afx_msg void OnUseevent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

