/******************************************************************************
* Filename: AttibuteDataEdit.cpp
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
//#include "uafwined.h"
#include "AttibuteDataEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttibuteDataEdit dialog


CAttibuteDataEdit::CAttibuteDataEdit(CString name, CString value, CWnd* pParent /*=NULL*/)
	: CDialog(CAttibuteDataEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttibuteDataEdit)
	m_Name = name;
	m_Value = value;
	//}}AFX_DATA_INIT
}


void CAttibuteDataEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttibuteDataEdit)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_VALUE, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttibuteDataEdit, CDialog)
	//{{AFX_MSG_MAP(CAttibuteDataEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttibuteDataEdit message handlers

void CAttibuteDataEdit::OnOK() 
{
  UpdateData(true);
  if (m_Name.GetLength() == 0)
  {
    MsgBoxInfo("Key has zero length","Error");
    return;
  };
  // So what if the value is empty!
  //if (m_Value.GetLength() == 0)
  //{
  //  MsgBoxInfo("Value has zero length","Error");
  //  return;
  //};
	CDialog::OnOK();
}

BOOL CAttibuteDataEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
  GotoDlgCtrl(GetDlgItem(IDC_NAME));		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
