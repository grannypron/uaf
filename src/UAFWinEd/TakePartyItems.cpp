/******************************************************************************
* Filename: TakePartyItems.cpp
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

#include "uafwined.h"
#include "itemdlg.h"
#include "GlobalData.h"
#include "GlobText.h"
#include "SelectPicDlg.h"
#include "Char.h"
#include "TakePartyItems.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTakePartyItems dialog


CTakePartyItems::CTakePartyItems(TAKE_PARTY_ITEMS_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CTakePartyItems::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTakePartyItems)
	m_GemQty = 0;
	m_ItemQty = 0;
	m_JewelryQty = 0;
	m_MoneyQty = 0;
	m_HitReturn = FALSE;
	m_TakeGems = FALSE;
	m_TakeInv = FALSE;
	m_TakeJewelry = FALSE;
	m_TakeMoney = FALSE;
	m_Text = _T("");
	m_UseVault = FALSE;
	//}}AFX_DATA_INIT

  m_data = data;
}


void CTakePartyItems::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTakePartyItems)
	DDX_Control(pDX, IDC_WHICHVAULT, m_WhichVault);
	DDX_Control(pDX, IDC_QTYTYPEMONEY, m_QtyTypeMoney);
	DDX_Control(pDX, IDC_QTYTYPEJEWELRY, m_QtyTypeJewelry);
	DDX_Control(pDX, IDC_QTYTYPEINV, m_QtyTypeInv);
	DDX_Control(pDX, IDC_QTYTYPEGEMS, m_QtyTypeGems);
	DDX_Control(pDX, IDC_MONEYTYPE, m_MoneyType);
	DDX_Control(pDX, IDC_TAKEFROM, m_TakeFrom);
	DDX_Control(pDX, IDC_SEECHAR, m_ChooseArt);
	DDX_Text(pDX, IDC_GEMQTY, m_GemQty);
	DDX_Text(pDX, IDC_ITEMQTY, m_ItemQty);
	DDX_Text(pDX, IDC_JEWELRYQTY, m_JewelryQty);
	DDX_Text(pDX, IDC_MONEYQTY, m_MoneyQty);
	DDX_Check(pDX, IDC_MUSTHITRETURN, m_HitReturn);
	DDX_Check(pDX, IDC_TAKEGEMS, m_TakeGems);
	DDX_Check(pDX, IDC_TAKEINV, m_TakeInv);
	DDX_Check(pDX, IDC_TAKEJEWELRY, m_TakeJewelry);
	DDX_Check(pDX, IDC_TAKEMONEY, m_TakeMoney);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Check(pDX, IDC_USEVAULT, m_UseVault);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTakePartyItems, CDialog)
	//{{AFX_MSG_MAP(CTakePartyItems)
	ON_BN_CLICKED(IDC_SEECHAR, OnSeechar)
	ON_BN_CLICKED(IDC_CHOOSEITEMS, OnChooseitems)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTakePartyItems message handlers

void CTakePartyItems::OnOK() 
{
  UpdateData(TRUE);
  
  m_data.text = m_Text;

  m_data.takeItems = 0;

  if (m_TakeMoney)
    m_data.takeItems |= TakeMoneyAction;

  if (m_TakeGems)
    m_data.takeItems |= TakeGemsAction;

  if (m_TakeJewelry)
    m_data.takeItems |= TakeJewelryAction;

  if (m_TakeInv)
    m_data.takeItems |= TakeInventoryAction;

  m_data.StoreItems = m_UseVault;
  m_data.mustHitReturn = m_HitReturn;
  m_data.takeAffects = (takeItemsAffectsType)m_TakeFrom.GetCurSel();
  int currSel = m_QtyTypeInv.GetCurSel();
  m_data.itemSelectFlags = (takeItemQtyType)m_QtyTypeInv.GetItemData(currSel);
  m_data.platinumSelectFlags = (takeItemQtyType) m_QtyTypeMoney.GetCurSel();
  m_data.gemsSelectFlags = (takeItemQtyType) m_QtyTypeGems.GetCurSel();
  m_data.jewelrySelectFlags = (takeItemQtyType) m_QtyTypeJewelry.GetCurSel();
  currSel = m_MoneyType.GetCurSel();
  m_data.moneyType = (itemClassType)m_MoneyType.GetItemData(currSel);
  m_data.itemPcnt = m_ItemQty;
  m_data.platinum = m_MoneyQty;
  m_data.gems = m_GemQty;
  m_data.jewelry = m_JewelryQty;
  m_data.WhichVault = m_WhichVault.GetCurSel();

	CDialog::OnOK();
}

BOOL CTakePartyItems::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_TakeFrom.ResetContent();
  m_QtyTypeInv.ResetContent();
  m_QtyTypeMoney.ResetContent();
  m_QtyTypeGems.ResetContent();
  m_QtyTypeJewelry.ResetContent();
  m_MoneyType.ResetContent();
  int i;

  for (i=0; i<NUM_TAKEAFFECTS_TYPES; i++)
    m_TakeFrom.AddString(TakeItemsAffectsText[i]);

  m_TakeFrom.SetCurSel(m_data.takeAffects);

  int added = 0;
  int sel = 0;
  for (i=0; i<NUM_TAKEITEMQTY_TYPES; i++)
  {
    if (((takeItemQtyType)i == TakeRndQty) || ((takeItemQtyType)i == TakePcntQty))
      continue;
    
    m_QtyTypeInv.AddString(TakeItemsQtyText[i]);
    m_QtyTypeInv.SetItemData(added, i);
    if (i == m_data.itemSelectFlags)
      sel = added;
    added++;
  }

  for (i=0; i<NUM_TAKEITEMQTY_TYPES; i++)
    m_QtyTypeMoney.AddString(TakeItemsQtyText[i]);
  for (i=0; i<NUM_TAKEITEMQTY_TYPES; i++)
    m_QtyTypeGems.AddString(TakeItemsQtyText[i]);
  for (i=0; i<NUM_TAKEITEMQTY_TYPES; i++)
    m_QtyTypeJewelry.AddString(TakeItemsQtyText[i]);

  m_QtyTypeInv.SetCurSel(sel);
  m_QtyTypeMoney.SetCurSel(m_data.platinumSelectFlags);
  m_QtyTypeGems.SetCurSel(m_data.gemsSelectFlags);
  m_QtyTypeJewelry.SetCurSel(m_data.jewelrySelectFlags);

  for (i=0;i<globalData.moneyData.NumCoinTypes();i++)
  {
    if (globalData.moneyData.IsActive(MONEY_DATA_TYPE::GetItemClass(i)))
    {
      int item = m_MoneyType.AddString(globalData.moneyData.GetName(MONEY_DATA_TYPE::GetItemClass(i)));
      m_MoneyType.SetItemData(item, MONEY_DATA_TYPE::GetItemClass(i));
    }
  }

  m_MoneyType.SetCurSel(0);
  for (i=0;i<m_MoneyType.GetCount();i++)
  {
    if (m_MoneyType.GetItemData(i) == (DWORD)m_data.moneyType)
    {
      m_MoneyType.SetCurSel(i);
      break;
    }
  }

  m_UseVault = m_data.StoreItems;
  m_HitReturn = m_data.mustHitReturn;
  m_ItemQty = m_data.itemPcnt;
  m_MoneyQty = m_data.platinum;
  m_GemQty = m_data.gems;
  m_JewelryQty = m_data.jewelry;
  m_Text = m_data.text;
	
  if (m_data.takeItems & TakeMoneyAction) 
    m_TakeMoney = TRUE;
  if (m_data.takeItems & TakeGemsAction) 
    m_TakeGems = TRUE;
  if (m_data.takeItems & TakeJewelryAction) 
    m_TakeJewelry = TRUE;
  if (m_data.takeItems & TakeInventoryAction) 
    m_TakeInv = TRUE;

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_ChooseArt.SetWindowText(name);

  for (i=0;i<MAX_GLOBAL_VAULTS;i++)
  {
    CString tmp;
    tmp.Format("%i", i+1);
    m_WhichVault.AddString(tmp);
  }

  m_WhichVault.SetCurSel(m_data.WhichVault);
  
  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

  UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTakePartyItems::OnSeechar() 
{
   CSelectPicDlg dlg(m_data.pic, SmallPicDib | BigPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_data.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
   }	
}

void CTakePartyItems::OnChooseitems() 
{
	CItemDlg iDlg(m_data.items);
	iDlg.m_AllItemsAreIdentified = TRUE;

	UINT result = iDlg.DoModal();

	if (result == IDOK)
		iDlg.GetItemList(m_data.items);			
}
