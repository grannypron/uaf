/******************************************************************************
* Filename: Shopdlg.cpp
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

//#include "UAFWinEd.h"
#include "GlobalData.h"
#include "items.h"
#include "Char.h"
#include "GameEvent.h"

#include "itemdlg.h"
#include "SelectPicDlg.h"
#include "ShopDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShop dialog


CShop::CShop(SHOP& shops, CWnd* pParent /*=NULL*/)
	: CDialog(CShop::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShop)
	m_ForceExit = FALSE;
	m_EntryText = _T("");
	m_IdCost = 0;
	m_MoneyName = _T("");
	m_BuyPercentage = 0;
	m_CanId = FALSE;
	m_CanApprGems = FALSE;
	m_CanApprJewelry = FALSE;
	m_BuyItemsSoldOnly = FALSE;
	//}}AFX_DATA_INIT

	m_shops = shops;
}


void CShop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShop)
	DDX_Control(pDX, IDC_COSTFACTOR, m_CostFactor);
	DDX_Control(pDX, IDC_RETAILER, m_Retailer);
	DDX_Control(pDX, IDC_CHOOSE_ITEMS, m_ChooseItems);
	DDX_Check(pDX, IDC_FORCEEXIT, m_ForceExit);
	DDX_Text(pDX, IDC_SHOPENTERTEXT, m_EntryText);
	DDX_Text(pDX, IDC_IDCOST, m_IdCost);
	DDX_Text(pDX, IDC_MONEYNAME, m_MoneyName);
	DDX_Text(pDX, IDC_BUYBACK, m_BuyPercentage);
	DDX_Check(pDX, IDC_CANID, m_CanId);
	DDX_Check(pDX, IDC_CANAPPRGEMS, m_CanApprGems);
	DDX_Check(pDX, IDC_CANAPPRJEWELS, m_CanApprJewelry);
	DDX_Check(pDX, IDC_BUYITEMSSOLDONLY, m_BuyItemsSoldOnly);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShop, CDialog)
	//{{AFX_MSG_MAP(CShop)
	ON_BN_CLICKED(IDC_CHOOSE_ITEMS, OnChooseItems)
	ON_BN_CLICKED(IDC_RETAILER, OnRetailer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShop message handlers

void CShop::OnChooseItems() 
{
	CItemDlg iDlg(m_shops.itemsAvail);
	iDlg.m_AllItemsAreIdentified = TRUE;
  
	UINT result = iDlg.DoModal();

	if (result == IDOK)
		iDlg.GetItemList(m_shops.itemsAvail);		

  POSITION pos = m_shops.itemsAvail.GetHeadPosition();
  while (pos != NULL)
    m_shops.itemsAvail.GetNext(pos).identified = TRUE; // shops disclose full name
}

void CShop::OnRetailer() 
{
  CSelectPicDlg dlg(m_shops.pic, SmallPicDib);

  if (dlg.DoModal() == IDOK)
  {
    dlg.GetData(m_shops.pic);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_shops.pic.filename, name, MAX_MEDITBUTTON_TEXT);
    m_Retailer.SetWindowText(name);
  }
}

BOOL CShop::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ForceExit = m_shops.forceExit;
  m_EntryText = m_shops.text;
  m_IdCost = m_shops.costToId;
  m_CanId = m_shops.canId;
  m_BuyItemsSoldOnly = m_shops.buyItemsSoldOnly;
  
  m_CanApprGems = m_shops.canApprGems;
  m_CanApprJewelry = m_shops.canApprJewels;

  CWnd  *pwnd;
  pwnd = GetDlgItem(IDC_CANAPPRGEMS);
  if (pwnd!=NULL)
  {
    CString GemName = globalData.moneyData.GetName(GemType);
    CString label;
    label.Format("Can Appraise %s", GemName);
    pwnd->SetWindowText(label);
  }  
  pwnd = GetDlgItem(IDC_CANAPPRJEWELS);
  if (pwnd!=NULL)
  {
    CString JewelryName = globalData.moneyData.GetName(JewelryType);
    CString label;
    label.Format("Can Appraise %s", JewelryName);
    pwnd->SetWindowText(label);
  }

  if (m_IdCost < 0) m_IdCost = 0;
  m_BuyPercentage = m_shops.buybackPercentage;
  if (m_BuyPercentage < 0) m_BuyPercentage=0;
  if (m_BuyPercentage > 100) m_BuyPercentage=100;

	m_CostFactor.ResetContent();

  int i;
  for (i=0;i<NUM_COST_FACTOR_TYPES;i++)
    m_CostFactor.AddString(costFactorText[i]);

  m_CostFactor.SetCurSel(m_shops.costFactor);

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_shops.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_Retailer.SetWindowText(name);

  
  m_MoneyName = globalData.moneyData.GetName(globalData.moneyData.GetDefaultType());
  UpdateData(FALSE);	

  GotoDlgCtrl(GetDlgItem(IDC_SHOPENTERTEXT));  
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CShop::GetShopData(SHOP& shop)
{
	shop = m_shops;
}

void CShop::OnOK() 
{
	UpdateData(TRUE);

	m_shops.forceExit = m_ForceExit;
  m_shops.text = m_EntryText;
  m_shops.costFactor = (costFactorType)m_CostFactor.GetCurSel();
  m_shops.canId=m_CanId;
  m_shops.canApprGems=m_CanApprGems;
  m_shops.canApprJewels=m_CanApprJewelry;
  m_shops.buyItemsSoldOnly=m_BuyItemsSoldOnly;
  
  m_shops.costToId = m_IdCost;
  if (m_shops.costToId < 0) m_shops.costToId = 0;

  m_shops.buybackPercentage = m_BuyPercentage;
  if (m_shops.buybackPercentage < 0) m_shops.buybackPercentage=0;
  if (m_shops.buybackPercentage > 100) m_shops.buybackPercentage=100;

	CDialog::OnOK();
}

