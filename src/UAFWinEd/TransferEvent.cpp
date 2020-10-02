/******************************************************************************
* Filename: TransferEvent.cpp
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
#include "PicData.h"
#include "SelectPicDlg.h"
#include "Char.h"
//#include "GlobalData.h"
#include "GameEvent.h"

#include "TeleportDlg.h"
#include "TransferEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransferEvent dialog


CTransferEvent::CTransferEvent(TRANSFER_EVENT_DATA &data, BOOL allowLevel, CWnd* pParent /*=NULL*/)
	: CDialog(CTransferEvent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransferEvent)
	m_AskYesNo = FALSE;
	m_QuestionText = _T("");
	m_YesTransfer = 0;
	m_DestroyDrow = FALSE;
	m_TransferText = _T("");
	m_AttemptsEntry = 1;
	//}}AFX_DATA_INIT

   m_AllowLevel = allowLevel;
   m_data = data;
}

void CTransferEvent::GetData(TRANSFER_EVENT_DATA &data)
{
   data = m_data;
}

void CTransferEvent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferEvent)
	DDX_Control(pDX, IDC_CHOOSESPIC, m_ChooseArt);
	DDX_Check(pDX, IDC_ASKYESNOQ, m_AskYesNo);
	DDX_Text(pDX, IDC_QUESTIONTEXT, m_QuestionText);
	DDX_Radio(pDX, IDC_YESTRANSFER, m_YesTransfer);
	DDX_Check(pDX, IDC_DESTROYDROW, m_DestroyDrow);
	DDX_Text(pDX, IDC_TRANSFERTEXT, m_TransferText);
	DDX_Radio(pDX, IDC_ATTEMPTSENTRY, m_AttemptsEntry);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferEvent, CDialog)
	//{{AFX_MSG_MAP(CTransferEvent)
	ON_BN_CLICKED(IDC_CHOOSESPIC, OnChoosespic)
	ON_BN_CLICKED(IDC_ASKYESNOQ, OnAskyesnoq)
	ON_BN_CLICKED(IDC_TELEPORT, OnTeleport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransferEvent message handlers

void CTransferEvent::OnChoosespic() 
{
   CSelectPicDlg dlg(m_data.pic, SmallPicDib | BigPicDib | SpriteDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_data.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_ChooseArt.SetWindowText(name);
      //UpdateData(FALSE);
   }
}


BOOL CTransferEvent::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   m_QuestionText = m_data.text;
   m_TransferText = m_data.text2;
   m_AskYesNo = m_data.askYesNo;
   m_YesTransfer = !m_data.transferOnYes;
   m_DestroyDrow = m_data.destroyDrow;

   if (m_data.activateBeforeEntry)
      m_AttemptsEntry = 0;
   else
      m_AttemptsEntry = 1;

   CWnd *pWnd;

   BOOL enable;

   if (m_AskYesNo)
      enable = TRUE;
   else
      enable = FALSE;

   pWnd = GetDlgItem(IDC_YESTRANSFER);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);

   pWnd = GetDlgItem(IDC_NOTRANSFER);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);

   //pWnd = GetDlgItem(IDC_CHOOSESPIC);
   //if (pWnd != NULL)
   //   pWnd->EnableWindow(enable);

   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_ChooseArt.SetWindowText(name);

  GotoDlgCtrl(GetDlgItem(IDC_QUESTIONTEXT));

  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferEvent::OnOK() 
{
	CDialog::OnOK();

   m_data.text = m_QuestionText;
   m_data.text2 = m_TransferText;
   m_data.askYesNo = m_AskYesNo;
   m_data.destroyDrow = m_DestroyDrow;
   m_data.activateBeforeEntry = !m_AttemptsEntry;
   m_data.transferOnYes = !m_YesTransfer;

   //if (!m_data.askYesNo)
   //   m_data.pic.filename="";
}

void CTransferEvent::OnAskyesnoq() 
{
   UpdateData(TRUE);
   
   BOOL enable;

   if (m_AskYesNo)
      enable = TRUE;
   else
      enable = FALSE;

   CWnd *pWnd = GetDlgItem(IDC_YESTRANSFER);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);

   pWnd = GetDlgItem(IDC_NOTRANSFER);
   if (pWnd != NULL)
      pWnd->EnableWindow(enable);

   //pWnd = GetDlgItem(IDC_CHOOSESPIC);
   //if (pWnd != NULL)
   //   pWnd->EnableWindow(enable);
}

void CTransferEvent::OnTeleport() 
{
  CTeleportDlg dlg(m_data.transferData); // m_AllowLevel?

  if (dlg.DoModal() == IDOK)
    dlg.GetData(m_data.transferData);
}
