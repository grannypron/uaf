/******************************************************************************
* Filename: TavernTalesDlg.cpp
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
#include "TavernTalesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTavern dialog


CTavernTales::CTavernTales(TAVERN_TALES& event, CWnd* pParent /*=NULL*/)
	: CDialog(CTavernTales::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTavernTales)
	//m_ForceExit = FALSE;
	//m_AllowDrinks = FALSE;
	//m_AllowFights = FALSE;
	//m_DrinkNum = 0;
	//m_DrinkName = _T("");
	//m_DrinkPoints = 0;
	//m_Text = _T("");
	//m_PointTrigger = 0;
	m_TaleNum = 0;
	//m_TaleText = _T("");
	//m_TaleOrder = 1;
  m_highlight = FALSE;
  m_drink = FALSE;
  m_drunk = FALSE;
  m_replace = FALSE;
	//m_EachTaleOnceOnly = FALSE;
	//}}AFX_DATA_INIT

	m_event = event;
}


void CTavernTales::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTavernTales)

	DDX_Control(pDX, IDC_TALESPIN, m_TaleSpin);
	//DDX_Control(pDX, IDC_DRINKSPIN, m_DrinkSpin);
	//DDX_Control(pDX, IDC_BARKEEP, m_ChooseBarkeep);
	//DDX_Check(pDX, IDC_FORCEEXIT, m_ForceExit);
	DDX_Check(pDX, IDC_CUMULATIVE, m_cumulative);
	DDX_Check(pDX, IDC_DRINK, m_drink);
	DDX_Check(pDX, IDC_DRUNK, m_drunk);
	DDX_Check(pDX, IDC_REPLACE, m_replace);
	DDX_Check(pDX, IDC_Highlight, m_highlight);
	//DDX_Check(pDX, IDC_ALLOWDRINKS, m_AllowDrinks);
	//DDX_Check(pDX, IDC_ALLOWFIGHTS, m_AllowFights);
	//DDX_Text(pDX, IDC_DRINK_NUM, m_DrinkNum);
	//DDX_Text(pDX, IDC_DRINKNAME, m_DrinkName);
	//DDV_MaxChars(pDX, m_DrinkName, 15);
	//DDX_Text(pDX, IDC_DRINKPOINTS, m_DrinkPoints);
	//DDX_Text(pDX, IDC_ENTERTEXT, m_Text);
	//DDX_Text(pDX, IDC_POINTTRIGGER, m_PointTrigger);
	//DDX_Text(pDX, IDC_TALE_NUM, m_TaleNum);
	DDX_Text(pDX, IDC_TALETEXT, m_TaleText);
	//DDX_Radio(pDX, IDC_TALEORDER, m_TaleOrder);
	//DDX_Check(pDX, IDC_EACHTALEONCEONLY, m_EachTaleOnceOnly);


  //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTavernTales, CDialog)
	//{{AFX_MSG_MAP(CTavernTales)
	//ON_BN_CLICKED(IDC_BARKEEP, OnBarkeep)
	//ON_NOTIFY(UDN_DELTAPOS, IDC_DRINKSPIN, OnDeltaposDrinkspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TALESPIN, OnDeltaposTalespin)
  //ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTavernTales message handlers

BOOL CTavernTales::OnInitDialog() 
{
	CDialog::OnInitDialog();

  //char name[MAX_MEDITBUTTON_TEXT+1];
  
	//m_Inflation = m_event.inflation;
  //m_Text         = m_event.text;
  m_drink        = (m_event.m_flags & TTF_IF_DRINK) != 0;
  m_drunk        = (m_event.m_flags & TTF_IF_DRUNK) != 0;
  m_replace      = (m_event.m_flags & TTF_REPLACE) != 0;
  m_cumulative   = (m_event.m_flags & TTF_CUMULATIVE) != 0;


  m_TaleNum = 1;
   m_TaleSpin.SetRange(0, 32767);
	m_TaleSpin.SetPos(m_TaleNum);
  if (m_TaleNum < m_event.m_tales.GetSize()+1)
  {
    m_TaleText  = m_event.PeekTale(m_TaleNum-1)->m_tale;
    m_highlight = (m_event.PeekTale(m_TaleNum-1)->m_flags & TTF_HIGHLIGHT) != 0;
  }
  else
  {
    m_TaleText.Empty();
    m_highlight = FALSE;
  };

  GotoDlgCtrl(GetDlgItem(IDC_TALETEXT));

  UpdateData(FALSE);	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CTavernTales::PreTranslateMessage(MSG* pMsg)
{
   if(pMsg->message == WM_KEYDOWN   )  
   {
      if(pMsg->wParam == VK_PRIOR)
      {
        DeltaTaleNum(1);
      }
      else if(pMsg->wParam == VK_NEXT)
      {
        DeltaTaleNum(-1);      
      }
   }
   return CDialog::PreTranslateMessage(pMsg);  
}

//void CTavernTales::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//}


void CTavernTales::GetTavernTalesData(TAVERN_TALES& event)
{
	event = m_event;
}

void CTavernTales::OnOK() 
{
  UpdateData(TRUE);

  m_event.m_flags = 0;
  if (m_drink)      m_event.m_flags |= TTF_IF_DRINK;
  if (m_drunk)      m_event.m_flags |= TTF_IF_DRUNK;
  if(m_replace)     m_event.m_flags |= TTF_REPLACE;
  if(m_cumulative)  m_event.m_flags |= TTF_CUMULATIVE;


  //m_event.forceExit = m_ForceExit;
  //m_event.text = m_Text;

  //m_event.allowFights = m_AllowFights;
  //m_event.allowDrinks = m_AllowDrinks;

  //m_event.tales[m_TaleNum-1].tale = m_TaleText;

  //m_event.drinks[m_DrinkNum-1].name = m_DrinkName;
  //m_event.drinks[m_DrinkNum-1].points = m_DrinkPoints;
  //m_event.drinkPointTrigger = m_PointTrigger;

  //m_event.taleOrder = (taleOrderType)m_TaleOrder;
  //m_event.EachTaleOnceOnly = m_EachTaleOnceOnly;
  if (m_TaleNum == m_event.m_tales.GetSize()+1)
  {
    if (!m_TaleText.IsEmpty())
    {
      TAVERN_TALES_TALE tale;
      tale.m_tale = m_TaleText;
      tale.m_flags = 0;
      if (m_highlight) tale.m_flags |= TTF_HIGHLIGHT;
      m_event.m_tales.Add(tale);
    }
  }
  else
  {
    m_event.GetTale(m_TaleNum-1)->m_tale = m_TaleText;
    m_event.GetTale(m_TaleNum-1)->m_flags = 0;
    if (m_highlight)
    {
      m_event.GetTale(m_TaleNum-1)->m_flags |= TTF_HIGHLIGHT;
    };
  };

  
	CDialog::OnOK();
}


void CTavernTales::DeltaTaleNum(int delta)
{
  UpdateData(TRUE);
  if (m_TaleNum == m_event.m_tales.GetSize()+1)
  {
    // Ignore the spinner if the text is emtpy
    if (!m_TaleText.IsEmpty())
    {
      // Add it to the tales.
      TAVERN_TALES_TALE tale;
      tale.m_tale = m_TaleText;
      tale.m_flags = 0;
      if (m_highlight) tale.m_flags |= TTF_HIGHLIGHT;
      m_event.m_tales.Add(tale);
    };
  }
  else
  {
    m_event.GetTale(m_TaleNum-1)->m_tale = m_TaleText;
    m_event.GetTale(m_TaleNum-1)->m_flags = 0;
    if (m_highlight) m_event.GetTale(m_TaleNum-1)->m_flags |= TTF_HIGHLIGHT;
  };
    m_TaleNum += delta;

  	if (m_TaleNum > m_event.m_tales.GetSize()+1)
		  m_TaleNum = 1;

    if (m_TaleNum < 1)
  		m_TaleNum = m_event.m_tales.GetSize();
  
    if (m_TaleNum == 0)
    {
      m_TaleNum = 1;
    };

    if (m_TaleNum > m_event.m_tales.GetSize())
    {
      m_TaleText.Empty();
      m_highlight = FALSE;
    }
    else
    {
      m_TaleText = m_event.PeekTale(m_TaleNum-1)->m_tale;
      m_highlight = (m_event.PeekTale(m_TaleNum-1)->m_flags & TTF_HIGHLIGHT) != 0;
    };
  
  
  UpdateData(FALSE);
  m_TaleSpin.SetPos(m_TaleNum);

};


void CTavernTales::OnDeltaposTalespin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

  DeltaTaleNum(pNMUpDown->iDelta);
  m_TaleSpin.SetPos(m_TaleNum - pNMUpDown->iDelta);
  *pResult = 0;
}
