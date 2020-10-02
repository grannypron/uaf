/******************************************************************************
* Filename: GetMoney.cpp
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

#include "..\Shared\stdafx.h"
#include "resource.h"

#include "GlobalData.h"
#include "GetMoney.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetMoney dialog

CGetMoney::CGetMoney(int gems, int jewels, int platinum, CWnd* pParent /*=NULL*/)
	: CDialog(CGetMoney::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetMoney)
	m_Gems = 0;
	m_Jewelry = 0;
	m_Platinum = 0;
	m_CoinName = _T("");
	//}}AFX_DATA_INIT
  m_Gems = max(0, gems);
  m_Jewelry = max(0,jewels);
  m_Platinum = max(0,platinum);
}


void CGetMoney::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetMoney)
	DDX_Text(pDX, IDC_GEMS, m_Gems);
	DDX_Text(pDX, IDC_JEWELRY, m_Jewelry);
	DDX_Text(pDX, IDC_PLATINUM, m_Platinum);
	DDX_Text(pDX, IDC_COINNAME, m_CoinName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetMoney, CDialog)
	//{{AFX_MSG_MAP(CGetMoney)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetMoney message handlers

BOOL CGetMoney::OnInitDialog() 
{
	CDialog::OnInitDialog();	
  GotoDlgCtrl(GetDlgItem(IDC_PLATINUM));
  m_CoinName = globalData.moneyData.GetName(globalData.moneyData.GetDefaultType());
  UpdateData(FALSE);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
