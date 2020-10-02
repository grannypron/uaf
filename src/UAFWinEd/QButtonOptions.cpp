/******************************************************************************
* Filename: QButtonOptions.cpp
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
#include "QButtonOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CQButtonOptions dialog


CQButtonOptions::CQButtonOptions(QUESTION_BUTTON_DATA& event, CWnd* pParent /*=NULL*/)
	: CDialog(CQButtonOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CQButtonOptions)
	m_Label1 = _T("");
	m_Label2 = _T("");
	m_Label3 = _T("");
	m_Label4 = _T("");
	m_Label5 = _T("");
	m_Present1 = FALSE;
	m_Present2 = FALSE;
	m_Present3 = FALSE;
	m_Present4 = FALSE;
	m_Present5 = FALSE;
	m_Text = _T("");
	//}}AFX_DATA_INIT

  m_event = event;
}


void CQButtonOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQButtonOptions)
	DDX_Control(pDX, IDC_SEEART, m_SeeArt);
	DDX_Control(pDX, IDC_BUTTONOPTION5, m_AfterChain5);
	DDX_Control(pDX, IDC_BUTTONOPTION4, m_AfterChain4);
	DDX_Control(pDX, IDC_BUTTONOPTION3, m_AfterChain3);
	DDX_Control(pDX, IDC_BUTTONOPTION2, m_AfterChain2);
	DDX_Control(pDX, IDC_BUTTONOPTION1, m_AfterChain1);
	DDX_Text(pDX, IDC_LABEL1, m_Label1);
	DDX_Text(pDX, IDC_LABEL2, m_Label2);
	DDX_Text(pDX, IDC_LABEL3, m_Label3);
	DDX_Text(pDX, IDC_LABEL4, m_Label4);
	DDX_Text(pDX, IDC_LABEL5, m_Label5);
	DDX_Check(pDX, IDC_PRESENT1, m_Present1);
	DDX_Check(pDX, IDC_PRESENT2, m_Present2);
	DDX_Check(pDX, IDC_PRESENT3, m_Present3);
	DDX_Check(pDX, IDC_PRESENT4, m_Present4);
	DDX_Check(pDX, IDC_PRESENT5, m_Present5);
	DDX_Text(pDX, IDC_TEXT, m_Text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQButtonOptions, CDialog)
	//{{AFX_MSG_MAP(CQButtonOptions)
	ON_BN_CLICKED(IDC_SEEART, OnSeeart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQButtonOptions message handlers

void CQButtonOptions::OnSeeart() 
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


BOOL CQButtonOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
   m_Text = m_event.text;
   m_Label1 = m_event.buttons.buttons[0].label;
   m_Label2 = m_event.buttons.buttons[1].label;
   m_Label3 = m_event.buttons.buttons[2].label;
   m_Label4 = m_event.buttons.buttons[3].label;
   m_Label5 = m_event.buttons.buttons[4].label;
   
   m_Present1 = m_event.buttons.buttons[0].present;
   m_Present2 = m_event.buttons.buttons[1].present;
   m_Present3 = m_event.buttons.buttons[2].present;
   m_Present4 = m_event.buttons.buttons[3].present;
   m_Present5 = m_event.buttons.buttons[4].present;

   char name[MAX_MEDITBUTTON_TEXT+1];
   getBaseName(m_event.pic.filename, name, MAX_MEDITBUTTON_TEXT);
   m_SeeArt.SetWindowText(name);

   int i;
   m_AfterChain1.ResetContent();
   for (i=0;i<NUM_LABEL_POST_CHAIN_OPTION_TYPES;i++)
      m_AfterChain1.AddString(ButtonPostChainOptionText[i]);   
   m_AfterChain1.SetCurSel((int)m_event.buttons.buttons[0].postChainAction);

   m_AfterChain2.ResetContent();
   for (i=0;i<NUM_LABEL_POST_CHAIN_OPTION_TYPES;i++)
      m_AfterChain2.AddString(ButtonPostChainOptionText[i]);   
   m_AfterChain2.SetCurSel((int)m_event.buttons.buttons[1].postChainAction);

   m_AfterChain3.ResetContent();
   for (i=0;i<NUM_LABEL_POST_CHAIN_OPTION_TYPES;i++)
      m_AfterChain3.AddString(ButtonPostChainOptionText[i]);   
   m_AfterChain3.SetCurSel((int)m_event.buttons.buttons[2].postChainAction);

   m_AfterChain4.ResetContent();
   for (i=0;i<NUM_LABEL_POST_CHAIN_OPTION_TYPES;i++)
      m_AfterChain4.AddString(ButtonPostChainOptionText[i]);   
   m_AfterChain4.SetCurSel((int)m_event.buttons.buttons[3].postChainAction);

   m_AfterChain5.ResetContent();
   for (i=0;i<NUM_LABEL_POST_CHAIN_OPTION_TYPES;i++)
      m_AfterChain5.AddString(ButtonPostChainOptionText[i]);   
   m_AfterChain5.SetCurSel((int)m_event.buttons.buttons[4].postChainAction);
   
   GotoDlgCtrl(GetDlgItem(IDC_TEXT));

   UpdateData(FALSE);
   
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CQButtonOptions::OnOK() 
{
   UpdateData(TRUE);

   m_event.text = m_Text;

   if (    ((m_Label1.GetLength() == 0) || !m_Present1)
        && ((m_Label2.GetLength() == 0) || !m_Present2)
        && ((m_Label3.GetLength() == 0) || !m_Present3)
        && ((m_Label4.GetLength() == 0) || !m_Present4)
        && ((m_Label5.GetLength() == 0) || !m_Present5)
      )
   {
     MessageBox("No buttons were defined.  We added a default 'Exit' button for you.", "Warning", MB_OK);
     m_Present1 = TRUE;
     if (m_Label1.GetLength() == 0) m_Label1 = "Exit";
   };

   m_event.buttons.buttons[0].label = m_Label1;
   m_event.buttons.buttons[1].label = m_Label2;
   m_event.buttons.buttons[2].label = m_Label3;
   m_event.buttons.buttons[3].label = m_Label4;
   m_event.buttons.buttons[4].label = m_Label5;

   m_event.buttons.buttons[0].present = m_Present1;
   m_event.buttons.buttons[1].present = m_Present2;
   m_event.buttons.buttons[2].present = m_Present3;
   m_event.buttons.buttons[3].present = m_Present4;
   m_event.buttons.buttons[4].present = m_Present5;
	
   m_event.buttons.buttons[0].postChainAction = 
      (labelPostChainOptionsType) m_AfterChain1.GetCurSel(); 
   m_event.buttons.buttons[1].postChainAction = 
      (labelPostChainOptionsType) m_AfterChain2.GetCurSel(); 
   m_event.buttons.buttons[2].postChainAction = 
      (labelPostChainOptionsType) m_AfterChain3.GetCurSel(); 
   m_event.buttons.buttons[3].postChainAction = 
      (labelPostChainOptionsType) m_AfterChain4.GetCurSel(); 
   m_event.buttons.buttons[4].postChainAction = 
      (labelPostChainOptionsType) m_AfterChain5.GetCurSel(); 

   m_event.buttons.numListButtons = 0;

   for (int i=0;i<MAX_BUTTONS;i++)
   {
     if (   (strlen(m_event.buttons.buttons[i].label) > 0)
         && (m_event.buttons.buttons[i].present))
       m_event.buttons.numListButtons++;
   }
   
	CDialog::OnOK();
}

void CQButtonOptions::GetData(QUESTION_BUTTON_DATA& event)
{
	event = m_event;
}
