#if !defined(AFX_GETMONEYSACKDATA_H__2473A6A6_3ABE_419C_9683_7C5358E63AF5__INCLUDED_)
#define AFX_GETMONEYSACKDATA_H__2473A6A6_3ABE_419C_9683_7C5358E63AF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/******************************************************************************
* Filename: GetMoneySackData.h
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
// CGetMoneySackData dialog
#include "Money.h"

class CGetMoneySackData : public CDialog
{
// Construction
public:
	CGetMoneySackData(MONEY_SACK &data, CWnd* pParent = NULL);   // standard constructor
  void GetData(MONEY_SACK &data) { data=m_data; }

// Dialog Data
	//{{AFX_DATA(CGetMoneySackData)
	enum { IDD = IDD_GETMONEYSACKDATA };
	int		m_Amount1;
	int		m_Amount10;
	int		m_Amount2;
	int		m_Amount3;
	int		m_Amount4;
	int		m_Amount5;
	int		m_Amount6;
	int		m_Amount7;
	int		m_Amount8;
	int		m_Amount9;
	CString	m_GemName;
	CString	m_JewelryName;
	int		m_AmountGem;
	int		m_AmountJewelry;
	CString	m_Name1;
	CString	m_Name10;
	CString	m_Name2;
	CString	m_Name3;
	CString	m_Name4;
	CString	m_Name5;
	CString	m_Name6;
	CString	m_Name7;
	CString	m_Name8;
	CString	m_Name9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetMoneySackData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  MONEY_SACK m_data;

	// Generated message map functions
	//{{AFX_MSG(CGetMoneySackData)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETMONEYSACKDATA_H__2473A6A6_3ABE_419C_9683_7C5358E63AF5__INCLUDED_)
