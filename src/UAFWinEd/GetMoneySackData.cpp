/******************************************************************************
* Filename: GetMoneySackData.cpp
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
#include "GetMoneySackData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetMoneySackData dialog


CGetMoneySackData::CGetMoneySackData(MONEY_SACK &data, CWnd* pParent /*=NULL*/)
	: CDialog(CGetMoneySackData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetMoneySackData)
	m_Amount1 = 0;
	m_Amount10 = 0;
	m_Amount2 = 0;
	m_Amount3 = 0;
	m_Amount4 = 0;
	m_Amount5 = 0;
	m_Amount6 = 0;
	m_Amount7 = 0;
	m_Amount8 = 0;
	m_Amount9 = 0;
	m_GemName = _T("");
	m_JewelryName = _T("");
	m_AmountGem = 0;
	m_AmountJewelry = 0;
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
	//}}AFX_DATA_INIT
  m_data=data;
}


void CGetMoneySackData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetMoneySackData)
	DDX_Text(pDX, IDC_CONVERT1, m_Amount1);
	DDX_Text(pDX, IDC_CONVERT10, m_Amount10);
	DDX_Text(pDX, IDC_CONVERT2, m_Amount2);
	DDX_Text(pDX, IDC_CONVERT3, m_Amount3);
	DDX_Text(pDX, IDC_CONVERT4, m_Amount4);
	DDX_Text(pDX, IDC_CONVERT5, m_Amount5);
	DDX_Text(pDX, IDC_CONVERT6, m_Amount6);
	DDX_Text(pDX, IDC_CONVERT7, m_Amount7);
	DDX_Text(pDX, IDC_CONVERT8, m_Amount8);
	DDX_Text(pDX, IDC_CONVERT9, m_Amount9);
	DDX_Text(pDX, IDC_GEMNAME, m_GemName);
	DDX_Text(pDX, IDC_JEWELRYNAME, m_JewelryName);
	DDX_Text(pDX, IDC_MINGEM, m_AmountGem);
	DDX_Text(pDX, IDC_MINJEWELRY, m_AmountJewelry);
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetMoneySackData, CDialog)
	//{{AFX_MSG_MAP(CGetMoneySackData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetMoneySackData message handlers

BOOL CGetMoneySackData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_Name1 = globalData.moneyData.GetName(PlatinumType);
  m_Name2 = globalData.moneyData.GetName(GoldType);
  m_Name3 = globalData.moneyData.GetName(ElectrumType);
  m_Name4 = globalData.moneyData.GetName(SilverType);
  m_Name5 = globalData.moneyData.GetName(CopperType);
  m_Name6 = globalData.moneyData.GetName(CoinType6);
  m_Name7 = globalData.moneyData.GetName(CoinType7);
  m_Name8 = globalData.moneyData.GetName(CoinType8);
  m_Name9 = globalData.moneyData.GetName(CoinType9);
  m_Name10 = globalData.moneyData.GetName(CoinType10);	

  m_Amount1 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(PlatinumType)];
  m_Amount2 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(GoldType)];
  m_Amount3 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(ElectrumType)];
  m_Amount4 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(SilverType)];
  m_Amount5 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(CopperType)];
  m_Amount6 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(CoinType6)];
  m_Amount7 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(CoinType7)];
  m_Amount8 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(CoinType8)];
  m_Amount9 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(CoinType9)];
  m_Amount10 = m_data.Coins[MONEY_DATA_TYPE::GetIndex(CoinType10)];

  CWnd *pwnd;
  if (!globalData.moneyData.IsActive(PlatinumType))
  {
    pwnd=GetDlgItem(IDC_CONVERT1);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(GoldType))
  {
    pwnd=GetDlgItem(IDC_CONVERT2);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(ElectrumType))
  {
    pwnd=GetDlgItem(IDC_CONVERT3);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(SilverType))
  {
    pwnd=GetDlgItem(IDC_CONVERT4);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(CopperType))
  {
    pwnd=GetDlgItem(IDC_CONVERT5);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(CoinType6))
  {
    pwnd=GetDlgItem(IDC_CONVERT6);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(CoinType7))
  {
    pwnd=GetDlgItem(IDC_CONVERT7);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(CoinType8))
  {
    pwnd=GetDlgItem(IDC_CONVERT8);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(CoinType9))
  {
    pwnd=GetDlgItem(IDC_CONVERT9);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }
  if (!globalData.moneyData.IsActive(CoinType10))
  {
    pwnd=GetDlgItem(IDC_CONVERT10);
    if (pwnd!=NULL) pwnd->EnableWindow(FALSE);
  }

  m_GemName = globalData.moneyData.GetName(GemType);
  m_JewelryName = globalData.moneyData.GetName(JewelryType);
  m_AmountGem = m_data.NumGems();
  m_AmountJewelry = m_data.NumJewelry();

  UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGetMoneySackData::OnOK() 
{
	UpdateData(TRUE);
	m_data.Clear();

  m_data.Add(PlatinumType, m_Amount1);
  m_data.Add(GoldType, m_Amount2);
  m_data.Add(ElectrumType, m_Amount3);
  m_data.Add(SilverType, m_Amount4);
  m_data.Add(CopperType, m_Amount5);
  m_data.Add(CoinType6, m_Amount6);
  m_data.Add(CoinType7, m_Amount7);
  m_data.Add(CoinType8, m_Amount8);
  m_data.Add(CoinType9, m_Amount9);
  m_data.Add(CoinType10, m_Amount10);

  for (int g=0;g<m_AmountGem;g++) m_data.AddGem();
  for (int j=0;j<m_AmountJewelry;j++) m_data.AddJewelry();

	CDialog::OnOK();
}
