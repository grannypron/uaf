/******************************************************************************
* Filename: ChooseMonster.cpp
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
//#include "..\Shared\Version.h"
#include "resource.h"
#include "UAFWinEd.h"
#include "Monster.h"
#include "itemdlg.h"
#include "GetMoneySackData.h"
#include "CombatMonsterDetails.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCombatMonsterDetails dialog


CCombatMonsterDetails::CCombatMonsterDetails(CString name, MONSTER_EVENT &data, CWnd* pParent /*=NULL*/)
	: CDialog(CCombatMonsterDetails::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCombatMonsterDetails)
	m_Friendly = FALSE;
	m_Morale = 0;
	m_Qty = 1;
	m_Name = _T("");
	m_QuantityType = MONSTER_EVENT::meUseQty;
	m_DiceSides = 0;
	m_DiceBonus = 0;
	m_NbrDice = 0;
	//}}AFX_DATA_INIT
  m_name = name;
  m_data = data;
}


void CCombatMonsterDetails::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCombatMonsterDetails)
	DDX_Check(pDX, IDC_FRIENDLY1, m_Friendly);
	DDX_Text(pDX, IDC_MORALE, m_Morale);
	DDX_Text(pDX, IDC_NUMMONSTERS1, m_Qty);
	DDX_Text(pDX, IDC_MONSTERNAME, m_Name);
	DDX_Radio(pDX, IDC_QUANTITYTYPE, m_QuantityType);
	DDX_Text(pDX, IDC_DICESIDES, m_DiceSides);
	DDX_Text(pDX, IDC_DICEBONUS, m_DiceBonus);
	DDX_Text(pDX, IDC_NBRDICE, m_NbrDice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCombatMonsterDetails, CDialog)
	//{{AFX_MSG_MAP(CCombatMonsterDetails)
	ON_BN_CLICKED(IDC_ITEMS1, OnItems1)
	ON_BN_CLICKED(IDC_MONSTERMONEY, OnMonstermoney)
	ON_BN_CLICKED(IDC_QUANTITYTYPE, OnQuantitytype)
	ON_BN_CLICKED(IDC_QUANTITYTYPE2, OnQuantitytype2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCombatMonsterDetails message handlers

void CCombatMonsterDetails::OnItems1() 
{
  CItemDlg iDlg(m_data.items);
  iDlg.m_AllItemsAreIdentified = FALSE;
  
  if (iDlg.DoModal()==IDOK)
    iDlg.GetItemList(m_data.items);	
}

BOOL CCombatMonsterDetails::OnInitDialog() 
{
	CDialog::OnInitDialog();
  	
	m_Friendly = m_data.friendly;
  m_Morale = m_data.moraleAdj;
  m_Qty = m_data.qty;
  m_DiceBonus = m_data.qtyBonus;
  m_DiceSides = m_data.qtyDiceSides;
  m_NbrDice = m_data.qtyDiceQty;
  m_QuantityType = m_data.UseQty;

  m_Name = m_name;

  UpdateData(FALSE);

  if (m_QuantityType == MONSTER_EVENT::meUseQty)
    OnQuantitytype();
  else
    OnQuantitytype2();
  
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCombatMonsterDetails::OnOK() 
{
  UpdateData(TRUE);
  
  m_data.friendly = m_Friendly;
  m_data.moraleAdj = m_Morale;
  m_data.qty = m_Qty;	

  m_data.qtyBonus = m_DiceBonus;
  m_data.qtyDiceSides = m_DiceSides;
  m_data.qtyDiceQty = m_NbrDice;
  m_data.UseQty = m_QuantityType;

	CDialog::OnOK();
}

void CCombatMonsterDetails::OnMonstermoney() 
{
  CGetMoneySackData dlg(m_data.money);
  if (dlg.DoModal()==IDOK)
    dlg.GetData(m_data.money);
}

void CCombatMonsterDetails::OnQuantitytype() 
{
  UpdateData(TRUE);

  m_NbrDice = 0;
  m_DiceSides = 0;
  m_DiceBonus = 0;

  UpdateData(FALSE);

  CWnd *pWnd = GetDlgItem(IDC_NUMMONSTERS1);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
  
  pWnd = GetDlgItem(IDC_DICESIDES);
  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_DICEBONUS);
  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_NBRDICE);
  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
}

void CCombatMonsterDetails::OnQuantitytype2() 
{
  UpdateData(TRUE);
  
  m_Qty = 0;
  
  UpdateData(FALSE);	

  CWnd *pWnd = GetDlgItem(IDC_NUMMONSTERS1);
  if (pWnd != NULL) pWnd->EnableWindow(FALSE);

  pWnd = GetDlgItem(IDC_DICESIDES);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
  pWnd = GetDlgItem(IDC_DICEBONUS);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
  pWnd = GetDlgItem(IDC_NBRDICE);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
}
