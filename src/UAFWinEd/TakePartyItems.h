#if !defined(AFX_TAKEPARTYITEMS_H__0C1554A3_5BD9_11D4_864B_00A0CC25685D__INCLUDED_)
#define AFX_TAKEPARTYITEMS_H__0C1554A3_5BD9_11D4_864B_00A0CC25685D__INCLUDED_
/******************************************************************************
* Filename: TakePartyItems.h
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CTakePartyItems dialog

class CTakePartyItems : public CDialog
{
// Construction
public:
	CTakePartyItems(TAKE_PARTY_ITEMS_DATA &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(TAKE_PARTY_ITEMS_DATA &data) { data = m_data; }

  TAKE_PARTY_ITEMS_DATA m_data;

// Dialog Data
	//{{AFX_DATA(CTakePartyItems)
	enum { IDD = IDD_TAKEITEMS };
	CComboBox	m_WhichVault;
	CComboBox	m_QtyTypeMoney;
	CComboBox	m_QtyTypeJewelry;
	CComboBox	m_QtyTypeInv;
	CComboBox	m_QtyTypeGems;
	CComboBox	m_MoneyType;
	CComboBox	m_TakeFrom;
	CButton	m_ChooseArt;
	int		m_GemQty;
	int		m_ItemQty;
	int		m_JewelryQty;
	int		m_MoneyQty;
	BOOL	m_HitReturn;
	BOOL	m_TakeGems;
	BOOL	m_TakeInv;
	BOOL	m_TakeJewelry;
	BOOL	m_TakeMoney;
	CString	m_Text;
	BOOL	m_UseVault;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTakePartyItems)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTakePartyItems)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSeechar();
	afx_msg void OnChooseitems();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAKEPARTYITEMS_H__0C1554A3_5BD9_11D4_864B_00A0CC25685D__INCLUDED_)
