/******************************************************************************
* Filename: Campevnt.cpp
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
#include "CampEvnt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCampEvent dialog


CCampEvent::CCampEvent(CAMP_EVENT_DATA& camp, CWnd* pParent /*=NULL*/)
	: CDialog(CCampEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCampEvent)
	m_ForceBackup = FALSE;
	m_Text = _T("");
	//}}AFX_DATA_INIT

	m_camp = camp;
}


void CCampEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCampEvent)
	DDX_Control(pDX, IDC_CAMPART, m_CampArt);
	DDX_Check(pDX, IDC_BACKUP, m_ForceBackup);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCampEvent, CDialog)
	//{{AFX_MSG_MAP(CCampEvent)
	ON_BN_CLICKED(IDC_CAMPART, OnCampart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCampEvent message handlers

void CCampEvent::OnOK() 
{
	UpdateData(TRUE);	
	m_camp.ForceExit = m_ForceBackup;
	m_camp.text = m_Text;
	CDialog::OnOK();
}

void CCampEvent::OnCampart() 
{
   CSelectPicDlg dlg(m_camp.pic, SmallPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_camp.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_camp.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_CampArt.SetWindowText(name);
      //UpdateData(FALSE);
   }
}

BOOL CCampEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ForceBackup = m_camp.ForceExit;
	m_Text = m_camp.text;
	
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_camp.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_CampArt.SetWindowText(name);

   GotoDlgCtrl(GetDlgItem(IDC_TEXT));

	UpdateData(FALSE);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCampEvent::GetCampData(CAMP_EVENT_DATA& camp)
{
	camp = m_camp;
}
