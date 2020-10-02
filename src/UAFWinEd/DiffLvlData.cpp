/******************************************************************************
* Filename: DiffLvlData.cpp
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

//#include "GlobalData.h"
#include "DiffLvlData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_DIFFLVL_NAME 10

/////////////////////////////////////////////////////////////////////////////
// CDiffLvlData dialog


CDiffLvlData::CDiffLvlData(DIFFICULTY_DATA &data, CWnd* pParent /*=NULL*/)
	: CDialog(CDiffLvlData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDiffLvlData)
	m_AExpAmount = 0;
	m_HDAmount = 0;
	m_MExpAmount = 0;
	m_ModAExp = FALSE;
	m_ModHD = FALSE;
	m_ModMExp = FALSE;
	m_ModQty = FALSE;
	m_Name = _T("");
	m_QtyAmount = 0;
	//}}AFX_DATA_INIT
  m_data = data;
}


void CDiffLvlData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiffLvlData)
	DDX_Text(pDX, IDC_AEXPAMOUNT, m_AExpAmount);
	DDX_Text(pDX, IDC_HDAMOUNT, m_HDAmount);
	DDX_Text(pDX, IDC_MEXPAMOUNT, m_MExpAmount);
	DDX_Check(pDX, IDC_MODAEXP, m_ModAExp);
	DDX_Check(pDX, IDC_MODHD, m_ModHD);
	DDX_Check(pDX, IDC_MODMEXP, m_ModMExp);
	DDX_Check(pDX, IDC_MODQTY, m_ModQty);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_QTYAMOUNT, m_QtyAmount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiffLvlData, CDialog)
	//{{AFX_MSG_MAP(CDiffLvlData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiffLvlData message handlers

void CDiffLvlData::OnOK() 
{
  UpdateData(TRUE);

  if (m_Name.GetLength() > MAX_DIFFLVL_NAME) m_Name.SetAt(MAX_DIFFLVL_NAME,'\0');
  m_data.m_Name = m_Name;
  m_data.m_AExpAmount = m_AExpAmount;
  m_data.m_HDAmount = m_HDAmount;
  m_data.m_MExpAmount = m_MExpAmount;
  m_data.m_QtyAmount = m_QtyAmount;
  m_data.m_ModifyAllExp = m_ModAExp;
  m_data.m_ModifyHD = m_ModHD;
  m_data.m_ModifyMonsterExp = m_ModMExp;
  m_data.m_ModifyQty = m_ModQty;

	CDialog::OnOK();
}

BOOL CDiffLvlData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_Name = m_data.m_Name;
  if (m_Name.GetLength() > MAX_DIFFLVL_NAME) m_Name.SetAt(MAX_DIFFLVL_NAME,'\0');
  m_AExpAmount = m_data.m_AExpAmount;
  m_HDAmount = m_data.m_HDAmount;
  m_MExpAmount = m_data.m_MExpAmount;
  m_QtyAmount = m_data.m_QtyAmount;
  m_ModAExp = m_data.m_ModifyAllExp;
  m_ModHD = m_data.m_ModifyHD;
  m_ModMExp = m_data.m_ModifyMonsterExp;
  m_ModQty = m_data.m_ModifyQty;

  UpdateData(FALSE);
  GotoDlgCtrl(GetDlgItem(IDC_NAME));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
