/******************************************************************************
* Filename: Shopdlg.h
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

#include "GameEvent.h"

/////////////////////////////////////////////////////////////////////////////
// CShop dialog

class CShop : public CDialog
{
// Construction
public:
	CShop(SHOP& shops, CWnd* pParent = NULL);   // standard constructor
	void GetShopData(SHOP& shops);


// Dialog Data
	//{{AFX_DATA(CShop)
	enum { IDD = IDD_SHOP };
	CComboBox	m_CostFactor;
	CButton	m_Retailer;
	CButton	m_ChooseItems;
	BOOL	m_ForceExit;
	CString	m_EntryText;
	int		m_IdCost;
	CString	m_MoneyName;
	int		m_BuyPercentage;
	BOOL	m_CanId;
	BOOL	m_CanApprGems;
	BOOL	m_CanApprJewelry;
	BOOL	m_BuyItemsSoldOnly;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShop)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SHOP m_shops;

	// Generated message map functions
	//{{AFX_MSG(CShop)
	afx_msg void OnChooseItems();
	afx_msg void OnRetailer();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
