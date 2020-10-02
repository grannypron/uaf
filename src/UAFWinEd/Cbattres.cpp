/******************************************************************************
* Filename: Cbattres.cpp
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
#include "GameEvent.h"
//#include "chooseit.h"
#include "itemdlg.h"
#include "GetMoneySackData.h"
#include "CbatTres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCombatTreas dialog


CCombatTreas::CCombatTreas(GIVE_TREASURE_DATA& treasure, CWnd* pParent /*=NULL*/)
	: CDialog(CCombatTreas::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCombatTreas)
	m_SilentGiveToActive = FALSE;
	//}}AFX_DATA_INIT

	m_treasure = treasure;
}


void CCombatTreas::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCombatTreas)
	DDX_Check(pDX, IDC_SILENTGIVETOACTIVE, m_SilentGiveToActive);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCombatTreas, CDialog)
	//{{AFX_MSG_MAP(CCombatTreas)
	ON_BN_CLICKED(IDC_CHOOSEITEMS, OnChooseitems)
	ON_BN_CLICKED(IDC_MONEYTREAS, OnMoneytreas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCombatTreas message handlers

void CCombatTreas::OnChooseitems() 
{
	//CChooseItems dlg(m_treasure.items);	
	//UINT result = dlg.DoModal();
	//if (result == IDOK)
	//	dlg.GetItemData(m_treasure.items);

  CItemDlg iDlg(m_treasure.items);
  
  UINT result = iDlg.DoModal();
  iDlg.m_AllItemsAreIdentified = FALSE;
  
  if (result == IDOK)
    iDlg.GetItemList(m_treasure.items);		

}

void CCombatTreas::OnOK() 
{
	UpdateData(TRUE);
	m_treasure.SilentGiveToActiveChar=m_SilentGiveToActive;
	CDialog::OnOK();
}

BOOL CCombatTreas::OnInitDialog() 
{
	CDialog::OnInitDialog();	
  m_SilentGiveToActive=m_treasure.SilentGiveToActiveChar;
  UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCombatTreas::GetTreasureData(GIVE_TREASURE_DATA& treasure)
{
	treasure = m_treasure;
}

void CCombatTreas::OnMoneytreas() 
{
  CGetMoneySackData dlg(m_treasure.money);
  if (dlg.DoModal()==IDOK)
    dlg.GetData(m_treasure.money);	
}
