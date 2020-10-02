/******************************************************************************
* Filename: VaultEvent.cpp
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
#include "Char.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "VaultEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVaultEvent dialog


CVaultEvent::CVaultEvent(VAULT_EVENT_DATA& event, CWnd* pParent /*=NULL*/)
	: CDialog(CVaultEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVaultEvent)
	m_ForceBackup = FALSE;
	m_Text = _T("");
	//}}AFX_DATA_INIT

  m_event = event;
}


void CVaultEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVaultEvent)
	DDX_Control(pDX, IDC_WHICHVAULT, m_WhichVault);
	DDX_Control(pDX, IDC_CHOOSEART, m_SeeArt);
	DDX_Check(pDX, IDC_FORCEBACKUP, m_ForceBackup);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVaultEvent, CDialog)
	//{{AFX_MSG_MAP(CVaultEvent)
	ON_BN_CLICKED(IDC_CHOOSEART, OnChooseart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVaultEvent message handlers

void CVaultEvent::OnChooseart() 
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

BOOL CVaultEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeArt.SetWindowText(name);
  
  m_Text = m_event.text;
  m_ForceBackup = m_event.ForceBackup;	

  for (int i=0;i<MAX_GLOBAL_VAULTS;i++)
  {
    CString tmp;
    tmp.Format("%i", i+1);
    m_WhichVault.AddString(tmp);
  }

  m_WhichVault.SetCurSel(m_event.WhichVault);

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));
  UpdateData(FALSE);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVaultEvent::OnOK() 
{
  UpdateData(TRUE);
  
  m_event.text = m_Text;
  m_event.ForceBackup = m_ForceBackup;	
  m_event.WhichVault = m_WhichVault.GetCurSel();

	CDialog::OnOK();
}

void CVaultEvent::GetData(VAULT_EVENT_DATA& event)
{
	event = m_event;
}