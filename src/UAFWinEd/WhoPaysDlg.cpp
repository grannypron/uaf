/******************************************************************************
* Filename: WhoPaysDlg.cpp
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
#include "globtext.h"
#include "GlobalData.h"
#include "Char.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "TeleportDlg.h"
#include "WhoPaysDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWhoPaysDlg dialog


CWhoPaysDlg::CWhoPaysDlg(WHO_PAYS_EVENT_DATA& data, CWnd* pParent /*=NULL*/)
	: CDialog(CWhoPaysDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWhoPaysDlg)
	m_FailText = _T("");
	m_PayGems = 0;
	m_PayPlatinum = 0;
	m_PayJewels = 0;
	m_PayImpossible = FALSE;
	m_SuccessText = _T("");
	m_Text = _T("");
	m_GemName = _T("");
	m_JewelryName = _T("");
	//}}AFX_DATA_INIT

  m_data = data;
}


void CWhoPaysDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhoPaysDlg)
	DDX_Control(pDX, IDC_MONEYTYPE, m_MoneyType);
	DDX_Control(pDX, IDC_SUCCESSTELEPORT, m_SuccessTeleport);
	DDX_Control(pDX, IDC_SUCCESSACTION, m_SuccessAction);
	DDX_Control(pDX, IDC_FAILTELEPORT, m_FailTeleport);
	DDX_Control(pDX, IDC_FAILACTION, m_FailAction);
	DDX_Control(pDX, IDC_CHOOSEART, m_ChooseArt);
	DDX_Text(pDX, IDC_FAILTEXT, m_FailText);
	DDX_Text(pDX, IDC_PAYGEMS, m_PayGems);
	DDX_Text(pDX, IDC_PAYPLATINUM, m_PayPlatinum);
	DDX_Text(pDX, IDC_PAYJEWELS, m_PayJewels);
	DDX_Check(pDX, IDC_SUCCESSIMPOSSIBLE, m_PayImpossible);
	DDX_Text(pDX, IDC_SUCCESSTEXT, m_SuccessText);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Text(pDX, IDC_GEMNAME, m_GemName);
	DDX_Text(pDX, IDC_JEWELRYNAME, m_JewelryName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWhoPaysDlg, CDialog)
	//{{AFX_MSG_MAP(CWhoPaysDlg)
	ON_BN_CLICKED(IDC_CHOOSEART, OnChooseart)
	ON_CBN_SELCHANGE(IDC_FAILACTION, OnSelchangeFailaction)
	ON_BN_CLICKED(IDC_FAILTELEPORT, OnFailteleport)
	ON_CBN_SELCHANGE(IDC_SUCCESSACTION, OnSelchangeSuccessaction)
	ON_BN_CLICKED(IDC_SUCCESSTELEPORT, OnSuccessteleport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhoPaysDlg message handlers

void CWhoPaysDlg::OnOK() 
{
   UpdateData(TRUE);
   
   m_data.text = m_Text;  
   m_data.text3 = m_FailText;
   m_data.text2 = m_SuccessText;
   
   m_data.successAction = 
      (passwordActionType) m_SuccessAction.GetCurSel();
   m_data.failAction =
      (passwordActionType) m_FailAction.GetCurSel();
   m_data.gems = m_PayGems;
   m_data.jewels = m_PayJewels;
   m_data.platinum = m_PayPlatinum;
   m_data.impossible = m_PayImpossible;

   int currSel = m_MoneyType.GetCurSel();
   m_data.moneyType = (itemClassType)m_MoneyType.GetItemData(currSel);
   
	CDialog::OnOK();
}

BOOL CWhoPaysDlg::OnInitDialog() 
{
   int i;
   CDialog::OnInitDialog();
      
   m_MoneyType.ResetContent();
   
   m_FailText = m_data.text3;
   m_SuccessText = m_data.text2;
   m_Text = m_data.text;
   
   m_SuccessAction.ResetContent();
   m_FailAction.ResetContent();
   
   for (i=0;i<NUM_PASSACTION_TYPES;i++)
   {
      m_SuccessAction.AddString(PasswordActionText[i]);
      m_FailAction.AddString(PasswordActionText[i]);
   }
   
   m_SuccessAction.SetCurSel((int)m_data.successAction);
   m_FailAction.SetCurSel((int)m_data.failAction);
   
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseArt.SetWindowText(name);

   m_PayGems = m_data.gems;
   m_PayJewels = m_data.jewels;
   m_PayPlatinum = m_data.platinum;
   m_PayImpossible = m_data.impossible;

   GotoDlgCtrl(GetDlgItem(IDC_TEXT));
   m_GemName = globalData.moneyData.GetName(GemType);
   m_JewelryName = globalData.moneyData.GetName(JewelryType);

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

   UpdateData(FALSE);
   SetControlStates();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWhoPaysDlg::OnChooseart() 
{
   CSelectPicDlg dlg(m_data.pic, SmallPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_data.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
   }
}

void CWhoPaysDlg::OnSelchangeFailaction() 
{
   UpdateData(TRUE);
   
   m_data.failAction = 
      (passwordActionType) m_FailAction.GetCurSel();
   
   SetControlStates();
   UpdateData(FALSE);
}

void CWhoPaysDlg::OnFailteleport() 
{
	CTeleportDlg dlg(m_data.failTransferData);

  if (dlg.DoModal() == IDOK)
    dlg.GetData(m_data.failTransferData);
}

void CWhoPaysDlg::OnSelchangeSuccessaction() 
{
   UpdateData(TRUE);
   
   m_data.successAction = 
      (passwordActionType) m_SuccessAction.GetCurSel();
   
   SetControlStates();
   UpdateData(FALSE);
}

void CWhoPaysDlg::OnSuccessteleport() 
{
	CTeleportDlg dlg(m_data.successTransferData);

  if (dlg.DoModal() == IDOK)
    dlg.GetData(m_data.successTransferData);
}

void CWhoPaysDlg::SetControlStates(void)
{
   m_SuccessTeleport.EnableWindow(FALSE);
   m_FailTeleport.EnableWindow(FALSE);
   
   switch (m_data.successAction) 
   {
   case ChainEvent:
      break;
   case Teleport:
      m_SuccessTeleport.EnableWindow(TRUE);
      break;
   default:
      break;
   }
   
   switch (m_data.failAction) 
   {
   case ChainEvent:
      break;
   case Teleport:
      m_FailTeleport.EnableWindow(TRUE);
      break;
   default:
      break;
   }
}

void CWhoPaysDlg::GetData(WHO_PAYS_EVENT_DATA &data)
{
  data = m_data;
}
