/******************************************************************************
* Filename: PassTimeDlg.cpp
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
#include "GlobalData.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "PassTimeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPassTimeDlg dialog


CPassTimeDlg::CPassTimeDlg(PASS_TIME_EVENT_DATA& data, CWnd* pParent /*=NULL*/)
	: CDialog(CPassTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPassTimeDlg)
	m_Days = 0;
	m_Hours = 0;
	m_Minutes = 0;
	m_Text = _T("");
	m_AllowExit = FALSE;
	m_SetTime = FALSE;
	m_PassSilent = FALSE;
	//}}AFX_DATA_INIT

  m_event = data;
}


void CPassTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPassTimeDlg)
	DDX_Control(pDX, IDC_CHOOSEART, m_ChooseArt);
	DDX_Text(pDX, IDC_PASSDAYS, m_Days);
	DDX_Text(pDX, IDC_PASSHOURS, m_Hours);
	DDX_Text(pDX, IDC_PASSMINUTES, m_Minutes);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Check(pDX, IDC_ALLOWEXIT, m_AllowExit);
	DDX_Check(pDX, IDC_SETTIME, m_SetTime);
	DDX_Check(pDX, IDC_SILENTPASSING, m_PassSilent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CPassTimeDlg)
	ON_BN_CLICKED(IDC_CHOOSEART, OnChooseart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassTimeDlg message handlers

BOOL CPassTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_Days = m_event.Days;
  m_Hours = m_event.Hours;
  m_Minutes = m_event.Minutes;
  m_Text = m_event.text;
  m_AllowExit = m_event.AllowStop;
  m_SetTime = m_event.SetTime;
  m_PassSilent=m_event.PassSilent;
   
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_ChooseArt.SetWindowText(name);

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

  UpdateData(FALSE);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPassTimeDlg::OnOK() 
{
  UpdateData(TRUE);

  m_Days = max(0, min(m_Days, 250));
  m_Hours = max(0, min(m_Hours, 23));
  m_Minutes = max(0, min(m_Minutes, 59));

  m_event.text = m_Text;
  m_event.Days = m_Days;
  m_event.Hours = m_Hours;
  m_event.Minutes = m_Minutes;
  m_event.AllowStop = m_AllowExit;
  m_event.SetTime = m_SetTime;
  m_event.PassSilent = m_PassSilent;
   	
	CDialog::OnOK();
}

void CPassTimeDlg::OnChooseart() 
{
   CSelectPicDlg dlg(m_event.pic, SmallPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_event.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
   }
}
