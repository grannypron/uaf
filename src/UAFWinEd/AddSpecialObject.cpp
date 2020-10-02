/******************************************************************************
* Filename: AddSpecialObject.cpp
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
#include "EventViewer.h"
#include "AddSpecialObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAddSpecialObject dialog


CAddSpecialObject::CAddSpecialObject(CString name, DWORD ExEvent, CString ExLabel, BOOL drop, WORD type, WORD mask, CWnd* pParent /*=NULL*/)
	: CDialog(CAddSpecialObject::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddSpecialObject)
	m_Type = 0;
	m_Name = _T("");
	m_ExamineLabel = _T("");
	m_CanBeDropped = FALSE;
	//}}AFX_DATA_INIT
  m_CanBeDropped=drop;
  m_Name = name;
  m_Type = type;
  m_Mask = mask;
  m_Event = ExEvent;
  m_ExamineLabel = ExLabel;
  if (m_ExamineLabel.GetLength()==0) m_ExamineLabel="EXAMINE";
  if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');  
}

void CAddSpecialObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddSpecialObject)
	DDX_Control(pDX, IDC_EXAMINEEVENT, m_Examine);
	DDX_Radio(pDX, IDC_SPECIALITEM, m_Type);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_EXAMINELABEL, m_ExamineLabel);
	DDX_Check(pDX, IDC_CANBEDROPPED, m_CanBeDropped);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddSpecialObject, CDialog)
	//{{AFX_MSG_MAP(CAddSpecialObject)
	ON_BN_CLICKED(IDC_EXAMINEEVENT, OnExamineevent)
	ON_BN_CLICKED(IDC_QUEST, OnQuest)
	ON_BN_CLICKED(IDC_SPECIALITEM, OnSpecialitem)
	ON_BN_CLICKED(IDC_SPECIALKEY, OnSpecialkey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddSpecialObject message handlers

BOOL CAddSpecialObject::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  if ((m_Type & m_Mask) == 0)
  {
    if ((m_Mask & ITEM_FLAG) == ITEM_FLAG)
      m_Type = ITEM_FLAG;
    else if ((m_Mask & KEY_FLAG) == KEY_FLAG)
      m_Type = KEY_FLAG;
    else if ((m_Mask & QUEST_FLAG) == QUEST_FLAG)
      m_Type = QUEST_FLAG;
  }

  char name[MAX_MEDITBUTTON_TEXT+1];
  getBaseName(GetEventIdDescription(m_Event, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
  m_Examine.SetWindowText(name);
  
  switch (m_Type)
  {
  case ITEM_FLAG:
    m_Type = 0;
    break;
  case KEY_FLAG:
    m_Type = 1;
    break;
  case QUEST_FLAG:
    {
      m_Type = 2;
      m_CanBeDropped=FALSE;
      CWnd *pWnd = GetDlgItem(IDC_CANBEDROPPED);
      if (pWnd != NULL) pWnd->EnableWindow(FALSE);
      pWnd = GetDlgItem(IDC_EXAMINELABEL);
      if (pWnd != NULL) pWnd->EnableWindow(FALSE);
      pWnd = GetDlgItem(IDC_EXAMINEEVENT);
      if (pWnd != NULL) pWnd->EnableWindow(FALSE);
      ASSERT( m_Event == 0 );
    }
    break;
  }

  if ((m_Mask & ITEM_FLAG) != ITEM_FLAG)
  {
    CWnd *pWnd = GetDlgItem(IDC_SPECIALITEM);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
  }

  if ((m_Mask & KEY_FLAG) != KEY_FLAG)
  {
    CWnd *pWnd = GetDlgItem(IDC_SPECIALKEY);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
  }

  if ((m_Mask & QUEST_FLAG) != QUEST_FLAG)
  {
    CWnd *pWnd = GetDlgItem(IDC_QUEST);
    if (pWnd != NULL)
      pWnd->EnableWindow(FALSE);
  }

  UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddSpecialObject::OnOK() 
{
  UpdateData(TRUE);

  switch (m_Type)
  {
  case 0:
    m_Mask = ITEM_FLAG;
    break;
  case 1:
    m_Mask = KEY_FLAG;
    break;
  case 2:
    m_Mask = QUEST_FLAG;
    break;
  }
  if (m_ExamineLabel.GetLength()>10) m_ExamineLabel.SetAt(10,'\0');  
	CDialog::OnOK();
}

void CAddSpecialObject::OnExamineevent() 
{
  UpdateData(TRUE);
  if (m_Type == 2) return; // no event for quests

  CEventViewer dlg("Special Object Event", m_Event);
  if (dlg.DoModal()==IDOK)
  {
    m_Event = dlg.m_SingleEventRoot;
    char name[MAX_MEDITBUTTON_TEXT+1];
    getBaseName(GetEventIdDescription(m_Event, GlobalEventSrc), name, MAX_MEDITBUTTON_TEXT);
    m_Examine.SetWindowText(name);
  }		
}

void CAddSpecialObject::OnCancel() 
{
	CDialog::OnCancel();
}

void CAddSpecialObject::OnQuest() 
{
  m_CanBeDropped=FALSE;
  CWnd *pWnd = GetDlgItem(IDC_CANBEDROPPED);
  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_EXAMINELABEL);
  if (pWnd != NULL) pWnd->EnableWindow(FALSE);
  pWnd = GetDlgItem(IDC_EXAMINEEVENT);
  if (pWnd != NULL) pWnd->EnableWindow(FALSE);	
}

void CAddSpecialObject::OnSpecialitem() 
{
  CWnd *pWnd = GetDlgItem(IDC_EXAMINELABEL);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
  pWnd = GetDlgItem(IDC_EXAMINEEVENT);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);	
  pWnd = GetDlgItem(IDC_CANBEDROPPED);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
}

void CAddSpecialObject::OnSpecialkey() 
{
  CWnd *pWnd = GetDlgItem(IDC_EXAMINELABEL);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
  pWnd = GetDlgItem(IDC_EXAMINEEVENT);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);		
  pWnd = GetDlgItem(IDC_CANBEDROPPED);
  if (pWnd != NULL) pWnd->EnableWindow(TRUE);
}
