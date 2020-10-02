/******************************************************************************
* Filename: SmallTownEvent.cpp
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
//#include "GlobalData.h"
#include "Char.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"

#include "SmallTownEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmallTownEvent dialog


CSmallTownEvent::CSmallTownEvent(SMALL_TOWN_DATA &event, CWnd* pParent /*=NULL*/)
	: CDialog(CSmallTownEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSmallTownEvent)
	m_Text = _T("");
	//}}AFX_DATA_INIT
  m_event = event;
}


void CSmallTownEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSmallTownEvent)
	DDX_Control(pDX, IDC_TOWNART, m_SeeArt);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSmallTownEvent, CDialog)
	//{{AFX_MSG_MAP(CSmallTownEvent)
	ON_BN_CLICKED(IDC_TOWNART, OnTownart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmallTownEvent message handlers

void CSmallTownEvent::OnTownart() 
{
  CSelectPicDlg dlg(m_event.pic, SmallPicDib);

  if (dlg.DoModal() == IDOK)
  {
    dlg.GetData(m_event.pic);
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
    m_SeeArt.SetWindowText(name);
  }	
}

BOOL CSmallTownEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();

  m_Text = m_event.text;

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeArt.SetWindowText(name);

  UpdateData(FALSE);
  GotoDlgCtrl(GetDlgItem(IDC_TEXT));
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSmallTownEvent::OnOK() 
{
  UpdateData(TRUE);
  m_event.text = m_Text;
	CDialog::OnOK();
}
