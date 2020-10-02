/******************************************************************************
* Filename: EntryPointDlg.cpp
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
#include "EntryPointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntryPointDlg dialog


CEntryPointDlg::CEntryPointDlg(LEVEL_STATS &data, int cep, CWnd* pParent /*=NULL*/)
	: CDialog(CEntryPointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEntryPointDlg)
	m_x1 = 0;
	m_x2 = 0;
	m_x3 = 0;
	m_x4 = 0;
	m_x5 = 0;
	m_x6 = 0;
	m_x7 = 0;
	m_x8 = 0;
	m_y1 = 0;
	m_y2 = 0;
	m_y3 = 0;
	m_y4 = 0;
	m_y5 = 0;
	m_y6 = 0;
	m_y7 = 0;
	m_y8 = 0;
	m_CurrEP = 0;
	m_MaxX = _T("50");
	m_MaxY = _T("50");
	//}}AFX_DATA_INIT

   for (int i=0;i<MAX_ENTRY_POINTS;i++)
   {
      memcpy(&m_EntryPoints[i], &data.entryPoints[i], sizeof(m_EntryPoints[i]));
   }

   m_CurrEP = cep;
   m_MaxX.Format("%i", data.area_width-1);
   m_MaxY.Format("%i", data.area_height-1);
}


void CEntryPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntryPointDlg)
	DDX_Control(pDX, IDC_EPCOLOR8, m_Color8);
	DDX_Control(pDX, IDC_EPCOLOR7, m_Color7);
	DDX_Control(pDX, IDC_EPCOLOR6, m_Color6);
	DDX_Control(pDX, IDC_EPCOLOR5, m_Color5);
	DDX_Control(pDX, IDC_EPCOLOR4, m_Color4);
	DDX_Control(pDX, IDC_EPCOLOR3, m_Color3);
	DDX_Control(pDX, IDC_EPCOLOR2, m_Color2);
	DDX_Control(pDX, IDC_EPCOLOR1, m_Color1);
	DDX_Text(pDX, IDC_EPX1, m_x1);
	DDX_Text(pDX, IDC_EPX2, m_x2);
	DDX_Text(pDX, IDC_EPX3, m_x3);
	DDX_Text(pDX, IDC_EPX4, m_x4);
	DDX_Text(pDX, IDC_EPX5, m_x5);
	DDX_Text(pDX, IDC_EPX6, m_x6);
	DDX_Text(pDX, IDC_EPX7, m_x7);
	DDX_Text(pDX, IDC_EPX8, m_x8);
	DDX_Text(pDX, IDC_EPY1, m_y1);
	DDX_Text(pDX, IDC_EPY2, m_y2);
	DDX_Text(pDX, IDC_EPY3, m_y3);
	DDX_Text(pDX, IDC_EPY4, m_y4);
	DDX_Text(pDX, IDC_EPY5, m_y5);
	DDX_Text(pDX, IDC_EPY6, m_y6);
	DDX_Text(pDX, IDC_EPY7, m_y7);
	DDX_Text(pDX, IDC_EPY8, m_y8);
	DDX_Radio(pDX, IDC_SELECT1, m_CurrEP);
	DDX_Text(pDX, IDC_MAX_X, m_MaxX);
	DDX_Text(pDX, IDC_MAX_Y, m_MaxY);
	//}}AFX_DATA_MAP

   int mx = atoi(m_MaxX);
   int my = atoi(m_MaxY);

	DDV_MinMaxInt(pDX, m_x1, 0, mx);
	DDV_MinMaxInt(pDX, m_y1, 0, my);
	DDV_MinMaxInt(pDX, m_x2, 0, mx);
	DDV_MinMaxInt(pDX, m_y2, 0, my);
	DDV_MinMaxInt(pDX, m_x3, 0, mx);
	DDV_MinMaxInt(pDX, m_y3, 0, my);
	DDV_MinMaxInt(pDX, m_x4, 0, mx);
	DDV_MinMaxInt(pDX, m_y4, 0, my);
	DDV_MinMaxInt(pDX, m_x5, 0, mx);
	DDV_MinMaxInt(pDX, m_y5, 0, my);
	DDV_MinMaxInt(pDX, m_x6, 0, mx);
	DDV_MinMaxInt(pDX, m_y6, 0, my);
	DDV_MinMaxInt(pDX, m_x7, 0, mx);
	DDV_MinMaxInt(pDX, m_y7, 0, my);
	DDV_MinMaxInt(pDX, m_x8, 0, mx);
	DDV_MinMaxInt(pDX, m_y8, 0, my);
}


BEGIN_MESSAGE_MAP(CEntryPointDlg, CDialog)
	//{{AFX_MSG_MAP(CEntryPointDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntryPointDlg message handlers

BOOL CEntryPointDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	

   FillControl(&m_Color1,GetEPColor(0));
   FillControl(&m_Color2,GetEPColor(1));
   FillControl(&m_Color3,GetEPColor(2));
   FillControl(&m_Color4,GetEPColor(3));
   FillControl(&m_Color5,GetEPColor(4));
   FillControl(&m_Color6,GetEPColor(5));
   FillControl(&m_Color7,GetEPColor(6));
   FillControl(&m_Color8,GetEPColor(7));
	
   m_x1 = m_EntryPoints[0].x;
   m_y1 = m_EntryPoints[0].y;
   m_x2 = m_EntryPoints[1].x;
   m_y2 = m_EntryPoints[1].y;
   m_x3 = m_EntryPoints[2].x;
   m_y3 = m_EntryPoints[2].y;
   m_x4 = m_EntryPoints[3].x;
   m_y4 = m_EntryPoints[3].y;
   m_x5 = m_EntryPoints[4].x;
   m_y5 = m_EntryPoints[4].y;
   m_x6 = m_EntryPoints[5].x;
   m_y6 = m_EntryPoints[5].y;
   m_x7 = m_EntryPoints[6].x;
   m_y7 = m_EntryPoints[6].y;
   m_x8 = m_EntryPoints[7].x;
   m_y8 = m_EntryPoints[7].y;

   UpdateData(FALSE);

   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEntryPointDlg::OnOK() 
{
	CDialog::OnOK();

   m_EntryPoints[0].x = m_x1;
   m_EntryPoints[0].y = m_y1;
   m_EntryPoints[1].x = m_x2;
   m_EntryPoints[1].y = m_y2;
   m_EntryPoints[2].x = m_x3;
   m_EntryPoints[2].y = m_y3;
   m_EntryPoints[3].x = m_x4;
   m_EntryPoints[3].y = m_y4;
   m_EntryPoints[4].x = m_x5;
   m_EntryPoints[4].y = m_y5;
   m_EntryPoints[5].x = m_x6;
   m_EntryPoints[5].y = m_y6;
   m_EntryPoints[6].x = m_x7;
   m_EntryPoints[6].y = m_y7;
   m_EntryPoints[7].x = m_x8;
   m_EntryPoints[7].y = m_y8;

}

void CEntryPointDlg::GetData(LEVEL_STATS &data)
{
   for (int i=0;i<MAX_ENTRY_POINTS;i++)
   {
      memcpy(&data.entryPoints[i], &m_EntryPoints[i], sizeof(m_EntryPoints[i]));
   }

   currEntryPoint = m_CurrEP;
}
