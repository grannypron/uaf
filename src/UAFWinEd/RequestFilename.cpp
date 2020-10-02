/******************************************************************************
* Filename: RequestFilename.cpp
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

#include <sys/types.h> 
#include <sys/stat.h>
#include "UAFWinEd.h"
#include "GlobalData.h"
#include "RequestFilename.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRequestDesignFilename dialog


CRequestDesignFilename::CRequestDesignFilename(CString file, BOOL AllowExisting, CWnd* pParent /*=NULL*/)
	: CDialog(CRequestDesignFilename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRequestDesignFilename)
	m_Filename = _T("");
	//}}AFX_DATA_INIT

  m_AllowExisting = AllowExisting;
  m_file = file;
}

void AFXAPI DDX_ValidDesignFilename(CDataExchange* pDX, int nIDC, CString filename)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
	  TCHAR szWindowText[_MAX_PATH+1];
    TCHAR fullPath[_MAX_PATH+1];

	  ::GetWindowText(hWndCtrl, szWindowText, sizeof(szWindowText)-1);

#ifdef SIMPLE_STRUCTURE
     strcpy(fullPath, rte.DesignDir());
#else
     strcpy(fullPath, m_installDir);
#endif
     strcat(fullPath, szWindowText);   
     strcat(fullPath, ".dsn");

    struct stat fileInfo;
    if (stat(fullPath, &fileInfo) == 0)
    {
			AfxMessageBox("This name already exists");
			pDX->Fail();
		}
	}
}

void CRequestDesignFilename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRequestDesignFilename)
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	//}}AFX_DATA_MAP

  if (!m_AllowExisting)
    DDX_ValidDesignFilename(pDX, IDC_FILENAME, m_Filename);
}


BEGIN_MESSAGE_MAP(CRequestDesignFilename, CDialog)
	//{{AFX_MSG_MAP(CRequestDesignFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRequestDesignFilename message handlers

BOOL CRequestDesignFilename::OnInitDialog() 
{
	CDialog::OnInitDialog();
  
  m_Filename = m_file;

  UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRequestDesignFilename::OnOK() 
{
  //UpdateData(TRUE);
	CDialog::OnOK();
  m_file = m_Filename;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CRequestLevelFilename dialog


CRequestLevelFilename::CRequestLevelFilename(CString file, BOOL AllowExisting, CWnd* pParent /*=NULL*/)
	: CDialog(CRequestLevelFilename::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRequestLevelFilename)
	m_Filename = _T("");
	//}}AFX_DATA_INIT

  m_AllowExisting = AllowExisting;
  m_file = file;
}

void AFXAPI DDX_ValidLevelFilename(CDataExchange* pDX, int nIDC, CString filename)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
	  TCHAR szWindowText[_MAX_PATH+1];
    TCHAR fullPath[_MAX_PATH+1];

	  ::GetWindowText(hWndCtrl, szWindowText, sizeof(szWindowText)-1);

     strcpy(fullPath, rte.DesignDir());
     strcat(fullPath, szWindowText);   
     strcat(fullPath, ".lvl");

    struct stat fileInfo;
    if (stat(fullPath, &fileInfo) == 0)
    {
			AfxMessageBox("This name already exists");
			pDX->Fail();
		}
	}
}

void CRequestLevelFilename::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRequestLevelFilename)
	DDX_Text(pDX, IDC_FILENAME, m_Filename);
	//}}AFX_DATA_MAP

  if (!m_AllowExisting)
    DDX_ValidLevelFilename(pDX, IDC_FILENAME, m_Filename);
}


BEGIN_MESSAGE_MAP(CRequestLevelFilename, CDialog)
	//{{AFX_MSG_MAP(CRequestLevelFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRequestLevelFilename message handlers

BOOL CRequestLevelFilename::OnInitDialog() 
{
	CDialog::OnInitDialog();
  
  m_Filename = m_file;

  UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRequestLevelFilename::OnOK() 
{
  //UpdateData(TRUE); 

	CDialog::OnOK();
  m_file = m_Filename;
}
