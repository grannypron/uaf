// StartingEquipDlg.cpp : implementation file
//

#include "..\Shared\stdafx.h"
#include "resource.h"
#include "uafwined.h"

//#include "GlobalData.h"
#include "ItemDlg.h"
#include "StartingEquipDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStartingEquipDlg dialog


CStartingEquipDlg::CStartingEquipDlg(GLOBAL_STATS *data, CWnd* pParent /*=NULL*/)
	: CDialog(CStartingEquipDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStartingEquipDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_data = data;
}


void CStartingEquipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStartingEquipDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStartingEquipDlg, CDialog)
	//{{AFX_MSG_MAP(CStartingEquipDlg)
	ON_BN_CLICKED(IDC_BASEITEMS, OnBaseitems)
	ON_BN_CLICKED(IDC_CLERICITEMS, OnClericitems)
	ON_BN_CLICKED(IDC_DRUIDITEMS, OnDruiditems)
	ON_BN_CLICKED(IDC_FIGHTERITEMS, OnFighteritems)
	ON_BN_CLICKED(IDC_MUITEMS, OnMuitems)
	ON_BN_CLICKED(IDC_PALADINITEMS, OnPaladinitems)
	ON_BN_CLICKED(IDC_RANGERITEMS, OnRangeritems)
	ON_BN_CLICKED(IDC_THIEFITEMS, OnThiefitems)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStartingEquipDlg message handlers

void CStartingEquipDlg::OnBaseitems() 
{
  CItemDlg dlg(m_data->startEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->startEquip);
}

void CStartingEquipDlg::OnClericitems() 
{
  CItemDlg dlg(m_data->ClericEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->ClericEquip);
}

void CStartingEquipDlg::OnDruiditems() 
{
  CItemDlg dlg(m_data->DruidEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->DruidEquip);	
}

void CStartingEquipDlg::OnFighteritems() 
{
  CItemDlg dlg(m_data->FighterEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->FighterEquip);	
}

void CStartingEquipDlg::OnMuitems() 
{
  CItemDlg dlg(m_data->MagicUserEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->MagicUserEquip);	
}

void CStartingEquipDlg::OnPaladinitems() 
{
  CItemDlg dlg(m_data->PaladinEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->PaladinEquip);	
}

void CStartingEquipDlg::OnRangeritems() 
{
  CItemDlg dlg(m_data->RangerEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->RangerEquip);	
}

void CStartingEquipDlg::OnThiefitems() 
{
  CItemDlg dlg(m_data->ThiefEquip);
  dlg.m_AllItemsAreIdentified = TRUE;
  if (dlg.DoModal()==IDOK)
    dlg.GetItemList(m_data->ThiefEquip);		
}
