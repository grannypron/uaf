/******************************************************************************
* Filename: MoneyConfigDlg.cpp
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

#include "UAFWinEd.h"
#include "GlobalData.h"
#include "MoneyConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoneyConfigDlg dialog


CMoneyConfigDlg::CMoneyConfigDlg(MONEY_DATA_TYPE &data, CWnd* pParent /*=NULL*/)
	: CDialog(CMoneyConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMoneyConfigDlg)
	m_Convert1 = 1.0;
	m_Convert10 = 0.0;
	m_Convert2 = 5.0;
	m_Convert3 = 10.0;
	m_Convert4 = 100.0;
	m_Convert5 = 1000.0;
	m_Convert6 = 0.0;
	m_Convert7 = 0.0;
	m_Convert8 = 0.0;
	m_Convert9 = 0.0;
	m_Name1 = _T("");
	m_Name10 = _T("");
	m_Name2 = _T("");
	m_Name3 = _T("");
	m_Name4 = _T("");
	m_Name5 = _T("");
	m_Name6 = _T("");
	m_Name7 = _T("");
	m_Name8 = _T("");
	m_Name9 = _T("");
	m_GemName = _T("");
	m_Jewelry = _T("");
	m_MaxGem = 0;
	m_MaxJewelry = 0;
	m_MinGem = 0;
	m_MinJewelry = 0;
	m_CoinWeight = 0;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CMoneyConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoneyConfigDlg)
	DDX_Control(pDX, IDC_DEFAULTCOIN, m_DefaultCoin);
	DDX_Text(pDX, IDC_CONVERT1, m_Convert1);
	DDX_Text(pDX, IDC_CONVERT10, m_Convert10);
	DDX_Text(pDX, IDC_CONVERT2, m_Convert2);
	DDX_Text(pDX, IDC_CONVERT3, m_Convert3);
	DDX_Text(pDX, IDC_CONVERT4, m_Convert4);
	DDX_Text(pDX, IDC_CONVERT5, m_Convert5);
	DDX_Text(pDX, IDC_CONVERT6, m_Convert6);
	DDX_Text(pDX, IDC_CONVERT7, m_Convert7);
	DDX_Text(pDX, IDC_CONVERT8, m_Convert8);
	DDX_Text(pDX, IDC_CONVERT9, m_Convert9);
	DDX_Text(pDX, IDC_NAME1, m_Name1);
	DDX_Text(pDX, IDC_NAME10, m_Name10);
	DDX_Text(pDX, IDC_NAME2, m_Name2);
	DDX_Text(pDX, IDC_NAME3, m_Name3);
	DDX_Text(pDX, IDC_NAME4, m_Name4);
	DDX_Text(pDX, IDC_NAME5, m_Name5);
	DDX_Text(pDX, IDC_NAME6, m_Name6);
	DDX_Text(pDX, IDC_NAME7, m_Name7);
	DDX_Text(pDX, IDC_NAME8, m_Name8);
	DDX_Text(pDX, IDC_NAME9, m_Name9);
	DDX_Text(pDX, IDC_GEMNAME, m_GemName);
	DDX_Text(pDX, IDC_JEWELRYNAME, m_Jewelry);
	DDX_Text(pDX, IDC_MAXGEM, m_MaxGem);
	DDX_Text(pDX, IDC_MAXJEWELRY, m_MaxJewelry);
	DDX_Text(pDX, IDC_MINGEM, m_MinGem);
	DDX_Text(pDX, IDC_MINJEWELRY, m_MinJewelry);
	DDX_Text(pDX, IDC_COINWEIGHT, m_CoinWeight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoneyConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CMoneyConfigDlg)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoneyConfigDlg message handlers

BOOL CMoneyConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  Update();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMoneyConfigDlg::Update()
{
  int i;
  for (i=0;i<m_data.NumCoinTypes();i++)
  {
    if (m_data.IsActive(MONEY_DATA_TYPE::GetItemClass(i)))
    {
      if (strlen(m_data.GetName(MONEY_DATA_TYPE::GetItemClass(i))) == 0)
        m_data.SetRate(MONEY_DATA_TYPE::GetItemClass(i), 0.0);
    }
  }
  m_Name1 = m_data.GetName(PlatinumType);
  m_Convert1 = m_data.GetRate(PlatinumType);
  m_Name2 = m_data.GetName(GoldType);
  m_Convert2 = m_data.GetRate(GoldType);
  m_Name3 = m_data.GetName(ElectrumType);
  m_Convert3 = m_data.GetRate(ElectrumType);
  m_Name4 = m_data.GetName(SilverType);
  m_Convert4 = m_data.GetRate(SilverType);
  m_Name5 = m_data.GetName(CopperType);
  m_Convert5 = m_data.GetRate(CopperType);  
  m_Name6 = m_data.GetName(CoinType6);
  m_Convert6 = m_data.GetRate(CoinType6);
  m_Name7 = m_data.GetName(CoinType7);
  m_Convert7 = m_data.GetRate(CoinType7);
  m_Name8 = m_data.GetName(CoinType8);
  m_Convert8 = m_data.GetRate(CoinType8);
  m_Name9 = m_data.GetName(CoinType9);
  m_Convert9 = m_data.GetRate(CoinType9);
  m_Name10 = m_data.GetName(CoinType10);
  m_Convert10 = m_data.GetRate(CoinType10);
	
  m_DefaultCoin.ResetContent();
  for (i=0;i<m_data.NumCoinTypes();i++)
  {
    if (m_data.IsActive(MONEY_DATA_TYPE::GetItemClass(i)))
    {
      m_DefaultCoin.AddString(m_data.GetName(MONEY_DATA_TYPE::GetItemClass(i)));
    }
  }
  m_DefaultCoin.SetCurSel(m_data.GetIndex(m_data.GetDefaultType()));

  m_GemName = m_data.GetName(GemType);
  m_Jewelry = m_data.GetName(JewelryType);
  m_data.GetGemRate(m_MinGem, m_MaxGem);
  m_data.GetJewelryRate(m_MinJewelry, m_MaxJewelry);
  m_CoinWeight = m_data.GetWeight();

  UpdateData(FALSE);
}

void CMoneyConfigDlg::Retrieve() 
{
  UpdateData(TRUE);
  m_data.Clear();

  m_data.SetName(PlatinumType, m_Name1);
  m_data.SetRate(PlatinumType, m_Convert1);

  m_data.SetName(GoldType, m_Name2);
  m_data.SetRate(GoldType, m_Convert2);

  m_data.SetName(ElectrumType, m_Name3);
  m_data.SetRate(ElectrumType, m_Convert3);

  m_data.SetName(SilverType, m_Name4);
  m_data.SetRate(SilverType, m_Convert4);

  m_data.SetName(CopperType, m_Name5);
  m_data.SetRate(CopperType, m_Convert5);

  m_data.SetName(CoinType6, m_Name6);
  m_data.SetRate(CoinType6, m_Convert6);
  m_data.SetName(CoinType7, m_Name7);
  m_data.SetRate(CoinType7, m_Convert7);
  m_data.SetName(CoinType8, m_Name8);
  m_data.SetRate(CoinType8, m_Convert8);
  m_data.SetName(CoinType9, m_Name9);
  m_data.SetRate(CoinType9, m_Convert9);
  m_data.SetName(CoinType10, m_Name10);
  m_data.SetRate(CoinType10, m_Convert10);
  m_data.SetDefaultType(m_data.GetItemClass(m_DefaultCoin.GetCurSel()));

  m_data.SetName(GemType, m_GemName);
  m_data.SetName(JewelryType, m_Jewelry);
  m_data.SetGemRate(m_MinGem, m_MaxGem);
  m_data.SetJewelryRate(m_MinJewelry, m_MaxJewelry);
  m_data.SetWeight(m_CoinWeight);
}

void CMoneyConfigDlg::OnOK() 
{
  Retrieve();
	CDialog::OnOK();
}

void CMoneyConfigDlg::OnApply() 
{
  Retrieve();
  Update();
}
