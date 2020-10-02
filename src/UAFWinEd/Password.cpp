/******************************************************************************
* Filename: Password.cpp
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
//#include "globtext.h"
//#include "GlobalData.h"
#include "items.h"
#include "char.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "TeleportDlg.h"
#include "Password.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPassword dialog


CPassword::CPassword(PASSWORD_DATA& password, CWnd* pParent /*=NULL*/)
: CDialog(CPassword::IDD, pParent)
{
   //{{AFX_DATA_INIT(CPassword)
   m_FailText = _T("");
   m_NbrTries = 1;
   m_Password = _T("");
   m_SuccessText = _T("");
   m_Text = _T("");
	m_UseCase = FALSE;
	//}}AFX_DATA_INIT
   
   m_password = password;
}


void CPassword::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CPassword)
	DDX_Control(pDX, IDC_MATCHCOMBO, m_MatchCriteria);
	DDX_Control(pDX, IDC_FAILTELEPORT, m_FailTeleport);
	DDX_Control(pDX, IDC_SUCCESSTELEPORT, m_SuccessTeleport);
   DDX_Control(pDX, IDC_SUCCESSACTION, m_SuccessAction);
   DDX_Control(pDX, IDC_FAILACTION, m_FailAction);
   DDX_Control(pDX, IDC_CHOOSEART, m_ChooseArt);
   DDX_Text(pDX, IDC_FAILTEXT, m_FailText);
   DDX_Text(pDX, IDC_NBRTRIES, m_NbrTries);
   DDX_Text(pDX, IDC_PASSWORD, m_Password);
   DDX_Text(pDX, IDC_SUCCESSTEXT, m_SuccessText);
   DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Check(pDX, IDC_USECASE, m_UseCase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPassword, CDialog)
//{{AFX_MSG_MAP(CPassword)
ON_BN_CLICKED(IDC_CHOOSEART, OnChooseart)
ON_CBN_SELCHANGE(IDC_FAILACTION, OnSelchangeFailaction)
ON_CBN_SELCHANGE(IDC_SUCCESSACTION, OnSelchangeSuccessaction)
	ON_BN_CLICKED(IDC_FAILTELEPORT, OnFailteleport)
	ON_BN_CLICKED(IDC_SUCCESSTELEPORT, OnSuccessteleport)
	ON_CBN_SELCHANGE(IDC_MATCHCOMBO, OnSelchangeMatchcombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPassword message handlers

BOOL CPassword::OnInitDialog() 
{
   int i;
   CDialog::OnInitDialog();
   
   m_NbrTries = max(1, m_password.nbrTries);
   
   m_FailText = m_password.text3;
   m_SuccessText = m_password.text2;
   m_Password = m_password.password;
   m_Text = m_password.text;
   m_UseCase = m_password.matchCase;
   
   m_SuccessAction.ResetContent();
   m_FailAction.ResetContent();
   m_MatchCriteria.ResetContent();
   
   for (i=0;i<NUM_PASSACTION_TYPES;i++)
   {
      m_SuccessAction.AddString(PasswordActionText[i]);
      m_FailAction.AddString(PasswordActionText[i]);
   }
   
   for (i=0;i<NUM_PASSMATCH_TYPES;i++)
      m_MatchCriteria.AddString(PasswordMatchText[i]);

   m_SuccessAction.SetCurSel((int)m_password.successAction);
   m_FailAction.SetCurSel((int)m_password.failAction);
   m_MatchCriteria.SetCurSel((int)m_password.matchCriteria);
   
   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_password.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseArt.SetWindowText(name);

   GotoDlgCtrl(GetDlgItem(IDC_TEXT));

   UpdateData(FALSE);
   SetControlStates();
   
   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

void CPassword::OnChooseart() 
{
   CSelectPicDlg dlg(m_password.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_password.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_password.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
      //UpdateData(FALSE);
   }
}


void CPassword::OnSelchangeFailaction() 
{
   UpdateData(TRUE);
   
   m_password.failAction = 
      (passwordActionType) m_FailAction.GetCurSel();
   
   SetControlStates();
   UpdateData(FALSE);
}

void CPassword::OnSelchangeSuccessaction() 
{
   UpdateData(TRUE);
   
   m_password.successAction = 
      (passwordActionType) m_SuccessAction.GetCurSel();
   
   SetControlStates();
   UpdateData(FALSE);
}

void CPassword::OnOK() 
{
   UpdateData(TRUE);
   
   m_password.nbrTries = max(1, m_NbrTries);

   m_password.text = m_Text;  
   m_password.text3 = m_FailText;
   m_password.text2 = m_SuccessText;

   m_password.password = m_Password;
   
   m_password.successAction = 
      (passwordActionType) m_SuccessAction.GetCurSel();
   m_password.failAction =
      (passwordActionType) m_FailAction.GetCurSel();
   m_password.matchCriteria = 
     (passwordMatchType) m_MatchCriteria.GetCurSel();
   m_password.matchCase = m_UseCase;
   
   CDialog::OnOK();
}

void CPassword::GetPasswordData(PASSWORD_DATA& password)
{
   password = m_password;
}

void CPassword::SetControlStates(void)
{
   m_SuccessTeleport.EnableWindow(FALSE);
   m_FailTeleport.EnableWindow(FALSE);
   
   switch (m_password.successAction) {
   case ChainEvent:
      break;
   case Teleport:
      m_SuccessTeleport.EnableWindow(TRUE);
      break;
   default:
      break;
   }
   
   switch (m_password.failAction) {
   case ChainEvent:
      break;
   case Teleport:
      m_FailTeleport.EnableWindow(TRUE);
      break;
   default:
      break;
   }
}

void CPassword::OnFailteleport() 
{
	CTeleportDlg dlg(m_password.failTransferData);

  if (dlg.DoModal() == IDOK)
    dlg.GetData(m_password.failTransferData);
}

void CPassword::OnSuccessteleport() 
{
	CTeleportDlg dlg(m_password.successTransferData);

  if (dlg.DoModal() == IDOK)
    dlg.GetData(m_password.successTransferData);
}

void CPassword::OnSelchangeMatchcombo() 
{
   UpdateData(TRUE);
   
   m_password.matchCriteria = 
     (passwordMatchType) m_MatchCriteria.GetCurSel();
   
   SetControlStates();
   UpdateData(FALSE);	
}
