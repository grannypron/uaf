/******************************************************************************
* Filename: QuestEventData.cpp
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
#include "globtext.h"
#include "GlobalData.h"
#include "GameEvent.h"
#include "SelectPicDlg.h"
#include "ItemKeyQuestDlg.h"

#include "QuestEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQuestEventData dialog


CQuestEventData::CQuestEventData(QUEST_EVENT_DATA data, CWnd* pParent /*=NULL*/)
	: CDialog(CQuestEventData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQuestEventData)
	m_CompleteOnAccept = FALSE;
	m_FailOnReject = FALSE;
	m_Stage = 0;
	m_Text = _T("");
	//}}AFX_DATA_INIT

  m_data = data;
}


void CQuestEventData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQuestEventData)
	DDX_Control(pDX, IDC_ACCEPT, m_Accept);
	DDX_Control(pDX, IDC_QUESTSELECT, m_Quest);
	DDX_Control(pDX, IDC_ART, m_SeeArt);
	DDX_Check(pDX, IDC_COMPLETEONACCEPT, m_CompleteOnAccept);
	DDX_Check(pDX, IDC_FAILONREJECT, m_FailOnReject);
	DDX_Text(pDX, IDC_STAGE, m_Stage);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQuestEventData, CDialog)
	//{{AFX_MSG_MAP(CQuestEventData)
	ON_BN_CLICKED(IDC_ART, OnArt)
	ON_BN_CLICKED(IDC_QUESTSELECT, OnQuestselect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQuestEventData message handlers

BOOL CQuestEventData::OnInitDialog() 
{
  QUEST_TYPE questType;

	CDialog::OnInitDialog();
	
  m_Text = m_data.text;
  m_CompleteOnAccept = m_data.completeOnAccept;
  m_FailOnReject = m_data.failOnRejection;
  m_Stage = m_data.stage;

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
  m_SeeArt.SetWindowText(name);

  questType = m_data.QuestType();
  switch(questType)
  {
  case KEY_FLAG:
    getBaseName(globalData.questData.GetName(m_data.QuestID()), name, MAX_MEDITBUTTON_TEXT);
    break;
  case NO_FLAG:
    strcpy(name, "No Quest");
    break;
  case QUEST_FLAG:
    getBaseName(globalData.questData.GetName(m_data.QuestID()), name, MAX_MEDITBUTTON_TEXT);
    break;
  case ITEM_FLAG:
    getBaseName(GetSpecialItemName(m_data.QuestID()), name, MAX_MEDITBUTTON_TEXT);      
    break;
  default:
    MsgBoxInfo("Un-implemented quest type");
    name[0] = 0;
  };
  m_Quest.SetWindowText(name);

  m_Accept.ResetContent();

  for (int i=0;i<MAX_QUEST_ACCEPT_TYPES;i++)
    m_Accept.AddString(questAcceptText[i]);

  m_Accept.SetCurSel((int)m_data.accept);

  GotoDlgCtrl(GetDlgItem(IDC_TEXT));

  UpdateData(FALSE);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQuestEventData::OnArt() 
{
   CSelectPicDlg dlg(m_data.pic, SmallPicDib);

   if (dlg.DoModal() == IDOK)
   {
      dlg.GetData(m_data.pic);
      char name[MAX_MEDITBUTTON_TEXT+1];
      getBaseName(m_data.pic.filename, name, MAX_MEDITBUTTON_TEXT);
      m_SeeArt.SetWindowText(name);
   }
}

void CQuestEventData::OnQuestselect() 
{
  CItemKeyQuestDlg dlg(QUEST_FLAG | KEY_FLAG | ITEM_FLAG);

	if (dlg.DoModal() == IDOK)
  {
    m_data.SetQuestTypeAndID(dlg.m_type, dlg.m_selectedID);
    char name[MAX_MEDITBUTTON_TEXT+1];
    switch(m_data.QuestType())
    {
    case QUEST_FLAG:
      getBaseName(globalData.questData.GetName(m_data.QuestID()), name, MAX_MEDITBUTTON_TEXT);
      break;
    case ITEM_FLAG:
 	    getBaseName(GetSpecialItemName(m_data.QuestID()), name, MAX_MEDITBUTTON_TEXT);      
      break;
    case KEY_FLAG:
      getBaseName(GetSpecialKeyName(m_data.QuestID()), name, MAX_MEDITBUTTON_TEXT);
      break;
    default:
      MsgBoxInfo("Quest type not implemented");
    };
    m_Quest.SetWindowText(name);
  }
}

void CQuestEventData::OnOK() 
{
  UpdateData(TRUE);

  m_data.accept = (QuestAcceptType)m_Accept.GetCurSel();
  m_data.text = m_Text;
  m_data.completeOnAccept = m_CompleteOnAccept;
  m_data.failOnRejection = m_FailOnReject;
  m_data.stage = m_Stage;
	
	CDialog::OnOK();
}
