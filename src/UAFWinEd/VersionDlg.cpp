/******************************************************************************
* Filename: VersionDlg.cpp
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
//#include "..\Shared\ProjectVersion.h"
#include "resource.h"

//#include "uafwined.h"
#include "VersionDlg.h"
//#include "GlobalData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern const double VersionSpellNames;
extern const double VersionSpellIDs;
extern const double PRODUCT_VER;



/////////////////////////////////////////////////////////////////////////////
// VersionDlg dialog


VersionDlg::VersionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VersionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VersionDlg)
	m_VersionText = _T("");
	//}}AFX_DATA_INIT
}


void VersionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VersionDlg)
	DDX_Text(pDX, IDC_VersionText, m_VersionText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VersionDlg, CDialog)
	//{{AFX_MSG_MAP(VersionDlg)
	ON_BN_CLICKED(IDC_UpdateVersion, OnUpdateVersion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VersionDlg message handlers

void VersionDlg::OnUpdateVersion() 
{
	SetGameVersion(globalData,PRODUCT_VER);
	m_VersionText.Format("Design \"%s\" version = %4.7f", 
                        GetDesignName(globalData), GetGameVersion(globalData));
	UpdateData(FALSE);
}
