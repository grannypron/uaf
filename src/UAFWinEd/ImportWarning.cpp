/******************************************************************************
* Filename: ImportWarning.cpp
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
#ifdef UAFEDITOR
#include "resource.h"
#else
#include "..\UAFWin\resource.h"
#endif

//#include "Externs.h"


//#include "UAFWinEd.h"
//#include "globtext.h"
//#include "GlobalData.h"
//#include "GameEvent.h"
//#include "buttons.h"
//#include "SelectPicDlg.h"
#include "ImportWarning.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CImportWarning dialog


CImportWarning::CImportWarning(CWnd* pParent)
      : CDialog(CImportWarning::IDD, pParent)
{
   //{{AFX_DATA_INIT(CImportWarning)
   m_alpha = "";
   m_numeric = "";
	//}}AFX_DATA_INIT
   
}


void CImportWarning::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CImportWarning)
   DDX_Text(pDX, IDC_Alpha, m_alpha);
   DDX_Text(pDX, IDC_Numeric, m_numeric);
   DDX_Text(pDX, IDC_Warning, m_warning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportWarning, CDialog)
//{{AFX_MSG_MAP(CEncounter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString digits[10] = 
{
	"zero",
	"one",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine"
};


/////////////////////////////////////////////////////////////////////////////
// CEncounter message handlers

BOOL CImportWarning::OnInitDialog() 
{
   CDialog::OnInitDialog();



   m_i = timeGetTime()%100;
   m_alpha = "";
   m_alpha += digits[m_i/10];
   m_alpha += " ";
   m_alpha += digits[m_i%10];

   m_numeric = "";


  GotoDlgCtrl(GetDlgItem(IDC_Numeric));

   UpdateData(FALSE);
   
   return FALSE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void CImportWarning::OnOK() 
{
   UpdateData(TRUE);
   
   CDialog::OnOK();
}

