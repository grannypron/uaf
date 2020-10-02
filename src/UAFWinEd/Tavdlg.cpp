/******************************************************************************
* Filename: Tavdlg.cpp
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
#include "Char.h"
//#include "GlobalData.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "TavDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTavern dialog


CTavern::CTavern(TAVERN& event, CWnd* pParent /*=NULL*/)
	: CDialog(CTavern::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTavern)
	m_ForceExit = FALSE;
	m_AllowDrinks = FALSE;
	m_AllowFights = FALSE;
	m_DrinkNum = 0;
	m_DrinkName = _T("");
	m_DrinkPoints = 0;
	m_Text = _T("");
	m_PointTrigger = 0;
	m_TaleNum = 0;
	m_TaleText = _T("");
	m_TaleOrder = 0;
	m_EachTaleOnceOnly = FALSE;
	//}}AFX_DATA_INIT

	m_event = event;
}


void CTavern::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTavern)
	DDX_Control(pDX, IDC_TALESPIN, m_TaleSpin);
	DDX_Control(pDX, IDC_DRINKSPIN, m_DrinkSpin);
	DDX_Control(pDX, IDC_BARKEEP, m_ChooseBarkeep);
	DDX_Check(pDX, IDC_FORCEEXIT, m_ForceExit);
	DDX_Check(pDX, IDC_ALLOWDRINKS, m_AllowDrinks);
	DDX_Check(pDX, IDC_ALLOWFIGHTS, m_AllowFights);
	DDX_Text(pDX, IDC_DRINK_NUM, m_DrinkNum);
	DDX_Text(pDX, IDC_DRINKNAME, m_DrinkName);
	DDV_MaxChars(pDX, m_DrinkName, 15);
	DDX_Text(pDX, IDC_DRINKPOINTS, m_DrinkPoints);
	DDX_Text(pDX, IDC_ENTERTEXT, m_Text);
	DDX_Text(pDX, IDC_POINTTRIGGER, m_PointTrigger);
	DDX_Text(pDX, IDC_TALE_NUM, m_TaleNum);
	DDX_Text(pDX, IDC_TALETEXT, m_TaleText);
	DDX_Radio(pDX, IDC_TALEORDER, m_TaleOrder);
	DDX_Check(pDX, IDC_EACHTALEONCEONLY, m_EachTaleOnceOnly);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTavern, CDialog)
	//{{AFX_MSG_MAP(CTavern)
	ON_BN_CLICKED(IDC_BARKEEP, OnBarkeep)
	ON_NOTIFY(UDN_DELTAPOS, IDC_DRINKSPIN, OnDeltaposDrinkspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TALESPIN, OnDeltaposTalespin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTavern message handlers

void CTavern::OnBarkeep() 
{
  CSelectPicDlg dlg(m_event.pic, SmallPicDib);

  if (dlg.DoModal() == IDOK)
  {
    dlg.GetData(m_event.pic);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
    m_ChooseBarkeep.SetWindowText(name);
  }
}


BOOL CTavern::OnInitDialog() 
{
	CDialog::OnInitDialog();
  
  char name[MAX_MEDITBUTTON_TEXT+1];
  
	//m_Inflation = m_event.inflation;
	m_ForceExit = m_event.forceExit;
  m_Text = m_event.text;
  m_AllowFights = m_event.allowFights;
  m_AllowDrinks = m_event.allowDrinks;

  m_TaleNum = 1;
	m_TaleSpin.SetRange(1, MAX_TALES);
	m_TaleSpin.SetPos(m_TaleNum);
  m_TaleText = m_event.tales[m_TaleNum-1].tale;

  m_DrinkNum = 1;
	m_DrinkSpin.SetRange(1, MAX_DRINKS);
	m_DrinkSpin.SetPos(m_DrinkNum);
  m_DrinkName = m_event.drinks[m_DrinkNum-1].name;
  m_DrinkPoints = m_event.drinks[m_DrinkNum-1].points;
  m_PointTrigger = m_event.drinkPointTrigger;

  getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_ChooseBarkeep.SetWindowText(name);

  m_TaleOrder = m_event.taleOrder;
  m_EachTaleOnceOnly = m_event.EachTaleOnceOnly;

  GotoDlgCtrl(GetDlgItem(IDC_ENTERTEXT));

  UpdateData(FALSE);	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTavern::GetTavernData(TAVERN& event)
{
	event = m_event;
}

void CTavern::OnOK() 
{
	UpdateData(TRUE);

	//m_event.inflation = m_Inflation;
	m_event.forceExit = m_ForceExit;
	m_event.text = m_Text;

  m_event.allowFights = m_AllowFights;
  m_event.allowDrinks = m_AllowDrinks;

  m_event.tales[m_TaleNum-1].tale = m_TaleText;

  m_event.drinks[m_DrinkNum-1].name = m_DrinkName;
  m_event.drinks[m_DrinkNum-1].points = m_DrinkPoints;
  m_event.drinkPointTrigger = m_PointTrigger;

  m_event.taleOrder = (taleOrderType)m_TaleOrder;
  m_event.EachTaleOnceOnly = m_EachTaleOnceOnly;
  
	CDialog::OnOK();
}

void CTavern::OnDeltaposDrinkspin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
  UpdateData(TRUE);

  m_event.drinks[m_DrinkNum-1].name = m_DrinkName;
  m_event.drinks[m_DrinkNum-1].points = m_DrinkPoints;

  m_DrinkNum = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (m_DrinkNum < 1)
		m_DrinkNum = MAX_DRINKS;

	if (m_DrinkNum > MAX_DRINKS)
		m_DrinkNum = 1;

  m_DrinkName = m_event.drinks[m_DrinkNum-1].name;
  m_DrinkPoints = m_event.drinks[m_DrinkNum-1].points;

	UpdateData(FALSE);
	
	*pResult = 0;
}

void CTavern::OnDeltaposTalespin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  UpdateData(TRUE);

  m_event.tales[m_TaleNum-1].tale = m_TaleText;

  m_TaleNum = pNMUpDown->iPos + pNMUpDown->iDelta;

	if (m_TaleNum < 1)
		m_TaleNum = MAX_TALES;

	if (m_TaleNum > MAX_TALES)
		m_TaleNum = 1;

  m_TaleText = m_event.tales[m_TaleNum-1].tale;

	UpdateData(FALSE);
  
	*pResult = 0;
}
