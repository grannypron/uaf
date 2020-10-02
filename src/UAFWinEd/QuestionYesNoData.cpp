/******************************************************************************
* Filename: QuestionYesNoData.cpp
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
#include "globtext.h"
#include "GlobalData.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "QuestionYesNoData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CQuestionYesNoData dialog


CQuestionYesNoData::CQuestionYesNoData(QUESTION_YES_NO& event, CWnd* pParent /*=NULL*/)
	: CDialog(CQuestionYesNoData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuestionYesNoData)
	m_NoText = _T("");
	m_Text = _T("");
	m_YesText = _T("");
	//}}AFX_DATA_INIT

  m_event = event;
}


void CQuestionYesNoData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuestionYesNoData)
	DDX_Control(pDX, IDC_YESCHAINOPTION, m_YesChainOption);
	DDX_Control(pDX, IDC_NOCHAINOPTION, m_NoChainOption);
	DDX_Control(pDX, IDC_CHOOSEART, m_SeeArt);
	DDX_Text(pDX, IDC_NOTEXT, m_NoText);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	DDX_Text(pDX, IDC_YESTEXT, m_YesText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuestionYesNoData, CDialog)
	//{{AFX_MSG_MAP(CQuestionYesNoData)
	ON_BN_CLICKED(IDC_CHOOSEART, OnChooseart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuestionYesNoData message handlers

BOOL CQuestionYesNoData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Text = m_event.text;
  m_YesText = m_event.text2;
  m_NoText = m_event.text3;
	
  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeArt.SetWindowText(name);

  int i;
  m_YesChainOption.ResetContent();
  for (i=0;i<NUM_LABEL_POST_CHAIN_OPTION_TYPES;i++)
    m_YesChainOption.AddString(ButtonPostChainOptionText[i]);   
  m_YesChainOption.SetCurSel((int)m_event.YesChainAction);

  m_NoChainOption.ResetContent();
  for (i=0;i<NUM_LABEL_POST_CHAIN_OPTION_TYPES;i++)
    m_NoChainOption.AddString(ButtonPostChainOptionText[i]);   
  m_NoChainOption.SetCurSel((int)m_event.NoChainAction);

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

  UpdateData(FALSE);
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuestionYesNoData::OnChooseart() 
{
   CSelectPicDlg dlg(m_event.pic, SmallPicDib | BigPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_event.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SeeArt.SetWindowText(name);
   }
}

void CQuestionYesNoData::OnOK() 
{
  UpdateData(TRUE);
  
  m_event.text = m_Text;
  m_event.text2 = m_YesText;
  m_event.text3 = m_NoText;

  m_event.YesChainAction = 
    (labelPostChainOptionsType) m_YesChainOption.GetCurSel(); 
  m_event.NoChainAction = 
    (labelPostChainOptionsType) m_NoChainOption.GetCurSel(); 

	CDialog::OnOK();
}

void CQuestionYesNoData::GetData(QUESTION_YES_NO& event)
{
	event = m_event;
}
