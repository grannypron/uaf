/******************************************************************************
* Filename: AttributeSelect.cpp
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

#include "RunTimeIf.h"
#include "AttributeSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttributeSelect dialog


CAttributeSelect::CAttributeSelect(BOOL AllowReadOnly, CWnd* pParent /*=NULL*/)
	: CDialog(CAttributeSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttributeSelect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  attribute="";
  m_AllowReadOnly=AllowReadOnly;
}


void CAttributeSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttributeSelect)
	DDX_Control(pDX, IDC_ATTLIST, m_AttList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttributeSelect, CDialog)
	//{{AFX_MSG_MAP(CAttributeSelect)
	ON_LBN_DBLCLK(IDC_ATTLIST, OnDblclkAttlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttributeSelect message handlers

BOOL CAttributeSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_AttList.ResetContent();

  int i;
  char label[100];
  char key[50];
  char type[50];
  for (i=FIRST_CHAR_KEYWORD;i<=LAST_CHAR_KEYWORD;i++)
  {
    if ((!m_AllowReadOnly) && (IsDataReadOnly(i)))
      continue;

    GetDataText(i, key);
    GetDataSizeText(i, type);
    sprintf(label, "%s (%s)", key,type);
    int index = m_AttList.AddString(label);
    m_AttList.SetItemData(index, i);
  }
  for (i=FIRST_PARTY_KEYWORD;i<=LAST_PARTY_KEYWORD;i++)
  {
    if ((!m_AllowReadOnly) && (IsDataReadOnly(i)))
      continue;

    GetDataText(i, key);
    GetDataSizeText(i, type);
    sprintf(label, "%s (%s)", key,type);
    int index = m_AttList.AddString(label);
    m_AttList.SetItemData(index, i);
  }
  for (i=FIRST_GAME_KEYWORD;i<=LAST_GAME_KEYWORD;i++)
  {
    if ((!m_AllowReadOnly) && (IsDataReadOnly(i)))
      continue;

    GetDataText(i, key);
    GetDataSizeText(i, type);
    sprintf(label, "%s (%s)", key,type);
    int index = m_AttList.AddString(label);
    m_AttList.SetItemData(index, i);
  }	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAttributeSelect::OnDblclkAttlist() 
{
  int index = m_AttList.GetCurSel();
  DWORD data = m_AttList.GetItemData(index);
  char keyword[50];
  GetDataText(data, keyword);
  attribute=keyword;
  EndDialog(IDOK);
}
