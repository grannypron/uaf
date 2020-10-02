/******************************************************************************
* Filename: Buttons.cpp
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
#include "Buttons.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CButtons dialog


CButtons::CButtons(ENCOUNTER_BUTTON_DATA& buttons, CWnd* pParent /*=NULL*/)
: CDialog(CButtons::IDD, pParent)
{
   //{{AFX_DATA_INIT(CButtons)
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
   m_UpClose1 = FALSE;
   m_UpClose2 = FALSE;
   m_UpClose3 = FALSE;
   m_UpClose4 = FALSE;
   m_UpClose5 = FALSE;
	m_OnlyUpClose1 = FALSE;
	m_OnlyUpClose2 = FALSE;
	m_OnlyUpClose3 = FALSE;
	m_OnlyUpClose4 = FALSE;
	m_OnlyUpClose5 = FALSE;
	//}}AFX_DATA_INIT
   
   m_buttons = buttons;
}


void CButtons::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CButtons)
   DDX_Control(pDX, IDC_BUTTONOPTION5, m_Option5);
   DDX_Control(pDX, IDC_BUTTONOPTION4, m_Option4);
   DDX_Control(pDX, IDC_BUTTONOPTION3, m_Option3);
   DDX_Control(pDX, IDC_BUTTONOPTION2, m_Option2);
   DDX_Control(pDX, IDC_BUTTONOPTION1, m_Option1);
   DDX_Text(pDX, IDC_LABEL1, m_Label1);
   DDV_MaxChars(pDX, m_Label1, 10);
   DDX_Text(pDX, IDC_LABEL2, m_Label2);
   DDV_MaxChars(pDX, m_Label2, 10);
   DDX_Text(pDX, IDC_LABEL3, m_Label3);
   DDV_MaxChars(pDX, m_Label3, 10);
   DDX_Text(pDX, IDC_LABEL4, m_Label4);
   DDV_MaxChars(pDX, m_Label4, 10);
   DDX_Text(pDX, IDC_LABEL5, m_Label5);
   DDV_MaxChars(pDX, m_Label5, 10);
   DDX_Check(pDX, IDC_PRESENT1, m_Present1);
   DDX_Check(pDX, IDC_PRESENT2, m_Present2);
   DDX_Check(pDX, IDC_PRESENT3, m_Present3);
   DDX_Check(pDX, IDC_PRESENT4, m_Present4);
   DDX_Check(pDX, IDC_PRESENT5, m_Present5);
   DDX_Check(pDX, IDC_UPCLOSE1, m_UpClose1);
   DDX_Check(pDX, IDC_UPCLOSE2, m_UpClose2);
   DDX_Check(pDX, IDC_UPCLOSE3, m_UpClose3);
   DDX_Check(pDX, IDC_UPCLOSE4, m_UpClose4);
   DDX_Check(pDX, IDC_UPCLOSE5, m_UpClose5);
	DDX_Check(pDX, IDC_ONLYUPCLOSE1, m_OnlyUpClose1);
	DDX_Check(pDX, IDC_ONLYUPCLOSE2, m_OnlyUpClose2);
	DDX_Check(pDX, IDC_ONLYUPCLOSE3, m_OnlyUpClose3);
	DDX_Check(pDX, IDC_ONLYUPCLOSE4, m_OnlyUpClose4);
	DDX_Check(pDX, IDC_ONLYUPCLOSE5, m_OnlyUpClose5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CButtons, CDialog)
//{{AFX_MSG_MAP(CButtons)
ON_CBN_SELCHANGE(IDC_BUTTONOPTION1, OnSelchangeButtonoption1)
ON_CBN_SELCHANGE(IDC_BUTTONOPTION2, OnSelchangeButtonoption2)
ON_CBN_SELCHANGE(IDC_BUTTONOPTION3, OnSelchangeButtonoption3)
ON_CBN_SELCHANGE(IDC_BUTTONOPTION4, OnSelchangeButtonoption4)
ON_CBN_SELCHANGE(IDC_BUTTONOPTION5, OnSelchangeButtonoption5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtons message handlers


BOOL CButtons::OnInitDialog() 
{
   int i;
   CDialog::OnInitDialog();
   
   m_Option1.ResetContent();
   
   for (i=0;i<NUM_ENCOUNTER_BUTTON_OPTION_TYPES;i++)
      m_Option1.AddString(ButtonOptionText[i]);
   
   m_Option1.SetCurSel((int)m_buttons.buttons[0].optionResult);
   
   m_Option2.ResetContent();
   
   for (i=0;i<NUM_ENCOUNTER_BUTTON_OPTION_TYPES;i++)
      m_Option2.AddString(ButtonOptionText[i]);
   
   m_Option2.SetCurSel((int)m_buttons.buttons[1].optionResult);
   
   m_Option3.ResetContent();
   
   for (i=0;i<NUM_ENCOUNTER_BUTTON_OPTION_TYPES;i++)
      m_Option3.AddString(ButtonOptionText[i]);
   
   m_Option3.SetCurSel((int)m_buttons.buttons[2].optionResult);
   
   m_Option4.ResetContent();
   
   for (i=0;i<NUM_ENCOUNTER_BUTTON_OPTION_TYPES;i++)
      m_Option4.AddString(ButtonOptionText[i]);
   
   m_Option4.SetCurSel((int)m_buttons.buttons[3].optionResult);
   
   m_Option5.ResetContent();
   
   for (i=0;i<NUM_ENCOUNTER_BUTTON_OPTION_TYPES;i++)
      m_Option5.AddString(ButtonOptionText[i]);
   
   m_Option5.SetCurSel((int)m_buttons.buttons[4].optionResult);
      
   m_Label1 = m_buttons.buttons[0].label;
   m_Label2 = m_buttons.buttons[1].label;
   m_Label3 = m_buttons.buttons[2].label;
   m_Label4 = m_buttons.buttons[3].label;
   m_Label5 = m_buttons.buttons[4].label;
   
   m_Present1 = m_buttons.buttons[0].present;
   m_Present2 = m_buttons.buttons[1].present;
   m_Present3 = m_buttons.buttons[2].present;
   m_Present4 = m_buttons.buttons[3].present;
   m_Present5 = m_buttons.buttons[4].present;
     
   m_UpClose1 = m_buttons.buttons[0].allowedUpClose;
   m_UpClose2 = m_buttons.buttons[1].allowedUpClose;
   m_UpClose3 = m_buttons.buttons[2].allowedUpClose;
   m_UpClose4 = m_buttons.buttons[3].allowedUpClose;
   m_UpClose5 = m_buttons.buttons[4].allowedUpClose;

   m_OnlyUpClose1 = m_buttons.buttons[0].onlyUpClose;
   m_OnlyUpClose2 = m_buttons.buttons[1].onlyUpClose;
   m_OnlyUpClose3 = m_buttons.buttons[2].onlyUpClose;
   m_OnlyUpClose4 = m_buttons.buttons[3].onlyUpClose;
   m_OnlyUpClose5 = m_buttons.buttons[4].onlyUpClose;
   
   UpdateData(FALSE);
   
   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void CButtons::OnOK() 
{
   UpdateData(TRUE);

   if (    ((m_Label1.GetLength() == 0) || !m_Present1 || ((encounterButtonResultType) m_Option1.GetCurSel() == NoResult))
        && ((m_Label2.GetLength() == 0) || !m_Present2 || ((encounterButtonResultType) m_Option2.GetCurSel() == NoResult))
        && ((m_Label3.GetLength() == 0) || !m_Present3 || ((encounterButtonResultType) m_Option3.GetCurSel() == NoResult))
        && ((m_Label4.GetLength() == 0) || !m_Present4 || ((encounterButtonResultType) m_Option4.GetCurSel() == NoResult))
        && ((m_Label5.GetLength() == 0) || !m_Present5 || ((encounterButtonResultType) m_Option5.GetCurSel() == NoResult))
      )
   {
     MessageBox("No buttons were defined.  We added a default 'Exit' button for you.", "Warning", MB_OK);
     m_Present1 = TRUE;
     if (m_Label1.GetLength() == 0) m_Label1 = "Exit";
     if ((encounterButtonResultType) m_Option1.GetCurSel() == NoResult) 
     {
       m_Option1.SetCurSel(ChainToEvent);
       m_buttons.buttons[0].optionResult = (encounterButtonResultType) m_Option1.GetCurSel();
     };
    
   };



   m_buttons.buttons[0].label = m_Label1;
   m_buttons.buttons[1].label = m_Label2;
   m_buttons.buttons[2].label = m_Label3;
   m_buttons.buttons[3].label = m_Label4;
   m_buttons.buttons[4].label = m_Label5;

   m_buttons.buttons[0].present = m_Present1;
   m_buttons.buttons[1].present = m_Present2;
   m_buttons.buttons[2].present = m_Present3;
   m_buttons.buttons[3].present = m_Present4;
   m_buttons.buttons[4].present = m_Present5;
     
   m_buttons.buttons[0].allowedUpClose = m_UpClose1;
   m_buttons.buttons[1].allowedUpClose = m_UpClose2;
   m_buttons.buttons[2].allowedUpClose = m_UpClose3;
   m_buttons.buttons[3].allowedUpClose = m_UpClose4;
   m_buttons.buttons[4].allowedUpClose = m_UpClose5;

   m_buttons.buttons[0].onlyUpClose = m_OnlyUpClose1;
   m_buttons.buttons[1].onlyUpClose = m_OnlyUpClose2;
   m_buttons.buttons[2].onlyUpClose = m_OnlyUpClose3;
   m_buttons.buttons[3].onlyUpClose = m_OnlyUpClose4;
   m_buttons.buttons[4].onlyUpClose = m_OnlyUpClose5;
   
   int count = 0;
   for (int i=0;i<MAX_BUTTONS;i++)
   {
     if (m_buttons.buttons[i].present)
       count++;
   }
   m_buttons.numButtons = count;

   CDialog::OnOK();
}


void CButtons::OnSelchangeButtonoption1() 
{
   m_buttons.buttons[0].optionResult = 
      (encounterButtonResultType) m_Option1.GetCurSel(); 
}

void CButtons::OnSelchangeButtonoption2() 
{
   m_buttons.buttons[1].optionResult = 
      (encounterButtonResultType) m_Option2.GetCurSel();    
}

void CButtons::OnSelchangeButtonoption3() 
{
   m_buttons.buttons[2].optionResult = 
      (encounterButtonResultType) m_Option3.GetCurSel();
}

void CButtons::OnSelchangeButtonoption4() 
{
   m_buttons.buttons[3].optionResult = 
      (encounterButtonResultType) m_Option4.GetCurSel(); 
}

void CButtons::OnSelchangeButtonoption5() 
{
   m_buttons.buttons[4].optionResult = 
      (encounterButtonResultType) m_Option5.GetCurSel();    
}

void CButtons::GetButtonData(ENCOUNTER_BUTTON_DATA& buttons)
{
   buttons = m_buttons;
}

